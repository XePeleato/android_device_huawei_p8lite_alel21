/*
 * hi6421_ocp.c - Hisilicon Hi6421 PMIC ocp driver
 *
 * Copyright (C) 2013 Hisilicon Ltd.
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License. See the file "COPYING" in the main directory of this
 * archive for more details.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/module.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/mutex.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/err.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/mfd/hi3xxx_hi6421v300.h>
#include <linux/timer.h>
#include <linux/rtc.h>
#include "hi3xxx_pmic_mntn_inner.h"
#ifdef CONFIG_HI6421_SPK
#include <linux/delay.h>
#include <linux/wakelock.h>
#include <linux/workqueue.h>
#endif
#include <huawei_platform/dsm/dsm_pub.h>
#include <huawei_platform/log/log_exception.h>

static struct dsm_dev dsm_pmu_ocp = {
	.name = "dsm_pmu_ocp",
	.device_name = NULL,
	.ic_name = NULL,
	.module_name = NULL,
	.fops = NULL,
	.buff_size = 1024,
};
static struct dsm_client *ocp_dclient = NULL;

struct hi6421_ocp_info {
	int	irq;
	struct hi6421_pmic *pmic;
	struct workqueue_struct *regulator_ocp_wq;
	struct work_struct regulator_ocp_wk;
	u32 ocp_mask_val[7];
	u32 ocp_debug_enable;
	#ifdef CONFIG_HI6421_SPK
	struct delayed_work hi6421_ocp_delay_work;
	struct workqueue_struct *hi6421_ocp_delay_wq;
	struct wake_lock wake_lock;
	#endif
};

char *pmuocp_name1[] = {
	[BUCK0]			= "buck0",
	[BUCK1]			= "buck1",
	[BUCK2]			= "buck2",
	[BUCK3]			= "buck3",
	[BUCK4]			= "buck4",
	[BUCK5]			= "buck5",
	[BUCK6]			= "buck6",
	[LDO0_2]			= "ldo0_2",
	[LDO1]	 		= "ldo1",
	[LDO2]	 		= "ldo2",
	[LDO3]	 		= "ldo3",
	[LDO4]	 		= "ldo4",
	[LDO5]	 		= "ldo5",
	[LDO6]	 		= "ldo6",
	[LDO7]	 		= "ldo7",
	[LDO8]	 		= "ldo8",
	[LDO9]	 		= "ldo9",
	[LDO10]	 		= "ldo10",
	[LDO11]	 		= "ldo11",
	[LDO12]	 		= "ldo12",
	[LDO13]	 		= "ldo13",
	[LDO14]	 		= "ldo14",
	[LDO15]	 		= "ldo15",
	[LDO16]	 		= "ldo16",
	[LDO17]	 		= "ldo17",
	[LDO18]	 		= "ldo18",
	[LDO19]	 		= "ldo19",
	[LDO20]	 		= "ldo20",
	[LDO21]	 		= "ldo21",
	[LDO22]	 		= "ldo22",
	[LDO23]	 		= "ldo23",
	[LDO24]	 		= "ldo24",
};

char *pmuocp_name2[] = {
	[LDO25]	 		= "ldo25",
	[LDO26]	 		= "ldo26",
	[LSW50]	 		= "lsw50",
	[LSW51]	 		= "lsw51",
	[LSW52]	 		= "lsw52",
	[LSW53]	 		= "lsw53",
	[DCDR2]	 		= "dcdr2",
	[ACDR2]	 		= "acdr2",
	[BST]	 		= "bst",
	[CLASSD]	 		= "classd",
	[BSTSHORT]		= "bstshort",
	[BUCK0SHORT]	= "buck0short",
	[BUCK12SHORT]	= "buck12short",
	[BUCK3SHORT]	= "buck3short",
	[BUCK4SHORT]	= "buck4short",
	[BUCK5SHORT]	= "buck5short",
	[BUCK6SHORT]	= "buck6short",
};

#define LDO11_OCP_FLAG 0x20
#define LDO12_OCP_FLAG 0x10
#define LDO16_OCP_FLAG 0x01
#define LDO9_OCP_FLAG  0x80

#ifdef CONFIG_HI6421_SPK
#define CLASSD_OCP_FLAG 0x40
#define HI6421_IRQ_MASK_10	(0x10C << 2)
#endif
#define PMU_OCP_IRQ	49

#define	OCP_CONTRL_INT1	(0x125 << 2)
#define	OCP_CONTRL_INT2	(0x126 << 2)
#define	OCP_CONTRL_INT3	(0x127 << 2)
#define	OCP_CONTRL_INT4	(0x128 << 2)

#define	OCP_CONTRL_INT5	(0x129 << 2)
#define	OCP_CONTRL_INT6	(0x12A << 2)
#define	OCP_CONTRL_INT7	(0x12B << 2)

#define	OCP_STATE1    (0x03 << 2)
#define	OCP_STATE2    (0x04 << 2)
#define	OCP_STATE3    (0x05 << 2)
#define	OCP_STATE4    (0x06 << 2)
#define	OCP_STATE5    (0x07 << 2)
#define	OCP_STATE6    (0x08 << 2)

#define HI6421_OCP_LDO9_ADDR  (0x86 << 2)
#define HI6421_OCP_LDO11_ADDR (0x8a << 2)
#define HI6421_OCP_LDO12_ADDR (0x8b << 2)
#define HI6421_OCP_LDO16_ADDR (0x92 << 2)
#define HI6421_OCP_LDO9_VALUE  (0x00)
#define HI6421_OCP_LDO11_VALUE (0x05)
#define HI6421_OCP_LDO12_VALUE (0x05)
#define HI6421_OCP_LDO16_VALUE (0x06)

static void hi6421_ocp_mask_irq(struct hi6421_ocp_info *info, bool mask)
{
	struct irq_desc *hi6421_irq_desc;

	hi6421_irq_desc = irq_to_desc(info->irq);
	if (!hi6421_irq_desc) {
		pr_err("%s : irq_to_desc error\n", __FUNCTION__);
		return;
	}

	if (mask) {
		/* mask interrupts */
		hi6421_irq_desc->irq_data.chip->irq_mask(&hi6421_irq_desc->irq_data);
	} else {
		/* unmask interrupts */
		hi6421_irq_desc->irq_data.chip->irq_unmask(&hi6421_irq_desc->irq_data);
	}
}

#ifdef CONFIG_HI6421_SPK
extern void hi6421_spk_enable(void);

static void mask_classd_ocp(struct hi6421_ocp_info *ocp_info)
{
	u8 classd_state1;

	classd_state1 = ioread8(ocp_info->pmic->regs + HI6421_IRQ_MASK_10);
	classd_state1 |= CLASSD_OCP_FLAG;
	iowrite8(classd_state1, ocp_info->pmic->regs + HI6421_IRQ_MASK_10);
}

static void unmask_classd_ocp(struct hi6421_ocp_info *ocp_info)
{
	u8 classd_state1;

	classd_state1 = ioread8(ocp_info->pmic->regs + HI6421_IRQ_MASK_10);
	classd_state1 &= (~CLASSD_OCP_FLAG);
	iowrite8(classd_state1, ocp_info->pmic->regs + HI6421_IRQ_MASK_10);
}

static void hi6421_ocp_work_func(struct work_struct *work)
{
	struct hi6421_ocp_info *ocp_info =
		container_of(work, struct hi6421_ocp_info, hi6421_ocp_delay_work.work);
	u8 ocp_state;

	iowrite8(CLASSD_OCP_FLAG, ocp_info->pmic->regs + OCP_CONTRL_INT6);

	hi6421_spk_enable();

	ocp_state = ioread8(ocp_info->pmic->regs + OCP_CONTRL_INT6);
	if (ocp_state & CLASSD_OCP_FLAG) {
		BUG_ON(1);
	} else {
		unmask_classd_ocp(ocp_info);
	}
}
#endif
static void get_time_stamp(char* timestamp_buf,unsigned int len)
{
	struct timeval tv;
	struct rtc_time tm;

	if(NULL == timestamp_buf) {
		return;
	}
	memset(&tv, 0, sizeof(struct timeval));
	memset(&tm, 0, sizeof(struct rtc_time));
	do_gettimeofday(&tv);
	tv.tv_sec -= sys_tz.tz_minuteswest * 60;
	rtc_time_to_tm(tv.tv_sec, &tm);
	snprintf(timestamp_buf,len, "%04d%02d%02d%02d%02d%02d",
		tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

static void ocp_log_exception_archive(u32 ocp_status)
{
	char cmd[ARCH_NAME_MAX];
	char time_buf[16];
	int ret = 0;

	get_time_stamp(time_buf,16);
	snprintf(cmd, ARCH_NAME_MAX, "%s%s%s%s%s%x%s",
		"archive -i /data/android_logs/charge_log -i /data/android_logs/charge_log.1",
			" -i /data/android_logs/kmsgcat-log -i /data/android_logs/kmsgcat-log.1",
			" -o ", time_buf, "_OCP_status0x", ocp_status, " -z zip");

	ret = log_to_exception("OCP",cmd);
	if(ret < 0 ){
		pr_err("%s %d :upload ocp log error.\n", __func__, __LINE__);
	}
}
static void get_hi6421_register_info(struct hi6421_ocp_info *temp_info)
{
	pr_info("[%s]: OCP Interrupt1 [0x125] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + OCP_CONTRL_INT1));
	pr_info("[%s]: OCP Interrupt2 [0x126] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + OCP_CONTRL_INT2));
	pr_info("[%s]: OCP Interrupt3 [0x127] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + OCP_CONTRL_INT3));
	pr_info("[%s]: OCP Interrupt4 [0x128] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + OCP_CONTRL_INT4));
	pr_info("[%s]: OCP Interrupt5 [0x129] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + OCP_CONTRL_INT5));
	pr_info("[%s]: OCP Interrupt6 [0x12A] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + OCP_CONTRL_INT6));
	pr_info("[%s]: OCP Interrupt7 [0x12B] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + OCP_CONTRL_INT7));

	pr_info("[%s]: OCP Status1 [0x003] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + OCP_STATE1));
	pr_info("[%s]: OCP Status2 [0x004] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + OCP_STATE2));
	pr_info("[%s]: OCP Status3 [0x005] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + OCP_STATE3));
	pr_info("[%s]: OCP Status4 [0x006] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + OCP_STATE4));
	pr_info("[%s]: OCP Status5 [0x007] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + OCP_STATE5));
	pr_info("[%s]: OCP Status6 [0x008] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + OCP_STATE6));

	pr_info("[%s]: OCP LDO0_1 status [0x077] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO0_1_ADDR));
	pr_info("[%s]: OCP LDO0_2 status [0x078] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO0_2_ADDR));
	pr_info("[%s]: OCP LDO1 status [0x07A] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO1_ADDR));
	pr_info("[%s]: OCP LDO2 status [0x07B] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO2_ADDR));
	pr_info("[%s]: OCP LDO3 status [0x07C] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO3_ADDR));
	pr_info("[%s]: OCP LDO4 status [0x07E] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO4_ADDR));
	pr_info("[%s]: OCP LDO5 status [0x080] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO5_ADDR));
	pr_info("[%s]: OCP LDO6 status [0x082] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO6_ADDR));
	pr_info("[%s]: OCP LDO7 status [0x083] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO7_ADDR));
	pr_info("[%s]: OCP LDO8 status [0x085] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO8_ADDR));
	pr_info("[%s]: OCP LDO9 status [0x086] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO9_ADDR));
	pr_info("[%s]: OCP LDO10 status [0x088] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO10_ADDR));
	pr_info("[%s]: OCP LDO11 status [0x08A] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO11_ADDR));
	pr_info("[%s]: OCP LDO12 status [0x08B] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO12_ADDR));
	pr_info("[%s]: OCP LDO13 status [0x08C] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO13_ADDR));
	pr_info("[%s]: OCP LDO14 status [0x08E] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO14_ADDR));
	pr_info("[%s]: OCP LDO15 status [0x090] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO15_ADDR));
	pr_info("[%s]: OCP LDO16 status [0x092] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO16_ADDR));
	pr_info("[%s]: OCP LDO17 status [0x093] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO17_ADDR));
	pr_info("[%s]: OCP LDO18 status [0x094] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO18_ADDR));
	pr_info("[%s]: OCP LDO19 status [0x095] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO19_ADDR));
	pr_info("[%s]: OCP LDO20 status [0x096] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO20_ADDR));
	pr_info("[%s]: OCP LDO21 status [0x097] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO21_ADDR));
	pr_info("[%s]: OCP LDO22 status [0x098] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO22_ADDR));
	pr_info("[%s]: OCP LDO23 status [0x099] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO23_ADDR));
	pr_info("[%s]: OCP LDO24 status [0x09A] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO24_ADDR));
	pr_info("[%s]: OCP LDO25 status [0x09B] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO25_ADDR));
	pr_info("[%s]: OCP LDO26 status [0x09C] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LDO26_ADDR));
	pr_info("[%s]: OCP BUCK0 status [0x09E] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_BUCK0_ADDR));
	pr_info("[%s]: OCP BUCK12 status [0x0A0] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_BUCK12_ADDR));
	pr_info("[%s]: OCP BUCK3 status [0x0A2] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_BUCK3_ADDR));
	pr_info("[%s]: OCP BUCK4 status [0x0A4] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_BUCK4_ADDR));
	pr_info("[%s]: OCP BUCK5 status [0x0A6] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_BUCK5_ADDR));
	pr_info("[%s]: OCP BUCK6 status [0x0A8] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_BUCK6_ADDR));
	pr_info("[%s]: OCP LSW50 status [0x0AC] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LSW50_ADDR));
	pr_info("[%s]: OCP LSW51 status [0x0AD] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LSW51_ADDR));
	pr_info("[%s]: OCP LSW52 status [0x0AE] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LSW52_ADDR));
	pr_info("[%s]: OCP LSW53 status [0x0AF] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_LSW53_ADDR));
	pr_info("[%s]: OCP CLASSD status [0x0BF] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_CLASSD_ADDR));
	pr_info("[%s]: OCP BOOST status [0x0B0] = 0x%x.\n", __func__, ioread8(temp_info->pmic->regs + REGULATOR_BOOST_ADDR));
}

static void inquiry_hi6421_ocp_reg(struct work_struct *work)
{
	struct hi6421_ocp_info *ocp_info = container_of(work, struct hi6421_ocp_info, regulator_ocp_wk);
	u8 ocp_status1_bef, ocp_status2_bef, ocp_status3_bef, ocp_status4_bef, ocp_status5_bef, ocp_status6_bef, ocp_status7_bef;
	u8 ocp_status1, ocp_status2, ocp_status3, ocp_status4, ocp_status5, ocp_status6, ocp_status7;
	int i, dsm_error_found = -1;
	int error_offset = -1;
	u32 ocp_status_all1, ocp_status_all2, ocp_status_all1_bef, ocp_status_all2_bef;

	/*get ocp state*/
	ocp_status1 = ioread8(ocp_info->pmic->regs + OCP_CONTRL_INT1);
	ocp_status2 = ioread8(ocp_info->pmic->regs + OCP_CONTRL_INT2);
	ocp_status3 = ioread8(ocp_info->pmic->regs + OCP_CONTRL_INT3);
	ocp_status4 = ioread8(ocp_info->pmic->regs + OCP_CONTRL_INT4);
	ocp_status5 = ioread8(ocp_info->pmic->regs + OCP_CONTRL_INT5);
	ocp_status6 = ioread8(ocp_info->pmic->regs + OCP_CONTRL_INT6);
	ocp_status7 = ioread8(ocp_info->pmic->regs + OCP_CONTRL_INT7);

	ocp_status_all1 = ocp_status1 | (ocp_status2 << 8) | (ocp_status3 << 16) | (ocp_status4 << 24);
	ocp_status_all2 = ocp_status5 | (ocp_status6 << 8) | (ocp_status7 << 16);

	if (!dsm_client_ocuppy(ocp_dclient)) {
		dsm_error_found++;
	}

	/*judgement ocp state and clean interrupt*/
	for (i = 0; i < 32; i++) {
		if (ocp_status_all1 & (0x01 << i)) {
			pr_warn("hi6421 %s ocp happened, please attention!\n\r", pmuocp_name1[i]);
			if (-1 == error_offset) {
				if (dsm_error_found >= 0) {
					dsm_client_record(ocp_dclient, "hi6421 %s ocp happened, please attention!\n", pmuocp_name1[i]);
					dsm_error_found++;
					error_offset = i;
				}
			}
		}
	}
	for (i = 0; i < 24; i++) {
		if (ocp_status_all2 & (0x01 << i)) {
			pr_warn("hi6421 %s ocp happened, please attention!\n\r", pmuocp_name2[i]);
			if (-1 == error_offset) {
				if (dsm_error_found >= 0) {
					dsm_client_record(ocp_dclient, "hi6421 %s ocp happened, please attention!\n", pmuocp_name1[i]);
					dsm_error_found++;
					error_offset = i+32;
				}
			}
		}
	}

	if (dsm_error_found > 0) {
		dsm_client_notify(ocp_dclient, DSM_PMU_OCP_ERROR_NO_BASE + error_offset);
	} else if (!dsm_error_found) {
		dsm_client_unocuppy(ocp_dclient);
	} else {
		/* dsm ocp_dclient ocuppy failed */
	}

	get_hi6421_register_info(ocp_info);

	if (ocp_status3 & LDO11_OCP_FLAG) {/*SIM0 OCP Happend, close LDO11*/
		iowrite8(HI6421_OCP_LDO11_VALUE, ocp_info->pmic->regs + HI6421_OCP_LDO11_ADDR);
		pr_info("[%s]: OCP LDO11 status [0x08a] = 0x%x.\n", __func__, ioread8(ocp_info->pmic->regs + REGULATOR_LDO11_ADDR));
	} else if(ocp_status3 & LDO12_OCP_FLAG) {/*SIM1 OCP Happend, close LDO12*/
		iowrite8(HI6421_OCP_LDO12_VALUE, ocp_info->pmic->regs + HI6421_OCP_LDO12_ADDR);
		pr_info("[%s]: OCP LDO12 status [0x08b] = 0x%x.\n", __func__, ioread8(ocp_info->pmic->regs + REGULATOR_LDO12_ADDR));
	} else if(ocp_status3 & LDO16_OCP_FLAG) {/*SD OCP Happend, close LDO16*/
		iowrite8(HI6421_OCP_LDO16_VALUE, ocp_info->pmic->regs + HI6421_OCP_LDO16_ADDR);
		pr_info("[%s]: OCP LDO16 status [0x092] = 0x%x.\n", __func__, ioread8(ocp_info->pmic->regs + REGULATOR_LDO16_ADDR));
	} else if(ocp_status3 & LDO9_OCP_FLAG) {/*SD OCP Happend, close LDO9*/
		iowrite8(HI6421_OCP_LDO9_VALUE, ocp_info->pmic->regs + HI6421_OCP_LDO9_ADDR);
		pr_info("[%s]: OCP LDO9 status [0x086] = 0x%x.\n", __func__, ioread8(ocp_info->pmic->regs + REGULATOR_LDO9_ADDR));
	} else {
		/*bypass*/
	}

	iowrite8(ocp_status1, ocp_info->pmic->regs + OCP_CONTRL_INT1);
	iowrite8(ocp_status2, ocp_info->pmic->regs + OCP_CONTRL_INT2);
	iowrite8(ocp_status3, ocp_info->pmic->regs + OCP_CONTRL_INT3);
	iowrite8(ocp_status4, ocp_info->pmic->regs + OCP_CONTRL_INT4);
	iowrite8(ocp_status5, ocp_info->pmic->regs + OCP_CONTRL_INT5);
	iowrite8(ocp_status6, ocp_info->pmic->regs + OCP_CONTRL_INT6);
	iowrite8(ocp_status7, ocp_info->pmic->regs + OCP_CONTRL_INT7);

	hi6421_ocp_mask_irq(ocp_info, 0);

	ocp_status1_bef = ocp_status1 & (~ocp_info->ocp_mask_val[0]);
	ocp_status2_bef = ocp_status2 & (~ocp_info->ocp_mask_val[1]);
	ocp_status3_bef = ocp_status3 & (~ocp_info->ocp_mask_val[2]);
	ocp_status4_bef = ocp_status4 & (~ocp_info->ocp_mask_val[3]);
	ocp_status5_bef = ocp_status5 & (~ocp_info->ocp_mask_val[4]);
	ocp_status6_bef = ocp_status6 & (~ocp_info->ocp_mask_val[5]);
	ocp_status7_bef = ocp_status7 & (~ocp_info->ocp_mask_val[6]);

	ocp_status_all1_bef = ocp_status1_bef | (ocp_status2_bef << 8) | (ocp_status3_bef << 16) | (ocp_status4_bef << 24);
	ocp_status_all2_bef = ocp_status5_bef | (ocp_status6_bef << 8) | (ocp_status7_bef << 16);

	if (ocp_status_all1_bef || ocp_status_all2_bef) {
		if(ocp_info->ocp_debug_enable == 1){
			#ifdef CONFIG_HI6421_SPK
			if (ocp_status6_bef & CLASSD_OCP_FLAG) {/*classD ocp happend*/
				mask_classd_ocp(ocp_info);
				wake_lock_timeout(&ocp_info->wake_lock, 250);
				queue_delayed_work(ocp_info->hi6421_ocp_delay_wq,
					&ocp_info->hi6421_ocp_delay_work, msecs_to_jiffies(200));
			} else {
				BUG_ON(1);
			}
			#else
			BUG_ON(1);
			#endif
		}else{
			//upload ocp log to APR
			ocp_log_exception_archive(ocp_status_all1_bef | ocp_status_all2_bef);
		}
	}
}

static irqreturn_t hi6421_regulator_current_over_support(int irq, void *data)
{
	struct hi6421_ocp_info *hi6421_ocp_info = data;
	hi6421_ocp_mask_irq(hi6421_ocp_info, 1);
	queue_work(hi6421_ocp_info->regulator_ocp_wq, &hi6421_ocp_info->regulator_ocp_wk);

	return IRQ_HANDLED;
}
static int get_ocp_dt_parse(struct hi6421_ocp_info *info, struct device_node *np)
{
	unsigned int register_info[7];
	int ret = 0;

	/* parse ocp irq mask ctrl_reg */
	ret = of_property_read_u32_array(np, "hisi-ocp-mask",
						register_info, 7);
	if (ret) {
		pr_err("no hisi-ocp-mask set\n");
		return ret;
	}
	info->ocp_mask_val[0] = register_info[0];
	info->ocp_mask_val[1] = register_info[1];
	info->ocp_mask_val[2] = register_info[2];
	info->ocp_mask_val[3] = register_info[3];
	info->ocp_mask_val[4] = register_info[4];
	info->ocp_mask_val[5] = register_info[5];
	info->ocp_mask_val[6] = register_info[6];

	return ret;
}
static int hi6421_ocp_probe(struct platform_device *pdev)
{
	struct hi6421_ocp_info *info;
	struct device_node *np = NULL;
	struct device *dev = NULL;
	int ret = 0;
	int i = 0;

	if (pdev == NULL) {
		pr_err("[%s]ocp get  platform device para is err!\n", __func__);
		return -EINVAL;
	}
	dev = &pdev->dev;
	np = dev->of_node;

	info = devm_kzalloc(&pdev->dev, sizeof(*info), GFP_KERNEL);
	if (!info) {
		dev_err(&pdev->dev, "failed to allocate memory\n");
		return -ENOMEM;
	}

	info->pmic = dev_get_drvdata(pdev->dev.parent);
	if (info->pmic == NULL) {
		dev_err(&pdev->dev, "failed to get parent resource!\n");
		return -ENOMEM;
	}

	ret = of_property_read_u32(np, "ocp_debug_enable", &(info->ocp_debug_enable));
	if (ret) {
		dev_err(&pdev->dev, "get ocp_debug_enable flag fail\n");
		info->ocp_debug_enable = 0;
	}
	dev_info(&pdev->dev, "ocp_debug_enable is dts is : %d", info->ocp_debug_enable);
	if (strstr(saved_command_line, "androidboot.swtype=factory")){
		dev_info(&pdev->dev, "factory version,set ocp_debug_enable to 1\n");
		info->ocp_debug_enable = 1;
	}
	ret = get_ocp_dt_parse(info, np);
	if (ret) {
		dev_err(&pdev->dev, "get ocp dt parse fail!\n");
		return -ENOMEM;
	}
	for (i = 0; i < 7; ++i) {
		dev_info(&pdev->dev, "hisi-ocp-mask[%d] is 0x%x\n", i, info->ocp_mask_val[i]);
	}
	info->irq = platform_get_irq_byname(pdev, "ocpirq");
	if (info->irq < 0)
		return -ENOENT;

	#ifdef CONFIG_HI6421_SPK
	info->hi6421_ocp_delay_wq = create_singlethread_workqueue("hi6421_ocp_delay_wq");
	if (!(info->hi6421_ocp_delay_wq)) {
		pr_err("%s(%u) : workqueue create failed", __FUNCTION__,__LINE__);
		return -ENOMEM;
	}
	INIT_DELAYED_WORK(&info->hi6421_ocp_delay_work, hi6421_ocp_work_func);

	wake_lock_init(&info->wake_lock, WAKE_LOCK_SUSPEND, "hi6421-ocp-irq");
	#endif
	info->regulator_ocp_wq = create_singlethread_workqueue("hi6421_regulator_ocp");
	if (info->regulator_ocp_wq == NULL) {
		dev_err(&pdev->dev, "failed to create work queue\n");
		#ifdef CONFIG_HI6421_SPK
		ret = -ENOMEM;
		goto err_ocp_work_queue;
		#else
		return -ENOMEM;
		#endif
	}
	INIT_WORK(&info->regulator_ocp_wk, (void *)inquiry_hi6421_ocp_reg);
	ret = devm_request_irq(&pdev->dev, info->irq, hi6421_regulator_current_over_support,
			       IRQF_DISABLED | IRQF_NO_SUSPEND, "ocpirq", info);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to requset irq!\n");
		goto free_charger_wq;
	}


	platform_set_drvdata(pdev, info);

	if (!ocp_dclient) {
		ocp_dclient = dsm_register_client(&dsm_pmu_ocp);
	}

	return ret;

free_charger_wq:
	destroy_workqueue(info->regulator_ocp_wq);
#ifdef CONFIG_HI6421_SPK
err_ocp_work_queue:
	if(info->hi6421_ocp_delay_wq) {
		cancel_delayed_work(&info->hi6421_ocp_delay_work);
		flush_workqueue(info->hi6421_ocp_delay_wq);
		destroy_workqueue(info->hi6421_ocp_delay_wq);
	}
#endif
	return ret;
}

static int hi6421_ocp_remove(struct platform_device *pdev)
{
	struct hi6421_ocp_info *info;

	/*get hi6421_ocp_info*/
	info = platform_get_drvdata(pdev);
	if (NULL == info) {
		pr_err("%s %d platform_get_drvdata NULL\n", __func__, __LINE__);
		return -1;
	}

#ifdef CONFIG_HI6421_SPK
	if(info->hi6421_ocp_delay_wq) {
		cancel_delayed_work(&info->hi6421_ocp_delay_work);
		flush_workqueue(info->hi6421_ocp_delay_wq);
		destroy_workqueue(info->hi6421_ocp_delay_wq);
	}
#endif
	if (info->regulator_ocp_wq)
		destroy_workqueue(info->regulator_ocp_wq);

	return 0;
}

static struct of_device_id hi6421_ocp_of_match[] = {
	{ .compatible = "hisilicon,hi6421-ocpirq", },
	{ },
};
MODULE_DEVICE_TABLE(of, hi6421_ocp_of_match);

static struct platform_driver hi6421_ocp_driver = {
	.probe		= hi6421_ocp_probe,
	.remove		= hi6421_ocp_remove,
	.driver		= {
		.owner		= THIS_MODULE,
		.name		= "hi6421-ocp",
		.of_match_table	= hi6421_ocp_of_match,
	},
};

int __init hi6421_ocp_init(void)
{
	return platform_driver_register(&hi6421_ocp_driver);
}

static void __exit hi6421_ocp_exit(void)
{
	platform_driver_unregister(&hi6421_ocp_driver);
}

late_initcall(hi6421_ocp_init);
module_exit(hi6421_ocp_exit);

MODULE_DESCRIPTION("Hi6421 PMIC ocp driver");
MODULE_LICENSE("GPL v2");
