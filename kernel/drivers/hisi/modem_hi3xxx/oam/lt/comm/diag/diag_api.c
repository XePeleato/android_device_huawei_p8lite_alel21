/******************************************************************************

   Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : diag_api.c
  Description     :
  History         :
     1.w00182550       2012-11-20   Draft Enact

******************************************************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*lint --e{10,718,746,628}*/

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
/*lint -save -e537*/
#include  "vos.h"
#include  "diag_common.h"
#include  "diag_buf_ctrl.h"
#include  "SOCPInterface.h"
#include  "diag_debug.h"
#include  "diag_api.h"
#include  "diag_cfg.h"
#include  "msp_diag.h"
#include  "msp_errno.h"
/*lint -restore*/

/*lint -save -e767 原因:Log打印*/
#define    THIS_FILE_ID        MSP_FILE_ID_DIAG_API_C
/*lint -restore +e767*/
/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/


VOS_UINT32 diag_CheckMsgPara(DIAG_AIR_MSG_LOG_STRU* pstRptMessage);
VOS_UINT32 diag_GetLayerMsgCfg(VOS_UINT32 ulCatId, VOS_UINT32 ulMsgId);
VOS_UINT32 diag_GetLayerSrcCfg(VOS_UINT32 ulModuleId);
VOS_UINT32 diag_GetLayerDstCfg(VOS_UINT32 ulModuleId);
VOS_UINT32 diag_GetLayerCfg(VOS_UINT32 ulSrcModuleId, VOS_UINT32 ulDstModuleId, VOS_UINT32 ulMsgId);
VOS_UINT32 diag_GetUserPlaneCfg(VOS_UINT32 ulMsgId);
VOS_UINT32 diag_GetPrintCfg(VOS_UINT32 ulModuleId, VOS_UINT32 ulLevel);
/*****************************************************************************
  3 Function
*****************************************************************************/

