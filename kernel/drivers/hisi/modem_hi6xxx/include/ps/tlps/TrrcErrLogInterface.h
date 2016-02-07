

#ifndef __TRRCERRLOGINTERFACE_H__
#define __TRRCERRLOGINTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include "omerrorlog.h"
#pragma pack(4)

/*****************************************************************************
  2 macro
*****************************************************************************/
#include  "vos.h"
#include "omerrorlog.h"
/*****************************************************************************
  3 Massage Declare
*****************************************************************************/
/* TDS邻区的最大个数，上报最强3个临区 */
#define TRRC_APP_MAX_NCELL_NUM                     (3)

/* RRC error发生时，最新RRC错误码的最大记录个数 */
#define TRRC_ERRLOG_CODE_MAX_NUM                   (4)

/* RRC error发生时，模块接收最新信息的最大记录个数 */
#define TRRC_APP_INFO_NUM                          (16)

/* ErrorLog新增丢网信息begin */
/* RRC error发生时，丢网最大记录个数 */
#define TRRC_ERRLOG_AREA_LOST_MAX_NUM               (4)
/* ErrorLog新增丢网信息end */
/* Seattle Begin */
#define TRRC_ERRLOG_IRAT_HO_FAIL_MAX_NUM            (4)
#define TRRC_ERRLOG_IRAT_HO_EXP_MAX_NUM             (4)
#define TRRC_ERRLOG_RL_FAIL_MAX_NUM                 (4)
/* Seattle End */
/*****************************************************************************
  4 Enum
*****************************************************************************/

/*****************************************************************************
 枚举名    : RRC_APP_ERROR_CODE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : RRC错误码类型
*****************************************************************************/
enum TRRC_APP_ERROR_CODE_ENUM
{
    TRRC_APP_RB_PROTOCOL_CHECK_ERR            = 0,   /* RB配置失败--Protocol检查失败 */
    TRRC_APP_RB_TRANSACTION_CHECK_ERR,               /* RB配置失败--Transaction ID检查失败 */
    TRRC_APP_RB_INTEGRITY_CHECK_ERR,                 /* RB配置失败--Integrity检查失败 */

    TRRC_APP_RBREC_PROTOCOL_CHECK_ERR         = 10,  /* RB重配置失败--Protocol检查失败 */
    TRRC_APP_RBREC_TRANSACTION_CHECK_ERR,            /* RB重配置失败--Transaction ID检查失败 */
    TRRC_APP_RBREC_INTEGRITY_CHECK_ERR,              /* RB重配置失败--Integrity检查失败 */

    TRRC_APP_CELLUPDATECNF_PROTOCOL_CHECK_ERR     =20,  /* 小区更新失败--Protocol检查失败 */
    TRRC_APP_CELLUPDATECNF_TRANSACTION_CHECK_ERR,       /* 小区更新失败--Transaction ID检查失败 */
    TRRC_APP_CELLUPDATECNF_INTEGRITY_CHECK_ERR,         /* 小区更新失败--Integrity检查失败 */

    TRRC_APP_EST_T300_ERR                         =30,  /* 次数超过N300时记录 */
    TRRC_APP_EST_CS_ACC_PROBABILITY_ERR,                /* CS域建链AC等级判断失败 */
    TRRC_APP_EST_PS_ACC_PROBABILITY_ERR,                /* PS域建链AC等级判断失败 */

    TRRC_APP_EST_AREA_LOST_ERR                    =40,  /* RRC丢网 */
    TRRC_APP_ASN_RLST_ERR,                              /* ASN解析出错 */
    TRRC_APP_AM_INTEGRITY_CHECK_ERR,                    /* AM数据完整性检查失败 */
    TRRC_APP_UM_INTEGRITY_CHECK_ERR,                    /* UM数据完整性检查失败 */

    TRRC_APP_MEAS_CTRL_PROTOCOL_CHECK_ERR         =50,  /* 测量控制失败--Protocol检查失败 */
    TRRC_APP_MEAS_CTRL_TRANSACTION_CHECK_ERR,           /* 测量控制失败--Transaction ID检查失败 */

    TRRC_APP_ERR_CODE_BUTT
};
typedef VOS_UINT8 TRRC_APP_ERROR_CODE_ENUM_UINT8;

