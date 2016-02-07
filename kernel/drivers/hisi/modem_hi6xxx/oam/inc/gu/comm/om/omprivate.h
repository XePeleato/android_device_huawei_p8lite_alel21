/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : Omprivate.h
  版 本 号   : 初稿
  作    者           : 甘兰47350
  生成日期   : 2008年5月3日
  最近修改   :
  功能描述   : 该头文件为OM的头文件，主要提供给OM模块内部使用
  函数列表   :
  修改历史   :
  1.日    期 : 2008年5月3日
    作    者 : 甘兰47350
    修改内容 : 创建文件

******************************************************************************/
#ifndef _OM_PRIVATE_H_
#define _OM_PRIVATE_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "PsTypeDef.h"
#include "om.h"
#include "omrl.h"
#include "product_config.h"
#include "cpm.h"
#include "TtfOamInterface.h"
#include "WasOmInterface.h"
#include "OmToolInterface.h"
#include "SOCPInterface.h"
#include "OmApp.h"
#include "NVIM_Interface.h"
#include "OmCommonPpm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/*获得NV操作的返回值*/
#if defined(_lint)
#define  NV_GET_RETURN(errNo, nvId) {\
                VOS_UINT32 ulNvId = (VOS_UINT32)nvId;\
                ulNvId <<= 16;\
                errNo  = ulNvId | errNo;\
            }

#else
#define  NV_GET_RETURN(errNo, nvId) do {\
                VOS_UINT32 ulNvId = (VOS_UINT32)nvId;\
                ulNvId <<= 16;\
                errNo  = ulNvId | errNo;\
            }while(0)\

#endif
/*协议栈模块的最大个数*/
#define OM_MAX_PS_MODULE_NUM                      (VOS_PID_BUTT - VOS_PID_DOPRAEND)

/* OM可维可测SOCP寄存器的最大个数 */
#define OM_MAX_SOCP_REG_NUM                       (72)

/*标明消息全部打开，不用进行过滤，用于TRACE,EVENT*/
#define OM_ALL_MSG_ON                             0xffffffff
/*WPHY MailBox，控制当前HPA发送给OM的消息，没有被OM处理的数量*/
#define DSP_SEND_OM_MSG_MAX_NUM                   (VOS_FID_QUEUE_LENGTH - 110)
/*Used to control sequence number mutex.*/
#define OM_PV_TIMEOUT                             5000
/*值为OM_APP_READ_NV_STRU结构体中，sizeof(ulErrorCode) + sizeof(ulCount)*/
#define OM_READ_NV_HEAD_SIZE                      8
/*长度为NVID的值+NVID长度所占内存长度*/
#define OM_NV_ITEM_SIZE                           4
/*对于FuncId的分配划分，30及以下为OM本身使用，大于30为外部子系统使用*/
#define OM_FUNCID_PART_NUM                        30
#define OM_FUNCID_ACPU_PART_NUM                   45
#define OM_FUNCID_MAX_NUM                         60
/*The size of the password*/
#define OM_PASSWORD_LEN                           8
/*The type of restoring NV items*/
#define OM_RESTORE_ALL                            0
#define OM_RESTORE_MANUFACTURE                    1
/*当用户没有权限进行操作时，返回的错误码*/
#define OM_NEED_AUTH                              20
/*用于装备工具的版本查询，分为内部版本和外部版本*/
#define OM_INSIDE_VER                             0
#define OM_OUTSIDE_VER                            1
/*用在OM_FACTORY_VER_STRU结构中的版本长度*/
#define OM_FACTORY_VER_LEN                        32
/*用在OM_FACTORY_VER_STRU结构中的CDROM版本长度*/
#define OM_CDROM_VER_LEN                          128
/*IMEI项保存在NV项中的长度*/
#define OM_IMEI_NV_LEN                            16
/*返回给工具侧的IMEI项长度*/
#define OM_IMEI_LEN                               9
/*查询内存内容使用的内存个数*/
#define OM_MAX_MEMORY_ADDR_CNT                    8

/*高2bit是用来指示当前消息操作的Modem，因此真正的FuncID只有后6Bit*/
#define OM_FUNCID_VALUE_BITS                      (0x3F)

