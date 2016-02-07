/*
 * Synopsys DesignWare I2C adapter driver (master only).
 *
 * Based on the TI DAVINCI I2C adapter driver.
 *
 * Copyright (C) 2006 Texas Instruments.
 * Copyright (C) 2007 MontaVista Software Inc.
 * Copyright (C) 2009 Provigent Ltd.
 *
 * ----------------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * ----------------------------------------------------------------------------
 *
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/clk.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_i2c.h>
#include <linux/of_address.h>
#include <linux/slab.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/scatterlist.h>
#include <linux/pinctrl/consumer.h>

/*lint -emacro(750,*)*/
#define ENABLE_DELAY_SDA		1
#define DISABLE_DELAY_SDA		0

/*w00221568: sysctrl base addr*/
static void __iomem *sctrl_base;

/*add ChipVerification debug*/
#include <huawei_platform/chipverification/chipverification.h>

#undef CONFIG_AUDIENCE
#ifdef CONFIG_AUDIENCE
#include <linux/switch.h>
#endif   /* CONFIG_AUDIENCE */

/* this value must not a valid slave address */
#define I2C_CONFIGED_SLAVE	0x23

/*
 * Registers offset
 */

#define DW_IC_CON		0x0
#define DW_IC_TAR		0x4
#define DW_IC_SAR		0x8
#define DW_IC_DATA_CMD		0x10
#define DW_IC_SS_SCL_HCNT	0x14
#define DW_IC_SS_SCL_LCNT	0x18
#define DW_IC_FS_SCL_HCNT	0x1c
#define DW_IC_FS_SCL_LCNT	0x20
#define DW_IC_INTR_STAT		0x2c
#define DW_IC_INTR_MASK		0x30
#define DW_IC_RAW_INTR_STAT	0x34
#define DW_IC_RX_TL		0x38
#define DW_IC_TX_TL		0x3c
#define DW_IC_CLR_INTR		0x40
#define DW_IC_CLR_RX_UNDER	0x44
#define DW_IC_CLR_RX_OVER	0x48
#define DW_IC_CLR_TX_OVER	0x4c
#define DW_IC_CLR_RD_REQ	0x50
#define DW_IC_CLR_TX_ABRT	0x54
#define DW_IC_CLR_RX_DONE	0x58
#define DW_IC_CLR_ACTIVITY	0x5c
#define DW_IC_CLR_STOP_DET	0x60
#define DW_IC_CLR_START_DET	0x64
#define DW_IC_CLR_GEN_CALL	0x68
#define DW_IC_ENABLE		0x6c
#define DW_IC_STATUS		0x70
#define DW_IC_TXFLR		0x74
#define DW_IC_RXFLR		0x78
#define DW_IC_SDA_HOLD		0x7c
#define DW_IC_COMP_PARAM_1	0xf4
#define DW_IC_TX_ABRT_SOURCE	0x80
#define DW_IC_DMA_CR	0x88
#define DW_IC_DMA_TDLR	0x8c
#define DW_IC_DMA_RDLR	0x90
#define IC_ENABLE_STATUS	0x9c

#define DW_IC_CON_MASTER		0x1
#define DW_IC_CON_SPEED_STD		0x2
#define DW_IC_CON_SPEED_FAST		0x4
#define DW_IC_CON_10BITADDR_MASTER	0x10
#define DW_IC_CON_RESTART_EN		0x20
#define DW_IC_CON_SLAVE_DISABLE		0x40

#define DW_IC_INTR_RX_UNDER	0x001
#define DW_IC_INTR_RX_OVER	0x002
#define DW_IC_INTR_RX_FULL	0x004
#define DW_IC_INTR_TX_OVER	0x008
#define DW_IC_INTR_TX_EMPTY	0x010
#define DW_IC_INTR_RD_REQ	0x020
#define DW_IC_INTR_TX_ABRT	0x040
#define DW_IC_INTR_RX_DONE	0x080
#define DW_IC_INTR_ACTIVITY	0x100
#define DW_IC_INTR_STOP_DET	0x200
#define DW_IC_INTR_START_DET	0x400
#define DW_IC_INTR_GEN_CALL	0x800

#define DW_IC_TXDMAE		(1 << 1)	/* enable transmit dma */
#define DW_IC_RXDMAE		(1 << 0)	/* enable receive dma */

#define DW_IC_INTR_DEFAULT_MASK		(DW_IC_INTR_RX_FULL | \
					 DW_IC_INTR_TX_EMPTY | \
					 DW_IC_INTR_TX_ABRT | \
					 DW_IC_INTR_STOP_DET)

#define DW_IC_STATUS_ACTIVITY	0x1

#define DW_IC_ERR_TX_ABRT	0x1

#define DW_IC_IC_EN		0x1

/*
 * status codes
 */
#define STATUS_IDLE			0x0
#define STATUS_WRITE_IN_PROGRESS	0x1
#define STATUS_READ_IN_PROGRESS		0x2

#define TIMEOUT			20 /* ms */
#define RETRY_TIMES			2  	/* retry times for i2c-core.c*/

#define WAIT_FOR_COMPLETION		((HZ*totallen)/400+HZ)   /* wait 1s add 2.5ms every byte. */

#define I2C_POLL_DISABLE		40   	/* this value must be 10 times the signaling
										* period for the highest I2C transfer speed, if the
										* highest I2C transfer mode is 400 kb/s, then this
										* ti2c_poll is 25us. */
#define MAX_T_POLL_COUNT		100

#define SDA_HOLD_300NS			300

/*
 * hardware abort codes from the DW_IC_TX_ABRT_SOURCE register
 *
 * only expected abort codes are listed here
 * refer to the datasheet for the full list
 */
#define ABRT_7B_ADDR_NOACK	0
#define ABRT_10ADDR1_NOACK	1
#define ABRT_10ADDR2_NOACK	2
#define ABRT_TXDATA_NOACK	3
#define ABRT_GCALL_NOACK	4
#define ABRT_GCALL_READ		5
#define ABRT_SBYTE_ACKDET	7
#define ABRT_SBYTE_NORSTRT	9
#define ABRT_10B_RD_NORSTRT	10
#define ABRT_MASTER_DIS		11
#define ARB_LOST		12

#define DW_IC_TX_ABRT_7B_ADDR_NOACK	(1UL << ABRT_7B_ADDR_NOACK)
#define DW_IC_TX_ABRT_10ADDR1_NOACK	(1UL << ABRT_10ADDR1_NOACK)
#define DW_IC_TX_ABRT_10ADDR2_NOACK	(1UL << ABRT_10ADDR2_NOACK)
#define DW_IC_TX_ABRT_TXDATA_NOACK	(1UL << ABRT_TXDATA_NOACK)
#define DW_IC_TX_ABRT_GCALL_NOACK	(1UL << ABRT_GCALL_NOACK)
#define DW_IC_TX_ABRT_GCALL_READ	(1UL << ABRT_GCALL_READ)
#define DW_IC_TX_ABRT_SBYTE_ACKDET	(1UL << ABRT_SBYTE_ACKDET)
#define DW_IC_TX_ABRT_SBYTE_NORSTRT	(1UL << ABRT_SBYTE_NORSTRT)
#define DW_IC_TX_ABRT_10B_RD_NORSTRT	(1UL << ABRT_10B_RD_NORSTRT)
#define DW_IC_TX_ABRT_MASTER_DIS	(1UL << ABRT_MASTER_DIS)
#define DW_IC_TX_ARB_LOST		(1UL << ARB_LOST)
/*lint +emacro(750,*)*/
#define DW_IC_TX_ABRT_NOACK		(DW_IC_TX_ABRT_7B_ADDR_NOACK | \
					 DW_IC_TX_ABRT_10ADDR1_NOACK | \
					 DW_IC_TX_ABRT_10ADDR2_NOACK | \
					 DW_IC_TX_ABRT_TXDATA_NOACK | \
					 DW_IC_TX_ABRT_GCALL_NOACK)

#define I2C_DW_MAX_DMA_BUF_LEN          (60*1024)


