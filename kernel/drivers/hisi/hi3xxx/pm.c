#include <linux/init.h>
#include <linux/suspend.h>
#include <linux/syscore_ops.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/err.h>
#include <asm/suspend.h>
#include <linux/platform_device.h>
#include <linux/cpu_pm.h>
#include <asm/cputype.h>
#include <linux/hw_power_monitor.h>

#define POWER_STATE_TYPE_SYS_SUSPEND	3
/*line -emacro(750,*)*/
/*lint -e750*/
#define REG_SCBAKDATA8_OFFSET			(0x334)
#define REG_SCBAKDATA9_OFFSET			(0x338)


#define CPUIDLE_FLAG_REG(cluster)		((cluster == 0) ? REG_SCBAKDATA8_OFFSET :  REG_SCBAKDATA9_OFFSET)
/*lint +e750*/
/*line +emacro(750,*)*/

#define AP_SUSPEND_FLAG					BIT(16)

#define GIC_ENABLE_OFFSET					(0x100)
#define GIC_PENDING_OFFSET					(0x200)
#define IRQ_NUM_MAX		(320)
#define IRQ_GROUP_MAX		(10)
#define IRQ_NUM_PER_WORD	(32)

#define	AO_GPIO_GROUP		(22)
#define	AO_GPIO_IRQ_BASE	(130)

#define NO_SEQFILE 0

extern void pm_status_show(struct seq_file *s);
extern void pm_nvic_pending_dump(void);
extern void pmu_mcu_status_show(void);
#ifdef CONFIG_HI3XXX_SR_DEBUG
extern void dbg_io_status_show(void);
extern void dbg_pmu_status_show(void);
extern void dbg_clk_status_show(void);
#endif

static void __iomem *sysctrl_base;
static void __iomem *g_enable_base;
static void __iomem *g_pending_base;
static void __iomem *crgctrl_base_addr = NULL;
static unsigned int g_enable_value[10];
static void __iomem *ao_gpio_base[5];

