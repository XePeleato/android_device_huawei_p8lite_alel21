/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This software is protected by copyright, international           *
* treaties and patents, and distributed under multiple licenses.   *
* Any use of this Software as part of the Discretix CryptoCell or  *
* Packet Engine products requires a commercial license.            *
* Copies of this Software that are distributed with the Discretix  *
* CryptoCell or Packet Engine product drivers, may be used in      *
* accordance with a commercial license, or at the user's option,   *
* used and redistributed under the terms and conditions of the GNU *
* General Public License ("GPL") version 2, as published by the    *
* Free Software Foundation.                                        *
* This program is distributed in the hope that it will be useful,  *
* but WITHOUT ANY LIABILITY AND WARRANTY; without even the implied *
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. *
* See the GNU General Public License version 2 for more details.   *
* You should have received a copy of the GNU General Public        *
* License version 2 along with this Software; if not, please write *
* to the Free Software Foundation, Inc., 59 Temple Place - Suite   *
* 330, Boston, MA 02111-1307, USA.                                 *
* Any copy or reproduction of this Software, as permitted under    *
* the GNU General Public License version 2, must include this      *
* Copyright Notice as well as any other notices provided under     *
* the said license.                                                *
********************************************************************/

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <crypto/ctr.h>

#include "dx_config.h"
#include "dx_driver.h"
#include "dx_request_mgr.h"
#include "hw_queue_defs.h"
#include "dx_sysfs.h"
#include "dx_ivgen.h"

#define AXIM_MON_BASE_OFFSET DX_CC_REG_OFFSET(CRY_KERNEL, AXIM_MON_COMP8)

struct dx_request_mgr_handle {
	/* Request manager resources */
	unsigned int max_used_hw_slots;
	unsigned int max_used_sw_slots;
	struct dx_crypto_req req_queue[MAX_REQUEST_QUEUE_SIZE];
	uint32_t req_queue_head;
	uint32_t req_queue_tail;
	uint32_t axi_completed;
	uint32_t q_free_slots;
	spinlock_t hw_lock;
	HwDesc_s compl_desc;
	uint8_t *dummy_comp_buff;
	dma_addr_t dummy_comp_buff_dma;
#ifdef COMP_IN_WQ
	struct workqueue_struct *workq;
	struct delayed_work compwork;
#else
	struct tasklet_struct comptask;
#endif
	bool is_suspended;
};


static void comp_handler(unsigned long devarg);
#ifdef COMP_IN_WQ
static void comp_work_handler(struct work_struct *work);
#endif

int request_mgr_fini(struct dx_drvdata *drvdata)
{
	struct dx_request_mgr_handle *req_mgr_h = drvdata->request_mgr_handle;

	if (req_mgr_h == NULL)
		goto out;

	if (req_mgr_h->dummy_comp_buff_dma != 0)
		dma_free_coherent(&drvdata->plat_dev->dev,
			sizeof(uint32_t), req_mgr_h->dummy_comp_buff,
			req_mgr_h->dummy_comp_buff_dma);

	DX_LOG_DEBUG("max_used_hw_slots=%d\n", req_mgr_h->max_used_hw_slots);
	DX_LOG_DEBUG("max_used_sw_slots=%d\n", req_mgr_h->max_used_sw_slots);

#ifdef COMP_IN_WQ
	flush_workqueue(req_mgr_h->workq);
	destroy_workqueue(req_mgr_h->workq);
#else
	/* Kill tasklet */
	tasklet_kill(&req_mgr_h->comptask);
#endif
	memset(req_mgr_h, 0, sizeof(struct dx_request_mgr_handle));
	kfree(req_mgr_h);
	drvdata->request_mgr_handle = NULL;

out:
	return 0;
}

