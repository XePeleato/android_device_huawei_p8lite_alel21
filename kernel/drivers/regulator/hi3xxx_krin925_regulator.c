/*
 * Device driver for regulators in Hi3630 IC
 *
 * Copyright (c) 2013 Linaro Ltd.
 * Copyright (c) 2011 Hisilicon.
 *
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

#include <linux/slab.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/regmap.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/of_regulator.h>
#include <linux/mfd/hi3xxx_hi3630.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/clk.h>
#include <linux/regulator/consumer.h>
#include <linux/clk-private.h>
#include <linux/interrupt.h>
#include <linux/clk-private.h>
#include <linux/hwspinlock.h>
#include <linux/hisi/hisi_rproc.h>

#ifdef CONFIG_DEBUG_FS
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#endif
#define	REGULATOR_HWLOCK_ID	4
#define	REGULATOR_LOCK_TIMEOUT	1000
static struct hwspinlock	*regulator_hwlock;
#define	REGULATOR_NOC_HWLOCK_ID 16
#define	REGULATOR_NOC_LOCK_TIMEOUT	(0xffffffff)
static struct hwspinlock	*regulator_noc_hwlock;


#ifdef CONFIG_HISI_NOC_ERR_PROBE
extern void enable_err_probe_by_name(const char *name);
extern void disable_err_probe_by_name(const char *name);
#endif

enum hi3630_regulator_id {
	VCODECSUBSYS_ID = 0,
	VPPSUBSYS_ID,
	VPPIP_ID,
	JPEGIP_ID,
	VDEC_ID,
	VENC_ID,
	DSSUBSYS_ID,
	ISPSUBSYS_ID,
	SDP_ID,
	ADP_ID,
	DEBUGSUBSYS_ID,
	SEC_P_ID,
	TOP_CSSYS_ID,
	USB2OTG_ID,
	HSIC_ID,
	G3D_ID,
	ASP_ID,
	HIFI_ID,
	LDO8_ID,
};

/******************************macro parameter*********************************/
enum hi3630_regulator_flag {
	MTCMOS_FLAG = 1,
	SOFTRESET_FLAG,
	ISO_FLAG,
	TOPCSSYS_FLAG,
	CLOCK_FLAG,
	SYSCTRL_FLAG = 6,
	NOCIDLEMODE_FLAG,
	RECLOCK_FLAG,
};

enum hi3630_regulator_dts {
	MTCMOST_DTS = 1,
	SOTFREST_DTS,
	ISO_DTS,
	TOPCSSYS_DTS,
	CLOCK_DTS,
};

enum hi3630_regulator_addr_flag {
	CRGPERI_ADDR_FLAG = 0,
	DSS_ADDR_FLAG,
	VPP_QOS_ADDR_FLAG,
	VDEC_QOS_ADDR_FLAG,
	VENC_QOS_ADDR_FLAG,
	DSS_QOS_ADDR_FLAG,
	ISP_QOS_ADDR_FLAG,
};

enum {
	REGULATOR_DISABLE = 0,
	REGULATOR_ENABLE
};
/******************************pmc/dss/sctrl parameter***************************/
/*lint -e750*/
#define PMC_NOC_POWER_IDLEREQ		0x380
#define PMC_NOC_POWER_IDLEACK		0x384
#define PMC_NOC_POWER_IDLE			0x388

#define PMC_NOC_CODECSUBSYS_IDLE		(1 << 4)
#define PMC_NOC_ISPSUBSYS_IDLE		(1 << 5)
#define PMC_NOC_DEBUG_IDLE			(1 << 9)
#define PMC_NOC_VDEC_IDLE			(1 << 10)
#define PMC_NOC_VENC_IDLE			(1 << 11)
#define PMC_NOC_VPPSUBSYS_IDLE		(1 << 12)
#define PMC_NOC_DSSSUBSYS_IDLE		(1 << 13)

#define DSS_DIS_CLK_CFG				0x420
#define DSS_EN_CLK_CFG				0x408
#define DSS_DIS_RST_CFG			0x424
#define DSS_SDP_SOFT_RESET_EN_VALUE	0x1c29000
#define DSS_SDP_CLK_CLOSE_VALUE	0x1e0
#define DSS_SDP_CLK_OPEN_VALUE	0x1e0
#define DSS_SDP_SOTF_RESET_OFF_VALUE	0x29000
#define DSS_SDP_HDMI_EFUSE_VALUE	0x800000

#define DSS_RST_EN_CFG			0x424
#define DSS_RST_DIS_CFG		0x410
#define DSS_RST_VALUE			0x400000

#define DSS_ADP_SOFT_RESET_EN_VALUE	0x280001
#define DSS_ADP_CLK_CLOSE_VALUE	0x1800
#define DSS_ADP_CLK_OPEN_VALUE	0x1800
#define DSS_ADP_SOTF_RESET_OFF_VALUE	0x280001

#define DSS_CLK_VALUE			0x01

#define PERI_CTRL_ADDR	0x68
#define SDP_SC_DSI1_ITF0_BIT	8
#define SDP_SC_DSI1_ITF1_BIT	9
#define SDP_SC_DSI1_ITF2_BIT	10
#define SDP_SC_DSI1_ITF0_CLR	0
#define SDP_SC_DSI1_ITF0_SET	1

#define SCTRL_REPAIR_DISABLE			0x10FFFFF
#define SCTRL_REPAIR_DISABLE_ADDR		0x208
#define SCTRL_REPAIR_FRM_ADDR			0x20c
#define SCTRL_REPAIR_DISABLE_OK_ADDR	0x304
#define SCTRL_MEMORY_REPAIR_FLAG	0x34C
#define WORK_STEP_NUM					8
#define SCTRL_REPAIR_JTAG_MASK		0xFFF00000
#define SCTRL_REPAIR_CONTRL_MASK		0xFFFFF
#define MEMORY_REPAIR_DELAY_TIME		300

#define SCTRL_AUDIO_STATE_ADDR		0x0d8
#define SCTRL_AUDIO_STATE_MASK		0x01
#define LDO8_STATE_ADDR			0x85
#define LDO8_STATE_MASK			0x20

#define VDEC_QOS_BASE_ADDR			0xE8930000
#define VENC_QOS_BASE_ADDR			0xE8940000
#define VPP_QOS_BASE_ADDR			0xE8950000
#define DSS_QOS_BASE_ADDR			0xE85C0000
#define ISP_QOS_BASE_ADDR				0xE85D0000

#define VDEC_REPAIR_DISABLE			18
#define VENC_REPAIR_DISABLE			19
#define DSS_SUBSYS_REPAIR_DISABLE	4
#define ISP_REPAIR_DISABLE				11
#define SDP_REPAIR_DISABLE			5
#define CRGPERI_ADDR_OFFSET			0x04
#define DSS_ADDR_OFFSET				0x14

#define AP_TO_LPM3_MSG_NUM			2

#define VCODECSUBSYS_CLOCK_NUM			5
#define VPPSUBSYS_CLOCK_NUM			1
#define VPPIP_CLOCK_NUM				1
#define JPEGIP_CLOCK_NUM				1
#define VDEC_CLOCK_NUM				1
#define VENC_CLOCK_NUM				1
#define DSSUBSYS_CLOCK_NUM			4
#define ISPSUBSYS_CLOCK_NUM1			5
#define ISPSUBSYS_CLOCK_NUM2			6
#define SDP_CLOCK_NUM				3
#define ADP_CLOCK_NUM				1
#define SEC_P_CLOCK_NUM			1
#define TOP_CSSYS_CLOCK_NUM			1
#define USB2OTG_CLOCK_NUM			3
#define HSIC_CLOCK_NUM				5

#define VALID_FLAG_MASK 0x80000000
#define BYPASS_FLAG_MASK 0x100000
#define MRB_FLAG_MASK 0xFFFFF
#define MEMORY_REPAIR_INFO_ADDR	0x34c
#define MEMORY_REPAIR_DONE_ADDR 0x228
#define MEMORY_ERROR_FLAG_ADDR 0x220
#define MEMORY_REPAIR_DELAY_TIME1		300
#define MEMORY_REPAIR_DELAY_TIME2		20
#define SCTRL_REPAIR_DISABLE_ADDR_MASK	0xFFFFF
#define BYPASS_FLAG_MASK_VALUE	20
/*lint +e750*/
/*****************************hi3630 struct parameter*****************************/
struct hisi_regulator_hi3630_register_info {
	u32 mtcmos_state_reg;
	u32 mtcmos_state_mask;
	u32 mtcmos_state_addr;
	u32 mtcmos_en_reg;
	u32 mtcmos_en_mask;
	u32 mtcmos_en_addr;
	u32 swrest_en_reg;
	u32 swrest_en_mask;
	u32 swrest_en_addr;
	u32 iso_en_reg;
	u32 iso_en_mask;
	u32 iso_en_addr;
	u32 top_cssys_reg;
	u32 top_cssys_mask;
	u32 top_cssys_addr;
};
struct hisi_qos_noc_register_info {
	u32 qos_priority_reg;
	u32 qos_priority_mask;
	u32 qos_mode_reg;
	u32 qos_mode_mask;
	u32 qos_dss0_rd_reg;
	u32 qos_dss0_rd_mask;
	u32 qos_dss0_wr_reg;
	u32 qos_dss0_wr_mask;
	u32 qos_dss1_rd_reg;
	u32 qos_dss1_rd_mask;
	u32 qos_dss1_wr_reg;
	u32 qos_dss1_wr_mask;
	u32 qos_isp1_rd_reg;
	u32 qos_isp1_rd_mask;
	u32 qos_isp1_wr_reg;
	u32 qos_isp1_wr_mask;
	u32 qos_isp2_rd_reg;
	u32 qos_isp2_rd_mask;
	u32 qos_isp2_wr_reg;
	u32 qos_isp2_wr_mask;
	u32 qos_isp3_rd_reg;
	u32 qos_isp3_rd_mask;
	u32 qos_isp3_wr_reg;
	u32 qos_isp3_wr_mask;
	u32 qos_isp4_rd_reg;
	u32 qos_isp4_rd_mask;
	u32 qos_noc_addr_flag;
};
struct hisi_regulator_hi3630 {
	const char *name;
	struct hisi_regulator_hi3630_register_info register_info;
	struct hisi_qos_noc_register_info qos_noc_info;
	struct timeval last_off_time;
	u32 off_on_delay;
	u32 eco_uA;
	struct regulator_desc rdesc;
	int (*dt_parse)(struct hisi_regulator_hi3630 *, struct platform_device *);
	int regulator_id;
	struct mutex regulator_lock;
	unsigned int g_enable_work_info[2][WORK_STEP_NUM];
	unsigned int g_disable_work_info[2][WORK_STEP_NUM];
	u32 lpm3_enable_value[AP_TO_LPM3_MSG_NUM];
	u32 lpm3_disable_value[AP_TO_LPM3_MSG_NUM];
	struct device_node *np;
#ifdef CONFIG_HISI_NOC_ERR_PROBE
	const char *noc_node_name;
#endif
	u32 regulator_enalbe_flag;
};
struct hisi_regulator_addr {
	void __iomem *pmc_reg;
	void __iomem *sctrl_reg;
	void __iomem *dss_reg;
	void __iomem *vdec_qos_reg;
	void __iomem *venc_qos_reg;
	void __iomem *vpp_qos_reg;
	void __iomem *dss_qos_reg;
	void __iomem *isp_qos_reg;
	void __iomem *pmic_reg;
	void __iomem *crgperi_reg;
	void __iomem *pctrl_reg;
};
static struct hisi_regulator_addr g_regu;
/**************************control interface***************************************/
static void set_noc_enter_idle_mode(int value)
{
	int pmc_value, pmc_value1, pmc_value2;

	if (hwspin_lock_timeout(regulator_noc_hwlock, REGULATOR_NOC_LOCK_TIMEOUT)) {
		pr_err("%s: hwspinlock timeout!\n", __func__);
		return ;
	}

	pmc_value = readl(g_regu.pmc_reg + PMC_NOC_POWER_IDLEREQ);
	pmc_value &=  (~value);
	pmc_value |= value;
	writel(pmc_value, g_regu.pmc_reg + PMC_NOC_POWER_IDLEREQ);

	hwspin_unlock(regulator_noc_hwlock);

	while (1) {
		pmc_value1 = readl(g_regu.pmc_reg + PMC_NOC_POWER_IDLEACK);
		pmc_value2 = readl(g_regu.pmc_reg + PMC_NOC_POWER_IDLE);
		if (((pmc_value1 & value) != 0) && ((pmc_value2 & value) != 0))
			break;
	}
}
static void set_noc_exit_idle_mode(unsigned int value)
{
	int pmc_value, pmc_value1, pmc_value2;

	if (hwspin_lock_timeout(regulator_noc_hwlock, REGULATOR_NOC_LOCK_TIMEOUT)) {
		pr_err("%s: hwspinlock timeout!\n", __func__);
		return ;
	}

	pmc_value = readl(g_regu.pmc_reg + PMC_NOC_POWER_IDLEREQ);
	pmc_value &=  (~value);
	writel(pmc_value, g_regu.pmc_reg + PMC_NOC_POWER_IDLEREQ);

	hwspin_unlock(regulator_noc_hwlock);

	while (1) {
		pmc_value1 = readl(g_regu.pmc_reg + PMC_NOC_POWER_IDLEACK);
		pmc_value2 = readl(g_regu.pmc_reg + PMC_NOC_POWER_IDLE);
		if (((pmc_value1 & value) == 0) && ((pmc_value2 & value) == 0))
			break;
	}
}

static int  hisi_sctrl_regu_read(unsigned int reg)
{
	return readl(g_regu.sctrl_reg + reg);
}
static void hisi_sctrl_regu_write(unsigned int val, unsigned int reg)
{
	writel(val, g_regu.sctrl_reg + reg);
}

/*static int hisi_pmic_regu_read(unsigned int reg)
{
	return readl(g_regu.pmic_reg + (reg << 2));
}*/

static void set_sctrl_memory(unsigned int repair_disable_val)
{
	int ret, value, m_value, bypass_val, mrb_val;
	static int first_flag, err_flag, udelay_flag1 = 0, udelay_flag2 = 0;

	m_value = hisi_sctrl_regu_read(MEMORY_REPAIR_INFO_ADDR);
	bypass_val = m_value & BYPASS_FLAG_MASK;
	if (0 == bypass_val)
		return;
	mrb_val = m_value & (0x01 << repair_disable_val);
	if (0 == mrb_val)
		return;

	if (hwspin_lock_timeout(regulator_hwlock, REGULATOR_LOCK_TIMEOUT)) {
		pr_err("%s: hwspinlock timeout!\n", __func__);
		return ;
	}
	ret = hisi_sctrl_regu_read(SCTRL_REPAIR_FRM_ADDR);
	ret &= ~(0x03 << 3);
	hisi_sctrl_regu_write(ret, SCTRL_REPAIR_FRM_ADDR);

	ret = hisi_sctrl_regu_read(SCTRL_REPAIR_DISABLE_ADDR);
	ret &= SCTRL_REPAIR_JTAG_MASK;
	ret |= SCTRL_REPAIR_DISABLE_ADDR_MASK;
	hisi_sctrl_regu_write(ret, SCTRL_REPAIR_DISABLE_ADDR);
	udelay(1);

	ret = hisi_sctrl_regu_read(SCTRL_REPAIR_DISABLE_ADDR);
	ret &= SCTRL_REPAIR_JTAG_MASK;
	value = SCTRL_REPAIR_CONTRL_MASK & (~(1 << repair_disable_val));
	ret |= value;
	hisi_sctrl_regu_write(ret, SCTRL_REPAIR_DISABLE_ADDR);
	udelay(5);
	if ((first_flag & (1 << repair_disable_val)) == 0) {
		ret = hisi_sctrl_regu_read(SCTRL_REPAIR_FRM_ADDR);
		ret |= 1 << 4;
		hisi_sctrl_regu_write(ret, SCTRL_REPAIR_FRM_ADDR);
		first_flag |= 1 << repair_disable_val;
	} else {
		ret = hisi_sctrl_regu_read(SCTRL_REPAIR_FRM_ADDR);
		ret |= 1 << 3;
		hisi_sctrl_regu_write(ret, SCTRL_REPAIR_FRM_ADDR);
	}
	while(1) {
		ret = hisi_sctrl_regu_read(MEMORY_REPAIR_DONE_ADDR);
		if (ret & (1 << repair_disable_val)) {
			break;
		} else {
			udelay(1);
			udelay_flag2++;
			if (MEMORY_REPAIR_DELAY_TIME2 <= udelay_flag2) {
				pr_err("%s: [%d]memory repair donot complete! memory repair not start!\n", __func__, repair_disable_val);
				hwspin_unlock(regulator_hwlock);
				BUG_ON(1);
				return;
			}
		}
	}
	while (1) {
		ret = hisi_sctrl_regu_read(SCTRL_REPAIR_DISABLE_OK_ADDR);
		ret &= 1 << repair_disable_val;
		if (0 == ret) {
			break;
		} else {
			udelay(1);
			udelay_flag1++;
			if (MEMORY_REPAIR_DELAY_TIME1 <= udelay_flag1) {
				pr_err("%s: [%d]memory repair donot complete! Time out!\n", __func__, repair_disable_val);
				hwspin_unlock(regulator_hwlock);
				BUG_ON(1);
				return;
			}
		}
	}

	err_flag = hisi_sctrl_regu_read(MEMORY_ERROR_FLAG_ADDR);
	if (0 == err_flag) {
		ret = hisi_sctrl_regu_read(SCTRL_REPAIR_FRM_ADDR);
		ret &= ~(0x03 << 3);
		hisi_sctrl_regu_write(ret, SCTRL_REPAIR_FRM_ADDR);
		hwspin_unlock(regulator_hwlock);
	} else {
		pr_err("%d:memory repair not complete! repair date error err_flag=0x%x\n", repair_disable_val, err_flag);
		hwspin_unlock(regulator_hwlock);
		BUG_ON(1);
	}

	return;
}

static void hisi_dss_regu_write(unsigned int val, unsigned int reg)
{
	writel(val, g_regu.dss_reg + reg);
	return ;
}

static void hisi_pctrl_regu_rmw(int reg, u32 bits, u32 flags)
{
	u32 data;

	data = readl(g_regu.pctrl_reg + reg);
	if (1 == flags)
		data |= (0x01 << bits);
	else
		data &= ~(0x01 << bits);

	writel(data, g_regu.pctrl_reg + reg);

	return ;
}

/*static u32 hisi_pctrl_regu_read(unsigned int reg)
{
	return readl(g_regu.pctrl_reg + reg);
}*/

/*************************************************************************/
static inline struct hisi_regulator_hi3630_core *rdev_to_hi3630_core(struct regulator_dev *dev)
{
	/* regulator_dev parent to->
	 * hi6421 regulator platform device_dev parent to->
	 * hi6421 pmic platform device_dev
	 */
	return dev_get_drvdata(rdev_get_dev(dev)->parent->parent);
}