static char *abort_sources[] = {
	[ABRT_7B_ADDR_NOACK] =
		"slave address not acknowledged (7bit mode)",
	[ABRT_10ADDR1_NOACK] =
		"first address byte not acknowledged (10bit mode)",
	[ABRT_10ADDR2_NOACK] =
		"second address byte not acknowledged (10bit mode)",
	[ABRT_TXDATA_NOACK] =
		"data not acknowledged",
	[ABRT_GCALL_NOACK] =
		"no acknowledgement for a general call",
	[ABRT_GCALL_READ] =
		"read after general call",
	[ABRT_SBYTE_ACKDET] =
		"start byte acknowledged",
	[ABRT_SBYTE_NORSTRT] =
		"trying to send start byte when restart is disabled",
	[ABRT_10B_RD_NORSTRT] =
		"trying to read when restart is disabled (10bit mode)",
	[ABRT_MASTER_DIS] =
		"trying to use disabled adapter",
	[ARB_LOST] =
		"lost arbitration",
};

struct dw_i2c_dma_data {
	struct dma_chan		*chan;
	struct scatterlist	sg;
	u8			*buf;
};


/**
 * struct dw_i2c_dev - private i2c-designware data
 * @dev: driver model device node
 * @base: IO registers pointer
 * @mapbase: register base address
 * @cmd_complete: tx completion indicator
 * @lock: protect this struct and IO registers
 * @clk: input reference clock
 * @cmd_err: run time hadware error code
 * @msgs: points to an array of messages currently being transferred
 * @msgs_num: the number of elements in msgs
 * @msg_write_idx: the element index of the current tx message in the msgs
 *	array
 * @tx_buf_len: the length of the current tx buffer
 * @tx_buf: the current tx buffer
 * @msg_read_idx: the element index of the current rx message in the msgs
 *	array
 * @rx_buf_len: the length of the current rx buffer
 * @rx_buf: the current rx buffer
 * @msg_err: error status of the current transfer
 * @status: i2c master status, one of STATUS_*
 * @abort_source: copy of the TX_ABRT_SOURCE register
 * @irq: interrupt number for the i2c master
 * @adapter: i2c subsystem adapter node
 * @tx_fifo_depth: depth of the hardware tx fifo
 * @rx_fifo_depth: depth of the hardware rx fifo
 */
struct dw_i2c_dev {
	struct device		*dev;
	void __iomem		*base;
	resource_size_t         mapbase;
	struct completion	cmd_complete;
	struct mutex		lock;
	struct clk		*clk;
	int			cmd_err;
	struct i2c_msg		*msgs;
	int			msgs_num;
	int			msg_write_idx;
	u32			tx_buf_len;
	u8			*tx_buf;
	int			msg_read_idx;
	u32			rx_buf_len;
	u8			*rx_buf;
	int			msg_err;
	unsigned int		status;
	u32			abort_source;
	int			irq;
	volatile int		irq_is_run;
	struct i2c_adapter	adapter;
	unsigned int		tx_fifo_depth;
	unsigned int		rx_fifo_depth;
	/* DMA stuff */
	u32			dmacr;
	bool			using_tx_dma;
	bool			using_rx_dma;
	struct dw_i2c_dma_data  dmarx;
	struct dw_i2c_dma_data  dmatx;
	int  			timeout_count;
	struct completion	dma_complete;
	bool			using_dma;
#ifdef CONFIG_AUDIENCE
	/* FOR ES305*/
	struct switch_dev 	*s_dev;
	int			es305_state;
	struct timeval		old_tv;
	struct timeval		current_tv;
	int			timeout_flag_for_es305;
#endif   /* CONFIG_AUDIENCE */

	/* User defined special data */
	struct device		*platform_dev;
	struct pinctrl		*pinctrl;
	int	pinctrl_flag;
	u32 reset_enable_off;
	u32 reset_disable_off;
	u32 reset_status_off;
	u32 reset_bit;

	//struct pm_qos_request_list pm_qos_req;
};

#ifdef CONFIG_AUDIENCE
#define RESET_ES305		"reset_es305"
#endif   /* CONFIG_AUDIENCE */

/*
 * All the DMA operation mode stuff goes inside this ifdef.
 * This assumes that you have a generic DMA device interface,
 * no custom DMA interfaces are supported.
 */
//#ifdef CONFIG_DMA_ENGINE
#if 0   //clb
static int i2c_dw_dma_sg_init(struct dw_i2c_dev *dev,
				struct dw_i2c_dma_data *dma_data,
				unsigned long length);
static int i2c_dw_xfer_msg_dma(struct dw_i2c_dev *dev, int *alllen);
static int i2c_dw_dma_tx_refill(struct dw_i2c_dev *dev);
static void i2c_dw_dma_rx_callback(void *data);


static void i2c_dw_dma_probe_initcall(struct dw_i2c_dev *dev)
{
	struct dma_chan *chan;
	dma_cap_mask_t mask;

	/* We need platform data */
	if (!dev || !dev->dma_filter || !dev->dma_tx_param) {
		dev_info(dev->dev, "no DMA platform data\n");
		return;
	}

	/* DMA is the sole user of the platform data right now */
	struct dma_slave_config tx_conf = {
		.dst_addr = dev->mapbase + DW_IC_DATA_CMD,
		.dst_addr_width = DMA_SLAVE_BUSWIDTH_2_BYTES,
		.direction = DMA_TO_DEVICE,
		.dst_maxburst = 16,
	};

	struct dma_slave_config rx_conf = {
		.src_addr = dev->mapbase + DW_IC_DATA_CMD,
		.src_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE,
		.direction = DMA_FROM_DEVICE,
		.src_maxburst = 16,
	};


	/* Try to acquire a generic DMA engine slave TX channel */
	dma_cap_zero(mask);
	dma_cap_set(DMA_SLAVE, mask);

	chan = dma_request_channel(mask, dev->dma_filter, dev->dma_tx_param);
	if (!chan) {
		dev_err(dev->dev, "no TX DMA channel!\n");
		return;
	}

	dmaengine_slave_config(chan, &tx_conf);
	dev->dmatx.chan = chan;

	dev_info(dev->dev, "DMA channel TX %s-%d\n",
		 dma_chan_name(dev->dmatx.chan),
		 dev->dmatx.chan->chan_id);

	chan = dma_request_channel(mask, dev->dma_filter, dev->dma_rx_param);
	if (!chan) {
		dev_err(dev->dev, "no RX DMA channel!\n");
		return;
	}

	dmaengine_slave_config(chan, &rx_conf);
	dev->dmarx.chan = chan;

	dev_info(dev->dev, "DMA channel RX %s-%d\n",
		 dma_chan_name(dev->dmarx.chan),
		 dev->dmarx.chan->chan_id);
}

#ifndef MODULE
/*
 * Stack up the i2c and let the above initcall be done at device
 * initcall time, because the i2c driver is called as an arch
 * initcall, and at this time the DMA subsystem is not yet registered.
 * At this point the driver will switch over to using DMA where desired.
 */
struct dma_i2c {
	struct list_head node;
	struct dw_i2c_dev *dev;
};

static LIST_HEAD(designware_dma_i2cs);

static int __init i2c_dw_dma_initcall(void)
{
	struct list_head *node, *tmp;

	list_for_each_safe(node, tmp, &designware_dma_i2cs) {
		struct dma_i2c *dmau = list_entry(node, struct dma_i2c, node);
		i2c_dw_dma_probe_initcall(dmau->dev);
		list_del(node);
		kfree(dmau);
	}
	return 0;
}

device_initcall(i2c_dw_dma_initcall);

static void i2c_dw_dma_probe(struct dw_i2c_dev *dev)
{
	struct dma_i2c *dmau = kzalloc(sizeof(struct dma_i2c), GFP_KERNEL);

	dev_info(dev->dev, "i2c_dw_dma_probe\n");

	if (dmau) {
		dmau->dev = dev;
		list_add_tail(&dmau->node, &designware_dma_i2cs);
	}
}
#else
static void i2c_dw_dma_probe(struct dw_i2c_dev *dev)
{
	i2c_dw_dma_probe_initcall(dev);
}
#endif

static void i2c_dw_dma_remove(struct dw_i2c_dev *dev)
{
	/* TODO: remove the initcall if it has not yet executed */
	if (dev->dmatx.chan)
		dma_release_channel(dev->dmatx.chan);
	if (dev->dmarx.chan)
		dma_release_channel(dev->dmarx.chan);
}



/*
 * The current DMA TX buffer has been sent.
 * Try to queue up another DMA buffer.
 */
