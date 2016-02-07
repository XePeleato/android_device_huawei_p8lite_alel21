

#ifndef _USIMM_API_H_
#define _USIMM_API_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**/
/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "UsimPsApi.h"
#include "UsimPsInterface.h"
#include "NVIM_Interface.h"
#include "NasNvInterface.h"
#include "om.h"

#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/*定义USIMM模块接受的最多消息请求*/
#define USIMM_MAX_MSG_NUM               100

/*定义鉴权数据的长度*/
#define USIMM_AUTH_RAND_LENGTH          16
#define USIMM_AUTH_AUTN_LENGTH          16
#define USIMM_AUTH_IK_LENGTH            16
#define USIMM_AUTH_CK_LENGTH            16
#define USIMM_AUTN_RANDLEN              17
#define USIMM_AUTN_AUTHLEN              17

/*SAT命令类型定义*/
#define USIMM_SATCMD_TAG                0xD0
#define USIMM_CMDDETAIL_TAG             0x81

/*定义快速开机时候的卡文件长度*/
#define USIMM_EF6F62Len                 250
#define USIMM_EF6F7BLen                 12
#define USIMM_EF6F60Len                 170
#define USIMM_EF6F61Len                 250
#define USIMM_EF6F31Len                 1
#define USIMM_EF6FADLen                 4
#define USIMM_EF6F38Len                 7
#define USIMM_EF6F7ELen                 11
#define USIMM_EF6F73Len                 14
#define USIMM_EF6F53Len                 11
#define USIMM_EF6F07Len                 9
#define USIMM_EF6F08Len                 33
#define USIMM_EF6F09Len                 33
#define USIMM_EF6F20Len                 2
#define USIMM_EF6F52Len                 8
#define USIMM_EF4F20Len                 9
#define USIMM_EF4F52Len                 9
#define USIMM_EF6FB7Len                 14
#define USIMM_EF6F78Len                 2
#define USIMM_EF6F5BLen                 6
#define USIMM_EF6F5CLen                 3
#define USIMM_EF6FC4Len                 50
#define USIMM_EF6F74Len                 50

/*定义OM透明通道返回的消息参数*/
#define USIMM_OM_GET_CNF                0
#define USIMM_OM_SET_CNF                1
#define USIMM_GET_PRIMID                0xAA01
#define USIMM_SET_PRIMID                0xAA02

#define USIMM_TRANS_PRIMID              0x5001

/* 定义USIM初始化列表长度 */
#define USIMM_INIT_USIM_CARD_NUM        (11)

/* 定义SIM初始化列表长度 */
#define USIMM_INIT_SIM_CARD_NUM         (9)

/* 定义卡类型初始化列表长度 */
#define USIMM_CARD_INIT_TBL_SIZE        (2)

/*******************************************************************************
  3 枚举定义
*******************************************************************************/

#if (OSA_CPU_CCPU == VOS_OSA_CPU)
/*****************************************************************************
 枚举名    : USIMM_CARDSTATUS_IND_ENUM
 枚举说明  : SIM卡插入拔出类型

  1.日    期   : 2013年09月25日
    作    者   : h59254
    修改内容   : V3R3 SIM卡热插拨项目新增
*****************************************************************************/
enum USIMM_CARDSTATUS_IND_ENUM
{
    USIMM_CARDSTATUS_IND_PLUGOUT = SIM_CARD_OUT,
    USIMM_CARDSTATUS_IND_PLUGIN  = SIM_CARD_IN,
    USIMM_CARDSTATUS_IND_BUTT
};

typedef VOS_UINT USIMM_CARDSTATUS_IND_ENUM_UINT32;
#endif

