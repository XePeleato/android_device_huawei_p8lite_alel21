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
#include <asm/suspend.h>
#include <asm/mcpm.h>
#include <asm/idmap.h>
#include <asm/cacheflush.h>
#include <asm/cputype.h>
#include <asm/cp15.h>
#include <asm/psci.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/wakelock.h>

//#include "hi6xxx_ipc.h"
#include <linux/hisi/pm/pwrctrl_multi_memcfg.h>
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */
#include "soc_ao_sctrl_interface.h"
#include <linux/hisi/hi6xxx-ipc.h>                   /* For IO_ADDRESS access */
#include <linux/hisi/pm/hi6xxx-power-common.h>

#if defined(CONFIG_SECURE_EXTENSION)
#define PSCI_POWER_STATE_ID     (0)
#define IPC_SR_CMD (0x00000106)
#endif

/*lint -e750*/
#define PWRCTRL_JMP_INSTRUCTION             (0xea00007e)    /*B pc+0x200*/
/*lint +e750*/

/* gic reg offset */
#define GIC_ENABLE_OFFSET	(0x100)
#define GIC_PENDING_OFFSET	(0x200)

#define IRQ_NUM_MAX		(320)
#define IRQ_GROUP_MAX		(10)
#define IRQ_NUM_PER_WORD	(32)

extern void pm_asm_code_copy(void);
#ifdef DEBUG_SLEEP_WAKELOCK
extern void a_wl_rel(void);
#endif
extern void hisi_set_cluster_wfi(unsigned int id);
extern void hisi_cluster_exit_coherence(u32 cluster);
extern int get_cpu_type(void);
extern void hisi_pm_cci_disable(unsigned int id);
extern int g_adfreq_sr_switch;
#ifdef DEBUG_SLEEP_WAKELOCK
static struct wake_lock g_ulTstwakelock;
#endif

char *irq_name[IRQ_NUM_MAX] = {
	"IPI_WAKEUP",		/* 0 */
	"IPI_TIMER",		/* 1 */
	"IPI_RESCHEDULE",	/* 2 */
	"IPI_CALL_FUNC",	/* 3 */
	"IPI_CALL_FUNC_SINGLE",	/* 4 */
	"IPI_CPU_STOP",		/* 5 */
	"IPI_COMPLETION",	/* 6 */
	"IPI_CPU_BACKTRACE",	/* 7 */
	"IPI_NOTIFY_FUNC",	/* 8 */
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
	"MDM2AP[4]",	/* 32 */
	"A15_axierr",	/* 33 */
	"A15_pmu0",	/* 34 */
	"A15_pmu1",	/* 35 */
	"A15_pmu2",	/* 36 */
	"A15_pmu3",	/* 37 */
	"A15_cti0",	/* 38 */
	"A15_cti1",	/* 39 */
	"A15_cti2",	/* 40 */
	"A15_cti3",	/* 41 */
	"A15_COMMRX0",	/* 42 */
	"A15_COMMRX1",	/* 43 */
	"A15_COMMRX2",	/* 44 */
	"A15_COMMRX3",	/* 45 */
	"A15_COMMTX0",	/* 46 */
	"A15_COMMTX1",	/* 47 */
	"A15_COMMTX2",	/* 48 */
	"A15_COMMTX3",	/* 49 */
	"Reserved",	/* 50 */
	"A7_axierr",	/* 51 */
	"A7_pmu0",	/* 52 */
	"A7_pmu1",	/* 53 */
	"A7_pmu2",	/* 54 */
	"A7_pmu3",	/* 55 */
	"A7_cti0",	/* 56 */
	"A7_cti1",	/* 57 */
	"A7_cti2",	/* 58 */
	"A7_cti3",	/* 59 */
	"A7_COMMRX0",	/* 60 */
	"A7_COMMRX1",	/* 61 */
	"A7_COMMRX2",	/* 62 */
	"A7_COMMRX3",	/* 63 */
	"A7_COMMTX0",	/* 64 */
	"A7_COMMTX1",	/* 65 */
	"A7_COMMTX2",	/* 66 */
	"A7_COMMTX3",	/* 67 */
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
	"TSENSOR_A15",	/* 159 */
	"TSENSOR_A7",	/* 160 */
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
	"reserved",		/* 190 */
	"PMC-DDRC-DFS",	/* 191 */
	"PMC-DDRC-CFG", /* 192 */
	"PMC-DVFS-A15",	/* 193 */
	"PMC-DVFS-A7",	/* 194 */
	"PMC-DVFS-G3D",	/* 195 */
	"PMC-AVS-A15",	/* 196 */
	"PMC-AVS-A7",   /* 197 */
	"PMC-AVS-G3D",  /* 198 */
	"PMC-AVS-IDLE-A15",	/* 199 */
	"PMC-AVS-IDLE-A7",	/* 200 */
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
	"MDM2AP[5]",	/* 236 */
	"MDM2AP[6]",	/* 237 */
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
	"GIC_FIQ_OUT[7]"	/* 269 */
};

