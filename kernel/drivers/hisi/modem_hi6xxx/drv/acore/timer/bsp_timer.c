/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : BSP_TIMER.c
  版 本 号   : 初稿
  作    者   : 蔡喜 220237
  生成日期   : 2013年3月30日
  最近修改   :
  功能描述   : Timer逻辑层
  函数列表   :
              BSP_TIMER_Init
              BSP_TIMER_IntHandle
              BSP_TIMER_UsrClkConnect
              BSP_TIMER_UsrClkDisable
              BSP_TIMER_UsrClkEnableMs
              BSP_TIMER_UsrClkEnableUs
              BSP_TIMER_Show
  修改历史   :
  1.日    期   : 2012年3月30日
    作    者   : 蔡喜 220237
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
  *****************************************************************************/
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */
#include <linux/hisi/hi6xxx-platform.h>
#include "soc_timer_interface.h"
#include "soc_peri_sctrl_interface.h"
#include "BSP.h"
#include "bsp_timer_phy.h"
#include "bsp_timer.h"
#include "soc_ao_sctrl_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


extern unsigned int* pulTimerBase[TIMER_TOTAL_NUM + CCORE_TIMER_EXP];
/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/*保存各个timer的信息*/
TIMER_PARA g_timer_para[TIMER_TOTAL_NUM + CCORE_TIMER_EXP] = {{0}};

 /*对timer控制寄存器的使能位读写互斥保护*/
spinlock_t timer_en_lock;
/*****************************************************************************
  3 函数实现
*****************************************************************************/


signed int BSP_TIMER_InitTimerBase(void)
{
    unsigned int* timer_base = NULL;

    /* 映射timer2,3 */
    timer_base = (unsigned int*)ioremap(SOC_Timer1_BASE_ADDR, TIMER_REG_SIZE);
    if (!timer_base)
    {
        return BSP_ERROR;
    }
    pulTimerBase[2] = timer_base;
    pulTimerBase[2+1] = (unsigned int*)((unsigned char*)timer_base + 0x20);

    /* 映射timer10,11 */
    timer_base = (unsigned int*)ioremap(SOC_Timer5_BASE_ADDR, TIMER_REG_SIZE);
    if (!timer_base)
    {
        return BSP_ERROR;
    }
    pulTimerBase[10] = timer_base;
    pulTimerBase[10+1] = (unsigned int*)((unsigned char*)timer_base + 0x20);

    /* 映射timer12,13 */
    timer_base = (unsigned int*)ioremap(SOC_Timer6_BASE_ADDR, TIMER_REG_SIZE);
    if (!timer_base)
    {
        return BSP_ERROR;
    }
    pulTimerBase[12] = timer_base;
    pulTimerBase[12+1] = (unsigned int*)((unsigned char*)timer_base + 0x20);

    /* 映射C核timer8_1，放下标TIMER_TOTAL_NUM的位置 */
    timer_base = (unsigned int*)ioremap(SOC_MTimer8_BASE_ADDR, TIMER_REG_SIZE);
    if (!timer_base)
    {
        return BSP_ERROR;
    }
    pulTimerBase[TIMER_TOTAL_NUM] = (unsigned int*)((unsigned char*)timer_base + 0x20);
    return BSP_OK;
}


void BSP_TIMER_SetCcoreTimerInfo(void)
{
    g_timer_para[TIMER_TOTAL_NUM].ticksPerSecond = TIMER_DEFAULT_TICKS;
    g_timer_para[TIMER_TOTAL_NUM].inputClock = TCXO_CLK_FREQ;
}


unsigned int* TIMER_BASE(unsigned int clk_logic_id)
{
    signed int clk_phy_id;
    clk_phy_id = timer_find_phy_id((unsigned char)clk_logic_id);
    if ((clk_phy_id < 0) ||(clk_phy_id >= TIMER_ID_MAX_PHY) )
    {
        printk(TIMER_PRINT_ERR "TIMER_BASE: clk_phy_id(%d) is invalid.\n", clk_phy_id);
        return NULL;
    }
    return pulTimerBase[clk_phy_id];
}


signed int BSP_TIMER_Init(void)
{
    unsigned char i;
    unsigned long base = HISI_VA_ADDRESS(REG_BASE_SC_ON);
    int ret = 0;
    if (BSP_ERROR == BSP_TIMER_InitTimerBase())
    {
        printk(TIMER_PRINT_ERR "BSP_TIMER_Init: timer reg ioremap error.\n");
        return BSP_ERROR;
    }

    spin_lock_init(&timer_en_lock);

    /* 开时钟门控DTimer1,DTimer6；写0不影响 */
    TIMER_REG_WRITE(SOC_AO_SCTRL_SC_PERIPH_CLKEN4_ADDR(base),
        (SET_BIT_WIHT_TRUE <<SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_16pclk_timer1_START) |
        (SET_BIT_WIHT_TRUE <<SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_21pclk_timer6_START));

    /* 将时钟配置写入系统控制器 */
    (void)timer_sys_ctrl_set();

    /* 配置各个Timer时钟源 */
    for (i = 0; i < TIMER_ID_MAX_PHY; i++)
    {
        g_timer_para[i].ticksPerSecond = TIMER_DEFAULT_TICKS;

        /* 得到每个Timer的输入时钟配置 */
        g_timer_para[i].inputClock = timer_input_clk_get(i);
    }

    /* 配置A核可以访问的C核Timer信息，用于换算剩余时间 */
    BSP_TIMER_SetCcoreTimerInfo();

    ret = request_irq((unsigned int)INUM_TO_IVEC(INT_IRQ_TIMER2), BSP_TIMER_IntHandle,
        IRQF_DISABLED | IRQF_TIMER, "TIMER_ACPU_OSA_ID_PHY", (void*)TIMER_ACPU_OSA_ID_PHY);
    if(ret)
    {
        printk(TIMER_PRINT_ERR "BSP_TIMER_Init: %d irq error.\n", TIMER_ACPU_OSA_ID_PHY);
    }

    ret = request_irq((unsigned int)INUM_TO_IVEC(INT_IRQ_TIMER3), BSP_TIMER_IntHandle,
        IRQF_DISABLED | IRQF_TIMER, "TIMER_ACPU_SOFTTIMER_ID_PHY", (void*)TIMER_ACPU_SOFTTIMER_ID_PHY);
    if(ret)
    {
         printk(TIMER_PRINT_ERR "BSP_TIMER_Init: %d irq error.\n", TIMER_ACPU_SOFTTIMER_ID_PHY);
    }

#if 0
    ret = request_irq((unsigned int)INUM_TO_IVEC(INT_IRQ_TIMER12), BSP_TIMER_IntHandle,
        IRQF_DISABLED | IRQF_TIMER, "TIMER_ACPU_IDLE_ID_PHY", (void*)TIMER_ACPU_IDLE_ID_PHY);
    if(ret)
    {
         printk(TIMER_PRINT_ERR "BSP_TIMER_Init: %d irq error.\n", TIMER_ACPU_IDLE_ID_PHY);
    }
#endif

    ret = request_irq((unsigned int)INUM_TO_IVEC(INT_IRQ_TIMER13), BSP_TIMER_IntHandle,
        IRQF_DISABLED | IRQF_TIMER, "TIMER_ACPU_OM_TCXO_ID_PHY", (void*)TIMER_ACPU_OM_TCXO_ID_PHY);
    if(ret)
    {
         printk(TIMER_PRINT_ERR "BSP_TIMER_Init: %d irq error.\n", TIMER_ACPU_OM_TCXO_ID_PHY);
    }

    return BSP_OK;
}
irqreturn_t BSP_TIMER_IntHandle(int irq, void* para)
{
    unsigned int readValue;
    unsigned char timerID;

    timerID = (unsigned char)(unsigned int)(para);
    if (timerID >= TIMER_ID_MAX_PHY)
    {
        return IRQ_NONE;
    }

    readValue = timer_int_status_get(timerID);

    /* 中断有效 */
    if (0x0 != readValue)
    {
        timer_int_clear(timerID);
        if (BSP_NULL != g_timer_para[timerID].routine)
        {
            /* 调用中断处理函数和参数 */
            (void)((*(g_timer_para[timerID].routine))(g_timer_para[timerID].arg));
        }
    }
    return IRQ_HANDLED;
}


