/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名      : omappagent.h
  版 本 号      : 初稿
  作    者      : 甘兰47350
  生成日期      : 2011年10月9日
  最近修改      :
  功能描述      : 该头文件为OmAppAgent和OmAgent的公共头文件，存放两者间交互消息的定义
  函数列表      :
  修改历史      :
  1.日    期    : 2011年10月9日
    作    者    : 甘兰47350
    修改内容    : 创建文件

******************************************************************************/

#ifndef _OM_APP_AGENT_H_
#define _OM_APP_AGENT_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "omrl.h"
#include "omprivate.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif


/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define WAITING_CNF_TIMEOUT_LEN          (5000)

/* ACPU向CCPU发送的请求消息，并且需要等待其回复，高位字节为0*/
#define ERRORLOG_REQ                (0x0007)
#define ERRORLOG_CNF                (0x0008)
#define OM_AUTOCONFIG_REQ           (0x0009)
#define OM_AUTOCONFIG_CNF           (0x000A)
#define OM_WRITE_LOG_REQ            (0x000C)
#define OM_CONNECT_INFO_REQ         (0x000B)


/* CCPU向ACPU发送的请求消息，不需要等待回复，高位字节为3 */
#define OM_OLED_CLEAR_REQ           (0x0301)
#define OM_OLED_DISPLAY_REQ         (0x0303)
#define OM_LINK_STATUS_REQ          (0x0305)
#define OM_HSIC_CONNECT_REQ         (0x0307)
#define OM_RECORD_DBU_INFO_REQ      (0x0309)
#define OM_CONNECT_INFO_CNF         (0x030b)
#define OM_CBT_SEND_DATA_REQ        (0x030d)

#define IS_CAGENT_CNF_MSG(usPrimId) (0 == (usPrimId >> 8))

#define ERRORLOG_HEAD_LEN           (52)

#define OM_LOG_FILE_MAX_SIZE        (0x40000)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/


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
    VOS_MSG_HEADER
    VOS_UINT16  usPrimId;           /* 指示当前为ErrorLog输出请求 */
    VOS_UINT8   aucReserved[2];
    VOS_UINT8   aucFileName[32];    /* 调用ErrorLog API所在文件名 */
    VOS_UINT32  ulFileId;           /* 调用ErrorLog API所在文件的文件ID */
    VOS_UINT32  ulLine;             /* 调用ErrorLog API所在文件的行号 */
    VOS_UINT32  ulErrNo;            /* ErrorLog的错误号 */
    VOS_UINT32  ulLen;              /* ErrorLog的输出内容的长度 */
    VOS_UINT8   aucData[8];         /* ErrorLog的输出内容，内容长度根据ulLen值确定 */
}ERRORLOG_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16 usPrimId;            /* 指示当前为ErrorLog输出结果返回 */
    VOS_UINT16 usResult;           /* VOS_OK为成功，VOS_ERR为失败 */
}ERRORLOG_CNF_STRU;

typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16  usPrimId;           /* 指示当前的消息类型 */
    VOS_UINT8   aucReserved[2];
}OM_AUTOCONFIG_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16  usPrimId;           /* 指示当前的消息类型 */
    VOS_UINT8   aucData[2];
}OM_AUTOCONFIG_CNF_STRU;

typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16                          usPrimId;                           /* 指示当前的消息类型 */
    VOS_UINT16                          usReserve;                          /* 保留 */
    VOS_UINT32                          ulOMSwitchOnOff;                    /* C核建链状态 */
    OM_UE_PC_FAIL_STRU                  stUe2PcIndErrInfo;                     /* C核Ind通道错误信息 */
    OM_UE_PC_FAIL_STRU                  stUe2PcCnfErrInfo;                     /* C核Cnf通道错误信息 */
    VOS_UINT32                          aucPidCfg[(VOS_CPU_ID_0_PID_BUTT - VOS_PID_CPU_ID_0_DOPRAEND)];    /* PID配置 */
    VOS_UINT32                          aucReg[OM_MAX_SOCP_REG_NUM];        /* 寄存器内容 */
}OM_CONNECT_INFO_CNF_STRU;

typedef struct
{
    OM_APP_HEADER_STRU                  stAppHeader;
    VOS_UINT16                          usEventId;    /*Indicates current event type.*/
    VOS_UINT16                          usReserved;
    VOS_UINT32                          ulModuleId;   /*Indicates PID sends current event.*/
    VOS_UINT32                          ulOMSwitchOnOff;                    /* C核建链状态 */
    OM_UE_PC_FAIL_STRU                  stUe2PcIndErrInfo;                     /* C核Ind通道错误信息 */
    OM_UE_PC_FAIL_STRU                  stUe2PcCnfErrInfo;                     /* C核Cnf通道错误信息 */
    VOS_UINT32                          aucPidCfg[(VOS_CPU_ID_0_PID_BUTT - VOS_PID_CPU_ID_0_DOPRAEND)];    /* PID配置 */
    VOS_UINT32                          aucReg[OM_MAX_SOCP_REG_NUM];        /* 寄存器内容 */
}OM_CONNECT_INFO_EVENT_STRU;

typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16 usPrimId;            /* 消息ID */
    VOS_UINT16 usReserve;           /* 保留 */
}OM_OLED_CLEAR_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16 usPrimId;            /* 消息ID */
    VOS_UINT16 usReserve;           /* 保留 */
    VOS_UINT16 usXPos;
    VOS_UINT16 usYPos;
}OM_OLED_DISPLAY_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16  usPrimId;           /* 指示当前为ErrorLog输出请求 */
    VOS_UINT8   aucFileName[34];    /* Log文件名 */
    VOS_UINT32  ulLen;              /* Log内容的长度 */
    VOS_UINT8   aucData[4];         /* Log的内容，内容长度根据ulLen值确定 */
}OM_WRITELOG_REQ_STRU;

/*****************************************************************************
结构名    : OM_HSIC_CONNECT_REQ_STRU
结构说明  : AP+MODEM形态产品OM和HSIC关联请求消息

  1.日    期   : 2012年4月10日
    作    者   : h59254
    修改内容   : 初始生成
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16 usPrimId;            /* 消息ID */
    VOS_UINT16 usReserve;           /* 保留 */
}OM_HSIC_CONNECT_REQ_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID OMRL_AcpuConnectInfoReportEvent(OM_CONNECT_INFO_CNF_STRU *pstConnectInfo);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

