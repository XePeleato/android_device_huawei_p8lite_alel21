/*
  * hisi_pm_psci.c
  *
  * Copyright (c) 2013 Hisilicon Technologies CO., Ltd.
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License version 2 as
  * published by the Free Software Foundation.
  */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/irqchip/arm-gic.h>
#include <asm/mcpm.h>
#include <asm/proc-fns.h>
#include <asm/cacheflush.h>
#include <asm/psci.h>
#include <asm/atomic.h>
#include <asm/cputype.h>
#include <asm/cp15.h>
#include <linux/arm-cci.h>
#include <linux/of_address.h>
/*#include "drv_ipcm.h"*/
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */
#include "soc_ao_sctrl_interface.h"
#include <linux/hisi/hi6xxx-ipc.h>                   /* For IO_ADDRESS access */
/*#include "drv_ipcm.h"*/
#include <linux/hisi/pm/pwrctrl_multi_memcfg.h>
#include <linux/dma-mapping.h>
#include <asm/tlbflush.h>
#include "bsp_drv_ipc.h"                            /*for ipc api*/

/*lint -e750*/
/*#define PSCI_POWER_STATE_ID     (0)*/
#define HISI_MAX_CPUS		(4)
#define HISI_MAX_CLUSTERS	(2)
/*#define CLUTER_OFF_RESIDENCY (3000)*/
/*#define SLEEP_OFF_RESIDENCY (5000)*/
#define ACPU_32_BIT_CPUILDE
#define PM_LOCAL  static

int g_a7_core_state[4] = {0,0,0,0};

PM_LOCAL atomic_t hisi_pm_use_count[HISI_MAX_CPUS][HISI_MAX_CLUSTERS];
PM_LOCAL arch_spinlock_t hisi_pm_lock = __ARCH_SPIN_LOCK_UNLOCKED;
#if 0
PM_LOCAL arch_spinlock_t* hisi_suspend_lock = 0;

PM_LOCAL unsigned long		flags;
#endif
#if defined(CONFIG_SECURE_EXTENSION)
/* sysctrl and pctrl base  */
PM_LOCAL void __iomem *sysctrl_base;
PM_LOCAL void __iomem *pctrl_base;
#endif

#define RES0_LOCK_OFF	0x400
#define RES0_UNLOCK_OFF	0x404
#define RES0_LOCK_STAT_OFF	0x408

#define RES1_LOCK_OFF	0x40c
#define RES1_UNLOCK_OFF	0x410
#define RES1_LOCK_STAT_OFF	0x414

#define RES_LOCK_OFF(cluster)  (cluster == 0 ? RES0_LOCK_OFF : RES1_LOCK_OFF)
#define RES_UNLOCK_OFF(cluster)  (cluster == 0 ? RES0_UNLOCK_OFF : RES1_UNLOCK_OFF)
#define RES_LOCK_STAT_OFF(cluster)  (cluster == 0 ? RES0_LOCK_STAT_OFF : RES1_LOCK_STAT_OFF)

#define LOCK_BIT		(0x1 << 28)
#define LOCK_ID_MASK	(0x7 << 29)
#define LOCK_ID(core)	((0x6 - core)  << 29)
#define	LOCK_VAL(core)		(LOCK_ID(core) | LOCK_BIT)


#define SCBAKDATA8_OFFSET	0x334
#define SCBAKDATA9_OFFSET	0x338
/* scbakdata8 and scbakdata9  are used for cluster 0 and 1 */
#define CLUSTER_IDLE_STAT(cluster)  (cluster == 0 ? SCBAKDATA8_OFFSET : SCBAKDATA9_OFFSET)

/* core idle bit */
#define	CORE_IDLE_BIT(core)		(0x1 << core)
/* cluster idle bit */
#define CLUSTER_IDLE_BIT		 (0x1 << 8)
/*#define CLUSTER_IDLE_MASK	((0xf) | CLUSTER_IDLE_BIT)*/
/*lint +e750*/

PM_LOCAL volatile unsigned int  *g_pacpu_unlock_core_flag = NULL;
PM_LOCAL volatile unsigned int* cluster_core_state;
PM_LOCAL volatile unsigned int* cluster_pd_flag;
PM_LOCAL volatile unsigned int* acpu_a53_flag;
PM_LOCAL unsigned int cpu_type = 0;/*1:A53,0:other*/
PM_LOCAL unsigned int board_type = 1;/*1:asic,0:sft*/

extern unsigned int    g_acpu_unlock_flag_tmp;
void hisi_unlock_core_fordebug(void);
void hisi_unlock_core_fordebug(void)
{
    if (1 == g_acpu_unlock_flag_tmp)
    {
        asm volatile("MOV r0, #0x0");
        asm volatile("MCR p14, 0, r0, c1, c0, 4");
    }
}
#define CLUSTER_IDLE_MASK(cluster)    ((0x3|(0xc*board_type)) << cluster*4)


extern void hisi_cluster_exit_coherence(u32 cluster);
extern int get_cpu_type(void);
extern void hisi_set_cluster_wfi(unsigned int id);
extern void hisi_pm_cci_disable(unsigned int id);
extern void flush_tlb_all(void);
extern void set_cluster_idle_bit(unsigned int cluster);
extern void clr_cluster_idle_bit(unsigned int cluster);
/*extern void __iomem *g_acpu_sc_base_map;*/

int get_cpu_type(void)
{
    return cpu_type;
}

void get_resource_lock(unsigned int cluster, unsigned int core)
{
#if defined(CONFIG_SECURE_EXTENSION)
	do {
		writel(LOCK_VAL(core), pctrl_base + RES_LOCK_OFF(cluster));
	} while((readl(pctrl_base + RES_LOCK_STAT_OFF(cluster)) & LOCK_ID_MASK) != LOCK_ID(core));
#endif
}
EXPORT_SYMBOL(get_resource_lock);

void put_resource_lock(unsigned int cluster, unsigned int core)
{
#if defined(CONFIG_SECURE_EXTENSION)
	writel(LOCK_VAL(core), pctrl_base + RES_UNLOCK_OFF(cluster));
#endif
}
EXPORT_SYMBOL(put_resource_lock);

void set_cluster_idle_bit(unsigned int cluster)
{
    BSP_IPC_SpinLock(IPC_SEM_CPUIDLE);
    if(cluster == 1)
    {
        if((13== ((CLUSTER_IDLE_MASK(cluster)& (*cluster_core_state))>>4))|
        (14== ((CLUSTER_IDLE_MASK(cluster)& (*cluster_core_state))>>4))|
        (7== ((CLUSTER_IDLE_MASK(cluster)& (*cluster_core_state))>>4))|
        (11== ((CLUSTER_IDLE_MASK(cluster)& (*cluster_core_state))>>4)))
        {
            (*cluster_pd_flag) |= BIT(cluster);
        }
    }
    else if(cluster == 0)
    {
        if((13== (CLUSTER_IDLE_MASK(cluster)& (*cluster_core_state)))|
        (14== (CLUSTER_IDLE_MASK(cluster)& (*cluster_core_state)))|
        (7== (CLUSTER_IDLE_MASK(cluster)& (*cluster_core_state)))|
        (11== (CLUSTER_IDLE_MASK(cluster)& (*cluster_core_state))))
        {
            (*cluster_pd_flag) |= BIT(cluster);
        }
   }
    BSP_IPC_SpinUnLock(IPC_SEM_CPUIDLE);

}

void clr_cluster_idle_bit(unsigned int cluster)
{
    BSP_IPC_SpinLock(IPC_SEM_CPUIDLE);
    (*cluster_pd_flag) &= ~ BIT(cluster);
    BSP_IPC_SpinUnLock(IPC_SEM_CPUIDLE);
}
EXPORT_SYMBOL(set_cluster_idle_bit);
EXPORT_SYMBOL(clr_cluster_idle_bit);

static void arm_v8_smp_dis(unsigned val0,unsigned val1)
{
    asm volatile("mrrc p15, 1, r0, r1, c15");
    asm volatile("bic r0,r0,#0x40");
    asm volatile("mcrr p15, 1, r0, r1, c15");
    isb();
    dsb();    
}

