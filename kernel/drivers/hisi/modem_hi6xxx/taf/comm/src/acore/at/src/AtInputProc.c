/************************************************************************
  Copyright    : 2005-2007, Huawei Tech. Co., Ltd.
  File name    : AtInputProc.c
  Author       : ---
  Version      : V100R001
  Date         : 2005-04-19
  Description  : 该C文件给出了AT模块端口管理功能
  Function List:

  History      :
  1. Date:2005-04-19
     Author: ---
     Modification:Create

************************************************************************/

/*****************************************************************************
   1 头文件包含
*****************************************************************************/
#include "ATCmdProc.h"
#include "om.h"
#include "PppInterface.h"
#include "AtUsbComItf.h"
#include "AtInputProc.h"
#include "AtCsdInterface.h"
#include "AtTafAgentInterface.h"
#include "TafAgentInterface.h"
#include "cpm.h"
#include "AtCmdMsgProc.h"
#include "AtDataProc.h"
#include "ImmInterface.h"
#include "mdrv.h"

#include "AtMntn.h"
#include "AcpuReset.h"

#include "NasNvInterface.h"

#include "AtInternalMsg.h"
#include  "product_config.h"


#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767 -e960 修改人:罗建 107747;检视人:孙少华65952;原因:Log打印*/
#define    THIS_FILE_ID        PS_FILE_ID_AT_INPUTPROC_C
/*lint +e767 +e960 修改人:罗建 107747;检视人:sunshaohua*/

/*****************************************************************************
   2 全局变量定义
*****************************************************************************/
VOS_UINT32                              g_ulAtUsbDebugFlag = VOS_FALSE;


#if (FEATURE_ON == FEATURE_AT_HSIC)
AT_HSIC_CONTEXT_STRU                    g_astAtHsicCtx[] =
{
    {
        UDI_ACM_HSIC_ACM0_ID,      AT_HSIC_REPORT_ON,         AT_HsicOneReadDataCB,
        AT_HsicOneFreeDlDataBuf,   UDI_INVALID_HANDLE,        AT_CLIENT_ID_HSIC1,
        AT_CLIENT_TAB_HSIC1_INDEX, AT_HSIC1_USER,             AT_HSIC1_PORT_NO,
        {0, 0, 0, 0, 0, 0, 0}
    },

    {
        UDI_ACM_HSIC_ACM2_ID,      AT_HSIC_REPORT_ON,         AT_HsicTwoReadDataCB,
        AT_HsicTwoFreeDlDataBuf,   UDI_INVALID_HANDLE,        AT_CLIENT_ID_HSIC2,
        AT_CLIENT_TAB_HSIC2_INDEX, AT_HSIC2_USER,             AT_HSIC2_PORT_NO,
        {0, 0, 0, 0, 0, 0, 0}
    },

    {
        UDI_ACM_HSIC_ACM4_ID,      AT_HSIC_REPORT_OFF,        AT_HsicThreeReadDataCB,
        AT_HsicThreeFreeDlDataBuf, UDI_INVALID_HANDLE,        AT_CLIENT_ID_HSIC3,
        AT_CLIENT_TAB_HSIC3_INDEX, AT_HSIC3_USER,             AT_HSIC3_PORT_NO,
        {0, 0, 0, 0, 0, 0, 0}
    },

    {
        UDI_ACM_HSIC_ACM12_ID,     AT_HSIC_REPORT_ON,         AT_HsicFourReadDataCB,
        AT_HsicFourFreeDlDataBuf,  UDI_INVALID_HANDLE,        AT_CLIENT_ID_HSIC4,
        AT_CLIENT_TAB_HSIC4_INDEX, AT_HSIC4_USER,             AT_HSIC4_PORT_NO,
        {0, 0, 0, 0, 0, 0, 0}
    }
};
#endif

/* AT/DIAG通道的链路索引 */
VOS_UINT8                               gucOmDiagIndex    = AT_MAX_CLIENT_NUM;

/* USB NCM的UDI句柄 */
UDI_HANDLE                              g_ulAtUdiNdisHdl  = UDI_INVALID_HANDLE;

/* 该变量目前只保存USB-MODEM, HSIC-MODEM和HS-UART的句柄，后续需要添加需要与PL讨论 */
UDI_HANDLE                              g_alAtUdiHandle[AT_CLIENT_BUTT] = {UDI_INVALID_HANDLE};

/* AT帧结构与DRV 值之间的对应关系 */
AT_UART_FORMAT_PARAM_STRU               g_astAtUartFormatTab[] =
{
    /* auto detect */
    {AT_UART_FORMAT_AUTO_DETECT,            AT_UART_DATA_LEN_BUTT,
     AT_UART_STOP_LEN_BUTT,                 AT_UART_PARITY_LEN_BUTT},

    /* 8 data 2 stop */
    {AT_UART_FORMAT_8DATA_2STOP,            AT_UART_DATA_LEN_8_BIT,
     AT_UART_STOP_LEN_2_BIT,                AT_UART_PARITY_LEN_0_BIT},

    /* 8 data 1 parity 1 stop*/
    {AT_UART_FORMAT_8DATA_1PARITY_1STOP,    AT_UART_DATA_LEN_8_BIT,
     AT_UART_STOP_LEN_1_BIT,                AT_UART_PARITY_LEN_1_BIT},

    /* 8 data 1 stop */
    {AT_UART_FORMAT_8DATA_1STOP,            AT_UART_DATA_LEN_8_BIT,
     AT_UART_STOP_LEN_1_BIT,                AT_UART_PARITY_LEN_0_BIT},

    /* 7 data 2 stop */
    {AT_UART_FORMAT_7DATA_2STOP,            AT_UART_DATA_LEN_7_BIT,
     AT_UART_STOP_LEN_2_BIT,                AT_UART_PARITY_LEN_0_BIT},

    /* 7 data 1 parity 1 stop */
    {AT_UART_FORMAT_7DATA_1PARITY_1STOP,    AT_UART_DATA_LEN_7_BIT,
     AT_UART_STOP_LEN_1_BIT,                AT_UART_PARITY_LEN_1_BIT},

    /* 7 data 1 stop */
    {AT_UART_FORMAT_7DATA_1STOP,            AT_UART_DATA_LEN_7_BIT,
     AT_UART_STOP_LEN_1_BIT,                AT_UART_PARITY_LEN_0_BIT}
};


/*****************************************************************************
   3 函数、变量声明
*****************************************************************************/

/*****************************************************************************
   4 函数实现
*****************************************************************************/


VOS_VOID AT_GetAtMsgStruMsgLength(
    VOS_UINT32                          ulInfoLength,
    VOS_UINT32                         *pulMsgLength
)
{
    if (ulInfoLength > 4)
    {
        *pulMsgLength = (sizeof(AT_MSG_STRU) - VOS_MSG_HEAD_LENGTH)
                      + (ulInfoLength - 4);
    }
    else
    {
        *pulMsgLength = sizeof(AT_MSG_STRU) - VOS_MSG_HEAD_LENGTH;
    }

    return;
}


VOS_VOID AT_GetUserTypeFromIndex(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                          *pucUserType
)
{
    if (ucIndex < AT_MAX_CLIENT_NUM)
    {
        *pucUserType    = gastAtClientTab[ucIndex].UserType;
    }
    else
    {
        *pucUserType    = AT_BUTT_USER;
    }

    return;
}


VOS_VOID AT_DisplayVcomInputData(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                          *pData,
    VOS_UINT16                          usLen
)
{
    VOS_UINT8                          *pucSystemAppConfig;

    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

    /* E5无需回显 */
    if (SYSTEM_APP_WEBUI != *pucSystemAppConfig)
    {
        if ((usLen > 2) && (ucAtS3 == pData[usLen - 2]) && (ucAtS4 == pData[usLen - 1]))
        {
            APP_VCOM_Send(gastAtClientTab[ucIndex].ucPortNo, pData, (usLen - 1));
        }
        else
        {
            APP_VCOM_Send(gastAtClientTab[ucIndex].ucPortNo, pData, usLen);
        }
    }
}
VOS_VOID AT_SetAts3Value(VOS_UINT8 ucValue)
{
    if (VOS_TRUE == g_ulAtUsbDebugFlag)
    {
        ucAtS3 = ucValue;
    }

    return;
}


VOS_UINT32 At_CmdStreamPreProc(VOS_UINT8 ucIndex, VOS_UINT8* pData, VOS_UINT16 usLen)
{
    VOS_UINT8                          *pHead = TAF_NULL_PTR;
    VOS_UINT16                          usCount = 0;
    VOS_UINT16                          usTotal = 0;
    VOS_UINT32                          ulMuxUserFlg;
    VOS_UINT32                          ulHsicUserFlg;

    pHead = pData;

    /* Added by L60609 for MUX，2012-08-06,  Begin */
    ulHsicUserFlg = AT_CheckHsicUser(ucIndex);
    ulMuxUserFlg  = AT_CheckMuxUser(ucIndex);
    /* Added by L60609 for MUX，2012-08-06,  End */

    if (VOS_TRUE == g_ulAtUsbDebugFlag)
    {
        vos_printf("At_CmdStreamPreProc: AtEType = %d, UserType = %d, ucAtS3 = %d\r\n",
                   gucAtEType, gastAtClientTab[ucIndex].UserType, ucAtS3);
    }

    if( AT_E_ECHO_CMD == gucAtEType )
    {
        if(AT_USBCOM_USER == gastAtClientTab[ucIndex].UserType)
        {
            /*向USB COM口发送数据*/
            /* 判断pData码流的结尾是否为<CR><LF>形式，代码中2为回车换行两个字符长度 */
            if ((usLen > 2) && (ucAtS3 == pData[usLen - 2]) && (ucAtS4 == pData[usLen - 1]))
            {
                /* 删去结尾的<LF>字符 */
                DMS_COM_SEND(AT_USB_COM_PORT_NO, pData, usLen - 1);
                AT_MNTN_TraceCmdResult(ucIndex, pData, usLen - 1);
            }
            else
            {
                DMS_COM_SEND(AT_USB_COM_PORT_NO, pData, usLen);
                AT_MNTN_TraceCmdResult(ucIndex, pData, usLen);
            }

        }
        else if (AT_CTR_USER == gastAtClientTab[ucIndex].UserType)
        {
            /* 判断pData码流的结尾是否为<CR><LF>形式 */
            if ((usLen > 2) && (ucAtS3 == pData[usLen - 2]) && (ucAtS4 == pData[usLen - 1]))
            {
                /* 删去结尾的<LF>字符 */
                DMS_COM_SEND(AT_CTR_PORT_NO, pData, usLen - 1);
                AT_MNTN_TraceCmdResult(ucIndex, pData, usLen - 1);
            }
            else
            {
                DMS_COM_SEND(AT_CTR_PORT_NO, pData, usLen);
                AT_MNTN_TraceCmdResult(ucIndex, pData, usLen);
            }

        }
        /* Modified by s62952 for BalongV300R002 Build优化项目 2012-02-28, begin */
        else if (AT_MODEM_USER == gastAtClientTab[ucIndex].UserType)
        {
            if ((usLen > 2) && (ucAtS3 == pData[usLen - 2]) && (ucAtS4 == pData[usLen - 1]))
            {
                AT_SendDataToModem(ucIndex, pData,(usLen-1));
            }
            else
            {
                AT_SendDataToModem(ucIndex, pData,usLen);
            }
        }
        /* Modified by s62952 for BalongV300R002 Build优化项目 2012-02-28, end */
        /* VCOM AT口的回显处理 */
        else if (AT_APP_USER == gastAtClientTab[ucIndex].UserType)
        {
#if (FEATURE_ON == FEATURE_VCOM_EXT)
            if ((AT_CLIENT_TAB_APP9_INDEX != ucIndex) && (AT_CLIENT_TAB_APP12_INDEX != ucIndex))
#endif
            {
                AT_DisplayVcomInputData(ucIndex, pData, usLen);
            }
        }

        /* Modified by L60609 for AT Project，2011-10-18,  Begin*/
        else if (AT_SOCK_USER == gastAtClientTab[ucIndex].UserType)
        {
            if ( BSP_MODULE_SUPPORT == mdrv_misc_support_check(BSP_MODULE_TYPE_WIFI) )
            {
                if ((usLen > 2) && (ucAtS3 == pData[usLen - 2]) && (ucAtS4 == pData[usLen - 1]))
                {
                    CPM_ComSend(CPM_AT_COMM, pData, VOS_NULL_PTR, usLen - 1);
                }
                else
                {
                    CPM_ComSend(CPM_AT_COMM, pData, VOS_NULL_PTR, usLen);
                }
            }

        }
        /* Modified by L60609 for AT Project，2011-10-18,  End*/
        else if (AT_NDIS_USER == gastAtClientTab[ucIndex].UserType)
        {
            /* NDIS AT口无需回显 */
            AT_WARN_LOG("At_CmdStreamPreProc:WARNING: NDIS AT");
        }
        else if (VOS_TRUE == ulHsicUserFlg)
        {
#if (FEATURE_ON == FEATURE_AT_HSIC)
            /* 判断pData码流的结尾是否为<CR><LF>形式 */
            if ((usLen > 2) && (ucAtS3 == pData[usLen - 2]) && (ucAtS4 == pData[usLen - 1]))
            {
                /* 删去结尾的<LF>字符 */
                AT_SendDataToHsic(ucIndex, pData, (usLen - 1));
                AT_MNTN_TraceCmdResult(ucIndex, pData, usLen - 1);
            }
            else
            {
                AT_SendDataToHsic(ucIndex, pData, usLen);
                AT_MNTN_TraceCmdResult(ucIndex, pData, usLen);
            }
#endif
        }
        /* Added by L60609 for MUX，2012-08-06,  Begin */
        /* MUX user */
        else if (VOS_TRUE == ulMuxUserFlg)
        {
            AT_RcvMuxCmdStream_PreProc(ucIndex, pData, usLen);
        }
        /* Added by L60609 for MUX，2012-08-06,  End */
        else
        {
            AT_LOG1("At_CmdStreamPreProc:WARNING: Abnormal UserType,ucIndex:",ucIndex);
        }
    }

    /* MAC系统上的MP后台问题:AT+CMGS=**<CR><^z><Z>(或AT+CMGW=**<CR><^z><Z>)
       为了规避该问题，需要在接收到如上形式的码流后，
       需要将命令后的无效字符<^z><Z>删去 */
    AT_DiscardInvalidCharForSms(pData, &usLen);

    /* 解析到如下字符才将码流以消息方式发送到AT的消息队列中: <CR>/<ctrl-z>/<ESC> */
    while(usCount++ < usLen)
    {
        /*lint -e960 Note -- Violates MISRA 2004 Required Rule 4.1, Prohibited escape sequence used (hexadecimal)*/
        if (At_CheckSplitChar((*((pData + usCount) - 1))))
        /*lint +e960*/
        {
            if(g_aucAtDataBuff[ucIndex].ulBuffLen > 0)
            {
                if((g_aucAtDataBuff[ucIndex].ulBuffLen + usCount) >= AT_COM_BUFF_LEN)
                {
                    g_aucAtDataBuff[ucIndex].ulBuffLen = 0;
                    return AT_FAILURE;
                }
                PS_MEM_CPY(&g_aucAtDataBuff[ucIndex].aucDataBuff[g_aucAtDataBuff[ucIndex].ulBuffLen], pHead, usCount);
                At_SendCmdMsg(ucIndex,g_aucAtDataBuff[ucIndex].aucDataBuff, (TAF_UINT16)(g_aucAtDataBuff[ucIndex].ulBuffLen + usCount), 0);
                pHead   = pData + usCount;
                usTotal = usCount;
                g_aucAtDataBuff[ucIndex].ulBuffLen = 0;
            }
            else
            {
                At_SendCmdMsg(ucIndex, pHead, usCount - usTotal, 0);
                pHead   = pData + usCount;
                usTotal = usCount;
            }
        }
    }

    if(usTotal < usLen)
    {
        if((g_aucAtDataBuff[ucIndex].ulBuffLen + (usLen - usTotal)) >= AT_COM_BUFF_LEN)
        {
            g_aucAtDataBuff[ucIndex].ulBuffLen = 0;
            return AT_FAILURE;
        }
        PS_MEM_CPY(&g_aucAtDataBuff[ucIndex].aucDataBuff[g_aucAtDataBuff[ucIndex].ulBuffLen], pHead, (TAF_UINT32)(usLen - usTotal));
        g_aucAtDataBuff[ucIndex].ulBuffLen += (VOS_UINT16)((pData - pHead) + usLen);
    }

    return AT_SUCCESS;
}
VOS_VOID AT_StopFlowCtrl(VOS_UINT8 ucIndex)
{
    switch (gastAtClientTab[ucIndex].UserType)
    {
        case AT_MODEM_USER:
            AT_MNTN_TraceStopFlowCtrl(ucIndex, AT_FC_DEVICE_TYPE_MODEM);
            AT_CtrlCTS(ucIndex, AT_IO_LEVEL_HIGH);
            break;

        case AT_HSUART_USER:
            AT_MNTN_TraceStopFlowCtrl(ucIndex, AT_FC_DEVICE_TYPE_HSUART);
            AT_CtrlCTS(ucIndex, AT_IO_LEVEL_HIGH);
            break;

        default:
            break;
    }

    return;
}

/*****************************************************************************
 函 数 名  : At_OmDataProc
 功能描述  : 调用OM提供的各端口接收数据函数发送数据，不需要再区分数据模式
 输入参数  : VOS_UINT8                           ucPortNo
             VOS_UINT8                          *pData
             VOS_UINT16                          usLen
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年10月10日
    作    者   : 鲁琳/l60609
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 At_OmDataProc (
    VOS_UINT8                           ucPortNo,
    VOS_UINT8                          *pData,
    VOS_UINT16                          usLen
)
{
    VOS_UINT32                          ulRst;

    /*OM只处理UART PCUI CTRL口的数据*/
    switch(ucPortNo)
    {
        case AT_UART_PORT_NO:
            if (VOS_NULL_PTR == g_apAtPortDataRcvFuncTab[AT_UART_PORT])
            {
                AT_ERR_LOG("At_OmDataProc: Uart port proc func is NULL!");
                return VOS_ERR;
            }

            ulRst = g_apAtPortDataRcvFuncTab[AT_UART_PORT](pData, usLen);
            break;

        case AT_USB_COM_PORT_NO:
            if (VOS_NULL_PTR == g_apAtPortDataRcvFuncTab[AT_PCUI_PORT])
            {
                AT_ERR_LOG("At_OmDataProc: PCUI proc func is NULL!");
                return VOS_ERR;
            }

            ulRst = g_apAtPortDataRcvFuncTab[AT_PCUI_PORT](pData, usLen);
            break;

        case AT_CTR_PORT_NO:
            if (VOS_NULL_PTR == g_apAtPortDataRcvFuncTab[AT_CTRL_PORT])
            {
                AT_ERR_LOG("At_OmDataProc: CTRL proc func is NULL!");
                return VOS_ERR;
            }

            ulRst = g_apAtPortDataRcvFuncTab[AT_CTRL_PORT](pData, usLen);
            break;

        case AT_HSUART_PORT_NO:
            if (VOS_NULL_PTR == g_apAtPortDataRcvFuncTab[AT_HSUART_PORT])
            {
                AT_ERR_LOG("At_OmDataProc: HSUART proc func is NULL!");
                return VOS_ERR;
            }

            ulRst = g_apAtPortDataRcvFuncTab[AT_HSUART_PORT](pData, usLen);
            break;

        default:
            AT_WARN_LOG("At_OmDataProc: don't proc data of this port!");
            return VOS_ERR;
    }

    return ulRst;
}

/*****************************************************************************
 函 数 名  : At_DataStreamPreProc
 功能描述  : AT数据预处理函数
 输入参数  : TAF_UINT8 ucIndex
             TAF_UINT8 DataMode
             TAF_UINT8* pData
             TAF_UINT16 usLen
 输出参数  : 无
 返 回 值  : TAF_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
  2.日    期   : 2011年10月10日
    作    者   : 鲁琳/l60609
    修改内容   : 修改OM的接收数据函数

*****************************************************************************/
TAF_UINT32 At_DataStreamPreProc (TAF_UINT8 ucIndex,TAF_UINT8 DataMode,TAF_UINT8* pData, TAF_UINT16 usLen)
{

    AT_LOG1("At_DataStreamPreProc ucIndex:",ucIndex);
    AT_LOG1("At_DataStreamPreProc usLen:",usLen);
    AT_LOG1("At_DataStreamPreProc DataMode:",DataMode);

    switch(DataMode)    /* 当前用户的数传类型 */
    {
        case AT_CSD_DATA_MODE:
            break;

        /* Modified by L60609 for AT Project，2011-10-04,  Begin*/
        /*调用OM提供的各端口接收数据函数发送数据，不需要再区分数据模式*/
        case AT_DIAG_DATA_MODE:
        case AT_OM_DATA_MODE:
            At_OmDataProc(gastAtClientTab[ucIndex].ucPortNo, pData,usLen);
            break;
        /* Modified by L60609 for AT Project，2011-10-04,  End*/

        default:
            AT_WARN_LOG("At_DataStreamPreProc DataMode Wrong!");
            break;
    }
    return AT_SUCCESS;
}

/* Modified by s62952 for BalongV300R002 Build优化项目 2012-02-28, begin */

VOS_UINT32  AT_CsdDataModeRcvModemMsc(
    VOS_UINT8                           ucIndex
)
{
    TAFAGERNT_MN_CALL_INFO_STRU         astCallInfos[MN_CALL_MAX_NUM];
    VOS_UINT32                          i;
    VOS_UINT8                           ucNumOfCalls;
    VOS_UINT32                          ulRlst;


    PS_MEM_SET(astCallInfos, 0x00, sizeof(astCallInfos));


    /* 查询当前的呼叫状态信息，如果有VIDEO类型的呼叫，则挂断该呼叫，目前由于不会存在多个VIDEO呼叫，
       因此找到一个VIDEO类型的呼叫执行完毕后，即可退出 */

    /* Modified by l60609 for DSDA PhaseIII, 2013-3-13, begin */
    ulRlst          = TAF_AGENT_GetCallInfoReq(ucIndex, &ucNumOfCalls, astCallInfos);
    /* Modified by l60609 for DSDA PhaseIII, 2013-3-13, end */

    if(VOS_OK == ulRlst)
    {
        for (i = 0; i < ucNumOfCalls; i++)
        {
            if (MN_CALL_TYPE_VIDEO == astCallInfos[i].enCallType)
            {
                /* 未挂断过电话 */
                if (gastAtClientTab[ucIndex].CmdCurrentOpt != AT_CMD_END_SET)
                {
                    PS_LOG1(WUEPS_PID_AT, 0, PS_PRINT_INFO, "At_SetHPara: ulNumOfCalls is ",(TAF_INT32)ucNumOfCalls);

                    if(AT_SUCCESS == MN_CALL_End(gastAtClientTab[ucIndex].usClientId,
                                                 0,
                                                 astCallInfos[i].callId,
                                                 VOS_NULL_PTR))
                    {
                        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_END_SET;
                        return AT_SUCCESS;
                    }
                    else
                    {
                        return AT_ERROR;
                    }
                }
                else
                {
                    /* 之前已做过挂断电话操作，又收到拉低DTR管脚信号消息，不做处理 */
                    return AT_SUCCESS;
                }
            }
        }
    }


    return AT_CME_UNKNOWN;
}


VOS_UINT32  AT_PppDataModeRcvModemMsc(
    VOS_UINT8                           ucIndex,
    AT_DCE_MSC_STRU                     *pMscStru
)
{
    /* 1.判断(AT_CMD_PS_DATA_CALL_END_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
         的目的:若正常断开，则之前已经执行了PDP DEACTIVE流程，此时若再拉低DTR，则
         无需再执行该异常流程
        2.该分支的处理场景:若出于流控状态下,用户发起PPP断开，此时PPP报文无法交互，
          只能在最后拉低DTR信号的时候，执行PPP断开操作
    */
    if (pMscStru->OP_Dtr && (0 == pMscStru->ucDtr))
    {
        if ( (AT_CMD_PS_DATA_CALL_END_SET      == gastAtClientTab[ucIndex].CmdCurrentOpt)
          || (AT_CMD_WAIT_PPP_PROTOCOL_REL_SET == gastAtClientTab[ucIndex].CmdCurrentOpt) )
        {
            return AT_SUCCESS;
        }

        /* 若处于流控状态下，则PPP断开的协商报文是无法传到UE侧的，
           UE只能在DTR信号拉低的时候,执行PPP拨号断开操作*/
        if (0 == (gastAtClientTab[ucIndex].ModemStatus & IO_CTRL_CTS))
        {
            AT_StopFlowCtrl((TAF_UINT8)ucIndex);
        }

        /*向PPP发送释放PPP操作*/
        PPP_RcvAtCtrlOperEvent(gastAtClientTab[ucIndex].usPppId, PPP_AT_CTRL_REL_PPP_REQ);

        /*向PPP发送HDLC去使能操作*/
        PPP_RcvAtCtrlOperEvent(gastAtClientTab[ucIndex].usPppId, PPP_AT_CTRL_HDLC_DISABLE);

        /* 停止Modem口的AT定时器以及AT链路的当前操作指示 */
        AT_STOP_TIMER_CMD_READY(ucIndex);

        /*EVENT - RCV Down DTR to Disconnect PPP in Abnormal procedure(PDP type:IP) ;index*/
        AT_EventReport(WUEPS_PID_AT, NAS_OM_EVENT_DTE_DOWN_DTR_RELEASE_PPP_IP_TYPE,
                        &ucIndex, sizeof(ucIndex));

        if ( VOS_OK == TAF_PS_CallEnd(WUEPS_PID_AT,
                                      gastAtClientTab[ucIndex].usClientId,
                                      0,
                                      gastAtClientTab[ucIndex].ucCid) )
        {
            /* 开定时器 */
            if (AT_SUCCESS != At_StartTimer(AT_SET_PARA_TIME, ucIndex))
            {
                AT_ERR_LOG("At_UsbModemStatusPreProc:ERROR:Start Timer");
                return AT_FAILURE;
            }

            /* 设置当前操作类型 */
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_PS_DATA_CALL_END_SET;
        }
        else
        {
            return AT_FAILURE;
        }
    }

    return AT_SUCCESS;
}