/*****************************************************************************
 枚举名    : TRRC_APP_FTM_TYPE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : 工程模式上报类型，分周期型上报、更新后上报型
*****************************************************************************/
enum TRRC_APP_FTM_TYPE_ENUM
 {
    TRRC_APP_FTM_PERIOD_INFO,     /* 周期型上报的工程模式信息，目前Idle状态上报周期为DRX周期，Connected状态下周期为1秒 */
    TRRC_APP_FTM_CHANGE_INFO,     /* 更新后上报型的工程模式信息，变更上报分两种类型 */
    TRRC_APP_FTM_INFO_BUIT
 };
 typedef VOS_UINT32 TRRC_APP_FTM_TYPE_ENUM_UINT32;

 /*********************************************************
 枚举名    : TRRC_APP_FTM_CHANGE_INFO_TYPE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : 变更上报工程模式类型定义
**********************************************************/
 enum TRRC_APP_FTM_CHANGE_INFO_TYPE_ENUM
 {
     TRRC_APP_FTM_CHANGE_CELL_INFO,      /* RRC驻留小区信息，变化后上报，切换或重选完成后上报 */
     TRRC_APP_FTM_CHANGE_STATE_INFO,     /* RRC状态信息，变化后上报 */

     TRRC_APP_FTM_CHANGE_BUTT
 };
  typedef VOS_UINT32 TRRC_APP_FTM_CHANGE_INFO_TYPE_ENUM_UINT32;

 /*****************************************************************************
  枚举名    : TRRC_APP_PROTOCOL_STATE_ENUM
  协议表格  :
  ASN.1描述 :
  枚举说明  : TRRC协议状态
 *****************************************************************************/
 enum TRRC_APP_PROTOCOL_STATE_ENUM
 {
     TRRC_APP_PROTOCOL_STATE_IDLE                           = 0,                /* IDLE态 */
     TRRC_APP_PROTOCOL_STATE_CELL_PCH                          ,                /* CELL_PCH态 */
     TRRC_APP_PROTOCOL_STATE_URA_PCH                           ,                /* URA_PCH态 */
     TRRC_APP_PROTOCOL_STATE_CELL_FACH                         ,                /* CELL_FACH态 */
     TRRC_APP_PROTOCOL_STATE_CELL_DCH                          ,                /* CELL_DCH态 */

     TRRC_APP_PROTOCOL_STATE_BUTT
 };
 typedef VOS_UINT8 TRRC_APP_PROTOCOL_STATE_ENUM_UINT8;

 /*****************************************************************************
  枚举名     :TRRC_APP_GRR_BANDINDICATOR_ENUM
  协议表格  :
  ASN.1描述   :
  枚举说明 : 2G小区频带指示
 *****************************************************************************/
 enum TRRC_APP_GRR_BANDINDICATOR_ENUM
 {
     TRRC_APP_GRR_BANDINDICATOR_DCS1800       = 0,
     TRRC_APP_GRR_BANDINDICATOR_PCS1900       ,

     TRRC_APP_GRR_BANDINDICATOR_BUTT
 };
 typedef VOS_UINT16    TRRC_APP_GRR_BANDINDICATOR_ENUM_UINT16;

  /*****************************************************************************
  枚举名     :TRRC_APP_RRC_STATE_ENUM
  协议表格  :
  ASN.1描述   :
  枚举说明 : RRC状态，内部状态，非协议状态
 *****************************************************************************/
 enum TRRC_APP_RRC_STATE_ENUM
 {
    TRRC_APP_NUL,   // 0x00  /* TTRRC_APP_CHANGE FOR 注释: 开机前，和丢失覆盖*/
    TRRC_APP_SEL,   // 0x01
    TRRC_APP_IDL,   // 0x02
    TRRC_APP_RES,   // 0x03
    TRRC_APP_ACC,   // 0x04
    TRRC_APP_FCH,   // 0x05
    TRRC_APP_DCH,   // 0x06
    TRRC_APP_PCH,   // 0x07
    TRRC_APP_WCA,   // 0x08
    TRRC_APP_REL,   // 0x09
    TRRC_APP_CNF,   // 0x0A
    TRRC_APP_LIM,   // 0x0b
    TRRC_APP_SNW,   // 0x0c
    TRRC_APP_INACTIVE,//0x0d PDM100 200909 for dual mode
    TRRC_APP_PREDEF,  //PDM100 200909 for SIB16
    TRRC_APP_PEND,    //PDM100 200909 for inter-RAT res and CCO
    TRRC_APP_RATHO,   //PDM100 200909 for inter-RAT HO
    TRRC_APP_WAIT,
    TRRC_APP_POWEROFF
 };
typedef VOS_UINT8 TRRC_APP_RRC_STATE_ENUM_UINT8;
/*****************************************************************************
 枚举名    : TRRC_FLOW_CTRL_TYPE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : 流程控制类型枚举
             修改枚举时需要注意，失败类型取值 = 启动类型取值+1
 比如: LRRC_FLOW_CTRL_TYPE_G2L_RESEL_FAIL = LRRC_FLOW_CTRL_TYPE_G2L_START_RESEL + 1
*****************************************************************************/
enum TRRC_APP_FLOW_CTRL_TYPE_ENUM
{
    TRRC_APP_FLOW_CTRL_TYPE_T2T_NORMAL            = 0,     /* TDS内流程边界 */
    TRRC_APP_FLOW_CTRL_TYPE_T2T_NORMAL_IDL_SYS_UPDATA,     /* TDS内流程IDL态系统消息更新测量重配流程 */
    TRRC_APP_FLOW_CTRL_TYPE_T2T_NORMAL_PCH_SYS_UPDATA,     /* TDS内流程PCH态系统消息更新测量重配流程 */
    TRRC_APP_FLOW_CTRL_TYPE_T2T_NORMAL_ENTER_PCH,          /* TDS内流程从其他状态，DCH/FACH->PCH态转换流程 */
    TRRC_APP_FLOW_CTRL_TYPE_T2T_NORMAL_ENTER_FCH,          /* TDS内流程从其他状态，IDL->FCH态转换流程 */
    TRRC_APP_FLOW_CTRL_TYPE_T2T_NORMAL_ENTER_DCH,          /* TDS内流程从其他状态，IDL->DCH态转换流程 */
    TRRC_APP_FLOW_CTRL_TYPE_T2T_RES_ENTER_FCH,             /* TDS内流程重选后进入FCH状态流程 */
    /* TL RESEL PHASE2 begin */
    TRRC_APP_FLOW_CTRL_TYPE_T2T_NAS_SUSPEND,               /* TDS内流程NAS挂起流程 */
    /* TL RESEL PHASE2 end */
    TRRC_APP_FLOW_CTRL_TYPE_T2T_NORMAL_END,                /* TDS内流程边界 */
    TRRC_APP_FLOW_CTRL_TYPE_G2T_START_RESEL,               /* G->T开始重选 */
    TRRC_APP_FLOW_CTRL_TYPE_G2T_RESEL_FAIL,                /* G->T重选失败 */
    TRRC_APP_FLOW_CTRL_TYPE_G2T_STOP_RESEL,                /* G->T停止重选 */

    TRRC_APP_FLOW_CTRL_TYPE_T2G_START_RESEL,               /* T->G开始重选 */
    TRRC_APP_FLOW_CTRL_TYPE_T2G_RESEL_FAIL,                /* T->G重选失败 */

    TRRC_APP_FLOW_CTRL_TYPE_G2T_MEAS,                      /* G->T 测量配置 */
    /* TL RESEL BEGIN */
    TRRC_APP_FLOW_CTRL_TYPE_L2T_START_RESEL,               /* L->T开始重选 */
    TRRC_APP_FLOW_CTRL_TYPE_L2T_RESEL_FAIL,                /* L->T重选失败 */
    TRRC_APP_FLOW_CTRL_TYPE_L2T_STOP_RESEL,                /* L->T停止重选 */

