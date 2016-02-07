#include <linux/pm_qos.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/compiler.h>

#include "hisi_freq_ctl.h"

/*
The threshold of total io_wait_time:
If total io_wait_time is bigger than the threshold,
add ddr frequence request immediately.
*/
#define IO_WAIT_TIME_THRESHOLD			100

/* The band of ddr request */
#define DDR_REQUEST_VALUE_DOWN			1153
#define DDR_REQUEST_VALUE_UP			2303

/*
The period of the delete_request timer:
When there is not io_req waiting in flight, this timer should be started.
Before time out, if one io_req wait in flight, this timer should be canceled.
*/
#define REMOVE_REQ_TIME_MS				350

typedef struct {
	spinlock_t		lock;
	struct mutex	m_lock;
	int cur_type;/* The current stat of ddr frequence request */
	struct pm_qos_request* ddr_req;
	int delete_req_timer_start;/* The sign of the delete_request timer */
	int total_io_wait_time;/* The counter of total io_wait_time */
	s32 ddr_request_value;
	struct workqueue_struct* workqueue;
	struct work_struct work;
	struct delayed_work del_work;
	int new_req_type;
	int new_time_in_queue;
} freq_ctrl_t;

static freq_ctrl_t* freq_ctrl_ptr = NULL;

static void delete_freq_req_work(struct work_struct *work)
{
	if (unlikely((NULL == freq_ctrl_ptr) ||
			(NULL == freq_ctrl_ptr->ddr_req) ||
			(NULL == freq_ctrl_ptr->workqueue)))
		return;

	/* Delete the ddr frequence request */
	mutex_lock(&freq_ctrl_ptr->m_lock);
	freq_ctrl_ptr->cur_type = DDR_FREQ_REQ_REMOVE;
	pm_qos_remove_request(freq_ctrl_ptr->ddr_req);
	freq_ctrl_ptr->delete_req_timer_start = 0;
	printk(KERN_ERR "%s: block ddr freq request remove\n", __FUNCTION__);
	mutex_unlock(&freq_ctrl_ptr->m_lock);
	return;
}

static void ddr_freq_ctrl_work(struct work_struct *work)
{
	int cur_ddr_band = 0;
	s32 new_value = 0;
	int req_type;
	int time_in_queue;

	if (unlikely((NULL == freq_ctrl_ptr) ||
			(NULL == freq_ctrl_ptr->ddr_req) ||
			(NULL == freq_ctrl_ptr->workqueue)))
		return;

	spin_lock(&freq_ctrl_ptr->lock);
	req_type = freq_ctrl_ptr->new_req_type;
	time_in_queue = freq_ctrl_ptr->new_time_in_queue;
	spin_unlock(&freq_ctrl_ptr->lock);

	mutex_lock(&freq_ctrl_ptr->m_lock);

	if (DDR_FREQ_REQ_ADD == freq_ctrl_ptr->cur_type) {

		new_value = freq_ctrl_ptr->ddr_request_value;

		cur_ddr_band = pm_qos_request(PM_QOS_MEMORY_THROUGHPUT);

		if (cur_ddr_band < DDR_REQUEST_VALUE_DOWN) {
			new_value += DDR_REQUEST_VALUE_DOWN - cur_ddr_band;
		} else if (cur_ddr_band > DDR_REQUEST_VALUE_UP) {
			new_value = 0;
		}

		if (new_value != freq_ctrl_ptr->ddr_request_value) {
			freq_ctrl_ptr->ddr_request_value = new_value;
			pm_qos_update_request(freq_ctrl_ptr->ddr_req,
					freq_ctrl_ptr->ddr_request_value);
			printk(KERN_ERR "%s: block ddr freq request update to %d, "
				"curent ddr band is %d\n", __FUNCTION__,
				freq_ctrl_ptr->ddr_request_value, cur_ddr_band);
		}
	}

	if (DDR_FREQ_REQ_ADD == req_type) {
		if (DDR_FREQ_REQ_ADD == freq_ctrl_ptr->cur_type) {
			/*
			new request:	add_ddr_freq_req
			current state:	add_ddr_freq_req
			process:	Cancel the delete_request timer,
						if the timer has already been started.
			*/
			if (freq_ctrl_ptr->delete_req_timer_start) {
				cancel_delayed_work_sync(&freq_ctrl_ptr->del_work);
				freq_ctrl_ptr->delete_req_timer_start = 0;
			}
		} else {
			/*
			new request:	add_ddr_freq_req
			current state:	remove_ddr_freq_req
			process:	1) Add total_io_wait_time value.
					2) If total_io_wait_time is bigger than threshold,
					   start ddr frequence request.
			*/
			freq_ctrl_ptr->total_io_wait_time += time_in_queue;

			if (freq_ctrl_ptr->total_io_wait_time >= IO_WAIT_TIME_THRESHOLD) {
				freq_ctrl_ptr->ddr_req->pm_qos_class = 0;

				cur_ddr_band = pm_qos_request(PM_QOS_MEMORY_THROUGHPUT);
				if (cur_ddr_band > DDR_REQUEST_VALUE_DOWN)
					freq_ctrl_ptr->ddr_request_value = 0;
				else
					freq_ctrl_ptr->ddr_request_value =
							DDR_REQUEST_VALUE_DOWN - cur_ddr_band;
				pm_qos_add_request(freq_ctrl_ptr->ddr_req,
						PM_QOS_MEMORY_THROUGHPUT,
						freq_ctrl_ptr->ddr_request_value);
				freq_ctrl_ptr->cur_type = DDR_FREQ_REQ_ADD;
				freq_ctrl_ptr->total_io_wait_time = 0;
				printk(KERN_ERR "%s: block ddr freq request add. "
						"request_value = %d, curent ddr band is %d\n",
						__FUNCTION__, freq_ctrl_ptr->ddr_request_value,
						cur_ddr_band);
			}
		}
	} else {
		if (DDR_FREQ_REQ_REMOVE == freq_ctrl_ptr->cur_type) {
			/*
			new request:	remove_ddr_freq_req
			current state:	remove_ddr_freq_req
			process:	Reduce total_io_wait_time value.
			*/
			if (freq_ctrl_ptr->total_io_wait_time <= time_in_queue)
				freq_ctrl_ptr->total_io_wait_time = 0;
			else
				freq_ctrl_ptr->total_io_wait_time -= time_in_queue;
		} else {
			/*
			new request:	remove_ddr_freq_req
			current state:	add_ddr_freq_req
			process:	Start the delete_request timer.
			*/
			if (0 == freq_ctrl_ptr->delete_req_timer_start) {
				freq_ctrl_ptr->delete_req_timer_start = 1;
				queue_delayed_work(freq_ctrl_ptr->workqueue,
						&freq_ctrl_ptr->del_work,
						msecs_to_jiffies(REMOVE_REQ_TIME_MS));
			}
		}
	}

	mutex_unlock(&freq_ctrl_ptr->m_lock);
	return;
}