signed int BSP_TIMER_UsrClkConnect(FUNCPTR_1 routine, signed int arg, unsigned int clk_phy_id)
{
    if ((clk_phy_id >= TIMER_ID_MAX_PHY) || (TIMER_ACPU_SYSTEM_ID_PHY == clk_phy_id))
    {
        return BSP_ERROR;
    }

    if (BSP_OK == timer_is_running(clk_phy_id))
    {
        printk(TIMER_PRINT_ERR "BSP_TIMER_UsrClkConnect: g_timer_para[%d].running!\n",clk_phy_id);
        return BSP_ERROR;
    }

    g_timer_para[clk_phy_id].routine = BSP_NULL;
    g_timer_para[clk_phy_id].arg = arg;
    g_timer_para[clk_phy_id].routine = routine;

    return BSP_OK;
}


signed int BSP_TIMER_UsrClkDisable(unsigned int clk_logic_id)
{
    signed int clk_phy_id;

    clk_phy_id = timer_find_phy_id((unsigned char)clk_logic_id);
    if ((clk_phy_id < 0) ||(clk_phy_id >= TIMER_ID_MAX_PHY) || (TIMER_ACPU_SYSTEM_ID_PHY == clk_phy_id))
    {
        printk(TIMER_PRINT_ERR "BSP_TIMER_UsrClkDisable: clk_phy_id(%d) is invalid.\n", clk_phy_id);
        return BSP_ERROR;
    }

    if (BSP_OK == timer_is_running(clk_phy_id))
    {
        timer_stop((unsigned char)clk_phy_id);
        g_timer_para[clk_phy_id].running = BSP_FALSE;
    }

    return BSP_OK;
}


signed int BSP_TIMER_UsrClkEnableMs(unsigned int clk_phy_id, unsigned int value, unsigned int mode)
{
    unsigned int count = 0;

    if (g_timer_para[clk_phy_id].running)
    {
        printk(TIMER_PRINT_ERR "BSP_TIMER_UsrClkEnableMs: g_timer_para[%d].running!\n", clk_phy_id);
        return BSP_ERROR;
    }

    /* 确定计数初值：   对于19.2MHz先除1000，以提高value的取值范围[1,223696]
                        对于32KHz先乘value，以保证寄存器的精度，其取值范围[1,131071]*/
    if (TCXO_CLK_FREQ == g_timer_para[clk_phy_id].inputClock)
    {
        count = g_timer_para[clk_phy_id].inputClock / Second_To_Millisecond * value;
    }
    else if (SLEEP_CLK_FREQ == g_timer_para[clk_phy_id].inputClock)
    {
        count = g_timer_para[clk_phy_id].inputClock * value / Second_To_Millisecond;

    }
    else
    {
        printk(TIMER_PRINT_ERR "BSP_TIMER_UsrClkEnableMs: ERROR FREQ!\n");
        return BSP_ERROR;
    }

    timer_stop((unsigned char)clk_phy_id);

    /* 设置寄存器timer_load的值,并启动定时器 */
    timer_load_set((unsigned char)clk_phy_id, count);
    timer_start((unsigned char)clk_phy_id, mode);

    /* 设置timer运行状态标志 */
    g_timer_para[clk_phy_id].running = BSP_TRUE;

    return BSP_OK;
}


