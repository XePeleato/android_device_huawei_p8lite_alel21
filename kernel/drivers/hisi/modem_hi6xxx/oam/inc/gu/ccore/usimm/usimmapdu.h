

#ifndef __USIMM_APDU_H__
#define __USIMM_APDU_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "usimmbase.h"

#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/*SW check定义*/
#define USIMM_SIMSW_MAXNUMBER      (24)
#define USIMM_USIMSW_MAXNUMBER     (40)

#define USIMM_STORE_ESNMEID_MAX     (8)
#define USIMM_BS_RANDSEED_MAX       (4)

/*******************************************************************************
  3 枚举定义
*******************************************************************************/
/*SW定义值由于返回具体错误原因值需要移出到对外声明头文件中*/
typedef VOS_UINT16      USIMM_SWCHECK_ENUM_UINT16;

enum UICC_STORE_ESN_MEID_P1
{
    UICC_STORE_ESN_P1 = 0x00,
    UICC_STORE_MEID_P1= 0x01,
    UICC_STORE_ESN_MEID_P1_BUTT
};
typedef VOS_UINT8       UICC_STORE_ESN_MEID_P1_UINT8;

/*****************************************************************************
  4 STRUCT定义
*****************************************************************************/

typedef struct
{
    VOS_UINT8                   ucSW1;
    VOS_UINT8                   ucSW2;
    USIMM_SWCHECK_ENUM_UINT16   enResult;
}USIMM_SWCHECK_ST;

/*****************************************************************************
  5 全局变量声明
*****************************************************************************/
extern USIMM_APDU_ST    gstUSIMMAPDU;

extern USIMM_SWCHECK_ST gausSimSWCheck[USIMM_SIMSW_MAXNUMBER];

extern USIMM_SWCHECK_ST gausUsimSWCheck[USIMM_USIMSW_MAXNUMBER];

/*****************************************************************************
  6 函数声明
*****************************************************************************/
extern VOS_UINT32 USIMM_CheckSW(USIMM_APDU_ST *pstUSIMMAPDU);

extern VOS_UINT32 USIMM_ReselectFileAPDU(USIMM_APDU_ST   *pstApduInfo);

extern VOS_UINT32 USIMM_SelectFile_APDU(USIMM_APDU_ST   *pstApduInfo);

extern VOS_UINT32 USIMM_Status_APDU(VOS_UINT8 ucP1,VOS_UINT8 ucP2,VOS_UINT8 ucP3);

extern VOS_UINT32 USIMM_ReadBinary_APDU(VOS_UINT16 usOffset, VOS_UINT8 ucP3);

extern VOS_UINT32 USIMM_UpdateBinary_APDU(VOS_UINT16 usOffset, VOS_UINT8 ucP3, VOS_UINT8* pucData);

extern VOS_UINT32 USIMM_ReadRecord_APDU(VOS_UINT8 ucRecordNum, VOS_UINT8 ucMode, VOS_UINT8 ucLen);

extern VOS_UINT32 USIMM_UpdateRecord_APDU(VOS_UINT8 ucRecordNum, VOS_UINT8 ucMode, VOS_UINT8 ucLen, VOS_UINT8* pucData);

extern VOS_UINT32 USIMM_Search_APDU(VOS_UINT8 ucP1, VOS_UINT8 ucP2, VOS_UINT8 ucP3, VOS_UINT8* pucData);

extern VOS_UINT32 USIMM_Increase_APDU(VOS_UINT8 ucLen, VOS_UINT8* pucData);

extern VOS_UINT32 USIMM_PINVerify_APDU(VOS_UINT8 ucCmdType,VOS_UINT8 ucPINType, VOS_UINT8* pucPINData);

extern VOS_UINT32 USIMM_ChangePIN_APDU(VOS_UINT8 ucPINType, VOS_UINT8* pucOldPIN, VOS_UINT8* pucNewPIN);

