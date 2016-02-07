/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

******************************************************************************
  文 件 名   : TafClientApi.h
  版 本 号   : V100R002
  作    者   : D49431
  生成日期   : 2007-01-09
  最近修改   :
  功能描述   : This file contains APIs for client registering and deregistering.
  函数列表   :
  修改历史   :
  1.日    期   : 2006年8月25日
    作    者   : D49431
    修改内容   : 创建文件
  2.日    期   : 2008年6月10日
    作    者   : luojian id:00107747
    修改内容   : 问题单号:AT2D03704,增加MN_CALLBACK_SAT_DATA事件回调类型
  3.日    期   : 2008年10月20日
    作    者   : h44270
    修改内容   : 问题单号:A32D14153,来电时，用户尚未接听电话就打开了声码器,，AMR速率变换没有上报
******************************************************************************/
#ifndef  TAF_CLIENT_API_H
#define  TAF_CLIENT_API_H


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "TafTypeDef.h"
#include "vos.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 常量定义
*****************************************************************************/
typedef VOS_UINT8 MN_CALLBACK_TYPE_T;   /* Type of callback functions for event reporting*/
#define MN_CALLBACK_CS_CALL             0 /* CS call */
#define MN_CALLBACK_PS_CALL             1 /* PS call */
#define MN_CALLBACK_MSG                 2 /* Short message service */
#define MN_CALLBACK_SS                  3 /* Call independent supplemental service */
#define MN_CALLBACK_PHONE               4 /* Phone management and network register service */
#define MN_CALLBACK_DATA_IND            5 /* User plane data indication */
#define MN_CALLBACK_DATA_STATUS         6 /* User plane data status */
#define MN_CALLBACK_DATA_FLOW           7 /* User plane data flow */
#define MN_CALLBACK_SET                 8
#define MN_CALLBACK_QRY                 9
#define MN_CALLBACK_PHONE_BOOK          10 /* Phone book */
#define MN_CALLBACK_STK                 11
#define MN_CALLBACK_CMD_CNF             12 /* Cmd Cnf*/
#define MN_CALLBACK_CHANNEL_STATUS      13
#define MN_CALLBACK_PIH                 14
#define MN_CALLBACK_VOICE_CONTROL       15
#define MN_CALLBACK_LOG_PRINT           16
#define MN_CALLBACK_MAX                 17



#define  TAF_MAX_CLIENT_OF_ONE_PROC       OMA_CLIENT_ID_BUTT

#define TAF_FREE                        0   /*未使用*/
#define TAF_USED                        1   /*使用*/



#define TAF_CALL_APP_MSG_BASE           (0x0000)                                /* AT与CALL模块间消息起始 */
#define TAF_MSG_APP_MSG_BASE            (0x1000)                                /* AT与MSG模块间消息起始 */
#define TAF_SSA_APP_MSG_BASE            (0x2000)                                /* AT与SSA模块间消息起始 */
#define TAF_MMA_APP_MSG_BASE            (0x3000)                                /* AT与MMA模块间消息起始 */
#define TAF_APP_SET_MSG_BASE            (0x5000)                                /* AT与MN模块间设置消息起始 */
#define TAF_APP_QRY_MSG_BASE            (0x6000)                                /* AT与MN模块间查询消息起始 */
#define TAF_APS_MSG_BASE                (0x7000)                                /* AT/IMSA与PS适配模块间消息起始 */
#define TAF_SPM_IMSA_MSG_BASE           (0x8000)                                /* IMSA与SPM模块间消息起始 */
#define TAF_MSG_IMSA_MSG_BASE           (0x9000)                                /* IMSA与MSG模块间消息起始 */
#define TAF_MMA_IMSA_MSG_BASE           (0xa000)                                /* IMSA与MMA模块间消息起始 */
#define TAF_CALL_IMSA_MSG_BASE          (0xb000)                                /* CALL与IMSA公共消息起始 */
#define TAF_IMSA_COMM_MSG_BASE          (0xc000)                                /* TAF与IMSA公共消息起始 */

#define TAF_IMSA_MSG_MASK               (0xf000)                                /* IMSA与TAF间的MASK */

/*****************************************************************************
  3 类型定义
*****************************************************************************/





/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* TAF_CLIENT_API_H */


