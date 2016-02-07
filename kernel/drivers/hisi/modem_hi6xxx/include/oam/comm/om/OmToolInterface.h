/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名      : OmToolInterface.h
  版 本 号      : 初稿
  作    者      : 靳妮168360
  生成日期      : 2012年8月1日
  最近修改      :
  功能描述      : 该头文件为OM和工具交互相关的数据结构头文件
  函数列表      :
  修改历史      :
  1.日    期    : 2012年8月1日
    作    者    : 靳妮168360
    修改内容    : 创建文件

******************************************************************************/
#ifndef _OMTOOLINTERFACE_H_
#define _OMTOOLINTERFACE_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif


/*****************************************************************************
  2 宏定义
*****************************************************************************/
/*组包后的消息头*/
/* 功能类型 */
/* 上报数据时，此字段保留 */
/* 此字段后的数据长度 */
#define VOS_OM_HEADER                   VOS_UINT8   ucFuncType;  \
                                        VOS_UINT8   ucCpuId;  \
                                        VOS_UINT16  usLength;

/*Transparent Message*/
#define APP_OM_TRANS_CONFIG                       0x5000
#define OM_TRANS_PRIMID                           0x5001
/*OM<->APP: For Trace, Modified for SDT safty*/
#define APP_OM_TRACE_REQ                          0x8101
#define OM_APP_TRACE_CNF                          0x8002
#define OM_APP_TRACE_IND                          0x8003

#define APP_OM_TRACE_MASK_REQ                     0x8004
#define OM_APP_TRACE_MASK_CNF                     0x8005

/*OM<->APP: For Event*/
#define APP_OM_EVENT_REQ                          0x8011
#define OM_APP_EVENT_CNF                          0x8012
/*OM<->APP : Just for operating NV.*/
#define APP_OM_READ_NV_REQ                        0x8021
#define OM_APP_READ_NV_IND                        0x8022
#define APP_OM_WRITE_NV_REQ                       0x8023
#define OM_APP_WRITE_NV_CNF                       0x8024
#define APP_OM_NV_BACKUP_REQ                      0x8027
#define OM_APP_NV_BACKUP_CNF                      0x8028
#define APP_OM_NV_ID_LIST_REQ                     0x8020
#define OM_APP_NV_ID_LIST_CNF                     0x802f
/*OM<->APP : Get the RF lock status.*/
#define APP_OM_RF_LOCK_REQ                        0x8031
#define OM_APP_RF_LOCK_IND                        0x8032
/*OM<->APP : Get Aperiodic Status*/
#define APP_OM_APER_STASTUS_REQ                   0x8041
#define OM_APP_APER_STASTUS_IND                   0x8042
/*OM<->APP : Query Memory contents.*/
#define APP_OM_QUERY_MM_REQ                       0x8051
#define OM_APP_QUERY_MM_IND                       0x8052
/*OM<->APP : Set LOG config.*/
#define APP_OM_SET_PRINT_LEV_REQ                  0x8061
#define OM_APP_SET_PRINT_LEV_CNF                  0x8062
#define OM_APP_PRINT_IND                          0x8064
#define APP_OM_SET_PRINT_OUTPUT_REQ               0x8065
#define OM_APP_SET_PRINT_OUTPUT_CNF               0x8066
#define APP_OM_EXPORT_LOG_END_CNF                 0x8067
/*OM<->APP : Set boot flag config.*/
#define APP_OM_SET_BOOT_REQ                       0x8071
#define OM_APP_SET_BOOT_CNF                       0x8072
/*OM<->APP : Set PCUI AT communication.*/
#define APP_OM_SET_ATCOM_REQ                      0x8073
#define OM_APP_SET_ATCOM_CNF                      0x8074
/*OM<->APP : Query Version*/
#define APP_OM_QUERY_VER_REQ                      0x8081
#define OM_APP_QUERY_VER_IND                      0x8082
/*OM<->APP : Query Current UE's memory info.*/
#define APP_OM_QUERY_MEM_INFO_REQ                 0x8083
#define OM_APP_QUERY_MEM_INFO_IND                 0x8084
/*OM<->APP : For USIM*/
#define APP_OM_GET_USIM_REQ                       0x8091
#define OM_APP_GET_USIM_CNF                       0x8092
#define APP_OM_SET_USIM_REQ                       0x8093
#define OM_APP_SET_USIM_CNF                       0x8094
/*OM<->APP : Just for LMT.*/
#define APP_OM_HANDLE_LMT_REQ                     0x80a1
#define OM_APP_HANDLE_LMT_CNF                     0x80a2
/*OM<->APP : Just for LMT.*/
#define APP_OM_ACTIVE_PHY_REQ                     0x80a3
#define OM_APP_ACTIVE_PHY_CNF                     0x80a4
/*OM<->APP : Just for LMT.*/
#define APP_OM_QUERY_DSP_REQ                      0x80a5
#define OM_APP_QUERY_DSP_CNF                      0x80a6
/*OM<->APP : Restablish link*/
#define APP_OM_ESTABLISH_REQ                      0x80b1
#define OM_APP_ESTABLISH_CNF                      0x80b2
/*OM<->APP : Release link*/
#define APP_OM_RELEASE_REQ                        0x80b3
#define OM_APP_RELEASE_CNF                        0x80b4
/*使用对某些特定功能需要进行保护*/
#define APP_OM_AUTHORIZE_REQ                      0x80b5
#define OM_APP_AUTHORIZE_CNF                      0x80b6
/* OM<->APP : NV ID filter list Req */
#define APP_OM_FILTER_LIST_REQ                    0x80b7
#define OM_APP_FILTER_LIST_CNF                    0x80b8
/*OM<->APP :查询PA的属性*/
#define APP_OM_PA_ATTRIBUTE_REQ                   0x80c1
#define OM_APP_PA_ATTRIBUTE_IND                   0x80c2
/*OM<->APP : 启动LOG文件保存*/
#define APP_OM_START_UP_LOG_REQ                   0x80c3
#define OM_APP_START_UP_LOG_CNF                   0x80c4
/*OM<->APP : 停止LOG文件保存*/
#define APP_OM_STOP_LOG_REQ                       0x80c5
#define OM_APP_STOP_LOG_CNF                       0x80c6

/*OM<->APP : 查询文件所在的目录, Modified for SDT safty*/
#define APP_OM_QUERY_DIR_REQ                      0x81d1
#define OM_APP_QUERY_DIR_IND                      0x80d2
/*OM<->APP : 获取目录中文件或文件夹的信息*/
#define APP_OM_SCAN_DIR_REQ                       0x80d3
#define OM_APP_SCAN_DIR_IND                       0x80d4
/*OM<->APP : 打开文件*/
#define APP_OM_OPEN_FILE_REQ                      0x80d5
#define OM_APP_OPEN_FILE_CNF                      0x80d6
/*OM<->APP : 读取文件内容*/
#define APP_OM_READ_FILE_REQ                      0x80d7
#define OM_APP_READ_FILE_IND                      0x80d8
/*OM<->APP : 写文件内容*/
#define APP_OM_WRITE_FILE_REQ                     0x80d9
#define OM_APP_WRITE_FILE_CNF                     0x80da
/*OM<->APP : 创建文件夹*/
#define APP_OM_MAKE_DIR_REQ                       0x80db
#define OM_APP_MAKE_DIR_CNF                       0x80dc
/*OM<->APP : 删除文件或文件夹*/
#define APP_OM_DELETE_FILE_REQ                    0x80dd
#define OM_APP_DELETE_FILE_CNF                    0x80de
/*OM<->APP : 重命名文件系统中的文件或文件夹*/
#define APP_OM_RENAME_FILE_REQ                    0x80df
#define OM_APP_RENAME_FILE_CNF                    0x80d0
/*OM<->APP : 查询文件系统的内存空间使用情况*/
#define APP_OM_QUERY_SPACE_REQ                    0x80f1
#define OM_APP_QUERY_SPACE_CNF                    0x80f2

/*OM<->APP : 读取SD卡数据*/
#define APP_OM_READ_SD_DATA_REQ                   0x80f3
#define APP_OM_READ_SD_CNF                        0x80f4
#define APP_OM_READ_SD_CLUSTER_REQ                0x80f5
#define APP_OM_READ_SD_IND                        0x80f6

/*软复位操作*/
#define APP_OM_REBOOT_REQ                         0x80e1
#define OM_APP_REBOOT_CNF                         0x80e2
/*查询软、硬件版本*/
#define APP_OM_VERSION_REQ                        0x80e3
#define OM_APP_VERSION_IND                        0x80e4
/*设置从信令下切换到非信令下*/
#define APP_OM_SET_FTM_REQ                        0x80e5
#define OM_APP_SET_FTM_CNF                        0x80e6
/*恢复出厂配置*/
#define APP_OM_LOAD_DEFAULT_REQ                   0x80e7
#define OM_APP_LOAD_DEFAULT_CNF                   0x80e8
/*查询单板侧标识，目前只支持IMEI查询*/
#define APP_OM_QUERY_LABEL_REQ                    0x80e9
#define OM_APP_QUERY_LABEL_IND                    0x80ea

/* PC测通过发送消息控制APT开关 */
#define APP_OM_CTRL_APT_REQ                       0x8101
#define OM_APP_CTRL_APT_CNF                       0x8102

/* PC侧下发BBP采数配置 */
#define APP_OM_BBP_DUMP_SET_REQ                   0x8103
#define OM_APP_BBP_DUMP_SET_CNF                   0x8104

/* PC侧下发BBP 可维可测配置 */
#define APP_OM_BBP_DBG_SET_REQ                    0x8105
#define OM_APP_BBP_DBG_SET_CNF                    0x8106

/* 在BBP 数采中断中模拟工具下发的一条消息，通知CCPU OM 数采结束 */
#define APP_OM_BBP_DUMP_END_REQ                   0x8107

/* 单板侧采数完成 */
#define OM_APP_BBP_DUMP_END_CNF                   0x8110

/* BBP可维可测EDMA上报异常，模似工具发送消息通知CCPU OM */
#define APP_OM_BBP_EDMA_ERR_REQ                   0x8113

/* 通知工具EDMA 错误 */
#define OM_APP_BBP_EDMA_ERR_CNF                   0x8114

/* BPP dump数据选择 */
#define APP_OM_BBP_DUMP_DUMP_REQ                  0x8115
#define OM_APP_BBP_DUMP_DUMP_CNF                  0x8116

/* 通知工具C核已经复位 */
#define OM_APP_CCORE_RESET_CNF                    0x8118

/* OM<->APP :SDT动态配置OM_Printf输出端口 */
#define APP_OM_SET_PRINTF_PORT_REQ                0x8201
#define OM_APP_SET_PRINTF_PORT_CNF                0x8202
/* OM->APP : 发送OM Printf模块输出到SDT*/
#define OM_APP_OM_PRINTF_IND                      0x8203
/* OM<->APP :SDT设置OM_Printf输出级别 */
#define APP_OM_SET_PRINTF_OUTPUT_LEV_REQ          0x8204
#define APP_OM_SET_PRINTF_OUTPUT_LEV_CNF          0x8205
/*配置RF参数使用的原语ID*/
#define OM_WDSP_RF_CONFIG_REQ                     0x1221
/*老化测试使用的原语ID*/
#define OM_GDSP_AGING_CONFIG_REQ                  0x1788

/* OM<->APP :LMT查询DSDA支持状态 */
#define APP_OM_QUERY_MODEM_NUM_REQ                0x8211
#define OM_APP_QUERY_MODEM_NUM_CNF                0x8212

/*OM<->APP : 查询64位slice值 */
#define APP_OM_QUERY_SLICE_REQ                    0x8215
#define OM_APP_QUERY_SLICE_CNF                    0x8216

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*****************************************************************************
  枚举名    : OM_TOOL_CPUID_ENUM
  结构说明  : OM和TOOL CPU ID枚举
1.日    期  : 2012年08月08日
  作    者  : J00168360
  修改内容  : V9R1 MSP拆分和可维可测开发项目新增结构
*****************************************************************************/
enum OM_TOOL_CPUID_ENUM
{
     OM_TOOL_RSV_ID,
     OM_TOOL_ACPU_ID,
     OM_TOOL_CCPU_ID,
     OM_TOOL_LDSP_ID,
     OM_TOOL_LBBP_ID,
     OM_TOOL_HIFI_ID,
     OM_TOOL_GUDSP_ID,
     OM_TOOL_TDSDSP_ID,
     OM_TOOL_TDSBBP_ID,
     OM_TOOL_MCU_ID,
     OM_TOOL_GPU_ID,
     OM_TOOL_GUBBP_ID,
     OM_TOOL_CPUID_BUTT
};
typedef VOS_UINT8 OM_TOOL_CPUID_ENUM_UINT8;

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
结构名    : OM_SOCP_HDR_STRU
结构说明  : SOCP的Service Layer层包头
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucCPUID;   /* 消息发送的目的CPU ID,取值范围为0-11，为兼容校准工具，此值为0和2均代表CCPU */
    VOS_UINT8                           aucRsv[3]; /* 保留字段 */
    VOS_UINT32                          ulRTC;     /* Real Timer Clock */
}OM_SOCP_HDR_STRU;

/*****************************************************************************
结构名    : OM_APP_MSG_SEGMENT_STRU
结构说明  : OM分包结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16    usSegLen;             /*当前帧的长度:该长度应用是在物理接口上传输的长度，即：aucSegMsgData指向的总数据长度＋5*/
    VOS_UINT8     ucSegNum;             /*总的分段数:Range[1,255] 1: 没有分段 other: 段的数目，第一阶段实现时，固定填写为1，即不分段*/
    VOS_UINT8     ucSegSn;              /*当前段序号:Range[1,255]*/
}OM_APP_MSG_SEGMENT_STRU;

/*****************************************************************************
结构名    : OM_APP_MSG_SEG_EX_STRU
结构说明  : OM上报数据帧扩展的分包结构
*****************************************************************************/
typedef struct
{
    OM_SOCP_HDR_STRU                    stSocpHdr;      /* SOCP的Service Layer层包头 */
    VOS_UINT32                          ulFrameHead;    /* 固定为0xaaaa5555 */
    OM_APP_MSG_SEGMENT_STRU             stMsgSeg;       /* 消息分段头 */
}OM_APP_MSG_SEG_EX_STRU;

/*****************************************************************************
结构名    : OM_APP_MSG_SEG_EX_STRU
结构说明  : 工具下发数据包扩展结构
*****************************************************************************/
typedef struct
{
    VOS_OM_HEADER
    VOS_UINT16                          usPrimId;     /* 指示当前消息类型 */
    VOS_UINT16                          usToolId;     /* 目前未使用 */
    VOS_UINT8                           aucPara[4];   /* 消息内容 */
}APP_OM_MSG_EX_STRU;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