    TRRC_APP_FLOW_CTRL_TYPE_L2T_IDLE_MEAS,                 /* L->T IDLE测量配置 */
    TRRC_APP_FLOW_CTRL_TYPE_L2T_CONN_MEAS,                 /* L->T CONN测量配置 */
    TRRC_APP_FLOW_CTRL_TYPE_L2T_RELALL,                    /* L->T RELALL */

    TRRC_APP_FLOW_CTRL_TYPE_T2L_START_RESEL,               /* T->L开始重选 */
    TRRC_APP_FLOW_CTRL_TYPE_T2L_RESEL_FAIL,                /* T->L重选失败 */

	/* TOER_HO BEGIN */
    TRRC_APP_FLOW_CTRL_TYPE_G2T_START_CCO,                 /* G->T开始CCO */
    TRRC_APP_FLOW_CTRL_TYPE_G2T_CCO_FAIL,                  /* G->T CCO失败 */
    TRRC_APP_FLOW_CTRL_TYPE_G2T_STOP_CCO,                  /* G->T停止CCO */

    TRRC_APP_FLOW_CTRL_TYPE_T2G_START_CCO,                 /* T->G开始CCO */
    TRRC_APP_FLOW_CTRL_TYPE_T2G_CCO_FAIL,                  /* T->G CCO失败 */

    TRRC_APP_FLOW_CTRL_TYPE_T2L_START_HO,                  /* T->L开始切换 */
    TRRC_APP_FLOW_CTRL_TYPE_T2L_HO_FAIL,                   /* T->L切换失败 */

    TRRC_APP_FLOW_CTRL_TYPE_L2T_START_HO,                  /* L->T开始HO */
    TRRC_APP_FLOW_CTRL_TYPE_L2T_HO_FAIL,                   /* L->T HO失败 */
    TRRC_APP_FLOW_CTRL_TYPE_L2T_STOP_HO,                   /* L->T停止HO */

    TRRC_APP_FLOW_CTRL_TYPE_T2G_START_HO,                  /* T->G开始切换 */
    TRRC_APP_FLOW_CTRL_TYPE_T2G_HO_FAIL,                   /* T->G切换失败 */

    TRRC_APP_FLOW_CTRL_TYPE_G2T_START_HO,                  /* G->T开始HO */
    TRRC_APP_FLOW_CTRL_TYPE_G2T_HO_FAIL,                   /* G->T HO失败 */
    TRRC_APP_FLOW_CTRL_TYPE_G2T_STOP_HO,                   /* G->T停止HO */
    /* TOER_HO END */

    TRRC_APP_FLOW_CTRL_TYPE_T2T_SYSCFG_SUSPEND,            /* TDS内流程自我挂起流程 */
    /* TL RESEL PHASE2 begin */
    #if 0
    TRRC_APP_FLOW_CTRL_TYPE_T2L_MEAS,                      /* T->L 测量配置 */
    #endif
    /* TL RESEL PHASE2 end */
    /* TL RESEL end */
    /* TL Redir begin */
    TRRC_APP_FLOW_CTRL_TYPE_T2L_REDIR,                     /* T->L重定向 */
    /* TL Redir end */
    LRRC_APP_FLOW_CTRL_TYPE_L2T_REDIR_FAIL,                /* L->W重定向失败 */

    TRRC_APP_FLOW_CTRL_TYPE_L2T_STOP_REDIR,                /* L->T重定向停止 */

    /*准FR特性begin*/
    TRRC_APP_FLOW_CTRL_TYPE_T2G_REDIR,                     /* T->G重定向 */

    TRRC_APP_FLOW_CTRL_TYPE_G2T_REDIR_FAIL,                /* G->T重定向失败 */

    TRRC_APP_FLOW_CTRL_TYPE_G2T_STOP_REDIR,                /* G->T重定向停止 */
    /*准FR特性end*/

    TRRC_APP_FLOW_CTRL_TYPE_BUTT
};
typedef VOS_UINT16 TRRC_APP_FLOW_CTRL_TYPE_ENUM_UINT16;
/*****************************************************************************
 枚举名    : TRRC_APP_EST_SIGNALLING_TYPE
 协议表格  :
 ASN.1描述 :
 枚举说明  : 链路类型
*****************************************************************************/

enum  TRRC_APP_EST_SIGNALLING_TYPE
{
  TRRC_APP_NO_SIGNALLING_CONNECTION = 0,
  TRRC_APP_SIGNALLING_CONNECTION_ESTABLISHING,
  TRRC_APP_SIGNALLING_CONNECTION_ESTABLISHED
};

typedef VOS_UINT32 TRRC_APP_EST_SIGNALLING_TYPE_UINT32;
/*****************************************************************************
 枚举名    : TRRC_APP_EST_CAUSE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : RRMM_EST_REQ_STRU消息中的enEstCause，与25.331协议定义保持一致
*****************************************************************************/
enum TRRC_APP_EST_CAUSE_ENUM
{
    TRRC_APP_EST_CAUSE_ORIGIN_CONVERSAT_CALL             = 0,
    TRRC_APP_EST_CAUSE_ORIGIN_STREAM_CALL                = 1,
    TRRC_APP_EST_CAUSE_ORIGIN_INTERACT_CALL              = 2,
    TRRC_APP_EST_CAUSE_ORIGIN_BACKGROUND_CALL            = 3,
    TRRC_APP_EST_CAUSE_ORIGIN_SUBSCRIB_TRAFFIC_CALL      = 4,
    TRRC_APP_EST_CAUSE_TERMINAT_CONVERSAT_CALL           = 5,
    TRRC_APP_EST_CAUSE_TERMINAT_STREAM_CALL              = 6,
    TRRC_APP_EST_CAUSE_TERMINAT_INTERACT_CALL            = 7,
    TRRC_APP_EST_CAUSE_TERMINAT_BACKGROUND_CALL          = 8,
    TRRC_APP_EST_CAUSE_EMERGENCY_CALL                    = 9,
    TRRC_APP_EST_CAUSE_INTER_RAT_CELL_RESELECT           = 10,
    TRRC_APP_EST_CAUSE_INTER_RAT_CELL_CHANGE_ORDER       = 11,
    TRRC_APP_EST_CAUSE_REGISTRATION                      = 12,
    TRRC_APP_EST_CAUSE_DETACH                            = 13,
    TRRC_APP_EST_CAUSE_ORIGIN_HIGH_PRIORITY_SIGNAL       = 14,
    TRRC_APP_EST_CAUSE_ORIGIN_LOW_PRIORITY_SIGNAL        = 15,
    TRRC_APP_EST_CAUSE_CALL_RE_ESTABLISH                 = 16,
    TRRC_APP_EST_CAUSE_TERMINAT_HIGH_PRIORITY_SIGNAL     = 17,
    TRRC_APP_EST_CAUSE_TERMINAT_LOW_PRIORITY_SIGNAL      = 18,
    TRRC_APP_EST_CAUSE_TERMINAT_CAUSE_UNKNOWN            = 19,
    TRRC_APP_EST_CAUSE_BUTT
};
typedef VOS_UINT32 TRRC_APP_EST_CAUSE_ENUM_UINT32;

