
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <asm/io.h>
/*#include <asm/system.h>*/
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */
#include "soc_baseaddr_interface.h"
#include "soc_peri_sctrl_interface.h"
#include "bsp_mem_drv.h"
#include "ipf_driver.h"
/*#include "drv_timer.h"*/

/*****************************************************************************
  函数申明
*****************************************************************************/
irqreturn_t  IPF_IntHandler (int irq, void* dev);
void IPF_Int_Connect(void);
int IPF_FilterList_Init(void);
int IPF_WaitResume(void);
static int IPF_Probe(struct platform_device *pdev);
static int IPF_Remove(struct platform_device *pdev);
static int IPF_Suspend(struct platform_device *pdev,pm_message_t state);
static int IPF_Resume(struct platform_device *pdev);

/*****************************************************************************
  宏定义
*****************************************************************************/
#define IPF_INIT_CHECK()    \
do {\
    if(!g_stIpfCtx.isInit) \
    { \
        IPF_PRINT_ERROR("ipf is not init\n"); \
        return BSP_ERR_IPF_NOT_INIT; \
    } \
}while(0)

/*****************************************************************************
  变量定义
*****************************************************************************/
IPF_UL_S g_stIpfUl = {0};
IPF_DL_S g_stIpfDl = {0};
IPF_CONTEXT_S g_stIpfCtx = {0};

unsigned long ifp_ulbd_mem_addr = 0;
unsigned long ipf_regs_base_addr = 0;

#ifdef __BSP_IPF_DEBUG__
IPF_DEBUG_INFO_S* g_stIPFDebugInfo = (IPF_DEBUG_INFO_S*)NULL;
unsigned int g_32IpfDebugReg[IPF_REG_NUM] = {0};
#endif
static IPF_FORRESET_CONTROL_E g_forCcore_reset_Up_flag = 0;
static struct platform_driver ipf_pfdrv = {
	.probe		= IPF_Probe,
	.remove		= IPF_Remove,
    .suspend    = IPF_Suspend,
    .resume     = IPF_Resume,
	.driver		= {
		.name	= "balong_ipf",
		.owner	= THIS_MODULE,
		},
};
static struct platform_device ipf_pfdev = {
	.name		  = "balong_ipf",
	.id                	  =0,
};
static unsigned long ipf_peri_ctrl_base = 0;

irqreturn_t  IPF_IntHandler (int irq, void* dev)
{
    unsigned int u32IpfAdqInt = 0;
    SOC_IPF_INT1_UNION unIpfInt;

    unIpfInt.value = 0;
    /* 读取中断状态 */
    IPF_REG_READ(SOC_IPF_INT1_ADDR(IPF_REGBASE_ADR), unIpfInt.value);
    /* 下行结果上报和结果超时上报 */
    if((unIpfInt.reg.dl_rpt_int1) || (unIpfInt.reg.dl_timeout_int1))
    {
        /* 写1清除下行结果和结果超时中断 */
        IPF_REG_WRITE(SOC_IPF_INT_STATE_ADDR(IPF_REGBASE_ADR), (IPF_DL_RPT_INT | IPF_DL_TIMEOUT_INT));
    #ifdef __BSP_IPF_DEBUG__
        if(unIpfInt.reg.dl_rpt_int1)
        {
            g_stIPFDebugInfo->u32DlResultCnt++;
        }
        if(unIpfInt.reg.dl_timeout_int1)
        {
            g_stIPFDebugInfo->u32DlResultTimeoutCnt++;
        }
    #endif
        /* 唤醒ps下行任务 */
        if(g_stIpfDl.pFnDlIntCb != NULL)
        {
            (void)g_stIpfDl.pFnDlIntCb();
        }
        else
        {
            IPF_PRINT_ERROR("dl int callback is null\n");
        }
    }

    /* 下行ADQ0、ADQ1空指示 */
    if((unIpfInt.reg.dl_adq0_epty_int1) || (unIpfInt.reg.dl_adq1_epty_int1))
    {
        /* 写1清除下行ADQ、ADQ1空中断 */
        IPF_REG_WRITE(SOC_IPF_INT_STATE_ADDR(IPF_REGBASE_ADR), (IPF_DL_ADQ0_EMPTY_INT | IPF_DL_ADQ1_EMPTY_INT));
        if(unIpfInt.reg.dl_adq0_epty_int1)
        {
        #ifdef __BSP_IPF_DEBUG__
            g_stIPFDebugInfo->u32DlAdq0Empty++;
        #endif
            u32IpfAdqInt += IPF_EMPTY_ADQ0;
        }
        if(unIpfInt.reg.dl_adq1_epty_int1)
        {
        #ifdef __BSP_IPF_DEBUG__
            g_stIPFDebugInfo->u32DlAdq1Empty++;
        #endif
            u32IpfAdqInt += IPF_EMPTY_ADQ1;
        }
        if(g_stIpfDl.pAdqEmptyDlCb != NULL)
        {
            (void)g_stIpfDl.pAdqEmptyDlCb(u32IpfAdqInt);
        }
        else
        {
            IPF_PRINT_ERROR("dl adq callback is null\n");
        }
    }
    return IRQ_HANDLED;
}
/*****************************************************************************
* 函 数 名  : IPF_Int_Connect
*
* 功能描述  : 挂IPF中断处理函数(两核都提供)
*
* 输入参数  : void
* 输出参数  : 无
* 返 回 值  : 无
*
* 修改记录  :2011年12月2日   鲁婷  创建
*****************************************************************************/
void IPF_Int_Connect(void)
{
    if(request_irq(INT_LVL_IPF, (irq_handler_t)IPF_IntHandler, 0, "IPF_APP_IRQ", NULL) != 0)
    {
        IPF_PRINT_ERROR("irequest_irq error\n");
    }
}