/*just for A7/A9*/
static void hisi_core_exit_coherence(void)
{
    unsigned int val = 0;
    if(get_cpu_type()){
        return;
    }

    set_cr(get_cr() & ~CR_C);
	
	if((g_a7_core_state[0] && g_a7_core_state[1] && g_a7_core_state[2] && g_a7_core_state[3]) == 1)
	{
		flush_cache_all();
		int i;
		for (i=0;i<4;i++)
		{
			g_a7_core_state[i] = 0;
		}
	}
	else
	{
		flush_cache_louis();
	}
	
	asm volatile ("clrex");
	flush_bp_all();
	local_flush_tlb_all();
    asm volatile("mcr	p15, 0, %0, c1, c0, 1	@ set CR"
	  : : "r" (val) : "cc");
	isb();
	dsb();
}
/*just for A53*/
void hisi_cluster_exit_coherence(u32 cluster)
{
    volatile unsigned int val0 = 0;
    volatile unsigned int val1 = 0;

    if(!get_cpu_type()){
        return;
    }
 	  
    flush_cache_all_pm();
	asm volatile("clrex");
	flush_tlb_all();

    arm_v8_smp_dis(val0,val1);
    	        	
    hisi_pm_cci_disable(cluster);
	/*
	 * Ensure that both C & I bits are disabled in the SCTLR
	 * before disabling ACE snoops. This ensures that no
	 * coherency traffic will originate from this cpu after
	 * ACE snoops are turned off.
	 */
	    
	cpu_proc_fin();
}

PM_LOCAL void hisi_pm_psci_system_off(void)
{
	BUG_ON(!psci_ops.system_off);

	psci_ops.system_off();

	while(1)
		asm volatile ("wfi");

	return ;
}
/*extern unsigned int get_cci_port_s4(void);*/
PM_LOCAL int hisi_pm_psci_power_up(unsigned int cpu, unsigned int cluster)
{
#if defined(CONFIG_SECURE_EXTENSION)
	unsigned int mpidr = (cluster << 8) | cpu;
#endif
	int ret = 0;
#if defined(CONFIG_SECURE_EXTENSION)
	BUG_ON(!psci_ops.cpu_on);
#endif
	local_irq_disable();

    
	arch_spin_lock(&hisi_pm_lock);
	switch (atomic_inc_return(&hisi_pm_use_count[cpu][cluster])) {
	case 1:
		/*
		 * This is a request to power up a cpu that linux thinks has
		 * been powered down. Retries are needed if the firmware has
		 * seen the power down request as yet.
		 */
#if 0
        if(get_cpu_type())
        {
            BSP_IPC_SpinLock(IPC_SEM_CPUIDLE);				
            *cluster_core_state &= ~(BIT(cpu)<<cluster*4);
            BSP_IPC_SpinUnLock(IPC_SEM_CPUIDLE);
        }
#endif
		local_irq_enable();
		arch_spin_unlock(&hisi_pm_lock);
		
#if defined(CONFIG_SECURE_EXTENSION)
		do {
			ret = psci_ops.cpu_on(mpidr,
					      virt_to_phys(mcpm_entry_point));
		} while (ret == -EAGAIN);
#endif
              hisi_unlock_core_fordebug();
        /*printk(KERN_ERR"%s:use count %d,cci:0x%x.\n",__FUNCTION__, 1,get_cci_port_s4());*/
		return ret;
	case 2:
		/* This power up request has overtaken a power down request */
		local_irq_enable();
		arch_spin_unlock(&hisi_pm_lock);
		/*printk(KERN_ERR"%s:use count %d\n",__FUNCTION__, 2);*/
		return ret;
	default:
		/* Any other value is a bug */
		BUG();
	}
}

PM_LOCAL void hisi_pm_psci_power_down(void)
{
	/*struct psci_power_state power_state;*/
	unsigned int mpidr, cpu, cluster;
	
    volatile unsigned int val0 = 0;
    volatile unsigned int val1 = 0;

    
	mpidr = read_cpuid_mpidr();
	cpu = MPIDR_AFFINITY_LEVEL(mpidr, 0);
	cluster = MPIDR_AFFINITY_LEVEL(mpidr, 1);

#if defined(CONFIG_SECURE_EXTENSION)
	BUG_ON(!psci_ops.cpu_off);
#endif
   
	arch_spin_lock(&hisi_pm_lock);
	switch (atomic_dec_return(&hisi_pm_use_count[cpu][cluster])) {
	case 1:
		/*
		 * Overtaken by a power up. Flush caches, exit coherency,
		 * return & fake a reset
		 */
		arch_spin_unlock(&hisi_pm_lock);
		if(!get_cpu_type())
		{
		    hisi_core_exit_coherence();
		}
		else
		{
			/*A53 L1 cache flush differ from A9/A7,function call can't pop from stack if C bit is clear*/
	    	flush_cache_louis_pm();  
        	asm volatile("clrex");
        	flush_tlb_all();	

	        arm_v8_smp_dis(val0,val1);
        }		
		set_auxcr(get_auxcr() & ~(1 << 6));
        /*printk(KERN_ERR"%s:use count %d\n",__FUNCTION__, 1);	*/	
		BUG();
		return;
	case 0:
	    /*printk(KERN_ERR"%s:use count %d\n",__FUNCTION__, 0);*/
		/* A normal request to possibly power down the cluster */
#if defined(CONFIG_SECURE_EXTENSION)		
		power_state.id = PSCI_POWER_STATE_ID;
		power_state.type = PSCI_POWER_STATE_TYPE_POWER_DOWN;
		power_state.affinity_level = PSCI_POWER_STATE_AFFINITY_LEVEL1;
#endif		
		gic_cpu_if_down();
		/*es*/
		if(!get_cpu_type())
		{
			arch_spin_unlock(&hisi_pm_lock);		
		    hisi_core_exit_coherence();
		}
		else/*cs*/
		{
            BSP_IPC_SpinLock(IPC_SEM_CPUIDLE);		
            *cluster_core_state |= (BIT(cpu)<<cluster*4);
            BSP_IPC_SpinUnLock(IPC_SEM_CPUIDLE);            
            
    		/*asic, 4 core per cluster*/
			if(board_type)
			{
			    if(!atomic_read(&hisi_pm_use_count[0][cluster])&&
			        !atomic_read(&hisi_pm_use_count[1][cluster])&&
			        !atomic_read(&hisi_pm_use_count[2][cluster])&&
			        !atomic_read(&hisi_pm_use_count[3][cluster])){
    				arch_spin_unlock(&hisi_pm_lock);
    				hisi_cluster_exit_coherence(cluster);    			    
			    }
			    else
			    {
        			arch_spin_unlock(&hisi_pm_lock);
                 
                    /*A53 L1 cache flush differ from A9/A7,function call can't pop from stack if C bit is clear*/
                	flush_cache_louis_pm();
                	asm volatile("clrex");
                	flush_tlb_all();

        	        arm_v8_smp_dis(val0,val1);

        	    }

    		} else {
    		    if(!atomic_read(&hisi_pm_use_count[0][cluster])&&
			        !atomic_read(&hisi_pm_use_count[1][cluster])){
    				arch_spin_unlock(&hisi_pm_lock);
    				hisi_cluster_exit_coherence(cluster);
			    }
			    else{
        			arch_spin_unlock(&hisi_pm_lock);
                 
                    /*A53 L1 cache flush differ from A9/A7,function call can't pop from stack if C bit is clear*/
                	flush_cache_louis_pm();
                	asm volatile("clrex");
                	flush_tlb_all();

        	        arm_v8_smp_dis(val0,val1);
                    
                }
    	    }
	    }
#if defined(CONFIG_SECURE_EXTENSION)
		psci_ops.cpu_off(power_state);
#endif
        dsb();
		while(1)
			wfi();
		return;
	default:
		/* Any other value is a bug */
		BUG();
	}
}