char *irq_name[IRQ_NUM_MAX] = {
	"IPI_RESCHEDULE",	/* 0 */
	"IPI_CALL_FUNC",	/* 1 */
	"IPI_CALL_FUNC_SINGLE",	/* 2 */
	"IPI_CPU_STOP",		/* 3 */
	"IPI_TIMER",		/* 4 */
	NULL,		/* 5 */
	NULL,		/* 6 */
	NULL,		/* 7 */
	NULL,		/* 8 */
	NULL,		/* 9 */
	NULL,		/* 10 */
	NULL,		/* 11 */
	NULL,		/* 12 */
	NULL,		/* 13 */
	NULL,		/* 14 */
	NULL,		/* 15 */
	NULL,		/* 16 */
	NULL,		/* 17 */
	NULL,		/* 18 */
	NULL,		/* 19 */
	NULL,		/* 20 */
	NULL,		/* 21 */
	NULL,		/* 22 */
	NULL,		/* 23 */
	NULL,		/* 24 */
	"Virtual maintenance interrupt",	/* 25 */
	"Hypervisor timer",			/* 26 */
	"Virtual timer",			/* 27 */
	"Legacy FIQ signal",			/* 28 */
	"Secure physical timer",		/* 29 */
	"Non-secure physical timer",		/* 30 */
	"Legacy IRQ signal",			/* 31 */
	"a53_0_interr",	/* 32 */
	"a53_0_exterr",	/* 33 */
	"a53_0_pmu0",	/* 34 */
	"a53_0_pmu1",	/* 35 */
	"a53_0_pmu2",	/* 36 */
	"a53_0_pmu3",	/* 37 */
	"a53_0_cti0",	/* 38 */
	"a53_0_cti1",	/* 39 */
	"a53_0_cti2",	/* 40 */
	"a53_0_cti3",	/* 41 */
	"a53_0_COMMRX0",	/* 42 */
	"a53_0_COMMRX1",	/* 43 */
	"a53_0_COMMRX2",	/* 44 */
	"a53_0_COMMRX3",	/* 45 */
	"a53_0_COMMTX0",	/* 46 */
	"a53_0_COMMTX1",	/* 47 */
	"a53_0_COMMTX2",	/* 48 */
	"a53_0_COMMTX3",	/* 49 */
	"a53_1_interr",	/* 50 */
	"a53_1_exterr",	/* 51 */
	"a53_1_pmu0",	/* 52 */
	"a53_1_pmu1",	/* 53 */
	"a53_1_pmu2",	/* 54 */
	"a53_1_pmu3",	/* 55 */
	"a53_1_cti0",	/* 56 */
	"a53_1_cti1",	/* 57 */
	"a53_1_cti2",	/* 58 */
	"a53_1_cti3",	/* 59 */
	"a53_1_COMMRX0",	/* 60 */
	"a53_1_COMMRX1",	/* 61 */
	"a53_1_COMMRX2",	/* 62 */
	"a53_1_COMMRX3",	/* 63 */
	"a53_1_COMMTX0",	/* 64 */
	"a53_1_COMMTX1",	/* 65 */
	"a53_1_COMMTX2",	/* 66 */
	"a53_1_COMMTX3",	/* 67 */
	"TIME00",	/* 68 */
	"TIME01",	/* 69 */
	"TIME10",	/* 70 */
	"TIME11",	/* 71 */
	"TIME20",	/* 72 */
	"TIME21",	/* 73 */
	"TIME30",	/* 74 */
	"TIME31",	/* 75 */
	"TIME40",	/* 76 */
	"TIME41",	/* 77 */
	"TIME50",	/* 78 */
	"TIME51",	/* 79 */
	"TIME60",	/* 80 */
	"TIME61",	/* 81 */
	"TIME70",	/* 82 */
	"TIME71",	/* 83 */
	"WatchDog0",	/* 84 */
	"WatchDog1",	/* 85 */
	"RTC0",		/* 86 */
	"RTC1",		/* 87 */
	"UART0",	/* 88 */
	"UART1",	/* 89 */
	"UART2",	/* 90 */
	"UART3",	/* 91 */
	"UART4",	/* 92 */
	"UART5",	/* 93 */
	"UART6",	/* 94 */
	"SPI0",		/* 95 */
	"SPI1",		/* 96 */
	"TSI0",		/* 97 */
	"TSI1",		/* 98 */
	"I2C0",		/* 99 */
	"I2C1",		/* 100 */
	"I2C2",		/* 101 */
	"I2C3",		/* 102 */
	"I2C4",		/* 103 */
	"I2C5",		/* 104 */
	"reserved",	/* 105 */
	"reserved",	/* 106 */
	"BLPWM",	/* 107 */
	"GPIO0",	/* 108 */
	"GPIO1",	/* 109 */
	"GPIO2",	/* 110 */
	"GPIO3",	/* 111 */
	"GPIO4",	/* 112 */
	"GPIO5",	/* 113 */
	"GPIO6",	/* 114 */
	"GPIO7",	/* 115 */
	"GPIO8",	/* 116 */
	"GPIO9",	/* 117 */
	"GPIO10",	/* 118 */
	"GPIO11",	/* 119 */
	"GPIO12",	/* 120 */
	"GPIO13",	/* 121 */
	"GPIO14",	/* 122 */
	"GPIO15",	/* 123 */
	"GPIO16",	/* 124 */
	"GPIO17",	/* 125 */
	"GPIO18",	/* 126 */
	"GPIO19",	/* 127 */
	"GPIO20",	/* 128 */
	"GPIO21",	/* 129 */
	"GPIO22",	/* 130 */
	"GPIO23",	/* 131 */
	"GPIO24",	/* 132 */
	"GPIO25",	/* 133 */
	"GPIO26",	/* 134 */
	"reserved",	/* 135 */
	"CoreSight_ETR_Full",	/* 136 */
	"CoreSight_ETF_Full",	/* 137 */
	"CCI400_overflow[3:0]",	/* 138 */
	"CCI400_overflow[4]",	/* 139 */
	"CCI400_err",		/* 140 */
	"SECENG_P",		/* 141 */
	"SECENG_S",		/* 142 */
	"SMMU0_combns",	/* 143 */
	"SMMU0_combs",	/* 144 */
	"SMMU1_combns",	/* 145 */
	"SMMU1_combs",	/* 146 */
	"USB2OTG",		/* 147 */
	"USB2HST",		/* 148 */
	"NANDC",		/* 149 */
	"EMMC",			/* 150 */
	"SD",			/* 151 */
	"SDIO0",		/* 152 */
	"DMAC0",		/* 153 */
	"DMAC_ns0",		/* 154 */
	"NOC-comb",		/* 155 */
	"DDRC",			/* 156 */
	"SCTRL",		/* 157 */
	"PMCTRL",		/* 158 */
	"TSENSOR_a53_0",	/* 159 */
	"TSENSOR_a53_1",	/* 160 */
	"TSENSOR_G3D",	/* 161 */
	"TSENSOR_Modem",/* 162 */
	"ISP",			/* 163 */
	"ISP-mmu",		/* 164 */
	"CSI-p",		/* 165 */
	"CSI-s",		/* 166 */
	"ASP",			/* 167 */
	"ASP-ns",		/* 168 */
	"G3D_JOB",		/* 169 */
	"G3D_MMU",		/* 170 */
	"G3D_GPU",		/* 171 */
	"JPEG",			/* 172 */
	"VPP",			/* 173 */
	"VPP_mmu",		/* 174 */
	"VENC",			/* 175 */
	"VDEC",			/* 176 */
	"DSS-pdp",		/* 177 */
	"DSS-sdp",		/* 178 */
	"DSS-offline",	/* 179 */
	"DSS_mcu_pdp",	/* 180 */
	"DSS_mcu_sdp",	/* 181 */
	"DSS_mcu_offline",	/* 182 */
	"DSS_dsi0",		/* 183 */
	"DSS_dsi1",		/* 184 */
	"DSS_hdmi",		/* 185 */
	"reserved",		/* 186 */
	"reserved",		/* 187 */
	"reserved",		/* 188 */
	"reserved",		/* 189 */
	"VDM_A53_0",		/* 190 */
	"PMC-DDRC-DFS",	/* 191 */
	"PMC-DDRC-CFG", /* 192 */
	"PMC-DVFS-a53_0",	/* 193 */
	"PMC-DVFS-a53_1",	/* 194 */
	"PMC-DVFS-G3D",	/* 195 */
	"PMC-AVS-a53_0",	/* 196 */
	"PMC-AVS-a53_1",   /* 197 */
	"PMC-AVS-G3D",  /* 198 */
	"PMC-AVS-IDLE-a53_0",	/* 199 */
	"PMC-AVS-IDLE-a53_1",	/* 200 */
	"PMC-AVS-IDLE-G3D",	/* 201 */
	"M3_IO_wd",		/* 202 */
	"M3_LP_wd",		/* 203 */
	"reserved",		/* 204 */
	"IPC_S_int0",	/* 205 */
	"IPC_S_int1",	/* 206 */
	"IPC_S_int4",	/* 207 */
	"IPC_S_mbx0",	/* 208 */
	"IPC_S_mbx1",	/* 209 */
	"IPC_S_mbx2",	/* 210 */
	"IPC_S_mbx3",	/* 211 */
	"IPC_S_mbx4",	/* 212 */
	"IPC_S_mbx5",	/* 213 */
	"IPC_S_mbx6",	/* 214 */
	"IPC_S_mbx7",	/* 215 */
	"IPC_S_mbx8",	/* 216 */
	"IPC_S_mbx9",	/* 217 */
	"IPC_S_mbx18",	/* 218 */
	"reserved",	/* 219 */
	"IPC_NS_int0",	/* 220 */
	"IPC_NS_int1",	/* 221 */
	"IPC_NS_int4",	/* 222 */
	"IPC_NS_int5",	/* 223 */
	"IPC_NS_int6",	/* 224 */
	"IPC_NS_mbx0",	/* 225 */
	"IPC_NS_mbx1",	/* 226 */
	"IPC_NS_mbx2",	/* 227 */
	"IPC_NS_mbx3",	/* 228 */
	"IPC_NS_mbx4",	/* 229 */
	"IPC_NS_mbx5",	/* 230 */
	"IPC_NS_mbx6",	/* 231 */
	"IPC_NS_mbx7",	/* 232 */
	"IPC_NS_mbx8",	/* 233 */
	"IPC_NS_mbx9",	/* 234 */
	"IPC_NS_mbx18",	/* 235 */
	"aximon_cpufast_intr|aximon_soc_intr",	/* 236 */
	"MDM_WDOG_intr",	/* 237 */
	"reserved",	/* 238 */
	"ASP-IPC-ARM",	/* 239 */
	"ASP-IPC-MCPU",	/* 240 */
	"ASP-IPC-BBE16",/* 241 */
	"ASP_WD",	/* 242 */
	"ASP_AXI_DLOCK",/* 243 */
	"ASP_DMA_SECURE",	/* 244 */
	"ASP_DMA_SECURE_N",	/* 245 */
	"SCI0",			/* 246 */
	"SCI1",			/* 247 */
	"SOCP0",		/* 248 */
	"SOCP1",		/* 249 */
	"MDM2AP[0]",	/* 250 */
	"MDM2AP[1]",	/* 251 */
	"MDM2AP[2]",	/* 252 */
	"MDM2AP[3]",	/* 253 */
	"GIC_IRQ_OUT[0]",	/* 254 */
	"GIC_IRQ_OUT[1]",	/* 255 */
	"GIC_IRQ_OUT[2]",	/* 256 */
	"GIC_IRQ_OUT[3]",	/* 257 */
	"GIC_IRQ_OUT[4]",	/* 258 */
	"GIC_IRQ_OUT[5]",	/* 259 */
	"GIC_IRQ_OUT[6]",	/* 260 */
	"GIC_IRQ_OUT[7]",	/* 261 */
	"GIC_FIQ_OUT[0]",	/* 262 */
	"GIC_FIQ_OUT[1]",	/* 263 */
	"GIC_FIQ_OUT[2]",	/* 264 */
	"GIC_FIQ_OUT[3]",	/* 265 */
	"GIC_FIQ_OUT[4]",	/* 266 */
	"GIC_FIQ_OUT[5]",	/* 267 */
	"GIC_FIQ_OUT[6]",	/* 268 */
	"GIC_FIQ_OUT[7]", 	/* 269 */
	"UICC_intr",		/* 270 */
	"A53_0_COMMMIRQ0",	/* 271 */
	"A53_0_COMMMIRQ1",	/* 272 */
	"A53_0_COMMMIRQ2",	/* 273 */
	"A53_0_COMMMIRQ3",	/* 274 */
	"A53_1_COMMMIRQ0",	/* 275 */
	"A53_1_COMMMIRQ1",	/* 276 */
	"A53_1_COMMMIRQ2",	/* 277 */
	"A53_1_COMMMIRQ3",	/* 278 */
};

static int pm_ao_gpio_irq_dump(unsigned int irq_num)
{
	int i, group, data;

	group = (int)irq_num - AO_GPIO_IRQ_BASE;
	if (group > 4 || group < 0)
		return -EINVAL;

	data = readl(ao_gpio_base[group] + 0x410)
			& readl(ao_gpio_base[group] + 0x418);

	for (i = 0; i < 8; i++)
		if (data & BIT(i))
			return ((group + AO_GPIO_GROUP) * 8 + i);

	return  -EINVAL;
}

void pm_gic_dump(void)
{
	unsigned int i;

	for (i = 2; i < IRQ_GROUP_MAX; i++)
		g_enable_value[i] = readl(g_enable_base + i * 4);
}

void pm_gic_pending_dump(void)
{
	unsigned int i;
	unsigned int j;
	unsigned int value;
	unsigned int irq;
	int gpio;

	for (i = 2; i < IRQ_GROUP_MAX; i++) {
		value = readl(g_pending_base + i * 4);

		for (j = 0; j < IRQ_NUM_PER_WORD; j++) {
			if ((value & BIT_MASK(j)) && ((value & BIT_MASK(j)) == (g_enable_value[i] & BIT_MASK(j)))) {
				irq = i * IRQ_NUM_PER_WORD + j;
				printk("wake up irq num: %d, irq name: %s", irq, irq_name[irq]);
                power_monitor_report(WAKEUP_IRQ,"%s",irq_name[irq]);
				gpio = pm_ao_gpio_irq_dump(irq);
				if (gpio >= 0)
					printk("(gpio-%d)", gpio);
				printk("\n");
			}
		}
	}
}

