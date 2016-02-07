/*
 * Device driver for regulators in Hi6421 IC
 *
 * Copyright (C) 2013 Linaro Ltd. Guodong Xu <guodong.xu@linaro.org>
 *
 * Porting to hi3635 by Young Choi.
 * Copyright (C) 2014 Hisilicon, Young Choi <cuiyong1@hisilicon.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include <linux/io.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/platform_device.h>
#include <linux/pasr.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_irq.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/hwspinlock.h>

#define ACCESS_DDR_MR17_BY_AP

#define MASK_BTYE(n)                        (0xFF << (n * 8))
#define CLEAR_BYTE(n)                       (~(0xFF << (n * 8)))
#define SCBAKDATA(n)                        (0x314 + ((n) << 2))

#define RANK_DISABLE(n)                     (1 << n)
#define SFC_RANK(n)                         (1 << (n + 16))
#define SFC_CMD_REQ                         (1)
#define SFC_MR8                             (8)
#define SFC_MR17                            (17)
#define SFC_LOAD_MR                         (2)
#define SFC_READ_MR                         (7)
#define DDR_MR8_DENSITY_OFF                 (2)
#define DDR_MR8_DENSITY_MASK                (0xF << DDR_MR8_DENSITY_OFF)
#define DDR_RANK_NR_OFF                     (20)
#define DDR_RANK_NR_MASK                    (0x3 << DDR_RANK_NR_OFF)
#define DDR_RANK_SIZE_512M                  (0x20000000)
#define DDR_RANK_SIZE_1024M                 (0x40000000)
#define DDR_RANK_SIZE_1536M                 (0x60000000)
#define DDR_RANK_SIZE_2048M                 (0x80000000)
#define DDR_MR17_MASK                       (0xFF)
#define DDR_MR17_RANK0_512M_MASK            (0x00)
#define DDR_MR17_RANK0_1024M_MASK           (0x00)
#define DDR_MR17_RANK0_1536M_MASK           (0x00)
#define DDR_MR17_RANK0_2048M_MASK           (0x00)
#define DDR_HWLOCK_ID                       (22)
#define DDR_HWLOCK_TIMEOUT                  (1000)

#define DDRC_DMC_OFFSET                     (0x8000)
#define DDRC_CFG_WORKMODE2(n)               (DDRC_DMC_OFFSET + ((n) * 0x1000) + 0x044)
#define DDRC_CFG_DDRMODE(n)                 (DDRC_DMC_OFFSET + ((n) * 0x1000) + 0x050)
#define DDRC_CFG_SFC(n)                     (DDRC_DMC_OFFSET + ((n) * 0x1000) + 0x210)
#define DDRC_CFG_SFC_ADDR1(n)               (DDRC_DMC_OFFSET + ((n) * 0x1000) + 0x218)
#define DDRC_HIS_SFC_RDATA0(n)              (DDRC_DMC_OFFSET + ((n) * 0x1000) + 0x4A8)
#define DDRC_CTRL_SFC(n)                    (DDRC_DMC_OFFSET + ((n) * 0x1000) + 0x00C)

struct ddr_rank_info {
	u32 addr;
	u32 size;
	u32 memreg;
};

static struct hwspinlock *ddr_hwlock = NULL;
static void __iomem *ddrc_base = NULL;
static spinlock_t pasr_lock;

#ifndef ACCESS_DDR_MR17_BY_AP
static void __iomem *sysctrl_base = NULL;
#endif

static u32 rank_nr = 0;
static struct ddr_rank_info rank_info[PASR_MAX_DIE_NR];

static ssize_t show_mem_regs(struct device *dev,
			struct device_attribute *devattr, char *buf);

static DEVICE_ATTR(memregs, S_IRUGO, show_mem_regs, NULL);

static struct attribute *hi3635_pasr_attributes[] = {
	&dev_attr_memregs.attr,
	NULL,
};

static const struct attribute_group hi3635_attr_group = {
	.attrs = hi3635_pasr_attributes,
};

static ssize_t show_mem_regs(struct device *dev,
			struct device_attribute *devattr, char *buf)
{
	struct ddr_rank_info *info;
	int i,size = 0;

	for(i = 0; i < rank_nr; i++) {
		info = &rank_info[i];
		size += snprintf(buf + size, PAGE_SIZE / rank_nr,
			"%s: rank%d addr=0x%08x, size = 0x%08x, mem_reg = 0x%08x\n",
			__func__, i, info->addr, info->size, info->memreg);
	}

	return size;
}

static void hi3635_pasr_refresh_disable(int rank)
{
	u32 value;

	value = readl(ddrc_base + DDRC_CFG_WORKMODE2(0)) | RANK_DISABLE(rank);
	writel(value, ddrc_base + DDRC_CFG_WORKMODE2(0));
	value = readl(ddrc_base + DDRC_CFG_WORKMODE2(1)) | RANK_DISABLE(rank);
	writel(value, ddrc_base + DDRC_CFG_WORKMODE2(1));
}

static void hi3635_pasr_refresh_enable(int rank)
{
	u32 value;

	value = readl(ddrc_base + DDRC_CFG_WORKMODE2(0)) & (~RANK_DISABLE(rank));
	writel(value, ddrc_base + DDRC_CFG_WORKMODE2(0));
	value = readl(ddrc_base + DDRC_CFG_WORKMODE2(1)) & (~RANK_DISABLE(rank));
	writel(value, ddrc_base + DDRC_CFG_WORKMODE2(1));
}

static void hi3635_pasr_send_sfc_cmd(u32 cmd, int rank)
{
	int timeout = 500;

	writel(cmd, ddrc_base + DDRC_CFG_SFC(0));
	writel(cmd, ddrc_base + DDRC_CFG_SFC(1));

	writel(SFC_RANK(rank), ddrc_base + DDRC_CFG_SFC_ADDR1(0));
	writel(SFC_RANK(rank), ddrc_base + DDRC_CFG_SFC_ADDR1(1));

	writel(SFC_CMD_REQ, ddrc_base + DDRC_CTRL_SFC(0));
	writel(SFC_CMD_REQ, ddrc_base + DDRC_CTRL_SFC(1));

	while ((readl(ddrc_base + DDRC_CTRL_SFC(0)) & SFC_CMD_REQ)
		|| (readl(ddrc_base + DDRC_CTRL_SFC(1)) & SFC_CMD_REQ)) {
		if (timeout-- < 0) {
			pr_err("%s: wait cmd done timeout!\n", __func__);
			break;
		}
	}
}

static u32 hi3635_pasr_read_ddr_mr(int rank, int target, int channel)
{
	u32 value = 0, cmd;

	if (hwspin_lock_timeout(ddr_hwlock, DDR_HWLOCK_TIMEOUT)) {
		pr_err("%s: hwspinlock timeout!\n", __func__);
		return value;
	}

	hi3635_pasr_refresh_disable(rank);
	cmd = ((target << 8) | SFC_READ_MR);
	hi3635_pasr_send_sfc_cmd(cmd, rank);
	value = readl(ddrc_base + DDRC_HIS_SFC_RDATA0(channel));
	hi3635_pasr_refresh_enable(rank);

	hwspin_unlock(ddr_hwlock);

	return value;
}

static void hi3635_pasr_write_ddr_mr(u32 value , int rank, int target)
{
	u32 cmd;

	if (hwspin_lock_timeout(ddr_hwlock, DDR_HWLOCK_TIMEOUT)) {
		pr_err("%s: hwspinlock timeout!\n", __func__);
		return ;
	}

	hi3635_pasr_refresh_disable(rank);
	cmd = ((value << 16) | (target << 8) | SFC_LOAD_MR);
	hi3635_pasr_send_sfc_cmd(cmd, rank);
	hi3635_pasr_refresh_enable(rank);

	hwspin_unlock(ddr_hwlock);
}

static u32 hi3635_pasr_mask_reserved_ddr_region(u32 value, int rank)
{
	u32 size = rank_info[rank].size;
	value &= DDR_MR17_MASK;

	if(rank)
		return value;

	switch (size) {
	case DDR_RANK_SIZE_1024M:
		value &= DDR_MR17_RANK0_1024M_MASK;
		break;
	case DDR_RANK_SIZE_1536M:
		value &= DDR_MR17_RANK0_1536M_MASK;
		break;
	case DDR_RANK_SIZE_2048M:
		value &= DDR_MR17_RANK0_2048M_MASK;
		break;
	default:
		pr_err("%s: invalid rank size %u.\n",
			__func__, value);
		break;
	}

	return value;
}

#ifndef ACCESS_DDR_MR17_BY_AP
static void hi3635_pasr_write_sysctrl_reg(u32 value, int rank)
{
	u32 reg = readl(sysctrl_base + SCBAKDATA(11));
	reg &= CLEAR_BYTE(rank);
	reg |= (value << (rank * 8));
	writel(reg, sysctrl_base + SCBAKDATA(11));
}
#endif

static void hi3635_pasr_apply_mask(u32 *mem_reg, void *cookie)
{
	int rank = (int)cookie;
	u32 value = *mem_reg;

	if ((rank >= rank_nr) || (rank < 0)) {
		pr_err("%s: invalid rank value %d.\n",
				__func__, rank);
		return ;
	}

	rank_info[rank].memreg = value;
	value = hi3635_pasr_mask_reserved_ddr_region(value, rank);

#ifndef ACCESS_DDR_MR17_BY_AP
	if (sysctrl_base != NULL)
		hi3635_pasr_write_sysctrl_reg(value, rank);
#else
	if (ddr_hwlock != NULL)
		hi3635_pasr_write_ddr_mr(value, rank, SFC_MR17);
#endif
}

static u32 hi3635_pasr_get_rank_size(int rank)
{
	u32 size = 0;
	u32 value = hi3635_pasr_read_ddr_mr(rank, SFC_MR8, 0);
	value = (value & DDR_MR8_DENSITY_MASK) >> DDR_MR8_DENSITY_OFF;

	switch (value) {
	case 0x6:
		size = DDR_RANK_SIZE_1024M;
		break;
	case 0x7:
		size = DDR_RANK_SIZE_2048M;
		break;
	case 0xe:
		size = DDR_RANK_SIZE_1536M;
		break;
	default:
		pr_err("%s: invalid mr8 value %u.\n",
				__func__, value);
		break;
	}

	return size;
}

static void hi3635_pasr_get_rank_info(void)
{
	int i;
	u32 size;
	unsigned long flags = 0;

	rank_nr = readl(ddrc_base + DDRC_CFG_DDRMODE(0)) & DDR_RANK_NR_MASK;
	rank_nr = (rank_nr >> DDR_RANK_NR_OFF) + 1;

	memset(rank_info, 0, sizeof(rank_info));

	for (i = 0; i < rank_nr; i++) {
		spin_lock_irqsave(&pasr_lock, flags);
		size = hi3635_pasr_get_rank_size(i);
		spin_unlock_irqrestore(&pasr_lock, flags);

		if (size == 0)
			break;

		rank_info[i].size = size;

		if (i > 0)
			rank_info[i].addr = rank_info[i-1].addr + rank_info[i-1].size;
	}
}

static int hi3635_pasr_probe(struct platform_device *pdev)
{
	struct device_node *np;
	struct device *dev = &pdev->dev;
	int err, i;

	dev_crit(dev, "%s: enter\n", __func__);

	np = dev->of_node;

#ifndef ACCESS_DDR_MR17_BY_AP
	sysctrl_base = of_iomap(np, 1);
	if (sysctrl_base == NULL) {
		dev_err(dev, "failed to iomap sysctrl regs!\n");
		return -ENXIO;
	}
#endif

	ddrc_base = of_iomap(np, 0);
	if (ddrc_base == NULL) {
		dev_err(dev, "failed to iomap ddrc regs!\n");
		return -ENXIO;
	}

	spin_lock_init(&pasr_lock);

	ddr_hwlock = hwspin_lock_request_specific(DDR_HWLOCK_ID);
	if (ddr_hwlock == NULL) {
		dev_err(dev, "pasr get hwspin_lock failed\n");
		return -EBUSY;
	}

	hi3635_pasr_get_rank_info();

	for (i = 0; i < rank_nr; i++) {
		if (pasr_register_mask_function(rank_info[i].addr,
					&hi3635_pasr_apply_mask, (void *)i)) {
			dev_err(dev, "pasr register failed, rank %d\n", i);
		}
	}

	err = sysfs_create_group(&pdev->dev.kobj, &hi3635_attr_group);
	if (err < 0) {
		dev_err(dev, "create sysfs group failed (%d)\n", err);
		return err;
	}

	dev_crit(dev, "%s: exit\n", __func__);

	return 0;
}

static int hi3635_pasr_remove(struct platform_device *pdev)
{
	return 0;
}

#ifdef CONFIG_OF
static struct of_device_id of_hi3635_pasr_match_tbl[] = {
	{
		.compatible = "hisilicon,pasr",
	},
	{ /* end */ }
};
MODULE_DEVICE_TABLE(of, of_hi3635_pasr_match_tbl);
#endif

static struct platform_driver hi3635_pasr_driver = {
	.driver = {
		.name	= "hi3635_pasr",
		.owner	= THIS_MODULE,
#ifdef CONFIG_OF
		.of_match_table = of_match_ptr(of_hi3635_pasr_match_tbl),
#endif
	},
	.probe	= hi3635_pasr_probe,
	.remove	= hi3635_pasr_remove,
};
module_platform_driver(hi3635_pasr_driver);

MODULE_AUTHOR("Young Choi <cuiyong1@hisilicon.com>");
MODULE_DESCRIPTION("Hi3xxx PASR driver");
MODULE_LICENSE("GPL v2");