VOS_UINT32  AT_IpDataModeRcvModemMsc(
    VOS_UINT8                           ucIndex,
    AT_DCE_MSC_STRU                     *pMscStru
)
{
    if (pMscStru->OP_Dtr && (0 == pMscStru->ucDtr))
    {
        /*若原先开启了流控，则需停止流控*/
        if (0 == (gastAtClientTab[ucIndex].ModemStatus & IO_CTRL_CTS))
        {
            AT_StopFlowCtrl((TAF_UINT8)ucIndex);
        }

        PPP_RcvAtCtrlOperEvent(gastAtClientTab[ucIndex].usPppId, PPP_AT_CTRL_REL_PPP_RAW_REQ);

        /*向PPP发送HDLC去使能操作*/
        PPP_RcvAtCtrlOperEvent(gastAtClientTab[ucIndex].usPppId, PPP_AT_CTRL_HDLC_DISABLE);

        /* 停止Modem口的AT定时器以及AT链路的当前操作指示 */
        AT_STOP_TIMER_CMD_READY(ucIndex);;

        /*EVENT - RCV Down DTR to Disconnect PPP in Abnormal procedure(PDP type:PPP) ;index*/
        AT_EventReport(WUEPS_PID_AT, NAS_OM_EVENT_DTE_DOWN_DTR_RELEASE_PPP_PPP_TYPE,
                        &ucIndex, sizeof(ucIndex));

        if ( VOS_OK == TAF_PS_CallEnd(WUEPS_PID_AT,
                                      gastAtClientTab[ucIndex].usClientId,
                                      0,
                                      gastAtClientTab[ucIndex].ucCid) )
        {
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_PS_DATA_CALL_END_SET;
            return AT_SUCCESS;
        }
        else
        {
            return AT_FAILURE;
        }
    }

    return AT_SUCCESS;
}


VOS_VOID AT_MODEM_ProcDtrChange(
    VOS_UINT8                           ucIndex,
    AT_DCE_MSC_STRU                    *pstDceMsc
)
{
    if (1 == pstDceMsc->ucDtr)
    {
        /*拉高DSR、CTS信号*/
        AT_CtrlDSR(ucIndex, AT_IO_LEVEL_HIGH);
        AT_StopFlowCtrl(ucIndex);
    }
    else
    {
        /* 参考Q实现，DSR信号在上电后一直保持拉高状态，即使收到DTR也不拉低DSR；
           同时，PC在正常流程中一般不会拉低DTR信号，在异常流程中会将之拉低，
           所以UE在收到DTR拉低 的时候，需要将DCD拉低 */
        if ( (AT_DATA_MODE == gastAtClientTab[ucIndex].Mode)
          && (AT_CSD_DATA_MODE == gastAtClientTab[ucIndex].DataMode) )
        {
            g_ucDtrDownFlag = VOS_TRUE;
        }

        AT_CtrlDCD(ucIndex, AT_IO_LEVEL_LOW);
    }

}