signed int BSP_TIMER_UsrClkEnableUs(unsigned int clk_phy_id, unsigned int value, unsigned int mode)
{
     unsigned int count = 0;

    if (g_timer_para[clk_phy_id].running)
    {
        printk(TIMER_PRINT_ERR "BSP_TIMER_UsrClkEnableUs: g_timer_para[%d].running!\n", clk_phy_id);
        return BSP_ERROR;
    }

    /* 对19.2M的时钟，正常处理 */
    if (TCXO_CLK_FREQ == g_timer_para[clk_phy_id].inputClock)
    {
        count = value * (g_timer_para[clk_phy_id].inputClock / Second_To_Millisecond) / Second_To_Millisecond;
    }
    /* 对32K的时钟，特殊处理；最小刻度约31us */
    else if (SLEEP_CLK_FREQ == g_timer_para[clk_phy_id].inputClock)
    {
        if (value > TIMER_32K_US_BOUNDARY)
        {
            count = (value * g_timer_para[clk_phy_id].inputClock) / Second_To_Millisecond / Second_To_Millisecond;
        }
        else
        {
            /* 对于不大于31us的定时，直接往load寄存器写1 */
            count = 1;
        }
    }
    /* 其他频率的时钟报错 */
    else
    {
        printk(TIMER_PRINT_ERR "BSP_TIMER_UsrClkEnableUs: error! CLK_FREQ is error!\n");
        return BSP_ERROR;
    }

    timer_stop((unsigned char)clk_phy_id);

    /* 设置寄存器timer_load的值,并启动定时器 */
    timer_load_set((unsigned char)clk_phy_id, count);
    timer_start((unsigned char)clk_phy_id, mode);

    /* 设置timer运行状态标志 */
    g_timer_para[clk_phy_id].running = BSP_TRUE;

    return BSP_OK;
}


signed int BSP_TIMER_UsrClkEnableNone(unsigned int clk_phy_id, unsigned int value, unsigned int mode)
{
    if (g_timer_para[clk_phy_id].running)
    {
        printk(TIMER_PRINT_ERR "g_timer_para[%d].running!\n", clk_phy_id);
        return BSP_ERROR;
    }

    timer_stop((unsigned char)clk_phy_id);

    /* 设置寄存器timer_load的值,并启动定时器 */
    timer_load_set((unsigned char)clk_phy_id, value);
    timer_start((unsigned char)clk_phy_id, mode);

    /* 设置timer运行状态标志 */
    g_timer_para[clk_phy_id].running = BSP_TRUE;

    return BSP_OK;
}


signed int BSP_TIMER_GetRestTime(unsigned int clk_logic_id, unsigned int unitType, unsigned int * pRestTime)
{
    unsigned int RestCount = 0;
    signed int clk_phy_id = 0;

    clk_phy_id = timer_find_phy_id((unsigned char)clk_logic_id);

    /* 因查询C核Timer的value值，放宽参数限制*/
    if ((clk_phy_id < 0) || (clk_phy_id >= (TIMER_TOTAL_NUM + CCORE_TIMER_EXP)) || (TIMER_ACPU_SYSTEM_ID_PHY == clk_phy_id))
    {
        printk(TIMER_PRINT_ERR "BSP_TIMER_GetRestTime: clk_phy_id(%d) is invalid.\n", clk_phy_id);
        return BSP_ERROR;
    }

    RestCount = timer_value_get((unsigned char)clk_phy_id);

    if ((TIMER_UNIT_US == unitType) && (0 != g_timer_para[clk_phy_id].inputClock))
    {
        RestCount = (RestCount * Second_To_Millisecond) / (g_timer_para[clk_phy_id].inputClock / Second_To_Millisecond);
    }
    else if ((TIMER_UNIT_MS == unitType) && (0 != g_timer_para[clk_phy_id].inputClock))
    {
        RestCount = RestCount / (g_timer_para[clk_phy_id].inputClock / Second_To_Millisecond);
    }
    else if (TIMER_UNIT_NONE == unitType)
    {
        /* 不需要对RestCount进行处理 */
    }
    else
    {
        printk(TIMER_PRINT_ERR "BSP_TIMER_GetRestTime: Cannot get RestTime!");
        return BSP_ERROR;
    }

    *pRestTime = RestCount;

    return BSP_OK;
}


signed int BSP_TIMER_UsrClkStart
(
    unsigned int     clk_logic_id,
    FUNCPTR_1   routine,
    signed int     arg,
    unsigned int     timerValue,
    unsigned int     mode,
    unsigned int     unitType
)
{
    signed int ConnectResult = BSP_ERROR;
    signed int EnableResult = BSP_ERROR;
    signed int clk_phy_id;

    clk_phy_id = timer_find_phy_id((unsigned char)clk_logic_id);
    if ((clk_phy_id < 0) ||(clk_phy_id >= TIMER_ID_MAX_PHY) || (TIMER_ACPU_SYSTEM_ID_PHY == clk_phy_id))
    {
        printk(TIMER_PRINT_ERR "BSP_TIMER_UsrClkStart: error! clk_logic_id = %d, clk_phy_id = %d.\n", clk_logic_id, clk_phy_id);
        return BSP_ERROR;
    }

    if ((mode >= TIMER_COUNT_BUTT) || (unitType >= TIMER_UNIT_BUTT))
    {
        printk(TIMER_PRINT_ERR "BSP_TIMER_UsrClkStart: error! mode = %d, unitType = %d.\n", mode, unitType);
        return BSP_ERROR;
    }

    /* 挂接用户时钟中断处理函数 */
    ConnectResult = BSP_TIMER_UsrClkConnect(routine, arg, clk_phy_id);
    if (BSP_OK != ConnectResult)
    {
        printk(TIMER_PRINT_ERR "BSP_TIMER_UsrClkStart: Connect fail! clk_phy_id=%d\n", clk_phy_id);
        return BSP_ERROR;
    }

    /* 载入计数值并使能定时器 */
    if (TIMER_UNIT_MS == unitType)
    {
        EnableResult = BSP_TIMER_UsrClkEnableMs(clk_phy_id, timerValue, mode);
    }
    else if (TIMER_UNIT_US == unitType)
    {
        EnableResult = BSP_TIMER_UsrClkEnableUs(clk_phy_id, timerValue, mode);
    }
    else if (TIMER_UNIT_NONE == unitType)
    {
        EnableResult = BSP_TIMER_UsrClkEnableNone(clk_phy_id, timerValue, mode);
    }
    else
    {
        printk(TIMER_PRINT_ERR "BSP_TIMER_UsrClkStart: unitType is error!");
        return BSP_ERROR;
    }

    if (BSP_OK != EnableResult)
    {
        printk(TIMER_PRINT_ERR "BSP_TIMER_UsrClkStart: Enable fail!clk_phy_id=%d\n",clk_phy_id);
        return BSP_ERROR;
    }

    return BSP_OK;
}