int request_mgr_init(struct dx_drvdata *drvdata)
{
	struct dx_request_mgr_handle *req_mgr_h;
	int rc = 0;

	req_mgr_h = kzalloc(sizeof(struct dx_request_mgr_handle),GFP_KERNEL);
	if (req_mgr_h == NULL) {
		rc = -ENOMEM;
		goto out;
	}

	spin_lock_init(&req_mgr_h->hw_lock);
#ifdef COMP_IN_WQ
	DX_LOG_DEBUG("Initializing completion workqueue\n");
	req_mgr_h->workq = create_singlethread_workqueue("dx_cc44p_wq");
	if (unlikely(req_mgr_h->workq == NULL)) {
		DX_LOG_ERR("Failed creating work queue\n");
		rc = -ENOMEM;
		goto out;
	}
	INIT_DELAYED_WORK(&req_mgr_h->compwork, comp_work_handler);
#else
	DX_LOG_DEBUG("Initializing completion tasklet\n");
	tasklet_init(&req_mgr_h->comptask, comp_handler, (unsigned long)drvdata);
#endif
	req_mgr_h->max_used_hw_slots = 0;
	req_mgr_h->max_used_sw_slots = 0;
	req_mgr_h->is_suspended = 0;
	drvdata->request_mgr_handle = req_mgr_h;

	/* Allocate DMA word for "dummy" completion descriptor use */
	req_mgr_h->dummy_comp_buff = dma_alloc_coherent(&drvdata->plat_dev->dev,
		sizeof(uint32_t), &req_mgr_h->dummy_comp_buff_dma, GFP_KERNEL);
	if (!req_mgr_h->dummy_comp_buff) {
		DX_LOG_ERR("Not enough memory to allocate DMA(%d) dropped "
			   "buffer\n", sizeof(uint32_t));
		rc = -ENOMEM;
		goto out;
	}

	/* Init. "dummy" completion descriptor */
	HW_DESC_INIT(&req_mgr_h->compl_desc);
	HW_DESC_SET_DIN_CONST(&req_mgr_h->compl_desc, 0, sizeof(uint32_t));
	HW_DESC_SET_DOUT_DLLI(&req_mgr_h->compl_desc,
		req_mgr_h->dummy_comp_buff_dma,
		sizeof(uint32_t), AXI_ID, NS_BIT, 1);
	HW_DESC_SET_FLOW_MODE(&req_mgr_h->compl_desc, BYPASS);

	return 0;

out:
	request_mgr_fini(drvdata);
	return rc;
}

static inline void enqueue_seq(
	void __iomem *cc_base,
	HwDesc_s seq[], unsigned int seq_len)
{
	int i;

	for (i = 0; i < seq_len; i++) {
#if (DX_DEV_SIGNATURE == DX_CC441P_SIG)
		writel_relaxed(seq[i].word[0], (volatile void __iomem *)(cc_base+DX_CC_REG_OFFSET(CRY_KERNEL, DSCRPTR_QUEUE0_WORD0)));
		writel_relaxed(seq[i].word[1], (volatile void __iomem *)(cc_base+DX_CC_REG_OFFSET(CRY_KERNEL, DSCRPTR_QUEUE0_WORD1)));
		writel_relaxed(seq[i].word[2], (volatile void __iomem *)(cc_base+DX_CC_REG_OFFSET(CRY_KERNEL, DSCRPTR_QUEUE0_WORD2)));
		writel_relaxed(seq[i].word[3], (volatile void __iomem *)(cc_base+DX_CC_REG_OFFSET(CRY_KERNEL, DSCRPTR_QUEUE0_WORD3)));
		wmb();
		writel_relaxed(seq[i].word[4], (volatile void __iomem *)(cc_base+DX_CC_REG_OFFSET(CRY_KERNEL, DSCRPTR_QUEUE0_WORD4)));
		wmb();
#else /* DX_CC442P_SIG */
		writel_relaxed(seq[i].word[0], (volatile void __iomem *)(cc_base+DX_CC_REG_OFFSET(CRY_KERNEL, DSCRPTR_QUEUE0_WORD0)));
		writel_relaxed(seq[i].word[1], (volatile void __iomem *)(cc_base+DX_CC_REG_OFFSET(CRY_KERNEL, DSCRPTR_QUEUE0_WORD0)));
		writel_relaxed(seq[i].word[2], (volatile void __iomem *)(cc_base+DX_CC_REG_OFFSET(CRY_KERNEL, DSCRPTR_QUEUE0_WORD0)));
		writel_relaxed(seq[i].word[3], (volatile void __iomem *)(cc_base+DX_CC_REG_OFFSET(CRY_KERNEL, DSCRPTR_QUEUE0_WORD0)));
		writel_relaxed(seq[i].word[4], (volatile void __iomem *)(cc_base+DX_CC_REG_OFFSET(CRY_KERNEL, DSCRPTR_QUEUE0_WORD0)));
#endif
#ifdef DX_DUMP_DESCS
		DX_LOG_DEBUG("desc[%02d]: 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X\n", i,
			seq[i].word[0], seq[i].word[1], seq[i].word[2], seq[i].word[3], seq[i].word[4]);
#endif
	}
}