VOS_UINT32 AT_MODEM_WriteMscCmd(
    VOS_UINT8                           ucIndex,
    AT_DCE_MSC_STRU                    *pstDceMsc
)
{
    UDI_HANDLE                          lUdiHandle;
    VOS_INT32                           lResult;

    /* 检查UDI句柄有效性 */
    lUdiHandle = g_alAtUdiHandle[ucIndex];
    if (UDI_INVALID_HANDLE == lUdiHandle)
    {
        AT_ERR_LOG("AT_MODEM_WriteMscCmd: Invalid UDI handle!");
        return AT_FAILURE;
    }

    /* 写管脚信号 */
    lResult = mdrv_udi_ioctl(lUdiHandle, ACM_MODEM_IOCTL_MSC_WRITE_CMD, pstDceMsc);
    if (VOS_OK != lResult)
    {
        AT_ERR_LOG("AT_MODEM_WriteMscCmd: Write failed!");
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}


VOS_UINT32 AT_MODEM_StartFlowCtrl(
    VOS_UINT32                          ulParam1,
    VOS_UINT32                          ulParam2
)
{
    VOS_UINT8                           ucIndex;

    for (ucIndex = 0; ucIndex < AT_MAX_CLIENT_NUM; ucIndex++)
    {
        if ( (AT_MODEM_USER == gastAtClientTab[ucIndex].UserType)
          && (AT_DATA_MODE == gastAtClientTab[ucIndex].Mode) )
        {
            if ( (AT_PPP_DATA_MODE == gastAtClientTab[ucIndex].DataMode)
              || (AT_IP_DATA_MODE == gastAtClientTab[ucIndex].DataMode)
              || (AT_CSD_DATA_MODE == gastAtClientTab[ucIndex].DataMode) )
            {
                AT_MNTN_TraceStartFlowCtrl(ucIndex, AT_FC_DEVICE_TYPE_MODEM);
                AT_CtrlCTS(ucIndex, AT_IO_LEVEL_LOW);
            }
        }
    }

    return AT_SUCCESS;
}


VOS_UINT32 AT_MODEM_StopFlowCtrl(
    VOS_UINT32                          ulParam1,
    VOS_UINT32                          ulParam2
)
{
    VOS_UINT8                           ucIndex;

    for(ucIndex = 0; ucIndex < AT_MAX_CLIENT_NUM; ucIndex++)
    {
        if ( (AT_MODEM_USER == gastAtClientTab[ucIndex].UserType)
          && (AT_DATA_MODE == gastAtClientTab[ucIndex].Mode) )
        {
            if ( (AT_PPP_DATA_MODE == gastAtClientTab[ucIndex].DataMode)
                || (AT_IP_DATA_MODE == gastAtClientTab[ucIndex].DataMode)
                || (AT_CSD_DATA_MODE == gastAtClientTab[ucIndex].DataMode) )
            {
                AT_MNTN_TraceStopFlowCtrl(ucIndex, AT_FC_DEVICE_TYPE_MODEM);
                AT_CtrlCTS(ucIndex, AT_IO_LEVEL_HIGH);
            }
        }
    }

    return AT_SUCCESS;
}


VOS_UINT32 AT_ModemStatusPreProc(
    VOS_UINT8                           ucIndex,
    AT_DCE_MSC_STRU                    *pMscStru
)
{

    NAS_OM_EVENT_ID_ENUM_UINT16         enEventId;

#if (FEATURE_ON == FEATURE_AT_HSUART)
    VOS_UINT32                          ulRet;
    ulRet = VOS_TRUE;
#endif



    if (VOS_NULL_PTR == pMscStru)
    {
        return AT_FAILURE;
    }

    if (pMscStru->OP_Dtr)
    {
        enEventId = (0 != pMscStru->ucDtr) ?
                    NAS_OM_EVENT_DTE_UP_DTR : NAS_OM_EVENT_DTE_DOWN_DTR;

        AT_EventReport(WUEPS_PID_AT, enEventId, &ucIndex, sizeof(VOS_UINT8));

        if (VOS_TRUE == AT_CheckModemUser(ucIndex))
        {
            AT_MODEM_ProcDtrChange(ucIndex, pMscStru);
        }

#if (FEATURE_ON == FEATURE_AT_HSUART)
        if (VOS_TRUE == AT_CheckHsUartUser(ucIndex))
        {
            ulRet = AT_UART_ProcDtrChange(ucIndex, pMscStru);
            if (VOS_FALSE == ulRet)
            {
                return AT_SUCCESS;
            }
        }
#endif
    }

    /* 数传模式响应MSC处理 */
    if ( (AT_DATA_MODE == gastAtClientTab[ucIndex].Mode)
      || (AT_ONLINE_CMD_MODE == gastAtClientTab[ucIndex].Mode) )
    {
        switch (gastAtClientTab[ucIndex].DataMode)
        {
        case AT_CSD_DATA_MODE:
            if ((pMscStru->OP_Dtr) && (0 == pMscStru->ucDtr))
            {
                AT_CsdDataModeRcvModemMsc(ucIndex);
            }
            return AT_SUCCESS;

        case AT_PPP_DATA_MODE:
            return AT_PppDataModeRcvModemMsc(ucIndex, pMscStru);

        case AT_IP_DATA_MODE:
            return AT_IpDataModeRcvModemMsc(ucIndex, pMscStru);

        default:
            AT_WARN_LOG("At_UsbModemStatusPreProc: DataMode Wrong!");
            break;
        }
    }
    else
    {
         /* 有可能在还没接听的时候，拉低管脚信号，此时还处于命令状态，
            目前来说只有CSD模式下会有这样的情况 */
         if ((pMscStru->OP_Dtr) && (0 == pMscStru->ucDtr))
         {
             AT_CsdDataModeRcvModemMsc(ucIndex);
         }
    }

    return AT_SUCCESS;
}
/* Modified by s62952 for BalongV300R002 Build优化项目 2012-02-28, end */

VOS_VOID AT_ModemSetCtlStatus(
    VOS_UINT8                           ucIndex,
    AT_DCE_MSC_STRU                    *pMscStru
)
{
    if (TAF_NULL_PTR == pMscStru)
    {
        return;
    }

    /*更新dsr信号*/
    if ( pMscStru->OP_Dsr )
    {
        if ( 1 == pMscStru->ucDsr )
        {
            gastAtClientTab[ucIndex].ModemStatus |= IO_CTRL_DSR;
        }
        else
        {
            gastAtClientTab[ucIndex].ModemStatus &= ~IO_CTRL_DSR;
        }
    }

    /*更新CTS信号*/
    if ( pMscStru->OP_Cts )
    {
        if ( 1 == pMscStru->ucCts )
        {
            gastAtClientTab[ucIndex].ModemStatus |= IO_CTRL_CTS;
        }
        else
        {
            gastAtClientTab[ucIndex].ModemStatus &= ~IO_CTRL_CTS;
        }
    }

    /*更新RI信号*/
    if ( pMscStru->OP_Ri )
    {
        if ( 1 == pMscStru->ucRi )
        {
            gastAtClientTab[ucIndex].ModemStatus |= IO_CTRL_RI;
        }
        else
        {
            gastAtClientTab[ucIndex].ModemStatus &= ~IO_CTRL_RI;
        }
    }

    /*更新DCD信号*/
    if ( pMscStru->OP_Dcd )
    {
        if ( 1 == pMscStru->ucDcd )
        {
            gastAtClientTab[ucIndex].ModemStatus |= IO_CTRL_DCD;
        }
        else
        {
            gastAtClientTab[ucIndex].ModemStatus &= ~IO_CTRL_DCD;
        }
    }

    /*更新FC信号*/
    if ( pMscStru->OP_Fc )
    {
        if ( 1 == pMscStru->ucFc )
        {
            gastAtClientTab[ucIndex].ModemStatus |= IO_CTRL_FC;
        }
        else
        {
            gastAtClientTab[ucIndex].ModemStatus &= ~IO_CTRL_FC;
        }
    }

}
VOS_UINT32 AT_SetModemStatus(
    VOS_UINT8                           ucIndex,
    AT_DCE_MSC_STRU                    *pstMsc
)
{
    VOS_UINT32                          ulResult;

    if (VOS_NULL_PTR == pstMsc)
    {
        return AT_FAILURE;
    }

    if (ucIndex >= AT_CLIENT_BUTT)
    {
        return AT_FAILURE;
    }

    /* 更新本地管脚信号*/
    AT_ModemSetCtlStatus(ucIndex, pstMsc);

    /* 输出管脚信号可维可测 */
    AT_MNTN_TraceOutputMsc(ucIndex, pstMsc);

    /* 写入管脚信号参数 */
    switch (gastAtClientTab[ucIndex].UserType)
    {
        case AT_MODEM_USER:
            ulResult = AT_MODEM_WriteMscCmd(ucIndex, pstMsc);
            break;

#if (FEATURE_ON == FEATURE_AT_HSUART)
        case AT_HSUART_USER:
            ulResult = AT_HSUART_WriteMscCmd(ucIndex, pstMsc);
            break;
#endif

        default:
            ulResult = AT_SUCCESS;
            break;
    }

    return ulResult;
}

/* 删除At_SetModemStatusForFC函数, 功能和At_SetModemStatus重复 */


VOS_UINT32 At_ModemEst (
    VOS_UINT8                           ucIndex,
    AT_CLIENT_ID_ENUM_UINT16            usClientId,
    VOS_UINT8                           ucPortNo
)
{

    /* 清空对应表项 */
    PS_MEM_SET(&gastAtClientTab[ucIndex], 0x00, sizeof(AT_CLIENT_MANAGE_STRU));

    /* 填写用户表项 */
    gastAtClientTab[ucIndex].usClientId      = usClientId;
    gastAtClientTab[ucIndex].ucPortType      = ucPortNo;
    gastAtClientTab[ucIndex].ucDlci          = AT_MODEM_USER_DLCI;
    gastAtClientTab[ucIndex].ucPortNo        = ucPortNo;
    gastAtClientTab[ucIndex].UserType        = AT_MODEM_USER;
    gastAtClientTab[ucIndex].ucUsed          = AT_CLIENT_USED;

    /* 以下可以不用填写，前面PS_MEMSET已经初始化，只为可靠起见 */
    gastAtClientTab[ucIndex].Mode            = AT_CMD_MODE;
    gastAtClientTab[ucIndex].IndMode         = AT_IND_MODE;
    gastAtClientTab[ucIndex].DataMode        = AT_DATA_BUTT_MODE;
    gastAtClientTab[ucIndex].DataState       = AT_DATA_STOP_STATE;
    gastAtClientTab[ucIndex].CmdCurrentOpt   = AT_CMD_CURRENT_OPT_BUTT;
    g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_READY;

    AT_LOG1("At_ModemEst ucIndex:",ucIndex);

    return AT_SUCCESS;
}
VOS_UINT32 At_ModemMscInd (
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           ucDlci,
    AT_DCE_MSC_STRU                    *pMscStru
)
{
    AT_PPP_MODEM_MSC_IND_MSG_STRU      *pMsg;
    VOS_UINT32                          ulLength;
    VOS_UINT_PTR                        ulTmpAddr;

    ulLength = (sizeof(AT_PPP_MODEM_MSC_IND_MSG_STRU) - VOS_MSG_HEAD_LENGTH)
               + (sizeof(AT_DCE_MSC_STRU) - 2);

    pMsg = ( AT_PPP_MODEM_MSC_IND_MSG_STRU * )PS_ALLOC_MSG( PS_PID_APP_PPP, ulLength );
    if ( VOS_NULL_PTR == pMsg )
    {
        /*打印出错信息---申请消息包失败:*/
        AT_WARN_LOG("At_ModemMscInd: Alloc AT_PPP_MODEM_MSC_IND_MSG_STRU msg fail!");
        return AT_FAILURE;
    }

    /*填写消息头:*/
    pMsg->MsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pMsg->MsgHeader.ulSenderPid     = PS_PID_APP_PPP;
    pMsg->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pMsg->MsgHeader.ulReceiverPid   = WUEPS_PID_AT;
    pMsg->MsgHeader.ulLength        = ulLength;
    pMsg->MsgHeader.ulMsgName       = AT_PPP_MODEM_MSC_IND_MSG;

    /*填写消息体*/
    pMsg->ucIndex                   = ucIndex;
    pMsg->ucDlci                    = ucDlci;

    /* 填写管脚数据 */
    ulTmpAddr = (VOS_UINT_PTR)(pMsg->aucMscInd);

    PS_MEM_CPY((VOS_VOID *)ulTmpAddr, (VOS_UINT8 *)pMscStru, sizeof(AT_DCE_MSC_STRU));

    /* 发送消息 */
    if ( VOS_OK != PS_SEND_MSG( PS_PID_APP_PPP, pMsg ) )
    {
        /*打印警告信息---发送消息失败:*/
        AT_WARN_LOG( "At_ModemMscInd:WARNING:SEND AT_PPP_MODEM_MSC_IND_MSG_STRU msg FAIL!" );
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}


VOS_UINT32 AT_ModemGetUlDataBuf(
    VOS_UINT8                           ucIndex,
    IMM_ZC_STRU                       **ppstBuf
)
{
    ACM_WR_ASYNC_INFO                   stCtlParam;
    VOS_INT32                           ulResult;


    PS_MEM_SET(&stCtlParam, 0x00, sizeof(stCtlParam));


    /* Modified by L60609 for PS Project，2011-12-06,  Begin*/

    /* 获取底软上行数据buffer */
    ulResult = mdrv_udi_ioctl(g_alAtUdiHandle[ucIndex], ACM_IOCTL_GET_RD_BUFF, &stCtlParam);

    if ( VOS_OK != ulResult )
    {
        AT_ERR_LOG1("AT_ModemGetUlDataBuf, WARNING, Get modem buffer failed code %d!",
                  ulResult);
        AT_MODEM_DBG_UL_GET_RD_FAIL_NUM(1);
        return AT_FAILURE;
    }

    if (VOS_NULL_PTR == stCtlParam.pVirAddr)
    {
        AT_ERR_LOG("AT_ModemGetUlDataBuf, WARNING, Data buffer error");
        AT_MODEM_DBG_UL_INVALID_RD_NUM(1);
        return AT_FAILURE;
    }

    AT_MODEM_DBG_UL_GET_RD_SUCC_NUM(1);

    *ppstBuf = (IMM_ZC_STRU *)stCtlParam.pVirAddr;

    /* Modified by L60609 for PS Project，2011-12-06,  End*/
    return AT_SUCCESS;
}
VOS_UINT32 At_ModemDataInd(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           ucDlci,
    IMM_ZC_STRU                        *pstData
)
{
    /* Modified by L60609 for PS Project，2011-12-06,  Begin*/
    AT_DCE_MSC_STRU                     stMscStru;
    VOS_UINT32                          ulRet;
    /* pData为数据内容指针 */
    VOS_UINT8                          *pData;
    /* usLen为数据内容的长度 */
    VOS_UINT16                          usLen;

    /* 检查index和Dlci是否正确 */
#if (FEATURE_ON == FEATURE_AT_HSIC)
    if ((AT_CLIENT_TAB_MODEM_INDEX != ucIndex)
     && (AT_CLIENT_TAB_HSIC_MODEM_INDEX != ucIndex))
#else
    if (AT_CLIENT_TAB_MODEM_INDEX != ucIndex)
#endif
    {
        /*释放内存*/
        AT_ModemFreeUlDataBuf(ucIndex, pstData);
        return AT_FAILURE;
    }

    /* 从pstData(IMM_ZC_STRU类型)中取出数据内容和长度，分别保存在pData和usLen中 */
    pData = pstData->data;
    usLen = (VOS_UINT16)pstData->len;

    if ( AT_CMD_MODE == gastAtClientTab[ucIndex].Mode )
    {
        /*若Modem通道已经切入命令态，但此时仍然收到PPP帧，则直接丢弃*/
        if ((usLen > 0) && (0x7e == pData[0]) && (0x7e == pData[usLen - 1]))
        {
            /*释放BSP内存*/
            AT_ModemFreeUlDataBuf(ucIndex, pstData);
            return AT_SUCCESS;
        }

        ulRet = At_CmdStreamPreProc(ucIndex,pData,usLen);

        /*释放BSP内存*/
        AT_ModemFreeUlDataBuf(ucIndex, pstData);
        return ulRet;
    }

    /* 根据modem口的状态进行分发*/
    switch ( gastAtClientTab[ucIndex].DataMode )
    {
        case AT_PPP_DATA_MODE:

            /* (AT2D17549)规避MAC 10.6.2系统拨号挂断失败问题.参照标杆的方式，
               若接收数据为"+++"，则模拟拉底DTR信号的处理方式
            */
            if (3 == usLen)
            {
                if (('+' == pData[0]) && ('+' == pData[1]) && ('+' == pData[2]))
                {
                    /*模拟拉底DTR信号*/
                    VOS_MemSet(&stMscStru, 0x00, sizeof(stMscStru));
                    stMscStru.OP_Dtr = 1;
                    stMscStru.ucDtr  = 0;
                    At_ModemMscInd(ucIndex, ucDlci, &stMscStru);
                    break;
                }
            }
            /* PPP负责释放上行内存 */
            PPP_PullPacketEvent(gastAtClientTab[ucIndex].usPppId, pstData);
            return AT_SUCCESS;

        case AT_IP_DATA_MODE:
            if (3 == usLen)
            {
                if (('+' == pData[0]) && ('+' == pData[1]) && ('+' == pData[2]))
                {
                    /*模拟拉底DTR信号*/
                    VOS_MemSet(&stMscStru, 0x00, sizeof(stMscStru));
                    stMscStru.OP_Dtr = 1;
                    stMscStru.ucDtr  = 0;
                    At_ModemMscInd(ucIndex, ucDlci, &stMscStru);
                    break;
                }
            }
            /* PPP负责释放上行内存 */
            PPP_PullRawDataEvent(gastAtClientTab[ucIndex].usPppId, pstData);
            return AT_SUCCESS;

        /* Modified by s62952 for AT Project，2011-10-17,  Begin*/
        case AT_CSD_DATA_MODE:
#if(FEATURE_ON == FEATURE_CSD)
            CSD_UL_SendData(pstData);
            return AT_SUCCESS;
#endif
         /* Modified by s62952 for AT Project，2011-10-17,  end*/

        default:
            AT_WARN_LOG("At_ModemDataInd: DataMode Wrong!");
            break;
    }

    /*释放内存*/
    AT_ModemFreeUlDataBuf(ucIndex, pstData);
    /* Modified by L60609 for PS Project，2011-12-06,  End*/
    return AT_SUCCESS;
}


VOS_UINT32 AT_ModemInitUlDataBuf(
    VOS_UINT8                           ucIndex,
    VOS_UINT32                          ulEachBuffSize,
    VOS_UINT32                          ulTotalBuffNum
)
{
    ACM_READ_BUFF_INFO                  stReadBuffInfo;
    VOS_INT32                           ulResult;


    /* 填写需要释放的内存指针 */
    stReadBuffInfo.u32BuffSize = ulEachBuffSize;
    stReadBuffInfo.u32BuffNum  = ulTotalBuffNum;

    ulResult= mdrv_udi_ioctl(g_alAtUdiHandle[ucIndex], ACM_IOCTL_RELLOC_READ_BUFF, &stReadBuffInfo);

    if ( VOS_OK != ulResult )
    {
        AT_ERR_LOG1("AT_ModemInitUlDataBuf, WARNING, Initialize data buffer failed code %d!\r\n",
                  ulResult);

        return AT_FAILURE;
    }

    return AT_SUCCESS;
}
VOS_UINT32 AT_ModemFreeUlDataBuf(
    VOS_UINT8                           ucIndex,
    IMM_ZC_STRU                        *pstBuf
)
{
    ACM_WR_ASYNC_INFO                   stCtlParam;
    VOS_INT32                           ulResult;

    /* Modified by L60609 for PS Project，2011-12-06,  Begin*/
    /* 填写需要释放的内存指针 */
    stCtlParam.pVirAddr = (VOS_CHAR*)pstBuf;
    stCtlParam.pPhyAddr = VOS_NULL_PTR;
    stCtlParam.u32Size  = 0;
    stCtlParam.pDrvPriv = VOS_NULL_PTR;
    /* Modified by L60609 for PS Project，2011-12-06,  End*/

    ulResult = mdrv_udi_ioctl(g_alAtUdiHandle[ucIndex], ACM_IOCTL_RETURN_BUFF, &stCtlParam);

    if ( VOS_OK != ulResult )
    {
        AT_ERR_LOG1("AT_ModemFreeUlDataBuf, ERROR, Return modem buffer failed, code %d!\r\n",
                  ulResult);
        AT_MODEM_DBG_UL_RETURN_BUFF_FAIL_NUM(1);
        return AT_FAILURE;
    }

    AT_MODEM_DBG_UL_RETURN_BUFF_SUCC_NUM(1);

    return AT_SUCCESS;
}

/*****************************************************************************
函 数 名  : AT_ModemFreeDlDataBuf
功能描述  : 释放PPP模块下行数据内存，注册给底软使用
输入参数  : pstBuf     ----      上行数据指针
输出参数  : 无
返 回 值  : AT_SUCCESS ----      成功；
            AT_FAILURE ----      失败
调用函数  :
被调函数  :

修改历史      :
 1.日    期   : 2011年10月17日
   作    者   : sunshaohua
   修改内容   : 新生成函数
 2.日    期   : 2011年12月8日
   作    者   : 鲁琳/l60609
   修改内容   : Buf类型修改为IMM_ZC_STRU
*****************************************************************************/
VOS_VOID AT_ModemFreeDlDataBuf(
    IMM_ZC_STRU                        *pstBuf
)
{
    AT_MODEM_DBG_DL_FREE_BUFF_NUM(1);

    /* Modified by L60609 for PS Project，2011-12-06,  Begin*/
    /* 释放pstBuf */
    IMM_ZcFree(pstBuf);
    /* Modified by L60609 for PS Project，2011-12-06,  End*/
    return;
}
VOS_UINT32 AT_ModemWriteData(
    VOS_UINT8                           ucIndex,
    IMM_ZC_STRU                        *pstBuf
)
{
    ACM_WR_ASYNC_INFO                   stCtlParam;
    VOS_INT32                           ulResult;

    /* 待写入数据内存地址 */
    stCtlParam.pVirAddr                 = (VOS_CHAR*)pstBuf;
    stCtlParam.pPhyAddr                 = VOS_NULL_PTR;
    stCtlParam.u32Size                  = 0;
    stCtlParam.pDrvPriv                 = VOS_NULL_PTR;

    if (UDI_INVALID_HANDLE == g_alAtUdiHandle[ucIndex])
    {
        AT_ModemFreeDlDataBuf(pstBuf);
        return AT_FAILURE;
    }

    /* 异步方式写数，*/
    ulResult = mdrv_udi_ioctl(g_alAtUdiHandle[ucIndex], ACM_IOCTL_WRITE_ASYNC, &stCtlParam);

    if (VOS_OK != ulResult)
    {
        AT_WARN_LOG("AT_ModemWriteData: Write data failed with code!\r\n");
        AT_MODEM_DBG_DL_WRITE_ASYNC_FAIL_NUM(1);
        AT_ModemFreeDlDataBuf(pstBuf);
        return AT_FAILURE;
    }

    AT_MODEM_DBG_DL_WRITE_ASYNC_SUCC_NUM(1);

    return AT_SUCCESS;
}


VOS_UINT32 AT_SendDataToModem(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                          *pucDataBuf,
    VOS_UINT16                          usLen
)
{
    IMM_ZC_STRU                        *pstData;
    VOS_CHAR                           *pstZcPutData;

    pstData = VOS_NULL_PTR;

    /* Modified by L60609 for PS Project, 2011-12-06,  Begin*/
    pstData = IMM_ZcStaticAlloc((VOS_UINT16)usLen);

    if (VOS_NULL_PTR == pstData)
    {
        return AT_FAILURE;
    }

    /*此步骤不能少，用来偏移数据尾指针*/
    /* Modified by l60609 for AP适配项目 ，2012-08-30 Begin */
    pstZcPutData = (VOS_CHAR *)IMM_ZcPut(pstData, usLen);
    /* Modified by l60609 for AP适配项目 ，2012-08-30 End */

    PS_MEM_CPY(pstZcPutData, pucDataBuf, usLen);

    /*将数据写往MODEM设备，写成功后内存由底软负责释放*/
    if (AT_SUCCESS != AT_ModemWriteData(ucIndex, pstData))
    {
        return AT_FAILURE;
    }
    /* Modified by L60609 for PS Project, 2011-12-06,  End*/

    return AT_SUCCESS;
}


VOS_UINT32 AT_SendZcDataToModem(
    VOS_UINT16                          usPppId,
    IMM_ZC_STRU                        *pstDataBuf
)
{
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    ucIndex = gastAtPppIndexTab[usPppId];

    switch (gastAtClientTab[ucIndex].UserType)
    {
        case AT_MODEM_USER:
            ulResult = AT_ModemWriteData(ucIndex, pstDataBuf);
            break;

#if (FEATURE_ON == FEATURE_AT_HSUART)
        case AT_HSUART_USER:
            ulResult = AT_HSUART_WriteDataAsync(ucIndex, pstDataBuf);
            break;
#endif

        default:
            ulResult = AT_SUCCESS;
            break;
    }

    return ulResult;
}


VOS_UINT32 AT_SendCsdZcDataToModem(
    VOS_UINT8                           ucIndex,
    IMM_ZC_STRU                        *pstDataBuf
)
{
    /*将数据写往MODEM设备，写成功后内存由底软负责释放*/
    if (AT_SUCCESS != AT_ModemWriteData(ucIndex, pstDataBuf))
    {
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}
VOS_VOID AT_UsbModemEnableCB(PS_BOOL_ENUM_UINT8 ucEnable)
{
    VOS_UINT8                           ucIndex;

    ucIndex = AT_CLIENT_TAB_MODEM_INDEX;

    AT_ModemeEnableCB(ucIndex, ucEnable);

    return;
}


VOS_VOID AT_UsbModemReadDataCB( VOS_VOID )
{
    VOS_UINT8                           ucIndex;
    VOS_UINT8                           ucDlci;
    IMM_ZC_STRU                        *pstBuf;

    pstBuf          = VOS_NULL_PTR;

    /* HSIC MODEM索引号 */
    ucIndex     = AT_CLIENT_TAB_MODEM_INDEX;

    AT_MODEM_DBG_UL_DATA_READ_CB_NUM(1);

    /* Modified by L60609 for PS Project，2011-12-06,  Begin*/
    if (AT_SUCCESS == AT_ModemGetUlDataBuf(ucIndex, &pstBuf))
    {

        /*MODEM链路号 */
        ucDlci      = AT_MODEM_USER_DLCI;

        /* 根据设备当前模式，分发上行数据 */
        At_ModemDataInd(ucIndex, ucDlci, pstBuf);
    }
    /* Modified by L60609 for PS Project，2011-12-06,  End*/

    return;
}
VOS_VOID AT_UsbModemReadMscCB(AT_DCE_MSC_STRU *pstRcvedMsc)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT8                           ucDlci;

    if (VOS_NULL_PTR == pstRcvedMsc)
    {
        AT_WARN_LOG("AT_UsbModemReadMscCB, WARNING, Receive NULL pointer MSC info!");

        return;
    }

    /* MODEM索引号 */
    ucIndex     = AT_CLIENT_TAB_MODEM_INDEX;

    /*MODEM链路号 */
    ucDlci      = AT_MODEM_USER_DLCI;

     /* 输入管脚信号可维可测 */
    AT_MNTN_TraceInputMsc(ucIndex, pstRcvedMsc);

    At_ModemMscInd(ucIndex, ucDlci, pstRcvedMsc);

    return;
}
VOS_VOID AT_UsbModemInit( VOS_VOID )
{
    UDI_OPEN_PARAM_S                    stParam;
    VOS_UINT8                           ucIndex;

    ucIndex         = AT_CLIENT_TAB_MODEM_INDEX;
    stParam.devid   = UDI_ACM_MODEM_ID;

    /* 打开Device，获得ID */
    g_alAtUdiHandle[ucIndex] = mdrv_udi_open(&stParam);

    if (UDI_INVALID_HANDLE == g_alAtUdiHandle[ucIndex])
    {
        AT_ERR_LOG("AT_UsbModemInit, ERROR, Open usb modem device failed!");

        return;
    }

    /* 注册MODEM设备上行数据接收回调 */
    if (VOS_OK != mdrv_udi_ioctl (g_alAtUdiHandle[ucIndex], ACM_IOCTL_SET_READ_CB, AT_UsbModemReadDataCB))
    {
        AT_ERR_LOG("AT_UsbModemInit, ERROR, Set data read callback for modem failed!");

        return;
    }

    /* 注册MODEM下行数据内存释放接口 */
    if (VOS_OK != mdrv_udi_ioctl (g_alAtUdiHandle[ucIndex], ACM_IOCTL_SET_FREE_CB, AT_ModemFreeDlDataBuf))
    {
        AT_ERR_LOG("AT_UsbModemInit, ERROR, Set memory free callback for modem failed!");

        return;
    }

    /* 注册管脚信号通知回调 */
    if (VOS_OK != mdrv_udi_ioctl (g_alAtUdiHandle[ucIndex], ACM_MODEM_IOCTL_SET_MSC_READ_CB, AT_UsbModemReadMscCB))
    {
        AT_ERR_LOG("AT_UsbModemInit, ERROR, Set msc read callback for modem failed!");

        return;
    }

    /* 注册MODEM设备使能、去使能通知回调 */
    if (VOS_OK != mdrv_udi_ioctl (g_alAtUdiHandle[ucIndex], ACM_MODEM_IOCTL_SET_REL_IND_CB, AT_UsbModemEnableCB))
    {
        AT_ERR_LOG("AT_UsbModemInit, ERROR, Set enable callback for modem failed!");

        return;
    }

    /* 设置MODEM设备上行数据buffer规格 */
    AT_ModemInitUlDataBuf(ucIndex, AT_MODEM_UL_DATA_BUFF_SIZE, AT_MODEM_UL_DATA_BUFF_NUM);

    /* 初始化MODME统计信息 */
    AT_InitModemStats();

    /*注册client id*/
    At_ModemEst(ucIndex, AT_CLIENT_ID_MODEM, AT_USB_MODEM_PORT_NO);

    AT_ConfigTraceMsg(ucIndex, ID_AT_CMD_MODEM, ID_AT_MNTN_RESULT_MODEM);

    return;
}


VOS_VOID AT_UsbModemClose(VOS_VOID)
{
    AT_CLIENT_TAB_INDEX_UINT8           ucIndex;

    ucIndex = AT_CLIENT_TAB_MODEM_INDEX;

    /* 去注册MODEM流控点(经TTF确认未注册流控点也可以去注册流控点)。 */
    AT_DeRegModemPsDataFCPoint(ucIndex, gastAtClientTab[ucIndex].ucPsRabId);

    if (UDI_INVALID_HANDLE != g_alAtUdiHandle[ucIndex])
    {
        mdrv_udi_close(g_alAtUdiHandle[ucIndex]);

        g_alAtUdiHandle[ucIndex] = UDI_INVALID_HANDLE;

        vos_printf("AT_UsbModemClose....\n");
    }

    return;
}
VOS_VOID AT_SetUsbDebugFlag(VOS_UINT32 ulFlag)
{
    g_ulAtUsbDebugFlag = ulFlag;
}

/*****************************************************************************
 Prototype      : At_RcvFromUsbCom
 Description    : AT注册给USB COM驱动的回调函数，用于从串口中获取数据
 Input          : ucPortNo     --    端口号
                  pucData      --    AT从串口接收的数据指针
                  uslength　   --    AT从串口接收的数据长度
 Output         : ---
 Return Value   : AT_DRV_FAILURE --- 成功
                  AT_DRV_FAILURE --- 失败
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2008-06-09
    Author      : L47619
    Modification: Created function
  2.Date        : 2012-02-24
    Author      : L47619
    Modification: V7R1C50 IPC项目:增加HSIC AT通道处理AT命令的逻辑
  3.日    期   : 2012年8月3日
    作    者   : L60609
    修改内容   : MUX项目：增加MUX AT通道处理AT命令的逻辑
*****************************************************************************/

VOS_INT At_RcvFromUsbCom(
    VOS_UINT8                           ucPortNo,
    VOS_UINT8                          *pData,
    VOS_UINT16                          uslength
)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulRet;
    /* Added by L60609 for MUX，2012-08-08,  Begin */
    VOS_UINT32                          ulMuxUserFlg;
    VOS_UINT32                          ulHsicUserFlg;
    /* Added by L60609 for MUX，2012-08-08,  End */

    if (VOS_TRUE == g_ulAtUsbDebugFlag)
    {
        vos_printf("At_RcvFromUsbCom: PortNo = %d, length = %d, data = %s\r\n", ucPortNo, uslength, pData);
    }

    if (VOS_NULL_PTR == pData)
    {
        AT_WARN_LOG("At_RcvFromUsbCom: pData is NULL PTR!");
        return AT_DRV_FAILURE;
    }

    if (0 == uslength)
    {
        AT_WARN_LOG("At_RcvFromUsbCom: uslength is 0!");
        return AT_DRV_FAILURE;
    }

    /*PCUI和CTRL共用*/
    for (ucIndex = 0; ucIndex < AT_MAX_CLIENT_NUM; ucIndex++)
    {
        /* Modified by L60609 for MUX，2012-08-03,  Begin */
        ulMuxUserFlg = AT_CheckMuxUser(ucIndex);
        ulHsicUserFlg = AT_CheckHsicUser(ucIndex);

        if ((AT_USBCOM_USER == gastAtClientTab[ucIndex].UserType)
         || (AT_CTR_USER == gastAtClientTab[ucIndex].UserType)
         || (AT_UART_USER == gastAtClientTab[ucIndex].UserType)
         || (VOS_TRUE == ulMuxUserFlg)
         || (VOS_TRUE == ulHsicUserFlg))
        {
            if (AT_CLIENT_NULL != gastAtClientTab[ucIndex].ucUsed)
            {
                if (gastAtClientTab[ucIndex].ucPortNo == ucPortNo)
                {
                    break;
                }
            }
        }
        /* Modified by L60609 for MUX，2012-08-03,  End */
    }

    if (VOS_TRUE == g_ulAtUsbDebugFlag)
    {
        vos_printf("At_RcvFromUsbCom: ucIndex = %d\r\n", ucIndex);
    }

    if (ucIndex >= AT_MAX_CLIENT_NUM)
    {
        AT_WARN_LOG("At_RcvFromUsbCom (ucIndex >= AT_MAX_CLIENT_NUM)");
        return AT_DRV_FAILURE;
    }

    if (VOS_TRUE == g_ulAtUsbDebugFlag)
    {
        vos_printf("At_RcvFromUsbCom: CmdMode = %d\r\n", gastAtClientTab[ucIndex].Mode);
    }

    if (AT_CMD_MODE == gastAtClientTab[ucIndex].Mode)
    {
        ulRet = At_CmdStreamPreProc(ucIndex,pData,uslength);
    }
    else
    {
        ulRet = At_DataStreamPreProc(ucIndex,gastAtClientTab[ucIndex].DataMode,pData,uslength);
    }

    if ( AT_SUCCESS == ulRet )
    {
        return AT_DRV_SUCCESS;
    }
    else
    {
        return AT_DRV_FAILURE;
    }
}
VOS_UINT32 At_UsbPcuiEst(VOS_UINT8 ucPortNo)
{
    /* Modified by L60609 for AT Project，2011-10-04,  Begin*/
    VOS_UINT8                           ucIndex;

    if (AT_USB_COM_PORT_NO != ucPortNo)
    {
        AT_WARN_LOG("At_UsbPcuiEst the PortNo is error)");
        return VOS_ERR;
    }

    ucIndex = AT_CLIENT_TAB_PCUI_INDEX;

    /* 清空对应表项 */
    PS_MEM_SET(&gastAtClientTab[ucIndex],0x00,sizeof(AT_CLIENT_MANAGE_STRU));

    AT_ConfigTraceMsg(ucIndex, ID_AT_CMD_PCUI, ID_AT_MNTN_RESULT_PCUI);

    /* 填写用户表项 */
    gastAtClientTab[ucIndex].usClientId      = AT_CLIENT_ID_PCUI;
    gastAtClientTab[ucIndex].ucPortNo        = ucPortNo;
    gastAtClientTab[ucIndex].UserType        = AT_USBCOM_USER;
    gastAtClientTab[ucIndex].ucUsed          = AT_CLIENT_USED;

    /* 以下可以不用填写，前面PS_MEMSET已经初始化，只为可靠起见 */
    gastAtClientTab[ucIndex].Mode            = AT_CMD_MODE;
    gastAtClientTab[ucIndex].IndMode         = AT_IND_MODE;
    gastAtClientTab[ucIndex].DataMode        = AT_DATA_BUTT_MODE;
    gastAtClientTab[ucIndex].DataState       = AT_DATA_STOP_STATE;
    gastAtClientTab[ucIndex].CmdCurrentOpt   = AT_CMD_CURRENT_OPT_BUTT;
    g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_READY;

    /* Modified by L60609 for AT Project，2011-10-14,  Begin*/
    #if (VOS_WIN32 == VOS_OS_VER)
    Sock_RecvCallbackRegister(ucPortNo, (pComRecv)At_RcvFromUsbCom);
    #else
    /*向DMS注册从串口中获取数据的回调函数*/
    (VOS_VOID)DMS_COM_RCV_CALLBACK_REGI(ucPortNo, (pComRecv)At_RcvFromUsbCom);
    #endif

#if 0
    /* 向DMS注册串口从使能到去使能状态时通知上层的回调函数 */
    DMS_COM_STATUS_CALLBACK_REGI(AT_USB_COM_PORT_NO, (pComStatusNotify)At_NotifyStatusChgFromUsbCom);
#endif

    /* Modified by L60609 for AT Project，2011-10-14,  End*/

    AT_LOG1("At_UsbPcuiEst ucIndex:",ucIndex);

    /* Modified by L60609 for AT Project，2011-10-04,  End*/
    return VOS_OK;
}



VOS_UINT32 At_UsbCtrEst(VOS_UINT8 ucPortNo)
{
    /* Modified by L60609 for AT Project，2011-10-04,  Begin*/
    VOS_UINT8                           ucIndex;


    if (AT_CTR_PORT_NO != ucPortNo)
    {
        AT_WARN_LOG("At_UsbCtrEst the PortNo is error)");
        return VOS_ERR;
    }

    ucIndex = AT_CLIENT_TAB_CTRL_INDEX;

    /* 清空对应表项 */
    PS_MEM_SET(&gastAtClientTab[ucIndex], 0x00, sizeof(AT_CLIENT_MANAGE_STRU));

    AT_ConfigTraceMsg(ucIndex, ID_AT_CMD_CTRL, ID_AT_MNTN_RESULT_CTRL);

    /* 填写用户表项 */
    gastAtClientTab[ucIndex].usClientId      = AT_CLIENT_ID_CTRL;
    gastAtClientTab[ucIndex].ucPortNo        = ucPortNo;
    gastAtClientTab[ucIndex].UserType        = AT_CTR_USER;
    gastAtClientTab[ucIndex].ucUsed          = AT_CLIENT_USED;

    /* 以下可以不用填写，前面PS_MEMSET已经初始化，只为可靠起见 */
    gastAtClientTab[ucIndex].Mode            = AT_CMD_MODE;
    gastAtClientTab[ucIndex].IndMode         = AT_IND_MODE;
    gastAtClientTab[ucIndex].DataMode        = AT_DATA_BUTT_MODE;
    gastAtClientTab[ucIndex].DataState       = AT_DATA_STOP_STATE;
    gastAtClientTab[ucIndex].CmdCurrentOpt   = AT_CMD_CURRENT_OPT_BUTT;
    g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_READY;


    /*向底软DMS从串口中获取数据的回调函数*/
    (VOS_VOID)DMS_COM_RCV_CALLBACK_REGI(ucPortNo, (pComRecv)At_RcvFromUsbCom);
    /* Modified by L60609 for AT Project，2011-10-14,  End*/

    return VOS_OK;
}


VOS_UINT32 AT_UART_GetUlDataBuff(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                         **ppucData,
    VOS_UINT32                         *pulLen
)
{
    ACM_WR_ASYNC_INFO                   stCtlParam;
    UDI_HANDLE                          lUdiHandle;
    VOS_INT32                           lResult;

    lUdiHandle = g_alAtUdiHandle[ucIndex];
    if (UDI_INVALID_HANDLE == lUdiHandle)
    {
        AT_ERR_LOG("AT_UART_GetUlDataBuff: Invalid UDI handle!\r\n");
        return AT_FAILURE;
    }

    /* 获取底软上行数据BUFFER */
    stCtlParam.pVirAddr = VOS_NULL_PTR;
    stCtlParam.pPhyAddr = VOS_NULL_PTR;
    stCtlParam.u32Size  = 0;
    stCtlParam.pDrvPriv = VOS_NULL_PTR;

    lResult = mdrv_udi_ioctl(lUdiHandle, UART_IOCTL_GET_RD_BUFF, &stCtlParam);
    if (VOS_OK != lResult)
    {
        AT_ERR_LOG("AT_UART_GetUlDataBuff: Get buffer failed!\r\n");
        return AT_FAILURE;
    }

    if ( (VOS_NULL_PTR == stCtlParam.pVirAddr)
      || (AT_INIT_DATA_LEN == stCtlParam.u32Size) )
    {
        AT_ERR_LOG("AT_UART_GetUlDataBuff: Data buffer error!\r\n");
        return AT_FAILURE;
    }

    *ppucData = (VOS_UINT8 *)stCtlParam.pVirAddr;
    *pulLen   = stCtlParam.u32Size;

    return AT_SUCCESS;
}
VOS_UINT32 AT_UART_WriteDataSync(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                          *pucData,
    VOS_UINT32                          ulLen
)
{
    UDI_HANDLE                          lUdiHandle;
    VOS_INT32                           lResult;

    /* 检查UDI句柄有效性 */
    lUdiHandle = g_alAtUdiHandle[ucIndex];
    if (UDI_INVALID_HANDLE == lUdiHandle)
    {
        AT_ERR_LOG("AT_UART_WriteDataSync: Invalid UDI handle!\r\n");
        return AT_FAILURE;
    }

    /* 检查数据有效性 */
    if ((VOS_NULL_PTR == pucData) || (0 == ulLen))
    {
        AT_ERR_LOG("AT_UART_WriteDataSync: DATA is invalid!\r\n");
        return AT_FAILURE;
    }

    lResult = mdrv_udi_write(lUdiHandle, pucData, ulLen);
    if (VOS_OK != lResult)
    {
        AT_HSUART_DBG_DL_WRITE_SYNC_FAIL_LEN(ulLen);
        AT_ERR_LOG("AT_UART_WriteDataSync: Write buffer failed!\r\n");
        return AT_FAILURE;
    }

    AT_HSUART_DBG_DL_WRITE_SYNC_SUCC_LEN(ulLen);

    return AT_SUCCESS;
}
VOS_UINT32 AT_UART_SendDlData(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                          *pucData,
    VOS_UINT16                          usLen
)
{
    /* 同步写UART设备, 数据无需释放 */
    return AT_UART_WriteDataSync(ucIndex, pucData, usLen);
}
VOS_UINT32 AT_UART_SendRawDataFromOm(
    VOS_UINT8                          *pucVirAddr,
    VOS_UINT8                          *pucPhyAddr,
    VOS_UINT32                          ulLen
)
{
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    ucIndex  = AT_CLIENT_TAB_UART_INDEX;

    ulResult = AT_UART_WriteDataSync(ucIndex, pucVirAddr, (VOS_UINT16)ulLen);
    if (AT_SUCCESS != ulResult)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : AT_HSUART_UlDataReadCB
 功能描述  : UART设备数据读回调，注册给底软
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年10月18日
    作    者   : sunshaohua
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID AT_UART_UlDataReadCB(VOS_VOID)
{
    VOS_UINT8                          *pucData = VOS_NULL_PTR;
    VOS_UINT32                          ulLen;
    VOS_UINT8                           ucIndex;

    ulLen   = 0;
    ucIndex = AT_CLIENT_TAB_UART_INDEX;

    if (AT_SUCCESS == AT_UART_GetUlDataBuff(ucIndex, &pucData, &ulLen))
    {
        /* 根据设备当前模式，分发上行数据 */
        At_RcvFromUsbCom(AT_UART_PORT_NO, pucData, (VOS_UINT16)ulLen);
    }

    return;
}
VOS_VOID AT_UART_InitLink(VOS_UINT8 ucIndex)
{
    TAF_AT_NVIM_DEFAULT_LINK_OF_UART_STRU    stDefaultLinkType;


    stDefaultLinkType.enUartLinkType = AT_UART_LINK_TYPE_BUTT;


    /* 清空对应表项 */
    PS_MEM_SET(&gastAtClientTab[ucIndex],0x00,sizeof(AT_CLIENT_MANAGE_STRU));

    gastAtClientTab[ucIndex].ucPortNo  = AT_UART_PORT_NO;
    gastAtClientTab[ucIndex].UserType  = AT_UART_USER;
    gastAtClientTab[ucIndex].ucUsed    = AT_CLIENT_USED;


    /* 读取UART端口默认工作模式NV项 */
    if (NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_DEFAULT_LINK_OF_UART,
                        &stDefaultLinkType.enUartLinkType,
                        sizeof(stDefaultLinkType.enUartLinkType)))
    {
        /* NV项读取失败，将UART端口的工作模式设置为OM模式 */
        AT_ERR_LOG("AT_UART_InitLink:Read NV failed!");

        /*记录AT/OM通道所对应的索引号*/
        gucAtOmIndex = ucIndex;

        /* 切换至OM数传模式 */
        At_SetMode(ucIndex, AT_DATA_MODE, AT_OM_DATA_MODE);
        gastAtClientTab[ucIndex].DataState = AT_DATA_START_STATE;

        AT_AddUsedClientId2Tab(AT_CLIENT_TAB_UART_INDEX);

        /* 通知OAM切换UART至OM模式 */
        CBTCPM_NotifyChangePort(AT_UART_PORT);
    }
    else
    {
        /* NV读取成功，检查UART端口的默认工作模式 */
        if (AT_UART_LINK_TYPE_AT != stDefaultLinkType.enUartLinkType)
        {
            AT_NORM_LOG("AT_UART_InitLink:DEFAULT UART LINK TYPE is OM!");

            /*记录AT/OM通道所对应的索引号*/
            gucAtOmIndex = ucIndex;

            /* 切换至OM数传模式 */
            At_SetMode(ucIndex, AT_DATA_MODE, AT_OM_DATA_MODE);
            gastAtClientTab[ucIndex].DataState = AT_DATA_START_STATE;

            AT_AddUsedClientId2Tab(AT_CLIENT_TAB_UART_INDEX);

            /* 通知OAM切换UART至OM模式 */
            CBTCPM_NotifyChangePort(AT_UART_PORT);
        }
        else
        {
            /* 填写用户表项 */
            gastAtClientTab[ucIndex].usClientId      = AT_CLIENT_ID_UART;

            /* 以下可以不用填写，前面PS_MEMSET已经初始化，只为可靠起见 */
            gastAtClientTab[ucIndex].Mode            = AT_CMD_MODE;
            gastAtClientTab[ucIndex].IndMode         = AT_IND_MODE;
            gastAtClientTab[ucIndex].DataMode        = AT_DATA_BUTT_MODE;
            gastAtClientTab[ucIndex].DataState       = AT_DATA_STOP_STATE;
            gastAtClientTab[ucIndex].CmdCurrentOpt   = AT_CMD_CURRENT_OPT_BUTT;
            g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_READY;
        }
    }

    return;
}


VOS_VOID AT_UART_InitPort(VOS_VOID)
{
    UDI_OPEN_PARAM_S                    stParam;
    UDI_HANDLE                          lUdiHandle;
    VOS_UINT8                           ucIndex;

    stParam.devid = UDI_UART_0_ID;
    ucIndex       = AT_CLIENT_TAB_UART_INDEX;

    AT_ConfigTraceMsg(ucIndex, ID_AT_CMD_UART, ID_AT_MNTN_RESULT_UART);

    lUdiHandle = mdrv_udi_open(&stParam);
    if (UDI_INVALID_HANDLE != lUdiHandle)
    {
        /* 注册UART设备上行数据接收回调 */
        if (VOS_OK != mdrv_udi_ioctl (lUdiHandle, UART_IOCTL_SET_READ_CB, AT_UART_UlDataReadCB))
        {
            AT_ERR_LOG("AT_UART_InitPort: Reg data read callback failed!\r\n");
        }

        /* 初始化UART链路 */
        AT_UART_InitLink(ucIndex);
        g_alAtUdiHandle[ucIndex] = lUdiHandle;
    }
    else
    {
        AT_ERR_LOG("AT_UART_InitPort: Open UART device failed!\r\n");
        g_alAtUdiHandle[ucIndex] = UDI_INVALID_HANDLE;
    }

    return;
}

#if (FEATURE_ON == FEATURE_AT_HSUART)

VOS_UINT32 AT_HSUART_IsBaudRateValid(AT_UART_BAUDRATE_ENUM_UINT32 enBaudRate)
{
    VOS_UINT32                          ulRet = VOS_FALSE;

    switch (enBaudRate)
    {
        case AT_UART_BAUDRATE_0:
        case AT_UART_BAUDRATE_300:
        case AT_UART_BAUDRATE_600:
        case AT_UART_BAUDRATE_1200:
        case AT_UART_BAUDRATE_2400:
        case AT_UART_BAUDRATE_4800:
        case AT_UART_BAUDRATE_9600:
        case AT_UART_BAUDRATE_19200:
        case AT_UART_BAUDRATE_38400:
        case AT_UART_BAUDRATE_57600:
        case AT_UART_BAUDRATE_115200:
        case AT_UART_BAUDRATE_230400:
        case AT_UART_BAUDRATE_460800:
        case AT_UART_BAUDRATE_921600:
        case AT_UART_BAUDRATE_1152000:
        case AT_UART_BAUDRATE_1500000:
        case AT_UART_BAUDRATE_2000000:
        case AT_UART_BAUDRATE_3000000:
            ulRet = VOS_TRUE;
            break;
        default:
            ulRet = VOS_FALSE;
            break;
    }

    return ulRet;
}
VOS_UINT32 AT_HSUART_IsFormatValid(AT_UART_FORMAT_ENUM_UINT8 enFormat)
{
    VOS_UINT32                          ulRet = VOS_FALSE;

    switch (enFormat)
    {
        case AT_UART_FORMAT_AUTO_DETECT:
        case AT_UART_FORMAT_8DATA_2STOP:
        case AT_UART_FORMAT_8DATA_1PARITY_1STOP:
        case AT_UART_FORMAT_8DATA_1STOP:
        case AT_UART_FORMAT_7DATA_2STOP:
        case AT_UART_FORMAT_7DATA_1PARITY_1STOP:
        case AT_UART_FORMAT_7DATA_1STOP:
            ulRet = VOS_TRUE;
            break;

        default:
            ulRet = VOS_FALSE;
            break;
    }

    return ulRet;
}


VOS_UINT32 AT_HSUART_IsParityValid(AT_UART_PARITY_ENUM_UINT8 enParity)
{
    VOS_UINT32                          ulRet = VOS_FALSE;

    switch (enParity)
    {
        case AT_UART_PARITY_ODD:
        case AT_UART_PARITY_EVEN:
        case AT_UART_PARITY_MARK:
        case AT_UART_PARITY_SPACE:
        case AT_UART_PARITY_NONE:
            ulRet = VOS_TRUE;
            break;

        default:
            ulRet = VOS_FALSE;
            break;
    }

    return ulRet;
}


AT_UART_FORMAT_PARAM_STRU *AT_HSUART_GetFormatParam(
    AT_UART_FORMAT_ENUM_UINT8           enFormat
)
{
    AT_UART_FORMAT_PARAM_STRU          *pstFormatTblPtr = VOS_NULL_PTR;
    AT_UART_FORMAT_PARAM_STRU          *pstFormatParam  = VOS_NULL_PTR;
    VOS_UINT32                          ulCnt;

    pstFormatTblPtr = AT_UART_GET_FORMAT_TBL_PTR();

    for (ulCnt = 0; ulCnt < AT_UART_GET_FORMAT_TBL_SIZE(); ulCnt++)
    {
        if (enFormat == pstFormatTblPtr[ulCnt].enFormat)
        {
            pstFormatParam = &pstFormatTblPtr[ulCnt];
        }
    }

    return pstFormatParam;
}


VOS_UINT32 AT_HSUART_GetUdiValueByDataLen(
    AT_UART_DATA_LEN_ENUM_UINT8         enDataLen,
    VOS_UINT32                         *pulUdiValue
)
{
    /* 指针非空检测 */
    if (VOS_NULL_PTR == pulUdiValue)
    {
        return VOS_ERR;
    }

    /* 映射底软UDI VALUE */
    switch (enDataLen)
    {
        case AT_UART_DATA_LEN_5_BIT:
            *pulUdiValue = WLEN_5_BITS;
            break;

        case AT_UART_DATA_LEN_6_BIT:
            *pulUdiValue = WLEN_6_BITS;
            break;

        case AT_UART_DATA_LEN_7_BIT:
            *pulUdiValue = WLEN_7_BITS;
            break;

        case AT_UART_DATA_LEN_8_BIT:
            *pulUdiValue = WLEN_8_BITS;
            break;

        default:
            return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 AT_HSUART_GetUdiValueByStopLen(
    AT_UART_STOP_LEN_ENUM_UINT8         enStopLen,
    VOS_UINT32                         *pulUdiValue
)
{
    /* 指针非空检测 */
    if (VOS_NULL_PTR == pulUdiValue)
    {
        return VOS_ERR;
    }

    /* 映射底软UDI VALUE */
    switch (enStopLen)
    {
        case AT_UART_STOP_LEN_1_BIT:
            *pulUdiValue = STP2_OFF;
            break;

        case AT_UART_STOP_LEN_2_BIT:
            *pulUdiValue = STP2_ON;
            break;

        default:
            return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 AT_HSUART_GetUdiValueByParity(
    AT_UART_PARITY_ENUM_UINT8           enParity,
    VOS_UINT32                         *pulUdiValue
)
{
    /* 指针非空检测 */
    if (VOS_NULL_PTR == pulUdiValue)
    {
        return VOS_ERR;
    }

    /* 映射底软UDI VALUE */
    switch (enParity)
    {
        case AT_UART_PARITY_ODD:
            *pulUdiValue = PARITY_CHECK_ODD;
            break;

        case AT_UART_PARITY_EVEN:
            *pulUdiValue = PARITY_CHECK_EVEN;
            break;

        case AT_UART_PARITY_MARK:
            *pulUdiValue = PARITY_CHECK_MARK;
            break;

        case AT_UART_PARITY_SPACE:
            *pulUdiValue = PARITY_CHECK_SPACE;
            break;

        case AT_UART_PARITY_NONE:
            *pulUdiValue = PARITY_NO_CHECK;
            break;

        default:
            return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 AT_HSUART_WriteMscCmd(
    VOS_UINT8                           ucIndex,
    AT_DCE_MSC_STRU                    *pstDceMsc
)
{
    UDI_HANDLE                          lUdiHandle;
    VOS_INT32                           lResult;

    /* 检查UDI句柄有效性 */
    lUdiHandle = g_alAtUdiHandle[ucIndex];
    if (UDI_INVALID_HANDLE == lUdiHandle)
    {
        AT_ERR_LOG("AT_HSUART_WriteMscCmd: Invalid UDI handle!\r\n");
        return AT_FAILURE;
    }

    /* 写管脚信号 */
    lResult = mdrv_udi_ioctl(lUdiHandle, UART_IOCTL_MSC_WRITE_CMD, pstDceMsc);
    if (VOS_OK != lResult)
    {
        AT_ERR_LOG("AT_HSUART_WriteMscCmd: Write failed!\r\n");
        AT_HSUART_DBG_IOCTL_MSC_WRITE_FAIL_NUM(1);
        return AT_FAILURE;
    }

    AT_HSUART_DBG_IOCTL_MSC_WRITE_SUCC_NUM(1);
    return AT_SUCCESS;
}


VOS_UINT32 AT_HSUART_FreeUlDataBuff(
    VOS_UINT8                           ucIndex,
    IMM_ZC_STRU                        *pstImmZc
)
{
    ACM_WR_ASYNC_INFO                   stCtlParam;
    UDI_HANDLE                          lUdiHandle;
    VOS_INT32                           lResult;

    /* 检查UDI句柄有效性 */
    lUdiHandle = g_alAtUdiHandle[ucIndex];
    if (UDI_INVALID_HANDLE == lUdiHandle)
    {
        AT_ERR_LOG("AT_HSUART_FreeUlDataBuff: Invalid UDI handle!");
        return AT_FAILURE;
    }

    /* 填写待释放的内存地址 */
    stCtlParam.pVirAddr = (VOS_CHAR *)pstImmZc;
    stCtlParam.pPhyAddr = VOS_NULL_PTR;
    stCtlParam.u32Size  = 0;
    stCtlParam.pDrvPriv = VOS_NULL_PTR;

    /* 底软执行释放内存操作 */
    lResult = mdrv_udi_ioctl(lUdiHandle, UART_IOCTL_RETURN_BUFF, &stCtlParam);
    if (VOS_OK != lResult)
    {
        AT_ERR_LOG1("AT_HSUART_FreeUlDataBuff: Return buffer failed! <code>\r\n", lResult);
        AT_HSUART_DBG_UL_RETURN_BUFF_FAIL_NUM(1);
        IMM_ZcFree(pstImmZc);
        return AT_FAILURE;
    }

    AT_HSUART_DBG_UL_RETURN_BUFF_SUCC_NUM(1);
    return AT_SUCCESS;
}
VOS_VOID AT_HSUART_FreeDlDataBuff(IMM_ZC_STRU *pstImmZc)
{
    if (VOS_NULL_PTR != pstImmZc)
    {
        AT_HSUART_DBG_DL_FREE_BUFF_NUM(1);
        IMM_ZcFree(pstImmZc);
    }

    return;
}


VOS_UINT32 AT_HSUART_GetUlDataBuff(
    VOS_UINT8                           ucIndex,
    IMM_ZC_STRU                       **pstImmZc,
    VOS_UINT32                         *pulLen
)
{
    ACM_WR_ASYNC_INFO                   stCtlParam;
    UDI_HANDLE                          lUdiHandle;
    VOS_INT32                           lResult;

    /* 检查UDI句柄有效性 */
    lUdiHandle = g_alAtUdiHandle[ucIndex];
    if (UDI_INVALID_HANDLE == lUdiHandle)
    {
        AT_ERR_LOG("AT_HSUART_GetUlDataBuff: Invalid UDI handle!\r\n");
        return AT_FAILURE;
    }

    /* 获取底软上行数据BUFFER */
    stCtlParam.pVirAddr = VOS_NULL_PTR;
    stCtlParam.pPhyAddr = VOS_NULL_PTR;
    stCtlParam.u32Size  = 0;
    stCtlParam.pDrvPriv = VOS_NULL_PTR;

    lResult = mdrv_udi_ioctl(lUdiHandle, UART_IOCTL_GET_RD_BUFF, &stCtlParam);
    if (VOS_OK != lResult)
    {
        AT_ERR_LOG1("AT_HSUART_GetUlDataBuff: Get buffer failed! <code>", lResult);
        AT_HSUART_DBG_UL_GET_RD_FAIL_NUM(1);
        return AT_FAILURE;
    }

    /* 数据有效性检查 */
    if ( (VOS_NULL_PTR == stCtlParam.pVirAddr)
      || (AT_INIT_DATA_LEN == stCtlParam.u32Size) )
    {
        AT_ERR_LOG("AT_HSUART_GetUlDataBuff: Data buffer error");
        AT_HSUART_DBG_UL_INVALID_RD_NUM(1);
        return AT_FAILURE;
    }

    AT_HSUART_DBG_UL_GET_RD_SUCC_NUM(1);

    *pstImmZc = (IMM_ZC_STRU *)stCtlParam.pVirAddr;
    *pulLen   = stCtlParam.u32Size;

    return AT_SUCCESS;
}
VOS_UINT32 AT_HSUART_WriteDataAsync(
    VOS_UINT8                           ucIndex,
    IMM_ZC_STRU                        *pstImmZc
)
{
    ACM_WR_ASYNC_INFO                   stCtlParam;
    UDI_HANDLE                          lUdiHandle;
    VOS_INT32                           ulResult;

    /* 检查UDI句柄有效性 */
    lUdiHandle = g_alAtUdiHandle[ucIndex];
    if (UDI_INVALID_HANDLE == lUdiHandle)
    {
        AT_ERR_LOG("AT_HSUART_WriteDataAsync: Invalid UDI handle!\r\n");
        AT_HSUART_FreeDlDataBuff(pstImmZc);
        return AT_FAILURE;
    }

    /* 待写入数据内存地址 */
    stCtlParam.pVirAddr = (VOS_CHAR *)pstImmZc;
    stCtlParam.pPhyAddr = VOS_NULL_PTR;
    stCtlParam.u32Size  = 0;
    stCtlParam.pDrvPriv = VOS_NULL_PTR;

    /* 异步方式写数 */
    ulResult = mdrv_udi_ioctl(g_alAtUdiHandle[ucIndex], UART_IOCTL_WRITE_ASYNC, &stCtlParam);
    if (VOS_OK != ulResult)
    {
        AT_ERR_LOG("AT_HSUART_WriteDataAsync: Write failed!\r\n");
        AT_HSUART_DBG_DL_WRITE_ASYNC_FAIL_NUM(1);
        AT_HSUART_FreeDlDataBuff(pstImmZc);
        return AT_FAILURE;
    }

    AT_HSUART_DBG_DL_WRITE_ASYNC_SUCC_NUM(1);
    return AT_SUCCESS;
}

#ifndef DMT
VOS_UINT32 AT_HSUART_SendDlData(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                          *pucData,
    VOS_UINT16                          usLen
)
{
    IMM_ZC_STRU                        *pstImmZc  = VOS_NULL_PTR;
    VOS_CHAR                           *pcPutData = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;

    /* 从A核数传内存中分配空间 */
    pstImmZc = IMM_ZcStaticAlloc((VOS_UINT16)usLen);
    if (VOS_NULL_PTR == pstImmZc)
    {
        return AT_FAILURE;
    }

    /* 偏移数据尾指针 */
    pcPutData = (VOS_CHAR *)IMM_ZcPut(pstImmZc, usLen);

    /* 拷贝数据 */
    PS_MEM_CPY(pcPutData, pucData, usLen);

    /* 异步写HSUART设备, 写成功后内存由底软负责释放 */
    ulResult = AT_HSUART_WriteDataAsync(ucIndex, pstImmZc);

    return ulResult;
}
#endif


VOS_VOID AT_HSUART_ProcUlData(
    VOS_UINT8                           ucIndex,
    IMM_ZC_STRU                        *pstImmZc
)
{
    VOS_UINT8                          *pucData = VOS_NULL_PTR;
    VOS_UINT16                          usLen;

    /* 从pstData(IMM_ZC_STRU类型)中取出数据内容和长度，分别保存在pData和usLen中 */
    pucData = pstImmZc->data;
    usLen   = (VOS_UINT16)pstImmZc->len;

    /* 如果当前处于命令模式 或者是 online_command模式 */
    if ( (AT_CMD_MODE == gastAtClientTab[ucIndex].Mode)
      || (AT_ONLINE_CMD_MODE == gastAtClientTab[ucIndex].Mode) )
    {
        /* 若UART通道已经切入命令态，但此时仍然收到PPP帧或者OM数据，则直接丢弃 */
        if ((usLen > 0) && (0x7e == pucData[0]) && (0x7e == pucData[usLen - 1]))
        {
            AT_HSUART_DBG_UL_INVALID_CMD_DATA_NUM(1);
            AT_HSUART_FreeUlDataBuff(ucIndex, pstImmZc);
            return;
        }

        AT_HSUART_DBG_UL_VALID_CMD_DATA_NUM(1);

        /* 若是AT命令，则分发送给 AT */
        if (AT_SUCCESS != At_CmdStreamPreProc(ucIndex, pucData, usLen))
        {
            AT_WARN_LOG("AT_HSUART_ProcUlData: At_CmdStreamPreProc fail!");
        }

        AT_HSUART_FreeUlDataBuff(ucIndex, pstImmZc);
        return;
    }

    /* 根据UART口的状态进行分发*/
    switch (gastAtClientTab[ucIndex].DataMode)
    {
        /* 处理PPP data数据 */
        case AT_PPP_DATA_MODE:

            /* PPP负责释放上行内存 */
            PPP_PullPacketEvent(gastAtClientTab[ucIndex].usPppId, pstImmZc);

            /* 记录可维可测 */
            AT_HSUART_DBG_UL_IP_DATA_NUM(1);
            return;

        /* 处理IP data 数据 */
        case AT_IP_DATA_MODE:

            /* PPP负责释放上行内存 */
            PPP_PullRawDataEvent(gastAtClientTab[ucIndex].usPppId, pstImmZc);

            /* 记录可维可测 */
            AT_HSUART_DBG_UL_PPP_DATA_NUM(1);
            return;

        /* 处理OM数据 */
        case AT_DIAG_DATA_MODE:
        case AT_OM_DATA_MODE:
            At_OmDataProc(gastAtClientTab[ucIndex].ucPortNo, pucData, usLen);

            /* 记录可维可测 */
            AT_HSUART_DBG_UL_OM_DATA_NUM(1);
            break;

#if(FEATURE_ON == FEATURE_CSD)
        /* 处理CSD数据 暂不实现*/
        case AT_CSD_DATA_MODE:
#endif
        default:
            AT_WARN_LOG("AT_HSUART_ProcUlData: DataMode Wrong!");
            AT_HSUART_DBG_UL_INVALID_DATA_NUM(1);
            break;
    }

    AT_HSUART_FreeUlDataBuff(ucIndex, pstImmZc);
    return;
}



VOS_UINT32 AT_UART_ProcDtrChange(
    VOS_UINT8                           ucIndex,
    AT_DCE_MSC_STRU                    *pstDceMsc
)
{
    AT_UART_LINE_CTRL_STRU             *pstLineCtrl = VOS_NULL_PTR;
    VOS_UINT32                          ulRet;

    pstLineCtrl = AT_GetUartLineCtrlInfo();
    ulRet       = VOS_TRUE;

    if (1 == pstDceMsc->ucDtr)
    {
        /* 判断&S[<value>] */
        if (AT_UART_DSR_MODE_ALWAYS_ON == pstLineCtrl->enDsrMode)
        {
            AT_CtrlDSR(ucIndex, AT_IO_LEVEL_HIGH);
        }

        /* 判断&C[<value>] */
        if (AT_UART_DCD_MODE_ALWAYS_ON == pstLineCtrl->enDcdMode)
        {
            AT_CtrlDCD(ucIndex, AT_IO_LEVEL_HIGH);
        }

        /* 停止流控 */
        AT_StopFlowCtrl(ucIndex);
    }
    else
    {
        ulRet = AT_UART_ProcDtrCtrlMode();
    }

    return ulRet;
}
VOS_UINT32 AT_UART_ProcDtrCtrlMode(VOS_VOID)
{
    AT_UART_LINE_CTRL_STRU             *pstLineCtrl = VOS_NULL_PTR;
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucIndex;

    pstLineCtrl = AT_GetUartLineCtrlInfo();
    ucIndex     = AT_CLIENT_TAB_HSUART_INDEX;

    switch (pstLineCtrl->enDtrMode)
    {
        case AT_UART_DTR_MODE_IGNORE:
            ulRet = VOS_FALSE;
            break;

        case AT_UART_DTR_MODE_SWITCH_CMD_MODE:
            /* 目前只支持PPP和IP模式下切换为ONLINE-COMMAND模式 */
            if ( (AT_DATA_MODE == gastAtClientTab[ucIndex].Mode)
              && ( (AT_PPP_DATA_MODE == gastAtClientTab[ucIndex].DataMode)
                || (AT_IP_DATA_MODE == gastAtClientTab[ucIndex].DataMode) ) )
            {
                At_SetMode(ucIndex, AT_ONLINE_CMD_MODE, AT_NORMAL_MODE);
                At_FormatResultData(ucIndex, AT_OK);
            }
            ulRet = VOS_FALSE;
            break;

        case AT_UART_DTR_MODE_HANGUP_CALL:
            ulRet = VOS_TRUE;
            break;

        default:
            ulRet = VOS_FALSE;
            break;
    }

    return ulRet;
}
VOS_UINT32 AT_HSUART_StartFlowCtrl(
    VOS_UINT32                          ulParam1,
    VOS_UINT32                          ulParam2
)
{
    VOS_UINT8                           ucIndex;

    ucIndex = AT_CLIENT_TAB_HSUART_INDEX;

    if (AT_DATA_MODE == gastAtClientTab[ucIndex].Mode)
    {
        if ( (AT_PPP_DATA_MODE == gastAtClientTab[ucIndex].DataMode)
          || (AT_IP_DATA_MODE == gastAtClientTab[ucIndex].DataMode)
          || (AT_CSD_DATA_MODE == gastAtClientTab[ucIndex].DataMode) )
        {
            AT_MNTN_TraceStartFlowCtrl(ucIndex, AT_FC_DEVICE_TYPE_HSUART);
            AT_CtrlCTS(ucIndex, AT_IO_LEVEL_LOW);
        }
    }

    return VOS_OK;
}


VOS_UINT32 AT_HSUART_StopFlowCtrl(
    VOS_UINT32                          ulParam1,
    VOS_UINT32                          ulParam2
)
{
    VOS_UINT8                           ucIndex;

    ucIndex = AT_CLIENT_TAB_HSUART_INDEX;

    if (AT_DATA_MODE == gastAtClientTab[ucIndex].Mode)
    {
        if ( (AT_PPP_DATA_MODE == gastAtClientTab[ucIndex].DataMode)
          || (AT_IP_DATA_MODE == gastAtClientTab[ucIndex].DataMode)
          || (AT_CSD_DATA_MODE == gastAtClientTab[ucIndex].DataMode) )
        {
            AT_MNTN_TraceStopFlowCtrl(ucIndex, AT_FC_DEVICE_TYPE_HSUART);
            AT_CtrlCTS(ucIndex, AT_IO_LEVEL_HIGH);
        }
    }

    return VOS_OK;
}



VOS_VOID AT_HSUART_UlDataReadCB(VOS_VOID)
{
    IMM_ZC_STRU                        *pstImmZc = VOS_NULL_PTR;
    VOS_UINT32                          ulLen;
    VOS_UINT8                           ucIndex;

    ulLen   = 0;
    ucIndex = AT_CLIENT_TAB_HSUART_INDEX;

    AT_HSUART_DBG_UL_DATA_READ_CB_NUM(1);

    if (AT_SUCCESS == AT_HSUART_GetUlDataBuff(ucIndex, &pstImmZc, &ulLen))
    {
        /* 根据设备当前模式，分发上行数据 */
        AT_HSUART_ProcUlData(ucIndex, pstImmZc);
    }

    return;
}
VOS_UINT32 AT_HSUART_SendRawDataFromOm(
    VOS_UINT8                          *pucVirAddr,
    VOS_UINT8                          *pucPhyAddr,
    VOS_UINT32                          ulLen
)
{
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    ucIndex = AT_CLIENT_TAB_HSUART_INDEX;

    ulResult = AT_UART_WriteDataSync(ucIndex, pucVirAddr, ulLen);
    if (AT_SUCCESS != ulResult)
    {
        AT_ERR_LOG("AT_HSUART_SendRawDataFromOm: Send data failed!\r\n");
        AT_HSUART_DBG_DL_WRITE_SYNC_FAIL_NUM(1);
        return VOS_ERR;
    }

    AT_HSUART_DBG_DL_WRITE_SYNC_SUCC_NUM(1);

    return VOS_OK;
}


VOS_VOID AT_HSUART_MscReadCB(AT_DCE_MSC_STRU *pstDceMsc)
{
    VOS_UINT8                           ucIndex;

    ucIndex = AT_CLIENT_TAB_HSUART_INDEX;

    /* 入参检查 */
    if (VOS_NULL_PTR == pstDceMsc)
    {
        AT_ERR_LOG("AT_HSUART_MscReadCB: pstDceMsc is NULL!");
        return;
    }

    /* 输入管脚信号可维可测 */
    AT_HSUART_DBG_IOCTL_MSC_READ_CB_NUM(1);

    /* 将管脚信息发送出去 */
    At_ModemMscInd(ucIndex, AT_MODEM_USER_DLCI, pstDceMsc);

    return;
}
VOS_VOID AT_HSUART_SwitchCmdDetectCB(VOS_VOID)
{
    AT_MSG_STRU                        *pstMsg = VOS_NULL_PTR;

    /* 记录可维可测 */
    AT_HSUART_DBG_IOCTL_SWITCH_CB_NUM(1);

    /* 构造消息 */
    pstMsg = (AT_MSG_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                WUEPS_PID_AT,
                                sizeof(AT_MSG_STRU));

    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("AT_HSUART_SwitchCmdDetectCB: Alloc message failed!");
        return;
    }

    /* 初始化消息 */
    PS_MEM_SET((VOS_CHAR *)pstMsg + VOS_MSG_HEAD_LENGTH,
               0x00,
               sizeof(AT_MSG_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填写消息头 */
    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid   = WUEPS_PID_AT;
    pstMsg->enMsgId         = ID_AT_SWITCH_CMD_MODE;

    /* 填写消息内容 */
    pstMsg->ucType          = AT_SWITCH_CMD_MODE_MSG;
    pstMsg->ucIndex         = AT_CLIENT_TAB_HSUART_INDEX;

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_ERR_LOG("AT_HSUART_SwitchCmdDetectCB: Send message failed!");
    }

    return;
}


VOS_VOID AT_HSUART_ConfigPort(UDI_HANDLE lUartHdl)
{
    AT_UART_CTX_STRU                   *pstUartCtx     = VOS_NULL_PTR;
    AT_UART_FORMAT_PARAM_STRU          *pstFormatParam = VOS_NULL_PTR;
    VOS_UINT32                          ulBaudRate;
    VOS_UINT32                          ulDataLen;
    VOS_UINT32                          ulStopLen;
    VOS_UINT32                          ulParity;

    pstUartCtx      = AT_GetUartCtxAddr();
    pstFormatParam  = AT_HSUART_GetFormatParam(pstUartCtx->stPhyConfig.stFrame.enFormat);
    ulDataLen       = WLEN_8_BITS;
    ulStopLen       = STP2_OFF;
    ulParity        = PARITY_NO_CHECK;
    ulBaudRate      = pstUartCtx->stPhyConfig.enBaudRate;

    if (VOS_NULL_PTR != pstFormatParam)
    {
        /* 将上下文中记录的数据位宽转换成DRV数据位宽 */
        if (VOS_OK != AT_HSUART_GetUdiValueByDataLen(pstFormatParam->enDataBitLength, &ulDataLen))
        {
            AT_ERR_LOG("AT_HSUART_ConfigPort, Trans Data Length Err!");
        }

        /* 将上下文中记录的停止位宽转换成DRV停止位宽 */
        if (VOS_OK != AT_HSUART_GetUdiValueByStopLen(pstFormatParam->enStopBitLength, &ulStopLen))
        {
            AT_ERR_LOG("AT_HSUART_ConfigPort, Trans Stop Length Err!");
        }

        /* 判断是否有校验 */
        if (AT_UART_PARITY_LEN_1_BIT == pstFormatParam->enParityBitLength)
        {
            /* 将上下文中记录的校验方式转换成DRV校验方式 */
            if (VOS_OK != AT_HSUART_GetUdiValueByParity(pstUartCtx->stPhyConfig.stFrame.enParity, &ulParity))
            {
                AT_ERR_LOG("AT_HSUART_ConfigPort, Trans Parity Err!");
            }
        }
    }

    /* 调用DRV函数设置串口的波特率 */
    if (VOS_OK != mdrv_udi_ioctl(lUartHdl, UART_IOCTL_SET_BAUD, (VOS_VOID *)&ulBaudRate))
    {
        AT_HSUART_DBG_IOCTL_SET_BAUD_FAIL_NUM(1);
    }

    /* 调用DRV函数设置串口数据位长度 */
    if (VOS_OK != mdrv_udi_ioctl(lUartHdl, UART_IOCTL_SET_WLEN, (VOS_VOID *)&ulDataLen))
    {
        AT_HSUART_DBG_IOCTL_SET_WLEN_FAIL_NUM(1);
    }

    /* 调用DRV函数设置串口停止位长度 */
    if (VOS_OK != mdrv_udi_ioctl(lUartHdl, UART_IOCTL_SET_STP2, (VOS_VOID *)&ulStopLen))
    {
        AT_HSUART_DBG_IOCTL_SET_STP_FAIL_NUM(1);
    }

    /* 调用DRV函数设置串口校验位长度 */
    if (VOS_OK != mdrv_udi_ioctl(lUartHdl, UART_IOCTL_SET_EPS, (VOS_VOID *)&ulParity))
    {
        AT_HSUART_DBG_IOCTL_SET_PARITY_FAIL_NUM(1);
    }

    return;
}


VOS_VOID AT_HSUART_InitLink(VOS_UINT8 ucIndex)
{
    PS_MEM_SET(&gastAtClientTab[ucIndex], 0x00, sizeof(AT_CLIENT_MANAGE_STRU));

    gastAtClientTab[ucIndex].usClientId      = AT_CLIENT_ID_HSUART;
    gastAtClientTab[ucIndex].ucUsed          = AT_CLIENT_USED;
    gastAtClientTab[ucIndex].UserType        = AT_HSUART_USER;
    gastAtClientTab[ucIndex].ucPortNo        = AT_HSUART_PORT_NO;
    gastAtClientTab[ucIndex].Mode            = AT_CMD_MODE;
    gastAtClientTab[ucIndex].IndMode         = AT_IND_MODE;
    gastAtClientTab[ucIndex].DataMode        = AT_DATA_BUTT_MODE;
    gastAtClientTab[ucIndex].DataState       = AT_DATA_STOP_STATE;
    gastAtClientTab[ucIndex].CmdCurrentOpt   = AT_CMD_CURRENT_OPT_BUTT;
    g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_READY;

    return;
}


VOS_VOID AT_HSUART_InitPort(VOS_VOID)
{
    ACM_READ_BUFF_INFO                  stReadBuffInfo;
    UDI_OPEN_PARAM_S                    stParam;
    UDI_HANDLE                          lUdiHandle;
    VOS_UINT8                           ucIndex;

    stParam.devid              = UDI_HSUART_0_ID;
    stReadBuffInfo.u32BuffSize = AT_UART_UL_DATA_BUFF_SIZE;
    stReadBuffInfo.u32BuffNum  = AT_UART_UL_DATA_BUFF_NUM;
    ucIndex                    = AT_CLIENT_TAB_HSUART_INDEX;

    /* 配置HSUART端口消息映射 */
    AT_ConfigTraceMsg(ucIndex, ID_AT_CMD_HSUART, ID_AT_MNTN_RESULT_HSUART);

    /* 打开Device，获得ID */
    lUdiHandle = mdrv_udi_open(&stParam);
    if (UDI_INVALID_HANDLE != lUdiHandle)
    {
        /* 配置UART口 */
        AT_HSUART_ConfigPort(lUdiHandle);

        /* 注册UART设备上行数据接收回调 */
        if (VOS_OK != mdrv_udi_ioctl(lUdiHandle, UART_IOCTL_SET_READ_CB, AT_HSUART_UlDataReadCB))
        {
            AT_HSUART_DBG_IOCTL_SET_READ_CB_ERR(1);
        }

        /* 设置UART设备上行数据缓存规格 */
        if (VOS_OK != mdrv_udi_ioctl(lUdiHandle, UART_IOCTL_RELLOC_READ_BUFF, &stReadBuffInfo))
        {
            AT_HSUART_DBG_IOCTL_RELLOC_READ_BUFF_ERR(1);
        }

        /* 注册UART下行数据内存释放接口 */
        if (VOS_OK != mdrv_udi_ioctl(lUdiHandle, UART_IOCTL_SET_FREE_CB, AT_HSUART_FreeDlDataBuff))
        {
            AT_HSUART_DBG_IOCTL_SET_FREE_CB_ERR(1);
        }

        /* 注册管脚信号通知回调 */
        if (VOS_OK != mdrv_udi_ioctl(lUdiHandle, UART_IOCTL_SET_MSC_READ_CB, AT_HSUART_MscReadCB))
        {
            AT_HSUART_DBG_IOCTL_SET_MSC_READ_CB_ERR(1);
        }

        /* 注册"+++"命令检测回调 */
        if (VOS_OK != mdrv_udi_ioctl(lUdiHandle, UART_IOCTL_SWITCH_MODE_CB, AT_HSUART_SwitchCmdDetectCB))
        {
            AT_HSUART_DBG_IOCTL_SET_SWITCH_CB_ERR(1);
        }

        /* 初始化HSUART链路 */
        AT_HSUART_InitLink(ucIndex);
        g_alAtUdiHandle[ucIndex] = lUdiHandle;
    }
    else
    {
        AT_ERR_LOG("AT_HSUART_InitPort, ERROR, Open UART device failed!");
        g_alAtUdiHandle[ucIndex] = UDI_INVALID_HANDLE;
    }

    return;
}
VOS_VOID AT_HSUART_ConfigFlowCtrl(
            VOS_UINT8                   ucIndex,
            VOS_UINT32                  ulRtsFlag,
            VOS_UINT32                  ulCtsFlag
)
{
    uart_flow_ctrl_union                unFlowCtrlValue;

    VOS_MemSet(&unFlowCtrlValue, 0x0, sizeof(unFlowCtrlValue));

    unFlowCtrlValue.reg.rtsen = ulRtsFlag;
    unFlowCtrlValue.reg.ctsen = ulCtsFlag;

    if (VOS_OK != mdrv_udi_ioctl(g_alAtUdiHandle[ucIndex], UART_IOCTL_SET_FLOW_CONTROL, &unFlowCtrlValue))
    {
        AT_HSUART_DBG_IOCTL_CON_FLOW_CTR_ERR(1);
        return;
    }

    return;

}

#endif


VOS_VOID AT_CtrlDCD(
    VOS_UINT8                           ucIndex,
    AT_IO_LEVEL_ENUM_UINT8              enIoLevel
)
{
    AT_DCE_MSC_STRU                     stDceMsc;
    NAS_OM_EVENT_ID_ENUM_UINT16         enEventId;

    PS_MEM_SET(&stDceMsc, 0x00, sizeof(AT_DCE_MSC_STRU));

    stDceMsc.OP_Dcd = VOS_TRUE;
    stDceMsc.ucDcd  = enIoLevel;

    AT_SetModemStatus(ucIndex, &stDceMsc);

    enEventId = (AT_IO_LEVEL_HIGH == enIoLevel) ?
                NAS_OM_EVENT_DCE_UP_DCD : NAS_OM_EVENT_DCE_DOWN_DCD;

    AT_EventReport(WUEPS_PID_AT, enEventId, &ucIndex, sizeof(VOS_UINT8));

    return;
}


VOS_VOID AT_CtrlDSR(
    VOS_UINT8                           ucIndex,
    AT_IO_LEVEL_ENUM_UINT8              enIoLevel
)
{
    AT_DCE_MSC_STRU                     stDceMsc;
    NAS_OM_EVENT_ID_ENUM_UINT16         enEventId;

    PS_MEM_SET(&stDceMsc, 0x00, sizeof(AT_DCE_MSC_STRU));

    stDceMsc.OP_Dsr = VOS_TRUE;
    stDceMsc.ucDsr  = enIoLevel;

    AT_SetModemStatus(ucIndex, &stDceMsc);

    enEventId = (AT_IO_LEVEL_HIGH == enIoLevel) ?
                NAS_OM_EVENT_DCE_UP_DSR: NAS_OM_EVENT_DCE_DOWN_DSR;

    AT_EventReport(WUEPS_PID_AT, enEventId, &ucIndex, sizeof(VOS_UINT8));

    return;
}


VOS_VOID AT_CtrlCTS(
    VOS_UINT8                           ucIndex,
    AT_IO_LEVEL_ENUM_UINT8              enIoLevel
)
{
    AT_DCE_MSC_STRU                     stDceMsc;
    NAS_OM_EVENT_ID_ENUM_UINT16         enEventId;

    PS_MEM_SET(&stDceMsc, 0x00, sizeof(AT_DCE_MSC_STRU));

    stDceMsc.OP_Cts = VOS_TRUE;
    stDceMsc.ucCts  = enIoLevel;

    AT_SetModemStatus(ucIndex, &stDceMsc);

    enEventId = (AT_IO_LEVEL_HIGH == enIoLevel) ?
                NAS_OM_EVENT_DCE_UP_CTS: NAS_OM_EVENT_DCE_DOWN_CTS;

    AT_EventReport(WUEPS_PID_AT, enEventId, &ucIndex, sizeof(VOS_UINT8));

    return;
}


VOS_VOID AT_CtrlRI(
    VOS_UINT8                           ucIndex,
    AT_IO_LEVEL_ENUM_UINT8              enIoLevel
)
{
    AT_DCE_MSC_STRU                     stDceMsc;
    NAS_OM_EVENT_ID_ENUM_UINT16         enEventId;

    PS_MEM_SET(&stDceMsc, 0x00, sizeof(AT_DCE_MSC_STRU));

    stDceMsc.OP_Ri = VOS_TRUE;
    stDceMsc.ucRi  = enIoLevel;

    AT_SetModemStatus(ucIndex, &stDceMsc);

    enEventId = (AT_IO_LEVEL_HIGH == enIoLevel) ?
                NAS_OM_EVENT_DCE_UP_RI: NAS_OM_EVENT_DCE_DOWN_RI;

    AT_EventReport(WUEPS_PID_AT, enEventId, &ucIndex, sizeof(VOS_UINT8));

    return;
}


AT_IO_LEVEL_ENUM_UINT8 AT_GetIoLevel(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           ucIoCtrl
)
{
    if (0 == (gastAtClientTab[ucIndex].ModemStatus & ucIoCtrl))
    {
        return AT_IO_LEVEL_LOW;
    }

    return AT_IO_LEVEL_HIGH;
}

/*****************************************************************************
 函 数 名  : App_VcomRecvCallbackRegister
 功能描述  : 注册回调函数
 输入参数  : uPortNo
             pCallback
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年10月17日
    作    者   : sunshaohua
    修改内容   : 新生成函数
*****************************************************************************/
int  App_VcomRecvCallbackRegister(unsigned char  uPortNo, pComRecv pCallback)
{
    return VOS_OK;
}


VOS_INT AT_RcvFromAppCom(
    VOS_UINT8                           ucVcomId,
    VOS_UINT8                          *pData,
    VOS_UINT16                          uslength
)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulRet;

    if (ucVcomId >= APP_VCOM_DEV_INDEX_BUTT)
    {
        AT_WARN_LOG("AT_RcvFromAppCom: Port No ERR!");
        return VOS_ERR;
    }
    /* APPVCOM最后几个不是AT通道 */
    if (ucVcomId >= AT_VCOM_AT_CHANNEL_MAX)
    {
        AT_WARN_LOG("AT_RcvFromAppCom: Port No ERR!");
        return VOS_ERR;
    }
    if (VOS_NULL_PTR == pData)
    {
        AT_WARN_LOG("AT_RcvFromAppCom: pData is NULL PTR!");
        return VOS_ERR;
    }

    if (0 == uslength)
    {
        AT_WARN_LOG("AT_RcvFromAppCom: uslength is 0!");
        return VOS_ERR;
    }

    /* 根据端口号确定Index的值 */
    ucIndex = AT_CLIENT_TAB_APP_INDEX + ucVcomId;

    /* 判断是否是APP通道 */
    if ((AT_APP_USER != gastAtClientTab[ucIndex].UserType)
     || (AT_CLIENT_NULL == gastAtClientTab[ucIndex].ucUsed))
    {
        AT_WARN_LOG("AT_RcvFromAppCom: APP client is unused!");
        return VOS_ERR;
    }

    if (AT_CMD_MODE == gastAtClientTab[ucIndex].Mode)
    {
        ulRet = At_CmdStreamPreProc(ucIndex, pData, uslength);
    }
    else
    {
        ulRet = At_DataStreamPreProc(ucIndex, gastAtClientTab[ucIndex].DataMode, pData, uslength);
    }

    if ( AT_SUCCESS == ulRet )
    {
        return VOS_OK;
    }
    else
    {
        return VOS_ERR;
    }
}
VOS_INT32 AT_AppComEst(VOS_VOID)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT8                           ucLoop;

    for (ucLoop = 0; ucLoop < AT_VCOM_AT_CHANNEL_MAX; ucLoop++)
    {
        ucIndex = AT_CLIENT_TAB_APP_INDEX + ucLoop;

        /* 清空对应表项 */
        PS_MEM_SET(&gastAtClientTab[ucIndex], 0x00, sizeof(AT_CLIENT_MANAGE_STRU));

        AT_ConfigTraceMsg(ucIndex, (ID_AT_CMD_APP + ucLoop), (ID_AT_MNTN_RESULT_APP + ucLoop));

        gastAtClientTab[ucIndex].usClientId     = AT_CLIENT_ID_APP + ucLoop;

        /* 填写用户表项 */
        gastAtClientTab[ucIndex].ucPortNo        = APP_VCOM_DEV_INDEX_0 + ucLoop;
        gastAtClientTab[ucIndex].UserType        = AT_APP_USER;
        gastAtClientTab[ucIndex].ucUsed          = AT_CLIENT_USED;
        gastAtClientTab[ucIndex].Mode            = AT_CMD_MODE;
        gastAtClientTab[ucIndex].IndMode         = AT_IND_MODE;
        gastAtClientTab[ucIndex].DataMode        = AT_DATA_BUTT_MODE;
        gastAtClientTab[ucIndex].DataState       = AT_DATA_STOP_STATE;
        gastAtClientTab[ucIndex].CmdCurrentOpt   = AT_CMD_CURRENT_OPT_BUTT;
        g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_READY;

        /* 注册回调函数 */
        APP_VCOM_RegDataCallback(gastAtClientTab[ucIndex].ucPortNo, (SEND_UL_AT_FUNC)AT_RcvFromAppCom);

    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : AT_RcvFromSock
 功能描述  : AT接收SOCK的数据
 输入参数  : ucPortNo    - 端口号
             *pData      - 指向数据的指针
             uslength    - 数据长度
 输出参数  : 无
 返 回 值  : AT_SUCCESS --- 成功
             AT_FAILURE --- 失败

 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2010年9月13日
    作    者   : s62952
    修改内容   : 新生成函数
  2.日    期   : 2011年10月18日
    作    者   : 鲁琳/l60609
    修改内容   : AT Project: 静态分配 client id

*****************************************************************************/
VOS_INT AT_RcvFromSock(
    VOS_UINT8                          *pData,
    VOS_UINT32                         uslength
)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulRet;

    ucIndex = AT_CLIENT_TAB_SOCK_INDEX;

    if (VOS_NULL_PTR == pData)
    {
        AT_WARN_LOG("AT_RcvFromSock: pData is NULL PTR!");
        return VOS_ERR;
    }
    if (0 == uslength)
    {
        AT_WARN_LOG("AT_RcvFromSock: uslength is 0!");
        return VOS_ERR;
    }

    if ((AT_SOCK_USER != gastAtClientTab[ucIndex].UserType)
        ||(AT_CLIENT_NULL == gastAtClientTab[ucIndex].ucUsed))
    {
        AT_WARN_LOG("AT_RcvFromSock: SOCK client is unused!");
        return VOS_ERR;
    }

    if (AT_CMD_MODE == gastAtClientTab[ucIndex].Mode)
    {
        ulRet = At_CmdStreamPreProc(ucIndex,pData,(VOS_UINT16)uslength);
    }
    else
    {
        ulRet = At_DataStreamPreProc(ucIndex,gastAtClientTab[ucIndex].DataMode,pData,(VOS_UINT16)uslength);
    }
    if ( AT_SUCCESS == ulRet )
    {
        return VOS_OK;
    }
    else
    {
        return VOS_ERR;
    }
}


VOS_INT32 AT_SockComEst(VOS_UINT8 ucPortNo)
{

    VOS_UINT8                           ucIndex;

    ucIndex = AT_CLIENT_TAB_SOCK_INDEX;

    if (AT_SOCK_PORT_NO != ucPortNo)
    {
        AT_WARN_LOG("At_SockComEst the PortNo is error)");
        return VOS_ERR;
    }

    PS_MEM_SET(&gastAtClientTab[ucIndex], 0x00, sizeof(AT_CLIENT_MANAGE_STRU));

    AT_ConfigTraceMsg(ucIndex, ID_AT_CMD_SOCK, ID_AT_MNTN_RESULT_SOCK);

    gastAtClientTab[ucIndex].usClientId      = AT_CLIENT_ID_SOCK;
    gastAtClientTab[ucIndex].ucPortNo        = ucPortNo;
    gastAtClientTab[ucIndex].UserType        = AT_SOCK_USER;
    gastAtClientTab[ucIndex].ucUsed          = AT_CLIENT_USED;
    gastAtClientTab[ucIndex].Mode            = AT_CMD_MODE;
    gastAtClientTab[ucIndex].IndMode         = AT_IND_MODE;
    gastAtClientTab[ucIndex].DataMode        = AT_DATA_BUTT_MODE;
    gastAtClientTab[ucIndex].DataState       = AT_DATA_STOP_STATE;
    gastAtClientTab[ucIndex].CmdCurrentOpt   = AT_CMD_CURRENT_OPT_BUTT;
    g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_READY;

    /* Modified by s62952 for AT Project，2011-10-17,  Begin*/
    /*向DMS注册从串口中获取数据的回调函数*/
    (VOS_VOID)CPM_LogicRcvReg(CPM_AT_COMM,(CPM_RCV_FUNC)AT_RcvFromSock);
    /* Modified by s62952 for AT Project，2011-10-17,  end*/

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : AT_RcvFromAppSock
 功能描述  : 注册给应用的接口，用于接收AT命令
 输入参数  : ucPortNo    - 端口号
             *pData      - 指向数据的指针
             uslength    - 数据长度
 输出参数  : 无
 返 回 值  : AT_SUCCESS --- 成功
             AT_FAILURE --- 失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2010年9月7日
    作    者   : s62952
    修改内容   : 新生成函数
  2.日    期   : 2011年10月18日
    作    者   : 鲁琳/l60609
    修改内容   : AT Project: 静态分配 client id

*****************************************************************************/
VOS_INT AT_RcvFromAppSock(
    VOS_UINT8                           ucPortNo,
    VOS_UINT8                          *pData,
    VOS_UINT16                          uslength
)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulRet;

    ucIndex = AT_CLIENT_TAB_APPSOCK_INDEX;

    if (VOS_NULL_PTR == pData)
    {
        AT_WARN_LOG("AT_RcvFromAppSock: pData is NULL PTR!");
        return VOS_ERR;
    }

    if (0 == uslength)
    {
        AT_WARN_LOG("AT_RcvFromAppSock: uslength is 0!");
        return VOS_ERR;
    }

    if (AT_APP_SOCK_PORT_NO != ucPortNo)
    {
        AT_WARN_LOG("AT_RcvFromAppSock: Port No ERR!");
        return VOS_ERR;
    }

    if ( (AT_APP_SOCK_USER != gastAtClientTab[ucIndex].UserType)
       ||(AT_CLIENT_NULL == gastAtClientTab[ucIndex].ucUsed))
    {
        AT_WARN_LOG("AT_RcvFromAppSock: SOCK client is unused!");
        return VOS_ERR;
    }

    if (AT_CMD_MODE == gastAtClientTab[ucIndex].Mode)
    {
        ulRet = At_CmdStreamPreProc(ucIndex,pData,uslength);
    }
    else
    {
        ulRet = At_DataStreamPreProc(ucIndex,gastAtClientTab[ucIndex].DataMode,pData,uslength);
    }

    if ( AT_SUCCESS == ulRet )
    {
        return VOS_OK;
    }
    else
    {
        return VOS_ERR;
    }
}


VOS_INT32 AT_AppSockComEst(VOS_UINT8 ucPortNo)
{
    /* Modified by L60609 for AT Project，2011-10-04,  Begin*/
    VOS_UINT8                           ucIndex;

    ucIndex = AT_CLIENT_TAB_APPSOCK_INDEX;

    if (AT_APP_SOCK_PORT_NO != ucPortNo)
    {
         AT_WARN_LOG("AT_E5SockComEst the PortNo is error)");
        return VOS_ERR;
    }

    AT_ConfigTraceMsg(ucIndex, ID_AT_CMD_APPSOCK, ID_AT_MNTN_RESULT_APPSOCK);

    /* 清空对应表项 */
    PS_MEM_SET(&gastAtClientTab[ucIndex], 0x00, sizeof(AT_CLIENT_MANAGE_STRU));

    /* 填写用户表项 */
    gastAtClientTab[ucIndex].usClientId      = AT_CLIENT_ID_APPSOCK;
    gastAtClientTab[ucIndex].ucPortNo        = ucPortNo;
    gastAtClientTab[ucIndex].UserType        = AT_APP_SOCK_USER;
    gastAtClientTab[ucIndex].ucUsed          = AT_CLIENT_USED;
    gastAtClientTab[ucIndex].Mode            = AT_CMD_MODE;
    gastAtClientTab[ucIndex].IndMode         = AT_IND_MODE;
    gastAtClientTab[ucIndex].DataMode        = AT_DATA_BUTT_MODE;
    gastAtClientTab[ucIndex].DataState       = AT_DATA_STOP_STATE;
    gastAtClientTab[ucIndex].CmdCurrentOpt   = AT_CMD_CURRENT_OPT_BUTT;
    g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_READY;

    /* Modified by s62952 for AT Project，2011-10-17,  Begin*/
    /*向DMS注册从串口中获取数据的回调函数*/
    (VOS_VOID)App_VcomRecvCallbackRegister(ucPortNo, (pComRecv)AT_RcvFromAppSock);
    /* Modified by s62952 for AT Project，2011-10-17,  end*/

    /* Modified by L60609 for AT Project，2011-10-04,  End*/

    return VOS_OK;
}

/* Modified by s62952 for BalongV300R002 Build优化项目 2012-02-28, begin */
/******************************************************************************
 函 数 名  : At_UsbGetWwanMode
 功能描述  : 底软获取NDIS当前模式
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 1 : WCDMA
             2 : CDMA

 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年4月
    作    者   : s62952
    修改内容   : 新生成函数
******************************************************************************/
VOS_UINT32 At_UsbGetWwanMode(VOS_VOID)
{
    return WWAN_WCDMA;
}


VOS_INT AT_UsbCtrlBrkReqCB(VOS_VOID)
{
    /* 如果所有的PDP都处于IDLE，返回OK; 如果有一个处于DEACTING也返回OK */
    if (VOS_TRUE == AT_NdisCheckPdpIdleState())
    {
        AT_WARN_LOG("AT_UsbCtrlBrkReqCB: the state is already IDLE.");
        return 0;
    }

    if ((AT_PDP_STATE_ACTING == g_stAtNdisDhcpPara.enIpv4v6State)
     || (AT_PDP_STATE_ACTED  == g_stAtNdisDhcpPara.enIpv4v6State) )
    {
        if (VOS_OK == TAF_PS_CallEnd(WUEPS_PID_AT,
                                     gastAtClientTab[AT_CLIENT_TAB_NDIS_INDEX].usClientId,
                                     0,
                                     g_stAtNdisDhcpPara.ucIpv4v6Cid) )
        {
            AT_NdisSetState(TAF_PDP_IPV4V6, AT_PDP_STATE_DEACTING);
        }
    }
    else
    {
        if ( (AT_PDP_STATE_ACTING == g_stAtNdisDhcpPara.enIpv4State)
          || (AT_PDP_STATE_ACTED  == g_stAtNdisDhcpPara.enIpv4State) )
        {
            if (VOS_OK == TAF_PS_CallEnd(WUEPS_PID_AT,
                                         gastAtClientTab[AT_CLIENT_TAB_NDIS_INDEX].usClientId,
                                         0,
                                         g_stAtNdisDhcpPara.ucIpv4Cid) )
            {
                AT_NdisSetState(TAF_PDP_IPV4, AT_PDP_STATE_DEACTING);
            }
        }

        if ( (AT_PDP_STATE_ACTING == g_stAtNdisDhcpPara.enIpv6State)
          || (AT_PDP_STATE_ACTED  == g_stAtNdisDhcpPara.enIpv6State) )
        {
            if (VOS_OK == TAF_PS_CallEnd(WUEPS_PID_AT,
                                         gastAtClientTab[AT_CLIENT_TAB_NDIS_INDEX].usClientId,
                                         0,
                                         g_stAtNdisDhcpPara.ucIpv6Cid) )
            {
                AT_NdisSetState(TAF_PDP_IPV6, AT_PDP_STATE_DEACTING);
            }
        }
    }

    return 0;
}

/*****************************************************************************
 函 数 名  : At_RcvFromNdisCom
 功能描述  : NDIS注册给应用的接口，用于接收AT命令
 输入参数  : ucPortNo    - 端口号
             *pucData    - 指向数据的指针
             uslength    - 数据长度
 输出参数  : 无
 返 回 值  : AT_SUCCESS --- 成功
             AT_FAILURE --- 失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年4月3日
    作    者   : s62952
    修改内容   : 新生成函数

*****************************************************************************/
VOS_INT AT_RcvFromNdisCom(
    VOS_UINT8                           *pucData,
    VOS_UINT16                          uslength
)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulRet;

    /* 参数检查 */
    if (VOS_NULL_PTR == pucData)
    {
        AT_WARN_LOG("At_RcvFromNdisCom: pData is NULL PTR!");
        return VOS_ERR;
    }

    /* 参数检查 */
    if (0 == uslength)
    {
        AT_WARN_LOG("At_RcvFromNdisCom: uslength is 0!");
        return VOS_ERR;
    }

    ucIndex = AT_CLIENT_TAB_NDIS_INDEX;

    /* NDIS链路没有建立 */
    if ( (AT_NDIS_USER != gastAtClientTab[ucIndex].UserType)
       ||(AT_CLIENT_NULL == gastAtClientTab[ucIndex].ucUsed))
    {
        AT_WARN_LOG("At_RcvFromNdisCom: NDIS is unused");
        return VOS_ERR;
    }

    /*设置NDIS通道状态为可上报数据*/
    DMS_SetNdisChanStatus(ACM_EVT_DEV_READY);

    if (AT_CMD_MODE == gastAtClientTab[ucIndex].Mode)
    {
        ulRet = At_CmdStreamPreProc(ucIndex,pucData,uslength);
    }
    else
    {
        ulRet = At_DataStreamPreProc(ucIndex,gastAtClientTab[ucIndex].DataMode,pucData,uslength);
    }

    if ( AT_SUCCESS == ulRet )
    {
        return VOS_OK;
    }
    else
    {
        return VOS_ERR;
    }
}


VOS_UINT32 AT_UsbNdisEst(VOS_VOID)
{
    /* Modified by L60609 for AT Project，2011-10-04,  Begin*/
    VOS_UINT8                           ucIndex;

    ucIndex = AT_CLIENT_TAB_NDIS_INDEX;

    /* 清空对应表项 */
    PS_MEM_SET(&gastAtClientTab[ucIndex],0x00,sizeof(AT_CLIENT_MANAGE_STRU));

    AT_ConfigTraceMsg(ucIndex, ID_AT_CMD_NDIS, ID_AT_MNTN_RESULT_NDIS);

    /* 填写用户表项 */
    gastAtClientTab[ucIndex].usClientId      = AT_CLIENT_ID_NDIS;
    gastAtClientTab[ucIndex].ucPortNo        = AT_NDIS_PORT_NO;
    gastAtClientTab[ucIndex].UserType        = AT_NDIS_USER;
    gastAtClientTab[ucIndex].ucUsed          = AT_CLIENT_USED;

    /* 以下可以不用填写，前面PS_MEMSET已经初始化，只为可靠起见 */
    gastAtClientTab[ucIndex].Mode            = AT_CMD_MODE;
    gastAtClientTab[ucIndex].IndMode         = AT_IND_MODE;
    gastAtClientTab[ucIndex].DataMode        = AT_DATA_BUTT_MODE;
    gastAtClientTab[ucIndex].DataState       = AT_DATA_STOP_STATE;
    gastAtClientTab[ucIndex].CmdCurrentOpt   = AT_CMD_CURRENT_OPT_BUTT;
    g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_READY;

    /*初始化NDIS ADDR参数*/
    PS_MEM_SET(&gstAtNdisAddParam, 0, sizeof(AT_DIAL_PARAM_STRU));

    return VOS_OK;
}
VOS_VOID AT_OpenUsbNdis(VOS_VOID)
{
    UDI_OPEN_PARAM_S                    stParam;
    VOS_UINT32                          ulRst;

    stParam.devid   = UDI_NCM_NDIS_ID;

    /* 打开Device，获得ID */
    g_ulAtUdiNdisHdl = mdrv_udi_open(&stParam);

    if (UDI_INVALID_HANDLE == g_ulAtUdiNdisHdl)
    {
        AT_ERR_LOG("AT_OpenUsbNdis, ERROR, Open usb ndis device failed!");

        return;
    }

    /* 注册DMS回调函数指针 */
    /*lint -e732   类型不统一，暂时注掉，确认接口再解决*/
    ulRst =  DMS_USB_NAS_REGFUNC((USB_NAS_BRK)AT_UsbCtrlBrkReqCB,
                                 (USB_NAS_AT_CMD_RECV)AT_RcvFromNdisCom,
                                 (USB_NAS_GET_WWAN_MODE)At_UsbGetWwanMode);
    if (VOS_OK != ulRst)
    {
        AT_ERR_LOG("AT_OpenUsbNdis, ERROR, Reg NCM failed!");

        return;
    }
    /*lint +e732*/
    /* Modified by L60609 for AT Project，2011-10-04,  End*/

    return;
}
VOS_VOID AT_CloseUsbNdis(VOS_VOID)
{
    /* 如果有PDP激活,则需要去激活 */
    AT_UsbCtrlBrkReqCB();

    if (UDI_INVALID_HANDLE != g_ulAtUdiNdisHdl)
    {
        mdrv_udi_close(g_ulAtUdiNdisHdl);

        g_ulAtUdiNdisHdl = UDI_INVALID_HANDLE;
    }

    return;
}

/* Added by L60609 for MUX，2012-08-03,  Begin */
/*****************************************************************************
 函 数 名  : AT_GetMuxSupportFlg
 功能描述  : 获取是否支持MUX的标志
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_UINT8
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月9日
    作    者   : l60609
    修改内容   : 新生成函数
  2.日    期   : 2013年3月4日
    作    者   : l60609
    修改内容   : DSDA PHASE III
*****************************************************************************/
VOS_UINT8 AT_GetMuxSupportFlg(VOS_VOID)
{
    /* Modified by l60609 for DSDA Phase III, 2013-3-4, Begin */
    return AT_GetCommCtxAddr()->stMuxCtx.ucMuxSupportFlg;
    /* Modified by l60609 for DSDA Phase III, 2013-3-4, End */
}

/*****************************************************************************
 函 数 名  : AT_SetMuxSupportFlg
 功能描述  : 设置是否支持MUX的标志
 输入参数  : VOS_UINT8 ucMuxSupportFlg
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月9日
    作    者   : l60609
    修改内容   : 新生成函数
  2.日    期   : 2013年3月4日
    作    者   : l60609
    修改内容   : DSDA PHASE III
*****************************************************************************/
VOS_VOID AT_SetMuxSupportFlg(VOS_UINT8 ucMuxSupportFlg)
{
    /* Modified by l60609 for DSDA Phase III, 2013-3-4, Begin */
    AT_COMM_CTX_STRU                   *pstCommCtx = VOS_NULL_PTR;

    pstCommCtx = AT_GetCommCtxAddr();

    pstCommCtx->stMuxCtx.ucMuxSupportFlg = ucMuxSupportFlg;
    /* Modified by l60609 for DSDA Phase III, 2013-3-4, End */
}

/*****************************************************************************
 函 数 名  : AT_CheckMuxDlci
 功能描述  : 检查DLCI的有效性
 输入参数  : AT_MUX_DLCI_TYPE_ENUM_UINT8 enDlci
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月8日
    作    者   : l60609
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AT_CheckMuxDlci(AT_MUX_DLCI_TYPE_ENUM_UINT8 enDlci)
{
    /* 此次开发只支持8个通道，DLCI从1到8。在1-8范围的返回VOS_OK，否则返回VOS_ERR */
    if ((enDlci >= AT_MUX_DLCI1_ID)
     && (enDlci < (AT_MUX_DLCI1_ID + AT_MUX_AT_CHANNEL_MAX)))
    {
        return VOS_OK;
    }

    return VOS_ERR;
}

/*****************************************************************************
 函 数 名  : AT_CheckMuxUser
 功能描述  : 检查当前命令下发端口是否为MUX通道
 输入参数  : VOS_UINT8 ucIndex
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月3日
    作    者   : L60609
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AT_CheckMuxUser(VOS_UINT8 ucIndex)
{
#if (FEATURE_ON == FEATURE_AT_HSIC)
    if ((gastAtClientTab[ucIndex].UserType >= AT_MUX1_USER)
     && (gastAtClientTab[ucIndex].UserType < (AT_MUX1_USER + AT_MUX_AT_CHANNEL_MAX)))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
#else
    return VOS_FALSE;
#endif
}

/*****************************************************************************
 函 数 名  : AT_IsHsicOrMuxUser
 功能描述  : 判断是否为HSIC端口或者MUX端口
 输入参数  : VOS_UINT8 ucIndex
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月13日
    作    者   : l60609
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AT_IsHsicOrMuxUser(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulHsicUserFlg;
    VOS_UINT32                          ulMuxUserFlg;

    ulHsicUserFlg = AT_CheckHsicUser(ucIndex);
    ulMuxUserFlg  = AT_CheckMuxUser(ucIndex);

    /* 既不是HSIC端口，也不是MUX端口 */
    if ((VOS_FALSE == ulHsicUserFlg)
     && (VOS_FALSE == ulMuxUserFlg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 函 数 名  : AT_GetMuxDlciFromClientIdx
 功能描述  : 根据client index获取MUX的dlci值
 输入参数  : VOS_UINT8                           ucIndex
             AT_MUX_DLCI_TYPE_ENUM_UINT8        *penDlci
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月6日
    作    者   : l60609
    修改内容   : 新生成函数
  2.日    期   : 2013年3月4日
    作    者   : l60609
    修改内容   : DSDA PHASE III
*****************************************************************************/
VOS_UINT32 AT_GetMuxDlciFromClientIdx(
    VOS_UINT8                           ucIndex,
    AT_MUX_DLCI_TYPE_ENUM_UINT8        *penDlci
)
{
    VOS_UINT8                           ucLoop;

    /* Modified by l60609 for DSDA Phase III, 2013-3-4, Begin */
    for (ucLoop = 0; ucLoop < AT_MUX_AT_CHANNEL_MAX; ucLoop++)
    {
        if (ucIndex == AT_GetCommCtxAddr()->stMuxCtx.astMuxClientTab[ucLoop].enAtClientTabIdx)
        {
            *penDlci = AT_GetCommCtxAddr()->stMuxCtx.astMuxClientTab[ucLoop].enDlci;
            break;
        }
    }
    /* Modified by l60609 for DSDA Phase III, 2013-3-4, End */

    if (ucLoop >= AT_MUX_AT_CHANNEL_MAX)
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

VOS_UINT32 AT_CheckPcuiCtrlConcurrent(
    VOS_UINT8                           ucIndexOne,
    VOS_UINT8                           ucIndexTwo
)
{
    VOS_UINT32                          ulPcuiUserFlg1;
    VOS_UINT32                          ulCtrlUserFlg1;
    VOS_UINT32                          ulPcuiUserFlg2;
    VOS_UINT32                          ulCtrlUserFlg2;

    ulPcuiUserFlg1 = AT_CheckUsbcomUser(ucIndexOne);
    ulCtrlUserFlg1 = AT_CheckCtrUser(ucIndexOne);

    ulPcuiUserFlg2 = AT_CheckUsbcomUser(ucIndexTwo);
    ulCtrlUserFlg2 = AT_CheckCtrUser(ucIndexTwo);

    if (((VOS_TRUE == ulPcuiUserFlg1) && (VOS_TRUE == ulCtrlUserFlg2))
     || ((VOS_TRUE == ulCtrlUserFlg1) && (VOS_TRUE == ulPcuiUserFlg2)))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 AT_IsConcurrentPorts(
    VOS_UINT8                           ucIndexOne,
    VOS_UINT8                           ucIndexTwo
)
{
    VOS_UINT32                          ulMuxUserFlg1;
    VOS_UINT32                          ulHsicUserFlg1;
    VOS_UINT32                          ulAppUserFlg1;
    VOS_UINT32                          ulMuxUserFlg2;
    VOS_UINT32                          ulHsicUserFlg2;
    VOS_UINT32                          ulAppUserFlg2;

    /* 同一个通道不支持并发，由外层函数保证 */
    ulMuxUserFlg1  = AT_CheckMuxUser(ucIndexOne);
    ulHsicUserFlg1 = AT_CheckHsicUser(ucIndexOne);
    ulAppUserFlg1  = AT_CheckAppUser(ucIndexOne);

    ulMuxUserFlg2  = AT_CheckMuxUser(ucIndexTwo);
    ulHsicUserFlg2 = AT_CheckHsicUser(ucIndexTwo);
    ulAppUserFlg2  = AT_CheckAppUser(ucIndexTwo);

    /* 通道1是HSIC通道或MUX通道,通道2也是HSIC通道或MUX通道 */
    if ((VOS_TRUE == ulMuxUserFlg1)
     || (VOS_TRUE == ulHsicUserFlg1))
    {
        if ((VOS_TRUE == ulMuxUserFlg2)
         || (VOS_TRUE == ulHsicUserFlg2))
        {
            return VOS_TRUE;
        }
    }

    /* 通道1是APP通道,通道2也是APP通道 */
    if (VOS_TRUE == ulAppUserFlg1)
    {
        if (VOS_TRUE == ulAppUserFlg2)
        {
            return VOS_TRUE;
        }
    }

    /* PCUI和CTRL口并发判断，仅供测试用 */
    if (VOS_TRUE == AT_GetPcuiCtrlConcurrentFlag())
    {
        if (VOS_TRUE == AT_CheckPcuiCtrlConcurrent(ucIndexOne, ucIndexTwo))
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}

/*****************************************************************************
 函 数 名  : AT_RcvMuxCmdStream_PreProc
 功能描述  : MUX通道命令预处理
 输入参数  : VOS_UINT8                           ucIndex
             VOS_UINT8                          *pData
             VOS_UINT16                          usLen
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月6日
    作    者   : l60609
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AT_RcvMuxCmdStream_PreProc(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                          *pData,
    VOS_UINT16                          usLen
)
{
    AT_MUX_DLCI_TYPE_ENUM_UINT8         enDlci;
    VOS_UINT32                          ulRslt;

    enDlci = AT_MUX_DLCI_TYPE_BUTT;

    ulRslt = AT_GetMuxDlciFromClientIdx(ucIndex, &enDlci);

    if (VOS_TRUE != ulRslt)
    {
        return;
    }

    /* 判断pData码流的结尾是否为<CR><LF>形式 */
    if ((usLen > 2)
     && (ucAtS3 == pData[usLen - 2])
     && (ucAtS4 == pData[usLen - 1]))
    {
        /* 删去结尾的<LF>字符 */
        MUX_DlciDlDataSend(enDlci, pData, (usLen - 1));
        AT_MNTN_TraceCmdResult(ucIndex, pData, (usLen - 1));
    }
    else
    {
        MUX_DlciDlDataSend(enDlci, pData, usLen);
        AT_MNTN_TraceCmdResult(ucIndex, pData, usLen);
    }

    return;
}

/*****************************************************************************
 函 数 名  : AT_MemSingleCopy
 功能描述  : 获取给定内存单元实际使用的总字节数，包括整个数据链
 输入参数  : pMemSrc  -- 要获取数据的TTF内存块头指针
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年10月17日
    作    者   : sunshaohua
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AT_MemSingleCopy(
    VOS_UINT8                          *pucDest,
    VOS_UINT8                          *pucSrc,
    VOS_UINT32                          ulLen
)
{
    /* 使用的内存可能为不可Cache，不能使用DM */
    mdrv_memcpy(pucDest, pucSrc, (unsigned long)ulLen);

    return;
}

#if (FEATURE_ON == FEATURE_AT_HSIC)

VOS_VOID AT_InitMuxCtx(VOS_VOID)
{
    VOS_UINT8                           ucLoop;
    /* Modified by l60609 for DSDA Phase III, 2013-2-25, Begin */
    AT_COMM_CTX_STRU                   *pCommCtx = VOS_NULL_PTR;
    TAF_AT_NVIM_MUX_REPORT_CFG_STRU     stMuxReportCfg;

    stMuxReportCfg.ulMuxReportCfg  = AT_HSIC_REPORT_ON;

    pCommCtx = AT_GetCommCtxAddr();

    /* 从NV中读取MUX通道主动上报命令状态，读取失败默认全部上报 */
    if (NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_MUX_REPORT_CFG,
                         &stMuxReportCfg,
                         sizeof(stMuxReportCfg)))
    {
        stMuxReportCfg.ulMuxReportCfg  = AT_HSIC_REPORT_ON;
    }

    for (ucLoop = 0; ucLoop < AT_MUX_AT_CHANNEL_MAX; ucLoop++)
    {
        pCommCtx->stMuxCtx.astMuxClientTab[ucLoop].pReadDataCB      = AT_MuxReadDataCB;
        pCommCtx->stMuxCtx.astMuxClientTab[ucLoop].enAtClientId     = AT_CLIENT_ID_MUX1 + ucLoop;
        pCommCtx->stMuxCtx.astMuxClientTab[ucLoop].enAtClientTabIdx = AT_CLIENT_TAB_MUX1_INDEX + ucLoop;
        pCommCtx->stMuxCtx.astMuxClientTab[ucLoop].ucMuxUser        = AT_MUX1_USER + ucLoop;
        pCommCtx->stMuxCtx.astMuxClientTab[ucLoop].ucMuxPort        = AT_MUX1_PORT_NO + ucLoop;
        pCommCtx->stMuxCtx.astMuxClientTab[ucLoop].enDlci           = AT_MUX_DLCI1_ID + ucLoop;

        /* 按位取出NV中配置的MUX端口主动上报状态 */
        /* Bit值与MUX端口的对应关系见下表
                 |-------------1 Oct-------------|------------1 Oct----------|
            Bit  15  14  13  12  11  10  9   8   7   6   5   4   3   2   1   0
            MUX                                  8   7   6   5   4   3   2   1
            注：Bit位取0表示主动上报，1表示不主动上报。 */
        if ((1 << ucLoop) == (stMuxReportCfg.ulMuxReportCfg & (1 << ucLoop)))
        {
            pCommCtx->stMuxCtx.astMuxClientTab[ucLoop].enRptType    = AT_HSIC_REPORT_OFF;
        }
        else
        {
            pCommCtx->stMuxCtx.astMuxClientTab[ucLoop].enRptType    = AT_HSIC_REPORT_ON;
        }
    }
    /* Modified by l60609 for DSDA Phase III, 2013-2-25, End */
    return;
}
VOS_VOID AT_MuxInit(VOS_VOID)
{
    VOS_UINT8                           ucLoop;
    TAF_AT_NVIM_MUX_SUPPORT_FLG_STRU    stMuxSupportFlg;
    /* Modified by l60609 for DSDA Phase III, 2013-2-25, Begin */
    AT_COMM_CTX_STRU                   *pCommCtx = VOS_NULL_PTR;
    VOS_UINT32                          ulLength;

    ulLength = 0;

    pCommCtx = AT_GetCommCtxAddr();

    stMuxSupportFlg.ucMuxSupportFlg = VOS_FALSE;

    NV_GetLength(en_NV_Item_Mux_Support_Flg, &ulLength);

    /* 读NV失败，直接返回 */
    if (NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_Mux_Support_Flg, &stMuxSupportFlg, ulLength))
    {
        AT_SetMuxSupportFlg(stMuxSupportFlg.ucMuxSupportFlg);
        return;
    }

    /* MUX特性不支持，直接返回 */
    if (VOS_TRUE != stMuxSupportFlg.ucMuxSupportFlg)
    {
        AT_SetMuxSupportFlg(stMuxSupportFlg.ucMuxSupportFlg);
        return;
    }

    /* 初始化MUX上下文 */
    AT_InitMuxCtx();

    AT_SetMuxSupportFlg(stMuxSupportFlg.ucMuxSupportFlg);

    for (ucLoop = 0; ucLoop < AT_MUX_AT_CHANNEL_MAX; ucLoop++)
    {
        /* 注册Mux AT通道上行数据接收回调 */
        MUX_AtRgstUlPortCallBack(pCommCtx->stMuxCtx.astMuxClientTab[ucLoop].enDlci, (RCV_UL_DLCI_DATA_FUNC)(pCommCtx->stMuxCtx.astMuxClientTab[ucLoop].pReadDataCB));

        /* 注册Mux AT通道client id */
        AT_MuxEst(ucLoop);
    }
    /* Modified by l60609 for DSDA Phase III, 2013-2-25, End */

    return;
}
VOS_UINT32 AT_MuxEst(VOS_UINT8 ucMuxAtCtxId)
{
    VOS_UINT8                           ucIndex;
    /* Modified by l60609 for DSDA Phase III, 2013-2-25, Begin */
    AT_COMM_CTX_STRU                   *pCommCtx = VOS_NULL_PTR;

    pCommCtx = AT_GetCommCtxAddr();

    ucIndex = pCommCtx->stMuxCtx.astMuxClientTab[ucMuxAtCtxId].enAtClientTabIdx;

    /* 清空对应表项 */
    PS_MEM_SET(&gastAtClientTab[ucIndex], 0x00, sizeof(AT_CLIENT_MANAGE_STRU));

    AT_ConfigTraceMsg(ucIndex, (ID_AT_CMD_MUX1 + ucMuxAtCtxId), (ID_AT_MNTN_RESULT_MUX1 + ucMuxAtCtxId));

    /* 填写用户表项 */
    gastAtClientTab[ucIndex].usClientId      = pCommCtx->stMuxCtx.astMuxClientTab[ucMuxAtCtxId].enAtClientId;
    gastAtClientTab[ucIndex].ucPortNo        = pCommCtx->stMuxCtx.astMuxClientTab[ucMuxAtCtxId].ucMuxPort;
    gastAtClientTab[ucIndex].UserType        = pCommCtx->stMuxCtx.astMuxClientTab[ucMuxAtCtxId].ucMuxUser;
    gastAtClientTab[ucIndex].ucUsed          = AT_CLIENT_USED;

    gastAtClientTab[ucIndex].Mode            = AT_CMD_MODE;
    gastAtClientTab[ucIndex].IndMode         = AT_IND_MODE;
    gastAtClientTab[ucIndex].DataMode        = AT_DATA_BUTT_MODE;
    gastAtClientTab[ucIndex].DataState       = AT_DATA_STOP_STATE;
    gastAtClientTab[ucIndex].CmdCurrentOpt   = AT_CMD_CURRENT_OPT_BUTT;
    g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_READY;
    /* Modified by l60609 for DSDA Phase III, 2013-2-25, End */

    return AT_SUCCESS;
}

/*****************************************************************************
 函 数 名  : AT_MuxReadDataCB
 功能描述  : MUX AT通道数据读回调，注册给MUX
 输入参数  : AT_MUX_DLCI_TYPE_ENUM_UINT8         enDlci
             VOS_UINT8                          *pData
             VOS_UINT16                          usDataLen
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月3日
    作    者   : L60609
    修改内容   : 新生成函数
  2.日    期   : 2013年2月25日
    作    者   : l60609
    修改内容   : DSDA PHASE III
*****************************************************************************/
VOS_UINT32 AT_MuxReadDataCB(
    AT_MUX_DLCI_TYPE_ENUM_UINT8         enDlci,
    VOS_UINT8                          *pData,
    VOS_UINT16                          usDataLen
)
{
    VOS_UINT8                           ucPortNo;
    VOS_UINT8                           ucLoop;
    /* Modified by l60609 for DSDA Phase III, 2013-2-25, Begin */
    AT_COMM_CTX_STRU                   *pCommCtx = VOS_NULL_PTR;

    pCommCtx = AT_GetCommCtxAddr();

    /* 入参合法性检查 */
    if ((VOS_NULL_PTR == pData)
     || (0 == usDataLen))
    {
        AT_ERR_LOG("AT_MuxReadDataCB: Para Error!");
        return VOS_ERR;
    }

    /* 链路号合法性检查 */
    if (VOS_OK != AT_CheckMuxDlci(enDlci))
    {
        AT_ERR_LOG("AT_MuxReadDataCB: DLCI Error!");
        return VOS_ERR;
    }

    /* 根据链路号获取AT端口号 */
    for (ucLoop = 0; ucLoop < AT_MUX_AT_CHANNEL_MAX; ucLoop++)
    {
        if (enDlci == pCommCtx->stMuxCtx.astMuxClientTab[ucLoop].enDlci)
        {
            break;
        }
    }

    if (ucLoop >= AT_MUX_AT_CHANNEL_MAX)
    {
        AT_ERR_LOG("AT_MuxReadDataCB: Dlci Error!");
        return VOS_ERR;
    }

    ucPortNo = pCommCtx->stMuxCtx.astMuxClientTab[ucLoop].ucMuxPort;
    /* Modified by l60609 for DSDA Phase III, 2013-2-25, End */

    /* 分发上行数据 */
    At_RcvFromUsbCom(ucPortNo, pData, usDataLen);

    return VOS_OK;
}

/* Added by L60609 for MUX，2012-08-03,  End */

/* Modified by s62952 for BalongV300R002 Build优化项目 2012-02-28, end */
/*****************************************************************************
函 数 名  : AT_HsicInit
功能描述  : 初始化AP和MODEM间通过HSIC承载的AT通道
输入参数  : 无
输出参数  : 无
返 回 值  : AT_SUCCESS  ----      成功；
            AT_FAILURE  ----      失败
调用函数  :
被调函数  :

修改历史      :
 1.日    期   : 2012年2月17日
   作    者   : L47619
   修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32 AT_HsicInit( VOS_VOID )
{
    UDI_OPEN_PARAM_S                    stParam;
    VOS_UINT8                           ucLoop;

    /*  产品不支持HSIC特性，直接初始化成功 */
    if (BSP_MODULE_SUPPORT != mdrv_misc_support_check(BSP_MODULE_TYPE_HSIC))
    {
        return AT_SUCCESS;
    }

    for (ucLoop = 0; ucLoop < AT_HSIC_AT_CHANNEL_MAX; ucLoop++)
    {
        stParam.devid   = (UDI_DEVICE_ID_E)g_astAtHsicCtx[ucLoop].enAcmChannelId;

        /* 打开UDI设备 */
        g_astAtHsicCtx[ucLoop].lHdlId = mdrv_udi_open(&stParam);
        if (UDI_INVALID_HANDLE == g_astAtHsicCtx[ucLoop].lHdlId)
        {
            AT_ERR_LOG1("AT_HsicInit, ERROR, Open HSIC device %d failed!", ucLoop);
            return AT_FAILURE;
        }

        /* 注册HSIC AT通道上行数据接收回调 */
        if (VOS_OK != mdrv_udi_ioctl (g_astAtHsicCtx[ucLoop].lHdlId, ACM_IOCTL_SET_READ_CB, g_astAtHsicCtx[ucLoop].pReadDataCB))
        {
            AT_ERR_LOG1("AT_HsicInit, ERROR, Set data read callback for HSIC device %d failed!", ucLoop);
            return AT_FAILURE;
        }

        /* 注册HSIC AT通道下行数据内存释放接口 */
        if (VOS_OK != mdrv_udi_ioctl (g_astAtHsicCtx[ucLoop].lHdlId, ACM_IOCTL_SET_FREE_CB, g_astAtHsicCtx[ucLoop].pFreeDlDataCB))
        {
            AT_ERR_LOG1("AT_HsicInit, ERROR, Set memory free callback for HSIC device %d failed!", ucLoop);
            return AT_FAILURE;
        }

        /* 设置HSIC AT通道上行数据buffer规格 */
        if(AT_SUCCESS != AT_HsicInitUlDataBuf(g_astAtHsicCtx[ucLoop].lHdlId, AT_HSIC_UL_DATA_BUFF_SIZE, AT_HSIC_UL_DATA_BUFF_NUM))
        {
            AT_ERR_LOG1("AT_HsicInit, ERROR, AT_HsicInitUlDataBuf for HSIC device %d failed!", ucLoop);
            return AT_FAILURE;
        }

        /* 注册HSIC AT通道client id */
        AT_HsicEst(ucLoop);
    }

    return AT_SUCCESS;
}


VOS_UINT32 AT_HsicEst ( VOS_UINT8   ucHsicAtCtxId )
{
    VOS_UINT8   ucIndex;

    ucIndex = g_astAtHsicCtx[ucHsicAtCtxId].ucAtClientTabIdx;

    /* 清空对应表项 */
    PS_MEM_SET(&gastAtClientTab[ucIndex],0x00,sizeof(AT_CLIENT_MANAGE_STRU));

    AT_ConfigTraceMsg(ucIndex, (ID_AT_CMD_HSIC1 + ucHsicAtCtxId), (ID_AT_MNTN_RESULT_HSIC1 + ucHsicAtCtxId));

    /* 填写用户表项 */
    gastAtClientTab[ucIndex].usClientId      = g_astAtHsicCtx[ucHsicAtCtxId].enAtClientId;
    gastAtClientTab[ucIndex].ucPortNo        = g_astAtHsicCtx[ucHsicAtCtxId].ucHsicPort;
    gastAtClientTab[ucIndex].UserType        = g_astAtHsicCtx[ucHsicAtCtxId].ucHsicUser;
    gastAtClientTab[ucIndex].ucUsed          = AT_CLIENT_USED;

    gastAtClientTab[ucIndex].Mode            = AT_CMD_MODE;
    gastAtClientTab[ucIndex].IndMode         = AT_IND_MODE;
    gastAtClientTab[ucIndex].DataMode        = AT_DATA_BUTT_MODE;
    gastAtClientTab[ucIndex].DataState       = AT_DATA_STOP_STATE;
    gastAtClientTab[ucIndex].CmdCurrentOpt   = AT_CMD_CURRENT_OPT_BUTT;
    g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_READY;

    return AT_SUCCESS;
}

/*****************************************************************************
函 数 名  : AT_HsicOneReadDataCB
功能描述  : HSIC AT通道1(对应于HSIC ACM通道ID为UDI_ACM_HSIC_ACM0_ID)数据读回调，注册给底软
            由于向底软注册的上行接收和下行释放回调函数中，入口参数均无通道索引，无法区分是哪个通道来的数据，
            所以对每一个HSIC AT通道，都提供一个上行接收及下行释放回调，以此来区分不同的通道
输入参数  : 无
输出参数  : 无
返 回 值  : 无
调用函数  :
被调函数  :

修改历史      :
 1.日    期   : 2012年2月17日
   作    者   : L47619
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AT_HsicOneReadDataCB( VOS_VOID )
{
    VOS_UINT8                           ucPortNo;
    VOS_UINT8                          *pucBuf;
    VOS_UINT32                          ulLen;

    pucBuf  = VOS_NULL_PTR;
    ulLen   = 0;


    /* 获取上行数据缓存 */
    if ( AT_SUCCESS == AT_HsicGetUlDataBuf(g_astAtHsicCtx[AT_HSIC_AT_CHANNEL_INDEX_ONE].lHdlId, &pucBuf, &ulLen) )
    {
        /*HSIC AT端口号 */
        ucPortNo  = AT_HSIC1_PORT_NO;

        /* 根据设备当前模式，分发上行数据 */
        At_RcvFromUsbCom(ucPortNo, pucBuf, (VOS_UINT16)ulLen);

        /* AT命令处理完毕，释放上行缓存 */
        if (AT_SUCCESS != AT_HsicFreeUlDataBuf(g_astAtHsicCtx[AT_HSIC_AT_CHANNEL_INDEX_ONE].lHdlId, pucBuf, ulLen))
        {
            AT_ERR_LOG("AT_HsicOneReadDataCB, WARNING, Free UL HSIC AT buffer failed !");
        }
    }
    else
    {
        AT_ERR_LOG("AT_HsicOneReadDataCB, WARNING, AT_HsicGetUlDataBuf failed !");
    }

    return;
}


/*****************************************************************************
函 数 名  : AT_HsicTwoReadDataCB
功能描述  : HSIC AT通道2(对应于HSIC ACM通道ID为UDI_ACM_HSIC_ACM2_ID)数据读回调，注册给底软
            由于向底软注册的上行接收和下行释放回调函数中，入口参数均无通道索引，无法区分是哪个通道来的数据，
            所以对每一个HSIC AT通道，都提供一个上行接收及下行释放回调，以此来区分不同的通道
输入参数  : 无
输出参数  : 无
返 回 值  : 无
调用函数  :
被调函数  :

修改历史      :
 1.日    期   : 2012年2月17日
   作    者   : L47619
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AT_HsicTwoReadDataCB( VOS_VOID )
{
    VOS_UINT8                           ucPortNo;
    VOS_UINT8                          *pucBuf;
    VOS_UINT32                          ulLen;

    pucBuf  = VOS_NULL_PTR;
    ulLen   = 0;


    /* 获取上行数据缓存 */
    if ( AT_SUCCESS == AT_HsicGetUlDataBuf(g_astAtHsicCtx[AT_HSIC_AT_CHANNEL_INDEX_TWO].lHdlId, &pucBuf, &ulLen) )
    {
        /*HSIC AT端口号 */
        ucPortNo  = AT_HSIC2_PORT_NO;

        /* 根据设备当前模式，分发上行数据 */
        At_RcvFromUsbCom(ucPortNo, pucBuf, (VOS_UINT16)ulLen);

        /* AT命令处理完毕，释放上行缓存 */
        if (AT_SUCCESS != AT_HsicFreeUlDataBuf(g_astAtHsicCtx[AT_HSIC_AT_CHANNEL_INDEX_TWO].lHdlId, pucBuf, ulLen))
        {
            AT_ERR_LOG("AT_HsicTwoReadDataCB, WARNING, Free UL HSIC AT buffer failed !");
        }
    }
    else
    {
        AT_ERR_LOG("AT_HsicTwoReadDataCB, WARNING, AT_HsicGetUlDataBuf failed !");
    }

    return;
}


/*****************************************************************************
函 数 名  : AT_HsicThreeReadDataCB
功能描述  : HSIC AT通道3(对应于HSIC ACM通道ID为UDI_ACM_HSIC_ACM4_ID)数据读回调，注册给底软
            由于向底软注册的上行接收和下行释放回调函数中，入口参数均无通道索引，无法区分是哪个通道来的数据，
            所以对每一个HSIC AT通道，都提供一个上行接收及下行释放回调，以此来区分不同的通道
输入参数  : 无
输出参数  : 无
返 回 值  : 无
调用函数  :
被调函数  :

修改历史      :
 1.日    期   : 2012年2月17日
   作    者   : L47619
   修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AT_HsicThreeReadDataCB( VOS_VOID )
{
    VOS_UINT8                           ucPortNo;
    VOS_UINT8                          *pucBuf;
    VOS_UINT32                          ulLen;

    pucBuf  = VOS_NULL_PTR;
    ulLen   = 0;


    /* 获取上行数据缓存 */
    if ( AT_SUCCESS == AT_HsicGetUlDataBuf(g_astAtHsicCtx[AT_HSIC_AT_CHANNEL_INDEX_THREE].lHdlId, &pucBuf, &ulLen) )
    {
        /*HSIC AT端口号 */
        ucPortNo  = AT_HSIC3_PORT_NO;

        /* 根据设备当前模式，分发上行数据 */
        At_RcvFromUsbCom(ucPortNo, pucBuf, (VOS_UINT16)ulLen);

        /* AT命令处理完毕，释放上行缓存 */
        if (AT_SUCCESS != AT_HsicFreeUlDataBuf(g_astAtHsicCtx[AT_HSIC_AT_CHANNEL_INDEX_THREE].lHdlId, pucBuf, ulLen))
        {
            AT_ERR_LOG("AT_HsicThreeReadDataCB, WARNING, Free UL HSIC AT buffer failed !");
        }
    }
    else
    {
        AT_ERR_LOG("AT_HsicThreeReadDataCB, WARNING, AT_HsicGetUlDataBuf failed !");
    }

    return;
}

/* Added by L47619 for V7R1C50 A-GPS Project, 2012/06/28, begin */
/*****************************************************************************
函 数 名  : AT_HsicFourReadDataCB
功能描述  : HSIC AT通道4(对应于HSIC ACM通道ID为UDI_ACM_HSIC_ACM10_ID)数据读回调，注册给底软
            由于向底软注册的上行接收和下行释放回调函数中，入口参数均无通道索引，无法区分是哪个通道来的数据，
            所以对每一个HSIC AT通道，都提供一个上行接收及下行释放回调，以此来区分不同的通道
输入参数  : 无
输出参数  : 无
返 回 值  : 无
调用函数  :
被调函数  :

修改历史      :
 1.日    期   : 2012年6月28日
   作    者   : L47619
   修改内容   : V7R1C50 A-GPS项目新增

*****************************************************************************/
VOS_VOID AT_HsicFourReadDataCB( VOS_VOID )
{
    VOS_UINT8                           ucPortNo;
    VOS_UINT8                          *pucBuf;
    VOS_UINT32                          ulLen;

    pucBuf  = VOS_NULL_PTR;
    ulLen   = 0;


    /* 获取上行数据缓存 */
    if ( AT_SUCCESS == AT_HsicGetUlDataBuf(g_astAtHsicCtx[AT_HSIC_AT_CHANNEL_INDEX_FOUR].lHdlId, &pucBuf, &ulLen) )
    {
        /*HSIC AT端口号 */
        ucPortNo  = AT_HSIC4_PORT_NO;

        /* 根据设备当前模式，分发上行数据 */
        At_RcvFromUsbCom(ucPortNo, pucBuf, (VOS_UINT16)ulLen);

        /* AT命令处理完毕，释放上行缓存 */
        if (AT_SUCCESS != AT_HsicFreeUlDataBuf(g_astAtHsicCtx[AT_HSIC_AT_CHANNEL_INDEX_FOUR].lHdlId, pucBuf, ulLen))
        {
            AT_ERR_LOG("AT_HsicFourReadDataCB, WARNING, Free UL HSIC AT buffer failed !");
        }
    }
    else
    {
        AT_ERR_LOG("AT_HsicFourReadDataCB, WARNING, AT_HsicGetUlDataBuf failed !");
    }

    return;
}
/* Added by L47619 for V7R1C50 A-GPS Project, 2012/06/28, end */

VOS_VOID AT_HsicOneFreeDlDataBuf(VOS_UINT8 *pucBuf)
{
    /* 调用BSP释放内存的接口 */
    BSP_FREE(pucBuf);
    return;
}
VOS_VOID AT_HsicTwoFreeDlDataBuf(VOS_UINT8 *pucBuf)
{
    /* 调用BSP释放内存的接口 */
    BSP_FREE(pucBuf);
    return;
}
VOS_VOID AT_HsicThreeFreeDlDataBuf(VOS_UINT8 *pucBuf)
{
    /* 调用BSP释放内存的接口 */
    BSP_FREE(pucBuf);
    return;
}


/* Added by L47619 for V7R1C50 A-GPS Project, 2012/06/28, begin */
/*****************************************************************************
函 数 名  : AT_HsicFourFreeDlDataBuf
功能描述  : 释放HSIC AT通道4(对应于HSIC ACM通道ID为UDI_ACM_HSIC_ACM10_ID)下行数据内存，注册给底软使用
            由于向底软注册的上行接收和下行释放回调函数中，入口参数均无通道索引，无法区分是哪个通道来的数据，
            所以对每一个HSIC AT通道，都提供一个上行接收及下行释放回调，以此来区分不同的通道
输入参数  : pucBuf     ----      下行数据指针
            ulLen      ----      数据长度
输出参数  : 无
返 回 值  : AT_SUCCESS ----      成功；
            AT_FAILURE ----      失败
调用函数  :
被调函数  :

修改历史      :
 1.日    期   : 2012年2月17日
   作    者   : L47619
   修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID AT_HsicFourFreeDlDataBuf(VOS_UINT8 *pucBuf)
{
    /* 调用BSP释放内存的接口 */
    BSP_FREE(pucBuf);
    return;
}
/* Added by L47619 for V7R1C50 A-GPS Project, 2012/06/28, end */