/*****************************************************************************
 Function Name   : diag_SocpPsLogPkt
 Description     : 外部API组SOCP包
 Input           :VOS_UINT8 * pstSocpPackt
                VOS_UINT8 ucMsgType
                VOS_UINT32 ulCmdParaLen
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-20  Draft Enact

*****************************************************************************/
/*lint -save -e958*/
VOS_VOID diag_SocpPsLogPkt(VOS_UINT8 * pstSocpPackt, VOS_UINT8 ucMsgType, VOS_UINT32 ulCmdParaLen)
{
    VOS_UINT64 ulTime = 0;

    /* 组SOCP包*/
    ulTime                           = diag_GetFrameTime();
    ((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->usSId              = MSP_SOCP_SID_DIAG_SERVER;
    ((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->usSSId             = MSP_SOCP_HIGH4BIT_SSID;    // 高四位
    ((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->ucServiceSessionId = MSP_SOCP_SERVER_SESSION_ID;
    ((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->ucMsgType          = MSP_SOCP_MSG_TYPE_IND;
    VOS_MemCpy(((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->aucTimeData, &ulTime, sizeof(VOS_UINT64));
    ((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->ulMsgTransId = (g_DiagLogPktNum.ulIndNum)++;

    /* 组包结构化ID(DIAG HEADER)*/
    ((MSP_DIAG_HEAD_STRU*)(pstSocpPackt + sizeof(MSP_SOCP_HEAD_STRU)))->ulDataSize = ulCmdParaLen;
    ((MSP_DIAG_HEAD_STRU*)(pstSocpPackt + sizeof(MSP_SOCP_HEAD_STRU)))->ulID =
        (MSP_STRU_ID_GET_28_31_FIELD(MSP_STRU_ID_28_31_GROUP_PS) | MSP_STRU_ID_GET_16_23_FIELD(ucMsgType));

}
/*lint -restore*/
/*****************************************************************************
 Function Name   : diag_SocpIndCmdPkt
 Description     : 主动上报命令组包接口
 Input           :MSP_SOCP_HEAD_STRU * pstSocpPackt
                VOS_UINT8 ucCmdType
                VOS_UINT32 ulCmdParaLen
                VOS_UINT16 ulCmdId
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-20  Draft Enact

*****************************************************************************/
/*lint -save -e958*/
VOS_VOID diag_SocpIndCmdPkt(VOS_UINT8 * pstSocpPackt, VOS_UINT8 ucCmdType, VOS_UINT32 ulCmdParaLen, VOS_UINT16 ulCmdId)
{
    VOS_UINT64 ulTime = 0;

    /* 组SOCP包*/
    ulTime                           = diag_GetFrameTime();
    ((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->usSId              = MSP_SOCP_SID_DIAG_SERVER;
    ((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->usSSId             = MSP_SOCP_HIGH4BIT_SSID;    // 高四位
    ((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->ucServiceSessionId = MSP_SOCP_SERVER_SESSION_ID;
    ((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->ucMsgType          = MSP_SOCP_MSG_TYPE_IND;
    VOS_MemCpy(((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->aucTimeData, &ulTime, sizeof(VOS_UINT64));
    ((MSP_SOCP_HEAD_STRU*)pstSocpPackt)->ulMsgTransId = (g_DiagLogPktNum.ulIndNum)++;

    /* 组包结构化ID(DIAG HEADER)*/
    ((MSP_DIAG_HEAD_STRU*)(pstSocpPackt + sizeof(MSP_SOCP_HEAD_STRU)))->ulDataSize = ulCmdParaLen;
    ((MSP_DIAG_HEAD_STRU*)(pstSocpPackt + sizeof(MSP_SOCP_HEAD_STRU)))->ulID =
        (MSP_STRU_ID_GET_28_31_FIELD(MSP_STRU_ID_28_31_GROUP_MSP) | MSP_STRU_ID_GET_16_23_FIELD(ucCmdType) \
        | MSP_STRU_ID_GET_0_15_FIELD(ulCmdId));

}
/*lint -restore*/
/*****************************************************************************
 Function Name   : diag_CheckMsgPara
 Description     : 检查输入参数的合法性
 Input           :DIAG_AIR_MSG_LOG_STRU* pstRptMessage
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-12-1  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_CheckMsgPara(DIAG_AIR_MSG_LOG_STRU* pstRptMessage)
{
    VOS_UINT32 ulRst = ERR_MSP_SUCCESS;

    /* 检查参数合法性*/
	if(NULL == pstRptMessage)
	{
		ulRst = ERR_MSP_INVALID_PARAMETER;
	}
	else if((pstRptMessage->ulDataSize == 0) && (NULL == pstRptMessage->pData))
	{
		ulRst = ERR_MSP_INVALID_PARAMETER;
        /*lint -save -e525*/
    }
    /*lint -restore*/
    return ulRst;
}

/*****************************************************************************
 Function Name   : diag_GetMsgCfg
 Description     : 获取层间消息过滤开关
 Input           :VOS_UINT32 ulCatId
                VOS_UINT32 ulMsgId
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-12-5  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_GetLayerMsgCfg(VOS_UINT32 ulCatId, VOS_UINT32 ulMsgId)
{
    DIAG_CFG_LOG_CAT_MSG_CFG_STRU *pstItemCfg =NULL;
    VOS_UINT32 i =  0;

    for(i = 0 ; i < g_stMsgCfg.ulCfgCnt; i++)
    {
       pstItemCfg = (g_stMsgCfg.astMsgCfgList + i);

       if(ulMsgId == pstItemCfg->ulId )
       {
           if(DIAG_CFG_SWT_CLOSE == pstItemCfg->ulSwt)
           {
              return ERR_MSP_CFG_LOG_NOT_ALLOW;
           }
           else if(DIAG_CFG_SWT_OPEN == pstItemCfg->ulSwt)
           {
              return ERR_MSP_SUCCESS;
           }
           else
           {
              return ERR_MSP_DIAG_MSG_CFG_NOT_SET;
           }
       }
    }

    return ERR_MSP_DIAG_MSG_CFG_NOT_SET;

}


VOS_UINT32 diag_GetLayerSrcCfg(VOS_UINT32 ulModuleId)
{
    /*lint -save -e40*/
    if(DOPRA_PID_TIMER == ulModuleId)
    {
        return ERR_MSP_SUCCESS;
    }
    /*lint -save -e539*/
	if(DIAG_CFG_LAYER_MODULE_IS_VALID((VOS_INT32)ulModuleId))
	{
   /*lint -restore*/
		if(DIAG_CFG_SWT_OPEN == g_LayerSrcModuleCfg[ulModuleId])
		{
			return	ERR_MSP_SUCCESS;
		}
	}
	/*lint -restore +e40*/
	return ERR_MSP_CFG_LOG_NOT_ALLOW;
}

VOS_UINT32 diag_GetLayerDstCfg(VOS_UINT32 ulModuleId)
{
        /*lint -save -e40*/
	if(DIAG_CFG_LAYER_MODULE_IS_VALID((VOS_INT32)ulModuleId))
	{
		if(DIAG_CFG_SWT_OPEN == g_LayerDecModuleCfg[ulModuleId])
		{
			return	ERR_MSP_SUCCESS;
		}
	}
	  /*lint -restore +e40*/
	return ERR_MSP_CFG_LOG_NOT_ALLOW;
}

/*****************************************************************************
 Function Name   : diag_GetLayerCfg
 Description     : 获取层间消息开关状态
 Input           :VOS_UINT32 ulSrcModuleId
                VOS_UINT32 ulDstModuleId
                VOS_UINT32 ulMsgId
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-12-5  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_GetLayerCfg(VOS_UINT32 ulSrcModuleId, VOS_UINT32 ulDstModuleId, VOS_UINT32 ulMsgId)
{
    VOS_UINT32 ret = ERR_MSP_CFG_LOG_NOT_ALLOW;
    VOS_UINT32 ulMsgCfg = ERR_MSP_DIAG_MSG_CFG_NOT_SET;

    /*先判断消息ID过滤功能是否打开，如果有，则根据消息ID过滤*/
    ulMsgCfg = diag_GetLayerMsgCfg(DIAG_CMD_LOG_CATETORY_LAYER_ID, ulMsgId);
    if(ERR_MSP_DIAG_MSG_CFG_NOT_SET == ulMsgCfg )
    {
    	ret = diag_GetLayerSrcCfg(ulSrcModuleId);
		ret |= diag_GetLayerDstCfg(ulDstModuleId);
		return ret;
    }
    else if(ERR_MSP_SUCCESS == ulMsgCfg )
    {
        return ERR_MSP_SUCCESS;
    }
    else
    {
        return ERR_MSP_CFG_LOG_NOT_ALLOW;
    }
}

/*****************************************************************************
 Function Name   : diag_GetUserPlaneCfg
 Description     : 获取用户面消息开关状态
 Input           :VOS_UINT32 ulMsgId
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-12-5  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_GetUserPlaneCfg(VOS_UINT32 ulMsgId)
{
    VOS_UINT32 i = 0;

    for(i=0;i<DIAG_CFG_USER_PLANE_ID_NUM;i++)
    {
        if(ulMsgId == g_stUserPlaneCfg[i].ulMsgId)
        {
            if(DIAG_CFG_SWT_OPEN == g_stUserPlaneCfg[i].ulSwitch)
            {
                return ERR_MSP_SUCCESS;
            }
            else if(DIAG_CFG_SWT_CLOSE == g_stUserPlaneCfg[i].ulSwitch)
            {
                return ERR_MSP_CFG_LOG_NOT_ALLOW;
            }
            else
            {
                return ERR_MSP_FAILURE;
            }
        }
    }

    return ERR_MSP_CFG_LOG_NOT_ALLOW;
}

/*****************************************************************************
 Function Name   : diag_GetPrintCfg
 Description     : 获取打印开关状态
 Input           :VOS_UINT32 ulModuleId
                VOS_UINT32 ulLevel
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-12-6  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_GetPrintCfg(VOS_UINT32 ulModuleId, VOS_UINT32 ulLevel)
{
    VOS_UINT32 ulLevelFilter = 0;

    /*将协议栈的LEVEL值转换成MSP与HSO之间的LEVEL值*/
    /*0x80000000<->0;0x40000000<->1;0x20000000<->2;0x10<->3;0x08000000<->4*/
	if((PS_LOG_LEVEL_OFF == ulLevel)||(ulLevel >= PS_LOG_LEVEL_BUTT))
	{
		return ERR_MSP_CFG_LOG_NOT_ALLOW;
	}

    ulLevelFilter = ((VOS_UINT32)1<<(7-ulLevel));

    /* 打印总开关模块打开情况*/
    if(DIAG_CFG_PRINT_TOTAL_MODULE_SWT_NOT_USE != g_PrintTotalCfg)
    {
        if(ulLevelFilter & g_PrintTotalCfg)
        {
            return ERR_MSP_SUCCESS;
        }
        else
        {
            return ERR_MSP_CFG_LOG_NOT_ALLOW;
        }
    }
    else
    {
        /*lint -save -e40*/
        /*获取模块打印开关状态*/
        if(DIAG_CFG_MODULE_IS_INVALID((VOS_INT32)ulModuleId))
        {
            return  ERR_MSP_CFG_LOG_NOT_ALLOW;
        }

        if(ulLevelFilter & g_PrintModuleCfg[ulModuleId - VOS_PID_DOPRAEND])
        {
            return ERR_MSP_SUCCESS;
        }
        else
        {
            return ERR_MSP_CFG_LOG_NOT_ALLOW;
        }
        /*lint -restore +e40*/
    }

}


#define LTE_DIAG_PRINTF_LEN             (256+sizeof(VOS_UINT32)+sizeof(VOS_UINT32))

#define LTE_LOG_AFFIRM(c) \
do{ \
    if(!(c)) \
    { \
        vos_printf("LTE_LOG_ASSERT %d.\n", __LINE__); \
        return -1; \
    } \
}while(0)
#if 0

/******************************************************************************
函数名称: LTE_DIAG_LOG
功能描述: 打印点类型的打印接口函数
参数说明:
            ulModuleId[in]  : 模块id，对应PID
            ulSubModId[in]  : 子模块id
            ulLevel[in]     : 打印级别
            ulLogId[in]     : 由文件号和行号根据DIAG_LOG_ID生成
            amount[in]      : 可变参数个数（不包括ulModuleId/ulLevel/ulLogId/amout）
            ...             : 可变参数
调用约束:
            1. 绝对禁止对此函数进行二次封装，只能转定义
            2. 支持可变的参数个数，但必须在调用时由参数amount指定参数个数
            3. 可变参数只支持int类型
            4. 目前版本中支持的最大参数个数是6个，超过的参数默认丢弃
******************************************************************************/
VOS_INT32 LTE_DIAG_LOG(VOS_UINT32 ulModuleId, VOS_UINT32 ulSubModId,
                        VOS_UINT32 ulLevel, VOS_UINT32 ulLogId,
                        VOS_UINT32 amount, ...)
{
    VOS_UINT32 ulRst          = ERR_MSP_SUCCESS;
    VOS_UINT32 ulParaBufSize  = 0;
    VOS_UINT8* pstSocpPackt   = NULL;
    DIAG_CMD_LOG_PRINT_RAW_TXT_IND_STRU* pRptInfo = NULL;
    VOS_UINT32 ulDataLength =0;
    VOS_CHAR ucDiagPrintData[LTE_DIAG_PRINTF_LEN]={0};
    VOS_INT32 val, i, lTmpLen, lSpareLen, lOccupyLen;
    va_list vl;
    VOS_UINT32 paranum;

    /* 只记录调用次数*/
    DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_DOT, ulLogId, 0, 0);

    /*检查DIAG是否初始化且HSO是否连接上*/
    if(!DIAG_IS_CONN_ON)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_DOT_ERR, ERR_MSP_NO_INITILIZATION, ulLogId, 1);
        return ERR_MSP_NO_INITILIZATION;
    }

    ulRst = diag_GetPrintCfg(ulModuleId, ulLevel);
    if(ERR_MSP_SUCCESS != ulRst)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_DOT_ERR, ERR_MSP_UNAVAILABLE, ulLogId, 2);
        return ERR_MSP_UNAVAILABLE;
    }

    *((VOS_UINT32*)ucDiagPrintData) = ulModuleId;
    *(VOS_UINT32*)(ucDiagPrintData + sizeof(VOS_UINT32)) = ulLevel;

    lOccupyLen = 8;
    /*lint -save -e713 -e732*/
    lSpareLen  = LTE_DIAG_PRINTF_LEN - lOccupyLen;

    lTmpLen    = VOS_nsprintf(ucDiagPrintData + lOccupyLen, lSpareLen, "%d, ", ulLogId);
    LTE_LOG_AFFIRM(0 <= lTmpLen);

    lOccupyLen += lTmpLen;
    LTE_LOG_AFFIRM(LTE_DIAG_PRINTF_LEN >= (VOS_UINT32)lOccupyLen);
    lSpareLen  = LTE_DIAG_PRINTF_LEN - lOccupyLen;

    lTmpLen    = VOS_nsprintf(ucDiagPrintData + lOccupyLen, lSpareLen, "%u, ", VOS_GetTick());
    LTE_LOG_AFFIRM(0 <= lTmpLen);

    lOccupyLen += lTmpLen;
    LTE_LOG_AFFIRM(LTE_DIAG_PRINTF_LEN >= (VOS_UINT32)lOccupyLen);
    lSpareLen  = LTE_DIAG_PRINTF_LEN - lOccupyLen;

    va_start(vl, amount);/*lint !e530*/
    paranum = (amount > LTE_DIAG_PRINTF_PARAM_MAX_NUM) ? LTE_DIAG_PRINTF_PARAM_MAX_NUM : amount;

    for(i = 0; i < (VOS_INT32)paranum; i++)
    {
        val = va_arg(vl, int);
        /* coverity[overrun-local] */
        lTmpLen = VOS_nsprintf(ucDiagPrintData + lOccupyLen, lSpareLen, "%d, ", val);
        if(0 > lTmpLen)
        {
            DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_DOT_ERR, ERR_MSP_TOO_SMALL_BUFFER, lSpareLen, 3);
            va_end(vl);
            return -1;
        }

        lOccupyLen += lTmpLen;
        /* coverity[cond_at_most] */
        if(LTE_DIAG_PRINTF_LEN < (VOS_UINT32)lOccupyLen)
        {
            DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_DOT_ERR, ERR_MSP_TOO_SMALL_BUFFER, lOccupyLen, 4);
            va_end(vl);
            return -1;
        }
        lSpareLen  = LTE_DIAG_PRINTF_LEN - lOccupyLen;
    }
    va_end(vl);

    ulDataLength = lOccupyLen;

    /*申请编码源BUFFER*/
    diag_LockCoderSrcBuf();
    ulParaBufSize = sizeof(DIAG_CMD_LOG_PRINT_RAW_TXT_IND_STRU) + ulDataLength;
    pstSocpPackt = diag_GetCoderSrcBuf(sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU) +
        ulParaBufSize, SOCP_CODER_SRC_LOM_NORMAL_IND);
    if (NULL == pstSocpPackt)
    {
        diag_UnLockCoderSrcBuf();
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_DOT_ERR, ERR_MSP_NOT_ENOUGH_MEMORY, 0, 5);
        return ERR_MSP_NOT_ENOUGH_MEMORY;
    }

    /*组装SOCP包*/
    diag_SocpPsLogPkt(pstSocpPackt, MSP_STRU_ID_16_23_TYPE_DOT, ulParaBufSize);
    pRptInfo = (DIAG_CMD_LOG_PRINT_RAW_TXT_IND_STRU*)(pstSocpPackt + sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU));

    /*组装DIAG命令参数*/
    pRptInfo->ulModule = ulModuleId;
    pRptInfo->ulNo = (g_DiagLogPktNum.ulPrintTxtNum)++;
    pRptInfo->ulLevel  = ulLevel;
    VOS_MemCpy(pRptInfo->szText, ucDiagPrintData, ulDataLength);

    /*写入数据到编码源BUFFER*/
    ulRst = diag_SendCoderSrcToSocp(pstSocpPackt,sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU) +
            ulParaBufSize, SOCP_CODER_SRC_LOM_NORMAL_IND);
    if(ulRst != ERR_MSP_SUCCESS)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_DOT_ERR, ERR_MSP_NOT_FOUND, ulRst, 6);
    }
    diag_UnLockCoderSrcBuf();

	return ulRst;
    /*lint -restore*/
}
#endif

VOS_UINT32 DIAG_PrintfV(VOS_UINT32 id, VOS_CHAR* pszFileName, VOS_UINT32 ulLineNum, VOS_CHAR* pszFmt, ...)
{
	VOS_UINT32 ulRst          = ERR_MSP_SUCCESS;
	VOS_UINT32 ulParaBufSize  = 0;                   // 参数Buf Size
	VOS_UINT32 ulModuleId     = 0;
    VOS_UINT32 ulLevelFilter  = 0;
	VOS_UINT32 ulPrintLevel = 0;
    /*lint -save -e40 -e530*/
    va_list arg;
    VOS_INT32 ulParamLen =0;
    VOS_UINT8* pstSocpPackt = NULL;
    DIAG_CMD_LOG_PRINT_RAW_TXT_IND_STRU* pRptInfo = NULL;
    VOS_UINT32 ulDataLength =0;
	VOS_CHAR ucDiagPrintData[DIAG_PRINTF_MAX_LEN+1]={0};

    /* 只记录调用次数*/
    DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_PRINTFV, id, 0, 0);

    /*检查DIAG是否初始化且HSO是否连接上*/
    if(!DIAG_IS_CONN_ON)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_PRINTFV_ERR, ERR_MSP_NO_INITILIZATION, id, 1);
        return ERR_MSP_NO_INITILIZATION;
    }

    /*获取数组下标*/
    ulModuleId = DIAG_GET_MODULE_ID(id);

    /*获取模块开关状态*/
    ulLevelFilter = DIAG_GET_PRINTF_LEVEL(id);
#if 1
    ulRst = diag_GetPrintCfg(ulModuleId,ulLevelFilter);
    if(ERR_MSP_SUCCESS != ulRst)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_PRINTFV_ERR, ERR_MSP_UNAVAILABLE, id, 2);
        return ERR_MSP_UNAVAILABLE;
    }
#endif

    /*给HSO的打印字符串形式如下:pszFileName[ulLineNum]data。HSO根据中括号[]去截取相应的信息*/
    /*lint -save -e539*/
	ulParamLen = VOS_nsprintf(ucDiagPrintData,DIAG_PRINTF_MAX_LEN,"%s[%d]",pszFileName,ulLineNum);
    /*lint -restore*/
    /* coverity[cond_at_least] */
    if(ulParamLen > DIAG_PRINTF_MAX_LEN)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_PRINTFV_ERR, ERR_MSP_FAILURE, id, 11);
    }

	/*ulParamLen 未包含\0*/
    va_start(arg, pszFmt);/*lint !e516*/
    /* coverity[overrun-local] */
    ulParamLen = VOS_nvsprintf(ucDiagPrintData + ulParamLen,\
				 DIAG_PRINTF_MAX_LEN - (VOS_UINT32)ulParamLen,(const VOS_CHAR *) pszFmt, arg);
    if(ulParamLen > DIAG_PRINTF_MAX_LEN)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_PRINTFV_ERR, ERR_MSP_FAILURE, id, 12);
    }
    va_end(arg);
    /*lint -restore +e40 +e530*/
	ucDiagPrintData[DIAG_PRINTF_MAX_LEN - 1] = '\0';
	ulDataLength = VOS_StrLen(ucDiagPrintData);

    /*申请编码源BUFFER*/
    diag_LockCoderSrcBuf();
    ulParaBufSize = sizeof(DIAG_CMD_LOG_PRINT_RAW_TXT_IND_STRU) + ulDataLength;
    pstSocpPackt = diag_GetCoderSrcBuf(sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU) +
        ulParaBufSize, SOCP_CODER_SRC_LOM_NORMAL_IND);
    if (NULL == pstSocpPackt)
    {
        diag_UnLockCoderSrcBuf();
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_PRINTFV_ERR, ERR_MSP_NOT_ENOUGH_MEMORY, 0, 3);
        return ERR_MSP_NOT_ENOUGH_MEMORY;
    }

    /*组装SOCP包*/
    diag_SocpPsLogPkt(pstSocpPackt, MSP_STRU_ID_16_23_TYPE_PRINT, ulParaBufSize);
    pRptInfo = (DIAG_CMD_LOG_PRINT_RAW_TXT_IND_STRU*)(pstSocpPackt + sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU));

    /*组装DIAG命令参数*/
    pRptInfo->ulModule = ulModuleId;
    pRptInfo->ulNo = (g_DiagLogPktNum.ulPrintTxtNum)++;
    pRptInfo->ulLevel  = ulPrintLevel;
    VOS_MemCpy(pRptInfo->szText, ucDiagPrintData, ulDataLength);

    /*写入数据到编码源BUFFER*/
    ulRst = diag_SendCoderSrcToSocp(pstSocpPackt,sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU) +
            ulParaBufSize, SOCP_CODER_SRC_LOM_NORMAL_IND);
    if(ulRst != ERR_MSP_SUCCESS)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_PRINTFV_ERR, ulRst, 0, 4);
    }
    diag_UnLockCoderSrcBuf();

	return ulRst;
}