/*!
 * Completion will take place if and only if user requested completion
 * by setting "is_dout = 0" in send_request().
 *
 * \param dev
 * \param dx_compl_h The completion event to signal
 */
static void request_mgr_complete(struct device *dev, void *dx_compl_h)
{
	struct completion *this_compl = dx_compl_h;
	complete(this_compl);
}

/*!
 * Enqueue caller request to crypto hardware.
 *
 * \param drvdata
 * \param dx_req The request to enqueue
 * \param desc The crypto sequence
 * \param len The crypto sequence length
 * \param is_dout If "true": completion is handled by the caller
 *		  If "false": this function adds a dummy descriptor completion
 *		  and waits upon completion signal.
 *
 * \return int Returns -EINPROGRESS if "is_dout=ture"; "0" if "is_dout=false"
 */
int send_request(
	struct dx_drvdata *drvdata, struct dx_crypto_req *dx_req,
	HwDesc_s *desc, unsigned int len, bool is_dout)
{
	void __iomem *cc_base = drvdata->cc_base;
	struct dx_request_mgr_handle *req_mgr_h = drvdata->request_mgr_handle;
	unsigned int used_sw_slots;
	unsigned int iv_seq_len = 0;
	unsigned int total_seq_len = len; /*initial sequence length*/
	HwDesc_s iv_seq[DX_IVPOOL_SEQ_LEN];
	int rc;
	DECL_CYCLE_COUNT_RESOURCES;

	spin_lock_bh(&req_mgr_h->hw_lock);

	if (req_mgr_h->is_suspended) {
		DX_LOG_ERR("The queue is suspended \n");
		spin_unlock_bh(&req_mgr_h->hw_lock);
		return -EACCES;
	}
	/* Additional completion descriptor is needed incase caller did not
	   enabled any DLLI/MLLI DOUT bit in the given sequence */
	if (!is_dout) {
		init_completion(&dx_req->seq_compl);
		dx_req->user_cb = request_mgr_complete;
		dx_req->user_arg = &(dx_req->seq_compl);
		total_seq_len++;
	}

	if (dx_req->ivgen_dma_addr) {
		DX_LOG_DEBUG("Acquire IV from pool IV=%08X IV-size=%d\n",
			dx_req->ivgen_dma_addr, dx_req->ivgen_size);
		/* Acquire IV from pool */
		rc = dx_ivgen_getiv(drvdata, dx_req->ivgen_dma_addr,
			dx_req->ivgen_size, iv_seq, &iv_seq_len);
		if (unlikely(rc != 0)) {
			DX_LOG_ERR("Failed to generate IV (rc=%d)\n", rc);
			spin_unlock_bh(&req_mgr_h->hw_lock);
			return -EAGAIN;
		}
		total_seq_len += iv_seq_len;
	}

	/* Check space in HW FIFO by read the register shadow variable */
	if (unlikely(req_mgr_h->q_free_slots < total_seq_len)) {
		req_mgr_h->q_free_slots = DX_HAL_ReadCcRegister(DX_CC_REG_OFFSET(CRY_KERNEL, DSCRPTR_QUEUE0_CONTENT));
		if (unlikely((HW_QUEUE_SIZE - req_mgr_h->q_free_slots) > req_mgr_h->max_used_hw_slots)) {
			req_mgr_h->max_used_hw_slots = HW_QUEUE_SIZE - req_mgr_h->q_free_slots;
		}

		if (unlikely(req_mgr_h->q_free_slots < total_seq_len)) {
			DX_LOG(KERN_INFO, "HW FIFO is full. Returning -EAGAIN. content=%d seq_len=%d\n", req_mgr_h->q_free_slots, total_seq_len);
			spin_unlock_bh(&req_mgr_h->hw_lock);
			return -EAGAIN; /* Should be -EBUSY ? */
		}
	}

	if (unlikely( ((req_mgr_h->req_queue_head + 1) & (MAX_REQUEST_QUEUE_SIZE - 1)) == req_mgr_h->req_queue_tail )) {
		DX_LOG(KERN_INFO, "SW FIFO is full. Returning -EAGAIN. head=%d len=%d\n", req_mgr_h->req_queue_head, MAX_REQUEST_QUEUE_SIZE);
		spin_unlock_bh(&req_mgr_h->hw_lock);
		return -EAGAIN; /* Should be -EBUSY ? */
	}
	used_sw_slots = ((req_mgr_h->req_queue_head - req_mgr_h->req_queue_tail) & (MAX_REQUEST_QUEUE_SIZE-1));
	if (unlikely(used_sw_slots > req_mgr_h->max_used_sw_slots)) {
		req_mgr_h->max_used_sw_slots = used_sw_slots;
	}

	/* TODO: Wait on water mark set to half queue size */

	/* Enqueue request - must be locked with HW lock*/
	req_mgr_h->req_queue[req_mgr_h->req_queue_head] = *dx_req;
	START_CYCLE_COUNT_AT(req_mgr_h->req_queue[req_mgr_h->req_queue_head].submit_cycle);
	req_mgr_h->req_queue_head = (req_mgr_h->req_queue_head + 1) & (MAX_REQUEST_QUEUE_SIZE - 1);
	/* TODO: Use circ_buf.h ? */

	DX_LOG_DEBUG("Enqueue request head=%u\n", req_mgr_h->req_queue_head);

#ifdef FLUSH_CACHE_ALL
	flush_cache_all();
#endif
	/* STAT_PHASE_4: Push sequence */
	START_CYCLE_COUNT();
	enqueue_seq(cc_base, iv_seq, iv_seq_len);
	enqueue_seq(cc_base, desc, len);
	enqueue_seq(cc_base, &req_mgr_h->compl_desc, (is_dout ? 0 : 1));
	END_CYCLE_COUNT(dx_req->op_type, STAT_PHASE_4);

	/* Update the free slots in HW queue */
	req_mgr_h->q_free_slots -= total_seq_len;

	spin_unlock_bh(&req_mgr_h->hw_lock);

	if (!is_dout) {
		/* Wait upon sequence completion.
		*  Return "0" -Operation done successfully. */
		return wait_for_completion_interruptible(&dx_req->seq_compl);
	} else {
		/* Operation still in process */
		return -EINPROGRESS;
	}
}

void complete_request(struct dx_drvdata *drvdata)
{
	struct dx_request_mgr_handle * request_mgr_handle =
						drvdata->request_mgr_handle;
#ifdef COMP_IN_WQ
	queue_delayed_work(request_mgr_handle->workq, &request_mgr_handle->compwork, 0);
#else
	tasklet_schedule(&request_mgr_handle->comptask);
#endif
}

#ifdef COMP_IN_WQ
static void comp_work_handler(struct work_struct *work)
{
	struct dx_drvdata *drvdata =
		container_of(work, struct dx_drvdata, compwork.work);

	comp_handler((unsigned long)drvdata);
}
#endif

static void proc_completions(struct dx_drvdata *drvdata)
{
	struct dx_crypto_req *dx_req;
	struct platform_device *plat_dev = drvdata->plat_dev;
	struct dx_request_mgr_handle * request_mgr_handle =
						drvdata->request_mgr_handle;
	DECL_CYCLE_COUNT_RESOURCES;

	while(request_mgr_handle->axi_completed) {
		request_mgr_handle->axi_completed--;

		/* Dequeue request */
		if (unlikely(request_mgr_handle->req_queue_head == request_mgr_handle->req_queue_tail)) {
			DX_LOG_ERR("Request queue is empty req_queue_head==req_queue_tail==%u\n", request_mgr_handle->req_queue_head);
			BUG();
		}

		dx_req = &request_mgr_handle->req_queue[request_mgr_handle->req_queue_tail];
		END_CYCLE_COUNT_AT(dx_req->submit_cycle, dx_req->op_type, STAT_PHASE_5); /* Seq. Comp. */

#ifdef FLUSH_CACHE_ALL
		flush_cache_all();
#endif

#ifdef COMPLETION_DELAY
		/* Delay */
		{
			int i;
			DX_LOG_DEBUG("Delay\n");
			for (i=0;i<1000000;i++) {
				axi_err = READ_REGISTER(cc_base + DX_CC_REG_OFFSET(CRY_KERNEL, AXIM_MON_ERR));
			}
		}
#endif /* COMPLETION_DELAY */

		if (likely(dx_req->user_cb)) {
			START_CYCLE_COUNT();
			dx_req->user_cb(&plat_dev->dev, dx_req->user_arg);
			END_CYCLE_COUNT(STAT_OP_TYPE_GENERIC, STAT_PHASE_3);
		}
		request_mgr_handle->req_queue_tail = (request_mgr_handle->req_queue_tail + 1) & (MAX_REQUEST_QUEUE_SIZE - 1);
		DX_LOG_DEBUG("Dequeue request tail=%u\n", request_mgr_handle->req_queue_tail);
		DX_LOG_DEBUG("Request completed. axi_completed=%d\n", request_mgr_handle->axi_completed);
	}
}