/* ErrorLog新增丢网信息begin */
/*****************************************************************************
 枚举名    : TDS_APP_FREQ_BAND_LIST_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : TDS band信息枚举值
*****************************************************************************/
enum TDS_APP_FREQ_BAND_LIST_ENUM
{
    TDS_APP_FREQ_BAND_NONE      = 0x00,
    TDS_APP_FREQ_BAND_A         = 0x01,         /* 频点范围: 9504~9596  10054~10121 */
    TDS_APP_FREQ_BAND_B         = 0x02,         /* 频点范围: 9254~9546  9654~9946 */
    TDS_APP_FREQ_BAND_C         = 0x04,         /* 频点范围: 9554~9646 */
    TDS_APP_FREQ_BAND_D         = 0x08,         /* 频点范围: 12854~13096 */
    TDS_APP_FREQ_BAND_E         = 0x10,         /* 频点范围: 11504~11996 */
    TDS_APP_FREQ_BAND_F         = 0x20,         /* 频点范围: 9404~9596 */
    TDS_APP_FREQ_BAND_BUTT
};
typedef VOS_UINT8  TDS_APP_FREQ_BAND_LIST_ENUM_UINT8;

/*****************************************************************************
 枚举名    : TDS_APP_ERR_LOG_RAT_TYPE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : 接入技术枚举值，根据检视意见GUTL各自定义但保持各制式顺序一致
*****************************************************************************/
enum TDS_APP_ERR_LOG_RAT_TYPE_ENUM
{
    TDS_APP_ERR_LOG_RAT_TYPE_GSM      = 1,      /* GSM 接入技术*/
    TDS_APP_ERR_LOG_RAT_TYPE_WCDMA,             /* WCDMA 接入技术*/
    TDS_APP_ERR_LOG_RAT_TYPE_LTE,               /* LTE 接入技术*/
    TDS_APP_ERR_LOG_RAT_TYPE_TDS,               /* TDS 接入技术*/
    TDS_APP_ERR_LOG_RAT_TYPE_BUTT
};
typedef VOS_UINT8  TDS_APP_ERR_LOG_RAT_TYPE_ENUM_UINT8;

/*****************************************************************************
 枚举名    : TDS_APP_ERR_LOG_AREA_LOST_CAUSE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : 丢网上报原因值
*****************************************************************************/
enum TDS_APP_ERR_LOG_AREA_LOST_CAUSE_ENUM
{
    TDS_APP_ERR_LOG_AREA_LOST_BUTT = 1         /* 当前预留该枚举值*/
};
typedef VOS_UINT8  TDS_APP_ERR_LOG_AREA_LOST_CAUSE_ENUM_UINT8;
/* ErrorLog新增丢网信息end */
/* Seattle Begin */
/*****************************************************************************
 枚举名    : TDS_ERR_LOG_CS_HANDOVER_FAIL_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : 切换失败的原因值
*****************************************************************************/
enum TDS_ERR_LOG_CS_HANDOVER_FAIL_ENUM
{
    ERR_TRRC_GRR_HANDOVER_RESULT_FREQ_NOT_IMPLEMENTED =1,
    ERR_TRRC_GRR_HANDOVER_RESULT_CONFIG_UNKNOW,
    ERR_TRRC_GRR_HANDOVER_RESULT_INVALID_CONFIG,
    ERR_TRRC_GRR_HANDOVER_RESULT_RRC_CONNECT_FAILURE,
    ERR_TRRC_GRR_HANDOVER_RESULT_PROTOCOL_ERROR,
    ERR_TRRC_GRR_HANDOVER_RESULT_UNSUPORTED_CONFIGURATION,
    ERR_TRRC_GRR_HANDOVER_RESULT_PHY_CHANNEL_FAIL,
    ERR_TRRC_GRR_HANDOVER_RESULT_MESSAGE_INVALID,
    ERR_TRRC_GRR_HANDOVER_RESULT_UNSPECIFIC,
    ERR_TRRC_GRR_HANDOVER_RESULT_BUTT
};
typedef VOS_UINT16 TDS_ERR_LOG_CS_HANDOVER_FAIL_ENUM_UINT16;

/*****************************************************************************
 枚举名    : TDS_ERR_LOG_CS_LINK_ERROR_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : 无线链路失败的原因值
*****************************************************************************/
enum TDS_ERR_LOG_CS_LINK_ERROR_ENUM
{
    TDS_CS_LINK_ERROR_T312_TIMEOUT =0,
    TDS_CS_LINK_ERROR_T313_TIMEOUT,
    TDS_CS_LINK_ERROR_BUTT
};
typedef VOS_UINT16 TDS_ERR_LOG_CS_LINK_ERROR_ENUM_UINT16;

/*****************************************************************************
 枚举名     :ERR_LOG_GSM_BAND_INDICATOR_ENUM
 协议表格  :
 ASN.1描述   :
 枚举说明 : GSM小区频段指示枚举
*****************************************************************************/
typedef enum
{
    ERR_LOG_EN_DSC_1800_USED                    = 0,
    ERR_LOG_EN_PDS_1900_USED
}ERR_LOG_GSM_BAND_INDICATOR_ENUM;
typedef VOS_UINT32  ERR_LOG_GSM_BAND_INDICATOR_ENUM_UINT32;

/* Seattle End */
/*****************************************************************************
   5 STRUCT
*****************************************************************************/

/*****************************************************************************
                               工程模式 上报数据结构
*****************************************************************************/

/*****************************************************************************
 结构名    : TRRC_APP_CHANGE_CELL_INFO_STRU
 结构说明  : 变化上报的工程模式结构
*****************************************************************************/
typedef struct
{
    VOS_UINT16                                usUlFreq;          /* 上行中心频点单位:100Khz */
    VOS_UINT16                                usDlFreq;          /* 下行中心频点单位:100Khz */
    VOS_UINT32                                ulCellId;          /* TD-SCDMA小区ID，指RRC层的小区ID，不是物理层的 */
    VOS_UINT16                                usPagingCycle;     /* 寻呼周期128,256,512,1024，单位ms*/
    VOS_UINT16                                usReserved;
}TRRC_APP_CHANGE_CELL_INFO_STRU;

/*****************************************************************************
 结构名    : TRRC_APP_TDS_NCELL_INFO_STRU
 结构说明  : TD-SCDMA同频、异频小区信息结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT16                         usFrequency;      /* TD-SCDMA临小区下行中心频点 单位:100Khz */
    VOS_UINT16                         usCellId;         /* TD-SCDMA临小区小区ID */
    VOS_UINT8                          ucRscp;           /* TD-SCDMA临小区Rscp接收功率，真实值=ucRscp - 116；单位dbm */
    VOS_UINT8                          aucReserved[3];
}TRRC_APP_TDS_NCELL_INFO_STRU;

/*****************************************************************************
 结构名    : TRRC_APP_TDS_NCELL_STRU
 结构说明  : TD-SCDMA同频、异频小区信息结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT32                      ulNCellNumber;
    TRRC_APP_TDS_NCELL_INFO_STRU    stCellMeasInfo[TRRC_APP_MAX_NCELL_NUM];
}TRRC_APP_TDS_NCELL_STRU;

/*****************************************************************************
 结构名    : TRRC_APP_LTE_NCELL_INFO_STRU
 结构说明  : TD-SCDMA异系统LTE小区信息结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usArfcn;
    VOS_UINT16                          usCellId;   /* 物理小区ID */
    VOS_INT16                           sRsrp;      /* 测量结果RSRP */
    VOS_INT16                           sRsrq;      /* 测量结果RSRQ */
}TRRC_APP_LTE_NCELL_INFO_STRU;

/*****************************************************************************
 结构名    : TRRC_APP_LTE_NCELL_STRU
 结构说明  : TD-SCDMA异系统LTE小区信息结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT32                      ulNCellNumber;
    TRRC_APP_LTE_NCELL_INFO_STRU    stCellMeasInfo[TRRC_APP_MAX_NCELL_NUM];
}TRRC_APP_LTE_NCELL_STRU;

/*****************************************************************************
 结构名    : TRRC_APP_GERAN_NCELL_INFO_STRU
 结构说明  : TD-SCDMA异系统GSM小区信息结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT16                                              usArfcn;
    TRRC_APP_GRR_BANDINDICATOR_ENUM_UINT16                  enBandInd;          /* 保持T->G时测量请求消息中的频段指示一致 */
    VOS_INT16                                               sRssi;              /* sRssi 取值范围[-110 * 8,-48 * 8],单位:dbm,精度1/8 */
    VOS_UINT8                                               ucNcc;              /*  移动通讯GSM网络识别参数: 网络色码(NCC) */
    VOS_UINT8                                               ucBcc;              /*  移动通讯GSM网络识别参数: 基站色码(BCC) */
}TRRC_APP_GERAN_NCELL_INFO_STRU;

/*****************************************************************************
 结构名    : TRRC_APP_GERAN_NCELL_STRU
 结构说明  : TD-SCDMA异系统GSM小区信息结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT32                        ulNCellNumber;
    TRRC_APP_GERAN_NCELL_INFO_STRU    stCellMeasInfo[TRRC_APP_MAX_NCELL_NUM];
}TRRC_APP_GERAN_NCELL_STRU;

/*****************************************************************************
 结构名    : TRRC_APP_NCELL_INFO_STRU
 结构说明  : 服务小区的同频、异频以及异系统测量结果
*****************************************************************************/
typedef struct
{
    TRRC_APP_TDS_NCELL_STRU               stIntraFreqInfo;/* 同频邻区信息*/
    TRRC_APP_TDS_NCELL_STRU               stInterFreqInfo;/* 异频邻区信息*/
    TRRC_APP_LTE_NCELL_STRU               stLTENcellInfo; /* LTE异系统邻区*/
    TRRC_APP_GERAN_NCELL_STRU             stGeranNcellInfo; /* GERAN异系统邻区 */
}TRRC_APP_NCELL_INFO_STRU;

/****************************************************************************
 结构名    : TRRC_APP_FTM_CHANGE_INFO_STRU
 结构说明  : 变化上报的工程模式结构
*****************************************************************************/
typedef struct
{
    TRRC_APP_FTM_CHANGE_INFO_TYPE_ENUM_UINT32     ulFtmChangeInfoType;

    union
    {
        TRRC_APP_CHANGE_CELL_INFO_STRU            stCellInfo;
        TRRC_APP_PROTOCOL_STATE_ENUM_UINT8        enTrrcProtocolState;
    }uchangeInfo;
}TRRC_APP_FTM_CHANGE_INFO_STRU;
/*****************************************************************************
 结构名    : APP_RRC_SINR_INFO_STRU
 结构说明  : SINR结构
*****************************************************************************/
typedef struct
{
	VOS_UINT16                                     usModuFormat;
	VOS_UINT16                                     usTbiOut;
}TRRC_APP_CQI_INFO_STRU;
/*****************************************************************************
 结构名    : APP_RRC_SINR_INFO_STRU
 结构说明  : SINR结构
*****************************************************************************/
typedef struct
{
    VOS_INT16                              sSinrDB;
    VOS_UINT16                             usRev;
}TRRC_APP_DPCH_SINR_INFO_STRU;
/*****************************************************************************
 结构名    : TDS_ERRLOG_UL_POWER_STRU
 结构说明  : UE上行发射功率
*****************************************************************************/
typedef struct
{
	VOS_INT16 sDpchPwr;
	VOS_INT16 sEpuchPwr;
}TDS_APP_ERRLOG_UL_POWER_STRU;
/*****************************************************************************
 结构名    : RRC_APP_FTM_PERIOD_INFO
 结构说明  : 周期上报的工程模式结构
*****************************************************************************/
typedef struct
{
    VOS_INT16                           sCellRSCP;
    VOS_UINT16                          usReserved;
    TDS_APP_ERRLOG_UL_POWER_STRU        stUeTxPower;
    TRRC_APP_DPCH_SINR_INFO_STRU        stSINR;             /* SINR  RS_SNR */
    TRRC_APP_CQI_INFO_STRU              stCQI;              /* CQI两个码字 */
    TRRC_APP_NCELL_INFO_STRU            stNcellInfo;        /*邻区信息*/
}TRRC_APP_FTM_PERIOD_INFO_STRU;

/*****************************************************************************
 结构名    : RRC_APP_FTM_INFO_STRU
 结构说明  : 工程模式上报数据结构
*****************************************************************************/
typedef struct
{
    TRRC_APP_FTM_TYPE_ENUM_UINT32             enRrcAppFtmType;

    union
    {
        TRRC_APP_FTM_PERIOD_INFO_STRU         stPeriodInfo;  /* 工程模式下，周期上报内容， Idle状态上报周期目前为DRX周期，Connected状态下周期为1秒 */
        TRRC_APP_FTM_CHANGE_INFO_STRU         stChangInfo;   /* 工程模式下，变更后上报内容 */
    }u;
}TRRC_APP_FTM_INFO_STRU;
/*****************************************************************************
 结构名    : TRRC_OM_FTM_REPROT_CONTENT_STRU
 结构说明  : 工程模式上报数据结构
*****************************************************************************/
typedef struct
{
    /* 子模块ID */
    OM_ERR_LOG_MOUDLE_ID_ENUM_UINT32    ulMsgModuleID;

    /* 00：主卡, 01：副卡 ,10/11:保留  */
    VOS_UINT16                          usModemId;

    VOS_UINT16                          usProjectID;
    VOS_UINT32                          ulLength;

    TRRC_APP_FTM_INFO_STRU               stTrrcFtmInfo;

}TRRC_OM_FTM_REPROT_CONTENT_STRU;

/*****************************************************************************
 结构名    : TRRC_OM_FTM_REPROT_IND_STRU
 结构说明  : 工程模式上报数据结构
*****************************************************************************/
/* 各组件工程模式主动上报内容 各组件 -> OM*/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgName;
    VOS_UINT32                          ulMsgType;
    VOS_UINT32                          ulMsgSN;
    VOS_UINT32                          ulRptlen;      /* 工程模式上报的内容长度,如果ulRptlen为0,aucContent内容长度也为0 */
    TRRC_OM_FTM_REPROT_CONTENT_STRU     stTrrcFtmContent;
}TRRC_OM_FTM_REPROT_IND_STRU;

/* Seattle Begin */
/*****************************************************************************
 结构名     :ERR_LOG_SAVE_CELL_INFO_FOR_GAS_ST
 协议表格  :
 ASN.1描述   :
 结构说明  :GSM小区的信息结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulNCC;
    VOS_UINT32                          ulBCC;
    VOS_UINT32                          ulBcchArcfn;
    VOS_UINT32                          ulFreqBandInd;
    VOS_UINT32                          ulFlag;                                 /* 该标记用于指示lFNOffset、ulTimeAlignmt是否有数据，0:表示无数据 1: 表示有数据*/
    VOS_INT32                           lFNOffset;
    VOS_UINT32                          ulTimeAlignmt;
    VOS_INT16                           sRxlev;                                 /* G小区信号电平，单位是dBm,没有测到信号电平时赋值为RRWRR_INVALID_RXLEV */
    VOS_UINT8                           aucReserve1[2];                         /* 4字节对齐，保留 */

}ERR_LOG_SAVE_CELL_INFO_FOR_GAS_ST;

/*****************************************************************************
 结构名     :TDS_ERR_LOG_CELL_INFO_STRU
 协议表格  :
 ASN.1描述   :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT8                                   ucCellId;                   /*小区ID*/
    VOS_UINT8                                   ucRscp;                     /*能量*/
    VOS_UINT16                                  usFreqId;                   /*工作频点*/
} TDS_ERR_LOG_CELL_INFO_STRU;

/*****************************************************************************
 结构名     :TDS_ERR_LOG_GSM_CELL_INFO_STRU
 协议表格  :
 ASN.1描述   :
 结构说明  :
*****************************************************************************/
typedef struct
{
    ERR_LOG_SAVE_CELL_INFO_FOR_GAS_ST           stRrcSaveInfoForGas;        /* 注意，只有1个小区的信息，与切换接口不同s */
    ERR_LOG_GSM_BAND_INDICATOR_ENUM_UINT32      enBandIndictor;             /* Band指示 */
} TDS_ERR_LOG_GSM_CELL_INFO_STRU;

/*****************************************************************************
 结构名     :TDS_ERR_LOG_EVT_CS_HANDOVER_FAIL_STRU
 协议表格  :
 ASN.1描述   :
 结构说明  :
*****************************************************************************/
typedef struct
{
    TDS_ERR_LOG_CS_HANDOVER_FAIL_ENUM_UINT16    enHandoverFailCause;  /* 切换失败原因值*/
    VOS_UINT8                                   aucReserve[2];        /* 保留位 */
    TDS_ERR_LOG_CELL_INFO_STRU                  stTdsOrginalCell;     /* 切换的TDS源小区信息 */
    TDS_ERR_LOG_GSM_CELL_INFO_STRU              stGsmTargetCell;      /* 切换的GSM目标小区信息*/
}TDS_ERR_LOG_EVT_CS_HANDOVER_FAIL_STRU;

/*****************************************************************************
 结构名     :TDS_ERR_LOG_EVT_CS_HANDOVER_SLOW_STRU
 协议表格  :
 ASN.1描述   :
 结构说明  :
*****************************************************************************/
typedef struct
{
    TDS_ERR_LOG_CELL_INFO_STRU                  stTdsOrginalCell;     /* 切换的TDS源小区信息 */
    TDS_ERR_LOG_GSM_CELL_INFO_STRU              stGsmTargetCell;      /* 切换的GSM 目标小区信息*/
    VOS_UINT32                                  ulHandoverTime;       /* 切换时长*/
}TDS_ERR_LOG_EVT_CS_HANDOVER_SLOW_STRU;

/*****************************************************************************
 结构名     :TDS_ERR_LOG_EVT_CS_HANDOVER_SLOW_STRU
 协议表格  :
 ASN.1描述   :
 结构说明  :
*****************************************************************************/
typedef struct
{
    VOS_UINT32                                  ulStatus;           /*当前状态*/
    TDS_ERR_LOG_CELL_INFO_STRU                  stTdsOrginalCell;   /* 切换的TDS源小区信息 */
} TDS_ERR_LOG_STATUS_INFO_STRU;

/*****************************************************************************
 结构名     :TDS_ERR_LOG_EVT_CS_LINK_ERROR_STRU
 协议表格  :
 ASN.1描述   :
 结构说明  :
*****************************************************************************/
typedef struct
{
    TDS_ERR_LOG_CS_LINK_ERROR_ENUM_UINT16       enRadioLinkErrorCause;      /*无线链路异常原因*/
    VOS_UINT8                                   aucReserve[2];              /* 保留位 */
    TDS_ERR_LOG_STATUS_INFO_STRU                stTdsStatusInfo;            /* TDS 当前状态信息记录*/
}TDS_ERR_LOG_EVT_CS_LINK_ERROR_STRU;
/* Seattle End */

/*****************************************************************************
                               ERROR LOG 上报数据结构
*****************************************************************************/





/*****************************************************************************
 结构名    : RRC_APP_FTM_PERIOD_INFO
 结构说明  : ERROR LOG 数据结构
 ulNextIndex保存下一个错误码索引，例如ulNextIndex = 1，则aenRrcAppErrorCode[0]
 是最新的错误码
 如果ulNextIndex = 0；aenRrcAppErrorCode[0] = TRRC_APP_ERR_CODE_BUTT；则没有保存错误码
*****************************************************************************/
typedef struct
{
    VOS_INT32                        ulNextIndex;
    TRRC_APP_ERROR_CODE_ENUM_UINT8   aenRrcAppErrorCode[TRRC_ERRLOG_CODE_MAX_NUM];
}TRRC_APP_ERROR_CODE_INFO_STRU;
/*****************************************************************************
 结构名    : RRC_APP_RECIVE_MSG_STRU
 结构说明  : RRC 入口消息的信息
*****************************************************************************/
typedef struct
{
    VOS_UINT32                   ulSendPid;    /* 发送任务Pid */
    VOS_UINT32                   ulMsgName;    /* 发送消息ID */
}TRRC_APP_RECIVE_MSG_STRU;
/*****************************************************************************
 结构名    : RRC_APP_RECIVE_MSG_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :ERROR打印信息保存结构
  ulNextIndex保存下一个收到消息索引，例如ulNextIndex = 1，则astReciveMsgInfo[0]
 是最新的收到的消息ID
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulNextIndex;
    TRRC_APP_RECIVE_MSG_STRU            astReciveMsgInfo[TRRC_APP_INFO_NUM];
} TRRC_APP_RECIVE_MSG_INFO_STRU;
/*****************************************************************************
 结构名    : RRC_APP_STATE_INFO_STRU
 结构说明  : ERROR LOG RRC状态信息，包括协议状态，内部状态，异系统过程
*****************************************************************************/
typedef struct
{
    TRRC_APP_PROTOCOL_STATE_ENUM_UINT8          enTrrcProtocolState;   /* RRC协议状态 */
    TRRC_APP_RRC_STATE_ENUM_UINT8               enRrcState;            /* RRC状态，内部状态，非协议状态 */
    TRRC_APP_FLOW_CTRL_TYPE_ENUM_UINT16         enErrcFlowCtrlType;    /* TD-SCDMA发生异系统过程时，异系统过程标识 */
}TRRC_APP_STATE_INFO_STRU;
/*****************************************************************************
 结构名    : TRRC_APP_EST_INFO_STRU
 结构说明  : ERROR LOG RRC状态信息
 enEstSignallingType[0]:CS;enEstSignallingType[1]:ps
*****************************************************************************/
typedef struct
{
    TRRC_APP_EST_SIGNALLING_TYPE_UINT32        enEstSignallingType[2];
    TRRC_APP_EST_CAUSE_ENUM_UINT32             enEstCause;
}TRRC_APP_EST_INFO_STRU;
/* ErrorLog新增丢网信息begin */

/*****************************************************************************
 结构名    : TDS_APP_AREA_LOST_INFO_STRU
 结构说明  : TDS丢网定位信息
*****************************************************************************/
typedef struct
{
    VOS_UINT32                                 ulTimeStamp;     /* 记录丢网的时刻 */
    VOS_UINT32                                 ulMcc;           /* MCC,3 bytes      */
    VOS_UINT32                                 ulMnc;           /* MNC,2 or 3 bytes */
    VOS_UINT16                                 usFrequency;     /* 丢网时服务小区中心频点 单位:100Khz */
    VOS_UINT16                                 usCellId;        /* 丢网时服务小区ID信息 */
    TDS_APP_FREQ_BAND_LIST_ENUM_UINT8          enBandInfo;      /* 丢网时服务小区所属band信息 */
    VOS_INT8                                   ucRscp;          /* 丢网前服务小区测量值,实际值 - 116；单位dbm */
    VOS_INT8                                   ucQrxLevMin;     /* 丢网时服务小区驻留门限*/
    VOS_UINT8                                  ucRac;           /* Rac */
    VOS_UINT16                                 usLac;           /* lac */
    TDS_APP_ERR_LOG_AREA_LOST_CAUSE_ENUM_UINT8 ucSubCause;      /* 记录丢网原因值:暂时保留待后续扩展使用 */
    VOS_UINT8                                  ucResv;          /* 保留 */
}TDS_APP_AREA_LOST_INFO_STRU;

/*****************************************************************************
 结构名    : TRRC_APP_AREA_LOST_INFO_STRU
 结构说明  : TDS丢网定位信息
*****************************************************************************/
typedef struct
{
    VOS_INT32                                  ulNextIndex;                                         /* 记录astTdsArealostInfo中有效数据个数 */
    TDS_APP_AREA_LOST_INFO_STRU                astTdsArealostInfo[TRRC_ERRLOG_AREA_LOST_MAX_NUM];   /* 丢网信息结构 */
    TDS_APP_ERR_LOG_RAT_TYPE_ENUM_UINT8        enRatType;                                           /* 丢网发生的网络制式 */
    VOS_UINT8                                  aucResv[3];
}TRRC_APP_AREA_LOST_INFO_STRU;

/* ErrorLog新增丢网信息end */
/* Seattle Begin */
/*****************************************************************************
 结构名    : TRRC_APP_HO_FAIL_INFO_STRU
 结构说明  : TDS切换失败
*****************************************************************************/
typedef struct
{
    /*VOS_INT32                                  ulNextIndex; */                                         /* 记录astTdsArealostInfo中有效数据个数 */
    TDS_ERR_LOG_EVT_CS_HANDOVER_FAIL_STRU       stTdsHoFailInfo;   /* 丢网信息结构 */
}TRRC_APP_HO_FAIL_INFO_STRU;

/*****************************************************************************
 结构名    : TRRC_APP_HO_EXP_INFO_STRU
 结构说明  : TDS切换超时
*****************************************************************************/
typedef struct
{
    /*VOS_INT32                                  ulNextIndex;*/                                            /* 记录astTdsArealostInfo中有效数据个数 */
    TDS_ERR_LOG_EVT_CS_HANDOVER_SLOW_STRU       stTdsHoTimeExpInfo;   /* 丢网信息结构 */
}TRRC_APP_HO_TIMER_EXP_INFO_STRU;

/*****************************************************************************
 结构名    : TRRC_APP_RL_ERROR_INFO_STRU
 结构说明  : TDS切换超时
*****************************************************************************/
typedef struct
{
    /*VOS_INT32                                 ulNextIndex;*/                                            /* 记录astTdsArealostInfo中有效数据个数 */
    TDS_ERR_LOG_EVT_CS_LINK_ERROR_STRU          stTdsRLErrorInfo;         /* 丢网信息结构 */
}TRRC_APP_RL_ERROR_INFO_STRU;
/* Seattle End */

/*****************************************************************************
 结构名    : RRC_APP_FTM_PERIOD_INFO
 结构说明  : ERROR LOG 数据结构
*****************************************************************************/
typedef struct
{
    TRRC_APP_RECIVE_MSG_INFO_STRU     stRrAppReciveMsgInfo;
    TRRC_APP_STATE_INFO_STRU          stRrcAppStateInfo;
    TRRC_APP_EST_INFO_STRU            stRrcAppEstInfo;
    /* ErrorLog新增丢网信息begin */
    TRRC_APP_AREA_LOST_INFO_STRU      stRrcAppAreaLostInfo;
    /* ErrorLog新增丢网信息end */
    /* Seattle Begin */
    TRRC_APP_HO_FAIL_INFO_STRU                   stHOFailInfo;
    TRRC_APP_HO_TIMER_EXP_INFO_STRU              stHOTimeExpInfo;
    TRRC_APP_RL_ERROR_INFO_STRU                  stRlErrorInfo;
    /* Seattle End */
}TRRC_APP_ERR_LOG_RRC_INFO_STRU;

/*****************************************************************************
 结构名    : TRRC_APP_ERR_LOG_INFO_STRU
 结构说明  : ERROR LOG 数据结构
*****************************************************************************/
typedef struct
{
    TRRC_APP_ERROR_CODE_INFO_STRU    stErrorCodeInfo;
    TRRC_APP_ERR_LOG_RRC_INFO_STRU   stErrorLogInfo;
}TRRC_APP_ERR_LOG_INFO_STRU;
/*****************************************************************************
 结构名    : RRC_APP_FTM_PERIOD_INFO
 结构说明  : ERROR LOG 数据结构
*****************************************************************************/
typedef struct
{
    /* 子模块ID */
    OM_ERR_LOG_MOUDLE_ID_ENUM_UINT32    ulMsgModuleID;

    /* 00：主卡, 01：副卡 ,10/11:保留  */
    VOS_UINT16                          usModemId;

    /*  sub sys,modeule,sub module   暂不使用   */
    VOS_UINT16                          usALMID;

    /* ERR LOG上报级别,
    Warning：0x04代表提示，
    Minor：0x03代表次要
    Major：0x02答标重要
    Critical：0x01代表紧急    */
    VOS_UINT16                          usALMLevel;

    /* 每个ERR LOG都有其类型:
    故障&告警类型
    通信：0x00
    业务质量：0x01
    处理出错：0x02
    设备故障：0x03
    环境故障：0x04    */
    VOS_UINT16                          usALMType;

    VOS_UINT32                          usAlmLowSlice;/*时间戳*/
    VOS_UINT32                          usAlmHighSlice;

    VOS_UINT32                          ulAlmLength;
    TRRC_APP_ERR_LOG_INFO_STRU          stAlmInfo;
}TRRC_OM_ERR_LOG_INFO_STRU;

/*****************************************************************************
 结构名    : OM_ERR_LOG_REPORT_CNF_STRU
 结构说明  : ERROR LOG 数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgName;
    VOS_UINT32                          ulMsgType;
    VOS_UINT32                          ulMsgSN;
    VOS_UINT32                          ulRptlen;      /* 故障内容长度,如果ulRptlen为0,aucContent内容长度也为0 */
    TRRC_OM_ERR_LOG_INFO_STRU           stAppFtmInfo;
} TRRC_OM_ERR_LOG_REPORT_CNF_STRU;

/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/


/*****************************************************************************
  9 OTHERS
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

#endif /* end of TrrcErrLogInterface.h */


