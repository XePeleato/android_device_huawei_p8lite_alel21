
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/smp.h>
#include <linux/notifier.h>
#include <linux/cpu.h>
#include <linux/completion.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <asm/cacheflush.h>
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */
#include <linux/hisi/hi6xxx-platform.h>
#include <soc_baseaddr_interface.h>
#include "soc_acpu_sctrl_interface.h"
#include <soc_ao_sctrl_interface.h>
/*#include "drv_ipcm.h"*/
#include "soc_pmctrl_interface.h"
#include <asm/cacheflush.h>
#include <linux/cpuidle.h>

#define ARM_ACPU_CORE_NUM 8
#define ACPU_PD_ID_CLUSTER0 0
#define ACPU_PD_ID_CLUSTER1 1
unsigned long acpu_sctrl_base_addr = 0;

unsigned int g_acpu_core_hotplug_stat[ARM_ACPU_CORE_NUM]=
{
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
};

unsigned long g_acpu_core_sc_baseaddr[ARM_ACPU_CORE_NUM] =
{
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
};
unsigned int g_cluster_pd_stat[2]=
{
	0,
	0,
};
typedef struct acpu_core_sc_stru_s
{
    unsigned int acpu_sc_ctrl;
    unsigned int acpu_sc_stat;
    unsigned int acpu_sc_clken;
    unsigned int acpu_sc_clkdis;
    unsigned int acpu_sc_clkstat;
    unsigned int acpu_sc_rsten;
    unsigned int acpu_sc_rstdis;
    unsigned int acpu_sc_rststat;
    unsigned int acpu_sc_mtcmos_en;
    unsigned int acpu_sc_mtcmos_dis;
    unsigned int acpu_sc_mtcmos_stat;
    unsigned int acpu_sc_mtcmos_ack_stat;
    unsigned int acpu_sc_isoen;
    unsigned int acpu_sc_isodis;
    unsigned int acpu_sc_isostat;
    unsigned int reserved[5];
    unsigned int acpu_sc_mtcmos_timer;
    unsigned int acpu_sc_mtcmos_timer_state;
}acpu_core_sc_stru;

/*balong v8r1 soc related function, to enable slave power down*/
void platform_cpu_power_down(int cpu)
{
    unsigned int reg, cnt = 0;
    unsigned int tmp = 0;
    unsigned long flag = 0;
    acpu_core_sc_stru *acpu_core_sc;

    if(cpu >= (sizeof(g_acpu_core_sc_baseaddr) / sizeof(g_acpu_core_sc_baseaddr[0])))
    {
    	printk(KERN_ERR"%s : cpu id:%d support max:%lu.\n", __FUNCTION__, cpu, (sizeof(g_acpu_core_sc_baseaddr) / sizeof(g_acpu_core_sc_baseaddr[0])));
		return;
    }

    acpu_core_sc = (acpu_core_sc_stru *)g_acpu_core_sc_baseaddr[cpu];

    if(NULL == acpu_core_sc)
    {
        printk(KERN_ERR"%s : error acore(%d) virtual is NULL!!!.\n", __FUNCTION__, cpu);
        return;
    }

    tmp = readl((unsigned long)&(acpu_core_sc->acpu_sc_isostat)) & 0x1;
    if(0 != tmp)
    {
        printk(KERN_ERR"%s: cpu:%d already power down.\n",__FUNCTION__,cpu);
        return;
    }

    /*make sure cpu in wfi status*/
    do {
        reg = readl((unsigned long)&(acpu_core_sc->acpu_sc_stat));
        //printk(KERN_ERR"%s : cpu %d stat %d addr:0x%x.\n", __FUNCTION__, cpu, reg, (unsigned int)&(acpu_core_sc->acpu_sc_stat));
        if (reg & BIT(0x1)) {
            break;
        }

        //msleep(1);
        if (cnt++ > 5000) {
            printk(KERN_ERR"%s : cpu %d not in wfi state.\n", __FUNCTION__, cpu);
            return;
        }
    }while(1);
    local_irq_save(flag);

	/*增加关闭HPM的操作*/
    writel(0x1, (unsigned long)&(acpu_core_sc->acpu_sc_isoen));
    do{
        tmp = readl((unsigned long)&(acpu_core_sc->acpu_sc_isostat)) & 0x1;
     } while (0x1 != tmp);

	/*可以一起复位*/
    writel(0x1F, (unsigned long)&(acpu_core_sc->acpu_sc_rsten));
    do{
        tmp = readl((unsigned long)&(acpu_core_sc->acpu_sc_rststat)) & 0x1F;
     } while (0x1F != tmp);

    writel(0x7, (unsigned long)&(acpu_core_sc->acpu_sc_clkdis));
    do{
        tmp = readl((unsigned long)&(acpu_core_sc->acpu_sc_clkstat)) & 0x7;
     } while (0x0 != tmp);

    writel(0x1, (unsigned long)&(acpu_core_sc->acpu_sc_mtcmos_dis));
    do{
        tmp = readl((unsigned long)&(acpu_core_sc->acpu_sc_mtcmos_timer_state)) & 0x1;
     } while (0x0 != tmp);
    g_acpu_core_hotplug_stat[cpu]=1;
    local_irq_restore(flag);

    return;
}
#if 0
void platform_cluster1_power_on_detect(void)
{
    unsigned int tmp = 0;

    if(0 == (readl(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_STA_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_STA_pw_iso_a53_1_sta_START)))
    {
        printk(KERN_ERR"cluster1 already power on.\n");
        return;
    }
    printk(KERN_ERR"cluster1_power_detect,start.\n");

    if(readl(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(acpu_sctrl_base_addr))&BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_pd_detect_start1_START))
    {
        printk(KERN_ERR"cluster1_power_detect,1.\n");
        /*pwc_clr_bits(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(SOC_ACPU_SCTRL_BASE_ADDR),BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_set_acinactm_high1_START));  */
        /*系统配置控制状态机开始Snoop接收打开状态检测*/
        //pwc_clr_bits(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(SOC_ACPU_SCTRL_BASE_ADDR),BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_pd_detect_start1_START));
        tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(acpu_sctrl_base_addr));
        tmp &= ~ BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_pd_detect_start1_START);
        writel(tmp,SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(acpu_sctrl_base_addr));
    }
    printk(KERN_ERR"cluster1_power_detect,2.\n");

    /*CPU0配置电源稳定时间数值ACPU_SC_A53_1_MTCMOS_TIMER[15:0]为16'hff，表示MTCMOS开启的稳定时间约为13.3us，参考时钟为19.2MHz；*/
    tmp = 0xff;
    writel(tmp,SOC_ACPU_SCTRL_ACPU_SC_A53_1_MTCMOS_TIMER_ADDR(acpu_sctrl_base_addr));
    printk(KERN_ERR"cluster1_power_detect,3.\n");

    /*复位*/
    /*tmp = pwc_read_reg32(SOC_ACPU_SCTRL_ACPU_SC_RSTEN_ADDR(SOC_ACPU_SCTRL_BASE_ADDR));*/
	tmp = 0;
    tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTEN_srst_aarm_l2_1_rsten_START);
    tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTEN_srst_l2_hpm_1_rsten_START);
    tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTEN_srst_cluster1_rsten_START);
    tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTEN_srst_preset1_rsten_START);
    writel(tmp,SOC_ACPU_SCTRL_ACPU_SC_RSTEN_ADDR(acpu_sctrl_base_addr));
    do{
        tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_ADDR(acpu_sctrl_base_addr));
    }while((0 == (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_aarm_l2_1_rststa_START)))||(0 == (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_l2_hpm_1_rststa_START))) || \
    (0 == (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_cluster1_rststa_START)))||(0 == (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_preset1_rststa_START))));

    printk(KERN_ERR"cluster1_power_detect,4.\n");

    /*CPU0配置ACPU_SC_A53_CLUSTER_MTCMOS_EN[pw_mtcmos_en_a53_1_en]为1'b1，等待ACPU_SC_A53_MTCMOS_TIMER_STAT[pw_mtcmos_en_a53_1_sta]=1,表示电源可靠稳定；*/
    tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_MTCMOS_EN_ADDR(acpu_sctrl_base_addr));
    tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_MTCMOS_EN_pw_mtcmos_en_a53_1_en_START);
    writel(tmp,SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_MTCMOS_EN_ADDR(acpu_sctrl_base_addr));
    do{
        tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_A53_1_MTCMOS_TIMER_STAT_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_1_MTCMOS_TIMER_STAT_a53_1_mtcmos_timer_sta_START);
    } while (0x0 == tmp);

    printk(KERN_ERR"cluster1_power_detect,5.\n");

    /*CPU0根据需要配置ACPU_SC_CLKEN [hpm_l2_1_clken ]=1，判断ACPU_SC_CLK_STAT [hpm_l2_1_clksta]=1标示hpm l2cache 1时钟打开；*/
    tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_CLKEN_ADDR(acpu_sctrl_base_addr));
    tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_CLKEN_hpm_l2_1_clken_START);
    writel(tmp,SOC_ACPU_SCTRL_ACPU_SC_CLKEN_ADDR(acpu_sctrl_base_addr));
    do{
        tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_CLK_STAT_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_CLK_STAT_hpm_l2_1_clksta_START);
    } while (0x0 == tmp);

    printk(KERN_ERR"cluster1_power_detect,6.\n");

    tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_CLKEN_ADDR(acpu_sctrl_base_addr));
    tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_CLKEN_g_cpu_1_clken_START);
    writel(tmp,SOC_ACPU_SCTRL_ACPU_SC_CLKEN_ADDR(acpu_sctrl_base_addr));
    do{
        tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_CLK_STAT_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_CLK_STAT_g_cpu_1_clksta_START);
    } while (0x0 == tmp);

    printk(KERN_ERR"cluster1_power_detect,7.\n");

    /*解ISO*/
    writel(BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_DIS_pw_iso_a53_1_dis_START),SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_DIS_ADDR(acpu_sctrl_base_addr));
    do{
        tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_STA_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_STA_pw_iso_a53_1_sta_START);
    } while (0x0 != tmp);

    printk(KERN_ERR"cluster1_power_detect,8.\n");

    /*解复位*/
    /*tmp = pwc_read_reg32(SOC_ACPU_SCTRL_ACPU_SC_RSTDIS_ADDR(SOC_ACPU_SCTRL_BASE_ADDR));*/
	tmp = 0;
    tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTDIS_srst_aarm_l2_1_rstdis_START);
    tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTDIS_srst_l2_hpm_1_rstdis_START);
    tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTDIS_srst_cluster1_rstdis_START);
    tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTDIS_srst_preset1_rstdis_START);
    writel(tmp,SOC_ACPU_SCTRL_ACPU_SC_RSTDIS_ADDR(acpu_sctrl_base_addr));
    do{
        tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_ADDR(acpu_sctrl_base_addr));
    }while((0 != (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_aarm_l2_1_rststa_START)))||(0 != (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_l2_hpm_1_rststa_START))) || \
    (0 != (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_cluster1_rststa_START)))||(0 != (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_preset1_rststa_START))));
    printk(KERN_ERR"cluster1_power_detect,end.\n");

}
#endif
void platform_cluster_power_down(int clusterId)
{

	volatile unsigned int tmp=0;
	unsigned long flag = 0;
	unsigned int cnt=0;
	switch(clusterId)
		{
		case ACPU_PD_ID_CLUSTER1:
			if(2 == (readl(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_STA_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_STA_pw_iso_a53_1_sta_START)))
			{
				printk(KERN_ERR"cluster1 already power down.\n");
				return;
			}
			/*-------------cluster1--------------*/

			/*系统配置控制状态机关闭Snoop接收打开状态检测*/

	             tmp=readl(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(acpu_sctrl_base_addr))|(BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_pd_detect_start1_START));
	             writel(tmp,SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(acpu_sctrl_base_addr));
			/*pwc_set_bits(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(SOC_ACPU_SCTRL_BASE_ADDR),BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_set_acinactm_high1_START));  */


			/*make sure cluster in standby status*/
			do {
				tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_CPU_STAT_ADDR(acpu_sctrl_base_addr));
				if ((0x1f << SOC_ACPU_SCTRL_ACPU_SC_CPU_STAT_a53_1_standbywfil2_START) == (tmp & (0x1f << SOC_ACPU_SCTRL_ACPU_SC_CPU_STAT_a53_1_standbywfil2_START))) {
					break;
			}

			if (cnt++ > 0x100000) {

				printk(KERN_ERR"cluster1 not in standby state tmp=0x%x.\n", tmp);

				return ;
			}
			}while(1);

			/*iso*/
			local_irq_save(flag);
			writel(BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_EN_pw_iso_a53_1_en_START),SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_EN_ADDR(acpu_sctrl_base_addr));
			do{
				tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_STA_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_STA_pw_iso_a53_1_sta_START);
			} while (0x0 == tmp);


			/*hpm clk*/
			writel(BIT(SOC_ACPU_SCTRL_ACPU_SC_CLKDIS_hpm_l2_1_clkdis_START),SOC_ACPU_SCTRL_ACPU_SC_CLKDIS_ADDR(acpu_sctrl_base_addr));
			do{
				tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_CLK_STAT_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_CLK_STAT_hpm_l2_1_clksta_START);
			} while (0x0 != tmp);
			writel(BIT(SOC_ACPU_SCTRL_ACPU_SC_CLKDIS_g_cpu_1_clkdis_START),SOC_ACPU_SCTRL_ACPU_SC_CLKDIS_ADDR(acpu_sctrl_base_addr));
			do{
				tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_CLK_STAT_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_CLK_STAT_g_cpu_1_clksta_START);
			}while(0x0 != tmp);

			/*mtcmos*/
			writel(BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_MTCMOS_DIS_pw_mtcmos_en_a53_1_dis_START),SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_MTCMOS_DIS_ADDR(acpu_sctrl_base_addr));
			do{
				tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_MTCMOS_STA_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_MTCMOS_STA_pw_mtcmos_en_a53_1_sta_START);
			} while (0x0 != tmp);
			g_cluster_pd_stat[1]=1;
			local_irq_restore(flag);

			break;
		case ACPU_PD_ID_CLUSTER0:
			/*---------------cluster0-----------------*/
			if(1 == (readl(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_STA_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_STA_pw_iso_a53_0_sta_START)))
			{
				printk(KERN_ERR"cluster0 already power down.\n");
				return;
			}

			/*系统配置控制状态机关闭Snoop接收打开状态检测*/
                     tmp=readl(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(acpu_sctrl_base_addr))|(BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_pd_detect_start0_START));
	              writel(tmp,SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(acpu_sctrl_base_addr));
			/*pwc_set_bits(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(SOC_ACPU_SCTRL_BASE_ADDR),BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_set_acinactm_high0_START)); */


			/*make sure cluster in standby status*/
			do {
				tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_CPU_STAT_ADDR(acpu_sctrl_base_addr));
				if (0x1f == (tmp & 0x1f)) {
					break;
				}

				if (cnt++ > 5000) {
					printk(KERN_ERR"cluster0 not in standby state.\n");
					return ;
				}
			}while(1);

			/*iso*/
			local_irq_save(flag);
			writel(BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_EN_pw_iso_a53_0_en_START),SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_EN_ADDR(acpu_sctrl_base_addr));
			do{
				tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_STA_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_STA_pw_iso_a53_0_sta_START);
			} while (0x0 == tmp);



			/*hpm clk*/
			writel(BIT(SOC_ACPU_SCTRL_ACPU_SC_CLKDIS_hpm_l2_clkdis_START),SOC_ACPU_SCTRL_ACPU_SC_CLKDIS_ADDR(acpu_sctrl_base_addr));
			do{
				tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_CLK_STAT_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_CLK_STAT_hpm_l2_clksta_START);
			} while (0x0 != tmp);
			writel(BIT(SOC_ACPU_SCTRL_ACPU_SC_CLKDIS_g_cpu_clkdis_START),SOC_ACPU_SCTRL_ACPU_SC_CLKDIS_ADDR(acpu_sctrl_base_addr));
			do{
				tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_CLK_STAT_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_CLK_STAT_g_cpu_clksta_START);
			}while(0x0 != tmp);

			/*mtcmos*/
			writel(BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_MTCMOS_DIS_pw_mtcmos_en_a53_0_dis_START),SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_MTCMOS_DIS_ADDR(acpu_sctrl_base_addr));
			do{
				tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_MTCMOS_STA_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_MTCMOS_STA_pw_mtcmos_en_a53_0_sta_START);
			} while (0x0 != tmp);
			g_cluster_pd_stat[0]=1;
			local_irq_restore(flag);
			break;
		default:
			break;
		return;
		}
}