signed int BSP_TIMER_Show(void)
{
#if 0
    signed int i = 0;
    signed int LoadValue = 0;
    signed int mode = 0;
    SOC_TIMER_TIMERN_CONTROL_UNION *pTIMER_CNTL_REG = NULL;

    BSP_CHAR sTimerID[TIMER_ACPU_ID_MAX][24] = {
        "TIMER_OM_TCXO_TIMER",
        "TIMER_ACPU_SYSTEM_ID",
        "TIMER_ACPU_SOFTTIMER_ID",
        "TIMER_ACPU_OSA_ID",
        "TIMER_ACPU_CODEC_ID",
        "TIMER_ACPU_APP_ID",
    };

    printk("===============================================================================\n");
    printk("TimerID IDName                  InputClock IsRunning LoadRegValue IsPeriodicMode\n");
    for(i = TIMER_OM_TCXO_TIMER_PHY; i < TIMER_ID_MAX; i++)
    {
        LoadValue = (signed int)timer_load_get((unsigned char)i);
        pTIMER_CNTL_REG = (SOC_TIMER_TIMERN_CONTROL_UNION *)TIMER_CNTL_REG(pulTimerBase[i]);

        /* oneshot: 0表示周期模式 1表示单次模式 */
        mode = (signed int)(!(pTIMER_CNTL_REG->reg.oneshot));
        printk("%-7d %-23s %-10d %-9d 0x%-10x %-14d\n", i, sTimerID[i - TIMER_ACPU_ID_BEGIN] , g_timer_para[i].inputClock, g_timer_para[i].running, LoadValue, mode);
    }
    printk("===============================================================================\n");
#endif
    return BSP_OK;
}
unsigned int BSP_TIMER_Query(unsigned char clk_phy_id)
{
    printk(TIMER_PRINT_ERR "pulTimerBase[%d].load = 0x%X \n", clk_phy_id, *(pulTimerBase[clk_phy_id]));
    printk(TIMER_PRINT_ERR "pulTimerBase[%d].value = 0x%X \n", clk_phy_id, *(pulTimerBase[clk_phy_id]+1));
    printk(TIMER_PRINT_ERR "pulTimerBase[%d].ctrl = 0x%X \n", clk_phy_id, *(pulTimerBase[clk_phy_id]+2));
    printk(TIMER_PRINT_ERR "pulTimerBase[%d].INTCLR = 0x%X \n", clk_phy_id, *(pulTimerBase[clk_phy_id]+3));
    printk(TIMER_PRINT_ERR "pulTimerBase[%d].RIS = 0x%X \n", clk_phy_id, *(pulTimerBase[clk_phy_id]+4));
    printk(TIMER_PRINT_ERR "pulTimerBase[%d].MIS = 0x%X \n", clk_phy_id, *(pulTimerBase[clk_phy_id]+5));
    return 0;
}


int timer_base_show(void)
{
    int i=0;

     for(i=0;i<(TIMER_TOTAL_NUM + CCORE_TIMER_EXP);i++)
	 {
        printk(TIMER_PRINT_ERR "pulTimerBase[%d]=0x%p\n",i,pulTimerBase[i]);
     }

return 0;
}


int bsp_time_info_dump(void)
{
    int i=0;
    TIMER_PARA *cur_item=NULL;

     for(i=0;i<TIMER_TOTAL_NUM;i++){
        cur_item=&(g_timer_para[i]);
        printk(TIMER_PRINT_ERR "cur_item[%d].routine=0x%lx\n",i, (unsigned long)cur_item->routine);
        printk(TIMER_PRINT_ERR "cur_item[%d].arg=%d\n",i,cur_item->arg);
        printk(TIMER_PRINT_ERR "cur_item[%d].running=%d\n",i,cur_item->running);
        printk(TIMER_PRINT_ERR "cur_item[%d].tickPerSecond=%d\n",i,cur_item->ticksPerSecond);
        printk(TIMER_PRINT_ERR "cur_item[%d].inputClock=%d\n",i,cur_item->inputClock);
     }

return 0;
}