int IPF_WaitResume(void)
{
    unsigned int u32Times = 0;
    IPF_RESTORE_STATE_E IPF_Restore_flag = (IPF_RESTORE_STATE_E)0;

    unsigned long ipf_restore_flag_addr = HISI_VA_ADDRESS(MEMORY_RAM_CORESHARE_IPF_FLAG_ADDR);

    IPF_Restore_flag= (IPF_RESTORE_STATE_E)(*(volatile unsigned int*)ipf_restore_flag_addr);

    /* 获取IPF状态标识 */
    if(IPF_STATE_RESTORED == IPF_Restore_flag)
    {
        return IPF_SUCCESS;
    }

    /* 等待IPF_Restore_flag等于IPF_STATE_RESTORED，加入2ms超时机制 */
    do{
        IPF_Restore_flag = (IPF_RESTORE_STATE_E)(*(volatile unsigned int*)ipf_restore_flag_addr);
        udelay(10);
        u32Times++;
    }while((IPF_STATE_RESTORED != IPF_Restore_flag) && (u32Times < IPF_RESUME_COUNT));

    if(IPF_RESUME_COUNT == u32Times)
    {
        return IPF_ERROR;
    }

    return IPF_SUCCESS;
}


static int IPF_ObligateAdqRpt(IPF_CHANNEL_TYPE_E eChId, IPF_AD_TYPE_E eType, unsigned int *pu32Rpt)
{
    unsigned int u32Index = 0;
    unsigned int u32RptValue = 0;
    IPF_ADQ_INFO_S *pIpfAdqReg = NULL;

    /* ADQ读指针和状态寄存器表 */
    IPF_ADQ_INFO_S stIpfAdqReg[4] = {{0}};
    pIpfAdqReg = &stIpfAdqReg[0];
    pIpfAdqReg->pu32RptReg = (unsigned int *)(SOC_IPF_CH0_ADQ0_RPTR_ADDR(IPF_REGBASE_ADR));
    pIpfAdqReg = &stIpfAdqReg[1];
    pIpfAdqReg->pu32RptReg = (unsigned int *)(SOC_IPF_CH0_ADQ1_RPTR_ADDR(IPF_REGBASE_ADR));
    pIpfAdqReg = &stIpfAdqReg[2];
    pIpfAdqReg->pu32RptReg = (unsigned int *)(SOC_IPF_CH1_ADQ0_RPTR_ADDR(IPF_REGBASE_ADR));
    pIpfAdqReg = &stIpfAdqReg[3];
    pIpfAdqReg->pu32RptReg = (unsigned int *)(SOC_IPF_CH1_ADQ1_RPTR_ADDR(IPF_REGBASE_ADR));

    /*  入参检查 */
    if((IPF_CHANNEL_MAX <= eChId) || (IPF_AD_MAX <= eType) || (NULL == pu32Rpt))
    {
        IPF_PRINT_ERROR("parameter is error\n");
        return BSP_ERR_IPF_INVALID_PARA;
    }

    /* 根据通道ID和AD类型获取操作寄存器对象index值 */
    switch (eChId) {
        case IPF_CHANNEL_UP:
            u32Index = (IPF_AD_0 == eType) ? 0 : 1;
            break;
        case IPF_CHANNEL_DOWN:
            u32Index = (IPF_AD_0 == eType) ? 2 : 3;
            break;
        default:
            IPF_PRINT_ERROR("parameter is error\n");
            u32Index = 0;
            break;
    }

    /* 读取ADQ读指针寄存器 */
    IPF_REG_READ(stIpfAdqReg[u32Index].pu32RptReg, u32RptValue);

    /* 保存ADQ读指针 */
    *pu32Rpt = u32RptValue;
    return IPF_SUCCESS;
}
static void IPF_Assert(unsigned int u32ErrNo)
{
    /* 记录IPF所有寄存器的内容 */
     hisi_io_memcpy((void*)g_32IpfDebugReg, (void*)IPF_REGBASE_ADR, sizeof(g_32IpfDebugReg));

    /* 发起复位 */
    //BSP_MNTN_SystemError(BSP_MODU_IPF, u32ErrNo, 0, 0, 0);
	while(1)
	{
        __asm__ __volatile__("nop");
	}

    return;
}
/*****************************************************************************
* 函 数 名  : BSP_IPF_GetControlFLagForCcoreReset
*
* 功能描述  : 获得ccore复位时，设置的IPF标志，用于控制IPF上行
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 0，允许；1，不允许

*
* 修改记录  :2013年4月19日   卢彦胜 创建
*   1.修改日期 :
*     修改作者 :
*     修改记录 :
*****************************************************************************/
static IPF_FORRESET_CONTROL_E IPF_GetControlFLagForCcoreReset(void)
{
    return g_forCcore_reset_Up_flag;
}