static void i2c_dw_dma_tx_callback(void *data)
{
	struct dw_i2c_dev *dev = data;
	dev_dbg(dev->dev, "%s: entry.\n", __func__);

	dev->dmacr &= ~DW_IC_TXDMAE;
	writew(dev->dmacr, dev->base + DW_IC_DMA_CR);
	dev->using_tx_dma = false;
	if(!(dev->using_tx_dma) && !(dev->using_rx_dma))
		complete(&dev->dma_complete);
}

/*
 * Try to refill the TX DMA buffer.
 * Returns:
 *   1 if we queued up a TX DMA buffer.
 *   0 if we didn't want to handle this by DMA
 *  <0 on error
 */
static int i2c_dw_dma_tx_refill(struct dw_i2c_dev *dev)
{
	struct dw_i2c_dma_data *dmatx = &dev->dmatx;
	struct dma_chan *chan = dmatx->chan;
	struct dma_device *dma_dev = chan->device;
	struct dma_async_tx_descriptor *desc;

	if (!dev->dmatx.chan)
		return -EIO;

	if (dma_map_sg(dma_dev->dev, &dmatx->sg, 1, DMA_TO_DEVICE) != 1) {
		dev_warn(dev->dev, "unable to map TX DMA\n");
		return -EBUSY;
	}

	desc = dma_dev->device_prep_slave_sg(chan, &dmatx->sg, 1, DMA_TO_DEVICE,
					     DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
	if (!desc) {
		dma_unmap_sg(dma_dev->dev, &dmatx->sg, 1, DMA_TO_DEVICE);
		dev_warn(dev->dev, "TX DMA busy\n");
		return -EBUSY;
	}

	/* Some data to go along to the callback */
	desc->callback = i2c_dw_dma_tx_callback;
	desc->callback_param = dev;

	desc->tx_submit(desc);

	/* Fire the DMA transaction */
	dma_dev->device_issue_pending(chan);

	return 1;
}


/*
 * Returns:
 *	1 if we queued up a RX DMA buffer.
 *	0 if we didn't want to handle this by DMA
 */
static int i2c_dw_dma_rx_trigger_dma(struct dw_i2c_dev *dev)
{
	struct dw_i2c_dma_data *dmarx = &dev->dmarx;
	struct dma_chan *rxchan = dmarx->chan;
	struct dma_device *dma_dev = NULL;
	struct dma_async_tx_descriptor *desc;
	if (!rxchan) {
		dev_err(dev->dev, "rxchan is NULL!\n");
		return -EIO;
	}
	dma_dev =  rxchan->device;

	dev_dbg(dev->dev, "i2c_dw_dma_rx_trigger_dma, %d bytes to read\n",
		dev->dmarx.sg.length);

	if (dma_map_sg(dma_dev->dev, &dmarx->sg, 1, DMA_FROM_DEVICE) != 1) {
		dev_warn(dev->dev, "unable to map TX DMA\n");
		return -EBUSY;
	}

	desc = dma_dev->device_prep_slave_sg(rxchan, &dmarx->sg, 1,
			DMA_FROM_DEVICE, DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
	if (!desc) {
		dma_unmap_sg(dma_dev->dev, &dmarx->sg, 1, DMA_FROM_DEVICE);
		dev_warn(dev->dev, "RX DMA busy\n");
		return -EBUSY;
	}

	/* Some data to go along to the callback */
	desc->callback = i2c_dw_dma_rx_callback;
	desc->callback_param = dev;

	/* All errors should happen at prepare time */
	desc->tx_submit(desc);

	/* Fire the DMA transaction */
	dma_dev->device_issue_pending(rxchan);

	return 1;
}


static void i2c_dw_dma_rx_callback(void *data)
{
	struct dw_i2c_dev *dev = data;
	struct i2c_msg *msgs = dev->msgs;
	struct dw_i2c_dma_data *dmarx = &dev->dmarx;
	int rx_valid = dmarx->sg.length;
	int rd_idx = 0;
	u32 len;
	u8 *buf;

	dev_dbg(dev->dev, "%s: entry.\n", __func__);

	/* Sync in buffer */
	dma_sync_sg_for_cpu(dev->dev, &dmarx->sg, 1, DMA_FROM_DEVICE);
	dev->dmacr &= ~DW_IC_RXDMAE;
	writew(dev->dmacr, dev->base + DW_IC_DMA_CR);


	for (; dev->msg_read_idx < dev->msgs_num; dev->msg_read_idx++) {
		if (!(msgs[dev->msg_read_idx].flags & I2C_M_RD))
			continue;

		len = msgs[dev->msg_read_idx].len;
		buf = msgs[dev->msg_read_idx].buf;

		for (; len > 0 && rx_valid > 0; len--, rx_valid--)
			*buf++ = dmarx->buf[rd_idx++];
	}
	dev->using_rx_dma = false;
	if(!(dev->using_tx_dma) && !(dev->using_rx_dma))
		complete(&dev->dma_complete);
}

static int i2c_dw_dma_sg_init(struct dw_i2c_dev *dev,
				struct dw_i2c_dma_data *dma_data,
				unsigned long length)
{
	dma_data->buf = kmalloc(length, GFP_KERNEL);
	if (!dma_data->buf) {
		dev_err(dev->dev, "%s: no memory for DMA buffer, length: %lu\n",
			__func__, length);
		return -ENOMEM;
	}

	sg_init_one(&dma_data->sg, dma_data->buf, length);

	return 0;
}

static void i2c_dw_dma_clear(struct dw_i2c_dev *dev)
{
	if(dev->dmatx.buf) {
		dev->dmatx.chan->device->device_control(dev->dmatx.chan,
			DMA_TERMINATE_ALL, 0);

		dma_unmap_sg(dev->dmatx.chan->device->dev,
			&dev->dmatx.sg, 1, DMA_TO_DEVICE);

		kfree(dev->dmatx.buf);
		dev->dmatx.buf = NULL;
		dev->using_tx_dma = false;
	}

	if(dev->dmarx.buf) {
		dev->dmarx.chan->device->device_control(dev->dmarx.chan,
			DMA_TERMINATE_ALL, 0);
		dma_unmap_sg(dev->dmarx.chan->device->dev,
			&dev->dmarx.sg, 1, DMA_FROM_DEVICE);

		kfree(dev->dmarx.buf);
		dev->dmarx.buf = NULL;
		dev->using_rx_dma = false;
	}

	dev->using_dma = false;
	dev->dmacr = 0;
	writew(dev->dmacr, dev->base + DW_IC_DMA_CR);
}

#else
/* Blank functions if the DMA engine is not available */
static inline void i2c_dw_dma_probe(struct dw_i2c_dev *dev)
{
}

static inline void i2c_dw_dma_remove(struct dw_i2c_dev *dev)
{
}

static int i2c_dw_dma_tx_refill(struct dw_i2c_dev *dev)
{
	return -EIO;
}

static int i2c_dw_dma_rx_trigger_dma(struct dw_i2c_dev *dev)
{
	return -EIO;
}

static int i2c_dw_dma_sg_init(struct dw_i2c_dev *dev,
				struct dw_i2c_dma_data *dma_data,
				unsigned long length)
{
	return -EIO;
}

static void i2c_dw_dma_clear(struct dw_i2c_dev *dev)
{
}

#endif

/*I2C clock domain enable*/
static void i2c_clk_domain_enable(struct dw_i2c_dev *dev, unsigned int enable)
{
	unsigned int ret;
	int timeout = 10;

	if (enable)
		writel(BIT(dev->reset_bit), sctrl_base + dev->reset_enable_off);
	else
		writel(BIT(dev->reset_bit), sctrl_base + dev->reset_disable_off);

	do {
		ret = readl(sctrl_base + dev->reset_status_off);
		ret &= BIT(dev->reset_bit);
		udelay(1);
	}while(!ret && timeout--);
}

static u32
i2c_dw_scl_hcnt(u32 ic_clk, u32 tSYMBOL, u32 tf, int cond, int offset)
{
	/*
	 * DesignWare I2C core doesn't seem to have solid strategy to meet
	 * the tHD;STA timing spec.  Configuring _HCNT based on tHIGH spec
	 * will result in violation of the tHD;STA spec.
	 */
	if (cond)
		/*
		 * Conditional expression:
		 *
		 *   IC_[FS]S_SCL_HCNT + (1+4+3) >= IC_CLK * tHIGH
		 *
		 * This is based on the DW manuals, and represents an ideal
		 * configuration.  The resulting I2C bus speed will be
		 * faster than any of the others.
		 *
		 * If your hardware is free from tHD;STA issue, try this one.
		 */
		return (ic_clk * tSYMBOL + 5000) / 10000 - 8 + offset;
	else
		/*
		 * Conditional expression:
		 *
		 *   IC_[FS]S_SCL_HCNT + 3 >= IC_CLK * (tHD;STA + tf)
		 *
		 * This is just experimental rule; the tHD;STA period turned
		 * out to be proportinal to (_HCNT + 3).  With this setting,
		 * we could meet both tHIGH and tHD;STA timing specs.
		 *
		 * If unsure, you'd better to take this alternative.
		 *
		 * The reason why we need to take into account "tf" here,
		 * is the same as described in i2c_dw_scl_lcnt().
		 */
		return (ic_clk * (tSYMBOL + tf) + 5000) / 10000 - 3 + offset;
}

static u32 i2c_dw_scl_lcnt(u32 ic_clk, u32 tLOW, u32 tf, int offset)
{
	/*
	 * Conditional expression:
	 *
	 *   IC_[FS]S_SCL_LCNT + 1 >= IC_CLK * (tLOW + tf)
	 *
	 * DW I2C core starts counting the SCL CNTs for the LOW period
	 * of the SCL clock (tLOW) as soon as it pulls the SCL line.
	 * In order to meet the tLOW timing spec, we need to take into
	 * account the fall time of SCL signal (tf).  Default tf value
	 * should be 0.3 us, for safety.
	 */
	return ((ic_clk * (tLOW + tf) + 5000) / 10000) - 1 + offset;
}

/**
 * i2c_dw_init() - initialize the designware i2c master hardware
 * @dev: device private data
 *
 * This functions configures and enables the I2C master.
 * This function is called during I2C init function, and in case of timeout at
 * run time.
 */
static void i2c_dw_init(struct dw_i2c_dev *dev)
{
	u32 input_clock_khz = clk_get_rate(dev->clk) / 1000;
	u32 ic_con, hcnt, lcnt;

	/* Disable the adapter */
	writel(0, dev->base + DW_IC_ENABLE);

	/* set standard and fast speed deviders for high/low periods */

	/* Standard-mode */
	hcnt = i2c_dw_scl_hcnt(input_clock_khz,
				40,	/* tHD;STA = tHIGH = 4.0 us */
				3,	/* tf = 0.3 us */
				0,	/* 0: DW default, 1: Ideal */
				0);	/* No offset */
	lcnt = i2c_dw_scl_lcnt(input_clock_khz,
				47,	/* tLOW = 4.7 us */
				3,	/* tf = 0.3 us */
				0);	/* No offset */
	writel(hcnt, dev->base + DW_IC_SS_SCL_HCNT);
	writel(lcnt, dev->base + DW_IC_SS_SCL_LCNT);
	dev_dbg(dev->dev, "Standard-mode HCNT:LCNT = %d:%d\n", hcnt, lcnt);

	/* Fast-mode */
	hcnt = i2c_dw_scl_hcnt(input_clock_khz,
				7,	/* tHD;STA = tHIGH = 0.6 us */
				3,	/* tf = 0.3 us */
				0,	/* 0: DW default, 1: Ideal */
				0);	/* No offset */
	lcnt = i2c_dw_scl_lcnt(input_clock_khz,
				14,	/* tLOW = 1.3 us */
				3,	/* tf = 0.3 us */
				0);	/* No offset */
	writel(hcnt, dev->base + DW_IC_FS_SCL_HCNT);
	writel(lcnt, dev->base + DW_IC_FS_SCL_LCNT);
	dev_dbg(dev->dev, "Fast-mode HCNT:LCNT = %d:%d\n", hcnt, lcnt);

	writel(I2C_CONFIGED_SLAVE, dev->base + DW_IC_SAR);

	/* Configure Tx/Rx FIFO threshold levels */
	writel(dev->tx_fifo_depth - 1, dev->base + DW_IC_TX_TL);
	writel(16, dev->base + DW_IC_RX_TL);

	writel(dev->tx_fifo_depth - 16, dev->base + DW_IC_DMA_TDLR);
	writel(15, dev->base + DW_IC_DMA_RDLR);

	/* configure the i2c master */
	ic_con = DW_IC_CON_MASTER | DW_IC_CON_SLAVE_DISABLE |
		DW_IC_CON_RESTART_EN | DW_IC_CON_SPEED_FAST;
	writel(ic_con, dev->base + DW_IC_CON);

	writel(input_clock_khz*SDA_HOLD_300NS/1000000, dev->base + DW_IC_SDA_HOLD);

	writel(0, dev->base + DW_IC_INTR_MASK); /* disable IRQ */
}

/* print reg*/
static void i2c_print_controller_reg(struct dw_i2c_dev *dev)
{
	u32 i;
	dev_err(dev->dev, "I2C DUMP REGISTER\n");
	for(i = 0; i <= DW_IC_TX_ABRT_SOURCE; i += 0x10)
	{
		/* can't dump  DW_IC_DATA_CMD */
		if(i != DW_IC_DATA_CMD){
			printk(KERN_ERR "%08x: %08x, %08x, %08x, %08x\n", (unsigned long)(dev->base + i), readl(dev->base + i),
				readl(dev->base + i + 4), readl(dev->base + i + 8), readl(dev->base + i + 12));
		}else{
			printk(KERN_ERR "%08x: ........, %08x, %08x, %08x\n", (unsigned long)(dev->base + i),
				readl(dev->base + i + 4), readl(dev->base + i + 8), readl(dev->base + i + 12));
		}
	}
}
static void i2c_print_controller_reg_test(struct dw_i2c_dev *dev)
{
	u32 i;
	dev_err(dev->dev, "I2C DUMP REGISTER\n");
	for(i = 0; i <= 0x3C; i += 0x10)
	{
		/* can't dump  DW_IC_DATA_CMD */
		if(i != DW_IC_DATA_CMD){
			printk(KERN_ERR "%08x: %08x, %08x, %08x, %08x\n", (unsigned long)(dev->base + i), readl(dev->base + i),
				readl(dev->base + i + 4), readl(dev->base + i + 8), readl(dev->base + i + 12));
		}else{
			printk(KERN_ERR "%08x: ........, %08x, %08x, %08x\n", (unsigned long)(dev->base + i),
				readl(dev->base + i + 4), readl(dev->base + i + 8), readl(dev->base + i + 12));
		}
	}
	for(i = 0x6C; i <= 0xA4; i += 0x10)
	{
		/* can't dump  DW_IC_DATA_CMD */
		if(i != DW_IC_DATA_CMD){
			printk(KERN_ERR "%08x: %08x, %08x, %08x, %08x\n", (unsigned long)(dev->base + i), readl(dev->base + i),
				readl(dev->base + i + 4), readl(dev->base + i + 8), readl(dev->base + i + 12));
		}else{
			printk(KERN_ERR "%08x: ........, %08x, %08x, %08x\n", (unsigned long)(dev->base + i),
				readl(dev->base + i + 4), readl(dev->base + i + 8), readl(dev->base + i + 12));
		}
	}
}
/* print reg*/
static void reset_i2c_controller(struct dw_i2c_dev *dev)
{
//	i2c_clk_domain_enable(dev, 1);

	//i2c_clk_domain_enable(dev, 0);

	i2c_dw_init(dev);
}

/*
 * Waiting for bus not busy
 */
static int i2c_dw_wait_bus_not_busy(struct dw_i2c_dev *dev)
{
	int timeout = TIMEOUT;

	while (readl(dev->base + DW_IC_STATUS) & DW_IC_STATUS_ACTIVITY) {
		if (timeout <= 0) {
			dev_warn(dev->dev, "timeout waiting for bus ready, "
			         "reset controler!\n");
			i2c_print_controller_reg(dev);
			reset_i2c_controller(dev);
			if(readl(dev->base + DW_IC_STATUS) &
			   DW_IC_STATUS_ACTIVITY) {
				dev_err(dev->dev, "controller failed after reset.\n");
				return -EAGAIN;
			} else {
				return 0;
			}
		}
		timeout--;
		msleep(1);
	}

	return 0;
}

static void i2c_dw_xfer_init(struct dw_i2c_dev *dev)
{
	struct i2c_msg *msgs = dev->msgs;
	u32 ic_con;
	int retry_num = MAX_T_POLL_COUNT;

	/* Disable the adapter */
	writel(0, dev->base + DW_IC_ENABLE);
	while((--retry_num != 0) && (readl(dev->base + IC_ENABLE_STATUS)&DW_IC_IC_EN)){
		udelay(I2C_POLL_DISABLE);
	}

	if(retry_num <= 0){
		dev_err(dev->dev, "disable i2c controller error.\n");
		i2c_print_controller_reg(dev);
		reset_i2c_controller(dev);
	}

	/* set the slave (target) address */
	writel(msgs[dev->msg_write_idx].addr, dev->base + DW_IC_TAR);

	/* if the slave address is ten bit address, enable 10BITADDR */
	ic_con = readl(dev->base + DW_IC_CON);

	/* Judge flags for ES305 */
	if((1 == dev->msgs_num) && (I2C_M_RD == msgs->flags))
		ic_con &= ~DW_IC_CON_RESTART_EN;
	else
		ic_con |= DW_IC_CON_RESTART_EN;
	if (msgs[dev->msg_write_idx].flags & I2C_M_TEN)
		ic_con |= DW_IC_CON_10BITADDR_MASTER;
	else
		ic_con &= ~DW_IC_CON_10BITADDR_MASTER;
	writel(ic_con, dev->base + DW_IC_CON);

	/* Enable the adapter */
	writel(1, dev->base + DW_IC_ENABLE);

	/* Clear interrupts */
	readl(dev->base + DW_IC_CLR_INTR);

	/* Enable interrupts */
	writel(DW_IC_INTR_TX_ABRT | DW_IC_INTR_STOP_DET, dev->base + DW_IC_INTR_MASK);
}

/*
 * Initiate (and continue) low level master read/write transaction.
 * This function is only called from i2c_dw_isr, and pumping i2c_msg
 * messages into the tx buffer.  Even if the size of i2c_msg data is
 * longer than the size of the tx buffer, it handles everything.
 */
static void
i2c_dw_xfer_msg(struct dw_i2c_dev *dev)
{
	struct i2c_msg *msgs = dev->msgs;
	u32 intr_mask;
    u32 final_len;
	int tx_limit, rx_limit;
	u32 buf_len = dev->tx_buf_len;
	u8 *buf = dev->tx_buf;

	intr_mask = DW_IC_INTR_DEFAULT_MASK;
    final_len = dev->msgs_num - 1;

    /*Using writel_relaxed,should make previous register writing sync to memory*/
    wmb();

	for (; dev->msg_write_idx < dev->msgs_num; dev->msg_write_idx++) {
		if (!(dev->status & STATUS_WRITE_IN_PROGRESS)) {
			/* new i2c_msg */
			buf = msgs[dev->msg_write_idx].buf;
			buf_len = msgs[dev->msg_write_idx].len;
		}

		tx_limit = dev->tx_fifo_depth - readl(dev->base + DW_IC_TXFLR);
		rx_limit = dev->rx_fifo_depth - readl(dev->base + DW_IC_RXFLR);

		while (buf_len > 0 && tx_limit > 0 && rx_limit > 0) {
			if (msgs[dev->msg_write_idx].flags & I2C_M_RD) {
                if((1 == buf_len) && (dev->msg_write_idx == final_len)) {
				    writel_relaxed(0x300, dev->base + DW_IC_DATA_CMD);
                }else{
				    writel_relaxed(0x100, dev->base + DW_IC_DATA_CMD);
                }
				rx_limit--;
			} else{
                if((1 == buf_len) && (dev->msg_write_idx == final_len)) {
                    writel_relaxed(0x200|*buf++, dev->base + DW_IC_DATA_CMD);
                }else{
				    writel_relaxed(*buf++, dev->base + DW_IC_DATA_CMD);
                }
			}
			tx_limit--; buf_len--;
		}

		dev->tx_buf = buf;
		dev->tx_buf_len = buf_len;

		if (buf_len > 0) {
			/* more bytes to be written */
			dev->status |= STATUS_WRITE_IN_PROGRESS;
			break;
		} else
			dev->status &= ~STATUS_WRITE_IN_PROGRESS;
	}

	/*
	 * If i2c_msg index search is completed, we don't need TX_EMPTY
	 * interrupt any more.
	 */
	if (dev->msg_write_idx == dev->msgs_num)
		intr_mask &= ~DW_IC_INTR_TX_EMPTY;

	if (dev->msg_err)
		intr_mask = 0;

	writel(intr_mask, dev->base + DW_IC_INTR_MASK);
}

static void
i2c_dw_read(struct dw_i2c_dev *dev)
{
	struct i2c_msg *msgs = dev->msgs;
	int rx_valid;

	for (; dev->msg_read_idx < dev->msgs_num; dev->msg_read_idx++) {
		u32 len;
		u8 *buf;

		if (!(msgs[dev->msg_read_idx].flags & I2C_M_RD))
			continue;

		if (!(dev->status & STATUS_READ_IN_PROGRESS)) {
			len = msgs[dev->msg_read_idx].len;
			buf = msgs[dev->msg_read_idx].buf;
		} else {
			len = dev->rx_buf_len;
			buf = dev->rx_buf;
		}

		rx_valid = readl(dev->base + DW_IC_RXFLR);

		for (; len > 0 && rx_valid > 0; len--, rx_valid--)
			*buf++ = readl(dev->base + DW_IC_DATA_CMD);

		if (len > 0) {
			dev->status |= STATUS_READ_IN_PROGRESS;
			dev->rx_buf_len = len;
			dev->rx_buf = buf;
			return;
		} else
			dev->status &= ~STATUS_READ_IN_PROGRESS;
	}
}

static int i2c_dw_handle_tx_abort(struct dw_i2c_dev *dev)
{
	unsigned long abort_source = dev->abort_source;
	int i;
/*lint -e666*/
	if (abort_source & DW_IC_TX_ABRT_NOACK) {
		for_each_set_bit(i, &abort_source, ARRAY_SIZE(abort_sources))
			dev_err(dev->dev,
				"%s: %s\n", __func__, abort_sources[i]);
		return -EREMOTEIO;
	}

	for_each_set_bit(i, &abort_source, ARRAY_SIZE(abort_sources))
		dev_err(dev->dev, "%s: %s\n", __func__, abort_sources[i]);
/*lint +e666*/
	if (abort_source & DW_IC_TX_ARB_LOST)
	{
		i2c_print_controller_reg_test(dev);
		return -EAGAIN;
	}
	else if (abort_source & DW_IC_TX_ABRT_GCALL_READ)
		return -EINVAL; /* wrong msgs[] data */
	else
		return -EIO;
}


static int i2c_dw_xfer_msg_dma(struct dw_i2c_dev *dev, int *alllen)
{
	struct i2c_msg *msgs = dev->msgs;
	int rx_len = 0;
	int tx_len;
	int i;
	int total_len = 0;
	u32 buf_len = dev->tx_buf_len;
	u8 *buf = dev->tx_buf;
	u16 *dma_txbuf;
	int ret = -EPERM;

	if (!dev->dmatx.chan || !dev->dmarx.chan)
		return -EPERM;

	/* If total date length less than a fifodepth, not use DMA */
	for (i = dev->msg_write_idx; i < dev->msgs_num; i++)
		total_len += dev->msgs[i].len;

	dev_dbg(dev->dev, "%s: msg num: %d, total length: %d\n",
		__func__, dev->msgs_num, total_len);

	*alllen = total_len;
	if (total_len < dev->tx_fifo_depth)
		return -EPERM;

	tx_len = total_len * sizeof(unsigned short);

	if (tx_len > I2C_DW_MAX_DMA_BUF_LEN) {
		dev_err(dev->dev, "Too long to send with DMA: %d\n", tx_len);
		dev->msg_err = -EINVAL;
		return -EPERM;
	}

	dev_dbg(dev->dev, "use DMA transfer, len=%d\n", tx_len);

	INIT_COMPLETION(dev->dma_complete);
	dev->using_dma = true;

	ret = i2c_dw_dma_sg_init(dev, &dev->dmatx, tx_len);
	if (ret < 0)
		return ret;

	dev->using_tx_dma = true;

	dma_txbuf = (u16 *)dev->dmatx.buf;

	for (; dev->msg_write_idx < dev->msgs_num; dev->msg_write_idx++) {

		buf = msgs[dev->msg_write_idx].buf;
		buf_len = msgs[dev->msg_write_idx].len;

		if (msgs[dev->msg_write_idx].flags & I2C_M_RD) {
			for (i = 0; i < buf_len; i++) {
				*dma_txbuf++ = 0x100;
				rx_len++;
			}
		} else {
			for (i = 0; i < buf_len; i++)
				*dma_txbuf++ = *buf++;
		}
	}

	dev_dbg(dev->dev, "%s: dev->dmatx.sg.length: %d, tx_len: %d\n",
		__func__, dev->dmatx.sg.length, tx_len);

	if (rx_len > 0) {
		ret = i2c_dw_dma_sg_init(dev, &dev->dmarx, rx_len);
		if (ret < 0)
			goto error;

		dev->using_rx_dma = true;

		if (i2c_dw_dma_rx_trigger_dma(dev) >= 0)
			dev->dmacr |= DW_IC_RXDMAE;
		else {
			dev_warn(dev->dev, "Dma rx failed.\n");
			goto error;
		}
	}

	if (i2c_dw_dma_tx_refill(dev) >= 0)
		dev->dmacr |= DW_IC_TXDMAE;
	else {
		dev_warn(dev->dev, "Dma tx failed.\n");
		goto error;
	}

	writew(dev->dmacr, dev->base + DW_IC_DMA_CR);

	ret = 0;
error:
	if (ret < 0) {
		dev->using_dma = false;
		/* Restore for CPU transfer */
		dev->msg_write_idx = 0;
		dev->msg_read_idx = 0;
		dev->status &= ~STATUS_WRITE_IN_PROGRESS;
		dev_err(dev->dev, "i2c_dw_xfer_msg_dma return erron %d.\n",
			ret);
	}
	return ret;
}

static int i2c_param_check(struct dw_i2c_dev *dev, struct i2c_msg msgs[], int num)
{
	u16 addr = msgs[0].addr;
	int i;

	for (i=0; i < num; i++) {
		/*
		 * if target address has changed, we need to
		 * reprogram the target address in the i2c
		 * adapter when we are done with this transfer
		 */
		if (msgs[i].addr != addr) {
			dev_err(dev->dev, "%s: invalid target address, first slave addr 0x%x, now addr 0x%x\n",
					__func__, addr, msgs[i].addr);
			return -EINVAL;
		}

		if (msgs[i].len == 0) {
			dev_err(dev->dev, "%s: invalid message length, slave addr 0x%x\n", __func__, addr);
			return -EINVAL;
		}

		if (msgs[i].buf == NULL) {
			dev_err(dev->dev, "%s: invalid message buf, slave addr 0x%x\n", __func__, addr);
			return -EINVAL;
		}
	}

	return 0;
}


#ifdef CONFIG_AUDIENCE
static void send_reset_signal(struct dw_i2c_dev *dev)
{
	dev->es305_state = !dev->es305_state;
	if(dev->s_dev)
		switch_set_state(dev->s_dev, dev->es305_state);
	dev_err(dev->dev, "switch_set_state\n");
}
#endif   /* CONFIG_AUDIENCE */

/*
 * Prepare controller for a transaction and call i2c_dw_xfer_msg
 */
static int
i2c_dw_xfer(struct i2c_adapter *adap, struct i2c_msg msgs[], int num)
{
	struct dw_i2c_dev *dev = i2c_get_adapdata(adap);

	int ret;
	int totallen = 0;

	dev_dbg(dev->dev, "%s: msgs: %d\n", __func__, num);

	ret = i2c_param_check(dev, msgs, num);
	if(ret < 0){
		return ret;
	}


	mutex_lock(&dev->lock);
	//i2c_clk_domain_enable(dev, 0);

	ret = clk_prepare_enable(dev->clk);
	if(ret < 0) {
		dev_err(dev->dev, "clk_enable failed!\n");
		goto clk_set_failed;
	}

	dev->msgs = msgs;
	dev->msgs_num = num;
	dev->cmd_err = 0;
	dev->msg_write_idx = 0;
	dev->msg_read_idx = 0;
	dev->msg_err = 0;
	dev->status = STATUS_IDLE;
	dev->abort_source = 0;
	dev->using_tx_dma = false;
	dev->using_rx_dma = false;
	dev->dmarx.buf = NULL;
	dev->dmatx.buf = NULL;
	dev->using_dma = false;

	ret = i2c_dw_wait_bus_not_busy(dev);
	if (ret < 0)
		goto done;

	/* start the transfers */
	i2c_dw_xfer_init(dev);

	if((0x4 != (readl(dev->base + DW_IC_STATUS) & 0xC))
		|| (readl(dev->base + DW_IC_TXFLR) & 0x3F)
		|| (readl(dev->base + DW_IC_RXFLR) & 0x3F)) {
		dev_err(dev->dev,  "rx or tx fifo is not zero.\n");
		i2c_print_controller_reg(dev);
		reset_i2c_controller(dev);
		ret = -EAGAIN;
		goto done;
	}

	INIT_COMPLETION(dev->cmd_complete);

	if (i2c_dw_xfer_msg_dma(dev, &totallen) < 0) {
		if (dev->msg_err < 0) {
			ret = dev->msg_err;
			goto done;
		}
		writel(DW_IC_INTR_DEFAULT_MASK, dev->base + DW_IC_INTR_MASK);
	}
	/* wait for tx to complete */
	ret = wait_for_completion_timeout(&dev->cmd_complete, WAIT_FOR_COMPLETION);
	if (ret == 0) {
		int retry_num = 20;
        dev_err(dev->dev, "controller timed out, transfer %d, len = %d, slave_addr = 0x%x,\n", WAIT_FOR_COMPLETION, totallen, msgs[0].addr );
        i2c_print_controller_reg(dev);
		do {
			writel(0, dev->base + DW_IC_INTR_MASK);
			msleep(5);
		}while((dev->irq_is_run != 0) && (retry_num-- != 0));
		if(dev->irq_is_run != 0){
			dev_err(dev->dev, "Isr is runing, can't reset I2C IP.\n");
		}else{
			reset_i2c_controller(dev);
		}
	#ifdef CONFIG_AUDIENCE
		struct platform_device *pdev = container_of(dev->dev, struct platform_device, dev);
		if(1 == pdev->id) {
			if(0 == dev->timeout_flag_for_es305) {
				dev->timeout_flag_for_es305 = 1;
				do_gettimeofday(&dev->old_tv);
			}
			else {
				do_gettimeofday(&dev->current_tv);
				if(dev->current_tv.tv_sec - dev->old_tv.tv_sec > 10 ) {
					send_reset_signal(dev);
					//msleep(1000);
					dev->timeout_flag_for_es305 = 0;
				}
			}
		}
	#endif   /* CONFIG_AUDIENCE */
		ret = -ETIMEDOUT;
		goto done;
	} else if (ret < 0)
		goto done;

	if((!dev->cmd_err) && (dev->using_dma)) {
		ret = wait_for_completion_timeout(&dev->dma_complete, WAIT_FOR_COMPLETION);
		if (ret == 0) {
			dev_err(dev->dev, "wait for dma complete timed out, transfer %d, len = %d, slave_addr = 0x%x, clk = \n", WAIT_FOR_COMPLETION, totallen, msgs[0].addr);
			i2c_print_controller_reg(dev);
			reset_i2c_controller(dev);
			ret = -ETIMEDOUT;
			goto done;
		}
	}

	if (dev->msg_err) {
		ret = dev->msg_err;
		goto done;
	}

	/* no error */
	if (likely(!dev->cmd_err)) {
		/* Disable the adapter */
		//writel(0, dev->base + DW_IC_ENABLE);
		if((dev->status & (STATUS_READ_IN_PROGRESS|STATUS_WRITE_IN_PROGRESS))
				|| (DW_IC_INTR_TX_EMPTY == (readl(dev->base + DW_IC_INTR_MASK)&DW_IC_INTR_TX_EMPTY))){
			dev_err(dev->dev, "write or read is not complete, status 0x%x, len = %d.\n", dev->status, totallen);
			i2c_print_controller_reg(dev);
			ret = -EAGAIN;
		}
		else{
			ret = num;
			dev->timeout_count = 0;
		#ifdef CONFIG_AUDIENCE
			dev->timeout_flag_for_es305 = 0;
		#endif   /* CONFIG_AUDIENCE */
		}
		goto done;
	}

	/* We have an error */
	if (dev->cmd_err == DW_IC_ERR_TX_ABRT) {
		ret = i2c_dw_handle_tx_abort(dev);
		goto done;
	}
	ret = -EIO;

done:
	i2c_dw_dma_clear(dev);

	if (-ETIMEDOUT == ret) {
		ret = -EAGAIN;
		if(++(dev->timeout_count) >= 2){
			dev->timeout_count = 0;
			/*enable i2c clk domain reset*/
			//i2c_clk_domain_enable(dev, 1);
			/*disable i2c clk domain reset*/
			//i2c_clk_domain_enable(dev, 0);
		}
	}

	if(false == Chip_Verification_Check_Debug(CHIP_VERIFICATION_I2C_DEBUG) )
	{
	clk_disable_unprepare(dev->clk);
	}
clk_set_failed:
	//i2c_clk_domain_enable(dev, 1);

	mutex_unlock(&dev->lock);

	if(ret < 0){
		dev_err(dev->dev, "error info %d, slave addr 0x%x.\n", ret, msgs[0].addr);
	}

	return ret;
}

static u32 i2c_dw_func(struct i2c_adapter *adap)
{
	return	I2C_FUNC_I2C |
		I2C_FUNC_10BIT_ADDR |
		I2C_FUNC_SMBUS_BYTE |
		I2C_FUNC_SMBUS_BYTE_DATA |
		I2C_FUNC_SMBUS_WORD_DATA |
		I2C_FUNC_SMBUS_I2C_BLOCK;
}

static u32 i2c_dw_read_clear_intrbits(struct dw_i2c_dev *dev)
{
	u32 stat;

	/*
	 * The IC_INTR_STAT register just indicates "enabled" interrupts.
	 * Ths unmasked raw version of interrupt status bits are available
	 * in the IC_RAW_INTR_STAT register.
	 *
	 * That is,
	 *   stat = readl(IC_INTR_STAT);
	 * equals to,
	 *   stat = readl(IC_RAW_INTR_STAT) & readl(IC_INTR_MASK);
	 *
	 * The raw version might be useful for debugging purposes.
	 */
	stat = readl(dev->base + DW_IC_INTR_STAT);

	/*
	 * Do not use the IC_CLR_INTR register to clear interrupts, or
	 * you'll miss some interrupts, triggered during the period from
	 * readl(IC_INTR_STAT) to readl(IC_CLR_INTR).
	 *
	 * Instead, use the separately-prepared IC_CLR_* registers.
	 */
	if (stat & DW_IC_INTR_RX_UNDER)
		readl(dev->base + DW_IC_CLR_RX_UNDER);
	if (stat & DW_IC_INTR_RX_OVER)
		readl(dev->base + DW_IC_CLR_RX_OVER);
	if (stat & DW_IC_INTR_TX_OVER)
		readl(dev->base + DW_IC_CLR_TX_OVER);
	if (stat & DW_IC_INTR_RD_REQ)
		readl(dev->base + DW_IC_CLR_RD_REQ);
	if (stat & DW_IC_INTR_TX_ABRT) {
		/*
		 * The IC_TX_ABRT_SOURCE register is cleared whenever
		 * the IC_CLR_TX_ABRT is read.  Preserve it beforehand.
		 */
		dev->abort_source = readl(dev->base + DW_IC_TX_ABRT_SOURCE);
		readl(dev->base + DW_IC_CLR_TX_ABRT);
	}
	if (stat & DW_IC_INTR_RX_DONE)
		readl(dev->base + DW_IC_CLR_RX_DONE);
	if (stat & DW_IC_INTR_ACTIVITY)
		readl(dev->base + DW_IC_CLR_ACTIVITY);
	if (stat & DW_IC_INTR_STOP_DET)
		readl(dev->base + DW_IC_CLR_STOP_DET);
	if (stat & DW_IC_INTR_START_DET)
		readl(dev->base + DW_IC_CLR_START_DET);
	if (stat & DW_IC_INTR_GEN_CALL)
		readl(dev->base + DW_IC_CLR_GEN_CALL);

	return stat;
}

/*
 * Interrupt service routine. This gets called whenever an I2C interrupt
 * occurs.
 */
static irqreturn_t i2c_dw_isr(int this_irq, void *dev_id)
{
	struct dw_i2c_dev *dev = dev_id;
	u32 stat;

	dev->irq_is_run = 1;
	stat = i2c_dw_read_clear_intrbits(dev);
	dev_dbg(dev->dev, "%s: stat=0x%x\n", __func__, stat);

	if (stat & DW_IC_INTR_TX_ABRT) {
		dev->cmd_err |= DW_IC_ERR_TX_ABRT;
		dev->status = STATUS_IDLE;

		/*
		 * Anytime TX_ABRT is set, the contents of the tx/rx
		 * buffers are flushed.  Make sure to skip them.
		 */
		writel(0, dev->base + DW_IC_INTR_MASK);
		goto tx_aborted;
	}

	if (stat & DW_IC_INTR_RX_FULL)
		i2c_dw_read(dev);

	if ((stat & DW_IC_INTR_STOP_DET)){
		if(!(dev->using_dma))
			i2c_dw_read(dev);
		goto tx_aborted;
	}

	if (stat & DW_IC_INTR_TX_EMPTY){
		i2c_dw_xfer_msg(dev);
	}
	/*
	 * No need to modify or disable the interrupt mask here.
	 * i2c_dw_xfer_msg() will take care of it according to
	 * the current transmit status.
	 */

tx_aborted:
	if ((stat & (DW_IC_INTR_TX_ABRT | DW_IC_INTR_STOP_DET)) || dev->msg_err){
		writel(0, dev->base + DW_IC_INTR_MASK);
		complete(&dev->cmd_complete);
	}

	dev->irq_is_run = 0;

	return IRQ_HANDLED;
}

static struct i2c_algorithm i2c_dw_algo = {
	.master_xfer	= i2c_dw_xfer,
	.functionality	= i2c_dw_func,
};

/*Config the Hisilicon HI6620 SOC pin for I2C function*/
static int devm_pinctrl_state_select(struct dw_i2c_dev *dev,const char *name)
{
	struct pinctrl_state *s;
	int ret;
	if(0 == dev->pinctrl_flag) {
		dev->pinctrl= devm_pinctrl_get(dev->platform_dev);
		if (IS_ERR(dev->pinctrl))
			return -1;
		dev->pinctrl_flag = 1;
	}

	s = pinctrl_lookup_state(dev->pinctrl, name);
	if (IS_ERR(s)) {
		devm_pinctrl_put(dev->pinctrl);
		dev->pinctrl_flag = 0;
		return -1;
	}

	ret = pinctrl_select_state(dev->pinctrl, s);
	if (ret < 0) {
		devm_pinctrl_put(dev->pinctrl);
		dev->pinctrl_flag = 0;
		return -1;
	}
	return 0;
}


static int dw_i2c_probe(struct platform_device *pdev)
{
	struct dw_i2c_dev *dev;
	struct i2c_adapter *adap;
	struct resource *mem;
	int irq, r = 0;
	struct device_node *node;
	u32 data[4], param1;

	dev = (struct dw_i2c_dev *)devm_kzalloc(&pdev->dev,sizeof(struct dw_i2c_dev), GFP_KERNEL);
	if (!dev) {
		r = -ENOMEM;
		goto err_release_region;
	}

	/* NOTE: driver uses the static register mapping */
	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!mem) {
		printk(KERN_ERR "no mem resource?\n");
		kfree(dev);
		return -EINVAL;
	}

	dev->base = devm_ioremap_resource(&pdev->dev, mem);
	if (!dev->base) {
		dev_err(&pdev->dev, "memory map error!");
		return PTR_ERR(dev->base);
	}

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		dev_err(&pdev->dev, "no irq resource?\n");
		return irq; /* -ENXIO */
	}

	if(!sctrl_base) {
		node = of_find_compatible_node(NULL, NULL,"hisilicon,sctrl");
		if (node)
			sctrl_base = of_iomap(node, 0);
	}

	of_property_read_u32_array(pdev->dev.of_node, "reset-controller-reg", &data[0], 4);
	dev->reset_enable_off = data[0];
	dev->reset_disable_off = data[1];
	dev->reset_status_off = data[2];
	dev->reset_bit = data[3];

	dev->platform_dev = &pdev->dev;
	dev->pinctrl_flag = 0;
	dev->pinctrl = NULL;

	init_completion(&dev->cmd_complete);
	init_completion(&dev->dma_complete);
	mutex_init(&dev->lock);
	dev->dev = get_device(&pdev->dev);
	dev->irq = irq;
	platform_set_drvdata(pdev, dev);

	dev->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(dev->clk)) {
		dev_err(&pdev->dev, "clk get failed!");
		return PTR_ERR(dev->clk);
	}
	i2c_clk_domain_enable(dev, 0);

	clk_prepare_enable(dev->clk);

	r = devm_pinctrl_state_select(dev, PINCTRL_STATE_DEFAULT);
	if (r < 0) {
		dev_err(&pdev->dev, "Set I2C pinctrl to NORMAL failed!\n");
		return -1;
	}

	dev->mapbase = mem->start;


	param1 = readl(dev->base + DW_IC_COMP_PARAM_1);

	dev->tx_fifo_depth = ((param1 >> 16) & 0xff) + 1;
	dev->rx_fifo_depth = ((param1 >> 8)  & 0xff) + 1;
	dev_dbg(&pdev->dev, "tx_fifo_depth: %d, rx_fifo_depth: %d\n",
	dev->tx_fifo_depth, dev->rx_fifo_depth);

	i2c_dw_init(dev);

	dev->irq_is_run = 0;

	r = request_irq(dev->irq, i2c_dw_isr, IRQF_DISABLED | IRQF_NO_THREAD, pdev->name, dev);
	if (r) {
		dev_err(&pdev->dev, "failure requesting irq %i\n", dev->irq);
		goto err_iounmap;
	}

	adap = &dev->adapter;
	i2c_set_adapdata(adap, dev);
	adap->owner = THIS_MODULE;
	adap->class = I2C_CLASS_HWMON;
	strlcpy(adap->name, "Synopsys DesignWare I2C adapter",
			sizeof(adap->name));
	adap->algo = &i2c_dw_algo;
	adap->dev.parent = &pdev->dev;
	adap->retries = RETRY_TIMES;
	adap->timeout = 3*HZ;
	adap->dev.of_node = pdev->dev.of_node;

	adap->nr = pdev->id;
	r = i2c_add_numbered_adapter(adap);
	if (r) {
		dev_err(&pdev->dev, "failure adding adapter\n");
		goto err_free_irq;
	}

	/* DMA probe */
	i2c_dw_dma_probe(dev);
	if(false == Chip_Verification_Check_Debug(CHIP_VERIFICATION_I2C_DEBUG) )
	{
	clk_disable_unprepare(dev->clk);
	}
	//i2c_clk_domain_enable(dev, 1);