PM_LOCAL void hisi_pm_psci_suspend(u64 unused)
{
    volatile unsigned int val0 = 0;
    volatile unsigned int val1 = 0;

#if defined(CONFIG_SECURE_EXTENSION)
	struct psci_power_state power_state;
#endif
	unsigned int mpidr, cluster, core;
#if defined(CONFIG_SECURE_EXTENSION)
	unsigned long cluster_state = 0;
#endif

	mpidr = read_cpuid_mpidr();
	core = MPIDR_AFFINITY_LEVEL(mpidr, 0);
	cluster = MPIDR_AFFINITY_LEVEL(mpidr, 1);

#if defined(CONFIG_SECURE_EXTENSION)
	BUG_ON(!psci_ops.cpu_suspend);
#endif

	gic_cpu_if_down();

		

#if defined(CONFIG_SECURE_EXTENSION)
	if (unused != PSCI_POWER_STATE_AFFINITY_LEVEL3) {
		/* sync with lpm3 */
		get_resource_lock(cluster, core);

		/* set the core down bit */
		cluster_state = readl(sysctrl_base + CLUSTER_IDLE_STAT(cluster));
		cluster_state |= CORE_IDLE_BIT(core);
		writel(cluster_state, sysctrl_base + CLUSTER_IDLE_STAT(cluster));

		cluster_state = readl(sysctrl_base + CLUSTER_IDLE_STAT(cluster));
		if ((cluster_state & CLUSTER_IDLE_MASK) == CLUSTER_IDLE_MASK) {
			hisi_cluster_exit_coherence(cluster);
		} else {
			unused = PSCI_POWER_STATE_AFFINITY_LEVEL0;
		}

		put_resource_lock(cluster, core);
	} else {
		hisi_cluster_exit_coherence(cluster);
	}

	if (unused == PSCI_POWER_STATE_AFFINITY_LEVEL0)
		hisi_core_exit_coherence();
#endif 
    if(get_cpu_type())
    {
        if (unused != PSCI_POWER_STATE_AFFINITY_LEVEL3)
        {
            /*arch_spin_lock(hisi_suspend_lock);*/
            /* sync with lpm3 */        
            BSP_IPC_SpinLock(IPC_SEM_CPUIDLE);

            /*set the core down bit*/
            if(atomic_read(&hisi_pm_use_count[core][cluster]))
            {
                *cluster_core_state |= (BIT(core)<<cluster*4);
            }
            /*cluster down */
            if((CLUSTER_IDLE_MASK(cluster) == (CLUSTER_IDLE_MASK(cluster)& (*cluster_core_state)))&&(0 != ((*cluster_pd_flag) & BIT(cluster))))
            {
                unused = PSCI_POWER_STATE_AFFINITY_LEVEL3;
                /*printk(KERN_ERR"cluster down,core:%d,cluster:%d.\n",core,cluster);*/
                hisi_cluster_exit_coherence(cluster);
                /*hisi_set_cluster_wfi(cluster);*/
            }
            else/*core down*/
            {   
                unused = PSCI_POWER_STATE_AFFINITY_LEVEL0;
            }
            /*arch_spin_unlock(hisi_suspend_lock);*/
            BSP_IPC_SpinUnLock(IPC_SEM_CPUIDLE);

        }
    	else{
    	    hisi_cluster_exit_coherence(0);
    	}

        /*A53 L1 cache flush differ from A9/A7,function call can't pop from stack if C bit is clear*/
    	if (unused == PSCI_POWER_STATE_AFFINITY_LEVEL0)
    	{
                flush_cache_louis_pm();
            	asm volatile("clrex");
            	flush_tlb_all();

                arm_v8_smp_dis(val0,val1);                

    	}
    }
    else{
        g_a7_core_state[core] = 1;
        hisi_core_exit_coherence();
    }		

    
#if defined(CONFIG_SECURE_EXTENSION)
	/* construct idle power state */
	power_state.id = PSCI_POWER_STATE_ID;
	power_state.type = PSCI_POWER_STATE_TYPE_POWER_DOWN;
	power_state.affinity_level = (u8)unused;
#endif

#if 0/*tracer func*/
    val0 = *(unsigned int*)(g_acpu_sc_base_map+0x2c);
    val0 &= ~ BIT(core+cluster*8);
    *(unsigned int*)(g_acpu_sc_base_map+0x2c) = val0;
#endif    
    /*send ipc to mcu, need add code here*/
    hisi_ipc_core_suspend(core, cluster, PSCI_POWER_STATE_TYPE_POWER_DOWN);

    dsb();
	while(1)
		wfi();

#if defined(CONFIG_SECURE_EXTENSION)
	psci_ops.cpu_suspend(power_state, virt_to_phys(mcpm_entry_point));
#endif
	/* On success this function never returns */
	BUG();
}

PM_LOCAL const struct mcpm_platform_ops hisi_pm_power_ops = {
	.power_up      = hisi_pm_psci_power_up,
	.power_down    = hisi_pm_psci_power_down,
	.suspend       = hisi_pm_psci_suspend,
};