void platform_cluster_power_up(int clusterId)
{
	volatile unsigned int  tmp=0;
	unsigned long flag = 0;
	switch(clusterId)
	{
		case ACPU_PD_ID_CLUSTER0:

		        /*-----------------cluster 0----------------------*/
			if(0 == (readl(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_STA_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_STA_pw_iso_a53_0_sta_START)))
			{
				printk(KERN_ERR"cluster0 already power on.\n");
				return;
			}
		        /*pwc_clr_bits(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(SOC_ACPU_SCTRL_BASE_ADDR),BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_set_acinactm_high0_START));*/
		        /*系统配置控制状态机开始Snoop接收打开状态检测*/
			 local_irq_save(flag);
			 tmp=readl(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(acpu_sctrl_base_addr))&(~BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_pd_detect_start0_START));
		        writel(tmp,SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(acpu_sctrl_base_addr));
		        /*时钟配置*/
		        tmp = 0xff;
		        writel(tmp,SOC_ACPU_SCTRL_ACPU_SC_A53_0_MTCMOS_TIMER_ADDR(acpu_sctrl_base_addr));

		        /*复位*/
		        /*tmp = pwc_read_reg32(SOC_ACPU_SCTRL_ACPU_SC_RSTEN_ADDR(SOC_ACPU_SCTRL_BASE_ADDR));*/
			 tmp = 0;
		        tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTEN_srst_aarm_l2_rsten_START);
		        tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTEN_srst_l2_hpm_rsten_START);
		        tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTEN_srst_cluster0_rsten_START);
		        tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTEN_srst_preset0_rsten_START);
		        writel(tmp,SOC_ACPU_SCTRL_ACPU_SC_RSTEN_ADDR(acpu_sctrl_base_addr));
		        do{
		            tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_ADDR(acpu_sctrl_base_addr));
		        }while((0 == (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_aarm_l2_rststa_START)))||(0 == (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_l2_hpm_rststa_START))) || \
		        (0 == (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_cluster0_rststa_START)))||(0 ==( tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_preset0_rststa_START))));

		        /*MTCMOS使能*/
		        writel(BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_MTCMOS_EN_pw_mtcmos_en_a53_0_en_START),SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_MTCMOS_EN_ADDR(acpu_sctrl_base_addr));
		        do{
		            tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_A53_0_MTCMOS_TIMER_STAT_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_0_MTCMOS_TIMER_STAT_a53_0_mtcmos_timer_sta_START);
		        } while (0x0 == tmp);
		        /*CPU0根据需要配置ACPU_SC_CLKEN [hpm_l2_1_clken ]=1，判断ACPU_SC_CLK_STAT [hpm_l2_1_clksta]=1标示hpm l2cache 1时钟打开；*/
		        writel(BIT(SOC_ACPU_SCTRL_ACPU_SC_CLKEN_hpm_l2_clken_START),SOC_ACPU_SCTRL_ACPU_SC_CLKEN_ADDR(acpu_sctrl_base_addr));
		        do{
		            tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_CLK_STAT_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_CLK_STAT_hpm_l2_clksta_START);
		        } while (0x0 == tmp);
		        writel(BIT(SOC_ACPU_SCTRL_ACPU_SC_CLKEN_g_cpu_clken_START),SOC_ACPU_SCTRL_ACPU_SC_CLKEN_ADDR(acpu_sctrl_base_addr));
		        do{
		            tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_CLK_STAT_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_CLK_STAT_g_cpu_clksta_START);
		        } while (0x0 == tmp);


		        /*解ISO*/
		        writel(BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_DIS_pw_iso_a53_0_dis_START),SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_DIS_ADDR(acpu_sctrl_base_addr));
		        do{
		            tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_STA_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_STA_pw_iso_a53_0_sta_START);
		        } while (0x0 != tmp);
		        /*解复位*/
		        /*tmp = pwc_read_reg32(SOC_ACPU_SCTRL_ACPU_SC_RSTDIS_ADDR(SOC_ACPU_SCTRL_BASE_ADDR));*/
				tmp = 0;
		        tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTDIS_srst_aarm_l2_rstdis_START);
		        tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTDIS_srst_l2_hpm_rstdis_START);
		        tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTDIS_srst_cluster0_rstdis_START);
		        tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTDIS_srst_preset0_rstdis_START);
		        writel(tmp,SOC_ACPU_SCTRL_ACPU_SC_RSTDIS_ADDR(acpu_sctrl_base_addr));
		        do{
		            tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_ADDR(acpu_sctrl_base_addr));
		        }while((0 != (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_aarm_l2_rststa_START)))||(0 != (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_l2_hpm_rststa_START))) || \
		        (0 != (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_cluster0_rststa_START)))||(0 != (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_preset0_rststa_START))));
			 g_cluster_pd_stat[0]=0;
			 local_irq_restore(flag);
		        break;
		case ACPU_PD_ID_CLUSTER1:
		            /*----------------cluster 1------------------*/
				if(0 == (readl(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_STA_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_STA_pw_iso_a53_1_sta_START)))
				{
					printk(KERN_ERR"cluster1 already power on.\n");
					return;
				}
		            /*pwc_clr_bits(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(SOC_ACPU_SCTRL_BASE_ADDR),BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_set_acinactm_high1_START));  */
		            /*系统配置控制状态机开始Snoop接收打开状态检测*/
				local_irq_save(flag);
		             tmp=readl(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(acpu_sctrl_base_addr))&(~BIT(SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_pd_detect_start1_START));
		             writel(tmp,SOC_ACPU_SCTRL_ACPU_SC_SNOOP_PWD_ADDR(acpu_sctrl_base_addr));


		            /*CPU0配置电源稳定时间数值ACPU_SC_A53_1_MTCMOS_TIMER[15:0]为16'hff，表示MTCMOS开启的稳定时间约为13.3us，参考时钟为19.2MHz；*/
		            tmp = 0xff;
		            writel(tmp,SOC_ACPU_SCTRL_ACPU_SC_A53_1_MTCMOS_TIMER_ADDR(acpu_sctrl_base_addr));

		            /*复位*/
		            /*tmp = pwc_read_reg32(SOC_ACPU_SCTRL_ACPU_SC_RSTEN_ADDR(SOC_ACPU_SCTRL_BASE_ADDR));*/
					tmp = 0;
		            tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTEN_srst_aarm_l2_1_rsten_START);
		            tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTEN_srst_l2_hpm_1_rsten_START);
		            tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTEN_srst_cluster1_rsten_START);
		            tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTEN_srst_preset1_rsten_START);
		            writel(tmp,SOC_ACPU_SCTRL_ACPU_SC_RSTEN_ADDR(acpu_sctrl_base_addr));
		            do{
		                tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_ADDR(acpu_sctrl_base_addr));
		            }while((0 == (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_aarm_l2_1_rststa_START)))||(0 == (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_l2_hpm_1_rststa_START))) || \
		            (0 == (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_cluster1_rststa_START)))||(0 == (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_preset1_rststa_START))));


		            /*CPU0配置ACPU_SC_A53_CLUSTER_MTCMOS_EN[pw_mtcmos_en_a53_1_en]为1'b1，等待ACPU_SC_A53_MTCMOS_TIMER_STAT[pw_mtcmos_en_a53_1_sta]=1,表示电源可靠稳定；*/
		            writel(BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_MTCMOS_EN_pw_mtcmos_en_a53_1_en_START),SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_MTCMOS_EN_ADDR(acpu_sctrl_base_addr));
		            do{
		                tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_A53_1_MTCMOS_TIMER_STAT_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_1_MTCMOS_TIMER_STAT_a53_1_mtcmos_timer_sta_START);
		            } while (0x0 == tmp);
		            /*CPU0根据需要配置ACPU_SC_CLKEN [hpm_l2_1_clken ]=1，判断ACPU_SC_CLK_STAT [hpm_l2_1_clksta]=1标示hpm l2cache 1时钟打开；*/
		            writel(BIT(SOC_ACPU_SCTRL_ACPU_SC_CLKEN_hpm_l2_1_clken_START),SOC_ACPU_SCTRL_ACPU_SC_CLKEN_ADDR(acpu_sctrl_base_addr));
		            do{
		                tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_CLK_STAT_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_CLK_STAT_hpm_l2_1_clksta_START);
		            } while (0x0 == tmp);
		            writel(BIT(SOC_ACPU_SCTRL_ACPU_SC_CLKEN_g_cpu_1_clken_START),SOC_ACPU_SCTRL_ACPU_SC_CLKEN_ADDR(acpu_sctrl_base_addr));
		            do{
		                tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_CLK_STAT_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_CLK_STAT_g_cpu_1_clksta_START);
		            } while (0x0 == tmp);
		            /*解ISO*/
		            writel(BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_DIS_pw_iso_a53_1_dis_START),SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_DIS_ADDR(acpu_sctrl_base_addr));
		            do{
		                tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_STA_ADDR(acpu_sctrl_base_addr)) & BIT(SOC_ACPU_SCTRL_ACPU_SC_A53_CLUSTER_ISO_STA_pw_iso_a53_1_sta_START);
		            } while (0x0 != tmp);
		            /*解复位*/
		            /*tmp = pwc_read_reg32(SOC_ACPU_SCTRL_ACPU_SC_RSTDIS_ADDR(SOC_ACPU_SCTRL_BASE_ADDR));*/
					tmp = 0;
		            tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTDIS_srst_aarm_l2_1_rstdis_START);
		            tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTDIS_srst_l2_hpm_1_rstdis_START);
		            tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTDIS_srst_cluster1_rstdis_START);
		            tmp |= BIT(SOC_ACPU_SCTRL_ACPU_SC_RSTDIS_srst_preset1_rstdis_START);
		            writel(tmp,SOC_ACPU_SCTRL_ACPU_SC_RSTDIS_ADDR(acpu_sctrl_base_addr));
		            do{
		                tmp = readl(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_ADDR(acpu_sctrl_base_addr));
		            }while((0 != (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_aarm_l2_1_rststa_START)))||(0 != (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_l2_hpm_1_rststa_START))) || \
		            (0 != (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_cluster1_rststa_START)))||(0 != (tmp&BIT(SOC_ACPU_SCTRL_ACPU_SC_RST_STAT_srst_preset1_rststa_START))));
				 g_cluster_pd_stat[1]=0;
				 local_irq_restore(flag);
		            break;
			default:
				break;
			return;
		}

}