#ifdef CONFIG_AUDIENCE
	if (1 == pdev->id) {
		dev->s_dev = kzalloc(sizeof(struct switch_dev), GFP_KERNEL);
		dev->s_dev->name = RESET_ES305;
		r = switch_dev_register(dev->s_dev);
		if (r) {
			dev_err(dev->dev, "%s(%u):error registering switch device %d\n", __FUNCTION__, __LINE__, r);
			kfree(dev->s_dev);
			dev->s_dev = NULL;
		}
		dev->timeout_flag_for_es305 = 0;
		dev->es305_state = 0;
	}
#endif   /* CONFIG_AUDIENCE */
	of_i2c_register_devices(adap);
	return 0;

err_free_irq:
	free_irq(dev->irq, dev);

err_iounmap:
	iounmap(dev->base);
	clk_disable_unprepare(dev->clk);
	clk_put(dev->clk);
	dev->clk = NULL;

	//i2c_clk_domain_enable(dev, 1);
	platform_set_drvdata(pdev, NULL);
	put_device(&pdev->dev);
	kfree(dev);

err_release_region:
	return r;
}

static int dw_i2c_remove(struct platform_device *pdev)
{
	struct dw_i2c_dev *dev = platform_get_drvdata(pdev);
	struct resource *mem;

	platform_set_drvdata(pdev, NULL);
	i2c_del_adapter(&dev->adapter);
	put_device(&pdev->dev);
	clk_disable_unprepare(dev->clk);
	clk_put(dev->clk);
	dev->clk = NULL;

	i2c_dw_dma_remove(dev);

	devm_pinctrl_put(dev->pinctrl);

	writel(0, dev->base + DW_IC_ENABLE);
	free_irq(dev->irq, dev);
#ifdef CONFIG_AUDIENCE
	if(1 == pdev->id) {
		if(dev->s_dev)
			kfree(dev->s_dev);
	}
#endif   /* CONFIG_AUDIENCE */
	kfree(dev);

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	release_mem_region(mem->start, resource_size(mem));
	return 0;
}

