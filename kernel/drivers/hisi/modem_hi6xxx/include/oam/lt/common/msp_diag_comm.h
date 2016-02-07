#ifndef __MSP_DIAG_COMM_H__
#define __MSP_DIAG_COMM_H__
#ifdef __cplusplus
extern "C"
{
#endif
#include "vos.h"
#include "mdrv.h"
/**************************************************************************
  宏定义
**************************************************************************/
/*最好PS将MODID 和DIAG_AIR_MSG_LOG_ID的地方都替换成DIAG_ID*/
#define DIAG_ID(module_id, log_type)   (VOS_UINT32)(module_id | (log_type << 12))

#define MODID(module_id, log_type) DIAG_ID(module_id, log_type)

#define DIAG_AIR_MSG_LOG_ID(module_id, is_up_link)  DIAG_ID(module_id, is_up_link) /*module_id对应PID*/


/*is_up_link取值*/
#define OS_MSG_UL                (0x01)/* 表示上行消息*/
#define OS_MSG_DL                (0x02)/* 表示下行消息*/


#define DIAG_SIDE_UE             (0x1)  /* 表示UE接收的空口消息：NET-->UE*/
#define DIAG_SIDE_NET            (0x2)  /* 表示NET接收的空口消息：UE-->NET*/


/* 事件信息打印级别定义*/
#define LOG_TYPE_INFO            0x00000008UL
#define LOG_TYPE_AUDIT_FAILURE   0x00000004UL
#define LOG_TYPE_AUDIT_SUCCESS   0x00000002UL
#define LOG_TYPE_ERROR           0x00000001UL
#define LOG_TYPE_WARNING         0x00000010UL

/*diag fw发给AGENT模块的请求，以及相应的回复消息*/
/*原本放在diag_message_id.h中,*/
#define ID_MSG_DIAG_HSO_DISCONN_IND                 (0x00010004)

/**************************************************************************
  结构定义
**************************************************************************/
typedef struct
{
    VOS_UINT32 ulId;
    VOS_UINT32 ulMessageID;  /* Specify the message id.*/
    VOS_UINT32 ulSideId;     /* 空口消息的方向*/
    VOS_UINT32 ulDestMod;    /* 目的模块*/
    VOS_UINT32 ulDataSize;   /* the data size in bytes.*/
    VOS_VOID* pData;      /* Pointer to the data buffer.*/
} DIAG_AIR_MSG_LOG_STRU;

typedef struct
{
    VOS_UINT32 ulId;
    VOS_UINT32 ulMessageID;  /* Specify the message id.*/
    VOS_UINT32 ulSideId;     /* VoLTE消息的方向,0x1:网侧到UE侧 0x2:UE侧到网侧 0xFFFFFFFF:为无效值,代表与方向无关*/
    VOS_UINT32 ulDestMod;    /* 目的模块*/
    VOS_UINT32 ulDataSize;   /* the data size in bytes.*/
    VOS_VOID*  pData;        /* Pointer to the data buffer.*/
} DIAG_VoLTE_LOG_STRU;

typedef DIAG_AIR_MSG_LOG_STRU DIAG_LAYER_MSG_STRU;

typedef struct
{
    VOS_UINT32 ulMessageID;    /* Specify the message id.*/
    VOS_UINT32 ulDataSize;     /* the data size in bytes.*/
    VOS_VOID* pData;        /* Pointer to the data buffer.*/
} DIAG_USERPLANE_MSG_STRU;


#ifndef FEATURE_UPGRADE_TL
/* SOCP投票组件 */
enum SOCP_VOTE_ID_ENUM
{
    SOCP_VOTE_GU_OM_APP,    /* GU OM APP */
    SOCP_VOTE_GU_OM_COMM,   /* GU OM COMM*/
    SOCP_VOTE_GU_DSP,       /* GU DSP */
    SOCP_VOTE_DIAG_APP,     /* DIAG APP,代表DRV APP */
    SOCP_VOTE_DIAG_COMM,    /* DIAG COMM,代表LDSP、DRV COMM */
    SOCP_VOTE_DIAG_DEC,     /* DIAG解码 */
    SOCP_VOTE_ID_BUTT
};
typedef unsigned int SOCP_VOTE_ID_ENUM_U32;

/* SOCP投票类型 */
enum SOCP_VOTE_TYPE_ENUM
{
    SOCP_VOTE_FOR_SLEEP,    /* 允许睡眠 */
    SOCP_VOTE_FOR_WAKE,     /* 反对睡眠 */
    SOCP_VOTE_TYPE_BUTT
};
typedef unsigned int SOCP_VOTE_TYPE_ENUM_U32;
#endif
/*****************************************************************************
   5 STRUCT
*****************************************************************************/
/* 发送给MSP_PID_DIAG_APP_AGENT的投票请求消息结构体 */
typedef struct
{
    VOS_MSG_HEADER
    SOCP_VOTE_ID_ENUM_U32   ulVoteId;    /* 投票组件 */
    SOCP_VOTE_TYPE_ENUM_U32 ulVoteType;  /* 投票类型 */
}DIAG_MSG_SOCP_VOTE_REQ_STRU;
#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
/* 反对票消息回复 */
typedef struct
{
    VOS_MSG_HEADER
    SOCP_VOTE_ID_ENUM_U32   ulVoteId;    /* 投票组件 */
    SOCP_VOTE_TYPE_ENUM_U32 ulVoteType;  /* 投票类型 */
    VOS_UINT32              ulVoteRst;   /* 投票结果，0-成功, 0xFFFFFFFF-失败 */
}DIAG_MSG_SOCP_VOTE_WAKE_CNF_STRU;
#endif

/**************************************************************************
  函数声明
**************************************************************************/

/*****************************************************************************
 Function Name   : DIAG_PrintfV
 Description     : 打印上报接口
 Input           : VOS_UINT32 id:module id
                   VOS_CHAR* pszFileName:文件名
                   VOS_UINT32 ulLineNum:行号
                   VOS_CHAR* pszFmt:打印形式
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-20  Draft Enact

*****************************************************************************/
VOS_UINT32 DIAG_PrintfV(VOS_UINT32 id, VOS_CHAR* pszFileName, VOS_UINT32 ulLineNum, VOS_CHAR* pszFmt, ...);
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
VOS_UINT32 DIAG_ReportCommand(VOS_UINT16 ulID, VOS_UINT32 ulDataSize, VOS_VOID* pData);
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
VOS_UINT32 DIAG_ReportEventLog(VOS_UINT32 id, VOS_UINT32 ulEventID);
/*****************************************************************************
 Function Name   : DIAG_ReportAirMessageLog
 Description     : LTE和TDS空口上报接口，给PS使用
 Input           :DIAG_AIR_MSG_LOG_STRU* pRptMessage
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-20  Draft Enact

*****************************************************************************/
VOS_UINT32 DIAG_ReportAirMessageLog(DIAG_AIR_MSG_LOG_STRU* pRptMessage);
/*****************************************************************************
 Function Name   : DIAG_ReportVoLTELog
 Description     : VoLTE消息上报接口
 Input           : DIAG_VoLTE_LOG_STRU* pRptMessage
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.fuxin      2013-12-30  create
*****************************************************************************/
VOS_UINT32 DIAG_ReportVoLTELog(DIAG_VoLTE_LOG_STRU* pRptMessage);
/*****************************************************************************
 Function Name   : DIAG_ReportLayerMessageLog
 Description     : 层间消息上报接口，给PS使用
 Input           :DIAG_LAYER_MSG_STRU* pRptMessage
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-20  Draft Enact

*****************************************************************************/
VOS_UINT32 DIAG_ReportLayerMessageLog(DIAG_LAYER_MSG_STRU* pRptMessage);
/*****************************************************************************
 Function Name   : DIAG_ReportUserPlaneMessageLog
 Description     : 用户面上报接口，给PS使用
 Input           :DIAG_USERPLANE_MSG_STRU* pRptMessage
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-20  Draft Enact

*****************************************************************************/
VOS_UINT32 DIAG_ReportUserPlaneMessageLog(DIAG_USERPLANE_MSG_STRU* pRptMessage);

/******************************************************************************
函数名称: LTE_DIAG_LOG
功能描述: 打印点类型的打印接口函数
参数说明:
            ulModuleId[in]  : 模块id，对应PID
            ulSubModId[in]  : 子模块id
            ulLevel[in]     : 打印级别
            ulLogId[in]     : 由文件号和行号根据DIAG_LOG_ID生成
            amount[in]      : 可变参数个数（最大值6，不包括ulModuleId/ulLevel/ulLogId/amout）
            ...             : 可变参数
调用约束:
            1. 绝对禁止对此函数进行二次封装，只能转定义
            2. 支持可变的参数个数，但必须在调用时由参数amount指定参数个数
            3. 可变参数只支持int类型
            4. 目前版本中支持的最大参数个数是6个，超过的参数默认丢弃
******************************************************************************/
#if 0
extern VOS_INT32 LTE_DIAG_LOG(VOS_UINT32 ulModuleId, VOS_UINT32 ulSubModId,
                        VOS_UINT32 ulLevel, VOS_UINT32 ulLogId,
                        VOS_UINT32 amount, ...);
#endif



#ifdef __cplusplus
}
#endif
#endif