/*balong v9r1 soc related function, to enable slave power on*/
void platform_cpu_power_on(int cpu)
{
    unsigned int tmp;
    unsigned long flag = 0;

    acpu_core_sc_stru *acpu_core_sc;

    if(cpu >= (sizeof(g_acpu_core_sc_baseaddr) / sizeof(g_acpu_core_sc_baseaddr[0])))
    {
    	printk(KERN_ERR"%s : cpu id:%d support max:%lu.\n", __FUNCTION__, cpu, (sizeof(g_acpu_core_sc_baseaddr) / sizeof(g_acpu_core_sc_baseaddr[0])));
		return;
    }

    acpu_core_sc = (acpu_core_sc_stru *)g_acpu_core_sc_baseaddr[cpu];

    if(NULL == acpu_core_sc)
    {
        printk(KERN_ERR"%s : error acore(%d) virtual is NULL!!!.\n", __FUNCTION__, cpu);
        return;
    }
    tmp = readl((unsigned long)&(acpu_core_sc->acpu_sc_isostat)) & 0x1;
    if(0 == tmp)
    {
        printk(KERN_ERR"%s: cpu:%d already power on.\n",__FUNCTION__,cpu);
        return;
    }

    local_irq_save(flag);
	/*增加打开HPM操作*/
    /*power on cpu*/
    writel(0x1, (unsigned long)&(acpu_core_sc->acpu_sc_mtcmos_en));
    do{
        tmp = readl((unsigned long)&(acpu_core_sc->acpu_sc_mtcmos_timer_state)) & 0x1;
     } while (0x1 != tmp);

    writel(0x7, (unsigned long)&(acpu_core_sc->acpu_sc_clken));
    do{
        tmp = readl((unsigned long)&(acpu_core_sc->acpu_sc_clkstat)) & 0x7;
     } while (0x7 != tmp);

    writel(0x1, (unsigned long)&(acpu_core_sc->acpu_sc_isodis));
    do{
        tmp = readl((unsigned long)&(acpu_core_sc->acpu_sc_isostat)) & 0x1;
     } while (0x0 != tmp);

	/*可以合并*/
    writel(0x1F, (unsigned long)&(acpu_core_sc->acpu_sc_rstdis));
    do{
        tmp = readl((unsigned long)&(acpu_core_sc->acpu_sc_rststat)) & 0x1F;
     } while (0x0 != tmp);
    g_acpu_core_hotplug_stat[cpu]=0;
    local_irq_restore(flag);

    return;
}

