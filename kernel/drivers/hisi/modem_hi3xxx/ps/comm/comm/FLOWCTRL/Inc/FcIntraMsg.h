/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : FcIntraMsg.h
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2011年12月5日
  最近修改   :
  功能描述   : 定义流控模块内部使用的消息结构
  函数列表   :
  修改历史   :
******************************************************************************/

#ifndef __FCINTRAMSG_H__
#define __FCINTRAMSG_H__


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "PsTypeDef.h"
#include "FcInterface.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/*****************************************************************************
  2 枚举定义
*****************************************************************************/
enum FC_MSG_TYPE_ENUM
{
    ID_FC_CPU_A_OVERLOAD_IND            = 0x0001,           /* _H2ASN_MsgChoice FC_CPULOAD_IND_STRU */
    ID_FC_CPU_A_UNDERLOAD_IND           = 0x0002,           /* _H2ASN_MsgChoice FC_CPULOAD_IND_STRU */
    ID_FC_TEMPERATURE_OVERLOAD_IND      = 0x0003,           /* _H2ASN_MsgChoice FC_TEMPERATURE_IND_STRU */
    ID_FC_TEMPERATURE_UNDERLOAD_IND     = 0x0004,           /* _H2ASN_MsgChoice FC_TEMPERATURE_IND_STRU */
    ID_FC_TEMPERATURE_RECOVER_IND       = 0x0005,           /* _H2ASN_MsgChoice FC_TEMPERATURE_IND_STRU */
    ID_FC_SET_GPRS_FLOWCTRL_IND         = 0x0006,           /* _H2ASN_MsgChoice FC_SET_GPRS_FLOWCTRL_IND_STRU */
    ID_FC_STOP_GPRS_FLOWCTRL_IND        = 0x0007,           /* _H2ASN_MsgChoice FC_STOP_GPRS_FLOWCTRL_IND_STRU */
    ID_FC_SET_POINT_FLOWCTRL_IND        = 0x0008,           /* _H2ASN_MsgChoice FC_SET_POINT_FLOWCTRL_IND_STRU */
    ID_FC_STOP_POINT_FLOWCTRL_IND       = 0x0009,           /* _H2ASN_MsgChoice FC_STOP_POINT_FLOWCTRL_IND_STRU */
    ID_FC_REG_POINT_IND                 = 0x000A,           /* _H2ASN_MsgChoice FC_REG_POINT_IND_STRU */
    ID_FC_DEREG_POINT_IND               = 0x000B,           /* _H2ASN_MsgChoice FC_DEREG_POINT_IND_STRU */
    ID_FC_CHANGE_POINT_IND              = 0x000C,           /* _H2ASN_MsgChoice FC_CHANGE_POINT_IND_STRU */
    ID_FC_ADD_RAB_FCID_MAP_IND          = 0x000D,           /* _H2ASN_MsgChoice FC_ADD_RAB_FCID_MAP_IND_STRU */
    ID_FC_DEL_RAB_FCID_MAP_IND          = 0x000E,           /* _H2ASN_MsgChoice FC_DEL_RAB_FCID_MAP_IND_STRU */
    ID_FC_MEM_UP_TO_TARGET_PRI_IND      = 0x000F,           /* _H2ASN_MsgChoice FC_MEM_UP_TO_TARGET_PRI_IND_STRU */
    ID_FC_MEM_DOWN_TO_TARGET_PRI_IND    = 0x0010,           /* _H2ASN_MsgChoice FC_MEM_DOWN_TO_TARGET_PRI_IND_STRU */
    ID_FC_CPU_C_OVERLOAD_IND            = 0x0011,           /* _H2ASN_MsgChoice FC_CPULOAD_IND_STRU */
    ID_FC_CPU_C_UNDERLOAD_IND           = 0x0012,           /* _H2ASN_MsgChoice FC_CPULOAD_IND_STRU */
    ID_FC_GPRS_C_FLOWCTRL_NOTIFY        = 0x0013,           /* _H2ASN_MsgChoice FC_GPRS_FLOWCTRL_NOTIFY_STRU */
    ID_FC_ACORE_CRESET_START_IND        = 0x0014,           /* _H2ASN_MsgChoice FC_ACORE_CRESET_IND_STRU */
    ID_FC_ACORE_CRESET_END_IND          = 0x0015,           /* _H2ASN_MsgChoice FC_ACORE_CRESET_IND_STRU */
    ID_FC_ACORE_CRESET_START_RSP        = 0x0016,           /* _H2ASN_MsgChoice FC_ACORE_CRESET_RSP_STRU */
    ID_FC_ACORE_CRESET_END_RSP          = 0x0017,           /* _H2ASN_MsgChoice FC_ACORE_CRESET_RSP_STRU */

    ID_FC_MSG_TYPE_BUTT                 = 0xFFFF
};
typedef VOS_UINT16 FC_MSG_TYPE_ENUM_UINT16;


/*****************************************************************************
  3 宏定义
*****************************************************************************/


/*****************************************************************************
  4 STRUCT定义
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                      /* _H2ASN_Skip */
    FC_MSG_TYPE_ENUM_UINT16             usMsgName;      /* _H2ASN_Skip */
    VOS_UINT8                           aucRsv[2];
    VOS_UINT32                          ulCpuLoad;
    VOS_UINT32                          ulUlRate;
    VOS_UINT32                          ulDlRate;
}FC_CPULOAD_IND_STRU;


typedef struct
{
    VOS_MSG_HEADER                                      /* _H2ASN_Skip */
    FC_MSG_TYPE_ENUM_UINT16             usMsgName;      /* _H2ASN_Skip */
    VOS_UINT16                          usTemperature;
}FC_TEMPERATURE_IND_STRU;


typedef struct
{
    VOS_MSG_HEADER                                      /* _H2ASN_Skip */
    FC_MSG_TYPE_ENUM_UINT16             usMsgName;      /* _H2ASN_Skip */
    VOS_UINT8                           aucRsv[2];
}FC_SET_GPRS_FLOWCTRL_IND_STRU;

typedef struct
{
    VOS_MSG_HEADER                                      /* _H2ASN_Skip */
    FC_MSG_TYPE_ENUM_UINT16             usMsgName;      /* _H2ASN_Skip */
    VOS_UINT8                           aucRsv[2];
}FC_GPRS_FLOWCTRL_NOTIFY_STRU;


typedef struct
{
    VOS_MSG_HEADER                                      /* _H2ASN_Skip */
    FC_MSG_TYPE_ENUM_UINT16             usMsgName;      /* _H2ASN_Skip */
    VOS_UINT8                           aucRsv[2];
}FC_STOP_GPRS_FLOWCTRL_IND_STRU;

typedef struct
{
    VOS_MSG_HEADER                                      /* _H2ASN_Skip */
    FC_MSG_TYPE_ENUM_UINT16             usMsgName;      /* _H2ASN_Skip */
    FC_ID_ENUM_UINT8                    enFcId;
    VOS_UINT8                           aucRsv[1];
}FC_SET_POINT_FLOWCTRL_IND_STRU;


typedef struct
{
    VOS_MSG_HEADER                                      /* _H2ASN_Skip */
    FC_MSG_TYPE_ENUM_UINT16             usMsgName;      /* _H2ASN_Skip */
    FC_ID_ENUM_UINT8                    enFcId;
    VOS_UINT8                           aucRsv[1];
}FC_STOP_POINT_FLOWCTRL_IND_STRU;


typedef struct
{
    VOS_MSG_HEADER                                      /* _H2ASN_Skip */
    FC_MSG_TYPE_ENUM_UINT16             usMsgName;      /* _H2ASN_Skip */
    VOS_UINT8                           aucRsv[2];
    FC_REG_POINT_STRU                   stFcPoint;
}FC_REG_POINT_IND_STRU;


typedef struct
{
    VOS_MSG_HEADER                                      /* _H2ASN_Skip */
    FC_MSG_TYPE_ENUM_UINT16             usMsgName;      /* _H2ASN_Skip */
    MODEM_ID_ENUM_UINT16                enModemId;      /* _H2ASN_Replace VOS_UINT16  enModemId; */
    FC_ID_ENUM_UINT8                    enFcId;
    VOS_UINT8                           aucRsv[7];
}FC_DEREG_POINT_IND_STRU;

typedef struct
{
    VOS_MSG_HEADER                                      /* _H2ASN_Skip */
    FC_MSG_TYPE_ENUM_UINT16             usMsgName;      /* _H2ASN_Skip */
    MODEM_ID_ENUM_UINT16                enModemId;      /* _H2ASN_Replace VOS_UINT16  enModemId; */
    FC_ID_ENUM_UINT8                    enFcId;
    FC_POLICY_ID_ENUM_UINT8             enPolicyId;
    FC_PRI_ENUM_UINT8                   enPri;
    VOS_UINT8                           aucRsv[5];
}FC_CHANGE_POINT_IND_STRU;


typedef struct
{
    VOS_MSG_HEADER                                      /* _H2ASN_Skip */
    FC_MSG_TYPE_ENUM_UINT16             usMsgName;      /* _H2ASN_Skip */
    MODEM_ID_ENUM_UINT16                enModemId;      /* _H2ASN_Replace VOS_UINT16  enModemId; */
    FC_ID_ENUM_UINT8                    enFcId;
    VOS_UINT8                           ucRabId;
    VOS_UINT8                           aucRsv[6];
} FC_ADD_RAB_FCID_MAP_IND_STRU;

typedef struct
{
    VOS_MSG_HEADER                                      /* _H2ASN_Skip */
    FC_MSG_TYPE_ENUM_UINT16             usMsgName;      /* _H2ASN_Skip */
    MODEM_ID_ENUM_UINT16                enModemId;      /* _H2ASN_Replace VOS_UINT16  enModemId; */
    VOS_UINT8                           ucRabId;
    VOS_UINT8                           aucRsv[7];
} FC_DEL_RAB_FCID_MAP_IND_STRU;

typedef struct
{
    VOS_MSG_HEADER                                      /* _H2ASN_Skip */
    FC_MSG_TYPE_ENUM_UINT16             usMsgName;      /* _H2ASN_Skip */
    VOS_UINT16                          usMemFreeCnt;
    FC_PRI_ENUM_UINT8                   enTargetPri;
    VOS_UINT8                           aucRsv[7];
} FC_MEM_UP_TO_TARGET_PRI_IND_STRU;


typedef struct
{
    VOS_MSG_HEADER                                      /* _H2ASN_Skip */
    FC_MSG_TYPE_ENUM_UINT16             usMsgName;      /* _H2ASN_Skip */
    VOS_UINT16                          usMemFreeCnt;
    FC_PRI_ENUM_UINT8                   enTargetPri;
    VOS_UINT8                           aucRsv[7];
} FC_MEM_DOWN_TO_TARGET_PRI_IND_STRU;

/*****************************************************************************
 结构名    : FC_ACORE_CRESET_IND_STRU
 协议表格  : 无
 ASN.1描述 : 无
 结构说明  : C核单独复位特性中，底软通知FcACore的指示结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                      /* _H2ASN_Skip */
    FC_MSG_TYPE_ENUM_UINT16             usMsgName;      /* _H2ASN_Skip */
    VOS_UINT8                           aucRsv[2];
}FC_ACORE_CRESET_IND_STRU;

/*****************************************************************************
 结构名    : FC_ACORE_CRESET_RSP_STRU
 协议表格  : 无
 ASN.1描述 : 无
 结构说明  : C核单独复位特性中，FcACore完成回调事务后的通知消息结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                      /* _H2ASN_Skip */
    FC_MSG_TYPE_ENUM_UINT16             usMsgName;      /* _H2ASN_Skip */
    VOS_UINT8                           aucRsv[2];
    VOS_UINT32                          ulResult;
}FC_ACORE_CRESET_RSP_STRU;

/*****************************************************************************
  5 OTHERS定义
*****************************************************************************/
/*****************************************************************************
  H2ASN顶级消息结构定义
*****************************************************************************/
typedef struct
{
    FC_MSG_TYPE_ENUM_UINT16             enMsgID;    /*_H2ASN_MsgChoice_Export FC_MSG_TYPE_ENUM_UINT16*/

    VOS_UINT8                           aucMsgBlock[2];
    /***************************************************************************
        _H2ASN_MsgChoice_When_Comment          FC_MSG_TYPE_ENUM_UINT16
    ****************************************************************************/
}FC_INTRA_MSG_DATA;
/*_H2ASN_Length UINT32*/

typedef struct
{
    VOS_MSG_HEADER
    FC_INTRA_MSG_DATA                   stMsgData;
}FcIntraMsg_MSG;


/******************************************************************************
   6 外部函数声明
******************************************************************************/


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

#endif /* FcIntraMsg.h */