static void enable_mtcmos(struct hisi_regulator_hi3630_core *pmic, struct hisi_regulator_hi3630 *sreg)
{
	if (pmic == NULL || sreg ==  NULL)
		return;
	if (sreg->register_info.mtcmos_en_addr == CRGPERI_ADDR_FLAG)
		hisi_regulator_hi3630_core_write(pmic, sreg->register_info.mtcmos_en_reg,
						sreg->register_info.mtcmos_en_mask);
	else
		pr_err("Input addr not exist!\n\r");

	return;
}
static void disable_mtcmos(struct hisi_regulator_hi3630_core *pmic, struct hisi_regulator_hi3630 *sreg)
{
	if (pmic == NULL || sreg ==  NULL)
		return;
	if (sreg->register_info.mtcmos_en_addr == CRGPERI_ADDR_FLAG)
		hisi_regulator_hi3630_core_write(pmic, sreg->register_info.mtcmos_en_reg + CRGPERI_ADDR_OFFSET,
						sreg->register_info.mtcmos_en_mask);
	else
		pr_err("Input addr not exist!\n\r");

	return;
}
static void enable_softrst(struct hisi_regulator_hi3630_core *pmic, struct hisi_regulator_hi3630 *sreg)
{
	if (pmic == NULL || sreg ==  NULL)
		return;
	if (sreg->register_info.swrest_en_addr == CRGPERI_ADDR_FLAG) {
		hisi_regulator_hi3630_core_write(pmic, sreg->register_info.swrest_en_reg,
						sreg->register_info.swrest_en_mask);
	} else if (sreg->register_info.swrest_en_addr == DSS_ADDR_FLAG) {
		if (SDP_ID == sreg->regulator_id) {
			hisi_dss_regu_write(sreg->register_info.swrest_en_mask, sreg->register_info.swrest_en_reg);
			hisi_pctrl_regu_rmw(PERI_CTRL_ADDR, SDP_SC_DSI1_ITF1_BIT, SDP_SC_DSI1_ITF0_CLR);
		} else {
			hisi_dss_regu_write(sreg->register_info.swrest_en_mask, sreg->register_info.swrest_en_reg);
		}
	} else
		pr_err("Input addr not exist!\n\r");

	return;
}
static void disable_softrst(struct hisi_regulator_hi3630_core *pmic, struct hisi_regulator_hi3630 *sreg)
{
	if (pmic == NULL || sreg ==  NULL)
		return;
	if (sreg->register_info.swrest_en_addr == CRGPERI_ADDR_FLAG) {
		hisi_regulator_hi3630_core_write(pmic, sreg->register_info.swrest_en_reg + CRGPERI_ADDR_OFFSET,
						sreg->register_info.swrest_en_mask);
	} else if (sreg->register_info.swrest_en_addr == DSS_ADDR_FLAG) {
		if (SDP_ID == sreg->regulator_id) {
			hisi_pctrl_regu_rmw(PERI_CTRL_ADDR, SDP_SC_DSI1_ITF0_BIT, SDP_SC_DSI1_ITF0_SET);
			hisi_dss_regu_write(sreg->register_info.swrest_en_mask, sreg->register_info.swrest_en_reg + DSS_ADDR_OFFSET);
			hisi_pctrl_regu_rmw(PERI_CTRL_ADDR, SDP_SC_DSI1_ITF1_BIT, SDP_SC_DSI1_ITF0_SET);
		} else {
			hisi_dss_regu_write(sreg->register_info.swrest_en_mask, sreg->register_info.swrest_en_reg + DSS_ADDR_OFFSET);
		}
	} else {
		pr_err("Input addr not exist!\n\r");
	}
	return;
}
static inline void enable_iso(struct hisi_regulator_hi3630_core *pmic, struct hisi_regulator_hi3630 *sreg)
{
	if (pmic == NULL || sreg ==  NULL)
		return;
	if (sreg->register_info.iso_en_addr == CRGPERI_ADDR_FLAG)
		hisi_regulator_hi3630_core_write(pmic, sreg->register_info.iso_en_reg,
						sreg->register_info.iso_en_mask);
	else
		pr_err("Input addr not exist!\n\r");
	return;
}

static inline void disable_iso(struct hisi_regulator_hi3630_core *pmic, struct hisi_regulator_hi3630 *sreg)
{
	if (pmic == NULL || sreg ==  NULL)
		return;
	if (sreg->register_info.iso_en_addr == CRGPERI_ADDR_FLAG)
		hisi_regulator_hi3630_core_write(pmic, sreg->register_info.iso_en_reg + CRGPERI_ADDR_OFFSET,
						sreg->register_info.iso_en_mask);
	else
		pr_err("Input addr not exist!\n\r");
	return;
}

void set_top_cssys_softreset_on(struct hisi_regulator_hi3630_core *pmic,
												struct hisi_regulator_hi3630 *sreg)
{
	int reg_val;
	reg_val = hisi_regulator_hi3630_core_read(pmic, sreg->register_info.top_cssys_reg);
	reg_val &=  (~(sreg->register_info.top_cssys_mask));
	reg_val |= (sreg->register_info.top_cssys_mask);
	hisi_regulator_hi3630_core_write(pmic, sreg->register_info.top_cssys_reg, reg_val);
}

void set_top_cssys_softreset_off(struct hisi_regulator_hi3630_core *pmic,
												struct hisi_regulator_hi3630 *sreg)
{
	int reg_val;
	reg_val = hisi_regulator_hi3630_core_read(pmic, sreg->register_info.top_cssys_reg);
	reg_val &=  (~(sreg->register_info.top_cssys_mask));
	hisi_regulator_hi3630_core_write(pmic, sreg->register_info.top_cssys_reg, reg_val);
}

void regulator_address_info_list(void)
{
	if (g_regu.crgperi_reg == NULL || g_regu.pmc_reg == NULL)
		return;

	pr_info("NOC TIMEOUT INTSTAT = 0x%x\n", readl(g_regu.pmc_reg + 0x3a4));
	pr_info("PERI INTO MASK = 0x%x\n", readl(g_regu.pmc_reg + 0x3a0));

	/*VCODE Regulator state list*/
	pr_info("VCODE Softreset addr[0x68] = value[0x%x]\n", readl(g_regu.crgperi_reg + 0x68));
	pr_info("VCODE DIV0 Clock addr[0xf4] = value[0x%x]\n", readl(g_regu.crgperi_reg + 0xf4));
	pr_info("VCODE DIV1 Clock addr[0xf0] = value[0x%x]\n", readl(g_regu.crgperi_reg + 0xf0));
	pr_info("VCODE Main0 Clock addr[0x08] = value[0x%x]\n", readl(g_regu.crgperi_reg + 0x08));
	pr_info("VCODE Main1 Clock addr[0x0c] = value[0x%x]\n", readl(g_regu.crgperi_reg + 0x0c));
	pr_info("VCODE POWER IDLEREQ addr[0x380]= 0x%x\n", readl(g_regu.pmc_reg + 0x380));
	pr_info("VCODE POWER IDLEACK addr[0x384]= 0x%x\n", readl(g_regu.pmc_reg + 0x384));
	pr_info("VCODE POWER IDLE addr[0x388]= 0x%x\n", readl(g_regu.pmc_reg + 0x388));

	/*VENC Regulator state list*/
	pr_info("VENC MTCMOS addr[0x158] = value[0x%x]\n", readl(g_regu.crgperi_reg + 0x158));
	pr_info("VENC MTCMOS addr[0x15C] = value[0x%x]\n", readl(g_regu.crgperi_reg + 0x15C));
	pr_info("VENC ISO addr[0x14C] = value[0x%x]\n", readl(g_regu.crgperi_reg + 0x14C));
	pr_info("VENC SoftReset addr[0x8C] = value[0x%x]\n", readl(g_regu.crgperi_reg + 0x8C));
	pr_info("VENC CLOCK addr[0x38] = value[0x%x]\n", readl(g_regu.crgperi_reg + 0x38));
	pr_info("VENC CLOCK addr[0x3C] = value[0x%x]\n", readl(g_regu.crgperi_reg + 0x3C));

	/*JPEG Regulator state list*/
	pr_info("JPEG SoftReset addr[0x80] = value[0x%x]\n", readl(g_regu.crgperi_reg + 0x80));
	pr_info("JPEG DIV Clock addr[0xF8] = value[0x%x]\n", readl(g_regu.crgperi_reg + 0xf8));
	pr_info("JPEG CLOCK addr[0x58] = value[0x%x]\n", readl(g_regu.crgperi_reg + 0x58));
	pr_info("JPEG CLOCK addr[0x5c] = value[0x%x]\n", readl(g_regu.crgperi_reg + 0x5c));
}

static struct clk *of_regulator_clk_get(struct device_node *node , int index)
{
	struct clk *clk;
	const char *clk_name;
	int ret = 0;
	ret = of_property_read_string_index(node, "clock-names", index, &clk_name);
	if (ret != 0) {
		pr_err("failed to get regulator clk resource! ret=%d.\n", ret);
		return ERR_PTR(-EINVAL);
	}
	clk = clk_get(NULL, clk_name);
	if (IS_ERR(clk)) {
		pr_err(" %s: %s %d get failed!\n", __func__ , clk_name, __LINE__);
		return ERR_PTR(-EINVAL);
	}

	return clk;
}

static int  hisi_hi3630_clock_step(struct hisi_regulator_hi3630 *sreg, int clock_num, int flag)
{
	struct clk *temp_clock;
	int i, ret = 0;
	for (i = 0; i < clock_num; i++) {
		temp_clock = of_regulator_clk_get(sreg->np, i);
		if (IS_ERR(temp_clock)) {
			temp_clock = NULL;
			return -EINVAL;
		}
		/*flag: 0 disable clock; 1 enable clock*/
		if (flag == REGULATOR_ENABLE) {
			ret = clk_prepare_enable(temp_clock);
			if (ret) {
				pr_err("Regulator hi3630:regulator_id[%d],clock_id[%d] enable failed\r\n", sreg->regulator_id, i);
				clk_put(temp_clock);
				return ret;
			}
		} else {
			clk_disable_unprepare(temp_clock);
			clk_put(temp_clock);
		}
	}
	return ret;
}