/*****************************************************************************
 Function Name   : DIAG_ReportCommand
 Description     : MSP内部命令上报接口，给MSP使用
 Input           :VOS_UINT16 ulID
                VOS_UINT32 ulDataSize
                VOS_VOID* pData
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-20  Draft Enact

*****************************************************************************/
/*lint -save -e958*/
VOS_UINT32 DIAG_ReportCommand(VOS_UINT16 ulID, VOS_UINT32 ulDataSize, VOS_VOID* pData)
{
    VOS_UINT32 ulRst         = ERR_MSP_SUCCESS;
    VOS_UINT8* pstSocpPackt     = NULL;

    /* 只记录调用次数*/
    DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportStatLog, ulID, g_ulDiagCfgInfo, 0);

    /*检查DIAG是否初始化且HSO是否连接上*/
    if(!DIAG_IS_CONN_ON)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportStatLog_ERR, ulDataSize, ulID, 2);
        return ERR_MSP_NO_INITILIZATION;
    }

    /*检查参数合法性*/
    if((NULL == pData) ||(0 == ulDataSize) || (ulDataSize > DIAG_CMD_DATA_MAX_LEN))
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportStatLog_ERR, ulDataSize, ulID, 1);

        return ERR_MSP_INVALID_PARAMETER;
    }

    /*申请编码源BUFFER*/
    diag_LockCoderSrcBuf();
    pstSocpPackt = diag_GetCoderSrcBuf(sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU) +
        ulDataSize, SOCP_CODER_SRC_LOM_PRECEDENCE_IND);

    if (NULL == pstSocpPackt)
    {
        diag_UnLockCoderSrcBuf();
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportStatLog_ERR, ERR_MSP_NOT_ENOUGH_MEMORY, ulID, 6);
        return ERR_MSP_NOT_ENOUGH_MEMORY;
    }

    /*组装SOCP包*/
    diag_SocpIndCmdPkt(pstSocpPackt, MSP_STRU_ID_16_23_IND_MSP, ulDataSize, ulID);

    VOS_MemCpy(((MSP_DIAG_HEAD_STRU*)(pstSocpPackt+sizeof(MSP_SOCP_HEAD_STRU)))->ucData,pData, ulDataSize);

    /*写入数据到编码源BUFFER*/
    ulRst = diag_SendCoderSrcToSocp(pstSocpPackt,sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU) +ulDataSize, \
            SOCP_CODER_SRC_LOM_PRECEDENCE_IND);
    if(ulRst != ERR_MSP_SUCCESS)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportStatLog_ERR, ulDataSize, 0, 7);
    }
    diag_UnLockCoderSrcBuf();
    return ulRst;
}
/*lint -restore*/
/*****************************************************************************
 Function Name   : DIAG_ReportEventLog
 Description     : 事件上报接口，给PS使用
 Input           :VOS_UINT32 id
                VOS_UINT32 ulEventID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-20  Draft Enact

*****************************************************************************/
VOS_UINT32 DIAG_ReportEventLog(VOS_UINT32 id, VOS_UINT32 ulEventID)
{
	VOS_UINT32 ulRst                                = ERR_MSP_SUCCESS;
	VOS_UINT32 ulParaBufSize                        = 0;

    VOS_UINT8* pstSocpPackt           = NULL;
    DIAG_CMD_LOG_EVENT_IND_STRU *pEventIndInfo = NULL;

    /* 只记录调用次数*/
    DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportEventLog, id, ulEventID, 0);


    /*检查是否允许事件上报*/
    if(!DIAG_IS_EVENT_ON)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportEventLog_ERR, ulRst, id, 1);
        return ERR_MSP_NO_INITILIZATION;
    }

	/*申请编码源BUFFER*/
    diag_LockCoderSrcBuf();
    ulParaBufSize = sizeof(DIAG_CMD_LOG_EVENT_IND_STRU);
    pstSocpPackt = diag_GetCoderSrcBuf(sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU) +
        ulParaBufSize, SOCP_CODER_SRC_LOM_PRECEDENCE_IND);

    if (NULL == pstSocpPackt)
    {
        diag_UnLockCoderSrcBuf();
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportEventLog_ERR, ERR_MSP_NOT_ENOUGH_MEMORY, id, 4);
        return ERR_MSP_NOT_ENOUGH_MEMORY;

    }
    /*组装SOCP包*/
    diag_SocpPsLogPkt(pstSocpPackt, MSP_STRU_ID_16_23_TYPE_EVENT, ulParaBufSize);

    /*组装DIAG命令参数*/
    pEventIndInfo = (DIAG_CMD_LOG_EVENT_IND_STRU *)(pstSocpPackt + sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU));
    pEventIndInfo->ulNo = (g_DiagLogPktNum.ulEventNum)++;
    pEventIndInfo->ulId = ulEventID;

    /*写入数据到编码源BUFFER*/
    ulRst = diag_SendCoderSrcToSocp(pstSocpPackt,sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU) +ulParaBufSize,
            SOCP_CODER_SRC_LOM_PRECEDENCE_IND);
    if(ulRst != ERR_MSP_SUCCESS)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportEventLog_ERR, ulRst, 0, 7);
    }
    diag_UnLockCoderSrcBuf();

    return ulRst;
}

/*****************************************************************************
 Function Name   : DIAG_ReportAirMessageLog
 Description     : LTE和TDS空口上报接口，给PS使用
 Input           :DIAG_AIR_MSG_LOG_STRU* pRptMessage
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-20  Draft Enact

*****************************************************************************/
VOS_UINT32 DIAG_ReportAirMessageLog(DIAG_AIR_MSG_LOG_STRU* pRptMessage)
{
    VOS_UINT32 ulRst         = ERR_MSP_SUCCESS;
    VOS_UINT32 ulParaBufSize = 0;
    DIAG_CMD_LOG_AIR_IND_STRU* pRptInfo = NULL;
    VOS_UINT8* pstSocpPackt = NULL;

    /* 只记录调用次数*/
    DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportAirMessageLog, g_ulDiagCfgInfo, 0, 0);

    /*检查是否允许LT 空口上报*/
    if (!DIAG_IS_LT_AIR_ON)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportAirMessageLog_ERR, ulRst, pRptMessage->ulId, 2);
        return ERR_MSP_NO_INITILIZATION;
    }


    /*检查参数合法性*/
    ulRst = diag_CheckMsgPara(pRptMessage);

    if (ERR_MSP_SUCCESS != ulRst)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportAirMessageLog_ERR, ulRst, (NULL != pRptMessage ? pRptMessage->ulId : 0), 1);
        return ulRst;
    }

    /*申请编码源BUFFER*/
    diag_LockCoderSrcBuf();
    ulParaBufSize = sizeof(DIAG_CMD_LOG_AIR_IND_STRU) + pRptMessage->ulDataSize;
    pstSocpPackt = diag_GetCoderSrcBuf(sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU) +
        ulParaBufSize, SOCP_CODER_SRC_LOM_PRECEDENCE_IND);

    if (NULL == pstSocpPackt)
    {
        diag_UnLockCoderSrcBuf();
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportAirMessageLog_ERR, ulParaBufSize, pRptMessage->ulId, 6);
        return ERR_MSP_NOT_ENOUGH_MEMORY;
    }

    /*组装SOCP包*/
    diag_SocpPsLogPkt(pstSocpPackt, MSP_STRU_ID_16_23_TYPE_AIR, ulParaBufSize);
    pRptInfo = (DIAG_CMD_LOG_AIR_IND_STRU*)(pstSocpPackt + sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU));

    /*组装DIAG命令参数*/
    pRptInfo->ulId     = pRptMessage->ulMessageID;
    pRptInfo->ulSide   = pRptMessage->ulSideId;
    pRptInfo->ulModule = DIAG_GET_MODULE_ID(pRptMessage->ulId);
    pRptInfo->ulNo = (g_DiagLogPktNum.ulAirNum)++;
    VOS_MemCpy(pRptInfo->aucDta, pRptMessage->pData, pRptMessage->ulDataSize);

    /*写入数据到编码源BUFFER*/
    ulRst = diag_SendCoderSrcToSocp(pstSocpPackt,sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU) +
            ulParaBufSize, SOCP_CODER_SRC_LOM_PRECEDENCE_IND);
    if(ulRst != ERR_MSP_SUCCESS)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportAirMessageLog_ERR, ulRst, pRptMessage->ulId, 7);
    }
    diag_UnLockCoderSrcBuf();
	return ulRst;
}