VOS_UINT32 AT_HsicGetUlDataBuf(
    UDI_HANDLE                           ulUdiHdl,
    VOS_UINT8                          **ppucBuf,
    VOS_UINT32                          *pulLen
)
{
    ACM_WR_ASYNC_INFO                   stCtlParam;
    VOS_INT32                           lResult;


    PS_MEM_SET(&stCtlParam, 0x00, sizeof(stCtlParam));


    /* 获取底软上行数据*/
    lResult = mdrv_udi_ioctl(ulUdiHdl, ACM_IOCTL_GET_RD_BUFF, &stCtlParam);
    if ( VOS_OK != lResult )
    {
        AT_ERR_LOG1("AT_HsicGetUlDataBuf, WARNING, Get HSIC AT buffer failed code %d!",
                  lResult);
        return AT_FAILURE;
    }

    if ( (VOS_NULL_PTR == stCtlParam.pVirAddr)
      || (AT_INIT_DATA_LEN == stCtlParam.u32Size))
    {
        AT_ERR_LOG("AT_HsicGetUlDataBuf, WARNING, Data buffer error");

        return AT_FAILURE;
    }

    *ppucBuf = (VOS_UINT8 *)stCtlParam.pVirAddr;
    *pulLen  = stCtlParam.u32Size;

    return AT_SUCCESS;
}