/*****************************************************************************
* 函 数 名  : BSP_IPF_SetControlFLagForCcoreReset
*
* 功能描述  : ccore复位时，设置IPF标志，用于控制IPF上行
*
* 输入参数  : IPF_FORRESET_CONTROL_E flagvalue,0:允许；1:不允许上行
* 输出参数  : 无
* 返 回 值  : IPF_SUCCESS    初始化成功
*             IPF_ERROR      初始化失败
*
* 修改记录  :2013年4月19日   卢彦胜 创建
*   1.修改日期 :2013年9月12
*     修改作者 :周珊
*     修改记录 :添加入参检查、forbidden之后等待idle
*****************************************************************************/
void BSP_IPF_SetControlFLagForCcoreReset(IPF_FORRESET_CONTROL_E flagvalue)
{
    unsigned int u32UlState = 0;
    unsigned int u32DlState = 0;
    unsigned int u32Cnt = 0;

    /* 入参检查 */
    if(IPF_FORRESET_CONTROL_MAX <= flagvalue)
    {
        IPF_PRINT_ERROR("parameter is error\n");
    }

    /* 设置当前IPF状态 */
    g_forCcore_reset_Up_flag = flagvalue;

    if(IPF_FORRESET_CONTROL_FORBID == flagvalue)
    {
        /* 等待IPF进入idle态(不释放CPU但加入等待超时),如果无法进入idle，则等待时间后确保IPF不再处理数据为止 */
        do{
            IPF_REG_READ(SOC_IPF_CH0_STATE_ADDR(IPF_REGBASE_ADR), u32UlState);
            IPF_REG_READ(SOC_IPF_CH1_STATE_ADDR(IPF_REGBASE_ADR), u32DlState);
            msleep(10);
            u32Cnt++;
        }while(((u32UlState != IPF_CH_IDLE) || (u32DlState != IPF_CH_IDLE)) && (u32Cnt <= IPF_IDLE_TIMEOUT_NUM));
        if(u32UlState != IPF_CH_IDLE)
        {
            IPF_PRINT_ERROR("ul is not idle\n");
        }
        if(u32DlState != IPF_CH_IDLE)
        {
            IPF_PRINT_ERROR("ul is not idle\n");
        }
    }
}
EXPORT_SYMBOL(BSP_IPF_SetControlFLagForCcoreReset);
/*****************************************************************************
* 函 数 名  : BSP_IPF_ConfigTimeout
*
* 功能描述  : 调试使用，配置超时时间接口
*
* 输入参数  : unsigned int u32Timeout 配置的超时时间
* 输出参数  : 无
* 返 回 值  : IPF_SUCCESS                   成功
*             BSP_ERR_IPF_INVALID_PARA      参数无效
*
* 说明      : 1代表256个时钟周期
*
* 修改记录   : 2011年11月30日   鲁婷  创建
*****************************************************************************/
int BSP_IPF_ConfigTimeout(unsigned int u32Timeout)
{
    SOC_IPF_TIME_OUT_UNION unTimeout;

    unTimeout.value = 0;
    if((u32Timeout == 0) || (u32Timeout > 0xFFFF))
    {
        IPF_PRINT_ERROR("parameter is error\n");
        return BSP_ERR_IPF_INVALID_PARA;
    }
    unTimeout.reg.time_out_cfg = u32Timeout;
    unTimeout.reg.time_out_valid = 1;

    IPF_REG_WRITE(SOC_IPF_TIME_OUT_ADDR(IPF_REGBASE_ADR), unTimeout.value);

    return IPF_SUCCESS;
}
EXPORT_SYMBOL(BSP_IPF_ConfigTimeout);


int BSP_IPF_ConfigUpFilter(unsigned int u32Num, IPF_CONFIG_ULPARAM_S* pstUlPara)
{
    unsigned int i = 0;
    unsigned int u32BdqWptr = 0;
    IPF_CONFIG_ULPARAM_S* pstUlParam = pstUlPara;
#ifdef __BSP_IPF_DEBUG__
    SOC_IPF_CH0_ADQ_CTRL_UNION unCh0AdqCtrl;
#endif
    /*Modify_for_c_reset, l00212112,20130511, starts*/
    /*如果C核复位，则返回*/
    if (IPF_FORRESET_CONTROL_FORBID == IPF_GetControlFLagForCcoreReset())
    {
        IPF_PRINT_ERROR("ccore is resetting\n");
        return BSP_ERR_IPF_CCORE_RESETTING;
    }
    /*Modify_for_c_reset, l00212112,20130511, ends*/
    /* 参数检查 */
    if((NULL == pstUlPara))
    {
        IPF_PRINT_ERROR("point is null\n");
        return BSP_ERR_IPF_INVALID_PARA;
    }

    /* 检查模块是否初始化 */
    IPF_INIT_CHECK();

    if(u32Num > *(g_stIpfUl.pu32IdleBd))
    {
    #ifdef __BSP_IPF_DEBUG__
        g_stIPFDebugInfo->u32UlBdNotEnough++;
    #endif
        return IPF_ERROR;
    }

#ifdef __BSP_IPF_DEBUG__
    IPF_REG_READ(SOC_IPF_CH0_ADQ_CTRL_ADDR(IPF_REGBASE_ADR), unCh0AdqCtrl.value);
    if(unCh0AdqCtrl.reg.ul_adq_en != IPF_BOTH_ADQ_EN)
    {
        IPF_PRINT_ERROR("adq is not supported\n");
        return IPF_ERROR;
    }

    g_stIPFDebugInfo->u32UlBdNum += u32Num;
#endif

    /* 确保IPF已恢复 */
    if(IPF_ERROR == IPF_WaitResume())
    {
        IPF_PRINT_ERROR("wait for resume timeout\n");
        g_stIPFDebugInfo->u32UlResumeTimeoutCnt++;
        return  BSP_ERR_IPF_RESUME_TIMEOUT;
    }

    /* 读出BD写指针,将u32BdqWptr作为临时写指针使用 */
    IPF_REG_READ(SOC_IPF_CH0_BDQ_WPTR_ADDR(IPF_REGBASE_ADR), u32BdqWptr);

    for(i = 0; i < u32Num; i++)
    {
        if(0 == pstUlParam[i].u32Data)
        {
            IPF_PRINT_ERROR("ulbd inptr is null\n");
            return BSP_ERR_IPF_INVALID_PARA;
        }
        if(0 == pstUlParam[i].u16Len)
        {
            IPF_PRINT_ERROR("ulbd len is zero\n");
            g_stIPFDebugInfo->u32UlBdSizeErr = 1;
            return BSP_ERR_IPF_INVALID_PARA;
        }

        g_stIpfUl.pstIpfBDQ[u32BdqWptr].u16Attribute = pstUlParam[i].u16Attribute;
        g_stIpfUl.pstIpfBDQ[u32BdqWptr].u32InPtr = pstUlParam[i].u32Data;
        g_stIpfUl.pstIpfBDQ[u32BdqWptr].u16PktLen = pstUlParam[i].u16Len;
        g_stIpfUl.pstIpfBDQ[u32BdqWptr].u16UsrField1 = pstUlParam[i].u16UsrField1;
        g_stIpfUl.pstIpfBDQ[u32BdqWptr].u32UsrField2= pstUlParam[i].u32UsrField2;
        g_stIpfUl.pstIpfBDQ[u32BdqWptr].u32UsrField3 = pstUlParam[i].u32UsrField3;
        u32BdqWptr = ((u32BdqWptr + 1) < IPF_ULBD_DESC_SIZE)? (u32BdqWptr + 1) : 0;
    }

    /* 更新BD写指针*/
    IPF_REG_WRITE(SOC_IPF_CH0_BDQ_WPTR_ADDR(IPF_REGBASE_ADR), u32BdqWptr);

    return IPF_SUCCESS;
}
EXPORT_SYMBOL(BSP_IPF_ConfigUpFilter);