void ddr_freq_request(int req_type, int time_in_queue)
{
	if (unlikely((NULL == freq_ctrl_ptr) ||
			(NULL == freq_ctrl_ptr->ddr_req) ||
			(NULL == freq_ctrl_ptr->workqueue)))
		return;

	spin_lock(&freq_ctrl_ptr->lock);
	freq_ctrl_ptr->new_req_type = req_type;
	freq_ctrl_ptr->new_time_in_queue = time_in_queue;
	spin_unlock(&freq_ctrl_ptr->lock);

	queue_work(freq_ctrl_ptr->workqueue, &freq_ctrl_ptr->work);
	return;
}

void ddr_freq_ctrl_init(void)
{
	/* Init the struction When this function be called the first time */
	if (NULL == freq_ctrl_ptr) {
		freq_ctrl_ptr = kzalloc(sizeof(freq_ctrl_t), GFP_KERNEL);
		if (NULL == freq_ctrl_ptr) {
			printk(KERN_ERR "%s: kzalloc freq_ctrl_ptr error\n", __FUNCTION__);
			return;
		}
		printk(KERN_ERR "%s: hisi block ddr frequence ctrl: "
				"io_wait_time_threshold=%d, ddr_req_value_down=%d, "
				"ddr_req_value_up=%d, remove_req_time=%dms\n",
				__FUNCTION__, IO_WAIT_TIME_THRESHOLD,
				DDR_REQUEST_VALUE_DOWN, DDR_REQUEST_VALUE_UP,
				REMOVE_REQ_TIME_MS);
		spin_lock_init(&freq_ctrl_ptr->lock);
		mutex_init(&freq_ctrl_ptr->m_lock);
	}

	if (NULL == freq_ctrl_ptr->ddr_req) {
		freq_ctrl_ptr->ddr_req = kmalloc(sizeof(struct pm_qos_request),
				GFP_KERNEL);
		if (freq_ctrl_ptr->ddr_req == NULL) {
			printk(KERN_ERR "%s: malloc ddr req error\n", __FUNCTION__);
			return;
		}
	}

	if (NULL == freq_ctrl_ptr->workqueue) {
		freq_ctrl_ptr->workqueue =
				create_singlethread_workqueue("hisi_block_freq_ctrl");
		if (NULL == freq_ctrl_ptr->workqueue) {
			printk(KERN_ERR "%s: creat workqueue error\n", __FUNCTION__);
			return;
		}
		INIT_WORK(&freq_ctrl_ptr->work, ddr_freq_ctrl_work);
		INIT_DELAYED_WORK(&freq_ctrl_ptr->del_work, delete_freq_req_work);
	}
	/* Init end */

	return;
}