/*****************************************************************************
函 数 名  : AT_HsicFreeUlDataBuf
功能描述  : 释放上行数据BSP缓存
输入参数  : ulUdiHdl   ----       HSIC AT通道的句柄
            pucBuf     ----      上行数据指针
            ulLen      ----       数据长度
输出参数  : 无
返 回 值  : AT_SUCCESS ----      成功；
            AT_FAILURE ----      失败
调用函数  :
被调函数  :

修改历史      :
 1.日    期   : 2012年2月17日
   作    者   : L47619
   修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AT_HsicFreeUlDataBuf(
    UDI_HANDLE                          ulUdiHdl,
    VOS_UINT8                          *pucBuf,
    VOS_UINT32                          ulLen
)
{
    ACM_WR_ASYNC_INFO                   stCtlParam;
    VOS_INT32                           lResult;

    /* 填写需要释放的内存指针 */
    stCtlParam.pVirAddr = (VOS_CHAR*)pucBuf;
    stCtlParam.pPhyAddr = VOS_NULL_PTR;
    stCtlParam.u32Size  = ulLen;
    stCtlParam.pDrvPriv = VOS_NULL_PTR;

    lResult = mdrv_udi_ioctl(ulUdiHdl, ACM_IOCTL_RETURN_BUFF, &stCtlParam);

    if ( VOS_OK != lResult )
    {
        AT_ERR_LOG1("AT_HsicFreeUlDataBuf, ERROR, Return HSIC AT buffer failed, code %d!\r\n",
                  lResult);

        return AT_FAILURE;
    }
    return AT_SUCCESS;
}