enum USIMM_CMDTYPE_ENUM
{
    USIMM_CMDTYPE_INITSTART             = 0,
    USIMM_CMDTYPE_INITTHIRD             = 1,
    USIMM_CMDTYPE_SETFILE_REQ           = 2,
    USIMM_CMDTYPE_GETFILE_REQ           = 3,
    USIMM_CMDTYPE_AUTH_REQ              = 4,
    USIMM_CMDTYPE_FDN_REQ               = 5,
    USIMM_CMDTYPE_PINHANDLE_REQ         = 6,
    USIMM_CMDTYPE_MAXRECORD_REQ         = 7,
    USIMM_CMDTYPE_REFRESH_REQ           = 8,
    USIMM_CMDTYPE_ENVELOPE_REQ          = 9,
    USIMM_CMDTYPE_TERMINALRSP_REQ       = 10,
    USIMM_CMDTYPE_GACCESS_REQ           = 11,
    USIMM_CMDTYPE_RACCESS_REQ           = 12,
    USIMM_CMDTYPE_SPBFILE_REQ           = 13,
    USIMM_CMDTYPE_PBINIT_IND            = 14,
    USIMM_CMDTYPE_PROTECTRESET_REQ      = 15,
    USIMM_CMDTYPE_SEARCH_REQ            = 16,
    USIMM_CMDTYPE_STATUS_REQ            = 17,
    USIMM_CMDTYPE_DEACTIVE_REQ          = 18,
    USIMM_CMDTYPE_CARDINOUT_IND         = 19,
/* Added by h59254 for V7R1C50 ISDB Project,  2012-8-27 begin */
    USIMM_CMDTYPE_ISDBACC_REQ           = 20,
/* Added by h59254 for V7R1C50 ISDB Project,  2012-8-27 end */
    USIMM_CMDTYPE_OPENCHANNEL_REQ       = 21,
    USIMM_CMDTYPE_CLOSECHANNEL_REQ      = 22,
    USIMM_CMDTYPE_ACCESSCHANNEL_REQ     = 23,
    USIMM_CMDTYPE_INITISIM_REQ          = 24,
    USIMM_CMDTYPE_CARDSTATUS_IND_CB     = 25,
    USIMM_CMDTYPE_SENDTPDU_REQ          = 26,
#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
    USIMM_CMDTYPE_MAMAGESSD_REQ         = 27,
    USIMM_CMDTYPE_BSCHALLENGE_REQ       = 28,
    USIMM_CMDTYPE_GENERATE_KEYVPM_REQ   = 29,
    USIMM_CMDTYPE_CDMA_AUTH_REQ         = 30,
    USIMM_CMDTYPE_SETCFILE_REQ          = 31,
    USIMM_CMDTYPE_GETCFILE_REQ          = 32,
#endif
    USIMM_CMDTYPE_BUTT
};
typedef VOS_UINT16 USIMM_CMDTYPE_ENUM_UINT16;
typedef VOS_UINT32 USIMM_CMDTYPE_ENUM_UINT32;


enum USIMM_PIN_RESULT_ENUM
{
    USIMM_PIN_OK                        = 0,
    USIMM_PIN_ERROR                     = 1,
    USIMM_PIN_BUTT
};
typedef VOS_UINT32      USIMM_PIN_RESULT_ENUM_UINT32;

/*定义紧急呼叫号码类型*/
enum USIMM_ECC_TYPE_ENUM
{
    USIMM_USIM_ECC                      = 0,
    USIMM_SIM_ECC                       = 1,
    USIMM_ECC_TYPE_BUTT
};

enum USIMM_API_MSG_TYPE_ENUM
{
    USIMM_API_MSG_URGENT                = 0,
    USIMM_API_MSG_NORMAL                = 1,
    USIMM_API_MSG_BUTT
};
typedef VOS_UINT32      USIMM_API_MSG_TYPE_ENUM_UINT32;

enum USIMM_EFREADUPDATE_FLAG_ENUM
{
    USIMM_EF_NOTREADABLEORUPDATABLE     = 0,
    USIMM_EF_READABLEANDUPDATABLE       = 1,
    USIMM_EF_BUTT
};
typedef VOS_UINT32      USIMM_EFREADUPDATE_FLAG_ENUM_UINT32;
/*****************************************************************************
  4 全局变量声明
*****************************************************************************/
extern VOS_UINT32       gulUSIMMAPIMessageNum ;

extern VOS_UINT32       gulUSIMMOpneSpeed;

extern VOS_SEM          gulUSIMMApiSmId;