/* Deferred service handler, run as interrupt-fired tasklet */
static void comp_handler(unsigned long devarg)
{
	struct dx_drvdata *drvdata = (struct dx_drvdata *)devarg;
	void __iomem *cc_base = drvdata->cc_base;
#if (DX_DEV_SIGNATURE == DX_CC441P_SIG)
	uint32_t axi_err;
#endif
	struct dx_request_mgr_handle * request_mgr_handle =
						drvdata->request_mgr_handle;
	DECL_CYCLE_COUNT_RESOURCES;

	START_CYCLE_COUNT();
	request_mgr_handle->axi_completed += DX_CC_REG_FLD_GET(CRY_KERNEL, AXIM_MON_COMP8, VALUE,
		READ_REGISTER(cc_base + AXIM_MON_BASE_OFFSET));

	/* ISR-to-Tasklet latency */
	if (request_mgr_handle->axi_completed > 0) {
		/* Only if actually reflects ISR-to-completion-handling latency, i.e.,
		   not duplicate as a result of interrupt after AXIM_MON_ERR clear, before end of loop */
		END_CYCLE_COUNT_AT(drvdata->isr_exit_cycles, STAT_OP_TYPE_GENERIC, STAT_PHASE_1);
	}
#if (DX_DEV_SIGNATURE == DX_CC441P_SIG)
	else {
		/* Clear on read AXI MON completion event which will enable completion interrupts */
		axi_err = READ_REGISTER(cc_base + DX_CC_REG_OFFSET(CRY_KERNEL, AXIM_MON_ERR));
		if (unlikely((DX_CC_REG_FLD_GET(CRY_KERNEL, AXIM_MON_ERR, SOURCE, axi_err) != 1)) && (axi_err != 0)) {
			DX_LOG_ERR("Possible AXI completion error: axim_mon_err=0x%08X\n", axi_err);
		}
		/* Avoid race with above clear: Test completion counter once more */
		request_mgr_handle->axi_completed += DX_CC_REG_FLD_GET(CRY_KERNEL, AXIM_MON_COMP8, VALUE,
			READ_REGISTER(cc_base + AXIM_MON_BASE_OFFSET));
	}
#endif
#if (DX_DEV_SIGNATURE == DX_CC442P_SIG)
	else {
		/* To avoid the interrupt from firing as we unmask it, we clear it now */
		WRITE_REGISTER(cc_base + DX_CC_REG_OFFSET(HOST_RGF, HOST_ICR), DX_COMP_IRQ_MASK);

		/* Unmask AXI completion interrupt */
		WRITE_REGISTER(cc_base + DX_CC_REG_OFFSET(HOST_RGF, HOST_IMR),
			READ_REGISTER(cc_base + DX_CC_REG_OFFSET(HOST_RGF, HOST_IMR)) & ~(DX_COMP_IRQ_MASK));

		/* Avoid race with above clear: Test completion counter once more */
		request_mgr_handle->axi_completed += DX_CC_REG_FLD_GET(CRY_KERNEL, AXIM_MON_COMP8, VALUE,
			READ_REGISTER(cc_base + AXIM_MON_BASE_OFFSET));
	}
#endif

	while (request_mgr_handle->axi_completed > 0) {
		do {
			proc_completions(drvdata);
			request_mgr_handle->axi_completed += DX_CC_REG_FLD_GET(CRY_KERNEL, AXIM_MON_COMP8, VALUE,
				READ_REGISTER(cc_base + AXIM_MON_BASE_OFFSET));
		} while (request_mgr_handle->axi_completed > 0);

#if (DX_DEV_SIGNATURE == DX_CC441P_SIG)
		/* Clear on read AXI MON completion event which will enable completion interrupts */
		axi_err = READ_REGISTER(cc_base + DX_CC_REG_OFFSET(CRY_KERNEL, AXIM_MON_ERR));
		if (unlikely((DX_CC_REG_FLD_GET(CRY_KERNEL, AXIM_MON_ERR, SOURCE, axi_err) != 1)) && (axi_err != 0)) {
			DX_LOG_ERR("Possible AXI completion error: axim_mon_err=0x%08X\n", axi_err);
		}
#else /* DX_CC442P_SIG */
		/* To avoid the interrupt from firing as we unmask it, we clear it now */
		WRITE_REGISTER(cc_base + DX_CC_REG_OFFSET(HOST_RGF, HOST_ICR), DX_COMP_IRQ_MASK);

		/* Unmask AXI completion interrupt */
		WRITE_REGISTER(cc_base + DX_CC_REG_OFFSET(HOST_RGF, HOST_IMR),
			READ_REGISTER(cc_base + DX_CC_REG_OFFSET(HOST_RGF, HOST_IMR)) & ~(DX_COMP_IRQ_MASK));
#endif

		/* Avoid race with above clear: Test completion counter once more */
		request_mgr_handle->axi_completed += DX_CC_REG_FLD_GET(CRY_KERNEL, AXIM_MON_COMP8, VALUE,
			READ_REGISTER(cc_base + AXIM_MON_BASE_OFFSET));
	};

	END_CYCLE_COUNT(STAT_OP_TYPE_GENERIC, STAT_PHASE_2);
}