#ifdef CONFIG_PM
/* NOTE: could save a few mA by keeping clock off outside of i2c_dw_xfer... */
static int i2c_dw_suspend(struct platform_device *pdev, pm_message_t mesg)
{
	struct dw_i2c_dev *dev = platform_get_drvdata(pdev);
	int ret;

	if(NULL == dev) {
		dev_err(&pdev->dev, "platform_get_drvdata(pdev) return  NULL!\n");
		return -ENODEV;
	}
	dev_info(dev->dev, "%s: suspend +\n", __func__);
	if (!mutex_trylock(&dev->lock)) {
		dev_info(dev->dev, "%s: mutex_trylock.\n", __func__);
		return -EAGAIN;
	}

	ret = devm_pinctrl_state_select(dev, PINCTRL_STATE_IDLE);
	if (ret < 0) {
		dev_err(&pdev->dev, "Set I2C pinctrl to idle failed!\n");
		return -1;
	}

	dev_info(dev->dev, "%s: suspend -\n", __func__);
	return 0;
}

static int i2c_dw_resume(struct platform_device *pdev)
{
	struct dw_i2c_dev *dev = platform_get_drvdata(pdev);
	int ret = 0;

	if(NULL == dev) {
		dev_err(&pdev->dev, "platform_get_drvdata(pdev) return  NULL!\n");
		return -ENODEV;
	}
	dev_info(dev->dev, "%s: resume +\n", __func__);

	ret = devm_pinctrl_state_select(dev, PINCTRL_STATE_DEFAULT);
	if (ret < 0) {
		dev_err(&pdev->dev, "Set I2C pinctrl to NORMAL failed!\n");
		return -1;
	}

	ret = clk_prepare_enable(dev->clk);
	if(ret <0) {
		dev_err(dev->dev, "clk_enable failed!\n");
		return -EAGAIN;
	}

	reset_i2c_controller(dev);

	//i2c_dw_init(dev);
	if(false == Chip_Verification_Check_Debug(CHIP_VERIFICATION_I2C_DEBUG) )
	{
	clk_disable_unprepare(dev->clk);
	}
	mutex_unlock(&dev->lock);

	dev_info(dev->dev, "%s: resume -\n", __func__);
	return 0;
}
#else
#define i2c_dw_suspend		NULL
#define i2c_dw_resume		NULL
#endif


/* work with hotplug and coldplug */
MODULE_ALIAS("platform:i2c_designware");

#ifdef CONFIG_OF
static const struct of_device_id hs_hi6210_dw_i2c_of_match[] = {
	{ .compatible = "hisilicon,designware-i2c", },
	{},
};
MODULE_DEVICE_TABLE(of, hs_hi6210_dw_i2c_of_match);
#endif

static struct platform_driver dw_i2c_driver = {
	.remove		= dw_i2c_remove,
	.suspend	= i2c_dw_suspend,
	.resume		= i2c_dw_resume,
	.driver		= {
		.name	= "i2c_designware",
		.of_match_table = of_match_ptr(hs_hi6210_dw_i2c_of_match),
		.owner	= THIS_MODULE,
	},
};

static int __init dw_i2c_init_driver(void)
{
	return platform_driver_probe(&dw_i2c_driver, dw_i2c_probe);
}
arch_initcall_sync(dw_i2c_init_driver);

static void __exit dw_i2c_exit_driver(void)
{
	platform_driver_unregister(&dw_i2c_driver);
}
module_exit(dw_i2c_exit_driver);

MODULE_AUTHOR("Baruch Siach <baruch@tkos.co.il>");
MODULE_DESCRIPTION("Synopsys DesignWare I2C bus adapter");
MODULE_LICENSE("GPL");