/*用于获取高2Bit的Modem ID 的值*/
#define OM_GET_FUNCID_MODEMINFO(ucFuncID)           ((ucFuncID>>6)&0x3)
#define OM_ADD_FUNCID_MODEMINFO(ucFuncID, ucModemID)(ucFuncID|((ucModemID<<6)&0xc0))

#if (FEATURE_ON == FEATURE_MULTI_FS_PARTITION)
#define OAM_ICC_RECORD_MAX_NUM           (1024)
#else
#define OAM_ICC_RECORD_MAX_NUM           (1)
#endif

#define OAM_ICC_RECORD_MAX_FILE          (20)

#define OM_ICC_MAX_NAME_LEN              (40)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*Query PA attribute*/
enum
{
    OM_W_PA_TEMP = 1,
    OM_G_PA_TEMP,
    OM_W_PLL_LOCK,
    OM_G_PLL_LOCK,
    OM_W_HKADC,
    OM_G_HKADC,
    OM_W_BBP_PLL_LOCK,
    OM_G_BBP_PLL_LOCK,
    OM_DSP_PLL_LOCK,
    OM_ARM_PLL_LOCK,
    OM_SIM_TEMP,
    OM_TCM_STATUS,
    OM_SDMMC_STATUS,
    OM_BATTER_VOLT,
    OM_BATTER_TEMP,
    OM_OLED_TEMP,
    OM_DCXO_TEMP,
    OM_DCXO_TEMP_LT,
    OM_OLED_BUTT
};

/*硬件类型*/
typedef enum
{
    ID_OM_PMU_STATUS = 1,
    ID_OM_ABB_STATUS,
    ID_OM_SPI_STATUS,
    ID_OM_I2C_STATUS,
    ID_OM_SIC_STATUS,
    ID_OM_BUTT
}OM_HARDWARE_STATUS_ENUM;

typedef enum
{
    LEVEL_NORMAL = 0,
    LEVEL_ADVANCED,
    LEVEL_BUTT
}OM_PRIVILEGE_LEVEL_ENUM;

typedef enum
{
    OM_FILE_TIMER = 1,     /*用来指示操作文件时启动的超时定时器*/
    OM_ICC_TIMER,
    OM_BUFF_TIMER,
    OM_ERRLOG_TIMER,
    OM_CONNECT_INFO_TIMER,

    OM_FILE_TIMER_BUTT
}OM_TIMER_NAME_ENUM;

/* the definition of error number which results in reboot ,created by xucheng*/

enum
{
    LOG_FROM_PS_LOG = 0,        /* ps_log 输入的log */
    LOG_FROM_PRINTF,             /* printf 输入的log */
    LOG_FROM_OM,                /* om 输入的log     */
    LOG_SOURCE_BUTT
};

/* the definition of error number which results in reboot ,created by xucheng*/

enum
{
    OAM_PROTECTION_LOAD_DSP         = 0x20000000,
    OAM_PROTECTION_DSP_Init         = 0x20000001,
    OAM_UL_AHB_PARTITION_ERR        = 0x20000002,
    OAM_UL_AHB_LENGTH_ERR           = 0x20000003,
    OAM_DL_R99_AHB_HEAD_ERR         = 0x20000004,
    OAM_UL_R99_AHB_HEAD_ERR         = 0x20000005,
    OAM_DL_PP_AHB_HEAD_ERR          = 0x20000006,
    OAM_UL_PP_AHB_HEAD_ERR          = 0x20000007,
    OAM_DL_NPP_AHB_HEAD_ERR         = 0x20000008,
    OAM_UL_NPP_AHB_HEAD_ERR         = 0x20000009,
    OAM_PARA_CHECK_ERR              = 0x2000000a,
    DRV_CACHEMALLOC_ERR             = 0x2000000b,
    OAM_PROTECTION_DSP_SHARE        = 0x2000000c,
    OAM_USB_SEND_ERROR              = 0x2000000d,
    OAM_LOAD_DSP_BUFFER_ERR         = 0x2000000e,