void dx_resume_queue(struct dx_drvdata *drvdata)
{
	struct dx_request_mgr_handle * request_mgr_handle =
						drvdata->request_mgr_handle;
	/* lock the send_request */
	spin_lock_bh(&request_mgr_handle->hw_lock);
	if (!request_mgr_handle->is_suspended) {
		DX_LOG_DEBUG(" Driver wasn't suspended \n");
	}
	request_mgr_handle->is_suspended = false;
	spin_unlock_bh(&request_mgr_handle->hw_lock);

	return;
}


int dx_suspend_queue(struct dx_drvdata *drvdata)
{
	struct dx_request_mgr_handle * request_mgr_handle =
						drvdata->request_mgr_handle;

	/* lock the send_request */
	spin_lock_bh(&request_mgr_handle->hw_lock);
	if (request_mgr_handle->is_suspended) {
		DX_LOG_DEBUG("Driver is already suspended \n");
		spin_unlock_bh(&request_mgr_handle->hw_lock);
		return 0;
	}
	if (request_mgr_handle->req_queue_head !=
	    request_mgr_handle->req_queue_tail) {
		spin_unlock_bh(&request_mgr_handle->hw_lock);
		return -EBUSY;
	}
	request_mgr_handle->is_suspended = true;
	spin_unlock_bh(&request_mgr_handle->hw_lock);

	return 0;
}
