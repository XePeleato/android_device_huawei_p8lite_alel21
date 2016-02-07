/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : AtRabmInterface.h
  版 本 号   : 初稿
  作    者   : h44270
  生成日期   : 2011年10月19日
  最近修改   :
  功能描述   : AT与RABM之间的接口头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2011年10月19日
    作    者   : h4470
    修改内容   : 创建文件

******************************************************************************/
#ifndef __AT_RABM_INTERFACE_H__
#define __AT_RABM_INTERFACE_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "vos.h"
#include  "TafTypeDef.h"
#include  "PsTypeDef.h"
#include "NasNvInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/


/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*****************************************************************************
 枚举名    : AT_RABM_MSG_ID_ENUM
 枚举说明  : AT与RABM之间的消息接口定义
 1.日    期   : 2011年10月18日
   作    者   : h44270
   修改内容   : 新建
*****************************************************************************/
enum AT_RABM_MSG_ID_ENUM
{
    ID_AT_RABM_SET_FASTDORM_PARA_REQ = 0 ,                                           /* _H2ASN_MsgChoice AT_RABM_SET_FASTDORM_PARA_REQ_STRU */
    ID_AT_RABM_QRY_FASTDORM_PARA_REQ = 1 ,                                           /* _H2ASN_MsgChoice AT_RABM_QRY_FASTDORM_PARA_REQ_STRU */


    ID_RABM_AT_SET_FASTDORM_PARA_CNF = 2 ,                                           /* _H2ASN_MsgChoice RABM_AT_SET_FASTDORM_PARA_CNF_STRU */
    ID_RABM_AT_QRY_FASTDORM_PARA_CNF = 3 ,                                           /* _H2ASN_MsgChoice RABM_AT_QRY_FASTDORM_PARA_CNF_STRU */

    ID_AT_RABM_SET_RELEASE_RRC_REQ   = 4 ,                                           /* _H2ASN_MsgChoice AT_RABM_RELEASE_RRC_REQ_STRU */
    ID_RABM_AT_SET_RELEASE_RRC_CNF   = 5 ,                                           /* _H2ASN_MsgChoice RABM_AT_RELEASE_RRC_CNF_STRU */

    ID_RABM_AT_MSG_ID_ENUM_BUTT

};
typedef VOS_UINT32 AT_RABM_MSG_ID_ENUM_UINT32;

/*****************************************************************************
 枚举名    : AT_RABM_PARA_SET_RSLT_ENUM
 枚举说明  : 参数设置的结果
 1.日    期   : 2011年10月18日
   作    者   : h44270
   修改内容   : 新建
*****************************************************************************/
enum    AT_RABM_PARA_SET_RSLT_ENUM
{
    AT_RABM_PARA_SET_RSLT_SUCC         = 0,                                     /* 参数设置成功 */
    AT_RABM_PARA_SET_RSLT_FAIL,                                                 /* 参数设置失败 */
    AT_RABM_PARA_SET_RSLT_BUTT
};
typedef VOS_UINT32 AT_RABM_PARA_SET_RSLT_ENUM_UINT32;


/*****************************************************************************
 枚举名    : AT_RABM_FASTDORM_OPERATION_ENUM
 枚举说明  : 是否可以启动FAST DORMANCY查询结果
 1.日    期   : 2011年10月18日
   作    者   : h44270
   修改内容   : 新建
*****************************************************************************/
enum AT_RABM_FASTDORM_OPERATION_ENUM
{
    AT_RABM_FASTDORM_STOP_FD_ASCR,                                              /* 停止FD和ASCR */
    AT_RABM_FASTDORM_START_FD_ONLY,                                             /* 仅允许FD */
    AT_RABM_FASTDORM_START_ASCR_ONLY,                                           /* 仅允许ASCR */
    AT_RABM_FASTDORM_START_FD_ASCR,                                             /* 允许FD和ASCR */
    AT_RABM_FASTDORM_START_BUTT
};
typedef VOS_UINT32 AT_RABM_FASTDORM_OPERATION_ENUM_UINT32;


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/

/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
/*****************************************************************************
 结构名    : ATRABM_FASTDORM_SET_PARA_STRU
 结构说明  : FAST DORMANCY特性相关的参数
 1.日    期   : 2011年10月18日
   作    者   : h44270
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    AT_RABM_FASTDORM_OPERATION_ENUM_UINT32      enFastDormOperationType;        /* 操作类型 */
    VOS_UINT32                                  ulTimeLen;                      /* 无流量时长 */
}AT_RABM_FASTDORM_PARA_STRU;



/*****************************************************************************
 结构名    : AT_RABM_SET_FASTDORM_PARA_REQ_STRU
 结构说明  : AT通知RABM 设置FAST DORMANCY相关参数
 1.日    期   : 2011年10月19日
   作    者   : h44270
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /* 消息头 */ /*_H2ASN_Skip*/
    MN_CLIENT_ID_T                      usClientId;                             /* AT发消息给RABM所带的ClientId */
    MN_OPERATION_ID_T                   ucOpId;                                 /* AT发消息给RABM所带的OpId */
    VOS_UINT8                           aucReserve1[1];                         /* 保留 */
    AT_RABM_FASTDORM_PARA_STRU          stFastDormPara;                         /* FAST DORMANCY特性相关的参数 */
}AT_RABM_SET_FASTDORM_PARA_REQ_STRU;


