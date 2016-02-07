
#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif


#include "TafTypeDef.h"
#include "si_pb.h"
#include "product_config.h"

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767*/
#define      THIS_FILE_ID     PS_FILE_ID_PBAPI_C
/*lint +e767*/

#if ((OSA_CPU_ACPU == VOS_OSA_CPU) || (defined(DMT)))

/*****************************************************************************
函 数 名  : SI_PB_GetReceiverPid
功能描述  : FDN激活
输入参数  : 无
输出参数  : 无
返 回 值  : SI_UINT32 函数执行结果
调用函数  : 无
被调函数  : 外部接口
History     :
1.日    期  : 2008年10月18日
  作    者  : H59254
  修改内容  : Create
*****************************************************************************/
VOS_UINT32 SI_PB_GetReceiverPid(MN_CLIENT_ID_T  ClientId, VOS_UINT32 *pulReceiverPid)
{
#if ( FEATURE_MULTI_MODEM == FEATURE_ON )
    MODEM_ID_ENUM_UINT16    enModemID;

    /* 调用接口获取Modem ID */
    if(VOS_OK != AT_GetModemIdFromClient(ClientId,&enModemID))
    {
        return VOS_ERR;
    }

    if(MODEM_ID_1 == enModemID)
    {
        *pulReceiverPid = I1_MAPS_PB_PID;
    }
    else
    {
        *pulReceiverPid = I0_MAPS_PB_PID;
    }
#else
    *pulReceiverPid = MAPS_PB_PID;
#endif

    return VOS_OK;
}

/*****************************************************************************
函 数 名  : SI_PB_Read
功能描述  : 读取电话本记录
输入参数  : 无
输出参数  : 无
返 回 值  : SI_UINT32 函数执行结果
调用函数  : 无
被调函数  : 外部接口
History     :
1.日    期  : 2008年10月14日
  作    者  : H59254
  修改内容  : Create
*****************************************************************************/
SI_UINT32 SI_PB_Read(  MN_CLIENT_ID_T           ClientId,
                            MN_OPERATION_ID_T        OpId,
                            SI_PB_STORATE_TYPE       Storage,
                            SI_UINT16                Index1,
                            SI_UINT16                Index2)
{
#if (( FEATURE_MULTI_MODEM == FEATURE_ON )&&(!defined(DMT)))
    return TAF_FAILURE;
#else
    SI_PB_READ_REQ_STRU     *pMsg;

    pMsg = (SI_PB_READ_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PB_READ_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pMsg)
    {
        PB_ERROR_LOG("SI_PB_Read:VOS_AllocMsg Failed");

        return TAF_FAILURE;
    }

#if (VOS_WIN32 == VOS_OS_VER)
    pMsg->ulReceiverPid = MAPS_PB_PID;
#else
    pMsg->ulReceiverPid = ACPU_PID_PB;
#endif  /*(VOS_WIN32 == VOS_OS_VER)*/

    pMsg->ulMsgName     = SI_PB_READ_REQ;
    pMsg->usClient      = ClientId;
    pMsg->ucOpID        = OpId;
    pMsg->usIndex1      = Index1;
    pMsg->usIndex2      = Index2;

    if(SI_PB_STORAGE_UNSPECIFIED == Storage)
    {
        pMsg->ulStorage = gstPBCtrlInfo.enPBCurType;
    }
    else
    {
        pMsg->ulStorage = Storage;
    }

    if(VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pMsg))
    {
        PB_ERROR_LOG("SI_PB_Read:VOS_SendMsg Failed");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
#endif
}
SI_UINT32 SI_PB_SRead(  MN_CLIENT_ID_T           ClientId,
                            MN_OPERATION_ID_T        OpId,
                            SI_PB_STORATE_TYPE       Storage,
                            SI_UINT16                Index1,
                            SI_UINT16                Index2)
{
#if (( FEATURE_MULTI_MODEM == FEATURE_ON )&&(!defined(DMT)))
    return TAF_FAILURE;
#else
    SI_PB_READ_REQ_STRU     *pMsg;

    pMsg = (SI_PB_READ_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PB_READ_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pMsg)
    {
        PB_ERROR_LOG("SI_PB_Read:VOS_AllocMsg Failed");

        return TAF_FAILURE;
    }

#if (VOS_WIN32 == VOS_OS_VER)
    pMsg->ulReceiverPid = MAPS_PB_PID;
#else
    pMsg->ulReceiverPid = ACPU_PID_PB;
#endif  /*(VOS_WIN32 == VOS_OS_VER)*/

    pMsg->ulMsgName     = SI_PB_SREAD_REQ;
    pMsg->usClient      = ClientId;
    pMsg->ucOpID        = OpId;
    pMsg->usIndex1      = Index1;
    pMsg->usIndex2      = Index2;

    if(SI_PB_STORAGE_UNSPECIFIED == Storage)
    {
        pMsg->ulStorage = gstPBCtrlInfo.enPBCurType;
    }
    else
    {
        pMsg->ulStorage = Storage;
    }

    if(VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pMsg))
    {
        PB_ERROR_LOG("SI_PB_Read:VOS_SendMsg Failed");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
#endif
}

/*****************************************************************************
函 数 名  : SI_PB_Query
功能描述  : 电话本号码查询
输入参数  : 无
输出参数  : 无
返 回 值  : SI_UINT32 函数执行结果
调用函数  : 无
被调函数  : 外部接口
History     :
1.日    期  : 2008年10月14日
  作    者  : H59254
  修改内容  : Create
*****************************************************************************/
SI_UINT32 SI_PB_Query(     MN_CLIENT_ID_T           ClientId,
                                MN_OPERATION_ID_T        OpId)
{
#if (( FEATURE_MULTI_MODEM == FEATURE_ON )&&(!defined(DMT)))
    return TAF_FAILURE;
#else
    SI_PB_QUERY_REQ_STRU    *pMsg;

    pMsg = (SI_PB_QUERY_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PB_QUERY_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pMsg)
    {
        PB_ERROR_LOG("SI_PB_Query:VOS_AllocMsg Failed");

        return TAF_FAILURE;
    }

    pMsg->ulReceiverPid = MAPS_PB_PID;
    pMsg->ulMsgName     = SI_PB_QUERY_REQ;
    pMsg->usClient      = ClientId;
    pMsg->ucOpID        = OpId;
    pMsg->ulStorage     = gstPBCtrlInfo.enPBCurType;

    if(VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pMsg))
    {
        PB_ERROR_LOG("SI_PB_Query:VOS_SendMsg Failed");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
#endif
}

/*****************************************************************************
函 数 名  : SI_PB_Set
功能描述  : 设置当前号码本使用的存储器类型
输入参数  : 无
输出参数  : 无
返 回 值  : SI_UINT32 函数执行结果
调用函数  : 无
被调函数  : 外部接口
History     :
1.日    期  : 2008年10月14日
  作    者  : H59254
  修改内容  : Create
*****************************************************************************/
SI_UINT32 SI_PB_Set(    MN_CLIENT_ID_T           ClientId,
                            MN_OPERATION_ID_T        OpId,
                            SI_PB_STORATE_TYPE      Storage)
{
    SI_PB_SET_REQ_STRU  *pMsg;
    VOS_UINT32          ulReceiverPid;

#if (( FEATURE_MULTI_MODEM == FEATURE_ON )&&(!defined(DMT)))
    if(SI_PB_STORAGE_FD != Storage)
    {
        PB_ERROR_LOG("SI_PB_Set:Double Modem only support the FDN");

        return TAF_FAILURE;
    }
#endif

    if (VOS_OK != SI_PB_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PB_ERROR_LOG("SI_PB_Set:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    pMsg = (SI_PB_SET_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PB_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pMsg)
    {
        PB_ERROR_LOG("SI_PB_Set:VOS_AllocMsg Failed");

        return TAF_FAILURE;
    }

    pMsg->ulReceiverPid = ulReceiverPid;
    pMsg->ulMsgName     = SI_PB_SET_REQ;
    pMsg->usClient      = ClientId;
    pMsg->ucOpID        = OpId;
    pMsg->ulStorage     = Storage;

    if(VOS_OK != VOS_SendMsg(WUEPS_PID_AT, pMsg))
    {
        PB_ERROR_LOG("SI_PB_Set:VOS_SendMsg Failed");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

/*****************************************************************************
函 数 名  : SI_PB_Add
功能描述  : 电话本中追加一条记录
输入参数  : 无
输出参数  : 无
返 回 值  : SI_UINT32 函数执行结果
调用函数  : 无
被调函数  : 外部接口
History     :
1.日    期  : 2008年10月14日
  作    者  : H59254
  修改内容  : Create
*****************************************************************************/
SI_UINT32 SI_PB_Add(    MN_CLIENT_ID_T          ClientId,
                            MN_OPERATION_ID_T        OpId,
                             SI_PB_STORATE_TYPE      Storage,
                             SI_PB_RECORD_STRU       *pRecord)
{
    SI_PB_ADD_REP_STRU  *pMsg;
    VOS_UINT32          ulReceiverPid;

    if (VOS_OK != SI_PB_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PB_ERROR_LOG("SI_PB_Add:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    if(VOS_NULL_PTR == pRecord)
    {
        PB_ERROR_LOG("SI_PB_Add:pRecord is a NULL pointer");

        return TAF_FAILURE;
    }

    /*lint -e433  修改人: j00174725; 检视人: xucheng */
    pMsg = (SI_PB_ADD_REP_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PB_ADD_REP_STRU) - VOS_MSG_HEAD_LENGTH);
    /*lint +e433  修改人: j00174725; 检视人: xucheng */

    if (VOS_NULL_PTR == pMsg)
    {
        PB_ERROR_LOG("SI_PB_Add:VOS_AllocMsg Failed");

        return TAF_FAILURE;
    }

    pMsg->ulReceiverPid = ulReceiverPid;
    pMsg->ulMsgName     = SI_PB_ADD_REQ;
    pMsg->usClient      = ClientId;
    pMsg->ucOpID        = OpId;

#if( FEATURE_MULTI_MODEM == FEATURE_ON )
    pMsg->ulStorage = SI_PB_STORAGE_FD;     /*只能够操作FDN号码*/
#else
    if(SI_PB_STORAGE_UNSPECIFIED == Storage)
    {
        pMsg->ulStorage = gstPBCtrlInfo.enPBCurType;
    }
    else
    {
        pMsg->ulStorage = Storage;
    }
#endif

    pRecord->Index = 1;

    VOS_MemCpy(&pMsg->stRecord, pRecord, sizeof(SI_PB_RECORD_STRU));

    if(VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pMsg))
    {
        PB_ERROR_LOG("SI_PB_Add:VOS_SendMsg Failed");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}
SI_UINT32 SI_PB_SAdd(    MN_CLIENT_ID_T          ClientId,
                            MN_OPERATION_ID_T        OpId,
                             SI_PB_STORATE_TYPE      Storage,
                             SI_PB_RECORD_STRU       *pRecord)
{
#if (( FEATURE_MULTI_MODEM == FEATURE_ON )&&(!defined(DMT)))
    return TAF_FAILURE;
#else
    SI_PB_ADD_REP_STRU  *pMsg;

    if(VOS_NULL_PTR == pRecord)
    {
        PB_ERROR_LOG("SI_PB_Add:pRecord is a NULL pointer");

        return TAF_FAILURE;
    }

    /*lint -e433  修改人: j00174725; 检视人: xucheng */
    pMsg = (SI_PB_ADD_REP_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PB_ADD_REP_STRU) - VOS_MSG_HEAD_LENGTH);
    /*lint +e433  修改人: j00174725; 检视人: xucheng */

    if (VOS_NULL_PTR == pMsg)
    {
        PB_ERROR_LOG("SI_PB_Add:VOS_AllocMsg Failed");

        return TAF_FAILURE;
    }

    pMsg->ulReceiverPid = MAPS_PB_PID;
    pMsg->ulMsgName     = SI_PB_SADD_REQ;
    pMsg->usClient      = ClientId;
    pMsg->ucOpID        = OpId;

    if(SI_PB_STORAGE_UNSPECIFIED == Storage)
    {
        pMsg->ulStorage = gstPBCtrlInfo.enPBCurType;
    }
    else
    {
        pMsg->ulStorage = Storage;
    }

    pRecord->Index = 1;

    VOS_MemCpy(&pMsg->stRecord, pRecord, sizeof(SI_PB_RECORD_STRU));

    if(VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pMsg))
    {
        PB_ERROR_LOG("SI_PB_Add:VOS_SendMsg Failed");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
#endif
}

/*****************************************************************************
函 数 名  : SI_PB_Modify
功能描述  : 电话本中追加一条记录
输入参数  : 无
输出参数  : 无
返 回 值  : SI_UINT32 函数执行结果
调用函数  : 无
被调函数  : 外部接口
History     :
1.日    期  : 2008年10月14日
  作    者  : H59254
  修改内容  : Create
*****************************************************************************/
SI_UINT32 SI_PB_Modify(    MN_CLIENT_ID_T          ClientId,
                                MN_OPERATION_ID_T       OpId,
                                SI_PB_STORATE_TYPE      Storage,
                                SI_PB_RECORD_STRU       *pRecord )
{
    SI_PB_MODIFY_REP_STRU  *pMsg;
    VOS_UINT32              ulReceiverPid;

    if (VOS_OK != SI_PB_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PB_ERROR_LOG("SI_PB_Modify:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    if(VOS_NULL_PTR == pRecord)
    {
        PB_ERROR_LOG("SI_PB_Modify:pRecord is a NULL pointer");

        return TAF_FAILURE;
    }

    /*lint -e433  修改人: j00174725; 检视人: xucheng */
    pMsg = (SI_PB_MODIFY_REP_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PB_MODIFY_REP_STRU) - VOS_MSG_HEAD_LENGTH);
    /*lint +e433  修改人: j00174725; 检视人: xucheng */

    if (VOS_NULL_PTR == pMsg)
    {
        PB_ERROR_LOG("SI_PB_Modify:VOS_AllocMsg Failed");

        return TAF_FAILURE;
    }

    pMsg->ulReceiverPid = ulReceiverPid;
    pMsg->ulMsgName     = SI_PB_MODIFY_REQ;
    pMsg->usClient      = ClientId;
    pMsg->ucOpID        = OpId;

#if( FEATURE_MULTI_MODEM == FEATURE_ON )
    pMsg->ulStorage = SI_PB_STORAGE_FD;     /*只能够操作FDN号码*/
#else
    if(SI_PB_STORAGE_UNSPECIFIED == Storage)
    {
        pMsg->ulStorage = gstPBCtrlInfo.enPBCurType;
    }
    else
    {
        pMsg->ulStorage = Storage;
    }
#endif

    VOS_MemCpy(&pMsg->Record, pRecord, sizeof(SI_PB_RECORD_STRU));

    if(VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pMsg))
    {
        PB_ERROR_LOG("SI_PB_Modify:VOS_SendMsg Failed");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}
SI_UINT32 SI_PB_SModify(    MN_CLIENT_ID_T          ClientId,
                                MN_OPERATION_ID_T       OpId,
                                SI_PB_STORATE_TYPE      Storage,
                                SI_PB_RECORD_STRU       *pRecord )
{
#if (( FEATURE_MULTI_MODEM == FEATURE_ON )&&(!defined(DMT)))
    return TAF_FAILURE;
#else
    SI_PB_MODIFY_REP_STRU  *pMsg;

    if(VOS_NULL_PTR == pRecord)
    {
        PB_ERROR_LOG("SI_PB_Modify:pRecord is a NULL pointer");

        return TAF_FAILURE;
    }

    /*lint -e433  修改人: j00174725; 检视人: xucheng */
    pMsg = (SI_PB_MODIFY_REP_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PB_MODIFY_REP_STRU) - VOS_MSG_HEAD_LENGTH);
    /*lint +e433  修改人: j00174725; 检视人: xucheng */

    if (VOS_NULL_PTR == pMsg)
    {
        PB_ERROR_LOG("SI_PB_Modify:VOS_AllocMsg Failed");

        return TAF_FAILURE;
    }

    pMsg->ulReceiverPid = MAPS_PB_PID;
    pMsg->ulMsgName     = SI_PB_SMODIFY_REQ;
    pMsg->usClient      = ClientId;
    pMsg->ucOpID        = OpId;

    if(SI_PB_STORAGE_UNSPECIFIED == Storage)
    {
        pMsg->ulStorage = gstPBCtrlInfo.enPBCurType;
    }
    else
    {
        pMsg->ulStorage = Storage;
    }

    VOS_MemCpy(&pMsg->Record, pRecord, sizeof(SI_PB_RECORD_STRU));

    if(VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pMsg))
    {
        PB_ERROR_LOG("SI_PB_Modify:VOS_SendMsg Failed");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
#endif
}

/*****************************************************************************
函 数 名  : SI_PB_Delete
功能描述  : 电话本中删除一条记录
输入参数  : 无
输出参数  : 无
返 回 值  : SI_UINT32 函数执行结果
调用函数  : 无
被调函数  : 外部接口
History     :
1.日    期  : 2008年10月14日
  作    者  : H59254
  修改内容  : Create
*****************************************************************************/
SI_UINT32 SI_PB_Delete(     MN_CLIENT_ID_T             ClientId,
                                MN_OPERATION_ID_T           OpId,
                                SI_PB_STORATE_TYPE          Storage,
                                SI_UINT16                   Index)
{
    SI_PB_DELETE_REQ_STRU  *pMsg;
    VOS_UINT32              ulReceiverPid;

    if (VOS_OK != SI_PB_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PB_ERROR_LOG("SI_PB_Modify:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    pMsg = (SI_PB_DELETE_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PB_DELETE_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pMsg)
    {
        PB_ERROR_LOG("SI_PB_Delete:VOS_AllocMsg Failed");

        return TAF_FAILURE;
    }

    pMsg->ulReceiverPid = ulReceiverPid;
    pMsg->ulMsgName     = SI_PB_DELETE_REQ;
    pMsg->usClient      = ClientId;
    pMsg->ucOpID        = OpId;
    pMsg->usIndex       = Index;

#if( FEATURE_MULTI_MODEM == FEATURE_ON )
    pMsg->ulStorage = SI_PB_STORAGE_FD;     /*只能够操作FDN号码*/
#else
    if(SI_PB_STORAGE_UNSPECIFIED == Storage)
    {
        pMsg->ulStorage = gstPBCtrlInfo.enPBCurType;
    }
    else
    {
        pMsg->ulStorage = Storage;
    }
#endif

    if(VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pMsg))
    {
        PB_ERROR_LOG("SI_PB_Delete:VOS_SendMsg Failed");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

/*****************************************************************************
函 数 名  : SI_PB_Search
功能描述  : 电话本中查找一个记录
输入参数  : 无
输出参数  : 无
返 回 值  : SI_UINT32 函数执行结果
调用函数  : 无
被调函数  : 外部接口
History     :
1.日    期  : 2009年3月12日
  作    者  : H59254
  修改内容  : Create
*****************************************************************************/
SI_UINT32 SI_PB_Search(    MN_CLIENT_ID_T             ClientId,
                                MN_OPERATION_ID_T           OpId,
                                SI_PB_STORATE_TYPE          Storage,
                                SI_UINT8                    ucLength,
                                SI_UINT8                    *pucContent)
{
#if (( FEATURE_MULTI_MODEM == FEATURE_ON )&&(!defined(DMT)))
    return TAF_FAILURE;
#else
    SI_PB_SEARCH_REQ_STRU  *pMsg;

    /*lint -e433  修改人: j00174725; 检视人: xucheng */
    pMsg = (SI_PB_SEARCH_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PB_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    /*lint +e433  修改人: j00174725; 检视人: xucheng */

    if (VOS_NULL_PTR == pMsg)
    {
        PB_ERROR_LOG("SI_PB_Search:VOS_AllocMsg Failed");
        return TAF_FAILURE;
    }

#if (VOS_WIN32 == VOS_OS_VER)
    pMsg->ulReceiverPid = MAPS_PB_PID;
#else
    pMsg->ulReceiverPid = ACPU_PID_PB;
#endif  /*(VOS_WIN32 == VOS_OS_VER)*/

    pMsg->ulMsgName     = SI_PB_SEARCH_REQ;
    pMsg->usClient      = ClientId;
    pMsg->ucOpID        = OpId;
    pMsg->ucLength      = ucLength;

    pMsg->ulStorage = ((Storage == SI_PB_STORAGE_UNSPECIFIED)?(gstPBCtrlInfo.enPBCurType):(Storage));

    VOS_MemCpy(pMsg->aucContent, pucContent, ucLength);

    if(VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pMsg))
    {
        PB_ERROR_LOG("SI_PB_Search:VOS_SendMsg Failed");
        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
#endif
}
VOS_UINT32 SI_PB_GetStorateType(VOS_VOID)
{
#if (( FEATURE_MULTI_MODEM == FEATURE_ON )&&(!defined(DMT)))
    return SI_PB_STORAGE_UNSPECIFIED;   /*返回当前未指定*/
#else
    return (VOS_UINT32)gstPBCtrlInfo.enPBCurType;
#endif
}
VOS_UINT32 SI_PB_GetSPBFlag(VOS_VOID)
{
#if (( FEATURE_MULTI_MODEM == FEATURE_ON )&&(!defined(DMT)))
    return VOS_FALSE;   /*返回状态关闭*/
#else
    return (VOS_UINT32)gstPBConfigInfo.ucSPBFlag;
#endif
}


#endif

#if ((OSA_CPU_CCPU == VOS_OSA_CPU)||(defined(DMT)))
VOS_UINT32 SI_PB_GetEccNumber(SI_PB_ECC_DATA_STRU *pstEccData)
{
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucPBOffset=0;
    VOS_UINT8                          *ptemp;
    VOS_UINT32                          ulNum;
    VOS_UINT8                           i,j;
    VOS_UINT8                           ucCardStatus;
    VOS_UINT8                           ucCardType;

    ulResult = SI_PB_LocateRecord(PB_ECC, 1, 1, &ucPBOffset);

    if(VOS_OK != ulResult)     /*当前电话本不存在或者初始化未完成*/
    {
        PB_ERROR_LOG("SI_PB_GetEccNumber Error: SI_PB_LocateRecord Return Failed");

        pstEccData->bEccExists = SI_PB_CONTENT_INVALID;
        pstEccData->ulReocrdNum= VOS_NULL;

        return VOS_ERR;
    }

    pstEccData->bEccExists = SI_PB_CONTENT_VALID;

    ulNum = ((gastPBContent[ucPBOffset].usTotalNum>USIM_MAX_ECC_RECORDS)?USIM_MAX_ECC_RECORDS:gastPBContent[ucPBOffset].usTotalNum);

    ptemp = gastPBContent[ucPBOffset].pContent;

    for(i=0,j=0; i<ulNum; i++)   /*根据数据结构最大长度循环*/
    {
        ulResult = SI_PB_CheckEccValidity(ptemp);

        if(VOS_ERR == ulResult)     /*当前记录内容无效*/
        {
            PB_INFO_LOG("SI_PB_GetEccNumber Info: The Ecc Number is Empty");
        }
        else                                /*转换当前记录内容*/
        {
            PB_INFO_LOG("SI_PB_GetEccNumber Info: The Ecc Number is Not Empty");

            USIMM_GetCardType(&ucCardStatus, &ucCardType);

            VOS_MemCpy(pstEccData->astEccRecord[j].aucEccCode,
                                    ptemp, gastPBContent[ucPBOffset].ucNumberLen);

            if (USIMM_CARD_USIM == ucCardType)
            {
                pstEccData->astEccRecord[j].bESC = SI_PB_CONTENT_VALID;

                VOS_MemCpy(pstEccData->astEccRecord[j].AlphaIdentifier,
                           &ptemp[gastPBContent[ucPBOffset].ucNumberLen],
                           gastPBContent[ucPBOffset].ucNameLen);

                pstEccData->astEccRecord[j].ucESC = ptemp[gastPBContent[ucPBOffset].ucRecordLen - 1];
            }

            j++;
        }

        ptemp += gastPBContent[ucPBOffset].ucRecordLen;
    }

    pstEccData->ulReocrdNum = j;

    return VOS_OK;
}

/* Added by f62575 for C50_IPC Project, 2012/02/23, begin */

VOS_UINT32 SI_PB_FdnNumCheck(VOS_UINT32 ulPid, VOS_UINT32 ulContextIndex,VOS_UINT32 ulSendPara, PS_PB_FDN_NUM_STRU *pstFdnInfo)
{
    SI_PB_FDN_CHECK_REQ_STRU            *pstFDNCheckReq;

    /*参数检查*/
    if (VOS_NULL_PTR == pstFdnInfo)
    {
        PB_ERROR_LOG("SI_PB_FdnNumCheck: fdn num is null.\r\n");
        return VOS_ERR;
    }

    pstFDNCheckReq = (SI_PB_FDN_CHECK_REQ_STRU*)VOS_AllocMsg(ulPid, sizeof(SI_PB_FDN_CHECK_REQ_STRU)-VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstFDNCheckReq)
    {
        PB_ERROR_LOG("SI_PB_FdnNumCheck: alloc msg failed.\r\n");
        return VOS_ERR;
    }

    /*拷贝号码*/
    VOS_MemCpy(&pstFDNCheckReq->stFDNNum, pstFdnInfo, sizeof(PS_PB_FDN_NUM_STRU));


    pstFDNCheckReq->ulReceiverPid       = MAPS_PB_PID;
    pstFDNCheckReq->ulMsgName           = SI_PB_FDN_CHECK_REQ;
    pstFDNCheckReq->ulSendPara          = ulSendPara;
    pstFDNCheckReq->ulContextIndex      = ulContextIndex;

    (VOS_VOID)VOS_SendMsg(ulPid, pstFDNCheckReq);

    return VOS_OK;
}

/*****************************************************************************
函 数 名  :SI_PB_EcallNumberQry
功能描述  :提供给NAS获取ECALL(FDN/SDN)号码检查的API
输入参数  :  ulPid：发送FDN号码检查请求的Pid
             Storage:存储器类型
             ucListLen:NAS要读取的号码列表
             pucList:列表内容
输出参数  :无
返 回 值  : 0:有效、1:无效
修订记录  :
1. 日    期   : 2014年3月24日
   作    者   : h59254
   修改内容   : Creat
*****************************************************************************/
/*lint -save -e958 */
VOS_UINT32 SI_PB_EcallNumberQry(VOS_UINT32 ulPid, SI_PB_STORATE_TYPE  Storage, VOS_UINT8 ucListLen, VOS_UINT8 *pucList)
{
    SI_PB_ECALL_QRY_REQ_STRU           *pstEcallQryReqMsg;

    if (VOS_NULL_PTR == pucList)
    {
        PB_ERROR_LOG("SI_PB_EcallNumberQry: pucList is NULL");

        return VOS_ERR;
    }

    if ((SI_PB_STORAGE_FD != Storage) && (SI_PB_STORAGE_SD != Storage))
    {
        PB_ERROR_LOG("SI_PB_EcallNumberQry: Storage Type wrong");

        return VOS_ERR;
    }
/*lint -save -e961 */
    pstEcallQryReqMsg = (SI_PB_ECALL_QRY_REQ_STRU *)VOS_AllocMsg(ulPid, sizeof(SI_PB_ECALL_QRY_REQ_STRU)-VOS_MSG_HEAD_LENGTH + ucListLen);
/*lint -restore */
    if (VOS_NULL_PTR == pstEcallQryReqMsg)
    {
        PB_ERROR_LOG("SI_PB_EcallNumberQry: alloc msg failed");

        return VOS_ERR;
    }

    pstEcallQryReqMsg->ulReceiverPid    = MAPS_PB_PID;
    pstEcallQryReqMsg->ulMsgName        = SI_PB_ECALL_QRY_REQ;
    pstEcallQryReqMsg->ulStorage        = Storage;
    pstEcallQryReqMsg->ucListLen        = ucListLen;

    VOS_MemCpy(pstEcallQryReqMsg->aucList, pucList, ucListLen);

    (VOS_VOID)VOS_SendMsg(ulPid, pstEcallQryReqMsg);

    return VOS_OK;
}
/*lint -restore */
#endif

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif



