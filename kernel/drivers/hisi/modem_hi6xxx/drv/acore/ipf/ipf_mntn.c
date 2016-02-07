

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <linux/module.h>
#include "drv_global.h"
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */
#include "bsp_mem_drv.h"
#include "ipf_driver.h"

#define IPF_REG_OFFSET_TO_ADDR(i)   (IPF_REGBASE_ADR + 4 * i)

extern IPF_UL_S g_stIpfUl;
extern IPF_DL_S g_stIpfDl;
extern IPF_DEBUG_INFO_S* g_stIPFDebugInfo;
/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/
BSP_VOID BSP_IPF_Help(BSP_VOID)
{
    IPF_PRINT_ERROR("===============================================\n");
    IPF_PRINT_ERROR("BSP_IPF_RegDump 参数1:无\n");
    IPF_PRINT_ERROR("BSP_IPF_DbgInfo 参数1:无\n");
    IPF_PRINT_ERROR("BSP_IPF_MemInfo 参数1:无\n");
    IPF_PRINT_ERROR("BSP_IPF_BDInfo  参数1:通道类型(0-上行；1-下行)  参数2:BD指针偏移(0~63)\n");
    IPF_PRINT_ERROR("BSP_IPF_RDInfo  参数1:通道类型(0-上行；1-下行)  参数2:RD指针偏移(0~63)\n");
    IPF_PRINT_ERROR("BSP_IPF_CHInfo  参数1:通道类型(0-上行；1-下行)\n");
}

BSP_S32 BSP_IPF_RegDump(BSP_VOID)
{
    BSP_U32 i = 0;
    BSP_U32 u32RegValue = 0;

    IPF_PRINT_ERROR("============BEGIN===========\n");
    for(i = 0; i < IPF_REG_NUM; i++)
    {
        IPF_REG_READ(IPF_REG_OFFSET_TO_ADDR(i), u32RegValue);
        IPF_PRINT_ERROR("[0x%lx]:          0x%x\n", IPF_REG_OFFSET_TO_ADDR(i), u32RegValue); 
    }
    IPF_PRINT_ERROR("=============END============\n");

    return 0;
}

BSP_S32 BSP_IPF_DbgInfo(BSP_VOID)
{
    IPF_PRINT_ERROR("============BEGIN===========\n");
    IPF_PRINT_ERROR("上行处理结果中断上报次数:           %d\n",g_stIPFDebugInfo->u32UlResultCnt);
    IPF_PRINT_ERROR("上行处理结果超时中断上报次数:       %d\n",g_stIPFDebugInfo->u32UlResultTimeoutCnt);
    IPF_PRINT_ERROR("下行处理结果中断上报次数:           %d\n",g_stIPFDebugInfo->u32DlResultCnt);
    IPF_PRINT_ERROR("下行处理结果超时中断上报次数:       %d\n",g_stIPFDebugInfo->u32DlResultTimeoutCnt);
    IPF_PRINT_ERROR("上行ADQ0空中断次数:                 %d\n",g_stIPFDebugInfo->u32UlAdq0Empty);
    IPF_PRINT_ERROR("上行ADQ1空中断次数:                 %d\n",g_stIPFDebugInfo->u32UlAdq1Empty);
    IPF_PRINT_ERROR("下行ADQ0空中断次数:                 %d\n",g_stIPFDebugInfo->u32DlAdq0Empty);
    IPF_PRINT_ERROR("下行ADQ1空中断次数:                 %d\n",g_stIPFDebugInfo->u32DlAdq1Empty);
    IPF_PRINT_ERROR("上行配置BD,BD不够用次数:            %d\n",g_stIPFDebugInfo->u32UlBdNotEnough);
    IPF_PRINT_ERROR("下行配置BD,BD不够用次数:            %d\n",g_stIPFDebugInfo->u32DlBdNotEnough);
    IPF_PRINT_ERROR("下行配置CD,CD不够用次数:            %d\n",g_stIPFDebugInfo->u32DlCdNotEnough);

    IPF_PRINT_ERROR("上行过滤规则恢复超时次数:           %d\n",g_stIPFDebugInfo->u32UlResumeTimeoutCnt);
    IPF_PRINT_ERROR("下行过滤规则恢复超时次数:           %d\n",g_stIPFDebugInfo->u32DlResumeTimeoutCnt);
    IPF_PRINT_ERROR("上行发送BD计数:                     %d\n",g_stIPFDebugInfo->u32UlBdNum);
    IPF_PRINT_ERROR("上行配置AD0计数:                    %d\n",g_stIPFDebugInfo->u32UlAdq0Num);
    IPF_PRINT_ERROR("上行配置AD1计数:                    %d\n",g_stIPFDebugInfo->u32UlAdq1Num);
    IPF_PRINT_ERROR("上行接收RD计数:                     %d\n",g_stIPFDebugInfo->u32UlRdNum);
    IPF_PRINT_ERROR("下行发送BD计数:                     %d\n",g_stIPFDebugInfo->u32DlBdNum);
    IPF_PRINT_ERROR("下行配置AD0计数:                    %d\n",g_stIPFDebugInfo->u32DlAdq0Num);
    IPF_PRINT_ERROR("下行配置AD1计数:                    %d\n",g_stIPFDebugInfo->u32DlAdq1Num);
    IPF_PRINT_ERROR("下行接收RD计数:                     %d\n",g_stIPFDebugInfo->u32DlRdNum);

    IPF_PRINT_ERROR("上行BD对齐错误:                     %d\n",g_stIPFDebugInfo->u32UlBdAlignErr);
    IPF_PRINT_ERROR("下行BD对齐错误:                     %d\n",g_stIPFDebugInfo->u32DlBdAlignErr);
    IPF_PRINT_ERROR("上行BD大小错误:                     %d\n",g_stIPFDebugInfo->u32UlBdSizeErr);
    IPF_PRINT_ERROR("下行BD大小错误:                     %d\n",g_stIPFDebugInfo->u32DlBdSizeErr);

    IPF_PRINT_ERROR("MEMORY_AXI_IPF_ADDR:                0x%x\n",MEMORY_AXI_IPF_ADDR);

    IPF_PRINT_ERROR("=============END============\n");

    return 0;
}

BSP_VOID BSP_IPF_MemInfo(BSP_VOID)
{
    IPF_PRINT_ERROR("============BEGIN===========\n");
    IPF_PRINT_ERROR("   BSP_IPF_MEM          ADDR            SIZE\n");
    IPF_PRINT_ERROR("%s%#lx\t\t%#x\n", "IPF_ULBD_MEM_ADDR    ", IPF_ULBD_MEM_ADDR, (BSP_U32)IPF_ULBD_MEM_SIZE);
    IPF_PRINT_ERROR("%s%#lx\t\t%#x\n", "IPF_ULRD_MEM_ADDR    ", IPF_ULRD_MEM_ADDR, (BSP_U32)IPF_ULRD_MEM_SIZE);
    IPF_PRINT_ERROR("%s%#lx\t\t%#x\n", "IPF_DLBD_MEM_ADDR    ", IPF_DLBD_MEM_ADDR, (BSP_U32)IPF_DLBD_MEM_SIZE);
    IPF_PRINT_ERROR("%s%#lx\t\t%#x\n", "IPF_DLRD_MEM_ADDR    ", IPF_DLRD_MEM_ADDR, (BSP_U32)IPF_DLRD_MEM_SIZE);
    IPF_PRINT_ERROR("%s%#lx\t\t%#x\n", "IPF_DLCD_MEM_ADDR    ", IPF_DLCD_MEM_ADDR, (BSP_U32)IPF_DLCD_MEM_SIZE);
    IPF_PRINT_ERROR("%20s%#lx\t\t%#x\n", "IPF_DEBUG_INFO_ADDR  ", IPF_DEBUG_INFO_ADDR, (BSP_U32)IPF_DEBUG_INFO_SIZE);
    IPF_PRINT_ERROR("=============END============\n");
}

BSP_S32 BSP_IPF_BDInfo(IPF_CHANNEL_TYPE_E eChnType, BSP_U32 u32BdqPtr)
{
    BSP_U32 u32CdPtr;
    if(u32BdqPtr >= IPF_ULBD_DESC_SIZE)
    {
        return IPF_ERROR;
    }
    switch(eChnType)
    {
        case IPF_CHANNEL_UP:

                IPF_PRINT_ERROR("==========BD Info=========\n");
                IPF_PRINT_ERROR("BD位置:         %d\n",u32BdqPtr);
                IPF_PRINT_ERROR("u16Attribute:   %d\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u16Attribute);
                IPF_PRINT_ERROR("u16PktLen:      %d\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u16PktLen);
                IPF_PRINT_ERROR("u32InPtr:       0x%x\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u32InPtr);
                IPF_PRINT_ERROR("u32OutPtr:      0x%x\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u32OutPtr);
                IPF_PRINT_ERROR("u16Resv:        %d\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u16Result);
                IPF_PRINT_ERROR("u16UsrField1:   %d\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u16UsrField1);
                IPF_PRINT_ERROR("u32UsrField2:   0x%x\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u32UsrField2);
                IPF_PRINT_ERROR("u32UsrField3:   0x%x\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u32UsrField3);
            break;
       case IPF_CHANNEL_DOWN:
                IPF_PRINT_ERROR("==========BD Info=========\n");
                IPF_PRINT_ERROR("BD位置:         %d\n",u32BdqPtr);
                IPF_PRINT_ERROR("u16Attribute:   %d\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u16Attribute);
                IPF_PRINT_ERROR("u16PktLen:      %d\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u16PktLen);
                IPF_PRINT_ERROR("u32InPtr:       0x%x\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u32InPtr);
                IPF_PRINT_ERROR("u32OutPtr:      0x%x\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u32OutPtr);
                IPF_PRINT_ERROR("u16Resv:        %d\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u16Result);
                IPF_PRINT_ERROR("u16UsrField1:   %d\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u16UsrField1);
                IPF_PRINT_ERROR("u32UsrField2:   0x%x\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u32UsrField2);
                IPF_PRINT_ERROR("u32UsrField3:   0x%x\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u32UsrField3);
                u32CdPtr = (BSP_U32)((char*)HISI_VA_ADDRESS(g_stIpfDl.pstIpfBDQ[u32BdqPtr].u32InPtr)-(char*)g_stIpfDl.pstIpfCDQ);
                u32CdPtr = u32CdPtr/sizeof(IPF_CD_DESC_S);
                while(g_stIpfDl.pstIpfDebugCDQ[u32CdPtr].u16Attribute != 1)
                {
                    IPF_PRINT_ERROR("==========CD Info=========\n");
                    IPF_PRINT_ERROR("CD位置:             %d\n",u32CdPtr);
                    IPF_PRINT_ERROR("u16Attribute:       %d\n",g_stIpfDl.pstIpfDebugCDQ[u32CdPtr].u16Attribute);
                    IPF_PRINT_ERROR("u16PktLen:          %d\n",g_stIpfDl.pstIpfDebugCDQ[u32CdPtr].u16PktLen);
                    IPF_PRINT_ERROR("u32Ptr:             0x%x\n",g_stIpfDl.pstIpfDebugCDQ[u32CdPtr].u32Ptr);
                    u32CdPtr = ((u32CdPtr+1) < IPF_DLCD_DESC_SIZE)?(u32CdPtr+1):0;
                };
                IPF_PRINT_ERROR("==========CD Info=========\n");
                IPF_PRINT_ERROR("CD位置:             %d\n",u32CdPtr);
                IPF_PRINT_ERROR("u16Attribute:       %d\n",g_stIpfDl.pstIpfDebugCDQ[u32CdPtr].u16Attribute);
                IPF_PRINT_ERROR("u16PktLen:          %d\n",g_stIpfDl.pstIpfDebugCDQ[u32CdPtr].u16PktLen);
                IPF_PRINT_ERROR("u32Ptr:             0x%x\n",g_stIpfDl.pstIpfDebugCDQ[u32CdPtr].u32Ptr);
            break;
        default:
            break;
    }
    IPF_PRINT_ERROR("************************\n");
    return 0;
}

BSP_S32 BSP_IPF_RDInfo(IPF_CHANNEL_TYPE_E eChnType, BSP_U32 u32RdqPtr)
{
    if(u32RdqPtr >= IPF_ULRD_DESC_SIZE)
    {
        return IPF_ERROR;
    }
    switch(eChnType)
    {
        case IPF_CHANNEL_UP:
            IPF_PRINT_ERROR("===========RD Info==========\n");
            IPF_PRINT_ERROR("RD位置:             %d\n",u32RdqPtr);
            IPF_PRINT_ERROR("u16Attribute:       %d\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u16Attribute);
            IPF_PRINT_ERROR("u16PktLen:          %d\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u16PktLen);
            IPF_PRINT_ERROR("u32InPtr:           0x%x\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u32InPtr);
            IPF_PRINT_ERROR("u32OutPtr:          0x%x\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u32OutPtr);
            IPF_PRINT_ERROR("u16Result:          0x%x\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u16Result);
            IPF_PRINT_ERROR("u16UsrField1:       0x%x\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u16UsrField1);
            IPF_PRINT_ERROR("u32UsrField2:       0x%x\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u32UsrField2);
            IPF_PRINT_ERROR("u32UsrField3:       0x%x\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u32UsrField3);
            break;
       case IPF_CHANNEL_DOWN:
            IPF_PRINT_ERROR("============RD Info===========\n");
            IPF_PRINT_ERROR("RD位置:             %d\n",u32RdqPtr);
            IPF_PRINT_ERROR("u16Attribute:       %d\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u16Attribute);
            IPF_PRINT_ERROR("u16PktLen:          %d\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u16PktLen);
            IPF_PRINT_ERROR("u32InPtr:           0x%x\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u32InPtr);
            IPF_PRINT_ERROR("u32OutPtr:          0x%x\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u32OutPtr);
            IPF_PRINT_ERROR("u16Result:          0x%x\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u16Result);
            IPF_PRINT_ERROR("u16UsrField1:       0x%x\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u16UsrField1);
            IPF_PRINT_ERROR("u32UsrField2:       0x%x\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u32UsrField2);
            IPF_PRINT_ERROR("u32UsrField3:       0x%x\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u32UsrField3);
            break;
        default:
            break;
    }
    IPF_PRINT_ERROR("************************\n");
    return 0;
}

BSP_S32 BSP_IPF_CHInfo(IPF_CHANNEL_TYPE_E eChnType)
{
    BSP_U32 u32ChanCtrlInfo = 0;
    BSP_U32 u32ChanStateInfo = 0;

    BSP_U32 u32BdqBaseAddr = 0;
    BSP_U32 u32BdqDepth = 0;
    BSP_U32 u32BdqWptr = 0;
    BSP_U32 u32BdqRptr = 0;
    BSP_U32 u32BdqWaddr = 0;
    BSP_U32 u32BdqRaddr = 0;
    BSP_U32 u32RdqBaseAddr = 0;
    BSP_U32 u32RdqDepth = 0;
    BSP_U32 u32RdqRptr = 0;
    BSP_U32 u32RdqWptr = 0;
    BSP_U32 u32RdqWaddr = 0;
    BSP_U32 u32RdqRaddr = 0;
    BSP_U32 u32Depth = 0;

    BSP_U32 u32AdqCtrlInfo = 0;
    BSP_U32 u32Adq0BaseAddr = 0;
    BSP_U32 u32Adq0StateInfo = 0;
    BSP_U32 u32Adq0Wptr = 0;
    BSP_U32 u32Adq0Rptr = 0;
    BSP_U32 u32Adq1BaseAddr = 0;
    BSP_U32 u32Adq1StateInfo = 0;
    BSP_U32 u32Adq1Wptr = 0;
    BSP_U32 u32Adq1Rptr = 0;

    switch (eChnType) {

        case IPF_CHANNEL_UP:
            IPF_REG_READ(SOC_IPF_CH0_CTRL_ADDR(IPF_REGBASE_ADR), u32ChanCtrlInfo);
            IPF_REG_READ(SOC_IPF_CH0_STATE_ADDR(IPF_REGBASE_ADR), u32ChanStateInfo);

            IPF_REG_READ(SOC_IPF_CH0_BDQ_BADDR_ADDR(IPF_REGBASE_ADR), u32BdqBaseAddr);
            IPF_REG_READ(SOC_IPF_CH0_BDQ_SIZE_ADDR(IPF_REGBASE_ADR), u32BdqDepth);
            IPF_REG_READ(SOC_IPF_CH0_BDQ_WPTR_ADDR(IPF_REGBASE_ADR), u32BdqWptr);
            IPF_REG_READ(SOC_IPF_CH0_BDQ_RPTR_ADDR(IPF_REGBASE_ADR), u32BdqRptr);
            IPF_REG_READ(SOC_IPF_CH0_BDQ_WADDR_ADDR(IPF_REGBASE_ADR), u32BdqWaddr);
            IPF_REG_READ(SOC_IPF_CH0_BDQ_RADDR_ADDR(IPF_REGBASE_ADR), u32BdqRaddr);
            IPF_REG_READ(SOC_IPF_CH0_RDQ_BADDR_ADDR(IPF_REGBASE_ADR), u32RdqBaseAddr);
            IPF_REG_READ(SOC_IPF_CH0_RDQ_SIZE_ADDR(IPF_REGBASE_ADR), u32RdqDepth);
            IPF_REG_READ(SOC_IPF_CH0_RDQ_RPTR_ADDR(IPF_REGBASE_ADR), u32RdqRptr);
            IPF_REG_READ(SOC_IPF_CH0_RDQ_WPTR_ADDR(IPF_REGBASE_ADR), u32RdqWptr);
            IPF_REG_READ(SOC_IPF_CH0_RDQ_WADDR_ADDR(IPF_REGBASE_ADR), u32RdqWaddr);
            IPF_REG_READ(SOC_IPF_CH0_RDQ_RADDR_ADDR(IPF_REGBASE_ADR), u32RdqRaddr);
            IPF_REG_READ(SOC_IPF_CH0_DQ_DEPTH_ADDR(IPF_REGBASE_ADR), u32Depth);

            IPF_REG_READ(SOC_IPF_CH0_ADQ_CTRL_ADDR(IPF_REGBASE_ADR), u32AdqCtrlInfo);
            IPF_REG_READ(SOC_IPF_CH0_ADQ0_BASE_ADDR(IPF_REGBASE_ADR), u32Adq0BaseAddr);
            IPF_REG_READ(SOC_IPF_CH0_ADQ0_STAT_ADDR(IPF_REGBASE_ADR), u32Adq0StateInfo);
            IPF_REG_READ(SOC_IPF_CH0_ADQ0_WPTR_ADDR(IPF_REGBASE_ADR), u32Adq0Wptr);
            IPF_REG_READ(SOC_IPF_CH0_ADQ0_RPTR_ADDR(IPF_REGBASE_ADR), u32Adq0Rptr);
            IPF_REG_READ(SOC_IPF_CH0_ADQ1_BASE_ADDR(IPF_REGBASE_ADR), u32Adq1BaseAddr);
            IPF_REG_READ(SOC_IPF_CH0_ADQ1_STAT_ADDR(IPF_REGBASE_ADR), u32Adq1StateInfo);
            IPF_REG_READ(SOC_IPF_CH0_ADQ1_WPTR_ADDR(IPF_REGBASE_ADR), u32Adq1Wptr);
            IPF_REG_READ(SOC_IPF_CH0_ADQ1_RPTR_ADDR(IPF_REGBASE_ADR), u32Adq1Rptr);
            break;

        case IPF_CHANNEL_DOWN:
            IPF_REG_READ(SOC_IPF_CH1_CTRL_ADDR(IPF_REGBASE_ADR), u32ChanCtrlInfo);
            IPF_REG_READ(SOC_IPF_CH1_STATE_ADDR(IPF_REGBASE_ADR), u32ChanStateInfo);

            IPF_REG_READ(SOC_IPF_CH1_BDQ_BADDR_ADDR(IPF_REGBASE_ADR), u32BdqBaseAddr);
            IPF_REG_READ(SOC_IPF_CH1_BDQ_SIZE_ADDR(IPF_REGBASE_ADR), u32BdqDepth);
            IPF_REG_READ(SOC_IPF_CH1_BDQ_WPTR_ADDR(IPF_REGBASE_ADR), u32BdqWptr);
            IPF_REG_READ(SOC_IPF_CH1_BDQ_RPTR_ADDR(IPF_REGBASE_ADR), u32BdqRptr);
            IPF_REG_READ(SOC_IPF_CH1_BDQ_WADDR_ADDR(IPF_REGBASE_ADR), u32BdqWaddr);
            IPF_REG_READ(SOC_IPF_CH1_BDQ_RADDR_ADDR(IPF_REGBASE_ADR), u32BdqRaddr);
            IPF_REG_READ(SOC_IPF_CH1_RDQ_BADDR_ADDR(IPF_REGBASE_ADR), u32RdqBaseAddr);
            IPF_REG_READ(SOC_IPF_CH1_RDQ_SIZE_ADDR(IPF_REGBASE_ADR), u32RdqDepth);
            IPF_REG_READ(SOC_IPF_CH1_RDQ_RPTR_ADDR(IPF_REGBASE_ADR), u32RdqRptr);
            IPF_REG_READ(SOC_IPF_CH1_RDQ_WPTR_ADDR(IPF_REGBASE_ADR), u32RdqWptr);
            IPF_REG_READ(SOC_IPF_CH1_RDQ_WADDR_ADDR(IPF_REGBASE_ADR), u32RdqWaddr);
            IPF_REG_READ(SOC_IPF_CH1_RDQ_RADDR_ADDR(IPF_REGBASE_ADR), u32RdqRaddr);
            IPF_REG_READ(SOC_IPF_CH1_DQ_DEPTH_ADDR(IPF_REGBASE_ADR), u32Depth);

            IPF_REG_READ(SOC_IPF_CH1_ADQ_CTRL_ADDR(IPF_REGBASE_ADR), u32AdqCtrlInfo);
            IPF_REG_READ(SOC_IPF_CH1_ADQ0_BASE_ADDR(IPF_REGBASE_ADR), u32Adq0BaseAddr);
            IPF_REG_READ(SOC_IPF_CH1_ADQ0_STAT_ADDR(IPF_REGBASE_ADR), u32Adq0StateInfo);
            IPF_REG_READ(SOC_IPF_CH1_ADQ0_WPTR_ADDR(IPF_REGBASE_ADR), u32Adq0Wptr);
            IPF_REG_READ(SOC_IPF_CH1_ADQ0_RPTR_ADDR(IPF_REGBASE_ADR), u32Adq0Rptr);
            IPF_REG_READ(SOC_IPF_CH1_ADQ1_BASE_ADDR(IPF_REGBASE_ADR), u32Adq1BaseAddr);
            IPF_REG_READ(SOC_IPF_CH1_ADQ1_STAT_ADDR(IPF_REGBASE_ADR), u32Adq1StateInfo);
            IPF_REG_READ(SOC_IPF_CH1_ADQ1_WPTR_ADDR(IPF_REGBASE_ADR), u32Adq1Wptr);
            IPF_REG_READ(SOC_IPF_CH1_ADQ1_RPTR_ADDR(IPF_REGBASE_ADR), u32Adq1Rptr);
            break;

        default:
            IPF_PRINT_ERROR("invalidable channel type \n");
            break;
    }

    IPF_PRINT_ERROR("============BEGIN===========\n");
    IPF_PRINT_ERROR("通道控制寄存器:     0x%x\n", u32ChanCtrlInfo);
    IPF_PRINT_ERROR("通道状态寄存器:     0x%x\n", u32ChanStateInfo);

    IPF_PRINT_ERROR("BD 基地址:          0x%x\n", u32BdqBaseAddr);
    IPF_PRINT_ERROR("BD 深度:            0x%x\n", u32BdqDepth);
    IPF_PRINT_ERROR("BD 写指针:          0x%x\n", u32BdqWptr);
    IPF_PRINT_ERROR("BD 读指针:          0x%x\n", u32BdqRptr);
    IPF_PRINT_ERROR("BD 写地址:          0x%x\n", u32BdqWaddr);
    IPF_PRINT_ERROR("BD 读地址:          0x%x\n", u32BdqRaddr);
    IPF_PRINT_ERROR("RD 基地址:          0x%x\n", u32RdqBaseAddr);
    IPF_PRINT_ERROR("RD 深度:            0x%x\n", u32RdqDepth);
    IPF_PRINT_ERROR("RD 读指针:          0x%x\n", u32RdqRptr);
    IPF_PRINT_ERROR("RD 写指针:          0x%x\n", u32RdqWptr);
    IPF_PRINT_ERROR("RD 读地址:          0x%x\n", u32RdqRaddr);
    IPF_PRINT_ERROR("RD 写地址:          0x%x\n", u32RdqWaddr);
    IPF_PRINT_ERROR("通道深度寄存器:     0x%x\n", u32Depth);

    IPF_PRINT_ERROR("ADQ控制寄存器:      0x%x\n", u32AdqCtrlInfo);
    IPF_PRINT_ERROR("ADQ0 基地址:          0x%x\n", u32Adq0BaseAddr);
    IPF_PRINT_ERROR("ADQ0 状态寄存器:          0x%x\n", u32Adq0StateInfo);
    IPF_PRINT_ERROR("ADQ0 写指针:            0x%x\n", u32Adq0Wptr);
    IPF_PRINT_ERROR("ADQ0 读指针:          0x%x\n", u32Adq0Rptr);
    IPF_PRINT_ERROR("ADQ1 基地址:          0x%x\n", u32Adq1BaseAddr);
    IPF_PRINT_ERROR("ADQ1 状态寄存器:          0x%x\n", u32Adq1StateInfo);
    IPF_PRINT_ERROR("ADQ1 写指针:            0x%x\n", u32Adq1Wptr);
    IPF_PRINT_ERROR("ADQ1 读指针:          0x%x\n", u32Adq1Rptr);
    IPF_PRINT_ERROR("=============END============\n");
    return 0;
}

EXPORT_SYMBOL(BSP_IPF_Help);
EXPORT_SYMBOL(BSP_IPF_RegDump);
EXPORT_SYMBOL(BSP_IPF_DbgInfo);
EXPORT_SYMBOL(BSP_IPF_MemInfo);
EXPORT_SYMBOL(BSP_IPF_BDInfo);
EXPORT_SYMBOL(BSP_IPF_RDInfo);
EXPORT_SYMBOL(BSP_IPF_CHInfo);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