extern VOS_UINT32 USIMM_UnblockPIN_APDU(VOS_UINT8 ucPINType, VOS_UINT8* pucPUKData, VOS_UINT8* pucPINData);

extern VOS_UINT32 USIMM_DeactivateFile_APDU(VOS_VOID);

extern VOS_UINT32 USIMM_ActivateFile_APDU(VOS_VOID);

extern VOS_UINT32 USIMM_Authentication_APDU(VOS_UINT8 ucP1, VOS_UINT8 ucMode, VOS_UINT8 *pucData, VOS_UINT32 ulDataLen);

extern VOS_UINT32 USIMM_Envelope_APDU(VOS_UINT8 ucLen, VOS_UINT8* pucData);

extern VOS_UINT32 USIMM_Fetch_APDU(VOS_UINT8 ucLen);

extern VOS_UINT32 USIMM_TerminalResponse_APDU(VOS_UINT8 ucLen, VOS_UINT8* pucData);

extern VOS_UINT32 USIMM_TermimalProfile_APDU(VOS_UINT8 ucLen, VOS_UINT8* pucData);

extern VOS_UINT32 USIMM_FormatCsimApdu(USIMM_GACCESS_REQ_STRU *pstMsg, USIMM_APDU_ST *pstUsimmApdu);

extern VOS_UINT32 USIMM_FormatIsdbApdu(USIMM_ISDB_ACCESS_REQ_STRU *pstMsg, USIMM_APDU_ST *pstUsimmApdu);

extern VOS_UINT32 USIMM_Manage_Channel_APDU(VOS_UINT8 ucP1, VOS_UINT8 ucP2, VOS_UINT8 ucP3);

extern VOS_UINT32 USIMM_Terminal_Capability_APDU(VOS_UINT8 ucP1, VOS_UINT8 ucP2, USIMM_TERMINAL_CAPABILITY_STRU *pstTerminalCapability);

extern VOS_UINT32 USIMM_SelectFileByChannelID_APDU(VOS_UINT8 ucChannelID, USIMM_APDU_ST *pstApduInfo);

extern VOS_UINT32 USIMM_StatusByChannelID_APDU(VOS_UINT8 ucChannelID, VOS_UINT8 ucP1,VOS_UINT8 ucP2,VOS_UINT8 ucP3);

extern VOS_UINT32 USIMM_FormatStandardApdu(VOS_UINT8 *pucData, VOS_UINT16 usLen, USIMM_APDU_ST *pstUsimmApdu);

extern VOS_UINT32 USIMM_T1SendIFSD_APDU(VOS_UINT8 ucData);

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
extern VOS_UINT32 USIMM_StoreESNMEID_APDU(UICC_STORE_ESN_MEID_P1_UINT8 enDataType, VOS_UINT8 *pucData, USIMM_APDU_ST *pstUSIMMAPDU);

extern VOS_UINT32 USIMM_UpdateSsdAPDU(VOS_UINT8  ucLen, VOS_UINT8 *pucData, USIMM_APDU_ST *pstUSIMMAPDU);

extern VOS_UINT32 USIMM_ConfirmSsdAPDU(VOS_UINT8 ucLen, VOS_UINT8 *pucData, USIMM_APDU_ST *pstUSIMMAPDU);

extern VOS_UINT32 USIMM_ManageSsdAPDU(VOS_UINT8 ucCmd, VOS_UINT8 ucLen, VOS_UINT8 *pucData, USIMM_APDU_ST *pstUSIMMAPDU);

extern VOS_UINT32 USIMM_GenerateKeyVpmAPDU(VOS_UINT8 ucLen, VOS_UINT8 *pucData, USIMM_APDU_ST *pstUSIMMAPDU);

extern VOS_UINT32 USIMM_BSChallengeAPDU(VOS_UINT8 *pucRandSeed, USIMM_APDU_ST *pstUSIMMAPDU);
#endif


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

#endif /* end of UsimmAPDU.h*/