/*****************************************************************************
 函 数 名  : bsp_get_accuracy_timestamp
 功能描述  : 用来获取高精度的定时器的值，给协议栈提供的接口。
 输入参数  : 无
 输出参数  : unsigned int  *pulHigh32bitValue 存储高32bit值
             unsigned int  *pulLow32bitValue  存储低32bit值
 返回值    ：0 成功
            -1 失败
*****************************************************************************/
int bsp_get_accuracy_timestamp(unsigned int  *pulHigh32bitValue,unsigned int  *pulLow32bitValue)
{
    /*lint -save -e958*/
    unsigned int timervalue[4];
    /*lint -save -restore*/
    if(NULL == pulLow32bitValue)
    {
        return (-1);
    }

    timervalue[0] = (*(volatile unsigned int *)(SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT0_ADDR(HISI_VA_ADDRESS(SOC_AO_SCTRL_BASE_ADDR))));
    timervalue[1] = (*(volatile unsigned int *)(SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT1_ADDR(HISI_VA_ADDRESS(SOC_AO_SCTRL_BASE_ADDR))));
    timervalue[2] = (*(volatile unsigned int *)(SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT0_ADDR(HISI_VA_ADDRESS(SOC_AO_SCTRL_BASE_ADDR))));
    timervalue[3] = (*(volatile unsigned int *)(SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT1_ADDR(HISI_VA_ADDRESS(SOC_AO_SCTRL_BASE_ADDR))));

    *pulLow32bitValue = timervalue[0];

    if(NULL != pulHigh32bitValue)
    {
        if(timervalue[2] < timervalue[0])
        {
            (*pulHigh32bitValue) = timervalue[3] - 1;
        }
        else
        {
            (*pulHigh32bitValue) = timervalue[1];
        }
    }

    return 0;
}

EXPORT_SYMBOL(timer_base_show);
EXPORT_SYMBOL(BSP_TIMER_UsrClkStart);
EXPORT_SYMBOL(BSP_TIMER_UsrClkDisable);
EXPORT_SYMBOL(BSP_TIMER_GetRestTime);
EXPORT_SYMBOL(BSP_TIMER_Show);
EXPORT_SYMBOL(BSP_TIMER_Query);
EXPORT_SYMBOL(bsp_time_info_dump);
EXPORT_SYMBOL(bsp_get_accuracy_timestamp);




/* just for test,begin */
#define TIMER_DEBUG
#ifdef TIMER_DEBUG
unsigned int acore_timer_milliSecond = 5000;  /* 5s */
unsigned int acore_timer_mode = 0;            /* 0:单次，1:周期 */
unsigned int acore_timer_unitType = 2;        /* unitType:0表示单位ms，1表示单位us，2表示直接写load寄存器启动timer */
signed int IntCount[TIMER_ID_MAX] = {0};   /* 为每一个对外timer分配空间 */
unsigned int acore_timer_restTime = 0;
void tsTimerSetPara(unsigned int milliSecond, unsigned int mode, unsigned int unitType)
{
    acore_timer_milliSecond = milliSecond;
    acore_timer_mode        = mode;
    acore_timer_unitType    = unitType;
    printk(TIMER_PRINT_ERR "acore_timer_milliSecond = %d.\n", acore_timer_milliSecond);
    printk(TIMER_PRINT_ERR "acore_timer_mode        = %d.\n", acore_timer_mode);
    printk(TIMER_PRINT_ERR "acore_timer_unitType    = %d.\n", acore_timer_unitType);

	return;
}
void Init_IntCount(void)
{
    int i = 0;
    for(i = 0; i < TIMER_ID_MAX; i++)
    {
        IntCount[i] = 0;
    }
	return;
}

/* 调用者保证数组下标不越界 */
signed int tsTimerComplete(signed int arg)
{
    IntCount[arg]++;
    printk(TIMER_PRINT_INFO "tsTimerComplete!usrClkId=%d,IntCount[%d]=%d.\n", (signed int)arg, arg,IntCount[arg]);
    return 0;
}

/* 调用者保证该对外timer在a核可以访问；
   目的: shell口不好把函数tsTimerComplete坐参数，此函数只为测试细节时使用，通常用不到 */
signed int tsTimerConnect(unsigned int clk_logic_id)
{
    unsigned int clk_phy_id;
    clk_phy_id = timer_find_phy_id((unsigned char)clk_logic_id);
    return BSP_TIMER_UsrClkConnect((FUNCPTR_1)tsTimerComplete, (signed int)clk_phy_id, clk_phy_id);
}

/*****************************************************************************
  下面是对外接口测试
*****************************************************************************/

int tsTimerGetRestTime(unsigned int clk_logic_id)
{
    int result = 0;
    result = BSP_TIMER_GetRestTime(clk_logic_id, acore_timer_unitType, &acore_timer_restTime);
    printk(TIMER_PRINT_ERR "clk_logic_id=%d, RestTime=0x%x.\n", clk_logic_id, acore_timer_restTime);
    return result;
}

/* 测试1个timer， 5s, 周期, 测准确度 测start接口 */
int tsTimerClkGating(unsigned int test_id)
{
    unsigned int  clk_logic_id_1 = 6;
    unsigned int  clk_logic_id_2 = 13;
    unsigned int  l_timerValue=5000;
    unsigned int  l_mode=1;   /*周期*/
    unsigned int   l_unitType=0;   /*单位ms*/

    unsigned long base = HISI_VA_ADDRESS(REG_BASE_SC_ON);
    unsigned int result=0;

    #if 1
    if (2 == test_id)
    {
        BSP_TIMER_UsrClkStart(clk_logic_id_1, tsTimerComplete, clk_logic_id_1,  l_timerValue, l_mode, l_unitType);
        BSP_TIMER_UsrClkStart(clk_logic_id_2, tsTimerComplete, clk_logic_id_2,  l_timerValue, l_mode, l_unitType);
    }


    if (1 == test_id)
    {
        /*stop clk_logic_id_1*/
        BSP_TIMER_UsrClkDisable(clk_logic_id_1);
        TIMER_REG_READ(SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_ADDR(base), result);
        printk(TIMER_PRINT_ERR "SC_PERIPH_CLKSTAT4: 0x%x\n",result);
    }

    if (0 == test_id)
    {
        /*stop clk_logic_id_2*/
        BSP_TIMER_UsrClkDisable(clk_logic_id_2);
        TIMER_REG_READ(SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_ADDR(base), result);
        printk(TIMER_PRINT_ERR "SC_PERIPH_CLKSTAT4: 0x%x\n",result);
    }
    #endif
    return BSP_OK;
}

/* 测试1个timer， 5s, 周期, 测准确度 测start接口 */
int tsTimerStartOne(unsigned int clk_logic_id)
{
    return BSP_TIMER_UsrClkStart(clk_logic_id, tsTimerComplete, clk_logic_id, acore_timer_milliSecond, acore_timer_mode, acore_timer_unitType);
}

/* 测试all timer， 1s, 单次, 微秒模式 测功能 验证中断、基地址 */
int tsTimerStartAll(void)
{
/*    BSP_TIMER_UsrClkStart(22, tsTimerComplete, 22, 1000, 0, 0); */
    BSP_TIMER_UsrClkStart(23, tsTimerComplete, 23, 1000, 0, 0);
    BSP_TIMER_UsrClkStart(6,  tsTimerComplete, 6,  1000, 0, 0);
    BSP_TIMER_UsrClkStart(13, tsTimerComplete, 13, 1000, 0, 0);
    return 0;
}

/* 测功能 */
int tsTimerStopAll(void)
{


/*    BSP_TIMER_UsrClkDisable(22); */
    BSP_TIMER_UsrClkDisable(23);
    BSP_TIMER_UsrClkDisable(6);
    BSP_TIMER_UsrClkDisable(13);
    return 0;
}
EXPORT_SYMBOL(tsTimerSetPara);
EXPORT_SYMBOL(tsTimerStartOne);
EXPORT_SYMBOL(tsTimerStartAll);
EXPORT_SYMBOL(tsTimerStopAll);
#endif
/* just for test,end */

module_init(BSP_TIMER_Init);
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