unsigned int BSP_IPF_GetUlDescNum(void)
{
    unsigned int u32UlAd0Num = 0;
    unsigned int u32UlAd1Num = 0;
    unsigned int u32UlBdNum = 0;
    unsigned int u32UlAdwptr = 0;
    unsigned int u32UlAdrptr = 0;
    unsigned int u32UlBdDepth = 0;
    SOC_IPF_CH0_DQ_DEPTH_UNION unDQDepth;

    /* 计算BD深度 */
    unDQDepth.value = 0;
    IPF_REG_READ(SOC_IPF_CH0_DQ_DEPTH_ADDR(IPF_REGBASE_ADR), unDQDepth.value);
    u32UlBdDepth = unDQDepth.reg.ul_bdq_depth;

    /* 计算空闲AD0数量 */
    IPF_REG_READ(SOC_IPF_CH0_ADQ0_WPTR_ADDR(IPF_REGBASE_ADR), u32UlAdwptr);
    IPF_REG_READ(SOC_IPF_CH0_ADQ0_RPTR_ADDR(IPF_REGBASE_ADR), u32UlAdrptr);

    /*写指针在前，正常顺序*/
    if (u32UlAdwptr >= u32UlAdrptr)
    {
        u32UlAd0Num = u32UlAdwptr - u32UlAdrptr;
    }
    else
    {
        u32UlAd0Num = IPF_ULAD0_DESC_SIZE - (u32UlAdrptr -u32UlAdwptr);
    }
    if(u32UlAd0Num > u32UlBdDepth)
    {
        u32UlAd0Num -= u32UlBdDepth;
    }
    else
    {
        u32UlAd0Num = 0;
        g_stIPFDebugInfo->u32UlAd0NotEnough++;
    }

    /* 计算空闲AD1数量 */
    IPF_REG_READ(SOC_IPF_CH0_ADQ1_WPTR_ADDR(IPF_REGBASE_ADR), u32UlAdwptr);
    IPF_REG_READ(SOC_IPF_CH0_ADQ1_RPTR_ADDR(IPF_REGBASE_ADR), u32UlAdrptr);

    if (u32UlAdwptr >= u32UlAdrptr)/*写指针在前，正常顺序*/
    {
        u32UlAd1Num = u32UlAdwptr - u32UlAdrptr;
    }
    else
    {
        u32UlAd1Num =  IPF_ULAD1_DESC_SIZE - (u32UlAdrptr -u32UlAdwptr);
    }

    if(u32UlAd1Num > u32UlBdDepth)
    {
        u32UlAd1Num -= u32UlBdDepth;
    }
    else
    {
        u32UlAd1Num = 0;
        g_stIPFDebugInfo->u32UlAd1NotEnough++;
    }

    u32UlBdNum = BSP_IPF_GetUlBDNum();
    if(0 == u32UlBdNum)
    {
        g_stIPFDebugInfo->u32UlBdNotEnough++;
    }

    if(u32UlBdNum > u32UlAd0Num)
    {
        u32UlBdNum = u32UlAd0Num;
    }

    if(u32UlBdNum > u32UlAd1Num)
    {
        u32UlBdNum = u32UlAd1Num;
    }

    return u32UlBdNum;
}
EXPORT_SYMBOL(BSP_IPF_GetUlDescNum);

/*****************************************************************************
* 函 数 名  : BSP_IPF_GetUlBDNum
*
* 功能描述  : 该接口用于获取上行空闲BD 数目
*
* 输入参数  : 无
*
* 输出参数  : 无
* 返 回 值  : 空闲BD数目
*
* 修改记录  :2011年11月30日   鲁婷  创建
*****************************************************************************/
unsigned int BSP_IPF_GetUlBDNum(void)
{
    SOC_IPF_CH0_DQ_DEPTH_UNION unUlBdDepth;
    unsigned int u32IdleBd = 0;

    unUlBdDepth.value = 0;
    /* 计算空闲BD数量 */
    IPF_REG_READ(SOC_IPF_CH0_DQ_DEPTH_ADDR(IPF_REGBASE_ADR), unUlBdDepth.value);
    u32IdleBd = IPF_ULBD_DESC_SIZE - unUlBdDepth.reg.ul_bdq_depth;
    *(g_stIpfUl.pu32IdleBd) = u32IdleBd;

    return u32IdleBd;
}
EXPORT_SYMBOL(BSP_IPF_GetUlBDNum);

/*****************************************************************************
* 函 数 名  : BSP_IPF_UlStateIdle
*
* 功能描述  : 该接口用于获取上行通道是否为空闲
*
* 输入参数  : 无
*
* 输出参数  : 无
* 返 回 值  : IPF_SUCCESS  表示上行空闲，可以切换模式
*             IPF_ERROR      表示上行非空闲，不可以切换模式
*
* 修改记录  :2011年12月9日   鲁婷  创建
*****************************************************************************/
int BSP_IPF_UlStateIdle(void)
{
    unsigned int u32UlState = 0;

    IPF_REG_READ(SOC_IPF_CH0_STATE_ADDR(IPF_REGBASE_ADR), u32UlState);

    if(u32UlState != IPF_CH_IDLE)
    {
        return IPF_ERROR;
    }

    return IPF_SUCCESS;
}
EXPORT_SYMBOL(BSP_IPF_UlStateIdle);