/*****************************************************************************
函 数 名  : AT_HsicInitUlDataBuf
功能描述  : 向底软获取HSIC AT通道上行所需的缓存
输入参数  : ulEachBuffSize ----   底软上行数据块BUFFER大小　
            ulTotalBuffNum ----   底软上行数据块BUFFER个数
输出参数  : 无
返 回 值  : AT_SUCCESS     ----   成功；
            AT_FAILURE     ----   失败
调用函数  :
被调函数  :

修改历史      :
 1.日    期   : 2012年2月17日
   作    者   : L47619
   修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32 AT_HsicInitUlDataBuf(
    UDI_HANDLE                          ulUdiHdl,
    VOS_UINT32                          ulEachBuffSize,
    VOS_UINT32                          ulTotalBuffNum
)
{
    ACM_READ_BUFF_INFO                  stReadBuffInfo;
    VOS_INT32                           lResult;

    /* 填写需要向底软申请的内存指针 */
    stReadBuffInfo.u32BuffSize = ulEachBuffSize;
    stReadBuffInfo.u32BuffNum  = ulTotalBuffNum;

    /* 调用底软接口获取缓存 */
    lResult = mdrv_udi_ioctl(ulUdiHdl, ACM_IOCTL_RELLOC_READ_BUFF, &stReadBuffInfo);

    if ( VOS_OK != lResult )
    {
        AT_ERR_LOG1("AT_HsicInitUlDataBuf, WARNING, Initialize data buffer failed code %d!\r\n",
                  lResult);

        return AT_FAILURE;
    }

    return AT_SUCCESS;
}