void hisi_set_ap_suspend_flag(unsigned int cluster)
{
	unsigned int val;

	/* do not need lock, as the core is only one now. */
	val = readl(sysctrl_base + CPUIDLE_FLAG_REG(cluster));
	val |= AP_SUSPEND_FLAG;
	writel(val, sysctrl_base + CPUIDLE_FLAG_REG(cluster));
}

void hisi_clear_ap_suspend_flag(unsigned int cluster)
{
	unsigned int val;

	/* do not need lock, as the core is only one now. */
	val = readl(sysctrl_base + CPUIDLE_FLAG_REG(cluster));
	val &= ~AP_SUSPEND_FLAG;
	writel(val, sysctrl_base + CPUIDLE_FLAG_REG(cluster));
}

#ifdef CONFIG_HI3XXX_SR_DEBUG
extern void debuguart_reinit(void);
#endif

#define COREPWRACK_MASK (0x7F800)
/*line -emacro(750,*)*/
/*lint -e750*/
#define PERPWRACK_OFFSET (0x15c)
/*lint +e750*/
/*line +emacro(750,*)*/


static int hisi_test_pwrdn_othercores(unsigned int cluster, unsigned int core)
{
	unsigned int pwrack_stat = 0;
	unsigned int mask = 0;
	unsigned int plat_flag = 0;
	struct device_node *np = NULL;

	np = of_find_compatible_node(NULL, NULL, "hisilicon,lowpm_test");
	if (!np) {
		pr_err("%s: hisilicon,lowpm_test No compatible node found\n", __func__);
		return -ENODEV;
	}
	of_property_read_u32_array(np, "sr_plat_flag", &plat_flag, 1);

	if (1 == plat_flag) { /* FOR FPGA */
		if (0 == cluster)
			return (readl(crgctrl_base_addr + 0x318) != 0x6600);
		else
			return (readl(crgctrl_base_addr + 0x278) != 0x6660);
	}
	else {
		/* boot core mask */
		mask = (0x1 << (11 + cluster * 4 + core));
		pwrack_stat = readl(crgctrl_base_addr + PERPWRACK_OFFSET);
		/* non boot core mask */
		mask = COREPWRACK_MASK & (~mask);
		pwrack_stat &= mask;

		return pwrack_stat;
	}
}

