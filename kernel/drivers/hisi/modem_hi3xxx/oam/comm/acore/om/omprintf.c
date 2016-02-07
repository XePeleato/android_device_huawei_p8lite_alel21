

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "v_IO.h"
#include "OmApp.h"
#include "omrl.h"
#include "omringbuffer.h"
#include "omprivate.h"
#include "NVIM_Interface.h"
#include "omprintf.h"

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
#include "PsLogAdapterApp.h"
#endif

#if (VOS_OS_VER == VOS_LINUX)
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/kallsyms.h>
#include <asm/uaccess.h>
#endif


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*lint -e767 */
#define    THIS_FILE_ID        PS_FILE_ID_OM_PRINTF_C
/*lint +e767 */

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* OM Printf输入环形缓冲区ID */
OM_RING_ID                          g_pstOmPrintfBufId;

/* OM Printf输出端口，默认为SHELL口 */
VOS_UINT32                          g_ulOmPrintfPort = OM_OUTPUT_SHELL;

/* Printf BUFFER 保护信号量 */
VOS_SEM                             g_ulOmPrintfBuffSem;

/* 自处理任务信号量 */
VOS_SEM                             g_ulPrintfTaskSem;

/* 记录OM Printf 数据输出中无buffer错误 */
OM_PRINTF_ERROR_RECORD_STRU         g_stOmPrintfErrRecord;

/* 记录OM Printf 数据输出中获取信号量失败，存入数据失败等错误类型 */
VOS_UINT32                          g_ulPrintfErrType = OM_OK;

/* 标识OM Printf模块是否初始化*/
VOS_BOOL                            g_bIsPrintfInit = VOS_FALSE;

/* 保存模块打印级别 */
LOG_LEVEL_EN                        g_astOmPrintfOutPutLev[OM_PRINTF_MODULE_MAX_NUM];

#if (VOS_OS_VER == VOS_LINUX)
extern ssize_t OmPrintfModule_Write(struct file *file, const char __user *buf, size_t lLength, loff_t *ppos);
extern ssize_t OmPrintf_Write(struct file *file, const char __user *buf, size_t lLength, loff_t *ppos);
extern VOS_UINT32 OmPrintf_VFInit(VOS_VOID);

#define OM_VFILE_CRT_LEVEL (0660)

static const struct file_operations g_stOmPrintf_FOPS =
{
    .owner      = THIS_MODULE,
    .write      = OmPrintf_Write,
};

static const struct file_operations g_stOmPrintfModule_FOPS =
{
    .owner      = THIS_MODULE,
    .write      = OmPrintfModule_Write,
};
#endif

/*****************************************************************************
  3 函数实现
*****************************************************************************/



VOS_UINT32 OM_PrintfGetModuleIdLev(VOS_UINT32 ulModuleId)
{
    return g_astOmPrintfOutPutLev[ulModuleId-LOG_MIN_MODULE_ID_ACPU_DRV];
}