static void __iomem *g_enable_base;
static void __iomem *g_pending_base;
static void __iomem *g_cci_base;
static void __iomem *g_acpu_sc_base_map;

static void pm_gic_dump(void)
{
	unsigned int i;
	unsigned int j;
	unsigned int value;

	pr_info("============gic enable regs============\n");
	for (i = 0; i < IRQ_GROUP_MAX; i++) {
		value = readl((unsigned)(g_enable_base + i * 4));

		for (j = 0; j < IRQ_NUM_PER_WORD; j++) {
			if (((value >> j) & BIT_MASK(0)) && (irq_name[i * IRQ_NUM_PER_WORD + j] != NULL)){
				pr_info("irq num: %d, irq name: %s\n",
					i * IRQ_NUM_PER_WORD + j, irq_name[i * IRQ_NUM_PER_WORD + j]);
			}
		}
	}

	pr_info("============gic pending regs============\n");
	for (i = 0; i < IRQ_GROUP_MAX; i++) {
		value = readl((unsigned)(g_pending_base + i * 4));

		for (j = 0; j < IRQ_NUM_PER_WORD; j++) {
			if ((value >> j) & BIT_MASK(0)) {
				pr_info("irq num: %d, irq name: %s\n",
					i * IRQ_NUM_PER_WORD + j, irq_name[i * IRQ_NUM_PER_WORD + j]);
			}
		}
	}
}
#if 0
static void coherent_slave_port_ctrl(unsigned int id,unsigned int ctrl)
{
    unsigned int reg_val;
    unsigned int cnt = 0;

    if(0 == id)
    {
        /*1 配置Snoop和DVM特性*/
        writel(ctrl, (unsigned)(g_cci_base + 0x94000));

        /*2 轮询等待Status Register值为0x0*/
        reg_val = readl((unsigned)(g_cci_base + 0x9000c));
        while(0x0 != reg_val){
            reg_val = readl((unsigned)(g_cci_base + 0x9000c));
            if (cnt > 10000)
            {
                pr_err("coherent_slave_port_enable 0 failed!!  \n");
                break;
            }
            cnt++;
        };
    }
    else
    {
        /*1 配置Snoop和DVM特性*/
        writel(ctrl, (unsigned)(g_cci_base + 0x95000));

        /*2 轮询等待Status Register值为0x0*/
        reg_val = readl((unsigned)(g_cci_base + 0x9000c));
        while(0x0 != reg_val){
            reg_val = readl((unsigned)(g_cci_base + 0x9000c));
            if (cnt > 10000)
            {
                pr_err("coherent_slave_port_enable 1 failed!!  \n");
                break;
            }
            cnt++;
        };
    }

    return;
}
#endif
static void coherent_init(void)
{
    /*CCI init*/
    /*使能各Master和Slave接口的 speculative fetch功能*/
    writel(0x180003, (unsigned)(g_cci_base + 0x90004));
    /*使能snoop/DVM/speculative fetch/terminate barrier功能*/
    writel(0x18, (unsigned)(g_cci_base + 0x90000));
}