/*****************************************************************************
* 函 数 名  : BSP_IPF_PktLen
*
* 功能描述  : 该接口用来配置过滤器的最大和最小包长
*
* 输入参数  : unsigned int MaxLen   最大包长
*             unsigned int MinLen   最小包长
*
* 输出参数  : 无
* 返 回 值  : IPF_SUCCESS                成功
*             BSP_ERR_IPF_INVALID_PARA   参数错误(最大包长比最小包长小)
*
* 修改记录  :2011年2月17日   鲁婷  创建
*****************************************************************************/
int BSP_IPF_SetPktLen(unsigned int u32MaxLen, unsigned int u32MinLen)
{
    SOC_IPF_PKT_LEN_UNION unPktLen;

    unPktLen.value = 0;
    /* 参数检查 */
    if(u32MaxLen < u32MinLen)
    {
        IPF_PRINT_ERROR("parameter is error\n");
        return BSP_ERR_IPF_INVALID_PARA;
    }

    unPktLen.reg.max_pkt_len = u32MaxLen;
    unPktLen.reg.min_pkt_len = u32MinLen;

    IPF_REG_WRITE(SOC_IPF_PKT_LEN_ADDR(IPF_REGBASE_ADR), unPktLen.value);
    return IPF_SUCCESS;
}
EXPORT_SYMBOL(BSP_IPF_SetPktLen);

/*****************************************************************************
* 函 数 名  : BSP_IPF_GetStat
*
* 功能描述  : 该接口获取统计计数器的值
*
* 输入参数  : IPF_FILTER_STAT_S *pstFilterStat 统计计数结构体指针
*
*
* 输出参数  : 无
* 返 回 值  : IPF_SUCCESS                成功
*             BSP_ERR_IPF_INVALID_PARA   参数错误
*
* 修改记录  : 2011年2月17日   鲁婷  创建
*****************************************************************************/
int BSP_IPF_GetStat(IPF_FILTER_STAT_S *pstFilterStat)
{
    /* 打桩 */
    return IPF_SUCCESS;
}
EXPORT_SYMBOL(BSP_IPF_GetStat);
void BSP_IPF_DlRegReInit(void)
{
    /*打桩*/
}
EXPORT_SYMBOL(BSP_IPF_DlRegReInit);