    DRX_REPORT_ERROR                = 0x2000000f,
    DRX_REPORT_BBP_POWERDOWN_ERROR  = 0x20000010,
    DRX_REPORT_BBP_READ_ERROR       = 0x20000011,
    DRX_ACTIVATEHW_ERROR            = 0x20000012,
    DRX_REPORT_HANDSHAKE_ABNORMAL   = 0x20000013,
    DRX_PHY_PROTECT_ERROR           = 0x20000014,

    DRV_SOCP_ERROR_START                = 0x20000050,
    DRV_SOCP_ERROR_END                  = 0x20000060,
    OAM_REG_REPORT_ERROR_END            = 0x20000070,
    OAM_PC_LENGTH_TOO_BIG               = 0x20000080,

    HPA_2GFRAME_ISR_NOT_INTERRUPT_CALL  = 0x20000090,
    HPA_3GFRAME_ISR_NOT_INTERRUPT_CALL  = 0x20000092,

    OAM_PROTECTION_ID_BUTT              = 0x2fffffff
};

/* OM 的event定义 */
enum
{
    OAM_EVENT_TIMER        = 5,
    OAM_DEGUG_EVENT_ID1    = 6,
    OAM_DEGUG_EVENT_ID2    = 7,
    OAM_DEGUG_EVENT_ID3    = 8,
    OAM_DEGUG_EVENT_ID4    = 9,
    OAM_EVENT_CONNECT_INFO = 10,

    /* 下次定义从 100 开始往后 之前的被使用 */
    OAM_DEGUG_EVENT_ID100  = 100,

    OAM_EVENT_BUTT
};

/*****************************************************************************
枚举名    : OM_HSIC_INIT_ENUM
枚举说明  : OM初始化时初始化HSIC状态消息类型

  1.日    期   : 2012年4月3日
    作    者   : h59254
    修改内容   : V7R1C51 锁网锁卡项目新增
*****************************************************************************/
typedef enum
{
    OM_HSIC_INIT_NOTIFY     = 0x0,

    OM_HSIC_INIT_BUTT
}OM_HSIC_INIT_ENUM;
typedef VOS_UINT32      OM_HSIC_INIT_ENUM_UINT32;




/* 调用DRV_CACHEDMAM_ALLOC函数分配空间失败，直接复位 */
#define OM_DRV_CACHEDMAM_ALLOC_ERROR()\
    mdrv_om_system_error(DRV_CACHEMALLOC_ERR, (VOS_INT)THIS_FILE_ID, (VOS_INT)__LINE__, VOS_NULL_PTR, 0)

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
    VOS_OM_HEADER
    VOS_UINT16             usPrimId;   /*Indicates current msg type.*/
    VOS_UINT16             usToolId;   /*Not used now.*/
    VOS_UINT8              aucPara[4]; /*Message content.*/
}OM_APP_MSG_EX_STRU;

typedef VOS_UINT32 (*OM_FUN)(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId);

typedef struct
{
    OM_FUN      pfFun;              /*Reserves the pointer of function handles current msg.*/
    VOS_UINT32  ulPrimId;           /*Indicates current msg type.*/
    VOS_UINT32  ulReturnPrimId;     /*Indicates return msg type.*/
}OM_MSG_FUN_STRU;

typedef struct
{
    OM_REQUEST_PROCEDURE    *pOmFuncProc;
    VOS_UINT32               ulFuncType;
    OM_PRIVILEGE_LEVEL_ENUM  enPrivilegeLev;
}OM_FUNCID_PROC_STRU;

typedef struct
{
    OM_APP_HEADER_STRU  stAppHeader;
    VOS_UINT16          usEventId;    /*Indicates current event type.*/
    VOS_UINT16          usReserved;
    VOS_UINT32          ulModuleId;   /*Indicates PID sends current event.*/
    VOS_UINT8           aucPara[4];
}OM_APP_EVENT_STRU;

typedef struct
{
    OM_APP_HEADER_STRU  stAppHeader;
    VOS_UINT16          usPrimId;     /*Indicates current log type.*/
    VOS_UINT16          usReserved;
    VOS_UINT32          ulModuleId;   /*Indicates PID sends current event.*/
    VOS_UINT32          ulPrintLev;   /*Indicates print level.*/
    VOS_UINT8           aucPara[4];
}OM_APP_LOG_STRU;