extern unsigned int is_cpu_power_on(int cpu);
unsigned int is_cpu_power_on(int cpu)
{
    unsigned int tmp;
    acpu_core_sc_stru *acpu_core_sc;
    if(cpu >= (sizeof(g_acpu_core_sc_baseaddr) / sizeof(g_acpu_core_sc_baseaddr[0])))
    {
    	printk(KERN_ERR"%s : cpu id:%d support max:%lu.\n", __FUNCTION__, cpu, (sizeof(g_acpu_core_sc_baseaddr) / sizeof(g_acpu_core_sc_baseaddr[0])));
		return 0;
    }
    acpu_core_sc = (acpu_core_sc_stru *)g_acpu_core_sc_baseaddr[cpu];
    if(NULL == acpu_core_sc)
    {
        printk(KERN_ERR"%s : error acore(%d) virtual is NULL!!!.\n", __FUNCTION__, cpu);
        return 0;
	}
	tmp = readl((unsigned long)&(acpu_core_sc->acpu_sc_mtcmos_timer_state)) & 0x1;
    return tmp;
}
#ifndef CONFIG_ARM64
static int platform_cpu_up_notify (struct notifier_block *nfb,
                    unsigned long status, void *cpuid)
{
    unsigned int cpu = (unsigned int)cpuid;

    /*printk(KERN_INFO"[CPU %d] %s : cpu %d change to status %ld.\n", this_cpu, __FUNCTION__, cpu, status);*/

    switch(status){
        case CPU_UP_PREPARE:
        case CPU_UP_PREPARE_FROZEN:
            cpuidle_pause();
#ifdef ACPU_32_BIT_CPUILDE
            clr_cluster_idle_bit(cpuid>=4?1:0);
#endif
		if((cpu>3)&&(g_cluster_pd_stat[1] == 1))
		{
			platform_cluster_power_up(1);
		}
		if((cpu<=3)&&(g_cluster_pd_stat[0] == 1))
		{
			platform_cluster_power_up(0);
		}
            printk(KERN_INFO"platform_cpu_power_on begin...\n");
            platform_cpu_power_on(cpu);
            printk(KERN_INFO"platform_cpu_power_on end...\n");
            break;
        case CPU_ONLINE:
        case CPU_ONLINE_FROZEN:
        case CPU_UP_CANCELED:
        case CPU_UP_CANCELED_FROZEN:
            cpuidle_resume();
            break;
        default:
            /*just pass other event*/
            break;

    }
    return NOTIFY_DONE;
}

static int platform_cpu_down_notify (struct notifier_block *nfb,
                    unsigned long status, void *cpuid)
{
    unsigned int cpu = (unsigned int)cpuid;

    /*printk(KERN_INFO"[CPU %d] %s : cpu %d change to status %ld.\n", this_cpu, __FUNCTION__, cpu, status);*/

    switch(status){
        case CPU_DOWN_PREPARE:
        case CPU_DOWN_PREPARE_FROZEN:
#ifdef ACPU_32_BIT_CPUILDE
            clr_cluster_idle_bit(cpuid>=4?1:0);
#endif
        break;
        case CPU_DEAD:
        case CPU_DEAD_FROZEN:
            printk(KERN_INFO"platform_cpu_power_down begin...\n");
            platform_cpu_power_down(cpu);
	     if(g_acpu_core_hotplug_stat[4]&&
		 g_acpu_core_hotplug_stat[5]&&
		 g_acpu_core_hotplug_stat[6]&&
		 g_acpu_core_hotplug_stat[7])
	     	{

			platform_cluster_power_down(1);
		 }
	     if(g_acpu_core_hotplug_stat[0]&&
		 g_acpu_core_hotplug_stat[1]&&
		 g_acpu_core_hotplug_stat[2]&&
		 g_acpu_core_hotplug_stat[3])
	     	{

			platform_cluster_power_down(0);
		 }
            printk(KERN_INFO"platform_cpu_power_down end.\n");
            break;
        default:
            /*just pass other event*/
            break;

    }
    return NOTIFY_DONE;
}

static struct notifier_block __refdata platform_cpu_up_notifier = {
    .notifier_call = platform_cpu_up_notify,
    .priority = CPU_PRI_SCHED_ACTIVE,
};

static struct notifier_block __refdata platform_cpu_down_notifier = {
    .notifier_call = platform_cpu_down_notify,
    .priority = CPU_PRI_CPUSET_INACTIVE,
};
#endif

static int __init platform_hotplug_init(void)
{
    acpu_sctrl_base_addr = (unsigned long)HISI_VA_ADDRESS(SOC_ACPU_SCTRL_BASE_ADDR);

    g_acpu_core_sc_baseaddr[0] = SOC_ACPU_SCTRL_ACPU_SC_CPU0_CTRL_ADDR(acpu_sctrl_base_addr);
    g_acpu_core_sc_baseaddr[1] = SOC_ACPU_SCTRL_ACPU_SC_CPU1_CTRL_ADDR(acpu_sctrl_base_addr);
    g_acpu_core_sc_baseaddr[2] = SOC_ACPU_SCTRL_ACPU_SC_CPU2_CTRL_ADDR(acpu_sctrl_base_addr);
    g_acpu_core_sc_baseaddr[3] = SOC_ACPU_SCTRL_ACPU_SC_CPU3_CTRL_ADDR(acpu_sctrl_base_addr);
    g_acpu_core_sc_baseaddr[4] = SOC_ACPU_SCTRL_ACPU_SC_CPU4_CTRL_ADDR(acpu_sctrl_base_addr);
    g_acpu_core_sc_baseaddr[5] = SOC_ACPU_SCTRL_ACPU_SC_CPU5_CTRL_ADDR(acpu_sctrl_base_addr);
    g_acpu_core_sc_baseaddr[6] = SOC_ACPU_SCTRL_ACPU_SC_CPU6_CTRL_ADDR(acpu_sctrl_base_addr);
    g_acpu_core_sc_baseaddr[7] = SOC_ACPU_SCTRL_ACPU_SC_CPU7_CTRL_ADDR(acpu_sctrl_base_addr);
    printk("%s: %lu %lu %lu %lu\n", __FUNCTION__, g_acpu_core_sc_baseaddr[0],
        g_acpu_core_sc_baseaddr[1], g_acpu_core_sc_baseaddr[2],
        g_acpu_core_sc_baseaddr[3]);
    printk("%s: %lu %lu %lu %lu\n", __FUNCTION__, g_acpu_core_sc_baseaddr[4],
        g_acpu_core_sc_baseaddr[5], g_acpu_core_sc_baseaddr[6],
        g_acpu_core_sc_baseaddr[7]);
#ifndef CONFIG_ARM64
    register_hotcpu_notifier(&platform_cpu_up_notifier);
    register_hotcpu_notifier(&platform_cpu_down_notifier);
#endif
    return 0;
}

static void __exit platform_hotplug_exit(void)
{
#ifndef CONFIG_ARM64
    unregister_hotcpu_notifier(&platform_cpu_up_notifier);
    unregister_hotcpu_notifier(&platform_cpu_down_notifier);
#endif
    printk("%s: %lu%lu %lu %lu\n", __FUNCTION__, g_acpu_core_sc_baseaddr[0],
        g_acpu_core_sc_baseaddr[1], g_acpu_core_sc_baseaddr[2],
        g_acpu_core_sc_baseaddr[3]);
    printk("%s: %lu %lu %lu %lu\n", __FUNCTION__, g_acpu_core_sc_baseaddr[4],
        g_acpu_core_sc_baseaddr[5], g_acpu_core_sc_baseaddr[6],
        g_acpu_core_sc_baseaddr[7]);
    g_acpu_core_sc_baseaddr[0] = 0;
    g_acpu_core_sc_baseaddr[1] = 0;
    g_acpu_core_sc_baseaddr[2] = 0;
    g_acpu_core_sc_baseaddr[3] = 0;
    g_acpu_core_sc_baseaddr[4] = 0;
    g_acpu_core_sc_baseaddr[5] = 0;
    g_acpu_core_sc_baseaddr[6] = 0;
    g_acpu_core_sc_baseaddr[7] = 0;
    return;
}

module_init(platform_hotplug_init);
module_exit(platform_hotplug_exit);