/*****************************************************************************
* 函 数 名  : BSP_IPF_RegisterWakeupDlCb
*
* 功能描述  : 该接口用于注册下行PS任务回调函数
*
* 输入参数  : BSP_IPF_WakeupDlkCb *pFnWakeupDl
*
* 输出参数  : 无
* 返 回 值  : 无
*
* 修改记录  :2011年11月30日   鲁婷  创建
*****************************************************************************/
int BSP_IPF_RegisterWakeupDlCb(BSP_IPF_WakeupDlCb pFnWakeupDl)
{
    /* 参数检查 */
    if(NULL == pFnWakeupDl)
    {
        IPF_PRINT_ERROR("parameter is error\n");
        return BSP_ERR_IPF_INVALID_PARA;
    }
    g_stIpfDl.pFnDlIntCb = pFnWakeupDl;
    return IPF_SUCCESS;
}
EXPORT_SYMBOL(BSP_IPF_RegisterWakeupDlCb);
int BSP_IPF_RegisterAdqEmptyDlCb(BSP_IPF_AdqEmptyDlCb pAdqEmptyDl)
{
    /* 参数检查 */
    if(NULL == pAdqEmptyDl)
    {
        IPF_PRINT_ERROR("parameter is error\n");
        return BSP_ERR_IPF_INVALID_PARA;
    }
    g_stIpfDl.pAdqEmptyDlCb = pAdqEmptyDl;
    return IPF_SUCCESS;
}
EXPORT_SYMBOL(BSP_IPF_RegisterAdqEmptyDlCb);
unsigned int BSP_IPF_GetUlRdNum(void)
{
    SOC_IPF_CH0_DQ_DEPTH_UNION unRdqDepth;

    /* 读取上行RD深度寄存器 */
    unRdqDepth.value = 0;
    IPF_REG_READ(SOC_IPF_CH0_DQ_DEPTH_ADDR(IPF_REGBASE_ADR), unRdqDepth.value);

    return unRdqDepth.reg.ul_rdq_depth;
}
EXPORT_SYMBOL(BSP_IPF_GetUlRdNum);
unsigned int BSP_IPF_GetDlRdNum(void)
{
    SOC_IPF_CH1_DQ_DEPTH_UNION unRdqDepth;

    /* 读取上行RD深度寄存器 */
    unRdqDepth.value = 0;
    IPF_REG_READ(SOC_IPF_CH1_DQ_DEPTH_ADDR(IPF_REGBASE_ADR), unRdqDepth.value);

    return unRdqDepth.reg.dl_rdq_depth;
}
EXPORT_SYMBOL(BSP_IPF_GetDlRdNum);
void BSP_IPF_GetDlRd(unsigned int* pu32Num, IPF_RD_DESC_S *pstRd)
{
    SOC_IPF_CH1_DQ_DEPTH_UNION unDqDepth;
    unsigned int u32CdqRptr = 0;
    unsigned int u32RdqRptr = 0;
    unsigned int u32RdqDepth = 0;
    unsigned int u32Num = 0;
    unsigned int i = 0;

    if((NULL == pu32Num) || (NULL == pstRd))
    {
        IPF_PRINT_ERROR("parameter is error\n");
        return;
    }

    /* 读取RD深度 */
    unDqDepth.value = 0;
    IPF_REG_READ(SOC_IPF_CH1_DQ_DEPTH_ADDR(IPF_REGBASE_ADR), unDqDepth.value);
    u32RdqDepth = unDqDepth.reg.dl_rdq_depth;

    u32Num = (u32RdqDepth < *pu32Num)?u32RdqDepth:*pu32Num;
    if(u32Num > 0)
    {
        g_stIPFDebugInfo->u32DlRdNum += u32Num;
        for(i = 0; i < u32Num; i++)
        {
            /* 读取RD读指针 */
            IPF_REG_READ(SOC_IPF_CH1_RDQ_RPTR_ADDR(IPF_REGBASE_ADR), u32RdqRptr);
            if(u32RdqRptr >= IPF_DLRD_DESC_SIZE)
            {
                IPF_PRINT_ERROR("u32RdqRptr = %d\n", u32RdqRptr);
            }

            /* 获取RD */
            pstRd[i].u16Attribute = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u16Attribute;
            pstRd[i].u16PktLen = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u16PktLen;
            pstRd[i].u16Result = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u16Result;
            pstRd[i].u32InPtr = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u32InPtr;
            pstRd[i].u32OutPtr = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u32OutPtr;
            if((0 == pstRd[i].u32OutPtr)/* || (0x55555555 == pstRd[i].u32OutPtr) || (0xAAAAAAAA == pstRd[i].u32OutPtr)*/)
            {
                IPF_PRINT_ERROR("invalid rd of dl\n");
                IPF_Assert(BSP_ERR_IPF_INVALID_DLRD);
            }
            pstRd[i].u16UsrField1 = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u16UsrField1;
            pstRd[i].u32UsrField2 = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u32UsrField2;
            pstRd[i].u32UsrField3 = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u32UsrField3;

            u32RdqRptr = ((u32RdqRptr+1) < IPF_DLRD_DESC_SIZE)?(u32RdqRptr+1):0;

            /* 更新RD读指针 */
            IPF_REG_WRITE(SOC_IPF_CH1_RDQ_RPTR_ADDR(IPF_REGBASE_ADR), u32RdqRptr);
            /* 更新CD读指针 */
            u32CdqRptr = (unsigned int)((char *)(HISI_VA_ADDRESS(pstRd[i].u32InPtr)) - (char *)g_stIpfDl.pstIpfCDQ)/sizeof(IPF_CD_DESC_S);
            while(g_stIpfDl.pstIpfCDQ[u32CdqRptr].u16Attribute != 1)
            {
                /* 将释放的CD清0 */
                g_stIpfDl.pstIpfCDQ[u32CdqRptr].u16Attribute = 0;
                g_stIpfDl.pstIpfCDQ[u32CdqRptr].u16PktLen = 0;
                g_stIpfDl.pstIpfCDQ[u32CdqRptr].u32Ptr = 0;
                u32CdqRptr = ((u32CdqRptr+1) < IPF_DLCD_DESC_SIZE)?(u32CdqRptr+1):0;
            }
            u32CdqRptr = ((u32CdqRptr+1) < IPF_DLCD_DESC_SIZE)?(u32CdqRptr+1):0;
            *(g_stIpfDl.u32IpfCdRptr) = u32CdqRptr;
        }
    }
    *pu32Num = u32Num;
}
EXPORT_SYMBOL(BSP_IPF_GetDlRd);
int BSP_IPF_GetDlAdNum(unsigned int* pu32AD0Num, unsigned int* pu32AD1Num)
{
    unsigned int u32DlAd0Depth = 0;
    unsigned int u32DlAd1Depth = 0;
    unsigned int u32DlAd0wptr = 0;
    unsigned int u32DlAd0rptr = 0;
    unsigned int u32DlAd1wptr = 0;
    unsigned int u32DlAd1rptr = 0;
    unsigned int u32AD0Num = 0;
    unsigned int u32AD1Num = 0;
    /*入参检测*/
    if((NULL == pu32AD0Num)||(NULL == pu32AD1Num))
    {
        IPF_PRINT_ERROR("parameter is error\n");
        return BSP_ERR_IPF_INVALID_PARA;
    }

    /* 计算空闲AD0数量 */
    IPF_REG_READ(SOC_IPF_CH1_ADQ0_WPTR_ADDR(IPF_REGBASE_ADR), u32DlAd0wptr);
    IPF_REG_READ(SOC_IPF_CH1_ADQ0_RPTR_ADDR(IPF_REGBASE_ADR), u32DlAd0rptr);
    if (u32DlAd0wptr >= u32DlAd0rptr)/*写指针在前，正常顺序*/
        u32DlAd0Depth = IPF_DLAD0_DESC_SIZE - (u32DlAd0wptr - u32DlAd0rptr+1);
    else
        u32DlAd0Depth = u32DlAd0rptr - u32DlAd0wptr - 1;
    u32AD0Num = (u32DlAd0Depth > IPF_OBLIGATE_AD_NUM) ? (u32DlAd0Depth - IPF_OBLIGATE_AD_NUM) : 0;
    if(u32AD0Num > IPF_DLAD0_DESC_SIZE)
    {
        IPF_PRINT_ERROR("adq0 num is error\n");
        return BSP_ERR_IPF_INVALID_PARA;
    }
    *pu32AD0Num = u32AD0Num;

    /* 计算空闲AD1数量 */
    IPF_REG_READ(SOC_IPF_CH1_ADQ1_WPTR_ADDR(IPF_REGBASE_ADR), u32DlAd1wptr);
    IPF_REG_READ(SOC_IPF_CH1_ADQ1_RPTR_ADDR(IPF_REGBASE_ADR), u32DlAd1rptr);
    if (u32DlAd1wptr >= u32DlAd1rptr)/*写指针在前，正常顺序*/
        u32DlAd1Depth = IPF_DLAD1_DESC_SIZE - (u32DlAd1wptr - u32DlAd1rptr+1);
    else
        u32DlAd1Depth = u32DlAd1rptr - u32DlAd1wptr - 1;

    u32AD1Num = (u32DlAd1Depth > IPF_OBLIGATE_AD_NUM) ? (u32DlAd1Depth - IPF_OBLIGATE_AD_NUM) : 0;
    if(u32AD1Num > IPF_DLAD1_DESC_SIZE)
    {
        IPF_PRINT_ERROR("adq1 num is error\n");
        return BSP_ERR_IPF_INVALID_PARA;
    }
    *pu32AD1Num = u32AD1Num;

    return IPF_SUCCESS;
}
EXPORT_SYMBOL(BSP_IPF_GetDlAdNum);
int BSP_IPF_ConfigDlAd(IPF_AD_TYPE_E eAdType, unsigned int u32AdNum, IPF_AD_DESC_S *pstAdDesc  )
{
    unsigned int i;
    unsigned int u32ADQ0wptr;
    unsigned int u32ADQ1wptr;

    /*Modify_for_c_reset, l00212112,20130511, starts*/
    /*如果C核复位，则返回*/
    if (IPF_FORRESET_CONTROL_FORBID == IPF_GetControlFLagForCcoreReset())
    {
        IPF_PRINT_ERROR("ccore is resetting\n");
        return BSP_ERR_IPF_CCORE_RESETTING;
    }
    /*Modify_for_c_reset, l00212112,20130511, ends*/

    /* 入参检查 */
    if((IPF_AD_MAX <= eAdType) || (NULL == pstAdDesc))
    {
        IPF_PRINT_ERROR("parameter is error\n");
        return BSP_ERR_IPF_INVALID_PARA;
    }

    /* 检查模块是否初始化 */
    IPF_INIT_CHECK();

    if(IPF_AD_0 == eAdType)
    {
        g_stIPFDebugInfo->u32DlAdq0Num += u32AdNum;
        /* 读取ADQ0写指针 */
        IPF_REG_READ(SOC_IPF_CH1_ADQ0_WPTR_ADDR(IPF_REGBASE_ADR), u32ADQ0wptr);
        /* 获取目的结构体指针和目的结构体数据指针，插入到ADQ中 */
        for(i=0; i < u32AdNum; i++)
        {
            if((0 == pstAdDesc[i].u32OutPtr0) || (0 == pstAdDesc[i].u32OutPtr1))
            {
                IPF_PRINT_ERROR("point is null\n");
                return BSP_ERR_IPF_INVALID_PARA;
            }
            g_stIpfDl.pstIpfADQ0[u32ADQ0wptr].u32OutPtr0 = pstAdDesc[i].u32OutPtr0;
            g_stIpfDl.pstIpfADQ0[u32ADQ0wptr].u32OutPtr1 = pstAdDesc[i].u32OutPtr1;
            u32ADQ0wptr = ((u32ADQ0wptr + 1) < IPF_DLAD0_DESC_SIZE)? (u32ADQ0wptr + 1) : 0;
        }
        /* 更新ADQ0写指针 */
        IPF_REG_WRITE(SOC_IPF_CH1_ADQ0_WPTR_ADDR(IPF_REGBASE_ADR), u32ADQ0wptr);
    }
    else
    {
        g_stIPFDebugInfo->u32DlAdq1Num += u32AdNum;
        /* 读取ADQ1写指针 */
        IPF_REG_READ(SOC_IPF_CH1_ADQ1_WPTR_ADDR(IPF_REGBASE_ADR), u32ADQ1wptr);
        /* 获取目的结构体指针和目的结构体数据指针，插入到ADQ中 */
        for(i=0; i < u32AdNum; i++)
        {
            if((0 == pstAdDesc[i].u32OutPtr0) || (0 == pstAdDesc[i].u32OutPtr1))
            {
                IPF_PRINT_ERROR("point is null\n");
                return BSP_ERR_IPF_INVALID_PARA;
            }
            g_stIpfDl.pstIpfADQ1[u32ADQ1wptr].u32OutPtr0 = pstAdDesc[i].u32OutPtr0;
            g_stIpfDl.pstIpfADQ1[u32ADQ1wptr].u32OutPtr1 = pstAdDesc[i].u32OutPtr1;
            u32ADQ1wptr = ((u32ADQ1wptr + 1) < IPF_DLAD1_DESC_SIZE)? (u32ADQ1wptr + 1) : 0;
        }
        /* 更新ADQ1写指针 */
        IPF_REG_WRITE(SOC_IPF_CH1_ADQ1_WPTR_ADDR(IPF_REGBASE_ADR), u32ADQ1wptr);
    }
    return IPF_SUCCESS;
}
EXPORT_SYMBOL(BSP_IPF_ConfigDlAd);
int BSP_IPF_GetUsedDlAd(IPF_AD_TYPE_E eAdType, unsigned int *pu32AdNum, IPF_AD_DESC_S *pstAdDesc)
{
    unsigned int i = 0;
    unsigned int u32IpfAdqRpt = 0;
    unsigned int u32IpfAdqWpt = 0;
    unsigned int u32IpfAdqSize = 0;
    IPF_AD_DESC_S *pstIpfADQ = (IPF_AD_DESC_S*)NULL;
    unsigned int u32AdNum = 0;

    /*  入参检查 */
    if((IPF_AD_MAX <= eAdType)|| (NULL == pu32AdNum) || (NULL == pstAdDesc))
    {
        IPF_PRINT_ERROR("parameter is error\n");
        return BSP_ERR_IPF_INVALID_PARA;
    }

    /* 获取真实的读指针 */
    if(IPF_SUCCESS != IPF_ObligateAdqRpt(IPF_CHANNEL_DOWN, eAdType, &u32IpfAdqRpt))
    {
        IPF_PRINT_ERROR("IPF_ObligateAdqRpt error\n");
        return IPF_ERROR;
    }

    /* 根据通道ID和AD类型获取操作寄存器对象(写指针、大小、队列) */
    switch (eAdType) {
        case IPF_AD_0:
                IPF_REG_READ(SOC_IPF_CH1_ADQ0_WPTR_ADDR(IPF_REGBASE_ADR), u32IpfAdqWpt);
                u32IpfAdqSize = IPF_DLAD0_DESC_SIZE;
                pstIpfADQ = g_stIpfDl.pstIpfADQ0;
            break;
        case IPF_AD_1:
                IPF_REG_READ(SOC_IPF_CH1_ADQ1_WPTR_ADDR(IPF_REGBASE_ADR), u32IpfAdqWpt);
                u32IpfAdqSize = IPF_DLAD1_DESC_SIZE;
                pstIpfADQ = g_stIpfDl.pstIpfADQ1;
            break;
        default:
            IPF_PRINT_ERROR("parameter is error\n");
            break;
    }

    if((IPF_AD_DESC_S*)NULL == pstIpfADQ)
    {
        return BSP_ERR_IPF_INVALID_PARA;
    }

    /* 根据获取下行ADQ队列(对应类型)中已经被配置使用的AD信息，并传给pu32AdNum和pstAdDesc */
    for(i = u32IpfAdqRpt; i != u32IpfAdqWpt; i %= u32IpfAdqSize)
    {
        if(NULL == pstAdDesc)
        {
            IPF_PRINT_ERROR("point is null\n");
            return BSP_ERR_IPF_INVALID_PARA;
        }
        pstAdDesc->u32OutPtr0 = pstIpfADQ[i].u32OutPtr0;
        pstAdDesc->u32OutPtr1 = pstIpfADQ[i].u32OutPtr1;
        u32AdNum++;
        pstAdDesc++;
        i++;
    }
    *pu32AdNum = u32AdNum;

    return IPF_SUCCESS;
}
EXPORT_SYMBOL(BSP_IPF_GetUsedDlAd);
/*Modify_for_c_reset, l00212112,20130511, ends*/