typedef struct
{
    VOS_UINT16    usSysMsg;       /*0 - 不跟踪系统消息，1 - 跟踪系统消息*/
    VOS_UINT16    usL2Msg;        /*0 - 关闭层2消息, 1 - 上报层2消息     3 - 上报层2内容*/
    VOS_UINT32    ulModuleNum;    /* 需要设置模块的数目*/
    VOS_UINT32    aulModuleId[OM_MAX_PS_MODULE_NUM];
}OM_TRACE_CONFIG_REQ_STRU;

typedef struct
{
    VOS_UINT32    ulModuleNum;    /* 设置为上报模块的数目, 0为关闭，-1为全部上报*/
    VOS_UINT32    aulModuleId[OM_MAX_PS_MODULE_NUM];  /*工具侧只用发送上报的模块，不用上报的不发送*/
}OM_TRACE_MASK_CONFIG_REQ_STRU;

typedef struct
{
    VOS_UINT32    ulModuleNum;    /* 设置为上报模块的数目, 0为关闭，-1为全部上报*/
    VOS_UINT32    aulModuleId[OM_MAX_PS_MODULE_NUM];  /*工具侧只用发送上报的模块，不用上报的不发送*/
}OM_EVENT_CONFIG_TABLE_STRU;

typedef struct
{
    VOS_UINT16    usSysMsg;       /*0 - 不跟踪系统消息，1 - 跟踪系统消息*/
    VOS_UINT16    usL2Msg;        /*0 - 关闭层2消息, 1 - 上报层2消息     3 - 上报层2内容*/
    VOS_UINT32    aulRptConfigLevl[OM_MAX_PS_MODULE_NUM]; /*PS模块Trace和Event上报状态 bit0为Trace，bit1为Event*/
}OM_TRACE_EVENT_CONFIG_PS_STRU;

/*Log config structs come from last version, I doesn't attend to modify in this version.*/
typedef struct
{
    VOS_UINT32    ulModuleNumber;
    VOS_UINT32    ulSubModuleNumber;
    VOS_UINT32    ulPrintLev;
}OM_MODULE_PRINT_LEV_STRU;

typedef struct
{
    VOS_UINT32 ulCount;	            /*要读取的NV项个数*/
    VOS_UINT16 ausNvItemId[2];      /*要读取的NV项ID数组，其中个数为ulCount*/
}APP_OM_READ_NV_STRU;

typedef struct
{
    VOS_UINT32 ulErrorCode;        /*返回执行结果*/
    VOS_UINT32 ulCount;            /*返回的NV项个数*/
    VOS_UINT16 ausNVItemData[2];   /*包括NVID值、NVID内容的长度、NVID的内容*/
}OM_APP_READ_NV_STRU;

typedef struct
{
    VOS_UINT32 ulCount;             /*要写入的NV项个数*/
    VOS_UINT16 ausNvItemData[2];    /*包括NVID值、NVID内容的长度、NVID的内容*/
}APP_OM_WRITE_NV_STRU;

typedef struct
{
    VOS_UINT32          ulErrorCode;    /*返回执行结果*/
    VOS_UINT16          usCount;        /*返回的NV项个数*/
    VOS_UINT8           ucTotalMsgCnt;  /*总消息包，留作将来分包上报使用*/
    VOS_UINT8           ucMsgIndex;     /*当前消息包，留作将来分包上报使用*/
    NV_LIST_INFO_STRU   astNvInfo[1];
}OM_APP_GET_NV_LIST_STRU;

typedef struct
{
    VOS_UINT32          ulErrorCode;        /* 错误码：OK/ERR */
    VOS_UINT32          ulProductId;        /* 产品号         */
    VOS_UINT32          ulCount;            /* 返回NV Id个数  */
    VOS_UINT16          ausNvFilterList[2]; /* NV Id列表      */
}OM_APP_GET_NV_FILTER_LIST_STRU;

/*查询当前硬件的非周期性状态信息*/
typedef struct
{
   VOS_UINT32 ulNum;
   VOS_UINT32 aulHWId[1];
}APP_OM_APERIODIC_STATUS_STRU;

typedef struct
{
    VOS_UINT32 ulHWId;
    VOS_UINT32 ulHWStatus;
}OM_HW_STATUS_STRU;