PM_LOCAL void __init hisi_pm_usage_count_init(void)
{
	unsigned int mpidr, cpu, cluster;

	mpidr = read_cpuid_mpidr();
	cpu = MPIDR_AFFINITY_LEVEL(mpidr, 0);
	cluster = MPIDR_AFFINITY_LEVEL(mpidr, 1);

	pr_debug("%s: cpu %u cluster %u\n", __func__, cpu, cluster);

	atomic_set(&hisi_pm_use_count[cpu][cluster], 1);
}
#if 0
void suspend_lock_init(void)
{
	dma_addr_t phy;

    if(0 == hisi_suspend_lock)
    {
    	hisi_suspend_lock = (arch_spinlock_t*)dma_alloc_coherent(0,sizeof(arch_spinlock_t),&phy,GFP_KERNEL);
    	if(NULL == hisi_suspend_lock)
    	{
    	    printk(KERN_ERR"###hisi_suspend_lock alloc fail.\n");
    	    return;
    	}	
        memset(hisi_suspend_lock,0,sizeof(arch_spinlock_t));
    }
}
#endif
PM_LOCAL int __init hisi_pm_psci_init(void)
{
#if 1/*defined(CONFIG_SECURE_EXTENSION)*/
	struct device_node *np = NULL;
#endif
	int ret;
	unsigned int val;

#if defined(CONFIG_SECURE_EXTENSION)
	ret = psci_probe();
	if (ret) {
		pr_err("psci not found. Aborting psci init\n");
		return -ENODEV;
	}
#endif
	cluster_core_state = (unsigned int*)ioremap(ACPU_CORE_BITS_ADDR,SZ_8);
	cluster_pd_flag = (unsigned int*)ioremap(ACPU_CLUSTER_IDLE_ADDR,SZ_8);
	acpu_a53_flag=(unsigned int*)ioremap(ACPU_A53_FLAGS_ADDR,SZ_8);
	g_pacpu_unlock_core_flag = (unsigned int*)ioremap(ACPU_UNLOCK_CORE_FLAGS_ADDR,SZ_8);
    *g_pacpu_unlock_core_flag = g_acpu_unlock_flag_tmp;

    if(of_find_compatible_node(NULL, NULL, "arm,cortex-a53"))
    {
    	cpu_type =1;
    	*acpu_a53_flag = 1;
    }
    else
    {
    	cpu_type = 0;
    	*acpu_a53_flag = 0;
    }
	np = of_find_compatible_node(NULL, NULL, "arm,generic");
	if(np)
	{
        of_property_read_u32(np, "product_id",&val);
        
        if(val)
        {
            board_type = 1;/*asic*/
        }
        else
        {
            board_type = 0;/*sft*/
        }
	}
        	
       printk(KERN_ERR"cpu_type:%d,board_type:%d,A53 Flag addr:%p,val:0x%x.\n",cpu_type,board_type,acpu_a53_flag,*acpu_a53_flag);

	pm_power_off = hisi_pm_psci_system_off;

	hisi_pm_usage_count_init();

#if defined(CONFIG_SECURE_EXTENSION)
	if (sysctrl_base == NULL) {
		np = of_find_compatible_node(NULL, NULL, "hisilicon,sysctrl");
		if (!np) {
			pr_err("get sysctrl node error !\n");
			BUG_ON(1);
			return -ENODEV;
		}
		sysctrl_base = of_iomap(np, 0);
		BUG_ON(!sysctrl_base);
	}

	if (pctrl_base == NULL) {
		np = of_find_compatible_node(NULL, NULL, "hisilicon,pctrl");
		if (!np) {
			pr_err("get pctrl node error !\n");
			BUG_ON(1);
			return -ENODEV;
		}
		pctrl_base = of_iomap(np, 0);
		BUG_ON(!pctrl_base);
	}
#endif

	ret = mcpm_platform_register(&hisi_pm_power_ops);
	if (!ret)
		ret = mcpm_sync_init(NULL);
	if (!ret)
		pr_info("Hisilicon psci power management initialized\n");

	return ret;
}

early_initcall(hisi_pm_psci_init);
MODULE_DESCRIPTION("Hisilicon psci power management driver");
MODULE_AUTHOR("zhanjun 00183814");
MODULE_LICENSE("GPL");