VOS_UINT32 OM_PrintfSetModuleIdLev(VOS_UINT32 ulModuleId, VOS_UINT32 ulLev)
{
    /* 输入参数检查 */
    if((LOG_MAX_MODULE_ID_APP < ulModuleId)||(LOG_MIN_MODULE_ID_ACPU_DRV > ulModuleId)
                                       || (LOG_LEVEL_BUTT <= ulLev))
    {
#if (VOS_OS_VER == VOS_LINUX)
        printk("\r\nOM_PrintfSetModuleIdLev:Unvalid para! ModuleID:%d, Level:%d",(VOS_INT)ulModuleId, (VOS_INT)ulLev);
#else
        printf("OM_PrintfSetModuleIdLev:Unvalid para!");
#endif
        return OM_ERR_UNVALIDPARA;
    }

    g_astOmPrintfOutPutLev[ulModuleId-LOG_MIN_MODULE_ID_ACPU_DRV] = (LOG_LEVEL_EN)ulLev;

    return OM_OK;
}
VOS_UINT32 OM_PrintfSetOutLev(LOG_ID_LEVEL_STRU *pstLogIdLevel, VOS_UINT32 ulLength)
{
    VOS_UINT32         ulIndex;
    VOS_UINT32         ulModuleNum;
    VOS_UINT32         ulModuleId;
    VOS_UINT32         ulLev;

    /*检测参数的合法性*/
    if (VOS_ERR == Log_CheckPara(pstLogIdLevel, ulLength))
    {
        return VOS_ERR;
    }

    ulModuleNum = pstLogIdLevel->ulModuleNum;

    /*将每个模块的打印级别填入到全局过滤表中*/
    for (ulIndex = 0; ulIndex < ulModuleNum; ulIndex++)
    {
        ulModuleId = pstLogIdLevel->astModuleLev[ulIndex].ulModuleId;
        ulLev      = pstLogIdLevel->astModuleLev[ulIndex].enPrintLev;

        OM_PrintfSetModuleIdLev(ulModuleId,ulLev);
    }

    return VOS_OK;
}