typedef struct
{
    VOS_UINT32        ulNum;
    OM_HW_STATUS_STRU astHWStatus[1];
}OM_APP_APERIODIC_STATUS_STRU;
/*查询PA属性结构体*/
typedef struct
{
    VOS_UINT16        usQueryType;
    VOS_INT16         sQueryResult;
}OM_PA_ITEM_STRU;

typedef struct
{
    VOS_UINT32        ulResult;
    OM_PA_ITEM_STRU   aPaItem[1];
}OM_APP_PA_ATTRIBUTE_STRU;

typedef struct
{
    VOS_OM_HEADER
    VOS_UINT16             usPrimId;   /*Indicates current msg type.*/
    VOS_UINT16             usToolId;   /*Not used now.*/
    VOS_UINT32             ulResult;   /*Result of operation*/
    VOS_UINT8              aucSoftwareVersion[OM_FACTORY_VER_LEN];  /*Software Version*/
    VOS_UINT8              aucHardwareVersion[OM_FACTORY_VER_LEN];  /*Hareware Version*/
    VOS_UINT8              aucProductVersion[OM_FACTORY_VER_LEN];   /*Product Version*/
    VOS_UINT8              aucCdRomVersion[OM_CDROM_VER_LEN];       /*CdRom Version*/
}OM_FACTORY_VER_STRU;

typedef struct
{
    VOS_OM_HEADER
    VOS_UINT16             usPrimId;   /*Indicates current msg type.*/
    VOS_UINT16             usToolId;   /*Not used now.*/
    VOS_UINT32             ulResult;   /*Result of operation*/
    VOS_UINT8              aucIMEI[OM_IMEI_LEN];  /*Software Version*/
    VOS_UINT8              aucResv[3];
}OM_FACTORY_LABEL_STRU;

typedef struct
{
    VOS_UINT32              ulLength;
    VOS_UINT8               aucPassword[OM_PASSWORD_LEN];
}OM_PASSWORD_STRU;

typedef struct
{
    VOS_UINT16              usSysMode;
    VOS_UINT8               ucNvEnable;
    VOS_UINT8               ucFtmMode;
}OM_LMT_MSG_STRU;

typedef struct
{
    VOS_UINT16              usSysMode;
    VOS_UINT16              usRsv;
}OM_ACTIVE_PHY_STRU;

typedef struct
{
    VOS_UINT32                  ulResult;
    VOS_UINT                    ulAllocMemSize;
    VOS_UINT                    ulTotalMemSize;
    VOS_UINT32                  ulMinPid;
    VOS_UINT32                  ulPidNum;
    VOS_MEM_OCCUPYING_INFO_ST   astMemOccupyInfo[1];
}OM_MEMORY_INFO_STRU;

/*NV项中保存当前支持频段的结构体*/
typedef struct
{
    VOS_UINT16                  usWBands;
    VOS_UINT16                  usGBands;
}OM_HW_BANDS_STRU;

typedef struct
{
    VOS_UINT16                  usNvId;
    VOS_UINT16                  usLen;
    VOS_CHAR*                   pcNvData;
}OM_RECOVER_NV_INfO;

typedef struct
{
    VOS_UINT32  ulCnt;
    VOS_UINT32  aulAddr[OM_MAX_MEMORY_ADDR_CNT];
}OM_QUERY_MEMORY_STRU;

typedef struct
{
    VOS_UINT16  usEfId;         /* 要读取的文件的ID */
    VOS_UINT8   ucRecordNum;    /* 文件的记录号     */
    VOS_UINT8   ucRsv;
}OM_QUERY_USIMM_STRU;

typedef struct
{
    VOS_UINT16  usEfId;         /* 要读取的文件的ID */
    VOS_UINT8   ucRecordNum;    /* 文件的记录号     */
    VOS_UINT8   ucEfLen;        /* 文件记录的长度   */
    VOS_UINT8   aucEf[4];       /* 文件的内容       */
}OM_CONFIG_USIMM_STRU;

/*****************************************************************************
结构名    : OM_HSIC_INIT_NOTIFY_MSG_STRU
结构说明  : AP+MODEM形态产品初始化HSIC状态的NOTIFY消息结构

  1.日    期   : 2012年4月10日
    作    者   : h59254
    修改内容   : 初始生成
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                  ulMsgName;
}OM_HSIC_INIT_NOTIFY_MSG_STRU;

typedef struct
{
    VOS_UINT16                          usSendPid;
    VOS_UINT16                          usRecvPid;
    VOS_UINT32                          ulMsgName;
    VOS_UINT32                          ulSlice;
    VOS_UINT32                          ulReserved;
}OM_ICC_INFO_STRU;

typedef struct
{
    VOS_UINT32                          ulResult;           /* 结果字段 */
    VOS_UINT32                          ulRsv;              /* 保留字段 */
    VOS_UINT64                          ulSlice;            /* SLICE值 */
}OM_QUERY_SLICE_CNF_STRU;



typedef struct
{
    VOS_UINT32                      ulUSBWriteNum1;
    VOS_UINT32                      ulUSBWriteNum2;
    VOS_UINT32                      ulUSBWriteCBNum;
    VOS_UINT32                      ulUSBWriteMaxTime;
    VOS_UINT32                      ulUSBINNum;
    VOS_UINT32                      ulUSBINTime;
    VOS_UINT32                      ulUSBOutNum;
    VOS_UINT32                      ulUSBOutTime;
    VOS_UINT32                      ulUSBStateErrNum;
    VOS_UINT32                      ulUSBStateErrTime;
    VOS_UINT32                      ulUSBWriteErrTime;
    VOS_UINT32                      ulUSBWriteErrNum;
    VOS_UINT32                      ulUSBWriteErrValue;
    VOS_UINT32                      ulUSBWriteErrLen;

    VOS_UINT32                      ulUSBOpenNum;
    VOS_UINT32                      ulUSBOpenSlice;
    VOS_UINT32                      ulUSBOpenOkNum;
    VOS_UINT32                      ulUSBOpenOkSlice;
    VOS_UINT32                      ulUSBOpenOk2Num;
    VOS_UINT32                      ulUSBOpenOk2Slice;
    VOS_UINT32                      ulUSBCloseNum;
    VOS_UINT32                      ulUSBCloseSlice;
    VOS_UINT32                      ulUSBCloseOkNum;
    VOS_UINT32                      ulUSBCloseOkSlice;

    /* Added by L00256032 for V8R1 OM_Optimize Project, 2014-05-25, begin */
    VOS_UINT32                      ulUSBIndPseudoSyncFailNum;
    VOS_UINT32                      ulUSBIndPseudoSyncFailSlice;
    VOS_UINT32                      ulUSBCnfPseudoSyncFailNum;
    VOS_UINT32                      ulUSBCnfPseudoSyncFailSlice;
    VOS_UINT32                      ulUSBUdiHandleErr;
    VOS_UINT32                      ulUSBUdiHandleReadGetBufferErr;
    VOS_UINT32                      ulUSBUdiCommRcvNullPtrErr;
    VOS_UINT32                      ulUSBUdiHandleReadBufferFreeErr;
    VOS_UINT32                      ulUSBRcvPktNum;
    VOS_UINT32                      ulUSBRcvPktByte;
    /* Added by L00256032 for V8R1 OM_Optimize Project, 2014-05-25, end */

}OM_ACPU_PORT_INFO;

typedef struct
{
    VOS_UINT32                          ulSocpCBParaErrNum;
    VOS_UINT32                          ulUSBSendNum;
    VOS_UINT32                          ulUSBSendLen;
    VOS_UINT32                          ulUSBSendRealNum;
    VOS_UINT32                          ulUSBSendErrNum;
    VOS_UINT32                          ulUSBSendErrLen;
    VOS_UINT32                          ulUSBSendCBAbnormalNum;
    VOS_UINT32                          ulUSBSendCBAbnormalLen;

    VOS_UINT32                          ulOmGetVirtErr;
    VOS_UINT32                          ulOmGetVirtSendLen;
    VOS_UINT32                          ulOmDiscardNum;
    VOS_UINT32                          ulOmDiscardLen;
    VOS_UINT32                          ulSocpReadDoneErrNum;
    VOS_UINT32                          ulSocpReadDoneErrLen;
}OM_SOCP_CHANNEL_DEBUG_INFO;