static int hisi_hi3630_clock_work(struct hisi_regulator_hi3630 *sreg, int flag)
{
	int ret = 0;
	if (sreg->regulator_id == VCODECSUBSYS_ID) {
		hisi_hi3630_clock_step(sreg, VCODECSUBSYS_CLOCK_NUM, flag);
	} else if (sreg->regulator_id == VPPSUBSYS_ID) {
		hisi_hi3630_clock_step(sreg, VPPSUBSYS_CLOCK_NUM, flag);
	} else if (sreg->regulator_id == VPPIP_ID) {
		hisi_hi3630_clock_step(sreg, VPPIP_CLOCK_NUM, flag);
	} else if (sreg->regulator_id == JPEGIP_ID) {
		hisi_hi3630_clock_step(sreg, JPEGIP_CLOCK_NUM, flag);
	} else if (sreg->regulator_id == VDEC_ID) {
		hisi_hi3630_clock_step(sreg, VDEC_CLOCK_NUM, flag);
	} else if (sreg->regulator_id == VENC_ID) {
		hisi_hi3630_clock_step(sreg, VENC_CLOCK_NUM, flag);
	} else if (sreg->regulator_id == DSSUBSYS_ID) {
		hisi_hi3630_clock_step(sreg, DSSUBSYS_CLOCK_NUM, flag);
	} else if (sreg->regulator_id == ISPSUBSYS_ID) {
		if (flag == REGULATOR_ENABLE) {
			hisi_hi3630_clock_step(sreg, ISPSUBSYS_CLOCK_NUM1, flag);
		} else {
			hisi_hi3630_clock_step(sreg, ISPSUBSYS_CLOCK_NUM2, flag);
		}
	} else if (sreg->regulator_id == SDP_ID) {
		if (flag == REGULATOR_ENABLE) {
			/*enable clock:hdmi,hdmiefc,ldi1*/
			hisi_hi3630_clock_step(sreg, SDP_CLOCK_NUM, flag);
			/*open clock[DSS]*/
			hisi_dss_regu_write(DSS_SDP_CLK_OPEN_VALUE, DSS_EN_CLK_CFG);
		} else {
			/*close clock[DSS]*/
			hisi_dss_regu_write(DSS_SDP_CLK_CLOSE_VALUE, DSS_DIS_CLK_CFG);
			hisi_hi3630_clock_step(sreg, SDP_CLOCK_NUM, flag);
		}
	} else if (sreg->regulator_id == ADP_ID) {
		if (flag == REGULATOR_ENABLE) {
			hisi_hi3630_clock_step(sreg, ADP_CLOCK_NUM, flag);
			/*enable clock[DSS]*/
			hisi_dss_regu_write(DSS_ADP_CLK_OPEN_VALUE, DSS_EN_CLK_CFG);
		} else {
			/*close clock[DSS]*/
			hisi_dss_regu_write(DSS_ADP_CLK_CLOSE_VALUE, DSS_DIS_CLK_CFG);
			hisi_hi3630_clock_step(sreg, ADP_CLOCK_NUM, flag);
		}
	} else if (sreg->regulator_id == SEC_P_ID) {
		hisi_hi3630_clock_step(sreg, SEC_P_CLOCK_NUM, flag);
	} else if (sreg->regulator_id == USB2OTG_ID) {
		hisi_hi3630_clock_step(sreg, USB2OTG_CLOCK_NUM, flag);
	} else if (sreg->regulator_id == HSIC_ID) {
		hisi_hi3630_clock_step(sreg, HSIC_CLOCK_NUM, flag);
	} else {
		/*bypass*/
	}
	return ret;
}

#define CHANGE_FUNC_TYPE  void (*) (struct hisi_regulator_hi3630_core *, struct hisi_regulator_hi3630 *)

static void(*g_enable_func[])(struct hisi_regulator_hi3630_core *, struct hisi_regulator_hi3630 *) = {
	[MTCMOS_FLAG]		= enable_mtcmos,
	[SOFTRESET_FLAG]	= disable_softrst,
	[ISO_FLAG]			= disable_iso,
	[TOPCSSYS_FLAG]			= set_top_cssys_softreset_off,
	[SYSCTRL_FLAG]		= (CHANGE_FUNC_TYPE) set_sctrl_memory,
	[NOCIDLEMODE_FLAG]	= (CHANGE_FUNC_TYPE) set_noc_exit_idle_mode,
};
static void(*g_disable_func[])(struct hisi_regulator_hi3630_core *, struct hisi_regulator_hi3630 *) = {
	[MTCMOS_FLAG]		= disable_mtcmos,
	[SOFTRESET_FLAG]	= enable_softrst,
	[ISO_FLAG]			= enable_iso,
	[NOCIDLEMODE_FLAG]	= (CHANGE_FUNC_TYPE) set_noc_enter_idle_mode,
};

void qos_noc_register_init(struct hisi_regulator_hi3630_core *pmic, struct hisi_regulator_hi3630 *sreg)
{
	int  regulator_id;
	void __iomem *base_qos_reg = NULL;

	regulator_id = sreg->regulator_id;
	if (sreg->qos_noc_info.qos_noc_addr_flag == VPP_QOS_ADDR_FLAG) {
		base_qos_reg = g_regu.vpp_qos_reg;
	} else if (sreg->qos_noc_info.qos_noc_addr_flag == VDEC_QOS_ADDR_FLAG) {
		base_qos_reg = g_regu.vdec_qos_reg;
	} else if (sreg->qos_noc_info.qos_noc_addr_flag == VENC_QOS_ADDR_FLAG) {
		base_qos_reg = g_regu.venc_qos_reg;
	} else if (sreg->qos_noc_info.qos_noc_addr_flag == DSS_QOS_ADDR_FLAG) {
		base_qos_reg = g_regu.dss_qos_reg;
	} else if (sreg->qos_noc_info.qos_noc_addr_flag == ISP_QOS_ADDR_FLAG) {
		base_qos_reg = g_regu.isp_qos_reg;
	} else {
		pr_err("%s: base_qos_reg get failled!\n", __func__);
		return ;
	}

	if (regulator_id == VPPIP_ID || regulator_id == JPEGIP_ID ||
			regulator_id == VDEC_ID || regulator_id == VENC_ID) {
		writel(sreg->qos_noc_info.qos_priority_mask, base_qos_reg + sreg->qos_noc_info.qos_priority_reg);
		writel(sreg->qos_noc_info.qos_mode_mask, base_qos_reg + sreg->qos_noc_info.qos_mode_reg);
	} else if (regulator_id == DSSUBSYS_ID) {
		writel(sreg->qos_noc_info.qos_dss0_rd_mask, base_qos_reg + sreg->qos_noc_info.qos_dss0_rd_reg);
		writel(sreg->qos_noc_info.qos_dss0_wr_mask, base_qos_reg + sreg->qos_noc_info.qos_dss0_wr_reg);
		writel(sreg->qos_noc_info.qos_dss1_rd_mask, base_qos_reg + sreg->qos_noc_info.qos_dss1_rd_reg);
		writel(sreg->qos_noc_info.qos_dss1_wr_mask, base_qos_reg + sreg->qos_noc_info.qos_dss1_wr_reg);
	} else if (regulator_id == ISPSUBSYS_ID) {
		writel(sreg->qos_noc_info.qos_isp1_rd_mask, base_qos_reg + sreg->qos_noc_info.qos_isp1_rd_reg);
		writel(sreg->qos_noc_info.qos_isp1_wr_mask, base_qos_reg + sreg->qos_noc_info.qos_isp1_wr_reg);
		writel(sreg->qos_noc_info.qos_isp2_rd_mask, base_qos_reg + sreg->qos_noc_info.qos_isp2_rd_reg);
		writel(sreg->qos_noc_info.qos_isp2_wr_mask, base_qos_reg + sreg->qos_noc_info.qos_isp2_wr_reg);
		writel(sreg->qos_noc_info.qos_isp3_rd_mask, base_qos_reg + sreg->qos_noc_info.qos_isp3_rd_reg);
		writel(sreg->qos_noc_info.qos_isp3_wr_mask, base_qos_reg + sreg->qos_noc_info.qos_isp3_wr_reg);
		writel(sreg->qos_noc_info.qos_isp4_rd_mask, base_qos_reg + sreg->qos_noc_info.qos_isp4_rd_reg);
	} else {
		/*bypass*/
	}

}
/*******************************regulator interface********************************/
static int hisi_hi3630_is_enabled(struct regulator_dev *dev)
{
	struct hisi_regulator_hi3630 *sreg = rdev_get_drvdata(dev);
	struct hisi_regulator_hi3630_core *pmic = rdev_to_hi3630_core(dev);
	if (sreg == NULL || pmic == NULL) {
		pr_err("[%s]regulator get  para is err!\n", __func__);
		return -EINVAL;
	}

	return sreg->regulator_enalbe_flag;
}

static int hisi_ispsubsys_clock_control(struct hisi_regulator_hi3630 *sreg)
{
	struct clk *temp_clock;
	int ret;

	if (sreg->regulator_id == ISPSUBSYS_ID) {
		temp_clock = of_regulator_clk_get(sreg->np, 5);/*5:clk_gate_dphy1*/
		if (IS_ERR(temp_clock)) {
			temp_clock = NULL;
			mutex_unlock(&sreg->regulator_lock);
			return -EINVAL;
		}
		ret = clk_prepare_enable(temp_clock);
		if (ret) {
			pr_err("Regulator hi3630:clock[%d] enable failed\r\n", sreg->regulator_id);
		}
	}

	return 0;
}

static char* dss_clock_name[4] = {"pclk_dss", "aclk_dss", "clk_edc0", "clk_ldi0"};
static int hisi_dsssubsys_clock_control(int state_flag)
{
	struct clk *temp_clock;
	int ret = 0, i = 0;

	if (REGULATOR_ENABLE == state_flag){
		for (i = 0; i < (sizeof(dss_clock_name)/sizeof(char*)); i++){
			temp_clock = clk_get(NULL, dss_clock_name[i]);
			if (IS_ERR(temp_clock)) {
				temp_clock = NULL;
				return -EINVAL;
			}
			ret = clk_prepare_enable(temp_clock);
			if (ret) {
				pr_err("Regulator hi3630:clock[pclk_dss] enable failed\r\n");
			}
		}
	} else if (REGULATOR_DISABLE == state_flag) {
		for (i = 0; i < (sizeof(dss_clock_name)/sizeof(char*)); i++){
			temp_clock = clk_get(NULL, dss_clock_name[i]);
			if (IS_ERR(temp_clock)) {
				temp_clock = NULL;
				return -EINVAL;
			}
			clk_disable_unprepare(temp_clock);
			clk_put(temp_clock);
		}
	} else {
		/*bypass*/
	}

	return ret;
}
static int hisi_hi3630_regulator_enabled(struct regulator_dev *dev)
{
	int i, ret;
	struct hisi_regulator_hi3630 *sreg = rdev_get_drvdata(dev);
	struct hisi_regulator_hi3630_core *pmic = rdev_to_hi3630_core(dev);
	if (sreg == NULL || pmic == NULL) {
		pr_err("[%s]regulator get  para is err!\n", __func__);
		return -EINVAL;
	}

	mutex_lock(&sreg->regulator_lock);

	if (sreg->regulator_id == ISPSUBSYS_ID || sreg->regulator_id == SDP_ID || sreg->regulator_id == ADP_ID) {
		ret = hisi_dsssubsys_clock_control(REGULATOR_ENABLE);
		if (ret) {
			pr_err("%s:Regulator dsssubsys control clock fail!\n", __func__);
			mutex_unlock(&sreg->regulator_lock);
			return ret;
		}
	}

	for (i = 0; i < WORK_STEP_NUM; i++) {
		if (sreg->g_enable_work_info[0][i] == 0)
			break;

		if (TOPCSSYS_FLAG >= sreg->g_enable_work_info[0][i]) {
			g_enable_func[sreg->g_enable_work_info[0][i]](pmic, sreg);
			if (0 != sreg->g_enable_work_info[1][i])
				udelay(sreg->g_enable_work_info[1][i]);
		}

		if (sreg->g_enable_work_info[0][i] == SYSCTRL_FLAG) {
			if (sreg->regulator_id == VDEC_ID) {
				((void (*) (unsigned int)) g_enable_func[SYSCTRL_FLAG])(VDEC_REPAIR_DISABLE);
			} else if (sreg->regulator_id == VENC_ID) {
				((void (*) (unsigned int)) g_enable_func[SYSCTRL_FLAG])(VENC_REPAIR_DISABLE);
			} else if (sreg->regulator_id == DSSUBSYS_ID) {
				((void (*) (unsigned int)) g_enable_func[SYSCTRL_FLAG])(DSS_SUBSYS_REPAIR_DISABLE);
			} else if (sreg->regulator_id == ISPSUBSYS_ID) {
				((void (*) (unsigned int)) g_enable_func[SYSCTRL_FLAG])(ISP_REPAIR_DISABLE);
			} else if (sreg->regulator_id == SDP_ID) {
				((void (*) (unsigned int)) g_enable_func[SYSCTRL_FLAG])(SDP_REPAIR_DISABLE);
			} else {
				/*bypass*/
			}
			if (0 != sreg->g_enable_work_info[1][i])
				udelay(sreg->g_enable_work_info[1][i]);
		}

		if (sreg->g_enable_work_info[0][i] == NOCIDLEMODE_FLAG) {
			if (sreg->regulator_id == VCODECSUBSYS_ID) {
				((void (*) (unsigned int)) g_enable_func[NOCIDLEMODE_FLAG])(PMC_NOC_CODECSUBSYS_IDLE);
			} else if (sreg->regulator_id == VPPSUBSYS_ID) {
				((void (*) (unsigned int)) g_enable_func[NOCIDLEMODE_FLAG])(PMC_NOC_VPPSUBSYS_IDLE);
			} else if (sreg->regulator_id == VDEC_ID) {
				((void (*) (unsigned int)) g_enable_func[NOCIDLEMODE_FLAG])(PMC_NOC_VDEC_IDLE);
			} else if (sreg->regulator_id == VENC_ID) {
				((void (*) (unsigned int)) g_enable_func[NOCIDLEMODE_FLAG])(PMC_NOC_VENC_IDLE);
			} else if (sreg->regulator_id == DSSUBSYS_ID) {
				((void (*) (unsigned int)) g_enable_func[NOCIDLEMODE_FLAG])(PMC_NOC_DSSSUBSYS_IDLE);
			} else if (sreg->regulator_id == ISPSUBSYS_ID) {
				((void (*) (unsigned int)) g_enable_func[NOCIDLEMODE_FLAG])(PMC_NOC_ISPSUBSYS_IDLE);
			} else {
				/*bypass*/
			}
			if (0 != sreg->g_enable_work_info[1][i])
				udelay(sreg->g_enable_work_info[1][i]);
		}

		if (sreg->g_enable_work_info[0][i] == CLOCK_FLAG) {
			hisi_hi3630_clock_work(sreg, REGULATOR_ENABLE);
			if (0 != sreg->g_enable_work_info[1][i])
				udelay(sreg->g_enable_work_info[1][i]);
		}

		if (sreg->g_enable_work_info[0][i] == RECLOCK_FLAG) {
			hisi_hi3630_clock_work(sreg, REGULATOR_ENABLE);
			if (0 != sreg->g_disable_work_info[1][i])
				udelay(sreg->g_disable_work_info[1][i]);

			ret = hisi_ispsubsys_clock_control(sreg);
			if (ret) {
				pr_err("Regulator ispsubsys control clock fail!\n");
				return ret;
			}

			hisi_hi3630_clock_work(sreg, REGULATOR_DISABLE);
			if (0 != sreg->g_disable_work_info[1][i])
				udelay(sreg->g_disable_work_info[1][i]);
		}
	}

	ret = hisi_ispsubsys_clock_control(sreg);
	if (ret) {
		pr_err("Regulator ispsubsys control clock fail!\n");
		mutex_unlock(&sreg->regulator_lock);
		return ret;
	}

#ifdef CONFIG_HISI_NOC_ERR_PROBE
	/* enable errprobe  */
	if (sreg->noc_node_name)
			enable_err_probe_by_name(sreg->noc_node_name);
#endif

	if (sreg->regulator_id == VPPIP_ID || sreg->regulator_id == JPEGIP_ID ||
			sreg->regulator_id == VDEC_ID || sreg->regulator_id == VENC_ID ||
			sreg->regulator_id == DSSUBSYS_ID || sreg->regulator_id == ISPSUBSYS_ID) {
		qos_noc_register_init(pmic, sreg);
	}

	sreg->regulator_enalbe_flag = 1;
	mutex_unlock(&sreg->regulator_lock);

	return 0;
}

static int hisi_hi3630_regulator_disabled(struct regulator_dev *dev)
{
	int  i;
	struct hisi_regulator_hi3630 *sreg = rdev_get_drvdata(dev);
	struct hisi_regulator_hi3630_core *pmic = rdev_to_hi3630_core(dev);
	if (sreg == NULL || pmic == NULL) {
		pr_err("[%s]regulator get  para is err!\n", __func__);
		return -EINVAL;
	}

	mutex_lock(&sreg->regulator_lock);

#ifdef CONFIG_HISI_NOC_ERR_PROBE
	/* disable noc errprobe */
	if (sreg->noc_node_name)
			disable_err_probe_by_name(sreg->noc_node_name);
#endif

	for (i = 0; i < WORK_STEP_NUM; i++) {
		if (sreg->g_disable_work_info[0][i] == 0)
			break;

		if (ISO_FLAG >= sreg->g_disable_work_info[0][i]) {
			g_disable_func[sreg->g_disable_work_info[0][i]](pmic, sreg);
			if (0 != sreg->g_disable_work_info[1][i])
				udelay(sreg->g_disable_work_info[1][i]);
		}

		if (sreg->g_disable_work_info[0][i] == NOCIDLEMODE_FLAG) {
			if (sreg->regulator_id == VCODECSUBSYS_ID) {
				((void (*) (unsigned int)) g_disable_func[NOCIDLEMODE_FLAG])(PMC_NOC_CODECSUBSYS_IDLE);
			} else if (sreg->regulator_id == VPPSUBSYS_ID) {
				((void (*) (unsigned int)) g_disable_func[NOCIDLEMODE_FLAG])(PMC_NOC_VPPSUBSYS_IDLE);
			} else if (sreg->regulator_id == VDEC_ID) {
				((void (*) (unsigned int)) g_disable_func[NOCIDLEMODE_FLAG])(PMC_NOC_VDEC_IDLE);
			} else if (sreg->regulator_id == VENC_ID) {
				((void (*) (unsigned int)) g_disable_func[NOCIDLEMODE_FLAG])(PMC_NOC_VENC_IDLE);
			} else if (sreg->regulator_id == DSSUBSYS_ID) {
				((void (*) (unsigned int)) g_disable_func[NOCIDLEMODE_FLAG])(PMC_NOC_DSSSUBSYS_IDLE);
			} else if (sreg->regulator_id == ISPSUBSYS_ID) {
				((void (*) (unsigned int)) g_disable_func[NOCIDLEMODE_FLAG])(PMC_NOC_ISPSUBSYS_IDLE);
			} else {
				/*bypass*/
			}
			if (0 != sreg->g_disable_work_info[1][i])
				udelay(sreg->g_disable_work_info[1][i]);
		}

		if (sreg->g_disable_work_info[0][i] == CLOCK_FLAG) {
			hisi_hi3630_clock_work(sreg, REGULATOR_DISABLE);
			if (0 != sreg->g_disable_work_info[1][i])
				udelay(sreg->g_disable_work_info[1][i]);
		}
	}

	if (sreg->regulator_id == ISPSUBSYS_ID || sreg->regulator_id == SDP_ID || sreg->regulator_id == ADP_ID) {
		hisi_dsssubsys_clock_control(REGULATOR_DISABLE);
	}

	sreg->regulator_enalbe_flag = 0;
	mutex_unlock(&sreg->regulator_lock);
	return 0;
}

/**************************remote processor control****************************************/
#ifdef CONFIG_HI3XXX_RPROC
static int hisi_hi3630_regulator_cmd_send(struct regulator_dev *dev, int cmd)
{
	struct hisi_regulator_hi3630 *sreg = rdev_get_drvdata(dev);
	rproc_msg_t ack_buffer[2] = {0};
	rproc_msg_t *tx_buffer = NULL;
	int err = 0;

	if (sreg == NULL) {
		pr_err("[%s]regulator get  para is err!\n", __func__);
		return -EINVAL;
	}

	if (REGULATOR_ENABLE == cmd)
		tx_buffer = sreg->lpm3_enable_value;
	else
		tx_buffer = sreg->lpm3_disable_value;

	err = RPROC_SYNC_SEND(HISI_RPROC_LPM3_FOR_REGULATOR, tx_buffer,
		AP_TO_LPM3_MSG_NUM, SYNC_MSG, ack_buffer, AP_TO_LPM3_MSG_NUM);
	if (err || (ack_buffer[0] != tx_buffer[0]) || (ack_buffer[1] != 0x0)) {
		pr_err("%s: regulator ID[%d] rproc sync send err!\n",
						__func__, sreg->regulator_id);
		return err;
	}

	return 0;
}

/**************************g3d****************************************/
static int hisi_hi3630_to_lpm3_is_enabled(struct regulator_dev *dev)
{
	struct hisi_regulator_hi3630 *sreg = rdev_get_drvdata(dev);
	struct hisi_regulator_hi3630_core *pmic = rdev_to_hi3630_core(dev);

	if (sreg == NULL || pmic == NULL) {
		pr_err("[%s]regulator get  para is err!\n", __func__);
		return -EINVAL;
	}

	return sreg->regulator_enalbe_flag;
}