extern const VOS_UINT8  g_aucServIsimToUsim[];

/*****************************************************************************
  5 PV 定义
*****************************************************************************/

/* 互斥相关操作 */
#define USIMM_API_SMP                           VOS_SmP(gulUSIMMApiSmId,0)
#define USIMM_API_SMV                           VOS_SmV(gulUSIMMApiSmId)


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/


typedef struct
{
    VOS_UINT16                      usEfid;
    VOS_UINT16                      usEFLen;
    VOS_UINT8                       ucRecordNum;
    VOS_UINT8                       aucRsv[3];
}USIMM_SETCNF_INFO_STRU;

typedef struct
{
    VOS_UINT16                      usEfId;
    VOS_UINT16                      usEfLen;
    VOS_UINT8                       ucRecordNum;
    VOS_UINT8                       ucTotalNum;
    VOS_UINT16                      usDataLen;
    VOS_UINT8                       *pucEf;
}USIMM_GETCNF_INFO_STRU;

/*****************************************************************************
 结构名    : USIMM_TELECOM_AUTH_INFO_STRU
 结构说明  : 电信鉴权回复信息
 1.日    期   : 2013年07月27日
   作    者   : g47350
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    USIMM_AUTH_RESULT_ENUM_UINT32   enResult;
    VOS_UINT8                      *pucIK;      /* Integrity key */
    VOS_UINT8                      *pucCK;      /* Cipher key */
    VOS_UINT8                      *pucGsmKC;
    VOS_UINT8                      *pucAuts;
    VOS_UINT8                      *pucAuthRes; /* Auth user response */
}USIMM_TELECOM_AUTH_INFO_STRU;

/*****************************************************************************
 结构名    : USIMM_IMS_AUTH_INFO_STRU
 结构说明  : IMS鉴权回复信息
 1.日    期   : 2013年07月27日
   作    者   : g47350
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    USIMM_AUTH_RESULT_ENUM_UINT32   enResult;
    VOS_UINT8                      *pucIK;
    VOS_UINT8                      *pucCK;
    VOS_UINT8                      *pucAuts;
    VOS_UINT8                      *pucAutsRes;
    VOS_UINT8                      *pucKs_ext_NAF;
}USIMM_IMS_AUTH_INFO_STRU;

typedef struct
{
    USIMM_AUTH_RESULT_ENUM_UINT32   enResult;
    VOS_UINT8                      *pucIK;      /* Integrity key */
    VOS_UINT8                      *pucCK;      /* Cipher key */
    VOS_UINT8                      *pucAuthr;
    VOS_UINT8                      *pucAuts;
    VOS_UINT8                      *pucAuthRes; /* Auth user response */
}USIMM_CDMA_AUTH_INFO_STRU;

typedef struct
{
    VOS_UINT16                      usEFId;
    VOS_UINT8                       ucRecordNum;
    VOS_UINT8                       ucRecordLen;
    VOS_UINT32                      ulFileStatus;
    VOS_UINT32                      ulFileReadUpdateFlag;
}USIMM_FILECNF_INFO_STRU;

typedef struct
{
    VOS_UINT8                       ucSW1;
    VOS_UINT8                       ucSW2;
    VOS_UINT8                       ucDataType;
    VOS_UINT8                       ucRsv;
    VOS_UINT32                      ulDataLen;
    VOS_UINT8                       *pucData;
}USIMM_ENVELOPECNF_INFO_STRU;

typedef struct
{
    VOS_UINT32                      ulErrorCode;
    VOS_UINT8                       ucSW1;
    VOS_UINT8                       ucSW2;
    VOS_UINT8                       ucRsv;
    VOS_UINT8                       ucLen;
    VOS_UINT8                       *pContent;
}USIMM_RACCESSCNF_INFO_STRU;

typedef struct
{
    VOS_UINT16                      usEfid;
    VOS_UINT16                      usTotalRecNum;
    VOS_UINT32                      ulLen;
    VOS_UINT8                       *pucData;
}USIMM_SEARCHCNF_INFO_STRU;