/*用于记录当前OM通道的信息*/
typedef struct
{
    OM_SOCP_CHANNEL_DEBUG_INFO          stIndDebugInfo;
    OM_SOCP_CHANNEL_DEBUG_INFO          stCnfDebugInfo;
    OM_ACPU_PORT_INFO                   astPortInfo[OM_PORT_HANDLE_BUTT];
    VOS_UINT32                          ulInvaldChannel;
}OM_ACPU_DEBUG_INFO;


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU        g_stAcpuTxCnfCtrlInfo;
extern OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU        g_stAcpuTxIndCtrlInfo;

extern VOS_UINT16  *g_pusAdcTable;           /*从NV项中获得RF的温度*/
extern VOS_UINT16  *g_pusUsimTempTable;    /*温度HKADC转换关系表*/
extern VOS_UINT16  *g_pusBatteryTempTable; /*Battery hkadc转换表*/
extern VOS_UINT16  *g_pusLedTempTable;      /*LED hkadc转换表*/
extern VOS_UINT16  *g_pusDcxoAdcTable;       /*DCXO hkadc转换表*/

extern VOS_UINT32                       g_ulOMSwitchOnOff;
extern OM_FUNCID_PROC_STRU              g_astAcpuOmFuncIdProcTbl[];
extern OM_FUNCID_PROC_STRU              g_astOmFuncIdProcTbl[];
extern VOS_UINT32                       g_aulPidTraceSendNum[];

extern OM_ACPU_DEBUG_INFO               g_stAcpuDebugInfo;
extern OM_TRACE_EVENT_CONFIG_PS_STRU    g_stAcpuTraceEventConfig;

extern OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU        g_stTxCnfCtrlInfo;

extern OMRL_MNTN_TX_CHAN_CTRL_INFO_STRU        g_stTxIndCtrlInfo;

/*供OM模块内部使用*/
extern VOS_VOID   OM_SendResult(VOS_UINT8 ucFuncType,
                              VOS_UINT32 ulResult, VOS_UINT16 usReturnPrimId);
extern VOS_VOID   OM_SendContent(VOS_UINT8 ucFuncType, OM_APP_MSG_EX_STRU *pstOmToAppMsg, VOS_UINT16 usReturnPrimId);

extern VOS_VOID OM_SendContentChannel(OM_LOGIC_CHANNEL_ENUM_UINT32 enChannel, VOS_UINT8 ucFuncType, OM_APP_MSG_EX_STRU *pstOmToAppMsg, VOS_UINT16 usReturnPrimId);

extern VOS_VOID OM_SendResultChannel(OM_LOGIC_CHANNEL_ENUM_UINT32 enChannel,VOS_UINT8 ucFuncType, VOS_UINT32 ulResult, VOS_UINT16 usReturnPrimId);
extern VOS_UINT32 OM_SendLog(VOS_UINT8 *pucLogData, VOS_UINT32 lLength);
extern VOS_VOID   OM_QueryMsgProc(OM_REQ_PACKET_STRU *pRspPacket, OM_RSP_FUNC *pRspFuncPtr);
extern VOS_VOID   OM_InitAuthVariable(VOS_VOID);
extern VOS_VOID   OM_ConfigMsgProc(OM_REQ_PACKET_STRU *pRspPacket, OM_RSP_FUNC *pRspFuncPtr);
extern VOS_UINT32 OM_ReadNv(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId);
extern VOS_UINT32 OM_WriteNv(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId);
extern VOS_UINT32 OM_SysCtrlCmd(OM_LOGIC_CHANNEL_ENUM_UINT32 enChannel, VOS_UINT16 usClientId, VOS_UINT32 ulMode, MODEM_ID_ENUM_UINT16 enModemID);
extern VOS_UINT32 OM_SetFTMReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId);
extern VOS_UINT32 OM_QueryModemNumReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId);
extern VOS_UINT32  OM_SysparaGetUsimFileReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg,VOS_UINT16 usReturnPrimId);
extern VOS_UINT32 OM_SysparaSetUsimFileReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg,VOS_UINT16 usReturnPrimId);

/*OM调用其他模块的API*/
extern VOS_UINT32 Log_InitFile(VOS_VOID);
extern VOS_VOID   Log_OmMsgProc(OM_REQ_PACKET_STRU *pRspPacket, OM_RSP_FUNC *pRspFuncPtr);
extern VOS_UINT32 Mmc_GetStaVersionInfo(OM_QUERY_VERSION_INFO_STRU * pstStaVersionInfo);

extern VOS_UINT32 Hi_Media_GetVersion( VOS_UINT8* pucBuff, VOS_UINT ulLength);
extern VOS_UINT32 WuepsAgingPidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_VOID   Aging_MsgProc(MsgBlock* pMsg);
extern VOS_UINT32 WuepsRegPidInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_VOID   Reg_MsgProc(MsgBlock *pRcvMsg);
extern VOS_UINT32 OM_PrintfInit(VOS_VOID);
extern VOS_VOID   OM_PrintfMsgProc(OM_REQ_PACKET_STRU *pstRspPacket, OM_RSP_FUNC *pstRspFuncPtr);
extern VOS_VOID   OM_PrintfTask(VOS_VOID);
extern VOS_INT    OM_RuningPsStateCheckCb(VOS_VOID);
extern VOS_UINT32 OM_QueryDspType(APP_OM_MSG_EX_STRU *pstAppToOmMsg,VOS_UINT16 usReturnPrimId);
extern VOS_VOID   OM_DspTypeInd(VOS_UINT8 ucModemId,VOS_UINT32 ulPhyMode,VOS_UINT16 usReturnPrimId);
extern VOS_UINT32 OM_InitLogFile(VOS_UINT32 ulModuleId, VOS_UINT32 ulFileSize);
extern VOS_UINT32 OM_WriteLogFile(VOS_UINT32 ulModuleId, VOS_CHAR *pcLogData, VOS_UINT32 ulLength);
extern VOS_VOID   OM_RemoveLogFile(VOS_UINT32 ulModuleId);
extern VOS_VOID   OM_PauseLogFile(VOS_UINT32 ulModuleId);
extern VOS_VOID   OM_ResumeLogFile(VOS_UINT32 ulModuleId);
extern VOS_VOID   OM_InitLogPath(void);
extern VOS_VOID   OM_ReadSdMsgProc(OM_REQ_PACKET_STRU *pstRspPacket, OM_RSP_FUNC *pstRspFuncPtr);

extern VOS_UINT32 OMRL_IccDataRecv(VOS_UINT8 *pucData, VOS_UINT16 uslen);

extern VOS_VOID   OM_ComRx_RegisterCB(OM_PORT_READ_DATA_CB pfDataFun);

extern VOS_UINT32 OM_HsicConnectReq(VOS_VOID);

extern VOS_VOID   OM_AcpuQueryMsgProc(OM_REQ_PACKET_STRU *pRspPacket, OM_RSP_FUNC *pRspFuncPtr);

extern VOS_VOID   SCM_SocpSendDataToUDI(SOCP_CODER_DST_ENUM_U32 enChanID, VOS_UINT8 *pucData, VOS_UINT8 *pucPHYData, VOS_UINT32 ulDataLen);

extern VOS_UINT32 OM_EstablishReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId);

extern VOS_UINT32 OM_ReleaseReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                           VOS_UINT16 usReturnPrimId);

extern VOS_VOID OM_NotifyOtherCpuReleaseLink(VOS_VOID);

extern VOS_UINT32 OM_AcpuSendDataChannel(OM_LOGIC_CHANNEL_ENUM_UINT32 enChannel, OM_RSP_PACKET_STRU *pucMsg, VOS_UINT16 usMsgLen);

extern VOS_VOID OM_AcpuSendResultChannel(OM_LOGIC_CHANNEL_ENUM_UINT32 enChannel, VOS_UINT8 ucFuncType,
                            VOS_UINT32 ulResult, VOS_UINT16 usReturnPrimId);
extern VOS_VOID OM_AcpuSendContentChannel(OM_LOGIC_CHANNEL_ENUM_UINT32 enChannel,VOS_UINT8 ucFuncType,
                             OM_RSP_PACKET_STRU *pstOmToAppMsg, VOS_UINT16 usReturnPrimId);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