VOS_VOID OM_PrintfMsgProc(OM_REQ_PACKET_STRU *pstRspPacket, OM_RSP_FUNC *pstRspFuncPtr)
{
    APP_OM_MSG_EX_STRU             *pstAppToOmMsg;
    VOS_UINT16                      usReturnPrimId;
    VOS_UINT32                      ulRet = VOS_ERR;
    VOS_UINT32                      ulOutputType;

    pstAppToOmMsg   = (APP_OM_MSG_EX_STRU*)pstRspPacket;

    /* SDT动态修改OM_Printf输出端口 */
    if (APP_OM_SET_PRINTF_PORT_REQ == pstAppToOmMsg->usPrimId)
    {
        /* 获取消息中端口类型 */
        ulOutputType    = *((VOS_UINT32*)pstAppToOmMsg->aucPara);

        /* 入参判断 */
        if( OM_OUTPUT_BUTT <= ulOutputType)
        {
            vos_printf("OM_PrintfMsgProc:wrong output port: %d\n", ulOutputType);
            return;
        }

        usReturnPrimId = OM_APP_SET_PRINTF_PORT_CNF;
        g_ulOmPrintfPort = ulOutputType;

        ulRet = VOS_OK;
    }

    /*设置日志打印级别的原语消息*/
    else if (APP_OM_SET_PRINTF_OUTPUT_LEV_REQ == pstAppToOmMsg->usPrimId)
    {
        usReturnPrimId = APP_OM_SET_PRINTF_OUTPUT_LEV_CNF;

        ulRet = OM_PrintfSetOutLev((LOG_ID_LEVEL_STRU*)(pstAppToOmMsg->aucPara),
          (VOS_UINT32)pstAppToOmMsg->usLength - (OM_APP_MSG_EX_LEN - VOS_OM_HEADER_LEN));
    }
    else
    {
        vos_printf("OM_PrintfMsgProc :Undefined PrimId : %d\n", pstAppToOmMsg->usPrimId);
        return;
    }

    /* 将配置结果发送给PC侧 */
    OM_AppGreenChannel(OM_PRINTF_FUNC, usReturnPrimId,(VOS_UINT8*)&ulRet, sizeof(ulRet));

    return;
}
VOS_UINT32 OM_PrintfDataPut(VOS_CHAR *pucDataBuf, VOS_UINT32 ulDataLen)
{
    VOS_UINT32          ulFreeSize;
    VOS_UINT32          ulRet;
    VOS_UINT32          ulReturn = OM_OK;
    VOS_UINT32          ulBufAlignLen;
    VOS_CHAR            *pcWarning;

    /* OM Printf未初始化时，将打印输出到SHELL，打印时跳过模块ID四个字节 */
    if(VOS_FALSE == g_bIsPrintfInit)
    {
#if (VOS_OS_VER == VOS_LINUX)
        printk( "%s", pucDataBuf + OM_PRINTF_OFFSET);
#else
        printf( "%s", pucDataBuf + OM_PRINTF_OFFSET);
#endif
        return OM_OK;
    }

    g_stOmPrintfErrRecord.ulOmPrintfRecvLen += ulDataLen;
    g_stOmPrintfErrRecord.usOmPrintfRecvCount++;

    /* 获取buffer保护的信号量 */
    if(VOS_OK != VOS_SmP(g_ulOmPrintfBuffSem, 0))
    {
        g_ulPrintfErrType = OM_ERR_FAILTAKESEM;
        return OM_ERR_FAILTAKESEM;
    }

    /* 查看OM Printf ringbuffer 剩余空间 */
    ulFreeSize = (VOS_UINT32)OM_RingBufferFreeBytes(g_pstOmPrintfBufId);

    ulBufAlignLen = (ulDataLen+OM_ALIGNMENT)&(~OM_ALIGNMENT);

    /* 数据长度加四字节长度字段 */
    if ((ulBufAlignLen + sizeof(VOS_UINT32)) > ulFreeSize)
    {
       g_stOmPrintfErrRecord.ulOmPrintfNoPrintfBufferLostLen
                          += ((OM_PRINTF_BUFFER_SIZE - ulFreeSize) + ulDataLen);
       g_stOmPrintfErrRecord.usOmPrintfNoPrintfBufferLostCount++;
       g_ulPrintfErrType = OM_ERR_NOBUF;

       /* OM Printf缓冲区满时，清空ringbuffer，提示用户有数据丢失 */
       OM_RingBufferFlush(g_pstOmPrintfBufId);

       pcWarning = "OM_Printf:Warning!Some data lost!\n";

       *((VOS_UINT32*)pucDataBuf)     = WUEPS_PID_OM;
       *(((VOS_UINT32*)pucDataBuf)+1) = LOG_LEVEL_INFO;

       /* 跳过用于存放模块ID的前四字节 */
       VOS_StrCpy(pucDataBuf + OM_PRINTF_OFFSET, pcWarning );
       ulDataLen = VOS_StrLen(pcWarning)+OM_PRINTF_OFFSET;
       ulBufAlignLen = (ulDataLen+OM_ALIGNMENT)&(~OM_ALIGNMENT);

       ulReturn = OM_ERR_NOBUF;
    }

    /* 把长度字段放入OM Printf ringbuffer*/
    ulRet = (VOS_UINT32)OM_RingBufferPut(g_pstOmPrintfBufId, (VOS_CHAR *)&ulDataLen,
                                         (VOS_INT)sizeof(VOS_UINT32));
    if(sizeof(VOS_UINT32) != ulRet)
    {
        OM_RingBufferFlush(g_pstOmPrintfBufId);
        VOS_SmV(g_ulOmPrintfBuffSem);
        g_ulPrintfErrType = OM_ERR_FAILPUTDATA;
        return OM_ERR_FAILPUTDATA;
    }

    /* 把数据放入OM Printf ringbuffer*/
    ulRet = (VOS_UINT32)OM_RingBufferPut(g_pstOmPrintfBufId, pucDataBuf,(VOS_INT)ulBufAlignLen);
    if(ulBufAlignLen != ulRet)
    {
        OM_RingBufferFlush(g_pstOmPrintfBufId);
        VOS_SmV(g_ulOmPrintfBuffSem);
        g_ulPrintfErrType = OM_ERR_FAILPUTDATA;
        return OM_ERR_FAILPUTDATA;
    }

    VOS_SmV(g_ulOmPrintfBuffSem);

    /* 释放自处理任务信号量 */
    VOS_SmV(g_ulPrintfTaskSem);

    return ulReturn;

}
/*lint -e960 */
VOS_UINT32 OM_PrintfWithModule(VOS_UINT32 ulModuleId, VOS_UINT32 ulLevel, VOS_CHAR * pcformat, ... )
/*lint +e960 */
{
    VOS_UINT32          ulReturn = OM_OK;
    VOS_CHAR            *pcWarning;
    VOS_UINT32          ulTempLen;
    VOS_INT32           lRetLen;
    va_list             argument;
    VOS_UINT32          ulDataLen = 0;

    /* 数组前四字节存储模块ID，从第九字节开始为转换后字符串，为确保在转换为字符串
     过程中不越界，多定义四字节作保护 */
    /*lint -e813 */
    VOS_CHAR            acOutput[VOS_MAX_PRINT_LEN + 12];
    /*lint +e813 */

#if (VOS_OS_VER == VOS_LINUX)
    if(in_interrupt())
    {
        printk("\r\nOM_PrintfWithModule: RUN in the IRQ");

        return OM_ERR_RUNINIRQ;
    }
#endif

    /* 输入参数检查 */
    if((LOG_MAX_MODULE_ID_APP < ulModuleId)||(LOG_MIN_MODULE_ID_ACPU_DRV > ulModuleId)
                                       ||(LOG_LEVEL_BUTT <= ulLevel))
    {
        vos_printf("\r\nOM_PrintfWithModule: Para Error, ModuleId is %d, Level is %d", ulModuleId, ulLevel);
        return OM_ERR_UNVALIDPARA;
    }

    ulReturn = OM_PrintfGetModuleIdLev(ulModuleId);
    if((ulLevel > ulReturn)||(LOG_LEVEL_OFF == ulLevel))
    {
        return OM_ERR_LOWLEVEL;
    }

    *((VOS_UINT32*)acOutput) = ulModuleId;
    *(((VOS_UINT32*)acOutput)+1) = ulLevel;

    /* 将格式化字符串和可变参数转换为字符串 */
    va_start( argument, pcformat );
    lRetLen = VOS_nvsprintf(acOutput + OM_PRINTF_OFFSET, VOS_MAX_PRINT_LEN, pcformat, argument);
    va_end( argument );

    /* 添加字符串结束标志 */
    acOutput[VOS_MAX_PRINT_LEN + OM_PRINTF_OFFSET - 1] = '\0';

    /* 对转换结果进行判断，并在转换后字符串中添加相应提示信息 */
    if(lRetLen >= (VOS_MAX_PRINT_LEN - 1))
    {
        pcWarning = "OM_Printf: Warning!Print too long!!!";
        ulTempLen = VOS_StrLen(pcWarning );
        VOS_MemCpy(acOutput + OM_PRINTF_OFFSET, pcWarning, ulTempLen);

        /* 在转换后字符串倒数第二个字节添加换行符 */
        acOutput[VOS_MAX_PRINT_LEN + OM_PRINTF_OFFSET- 2] = '\n';
        ulDataLen = VOS_MAX_PRINT_LEN + OM_PRINTF_OFFSET- 1;
    }
    else if( lRetLen < 0 )
    {
        pcWarning = "OM_Printf:unknown internal error.\r\n";
        VOS_StrCpy(acOutput + OM_PRINTF_OFFSET, pcWarning );
        ulDataLen = VOS_StrLen(pcWarning ) + OM_PRINTF_OFFSET;
    }
    else
    {
        ulDataLen = (VOS_UINT32)lRetLen + OM_PRINTF_OFFSET;
    }

    ulReturn = OM_PrintfDataPut(acOutput, ulDataLen);

    return ulReturn;
}
/*lint -e960 */
VOS_UINT32 OM_Printf(VOS_CHAR * pcformat, ... )
/*lint +e960 */
{
    VOS_UINT32          ulReturn = OM_OK;
    VOS_CHAR            *pcWarning;
    VOS_UINT32          ulTempLen;
    VOS_INT32           lRetLen;
    va_list             argument;
    VOS_UINT32          ulDataLen = 0;

    /* 数组前四字节存储模块ID，接着四个直接存储打印机别，从第九字节开始为转换后字符串，为确保在转换为字符串
     过程中不越界，多定义四字节作保护 */
    /*lint -e813 */
    VOS_CHAR            acOutput[VOS_MAX_PRINT_LEN + 12];
    /*lint +e813 */

#if (VOS_OS_VER == VOS_LINUX)
    if(in_interrupt())
    {
        printk("\r\nOM_Printf: RUN in the IRQ");

        return OM_ERR_RUNINIRQ;
    }
#endif

    *((VOS_UINT32*)acOutput)     = ACPU_PID_OM;
    *(((VOS_UINT32*)acOutput)+1) = LOG_LEVEL_INFO;

    /* 将格式化字符串和可变参数转换为字符串 */
    va_start( argument, pcformat );
    lRetLen = VOS_nvsprintf(acOutput + OM_PRINTF_OFFSET, VOS_MAX_PRINT_LEN, pcformat, argument);
    va_end( argument );

    /* 添加字符串结束标志 */
    acOutput[VOS_MAX_PRINT_LEN + OM_PRINTF_OFFSET - 1] = '\0';

    /* 对转换结果进行判断，并在转换后字符串中添加相应提示信息 */
    if( lRetLen >= (VOS_MAX_PRINT_LEN - 1) )
    {
        pcWarning = "OM_Printf: Warning!Print too long!!!";
        ulTempLen = VOS_StrLen(pcWarning );
        VOS_MemCpy(acOutput + OM_PRINTF_OFFSET, pcWarning, ulTempLen);

        /* 在转换后字符串倒数第二个字节添加换行符 */
        acOutput[VOS_MAX_PRINT_LEN + OM_PRINTF_OFFSET- 2] = '\n';
        ulDataLen = VOS_MAX_PRINT_LEN + OM_PRINTF_OFFSET - 1;
    }
    else if( lRetLen < 0 )
    {
        pcWarning = "OM_Printf:unknown internal error.\r\n";
        VOS_StrCpy(acOutput + OM_PRINTF_OFFSET, pcWarning );
        ulDataLen = VOS_StrLen(pcWarning ) + OM_PRINTF_OFFSET;
    }
    else
    {
        ulDataLen = (VOS_UINT32)lRetLen + OM_PRINTF_OFFSET;
    }

    ulReturn = OM_PrintfDataPut(acOutput, ulDataLen);

    return ulReturn;
}