/*****************************************************************************
 Function Name   : DIAG_ReportVoLTELog
 Description     : VoLTE消息上报接口
 Input           : DIAG_VoLTE_LOG_STRU* pRptMessage
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.fuxin      2013-12-30  create
*****************************************************************************/
VOS_UINT32 DIAG_ReportVoLTELog(DIAG_VoLTE_LOG_STRU* pRptMessage)
{
    VOS_UINT32 ulRst         = ERR_MSP_SUCCESS;
    VOS_UINT32 ulParaBufSize = 0;
    VOS_UINT32 ulModuleId;
    DIAG_CMD_LOG_VoLTE_IND_STRU* pRptInfo = NULL;
    VOS_UINT8* pstSocpPackt = NULL;

    /* 只记录调用次数*/
    DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportVoLTELog_ERR, g_ulDiagCfgInfo, 0, 0);

    /*检查DIAG是否初始化且HSO是否连接上*/
    if(!DIAG_IS_CONN_ON)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportVoLTELog_ERR, ulRst, pRptMessage->ulId, 2);
        return ERR_MSP_NO_INITILIZATION;
    }

    /*检查参数合法性*/
    ulRst = diag_CheckMsgPara((DIAG_AIR_MSG_LOG_STRU *)pRptMessage);

    if (ERR_MSP_SUCCESS != ulRst)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportVoLTELog_ERR, ulRst, (NULL != pRptMessage ? pRptMessage->ulId : 0), 1);
        return ulRst;
    }

    ulModuleId  = DIAG_GET_MODULE_ID(pRptMessage->ulId);
    ulRst = diag_GetLayerCfg(ulModuleId, ulModuleId, pRptMessage->ulMessageID);

    if(ERR_MSP_SUCCESS != ulRst)
    {
        ulRst = ERR_MSP_CFG_LOG_NOT_ALLOW;
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportVoLTELog_ERR, ulRst, pRptMessage->ulId, 4);
	  return ulRst;
    }

    /*申请编码源BUFFER*/
    diag_LockCoderSrcBuf();
    ulParaBufSize = sizeof(DIAG_CMD_LOG_VoLTE_IND_STRU) + pRptMessage->ulDataSize;
    pstSocpPackt = diag_GetCoderSrcBuf(sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU) +
        ulParaBufSize, SOCP_CODER_SRC_LOM_PRECEDENCE_IND);

    if (NULL == pstSocpPackt)
    {
        diag_UnLockCoderSrcBuf();
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportVoLTELog_ERR, ulParaBufSize, pRptMessage->ulId, 6);
        return ERR_MSP_NOT_ENOUGH_MEMORY;
    }

    /*组装SOCP包*/
    diag_SocpPsLogPkt(pstSocpPackt, MSP_STRU_ID_16_23_VoLTE, ulParaBufSize);
    pRptInfo = (DIAG_CMD_LOG_VoLTE_IND_STRU *)(pstSocpPackt + sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU));

    /*组装DIAG命令参数*/
    pRptInfo->ulId     = pRptMessage->ulMessageID;
    pRptInfo->ulSide   = pRptMessage->ulSideId;
    pRptInfo->ulModule = DIAG_GET_MODULE_ID(pRptMessage->ulId);
    pRptInfo->ulNo = (g_DiagLogPktNum.ulVoLTENum)++;
    VOS_MemCpy(pRptInfo->aucDta, pRptMessage->pData, pRptMessage->ulDataSize);

    /*写入数据到编码源BUFFER*/
    ulRst = diag_SendCoderSrcToSocp(pstSocpPackt,sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU) +
            ulParaBufSize, SOCP_CODER_SRC_LOM_PRECEDENCE_IND);
    if(ulRst != ERR_MSP_SUCCESS)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportVoLTELog_ERR, ulRst, pRptMessage->ulId, 7);
    }
    diag_UnLockCoderSrcBuf();

	return ulRst;
}
/*****************************************************************************
 Function Name   : DIAG_ReportLayerMessageLog
 Description     : 层间消息上报接口，给PS使用
 Input           :DIAG_LAYER_MSG_STRU* pRptMessage
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-20  Draft Enact

*****************************************************************************/
VOS_UINT32 DIAG_ReportLayerMessageLog(DIAG_LAYER_MSG_STRU* pRptMessage)
{
    VOS_UINT32 ulRst         = ERR_MSP_SUCCESS;
    VOS_UINT32 ulParaBufSize = 0;                   // 参数Buf Size
    VOS_UINT32 ulModuleId;
    VOS_UINT8* pstSocpPackt           = NULL;
    DIAG_CMD_LOG_LAYER_IND_STRU *pLayerIndInfo = NULL;

    /* 只记录调用次数*/
    DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportLayerMessageLog, 0, 0, 0);

    /*检查DIAG是否初始化且HSO是否连接上*/
    if(!DIAG_IS_CONN_ON)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportLayerMessageLog_ERR, ulRst, pRptMessage->ulId, 2);
        return ERR_MSP_NO_INITILIZATION;
    }

    /*检查参数合法性*/
    ulRst = diag_CheckMsgPara((DIAG_AIR_MSG_LOG_STRU*)pRptMessage);
    if(ERR_MSP_SUCCESS != ulRst)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportLayerMessageLog_ERR,ulRst,(NULL != pRptMessage ? pRptMessage->ulId : 0),1);
        return ulRst;
    }

    ulModuleId  = DIAG_GET_MODULE_ID(pRptMessage->ulId);
    /*检查是否允许层间消息上报*/
    ulRst = diag_GetLayerCfg(ulModuleId, pRptMessage->ulDestMod, pRptMessage->ulMessageID);

    if(ERR_MSP_SUCCESS != ulRst)
    {
        ulRst = ERR_MSP_CFG_LOG_NOT_ALLOW;
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportLayerMessageLog_ERR, ulRst, pRptMessage->ulId, 4);
	  return ulRst;
    }

    /*申请编码源BUFFER*/
    diag_LockCoderSrcBuf();
    ulParaBufSize  = sizeof(DIAG_CMD_LOG_LAYER_IND_STRU) + pRptMessage->ulDataSize;
    pstSocpPackt = diag_GetCoderSrcBuf(sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU) +
        ulParaBufSize, SOCP_CODER_SRC_LOM_PRECEDENCE_IND);

    if (NULL == pstSocpPackt)
    {
        diag_UnLockCoderSrcBuf();
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportLayerMessageLog_ERR, ERR_MSP_NOT_ENOUGH_MEMORY, pRptMessage->ulId, 5);
	    return ERR_MSP_NOT_ENOUGH_MEMORY;
    }

    /*组装SOCP包*/
    diag_SocpPsLogPkt(pstSocpPackt, MSP_STRU_ID_16_23_TYPE_LAYER, ulParaBufSize);
    pLayerIndInfo = (DIAG_CMD_LOG_LAYER_IND_STRU *)(pstSocpPackt + sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU));

    /*组装DIAG命令参数*/
    pLayerIndInfo->ulModule  = ulModuleId;
    pLayerIndInfo->ulDestMod = pRptMessage->ulDestMod;
    pLayerIndInfo->ulNo = (g_DiagLogPktNum.ulLayerNum++);
    pLayerIndInfo->ulId      = pRptMessage->ulMessageID;
    VOS_MemCpy(pLayerIndInfo->aucDta, pRptMessage->pData, pRptMessage->ulDataSize);

    /*写入数据到编码源BUFFER*/
    ulRst = diag_SendCoderSrcToSocp(pstSocpPackt,sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU) +
            ulParaBufSize, SOCP_CODER_SRC_LOM_PRECEDENCE_IND);
    if(ulRst != ERR_MSP_SUCCESS)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportLayerMessageLog_ERR, ulRst, 0, 7);
    }
    diag_UnLockCoderSrcBuf();
    return ulRst;
}

/*****************************************************************************
 Function Name   : DIAG_ReportUserPlaneMessageLog
 Description     : 用户面上报接口，给PS使用
 Input           :DIAG_USERPLANE_MSG_STRU* pRptMessage
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-20  Draft Enact

*****************************************************************************/
VOS_UINT32 DIAG_ReportUserPlaneMessageLog(DIAG_USERPLANE_MSG_STRU* pRptMessage)
{
    VOS_UINT32 ulRst         = ERR_MSP_SUCCESS;
    VOS_UINT32 ulParaBufSize = 0;                   // 参数Buf Size
    VOS_UINT32 ulMessageId;
    VOS_UINT8* pstSocpPackt = NULL;
    DIAG_CMD_LOG_USERPLANE_IND_STRU* pRptInfo = NULL;


    /* 只记录调用次数*/
    DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportUserPlaneMessageLog, 0, 0, 0);

    /*检查DIAG是否初始化且HSO是否连接上*/
    if(!DIAG_IS_CONN_ON)
    {
        ulRst = ERR_MSP_NO_INITILIZATION;
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportUserPlaneMessageLog_ERR, ulRst, pRptMessage->ulMessageID, 2);
        return ulRst;
    }

    /*检查参数合法性*/
    if((NULL == pRptMessage)||((0 == pRptMessage->ulDataSize) && (NULL == pRptMessage->pData)))
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportUserPlaneMessageLog_ERR,ERR_MSP_INVALID_PARAMETER,
            (NULL != pRptMessage ? pRptMessage->ulMessageID : 0),1);

        return ERR_MSP_INVALID_PARAMETER;
    }

    ulMessageId = pRptMessage->ulMessageID;

    ulRst = diag_GetUserPlaneCfg(ulMessageId);
    if(ERR_MSP_SUCCESS != ulRst)
    {
        /* 过滤配置不允许上报*/
        ulRst = ERR_MSP_CFG_LOG_NOT_ALLOW;
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportUserPlaneMessageLog_ERR, ulRst, pRptMessage->ulMessageID, 4);

        return ulRst;
    }

    /*申请编码源BUFFER*/
    diag_LockCoderSrcBuf();
    ulParaBufSize = sizeof(DIAG_CMD_LOG_USERPLANE_IND_STRU) + pRptMessage->ulDataSize;
    pstSocpPackt = diag_GetCoderSrcBuf(sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU) +
        ulParaBufSize, SOCP_CODER_SRC_LOM_NORMAL_IND);
    if (NULL == pstSocpPackt)
    {
        diag_UnLockCoderSrcBuf();
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportUserPlaneMessageLog_ERR, ERR_MSP_NOT_ENOUGH_MEMORY, 0, 7);
        return ERR_MSP_NOT_ENOUGH_MEMORY;
    }

    /*组装SOCP包*/
    diag_SocpPsLogPkt(pstSocpPackt, MSP_STRU_ID_16_23_TYPE_USER, ulParaBufSize);
    pRptInfo = (DIAG_CMD_LOG_USERPLANE_IND_STRU*)(pstSocpPackt + sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU));

    /*组装DIAG命令参数*/
    pRptInfo->ulId = pRptMessage->ulMessageID;
    pRptInfo->ulNo = (g_DiagLogPktNum.ulUserNum)++;
    VOS_MemCpy(pRptInfo->aucDta, pRptMessage->pData, pRptMessage->ulDataSize);

    /*写入数据到编码源BUFFER*/
    ulRst = diag_SendCoderSrcToSocp(pstSocpPackt,sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU) +
        ulParaBufSize, SOCP_CODER_SRC_LOM_NORMAL_IND);
    if(ulRst != ERR_MSP_SUCCESS)
    {
        DIAG_DEBUG_SDM_FUN(EN_SDM_DIAG_ReportUserPlaneMessageLog_ERR, ulRst, 0, 7);
    }
    diag_UnLockCoderSrcBuf();
    return ulRst;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