/*fixme:Cluster上电完成，L2 Cache使能后，启动SMP前*/
static void coherent_slave_port_config(void)
{
    /*unsigned int reg_val = 0;*/

    /*CCI slave port config*/ /*S3和S4都需要配置*/
    /*1 设置Latency目标为128个周期*/
    writel(0x500050, (unsigned)(g_cci_base + 0x94130));
    writel(0x500050, (unsigned)(g_cci_base + 0x95130));

    /*2 设置每256个周期Qos增加1*/
    writel(0x30003, (unsigned)(g_cci_base + 0x94134));
    writel(0x30003, (unsigned)(g_cci_base + 0x95134));

    /*3 设置ACPU最低优先级为1，最高优先级为6*/
    writel(0x6010601, (unsigned)(g_cci_base + 0x94138));
    writel(0x6010601, (unsigned)(g_cci_base + 0x95138));

    /*4 使能Qos*/
    writel(0x3, (unsigned)(g_cci_base + 0x9410c));
    writel(0x3, (unsigned)(g_cci_base + 0x9510c));

    /*5 不改变传输命令的Domain特性 S3和S4没有，不需要配置*/
#if 0
    writel(0x, (g_cci_base + 0x9));
    writel(0x, (g_cci_base + 0x9));
#endif
    /*coherent_slave_port_ctrl(0,0x3);*/
    /*coherent_slave_port_ctrl(1,0x3);*/

    return;
}
#if 0
static unsigned int get_cci_port_s4(void)
{
    return *(unsigned int*)(g_cci_base + 0x95000);
}
static void hisi_pm_cci_enable(unsigned int id)
{
    volatile unsigned int reg_val = 0;
    if(0 == g_cci_base)
        return;

    if(1 == id)
    {
        writel(0x3, (unsigned)(g_cci_base + 0x95000));/*S4*/

        do{
            reg_val = readl((unsigned)(g_cci_base + 0x9000c));
        }while(0 != reg_val);

        /*reg_val = readl(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));
        reg_val &= ~BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_set_acinactm_high1_START);
        writel(reg_val,SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));*/

        dsb();
    }
    else if(0 == id)
    {
        writel(0x3, (unsigned)(g_cci_base + 0x94000));/*S3*/
        do{
            reg_val = readl((unsigned)(g_cci_base + 0x9000c));
        }
        while(0 != reg_val);

        /*reg_val = readl(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));
        reg_val &= ~BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_set_acinactm_high0_START);
        writel(reg_val,SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));*/

        dsb();
    }
    else
    {
        pr_err("hisi_pm_cluster_up param error!!  \n");
    }

}
#endif
 void hisi_set_cluster_wfi(unsigned int id)
{
    volatile unsigned int reg_val = 0;

    if(0 == id)
    {
        reg_val = readl((unsigned)(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map)));
        /*reg_val |= BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_set_acinactm_high0_START);*/
        reg_val |= BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_pd_detect_start0_START);
        writel(reg_val,(unsigned)SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));
    }
    else if(1 == id)
    {
        reg_val = readl((unsigned)(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map)));
        /*reg_val |= BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_set_acinactm_high1_START);*/
        reg_val |= BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_pd_detect_start1_START);
        writel(reg_val,(unsigned)SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));
    }

}

void hisi_pm_cci_disable(unsigned int id)
{
    volatile unsigned int reg_val = 0;
    if(0 == g_cci_base)
        return;

    if(1 == id)
    {
#if 0
        /*1)	系统配置系统控制器以拉高L2FLUSHREQ */
        reg_val = readl(SOC_ACPU_SCTRL_ACPU_SC_CPU_CTRL_ADDR(g_acpu_sc_base_map));
        reg_val |= BIT(SOC_ACPU_SCTRL_ACPU_SC_CPU_CTRL_l2flushreq1_START);
        writel(reg_val,SOC_ACPU_SCTRL_ACPU_SC_CPU_CTRL_ADDR(g_acpu_sc_base_map));
        /*2)	系统等待L2FLUSHDONE有效之后，拉低L2FLUSHREQ*/
        do{
            reg_val = readl(SOC_ACPU_SCTRL_ACPU_SC_CPU_CTRL_ADDR(g_acpu_sc_base_map));
        }while(0 == reg_val&BIT(SOC_ACPU_SCTRL_ACPU_SC_CPU_STAT_l2flshudone1_START));
        reg_val = readl(SOC_ACPU_SCTRL_ACPU_SC_CPU_CTRL_ADDR(g_acpu_sc_base_map));
        reg_val &= ~ BIT(SOC_ACPU_SCTRL_ACPU_SC_CPU_CTRL_l2flushreq1_START);
        writel(reg_val,SOC_ACPU_SCTRL_ACPU_SC_CPU_CTRL_ADDR(g_acpu_sc_base_map));

        /*系统配置控制状态机关闭Snoop接收打开状态检测*/
        reg_val = readl(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));
        reg_val |= BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_pd_detect_start1_START);
        reg_val |= BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_set_acinactm_high1_START);
        writel(reg_val,SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));