static int hisi_hi3630_lpm3_enabled(struct regulator_dev *dev)
{
	struct hisi_regulator_hi3630 *sreg = rdev_get_drvdata(dev);
	int ret = 0;
	ret = hisi_hi3630_regulator_cmd_send(dev, REGULATOR_ENABLE);
	if (!ret) {
		sreg->regulator_enalbe_flag = 1;
	} else {
		pr_err("%s:hisi hi3630 send enable ldo[%s] to lpm3 failled!\n\r", __func__, sreg->name);
	}
#ifdef CONFIG_HISI_NOC_ERR_PROBE
	/* enable noc errprobe */
	if (sreg->noc_node_name)
		enable_err_probe_by_name(sreg->noc_node_name);
#endif
	return ret;
}

static int hisi_hi3630_lpm3_disabled(struct regulator_dev *dev)
{
	struct hisi_regulator_hi3630 *sreg = rdev_get_drvdata(dev);
	int ret = 0;

#ifdef CONFIG_HISI_NOC_ERR_PROBE
	/* disable noc errprobe */
	if (sreg->noc_node_name)
			disable_err_probe_by_name(sreg->noc_node_name);
#endif

	ret = hisi_hi3630_regulator_cmd_send(dev, REGULATOR_DISABLE);
	if (!ret || (-ETIMEOUT == ret)) {
		sreg->regulator_enalbe_flag = 0;
		ret = 0;
	} else {
		pr_err("%s:hisi hi3630 send disable ldo[%s] to lpm3 failled!\n\r", __func__, sreg->name);
	}
	return ret;
}
#else
static int hisi_hi3630_to_lpm3_is_enabled(struct regulator_dev *dev)
{
	return 0;
}

static int hisi_hi3630_lpm3_enabled(struct regulator_dev *dev)
{
	return 0;
}
static int hisi_hi3630_lpm3_disabled(struct regulator_dev *dev)
{
	return 0;
}
#endif
/******************************resolve hi3630 dtsv interface************************/
static int hisi_mtcmos_dt_parse_common(struct hisi_regulator_hi3630 *sreg,
					struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	unsigned int register_info[3] = {0};
	int ret = 0;
	/* parse .hisi mtcmos state.ctrl_reg */
	ret = of_property_read_u32_array(np, "hisilicon,hisi-mtcmos-state-ctrl",
						register_info, 3);
	if (ret) {
		dev_err(dev, "no hisilicon,hisi mtcmos state property set\n");
		goto dt_parse_common_end;
	}
	sreg->register_info.mtcmos_state_reg = register_info[0];
	sreg->register_info.mtcmos_state_mask = register_info[1];
	sreg->register_info.mtcmos_state_addr = register_info[2];

	/* parse .hisi mtcmos enable.ctrl_reg */
	ret = of_property_read_u32_array(np, "hisilicon,hisi-mtcmos-en-ctrl",
						register_info, 3);
	if (ret) {
		dev_err(dev, "no hisilicon,hisi mtcmos enable property set\n");
		goto dt_parse_common_end;
	}
	sreg->register_info.mtcmos_en_reg = register_info[0];
	sreg->register_info.mtcmos_en_mask = register_info[1];
	sreg->register_info.mtcmos_en_addr = register_info[2];
	return 0;
dt_parse_common_end:
	return ret;

}

static int hisi_swreset_dt_parse_common(struct hisi_regulator_hi3630 *sreg,
					struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	unsigned int register_info[3] = {0};
	int ret = 0;

	/* parse .hisi software reset enable.ctrl_reg */
	ret = of_property_read_u32_array(np, "hisilicon,hisi-sw-reset-en-ctrl",
						register_info, 3);
	if (ret) {
		dev_err(dev, "no hisilicon,hisi software reset enable property set\n");
		goto dt_parse_common_end;
	}
	sreg->register_info.swrest_en_reg = register_info[0];
	sreg->register_info.swrest_en_mask = register_info[1];
	sreg->register_info.swrest_en_addr = register_info[2];
	return 0;
dt_parse_common_end:
	return ret;
}
static int hisi_isocontrol_dt_parse_common(struct hisi_regulator_hi3630 *sreg,
					struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	unsigned int register_info[3] = {0};
	int ret = 0;

	/* parse .hisi iso enable.ctrl_reg */
	ret = of_property_read_u32_array(np, "hisilicon,hisi-iso-en-ctrl",
						register_info, 3);
	if (ret) {
		dev_err(dev, "no hisilicon,hisi iso enable property set\n");
		goto dt_parse_common_end;
	}
	sreg->register_info.iso_en_reg = register_info[0];
	sreg->register_info.iso_en_mask = register_info[1];
	sreg->register_info.iso_en_addr = register_info[2];
	return 0;
dt_parse_common_end:
	return ret;
}
static int hisi_topcssyscontrol_dt_parse_common(struct hisi_regulator_hi3630 *sreg,
					struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	unsigned int register_info[3] = {0};
	int ret = 0;

	/* parse .hisi top cssys ctrl_reg */
	ret = of_property_read_u32_array(np, "hisilicon,hisi-top-cssys-ctrl",
						register_info, 3);
	if (ret) {
		dev_err(dev, "no hisilicon,hisi iso disable property set\n");
		goto dt_parse_common_end;
	}
	sreg->register_info.top_cssys_reg = register_info[0];
	sreg->register_info.top_cssys_mask = register_info[1];
	sreg->register_info.top_cssys_addr = register_info[2];

	return 0;
dt_parse_common_end:
	return ret;
}

static int hisi_regulatorid_dt_parse_common(struct hisi_regulator_hi3630 *sreg,
					struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	int id = 0;
	int ret = 0;

	/* regulator-id */
	ret = of_property_read_u32_array(np, "hisilicon,hisi-regulator-id",
						&id, 1);
	if (ret) {
		dev_err(dev, "no hisilicon,hisi-regulator-id property set\n");
		goto dt_parse_common_end;
	}
	sreg->regulator_id = id;

	return 0;
dt_parse_common_end:
	return ret;
}

#ifdef CONFIG_HISI_NOC_ERR_PROBE
static int hisi_noc_dt_parse(struct hisi_regulator_hi3630 *sreg,
				struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	int ret = 0;

	/* noc_node_name */
	ret = of_property_read_string(np, "hisi,noc-node-name", &sreg->noc_node_name);
	if (ret) {
		pr_debug("no hisi,noc-node-name property found\n");
	}

	return ret;
}
#endif

static int(*g_dts_func[])(struct hisi_regulator_hi3630 *, struct platform_device *) = {
	[MTCMOST_DTS]	= hisi_mtcmos_dt_parse_common,
	[SOTFREST_DTS]	= hisi_swreset_dt_parse_common,
	[ISO_DTS]		= hisi_isocontrol_dt_parse_common,
	[TOPCSSYS_DTS]	= hisi_topcssyscontrol_dt_parse_common,
};

/************************* dts solve***************************************/
int get_qos_noc_para(struct hisi_regulator_hi3630 *sreg,
				struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	unsigned int register_info[3] = {0};
	int ret = 0;
	int  regulator_id;
	regulator_id = sreg->regulator_id;

	/* parse .hisi qos noc state.ctrl_reg */
	if (regulator_id == VPPIP_ID || regulator_id == JPEGIP_ID ||
			regulator_id == VDEC_ID || regulator_id == VENC_ID) {
		ret = of_property_read_u32_array(np, "hisilicon,hisi-qos-priority-ctrl",
							register_info, 3);
		if (ret) {
			dev_err(dev, "no hisilicon,hisi qos noc priority set\n");
			goto dt_parse_common_end;
		}
		sreg->qos_noc_info.qos_priority_reg = register_info[0];
		sreg->qos_noc_info.qos_priority_mask = register_info[1];
		sreg->qos_noc_info.qos_noc_addr_flag = register_info[2];

		ret = of_property_read_u32_array(np, "hisilicon,hisi-qos-mode-ctrl",
							register_info, 3);
		if (ret) {
			dev_err(dev, "no hisilicon,hisi qos noc mode set\n");
			goto dt_parse_common_end;
		}
		sreg->qos_noc_info.qos_mode_reg = register_info[0];
		sreg->qos_noc_info.qos_mode_mask = register_info[1];
	} else if (regulator_id == DSSUBSYS_ID) {
		ret = of_property_read_u32_array(np, "hisilicon,hisi-qos-dss0-rd-ctrl",
							register_info, 3);
		if (ret) {
			dev_err(dev, "no hisilicon,hisi qos dss0  rd  set\n");
			goto dt_parse_common_end;
		}
		sreg->qos_noc_info.qos_dss0_rd_reg = register_info[0];
		sreg->qos_noc_info.qos_dss0_rd_mask = register_info[1];
		sreg->qos_noc_info.qos_noc_addr_flag = register_info[2];

		ret = of_property_read_u32_array(np, "hisilicon,hisi-qos-dss0-wr-ctrl",
							register_info, 3);
		if (ret) {
			dev_err(dev, "no hisilicon,hisi qos dss0  wr  set\n");
			goto dt_parse_common_end;
		}
		sreg->qos_noc_info.qos_dss0_wr_reg = register_info[0];
		sreg->qos_noc_info.qos_dss0_wr_mask = register_info[1];

		ret = of_property_read_u32_array(np, "hisilicon,hisi-qos-dss1-rd-ctrl",
							register_info, 3);
		if (ret) {
			dev_err(dev, "no hisilicon,hisi qos dss1 rd set\n");
			goto dt_parse_common_end;
		}
		sreg->qos_noc_info.qos_dss1_rd_reg = register_info[0];
		sreg->qos_noc_info.qos_dss1_rd_mask = register_info[1];

		ret = of_property_read_u32_array(np, "hisilicon,hisi-qos-dss1-wr-ctrl",
							register_info, 3);
		if (ret) {
			dev_err(dev, "no hisilicon,hisi qos dss1 wr set\n");
			goto dt_parse_common_end;
		}
		sreg->qos_noc_info.qos_dss1_wr_reg = register_info[0];
		sreg->qos_noc_info.qos_dss1_wr_mask = register_info[1];
	} else if (regulator_id == ISPSUBSYS_ID) {
		ret = of_property_read_u32_array(np, "hisilicon,hisi-qos-isp1-rd-ctrl",
							register_info, 3);
		if (ret) {
			dev_err(dev, "no hisilicon,hisi qos isp1 rd set\n");
			goto dt_parse_common_end;
		}
		sreg->qos_noc_info.qos_isp1_rd_reg = register_info[0];
		sreg->qos_noc_info.qos_isp1_rd_mask = register_info[1];
		sreg->qos_noc_info.qos_noc_addr_flag = register_info[2];
		ret = of_property_read_u32_array(np, "hisilicon,hisi-qos-isp1-wr-ctrl",
							register_info, 3);
		if (ret) {
			dev_err(dev, "no hisilicon,hisi qos isp1 wr set\n");
			goto dt_parse_common_end;
		}
		sreg->qos_noc_info.qos_isp1_wr_reg = register_info[0];
		sreg->qos_noc_info.qos_isp1_wr_mask = register_info[1];

		ret = of_property_read_u32_array(np, "hisilicon,hisi-qos-isp2-rd-ctrl",
							register_info, 3);
		if (ret) {
			dev_err(dev, "no hisilicon,hisi qos isp2 rd set\n");
			goto dt_parse_common_end;
		}
		sreg->qos_noc_info.qos_isp2_rd_reg = register_info[0];
		sreg->qos_noc_info.qos_isp2_rd_mask = register_info[1];
		ret = of_property_read_u32_array(np, "hisilicon,hisi-qos-isp2-wr-ctrl",
							register_info, 3);
		if (ret) {
			dev_err(dev, "no hisilicon,hisi qos isp2 wr set\n");
			goto dt_parse_common_end;
		}
		sreg->qos_noc_info.qos_isp2_wr_reg = register_info[0];
		sreg->qos_noc_info.qos_isp2_wr_mask = register_info[1];

		ret = of_property_read_u32_array(np, "hisilicon,hisi-qos-isp3-rd-ctrl",
							register_info, 3);
		if (ret) {
			dev_err(dev, "no hisilicon,hisi qos isp3 rd set\n");
			goto dt_parse_common_end;
		}
		sreg->qos_noc_info.qos_isp3_rd_reg = register_info[0];
		sreg->qos_noc_info.qos_isp3_rd_mask = register_info[1];
		ret = of_property_read_u32_array(np, "hisilicon,hisi-qos-isp3-wr-ctrl",
							register_info, 3);
		if (ret) {
			dev_err(dev, "no hisilicon,hisi qos isp3 wr set\n");
			goto dt_parse_common_end;
		}
		sreg->qos_noc_info.qos_isp3_wr_reg = register_info[0];
		sreg->qos_noc_info.qos_isp3_wr_mask = register_info[1];

		ret = of_property_read_u32_array(np, "hisilicon,hisi-qos-isp4-rd-ctrl",
							register_info, 3);
		if (ret) {
			dev_err(dev, "no hisilicon,hisi qos isp4 rd set\n");
			goto dt_parse_common_end;
		}
		sreg->qos_noc_info.qos_isp4_rd_reg = register_info[0];
		sreg->qos_noc_info.qos_isp4_rd_mask = register_info[1];
	} else {
		/*bypass*/
	}
	return 0;
dt_parse_common_end:
	return ret;
}

static int hisi_regualtor_dt_parse_common(struct hisi_regulator_hi3630 *sreg,
				struct platform_device *pdev)
{
	struct device *dev = NULL;
	struct device_node *np = NULL;
	unsigned int register_info0[WORK_STEP_NUM] = {0};
	unsigned int register_info1[WORK_STEP_NUM] = {0};
	int i;
	int ret = 0;
	if (sreg == NULL || pdev == NULL) {
		pr_err("[%s]regulator get dt para is err!\n", __func__);
		return -EINVAL;
	}

	dev = &pdev->dev;
	np = dev->of_node;

	/* regulator-id */
	ret = hisi_regulatorid_dt_parse_common(sreg, pdev);
	if (ret) {
		pr_err("[%s] regulatorid get error!\n", __func__);
		goto dt_parse_common_end;
	}

	ret = of_property_read_u32_array(np, "hi3630_regulator_operation_enable_step",
						register_info0, WORK_STEP_NUM);
	if (ret) {
		dev_err(dev, "no hi3630 regulator operation enable step\n");
		goto dt_parse_common_end;
	}
	ret = of_property_read_u32_array(np, "hi3630_regulator_enable_delay_time",
						register_info1, WORK_STEP_NUM);
	if (ret) {
		dev_err(dev, "no hi3630 regulator enable delay time\n");
		goto dt_parse_common_end;
	}

	for (i = 0; i < WORK_STEP_NUM; i++) {
		sreg->g_enable_work_info[0][i] = register_info0[i];
		sreg->g_enable_work_info[1][i] = register_info1[i];
	}

	for (i = 0; i < WORK_STEP_NUM; i++) {
		if (sreg->g_enable_work_info[0][i] == CLOCK_FLAG ||/*clock*/
					sreg->g_enable_work_info[0][i] == SYSCTRL_FLAG ||/*sysctrl*/
					sreg->g_enable_work_info[0][i] == NOCIDLEMODE_FLAG ||/*nocidlemode*/
					sreg->g_enable_work_info[0][i] == RECLOCK_FLAG)/*repeat clock*/
			continue;

		if (sreg->g_enable_work_info[0][i] != 0) {
			ret = g_dts_func[sreg->g_enable_work_info[0][i]](sreg, pdev);
			if (ret) {
				pr_err("[%s] mtcmos control error!\n", __func__);
				goto dt_parse_common_end;
			}
		} else {
			break;
		}
	}

	ret = of_property_read_u32_array(np, "hi3630_regulator_operation_disable_step",
						register_info0, WORK_STEP_NUM);
	if (ret) {
		dev_err(dev, "no hi3630 regulator operation disable step\n");
		goto dt_parse_common_end;
	}
	ret = of_property_read_u32_array(np, "hi3630_regulator_disable_delay_time",
						register_info1, WORK_STEP_NUM);
	if (ret) {
		dev_err(dev, "no hi3630 regulator disable delay time\n");
		goto dt_parse_common_end;
	}
	for (i = 0; i < WORK_STEP_NUM; i++) {
		sreg->g_disable_work_info[0][i] = register_info0[i];
		sreg->g_disable_work_info[1][i] = register_info1[i];
	}

#ifdef CONFIG_HISI_NOC_ERR_PROBE
	hisi_noc_dt_parse(sreg, pdev);
#endif
	/* QOS NOC*/
	get_qos_noc_para(sreg, pdev);
	return 0;

dt_parse_common_end:
	return ret;
}

static int hisi_dt_parse_hi3630_lpm3(struct hisi_regulator_hi3630 *sreg,
				struct platform_device *pdev)
{
	struct device *dev = NULL;
	struct device_node *np = NULL;
	int id = 0;
	unsigned int register_info[2] = {0};
	int ret = 0;

	if (sreg == NULL || pdev == NULL) {
		pr_err("[%s]regulator get  dt para is err!\n", __func__);
		return -EINVAL;
	}

	dev = &pdev->dev;
	np = dev->of_node;

	/* parse .hisi top cssys ctrl_reg */
	ret = of_property_read_u32_array(np, "hi3630_to_lpm3_enable_step",
						register_info, AP_TO_LPM3_MSG_NUM);
	if (ret) {
		dev_err(dev, "no hisilicon, hi3630 to lpm3 enable property set\n");
		goto dt_parse_common_end;
	}
	sreg->lpm3_enable_value[0] = register_info[0];
	sreg->lpm3_enable_value[1] = register_info[1];

	/* parse .hisi top cssys ctrl_reg */
	ret = of_property_read_u32_array(np, "hi3630_to_lpm3_disable_step",
						register_info, AP_TO_LPM3_MSG_NUM);
	if (ret) {
		dev_err(dev, "no hisilicon,hi3630 to lpm3 disable property set\n");
		goto dt_parse_common_end;
	}
	sreg->lpm3_disable_value[0] = register_info[0];
	sreg->lpm3_disable_value[1] = register_info[1];

	ret = of_property_read_u32_array(np, "hisilicon,hisi-regulator-id",
						&id, 1);
	if (ret) {
		dev_err(dev, "no hisilicon,hisi-regulator-id property set\n");
		goto dt_parse_common_end;
	}
	sreg->regulator_id = id;
#ifdef CONFIG_HISI_NOC_ERR_PROBE
	hisi_noc_dt_parse(sreg, pdev);
#endif
dt_parse_common_end:
	return ret;
}

/******************************hi3630 voltage region ops*************************/
struct regulator_ops hisi_hi3630_regulator_rops = {
	.is_enabled = hisi_hi3630_is_enabled,
	.enable = hisi_hi3630_regulator_enabled,
	.disable = hisi_hi3630_regulator_disabled,
};
struct regulator_ops hisi_hi3630_lpm3_rops = {
	.is_enabled = hisi_hi3630_to_lpm3_is_enabled,
	.enable = hisi_hi3630_lpm3_enabled,
	.disable = hisi_hi3630_lpm3_disabled,
};

/******************************hisi_regulator_hi3630******************************/
static const struct hisi_regulator_hi3630 hisi_regulator_hi3630_regulator = {
	.rdesc = {
		.ops = &hisi_hi3630_regulator_rops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	.dt_parse = hisi_regualtor_dt_parse_common,
};

static const struct hisi_regulator_hi3630 hisi_regulator_hi3630_lpm3 = {
	.rdesc = {
		.ops = &hisi_hi3630_lpm3_rops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	.dt_parse = hisi_dt_parse_hi3630_lpm3,
};

/*********************of_hisi_regulator_hi3630_match_tbl***************************/
static struct of_device_id of_hisi_regulator_hi3630_match_tbl[] = {
	{
		.compatible = "hi3630-regulator-type1",
		.data = &hisi_regulator_hi3630_regulator,
	},
	{
		.compatible = "hi3630-regulator-type2",
		.data = &hisi_regulator_hi3630_lpm3,
	},
	{ /* end */ }
};
#ifdef CONFIG_DEBUG_FS
static int dbg_control_vcc_show(struct seq_file *s, void *data)
{
	pr_info("dbg_control_hi3630_show \n\r");
	return 0;
}
static ssize_t dbg_control_vcc_set_value(struct file *filp, const char __user *buffer,
	size_t count, loff_t *ppos)
{
	char tmp[128] = {0};
	int index = 0;

	if (count > 128) {
		pr_info("error! buffer size big than internal buffer\n");
		return -EFAULT;
	}

	if (copy_from_user(tmp, buffer, count)) {
		pr_info("error!\n");
		return -EFAULT;
	}

	if (sscanf(tmp, "%d", &index)) {
		if (index == 0) {

		} else if (index == 1) {

		} else {
			pr_info("ERRR~\n");
		}
	} else {
		pr_info("ERRR~\n");
	}

	*ppos += count;

	return count;
}

static int dbg_control_vcc_open(struct inode *inode, struct file *file)
{
	file->private_data = inode->i_private;
	return single_open(file, dbg_control_vcc_show, &inode->i_private);
}

static const struct file_operations set_control_vcc_fops = {
	.open		= dbg_control_vcc_open,
	.read		= seq_read,
	.write		= dbg_control_vcc_set_value,
	.llseek		= seq_lseek,
	.release	= single_release,
};
#endif

static int hisi_regulator_hi3630_probe(struct platform_device *pdev)
{
	struct device *dev = NULL;
	struct device_node *np = NULL;
	struct regulator_desc *rdesc;
	struct regulator_dev *rdev;
	struct hisi_regulator_hi3630 *sreg = NULL;
	struct regulator_init_data *initdata;
	struct regulator_config config = { };
	const struct of_device_id *match;
	const struct hisi_regulator_hi3630 *temp = NULL;
	int ret = 0;
	size_t reg_size = (64*1024);
	static int regulator_flag;
	const char *supplyname = NULL;
#ifdef CONFIG_DEBUG_FS
	struct dentry *d;
#endif

	if (pdev == NULL) {
		pr_err("[%s]regulator get  platform device para is err!\n", __func__);
		return -EINVAL;
	}

	dev = &pdev->dev;
	np = dev->of_node;

	/* to check which type of regulator this is */
	match = of_match_device(of_hisi_regulator_hi3630_match_tbl, &pdev->dev);
	if (NULL == match) {
		pr_err("get hi6421 regulator fail!\n\r");
		return -EINVAL;
	}

	temp = match->data;

	initdata = of_get_regulator_init_data(dev, np);
	if (NULL == initdata) {
		pr_err("get regulator init data error !\n");
		return -EINVAL;
	}

	sreg = kmemdup(temp, sizeof(*sreg), GFP_KERNEL);
	if (!sreg)
		return -ENOMEM;

	sreg->name = initdata->constraints.name;
	rdesc = &sreg->rdesc;
	rdesc->name = sreg->name;

	supplyname = of_get_property(np, "hisilicon,supply_name", NULL);
	if (supplyname != NULL) {
		initdata->supply_regulator = supplyname;
	}

	memset(sreg->g_enable_work_info, 0, sizeof(sreg->g_enable_work_info));
	memset(sreg->g_disable_work_info, 0, sizeof(sreg->g_disable_work_info));

	mutex_init(&(sreg->regulator_lock));
	/* to parse device tree data for regulator specific */
	ret = sreg->dt_parse(sreg, pdev);
	if (ret) {
		dev_err(dev, "device tree parameter parse error!\n");
		goto hisi_hi3630_probe_end;
	}

	config.dev = &pdev->dev;
	config.init_data = initdata;
	config.driver_data = sreg;
	config.of_node = pdev->dev.of_node;
	sreg->np = np;
	sreg->regulator_enalbe_flag = 0;

	/* register regulator */
	rdev = regulator_register(rdesc, &config);
	if (IS_ERR(rdev)) {
		dev_err(dev, "failed to register %s\n",
			rdesc->name);
		ret = PTR_ERR(rdev);
		goto hisi_hi3630_probe_end;
	}

	if (regulator_flag == 0) {
		np = of_find_node_by_name(NULL, "crgctrl");
		BUG_ON(!np);
		g_regu.crgperi_reg = of_iomap(np, 0);
		BUG_ON(!g_regu.crgperi_reg);

		np = of_find_node_by_name(NULL, "pmctrl");
		BUG_ON(!np);
		g_regu.pmc_reg = of_iomap(np, 0);
		BUG_ON(!g_regu.pmc_reg);

		np = of_find_node_by_name(NULL, "dssctrl");
		BUG_ON(!np);
		g_regu.dss_reg = of_iomap(np, 0);
		BUG_ON(!g_regu.dss_reg);

		np = of_find_node_by_name(NULL, "sysctrl");
		BUG_ON(!np);
		g_regu.sctrl_reg = of_iomap(np, 0);
		BUG_ON(!g_regu.sctrl_reg);

		np = of_find_node_by_name(NULL, "pmic");
		BUG_ON(!np);
		g_regu.pmic_reg = of_iomap(np, 0);
		BUG_ON(!g_regu.pmic_reg);

		g_regu.vdec_qos_reg = devm_ioremap(&pdev->dev, VDEC_QOS_BASE_ADDR, reg_size);
		if (g_regu.vdec_qos_reg == NULL) {
			dev_err(dev, "failed to get vdec_qos_reg %s\n", rdesc->name);
			ret = -ENOMEM;
			goto hisi_hi3630_probe_fail;
		}

		g_regu.venc_qos_reg = devm_ioremap(&pdev->dev, VENC_QOS_BASE_ADDR, reg_size);
		if (g_regu.venc_qos_reg == NULL) {
			dev_err(dev, "failed to get vpp_qos_reg %s\n", rdesc->name);
			ret = -ENOMEM;
			goto hisi_hi3630_probe_fail;
		}

		g_regu.vpp_qos_reg = devm_ioremap(&pdev->dev, VPP_QOS_BASE_ADDR, reg_size);
		if (g_regu.vpp_qos_reg == NULL) {
			dev_err(dev, "failed to get vpp_qos_reg %s\n", rdesc->name);
			ret = -ENOMEM;
			goto hisi_hi3630_probe_fail;
		}

		g_regu.dss_qos_reg = devm_ioremap(&pdev->dev, DSS_QOS_BASE_ADDR, reg_size);
		if (g_regu.dss_qos_reg == NULL) {
			dev_err(dev, "failed to get dss_qos_reg %s\n", rdesc->name);
			ret = -ENOMEM;
			goto hisi_hi3630_probe_fail;
		}

		g_regu.isp_qos_reg = devm_ioremap(&pdev->dev, ISP_QOS_BASE_ADDR, reg_size);
		if (g_regu.isp_qos_reg == NULL) {
			dev_err(dev, "failed to get isp_qos_reg %s\n", rdesc->name);
			ret = -ENOMEM;
			goto hisi_hi3630_probe_fail;
		}
		regulator_noc_hwlock = hwspin_lock_request_specific(REGULATOR_NOC_HWLOCK_ID);
		if (regulator_noc_hwlock == NULL) {
			dev_err(dev, "Request hw noc spin lock failed !\n");
			ret = -ENOMEM;
			goto hwspin_lock_err0;
		}

		regulator_hwlock = hwspin_lock_request_specific(REGULATOR_HWLOCK_ID);
		if (regulator_hwlock == NULL) {
			dev_err(dev, "Request hwspin lock failed !\n");
			ret = -ENOMEM;
			goto hwspin_lock_err1;
		}

#ifdef CONFIG_DEBUG_FS
		d = debugfs_create_dir("hisi_hi3630_debugfs", NULL);
		if (!d) {
			dev_err(dev, "failed to create hi3630 regulator debugfs dir !\n");
			ret = -ENOMEM;
			goto hisi_hi3630_probe_debugfs;
		}
		(void) debugfs_create_file("control_vcc", S_IRUSR, d, NULL, &set_control_vcc_fops);
#endif
		regulator_flag = 1;
	}

	platform_set_drvdata(pdev, rdev);

	return 0;
#ifdef CONFIG_DEBUG_FS
hisi_hi3630_probe_debugfs:
	ret = hwspin_lock_free(regulator_hwlock);
	if (ret)
		pr_err("%s: hwspin lock free fail, ret = [%d]\n", __func__, ret);
#endif
hwspin_lock_err1:
	ret = hwspin_lock_free(regulator_noc_hwlock);
	if (ret)
		pr_err("%s: hwspin noc lock free fail, ret = [%d]\n", __func__, ret);
hwspin_lock_err0:
hisi_hi3630_probe_fail:
	regulator_unregister(rdev);
hisi_hi3630_probe_end:
	kfree(sreg);
	return ret;
}

static int hisi_regulator_hi3630_remove(struct platform_device *pdev)
{
	struct regulator_dev *rdev = platform_get_drvdata(pdev);
	struct hisi_regulator_hi3630 *sreg = rdev_get_drvdata(rdev);
	unsigned int ret;

	ret = hwspin_lock_free(regulator_hwlock);
	if (ret)
		pr_err("%s: hwspin lock free fail, ret = [%d]\n", __func__, ret);

	regulator_unregister(rdev);
	/* TODO: should i worry about that? devm_kzalloc */
	if (sreg->rdesc.volt_table)
		devm_kfree(&pdev->dev, (unsigned int *)sreg->rdesc.volt_table);

	kfree(sreg);
	return 0;
}

static struct platform_driver hisi_regulator_hi3630_driver = {
	.driver = {
		.name	= "hisi_regulator_hi3630",
		.owner  = THIS_MODULE,
		.of_match_table = of_hisi_regulator_hi3630_match_tbl,
	},
	.probe	= hisi_regulator_hi3630_probe,
	.remove	= hisi_regulator_hi3630_remove,
};

static int __init hisi_regulator_hi3630_init(void)
{
	return platform_driver_register(&hisi_regulator_hi3630_driver);
}
fs_initcall(hisi_regulator_hi3630_init);

static void __exit hisi_regulator_hi3630_exit(void)
{
	platform_driver_unregister(&hisi_regulator_hi3630_driver);
}
module_exit(hisi_regulator_hi3630_exit);

MODULE_DESCRIPTION("Hisi regulator hi3630 driver");
MODULE_LICENSE("GPL v2");
