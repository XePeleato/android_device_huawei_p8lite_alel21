

#ifndef __BASTETRRCINTERFACE_H__
#define __BASTETRRCINTERFACE_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "PsTypeDef.h"


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

enum BASTET_LRRC_MSG_TYPE_ENUM
{
    /* 消息名称 */                              /* 消息ID */                    /* 备注 */
    ID_BASTET_LRRC_CON_TO_IDLE_NTF                    = 0x0001,                    /* _H2ASN_MsgChoice BASTET_RRC_LTE_TO_IDLE_REQ_STRU */
    ID_LRRC_BASTET_RRC_STATUS_IND ,                                                /* _H2ASN_MsgChoice BASTET_RRC_LRRC_STATUS_IND_STRU */

    ID_BASTET_LRRC_MSG_TYPE_BUTT
};
typedef VOS_UINT32 BASTET_LRRC_MSG_TYPE_ENUM_UINT32;



enum BASTET_LRRC_STATE_ENUM
{
    BASTET_LRRC_STATE_IDLE          = 0,
    BASTET_LRRC_STATE_CONNECTED,
    BASTET_LRRC_STATE_BUTT
};
typedef VOS_UINT8 BASTET_LRRC_STATE_ENUM_UINT8;



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


typedef struct
{
    MSG_HEADER_STRU                             stMsgHeader;                    /*_H2ASN_Skip*/
}BASTET_RRC_LTE_TO_IDLE_REQ_STRU;
typedef struct
{
    MSG_HEADER_STRU                             stMsgHeader;                    /*_H2ASN_Skip*/
    BASTET_LRRC_STATE_ENUM_UINT8                enRrcState;
    VOS_UINT8                                   enReserve[3];
}BASTET_RRC_LRRC_STATUS_IND_STRU;



/*****************************************************************************
  8 UNION定义
*****************************************************************************/

/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  H2ASN顶级消息结构定义
*****************************************************************************/
typedef struct
{
    BASTET_LRRC_MSG_TYPE_ENUM_UINT32          enMsgID;    /*_H2ASN_MsgChoice_Export BASTET_LRRC_MSG_TYPE_ENUM_UINT32*/

    VOS_UINT8                                 aucMsgBlock[4];
    /***************************************************************************
        _H2ASN_MsgChoice_When_Comment          BASTET_LRRC_MSG_TYPE_ENUM_UINT32
    ****************************************************************************/
}BASTET_RRC_MSG_DATA;
/*_H2ASN_Length UINT32*/

typedef struct
{
    VOS_MSG_HEADER
    BASTET_RRC_MSG_DATA                       stMsgData;
}BastetRrcInterface_MSG;



/*****************************************************************************
  10 函数声明
*****************************************************************************/


#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif 