#if (VOS_OS_VER == VOS_LINUX)
EXPORT_SYMBOL(OM_Printf);
#endif


VOS_VOID OM_SndPrintfToOm(VOS_CHAR *pcData, VOS_UINT32 ulLength)
{
    VOS_UINT32          ulRet;
    OM_APP_TRACE_STRU   *pstAppMsg;

    /* 分配消息空间 */
    pstAppMsg = (OM_APP_TRACE_STRU*)VOS_MemAlloc(WUEPS_PID_OM,
                                         DYNAMIC_MEM_PT, ulLength + OM_APP_TRACE_LEN);
    if (VOS_NULL_PTR == pstAppMsg)
    {
        g_ulPrintfErrType = OM_ERR_MALLOC;
        return;
    }

    /* 拷贝消息内容 */
    VOS_MemCpy(pstAppMsg->aucPara, pcData, ulLength);

    /* 添加消息头部字段 */
    pstAppMsg->stAppHeader.ucFuncType   = OM_PRINTF_FUNC;
    pstAppMsg->stAppHeader.usLength     = ((VOS_UINT16)ulLength + OM_APP_TRACE_LEN)
                                                                - VOS_OM_HEADER_LEN;
    pstAppMsg->usPrimId                 = OM_APP_OM_PRINTF_IND;
    pstAppMsg->usToolId                 = 0;

    /* 添加消息序号和时间戳 */
    OM_AcpuAddSNTime(&(pstAppMsg->stAppHeader.ulSn), &(pstAppMsg->stAppHeader.ulTimeStamp));

    /* 调用OM API将数据放入OM链路 ringbuffer*/
    ulRet = OM_AcpuSendData((OM_RSP_PACKET_STRU*)pstAppMsg, (VOS_UINT16)(ulLength + OM_APP_TRACE_LEN));
    if (VOS_OK != ulRet)
    {
        g_ulPrintfErrType = OM_ERR_SENDTOOMBUF;
    }

    VOS_MemFree(WUEPS_PID_OM, pstAppMsg);

    return;
}
VOS_UINT32 OM_PrintfInit(VOS_VOID)
{
    OM_PORT_CFG_STRU                    stPortCfg;
    NAS_NVIM_SYSTEM_APP_CONFIG_STRU     stAPPConfig;

    stAPPConfig.usSysAppConfigType = SYSTEM_APP_BUTT;

    if(NV_OK != NV_Read(en_NV_Item_System_APP_Config, &stAPPConfig, sizeof(NAS_NVIM_SYSTEM_APP_CONFIG_STRU)))
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_AcpuSelfTaskReg:Read NV en_NV_Item_System_APP_Config fail!");
    }

    if(SYSTEM_APP_WEBUI != stAPPConfig.usSysAppConfigType)
    {
        return VOS_OK;
    }

    VOS_MemSet(&g_stOmPrintfErrRecord, 0, sizeof(OM_PRINTF_ERROR_RECORD_STRU));

    VOS_MemSet(g_astOmPrintfOutPutLev, LOG_LEVEL_OFF, OM_PRINTF_MODULE_MAX_NUM*sizeof(LOG_LEVEL_EN));

    /* 读NV项获取输出端口配置和文件大小，若失败则默认输出端口为SHELL口，
    文件大小为0表示取默认文件大小 */
    if(NV_OK != NV_Read(en_NV_Item_Om_Printf_Port, &stPortCfg, sizeof(OM_PORT_CFG_STRU)))
    {
        stPortCfg.ulMaxFileSize = 0;
    }
    else
    {
        /* 判断端口范围 */
        if (OM_OUTPUT_BUTT > stPortCfg.enPortType)
        {
            g_ulOmPrintfPort = (VOS_UINT32)stPortCfg.enPortType;
        }
    }

    /* 创建Printf BUFFER 保护信号量 */
    if(VOS_OK != VOS_SmMCreate("Pt", VOS_SEMA4_PRIOR | VOS_SEMA4_INVERSION_SAFE,
                                &g_ulOmPrintfBuffSem))
    {
        return VOS_ERR;
    }

    /*创建存储OM_Printf数据的环形缓冲区*/
    g_pstOmPrintfBufId = OM_RingBufferCreate(OM_PRINTF_BUFFER_SIZE);
    if (VOS_NULL_PTR == g_pstOmPrintfBufId)
    {
        VOS_SmDelete(g_ulOmPrintfBuffSem);
        return VOS_ERR;
    }

