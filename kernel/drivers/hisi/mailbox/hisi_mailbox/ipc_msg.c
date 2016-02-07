#include <linux/module.h>
#include <linux/printk.h>
#include <linux/hisi/hisi_rproc.h>
#include <linux/hisi/ipc_msg.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MODULE_NAME		"ipc_msg"
#define IPC_MSG_NULL	       ((void*)0)
#define ARRY_SIZE(a)			(sizeof(a)/sizeof((a)[0]))

#define IPCMSG_PR_ERR(fmt, args ...)	\
	({				\
		pr_err("%s(%d):" fmt "\n", \
			MODULE_NAME, __LINE__, ##args); \
	})
#define IPCMSG_PR_INFO(fmt, args ...)	\
	({				\
		pr_info("%s(%d):" fmt "\n", \
			MODULE_NAME, __LINE__, ##args); \
	})

static int (*ipc_msg_ops[MAX_CMD_OBJ][MAX_CMD]) (union ipc_data *);
static struct notifier_block ipc_msg_nb;
static int ipc_msg_test_init(void);
int ipc_msg_test_send(unsigned char des_obj, unsigned char send_mode, unsigned char cmd_type );
extern unsigned int omTimerGet(void);

static int Echo(union ipc_data *msg)
{
	unsigned int i;

	for (i = 0; i < MAX_MAIL_SIZE; i++) {
		IPCMSG_PR_INFO("echo 0x%x\n", msg->data[i]);
	}
	msg->data[1] = -1;
	return 0;
}

int ipc_msg_send(unsigned int processor_id, struct ipc_msg *msg, unsigned int mode)
{
	int ret = 0;
	rproc_msg_type_t is_sync_msg = ASYNC_MSG;

	if(!msg) {
		IPCMSG_PR_ERR("msg null\n");
		return -1;
	}

	switch (processor_id) {
		case OBJ_LPM3:
			msg->mailbox_id = HISI_RPROC_LPM3;
		break;
		case OBJ_HIFI:
			msg->mailbox_id = HISI_RPROC_HIFI;
		break;
		default:
			IPCMSG_PR_ERR("invalid processor id(%d)\n", processor_id);
			return -1;
	}

	is_sync_msg = (SYNC_CMD == msg->mode) ? SYNC_MSG : ASYNC_MSG;

	if (SYNC_MODE == mode) {
		/*ack buffer 与data buffer 复用*/
		ret = RPROC_SYNC_SEND(msg->mailbox_id,
							msg->data,
							ARRY_SIZE(msg->data),
							is_sync_msg,
							msg->data,
							ARRY_SIZE(msg->data));
	} else if (ASYNC_MODE == mode) {
		/*异步发送暂时不考虑complete 处理*/
		ret = RPROC_ASYNC_SEND(msg->mailbox_id,
							msg->data,
							ARRY_SIZE(msg->data),
							is_sync_msg,
							IPC_MSG_NULL,
							0);
	} else {
		IPCMSG_PR_ERR("invalid mode(%d)\n", mode);
		return -1;
	}

    return ret;
}

int ipc_msg_req_callback(unsigned int obj, unsigned int cmd, int (*func)(union ipc_data *))
{
	if (IPC_MSG_NULL != func  &&  obj < MAX_CMD_OBJ &&  cmd < MAX_CMD) {
		if (ipc_msg_ops[obj][cmd] != Echo) {
			IPCMSG_PR_ERR("callback exist:%d,%d\n", obj, cmd);
			return -1;
		}
		else{
			ipc_msg_ops[obj][cmd] = func;
			return 0;
		}
	}
	else{
		IPCMSG_PR_ERR("req para err:%d,%d\n", obj, cmd);
		return -1;
	}
}

int ipc_msg_put_callback(unsigned int obj, unsigned int cmd)
{
	if (obj >=  MAX_CMD_OBJ || cmd >=  MAX_CMD) {
		IPCMSG_PR_ERR("put para err:%d,%d\n", obj, cmd);
		return -1;
	}

	ipc_msg_ops[obj][cmd] = Echo;
	return 0;
}

static int ipc_msg_handle(struct notifier_block *nb, unsigned long len, void *d)
{
	struct ipc_msg *msg = (struct ipc_msg*)d;
	unsigned char cmd_obj, cmd;

	/* receive progress */
	cmd_obj = msg->cmd_mix.cmd_obj;
	cmd = msg->cmd_mix.cmd;

	/* cmd receieve, call callback func */
	if (cmd_obj >= MAX_CMD_OBJ || cmd >= MAX_CMD) {
		IPCMSG_PR_ERR("out of range:%d,%d\n", cmd_obj, cmd);
		Echo((union ipc_data *) msg->data);
	} else {
		/* execute cmd according to find table */
		ipc_msg_ops[cmd_obj][cmd]((union ipc_data *) msg->data);
	}

       return 0;
}

static int ipc_msg_init(void)
{
	unsigned int i, j;

	IPCMSG_PR_INFO("enter\r\n");

	/* operation table init */
	for (i = 0; i < MAX_CMD_OBJ; i++)
		for (j = 0; j < MAX_CMD; j++)
			ipc_msg_ops[i][j] = Echo;

	ipc_msg_nb.next = NULL;
	ipc_msg_nb.notifier_call = ipc_msg_handle;

	RPROC_MONITOR_REGISTER(HISI_RPROC_LPM3, &ipc_msg_nb);
	RPROC_MONITOR_REGISTER(HISI_RPROC_HIFI, &ipc_msg_nb);

	ipc_msg_test_init();
	IPCMSG_PR_INFO("leave\r\n");
    return 0;
}

static void ipc_msg_exit(void)
{
	unsigned int i, j;

	IPCMSG_PR_INFO("enter\r\n");
	/* operation table init */
	for (i = 0; i < MAX_CMD_OBJ; i++)
		for (j = 0; j < MAX_CMD; j++)
			ipc_msg_ops[i][j] = Echo;

	RPROC_MONITOR_UNREGISTER(HISI_RPROC_LPM3, &ipc_msg_nb);
	RPROC_MONITOR_UNREGISTER(HISI_RPROC_HIFI, &ipc_msg_nb);
	IPCMSG_PR_INFO("leave\r\n");
}

static int ipc_msg_test_cb(union ipc_data *msg)
{
	IPCMSG_PR_INFO("data0(0x%08x), data1(0x%08x)\n",
					msg->data[0], msg->data[1]);
	return 0;
}

static int ipc_msg_test_init(void)
{
	int ret = 0;

	ret = ipc_msg_req_callback(OBJ_TEST, CMD_TEST, ipc_msg_test_cb);

	return ret;
}

int ipc_msg_test_send(unsigned char des_obj, unsigned char send_mode, unsigned char cmd_type )
{
	struct ipc_msg msg;
	int ret = 0;

	msg.data[0] = 0x0;
	msg.cmd_mix.cmd_obj = OBJ_TEST;
	msg.cmd_mix.cmd = CMD_TEST;
	msg.mode = cmd_type;
	msg.data[1] = ((send_mode << 16) | cmd_type);

	IPCMSG_PR_INFO("[0x%08x]send start, mode=%d, type=%d\n", omTimerGet(), send_mode, cmd_type);
	ret = ipc_msg_send(des_obj, &msg, send_mode);
	if(ret) {
		IPCMSG_PR_ERR("[0x%08x]send fail, mode=%d, type=%d\n", omTimerGet(), send_mode, cmd_type);
		return -1;
	} else {
		if ((SYNC_MODE == send_mode) && (SYNC_CMD == cmd_type)) {
			if ((((OBJ_TEST << 16) | (CMD_TEST << 8)) == msg.data[0]) && (0x12345678 == msg.data[1])) {
				IPCMSG_PR_ERR("[0x%08x]send ok, mode=%d, type=%d\n", omTimerGet(), send_mode, cmd_type);
				return 0;
			} else {
				IPCMSG_PR_ERR("[0x%08x]send fail, mode=%d, type=%d, rsp fail [0x%08x][0x%08x]\n", omTimerGet(),
					send_mode, cmd_type, msg.data[0], msg.data[1]);
				return -1;
			}
		} else {
			IPCMSG_PR_ERR("[0x%08x]: send ok, mode=%d, type=%d\n", omTimerGet(), send_mode, cmd_type);
			return 0;
		}
	}
}

void test_ipc_msg_send_001(void)
{
    int ret = 0;

    ret = ipc_msg_send(0xff, NULL, 0);
    if(ret == -1){
        IPCMSG_PR_ERR("TEST1:msg is NULL ok!\r\n");
    } else {
        IPCMSG_PR_ERR("TEST1:msg is NULL fail!\r\n");
    }

}

void test_ipc_msg_send_002(void)
{
    struct ipc_msg msg;
    int ret = 0;

    ret = ipc_msg_send(0xff, &msg, 0);
    if(ret == -1){
        IPCMSG_PR_ERR("TEST2:invalid processor id ok!\r\n");
    } else {
        IPCMSG_PR_ERR("TEST2:invalid processor id fail!\r\n");
    }

}

void test_ipc_msg_send_003(void)
{
    struct ipc_msg msg;
    int ret = 0;

    ret = ipc_msg_send(OBJ_LPM3, &msg, 0xff);
    if(ret == -1){
        IPCMSG_PR_ERR("TEST3:invalid mode ok!\r\n");
    } else {
        IPCMSG_PR_ERR("TEST3:invalid mode fail!\r\n");
    }

}

subsys_initcall(ipc_msg_init);
module_exit(ipc_msg_exit);
MODULE_DESCRIPTION("HISI IPC MSG PROCESS");
MODULE_LICENSE("GPL V2");

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