static int IPF_Probe(struct platform_device *pdev)
{
    return IPF_SUCCESS;
}


static int IPF_Remove(struct platform_device *pdev)
{
    return IPF_SUCCESS;
}


static int IPF_Suspend(struct platform_device *pdev,pm_message_t state)
{
    /* 关闭IPF hclk时钟、axi总线时钟 */
    IPF_REG_WRITE(SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_ADDR(ipf_peri_ctrl_base), \
                (0x1 << SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_periph_clkdis2_ipf_acpu_START));

    return 0;
}
static int IPF_Resume(struct platform_device *pdev)
{
    /* 打开IPF hclk时钟、axi总线时钟 */
    IPF_REG_WRITE(SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_ADDR(ipf_peri_ctrl_base), \
                (0x1 << SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_ipf_acpu_START));

    return 0;
}
int IPF_Init(void)
{
    int s32Ret = 0;

    /* IPF内存配置越界检查 */
    if(IPF_MEM_USED_SIZE > IPF_AXI_MEM_SIZE)
    {
        IPF_PRINT_ERROR("memory overstep the boundary\n");
        return IPF_ERROR;
    }


    hisi_io_memset((BSP_VOID*)&g_stIpfCtx, 0x0, sizeof(IPF_CONTEXT_S));

    IPF_ULBD_MEM_ADDR = HISI_VA_ADDRESS(IPF_AXI_MEM_ADDR);

    if (0 == (IPF_REGBASE_ADR = (unsigned long)ioremap(REG_BASE_IPF,REG_IPF_IOSIZE))){
        BUG_ON(1);
        return IPF_ERROR;
    }

    /* 为上行BD、RD描述符分配一段连续的物理地址 */
    g_stIpfUl.pstIpfBDQ = (IPF_BD_DESC_S*)IPF_ULBD_MEM_ADDR;
    g_stIpfUl.pstIpfRDQ = (IPF_RD_DESC_S*)IPF_ULRD_MEM_ADDR;

    /* 为上行AD描述符分配一段连续内存（首地址8字节对齐）*/
    g_stIpfUl.pstIpfADQ0 = (IPF_AD_DESC_S*)IPF_ULAD0_MEM_ADDR;
    g_stIpfUl.pstIpfADQ1 = (IPF_AD_DESC_S*)IPF_ULAD1_MEM_ADDR;

    /* 为下行BD、RD描述符分配一段连续的物理地址 */
    g_stIpfDl.pstIpfBDQ = (IPF_BD_DESC_S*)IPF_DLBD_MEM_ADDR;
    g_stIpfDl.pstIpfRDQ = (IPF_RD_DESC_S*)IPF_DLRD_MEM_ADDR;

    /* 为下行AD描述符分配一段连续内存（首地址8字节对齐）*/
    g_stIpfDl.pstIpfADQ0 = (IPF_AD_DESC_S*)IPF_DLAD0_MEM_ADDR;
    g_stIpfDl.pstIpfADQ1 = (IPF_AD_DESC_S*)IPF_DLAD1_MEM_ADDR;

    /* 为下行CD描述符分配一段连续的物理地址 */
    g_stIpfDl.pstIpfCDQ = (IPF_CD_DESC_S*)IPF_DLCD_MEM_ADDR;

    /* 记录IPF上行空闲BD个数 */
    g_stIpfUl.pu32IdleBd = (unsigned int*)IPF_ULBD_IDLENUM_ADDR;

#ifdef __BSP_IPF_DEBUG__
    /* 记录IPF debug信息 */
    g_stIPFDebugInfo = (IPF_DEBUG_INFO_S*)IPF_DEBUG_INFO_ADDR;

    /* 记录IPF 下行CDdebug信息 */
    g_stIpfDl.pstIpfDebugCDQ = (IPF_CD_DESC_S*)IPF_DEBUG_DLCD_ADDR;
#endif

    /* 记录IPF 下行CD读写指针 */
    g_stIpfDl.u32IpfCdRptr = (unsigned int*)IPF_DLCD_PTR_ADDR;
    g_stIpfDl.u32IpfCdWptr = (unsigned int*)(IPF_DLCD_PTR_ADDR+4);

    /* 挂接IPF中断 */
    IPF_Int_Connect();

    /* 注册IPF设备 */
    s32Ret = platform_device_register(&ipf_pfdev);
    if(s32Ret)
    {
        IPF_PRINT_ERROR("ipf device register fail\n");
        return s32Ret;
    }

    /* 注册IPF驱动 */
    s32Ret = platform_driver_register(&ipf_pfdrv);
    if(s32Ret)
    {
        platform_device_unregister(&ipf_pfdev);
        IPF_PRINT_ERROR("ipf driver register fail\n");
        return s32Ret;
    }

    ipf_peri_ctrl_base = (unsigned long)HISI_VA_ADDRESS(SOC_PERI_SCTRL_BASE_ADDR);
	if (!ipf_peri_ctrl_base)
	{
        IPF_PRINT_ERROR("unable to ioremap ipf peri ctrl\n");
	}

    /* 打开IPF hclk时钟、axi总线时钟 */
    IPF_REG_WRITE(SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_ADDR(ipf_peri_ctrl_base), \
                (0x1 << SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_ipf_acpu_START));

    /* 等待另外一个core ipf初始化同步完成 */
    s32Ret = mdrv_sync_wait(SYNC_MODULE_IPF, 5000);
    if(s32Ret != BSP_OK)
    {
        IPF_PRINT_ERROR("BSP_SYNC_Wait timeout\n");
        return IPF_ERROR;
    }

    /* IPF初始化完成 */
    g_stIpfCtx.isInit = BSP_TRUE;


    IPF_PRINT_INFO("success\n");
    return IPF_SUCCESS;
}
/*module_init(IPF_Init);*/
MODULE_LICENSE("GPL V2");
