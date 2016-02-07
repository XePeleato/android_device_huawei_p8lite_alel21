/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : MtaRrcInterface.h
  版 本 号   : 初稿
  作    者   : h44270
  生成日期   : 2012年06月01日
  最近修改   :
  功能描述   : MTA与GU接入层之间的接口头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年06月01日
    作    者   : h44270
    修改内容   : 创建文件

 ******************************************************************************/

#ifndef __MTARRCINTERFACE_H__
#define __MTARRCINTERFACE_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "PsTypeDef.h"
#include "AgpsParaDef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define  NMR_MAX_FREQ_NUM                       (3)  /* 上报NMR数据的频点最大个数,W最多只支持三个频点，TD开发时扩充 */
#define  NMR_MAX_CELL_FREQ_NUM                  (32) /* 上报NMR数据的小区测量的最大个数 */
#define  RRC_MTA_UTRAN_NMR                      (0)
#define  RRC_MTA_GSM_NMR                        (1)
#define  RRC_MTA_NMR_MAX_GSM_ELEM_NUM           (15) /* 上报NMR数据的GSM元素的最大个数 */

#define  MTA_WRR_AUTOTEST_MAX_PARA_NUM          (10) /* autotest命令下发参数最大个数 */
#define  WRR_MTA_AUTOTEST_MAX_RSULT_NUM         (20) /* autotest命令上报结果最大个数 */
#define  WRR_MTA_MEANRPT_MAX_CELL_NUM           (8)  /* 一次测量报告中小区最大个数 */
#define  WRR_MTA_MAX_MEANRPT_NUM                (10) /* 上报最近测量报告的最大个数 */

#define  WRR_MTA_MAX_NCELL_NUM                  (8)  /* WAS最大临区个数 */

#define  MTA_LRRC_MSG_TYPE_BASE                 (0x1000)            /* MTA模块与LTE接入层消息基数 */
#define  LRRC_MAX_NCELL_NUM                     (8)

/*network monitor GSM临区 上报最大个数 */
#define NETMON_MAX_GSM_NCELL_NUM                (6)

/*network monitor UTRAN 临区 上报最大个数 */
#define NETMON_MAX_UTRAN_NCELL_NUM              (16)

/*network monitor LTE 临区 上报最大个数 */
#define NETMON_MAX_LTE_NCELL_NUM                (16)



/*****************************************************************************
  3 枚举定义
*****************************************************************************/

enum MTA_RRC_MSG_TYPE_ENUM
{
    /* 消息名称 */                              /* 消息ID */                    /* 备注 */
    ID_RRC_MTA_MSG_POSITION_REQ                    = 0x0001,                    /* _H2ASN_MsgChoice MTA_RRC_POSITION_REQ_STRU */

    ID_MTA_RRC_QRY_NMR_REQ                         = 0x0003,                    /* _H2ASN_MsgChoice MTA_RRC_QRY_NMR_REQ_STRU */

    ID_MTA_RRC_RESEL_OFFSET_CFG_NTF                = 0x0005,                    /* _H2ASN_MsgChoice MTA_RRC_RESEL_OFFSET_CFG_NTF_STRU */

    ID_MTA_WRR_AUTOTEST_QRY_REQ                    = 0x0007,                    /* _H2ASN_MsgChoice MTA_WRR_AUTOTEST_QRY_REQ_STRU     */

    ID_MTA_WRR_CELLINFO_QRY_REQ                    = 0x0009,                    /* _H2ASN_MsgChoice MTA_WRR_CELLINFO_QRY_REQ_STRU     */

    ID_MTA_WRR_MEASRPT_QRY_REQ                     = 0x000B,                    /* _H2ASN_MsgChoice MTA_WRR_MEANRPT_QRY_REQ_STRU      */

    ID_MTA_WRR_FREQLOCK_SET_REQ                    = 0x000D,                    /* _H2ASN_MsgChoice MTA_WRR_FREQLOCK_SET_REQ_STRU     */

    ID_MTA_WRR_RRC_VERSION_SET_REQ                 = 0x000F,                    /* _H2ASN_MsgChoice MTA_WRR_RRC_VERSION_SET_REQ_STRU  */

    ID_MTA_WRR_CELLSRH_SET_REQ                     = 0x0011,                    /* _H2ASN_MsgChoice MTA_WRR_CELLSRH_SET_REQ_STRU      */

    ID_MTA_WRR_FREQLOCK_QRY_REQ                    = 0x0013,                    /* _H2ASN_MsgChoice MTA_WRR_FREQLOCK_QRY_REQ_STRU     */

    ID_MTA_WRR_RRC_VERSION_QRY_REQ                 = 0x0015,                    /* _H2ASN_MsgChoice MTA_WRR_RRC_VERSION_QRY_REQ_STRU  */

    ID_MTA_WRR_CELLSRH_QRY_REQ                     = 0x0017,                    /* _H2ASN_MsgChoice MTA_WRR_CELLSRH_QRY_REQ_STRU      */

    ID_MTA_GRR_NCELL_MONITOR_SET_REQ               = 0x0019,                    /* _H2ASN_MsgChoice MTA_GRR_NCELL_MONITOR_SET_REQ_STRU      */

    ID_MTA_GRR_NCELL_MONITOR_QRY_REQ               = 0x001B,                    /* _H2ASN_MsgChoice MTA_GRR_NCELL_MONITOR_QRY_REQ_STRU      */

    ID_MTA_RRC_JAM_DETECT_REQ                      = 0x001D,                    /* _H2ASN_MsgChoice MTA_RRC_JAM_DETECT_REQ_STRU */

    ID_MTA_RRC_CHECK_FREQ_VALIDITY_REQ             = 0x001F,                    /* _H2ASN_MsgChoice MTA_RRC_CHECK_FREQ_VALIDITY_REQ_STRU */

    ID_MTA_GRR_FREQLOCK_SET_REQ                    = 0x0021,                    /* _H2ASN_MsgChoice MTA_GRR_FREQLOCK_SET_REQ_STRU     */

    ID_MTA_RRC_NETMON_CELL_QRY_REQ                 = 0x0023,
    ID_MTA_GRR_NETMON_CELL_QRY_REQ                 = 0x0025,
    ID_MTA_GRR_NETMON_TA_QRY_REQ                   = 0x0027,

    ID_MTA_RRC_MSG_POSITION_CNF                    = 0x0002,                    /* _H2ASN_MsgChoice MTA_RRC_POSITION_CNF_STRU */

    ID_RRC_MTA_QRY_NMR_CNF                         = 0x0004,                    /* _H2ASN_MsgChoice RRC_MTA_QRY_NMR_CNF_STRU */

    ID_WRR_MTA_AUTOTEST_QRY_CNF                    = 0x0008,                    /* _H2ASN_MsgChoice WRR_MTA_AUTOTEST_QRY_CNF_STRU     */

    ID_WRR_MTA_CELLINFO_QRY_CNF                    = 0x000A,                    /* _H2ASN_MsgChoice WRR_MTA_CELLINFO_QRY_CNF_STRU     */

    ID_WRR_MTA_MEASRPT_QRY_CNF                     = 0x000C,                    /* _H2ASN_MsgChoice WRR_MTA_MEANRPT_QRY_CNF_STRU      */

    ID_WRR_MTA_FREQLOCK_SET_CNF                    = 0x000E,                    /* _H2ASN_MsgChoice WRR_MTA_FREQLOCK_SET_CNF_STRU     */

    ID_WRR_MTA_RRC_VERSION_SET_CNF                 = 0x0010,                    /* _H2ASN_MsgChoice WRR_MTA_RRC_VERSION_SET_CNF_STRU  */

    ID_WRR_MTA_CELLSRH_SET_CNF                     = 0x0012,                    /* _H2ASN_MsgChoice WRR_MTA_CELLSRH_SET_CNF_STRU      */

    ID_WRR_MTA_FREQLOCK_QRY_CNF                    = 0x0014,                    /* _H2ASN_MsgChoice WRR_MTA_FREQLOCK_QRY_CNF_STRU     */

    ID_WRR_MTA_RRC_VERSION_QRY_CNF                 = 0x0016,                    /* _H2ASN_MsgChoice WRR_MTA_RRC_VERSION_QRY_CNF_STRU  */

    ID_WRR_MTA_CELLSRH_QRY_CNF                     = 0x0018,                    /* _H2ASN_MsgChoice WRR_MTA_CELLSRH_QRY_CNF_STRU      */


    ID_GRR_MTA_NCELL_MONITOR_SET_CNF              = 0x001A,                    /* _H2ASN_MsgChoice GRR_MTA_NCELL_MONITOR_SET_CNF_STRU      */

    ID_GRR_MTA_NCELL_MONITOR_QRY_CNF              = 0x001C,                    /* _H2ASN_MsgChoice GRR_MTA_NCELL_MONITOR_QRY_CNF_STRU      */

    ID_GRR_MTA_NCELL_MONITOR_IND                  = 0x001E,                    /* _H2ASN_MsgChoice GRR_MTA_NCELL_MONITOR_IND_STRU      */

    ID_RRC_MTA_JAM_DETECT_CNF                      = 0x0020,                    /* _H2ASN_MsgChoice RRC_MTA_JAM_DETECT_CNF_STRU */
    ID_RRC_MTA_JAM_DETECT_IND                      = 0x0022,                    /* _H2ASN_MsgChoice RRC_MTA_JAM_DETECT_IND_STRU */

    ID_RRC_MTA_CHECK_FREQ_VALIDITY_CNF             = 0x0024,                    /* _H2ASN_MsgChoice RRC_MTA_CHECK_FREQ_VALIDITY_CNF_STRU */

    ID_GRR_MTA_FREQLOCK_SET_CNF                    = 0x0026,                    /* _H2ASN_MsgChoice GRR_MTA_FREQLOCK_SET_CNF_STRU     */

    ID_GRR_MTA_NETMON_CELL_QRY_CNF                 = 0x0028,
    ID_GRR_MTA_NETMON_TA_QRY_CNF                   = 0x002A,
    ID_RRC_MTA_NETMON_CELL_QRY_CNF                 = 0x002C,

    /* 消息方向MTA->TLRRC */
    ID_MTA_LRRC_CELLINFO_QRY_REQ                = MTA_LRRC_MSG_TYPE_BASE + 1,   /* _H2ASN_MsgChoice MTA_LRRC_CELLINFO_QRY_REQ_STRU      */

    ID_MTA_TLRRC_SET_DPDTTEST_FLAG_NTF          = MTA_LRRC_MSG_TYPE_BASE + 3,   /* _H2ASN_MsgChoice MTA_TLRRC_SET_DPDTTEST_FLAG_NTF_STRU      */

    ID_MTA_TLRRC_SET_DPDT_VALUE_NTF             = MTA_LRRC_MSG_TYPE_BASE + 5,   /* _H2ASN_MsgChoice MTA_TLRRC_SET_DPDT_VALUE_NTF_STRU      */

    ID_MTA_TLRRC_QRY_DPDT_VALUE_REQ             = MTA_LRRC_MSG_TYPE_BASE + 7,   /* _H2ASN_MsgChoice MTA_TLRRC_QRY_DPDT_VALUE_REQ_STRU      */

    ID_MTA_LRRC_NETMON_CELL_QRY_REQ             = MTA_LRRC_MSG_TYPE_BASE +11,
    /* 消息方向TLRRC->MTA */
    ID_LRRC_MTA_CELLINFO_QRY_CNF                = MTA_LRRC_MSG_TYPE_BASE + 2,   /* _H2ASN_MsgChoice LRRC_MTA_CELLINFO_QRY_CNF_STRU      */

    ID_TLRRC_MTA_QRY_DPDT_VALUE_CNF              = MTA_LRRC_MSG_TYPE_BASE + 8,   /* _H2ASN_MsgChoice TLRRC_MTA_QRY_DPDT_VALUE_CNF_STRU      */


    /* 消息方向MTA->LRRC/GAS/WRR/TDRRC */
    ID_MTA_RRC_PROTECT_PS_IND                   = MTA_LRRC_MSG_TYPE_BASE + 9,   /* _H2ASN_MsgChoice MTA_RRC_PROTECT_PS_IND_STRU      */
    ID_LRRC_MTA_NETMON_CELL_QRY_CNF             = MTA_LRRC_MSG_TYPE_BASE + 12,
    /*T模FREQLOCK锁频REQ消息*/
    ID_MTA_TDS_FREQLOCK_SET_REQ                 = MTA_LRRC_MSG_TYPE_BASE + 13,  /* _H2ASN_MsgChoice MTA_TDS_FREQLOCK_SET_REQ_STRU      */

    /*T模FREQLOCK锁频CNF消息*/
    ID_TDS_MTA_FREQLOCK_SET_CNF                 = MTA_LRRC_MSG_TYPE_BASE + 14,


    /*L模FREQLOCK锁频REQ消息*/
    ID_MTA_LRRC_FREQLOCK_SET_REQ                = MTA_LRRC_MSG_TYPE_BASE + 15,  /* _H2ASN_MsgChoice MTA_TDS_FREQLOCK_SET_REQ_STRU      */

    /*L模FREQLOCK锁频CNF消息*/
    ID_LRRC_MTA_FREQLOCK_SET_CNF                = MTA_LRRC_MSG_TYPE_BASE + 16,

    ID_MTA_RRC_MSG_TYPE_BUTT
};
typedef VOS_UINT32 MTA_RRC_MSG_TYPE_ENUM_UINT32;


enum MTA_RRC_RESULT_ENUM
{
    MTA_RRC_RESULT_NO_ERROR                     = 0x000000,                     /* 消息处理正常 */
    MTA_RRC_RESULT_ERROR,                                                       /* 消息处理出错 */

    MTA_RRC_RESULT_BUTT
};
typedef VOS_UINT32 MTA_RRC_RESULT_ENUM_UINT32;


enum NMR_UTRAN_MEASURED_TYPE_ENUM
{
    NMR_UTRAN_MEASURED_TYPE_FDD = 0,
    NMR_UTRAN_MEASURED_TYPE_TDD,
    NMR_UTRAN_MEASURED_TYPE_BUTT
};
typedef VOS_UINT32 NMR_UTRAN_MEASURED_TYPE_ENUM_UINT32;


enum GRR_MTA_NCELL_STATE_ENUM
{
    GRR_MTA_NCELL_STATE_NULL    = 0x00,                                         /* 既没有TD的邻区，也没有4G邻区 */
    GRR_MTA_NCELL_STATE_3G      = 0x01,                                         /* 只有TD的邻区 */
    GRR_MTA_NCELL_STATE_4G      = 0x02,                                         /* 只有4G的邻区 */
    GRR_MTA_NCELL_STATE_3G4G    = 0x03,                                         /* 既有TD的邻区，也有4G邻区 */
    GRR_MTA_NCELL_STATE_BUTT

};
typedef VOS_UINT8 GRR_MTA_NCELL_STATE_ENUM_UINT8;


enum MTA_RRC_JAM_MODE_ENUM
{
    MTA_RRC_JAM_MODE_STOP               = 0x00,                                 /* 停止JAM检测 */
    MTA_RRC_JAM_MODE_START,                                                     /* 启动JAM检测 */

    MTA_RRC_JAM_MODE_BUTT
};
typedef VOS_UINT8 MTA_RRC_JAM_MODE_ENUM_UINT8;



enum MTA_RRC_JAM_RESULT_ENUM
{
    MTA_RRC_JAM_RESULT_JAM_DISAPPEARED  = 0x00,                                 /* 干扰不存在 */
    MTA_RRC_JAM_RESULT_JAM_DISCOVERED,                                          /* 干扰存在 */

    MTA_RRC_JAM_RESULT_BUTT
};
typedef VOS_UINT8 MTA_RRC_JAM_RESULT_ENUM_UINT8;


enum MTA_RRC_GSM_BAND_ENUM
{
    MTA_RRC_GSM_BAND_850  = 0x00,
    MTA_RRC_GSM_BAND_900,
    MTA_RRC_GSM_BAND_1800,
    MTA_RRC_GSM_BAND_1900,

    MTA_RRC_GSM_BAND_BUTT
};
typedef VOS_UINT16 MTA_RRC_GSM_BAND_ENUM_UINT16;



enum MTA_RRC_FREQLOCK_TYPE_ENUM
{
    MTA_RRC_FREQLOCK_TYPE_ENUM_FREQ_ONLY     = 1,                               /* 锁频ONLY类型 */
    MTA_RRC_FREQLOCK_TYPE_ENUM_FREQ_COMBINED = 2,                               /* 锁频+主扰码/扰码/物理小区ID */
    MTA_RRC_FREQLOCK_TYPE_ENUM_BUTT
};
typedef VOS_UINT8 MTA_RRC_FREQLOCK_TYPE_ENUM_UINT8;


enum MTA_NETMON_CELL_TYPE_ENUM
{
    MTA_NETMON_SCELL_TYPE               = 0,
    MTA_NETMON_NCELL_TYPE               = 1,
    MTA_NETMON_CELL_TYPE_BUTT
};

typedef VOS_UINT32 MTA_NETMON_CELL_TYPE_ENUM_UINT32;


enum MTA_NETMON_UTRAN_TYPE_ENUM
{
    MTA_NETMON_UTRAN_FDD_TYPE           = 0,
    MTA_NETMON_UTRAN_TDD_TYPE           = 1,
    MTA_NETMON_UTRAN_TYPE_BUTT
};
typedef VOS_UINT32 MTA_NETMON_UTRAN_TYPE_ENUM_UINT32;


enum MTA_NETMON_RESULT_ENUM
{
    MTA_NETMON_RESULT_NO_ERR            = 0,
    MTA_NETMON_RESULT_ERR               = 1,
    MTA_NETMON_RESULT_BUTT
};
typedef VOS_UINT32 MTA_NETMON_RESULT_ENUM_UINT32;


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
/*******************************************************************************
 结构名    : MTA_RRC_POSITION_REQ_STRU
 结构说明  : 接入层发送给GPS芯片的定位请求的参数
 1.日    期   : 2012年06月01日
   作    者   : y00142674,l00128652,h44270
   修改内容   : Added for AGPS
*******************************************************************************/
typedef struct
{
    MSG_HEADER_STRU                             stMsgHeader;                    /*_H2ASN_Skip*/

    VOS_UINT32                                  bitOpAssistData   : 1;
    VOS_UINT32                                  bitOpPosMeas      : 1;
    VOS_UINT32                                  bitOpSpare        : 30;

    PS_BOOL_ENUM_UINT8                          enDeleteAssistDataFlag;         /* 是否需要清除辅助数据的标记 */
    VOS_UINT8                                   aucRsv[3];                      /* 保留位 */
    AGPS_ASSIST_DATA_STRU                       stAssistData;                   /* assist_data,辅助数据 */
    AGPS_MEASURE_REQ_STRU                       stPosMeas;                      /* pos_meas,位置测量请求 */
}MTA_RRC_POSITION_REQ_STRU;

/*******************************************************************************
 结构名    : MTA_RRC_POSITION_CNF_STRU
 结构说明  : GPS的定位回复
 1.日    期   : 2012年06月01日
   作    者   : y00142674,l00128652,h44270
   修改内容   : Added for AGPS
*******************************************************************************/
typedef struct
{
    MSG_HEADER_STRU                                     stMsgHeader;            /*_H2ASN_Skip*/

    VOS_UINT8                                           aucRsv[3];              /* 保留位 */
    AGPS_POSITION_RESULT_TYPE_ENUM_UINT8                enResultTypeChoice;     /* 结果信息类型 */
    union
    {
        AGPS_LOCATION_INFO_STRU                         stLocationInfo;         /* location,位置信息 */
        AGPS_GPS_MEASURE_INFO_STRU                      stGpsMeasInfo;          /* GPS_meas,GPS测量信息 */
        AGPS_POSITION_GPS_ADDITION_ASSIST_REQ_STRU      stAssistDataReq;        /* GPS_assist_req,辅助数据请求 */
        AGPS_POSITION_ERR_STRU                          stPosErr;               /* pos_err,位置错误信息 */
    }u;
}MTA_RRC_POSITION_CNF_STRU;
typedef struct
{
    MSG_HEADER_STRU                             stMsgHeader;                    /*_H2ASN_Skip*/
    PS_BOOL_ENUM_UINT8                          enOffsetEnable;                 /* 0:灭屏；1:亮屏*/
    VOS_UINT8                                   aucRsv[3];                      /* 保留位*/
}MTA_RRC_RESEL_OFFSET_CFG_NTF_STRU;
typedef struct
{
    VOS_UINT32                                  bitOpUarfcn_UL        : 1;
    VOS_UINT32                                  bitOpSpare            : 31;
    VOS_UINT16                                  usUlUarfcn;           /* 上行频点 */
    VOS_UINT16                                  usDlUarfcn;           /* 下行频点 */
}NMR_UTRAN_FREQUENCY_INFO_FDD_STRU;


typedef struct
{
    NMR_UTRAN_MEASURED_TYPE_ENUM_UINT32           enFreqInfoChoice;     /* NMR频率信息类型:0:FDD,1:TDD */
    union
    {
        NMR_UTRAN_FREQUENCY_INFO_FDD_STRU         stFreqInfoFDD;         /* fdd */
        VOS_UINT32                                ulFreqInfoTDD;          /*  TDD */
    }u;

}NMR_UTRAN_FREQUENCY_INFO_STRU;

typedef struct
{
    VOS_UINT32                                  bitOpEcn0      : 1;
    VOS_UINT32                                  bitOpRscp      : 1;
    VOS_UINT32                                  bitOpPathloss  : 1;
    VOS_UINT32                                  bitOpSpare     : 29;

    VOS_UINT16                                  usCellPSC;
    VOS_UINT8                                   ucEcn0;              /* INTEGER(0..63) */
    VOS_UINT8                                   ucRscp;              /*  INTEGER(0..127)*/
    VOS_UINT8                                   ucPathloss;           /* INTEGER(46..173)  */
    VOS_UINT8                                   aucRsv[3];


}NMR_UTRAN_CELL_MEAS_RESULTS_FDD_STRU;
typedef struct
{
    VOS_UINT32                                  bitOpProposedTGSN  : 1;
    VOS_UINT32                                  bitOpRscp          : 1;
    VOS_UINT32                                  bitOpPathloss      : 1;
    VOS_UINT32                                  bitOpTimeslotISCP  : 1;
    VOS_UINT32                                  bitOpSpare         : 28;

    VOS_UINT8                                   ucCellParaID;         /* INTEGER(0..127) */
    VOS_UINT8                                   ucProposedTGSN;       /* INTEGER(0..14)  */
    VOS_UINT8                                   ucRscp;               /* INTEGER(0..127) */
    VOS_UINT8                                   ucPathloss;           /* INTEGER(46..173) */
    VOS_UINT8                                   ucTimeslotISCP;       /* INTEGER(46..173) */
    VOS_UINT8                                   aucRsv[3];

}NMR_UTRAN_CELL_MEAS_RESULTS_TDD_STRU;
typedef struct
{
    VOS_UINT32                                  bitOpCellID               : 1;
    VOS_UINT32                                  bitOpSpare                : 31;

    VOS_UINT32                                  ulCellID;                 /*  */
    NMR_UTRAN_MEASURED_TYPE_ENUM_UINT32         enCellMeasTypeChoice;     /* NMR频率信息类型:FDD,TDD */
    union
    {
        NMR_UTRAN_CELL_MEAS_RESULTS_FDD_STRU    stCellMeasResultsFDD;         /* FDD */
        NMR_UTRAN_CELL_MEAS_RESULTS_TDD_STRU    stCellMeasResultsTDD;         /*  TDD */
    }u;

}NMR_UTRAN_CELL_MEAS_RESULTS_STRU;


typedef struct
{
    VOS_UINT32                                  ulMeasCellNum;            /* (0,32) */
    NMR_UTRAN_CELL_MEAS_RESULTS_STRU            astCellMeasResults[NMR_MAX_CELL_FREQ_NUM];
}NMR_UTRAN_CELL_MEAS_RESULTS_LIST_STRU;
typedef struct
{
    VOS_UINT32                                  bitOpFrequencyInfo        : 1;
    VOS_UINT32                                  bitOpUTRA_CarrierRSSI     : 1;
    VOS_UINT32                                  bitOpCellMeasResultsList  : 1;
    VOS_UINT32                                  bitOpSpare                : 29;

    NMR_UTRAN_FREQUENCY_INFO_STRU               stFrequencyInfo;
    VOS_UINT32                                  ulUTRA_CarrierRSSI; /* 频点的RSSI */
    NMR_UTRAN_CELL_MEAS_RESULTS_LIST_STRU       stCellMeasResultsList;
}NMR_UTRAN_MEASURED_RESULTS_STRU;
typedef struct
{
    VOS_UINT32                                ulFreqNum;            /* (0,8) */
    NMR_UTRAN_MEASURED_RESULTS_STRU           astMeasResults[NMR_MAX_FREQ_NUM];
}RRC_MTA_UTRAN_NMR_STRU;
typedef struct
{
    VOS_UINT16                                usArfcn;            /* 小区绝对频点号, 取值范围[0,1023] */
    VOS_UINT8                                 ucBsic;             /* 小区BSIC, [0,63] */
    VOS_UINT8                                 ucRxlev;            /* 小区测量电平, 已经映射为[0,63]的取值范围的取值 */
}RRC_MTA_GSM_NMR_ELEMENT_STRU;
typedef struct
{
    VOS_UINT32                                ulElemNum;                        /* GSM NMR元素个数 [0,15] */

    /* GSM NMR元素数组, 第一个元素服务小区, 之后的元素是GSM邻区, GSM邻区按照信号强度由高到低排序 */
    RRC_MTA_GSM_NMR_ELEMENT_STRU              astNmrElem[RRC_MTA_NMR_MAX_GSM_ELEM_NUM];
}RRC_MTA_GSM_NMR_STRU;

/*****************************************************************************
 结构名    : MTA_RRC_QRY_NMR_REQ_STRU
 结构说明  : ID_MTA_RRC_QRY_NMR_REQ消息结构
 1.日    期   : 2012年11月23日
   作    者   : s46746
   修改内容   : 创建
*****************************************************************************/
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;          /* 消息头    */     /*_H2ASN_Skip*/
    /* MTA_RRC_MSG_TYPE_ENUM_UINT32        enMsgId; */

    VOS_UINT8                           aucReserve[4];                          /* 预留后续使用 */
} MTA_RRC_QRY_NMR_REQ_STRU;

/*****************************************************************************
 结构名    : RRC_MTA_QRY_NMR_CNF_STRU
 结构说明  : ID_RRC_MTA_QRY_NMR_CNF消息结构
 1.日    期   : 2012年11月23日
   作    者   : s46746
   修改内容   : 创建
*****************************************************************************/
typedef struct
{
    MSG_HEADER_STRU                     MsgHeader;          /* 消息头    */     /*_H2ASN_Skip*/

    /* ulChoice 的宏定义 */
    VOS_UINT32                          ulChoice;
    union
    {
        RRC_MTA_UTRAN_NMR_STRU          stUtranNMRData;
        RRC_MTA_GSM_NMR_STRU            stGsmNMRData;
    }u;
} RRC_MTA_QRY_NMR_CNF_STRU;


typedef struct
{
    VOS_UINT8                           ucCmd;                                  /* 自动化测试命令 */
    VOS_UINT8                           ucParaNum;                              /* 命令参数个数 */
    VOS_UINT16                          usReserve;                              /* 四字节对齐 */
    VOS_UINT32                          aulPara[MTA_WRR_AUTOTEST_MAX_PARA_NUM]; /* 命令参数 */
}MTA_WRR_AUTOTEST_PARA_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    MTA_WRR_AUTOTEST_PARA_STRU          stWrrAutotestPara;
}MTA_WRR_AUTOTEST_QRY_REQ_STRU;


typedef struct
{
    VOS_UINT32                          ulRsltNum;                              /* 查询结果个数 */
    VOS_UINT32                          aulRslt[WRR_MTA_AUTOTEST_MAX_RSULT_NUM];/* 查询结果 */
}WRR_MTA_AUTOTEST_QRY_RSLT_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulResult;
    WRR_MTA_AUTOTEST_QRY_RSLT_STRU      stWrrAutoTestRslt;
} WRR_MTA_AUTOTEST_QRY_CNF_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulSetCellInfo;                          /* 0:查询主小区信息 1:查询W邻区信息 */
} MTA_WRR_CELLINFO_QRY_REQ_STRU;
typedef struct
{
    VOS_UINT16                      usCellFreq;                                 /* 频点 */
    VOS_UINT16                      usPrimaryScramCode;                         /* 扰码 */
    VOS_INT16                       sCpichRscp;                                 /* RSCP值 */
    VOS_INT16                       sCpichEcN0;                                 /* ECN0值 */

}WRR_MTA_WCDMA_CELLINFO_STRU;


typedef struct
{
     VOS_UINT32                     ulCellNum;
     WRR_MTA_WCDMA_CELLINFO_STRU    astWCellInfo[WRR_MTA_MAX_NCELL_NUM];         /*最多支持W 8个邻区的查询*/

} WRR_MTA_WCDMA_CELLINFO_RSLT_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                              ulMsgName;                          /*_H2ASN_Skip*/
    VOS_UINT32                              ulResult;
    WRR_MTA_WCDMA_CELLINFO_RSLT_STRU        stWrrCellInfo;
} WRR_MTA_CELLINFO_QRY_CNF_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;
} MTA_WRR_MEANRPT_QRY_REQ_STRU;
typedef struct
{
    VOS_UINT16                      usEventId;                                  /* 对应的事件类型 */
    VOS_UINT16                      usCellNum;                                  /* 一次测量报告上报小区个数*/
    VOS_UINT16                      ausPrimaryScramCode[WRR_MTA_MEANRPT_MAX_CELL_NUM];/* 扰码 */
}WRR_MTA_MEANRPT_STRU;
typedef struct
{
    VOS_UINT32                      ulRptNum;                                   /* 最近的测量报告次数 */
    WRR_MTA_MEANRPT_STRU            astMeanRptInfo[WRR_MTA_MAX_MEANRPT_NUM];    /* 测量报告上报的小区 */
}WRR_MTA_MEANRPT_RSLT_STRU;



typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulResult;
    WRR_MTA_MEANRPT_RSLT_STRU           stMeanRptRslt;
} WRR_MTA_MEANRPT_QRY_CNF_STRU;


typedef struct
{
    VOS_UINT8                           ucFreqLockEnable;                       /* 是否锁频 0:未锁频 1:锁频 */
    VOS_UINT8                           aucReserved[1];
    VOS_UINT16                          usLockedFreq;                           /* 锁定的频点 */
} MTA_WRR_FREQLOCK_CTRL_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    MTA_WRR_FREQLOCK_CTRL_STRU          stFrelock;
} MTA_WRR_FREQLOCK_SET_REQ_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulResult;
} WRR_MTA_FREQLOCK_SET_CNF_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;
} MTA_WRR_FREQLOCK_QRY_REQ_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulResult;
    MTA_WRR_FREQLOCK_CTRL_STRU          stFreqLockInfo;
} WRR_MTA_FREQLOCK_QRY_CNF_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    VOS_UINT8                           ucRRCVersion;                           /* RRC版本 */
    VOS_UINT8                           aucReserv[3];
} MTA_WRR_RRC_VERSION_SET_REQ_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;
} MTA_WRR_RRC_VERSION_QRY_REQ_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulResult;
} WRR_MTA_RRC_VERSION_SET_CNF_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucRrcVersion;                           /* RRC版本 */
    VOS_UINT8                           aucReserved[3];
} WRR_MTA_RRC_VERSION_QRY_CNF_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    VOS_UINT8                           ucCellSrh;                              /* 搜网模式 0:正常模式 1:不使用先验信息模式 */
    VOS_UINT8                           aucReserve[3];
} MTA_WRR_CELLSRH_SET_REQ_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulResult;
} WRR_MTA_CELLSRH_SET_CNF_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;
} MTA_WRR_CELLSRH_QRY_REQ_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucCellSearchType;                       /* 搜网模式 0:正常模式 1:不使用先验信息模式 */
    VOS_UINT8                           aucReserve[3];
} WRR_MTA_CELLSRH_QRY_CNF_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    VOS_UINT8                           ucSwitch;
    VOS_UINT8                           aucReserved[3];
} MTA_GRR_NCELL_MONITOR_SET_REQ_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;
} MTA_GRR_NCELL_MONITOR_QRY_REQ_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulResult;
} GRR_MTA_NCELL_MONITOR_SET_CNF_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucSwitch;
    GRR_MTA_NCELL_STATE_ENUM_UINT8      enNcellState;
    VOS_UINT8                           aucReserved[2];
} GRR_MTA_NCELL_MONITOR_QRY_CNF_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    GRR_MTA_NCELL_STATE_ENUM_UINT8      enNcellState;
    VOS_UINT8                           aucReserved[3];
} GRR_MTA_NCELL_MONITOR_IND_STRU;


typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                /*_H2ASN_Skip*/
    VOS_UINT8                           aucReserved[4];             /* 保留位 */
}MTA_LRRC_CELLINFO_QRY_REQ_STRU;


typedef struct
{
    VOS_UINT32                          bitOpEarfcn   :1;
    VOS_UINT32                          bitOpRsrp     :1;
    VOS_UINT32                          bitOpRsrq     :1;
    VOS_UINT32                          bitOpTa       :1;
    VOS_UINT32                          bitSpare      :28;
    VOS_UINT32                          ulPhyCellCode;
    VOS_UINT32                          ulEarfcn;
    VOS_UINT32                          ulRsrp;
    VOS_UINT32                          ulRsrq;
    VOS_UINT32                          ulTa;                                   /* 时间提前量，取值范围0~20512 */
}LRRC_CELLINFO_STRU;
typedef struct
{
    VOS_UINT32                          ulTac;
    VOS_UINT32                          ulCellNum;
    LRRC_CELLINFO_STRU                  astCellInfo[LRRC_MAX_NCELL_NUM];
}LRRC_CELLINFO_RSLT_STRU;


typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                /*_H2ASN_Skip*/
    MTA_RRC_RESULT_ENUM_UINT32          enResult;
    LRRC_CELLINFO_RSLT_STRU             stCellInfoRslt;
}LRRC_MTA_CELLINFO_QRY_CNF_STRU;
typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;               /*_H2ASN_Skip*/
    PS_BOOL_ENUM_UINT8                  enProtectFlg;              /* ps 保护标志 */
    VOS_UINT8                           aucRsv[3];                 /* 保留位 */
} MTA_RRC_PROTECT_PS_IND_STRU;
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    VOS_UINT8                           ucFlag;
    VOS_UINT8                           aucReserved[3];
} MTA_TLRRC_SET_DPDTTEST_FLAG_NTF_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulDpdtValue;
} MTA_TLRRC_SET_DPDT_VALUE_NTF_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    VOS_UINT8                           aucReserved[4];
} MTA_TLRRC_QRY_DPDT_VALUE_REQ_STRU;


typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulDpdtValue;
    MTA_RRC_RESULT_ENUM_UINT32          enResult;
} TLRRC_MTA_QRY_DPDT_VALUE_CNF_STRU;


typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /*_H2ASN_Skip*/
    MTA_RRC_JAM_MODE_ENUM_UINT8         enMode;                                 /* JAM设置的模式 */
    VOS_UINT8                           ucMethod;                               /* JAM使用的方法，取值范围[0,1]*/
    VOS_UINT8                           ucThreshold;                            /* 检测需要达到的频点测量值，取值范围:[0,30] */
    VOS_UINT8                           ucFreqNum;                              /* 检测需要达到的频点个数，取值范围:[0,255] */
} MTA_RRC_JAM_DETECT_REQ_STRU;
typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /*_H2ASN_Skip*/
    MTA_RRC_RESULT_ENUM_UINT32          enResult;                               /* JAM设置的结果 */
} RRC_MTA_JAM_DETECT_CNF_STRU;


typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /*_H2ASN_Skip*/
    MTA_RRC_JAM_RESULT_ENUM_UINT8       enResult;                               /* JAM检测的结果 */
    VOS_UINT8                           aucReserved[3];
} RRC_MTA_JAM_DETECT_IND_STRU;


typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /*_H2ASN_Skip*/
    VOS_UINT16                          usFreq;                                 /* 频点值 */
    MTA_RRC_GSM_BAND_ENUM_UINT16        enBand;                                 /* GSM模式下需要带频带 */
} MTA_RRC_CHECK_FREQ_VALIDITY_REQ_STRU;
typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /*_H2ASN_Skip*/
    MTA_RRC_RESULT_ENUM_UINT32          enResult;                               /* MTA_RRC_RESULT_NO_ERROR:频点有效； MTA_RRC_RESULT_ERROR:频点无效*/
} RRC_MTA_CHECK_FREQ_VALIDITY_CNF_STRU;

typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /*_H2ASN_Skip*/
    PS_BOOL_ENUM_UINT8                  enLockFlg;                              /* PS_TRUE:锁定，PS_FALSE:去锁定 */
    VOS_UINT8                           aucReserved[3];                         /* 保留位 */
    VOS_UINT16                          usFreq;                                 /* 频点值 */
    MTA_RRC_GSM_BAND_ENUM_UINT16        enBand;                                 /* GSM频段 */
} MTA_GRR_FREQLOCK_SET_REQ_STRU;
typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /*_H2ASN_Skip*/
    MTA_RRC_RESULT_ENUM_UINT32          enResult;                               /* MTA_RRC_RESULT_NO_ERROR:锁频成功； MTA_RRC_RESULT_ERROR:锁频失败*/
} GRR_MTA_FREQLOCK_SET_CNF_STRU;

/* ========以下是接入层与MTA之间的消息结构体======== */

/**************************network monitor查询临区信息结构体部分**********************************************************/

/* MTA发给接入层的消息，通用，不区分2G/3G/服务小区/邻区 */

typedef struct
{
    VOS_MSG_HEADER                                     /* 消息头*/
    VOS_UINT32                          ulMsgName;     /*消息名称*/
    MTA_NETMON_CELL_TYPE_ENUM_UINT32    enCelltype;    /*0:查询服务小区，1:查询临区*/
}MTA_RRC_NETMON_CELL_QRY_REQ_STRU;


/*GSM 临区结构*/

typedef struct
{
    VOS_UINT32                          bitOpBsic     : 1;
    VOS_UINT32                          bitOpCellID   : 1;
    VOS_UINT32                          bitOpLAC      : 1;
    VOS_UINT32                          bitOpSpare    : 29;
    VOS_UINT32                          ulCellID;       /*小区ID*/
    VOS_UINT16                          usLAC;          /*位置区码*/
    VOS_UINT16                          usAfrcn;        /*频点*/
    VOS_INT16                           sRSSI;          /* 频点的RSSI */
    MTA_RRC_GSM_BAND_ENUM_UINT16        enBand;         /* band 0-3 */
    VOS_UINT8                           ucBsic;         /*小区基站码*/
    VOS_UINT8                           aucReserved[3];
}MTA_NETMON_GSM_NCELL_INFO_STRU;


/*LTE 临区结构,暂时定义的数据结构，根据需要进行调整*/

typedef struct
{
    VOS_UINT32                          ulPID;            /*物理小区ID*/
    VOS_UINT16                          usArfcn;          /*频点*/
    VOS_INT16                           sRSRP;            /* RSRP参考信号接收功率*/
    VOS_INT16                           sRSRQ;            /* RSRQ参考信号接收质量*/
    VOS_INT16                           sRSSI;            /* Receiving signal strength in dbm */
}MTA_NETMON_LTE_NCELL_INFO_STRU;


/*WCDMA 临区结构*/

typedef struct
{
    VOS_UINT16                          usArfcn;          /*频点*/
    VOS_UINT16                          usPSC;            /*主扰码*/
    VOS_INT16                           sECN0;            /*ECN0*/
    VOS_INT16                           sRSCP;            /*RSCP*/
}MTA_NETMON_UTRAN_NCELL_INFO_FDD_STRU;


/*TD_SCDMA 临区结构,暂时定义结构，根据需要继续调整*/

typedef struct
{
    VOS_UINT16                          usArfcn;          /*频点*/
    VOS_UINT16                          usSC;             /*扰码*/
    VOS_UINT16                          usSyncID;         /*下行导频码*/
    VOS_INT16                           sRSCP;            /*RSCP*/
}MTA_NETMON_UTRAN_NCELL_INFO_TDD_STRU;


/*临区数据结构*/

typedef struct
{
    VOS_UINT8                                ucGsmNCellCnt;      /*GSM 临区个数*/
    VOS_UINT8                                ucUtranNCellCnt;    /*WCDMA 临区个数*/
    VOS_UINT8                                ucLteNCellCnt;      /*LTE 临区个数*/
    VOS_UINT8                                ucReserved;
    MTA_NETMON_GSM_NCELL_INFO_STRU           astGsmNCellInfo[NETMON_MAX_GSM_NCELL_NUM];          /*GSM 临区数据结构*/
    MTA_NETMON_UTRAN_TYPE_ENUM_UINT32        enCellMeasTypeChoice;     /* NETMON频率信息类型:FDD,TDD */
    union
    {
        MTA_NETMON_UTRAN_NCELL_INFO_FDD_STRU astFddNCellInfo[NETMON_MAX_UTRAN_NCELL_NUM];         /* FDD临区数据结构 */
        MTA_NETMON_UTRAN_NCELL_INFO_TDD_STRU astTddNCellInfo[NETMON_MAX_UTRAN_NCELL_NUM];         /*  TDD临区数据结构 */
    }u;
    MTA_NETMON_LTE_NCELL_INFO_STRU           astLteNCellInfo[NETMON_MAX_LTE_NCELL_NUM];      /*LTE 临区数据结构*/
}RRC_MTA_NETMON_NCELL_INFO_STRU;


/*********************network monitor查询GSM 小区信息部分************************************************/

typedef struct
{
    VOS_MSG_HEADER                                     /* 消息头 */
    VOS_UINT32                          ulMsgName;     /* 消息名称 */
}MTA_GRR_NETMON_TA_QRY_REQ_STRU;

/*****************************************************************************
 结构名    : GRR_MTA_NETMON_TA_QRY_CNF_STRU
 结构说明  : GAS返回MTA TA查询结果的消息结构体

  1.日    期   : 2014年10月27日
    作    者   : jumingxuan
    修改内容   : 创建
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /* 消息头 */
    VOS_UINT32                          ulMsgName;          /* 消息名称 */
    MTA_NETMON_RESULT_ENUM_UINT32       enResult;           /* 返回结果 */
    VOS_UINT16                          usTa;               /* 返回的TA值 */
    VOS_UINT8                           aucReserved[2];     /* 四字节对齐的保留位 */
}GRR_MTA_NETMON_TA_QRY_CNF_STRU;
typedef struct
{
    VOS_UINT32                          ulMcc;              /* 移动国家码 */
    VOS_UINT32                          ulMnc;              /* 移动网络码 */
    VOS_UINT32                          ulCellID;           /* 小区ID */
    VOS_UINT16                          usLac;              /* 位置区码 */
    VOS_UINT16                          usArfcn;            /* 绝对频点号 */
    VOS_INT16                           sRssi;              /* Receiving signal strength in dbm */
    MTA_RRC_GSM_BAND_ENUM_UINT16        enBand;             /* GSM频段(0-3) */
    VOS_UINT8                           ucBsic;             /* 小区基站码 */
    VOS_UINT8                           ucRxQuality;        /* IDLE态下或者PS数传态下无效,专用态下填充信道质量值，范围[0,7] ,无效值99*/
    VOS_UINT8                           aucReserved[2];     /* 四字节对齐的保留位 */
}GRR_MTA_NETMON_SCELL_INFO_STRU;


typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgName;      /* 消息名称 */
    MTA_NETMON_RESULT_ENUM_UINT32       enResult;
    MTA_NETMON_CELL_TYPE_ENUM_UINT32    enCelltype;     /* 0:查询服务小区，1:查询临区 */
    union
    {
        GRR_MTA_NETMON_SCELL_INFO_STRU  stSCellinfo;    /* GSM下的服务小区信息 */
        RRC_MTA_NETMON_NCELL_INFO_STRU  stNCellinfo;    /* GSM下的邻区信息 */
    }u;
}GRR_MTA_NETMON_CELL_QRY_CNF_STRU;

/******************************network monitor查询UTRAN 小区信息部分****************************************************/


/*FDD 服务小区信息结构*/

typedef struct
{
    VOS_UINT32                          bitOpDRX      : 1;
    VOS_UINT32                          bitOpURA      : 1;
    VOS_UINT32                          bitOpSpare    : 30;
    VOS_UINT32                          ulDrx;          /* Discontinuous reception cycle length */
    VOS_INT16                           sECN0;          /*ECN0*/
    VOS_INT16                           sRSSI;          /* Receiving signal strength in dbm */
    VOS_INT16                           sRSCP;          /*Received Signal Code Power in dBm，接收信号码功率*/
    VOS_UINT16                          usPSC;          /*主扰码*/
    VOS_UINT16                          usURA;          /* UTRAN Registration Area Identity */
    VOS_UINT8                           aucReserved[2];

} MTA_NETMON_UTRAN_SCELL_INFO_FDD_STRU;


/*TDD 服务小区信息结构预留，待确认后填充*/

typedef struct
{
    VOS_UINT32                          ulDrx;              /* Discontinuous reception cycle length */
    VOS_UINT16                          usSC;               /*扰码*/
    VOS_UINT16                          usSyncID;           /*下行导频码*/
    VOS_UINT16                          usRac;              /*RAC*/
    VOS_INT16                           sRSCP;              /*RSCP*/
} MTA_NETMON_UTRAN_SCELL_INFO_TDD_STRU;



 /*UTRAN 服务小区结构*/

typedef struct
{
    VOS_UINT32                                  bitOpCellID  : 1;
    VOS_UINT32                                  bitOpLAC     : 1;
    VOS_UINT32                                  bitOpSpare   : 30;
    VOS_UINT32                                  ulMcc;                  /*移动国家码*/
    VOS_UINT32                                  ulMnc;                  /*移动网络码*/
    VOS_UINT32                                  ulCellID;               /*小区ID*/
    VOS_UINT16                                  usArfcn;                /*频点*/
    VOS_UINT16                                  usLAC;                  /*位置区码*/
    MTA_NETMON_UTRAN_TYPE_ENUM_UINT32           enCellMeasTypeChoice;   /* NETMON频率信息类型:FDD,TDD */
    union
    {
        MTA_NETMON_UTRAN_SCELL_INFO_FDD_STRU    stCellMeasResultsFDD;    /* FDD */
        MTA_NETMON_UTRAN_SCELL_INFO_TDD_STRU    stCellMeasResultsTDD;    /*  TDD */
    }u;
}RRC_MTA_NETMON_UTRAN_SCELL_INFO_STRU;



typedef struct
{
    VOS_MSG_HEADER                                              /* 消息头*/
    VOS_UINT32                                ulMsgName;        /*消息名称*/
    MTA_NETMON_RESULT_ENUM_UINT32             enResult;
    MTA_NETMON_CELL_TYPE_ENUM_UINT32          enCelltype;       /*0:查询服务小区，1:查询临区*/
    union
    {
        RRC_MTA_NETMON_UTRAN_SCELL_INFO_STRU  stSCellinfo;      /* UTRAN下的服务小区信息 */
        RRC_MTA_NETMON_NCELL_INFO_STRU        stNCellinfo;      /* UTRAN下的邻区信息 */
    }u;
}RRC_MTA_NETMON_CELL_INFO_QRY_CNF_STRU;

/*LTE 服务小区结构*/

typedef struct
{
    VOS_UINT32                                  ulMcc;                  /*移动国家码*/
    VOS_UINT32                                  ulMnc;                  /*移动网络码*/
    VOS_UINT32                                  ulCellID;               /*小区ID*/
    VOS_UINT32                                  ulPID;                  /*物理小区ID*/
    VOS_UINT16                                  usArfcn;                /*频点*/
    VOS_UINT16                                  usTAC;
    VOS_INT16                                   sRSRP;
    VOS_INT16                                   sRSRQ;
    VOS_INT16                                   sRSSI;
    VOS_UINT8                                   aucReserved[2];
} MTA_NETMON_EUTRAN_SCELL_INFO_STRU;
typedef struct
{
    VOS_MSG_HEADER                                  /* 消息头 */
    VOS_UINT32                          ulMsgName;   /* 消息名称 */
    MTA_NETMON_CELL_TYPE_ENUM_UINT32    enCelltype;  /* 0：查询服务小区，1：查询临区 */
}MTA_LRRC_NETMON_CELL_QRY_REQ_STRU;
typedef struct
{
    VOS_MSG_HEADER                                              /* 消息头*/
    VOS_UINT32                                ulMsgName;        /*消息名称*/
    MTA_NETMON_RESULT_ENUM_UINT32             enResult;
    MTA_NETMON_CELL_TYPE_ENUM_UINT32          enCelltype;       /*0:查询服务小区，1:查询临区*/
    union
    {
        MTA_NETMON_EUTRAN_SCELL_INFO_STRU     stSCellinfo;      /* LTE下的服务小区信息 */
        RRC_MTA_NETMON_NCELL_INFO_STRU        stNCellinfo;      /* LTE下的邻区信息 */
    }u;
}LRRC_MTA_NETMON_CELL_INFO_QRY_CNF_STRU;
typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /*_H2ASN_Skip*/
    PS_BOOL_ENUM_UINT8                  enLockFlg;                              /* PS_TRUE:锁定，PS_FALSE:去锁定 */
    MTA_RRC_FREQLOCK_TYPE_ENUM_UINT8    ucFreqType;                             /* 1: 锁频,2:频点+扰码 */
    VOS_UINT16                          usFreq;                                 /* 频点值 */
    VOS_UINT32                          ulSc;                                   /* 扰码 */
} MTA_TDS_FREQLOCK_SET_REQ_STRU;
typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                                    /*_H2ASN_Skip*/
    MTA_RRC_RESULT_ENUM_UINT32          enResult;                                       /* MTA_RRC_RESULT_NO_ERROR:锁频成功；MTA_RRC_RESULT_RRC:锁频失败 */
} TDS_MTA_FREQLOCK_SET_CNF_STRU;


typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /*_H2ASN_Skip*/
    PS_BOOL_ENUM_UINT8                  enLockFlg;                              /* PS_TRUE:锁定，PS_FALSE:去锁定 */
    MTA_RRC_FREQLOCK_TYPE_ENUM_UINT8    ucFreqType;                             /* 1: 锁频,2:频点+物理小区ID */
    VOS_UINT16                          usFreq;                                 /* 频点值 */
    VOS_UINT32                          ulPhyCellId;                            /* 物理小区ID */
} MTA_LRRC_FREQLOCK_SET_REQ_STRU;
typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                  /*_H2ASN_Skip*/
    MTA_RRC_RESULT_ENUM_UINT32          enResult;                     /* MTA_RRC_RESULT_NO_ERROR:锁频成功；MTA_RRC_RESULT_RRC:锁频失败 */
} LRRC_MTA_FREQLOCK_SET_CNF_STRU;
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
    MTA_RRC_MSG_TYPE_ENUM_UINT32        enMsgID;                                /*_H2ASN_MsgChoice_Export MTA_RRC_MSG_TYPE_ENUM_UINT32*/
    VOS_UINT8                           aucMsgBlock[4];
    /***************************************************************************
        _H2ASN_MsgChoice_When_Comment          MTA_RRC_MSG_TYPE_ENUM_UINT32
    ****************************************************************************/
}MTA_RRC_MSG_DATA;
/*_H2ASN_Length UINT32*/

typedef struct
{
    VOS_MSG_HEADER
    MTA_RRC_MSG_DATA                    stMsgData;
}MtaRrcInterface_MSG;

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

#endif /* end of MtaRrcInterface.h */