typedef struct
{
    VOS_UINT16                      usFileId;
    VOS_UINT16                      usEfLen;
    NV_ID_ENUM_U16                  enNVId;
}USIMM_OPENSPEED_FILE_TO_NVID;

typedef struct
{
    VOS_UINT8                           ucINS;
    VOS_UINT8                           ucRsv;
    VOS_UINT8                           ucSw1;
    VOS_UINT8                           ucSw2;
    VOS_UINT16                          usReserved;
    VOS_UINT16                          usDataLen;
    VOS_UINT8                           *pucData;
}USIMM_CSIM_CNF_STRU;

/*****************************************************************************
 结构名    : USIMM_ISDB_ACCESS_REQ_STRU
 结构说明  : USIM模块透传ISDB的APDU处理结果的回复数据结构
 1.日    期   : 2012年08月28日
   作    者   : h59254
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucSw1;
    VOS_UINT8                           ucSw2;
    VOS_UINT16                          usDataLen;
    VOS_UINT8                          *pucData;
}USIMM_ISDB_ACCESS_CNF_STRU;

/*****************************************************************************
  8 API 下发消息定义
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
}USIMM_MsgBlock;

/*存储符合电话本的相关信息*/
typedef struct
{
    USIMM_APP_TYPE_ENUM_UINT32      enAppType;
    VOS_UINT16                      usFileID;
    VOS_UINT16                      usDataLen;
    VOS_UINT8                       ucRecordNum;
    VOS_UINT8                       aucContent[255];
}USIMM_SET_PB_FILE_REQ;

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
    VOS_UINT32                      ulSPBReqCount;
    USIMM_SET_PB_FILE_REQ           stSPBReq[14];/*最多14个文件*/
}USIMM_SET_SPBFILE_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32           enMsgType;
    USIMM_PB_INIT_STATUS_ENUM_UINT16    enPBInitStatus;
    VOS_UINT16                          usRsv;
}USIMM_PB_INIT_STATUS_IND_STRU;

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
    USIMM_APP_TYPE_ENUM_UINT32      enAppType;
    VOS_UINT32                      ulSendPara;
    VOS_UINT16                      usFileID;
    VOS_UINT16                      usDataLen;
    VOS_UINT8                       ucRecordNum;
    VOS_UINT8                       aucContent[3];
}USIMM_SETFILE_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
    USIMM_APP_TYPE_ENUM_UINT32      enAppType;
    VOS_UINT32                      ulSendPara;
    VOS_UINT16                      usFileID;
    VOS_UINT8                       ucRecordNum;
    VOS_UINT8                       ucRsv;
}USIMM_GETFILE_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
    USIMM_APP_TYPE_ENUM_UINT32      enAppType;
    USIMM_AUTH_TYPE_ENUM_UINT32     enAuthType;
    VOS_UINT8                       ucOpId;
    VOS_UINT8                       aucRsv[3];
    VOS_UINT32                      ulDataLen;
    VOS_UINT8                       aucData[4];
}USIMM_AUTH_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
    USIMM_FBDN_HANDLE_ENUM_UINT32   enFDNHandleType;
    VOS_UINT8                       aucPIN2[USIMM_PINNUMBER_LEN];
}USIMM_FDN_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
    USIMM_PIN_CMD_TYPE_ENUM_UINT32  enCmdType;
    USIMM_PIN_TYPE_ENUM_UINT32      enPINType;
    VOS_UINT8                       aucOldPIN[USIMM_PINNUMBER_LEN];
    VOS_UINT8                       aucNewPIN[USIMM_PINNUMBER_LEN];
}USIMM_PIN_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
    USIMM_APP_TYPE_ENUM_UINT32      enAppType;
    VOS_UINT16                      usEfId;
    VOS_UINT16                      usRev;
}USIMM_MAXRECORD_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
    USIMM_POLL_FCP_ENUM_UINT32      enNeedFcp;
}USIMM_STATUS_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
    VOS_UINT32                      ulCommandNum;
    VOS_UINT32                      ulCommandType;
    USIMM_REFRESH_TYPE_ENUM_UINT32  enRefreshType;
    VOS_UINT32                      ulAidLen;
    VOS_UINT8                       aucAid[USIMM_AID_LEN_MAX];
    VOS_UINT16                      usLen;
    VOS_UINT8                       aucFileList[6];
}USIMM_REFRESH_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
    VOS_UINT32                      ulSendPara;
    VOS_UINT8                       ucDataLen;
    VOS_UINT8                       aucContent[3];
}USIMM_ENVELOPE_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
    VOS_UINT32                      ulSendPara;
    VOS_UINT8                       ucDataLen;
    VOS_UINT8                       aucContent[3];
}USIMM_TERMINALRESPONSE_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
    VOS_UINT32                      ulPathLen;
    VOS_UINT16                      ausPath[USIMM_MAX_PATH_LEN];
    VOS_UINT16                      usDataLen;
    VOS_UINT8                       aucContent[6];
}USIMM_GACCESS_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
    USIMM_CARDAPP_ENUM_UINT32       enAppType;
    USIMM_AUTH_TYPE_ENUM_UINT32     enAuthType;
    VOS_UINT8                       ucOpId;
    VOS_UINT8                       aucRsv[3];
    VOS_UINT32                      ulDataLen;
    VOS_UINT8                       aucData[UICC_AUTH_MSG_PADDING_LEN];
}USIMM_CMDTYPE_AUTHEN_REQ_STRU;

/*****************************************************************************
 结构名    : USIMM_ISDB_ACCESS_REQ_STRU
 结构说明  : USIM模块透传ISDB的APDU的数据结构
 1.日    期   : 2012年08月28日
   作    者   : h59254
   修改内容   : 新建
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32           enMsgType;          /* 消息类型 */
    VOS_UINT16                          usRsv;              /* Rsv */
    VOS_UINT16                          usDataLen;          /* APDU长度 */
    VOS_UINT8                           aucContent[4];      /* APDU的内容 */
}USIMM_ISDB_ACCESS_REQ_STRU;

/*****************************************************************************
 结构名    : USIMM_CHANNEL_INFO_STRU
 结构说明  : USIM模块CHANNEL INFO的数据结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulSessionId;                            /* 会话id */
    VOS_UINT32                          ulChanNum;                              /* 通道号 */
    VOS_UINT32                          ulAIDLen;                               /* AID长度 */
    VOS_UINT8                           aucADFName[2*USIMM_AID_LEN_MAX];        /* AID的内容 */
}USIMM_CHANNEL_INFO_STRU;

/*****************************************************************************
 结构名    : USIMM_CHANNEL_AIDFCPINFO_STRU
 结构说明  : USIM模块CHANNEL AID FCP INFO的数据结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulAIDFCPLen;                            /* AID选中后返回数据长度 */
    VOS_UINT8                           aucADFFcp[USIMM_TPDU_DATA_LEN_MAX];     /* AID的FCP内容 */
}USIMM_CHANNEL_AIDFCPINFO_STRU;

/*****************************************************************************
 结构名    : USIMM_OPEN_CHANNEL_REQ_STRU
 结构说明  : USIM模块OPEN CHANNEL APDU的数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32           enMsgType;                              /* 消息类型 */
    USIMM_CHANNEL_INFO_STRU             stChannelInfo;
}USIMM_OPEN_CHANNEL_REQ_STRU;

/*****************************************************************************
 结构名    : USIMM_CLOSE_CHANNEL_REQ_STRU
 结构说明  : USIM模块CLOSE CHANNEL APDU的数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32           enMsgType;                              /* 消息类型 */
    VOS_UINT32                          ulSessionID;                            /* 通道号 */
}USIMM_CLOSE_CHANNEL_REQ_STRU;

/*****************************************************************************
 结构名    : USIMM_ACCESS_CHANNEL_REQ_STRU
 结构说明  : USIM模块ACCESS CHANNEL APDU的数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32           enMsgType;                              /* 消息类型 */
    VOS_UINT32                          ulSessionID;                            /* 通道号 */
    VOS_UINT32                          ulDataLen;                              /* 透明数据长度 */
    VOS_UINT8                           aucData[4];                             /* 存放数据内容 */
}USIMM_ACCESS_CHANNEL_REQ_STRU;

/*****************************************************************************
 结构名    : USIMM_ACCESS_CHANNEL_CNF_STRU
 结构说明  : USIM模块ACCESS CHANNEL 返回的数据结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucSw1;
    VOS_UINT8                           ucSw2;
    VOS_UINT16                          usDataLen;
    VOS_UINT8                          *pucData;
}USIMM_ACCESS_CHANNEL_CNF_STRU;

/*****************************************************************************
 结构名    : USIMM_SENDTPDU_CNF_STRU
 结构说明  : USIM模块SEND TPDU REQ返回的数据结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucSw1;
    VOS_UINT8                           ucSw2;
    VOS_UINT16                          usDataLen;
    VOS_UINT8                          *pucData;
    VOS_UINT8                          *pucTPDUHead;
}USIMM_SENDTPDU_CNF_STRU;


typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
    VOS_UINT32                      ulSendPara;
    USIMM_RESTRIC_CMD_ENUM_UINT32   enCmdType;
    VOS_UINT16                      usEfId;
    VOS_UINT16                      usPathLen;
    VOS_UINT8                       ucP1;
    VOS_UINT8                       ucP2;
    VOS_UINT8                       ucP3;
    VOS_UINT8                       ucRsv;
    VOS_UINT16                      ausPath[USIMM_MAX_PATH_LEN];
    VOS_UINT8                       aucContent[USIMM_T0_APDU_MAX_LEN+1];
    VOS_UINT8                       aucRsv[3];
}USIMM_RACCESS_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
}USIMM_PROTECT_RESET_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
    USIMM_APP_TYPE_ENUM_UINT32      enAppType;
    VOS_UINT16                      usEfId;
    VOS_UINT8                       ucLen;
    VOS_UINT8                       ucRsv;
    VOS_UINT8                       aucContent[4];
}USIMM_SEARCH_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
}USIMM_DEACTIVE_CARD_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT16       enMsgType;
    VOS_UINT16                      usRsv;
}USIMM_ACTIVE_CARD_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
    VOS_UINT32                      ulCardInOutStatus;
}USIMM_CARD_INOUT_IND_STRU;

#if (OSA_CPU_CCPU == VOS_OSA_CPU)
/*****************************************************************************
 结构名    : USIMM_CARD_STATUS_IND_STRU
 结构说明  : SIM卡注册回调函数发送消息的数据结构

  1.日    期   : 2013年09月25日
    作    者   : h59254
    修改内容   : V3R3 SIM卡热插拨项目新增
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32           enMsgType;
    USIMM_CARDSTATUS_IND_ENUM_UINT32    ulCardStatus;
}USIMM_CARD_STATUS_IND_STRU;
#endif

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
    VOS_UINT32                      ulSessionID;
    USIMM_FILEPATH_INFO_STRU        stFilePath;
    VOS_UINT8                       aucTPDUHead[USIMM_TPDU_HEAD_LEN];
    VOS_UINT8                       aucRsv[3];
    VOS_UINT32                      ulTPDUDataLen;
    VOS_UINT8                       aucTPDUData[4];
}USIMM_SENDTPDU_REQ_STRU;

/*****************************************************************************
 结构名    : USIMM_BS_CHALLENGE_REQ_STRU
 结构说明  : Base station challenge请求结构

  1.日    期   : 2014年07月03日
    作    者   : h59254
    修改内容   : V8R1 UIM卡项目新增
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
    VOS_UINT32                      ulDataLen;
    VOS_UINT8                       aucData[4];
}USIMM_BS_CHALLENGE_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
    USIMM_MANAGESSD_DATA_STRU       stSsdData;
}USIMM_MANAGE_SSD_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER
    USIMM_CMDTYPE_ENUM_UINT32       enMsgType;
    VOS_UINT8                       ucFirstOctet;
    VOS_UINT8                       ucLastOctet;
    VOS_UINT8                       aucRsv[2];
}USIMM_GENERATE_KEYVPM_REQ_STRU;

/***********************************************************
对内API接口
***********************************************************/

extern VOS_UINT32 USIMM_ApiParaCheckSimple(VOS_VOID);

extern VOS_UINT32 USIMM_SendInitCardMsg(VOS_UINT32 ulSenderPid, USIMM_CMDTYPE_ENUM_UINT16 enMsgType);

extern VOS_UINT32 USIMM_Read_OpenSpeedFile(VOS_UINT32 ulSenderPid, VOS_UINT32 ulSendPara, VOS_UINT16 usFileID);

extern VOS_UINT32 USIMM_ApiParaCheck(VOS_VOID);

extern VOS_UINT32 USIMM_ApiSendMsg(USIMM_MsgBlock *pMsg,USIMM_API_MSG_TYPE_ENUM_UINT32 enMsgType);

extern VOS_VOID USIMM_OmFileCnf(VOS_UINT32 ulResult,VOS_UINT16 usEfLen,VOS_UINT8  *pucEf,VOS_UINT8 ucCmdType);

extern VOS_VOID USIMM_MaxRecordNumCnf(VOS_UINT32 ulReceiverPid,VOS_UINT32 ulErrorCode,USIMM_FILECNF_INFO_STRU *pstCnfInfo);

extern VOS_VOID USIMM_PinHandleCnf(VOS_UINT32 ulReceiverPid,USIMM_PIN_CMD_TYPE_ENUM_UINT32 enCmdType,USIMM_PIN_TYPE_ENUM_UINT32 enPINType,VOS_UINT32 ulResult,USIMM_PIN_INFO_STRU *pstPINinfo);

extern VOS_VOID USIMM_SetSPBFileCnf(VOS_UINT32 ulReceiverPid,VOS_UINT32 ulErrorCode,USIMM_SET_SPBFILE_REQ_STRU *pstMsg);

extern VOS_VOID USIMM_SetFileCnf(VOS_UINT32 ulReceiverPid,VOS_UINT32 ulSendPara,VOS_UINT32 ulErrorCode,USIMM_SETCNF_INFO_STRU *pstCnfInfo);

extern VOS_VOID USIMM_GetFileCnf(VOS_UINT32 ulReceiverPid,VOS_UINT32 ulSendPara,VOS_UINT32 ulErrorCode,USIMM_GETCNF_INFO_STRU *pstCnfInfo);

extern VOS_VOID USIMM_RefreshFileInd(VOS_UINT16 usEfIdNum, VOS_UINT16 *pusEfId, USIMM_APP_TYPE_ENUM_UINT32 *penAppType);

extern VOS_VOID USIMM_ResetCnf(VOS_UINT32 ulReceiverPid,USIMM_REFRESH_TYPE_ENUM_UINT32 enRefreshType,VOS_UINT32 ulResult, VOS_UINT16 usSATLen);

extern VOS_VOID USIMM_TelecomAuthCnf(USIMM_AUTH_REQ_STRU *pstMsg, USIMM_TELECOM_AUTH_INFO_STRU *pstCnfInfo);

extern VOS_VOID USIMM_ImsAuthCnf(USIMM_AUTH_REQ_STRU *pstMsg, USIMM_IMS_AUTH_INFO_STRU *pstCnfInfo);

extern VOS_VOID USIMM_CardStatusInd(VOS_UINT32 ulReceiverPid,USIMM_CARD_TYPE_ENUM_UINT32 enCardType,USIMM_CARD_SERVIC_ENUM_UINT32 enCardStatus);

extern VOS_VOID USIMM_GenericAccessCnf(VOS_UINT32 ulReceiverPid,VOS_UINT32  ulResult, VOS_UINT16 usPathLen, VOS_UINT16 *pusPath, USIMM_CSIM_CNF_STRU *pstData);

extern VOS_VOID USIMM_RestrictedAccessCnf(VOS_UINT32 ulReceiverPid,VOS_UINT32 ulSendPara,VOS_UINT32 ulResult,USIMM_RACCESSCNF_INFO_STRU *pstCnfInfo);

extern VOS_VOID USIMM_TerminalResponseCnf(VOS_UINT32 ulReceiverPid,VOS_UINT32 ulErrorCode, VOS_UINT32 ulSendPara, VOS_UINT8 ucSW1,VOS_UINT8 ucSW2);

extern VOS_VOID USIMM_EnvelopeCnf(VOS_UINT32 ulReceiverPid, VOS_UINT32 ulSendPara, VOS_UINT32 ulErrorCode, USIMM_ENVELOPECNF_INFO_STRU *pstCnfInfo);

extern VOS_VOID USIMM_SatDataInd(VOS_UINT8   ucCmdType,VOS_UINT16 usDataLen,VOS_UINT8 *pData);

extern VOS_VOID USIMM_VsimReDhNegotiateInd(VOS_VOID);

extern VOS_VOID USIMM_SingleCmdCnf(VOS_UINT32 ulReceiverPid, VOS_UINT32 ulMsgName, VOS_UINT32 ulResult);

extern VOS_VOID USIMM_EccNumberInd(VOS_UINT8 ucEccType,VOS_UINT16 usEfLen,VOS_UINT8 ucRecordNum,VOS_UINT8 *pucData);

extern VOS_VOID USIMM_FDNCnf(VOS_UINT32 ulReceiverPid, VOS_UINT32 ulResult, VOS_UINT32 ulFDNState);

extern VOS_VOID USIMM_PBInitStatusInd(USIMM_PB_INIT_STATUS_ENUM_UINT16 enPBInitStatus);

extern VOS_VOID USIMM_SearchHandleCnf(VOS_UINT32 ulReceiverPid, VOS_UINT32 ulErrorCode, USIMM_SEARCHCNF_INFO_STRU *pstCnfInfo);

#if (OSA_CPU_CCPU == VOS_OSA_CPU)
extern VOS_VOID USIMM_SCICardOutNotify(VOS_UINT ulCardInOutStatus);

extern VOS_VOID USIMM_SciCardStatusIndCbFunc(USIMM_CARDSTATUS_IND_ENUM_UINT32 enCardStatus);
#endif

extern VOS_VOID USIMM_IsdbAccessCnf(VOS_UINT32 ulReceiverPid, VOS_UINT32 ulResult, USIMM_ISDB_ACCESS_CNF_STRU *pstData);

extern VOS_VOID USIMM_OpenChannelCnf(VOS_UINT32 ulReceiverPid, VOS_UINT32 ulResult, VOS_UINT32 ulErrCode, VOS_UINT32 ulChannelId);

extern VOS_VOID USIMM_CloseChannelCnf(VOS_UINT32 ulReceiverPid, VOS_UINT32 ulResult, VOS_UINT32 ulErrCode);

extern VOS_VOID USIMM_AccessChannelCnf(VOS_UINT32 ulReceiverPid, VOS_UINT32 ulResult, VOS_UINT32 ulErrCode, USIMM_ACCESS_CHANNEL_CNF_STRU *pstDataCnf);

extern VOS_VOID USIMM_SendTPDUCnf(VOS_UINT32 ulReceiverPid,VOS_UINT32 ulResult,VOS_UINT32 ulErrCode,USIMM_SENDTPDU_CNF_STRU *pstDataCnf);

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
extern VOS_UINT32 USIMM_CdmaParaCheckSimple(VOS_VOID);

extern VOS_VOID USIMM_CdmaAuthCnf(USIMM_CMDTYPE_AUTHEN_REQ_STRU *pstMsg, USIMM_CDMA_AUTH_INFO_STRU  *pstCnfInfo);

extern VOS_VOID USIMM_BSChallengeCnf(VOS_UINT32 ulReceiverPid, VOS_UINT32 ulResult, VOS_UINT32 ulErrCode, VOS_UINT32 ulDataLen, VOS_UINT8 *pucRandBs);

extern VOS_VOID USIMM_GenerateKeyVpmCnf(VOS_UINT32 lReceiverPid, VOS_UINT32 ulResult,  VOS_UINT32 ulErrCode, VOS_UINT32 ulVpmLen, VOS_UINT8 *pucVpm, VOS_UINT8 *pucKey);

#endif

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

#endif /* end of UsimmApi.h*/