VOS_UINT32 AT_HsicWriteData(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                          *pucBuf,
    VOS_UINT32                          ulLen
)
{
    ACM_WR_ASYNC_INFO                   stCtlParam;
    VOS_INT32                           lResult;
    VOS_UINT8                           ucLoop;

    for (ucLoop = 0; ucLoop < AT_HSIC_AT_CHANNEL_MAX; ucLoop++)
    {
        if (ucIndex == g_astAtHsicCtx[ucLoop].ucAtClientTabIdx)
        {
            break;
        }
    }

    if (ucLoop >= AT_HSIC_AT_CHANNEL_MAX)
    {
        AT_WARN_LOG1("AT_HsicWriteData, WARNING, can't find match index : %d!",
                  ucIndex);
        return AT_FAILURE;
    }

    /* 如果句柄无效则直接返回 */
    if (UDI_INVALID_HANDLE == g_astAtHsicCtx[ucLoop].lHdlId)
    {
        return AT_FAILURE;
    }

    /* 将写入数据内存地址放入底软的ACM_WR_ASYNC_INFO结构体中 */
    stCtlParam.pVirAddr                 = (VOS_CHAR*)pucBuf;
    stCtlParam.pPhyAddr                 = VOS_NULL_PTR;
    stCtlParam.u32Size                  = ulLen;
    stCtlParam.pDrvPriv                 = VOS_NULL_PTR;

    /* 调用底软的接口，使用异步方式进行数据写操作 */
    lResult = mdrv_udi_ioctl(g_astAtHsicCtx[ucLoop].lHdlId, ACM_IOCTL_WRITE_ASYNC, &stCtlParam);

    /* 写入失败 */
    if ( VOS_OK != lResult )
    {
        AT_WARN_LOG1("AT_HsicWriteData, WARNING, Write data failed with code!",
                  lResult);

        return AT_FAILURE;
    }

    return AT_SUCCESS;
}

/*****************************************************************************
函 数 名  : AT_HsicModemEnableCB
功能描述  : MODEM设备使能通知回调，注册给底软
输入参数  : ucEnable    ----  是否使能
输出参数  : 无
返 回 值  : 无
调用函数  :
被调函数  :

修改历史      :
  1.日    期   : 2013年05月25日
    作    者   : 范晶/00179208
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID AT_HsicModemEnableCB(VOS_UINT8 ucEnable)
{
    VOS_UINT8                           ucIndex;

    ucIndex = AT_CLIENT_TAB_HSIC_MODEM_INDEX;

    AT_ModemeEnableCB(ucIndex, ucEnable);

    return;
}

/*****************************************************************************
 函 数 名  : AT_HsicModemReadDataCB
 功能描述  : HSIC MODEM设备数据读回调，注册给底软
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年05月25日
    作    者   : 范晶/00179208
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID AT_HsicModemReadDataCB( VOS_VOID )
{
    VOS_UINT8                           ucIndex;
    VOS_UINT8                           ucDlci;
    IMM_ZC_STRU                        *pstBuf;

    pstBuf          = VOS_NULL_PTR;

    /* HSIC MODEM索引号 */
    ucIndex     = AT_CLIENT_TAB_HSIC_MODEM_INDEX;

    if (AT_SUCCESS == AT_ModemGetUlDataBuf(ucIndex, &pstBuf))
    {
        /*MODEM链路号 */
        ucDlci      = AT_MODEM_USER_DLCI;

        /* 根据设备当前模式，分发上行数据 */
        At_ModemDataInd(ucIndex, ucDlci, pstBuf);
    }

    return;
}

/*****************************************************************************
函 数 名  : AT_HsicModemReadMscCB
功能描述  : 管脚信号处理
输入参数  : pstRcvedMsc ----      底软通知管脚信号指针
输出参数  : 无
返 回 值  : AT_SUCCESS  ----      成功；
            AT_FAILURE  ----      失败
调用函数  :
被调函数  :

修改历史      :
  1.日    期   : 2013年05月25日
    作    者   : 范晶/00179208
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID AT_HsicModemReadMscCB(AT_DCE_MSC_STRU *pstRcvedMsc)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT8                           ucDlci;


    if ( VOS_NULL_PTR == pstRcvedMsc )
    {
        AT_WARN_LOG("AT_HsicModemReadMscCB, WARNING, Receive NULL pointer MSC info!");

        return;
    }

    /* HSIC MODEM索引号 */
    ucIndex     = AT_CLIENT_TAB_HSIC_MODEM_INDEX;

    /*MODEM链路号 */
    ucDlci      = AT_MODEM_USER_DLCI;

     /* 输入管脚信号可维可测 */
    AT_MNTN_TraceInputMsc(ucIndex, pstRcvedMsc);

    At_ModemMscInd(ucIndex, ucDlci, pstRcvedMsc);

    return;
}
VOS_VOID AT_HsicModemInit(VOS_VOID)
{
    UDI_OPEN_PARAM_S                    stParam;

    VOS_UINT8                           ucIndex;

    ucIndex         = AT_CLIENT_TAB_HSIC_MODEM_INDEX;
    stParam.devid   = UDI_ACM_HSIC_MODEM0_ID;

    /* 打开Device，获得ID */
    g_alAtUdiHandle[ucIndex] = mdrv_udi_open(&stParam);

    if (UDI_INVALID_HANDLE == g_alAtUdiHandle[ucIndex])
    {
        AT_ERR_LOG("AT_HsicModemInit, ERROR, Open usb modem device failed!");

        return;
    }

    /* 注册MODEM设备上行数据接收回调 */
    if (VOS_OK != mdrv_udi_ioctl (g_alAtUdiHandle[ucIndex], ACM_IOCTL_SET_READ_CB, AT_HsicModemReadDataCB))
    {
        AT_ERR_LOG("AT_HsicModemInit, ERROR, Set data read callback for modem failed!");

        return;
    }

    /* 注册MODEM下行数据内存释放接口 */
    if (VOS_OK != mdrv_udi_ioctl (g_alAtUdiHandle[ucIndex], ACM_IOCTL_SET_FREE_CB, AT_ModemFreeDlDataBuf))
    {
        AT_ERR_LOG("AT_HsicModemInit, ERROR, Set memory free callback for modem failed!");

        return;
    }

    /* 注册管脚信号通知回调 */
    if (VOS_OK != mdrv_udi_ioctl (g_alAtUdiHandle[ucIndex], ACM_MODEM_IOCTL_SET_MSC_READ_CB, AT_HsicModemReadMscCB))
    {
        AT_ERR_LOG("AT_HsicModemInit, ERROR, Set msc read callback for modem failed!");

        return;
    }

    /* 注册MODEM设备使能、去使能通知回调 */
    if (VOS_OK != mdrv_udi_ioctl (g_alAtUdiHandle[ucIndex], ACM_MODEM_IOCTL_SET_REL_IND_CB, AT_HsicModemEnableCB))
    {
        AT_ERR_LOG("AT_HsicModemInit, ERROR, Set enable callback for modem failed!");

        return;
    }

    /* 设置HSIC MODEM设备上行数据buffer规格 */
    AT_ModemInitUlDataBuf(ucIndex, AT_MODEM_UL_DATA_BUFF_SIZE, AT_MODEM_UL_DATA_BUFF_NUM);

    /* 初始化MODME统计信息 */
    AT_InitModemStats();

    /*注册client id*/
    At_ModemEst(ucIndex, AT_CLIENT_ID_HSIC_MODEM, AT_HSIC_MODEM_PORT_NO);

    AT_ConfigTraceMsg(ucIndex, ID_AT_CMD_HSIC_MODEM, ID_AT_MNTN_RESULT_HSIC_MODEM);

    return;
}

/*****************************************************************************
 函 数 名  : AT_HsicModemClose
 功能描述  : USB拔除时，关闭MODEM设备
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年05月25日
    作    者   : 范晶/00179208
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID AT_HsicModemClose(VOS_VOID)
{
    VOS_UINT8                           ucIndex;

    ucIndex = AT_CLIENT_TAB_HSIC_MODEM_INDEX;

    /* 去注册MODEM流控点(经TTF确认未注册流控点也可以去注册流控点)。 */
    AT_DeRegModemPsDataFCPoint(ucIndex, gastAtClientTab[ucIndex].ucPsRabId);

    if (UDI_INVALID_HANDLE != g_alAtUdiHandle[ucIndex])
    {
        mdrv_udi_close(g_alAtUdiHandle[ucIndex]);

        g_alAtUdiHandle[ucIndex] = UDI_INVALID_HANDLE;

        vos_printf("AT_HsicModemClose....\n");
    }

    return;
}

/*****************************************************************************
 函 数 名  : AT_SendDataToHsic
 功能描述  : 将HSIC AT通道的下行发送数据发送给HSIC
 输入参数  :
             ucIndex      ----    AT通道索引
             pucDataBuf   ----    待发送下行数据内存指针
             usLen        ----    数据长度
 输出参数  :
 返 回 值  : AT_SUCCESS ----      成功；
             AT_FAILURE ----      失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年2月17日
    作    者   : L47619
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AT_SendDataToHsic(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                          *pucDataBuf,
    VOS_UINT16                          usLen
)
{
    VOS_UINT8                          *pucData;

    pucData = VOS_NULL_PTR;

    /* 申请内存，用于存储下行数据 */
    pucData = (VOS_UINT8 *)BSP_MALLOC((VOS_UINT32)usLen, MEM_NORM_DDR_POOL);
    if ( VOS_NULL_PTR == pucData )
    {
        AT_WARN_LOG("AT_SendDataToHsic, WARNING, Alloc DL memory failed!");

        return AT_FAILURE;
    }

    /* 将下行数据从字节流消息中拷出 */
    AT_MemSingleCopy(pucData, pucDataBuf, usLen);

    /* 将数据写往HSIC AT设备，写成功后内存由底软负责释放 */
    if ( AT_SUCCESS != AT_HsicWriteData(ucIndex, pucData, usLen) )
    {
        BSP_FREE(pucData);

        return AT_FAILURE;
    }

    return AT_SUCCESS;
}

#endif

/*****************************************************************************
 函 数 名  : AT_SendMuxSelResultData
 功能描述  : AT给MUX发送主动上报的数据
 输入参数  : VOS_UINT8                           ucIndex
             VOS_UINT8                          *pData
             VOS_UINT16                          usLen
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月7日
    作    者   : l60609
    修改内容   : 新生成函数
  2.日    期   : 2013年2月25日
    作    者   : l60609
    修改内容   : DSDA Phase III
*****************************************************************************/
VOS_UINT32 AT_SendMuxSelResultData(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                          *pData,
    VOS_UINT16                          usLen
)
{
    VOS_UINT8                           ucLoop;
    /* Modified by l60609 for DSDA Phase III, 2013-2-25, Begin */
    AT_COMM_CTX_STRU                   *pCommCtx = VOS_NULL_PTR;

    pCommCtx = AT_GetCommCtxAddr();

    /* 匹配是否为MUX的index */
    for (ucLoop = 0; ucLoop < AT_MUX_AT_CHANNEL_MAX; ucLoop++)
    {
        if (ucIndex == pCommCtx->stMuxCtx.astMuxClientTab[ucLoop].enAtClientTabIdx)
        {
            break;
        }
    }

    if (ucLoop >= AT_MUX_AT_CHANNEL_MAX)
    {
        return VOS_ERR;
    }

    /* 该通道不允许主动上报AT命令 */
    if (AT_HSIC_REPORT_OFF == pCommCtx->stMuxCtx.astMuxClientTab[ucLoop].enRptType)
    {
        return VOS_ERR;
    }
    /* Modified by l60609 for DSDA Phase III, 2013-2-25, End */

    AT_SendMuxResultData(ucIndex, pData, usLen);

    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : AT_SendMuxResultData
 功能描述  : AT给MUX发送结果数据
 输入参数  : VOS_UINT8                           ucIndex
             VOS_UINT8                          *pData
             VOS_UINT16                          usLen
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月6日
    作    者   : l60609
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AT_SendMuxResultData(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                          *pData,
    VOS_UINT16                          usLen
)
{
    AT_MUX_DLCI_TYPE_ENUM_UINT8         enDlci;
    VOS_UINT32                          ulRslt;

    enDlci = AT_MUX_DLCI_TYPE_BUTT;

    /* MUX特性是否支持，不支持直接返回失败 */
    if (VOS_TRUE != AT_GetMuxSupportFlg())
    {
        return AT_FAILURE;
    }

    /* 获取Dlci */
    ulRslt = AT_GetMuxDlciFromClientIdx(ucIndex, &enDlci);

    if (VOS_TRUE != ulRslt)
    {
        return AT_FAILURE;
    }

    AT_MNTN_TraceCmdResult(ucIndex, pData, usLen);

    /* 向MUX发送数据 */
    MUX_DlciDlDataSend(enDlci, pData, usLen);

    return AT_SUCCESS;
}


/*****************************************************************************
 函 数 名  : AT_SndDipcPdpActInd
 功能描述  : AT向DIPC通道发送PDP激活消息
 输入参数  : ucCid          ----  CID
             ucRabId        ----  RABID
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年2月17日
    作    者   : L47619
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID AT_SndDipcPdpActInd(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucCid,
    VOS_UINT8                           ucRabId
)
{
    VOS_UINT32                      ulLength;
    AT_DIPC_PDP_ACT_STRU           *pstAtDipcPdpAct;
    AT_MODEM_PS_CTX_STRU           *pstPsModemCtx = VOS_NULL_PTR;

    ulLength        = sizeof( AT_DIPC_PDP_ACT_STRU ) - VOS_MSG_HEAD_LENGTH;
    pstAtDipcPdpAct = ( AT_DIPC_PDP_ACT_STRU *)PS_ALLOC_MSG( WUEPS_PID_AT, ulLength );

    if ( VOS_NULL_PTR == pstAtDipcPdpAct )
    {
        /*打印出错信息---申请消息包失败:*/
        AT_WARN_LOG( "AT_SndDipcPdpActInd:ERROR:Allocates a message packet for AT_DIPC_PDP_ACT_STRU FAIL!" );
        return;
    }

    pstPsModemCtx = AT_GetModemPsCtxAddrFromClientId(usClientId);

    /*填写消息头:*/
    pstAtDipcPdpAct->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstAtDipcPdpAct->ulSenderPid     = WUEPS_PID_AT;
    pstAtDipcPdpAct->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstAtDipcPdpAct->ulReceiverPid   = PS_PID_APP_DIPC;
    pstAtDipcPdpAct->ulLength        = ulLength;
    /*填写消息体:*/
    pstAtDipcPdpAct->enMsgType       = ID_AT_DIPC_PDP_ACT_IND;
    pstAtDipcPdpAct->ucRabId         = ucRabId;
    pstAtDipcPdpAct->enUdiDevId      = (UDI_DEVICE_ID_E)pstPsModemCtx->astChannelCfg[ucCid].ulRmNetId;

    /*发送该消息:*/
    if ( VOS_OK != PS_SEND_MSG( WUEPS_PID_AT, pstAtDipcPdpAct ) )
    {
        /*打印警告信息---发送消息失败:*/
        AT_WARN_LOG( "AT_SndDipcPdpActInd:WARNING:SEND AT_DIPC_PDP_ACT_STRU msg FAIL!" );
    }

    return;
}