#endif

        /*3)	CPU0配置CCI-400 Snoop Control Register，关闭对应通道的所有发送snoop和接收snoop的使能 */
        writel(0x0, (unsigned)(g_cci_base + 0x95000));/*S4*/

        do{
            reg_val = readl((unsigned)(g_cci_base + 0x9000c));
        }while(0 != reg_val);
#if 0/*for cluster pd*/
        reg_val = readl(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));
        /*reg_val |= BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_set_acinactm_high1_START);*/
        reg_val |= BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_pd_detect_start1_START);
        writel(reg_val,SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));
#endif
        dsb();
#if 0
        /*5)	系统配置控制状态机开始Snoop接收关闭状态检测，Power detect start控制器 bit先写1，然后写0清除。*/
        reg_val = readl(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));
        reg_val |= BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_pd_detect_start1_START);
        writel(reg_val,SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));
        reg_val = readl(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));
        reg_val |= BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_pd_detect_clr1_START);
        writel(reg_val,SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));
#endif

    }
    else if(0 == id)
    {
#if 0
        /*1)	系统配置系统控制器以拉高L2FLUSHREQ */
        reg_val = readl(SOC_ACPU_SCTRL_ACPU_SC_CPU_CTRL_ADDR(g_acpu_sc_base_map));
        reg_val |= BIT(SOC_ACPU_SCTRL_ACPU_SC_CPU_CTRL_l2flushreq0_START);
        writel(reg_val,SOC_ACPU_SCTRL_ACPU_SC_CPU_CTRL_ADDR(g_acpu_sc_base_map));
        /*2)	系统等待L2FLUSHDONE有效之后，拉低L2FLUSHREQ*/
        do{
            reg_val = readl(SOC_ACPU_SCTRL_ACPU_SC_CPU_CTRL_ADDR(g_acpu_sc_base_map));
        }while(0 == reg_val&BIT(SOC_ACPU_SCTRL_ACPU_SC_CPU_STAT_l2flshudone0_START));
        reg_val = readl(SOC_ACPU_SCTRL_ACPU_SC_CPU_CTRL_ADDR(g_acpu_sc_base_map));
        reg_val &= ~ BIT(SOC_ACPU_SCTRL_ACPU_SC_CPU_CTRL_l2flushreq0_START);
        writel(reg_val,SOC_ACPU_SCTRL_ACPU_SC_CPU_CTRL_ADDR(g_acpu_sc_base_map));

        /*系统配置控制状态机关闭Snoop接收打开状态检测*/
        reg_val = readl(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));
        reg_val |= BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_pd_detect_start0_START);
        reg_val |= BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_set_acinactm_high0_START);
        writel(reg_val,SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));
#endif
        /*3)	CPU0配置CCI-400 Snoop Control Register，关闭对应通道的所有发送snoop和接收snoop的使能 */
        writel(0x0, (unsigned)(g_cci_base + 0x94000));/*S3*/
        do{
            reg_val = readl((unsigned)(g_cci_base + 0x9000c));
        }
        while(0 != reg_val);
#if 0/*for cluster pd*/
        reg_val = readl(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));
        /*reg_val |= BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_set_acinactm_high0_START);*/
        reg_val |= BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_pd_detect_start0_START);
        writel(reg_val,SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));
#endif

	dsb();
#if 0
        /*5)	系统配置控制状态机开始Snoop接收关闭状态检测，Power detect start控制器 bit先写1，然后写0清除。*/
        reg_val = readl(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));
        reg_val |= BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_pd_detect_start0_START);
        writel(reg_val,SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));
        reg_val = readl(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));
        reg_val |= BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_pd_detect_clr0_START);
        writel(reg_val,SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(g_acpu_sc_base_map));
#endif
    }
    else
    {
	printk("hisi_pm_cci_disable,param error.\n");
    }
}
#if 0
void hisi_pm_exit_coherence(void)
{
    volatile unsigned int val0 = 0;
    volatile unsigned int val1 = 0;

	if(!get_cpu_type())
	{
        set_cr(get_cr() & ~CR_C);
		flush_cache_all();

	    asm volatile("mcr	p15, 0, %0, c1, c0, 1	@ set CR"
		  : : "r" (val0) : "cc");
	}
	else
	{
        flush_cache_all_pm();
	    asm volatile("mrrc p15, 1, %0, %1, c15"
	                         :"=r"(val0),"=r"(val1));
	    val0 &= ~(1<<6);
	    asm volatile("bic r2,r2,#0x40");

	    asm volatile("mcrr p15, 1, %0, %1, c15"
	                         :"=r"(val0),"=r"(val1));
	}

	asm volatile ("clrex");
	set_auxcr(get_auxcr() & ~(1 << 6));
	cpu_proc_fin();
	isb();
	dsb();

}
#endif

static void hisi_pm_suspend(void)
{
#if defined(CONFIG_SECURE_EXTENSION)
	struct psci_power_state power_state;
#endif
	int cluster = 0;
	int core = 0;
	int mpidr = read_cpuid_mpidr();

	core = mpidr & 0xff;
	cluster = (mpidr >> 8) & 0xff;

#if defined(CONFIG_SECURE_EXTENSION)
	power_state.id = PSCI_POWER_STATE_ID;
	power_state.type = PSCI_POWER_STATE_TYPE_POWER_DOWN;
	power_state.affinity_level = PSCI_POWER_STATE_AFFINITY_LEVEL3;
#endif

	setup_mm_for_reboot();
	gic_cpu_if_down();

#if defined(CONFIG_SECURE_EXTENSION)
	psci_ops.cpu_suspend(power_state, virt_to_phys(mcpm_entry_point));
#else

    if(!get_cpu_type())
	{
		set_cr(get_cr() & ~CR_C);
		flush_cache_all();
		asm volatile ("clrex");
		set_auxcr(get_auxcr() & ~(1 << 6));
	}
	else
	{
		hisi_cluster_exit_coherence(0);
		hisi_set_cluster_wfi(1);
		hisi_set_cluster_wfi(0);
	    /*hisi_pm_exit_coherence();
        hisi_pm_cci_disable(0);
        hisi_pm_cci_disable(1);	*/
	}

	cpu_proc_fin();
    /*send ipc to mcu, need add code here*/
    hisi_ipc_pm_suspend(core, cluster, PSCI_POWER_STATE_TYPE_POWER_DOWN);
	g_adfreq_sr_switch=1;
#endif
	dsb();
	while(1)
		wfi();
}

static int hi6xxx_cpu_suspend(unsigned long arg)
{
	mcpm_set_entry_vector(0, 0, cpu_resume);
	hisi_pm_suspend();
	return 0;
}

#ifdef DEBUG_SLEEP_WAKELOCK
void a_wl_rel(void)
{
    wake_unlock(&g_ulTstwakelock);
}
#endif

#ifdef CONFIG_HISI_SR_DEBUG
extern void debuguart_reinit(void);
#endif
extern int get_cpu_type(void);
extern unsigned long long arch_timer_init_val __read_mostly;

static int hi6xxx_pm_enter(suspend_state_t state)
{
	unsigned long flag = 0;
    int ret = 0;

	printk(KERN_INFO"hi6xxx_pm_enter ++\n");

	local_irq_save(flag);
	ret = cpu_suspend(0, hi6xxx_cpu_suspend);
	arch_timer_init_val = 0; /*clear default arch count for sched_clock() when acpu_subsys resume*/
    if(ret)
    {
        printk(KERN_ERR "[%s]cpu suspend error.\n",__FUNCTION__);
    }

#ifdef CONFIG_HISI_SR_DEBUG
	debuguart_reinit();
#endif
    if (get_cpu_type()){
        /*cci上电配置*/
        coherent_init();
        coherent_slave_port_config();
    }

	pm_gic_dump();

    g_pwcAcpuWakeFlagIcc = 1;
    g_pwcAcpuWakeFlagRfile = 1;

	local_irq_restore(flag);

	printk(KERN_INFO"hi6xxx_pm_enter --\n");

	pwrctrl_mcu_debug_info_show();
	pwrctrl_ccpu_debug_info_show();
#ifdef DEBUG_SLEEP_WAKELOCK
    wake_lock(&g_ulTstwakelock);
#endif
	return 0;
}

static const struct platform_suspend_ops hi6xxx_pm_ops = {
	.enter		= hi6xxx_pm_enter,
	.valid		= suspend_valid_only_mem,
};

static void hisi_get_gic_base(void)
{
	void __iomem *hisi_gic_dist_base;
	struct device_node *node;

	node = of_find_compatible_node(NULL, NULL, "arm,cortex-a9-gic");
	hisi_gic_dist_base = of_iomap(node, 0);
	pr_info("hisi_gic_dist_base = 0x%x\n", (unsigned int)hisi_gic_dist_base);
	printk(KERN_INFO"hisi_gic_dist_base = 0x%x\n", (unsigned int)hisi_gic_dist_base);

	g_enable_base = hisi_gic_dist_base + GIC_ENABLE_OFFSET;
	g_pending_base = hisi_gic_dist_base + GIC_PENDING_OFFSET;
}

extern int  dt_insmod_pm_ap_general(void);
static __init int hi6xxx_pm_drvinit(void)
{
    volatile unsigned int reg_val = 0;

	if(dt_insmod_pm_ap_general())return 0;

#ifdef DEBUG_SLEEP_WAKELOCK
    wake_lock_init(&g_ulTstwakelock,WAKE_LOCK_SUSPEND,"pwrctrl_tst");
    wake_lock(&g_ulTstwakelock);
#endif
    pm_asm_code_copy();
    writel(PWRCTRL_JMP_INSTRUCTION, (unsigned)((void __iomem *)(HISI_VA_ADDRESS(PWRCTRL_ACPU_ASM_SPACE_ADDR))));
    writel(virt_to_phys(mcpm_entry_point), (unsigned)((void __iomem*)(HISI_VA_ADDRESS(MEMORY_AXI_SEC_CORE_BOOT_ADDR))));
    reg_val = readl((unsigned)((void __iomem *)(HISI_VA_ADDRESS(SOC_AO_SCTRL_SC_SYS_CTRL1_ADDR(SOC_AO_SCTRL_BASE_ADDR)))));
    reg_val |= BIT(SOC_AO_SCTRL_SC_SYS_CTRL1_remap_sram_aarm_START) | \
                     BIT(SOC_AO_SCTRL_SC_SYS_CTRL1_remap_sram_aarm_msk_START);
    writel(reg_val,(unsigned)( (void __iomem *)(HISI_VA_ADDRESS(SOC_AO_SCTRL_SC_SYS_CTRL1_ADDR(SOC_AO_SCTRL_BASE_ADDR)))));
    //memset((void *)HISI_VA_ADDRESS(MEMORY_AXI_DDR_DDL_ADDR), 0x0, MEMORY_AXI_DDR_DDL_SIZE);

	hisi_get_gic_base();

	suspend_set_ops(&hi6xxx_pm_ops);
    printk(KERN_INFO"MEMORY_AXI_SEC_CORE_BOOT_ADDR:0x%x.\n",MEMORY_AXI_SEC_CORE_BOOT_ADDR);
	printk(KERN_INFO"%s exit\n",__FUNCTION__);
	return 0;
}
static __init int addr_init(void)
{
	g_cci_base = (void __iomem *)ioremap(SOC_ACPU_CCI_BASE_ADDR+0x90000,SZ_128K) - 0x90000;
	printk(KERN_INFO"%s g_cci_base:0x%p.\n",__FUNCTION__,g_cci_base);
	g_acpu_sc_base_map = (void __iomem *)HISI_VA_ADDRESS(SOC_ACPU_SCTRL_BASE_ADDR);
	printk(KERN_INFO"%s g_acpu_sc_base_map:0x%p.\n",__FUNCTION__,g_acpu_sc_base_map);

    return 0;
}


arch_initcall(hi6xxx_pm_drvinit);
early_initcall(addr_init);
