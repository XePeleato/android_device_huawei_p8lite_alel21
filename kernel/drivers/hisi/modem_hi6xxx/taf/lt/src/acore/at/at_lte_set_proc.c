/*lint -save -e537 -e958 -e718 -e746*/
#include "osm.h"
#include "gen_msg.h"
#include "at_lte_common.h"
#include "ATCmdProc.h"

/***************************************************************************** 
 函 数 名  : atSetTmodePara
 功能描述  : 发送TMODE模式到TL C核 
 输入参数  : ucClientId Client ID
             ulTmode 模式 输出参数  : 无
 返 回 值  : 成功返回AT_OK，失败返回AT_ERROR*****************************************************************************/VOS_UINT32 atSetTmodePara(VOS_UINT8 ucClientId, VOS_UINT32 ulTmode)
{
    SYM_SET_TMODE_REQ_STRU stTmode ;
    stTmode.enTmodeMode = (SYM_TMODE_ENUM)ulTmode;

    if(AT_SUCCESS == atSendFtmDataMsg(MSP_SYS_FTM_PID, ID_MSG_SYM_SET_TMODE_REQ,ucClientId,
        (VOS_VOID *)(&stTmode), sizeof(SYM_SET_TMODE_REQ_STRU)))
    {
       return AT_OK;
    }
    else
    {
        return AT_ERROR;
    }
}

/***************************************************************************** 
 函 数 名  : atSetTmodeParaCnfProc
 功能描述  : TL C核模式设置返回处理函数 输入参数  : ucClientId Client ID
             pMsgBlock  消息内容             
 输出参数  : 无 
 返 回 值  : 返回AT_FW_CLIENT_STATUS_READY
*****************************************************************************/
VOS_UINT32 atSetTmodeParaCnfProc(VOS_UINT8 ucClientId, VOS_VOID *pMsgBlock)
{
    OS_MSG_STRU* pEvent = NULL;
    SYM_SET_TMODE_CNF_STRU * pTmodeCnf = NULL;

    /* pMsgBlock 在上层调用中已判断是否为空 */
    pEvent = (OS_MSG_STRU*)(((MsgBlock*)pMsgBlock)->aucValue);
    pTmodeCnf = (SYM_SET_TMODE_CNF_STRU *)pEvent->ulParam1;
    
    HAL_SDMLOG("\n atSetTmodeParaCnfProc, ulErrCode=0x%x\n", (VOS_INT)pTmodeCnf->ulErrCode);

    CmdErrProc(ucClientId, pTmodeCnf->ulErrCode, 0, NULL);
    return AT_FW_CLIENT_STATUS_READY;
}
/*lint -restore*/