#if (VOS_OS_VER == VOS_LINUX)
    if (VOS_OK != OmPrintf_VFInit())
    {
        VOS_SmDelete(g_ulOmPrintfBuffSem);
        return VOS_ERR;
    }
#endif

    DRV_MSP_PROC_REG(OM_PRINTF_WITH_MODULE,     (BSP_MspProc)OM_PrintfWithModule);
    DRV_MSP_PROC_REG(OM_PRINTF,                 (BSP_MspProc)OM_Printf);
    DRV_MSP_PROC_REG(OM_PRINTF_GET_MODULE_IDLEV,(BSP_MspProc)OM_PrintfGetModuleIdLev);
    /* 模块初始化完后将全局变量置TRUE*/
    g_bIsPrintfInit = VOS_TRUE;

    return VOS_OK;
}


VOS_VOID OM_PrintfTask(VOS_VOID)
{
    VOS_UINT32          ulBufLen;
    VOS_UINT32          ulBufAlignLen;
#if (VOS_WIN32 == VOS_OS_VER)
    VOS_UINT32          i;
#endif

    /*lint -e813 */
    /* 前四字节用来存储模块ID */
    VOS_CHAR            acOutput[VOS_MAX_PRINT_LEN+8];
    /*lint +e813 */

    VOS_UINT32          ulRet;

    if ( VOS_OK != VOS_SmBCreate( "PRT", 0, VOS_SEMA4_FIFO, &g_ulPrintfTaskSem))
    {
        return;
    }

#if (VOS_WIN32 == VOS_OS_VER)
    for (i = 0; i < 1; i++)
#else
    for (;;)
#endif
    {

        if(VOS_OK != VOS_SmP(g_ulPrintfTaskSem, 0))
        {
            g_ulPrintfErrType = OM_ERR_FAILTAKETASKSEM;
            continue;
        }

        /* ringbuf中无数据 */
#if (VOS_WIN32 == VOS_OS_VER)
        for (i = 0; i < 1; i++)
#else
        while(VOS_FALSE == OM_RingBufferIsEmpty(g_pstOmPrintfBufId))
#endif
        {
            /* 获取数据长度 */
            ulRet = (VOS_UINT32)OM_RingBufferGet(g_pstOmPrintfBufId, (VOS_CHAR *)&ulBufLen,
                                                 sizeof(VOS_UINT32));
            if (sizeof(VOS_UINT32) != ulRet)
            {
                g_ulPrintfErrType = OM_ERR_FAILPUTDATA;
                OM_RingBufferFlush(g_pstOmPrintfBufId);
                continue;
            }

            /* ringbuf中数据损坏 */
            if (VOS_MAX_PRINT_LEN <= ulBufLen)
            {
                g_ulPrintfErrType = OM_ERR_DATADESTROY;
                OM_RingBufferFlush(g_pstOmPrintfBufId);
                continue;
            }

            ulBufAlignLen = (ulBufLen+OM_ALIGNMENT)&(~OM_ALIGNMENT);

            /* 获取数据内容 */
            ulRet = (VOS_UINT32)OM_RingBufferGet(g_pstOmPrintfBufId, (VOS_CHAR *)acOutput,
                                (VOS_INT)ulBufAlignLen);
            if (ulBufAlignLen != ulRet)
            {
                g_ulPrintfErrType = OM_ERR_FAILPUTDATA;
                OM_RingBufferFlush(g_pstOmPrintfBufId);
                continue;
            }

            /* 根据输出端口分发数据 */
            switch(g_ulOmPrintfPort)
            {
                case OM_OUTPUT_SHELL:
                    acOutput[ulBufLen] = 0;
#if (VOS_OS_VER == VOS_LINUX)
                    printk( "%s", acOutput + OM_PRINTF_OFFSET);
#else
                    printf( "%s", acOutput + OM_PRINTF_OFFSET);
#endif
                    break;

                case OM_OUTPUT_SDT:
                    OM_SndPrintfToOm(acOutput, ulBufLen);
                    break;

                default:
                    break;
            }
        }
    }
}

#if (VOS_OS_VER == VOS_LINUX)


VOS_UINT32 OmPrintf_VFInit(VOS_VOID)
{
    if(VOS_NULL_PTR == proc_create("OmPrintf", OM_VFILE_CRT_LEVEL, VOS_NULL_PTR, &g_stOmPrintf_FOPS))
    {
        LogPrint("\r\nOmPrintf_VFInit: proc_create Return NULL");

        return VOS_ERR;
    }

    if(VOS_NULL_PTR == proc_create("OmPrintfModule", OM_VFILE_CRT_LEVEL, VOS_NULL_PTR, &g_stOmPrintfModule_FOPS))
    {
        LogPrint("\r\nOmPrintfModule_VFInit: proc_create Return NULL");

        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 OmPrintfWriteData(VOS_UINT32 ulModuleID, VOS_UINT32 ulLevel, VOS_CHAR *pcStr, VOS_UINT32 ulStrLen)
{
    VOS_CHAR    *pcWarning;
    VOS_UINT32  ulTempLen;
    VOS_UINT32  ulDataLen;

    /* 数组前四字节存储模块ID，接着四个直接存储打印机别，从第九字节开始为转换后字符串，
    为确保在转换为字符串过程中不越界，多定义四字节作保护 */
    /*lint -e813 */
    VOS_CHAR    acOutput[VOS_MAX_PRINT_LEN + 12];
    /*lint +e813 */

    /*数据头部需要默认填写*/
    *((VOS_UINT32*)acOutput)     = ulModuleID;
    *(((VOS_UINT32*)acOutput)+1) = ulLevel;

    /* 添加字符串结束标志 */
    acOutput[VOS_MAX_PRINT_LEN + OM_PRINTF_OFFSET - 1] = '\0';

    /* 对输入进行判断，并在转换后字符串中添加相应提示信息 */
    if( ulStrLen >= (VOS_MAX_PRINT_LEN - 1) )
    {
        pcWarning = "OM_Printf: Warning!Print too long!!!";
        ulTempLen = VOS_StrLen(pcWarning);
        VOS_MemCpy(acOutput + OM_PRINTF_OFFSET, pcWarning, ulTempLen);

        /* 在转换后字符串倒数第二个字节添加换行符 */
        acOutput[VOS_MAX_PRINT_LEN + OM_PRINTF_OFFSET- 2] = '\n';
        ulDataLen = VOS_MAX_PRINT_LEN + OM_PRINTF_OFFSET - 1;
    }
    else
    {
        ulDataLen = (VOS_UINT32)ulStrLen + OM_PRINTF_OFFSET;
        if ( VOS_OK != copy_from_user(acOutput + OM_PRINTF_OFFSET, pcStr, ulStrLen) ) /*拷贝用户空间数据到内核空间上面*/
        {
            /* make Coverity happy*/
        }
    }

    OM_PrintfDataPut(acOutput, ulDataLen);

    return VOS_OK;
}



ssize_t OmPrintf_Write(struct file *file, const char __user *buf, size_t lLength, loff_t *ppos)
{
    if(in_interrupt())
    {
        printk("\r\nOmPrintf_Write: RUN in the IRQ");

        return VOS_OK;
    }

    return (ssize_t)OmPrintfWriteData(ACPU_PID_OM, LOG_LEVEL_INFO, (VOS_CHAR*)buf, (VOS_UINT32)lLength);
}



ssize_t OmPrintfModule_Write(struct file *file, const char __user *buf, size_t lLength, loff_t *ppos)
{
    OMPRITNF_MODULE_APP_STRU    *pstAppData;
    VOS_UINT32                  ulReturn;

    if(in_interrupt())
    {
        printk("\r\nOmPrintfModule_Write: RUN in the IRQ");

        return VOS_OK;
    }

    pstAppData = (OMPRITNF_MODULE_APP_STRU *)buf;

    /* 参数检查 */
    if((LOG_MAX_MODULE_ID_APP < pstAppData->ulModuleId)||(LOG_MIN_MODULE_ID_ACPU_DRV > pstAppData->ulModuleId)
                                       ||(LOG_LEVEL_BUTT <= pstAppData->ulPrintLev))
    {
        printk("\r\n OmPrintfModule_Write: The Para is Error, ModuleId is %d, PrintLev is %d", (VOS_INT)pstAppData->ulModuleId, (VOS_INT)pstAppData->ulPrintLev);
        return VOS_ERROR;
    }

    ulReturn = OM_PrintfGetModuleIdLev(pstAppData->ulModuleId);

    if((pstAppData->ulPrintLev > ulReturn)||(LOG_LEVEL_OFF == pstAppData->ulPrintLev))
    {
        return VOS_ERROR;
    }

    return (ssize_t)OmPrintfWriteData(pstAppData->ulModuleId, pstAppData->ulPrintLev, pstAppData->aucStr, pstAppData->ulStrLen);
}

#endif

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif



