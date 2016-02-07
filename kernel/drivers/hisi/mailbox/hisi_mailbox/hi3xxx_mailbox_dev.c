/*
 * hi3xxx mailbox device driver
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
#include <linux/hisi/hi3xxx_irq_affinity.h>

#define IPCBITMASK(n)				(1 << (n))
#define IPCMBxSOURCE(mdev)			((mdev) << 6)
#define IPCMBxDSET(mdev)			(((mdev) << 6) + 0x04)
#define IPCMBxDCLR(mdev)			(((mdev) << 6) + 0x08)
#define IPCMBxDSTATUS(mdev)			(((mdev) << 6) + 0x0C)
#define IPCMBxMODE(mdev)			(((mdev) << 6) + 0x10)
#define IPCMBxIMASK(mdev)			(((mdev) << 6) + 0x14)
#define IPCMBxICLR(mdev)			(((mdev) << 6) + 0x18)
#define IPCMBxSEND(mdev)			(((mdev) << 6) + 0x1C)
#define IPCMBxDATA(mdev, index)		(((mdev) << 6) + 0x20 + ((index) << 2))
#define IPCCPUxIMST(cpu)			(((cpu) << 3) + 0x800)
#define IPCCPUxIRST(cpu)			(((cpu) << 3) + 0x804)
#define IPCLOCK()				(0xA00)

#define MODULE_NAME				"hi3xxx_mailbox_dev"
#define FAST_MBOX				(1 << 0)
#define COMM_MBOX				(1 << 1)
#define SOURCE_MBOX				(1 << 2)
#define DESTINATION_MBOX			(1 << 3)
#define SECURE_MBOX				(1 << 4)
#define NONSECURE_MBOX				(1 << 5)

#define HI3XXX_PL320_IPC_MBOX_NUM_NAX		26
#define IPC_UNLOCKED				0x00000000
#define IPCACKMSG				0x00000000
#define COMM_MBOX_IRQ				(-2)
#define AUTOMATIC_ACK_CONFIG			(1 << 0)
#define NO_FUNC_CONFIG				(0 << 0)

#define IDLE_STATUS				(1 << 4)
#define SOURCE_STATUS				(1 << 5)
#define DESTINATION_STATUS			(1 << 6)
#define ACK_STATUS				(1 << 7)

/* Optimize interrupts assignment */
#define IPC_IRQ_AFFINITY_CPU			(1)

#define MDEV_TRACE(fmt, args ...)	\
	({				\
		pr_err("%s(%d):" fmt "\n", \
			MODULE_NAME, __LINE__, ##args); \
	})
#define MDEV_DEBUG(fmt, args ...)	\
	({				\
		; \
	})

enum {
	RX_BUFFER_TYPE = 0,
	ACK_BUFFER_TYPE,
	MBOX_BUFFER_TYPE_MAX,
};

/*
 * Table for available remote processors. DTS sub-node, "remote_processor_type",
 * of node, "hi3xxx_mdev", is configured according to the table.
 *
 * If the table was modified, DTS configiuration should be updated accordingly.
 */
typedef enum {
	GIC_1 = 0,
	GIC_2,
	IOM3,
	LPM3,
	HIFI,
	MCPU,
	BBE16,
	UNCERTAIN_REMOTE_PROCESSOR,
	HI3XXX_RP_TYPES
} remote_processor_type_t;

struct hi3xxx_remote_processor {
	const char		*name;
	remote_processor_type_t	type;
};

struct hi3xxx_common_mbox_info {
	int			gic_1_irq_requested;
	int			gic_2_irq_requested;
	int			cmbox_gic_1_irq;
	int			cmbox_gic_2_irq;
	struct hisi_mbox_device *cmdev;
};

struct hi3xxx_ipc_device {
	void __iomem			*base;
	u32				unlock;
	mbox_msg_t			*buf_pool;
	struct hi3xxx_common_mbox_info	*cmbox_info;
	struct hisi_mbox_device		**mdev_res;
	int				rp_num;
	struct hi3xxx_remote_processor	*rp;
};

struct hi3xxx_mbox_device_priv {
	u8			func;
	const char		*src_name;
	const char		*des_name;
	remote_processor_type_t	src;
	remote_processor_type_t des;
	int			index;
	int			irq;
	int			capability;
	struct hi3xxx_ipc_device *idev;
};

static inline void __ipc_lock(void __iomem *base, unsigned int lock_key)
{
	__raw_writel(lock_key, base + IPCLOCK());
}

static inline void __ipc_unlock(void __iomem *base, unsigned int key)
{
	__raw_writel(key, base + IPCLOCK());
}

static inline unsigned int __ipc_lock_status(void __iomem *base)
{
	return __raw_readl(base + IPCLOCK());
}

static inline void __ipc_set_src(void __iomem *base, int source, int mdev)
{
	__raw_writel(IPCBITMASK(source), base + IPCMBxSOURCE(mdev));
}

static inline unsigned int __ipc_read_src(void __iomem *base, int mdev)
{
	return __raw_readl(base + IPCMBxSOURCE(mdev));
}

static inline void __ipc_set_des(void __iomem *base, int source, int mdev)
{
	__raw_writel(IPCBITMASK(source), base + IPCMBxDSET(mdev));
}

static inline void __ipc_clr_des(void __iomem *base, int source, int mdev)
{
	__raw_writel(IPCBITMASK(source), base + IPCMBxDCLR(mdev));
}

static inline unsigned int __ipc_des_status(void __iomem *base, int mdev)
{
	return __raw_readl(base + IPCMBxDSTATUS(mdev));
}

static inline void __ipc_send(void __iomem *base, int source, int mdev)
{
	__raw_writel(IPCBITMASK(source), base + IPCMBxSEND(mdev));
}

static inline unsigned int __ipc_read(void __iomem *base, int mdev, int index)
{
	return __raw_readl(base + IPCMBxDATA(mdev, index));
}

static inline void __ipc_write(void __iomem *base, u32 data, int mdev, int index)
{
	__raw_writel(data, base + IPCMBxDATA(mdev, index));
}

static inline unsigned int __ipc_cpu_imask_get(void __iomem *base, int mdev)
{
	return __raw_readl(base + IPCMBxIMASK(mdev));
}

static inline void __ipc_cpu_imask_clr(void __iomem *base, int source, int mdev)
{
	unsigned int reg;

	reg = __raw_readl(base + IPCMBxIMASK(mdev));
	reg = reg & (~(IPCBITMASK(source)));

	__raw_writel(reg, base + IPCMBxIMASK(mdev));
}

static inline void __ipc_cpu_imask_all(void __iomem *base, int mdev)
{
	unsigned int reg = 0x0;
	int i;

	for (i = GIC_1; i < UNCERTAIN_REMOTE_PROCESSOR; i++)
		reg = reg | IPCBITMASK(i);

	__raw_writel(reg, base + IPCMBxIMASK(mdev));
}

static inline void __ipc_cpu_iclr(void __iomem *base, int source, int mdev)
{
	__raw_writel(IPCBITMASK(source), base + IPCMBxICLR(mdev));
}

static inline int __ipc_cpu_istatus(void __iomem *base, int mdev)
{
	return __raw_readl(base + IPCMBxICLR(mdev));
}

static inline unsigned int __ipc_mbox_istatus(void __iomem *base, int cpu)
{
	return __raw_readl(base + IPCCPUxIMST(cpu));
}

static inline unsigned int __ipc_mbox_irstatus(void __iomem *base, int cpu)
{
	return __raw_readl(base + IPCCPUxIRST(cpu));
}

static inline unsigned int __ipc_status(void __iomem *base, int mdev)
{
	return __raw_readl(base + IPCMBxMODE(mdev));
}

static inline void __ipc_mode(void __iomem *base, unsigned int mode, int mdev)
{
	__raw_writel(mode, base + IPCMBxMODE(mdev));
}

static int hi3xxx_mdev_startup(struct hisi_mbox_device *mdev)
{
	/*
	 * nothing won't be done during suspend & resume flow for HI3xxx IPC.
	 * see dummy like SR function, hi3xxx_mdev_suspend & hi3xxx_mdev_resume.
	 * reserve runtime power management proceeding for further modification,
	 * if necessary.
	 */
	return 0;
}

static void hi3xxx_mdev_shutdown(struct hisi_mbox_device *mdev)
{
	/*
	 * nothing won't be done during suspend & resume flow for HI3xxx IPC.
	 * see dummy like SR function, hi3xxx_mdev_suspend & hi3xxx_mdev_resume.
	 * reserve runtime power management proceeding for further modification,
	 * if necessary.
	 */
	return;
}

static void
hi3xxx_mdev_dump_status(struct hisi_mbox_device *mdev)
{
	struct hi3xxx_mbox_device_priv *priv = mdev->priv;

	pr_info("====  mdev-%d registers dump  ====\n  [IPCMBSOURCE]  : 0x%08x\n  [IPCMBDSTATUS] : 0x%08x\n  [IPCMBMODE]    : 0x%08x\n  [IPCMBIMASK]   : 0x%08x\n  [IPCMBICLR]    : 0x%08x\n  [IPCCPUIRST]   : 0x%08x\n  [IPCMBDATA0]   : 0x%08x\n  [IPCMBDATA1]   : 0x%08x\n==============  end  =============\n",
		priv->index,
		__ipc_read_src(priv->idev->base, priv->index),
		__ipc_des_status(priv->idev->base, priv->index),
		__ipc_status(priv->idev->base, priv->index),
		__ipc_cpu_imask_get(priv->idev->base, priv->index),
		__ipc_cpu_istatus(priv->idev->base, priv->index),
		__ipc_mbox_irstatus(priv->idev->base, priv->des),
		__ipc_read(priv->idev->base, priv->index, 0),
		__ipc_read(priv->idev->base, priv->index, 1));

	return;
}

static void
hi3xxx_mdev_debug_track(struct hisi_mbox_device *mdev, unsigned int phase)
{
	return;
}

static int
hi3xxx_mdev_is_accessible(struct hisi_mbox_device *mdev,
				mbox_mail_type_t mtype, const char *rp_name)
{
	struct hi3xxx_mbox_device_priv *priv = mdev->priv;
	struct hi3xxx_ipc_device *idev = priv->idev;
	struct hi3xxx_remote_processor *rp = idev->rp;
	int rp_num = idev->rp_num;
	int ret = RPUNACCESSIBLE;
	int i;

	if (!rp_name)
		goto out;

	/* fast mailbox */
	if ((FAST_MBOX & priv->func)) {
		if ((TX_MAIL == mtype) && (SOURCE_MBOX & priv->func)) {
			if (!strcmp(rp_name, priv->des_name))
				ret = RPACCESSIBLE;
		} else if ((RX_MAIL == mtype) &&
				(DESTINATION_MBOX & priv->func)) {
			for (i = 0; i < rp_num; i++) {
				if (!strcmp(rp_name, rp[i].name)) {
					if (priv->src == rp[i].type)
						ret = RPACCESSIBLE;
					break;
				}
			}
		} else {
			;
		}

		goto out;
	}

	/* common mailbox */
	for (i = 0; i < rp_num; i++) {
		if (!strcmp(rp_name, rp[i].name)) {
			if (!strcmp(rp_name, HISI_MAILBOX_RP_UNCERTAIN))
				ret = RPUNCERTAIN;
			else
				ret = RPACCESSIBLE;
			goto out;
		}
	}

out:
	return ret;
}

static void hi3xxx_mdev_clr_irq_and_ack(struct hisi_mbox_device *mdev)
{
	struct hi3xxx_mbox_device_priv *priv = mdev->priv;
	unsigned int status = 0;
	unsigned int imask;
	int i;

	/*
	 * temporarily, local processor will clean msg register,
	 * and ack zero for an ipc from remote processors.
	 */
	for (i = 0; i < priv->capability; i++)
		__ipc_write(priv->idev->base, IPCACKMSG, priv->index, i);

	imask = __ipc_cpu_imask_get(priv->idev->base, priv->index);
	if (IPCBITMASK(GIC_1) & (~imask))
		__ipc_cpu_iclr(priv->idev->base, GIC_1, priv->index);

	if (IPCBITMASK(GIC_2) & (~imask))
		__ipc_cpu_iclr(priv->idev->base, GIC_2, priv->index);

	status = __ipc_status(priv->idev->base, priv->index);
	if ((DESTINATION_STATUS & status) && (!(AUTOMATIC_ACK_CONFIG & status))) {
		if (IPCBITMASK(GIC_1) & (~imask))
			__ipc_send(priv->idev->base, GIC_1, priv->index);

		if (IPCBITMASK(GIC_2) & (~imask))
			__ipc_send(priv->idev->base, GIC_2, priv->index);
	}

	return;
}

static void hi3xxx_mdev_ack(struct hisi_mbox_device *mdev)
{
	return;
}

static mbox_msg_len_t
hi3xxx_mdev_hw_read(struct hisi_mbox_device *mdev, mbox_msg_t *msg)
{
	struct hi3xxx_mbox_device_priv *priv = mdev->priv;
	mbox_msg_len_t cap;
	int i;

	cap = priv->capability;
	for (i = 0; i < cap; i++)
		msg[i] = __ipc_read(priv->idev->base, priv->index, i);

	return cap;
}

static int hi3xxx_mdev_is_ack(struct hisi_mbox_device *mdev)
{
	struct hi3xxx_mbox_device_priv *priv = mdev->priv;
	int is_ack = 0;

	if ((ACK_STATUS & __ipc_status(priv->idev->base, priv->index)))
		is_ack = 1;

	return is_ack;
}

static int hi3xxx_mdev_is_ipc(struct hisi_mbox_device *mdev)
{
	struct hi3xxx_mbox_device_priv *priv = mdev->priv;
	int is_ipc = 0;

	if ((DESTINATION_STATUS & __ipc_status(priv->idev->base, priv->index)))
		is_ipc = 1;

	return is_ipc;
}

static mbox_msg_len_t
hi3xxx_mdev_receive_msg(struct hisi_mbox_device *mdev, mbox_msg_t **buf)
{
	mbox_msg_t *_buf = NULL;
	mbox_msg_len_t len = 0;

	if (hi3xxx_mdev_is_ack(mdev))
		_buf = mdev->ack_buffer;
	else
		_buf = mdev->rx_buffer;

	if (_buf)
		len = hi3xxx_mdev_hw_read(mdev, _buf);
	*buf = _buf;

	hi3xxx_mdev_clr_irq_and_ack(mdev);
	return len;
}

static void
hi3xxx_mdev_config(struct hisi_mbox_device *mdev,
		const char *src_name, const char *des_name)
{
	struct hi3xxx_mbox_device_priv *priv = mdev->priv;
	struct hi3xxx_ipc_device *idev = priv->idev;
	struct hi3xxx_remote_processor *rp = idev->rp;
	int rp_num = idev->rp_num;
	int i;

	if ((FAST_MBOX & priv->func))
		return;

	for (i = 0; i < rp_num; i++) {
		if (!strcmp(src_name, rp[i].name))
			priv->src = rp[i].type;

		if (!strcmp(des_name, rp[i].name))
			priv->des = rp[i].type;
	}

	return;
}

static int hi3xxx_mdev_occupy(struct hisi_mbox_device *mdev)
{
	struct hi3xxx_mbox_device_priv *priv = mdev->priv;
	int retry = 11;

	do {
		/*
		 * Hardware lock
		 * A hardware lock is needed here to lock a mailbox resource,
		 * which could be used by another remote proccessor, such as
		 * a HiIPCV230 common mailbox-25/mailbox-26.
		 */
		if (!(__ipc_status(priv->idev->base, priv->index) & IDLE_STATUS)) {
			asm volatile("wfe");
		} else {
			__ipc_set_src(priv->idev->base, priv->src, priv->index);
			if (__ipc_read_src(priv->idev->base, priv->index) &
				IPCBITMASK(priv->src))
				break;
		}
		/* Hardware unlock */
	} while (--retry);

	if (!retry)
		return -ENODEV;

	return 0;
}

static int
hi3xxx_mdev_hw_send(struct hisi_mbox_device *mdev, mbox_msg_t *msg,
					mbox_msg_len_t len, int ack_mode)
{
	struct hi3xxx_mbox_device_priv *priv = mdev->priv;
	int i;

	/* interrupts unmask */
	__ipc_cpu_imask_all(priv->idev->base, priv->index);
	__ipc_cpu_imask_clr(priv->idev->base, priv->src, priv->index);
	__ipc_cpu_imask_clr(priv->idev->base, priv->des, priv->index);

	/* des config */
	if ((COMM_MBOX & priv->func))
		__ipc_set_des(priv->idev->base, priv->des, priv->index);

	/* ipc mode config */
	if (AUTO_ACK == ack_mode)
		__ipc_mode(priv->idev->base, AUTOMATIC_ACK_CONFIG, priv->index);
	else
		__ipc_mode(priv->idev->base, NO_FUNC_CONFIG, priv->index);

	/* write data */
	for (i = 0; i < ((priv->capability < len) ? priv->capability : len); i++)
		__ipc_write(priv->idev->base, msg[i], priv->index, i);

	/* enable sending */
	__ipc_send(priv->idev->base, priv->src, priv->index);
	return 0;
}

static int
hi3xxx_mdev_send_msg(struct hisi_mbox_device *mdev, const char *rp_name,
			mbox_msg_t *msg, mbox_msg_len_t len, int ack_mode)
{
	struct hi3xxx_mbox_device_priv *priv = mdev->priv;
	int err = 0;

	if ((FAST_MBOX & priv->func) && (DESTINATION_MBOX & priv->func)) {
		MDEV_TRACE("mdev %s has no tx ability\n", mdev->name);
		err = -EMDEVCLEAN;
		goto out;
	}

	hi3xxx_mdev_config(mdev, HISI_MAILBOX_RP_GIC2, rp_name);

	if (hi3xxx_mdev_occupy(mdev)) {
		MDEV_TRACE("mdev %s can not be occupied\n", mdev->name);
		err = -EMDEVCLEAN;
		goto out;
	}

	(void)hi3xxx_mdev_hw_send(mdev, msg, len, ack_mode);

out:
	return err;
}

static void hi3xxx_mdev_release(struct hisi_mbox_device *mdev)
{
	struct hi3xxx_mbox_device_priv *priv = mdev->priv;

	__ipc_cpu_imask_all(priv->idev->base, priv->index);
	__ipc_set_src(priv->idev->base, priv->src, priv->index);

	asm volatile("sev");
	return;
}

static int
hi3xxx_mdev_irq_request(struct hisi_mbox_device *mdev,
				mbox_irq_handler_t handler, void *p)
{
	struct hi3xxx_mbox_device_priv *priv = mdev->priv;
	int ret = 0;

	if (COMM_MBOX_IRQ == priv->irq) {
		if (!priv->idev->cmbox_info->gic_1_irq_requested++) {
			ret = request_irq(priv->idev->cmbox_info->cmbox_gic_1_irq,
					handler,
					IRQF_DISABLED,
					mdev->name,
					(void *)priv->idev->cmbox_info->cmdev);
			if (ret) {
				MDEV_TRACE("common %s request gic_1_irq %d failed\n",
					mdev->name,
					priv->idev->cmbox_info->cmbox_gic_1_irq);
				priv->idev->cmbox_info->gic_1_irq_requested--;
				goto out;
			}

			hisi_irqaffinity_register(priv->idev->cmbox_info->cmbox_gic_1_irq,
									IPC_IRQ_AFFINITY_CPU);
		}

		if (!priv->idev->cmbox_info->gic_2_irq_requested++) {
			ret = request_irq(priv->idev->cmbox_info->cmbox_gic_2_irq,
					handler,
					IRQF_DISABLED,
					mdev->name,
					(void *)priv->idev->cmbox_info->cmdev);
			if (ret) {
				MDEV_TRACE("common %s request gic_2_irq %d failed\n",
					mdev->name,
					priv->idev->cmbox_info->cmbox_gic_2_irq);
				priv->idev->cmbox_info->gic_2_irq_requested--;
				free_irq(priv->idev->cmbox_info->cmbox_gic_1_irq,
					(void *)priv->idev->cmbox_info->cmdev);
				priv->idev->cmbox_info->gic_1_irq_requested--;
				goto out;
			}

			hisi_irqaffinity_register(priv->idev->cmbox_info->cmbox_gic_2_irq,
									IPC_IRQ_AFFINITY_CPU);
		}
	} else if (priv->idev->cmbox_info->cmbox_gic_1_irq == priv->irq) {
		if (!priv->idev->cmbox_info->gic_1_irq_requested++) {
			ret = request_irq(priv->irq,
					handler,
					IRQF_DISABLED,
					mdev->name,
					(void *)priv->idev->cmbox_info->cmdev);
			if (ret) {
				MDEV_TRACE("fast source %s request gic_1_irq %d failed\n",
					mdev->name, priv->irq);
				priv->idev->cmbox_info->gic_1_irq_requested--;
				goto out;
			}

			hisi_irqaffinity_register(priv->irq, IPC_IRQ_AFFINITY_CPU);
		}
	} else if (priv->idev->cmbox_info->cmbox_gic_2_irq == priv->irq) {
		if (!priv->idev->cmbox_info->gic_2_irq_requested++) {
			ret = request_irq(priv->irq,
					handler,
					IRQF_DISABLED,
					mdev->name,
					(void *)priv->idev->cmbox_info->cmdev);
			if (ret) {
				MDEV_TRACE("fast source %s request gic_2_irq %d failed\n",
					mdev->name, priv->irq);
				priv->idev->cmbox_info->gic_2_irq_requested--;
				goto out;
			}

			hisi_irqaffinity_register(priv->irq, IPC_IRQ_AFFINITY_CPU);
		}
	} else {
		ret = request_irq(priv->irq, handler, IRQF_DISABLED, mdev->name, p);
		if (ret) {
			MDEV_TRACE("fast desitnation %s request irq %d failed\n",
					mdev->name, priv->irq);
			goto out;
		}

		hisi_irqaffinity_register(priv->irq, IPC_IRQ_AFFINITY_CPU);
	}

out:
	return ret;
}

static void hi3xxx_mdev_irq_free(struct hisi_mbox_device *mdev, void *p)
{
	struct hi3xxx_mbox_device_priv *priv = mdev->priv;

	if (COMM_MBOX_IRQ == priv->irq) {
		if (!--priv->idev->cmbox_info->gic_1_irq_requested)
			free_irq(priv->idev->cmbox_info->cmbox_gic_1_irq,
				(void *)priv->idev->cmbox_info->cmdev);

		if (!--priv->idev->cmbox_info->gic_2_irq_requested)
			free_irq(priv->idev->cmbox_info->cmbox_gic_2_irq,
				(void *)priv->idev->cmbox_info->cmdev);
	} else if (priv->idev->cmbox_info->cmbox_gic_1_irq == priv->irq) {
		if (!--priv->idev->cmbox_info->gic_1_irq_requested)
			free_irq(priv->irq, (void *)priv->idev->cmbox_info->cmdev);
	} else if (priv->idev->cmbox_info->cmbox_gic_2_irq == priv->irq) {
		if (!--priv->idev->cmbox_info->gic_2_irq_requested)
			free_irq(priv->irq, (void *)priv->idev->cmbox_info->cmdev);
	} else {
		free_irq(priv->irq, p);
	}

	return;
}

static void hi3xxx_mdev_irq_enable(struct hisi_mbox_device *mdev)
{
	enable_irq((unsigned int)mdev->cur_irq);
}

static void hi3xxx_mdev_irq_disable(struct hisi_mbox_device *mdev)
{
	disable_irq_nosync((unsigned int)mdev->cur_irq);
}

static struct hisi_mbox_device *
hi3xxx_mdev_irq_to_mdev(struct hisi_mbox_device *_mdev, struct hisi_mbox_device **list, int irq)
{
	struct hi3xxx_mbox_device_priv *_priv = _mdev->priv;
	struct hisi_mbox_device *mdev = NULL;
	struct hi3xxx_mbox_device_priv *priv = NULL;
	remote_processor_type_t src = UNCERTAIN_REMOTE_PROCESSOR;
	unsigned int regval = 0x0;
	int i;

	if (!list) {
		MDEV_TRACE("invalid input\n");
		goto err;
	}

	/* fast destination mailboxes use unique irq number */
	if ((DESTINATION_MBOX & _priv->func) && (FAST_MBOX & _priv->func)) {
		mdev = _mdev;
		goto out;
	}

	/* fast source & common mailboxes share GIC_1 & GIC_2 irq number */
	if (irq == _priv->idev->cmbox_info->cmbox_gic_1_irq) {
		src = GIC_1;
	} else if (irq == _priv->idev->cmbox_info->cmbox_gic_2_irq) {
		src = GIC_2;
	} else {
		MDEV_TRACE("odd irq for hisi mailboxes\n");
		goto err;
	}

	regval = __ipc_mbox_istatus(_priv->idev->base, src);
	for (i = 0; (mdev = list[i]); i++) {
		priv = mdev->priv;

		if ((regval & IPCBITMASK(priv->index)) &&
			((priv->func & SOURCE_MBOX) || (priv->func & COMM_MBOX)))
			goto out;
	}

err:
out:
	/* it is nearly occured */
	return mdev;
}

struct hisi_mbox_dev_ops hi3xxx_mdev_ops = {
	.startup = hi3xxx_mdev_startup,
	.shutdown = hi3xxx_mdev_shutdown,
	.ability = hi3xxx_mdev_is_accessible,

	.recv = hi3xxx_mdev_receive_msg,
	.send = hi3xxx_mdev_send_msg,
	.ack = hi3xxx_mdev_ack,
	.refresh = hi3xxx_mdev_release,

	.request_irq = hi3xxx_mdev_irq_request,
	.free_irq = hi3xxx_mdev_irq_free,
	.enable_irq = hi3xxx_mdev_irq_enable,
	.disable_irq = hi3xxx_mdev_irq_disable,
	.irq_to_mdev = hi3xxx_mdev_irq_to_mdev,
	.status = hi3xxx_mdev_dump_status,
	.track = hi3xxx_mdev_debug_track,
	.is_ack = hi3xxx_mdev_is_ack,
	.is_ipc = hi3xxx_mdev_is_ipc,
};

static void hi3xxx_mdev_put(struct hi3xxx_ipc_device *idev)
{
	struct hisi_mbox_device **list = idev->mdev_res;
	struct hisi_mbox_device *mdev = NULL;
	int i;

	iounmap(idev->base);

	kfree(idev->rp);
	kfree(idev->cmbox_info);
	kfree(idev->buf_pool);

	for (i = 0; (mdev = list[i]); i++) {
		kfree(mdev->priv);
		kfree(mdev);
	}

	return;
}

static int hi3xxx_mdev_remove(struct platform_device *pdev)
{
	struct hi3xxx_ipc_device *idev = platform_get_drvdata(pdev);

	platform_set_drvdata(pdev, NULL);

	hisi_mbox_device_unregister();

	if (idev) {
		hi3xxx_mdev_put(idev);
		kfree(idev->mdev_res);
		kfree(idev);
	}

	return 0;
}

static int
hi3xxx_mdev_get(struct hi3xxx_ipc_device *idev,
		struct hisi_mbox_device **mdevs, struct device_node *node)
{
	struct device_node *son = NULL;
	struct hi3xxx_common_mbox_info *cmbox_info = NULL;
	struct hisi_mbox_device *mdev;
	struct hi3xxx_mbox_device_priv *priv;
	remote_processor_type_t	src_bit;
	remote_processor_type_t	des_bit;
	mbox_msg_t *buf_pool = NULL;
	mbox_msg_len_t buf_pool_len = 0;
	const char *mdev_name = NULL;
	const char *src_name = NULL;
	const char *des_name = NULL;
	struct hi3xxx_remote_processor *rp;
	mbox_msg_t *rx_buffer = NULL;
	mbox_msg_t *ack_buffer = NULL;
	u8 func = 0;
	u32 output[3] = {0};
	int rp_num = 0;
	int irq = 0;
	int i = 0;
	int index;
	int ret = 0;
	int cm_gic_1_irq = -1;
	int cm_gic_2_irq = -1;
	int capability = 0;
	u32 unlock = 0;
	void __iomem *ipc_base = NULL;

	ipc_base = of_iomap(node, 0);
	if (!ipc_base) {
		MDEV_TRACE("iomap error\n");
		ret = -ENOMEM;
		goto out;
	}

	ret = of_property_read_u32(node, "capability", &capability);
	if (ret) {
		MDEV_TRACE("prop \"capability\" error %d\n", ret);
		ret = -ENODEV;
		goto to_iounmap;
	}

	ret = of_property_read_u32(node, "unlock_key", &unlock);
	if (ret) {
		MDEV_TRACE("prop \"key\" error %d\n", ret);
		ret = -ENODEV;
		goto to_iounmap;
	}

	ret = of_property_read_u32(node, "remote_processor_num", &rp_num);
	if (ret) {
		MDEV_TRACE("prop \"remote_processor_num\" error %d\n", ret);
		ret = -ENODEV;
		goto to_iounmap;
	}

	rp = kmalloc(sizeof(*rp) * rp_num, GFP_KERNEL);
	if (!rp) {
		ret = -ENOMEM;
		goto to_iounmap;
	}

	for (i = 0; i < rp_num; i++) {
		ret = of_property_read_string_index(node,
				"remote_processor", i, &rp[i].name);
		if (ret)
			break;

		ret = of_property_read_u32_index(node,
			"remote_processor_type", i, &rp[i].type);
		if (ret)
			break;

	}

	if (ret) {
		MDEV_TRACE("prop \"remote_processor\" error %d\n", ret);
		ret = -ENODEV;
		goto free_rp;
	}

	cmbox_info = kmalloc(sizeof(*cmbox_info), GFP_KERNEL);
	if (!cmbox_info) {
		ret = -ENOMEM;
		goto free_rp;
	}

	buf_pool_len = capability * MBOX_BUFFER_TYPE_MAX *
				HI3XXX_PL320_IPC_MBOX_NUM_NAX;
	buf_pool = kzalloc(sizeof(mbox_msg_t) * buf_pool_len, GFP_KERNEL);
	if (!buf_pool) {
		ret = -ENOMEM;
		goto free_cmbox;
	}

	cm_gic_1_irq = irq_of_parse_and_map(node, 0);
	cm_gic_2_irq = irq_of_parse_and_map(node, 1);

	cmbox_info->gic_1_irq_requested = 0;
	cmbox_info->gic_2_irq_requested = 0;
	cmbox_info->cmbox_gic_1_irq = cm_gic_1_irq;
	cmbox_info->cmbox_gic_2_irq = cm_gic_2_irq;

	idev->cmbox_info = cmbox_info;
	idev->unlock = unlock;
	idev->base = ipc_base;
	idev->mdev_res = mdevs;
	idev->buf_pool = buf_pool;
	idev->rp_num = rp_num;
	idev->rp = rp;

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
				"src_remote_processor", &src_name);
		if (ret)
			goto free_priv;

		ret = of_property_read_string(son,
				"des_remote_processor", &des_name);
		if (ret)
			goto free_priv;

		ret = of_property_read_u32(son, "src_bit", &src_bit);
		if (ret)
			goto free_priv;

		ret = of_property_read_u32(son, "des_bit", &des_bit);
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
			irq = irq_of_parse_and_map(son, 0);
		} else if ((FAST_MBOX & func) && (SOURCE_MBOX & func)) {
			irq = (GIC_1 == src_bit) ? cm_gic_1_irq : cm_gic_2_irq;
		} else {
			/* maybe GIC_1 OR GIC_2 */
			irq = COMM_MBOX_IRQ;
			cmbox_info->cmdev = mdev;
		}

		rx_buffer = buf_pool + capability * RX_BUFFER_TYPE;
		ack_buffer = buf_pool + capability * ACK_BUFFER_TYPE;
		buf_pool = buf_pool + capability * MBOX_BUFFER_TYPE_MAX;

		priv->capability = capability;
		priv->func = func;
		priv->src_name = src_name;
		priv->des_name = des_name;
		priv->src = src_bit;
		priv->des = des_bit;
		priv->irq = irq;
		priv->index = index;
		priv->idev = idev;

		mdev->name = mdev_name;
		mdev->priv = priv;
		mdev->rx_buffer =  rx_buffer;
		mdev->ack_buffer = ack_buffer;
		mdev->ops = &hi3xxx_mdev_ops;

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
free_cmbox:
	kfree(cmbox_info);
free_rp:
	kfree(rp);
to_iounmap:
	iounmap(ipc_base);
out:
	return ret;
}

static int hi3xxx_mdev_probe(struct platform_device *pdev)
{
	struct hi3xxx_ipc_device *idev = NULL;
	struct hisi_mbox_device **mdev_res = NULL;
	struct device_node *node = NULL;
	int mdev_num = 0;
	int ret = 0;
	int timeout = 10;

	idev = kmalloc(sizeof(*idev), GFP_KERNEL);
	if (!idev) {
		MDEV_TRACE("no mem for ipc resouce\n");
		ret = -ENOMEM;
		goto out;
	}

	node = of_find_compatible_node(NULL, NULL, "hisilicon,HiIPCV230");
	if (!node) {
		MDEV_TRACE("dts[%s] node not found\n", "hisilicon,HiIPCV230");
		ret = -ENODEV;
		goto free_idev;
	}

	ret = of_property_read_u32(node, "mailboxes", &mdev_num);
	if (ret) {
		MDEV_TRACE("no mailboxes resources\n");
		ret = -ENODEV;
		goto free_idev;
	}

	mdev_res = kzalloc((sizeof(*mdev_res) * mdev_num), GFP_KERNEL);
	if (!mdev_res) {
		ret = -ENOMEM;
		goto free_idev;
	}

	ret = hi3xxx_mdev_get(idev, mdev_res, node);
	if (ret) {
		MDEV_TRACE("can not get ipc resource\n");
		ret = -ENODEV;
		goto free_mdevs;
	}

	ret = hisi_mbox_device_register(&pdev->dev, mdev_res);
	if (ret) {
		MDEV_TRACE("mdevs register failed\n");
		ret = -ENODEV;
		goto put_res;
	}

	while (IPC_UNLOCKED != __ipc_lock_status(idev->base)) {
		__ipc_unlock(idev->base, idev->unlock);
		if (!timeout) {
			MDEV_TRACE("ipc unlock timeout\n");
			ret = -ENODEV;
			goto unregister;
		}
		timeout--;
		msleep(1);
	}

	platform_set_drvdata(pdev, idev);

	pr_info("%s: HiIPCV230 mailboxes are ready\n", MODULE_NAME);
	return 0;

unregister:
	hisi_mbox_device_unregister();
put_res:
	hi3xxx_mdev_put(idev);
free_mdevs:
	kfree(idev->mdev_res);
free_idev:
	kfree(idev);
out:
	return ret;
}

static int hi3xxx_mdev_suspend(struct device *dev)
{
	struct platform_device *pdev =
		container_of(dev, struct platform_device, dev);
	struct hi3xxx_ipc_device *idev = platform_get_drvdata(pdev);

	pr_info("%s: suspend +\n", __func__);
	if (idev)
		hisi_mbox_device_deactivate(idev->mdev_res);
	pr_info("%s: suspend -\n", __func__);
	return 0;
}

static int hi3xxx_mdev_resume(struct device *dev)
{
	struct platform_device *pdev =
		container_of(dev, struct platform_device, dev);
	struct hi3xxx_ipc_device *idev = platform_get_drvdata(pdev);

	pr_info("%s: resume +\n", __func__);
	if (idev)
		hisi_mbox_device_activate(idev->mdev_res);
	pr_info("%s: resume -\n", __func__);
	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id hi3xxx_mdev_of_match[] = {
	{.compatible = "hisilicon,HiIPCV230",},
	{},
};
MODULE_DEVICE_TABLE(of, hi3xxx_mdev_of_match);
#endif

static const struct dev_pm_ops hi3xxx_mdev_pm_ops = {
	.suspend_late = hi3xxx_mdev_suspend,
	.resume_early = hi3xxx_mdev_resume,
};

static struct platform_driver hi3xxx_mdev_driver = {
	.probe = hi3xxx_mdev_probe,
	.remove = hi3xxx_mdev_remove,
	.driver = {
		.name = "HiIPCV230-mailbox",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(hi3xxx_mdev_of_match),
		.pm = &hi3xxx_mdev_pm_ops,
	},
};

static int __init hi3xxx_mdev_init(void)
{
	pr_debug("%s: init\n", MODULE_NAME);

	platform_driver_register(&hi3xxx_mdev_driver);
	return 0;
}
core_initcall(hi3xxx_mdev_init);

static void __exit hi3xxx_mdev_exit(void)
{
	platform_driver_unregister(&hi3xxx_mdev_driver);
	return;
}
module_exit(hi3xxx_mdev_exit);

MODULE_DESCRIPTION("hi3xxx mailbox device driver");
MODULE_LICENSE("GPL V2");
