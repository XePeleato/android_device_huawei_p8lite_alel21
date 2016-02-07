
#include "debug_func.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
变量引用
*****************************************************************************/

/*****************************************************************************
函数声明
*****************************************************************************/
#ifdef CONFIG_ARCH_HI6XXX
extern unsigned int omTimerGet(void);
extern void reg_dbg_dump(unsigned long long pAddr, unsigned long long size, unsigned long long nodesize);
#endif

extern unsigned int  wdg_dbg_readreg(unsigned char* uaddr);
extern void wdg_dbg_enable_all(void);
extern void wdg_dbg_disable_all(void);


/*****************************************************************************
函数定义
*****************************************************************************/
/***********************************************************
 Function: wdg_dbg_enable
 Description: Enable watch dog of A core,C core or M core
 Input:
        ewdg_type, balong wdg type:0,Acore wdg;1,Ccore wdg;2,mcu local wdg;3,mcu global wdg.
        ewdg_type, k3v3+ wdg type:0,Acore wdg;2,mcu wdg;
 return:   0,if ok;-1 if ko.
 History:
 1.    20140121   Creat
************************************************************/
int wdg_dbg_enable(dbg_wdg_type ewdg_type)
{
    int     iret = 0;
    unsigned char*    ulBaseAddr = NULL;
    unsigned char*    clBaseAddr = NULL;
    unsigned long     regAddr = 0;
    struct device_node*  np;
    unsigned int board_id = 0;
    int ret = 0;

    np = of_find_compatible_node(NULL, NULL, "arm,sp805");
    if (IS_ERR(np))
    {
        printk("Can not find sp805 node\n");
        return -1;
    }
    /*if platform is k3v3+*/
    ret = of_property_read_u32_index(np, "board_id", 0, &board_id);
    if (ret)
    {
         switch (ewdg_type)
         {
         case DBG_WDG_ACORE:
            /*K3V3+ ACPU WDG*/
            ulBaseAddr = ioremap(SOC_K3_WDG_ACPU_ADDR,SZ_4K);
         /* open R/W */
            writel(WDT_REG_VALUE_UNLOCK, WDT_WDG_LOCK_ADDR(ulBaseAddr));
            writel(INT_MASK, WDT_WDG_INTCLR_ADDR(ulBaseAddr ));

          /* K3V3+ACPU WDG enable */
            writel(INT_ENABLE | RESET_ENABLE, WDT_WDG_CONTROL_ADDR(ulBaseAddr));
            writel(WDT_REG_VALUE_LOCK, WDT_WDG_LOCK_ADDR(ulBaseAddr ));
            break;
         case DBG_WDG_LOCAL_MCU:
            ulBaseAddr = ioremap(SOC_K3_WDG_LPM3_ADDR, SZ_4K);
            /* open R/W */
            writel(WDT_REG_VALUE_UNLOCK, WDT_WDG_LOCK_ADDR(ulBaseAddr));
            writel(CCORE_WDT_CLK_CNT_PER_S*DEFAULT_TIME, WDT_WDG_LOAD_ADDR(ulBaseAddr));

            /* K3V3+ LPM3 WDG enable */
            writel(INT_ENABLE | RESET_ENABLE, WDT_WDG_CONTROL_ADDR(ulBaseAddr));
            writel(WDT_REG_VALUE_LOCK, WDT_WDG_LOCK_ADDR(ulBaseAddr));
         break;
         default:
            printk("wrong core to set wdg!!\n");
            iret = -1;
            break;
         }
         iounmap((void*)ulBaseAddr);
         ulBaseAddr = NULL;
    }
    else if(BOARD_ID_INFO == board_id)
    {
        switch (ewdg_type)
        {
        case DBG_WDG_ACORE:
        /* 监控Balong ACPU，由ACPU负责监控，由MCU处理复位请求或引起全局复位 */
            ulBaseAddr = ioremap(SOC_BALONG_AO_SCTRL_BASE_ADDR,SZ_8K);
            writel(BIT(BALONG_WDG_CLKSTART_BIT),
                  SOC_BALONG_AO_SCTRL_SC_PERIPH_CLKEN4_ADDR(ulBaseAddr));

            break;
        case DBG_WDG_CCORE:
          ulBaseAddr = ioremap(SOC_BALONG_AO_SCTRL_BASE_ADDR,SZ_8K);
         /*to get ccpu is or not sleep*/
         regAddr = readl(ulBaseAddr + BALONG_CCPU_SLEEP_OFFSET);
         if(0 != (regAddr & BIT(BALONG_CCPU_SLEEP_BIT)))
        {
            clBaseAddr = ioremap(SOC_BALONG_MODEM_SCTRL_BASE_ADDR,SZ_8K);
            writel(BIT(BALONG_WDG_CLKSTART_BIT),
                     SOC_BALONG_MODEM_SCTRL_MDMSC_AO_CLKEN_ADDR(clBaseAddr));
            iounmap((void*)clBaseAddr);
            clBaseAddr = NULL;
            break;
        }
        else
        {
            printk(KERN_ERR "ccpu is slssp, can not enable\n");
            break;
        }
        case DBG_WDG_LOCAL_MCU:
            /*balong mcu local wdg set bit[6]*/
            ulBaseAddr = ioremap(SOC_BALONG_AO_SCTRL_BASE_ADDR,SZ_8K);
            writel(BIT(BALONG_MLWDG_CLKSTART_BIT),
                  SOC_BALONG_AO_SCTRL_SC_PERIPH_CLKEN4_ADDR(ulBaseAddr));
            break;
        case DBG_WDG_GLOBAL_MCU:
            /*balong mcu global wdg set bit[7]*/
            ulBaseAddr = ioremap(SOC_BALONG_AO_SCTRL_BASE_ADDR,SZ_8K);
            writel(BIT(BALONG_MGWDG_CLKSTART_BIT),
                  SOC_BALONG_AO_SCTRL_SC_PERIPH_CLKEN4_ADDR(ulBaseAddr));
            break;
        default:
            printk("wrong core to set wdg!!\n");
            iret = -1;
            break;
        }
        iounmap((void*)ulBaseAddr);
        ulBaseAddr = NULL;
    }
    return iret;
}

/***********************************************************
 Function: wdg_dbg_disable
 Description: Disable watch dog of A core,C core or M core
 Input:
        ewdg_type, wdg type:0,Acore wdg;1,Ccore wdg;2,mcu local wdg;3,mcu global wdg.
        ewdg_type, k3v3+ wdg type:0,Acore wdg;2,mcu wdg;
 return:   0,if ok;-1 of ko.
 History:
 1.    20140121   Creat
  2.   20140919   modify
************************************************************/
int wdg_dbg_disable(dbg_wdg_type ewdg_type)
{
    int     iret = 0;
    unsigned char*    ulBaseAddr = NULL;
    unsigned char*    clBaseAddr = NULL;
    unsigned long     regAddr = 0;
    struct device_node*  np;
    unsigned int board_id = 0;
    int ret = 0;

    np = of_find_compatible_node(NULL, NULL, "arm,sp805");
    if (IS_ERR(np))
    {
        printk("Can not find sp805 node\n");
        return -1;
    }
    /*if platform is k3v3+*/
    ret = of_property_read_u32_index(np, "board_id", 0, &board_id);
    if (ret)
    {
         switch (ewdg_type)
         {
          case DBG_WDG_ACORE:
            /*K3V3+ ACPU WDG disable*/
            ulBaseAddr = ioremap(SOC_K3_WDG_ACPU_ADDR,SZ_4K);
            writel(CCORE_WDT_REG_VALUE_UNLOCK, WDT_WDG_LOCK_ADDR(ulBaseAddr ));
            writel(0, WDT_WDG_CONTROL_ADDR(ulBaseAddr));
            writel(1, WDT_WDG_LOCK_ADDR(ulBaseAddr ));
            break;
         case DBG_WDG_LOCAL_MCU:
            ulBaseAddr = ioremap(SOC_K3_WDG_LPM3_ADDR, SZ_4K);
            /* open R/W */
            writel(CCORE_WDT_REG_VALUE_UNLOCK, WDT_WDG_LOCK_ADDR(ulBaseAddr));

            /* K3V3+ LPM3 WDG disable */
            writel(0, WDT_WDG_CONTROL_ADDR(ulBaseAddr));

            writel(BIT(0), WDT_WDG_LOCK_ADDR(ulBaseAddr));
                    break;
         default:
            printk("wrong core to set wdg!!\n");
            iret = -1;
            break;
         }
         iounmap((void*)ulBaseAddr);
         ulBaseAddr = NULL;
    }
    else if(BOARD_ID_INFO == board_id)
    {
        /*reg write referencing tsBSP_WDT_unInit in C core*/
        switch (ewdg_type)
        {
        case DBG_WDG_ACORE:
        /* 监控Balong ACPU，由ACPU负责监控，由MCU处理复位请求或引起全局复位 */
            ulBaseAddr = ioremap(SOC_BALONG_AO_SCTRL_BASE_ADDR,SZ_8K);
            writel(BIT(BALONG_WDG_CLKSTART_BIT),
                  SOC_BALONG_AO_SCTRL_SC_PERIPH_CLKDIS4_ADDR(ulBaseAddr));
            break;
        case DBG_WDG_CCORE:
            ulBaseAddr = ioremap(SOC_BALONG_AO_SCTRL_BASE_ADDR,SZ_8K);
        regAddr = readl(ulBaseAddr + BALONG_CCPU_SLEEP_OFFSET);
        if(0 != (regAddr & BIT(BALONG_CCPU_SLEEP_BIT)))
        {
              clBaseAddr = ioremap(SOC_BALONG_MODEM_SCTRL_BASE_ADDR,SZ_8K);
              writel(BIT(BALONG_WDG_CLKSTART_BIT),
                        SOC_BALONG_MODEM_SCTRL_MDMSC_AO_CLKDIS_ADDR(clBaseAddr));
              iounmap((void*)clBaseAddr);
              clBaseAddr = NULL;
              break;
        }
        else
        {
              printk(KERN_ERR "ccpu is slssp, can not disable\n");
              break;
        }
        case DBG_WDG_LOCAL_MCU:
            /*balong mcu local wdg*/
            ulBaseAddr = ioremap(SOC_BALONG_AO_SCTRL_BASE_ADDR,SZ_8K);
            writel(BIT(BALONG_MLWDG_CLKSTART_BIT),
                  SOC_BALONG_AO_SCTRL_SC_PERIPH_CLKDIS4_ADDR(ulBaseAddr));

            break;
        case DBG_WDG_GLOBAL_MCU:
            /*balong mcu global wdg*/
            ulBaseAddr = ioremap(SOC_BALONG_AO_SCTRL_BASE_ADDR,SZ_8K);
            writel(BIT(BALONG_MGWDG_CLKSTART_BIT),
                  SOC_BALONG_AO_SCTRL_SC_PERIPH_CLKDIS4_ADDR(ulBaseAddr));
            break;

        default:
            printk("wrong core to set wdg!!\n");
            iret = -1;
            break;
        }
        iounmap((void*)ulBaseAddr);
        ulBaseAddr = NULL;
   }

    return iret;
}

/***********************************************************
 Function: wdg_dbg_enable_all
 Description: Enable watch dog of A core,C core or M core
 Input:none
 return: none
 History:
 1.    20140121   Creat
************************************************************/
void wdg_dbg_enable_all(void)
{
    struct device_node*  np;
    unsigned int board_id = 0;
    int ret = 0;

    np = of_find_compatible_node(NULL, NULL, "arm,sp805");
    if (IS_ERR(np))
    {
        printk("Can not find sp805 node\n");
    }

    ret = of_property_read_u32_index(np, "board_id", 0, &board_id);
    if (ret)
    {
        wdg_dbg_enable(DBG_WDG_ACORE);
        wdg_dbg_enable(DBG_WDG_LOCAL_MCU);
    }
    else if(BOARD_ID_INFO == board_id)
    {
        wdg_dbg_enable(DBG_WDG_ACORE);
        wdg_dbg_enable(DBG_WDG_CCORE);
        wdg_dbg_enable(DBG_WDG_LOCAL_MCU);
        wdg_dbg_enable(DBG_WDG_GLOBAL_MCU);
    }
}

/***********************************************************
 Function: wdg_dbg_disable_all
 Description: Disable watch dog of A core,C core or M core
 Input:none
 return: none
 History:
 1.    20140121   Creat
 2.    20140919   modify
************************************************************/
void wdg_dbg_disable_all(void)
{
    struct device_node*  np;
    unsigned int board_id = 0;
    int ret = 0;

    np = of_find_compatible_node(NULL, NULL, "arm,sp805");
    if (IS_ERR(np))
    {
        printk("Can not find sp805 node\n");
    }

    ret = of_property_read_u32_index(np, "board_id", 0, &board_id);
    if (ret)
    {
        wdg_dbg_disable(DBG_WDG_ACORE);
        wdg_dbg_disable(DBG_WDG_LOCAL_MCU);
    }
    else if(BOARD_ID_INFO == board_id)
    {
        wdg_dbg_disable(DBG_WDG_ACORE);
        wdg_dbg_disable(DBG_WDG_CCORE);
        wdg_dbg_disable(DBG_WDG_LOCAL_MCU);
        wdg_dbg_disable(DBG_WDG_GLOBAL_MCU);
    }
}
unsigned int  wdg_dbg_readreg(unsigned char* uaddr)
{
    return (*(volatile unsigned *) (uaddr));
}
/***********************************************************
 Function: wdg_dbg_settime
 Description: Change the time of the watch dog
 Input:
        ewdg_type, wdg type:0,Acore wdg;1,Ccore wdg;2,mcu local wdg;3,mcu global wdg.
         ival_msec, milliseconds
 return:   0,if ok;-1 of ko.
 History:
 1.    20140121   Creat
************************************************************/
int wdg_dbg_settime(dbg_wdg_type ewdg_type,int ival_msec)
{
    int     iret = 0;
    unsigned int    ulRegValue = 0;
    unsigned char*    ulBaseAddr = NULL;


    unsigned char*    clBaseAddr = NULL;
    unsigned long     regAddr = 0;
    struct device_node*  np;
    unsigned int board_id = 0;
    int ret = 0;

    np = of_find_compatible_node(NULL, NULL, "arm,sp805");
    if (IS_ERR(np))
    {
        printk("Can not find sp805 node\n");
        return -1;
    }
    /*if platform is K3V3+*/
    ret = of_property_read_u32_index(np, "board_id", 0, &board_id);
    if (ret)
    {
        switch (ewdg_type)
        {
        case DBG_WDG_ACORE:
            ulBaseAddr = ioremap(SOC_K3_WDG_ACPU_ADDR, SZ_4K);
            /* open R/W */
            writel(CCORE_WDT_REG_VALUE_UNLOCK, WDT_WDG_LOCK_ADDR(ulBaseAddr));

            /* set k3v3+ acpu watchdog timeout count */
            writel(CCORE_WDT_CLK_CNT_PER_S*ival_msec/1000, WDT_WDG_LOAD_ADDR(ulBaseAddr));

            writel(0, WDT_WDG_LOCK_ADDR(ulBaseAddr));

            iounmap((void*)ulBaseAddr);
            ulBaseAddr = NULL;
            break;
        case DBG_WDG_LOCAL_MCU:
            ulBaseAddr = ioremap(SOC_K3_WDG_LPM3_ADDR, SZ_4K);
            /* open R/W */
            writel(CCORE_WDT_REG_VALUE_UNLOCK, WDT_WDG_LOCK_ADDR(ulBaseAddr));

            /* set k3v3+ mcu watchdog timerout count */
            writel(CCORE_WDT_CLK_CNT_PER_S*ival_msec/1000, (ulBaseAddr));

            writel(0, WDT_WDG_LOCK_ADDR(ulBaseAddr));

            iounmap((void*)ulBaseAddr);
            ulBaseAddr = NULL;

            break;

        default:
            printk("wrong core to set wdg!!\n");
            iret = -1;
            break;
        }

    }
    else if(BOARD_ID_INFO == board_id)
    {
        switch (ewdg_type)
        {
        case DBG_WDG_ACORE:
            ulBaseAddr = ioremap(SOC_BALONG_WDG_ACPU_ADDR, SZ_4K);
            /* open R/W */
            writel(CCORE_WDT_REG_VALUE_UNLOCK, WDT_WDG_LOCK_ADDR(ulBaseAddr));

            /* set balong acpu watchdog timerout count */
            writel(CCORE_WDT_CLK_CNT_PER_S*ival_msec/1000, WDT_WDG_LOAD_ADDR(ulBaseAddr));

            iounmap((void*)ulBaseAddr);
            ulBaseAddr = NULL;

            break;
        case DBG_WDG_CCORE:
            ulBaseAddr = ioremap(SOC_BALONG_AO_SCTRL_BASE_ADDR,SZ_8K);
        regAddr = readl(ulBaseAddr + BALONG_CCPU_SLEEP_OFFSET);
        if(0 != (regAddr & BIT(BALONG_CCPU_SLEEP_BIT)))
            {
                clBaseAddr = ioremap(SOC_BALONG_WDG_CCPU_ADDR, SZ_4K);
                /* open R/W */
                writel(CCORE_WDT_REG_VALUE_UNLOCK, WDT_WDG_LOCK_ADDR(clBaseAddr));

                writel(0, WDT_WDG_CONTROL_ADDR(clBaseAddr));
                /* set balong watchdog timeout count */
                writel(CCORE_WDT_CLK_CNT_PER_S*ival_msec/1000, WDT_WDG_LOAD_ADDR(clBaseAddr));

                writel(0x3, WDT_WDG_CONTROL_ADDR(clBaseAddr));
                /* close R/W ,have to remove this, or acpu will be dead here!!(on sft platform)
                writel(CCORE_WDT_REG_VALUE_LOCK, WDT_WDG_LOCK_ADDR(ulBaseAddr));*/
                iounmap((void*)clBaseAddr);
                clBaseAddr = NULL;
                break;
            }
                 else
                 {
                  printk(KERN_ERR "ccpu is slssp, can not disable\n");
               iounmap((void*)ulBaseAddr);
                  ulBaseAddr = NULL;
                       break;
            }

        case DBG_WDG_LOCAL_MCU:
            /*mcu local wdg*/
            ulBaseAddr = ioremap(SOC_BALONG_WDG_LMCU_ADDR, SZ_4K);
            /*判断WDT是否被锁*/
            ulRegValue = wdg_dbg_readreg(WDT_WDG_LOCK_ADDR(ulBaseAddr));
            if(0 != ulRegValue)
            {
                /*解锁*/
                writel(MCU_WDT_UNLOCK_VALUE, WDT_WDG_LOCK_ADDR(ulBaseAddr));
            }

            ulRegValue = M3_WDT_NOT_GLOBAL_FREQUENCY * (ival_msec/1000) - 1;

            /*Clear the WDT INT register*/
            writel(0, WDT_WDG_INTCLR_ADDR(ulBaseAddr));

            /*Set Counter Start Num*/
            writel(ulRegValue, WDT_WDG_LOAD_ADDR(ulBaseAddr));
            /*锁*/
            writel(0, WDT_WDG_LOCK_ADDR(ulBaseAddr));

            iounmap((void*)ulBaseAddr);
            ulBaseAddr = NULL;
            break;

        case DBG_WDG_GLOBAL_MCU:
            /*mcu global wdg*/
            ulBaseAddr = ioremap(SOC_BALONG_WDG_GMCU_ADDR, SZ_4K);
            /*判断WDT是否被锁*/
            ulRegValue = wdg_dbg_readreg(WDT_WDG_LOCK_ADDR(ulBaseAddr));
            if(0 != ulRegValue)
            {
                /*解锁*/
                writel(MCU_WDT_UNLOCK_VALUE, WDT_WDG_LOCK_ADDR(ulBaseAddr));
            }

            ulRegValue = M3_WDT_NOT_GLOBAL_FREQUENCY * (ival_msec/1000) - 1;

            /*Clear the WDT INT register*/
            writel(0, WDT_WDG_INTCLR_ADDR(ulBaseAddr));

            /*Set Counter Start Num*/
            writel(ulRegValue, WDT_WDG_LOAD_ADDR(ulBaseAddr));

            /*锁*/
            writel(0, WDT_WDG_LOCK_ADDR(ulBaseAddr));
            iounmap((void*)ulBaseAddr);
            ulBaseAddr = NULL;
            break;

        default:
            printk("wrong core to set wdg!!\n");
            iret = -1;
            break;
        }
    }
    return iret;
}

/*****************************************************************************
 Function: show_memory
 Description: show the data of memory given addr and size
 Input: void  *addr_in, addr of memory
            int nbytes,size of memory
            int ibool, ibool>0,vir addr;0>=ibool, phy addr
 Out: 无
 Return value: void
 History:
 1.     20140121    Creat
*****************************************************************************/
#ifdef CONFIG_ARCH_HI6XXX
void show_memory(unsigned long addr_in, unsigned int nbytes, int ibool)
{
    int i, j;
    u32 *p;
    unsigned long virtual_addr;
    unsigned int itotalbytes = nbytes;
    const unsigned int line_size = 32;
    unsigned int nlines;
    unsigned int size;

    printk("\naddress 0x%lx:\n", addr_in);

    /*vir addr*/
    if (ibool) {
        virtual_addr = addr_in;

        /* round address down to a 32/64 bit boundary */
        p = (u32 *)(virtual_addr & ~(sizeof(unsigned long) - 1));
        /* always dump a multiple of 32 bytes */
        itotalbytes += (virtual_addr & (sizeof(unsigned long) - 1));
        nlines = (itotalbytes + (line_size-1)) / line_size;

        for (i=0; i<nlines; i++) {
            /*
            * just display low 16 bits of address to keep
            * each line of the dump < 80 characters
            */
            printk("%04lx ", (unsigned long)p & 0xffff);
            for (j=0; j < line_size/sizeof(u32); j++) {
                u32 data;
                if (probe_kernel_address(p++, data)) {
                    printk(" ********");
                } else {
                    printk(" %08x", data);
                }
            }
            printk("\n");
        }
    }else { /*phy addr*/
        /* always dump a multiple of 32 bytes */
        nlines = (itotalbytes + (line_size-1)) / line_size;
        size = nlines*line_size / sizeof(u32);

        reg_dbg_dump(addr_in, size, sizeof(u32));
    }
}
#endif
/*****************************************************************************
 Function: test_get_slice
 Description: only for testting, to check whether the slices are same.
 Input:
 Out: 无
 Return value: void
 History:
 1.     20140121    Creat
*****************************************************************************/
#ifdef CONFIG_ARCH_HI6XXX
void test_get_slice(void)
{
    unsigned int    utime1 = 0;
    unsigned int    u64time2[2] = {0};
    unsigned int    uresult = 0;

    utime1 = omTimerGet();

    printk("\nom slice is %d, bbp slice is %d %d, result = %d\n", utime1, u64time2[0],u64time2[1],uresult);

}
#endif
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