/*****************************************************************************
 函 数 名  : AT_SndDipcPdpDeactInd
 功能描述  : AT向DIPC通道发送PDP去激活消息
 输入参数  : ucRabId        ----  RABID
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年2月17日
    作    者   : L47619
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID AT_SndDipcPdpDeactInd(
    VOS_UINT8                           ucRabId
)
{
    VOS_UINT32                      ulLength;
    AT_DIPC_PDP_DEACT_STRU         *pstAtDipcPdpDeact;

    ulLength          = sizeof( AT_DIPC_PDP_DEACT_STRU ) - VOS_MSG_HEAD_LENGTH;
    pstAtDipcPdpDeact = ( AT_DIPC_PDP_DEACT_STRU *)PS_ALLOC_MSG( WUEPS_PID_AT, ulLength );

    if ( VOS_NULL_PTR == pstAtDipcPdpDeact )
    {
        /*打印出错信息---申请消息包失败:*/
        AT_WARN_LOG( "AT_SndDipcPdpDeactInd:ERROR:Allocates a message packet for AT_DIPC_PDP_DEACT_STRU FAIL!" );
        return;
    }

    /*填写消息头:*/
    pstAtDipcPdpDeact->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstAtDipcPdpDeact->ulSenderPid     = WUEPS_PID_AT;
    pstAtDipcPdpDeact->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstAtDipcPdpDeact->ulReceiverPid   = PS_PID_APP_DIPC;
    pstAtDipcPdpDeact->ulLength        = ulLength;
    /*填写消息体:*/
    pstAtDipcPdpDeact->enMsgType       = ID_AT_DIPC_PDP_REL_IND;
    pstAtDipcPdpDeact->ucRabId         = ucRabId;

    /*发送该消息:*/
    if ( VOS_OK != PS_SEND_MSG( WUEPS_PID_AT, pstAtDipcPdpDeact ) )
    {
        /*打印警告信息---发送消息失败:*/
        AT_WARN_LOG( "AT_SndDipcPdpDeactInd:WARNING:SEND AT_DIPC_PDP_DEACT_STRU msg FAIL!" );
    }

    return;
}


/*****************************************************************************
 函 数 名  : AT_SetAtChdataCidActStatus
 功能描述  : AP-MODEM形态，设置g_astAtChdataCfg中指定CID的PDP的激活状态
 输入参数  : ucCid      ----      指定CID
             ulIsCidAct ----      指定CID的PDP的激活状态
 输出参数  :
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年2月17日
    作    者   : L47619
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID AT_SetAtChdataCidActStatus(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucCid,
    VOS_UINT32                          ulIsCidAct
)
{
    AT_MODEM_PS_CTX_STRU               *pstPsModemCtx = VOS_NULL_PTR;

    /* 检查CID合法性 */
    if ( ucCid > TAF_MAX_CID_NV)
    {
        AT_ERR_LOG1("AT_SetAtChdataCidActStatus, WARNING, CID error:%d\r\n", ucCid);
        return;
    }

    pstPsModemCtx = AT_GetModemPsCtxAddrFromClientId(usClientId);

    /* 清除CID与数传通道的映射关系 */
    pstPsModemCtx->astChannelCfg[ucCid].ulRmNetActFlg = ulIsCidAct;

    return;
}



/*****************************************************************************
 函 数 名  : AT_CleanAtChdataCfg
 功能描述  : AP-MODEM形态，将CID与数传通道的映射关系清除
 输入参数  :
             ucCid      ----      指定CID
 输出参数  :
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年2月17日
    作    者   : L47619
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID AT_CleanAtChdataCfg(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucCid
)
{
    AT_MODEM_PS_CTX_STRU               *pstPsModemCtx = VOS_NULL_PTR;

    /* 检查CID合法性 */
    if ( ucCid > TAF_MAX_CID_NV)
    {
        AT_ERR_LOG1("AT_CleanAtChdataCfg, WARNING, CID error:%d\r\n", ucCid);
        return;
    }

    pstPsModemCtx = AT_GetModemPsCtxAddrFromClientId(usClientId);

    /* 清除CID与数传通道的映射关系 */
    pstPsModemCtx->astChannelCfg[ucCid].ulUsed     = VOS_FALSE;
    pstPsModemCtx->astChannelCfg[ucCid].ulRmNetId  = AT_PS_INVALID_RMNET_ID;

    /* 将指定CID的PDP的激活状态设置为未激活态 */
    pstPsModemCtx->astChannelCfg[ucCid].ulRmNetActFlg = VOS_FALSE;

    return;
}
VOS_UINT32 AT_CheckHsicUser(VOS_UINT8 ucIndex)
{
#if (FEATURE_ON == FEATURE_AT_HSIC)
    if ( (AT_HSIC1_USER != gastAtClientTab[ucIndex].UserType)
       && (AT_HSIC2_USER != gastAtClientTab[ucIndex].UserType)
       && (AT_HSIC3_USER != gastAtClientTab[ucIndex].UserType)
       /* Modified by L47619 for V7R1C50 A-GPS Project, 2012/06/28, begin */
       && (AT_HSIC4_USER != gastAtClientTab[ucIndex].UserType))
       /* Modified by L47619 for V7R1C50 A-GPS Project, 2012/06/28, end */
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
#else
    return VOS_FALSE;
#endif

}

/* Added by l60609 for AP适配项目 ，2012-09-10 Begin */
/*****************************************************************************
 函 数 名  : AT_CheckAppUser
 功能描述  : 检查当前命令下发端口是否为APP通道
 输入参数  : VOS_UINT8 ucIndex
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月10日
    作    者   : L60609
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32 AT_CheckAppUser(VOS_UINT8 ucIndex)
{
    if (AT_APP_USER != gastAtClientTab[ucIndex].UserType)
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;

}
/* Added by l60609 for AP适配项目 ，2012-09-10 End */

VOS_UINT32 AT_CheckUsbcomUser(VOS_UINT8 ucIndex)
{
    if (AT_USBCOM_USER != gastAtClientTab[ucIndex].UserType)
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;

}

VOS_UINT32 AT_CheckCtrUser(VOS_UINT8 ucIndex)
{
    if (AT_CTR_USER != gastAtClientTab[ucIndex].UserType)
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;

}
/*****************************************************************************
 函 数 名  : AT_CheckNdisUser
 功能描述  : 检查当前命令下发端口是否为NDIS通道
 输入参数  : VOS_UINT8 ucIndex
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年5月2日
    作    者   : l60609
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AT_CheckNdisUser(VOS_UINT8 ucIndex)
{
    if (AT_NDIS_USER != gastAtClientTab[ucIndex].UserType)
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;

}


VOS_UINT32 AT_CheckHsUartUser(VOS_UINT8 ucIndex)
{
#if (FEATURE_ON == FEATURE_AT_HSUART)
    if (AT_HSUART_USER == gastAtClientTab[ucIndex].UserType)
    {
        return VOS_TRUE;
    }
#endif

    return VOS_FALSE;
}


VOS_UINT32 AT_CheckModemUser(VOS_UINT8 ucIndex)
{
    if (AT_MODEM_USER != gastAtClientTab[ucIndex].UserType)
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_VOID AT_InitFcMap(VOS_VOID)
{
    VOS_UINT8       ucLoop;

    /* 初始化g_stFcIdMaptoFcPri */
    for (ucLoop = 0; ucLoop < FC_ID_BUTT; ucLoop++)
    {
        g_stFcIdMaptoFcPri[ucLoop].ulUsed  = VOS_FALSE;
        g_stFcIdMaptoFcPri[ucLoop].enFcPri = FC_PRI_BUTT;
        g_stFcIdMaptoFcPri[ucLoop].ulRabIdMask  = 0;
        g_stFcIdMaptoFcPri[ucLoop].enModemId    = MODEM_ID_BUTT;
    }
}
VOS_UINT32 AT_SendDiagCmdFromOm(
    VOS_UINT8                           ucPortNo,
    VOS_UINT8                           ucType,
    VOS_UINT8                          *pData,
    VOS_UINT16                          uslength
)
{
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : AT_SendPcuiDataFromOm
 功能描述  : 提供给OM的PCUI口发送数据接口
 输入参数  : pucVirAddr - 待发送下行数据虚地址
             pucPhyAddr - 待发送下行数据实地址
             ulLength   - 数据长度
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年10月10日
    作    者   : 鲁琳/l60609
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AT_SendPcuiDataFromOm(
    VOS_UINT8                          *pucVirAddr,
    VOS_UINT8                          *pucPhyAddr,
    VOS_UINT32                          ulLength
)
{
    if (AT_SUCCESS != At_SendData(AT_CLIENT_TAB_PCUI_INDEX,
                                  gastAtClientTab[AT_CLIENT_TAB_PCUI_INDEX].DataMode,
                                  pucVirAddr,
                                  (VOS_UINT16)ulLength))
    {
        return VOS_ERR;
    }
    else
    {
        return VOS_OK;
    }
}

/*****************************************************************************
 函 数 名  : AT_SendCtrlDataFromOm
 功能描述  : 提供给OM的CTRL口发送数据接口
 输入参数  : pucVirAddr - 待发送下行数据虚地址
             pucPhyAddr - 待发送下行数据实地址
             ulLength   - 数据长度
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年10月10日
    作    者   : 鲁琳/l60609
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AT_SendCtrlDataFromOm(
    VOS_UINT8                          *pucVirAddr,
    VOS_UINT8                          *pucPhyAddr,
    VOS_UINT32                          ulLength
)
{
    if (AT_SUCCESS != At_SendData(AT_CLIENT_TAB_CTRL_INDEX,
                                  gastAtClientTab[AT_CLIENT_TAB_CTRL_INDEX].DataMode,
                                  pucVirAddr,
                                  (VOS_UINT16)ulLength))
    {
        return VOS_ERR;
    }
    else
    {
        return VOS_OK;
    }
}



CPM_SEND_FUNC AT_QuerySndFunc(AT_PHY_PORT_ENUM_UINT32 ulPhyPort)
{
    switch(ulPhyPort)
    {
        case AT_UART_PORT:
            return AT_UART_SendRawDataFromOm;

        case AT_PCUI_PORT:
            return AT_SendPcuiDataFromOm;

        case AT_CTRL_PORT:
            return AT_SendCtrlDataFromOm;

#if (FEATURE_ON == FEATURE_AT_HSUART)
        case AT_HSUART_PORT:
            return AT_HSUART_SendRawDataFromOm;
#endif

        default:
            AT_WARN_LOG("AT_QuerySndFunc: don't proc data of this port!");
            return VOS_NULL_PTR;
    }
}

/*****************************************************************************
 Prototype      : At_SendCmdMsg
 Description    : AT发送命令字符串
 Input          : ucIndex --- 用户ID
                  pData   --- 数据
                  usLen   --- 长度
                  ucType  --- 消息类型
 Output         : ---
 Return Value   : AT_SUCCESS --- 成功
                  AT_FAILURE --- 失败
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-04-19
    Author      : ---
    Modification: Created function
  2.日    期 : 2007-03-27
    作    者 : h59254
    修改内容 : 问题单号:A32D09820(PC-Lint修改)
*****************************************************************************/
TAF_UINT32 At_SendCmdMsg (TAF_UINT8 ucIndex,TAF_UINT8* pData, TAF_UINT16 usLen,TAF_UINT8 ucType)
{
    AT_MSG_STRU                        *pMsg = TAF_NULL_PTR;
    VOS_UINT_PTR                        ulTmpAddr;
    VOS_UINT32                          ulLength;

    if (VOS_NULL_PTR == pData)
    {
        AT_WARN_LOG("At_SendCmdMsg :pData is null ptr!");
        return AT_FAILURE;
    }

    if (0 == usLen)
    {
        AT_WARN_LOG("At_SendCmdMsg ulLength = 0");
        return AT_FAILURE;
    }

    if (AT_COM_BUFF_LEN < usLen)
    {
        AT_WARN_LOG("At_SendCmdMsg ulLength > AT_COM_BUFF_LEN");
        return AT_FAILURE;
    }

    /* 增加自定义的ITEM，共4个字节 */
    AT_GetAtMsgStruMsgLength(usLen, &ulLength);
    pMsg = (AT_MSG_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, ulLength);
    if ( pMsg == TAF_NULL_PTR )
    {
        AT_ERR_LOG("At_SendCmdMsg:ERROR:Alloc Msg");
        return AT_FAILURE;
    }

    /* 拷贝本地缓存和实体索引到pMsg->aucValue;*/
    pMsg->ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pMsg->ulSenderPid       = WUEPS_PID_AT;
    pMsg->ulReceiverPid     = WUEPS_PID_AT;

    if (AT_COMBIN_BLOCK_MSG == ucType)
    {
        pMsg->enMsgId = ID_AT_COMBIN_BLOCK_CMD;
    }
    else
    {
        pMsg->enMsgId = AT_GetCmdMsgID(ucIndex);
    }

    pMsg->ucType            = ucType;     /* 类型 */
    pMsg->ucIndex           = ucIndex;    /* 索引 */
    pMsg->usLen             = usLen;    /* 长度 */

    PS_MEM_SET(pMsg->aucValue, 0, sizeof(pMsg->aucValue));
    AT_GetUserTypeFromIndex(ucIndex, &pMsg->ucUserType);
    PS_MEM_SET(pMsg->aucReserved, 0x00, sizeof(pMsg->aucReserved));

    /* 填写新消息内容 */
    ulTmpAddr = (VOS_UINT_PTR)(pMsg->aucValue);
    PS_MEM_CPY((VOS_VOID*)ulTmpAddr, pData, usLen);  /* 内容 */

    /*发送消息到AT_PID;*/
    if ( 0 != PS_SEND_MSG( WUEPS_PID_AT, pMsg ) )
    {
        AT_ERR_LOG("At_SendCmdMsg:ERROR:VOS_SendMsg");
        return AT_FAILURE;
    }
    return AT_SUCCESS;
}

/*****************************************************************************
 函 数 名  : AT_IsApPort
 功能描述  : 判断是否为HSIC端口、MUX端口或者VCOM通道，即APP使用的通道
 输入参数  : VOS_UINT8 ucIndex
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月14日
    作    者   : l60609
    修改内容   : DSDA PHASE III
*****************************************************************************/
VOS_UINT32 AT_IsApPort(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulHsicUserFlg;
    VOS_UINT32                          ulMuxUserFlg;
    VOS_UINT32                          ulVcomUserFlg;
    VOS_UINT8                          *pucSystemAppConfig;

    if (0 == g_stAtDebugInfo.ucUnCheckApPortFlg)
    {
        /* 初始化 */
        pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

        ulHsicUserFlg = AT_CheckHsicUser(ucIndex);
        ulMuxUserFlg  = AT_CheckMuxUser(ucIndex);
        ulVcomUserFlg = AT_CheckAppUser(ucIndex);

        if (SYSTEM_APP_ANDROID == *pucSystemAppConfig)
        {
            /* 如果是手机形态，需要判断HSIC端口，MUX端口，VCOM端口 */
            if ((VOS_FALSE == ulHsicUserFlg)
             && (VOS_FALSE == ulMuxUserFlg)
             && (VOS_FALSE == ulVcomUserFlg))
            {
                return VOS_FALSE;
            }
        }
        else
        {
            /* 如果非手机形态，需要判断HSIC端口，MUX端口 */
            if ((VOS_FALSE == ulHsicUserFlg)
             && (VOS_FALSE == ulMuxUserFlg))
            {
                return VOS_FALSE;
            }
        }
    }

    return VOS_TRUE;
}
VOS_INT AT_ProcCCpuResetBefore(VOS_VOID)
{
    AT_MSG_STRU                        *pstMsg = VOS_NULL_PTR;

    /* 设置处于复位前的标志 */
    AT_SetResetFlag(VOS_TRUE);

    /* 清除TAFAGENT所有的信号量 */
    TAF_AGENT_ClearAllSem();

    /* 构造消息 */
    pstMsg = (AT_MSG_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(WUEPS_PID_AT,
                                                         sizeof(AT_MSG_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("AT_ProcCCpuResetBefore: Malloc Msg Failed!\r\n");
        return VOS_ERROR;
    }

    /* 初始化消息 */
    PS_MEM_SET((VOS_CHAR *)pstMsg + VOS_MSG_HEAD_LENGTH,
               0x00,
               (VOS_SIZE_T)(sizeof(AT_MSG_STRU) - VOS_MSG_HEAD_LENGTH));

    /* 填写消息头 */
    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = WUEPS_PID_AT;
    pstMsg->ucType                      = ID_CCPU_AT_RESET_START_IND;

    pstMsg->enMsgId                     = ID_AT_COMM_CCPU_RESET_START;

    /* 发消息 */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_ERR_LOG("AT_ProcCCpuResetBefore: Send Msg Failed!\r\n");
        return VOS_ERROR;
    }

    /* 等待回复信号量初始为锁状态，等待消息处理完后信号量解锁。 */
    if (VOS_OK != VOS_SmP(AT_GetResetSem(), AT_RESET_TIMEOUT_LEN))
    {
        AT_ERR_LOG("AT_ProcCCpuResetBefore: Lock Binary SEM Failed!\r\n");
        AT_DBG_LOCK_BINARY_SEM_FAIL_NUM(1);

        return VOS_ERROR;
    }

    /* 记录复位前的次数 */
    AT_DBG_SAVE_CCPU_RESET_BEFORE_NUM(1);

    return VOS_OK;
}


VOS_INT AT_ProcCCpuResetAfter(VOS_VOID)
{
    AT_MSG_STRU                        *pstMsg = VOS_NULL_PTR;

    /* 构造消息 */
    pstMsg = (AT_MSG_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(WUEPS_PID_AT,
                                                         sizeof(AT_MSG_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("AT_ProcCCpuResetAfter: Malloc Msg Failed!\r\n");
        return VOS_ERROR;
    }

    /* 初始化消息 */
    PS_MEM_SET((VOS_CHAR *)pstMsg + VOS_MSG_HEAD_LENGTH,
               0x00,
               (VOS_SIZE_T)(sizeof(AT_MSG_STRU) - VOS_MSG_HEAD_LENGTH));

    /* 填写消息头 */
    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = WUEPS_PID_AT;
    pstMsg->ucType                      = ID_CCPU_AT_RESET_END_IND;

    pstMsg->enMsgId                     = ID_AT_COMM_CCPU_RESET_END;

    /* 发消息 */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_ERR_LOG("AT_ProcCCpuResetAfter: Send Msg Failed!\r\n");
        return VOS_ERROR;
    }

    /* 记录复位后的次数 */
    AT_DBG_SAVE_CCPU_RESET_AFTER_NUM(1);

    return VOS_OK;
}
VOS_INT AT_CCpuResetCallback(
    DRV_RESET_CB_MOMENT_E               enParam,
    VOS_INT                             iUserData
)
{
    /* 复位前 */
    if (MDRV_RESET_CB_BEFORE == enParam)
    {
        return AT_ProcCCpuResetBefore();
    }
    /* 复位后 */
    else if (MDRV_RESET_CB_AFTER == enParam)
    {
        return AT_ProcCCpuResetAfter();
    }
    else
    {
        return VOS_ERROR;
    }
}


VOS_INT AT_HifiResetCallback(
    DRV_RESET_CB_MOMENT_E               enParam,
    VOS_INT                             iUserData
)
{
    AT_MSG_STRU                        *pstMsg = VOS_NULL_PTR;

    /* 参数为0表示复位前调用 */
    if (MDRV_RESET_CB_BEFORE == enParam)
    {
        /* 构造消息 */
        pstMsg = (AT_MSG_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(WUEPS_PID_AT,
                                                             sizeof(AT_MSG_STRU));
        if (VOS_NULL_PTR == pstMsg)
        {
            AT_ERR_LOG("AT_HifiResetCallback: Malloc Msg Failed!\r\n");
            return VOS_ERROR;
        }

        /* 初始化消息 */
        PS_MEM_SET((VOS_CHAR *)pstMsg + VOS_MSG_HEAD_LENGTH,
                   0x00,
                   (VOS_SIZE_T)(sizeof(AT_MSG_STRU) - VOS_MSG_HEAD_LENGTH));

        /* 填写消息头 */
        pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
        pstMsg->ulReceiverPid               = WUEPS_PID_AT;
        pstMsg->ucType                      = ID_HIFI_AT_RESET_START_IND;

        pstMsg->enMsgId                     = ID_AT_COMM_HIFI_RESET_START;

        /* 发消息 */
        if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
        {
            AT_ERR_LOG("AT_HRESET_Callback: Send Msg Failed!\r\n");
            return VOS_ERROR;
        }

        return VOS_OK;
    }
    /* 复位后 */
    else if (MDRV_RESET_CB_AFTER == enParam)
    {
        /* Added by L47619 for HIFI Reset End Report, 2013/07/08, begin */
        /* 构造消息 */
        pstMsg = (AT_MSG_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(WUEPS_PID_AT,
                                                             sizeof(AT_MSG_STRU));
        if (VOS_NULL_PTR == pstMsg)
        {
            AT_ERR_LOG("AT_HifiResetCallback: Malloc ID_HIFI_AT_RESET_END_IND Msg Failed!\r\n");
            return VOS_ERROR;
        }

        /* 初始化消息 */
        PS_MEM_SET((VOS_CHAR *)pstMsg + VOS_MSG_HEAD_LENGTH,
                   0x00,
                   (VOS_SIZE_T)(sizeof(AT_MSG_STRU) - VOS_MSG_HEAD_LENGTH));

        /* 填写消息头 */
        pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
        pstMsg->ulReceiverPid               = WUEPS_PID_AT;
        pstMsg->ucType                      = ID_HIFI_AT_RESET_END_IND;

        pstMsg->enMsgId                     = ID_AT_COMM_HIFI_RESET_END;

        /* 发消息 */
        if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
        {
            AT_ERR_LOG("AT_HifiResetCallback: Send ID_HIFI_AT_RESET_END_IND Msg Failed!\r\n");
            return VOS_ERROR;
        }
        /* Added by L47619 for HIFI Reset End Report, 2013/07/08, end */
        return VOS_OK;
    }
    else
    {
        return VOS_ERROR;
    }
}


/*****************************************************************************
函 数 名  : AT_ModemeEnableCB
功能描述  : MODEM设备使能通知
输入参数  : ucEnable    ----  是否使能
输出参数  : 无
返 回 值  : 无
调用函数  :
被调函数  :

修改历史      :
  1.日    期   : 2013年05月25日
    作    者   : 范晶/00179208
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID AT_ModemeEnableCB(
    VOS_UINT8                           ucIndex,
    PS_BOOL_ENUM_UINT8                  ucEnable
)
{
    /* 设备默认处于生效状态，有数据就通过读回调接收，
    　 设备失效时，根据当前状态，通知PPP，如处于数传态，
       则通知AT去激活PDP.
    */
    if (PS_FALSE == ucEnable)
    {
        if (AT_PPP_DATA_MODE == gastAtClientTab[ucIndex].DataMode)
        {
            PPP_RcvAtCtrlOperEvent(gastAtClientTab[ucIndex].usPppId,
                                   PPP_AT_CTRL_REL_PPP_REQ);

            /* 若原先开启了流控，则需停止流控 */
            if (0 == (gastAtClientTab[ucIndex].ModemStatus & IO_CTRL_CTS))
            {
                AT_StopFlowCtrl(ucIndex);
            }

            /* 断开拨号 */
            if (VOS_OK != TAF_PS_CallEnd(WUEPS_PID_AT,
                                         gastAtClientTab[ucIndex].usClientId,
                                         0,
                                         gastAtClientTab[ucIndex].ucCid))
            {
                AT_ERR_LOG("AT_ModemeEnableCB: TAF_PS_CallEnd failed in <AT_PPP_DATA_MODE>!");
                return;
            }
        }
        else if (AT_IP_DATA_MODE == gastAtClientTab[ucIndex].DataMode)
        {
            PPP_RcvAtCtrlOperEvent(gastAtClientTab[ucIndex].usPppId,
                                   PPP_AT_CTRL_REL_PPP_RAW_REQ);

            /* 若原先开启了流控，则需停止流控 */
            if (0 == (gastAtClientTab[ucIndex].ModemStatus & IO_CTRL_CTS))
            {
                AT_StopFlowCtrl(ucIndex);
            }

            /* 断开拨号 */
            if ( VOS_OK != TAF_PS_CallEnd(WUEPS_PID_AT,
                                          gastAtClientTab[ucIndex].usClientId,
                                          0,
                                          gastAtClientTab[ucIndex].ucCid) )
            {
                AT_ERR_LOG("AT_ModemeEnableCB: TAF_PS_CallEnd failed in <AT_IP_DATA_MODE>!");
                return;
            }
        }
        else
        {
            /* 空的else分支，避免PCLINT报错 */
        }

        /* 向PPP发送HDLC去使能操作 */
        PPP_RcvAtCtrlOperEvent(gastAtClientTab[ucIndex].usPppId,
                               PPP_AT_CTRL_HDLC_DISABLE);

        /* 停止定时器 */
        AT_StopRelTimer(ucIndex, &gastAtClientTab[ucIndex].hTimer);

        /* 管脚信号修改后，At_ModemRelInd函数只可能在USB被拔出的时候调用，
           为了达到Modem口always-on的目的，此时需要将该AT链路的状态迁入
           到正常的命令状态:
        */
        gastAtClientTab[ucIndex].Mode            = AT_CMD_MODE;
        gastAtClientTab[ucIndex].IndMode         = AT_IND_MODE;
        gastAtClientTab[ucIndex].DataMode        = AT_DATA_BUTT_MODE;
        gastAtClientTab[ucIndex].DataState       = AT_DATA_STOP_STATE;
        gastAtClientTab[ucIndex].CmdCurrentOpt   = AT_CMD_CURRENT_OPT_BUTT;
        g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_READY;
    }
}



#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif


