/*
 *	linux/arch/arm/mach-k3v3/hisi_lpregs.c
 *
 * Copyright (C) 2013 Hisilicon
 * License terms: GNU General Public License (GPL) version 2
 *
 */
#include <linux/init.h>
#include <linux/cpu_pm.h>
#include <linux/suspend.h>
#include <linux/syscore_ops.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/clockchips.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/arm-cci.h>
#include <linux/irqchip/arm-gic.h>
#include <linux/hw_power_monitor.h>
#include "hisi_lpregs.h"

#define PM_WAKE_STATUS_OFFSET			(0x034)
/*line -emacro(750,*)*/
/*lint -e750*/
#define PM_SYS_SLEEP_CNT_OFFSET			(0x060)
#define PM_SYS_WAKE_CNT_OFFSET			(0x064)
#define PM_AP_WAKE_CNT_OFFSET			(0x068)
#define PM_HIFI_WAKE_CNT_OFFSET			(0x06C)
#define PM_MODEM_WAKE_CNT_OFFSET		(0x070)
#define PM_IOM3_WAKE_CNT_OFFSET			(0x074)
#define PM_LPM3_WAKE_CNT_OFFSET			(0x078)
#define PM_AP_SUSPEND_CNT_OFFSET			(0x07C)
#define PM_AP_RESUME_CNT_OFFSET			(0x080)
#define PM_IOM3_SUSPEND_CNT_OFFSET		(0x084)
#define PM_IOM3_RESUME_CNT_OFFSET		(0x088)
#define PM_MODEM_SUSPEND_CNT_OFFSET		(0x08C)
#define PM_MODEM_RESUME_CNT_OFFSET		(0x090)
#define PM_HIFI_SUSPEND_CNT_OFFSET		(0x094)
#define PM_HIFI_RESUME_CNT_OFFSET			(0x098)
#define PM_WAKE_IRQ_OFFSET				(0x09C)
#define PM_AP_WAKE_IRQ_OFFSET			(0x0A0)
#define PM_AP_NSIPC_IRQ_OFFSET			(0x0A4)
#define PMU_MCU_POWER_STATUS_OFFSET			(0x0A8)

#define SCINT_STAT_OFFSET					(0x458)
/*lint +e750*/
/*line +emacro(750,*)*/


#define PM_BUFFER_SIZE						(256)

#define WAKE_STATUS_AP_MASK				(0x01)
#define WAKE_STATUS_HIFI_MASK				(0x02)
#define WAKE_STATUS_MODEM_MASK			(0x04)
#define WAKE_STATUS_IOM3_MASK			(0x08)
#define WAKE_STATUS_HOTPLUG_MASK		(0x10)
/***************************
  bit 0:gpio_22_int,
  bit 1:gpio_23_int,
  bit 2:gpio_24_int,
  bit 3:gpio_25_int,
  bit 4:gpio_26_int,
  bit 5:rtc_int,
  bit 6:rtc1_int,
  bit 15:gic_int0,
  bit 16:gic_int1,
  bit 17:gic_int2,
  bit 18:gic_int3,
 ***************************/
#define AP_INT_MASK (0x0001807f)

/*******************
  bit 22:drx0_int,
  bit 23:drx1_int,
  bit 24:drx2_int,
  bit 25:drx3_int,
  bit 26:drx4_int,
 *******************/
#define MODEM_INT_MASK (0x07c06800)

/*******************
  bit 12:timer5_int,
 *******************/
#define LPM3_INT_MASK (0x1000)
#define WAKE_IRQ_NUM_MAX 30

#define DEBG_SUSPEND_PRINTK		(1<<0)
#define DEBG_SUSPEND_IO_SHOW		(1<<1)
#define DEBG_SUSPEND_PMU_SHOW	(1<<2)
/*line -emacro(750,*)*/
/*lint -e750*/
#define DEBG_SUSPEND_IO_SET		(1<<3)
#define DEBG_SUSPEND_PMU_SET		(1<<4)
#define DEBG_SUSPEND_IO_S_SET		(1<<5)
#define DEBG_SUSPEND_RTC_EN		(1<<6)
#define DEBG_SUSPEND_TIMER_EN		(1<<7)
#define DEBG_SUSPEND_WAKELOCK	(1<<8)
#define DEBG_SUSPEND_AUDIO		(1<<9)
#define DEBG_SUSPEND_CLK_SHOW	(1<<10)
#define DEBG_SUSPEND_IPC_DATA_SHOW	(1<<11)
#define DEBUG_INFO				(1<<31)

#define REG_SCLPM3CLKEN_OFFSET	(0x500)
#define REG_SCLPM3RSTEN_OFFSET	(0x504)

#define REG_SCLPM3RSTDIS_OFFSET	(0x508)
/*lint +e750*/
/*line +emacro(750,*)*/

#define BIT_CLK_UART_SHIFT			(1 << 8)
#define BIT_RST_UART_SHIFT			(1 << 9)

#define PMU_CTRL_BEGIN		0x00
#define PMU_CTRL_END		0x11E
#define PMU_IRQ_BEGIN		0x120
#define PMU_IRQ_END		0x124
#define PMU_OCP_BEGIN		0x125
#define PMU_OCP_END		0x12B
#define PMU_RTC_BEGIN		0x12C
#define PMU_RTC_END		0x138
#define PMU_COUL_BEGIN		0x140
#define PMU_COUL_END		0x1D2
#define PMUSSI_REG(x) (sysreg_base.pmic_base + ((x)<<2))

#define SCTRLBASE       0xfff0a000
#define CRGBASE          0xfff35000
#define PMCBASE          0xfff31000
#define PMUBASE          0xfff34000
#define PCTRLBASE		0xE8A09000


/*ipc processor max*/
#define IPC_PROCESSOR_MAX		6
/*ipc mailbox max*/
#define NSIPC_MAILBOX_MAX		27

#define AP_IPC_PROC_MAX			2

#define IPC_MBXDATA_MAX			8
/*line -emacro(750,*)*/
/*lint -e750*/
#define IPC_MBX_SOURCE_OFFSET(m)		((m) << 6)
#define IPC_MBX_DSTATUS_OFFSET(m)		(0x0C + ((m) << 6))
#define IPC_MBXDATA_OFFSET(m, idex)		(0x20 + 4 * (idex) + ((m) << 6))
/*lint +e750*/
/*line +emacro(750,*)*/

#define DEBUG_FILE_MODE     (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)
#define NO_SEQFILE 0
#define LOWPM_MSG(seq_file, fmt, args ...) \
		{		\
			if (NO_SEQFILE == seq_file)	\
				printk(KERN_INFO fmt, ##args);	\
			else	\
				seq_printf(seq_file, fmt, ##args);	\
		}

unsigned char *processor_name[IPC_PROCESSOR_MAX] = {
	"gic1",
	"gic2",
	"iom3",
	"lpm3",
	"hifi",
	"modem"
};

#define LPM3_IRQ_MAX		230
static unsigned char *lpm3_irq_name[LPM3_IRQ_MAX] = {
	"a53_0_interr",		/*0*/
	"a53_0_exterr",		/*1*/
	"a53_0_pmu0",		/*2*/
	"a53_0_pmu1",		/*3*/
	"a53_0_pmu2",		/*4*/
	"a53_0_pmu3",		/*5*/
	"MDM_IPC_CM3_intr0",		/*6*/
	"MDM_IPC_CM3_intr1",		/*7*/
	"MDM_IPC_MODCPU_intr0",		/*8*/
	"MDM_IPC_MODCPU_intr1",		/*9*/
	"a53_1_interr",		/*10*/
	"a53_1_exterr",		/*11*/
	"a53_1_pmu0",		/*12*/
	"a53_1_pmu1",		/*13*/
	"a53_1_pmu2",		/*14*/
	"a53_1_pmu3",		/*15*/
	"MDM_IPC_DSP0_intr0",		/*16*/
	"MDM_IPC_DSP0_intr1",		/*17*/
	"MDM_IPC_APPCPU_intr0",		/*18*/
	"MDM_IPC_APPCPU_intr1",		/*19*/
	"TIME00",		/*20*/
	"TIME01",		/*21*/
	"TIME10",		/*22*/
	"TIME11",		/*23*/
	"TIME20",		/*24*/
	"TIME21",		/*25*/
	"TIME30",		/*26*/
	"TIME31",		/*27*/
	"TIME40",		/*28*/
	"TIME41",		/*29*/
	"TIME50",		/*30*/
	"TIME51",		/*31*/
	"TIME60",		/*32*/
	"TIME61",		/*33*/
	"TIME70",		/*34*/
	"TIME71",		/*35*/
	"WatchDog0",		/*36*/
	"WatchDog1",		/*37*/
	"RTC0",		/*38*/
	"RTC1",		/*39*/
	"UART0",		/*40*/
	"UART1",		/*41*/
	"UART2",		/*42*/
	"UART3",		/*43*/
	"UART4",		/*44*/
	"UART5",		/*45*/
	"UART6",		/*46*/
	"SPI0",		/*47*/
	"SPI1",		/*48*/
	"TSI0",		/*49*/
	"TSI1",		/*50*/
	"I2C0",		/*51*/
	"I2C1",		/*52*/
	"I2C2",		/*53*/
	"I2C3",		/*54*/
	"I2C4",		/*55*/
	"I2C5",		/*56*/
	"reserved",		/*57*/
	"reserved",		/*58*/
	"BLPWM",		/*59*/
	"GPIO0[]",		/*60*/
	"GPIO1[]",		/*61*/
	"GPIO2[]",		/*62*/
	"GPIO3[]",		/*63*/
	"GPIO4[]",		/*64*/
	"GPIO5[]",		/*65*/
	"GPIO6[]",		/*66*/
	"GPIO7[]",		/*67*/
	"GPIO8[]",		/*68*/
	"GPIO9[]",		/*69*/
	"GPIO10[]",		/*70*/
	"GPIO11[]",		/*71*/
	"GPIO12[]",		/*72*/
	"GPIO13[]",		/*73*/
	"GPIO14[]",		/*74*/
	"GPIO15[]",		/*75*/
	"GPIO16[]",		/*76*/
	"GPIO17[]",		/*77*/
	"GPIO18[]",		/*78*/
	"GPIO19[]",		/*79*/
	"GPIO20[]",		/*80*/
	"GPIO21[]",		/*81*/
	"GPIO22[]",		/*82*/
	"GPIO23[]",		/*83*/
	"GPIO24[]",		/*84*/
	"GPIO25[]",		/*85*/
	"GPIO26[]",		/*86*/
	"reserved",		/*87*/
	"CoreSight_ETR_Full",		/*88*/
	"CoreSight_ETF_Full",		/*89*/
	"|CCI400_overflow[3:0]",		/*90*/
	"CCI400_overflow[4]",		/*91*/
	"CCI400_err",		/*92*/
	"SECENG_P",		/*93*/
	"SECENG_S",		/*94*/
	"reserved",		/*95*/
	"intr_tds_hifi",		/*96*/
	"intr_mcpu_hifi",		/*97*/
	"intr_bbe16_hifi",		/*98*/
	"USB2OTG",		/*99*/
	"USB2HST",		/*100*/
	"NANDC",		/*101*/
	"EMMC",		/*102*/
	"SD",		/*103*/
	"SDIO0",		/*104*/
	"DMAC2",		/*105*/
	"DMAC_ns2",		/*106*/
	"NOC-comb",		/*107*/
	"DDRC",		/*108*/
	"SCTRL",		/*109*/
	"PMCTRL",		/*110*/
	"TSENSOR_a53_0",		/*111*/
	"TSENSOR_a53_1",		/*112*/
	"TSENSOR_G3D",		/*113*/
	"TSENSOR_Modem",		/*114*/
	"ISP",		/*115*/
	"ISP-mmu",		/*116*/
	"CSI-p",		/*117*/
	"CSI-s",		/*118*/
	"ASP",		/*119*/
	"ASP-ns",		/*120*/
	"G3D_JOB",		/*121*/
	"G3D_MMU",		/*122*/
	"G3D_GPU",		/*123*/
	"VDM_A53_1",		/*124*/
	"VPP",		/*125*/
	"VPP_mmu",		/*126*/
	"VENC",		/*127*/
	"VDEC",		/*128*/
	"DSS-pdp",		/*129*/
	"DSS-sdp",		/*130*/
	"DSS-offline",		/*131*/
	"DSS_mcu_pdp",		/*132*/
	"DSS_mcu_sdp",		/*133*/
	"DSS_mcu_offline",		/*134*/
	"DSS_dsi0",		/*135*/
	"DSS_dsi1",		/*136*/
	"reserved",		/*137*/
	"a53_0_core0_pwr",		/*138*/
	"a53_0_core1_pwr",		/*139*/
	"a53_0_core2_pwr",		/*140*/
	"a53_0_core3_pwr",		/*141*/
	"VDM_A53_0",		/*142*/
	"PMC-DDRC-DFS",		/*143*/
	"PMC-DDRC-CFG",		/*144*/
	"PMC-DVFS-a53_0",		/*145*/
	"PMC-DVFS-a53_1",		/*146*/
	"PMC-DVFS-G3D",		/*147*/
	"PMC-AVS-a53_0",		/*148*/
	"PMC-AVS-a53_1",		/*149*/
	"PMC-AVS-G3D",		/*150*/
	"PMC-AVS-IDLE-a53_0",		/*151*/
	"PMC-AVS-IDLE-a53_1",		/*152*/
	"PMC-AVS-IDLE-G3D",		/*153*/
	"M3_IO_wd",		/*154*/
	"M3_LP_wd",		/*155*/
	"reserved",		/*156*/
	"IPC_S_int3",		/*157*/
	"IPC_S_int4",		/*158*/
	"IPC_S_mbx13",		/*159*/
	"IPC_S_mbx14",		/*160*/
	"IPC_S_mbx15",		/*161*/
	"IPC_S_mbx16",		/*162*/
	"IPC_S_mbx17",		/*163*/
	"IPC_S_mbx18",		/*164*/
	"IPC_S_mbx[20:19]",		/*165*/
	"IPC_NS_int3",		/*166*/
	"IPC_NS_int4",		/*167*/
	"IPC_NS_int5",		/*168*/
	"IPC_NS_int6",		/*169*/
	"IPC_NS_mbx13",		/*170*/
	"IPC_NS_mbx14",		/*171*/
	"IPC_NS_mbx15",		/*172*/
	"IPC_NS_mbx16",		/*173*/
	"IPC_NS_mbx17",		/*174*/
	"IPC_NS_mbx18",		/*175*/
	"IPC_NS_mbx[21:19]",		/*176*/
	"{IPC_NS_mbx[24:22]",		/*177*/
	"MDM_WDOG_intr",		/*178*/
	"ASP-IPC-ARM",		/*179*/
	"ASP-IPC-MCPU",		/*180*/
	"ASP-IPC-BBE16",		/*181*/
	"ASP_WD",		/*182*/
	"ASP_AXI_DLOCK",		/*183*/
	"ASP_DMA_SECURE",		/*184*/
	"ASP_DMA_SECURE_N",		/*185*/
	"SCI0",		/*186*/
	"SCI1",		/*187*/
	"SOCP0",		/*188*/
	"SOCP1",		/*189*/
	"LTE_ARM_CLK_SWITCH",		/*190*/
	"LTE_ARM_WAKEUP",		/*191*/
	"TDS_DRX_ARM_WAKEUP",		/*192*/
	"TDS_DRX_ARM_CLKSWITCH",		/*193*/
	"G1_INT_BBP_TO_CPU_clkswitch",		/*194*/
	"G1_INT_BBP_TO_CPU_32K_wakeup",		/*195*/
	"G1_INT_BBP_TO_DSP_32K_wakeup",		/*196*/
	"G2_INT_BBP_TO_CPU_clkswitch",		/*197*/
	"G2_INT_BBP_TO_CPU_32K_wakeup",		/*198*/
	"G2_INT_BBP_TO_DSP_32K_wakeup",		/*199*/
	"W_ARM_INT_02_wakeup",		/*200*/
	"W_ARM_INT_03_clkswitch",		/*201*/
	"MDM_IPF_intr0",		/*202*/
	"MDM_IPF_intr1",		/*203*/
	"MDM_IPF_intr0",		/*204*/
	"MDM_IPF_intr1",		/*205*/
	"MDM_EDMAC_intr[2]",		/*206*/
	"GIC_IRQ_OUT[0]",		/*207*/
	"GIC_IRQ_OUT[1]",		/*208*/
	"GIC_IRQ_OUT[2]",		/*209*/
	"GIC_IRQ_OUT[3]",		/*210*/
	"GIC_IRQ_OUT[4]",		/*211*/
	"GIC_IRQ_OUT[5]",		/*212*/
	"GIC_IRQ_OUT[6]",		/*213*/
	"GIC_IRQ_OUT[7]",		/*214*/
	"GIC_FIQ_OUT[0]",		/*215*/
	"GIC_FIQ_OUT[1]",		/*216*/
	"GIC_FIQ_OUT[2]",		/*217*/
	"GIC_FIQ_OUT[3]",		/*218*/
	"GIC_FIQ_OUT[4]",		/*219*/
	"GIC_FIQ_OUT[5]",		/*220*/
	"GIC_FIQ_OUT[6]",		/*221*/
	"GIC_FIQ_OUT[7]",		/*222*/
	"IPC_COMBO_AP",		/*223*/
	"IPC_COMBO_IOM3",		/*224*/
	"IPC_S_int5",		/*225*/
	"a53_1_core0_pwr",		/*226*/
	"a53_1_core1_pwr",		/*227*/
	"a53_1_core2_pwr",		/*228*/
	"a53_1_core3_pwr",		/*229*/
};
#define IPC_COMBO_AP		223


extern bool console_suspend_enabled;

static unsigned g_usavedcfg;
static int g_suspended;
static unsigned g_utimer_inms;
static unsigned g_urtc_ins;
unsigned long hisi_reserved_pm_phymem = 0x3FF7FC00;

extern int get_console_index(void);

#ifdef CONFIG_HI3XXX_SR_DEBUG
#include <linux/wakelock.h>
static struct wake_lock lowpm_wake_lock;

/* struct sysreg_bases to hold the base address of some system registers.*/
struct sysreg_base_addr {
	void __iomem *uart_base;
	void __iomem *pctrl_base;
	void __iomem *pmctrl_base;
	void __iomem *sysctrl_base;
	void __iomem *ioc_base[4];
	void __iomem *gpio_base[27];
	void __iomem *crg_base;
	void __iomem *pmic_base;
	void __iomem *reserved_base;
	void __iomem *nsipc_base;
};

struct sysreg_base_addr sysreg_base;

struct lp_clk {
	unsigned int reg_base;
	unsigned int reg_off;
	unsigned int bit_idx;
	const char *clk_name;
	unsigned int status;
};

struct sys_reg {
	unsigned int reg_base;
	unsigned int reg_off;
	const char *reg_name;
};
#define IO_BUFFER_LENGTH		40

static int map_io_regs(void)
{
	int i;
	struct device_node *np = NULL;
	char *io_buffer;
	io_buffer = (char *)kmalloc(IO_BUFFER_LENGTH * sizeof(char), GFP_KERNEL);

	for (i = 0; i < 27; i++) {
		memset(io_buffer, 0, IO_BUFFER_LENGTH);
		sprintf(io_buffer, "arm,primecell%d", i);
		np = of_find_compatible_node(NULL, NULL, io_buffer);
		sysreg_base.gpio_base[i] = of_iomap(np, 0);
		if (!sysreg_base.gpio_base[i])
			goto err;
	}
	for (i = 0; i < 4; i++) {
		memset(io_buffer, 0, IO_BUFFER_LENGTH);
		sprintf(io_buffer, "pinctrl-single%d", i);
		np = of_find_compatible_node(NULL, NULL, io_buffer);
		sysreg_base.ioc_base[i] = of_iomap(np, 0);
		if (!sysreg_base.ioc_base[i])
			goto err;
	}
	kfree(io_buffer);
	io_buffer = NULL;
	return 0;
err:

	return -ENODEV;
}

/*map system registers*/
static int map_sysregs(void)
{
	unsigned int uart_idx = 0;
	struct device_node *np = NULL;

	uart_idx = get_console_index();
	switch (uart_idx) {
		case 0:
			np = of_find_compatible_node(NULL, NULL, "arm,pl011");
			break;
		case 6:
			np = of_find_compatible_node(NULL, NULL,  "hisilicon,lowpm_test");
			break;
		default:
			break;
	}

	if (NULL != np) {
		sysreg_base.uart_base = of_iomap(np, 0);
		if (!sysreg_base.uart_base)
			goto err;
	}

	np = of_find_compatible_node(NULL, NULL, "hisilicon,sysctrl");
	sysreg_base.sysctrl_base = of_iomap(np, 0);
	if (!sysreg_base.sysctrl_base)
		goto err;

	np = of_find_compatible_node(NULL, NULL, "hisilicon,pctrl");
	sysreg_base.pctrl_base = of_iomap(np, 0);
	if (!sysreg_base.pctrl_base)
		goto err;

	np = of_find_compatible_node(NULL, NULL, "hisilicon,pmctrl");
	sysreg_base.pmctrl_base = of_iomap(np, 0);
	if (!sysreg_base.pctrl_base)
		goto err;

	np = of_find_compatible_node(NULL, NULL, "hisilicon,crgctrl");
	sysreg_base.crg_base= of_iomap(np, 0);
	if (!sysreg_base.crg_base)
		goto err;

	np = of_find_node_by_name(NULL, "pmic");
	sysreg_base.pmic_base = of_iomap(np, 0);
	if (!sysreg_base.pmic_base)
		goto err;

	np = of_find_compatible_node(NULL, NULL, "hisilicon,HiIPCV230");
	if (!np) {
		pr_err("%s: dts[%s] node not found\n",
				__func__, "hisilicon,HiIPCV230");
		goto err;
	}
	sysreg_base.nsipc_base = of_iomap(np, 0);
	if (!sysreg_base.nsipc_base)
		goto err;

	sysreg_base.reserved_base = ioremap((unsigned long)hisi_reserved_pm_phymem, PM_BUFFER_SIZE);
	if (!sysreg_base.reserved_base)
		goto err;

	if (!map_io_regs())
		return 0;

err:
	printk("hisi_lpregs:map_sysregs failed.\n");
	sysreg_base.uart_base = NULL;
	sysreg_base.sysctrl_base = NULL;
	sysreg_base.pctrl_base = NULL;
	sysreg_base.crg_base = NULL;
	sysreg_base.reserved_base = NULL;
	sysreg_base.nsipc_base = NULL;

	return -ENODEV;
}

/****************************************
 *function: debuguart_reinit
 *description:
 *  reinit debug uart.
 *****************************************/
void debuguart_reinit(void)
{
	unsigned int uart_idx = 0;
	unsigned int uregv = 0;

	if ((console_suspend_enabled == 1) || (sysreg_base.uart_base == NULL))
		return;

	uart_idx = get_console_index();
	if (uart_idx == 6) {
		/* Config necessary IOMG configuration */
		writel(0x1, (sysreg_base.ioc_base + 0xF4));
		/*disable clk*/
		uregv = readl(sysreg_base.sysctrl_base + REG_SCLPM3CLKEN_OFFSET) & (~BIT_CLK_UART_SHIFT);
		writel(uregv, (sysreg_base.sysctrl_base + REG_SCLPM3CLKEN_OFFSET));
		/*enable clk*/
		uregv = readl(sysreg_base.sysctrl_base + REG_SCLPM3CLKEN_OFFSET) | (BIT_CLK_UART_SHIFT);
		writel(uregv, (sysreg_base.sysctrl_base + REG_SCLPM3CLKEN_OFFSET));
		/*reset undo*/
		writel(BIT_RST_UART_SHIFT, (sysreg_base.sysctrl_base + REG_SCLPM3RSTEN_OFFSET));
		writel(BIT_RST_UART_SHIFT, (sysreg_base.sysctrl_base + REG_SCLPM3RSTDIS_OFFSET));
	} else if (uart_idx == 0) {
		/* Config necessary IOMG configuration */
		writel(0x2, (sysreg_base.ioc_base + 0xF0));
		uregv = (1 << (uart_idx + 10));
		writel(uregv, (sysreg_base.crg_base + 0x24));
		/*@ enable clk*/
		writel(uregv, (sysreg_base.crg_base + 0x20));
		/*reset undo*/
		writel(uregv, (sysreg_base.crg_base + 0x78));
		writel(uregv, (sysreg_base.crg_base + 0x7C));
	} else {
		return;
	}

	/*@;disable recieve and send*/
	uregv = 0x0;
	writel(uregv, (sysreg_base.uart_base + 0x30));

	/*@;enable FIFO*/
	uregv = 0x70;
	writel(uregv, (sysreg_base.uart_base + 0x2c));

	/*@;set baudrate*/
	uregv = 0xA;
	writel(uregv, (sysreg_base.uart_base + 0x24));

	uregv = 0x1B;
	writel(uregv, (sysreg_base.uart_base + 0x28));

	/*@;clear buffer*/
	uregv = readl(sysreg_base.uart_base);

	/*@;enable FIFO*/
	uregv = 0x70;
	writel(uregv, (sysreg_base.uart_base + 0x2C));

	/*@;set FIFO depth*/
	uregv = 0x10A;
	writel(uregv, (sysreg_base.uart_base + 0x34));

	uregv = 0x50;
	writel(uregv, (sysreg_base.uart_base + 0x38));

	/*@;enable uart trans*/
	uregv = 0x301;
	writel(uregv, (sysreg_base.uart_base + 0x30));
}


unsigned int proc_mask_to_id(unsigned int mask)
{
	unsigned int i = 0;

	for (i = 0; i < IPC_PROCESSOR_MAX; i++) {
		if (mask & BIT(i)) {
			break;
		}
	}

	return i;
}

void ipc_mbx_irq_show(struct seq_file *s, void __iomem *base, unsigned int mbx)
{
	unsigned int ipc_source = 0;
	unsigned int ipc_dest = 0;
	unsigned int src_id = 0;
	unsigned int dest_id = 0;
	unsigned int i = 0;

	ipc_source = readl(base + IPC_MBX_SOURCE_OFFSET(mbx));
	src_id = proc_mask_to_id(ipc_source);
	if (src_id < AP_IPC_PROC_MAX) {
		/*is ack irq*/
		ipc_dest = readl(base + IPC_MBX_DSTATUS_OFFSET(mbx));
		dest_id = proc_mask_to_id(ipc_dest);
		if (dest_id < IPC_PROCESSOR_MAX) {
			LOWPM_MSG(s, "SR:mailbox%d: ack from %s\n", mbx, processor_name[dest_id]);
		} else {
			LOWPM_MSG(s, "SR:mailbox%d: ack from unknown\n", mbx);
		}
	} else if (src_id < IPC_PROCESSOR_MAX) {
		/*is receive irq*/
		LOWPM_MSG(s, "SR:mailbox%d: send by %s\n", mbx, processor_name[src_id]);
	} else {
		LOWPM_MSG(s, "SR:mailbox%d: src id unknown\n", mbx);
	}

	if ((g_usavedcfg & DEBG_SUSPEND_IPC_DATA_SHOW) > 0) {
		for (i = 0; i < IPC_MBXDATA_MAX; i++) {
			LOWPM_MSG(s, "SR:[MBXDATA%d]:0x%x\n", i, readl(base + IPC_MBXDATA_OFFSET(mbx, i)));
		}
	}
}

void combo_ipc_irq_show(struct seq_file *s, unsigned int nsipc_state)
{
	unsigned int mbx = 0;

	/*ns ipc irq detail show*/
	LOWPM_MSG(s, "SR:nsipc irq state:0x%x\n", nsipc_state);
	if (sysreg_base.nsipc_base == NULL) {
		LOWPM_MSG(s, "SR:nsipc base is NULL\n");
		return;
	}
	for (mbx = 0; mbx <= NSIPC_MAILBOX_MAX; mbx++) {
		if ((nsipc_state & BIT(mbx)) > 0) {
			ipc_mbx_irq_show(s, sysreg_base.nsipc_base, mbx);
		}
	}

}

/****************************************
 *function: pm_status_show
 *description:
 *  show pm status.
 *****************************************/
void pm_status_show(struct seq_file *s)
{
	unsigned int wake_status = 0;
	unsigned int wake_irq = 0;
	unsigned int bit_shift = 0;
	unsigned int i = 0;
	unsigned int ap_irq = 0;


	wake_status = readb(sysreg_base.reserved_base + PM_WAKE_STATUS_OFFSET);
	LOWPM_MSG(s, "SR:wakelock status, ap:%d,hifi:%d, modem:%d, iom3:%d, hotplug:%d.\n",\
			(wake_status & WAKE_STATUS_AP_MASK) ? 1 : 0,\
			(wake_status & WAKE_STATUS_HIFI_MASK) ? 1 : 0,\
			(wake_status & WAKE_STATUS_MODEM_MASK) ? 1 : 0,\
			(wake_status & WAKE_STATUS_IOM3_MASK) ? 1 : 0,\
			(wake_status & WAKE_STATUS_HOTPLUG_MASK) ? 1 : 0);
    power_monitor_report(SOC_VOTE,"ap:%d,hifi:%d, modem:%d, iom3:%d, hotplug:%d",\
                        (wake_status & WAKE_STATUS_AP_MASK) ? 1 : 0,\
                        (wake_status & WAKE_STATUS_HIFI_MASK) ? 1 : 0,\
                        (wake_status & WAKE_STATUS_MODEM_MASK) ? 1 : 0,\
                        (wake_status & WAKE_STATUS_IOM3_MASK) ? 1 : 0,\
                        (wake_status & WAKE_STATUS_HOTPLUG_MASK) ? 1 : 0);

	LOWPM_MSG(s, "SR:system sleeped %d times.\n", readl(sysreg_base.reserved_base + PM_SYS_SLEEP_CNT_OFFSET));
	LOWPM_MSG(s, "SR:wake times, system:%d, woken up by ap:%d, modem:%d, hifi:%d, iom3:%d, lpm3:%d.\n",\
			readl(sysreg_base.reserved_base + PM_SYS_WAKE_CNT_OFFSET),\
			readl(sysreg_base.reserved_base + PM_AP_WAKE_CNT_OFFSET),\
			readl(sysreg_base.reserved_base + PM_MODEM_WAKE_CNT_OFFSET),\
			readl(sysreg_base.reserved_base + PM_HIFI_WAKE_CNT_OFFSET),\
			readl(sysreg_base.reserved_base + PM_IOM3_WAKE_CNT_OFFSET),\
			readl(sysreg_base.reserved_base + PM_LPM3_WAKE_CNT_OFFSET));
	LOWPM_MSG(s, "SR:sr times of sub system, ap:s-%d, r-%d, modem:s-%d, r-%d, hifi:s-%d, r-%d, iom3:s-%d, r-%d.\n",\
			readl(sysreg_base.reserved_base + PM_AP_SUSPEND_CNT_OFFSET),\
			readl(sysreg_base.reserved_base + PM_AP_RESUME_CNT_OFFSET),\
			readl(sysreg_base.reserved_base + PM_MODEM_SUSPEND_CNT_OFFSET),\
			readl(sysreg_base.reserved_base + PM_MODEM_RESUME_CNT_OFFSET),
			readl(sysreg_base.reserved_base + PM_HIFI_SUSPEND_CNT_OFFSET),\
			readl(sysreg_base.reserved_base + PM_HIFI_RESUME_CNT_OFFSET),\
			readl(sysreg_base.reserved_base + PM_IOM3_SUSPEND_CNT_OFFSET),\
			readl(sysreg_base.reserved_base + PM_IOM3_RESUME_CNT_OFFSET));
    power_monitor_report(AP_SLEEP,"%d",readl(sysreg_base.reserved_base + PM_AP_SUSPEND_CNT_OFFSET));
    power_monitor_report(MODEM_SLEEP,"%d",readl(sysreg_base.reserved_base + PM_MODEM_SUSPEND_CNT_OFFSET));
    power_monitor_report(IOM3_SLEEP,"%d",readl(sysreg_base.reserved_base + PM_IOM3_SUSPEND_CNT_OFFSET));
	LOWPM_MSG(s, "SR:SCINT_STAT:0x%x.\n", readl(sysreg_base.sysctrl_base + SCINT_STAT_OFFSET));

	wake_irq = readl(sysreg_base.reserved_base + PM_WAKE_IRQ_OFFSET);
	for (i = 0; i < WAKE_IRQ_NUM_MAX; i++) {
		bit_shift = (1 << i);
		if ((wake_irq & AP_INT_MASK) & bit_shift) {
			LOWPM_MSG(s, "BIT: %d(AP)\n", i);
		}

		if ((wake_irq & MODEM_INT_MASK) & bit_shift) {
			LOWPM_MSG(s, "BIT: %d(MODEM)\n", i);
		}

		if ((wake_irq & LPM3_INT_MASK) & bit_shift) {
			LOWPM_MSG(s, "BIT: %d(LPM3)\n", i);
		}
	}

	ap_irq = readl(sysreg_base.reserved_base + PM_AP_WAKE_IRQ_OFFSET);
	if (LPM3_IRQ_MAX > ap_irq) {
		LOWPM_MSG(s, "SR:AP WAKE IRQ(LPM3 NVIC): %d (%s)\n", ap_irq, lpm3_irq_name[ap_irq]);
	} else {
		LOWPM_MSG(s, "SR:AP WAKE IRQ(LPM3 NVIC): %d (no exist)\n", ap_irq);
	}
	if (IPC_COMBO_AP == ap_irq) {
		combo_ipc_irq_show(s, readl(sysreg_base.reserved_base + PM_AP_NSIPC_IRQ_OFFSET));
	}
}

void pmu_mcu_status_show(void)
{
	unsigned int pmu_mcu_status  = 0;
	unsigned int i = 0;
	unsigned int j = 0;

	pmu_mcu_status = readl(sysreg_base.reserved_base + PMU_MCU_POWER_STATUS_OFFSET);

	for (i = 0; i < (sizeof(hisi_pmu_idx) / sizeof(hisi_pmu_idx[0])); i++) {
		while (hisi_pmu_idx[j].status != LPM3_DIS)
			j++;
		if (pmu_mcu_status & BIT(i))
			pr_info("PMU:[%s%s]enabled -E\n", hisi_pmu_idx[j].name, (g_usavedcfg & DEBUG_INFO) ? hisi_pmu_idx[j].module : "");
	}
}

void set_wakelock(int iflag)
{
	if ((1 == iflag) && (0 == wake_lock_active(&lowpm_wake_lock)))
		wake_lock(&lowpm_wake_lock);
	else if ((0 == iflag) && (0 != wake_lock_active(&lowpm_wake_lock)))
		wake_unlock(&lowpm_wake_lock);
}

#ifdef CONFIG_DEBUG_FS

#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>

#define MX_BUF_LEN		1024
char g_ctemp[MX_BUF_LEN] = {0};

/*****************************************************************
 * function:    dbg_common_open
 * description:
 *  adapt to the interface.
 ******************************************************************/
static int dbg_common_open(struct inode *inode, struct file *filp)
{
	filp->private_data = inode->i_private;
	return 0;
}

/*****************************************************************
 * function:    dbg_cfg_read
 * description:
 *  show the debug cfg for user.
 ******************************************************************/
static ssize_t dbg_cfg_read(struct file *filp, char __user *buffer,
		size_t count, loff_t *ppos)
{
	if (*ppos >= MX_BUF_LEN)
		return 0;

	if (*ppos + count > MX_BUF_LEN)
		count = MX_BUF_LEN - *ppos;

	memset(g_ctemp, 0, MX_BUF_LEN);

	sprintf(g_ctemp,
			"0x1<<0 enable suspend console\n"
			"0x1<<1 ENABLE IO STATUS SHOW\n"
			"0x1<<2 ENABLE PMU STATUS SHOW\n"
			"0x1<<3 ENABLE IO SET\n"
			"0x1<<4 ENABLE PMU SET\n"
			"0x1<<5 ENABLE SINGLE IO SET\n"
			"0x1<<6 ENABLE 1s RTC wakeup (no support now)\n"
			"0x1<<7 ENABLE 500ms TIMER wakeup (no support now)\n"
			"0x1<<8 ENABLE a wakelock\n"
			"0x1<<9 ENABLE SUSPEND AUDIO\n"
			"0x1<<10 ENABLE CLK STATUS SHOW\n"
			"0x1<<11 ENABLE IPC DATA SHOW\n"
			"0x1<<31 ENABLE DEBUG INFO\n"
			"g_usavedcfg=%x\n", g_usavedcfg);

	if (copy_to_user(buffer, g_ctemp + *ppos, count))
		return -EFAULT;

	*ppos += count;
	return count;
}

/*****************************************************************
 * function:    dbg_cfg_write
 * description:
 *  recieve the configuer of the user.
 ******************************************************************/
static ssize_t dbg_cfg_write(struct file *filp, const char __user *buffer,
		size_t count, loff_t *ppos)
{
	int index = 0;

	memset(g_ctemp, 0, MX_BUF_LEN);

	if (copy_from_user(g_ctemp, buffer, count)) {
		pr_info("error!\n");
		return -EFAULT;
	}

	if (('0' == g_ctemp[0]) && ('x' == g_ctemp[1])) {
		if (sscanf(g_ctemp + 2, "%x", &index))
			g_usavedcfg = index;
		else
			pr_info("sscanf error!\n");
	} else {
		if (sscanf(g_ctemp, "%d", &index))
			g_usavedcfg = index;
		else
			pr_info("sscanf error\n");
	}

	pr_info("%s %d, g_usavedcfg=0x%x\n", __func__, __LINE__, g_usavedcfg);

	/*suspend print enable*/
	if (DEBG_SUSPEND_PRINTK & g_usavedcfg)
		console_suspend_enabled = 0;
	else
		console_suspend_enabled = 1;

	if (DEBG_SUSPEND_WAKELOCK & g_usavedcfg)
		set_wakelock(1);
	else
		set_wakelock(0);

	*ppos += count;

	return count;
}

const struct file_operations dbg_cfg_fops = {
	.owner	= THIS_MODULE,
	.open	= dbg_common_open,
	.read	= dbg_cfg_read,
	.write	= dbg_cfg_write,
};

/*****************************************************************
 * function:    dbg_timer_read
 * description:
 *  show the debug cfg for user.
 ******************************************************************/
static ssize_t dbg_timer_read(struct file *filp, char __user *buffer,
		size_t count, loff_t *ppos)
{
	if (*ppos >= MX_BUF_LEN)
		return 0;

	if (*ppos + count > MX_BUF_LEN)
		count = MX_BUF_LEN - *ppos;

	memset(g_ctemp, 0, MX_BUF_LEN);

	sprintf(g_ctemp, "ENABLE %dms TIMER wakeup\n", g_utimer_inms);

	if (copy_to_user(buffer, g_ctemp + *ppos, count))
		return -EFAULT;

	*ppos += count;
	return count;
}

/*****************************************************************
 * function:    dbg_timer_write
 * description:
 *  recieve the configuer of the user.
 ******************************************************************/
static ssize_t dbg_timer_write(struct file *filp, const char __user *buffer,
		size_t count, loff_t *ppos)
{
	int index = 0;

	memset(g_ctemp, 0, MX_BUF_LEN);

	if (copy_from_user(g_ctemp, buffer, count)) {
		pr_info("error!\n");
		return -EFAULT;
	}

	if (sscanf(g_ctemp, "%d", &index))
		g_utimer_inms = index;
	else
		pr_info("ERRR~\n");

	pr_info("%s %d, g_utimer_inms=%x\n", __func__, __LINE__, g_utimer_inms);

	*ppos += count;

	return count;
}

const struct file_operations dbg_timer_fops = {
	.owner	= THIS_MODULE,
	.open	= dbg_common_open,
	.read	= dbg_timer_read,
	.write	= dbg_timer_write,
};

/*****************************************************************
 * function:    dbg_timer_read
 * description:
 *  show the debug cfg for user.
 ******************************************************************/
static ssize_t dbg_rtc_read(struct file *filp, char __user *buffer,
		size_t count, loff_t *ppos)
{
	if (*ppos >= MX_BUF_LEN)
		return 0;

	if (*ppos + count > MX_BUF_LEN)
		count = MX_BUF_LEN - *ppos;

	memset(g_ctemp, 0, MX_BUF_LEN);

	sprintf(g_ctemp, "ENABLE %dms rtc wakeup\n", g_urtc_ins);

	if (copy_to_user(buffer, g_ctemp + *ppos, count))
		return -EFAULT;

	*ppos += count;
	return count;
}

/*****************************************************************
 * function:    dbg_timer_write
 * description:
 *  recieve the configuer of the user.
 ******************************************************************/
static ssize_t dbg_rtc_write(struct file *filp, const char __user *buffer,
		size_t count, loff_t *ppos)
{
	int index = 0;

	memset(g_ctemp, 0, MX_BUF_LEN);

	if (copy_from_user(g_ctemp, buffer, count)) {
		pr_info("error!\n");
		return -EFAULT;
	}

	if (sscanf(g_ctemp, "%d", &index))
		g_urtc_ins = index;
	else
		pr_info("ERRR~\n");

	pr_info("%s %d, g_urtc_ins=%x\n", __func__, __LINE__, g_urtc_ins);

	*ppos += count;

	return count;
}

const struct file_operations dbg_rtc_fops = {
	.owner	= THIS_MODULE,
	.open	= dbg_common_open,
	.read	= dbg_rtc_read,
	.write	= dbg_rtc_write,
};

/*****************************************************************
 * function:    dbg_pm_status_show
 * description:
 *  show the debug cfg for user.
 ******************************************************************/
static int dbg_pm_status_show(struct seq_file *s, void *unused)
{
	pm_status_show(s);
	return 0;
}

/*****************************************************************
 * function:    dbg_pm_status_open
 * description:
 * adapt to the interface.
 ******************************************************************/
static int dbg_pm_status_open(struct inode *inode, struct file *file)
{
	return single_open(file, dbg_pm_status_show, &inode->i_private);
}

const struct file_operations dbg_pm_status_fops = {
	.owner	= THIS_MODULE,
	.open	= dbg_pm_status_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

#define GPIO_DIR(x)		((x) + 0x400)
#define GPIO_DATA(x, y)		((x) + (1 << (2 + y)))
#define GPIO_BIT(x, y)		((x) << (y))
#define GPIO_IS_SET(x, y)	(((x) >> (y)) & 0x1)

/* check all io status */
void dbg_io_status_show(void)
{
	int i;
	unsigned int uregv, value, data;
	void __iomem *addr, *addr1;

	if (!(g_usavedcfg & DEBG_SUSPEND_IO_SHOW))
		return;

	printk("IO_LIST_LENGTH is %d\n", (int)IO_LIST_LENGTH);
	for (i = 0; i < IO_LIST_LENGTH; i++) {
		uregv = ((hisi_iocfg_lookups[i].ugpiog << 3) + hisi_iocfg_lookups[i].ugpio_bit);

		printk("gpio - %d  owner - %-10s\t", (int)uregv, hisi_iocfg_lookups[i].owner);

		/* show iomg register's value */
		if (hisi_iocfg_lookups[i].uiomg_off != -1) {
			if (uregv <= 164)
				addr = sysreg_base.ioc_base[0] + hisi_iocfg_lookups[i].uiomg_off;
			else
				addr = sysreg_base.ioc_base[1] + hisi_iocfg_lookups[i].uiomg_off;

			value = readl(addr);
			printk("iomg = Func-%d", value);

			if (value != hisi_iocfg_lookups[i].iomg_val) {
				printk(" -E [Func-%d]", hisi_iocfg_lookups[i].iomg_val);
			} else {
				printk("             ");
			}
			printk("\t");
		} else {
			printk("iomg = Null               \t");
		}

		/* show iocg register */
		if (uregv <= 164)
			addr = sysreg_base.ioc_base[2] + hisi_iocfg_lookups[i].uiocg_off;
		else
			addr = sysreg_base.ioc_base[3] + hisi_iocfg_lookups[i].uiocg_off;

		value = readl(addr) & 0x03;
		printk("iocg = %s", pulltype[value]);

		if (value != hisi_iocfg_lookups[i].iocg_val) {
			printk(" -E [%s]", pulltype[hisi_iocfg_lookups[i].iocg_val]);
		} else {
			printk("        ");
		}
		printk("\t");

		/* gpio controller register */
		if (!hisi_iocfg_lookups[i].iomg_val) {
			addr = GPIO_DIR(sysreg_base.gpio_base[hisi_iocfg_lookups[i].ugpiog]);
			addr1 = GPIO_DATA(sysreg_base.gpio_base[hisi_iocfg_lookups[i].ugpiog], hisi_iocfg_lookups[i].ugpio_bit);

			value = GPIO_IS_SET(readl(addr), hisi_iocfg_lookups[i].ugpio_bit);
			data = GPIO_IS_SET(readl(addr1), hisi_iocfg_lookups[i].ugpio_bit);
			printk("gpio - %s", value ? "O" : "I ");

			if (value)
				printk("%s", data ? "H" : "L");

			if (value != hisi_iocfg_lookups[i].gpio_dir) {
				printk("     -E [%s", hisi_iocfg_lookups[i].gpio_dir ? "O" : "I");
				if (hisi_iocfg_lookups[i].gpio_dir && (data != hisi_iocfg_lookups[i].gpio_val))
					printk("%s", hisi_iocfg_lookups[i].gpio_val ? "H" : "L");
				printk("]");
			}

			printk("\n");
		} else {
			printk("\n");
		}

	}
}

/*****************************************************************
 * function:    dbg_iomux_read
 * description:
 *  print out he io status on the COM.
 ******************************************************************/
static ssize_t dbg_iomux_read(struct file *filp, char __user *buffer,
		size_t count, loff_t *ppos)
{
	char temp[32] = {0};
	if (!(g_usavedcfg & DEBG_SUSPEND_IO_SHOW))
		return 0;

	if (*ppos >= 32)
		return 0;

	if (*ppos + count > 32)
		count = 32 - *ppos;

	if (copy_to_user(buffer, temp + *ppos, count))
		return -EFAULT;

	*ppos += count;
	dbg_io_status_show();
	return count;
}

const struct file_operations dbg_iomux_fops = {
	.owner	= THIS_MODULE,
	.open	= dbg_common_open,
	.read	= dbg_iomux_read,
};

/*****************************************************************
 * function:    dbg_pmu_status_show
 * description:
 *  show the pmu status.
 ******************************************************************/
void dbg_pmu_status_show(void)
{
	int i = 7, k = 0;
	unsigned char uregv;

	if (!(g_usavedcfg & DEBG_SUSPEND_PMU_SHOW))
		return ;
	printk("[%s] %d enter.\n", __func__, __LINE__);
	printk("PMU_CTRL register value list:\n");
	for (i = PMU_CTRL_BEGIN; i < PMU_CTRL_END; i++) {
		uregv = readl(PMUSSI_REG(i));
		printk("PMU_CTRL 0x%02X=0x%02X", i, uregv);
		printk("\t");

		if (i == pmuregs_lookups[k].ucoffset) {
			printk("[%s]", hisi_pmu_idx[k].name);
			if ((uregv & pmuregs_lookups[k].cmask) == pmuregs_lookups[k].cval) {
				printk("enabled  %s\t", (hisi_pmu_idx[k].status & AP_DIS) ? "-E" : "");
			} else {
				printk("disabled %s\t", (hisi_pmu_idx[k].status & (SYS_CLK_EN | NULL_OWNER | AP_DIS | LPM3_DIS)) ? "" : "-E");
			}
			printk("owner:%s", hisi_pmu_idx[k].module);
			k++;
		}
		printk("\n");
	}

	printk("PMU_IRQ register value list:\n");
	for (i = PMU_IRQ_BEGIN; i < PMU_IRQ_END; i++) {
		uregv = readl(PMUSSI_REG(i));
		printk("PMU_IRQ 0x%02X=0x%02X", i, uregv);
		printk("\n");
	}

	printk("PMU_OCP register value list:\n");
	for (i = PMU_OCP_BEGIN; i < PMU_OCP_END; i++) {
		uregv = readl(PMUSSI_REG(i));
		printk("PMU_OCP 0x%02X=0x%02X", i, uregv);
		printk("\n");
	}

	printk("PMU_RTC register value list:\n");
	for (i = PMU_RTC_BEGIN; i < PMU_RTC_END; i++) {
		uregv = readl(PMUSSI_REG(i));
		printk("PMU_RTC 0x%02X=0x%02X", i, uregv);
		printk("\n");
	}

#if 0
	pr_info("PMU_COUL register value list:\n");
	for (i = PMU_COUL_BEGIN; i < PMU_COUL_END; i++) {
		uregv = readl(PMUSSI_REG(i));
		printk("PMU_COUL 0x%02X=0x%02X", i, uregv);
		printk("\n");
	}
#endif
	printk("[%s] %d leave.\n", __func__, __LINE__);
}

int dbg_pmu_show(struct seq_file *s, void *unused)
{
	dbg_pmu_status_show();

	return 0;
}

/*****************************************************************
 * function:    dbg_pmu_open
 * description:
 *  adapt to the interface.
 ******************************************************************/
static int dbg_pmu_open(struct inode *inode, struct file *file)
{
	return single_open(file, dbg_pmu_show, &inode->i_private);
}

static const struct file_operations debug_pmu_fops = {
	.open		= dbg_pmu_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};


#define NULL_CLOSE	0
#define AP_CLOSE	1
#define LPM3_CLOSE	2
#define	AP_ON		3
#define AO_ON		4
#define ON_OFF		5
#define MODEM_ON	6

#define LP_CLK(_base, _offset, _bit, _clk_name, _status) \
{                               \
	.reg_base = _base, \
	.reg_off = _offset,  \
	.bit_idx    = _bit, \
	.clk_name = _clk_name,  \
	.status = _status,	\
}

static struct lp_clk clk_lookups[] = {
	LP_CLK(SCTRLBASE, 0x038, 31, "clk_ppll0_sscg", NULL_CLOSE),
	LP_CLK(SCTRLBASE, 0x038, 30, "pclk_efusec", AP_ON),
	LP_CLK(SCTRLBASE, 0x038, 29, "clk_aobus", AP_ON),
	LP_CLK(SCTRLBASE, 0x038, 28, "clk_bbpdrx", MODEM_ON),
	LP_CLK(SCTRLBASE, 0x038, 27, "clk_asp_tcxo", LPM3_CLOSE),
	LP_CLK(SCTRLBASE, 0x038, 26, "clk_asp_subsys", LPM3_CLOSE),
	LP_CLK(SCTRLBASE, 0x038, 25, "clk_memrep", AO_ON),
	LP_CLK(SCTRLBASE, 0x038, 24, "clk_sci1", MODEM_ON),
	LP_CLK(SCTRLBASE, 0x038, 23, "clk_sci0", MODEM_ON),
	LP_CLK(SCTRLBASE, 0x038, 22, "pclk_ao_gpio5", NULL_CLOSE),
	LP_CLK(SCTRLBASE, 0x038, 21, "pclk_ao_gpio4", AP_ON),
	LP_CLK(SCTRLBASE, 0x038, 20, "clk_syscnt", LPM3_CLOSE),
	LP_CLK(SCTRLBASE, 0x038, 19, "pclk_syscnt", LPM3_CLOSE),
	LP_CLK(SCTRLBASE, 0x038, 18, "clk_jtag_auth", LPM3_CLOSE),
	LP_CLK(SCTRLBASE, 0x038, 17, "clk_out1", AP_CLOSE),
	LP_CLK(SCTRLBASE, 0x038, 16, "clk_out0", AP_CLOSE),
	LP_CLK(SCTRLBASE, 0x038, 15, "pclk_ao_ioc", AO_ON),
	LP_CLK(SCTRLBASE, 0x038, 14, "pclk_ao_gpio3", AO_ON),
	LP_CLK(SCTRLBASE, 0x038, 13, "pclk_ao_gpio2", AO_ON),
	LP_CLK(SCTRLBASE, 0x038, 12, "pclk_ao_gpio1", AO_ON),
	LP_CLK(SCTRLBASE, 0x038, 11, "pclk_ao_gpio0", AO_ON),
	LP_CLK(SCTRLBASE, 0x038, 10, "clk_timer3", MODEM_ON),
	LP_CLK(SCTRLBASE, 0x038,  9,  "pclk_timer3", MODEM_ON),
	LP_CLK(SCTRLBASE, 0x038,  8,  "clk_timer2", MODEM_ON),
	LP_CLK(SCTRLBASE, 0x038,  7,  "pclk_timer2", MODEM_ON),
	LP_CLK(SCTRLBASE, 0x038,  6,  "clk_timer1", AP_ON),
	LP_CLK(SCTRLBASE, 0x038,  5,  "pclk_timer1", AP_ON),
	LP_CLK(SCTRLBASE, 0x038,  4,  "clk_timer0", MODEM_ON),
	LP_CLK(SCTRLBASE, 0x038,  3,  "pclk_timer0", MODEM_ON),
	LP_CLK(SCTRLBASE, 0x038,  2,  "pclk_rtc1", AP_ON),/* secureos */
	LP_CLK(SCTRLBASE, 0x038,  1,  "pclk_rtc", AP_ON),
	LP_CLK(SCTRLBASE, 0x048,  4,  "clk_bbpdrx_oth", MODEM_ON),
	LP_CLK(SCTRLBASE, 0x048,  3,  "clk_asp_cfg", LPM3_CLOSE),
	LP_CLK(SCTRLBASE, 0x048,  2,  "clk_asp_h2p", LPM3_CLOSE),
	LP_CLK(SCTRLBASE, 0x048,  1,  "clk_asp_x2p", LPM3_CLOSE),
	LP_CLK(SCTRLBASE, 0x048,  0,  "clk_asp_dw_axi", LPM3_CLOSE),
	LP_CLK(SCTRLBASE, 0x108, 4,  "clkgt_asp_hclk", LPM3_CLOSE),
	LP_CLK(SCTRLBASE, 0x108, 3,  "clkgt_aobus", AO_ON),
	LP_CLK(SCTRLBASE, 0x108, 2,  "clkgt_sci", MODEM_ON),
	LP_CLK(SCTRLBASE, 0x108, 1,  "clkgt_hifidsp", LPM3_CLOSE),
	LP_CLK(SCTRLBASE, 0x108, 0,  "clkgt_asp_subsys", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x008, 31,  "clk_vppbus", AP_CLOSE),
	LP_CLK(CRGBASE, 0x008, 30,  "clk_vencbus", AP_CLOSE),
	LP_CLK(CRGBASE, 0x008, 29,  "clk_vdecbus", AP_CLOSE),
	LP_CLK(CRGBASE, 0x008, 26,  "clk_dbgsechsic2sysbus", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x008, 25,  "clk_dbgsechsicbus", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x008, 23,  "clk_dma2cfgbus", AO_ON),
	LP_CLK(CRGBASE, 0x008, 22,  "clk_dmabus", AO_ON),
	LP_CLK(CRGBASE, 0x008, 21,  "clk_vppjpegbus", AP_CLOSE),
	LP_CLK(CRGBASE, 0x008, 15,  "clk_mmc1_peribus", AP_ON),/* FIXME */
	LP_CLK(CRGBASE, 0x008, 14,  "clk_mmc1peri2sysbus", AP_ON),/* FIXME */
	//LP_CLK(CRGBASE, 0x008, 12,  "clk_smmucfg", NULL_CLOSE),
	LP_CLK(CRGBASE, 0x008, 11,  "clk_sys2cfgbus", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x008, 10,  "clk_cfgbus", AO_ON),
	LP_CLK(CRGBASE, 0x008,  9,  "clk_sysbus", AO_ON),
	LP_CLK(CRGBASE, 0x008,  6,  "clk_vcodecbus", AP_CLOSE),
	LP_CLK(CRGBASE, 0x008,  5,  "clk_vcodeccfg", AP_CLOSE),
	LP_CLK(CRGBASE, 0x008,  4,  "clk_ddrbus", AO_ON),
	LP_CLK(CRGBASE, 0x008,  2,  "clk_ddrc", AO_ON),
	LP_CLK(CRGBASE, 0x008,  1,  "clk_ddrphy_b", AO_ON),
	LP_CLK(CRGBASE, 0x008,  0,  "clk_ddrphy", AO_ON),
	LP_CLK(CRGBASE, 0x018, 29, "clk_lpm32cfgbus", AO_ON),
	LP_CLK(CRGBASE, 0x018, 27, "clk_djtag", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018, 26, "clk_socp", AP_ON),/* FIXME */
	LP_CLK(CRGBASE, 0x018, 25, "pclk_timer7", MODEM_ON),
	LP_CLK(CRGBASE, 0x018, 24, "pclk_timer6", MODEM_ON),/* modem and secureos */
	LP_CLK(CRGBASE, 0x018, 23, "pclk_timer5", AP_ON),
	LP_CLK(CRGBASE, 0x018, 22, "pclk_timer4", AP_ON),
	LP_CLK(CRGBASE, 0x018, 21, "pclk_gpio21", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018, 20, "pclk_gpio20", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018, 19, "pclk_gpio19", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018, 18, "pclk_gpio18", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018, 17, "pclk_gpio17", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018, 16,  "pclk_gpio16", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018, 15,  "pclk_gpio15", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018, 14,  "pclk_gpio14", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018, 13,  "pclk_gpio13", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018, 12,  "pclk_gpio12", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018, 11,  "pclk_gpio11", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018, 10,  "pclk_gpio10", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018,  9,  "pclk_gpio9", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018,  8,  "pclk_gpio8", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018,  7,  "pclk_gpio7", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018,  6,  "pclk_gpio6", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018,  5,  "pclk_gpio5", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018,  4,  "pclk_gpio4", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018,  3,  "pclk_gpio3", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018,  2,  "pclk_gpio2", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018,  1,  "pclk_gpio1", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x018,  0,  "pclk_gpio0", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x028, 31, "pclk_pctrl", AO_ON),
	LP_CLK(CRGBASE, 0x028, 27, "clk_i2c4", NULL_CLOSE),
	LP_CLK(CRGBASE, 0x028, 26, "clk_codecssi", AP_CLOSE),
	LP_CLK(CRGBASE, 0x028, 25, "pclk_ioc1", AO_ON),
	LP_CLK(CRGBASE, 0x028, 24, "clk_hkadcssi", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x028, 23, "clk_gic", AP_ON),
	LP_CLK(CRGBASE, 0x028, 22, "clk_adb_mst_a53_1", AP_CLOSE),
	LP_CLK(CRGBASE, 0x028, 21, "clk_adb_mst_a53_0", AP_CLOSE),/* FIXME */
	LP_CLK(CRGBASE, 0x028, 20, "pclk_tsi1", AP_CLOSE),
	LP_CLK(CRGBASE, 0x028, 19,  "pclk_tsi0", AP_CLOSE),
	LP_CLK(CRGBASE, 0x028, 18,  "pclk_tzpc", AP_ON),
	LP_CLK(CRGBASE, 0x028, 17,  "pclk_wd1", AP_ON),
	LP_CLK(CRGBASE, 0x028, 16,  "pclk_wd0", AP_CLOSE),
	LP_CLK(CRGBASE, 0x028, 15,  "clk_uart5", MODEM_ON),
	LP_CLK(CRGBASE, 0x028, 14,  "clk_uart4", ON_OFF),/* FIXME */
	LP_CLK(CRGBASE, 0x028, 13,  "clk_uart3", ON_OFF),/* FIXME */
	LP_CLK(CRGBASE, 0x028, 12,  "clk_uart2", ON_OFF),/* FIXME */
	LP_CLK(CRGBASE, 0x028, 11,  "clk_uart1", ON_OFF),/* FIXME */
	LP_CLK(CRGBASE, 0x028, 10,  "clk_uart0", AP_ON),
	LP_CLK(CRGBASE, 0x028,  9,  "clk_spi1", AP_CLOSE),
	LP_CLK(CRGBASE, 0x028,  8,  "clk_spi0", AP_CLOSE),
	LP_CLK(CRGBASE, 0x028,  7,  "clk_i2c3", AP_CLOSE),
	LP_CLK(CRGBASE, 0x028,  6,  "clk_i2c2", AP_CLOSE),
	LP_CLK(CRGBASE, 0x028,  5,  "clk_i2c1", AP_CLOSE),
	LP_CLK(CRGBASE, 0x028,  4,  "clk_i2c0", AP_CLOSE),
	LP_CLK(CRGBASE, 0x028,  3,  "clk_ipc1", AP_ON),
	LP_CLK(CRGBASE, 0x028,  2,  "clk_ipc0", AP_ON),
	LP_CLK(CRGBASE, 0x028,  1,  "clk_pwm1", AP_CLOSE),
	LP_CLK(CRGBASE, 0x028,  0,  "clk_pwm0", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038, 31, "clk_iom32cfgbus", AP_ON),
	LP_CLK(CRGBASE, 0x038, 30, "clk_ddrcfg", AO_ON),
	LP_CLK(CRGBASE, 0x038, 29, "pclk_hdmiefc", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038, 27, "clk_ispcore", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038, 26, "clk_ispmcu", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038, 25, "clk_ispmipi", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038, 24, "hclk_isp", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038, 23, "aclk_isp", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038, 22, "clk_dphy2", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038, 21, "clk_dphy1", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038, 20, "clk_dphy0", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038, 19, "clk_cec", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038, 18, "pclk_hdmi", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038, 17, "clk_edc0", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038, 16, "clk_edc1", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038, 15, "clk_ldi0", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038, 14, "clk_ldi1", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038, 13, "aclk_dss", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038, 12, "pclk_dss", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038, 11, "clk_vdec", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038, 10, "clk_venc", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038,  9,  "clk_vpp", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038,  5,  "clk_g3d", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x038,  4,  "clk_g3dmt", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x038,  3,  "pclk_g3d", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x038,  2,  "clk_dphy3", AP_CLOSE),
	LP_CLK(CRGBASE, 0x038,  1,  "clk_dmac1", AO_ON),
	LP_CLK(CRGBASE, 0x038,  0,  "clk_dmac0", AO_ON),/* FIXME */
	LP_CLK(CRGBASE, 0x048, 31,  "clk_ppll6_sscg", NULL_CLOSE),
	LP_CLK(CRGBASE, 0x048, 30,  "clk_ppll5_sscg", AP_CLOSE),
	LP_CLK(CRGBASE, 0x048, 29,  "clk_ppll4_sscg", AP_CLOSE),
	LP_CLK(CRGBASE, 0x048, 28,  "clk_ppll3_sscg", AP_CLOSE),
	LP_CLK(CRGBASE, 0x048, 27,  "clk_ppll2_sscg", AP_CLOSE),
	LP_CLK(CRGBASE, 0x048, 26,  "clk_ppll1_sscg", AP_CLOSE),
	LP_CLK(CRGBASE, 0x048, 25,  "clk_apll2_sscg", AP_CLOSE),
	LP_CLK(CRGBASE, 0x048, 24,  "clk_apll1_sscg", AP_CLOSE),
	LP_CLK(CRGBASE, 0x048, 23,  "clk_apll0_sscg", NULL_CLOSE),
	LP_CLK(CRGBASE, 0x048, 22,  "clk_a53_0_tsensor", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x048, 21,  "clk_a53_1_tsensor", LPM3_CLOSE),
	//LP_CLK(CRGBASE, 0x048, 20,  "clk_smmu", NULL_CLOSE),
	LP_CLK(CRGBASE, 0x048, 18,  "clk_mmc2", AP_CLOSE),
	LP_CLK(CRGBASE, 0x048, 17,  "clk_sd", AP_CLOSE),
	LP_CLK(CRGBASE, 0x048, 16,  "clk_emmc", AP_CLOSE),
	LP_CLK(CRGBASE, 0x048, 12,  "clk_secp", AP_CLOSE),
	LP_CLK(CRGBASE, 0x048, 10,  "clk_hsic", AP_CLOSE),
	LP_CLK(CRGBASE, 0x048,  9, "clk_usb2host_ref", AP_CLOSE),
	LP_CLK(CRGBASE, 0x048,  8, "clk_usb2otg_ref", AP_CLOSE),
	LP_CLK(CRGBASE, 0x048,  4, "clk_usbhost12", AP_CLOSE),
	LP_CLK(CRGBASE, 0x048,  3, "clk_usbhost48", AP_CLOSE),
	LP_CLK(CRGBASE, 0x048,  2, "hclk_usb2host", AP_CLOSE),
	LP_CLK(CRGBASE, 0x048,  1, "hclk_usb2otg", AP_CLOSE),
	LP_CLK(CRGBASE, 0x048,  0, "hclk_usb2otg_pmu", AP_CLOSE),
	LP_CLK(CRGBASE, 0x058, 19, "clk_a53_1_hpm", AP_CLOSE),
	//LP_CLK(CRGBASE, 0x058, 18, "cclk_smmu1", NULL_CLOSE),
	//LP_CLK(CRGBASE, 0x058, 17, "bclk_smmu1", NULL_CLOSE),
	//LP_CLK(CRGBASE, 0x058, 16, "cclk_smmu0", NULL_CLOSE),
	//LP_CLK(CRGBASE, 0x058, 15, "bclk_smmu0", NULL_CLOSE),
	LP_CLK(CRGBASE, 0x058, 14, "clk_cci400", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x058, 13, "clk_iom32sysbus", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x058, 12, "clk_peri_hpm", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x058, 11, "clk_mcpuddr_asyn", AO_ON),
	LP_CLK(CRGBASE, 0x058, 10, "clk_ace2acel", AO_ON),
	LP_CLK(CRGBASE, 0x058,  9,  "clk_iomcu", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x058,  8,  "clk_lpmcu", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x058,  7,  "clk_slimbus", LPM3_CLOSE),/* FIXME */
	LP_CLK(CRGBASE, 0x058,  6,  "clk_modem_tsensor", MODEM_ON),
	LP_CLK(CRGBASE, 0x058,  5,  "clk_g3d_tsensor", AP_CLOSE),
	LP_CLK(CRGBASE, 0x058,  4,  "clk_jpeg", AP_CLOSE),
	LP_CLK(CRGBASE, 0x058,  3,  "clk_noc_timeout_extref", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x058,  2,  "clk_modem_subsys", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x058,  1,  "clk_usb2hostbus", AP_ON),/* FIXME */
	LP_CLK(CRGBASE, 0x058,  0,  "clk_dmatsibus", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F0, 15,  "clkgt_vdec", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F0, 14,  "clkgt_ispmcu", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F0, 13,  "clkgt_ispmipi", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F0, 12,  "clkgt_isp", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F0, 11,  "clkgt_out1", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F0, 10,  "clkgt_out0", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F0,  9,  "clkgt_edc1", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F0,  8,  "clkgt_edc0", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F0,  7,  "clkgt_ldi1", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F0,  6,  "clkgt_ldi0", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F0,  4,  "clkgt_lpmcu", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x0F0,  2,  "clkgt_ddr", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x0F0,  1,  "clkgt_g3daxi1", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x0F0,  0,  "clkgt_g3daxi0", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x0F4, 15,  "clkgt_slimbus", LPM3_CLOSE),
	LP_CLK(CRGBASE, 0x0F4, 14,  "pclkgt_hdmi", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F4, 13,  "clkgt_spi", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F4, 12,  "clkgt_uartl", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F4, 11,  "clkgt_uarth", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F4, 10,  "clkgt_modem_uart", MODEM_ON),
	LP_CLK(CRGBASE, 0x0F4,  9,  "clkgt_uicc", AP_ON),/* FIXME */
	LP_CLK(CRGBASE, 0x0F4,  8,  "clkgt_usbhost48", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F4,  7,  "clkgt_hsic", AP_ON),/* FIXME */
	LP_CLK(CRGBASE, 0x0F4,  4,  "clkgt_mmc2", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F4,  3,  "clkgt_sd", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F4,  2,  "clkgt_emmc", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F4,  1,  "clkgt_vpp", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F4,  0,  "clkgt_venc", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F8,  7,  "clkgt_modem_bbe16", MODEM_ON),
	LP_CLK(CRGBASE, 0x0F8,  6,  "clkgt_modem_mcpu", MODEM_ON),
	LP_CLK(CRGBASE, 0x0F8,  4,  "clkgt_jpeg", AP_CLOSE),
	LP_CLK(CRGBASE, 0x0F8,  0,  "clkgt_i2c", AP_ON),
};

#define CLK_LIST_LENGTH      (int)(sizeof(clk_lookups)/sizeof(clk_lookups[0]))

void clk_showone(char *ctrl_name,int index)
{
	u32 regval = 0;
	u32	bitval = 0;
	u32 regoff = 0;
	u32 bit_idx = 0;
	void __iomem *regbase = NULL;

	switch(clk_lookups[index].reg_base) {
		case SCTRLBASE:
			strcpy(ctrl_name, "SYSCTRL");
			regbase = sysreg_base.sysctrl_base;
			break;
		case CRGBASE:
			strcpy(ctrl_name, "CRGPERI");
			regbase = sysreg_base.crg_base;
			break;
		case PMCBASE:
			strcpy(ctrl_name, "PMCCTRL");
			regbase = sysreg_base.pctrl_base;
			break;
		case PMUBASE:
			strcpy(ctrl_name, "PMUCTRL");
			regbase = sysreg_base.pmic_base;
			clk_lookups[index].reg_off =  clk_lookups[index].reg_off << 2;
			break;
	}

	regoff = clk_lookups[index].reg_off;
	bit_idx = clk_lookups[index].bit_idx;
	regval = readl(regbase + regoff);
	bitval = regval & BIT(bit_idx);

	printk("[%s] offset:0x%x regval:0x%x bit_idx:%s%d state:%d %s",
			ctrl_name, regoff, regval, bit_idx > 9 ? "" : "0", bit_idx, bitval ? 1 : 0, clk_lookups[index].clk_name );

	if (bitval) {
		if ((NULL_CLOSE == clk_lookups[index].status) || (AP_CLOSE == clk_lookups[index].status))
			printk(" -E[OFF]");
	} else {
		if ((AP_ON == clk_lookups[index].status) || (AO_ON == clk_lookups[index].status))
			printk(" -E[ON]");
	}
	if (ON_OFF == clk_lookups[index].status)
		printk("[FIXME: on or off]");
	if (MODEM_ON == clk_lookups[index].status)
		printk("[MODEM: on or off]");

	printk("\n");
}

#define SYS_REG(_base, _offset, _reg_name) \
{                               \
	.reg_base = _base, \
	.reg_off = _offset,  \
	.reg_name = _reg_name,  \
}
static struct sys_reg sysreg_lookups[] = {
	/*sysctrlregs*/
	SYS_REG(SCTRLBASE, 0x020, "SCPPLLCTRL0"),
	SYS_REG(SCTRLBASE, 0x024, "SCPPLLCTRL1"),
	SYS_REG(SCTRLBASE, 0x028, "SCPPLLSSCCTRL"),
	SYS_REG(SCTRLBASE, 0x02C, "SCPPLLSTAT"),
	SYS_REG(SCTRLBASE, 0x038, "SCPERCLKEN0"),
	SYS_REG(SCTRLBASE, 0x048, "SCPERCLKEN1"),
	SYS_REG(SCTRLBASE, 0x088, "SCPERRSTSTAT0"),
	SYS_REG(SCTRLBASE, 0x0C8, "SCISOSTAT"),
	SYS_REG(SCTRLBASE, 0x0D8, "SCPWRSTAT"),
	SYS_REG(SCTRLBASE, 0x100, "SCCLKDIV0"),
	SYS_REG(SCTRLBASE, 0x104, "SCCLKDIV1"),
	SYS_REG(SCTRLBASE, 0x108, "SCCLKDIV2"),
	SYS_REG(SCTRLBASE, 0x21C, "SCPERSTATUS0"),
	SYS_REG(SCTRLBASE, 0x220, "SCPERSTATUS1"),
	SYS_REG(SCTRLBASE, 0x224, "SCPERSTATUS2"),
	SYS_REG(SCTRLBASE, 0x228, "SCPERSTATUS3"),
	SYS_REG(SCTRLBASE, 0x22C, "SCPERSTATUS4"),
	SYS_REG(SCTRLBASE, 0x300, "SCDEEPSLEEPED"),
	SYS_REG(SCTRLBASE, 0x304, "SCMRBBUSYSTAT"),
	SYS_REG(SCTRLBASE, 0x314, "SCBAKDATA0"),
	SYS_REG(SCTRLBASE, 0x318, "SCBAKDATA1"),
	SYS_REG(SCTRLBASE, 0x31C, "SCBAKDATA2"),
	SYS_REG(SCTRLBASE, 0x320, "SCBAKDATA3"),
	SYS_REG(SCTRLBASE, 0x324, "SCBAKDATA4"),
	SYS_REG(SCTRLBASE, 0x328, "SCBAKDATA5"),
	SYS_REG(SCTRLBASE, 0x32C, "SCBAKDATA6"),
	SYS_REG(SCTRLBASE, 0x330, "SCBAKDATA7"),
	SYS_REG(SCTRLBASE, 0x334, "SCBAKDATA8"),
	SYS_REG(SCTRLBASE, 0x338, "SCBAKDATA9"),
	SYS_REG(SCTRLBASE, 0x33C, "SCBAKDATA10"),
	SYS_REG(SCTRLBASE, 0x340, "SCBAKDATA11"),
	SYS_REG(SCTRLBASE, 0x344, "SCBAKDATA12"),
	SYS_REG(SCTRLBASE, 0x348, "SCBAKDATA13"),
	SYS_REG(SCTRLBASE, 0x34C, "SCBAKDATA14"),
	SYS_REG(SCTRLBASE, 0x350, "SCBAKDATA15"),
	SYS_REG(SCTRLBASE, 0x450, "SCINT_GATHER_STAT"),
	SYS_REG(SCTRLBASE, 0x454, "SCINT_MASK"),
	SYS_REG(SCTRLBASE, 0x458, "SCINT_STAT"),
	SYS_REG(SCTRLBASE, 0x45C, "SCDRX_INT_CFG"),
	SYS_REG(SCTRLBASE, 0x460, "SCLPM3WFI_INT_CLR"),
	SYS_REG(SCTRLBASE, 0x48C, "SCMALIBYPCFG"),
	SYS_REG(SCTRLBASE, 0x500, "SCLPM3CLKEN"),
	SYS_REG(SCTRLBASE, 0x50C, "SCLPM3RSTSTAT"),
	SYS_REG(SCTRLBASE, 0x510, "SCLPM3CTRL"),
	SYS_REG(SCTRLBASE, 0x514, "SCLPM3STAT"),
	SYS_REG(SCTRLBASE, 0x518, "SCLPM3RAMCTRL"),
	SYS_REG(SCTRLBASE, 0x530, "SCBBPDRXSTAT0"),
	SYS_REG(SCTRLBASE, 0x550, "SCA7_EVENT_MASK"),
	SYS_REG(SCTRLBASE, 0x554, "SCA15_EVENT_MASK"),
	SYS_REG(SCTRLBASE, 0x558, "SCIOM3_EVENT_MASK"),
	SYS_REG(SCTRLBASE, 0x55C, "SCLPM3_EVENT_MASK"),
	SYS_REG(SCTRLBASE, 0x560, "SCMCPU_EVENT_MASK"),
	SYS_REG(SCTRLBASE, 0x564, "SCEVENT_STAT"),
	/*crgperiregs*/
	SYS_REG(CRGBASE, 0x008, "PERCLKEN0"),
	SYS_REG(CRGBASE, 0x018, "PERCLKEN1"),
	SYS_REG(CRGBASE, 0x028, "PERCLKEN2"),
	SYS_REG(CRGBASE, 0x038, "PERCLKEN3"),
	SYS_REG(CRGBASE, 0x048, "PERCLKEN4"),
	SYS_REG(CRGBASE, 0x058, "PERCLKEN5"),
	SYS_REG(CRGBASE, 0x068, "PERRSTSTAT0"),
	SYS_REG(CRGBASE, 0x074, "PERRSTSTAT1"),
	SYS_REG(CRGBASE, 0x080, "PERRSTSTAT2"),
	SYS_REG(CRGBASE, 0x08C, "PERRSTSTAT3"),
	SYS_REG(CRGBASE, 0x098, "PERRSTSTAT4"),
	SYS_REG(CRGBASE, 0x0A8, "CLKDIV0"),
	SYS_REG(CRGBASE, 0x0AC, "CLKDIV1"),
	SYS_REG(CRGBASE, 0x0B0, "CLKDIV2"),
	SYS_REG(CRGBASE, 0x0B4, "CLKDIV3"),
	SYS_REG(CRGBASE, 0x0B8, "CLKDIV4"),
	SYS_REG(CRGBASE, 0x0BC, "CLKDIV5"),
	SYS_REG(CRGBASE, 0x0C0, "CLKDIV6"),
	SYS_REG(CRGBASE, 0x0C4, "CLKDIV7"),
	SYS_REG(CRGBASE, 0x0C8, "CLKDIV8"),
	SYS_REG(CRGBASE, 0x0CC, "CLKDIV9"),
	SYS_REG(CRGBASE, 0x0D0, "CLKDIV10"),
	SYS_REG(CRGBASE, 0x0D4, "CLKDIV11"),
	SYS_REG(CRGBASE, 0x0D8, "CLKDIV12"),
	SYS_REG(CRGBASE, 0x0DC, "CLKDIV13"),
	SYS_REG(CRGBASE, 0x0E0, "CLKDIV14"),
	SYS_REG(CRGBASE, 0x0E4, "CLKDIV15"),
	SYS_REG(CRGBASE, 0x0E8, "CLKDIV16"),
	SYS_REG(CRGBASE, 0x0EC, "CLKDIV17"),
	SYS_REG(CRGBASE, 0x0F0, "CLKDIV18"),
	SYS_REG(CRGBASE, 0x0F4, "CLKDIV19"),
	SYS_REG(CRGBASE, 0x0F8, "CLKDIV20"),
	SYS_REG(CRGBASE, 0x114, "PER_STAT1"),
	SYS_REG(CRGBASE, 0x118, "PER_STAT2"),
	SYS_REG(CRGBASE, 0x11C, "PER_STAT3"),
	SYS_REG(CRGBASE, 0x120, "PERCTRL0"),
	SYS_REG(CRGBASE, 0x124, "PERCTRL1"),
	SYS_REG(CRGBASE, 0x128, "PERCTRL2"),
	SYS_REG(CRGBASE, 0x12C, "PERCTRL3"),
	SYS_REG(CRGBASE, 0x130, "PERCTRL4"),
	SYS_REG(CRGBASE, 0x134, "PERCTRL5"),
	SYS_REG(CRGBASE, 0x138, "PERCTRL6"),
	SYS_REG(CRGBASE, 0x140, "PERTIMECTRL"),
	SYS_REG(CRGBASE, 0x14C, "ISOSTAT"),
	SYS_REG(CRGBASE, 0x158, "PERPWRSTAT"),
	SYS_REG(CRGBASE, 0x160, "A7CLKEN"),
	SYS_REG(CRGBASE, 0x16C, "A7RSTSTAT"),
	SYS_REG(CRGBASE, 0x170, "A7LPILPCTRL"),
	SYS_REG(CRGBASE, 0x174, "A7ADBLPSTAT"),
	SYS_REG(CRGBASE, 0x178, "A7CTRL0"),
	SYS_REG(CRGBASE, 0x17C, "A7CTRL1"),
	SYS_REG(CRGBASE, 0x180, "A7STAT"),
	SYS_REG(CRGBASE, 0x190, "A15CLKEN"),
	SYS_REG(CRGBASE, 0x19C, "A15RSTSTAT"),
	SYS_REG(CRGBASE, 0x1A0, "A15LPILPCTRL"),
	SYS_REG(CRGBASE, 0x1A4, "A15ADBLPCTRL"),
	SYS_REG(CRGBASE, 0x1A8, "A15CTRL0"),
	SYS_REG(CRGBASE, 0x1AC, "A15CTRL1"),
	SYS_REG(CRGBASE, 0x1B0, "A15STAT"),
	SYS_REG(CRGBASE, 0x1C4, "CORESIGHTLPSTAT"),
	SYS_REG(CRGBASE, 0x1C8, "CORESIGHTCTRL0"),
	SYS_REG(CRGBASE, 0x1CC, "CORESIGHTETFINT"),
	SYS_REG(CRGBASE, 0x1D0, "CORESIGHTETRINT"),
	SYS_REG(CRGBASE, 0x1E0, "IOM3CLKEN"),
	SYS_REG(CRGBASE, 0x1EC, "IOM3RSTSTAT"),
	SYS_REG(CRGBASE, 0x1F0, "IOM3CTRL"),
	SYS_REG(CRGBASE, 0x1F4, "IOM3STAT"),
	SYS_REG(CRGBASE, 0x200, "ADB400STAT"),
	SYS_REG(CRGBASE, 0x204, "SMMUCTL"),
	SYS_REG(CRGBASE, 0x208, "CCI400CTRL0"),
	SYS_REG(CRGBASE, 0x20C, "CCI400CTRL1"),
	SYS_REG(CRGBASE, 0x210, "CCI400STAT"),
	SYS_REG(CRGBASE, 0x1C8, "CORESIGHTCTRL0"),
	SYS_REG(CRGBASE, 0x1C8, "CORESIGHTCTRL0"),
	/*pmctrlregs*/
	SYS_REG(PMCBASE, 0x004, "PMCINTSTAT"),
	SYS_REG(PMCBASE, 0x00C, "PMCSTATUS"),
	SYS_REG(PMCBASE, 0x010, "APLL0CTRL0"),
	SYS_REG(PMCBASE, 0x014, "APLL0CTRL1"),
	SYS_REG(PMCBASE, 0x018, "APLL1CTRL0"),
	SYS_REG(PMCBASE, 0x01C, "APLL1CTRL1"),
	SYS_REG(PMCBASE, 0x020, "APLL2CTRL0"),
	SYS_REG(PMCBASE, 0x024, "APLL2CTRL1"),
	SYS_REG(PMCBASE, 0x038, "PPLL1CTRL0"),
	SYS_REG(PMCBASE, 0x03C, "PPLL1CTRL1"),
	SYS_REG(PMCBASE, 0x040, "PPLL2CTRL0"),
	SYS_REG(PMCBASE, 0x044, "PPLL2CTRL1"),
	SYS_REG(PMCBASE, 0x048, "PPLL3CTRL0"),
	SYS_REG(PMCBASE, 0x04C, "PPLL3CTRL1"),
	SYS_REG(PMCBASE, 0x070, "APLL0SSCCTRL"),
	SYS_REG(PMCBASE, 0x074, "APLL1SSCCTRL"),
	SYS_REG(PMCBASE, 0x078, "APLL2SSCCTRL"),
	SYS_REG(PMCBASE, 0x084, "PPLL1SSCCTRL"),
	SYS_REG(PMCBASE, 0x088, "PPLL2SSCCTRL"),
	SYS_REG(PMCBASE, 0x08C, "PPLL3SSCCTRL"),
	/*pctrlregs*/
	SYS_REG(PCTRLBASE, 0x408, "RESOURCE0_LOCK_ST"),
	SYS_REG(PCTRLBASE, 0x414, "RESOURCE1_LOCK_ST"),
	SYS_REG(PCTRLBASE, 0x420, "RESOURCE2_LOCK_ST"),
	SYS_REG(PCTRLBASE, 0x42C, "RESOURCE3_LOCK_ST"),
	SYS_REG(PCTRLBASE, 0x808, "RESOURCE4_LOCK_ST"),
	SYS_REG(PCTRLBASE, 0x814, "RESOURCE5_LOCK_ST"),
	SYS_REG(PCTRLBASE, 0x820, "RESOURCE6_LOCK_ST"),
	SYS_REG(PCTRLBASE, 0x82C, "RESOURCE7_LOCK_ST"),
};

#define SYSREG_LIST_LENGTH      (int)(sizeof(sysreg_lookups)/sizeof(sysreg_lookups[0]))

void sysreg_showone(char *ctrl_name,int index)
{
	u32 regval = 0;
	u32 regoff = 0;
	void __iomem *regbase = NULL;

	switch(sysreg_lookups[index].reg_base) {
		case SCTRLBASE:
			strcpy(ctrl_name, "SYSCTRL");
			regbase = sysreg_base.sysctrl_base;
			break;
		case CRGBASE:
			strcpy(ctrl_name, "CRGPERI");
			regbase = sysreg_base.crg_base;
			break;
		case PCTRLBASE:
			strcpy(ctrl_name, "PCTRL");
			regbase = sysreg_base.pctrl_base;
			break;
		case PMCBASE:
			strcpy(ctrl_name, "PMCTRL");
			regbase = sysreg_base.pmctrl_base;
			break;
		case PMUBASE:
			strcpy(ctrl_name, "PMUCTRL");
			regbase = sysreg_base.pmic_base;
			sysreg_lookups[index].reg_off =  sysreg_lookups[index].reg_off << 2;
			break;
		default:
			pr_err("%s not exist the ip defined field\n",
					__func__);
			return;
	}

	regoff = sysreg_lookups[index].reg_off;
	regval = readl(regbase + regoff);

	pr_info("[%7s]%s [%17s]%s [0x%3x]%s  val = 0x%x\n",
			ctrl_name,"", sysreg_lookups[index].reg_name, "", regoff, "", regval);

}

void hisi_sysregs_dump(void)
{
	int i = 0;
	char *ctrl_name ;

	pr_info("[%s] %d enter.\n", __func__, __LINE__);

	ctrl_name = kzalloc(sizeof(char) * 10, GFP_ATOMIC);
	if (!ctrl_name) {
		pr_err("Cannot allocate ctrl_name.\n");
		return ;
	}

	for (i = 0; i < SYSREG_LIST_LENGTH; i++)
		sysreg_showone(ctrl_name,i);

	kfree(ctrl_name);
	ctrl_name = NULL;

	pr_info("[%s] %d leave.\n", __func__, __LINE__);
}
EXPORT_SYMBOL_GPL(hisi_sysregs_dump);

/*****************************************************************
 * function:    dbg_clk_status_show
 * description:
 *  show the clk status.
 ******************************************************************/
void dbg_clk_status_show(void)
{
	int i = 0;
	char *ctrl_name;

	if (!(g_usavedcfg & DEBG_SUSPEND_CLK_SHOW))
		return ;
	pr_info("[%s] %d enter.\n", __func__, __LINE__);

	ctrl_name = kzalloc(sizeof(char) * 10, GFP_KERNEL);
	if (!ctrl_name) {
		pr_err("Cannot allocate clk_name.\n");
		return ;
	}

	for (i = 0; i < CLK_LIST_LENGTH; i++)
		clk_showone(ctrl_name,i);

	kfree(ctrl_name);

	pr_info("[%s] %d leave.\n", __func__, __LINE__);
}

static int dbg_clk_show(struct seq_file *s, void *unused)
{
	dbg_clk_status_show();

	return 0;
}

/*****************************************************************
 * function:    dbg_clk_open
 * description:
 *  adapt to the interface.
 ******************************************************************/
static int dbg_clk_open(struct inode *inode, struct file *file)
{
	return single_open(file, dbg_clk_show, &inode->i_private);
}

static const struct file_operations debug_clk_fops = {
	.open		= dbg_clk_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

/*****************************************************************
 * function:    lowpm_test_probe
 * description:
 *  driver interface.
 ******************************************************************/
static int lowpm_test_probe(struct platform_device *pdev)
{
	int status = 0;
	struct dentry *pdentry;

	pr_info("[%s] %d enter.\n", __func__, __LINE__);

	g_suspended = 0;

	/*default timer0 wakeup time 500ms*/
	g_utimer_inms = 200;

	/*default rtc wakeup time in 1s*/
	g_urtc_ins = 1;

	map_sysregs();

	wake_lock_init(&lowpm_wake_lock, WAKE_LOCK_SUSPEND, "lowpm_test");

#ifndef CONFIG_HI3XXX_SR_DEBUG_SLEEP
	set_wakelock(1);
#endif

	pdentry = debugfs_create_dir("lowpm_test", NULL);
	if (!pdentry) {
		pr_info("%s %d error can not create debugfs lowpm_test.\n", __func__, __LINE__);
		return -ENOMEM;
	}

	(void) debugfs_create_file("clk", DEBUG_FILE_MODE, pdentry, NULL, &debug_clk_fops);

	(void) debugfs_create_file("pmu", DEBUG_FILE_MODE, pdentry, NULL, &debug_pmu_fops);

	(void) debugfs_create_file("io", DEBUG_FILE_MODE, pdentry, NULL, &dbg_iomux_fops);

	(void) debugfs_create_file("cfg", DEBUG_FILE_MODE, pdentry, NULL, &dbg_cfg_fops);

	(void) debugfs_create_file("timer", DEBUG_FILE_MODE, pdentry, NULL, &dbg_timer_fops);

	(void) debugfs_create_file("rtc", DEBUG_FILE_MODE, pdentry, NULL, &dbg_rtc_fops);

	(void) debugfs_create_file("debug_sr", DEBUG_FILE_MODE, pdentry, NULL, &dbg_pm_status_fops);

	pr_info("[%s] %d leave.\n", __func__, __LINE__);

	return status;
}

/*****************************************************************
 * function:    lowpm_test_remove
 * description:
 *  driver interface.
 ******************************************************************/
static int lowpm_test_remove(struct platform_device *pdev)
{
	return 0;
}

#ifdef CONFIG_PM
static int lowpm_test_suspend(struct platform_device *pdev,
		pm_message_t state)
{
	g_suspended = 1;
	return 0;
}

static int lowpm_test_resume(struct platform_device *pdev)
{
	g_suspended = 0;
	return 0;
}
#else
#define lowpm_test_suspend	NULL
#define lowpm_test_resume	NULL
#endif

#define MODULE_NAME		"hisilicon,lowpm_test"

static const struct of_device_id lowpm_test_match[] = {
	{ .compatible = MODULE_NAME },
	{},
};

static struct platform_driver lowpm_test_drv = {
	.probe		= lowpm_test_probe,
	.remove		= lowpm_test_remove,
	.suspend	= lowpm_test_suspend,
	.resume		= lowpm_test_resume,
	.driver = {
		.name	= MODULE_NAME,
		.of_match_table = of_match_ptr(lowpm_test_match),
	},
};

static int __init lowpmreg_init(void)
{
	return platform_driver_register(&lowpm_test_drv);
}

static void __exit lowpmreg_exit(void)
{
	platform_driver_unregister(&lowpm_test_drv);
}
#endif /*CONFIG_DEBUG_FS*/

#else

static int __init lowpmreg_init(void)
{
	return 0;
}

static void __exit lowpmreg_exit(void)
{
}

#endif

module_init(lowpmreg_init);
module_exit(lowpmreg_exit);