static int hisi_3635_pm_enter(suspend_state_t state)
{
	unsigned int cluster = 0;
	unsigned int core = 0;
	unsigned long mpidr = read_cpuid_mpidr();

	pr_err("%s ++\n", __func__);

	cluster = (mpidr >> 8) & 0xff;
	core = mpidr & 0xff;

	pm_gic_dump();

	dbg_io_status_show();
	dbg_pmu_status_show();
	dbg_clk_status_show();

	while (hisi_test_pwrdn_othercores(cluster ,core))
		;
	hisi_set_ap_suspend_flag(cluster);
	cpu_cluster_pm_enter();
	cpu_suspend(POWER_STATE_TYPE_SYS_SUSPEND);
#ifdef CONFIG_HI3XXX_SR_DEBUG
	debuguart_reinit();
#endif
	cpu_cluster_pm_exit();
	hisi_clear_ap_suspend_flag(cluster);

	pm_gic_pending_dump();
	pm_status_show(NO_SEQFILE);
	pmu_mcu_status_show();

	pr_err("%s --\n", __func__);

	return 0;
}

static const struct platform_suspend_ops hi3635_pm_ops = {
	.enter		= hisi_3635_pm_enter,
	.valid		= suspend_valid_only_mem,
};

static int hisi_get_gic_base(void)
{
	void __iomem *hisi_gic_dist_base;
	struct device_node *node;

	node = of_find_compatible_node(NULL, NULL, "arm,cortex-a9-gic");
	if (!node) {
		pr_err("%s: hisilicon,gic No compatible node found\n", __func__);
		return -ENODEV;
	}

	hisi_gic_dist_base = of_iomap(node, 0);
	if (!hisi_gic_dist_base) {
		pr_err("%s: hisilicon,gic_dist_base is NULL\n", __func__);
		return -ENODEV;
	}

	pr_info("hisi_gic_dist_base = %p\n", hisi_gic_dist_base);

	g_enable_base = hisi_gic_dist_base + GIC_ENABLE_OFFSET;
	g_pending_base = hisi_gic_dist_base + GIC_PENDING_OFFSET;

	return 0;
}

static int hisi_get_gpio_regs(void)
{
	int i;
	struct device_node *np = NULL;
	char *io_buffer;

	io_buffer = (char *)kmalloc(40 * sizeof(char), GFP_KERNEL);

	for (i = 0; i < 5; i++) {
		memset(io_buffer, 0, 40);
		sprintf(io_buffer, "arm,primecell%d", i + AO_GPIO_GROUP);
		np = of_find_compatible_node(NULL, NULL, io_buffer);
		if (!np) {
			pr_err("%s: hisilicon,primecell%d No compatible node found\n", __func__, i + AO_GPIO_GROUP);
			return -ENODEV;
		}

		ao_gpio_base[i] = of_iomap(np, 0);
		if (!ao_gpio_base[i])
			goto err;

		of_node_put(np);
	}

	kfree(io_buffer);
	io_buffer = NULL;
	return 0;
err:
	of_node_put(np);
	kfree(io_buffer);
	return -ENODEV;
}

static __init int hi3635_pm_drvinit(void)
{
	struct device_node *np = NULL;

	if (hisi_get_gpio_regs()) {
		pr_err("%s: hisilicon,get gpio base failed!\n", __func__);
		return -ENODEV;
	}

	np = of_find_compatible_node(NULL, NULL, "hisilicon,sysctrl");
	if (!np) {
		pr_err("%s: hisilicon,sysctrl No compatible node found\n", __func__);
		return -ENODEV;
	}

	sysctrl_base = of_iomap(np, 0);
	if (!sysctrl_base) {
		pr_err("%s: hisilicon,sysctrl_base is NULL\n", __func__);
		return -ENODEV;
	}

	of_node_put(np);

	np = of_find_compatible_node(NULL, NULL, "hisilicon,crgctrl");
	if (!np) {
		pr_info("%s: hisilicon,crgctrl No compatible node found\n", __func__);
		return -ENODEV;
	}

	crgctrl_base_addr = of_iomap(np, 0);
	if (!crgctrl_base_addr) {
		pr_err("%s: hisilicon,crgctrl_base_addr is NULL\n", __func__);
		return -ENODEV;
	}

	of_node_put(np);

	if (hisi_get_gic_base()) {
		pr_err("%s: hisilicon,get gic base failed!\n", __func__);
		return -ENODEV;
	}

	suspend_set_ops(&hi3635_pm_ops);

	return 0;
}
arch_initcall(hi3635_pm_drvinit);