/*****************************************************************************
 结构名    : AT_RABM_QRY_FASTDORM_PARA_REQ_STRU
 结构说明  : AT通知RABM 查询FAST DORMANCY相关设置参数
 1.日    期   : 2011年7月11日
   作    者   : h44270
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /* 消息头 */ /*_H2ASN_Skip*/
    MN_CLIENT_ID_T                      usClientId;                             /* AT发消息给RABM所带的ClientId */
    MN_OPERATION_ID_T                   ucOpId;                                 /* AT发消息给RABM所带的OpId */
    VOS_UINT8                           aucReserve1[1];                         /* 保留 */
}AT_RABM_QRY_FASTDORM_PARA_REQ_STRU;


typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /* 消息头 */ /*_H2ASN_Skip*/
    VOS_UINT16                          usClientId;                             /* AT发消息给RABM所带的ClientId */
    VOS_UINT8                           ucOpId;                                 /* AT发消息给RABM所带的OpId */
    VOS_UINT8                           aucReserve1[1];                         /* 保留 */
}AT_RABM_RELEASE_RRC_REQ_STRU;


typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /* 消息头 */ /*_H2ASN_Skip*/
    VOS_UINT16                          usClientId;                             /* AT发消息给RABM所带的ClientId */
    VOS_UINT8                           ucOpId;                                 /* AT发消息给RABM所带的OpId */
    VOS_UINT8                           aucReserve1[1];                         /* 保留 */
    VOS_UINT32                          ulRslt;                                 /* 设置结果 */
}RABM_AT_RELEASE_RRC_CNF_STRU;

/*****************************************************************************
 结构名    : RABM_AT_SET_FASTDORM_PARA_CNF_STRU
 结构说明  : RABM回复AT设置FAST DORMANCY结果
 1.日    期   : 2011年10月19日
   作    者   : h44270
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /* 消息头 */ /*_H2ASN_Skip*/
    MN_CLIENT_ID_T                      usClientId;                             /* AT发消息给RABM所带的ClientId */
    MN_OPERATION_ID_T                   ucOpId;                                 /* AT发消息给RABM所带的OpId */
    VOS_UINT8                           aucReserve1[1];                         /* 保留 */
    AT_RABM_PARA_SET_RSLT_ENUM_UINT32   enRslt;                                 /* 设置结果 */
}RABM_AT_SET_FASTDORM_PARA_CNF_STRU;


/*****************************************************************************
 结构名    : RABM_AT_QRY_FASTDORM_PARA_CNF_STRU
 结构说明  : AT通知RABM 查询FAST DORMANCY相关设置参数
 1.日    期   : 2011年7月11日
   作    者   : h44270
   修改内容   : 新建

 2.日    期   : 2012年3月20日
    作    者   : l60609
    修改内容   : B070 Project:Fastdormancy增加NV项控制

*****************************************************************************/
typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /* 消息头 */ /*_H2ASN_Skip*/
    MN_CLIENT_ID_T                      usClientId;                             /* AT发消息给RABM所带的ClientId */
    MN_OPERATION_ID_T                   ucOpId;                                 /* AT发消息给RABM所带的OpId */
    /* Added by l60609 for B070 Project, 2012/03/20, begin */
    VOS_UINT8                           ucRslt;                                 /* 结果 */
    /* Added by l60609 for B070 Project, 2012/03/20, end */
    AT_RABM_FASTDORM_PARA_STRU          stFastDormPara;                         /* 查询返回的当前参数 */
}RABM_AT_QRY_FASTDORM_PARA_CNF_STRU;


/*****************************************************************************
  8 UNION定义
*****************************************************************************/
/*****************************************************************************
  H2ASN顶级消息结构定义
*****************************************************************************/
typedef struct
{
    AT_RABM_MSG_ID_ENUM_UINT32          enMsgID;    /*_H2ASN_MsgChoice_Export AT_RABM_MSG_ID_ENUM_UINT32 */
                                                    /* 为了兼容NAS的消息头定义，所以转换ASN.AT_RABM_MSG_ID_ENUM_UINT32 */

    VOS_UINT8                           aucMsgBlock[4];
    /***************************************************************************
        _H2ASN_MsgChoice_When_Comment          AT_RABM_MSG_ID_ENUM_UINT32
    ****************************************************************************/
}AT_RABM_SND_INTERNAL_DATA;
/*_H2ASN_Length UINT32*/

typedef struct
{
    VOS_MSG_HEADER
    AT_RABM_SND_INTERNAL_DATA                    stMsgData;
}AtRabmInterfacemsg_MSG;



/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of NasMmcSuspendProcAct.h */

