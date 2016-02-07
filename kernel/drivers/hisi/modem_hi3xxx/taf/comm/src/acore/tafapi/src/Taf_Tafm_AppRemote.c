/*****************************************************************************
   Copyright (c) ,1988-2001,Huawei Tech. Co.,Ltd
   File name     :Taf_Tafm_AppRemote.c
   Author        :zzy
   Description   :1、TAF-APP远端API
                  2、TAF-MUX远端回调API
   Others        :
   Function List :
   History       :
                 :2005-04-13 初稿
                 :2005-10-07 增加Detach,Attach功能，修改人:刘?
                 :2005-10-27 增加对流量上报的处理   修改人:l47619
                 :2006-03-03 问题单:A32D02338       修改人:l47619
                 :2006-04-12     for A32D02451      韩鲁峰
               6.日    期   : 2006年4月6日
                 作    者   : liuyang id:48197
                 修改内容   : 问题单号:A32D01738
               7 日    期 : 2006-12-07
                 作    者 : 韩鲁峰
                 修改内容 : A32D07824 增加CGEQNEG的处理
               8.日    期   : 2007年01月05日
                 作    者   : s46746
                 修改内容   : A32D07668
               9.日    期   : 2007年01月25日
                 作    者   : s62952
                 修改内容   : A32D08540
              10.日    期   : 2007年05月09日
                 作    者   : 罗建 id:60022475
                 修改内容   : 问题单号:A32D10838
              11.日    期   : 2007年10月17日
                 作    者   : 傅映君 id:62575
                 修改内容   : 问题单号:A32D13062
              12.日    期   : 2008年3月17日
                 作    者   : s46746
                 修改内容   : 问题单号:AT2D02570,NAS B005版本新增需求合入
 ******************************************************************************/
#include "vos.h"
#include "Taf_Tafm_Remote.h"

#include "MnCommApi.h"

#include "TafClientApi.h"

#include "MnClient.h"
#include "TafAppMma.h"


#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767 修改人:罗建 107747;检视人:孙少华65952;原因:Log打印*/
#define    THIS_FILE_ID PS_FILE_ID_TAF_TAFM_APPREMOTE_C
/*lint +e767 修改人:罗建 107747;检视人:sunshaohua*/

/****************************************************/
/*    远端API使用的全局变量全部在定义时进行初始化   */
/****************************************************/
/*记录APP回调函数信息结果*/


/*记录TAF远端是否注册过MUX回调函数*/
VOS_UINT8 g_ucTafRetRegMuxCallbackFlag = TAF_REG_MUX_CALLBACK_NO;

/*****************************************************************************
 函 数 名  : Taf_GetAppMsgLen
 功能描述  :
 输入参数  : VOS_UINT32 ulParaLen
             VOS_UINT32 *pulMsgLen
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年7月8日
    作    者   : luojian id:107747
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32  Taf_GetAppMsgLen(VOS_UINT32 ulParaLen, VOS_UINT32 *pulMsgLen)
{
    VOS_UINT32  ulMsgLen;

    if(VOS_NULL_PTR == pulMsgLen)
    {
        return VOS_ERR;
    }

    if ( ulParaLen <= 4 )
    {
        ulMsgLen = sizeof(MN_APP_REQ_MSG_STRU);
    }
    else
    {
        ulMsgLen = (sizeof(MN_APP_REQ_MSG_STRU) + ulParaLen) -4;
    }

    *pulMsgLen = ulMsgLen;

    return VOS_OK;

}

/**********************************************************/
/*                  通信参数设置查询API                   */
/**********************************************************/

VOS_UINT32  Taf_ParaQuery  (
    VOS_UINT16                     ClientId,
    VOS_UINT8                      OpId,
    TAF_PARA_TYPE                  ParaType,
    VOS_VOID                      *pPara
)
{
    VOS_UINT8                           *pucPara;
    VOS_UINT16                           usMsgName;
    VOS_UINT32                           ulParaLen;
    VOS_UINT32                           ulRst;
    VOS_UINT32                           ulReceiverPid;

    if ((ParaType >= TAF_PH_ACCESS_MODE_PARA)
        && (ParaType < TAF_TELE_PARA_BUTT))
    {
        ulParaLen = sizeof(TAF_PARA_TYPE);
        ulReceiverPid = I0_WUEPS_PID_MMA;
        usMsgName = TAF_MSG_PARA_READ;
    }
    else
    {
        return TAF_FAILURE;
    }

    pucPara = (VOS_UINT8 *)PS_MEM_ALLOC( WUEPS_PID_AT, ulParaLen );

    if (VOS_NULL_PTR == pucPara)
    {
        return TAF_FAILURE;
    }

    PS_MEM_SET( pucPara, 0 , ulParaLen );

    PS_MEM_CPY( pucPara, &ParaType, sizeof(TAF_PARA_TYPE) );

    ulRst = MN_FillAndSndAppReqMsg( ClientId,
                                  OpId,
                                  usMsgName,
                                  pucPara,
                                  ulParaLen,
                                  ulReceiverPid);
    PS_MEM_FREE( WUEPS_PID_AT, pucPara );

    return ulRst;
}

/*****************************************************************************
 函 数 名  : Taf_PrefPlmnHandle
 功能描述  :
 输入参数  : TAF_PH_PrefPLMN_OPERATE_STRU        stPrefPlmnOperate
 输出参数  : NONE
 返 回 值  : TAF_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年8月19日
    作    者   : z40661
    修改内容   : 新增函数，增加 AT+CPOL 命令支持

*****************************************************************************/
TAF_UINT32 Taf_PrefPlmnHandle (
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    TAF_PH_PREF_PLMN_OPERATE_STRU  *pstPrefPlmnOperate
)

{
    /* 检查入口参数合法性 */
    if ( (pstPrefPlmnOperate->enPrefPLMNType > MN_PH_PREF_PLMN_HPLMN)
      || (pstPrefPlmnOperate->enPrefPlmnOperType > MN_PH_PrefPLMN_MODIFY_ONE))
    {
        return TAF_FAILURE;
    }

    return MN_FillAndSndAppReqMsg( ClientId,
                                   OpId,
                                   TAF_MSG_MMA_HANDLE_PREF_PLMN_INFO,
                                   pstPrefPlmnOperate,
                                   sizeof(TAF_PH_PREF_PLMN_OPERATE_STRU),
                                   I0_WUEPS_PID_MMA);


}

#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

