/*
 * hi6xxx mailbox device driver
 *
 * Copyright (c) 2013-2014 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/pm_runtime.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/mod_devicetable.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/hisi/hisi_mailbox.h>
#include <linux/hisi/hi6xxx-iomap.h>   
#include "../../ipc_hi6xxx/bsp_drv_ipc.h"

#define MODULE_NAME				"hi6xxx_mailbox_dev"

 /* hardware configulation information */
#define MDEV_PHY2VIRT(phy)                (HISI_VA_ADDRESS(phy))
#define MDEV_BASE_PHY_ADDR             (MEMORY_RAM_MBOX_DEVICE_ADDR)
#define MDEV_BASE_VIRT_ADDR		(MDEV_PHY2VIRT(MDEV_BASE_PHY_ADDR))
#define MDEV_ADDR_SIZE                      (MEMORY_RAM_MBOX_DEVICE_SIZE)
#define MDEV_NUM         				(32)
#define MDEV_MODE(mdev)                    ((mdev<<6) + 0x00)
#define MDEV_DATA(mdev, index)         ((mdev<<6) + 0x04 + (index << 2))
#define MDEV_TRACK(mdev, index)        ((mdev<<6) + 0x24 + (index << 2))

/* mailbox device type*/
#define FAST_MBOX					(1 << 0)
#define COMM_MBOX					(1 << 1)
#define SOURCE_MBOX				(1 << 2)
#define DESTINATION_MBOX			(1 << 3)

/* mode and status configulation */
#define ACK_CONFIG_MASK			(0x1)
#define AUTOMATIC_ACK_CONFIG		(1 << 0)
#define NO_FUNC_CONFIG				(0 << 0)

#define STATUS_MASK				(0xF0)
#define IDLE_STATUS					(1 << 4)
#define SOURCE_STATUS				(1 << 5)
#define DESTINATION_STATUS			(1 << 6)
#define ACK_STATUS					(1 << 7)

#define MDEV_PR_ERR(fmt, args ...)	\
	({				\
		pr_err("%s(%d):" fmt "\n", \
			MODULE_NAME, __LINE__, ##args); \
	})
#define MDEV_PR_INFO(fmt, args ...)	\
	({				\
		pr_info("%s(%d):" fmt "\n", \
			MODULE_NAME, __LINE__, ##args); \
	})
#define MDEV_PR_DEBUG(fmt, args ...)	\
	({				\
		; \
	})

enum {
	RX_BUFFER_TYPE = 0,
	ACK_BUFFER_TYPE,
	BUFFER_TYPE_MAX
};

typedef unsigned int remote_processor_type_t;

struct hi6xxx_remote_processor {
	const char		*name;
	remote_processor_type_t	type;
};

struct hi6xxx_ipc_device {
	void __iomem			*base;
	mbox_msg_t			*buf_pool;
	struct hisi_mbox_device		**mdev_res;
	int				rp_num;
	struct hi6xxx_remote_processor	*rp;
};

struct hi6xxx_mbox_device_priv {
	unsigned char				func;
	const char		*src_name;
	const char		*des_name;
	remote_processor_type_t	src;
	remote_processor_type_t 	des;
	unsigned char 			ack_bit;
	unsigned char 			data_bit;
	int			index;
	int			irq;
	int			capability;
	struct hi6xxx_ipc_device *idev;
};

/* ipc connect not support 64bit parameter,such as address or point, so find mbox device by table  */
static int hi6xxx_mbox_device_num = 0;
static struct hisi_mbox_device **hi6xxx_mbox_device_res = NULL;

extern unsigned int omTimerGet(void);

static inline unsigned int __ipc_data_read(void __iomem *base, int mdev, int index)
{
	return readl(base + MDEV_DATA(mdev, index));
}

static inline void __ipc_data_write(void __iomem *base, unsigned int data, int mdev, int index)
{
	writel(data, base + MDEV_DATA(mdev, index));  
}

static inline void __ipc_track_set(void __iomem *base, int mdev, int phase)
{
	unsigned int slice = omTimerGet();

	writel(slice , base + MDEV_TRACK(mdev, phase));  
}

static inline unsigned int __ipc_track_get(void __iomem *base, int mdev, int phase)
{
	return readl(base + MDEV_TRACK(mdev, phase));
}

static inline unsigned int __ipc_status_get(void __iomem *base, int mdev)
{
	return readl(base + MDEV_MODE(mdev));
}

static inline void __ipc_status_set(void __iomem *base, unsigned int status, int mdev)
{
	unsigned int val;

	val = readl(base + MDEV_MODE(mdev));
	val &= (~STATUS_MASK);
	val |= status;
	writel(val, base + MDEV_MODE(mdev));  
}

static inline void __ipc_mode_set(void __iomem *base, unsigned int mode, int mdev)
{
	unsigned int val;

	val = readl(base + MDEV_MODE(mdev));
	val &= (~ACK_CONFIG_MASK);
	val |= mode;
	writel(val, base + MDEV_MODE(mdev));  
}

static unsigned int just_resume_flag = 0;

static inline void just_resume_set(void)
{
	just_resume_flag = 1;
}

static inline void just_resume_clr(void)
{
	just_resume_flag = 0;
}

static inline unsigned int just_resume_get(void)
{
	return just_resume_flag;
}

static int hi6xxx_mdev_startup(struct hisi_mbox_device *mdev)
{
	/*
	 * nothing won't be done during suspend & resume flow for hi6xxx mailbox device.
	 * see dummy like SR function, hi6xxx_mdev_suspend & hi6xxx_mdev_resume.
	 * reserve runtime power management proceeding for further modification,
	 * if necessary.
	 */
	return 0;
}

static void hi6xxx_mdev_shutdown(struct hisi_mbox_device *mdev)
{
	/*
	 * nothing won't be done during suspend & resume flow for hi6xxx mailbox device.
	 * see dummy like SR function, hi6xxx_mdev_suspend & hi6xxx_mdev_resume.
	 * reserve runtime power management proceeding for further modification,
	 * if necessary.
	 */
	return;
}

static void
hi6xxx_mdev_dump_status(struct hisi_mbox_device *mdev)
{
	struct hi6xxx_mbox_device_priv *priv = mdev->priv;

	MDEV_PR_INFO("====  mdev-%d registers dump  ====\n", priv->index);
	MDEV_PR_INFO("  [IPCMBMODE ]   : 0x%08x\n", __ipc_status_get(priv->idev->base, priv->index));
	MDEV_PR_INFO("  [IPCMBDATA0]   : 0x%08x\n", __ipc_data_read(priv->idev->base, priv->index, 0));
	MDEV_PR_INFO("  [IPCMBDATA1]   : 0x%08x\n", __ipc_data_read(priv->idev->base, priv->index, 1));
	MDEV_PR_INFO("  [IPCMBTRACK_SENDMSG]    : 0x%08x\n", __ipc_track_get(priv->idev->base, priv->index, SEND_MSG));
	MDEV_PR_INFO("  [IPCMBTRACK_MSGSCHE]    : 0x%08x\n", __ipc_track_get(priv->idev->base, priv->index, MSG_SCHE));
	MDEV_PR_INFO("  [IPCMBTRACK_DEALSTART]  : 0x%08x\n", __ipc_track_get(priv->idev->base, priv->index, DEAL_START));
	MDEV_PR_INFO("  [IPCMBTRACK_DEALEND]    : 0x%08x\n", __ipc_track_get(priv->idev->base, priv->index, DEAL_END));
	MDEV_PR_INFO("  [IPCMBTRACK_SENDACK]    : 0x%08x\n", __ipc_track_get(priv->idev->base, priv->index, SEND_ACK));
	MDEV_PR_INFO("  [IPCMBTRACK_ACKSCHE]    : 0x%08x\n", __ipc_track_get(priv->idev->base, priv->index, ACK_SCHE));
	MDEV_PR_INFO("  [IPCMBTRACK_FREECHAN]   : 0x%08x\n", __ipc_track_get(priv->idev->base, priv->index, FREE_CHAN));
	MDEV_PR_INFO("==============  end  =============\n");

	return;
}

static void
hi6xxx_mdev_debug_track(struct hisi_mbox_device *mdev, unsigned int phase)
{
	struct hi6xxx_mbox_device_priv *priv = mdev->priv;

	if ((phase < TRACK_MAX) && (priv->index < MDEV_NUM))
		__ipc_track_set(priv->idev->base, priv->index, phase);
}

static int
hi6xxx_mdev_is_accessible(struct hisi_mbox_device *mdev,
				mbox_mail_type_t mtype, const char *rp_name)
{
	struct hi6xxx_mbox_device_priv *priv = mdev->priv;
	int ret = RPUNACCESSIBLE;

	if (!rp_name) {
		MDEV_PR_ERR("invalid rp_name for access \n");		
		goto out;
	}

	if ((TX_MAIL == mtype) &&
		(SOURCE_MBOX & priv->func) &&
		!strcmp(rp_name, priv->des_name)) {
		ret = RPACCESSIBLE;
	} else if ((RX_MAIL == mtype) &&
		(DESTINATION_MBOX & priv->func) &&
		!strcmp(rp_name, priv->src_name)) {
		ret = RPACCESSIBLE;
       }

out:
	return ret;
}

static void hi6xxx_mdev_ack_msg(struct hisi_mbox_device *mdev, mbox_msg_t *msg, mbox_msg_len_t len)
{
	struct hi6xxx_mbox_device_priv *priv = mdev->priv;
	unsigned int status = 0;
	int i;

	MDEV_PR_DEBUG("mdev %s ack msg\n", mdev->name);
	status = __ipc_status_get(priv->idev->base, priv->index);
	if ((DESTINATION_STATUS & status) && (!(AUTOMATIC_ACK_CONFIG & status))) {
		/* write data */
		for (i = 0; (i < ((priv->capability < len) ? priv->capability : len)) && msg; i++) {
			MDEV_PR_DEBUG("mdev %s msg[%d]=0x%08x\r\n", mdev->name, i, msg[i]);
			__ipc_data_write(priv->idev->base, msg[i], priv->index, i);
		}
		BSP_IPC_IntSend(priv->src, priv->ack_bit);
		MDEV_PR_DEBUG("mdev %s src=%d, ack_bit=%d\n", mdev->name, priv->src, priv->ack_bit);
	}

	return;
}

static mbox_msg_len_t
hi6xxx_mdev_hw_read(struct hisi_mbox_device *mdev, mbox_msg_t *msg)
{
	struct hi6xxx_mbox_device_priv *priv = mdev->priv;
	mbox_msg_len_t cap;
	unsigned int status = 0;
	int i;

	MDEV_PR_DEBUG("mdev %s hw read, index=%d\n", mdev->name, priv->index);

	cap = priv->capability;
	for (i = 0; i < cap; i++) {
		msg[i] = __ipc_data_read(priv->idev->base, priv->index, i);
		if(just_resume_get()) {
			MDEV_PR_INFO("just resume, mdev %s msg[%d]=0x%08x\n", mdev->name, i, msg[i]);
		} else {
			MDEV_PR_DEBUG("mdev %s msg[%d]=0x%08x\n", mdev->name, i, msg[i]);
		}
	}

	status = __ipc_status_get(priv->idev->base, priv->index);
	if ((DESTINATION_STATUS & status) && (AUTOMATIC_ACK_CONFIG & status)) {
		__ipc_status_set(priv->idev->base, IDLE_STATUS, priv->index);
		MDEV_PR_DEBUG("mdev %s no ack, status=0x%x\r\n", mdev->name, status);
	}

	just_resume_clr();
	return cap;
}

static int hi6xxx_mdev_is_ack(struct hisi_mbox_device *mdev)
{
	struct hi6xxx_mbox_device_priv *priv = mdev->priv;
	int is_ack = 0;

	if ((ACK_STATUS & __ipc_status_get(priv->idev->base, priv->index)))
		is_ack = 1;

	return is_ack;
}

static int hi6xxx_mdev_is_ipc(struct hisi_mbox_device *mdev)
{
	struct hi6xxx_mbox_device_priv *priv = mdev->priv;
	int is_ipc = 0;

	if ((DESTINATION_STATUS & __ipc_status_get(priv->idev->base, priv->index)))
		is_ipc = 1;

	return is_ipc;
}

static mbox_msg_len_t
hi6xxx_mdev_receive_msg(struct hisi_mbox_device *mdev, mbox_msg_t **buf)
{
	mbox_msg_t *_buf = NULL;
	mbox_msg_len_t len = 0;

	if (hi6xxx_mdev_is_ack(mdev)) {
		MDEV_PR_DEBUG("mdev %s recv msg, ack\n", mdev->name);
		_buf = mdev->ack_buffer;
	} else {
		MDEV_PR_DEBUG("mdev %s recv msg, ipc\n", mdev->name);
		_buf = mdev->rx_buffer;
	}

	if (_buf)
		len = hi6xxx_mdev_hw_read(mdev, _buf);
	*buf = _buf;

	return len;
}

static int hi6xxx_mdev_occupy(struct hisi_mbox_device *mdev)
{
	struct hi6xxx_mbox_device_priv *priv = mdev->priv;
	int retry = 300;

	do {
		/* If mdev is idle, then occupied by him, and set the state of mdev to source status*/
		if (!(__ipc_status_get(priv->idev->base, priv->index) & IDLE_STATUS)) {
			asm volatile("wfe");
		} else {
			__ipc_status_set(priv->idev->base, SOURCE_STATUS, priv->index);
			break;
		}
	} while (--retry);

	if (!retry) {
		hi6xxx_mdev_dump_status(mdev);
		return -ENODEV;
	}

	return 0;
}

static int
hi6xxx_mdev_hw_send(struct hisi_mbox_device *mdev, mbox_msg_t *msg,
					mbox_msg_len_t len, int ack_mode)
{
	struct hi6xxx_mbox_device_priv *priv = mdev->priv;
	int i;

	MDEV_PR_DEBUG("mdev %s hw send, ack=%d, index=%d\r\n", mdev->name, ack_mode, priv->index);

	/* ack mode config */
	if (NO_ACK == ack_mode)
		__ipc_mode_set(priv->idev->base, AUTOMATIC_ACK_CONFIG, priv->index);
	else
		__ipc_mode_set(priv->idev->base, NO_FUNC_CONFIG, priv->index);

	/* write data */
	for (i = 0; i < ((priv->capability < len) ? priv->capability : len); i++) {
		MDEV_PR_DEBUG("mdev %s msg[%d]=0x%08x\r\n", mdev->name, i, msg[i]);
		__ipc_data_write(priv->idev->base, msg[i], priv->index, i);
	}

	/* enable sending */
	BSP_IPC_IntSend(priv->des, priv->data_bit);
	MDEV_PR_DEBUG("mdev %s, des=%d, data_bit=%d \n", mdev->name, priv->des, priv->data_bit);
	return 0;
}

static int
hi6xxx_mdev_send_msg(struct hisi_mbox_device *mdev, const char *rp_name,
			mbox_msg_t *msg, mbox_msg_len_t len, int ack_mode)
{
	struct hi6xxx_mbox_device_priv *priv = mdev->priv;
	int err = 0;

	if ((FAST_MBOX & priv->func) && (DESTINATION_MBOX & priv->func)) {
		MDEV_PR_ERR("mdev %s has no tx ability\n", mdev->name);
		err = -EMDEVCLEAN;
		goto out;
	}

	if (hi6xxx_mdev_occupy(mdev)) {
		MDEV_PR_ERR("mdev %s can not be occupied\n", mdev->name);
		err = -EMDEVCLEAN;
		goto out;
	}

	(void)hi6xxx_mdev_hw_send(mdev, msg, len, ack_mode);

out:
	return err;
}

static void hi6xxx_mdev_release(struct hisi_mbox_device *mdev)
{
	struct hi6xxx_mbox_device_priv *priv = mdev->priv;

	__ipc_status_set(priv->idev->base, IDLE_STATUS, priv->index);

	asm volatile("sev");
	return;
}

static void hi6xxx_mdev_irq_handle(unsigned int para)
{
	struct hisi_mbox_device * mdev = NULL;
	struct hi6xxx_mbox_device_priv *priv = NULL;
	unsigned char pre_status;
	unsigned char cur_status;
	unsigned int i = 0;

	/* ipc connect not support 64bit parameter,such as address or point, so find mbox device by table  */
	if (NULL == hi6xxx_mbox_device_res) {
		MDEV_PR_ERR("hi6xxx_mbox_device_res null\r\n");
		return;
	}
	for (i = 0; i < hi6xxx_mbox_device_num; i++) {
		mdev = hi6xxx_mbox_device_res[i];
		priv = mdev->priv;
		if (para == priv->irq) {
			break;
		}
	}
	if ((i == hi6xxx_mbox_device_num) ||(!mdev) || (!priv)) {
		MDEV_PR_ERR("hi6xxx_mbox_device_res invalid\r\n");
		return;
	}

	pre_status = __ipc_status_get(priv->idev->base, priv->index);
	if (SOURCE_STATUS & pre_status) {
		cur_status = DESTINATION_STATUS;
	} else if (DESTINATION_STATUS & pre_status) {
		cur_status = ACK_STATUS;
	} else {
		MDEV_PR_ERR("mdev %s pre_status=0x%x, index=%d\r\n", 
				mdev->name, pre_status, priv->index);
		return;
	}
	__ipc_status_set(priv->idev->base, cur_status, priv->index);

	if(mdev->irq_handler) {
		(void)(mdev->irq_handler)(priv->irq, (void*)mdev);
	}
}

static int
hi6xxx_mdev_irq_request(struct hisi_mbox_device *mdev,
				mbox_irq_handler_t handler, void *p)
{
	struct hi6xxx_mbox_device_priv *priv = mdev->priv;
	int ret = 0;

	mdev->irq_handler = handler;

	/*64位系统需要注意修改*/
	ret = BSP_IPC_IntConnect(priv->irq, hi6xxx_mdev_irq_handle, priv->irq);
	if(0 != ret) {
		MDEV_PR_ERR("mdev %s request irq %d failed\n",
				mdev->name, priv->irq);
	}
	(void)BSP_IPC_IntEnable(priv->irq);

	return ret;
}

static void hi6xxx_mdev_irq_free(struct hisi_mbox_device *mdev, void *p)
{
	struct hi6xxx_mbox_device_priv *priv = mdev->priv;
	int ret = 0;

	ret = BSP_IPC_IntDisonnect(priv->irq, 0, (unsigned long)mdev);
	if(0 != ret) {
		MDEV_PR_ERR("mdev %s free irq %d failed\n",
				mdev->name, priv->irq);
	}
	(void)BSP_IPC_IntDisable(priv->irq);

	return;
}

static void hi6xxx_mdev_irq_enable(struct hisi_mbox_device *mdev)
{
	(void)BSP_IPC_IntEnable(mdev->cur_irq);
}

static void hi6xxx_mdev_irq_disable(struct hisi_mbox_device *mdev)
{
	(void)BSP_IPC_IntDisable(mdev->cur_irq);
}

static struct hisi_mbox_device *
hi6xxx_mdev_irq_to_mdev(struct hisi_mbox_device *_mdev, struct hisi_mbox_device **list, int irq)
{
	struct hi6xxx_mbox_device_priv *_priv = _mdev->priv;
	struct hisi_mbox_device *mdev = NULL;

	MDEV_PR_DEBUG("irq to mdev\n");

	if (!list) {
		MDEV_PR_ERR("irq to mdev, invalid input\n");
		goto out;
	}

	/* fast source and destination mailboxes use unique irq number */
	if ((SOURCE_MBOX & _priv->func) && (FAST_MBOX & _priv->func)) {
		mdev = _mdev;
	} else if ((DESTINATION_MBOX & _priv->func) && (FAST_MBOX & _priv->func)) {
		mdev = _mdev;
	} else {
		MDEV_PR_ERR("irq to mdev, the func not support\n");
	}

out:
	/* it is nearly occured */
	return mdev;
}

static struct hisi_mbox_dev_ops hi6xxx_mdev_ops = {
	.startup = hi6xxx_mdev_startup,
	.shutdown = hi6xxx_mdev_shutdown,
	.ability = hi6xxx_mdev_is_accessible,

	.recv = hi6xxx_mdev_receive_msg,
	.send = hi6xxx_mdev_send_msg,
	.ack = hi6xxx_mdev_ack_msg,
	.refresh = hi6xxx_mdev_release,

	.request_irq = hi6xxx_mdev_irq_request,
	.free_irq = hi6xxx_mdev_irq_free,
	.enable_irq = hi6xxx_mdev_irq_enable,
	.disable_irq = hi6xxx_mdev_irq_disable,
	.irq_to_mdev = hi6xxx_mdev_irq_to_mdev,

	.status = hi6xxx_mdev_dump_status,
	.track = hi6xxx_mdev_debug_track,
	.is_ack = hi6xxx_mdev_is_ack,
	.is_ipc = hi6xxx_mdev_is_ipc,
};

static void hi6xxx_mdev_put(struct hi6xxx_ipc_device *idev)
{
	struct hisi_mbox_device **list = idev->mdev_res;
	struct hisi_mbox_device *mdev = NULL;
	int i;

	iounmap(idev->base);

	kfree(idev->rp);
	kfree(idev->buf_pool);

	for (i = 0; (mdev = list[i]); i++) {
		kfree(mdev->priv);
		kfree(mdev);
	}

	return;
}

static int hi6xxx_mdev_remove(struct platform_device *pdev)
{
	struct hi6xxx_ipc_device *idev = platform_get_drvdata(pdev);

	platform_set_drvdata(pdev, NULL);

	hisi_mbox_device_unregister();

	if (idev) {
		hi6xxx_mdev_put(idev);
		kfree(idev->mdev_res);
		kfree(idev);
	}

	return 0;
}

static int
hi6xxx_mdev_get(struct hi6xxx_ipc_device *idev,
		struct hisi_mbox_device **mdevs, struct device_node *node)
{
	struct device_node *son = NULL;
	struct hisi_mbox_device *mdev;
	struct hi6xxx_mbox_device_priv *priv;
	unsigned int ack_bit;
	unsigned int data_bit;
	mbox_msg_t *buf_pool = NULL;
	mbox_msg_len_t buf_pool_len = 0;
	const char *mdev_name = NULL;
	const char *src_name = NULL;
	const char *des_name = NULL;
	remote_processor_type_t src_type;
	remote_processor_type_t des_type;
	struct hi6xxx_remote_processor *rp;
	mbox_msg_t *rx_buffer = NULL;
	mbox_msg_t *ack_buffer = NULL;
	u8 func = 0;
	u32 output[3] = {0};
	int rp_num = 0;
	int irq = 0;
	int i = 0;
	int index;
	int ret = 0;
	int capability = 0;
	void __iomem *ipc_base = NULL;

	MDEV_PR_INFO("ipc_base_phy = 0x%x, ipc_base_virt = 0x%lx\n", MDEV_BASE_PHY_ADDR, MDEV_BASE_VIRT_ADDR);
	ipc_base = (void __iomem *)MDEV_BASE_VIRT_ADDR;

	if (!ipc_base) {
		MDEV_PR_ERR("iomap error\n");
		ret = -ENOMEM;
		goto out;
	}

	ret = of_property_read_u32(node, "capability", &capability);
	if (ret) {
		MDEV_PR_ERR("prop \"capability\" error %d\n", ret);
		ret = -ENODEV;
		goto out;
	}

	ret = of_property_read_u32(node, "remote_processor_num", &rp_num);
	if (ret) {
		MDEV_PR_ERR("prop \"remote_processor_num\" error %d\n", ret);
		ret = -ENODEV;
		goto out;
	}

	rp = kmalloc(sizeof(*rp) * rp_num, GFP_KERNEL);
	if (!rp) {
		ret = -ENOMEM;
		goto out;
	}

	for (i = 0; i < rp_num; i++) {
		ret = of_property_read_string_index(node,
				"remote_processor_name", i, &rp[i].name);
		if (ret)
			break;

		ret = of_property_read_u32_index(node,
			"remote_processor_type", i, &rp[i].type);
		if (ret)
			break;

	}

	if (ret) {
		MDEV_PR_ERR("prop \"remote_processor\" error %d\n", ret);
		ret = -ENODEV;
		goto free_rp;
	}

	buf_pool_len = capability * BUFFER_TYPE_MAX *
				MDEV_NUM;
	buf_pool = kzalloc(sizeof(mbox_msg_t) * buf_pool_len, GFP_KERNEL);
	if (!buf_pool) {
		ret = -ENOMEM;
		goto free_rp;
	}

	idev->base = ipc_base;
	idev->mdev_res = mdevs;
	idev->buf_pool = buf_pool;
	idev->rp_num = rp_num;
	idev->rp = rp;

	MDEV_PR_DEBUG("ipc_base=0x%p, rp_num=%d\n", ipc_base, rp_num);

	for (i = 0; (son = of_get_next_child(node, son)); i++) {
		mdev = NULL;
		priv = NULL;
		mdev_name = NULL;
		func = 0;
		src_name = NULL;
		des_name = NULL;
		index = -1;
		rx_buffer = NULL;
		ack_buffer = NULL;

		mdev = kzalloc(sizeof(*mdev), GFP_KERNEL);
		if (!mdev) {
			ret = -ENOMEM;
			goto to_break;
		}

		priv = kzalloc(sizeof(*priv), GFP_KERNEL);
		if (!priv) {
			ret = -ENOMEM;
			goto free_mdev;
		}

		mdev_name = son->name;

		ret = of_property_read_string(son,
				"src_remote_processor_name", &src_name);
		if (ret)
			goto free_priv;
		ret = of_property_read_string(son,
				"des_remote_processor_name", &des_name);
		if (ret)
			goto free_priv;
		ret = of_property_read_u32(son, "src_remote_processor_type", &src_type);
		if (ret)
			goto free_priv;
		ret = of_property_read_u32(son, "des_remote_processor_type", &des_type);
		if (ret)
			goto free_priv;
		ret = of_property_read_u32(son, "ack_bit", &ack_bit);
		if (ret)
			goto free_priv;
		ret = of_property_read_u32(son, "data_bit", &data_bit);
		if (ret)
			goto free_priv;
		ret  = of_property_read_u32(son, "index", &index);
		if (ret)
			goto free_priv;
		ret = of_property_read_u32_array(son, "func", output, 3);
		if (ret)
			goto free_priv;
		func |= (output[0] ? FAST_MBOX : COMM_MBOX);

		func |= (output[1] ? SOURCE_MBOX : 0);

		func |= (output[2] ? DESTINATION_MBOX : 0);

		if ((FAST_MBOX & func) && (DESTINATION_MBOX & func)) {
			irq = data_bit;
		} else if ((FAST_MBOX & func) && (SOURCE_MBOX & func)) {
			irq =ack_bit;
		} else {
 			MDEV_PR_ERR("func is 0x%x, not support\n", func);
			goto free_priv;
		}
		rx_buffer = buf_pool + capability * RX_BUFFER_TYPE;
		ack_buffer = buf_pool + capability * ACK_BUFFER_TYPE;
		buf_pool = buf_pool + capability * BUFFER_TYPE_MAX;

		priv->capability = capability;
		priv->func = func;
		priv->src_name = src_name;
		priv->des_name = des_name;
		priv->src = src_type;
		priv->des = des_type;
		priv->ack_bit = ack_bit;
		priv->data_bit = data_bit;
		priv->irq = irq;
		priv->index = index;
		priv->idev = idev;

		mdev->name = mdev_name;
		mdev->priv = priv;
		mdev->rx_buffer =  rx_buffer;
		mdev->ack_buffer = ack_buffer;
		mdev->ops = &hi6xxx_mdev_ops;
		mdevs[i] = mdev;

		continue;
free_priv:
		kfree(priv);
free_mdev:
		kfree(mdev);
to_break:
		break;
	}

	if (ret)
		goto deinit_mdevs;

	return ret;

deinit_mdevs:
	while (i--) {
		kfree(mdevs[i]->priv);
		kfree(mdevs[i]);
	}

	kfree(idev->buf_pool);

free_rp:
	kfree(rp);
out:
	return ret;
}

static int hi6xxx_mdev_probe(struct platform_device *pdev)
{
	struct hi6xxx_ipc_device *idev = NULL;
	struct hisi_mbox_device **mdev_res = NULL;
	struct device_node *node = NULL;
	int mdev_num = 0;
	int ret = 0;
	int i = 0;

	MDEV_PR_INFO("enter\n");

       /* mdev memory set zero, and all channel set idle */
       memset((void*)MDEV_BASE_VIRT_ADDR, 0, MDEV_ADDR_SIZE);
       for (i = 0; i < MDEV_NUM; i++) {
		__ipc_status_set((void __iomem *)MDEV_BASE_VIRT_ADDR, IDLE_STATUS, i);
	}
       
	idev = kmalloc(sizeof(*idev), GFP_KERNEL);
	if (!idev) {
		MDEV_PR_ERR("no mem for ipc resouce\n");
		ret = -ENOMEM;
		goto out;
	}

	node = of_find_compatible_node(NULL, NULL, "hisilicon,hi6xxx_mdev");
	if (!node) {
		MDEV_PR_ERR("dts[%s] node not found\n", "hisilicon,hi6xxx_mdev");
		ret = -ENODEV;
		goto free_idev;
	}

	ret = of_property_read_u32(node, "mailboxes", &mdev_num);
	if (ret) {
		MDEV_PR_ERR("no mailboxes resources\n");
		ret = -ENODEV;
		goto free_idev;
	}

	mdev_res = kzalloc((sizeof(*mdev_res) * mdev_num), GFP_KERNEL);
	if (!mdev_res) {
		ret = -ENOMEM;
		goto free_idev;
	}

	ret = hi6xxx_mdev_get(idev, mdev_res, node);
	if (ret) {
		MDEV_PR_ERR("can not get ipc resource\n");
		ret = -ENODEV;
		goto free_mdevs;
	}

	ret = hisi_mbox_device_register(&pdev->dev, mdev_res);
	if (ret) {
		MDEV_PR_ERR("mdevs register failed\n");
		ret = -ENODEV;
		goto put_res;
	}

	/* ipc connect not support 64bit parameter,such as address or point, so find mbox device by table  */
	hi6xxx_mbox_device_num = mdev_num;
	hi6xxx_mbox_device_res = mdev_res;

	platform_set_drvdata(pdev, idev);

	MDEV_PR_INFO("leave\n");

	return 0;

put_res:
	hi6xxx_mdev_put(idev);
free_mdevs:
	kfree(idev->mdev_res);
free_idev:
	kfree(idev);
out:
	return ret;
}

static int hi6xxx_mdev_suspend(struct device *dev)
{
	struct platform_device *pdev =
		container_of(dev, struct platform_device, dev);
	struct hi6xxx_ipc_device *idev = platform_get_drvdata(pdev);

	MDEV_PR_INFO("suspend +\n");
	if (idev)
		hisi_mbox_device_deactivate(idev->mdev_res);
	MDEV_PR_INFO("suspend -\n");
	return 0;
}

static int hi6xxx_mdev_resume(struct device *dev)
{
	struct platform_device *pdev =
		container_of(dev, struct platform_device, dev);
	struct hi6xxx_ipc_device *idev = platform_get_drvdata(pdev);

	MDEV_PR_INFO("resume +\n");
	if (idev)
		hisi_mbox_device_activate(idev->mdev_res);
	just_resume_set();
	MDEV_PR_INFO("resume -\n");
	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id hi6xxx_mdev_of_match[] = {
	{.compatible = "hisilicon,hi6xxx_mdev",},
	{},
};
MODULE_DEVICE_TABLE(of, hi6xxx_mdev_of_match);
#endif

static const struct dev_pm_ops hi6xxx_mdev_pm_ops = {
	.suspend_late = hi6xxx_mdev_suspend,
	.resume_early = hi6xxx_mdev_resume,
};

static struct platform_driver hi6xxx_mdev_driver = {
	.probe = hi6xxx_mdev_probe,
	.remove = hi6xxx_mdev_remove,
	.driver = {
		.name = "hi6xxx_mdev",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(hi6xxx_mdev_of_match),
		.pm = &hi6xxx_mdev_pm_ops,
	},
};

static int __init hi6xxx_mdev_init(void)
{
	MDEV_PR_INFO("enter");
	platform_driver_register(&hi6xxx_mdev_driver);
	MDEV_PR_INFO("leave");
	return 0;
}
core_initcall(hi6xxx_mdev_init);

static void __exit hi6xxx_mdev_exit(void)
{
	platform_driver_unregister(&hi6xxx_mdev_driver);
	return;
}
module_exit(hi6xxx_mdev_exit);

MODULE_DESCRIPTION("hi6xxx mailbox device driver");
MODULE_LICENSE("GPL V2");
