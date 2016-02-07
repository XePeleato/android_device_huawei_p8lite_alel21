/******************************************************************************

    Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : diag_api.h
  Description     : diag_api.c header file
  History         :
     1.w00182550       2012-11-20     Draft Enact
     2. 
******************************************************************************/

#ifndef __DIAG_API_H__
#define __DIAG_API_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile                            
*****************************************************************************/
#include  "vos.h" 
#include  "diag_cmd_param_plugin.h"
#include  "diag_cmd_id.h" 
#include  "msp_diag.h"
#pragma pack(4)

/*****************************************************************************
  2 macro
*****************************************************************************/

/* diag初始化成功且HSO连接上 */
#define  DIAG_IS_CONN_ON            ((g_ulDiagCfgInfo & (DIAG_CFG_INIT | DIAG_CFG_CONN )) == (DIAG_CFG_INIT | DIAG_CFG_CONN ))

/* 允许LT 空口上报 */
#define  DIAG_IS_LT_AIR_ON          ((g_ulDiagCfgInfo & (DIAG_CFG_INIT | DIAG_CFG_CONN | DIAG_CFG_LT_AIR)) == (DIAG_CFG_INIT | DIAG_CFG_CONN | DIAG_CFG_LT_AIR))

/* 允许GU 空口上报 */
#define  DIAG_IS_GU_AIR_ON          ((g_ulDiagCfgInfo & (DIAG_CFG_INIT | DIAG_CFG_CONN | DIAG_CFG_GU_AIR)) == (DIAG_CFG_INIT | DIAG_CFG_CONN | DIAG_CFG_GU_AIR))

/* 允许事件上报 */
#define  DIAG_IS_EVENT_ON           ((g_ulDiagCfgInfo & (DIAG_CFG_INIT | DIAG_CFG_CONN | DIAG_CFG_EVT)) == (DIAG_CFG_INIT | DIAG_CFG_CONN | DIAG_CFG_EVT))


#define ID_WAS_OM_OTA_IND     0XB022
#define  ID_GAS_OM_OTA_IND     0xA025
#define NAS_OM_OTA_IND           0xC103
#define GU_PID_NAS 99

#if(VOS_OS_VER == VOS_WIN32)
#define WUEPS_PID_OM   105
#define UEPS_PID_GAS   128
#define WUEPS_PID_WRR   133
#define WUEPS_PID_USIM   101
#endif


#define WAS_OM_OTA_IND_OFFSET 24
#define GAS_OM_OTA_IND_OFFSET 16 
#define NAS_OM_OTA_OFFSET 24

/*diag printV最大允许字节数,包括前面文件名和行号长度，-1是预留\0结束符*/
#define DIAG_PRINTF_MAX_LEN 	(1000-1)
/*****************************************************************************
  3 Massage Declare 
*****************************************************************************/


/*****************************************************************************
  4 Enum 
*****************************************************************************/
/* 打印级别定义 */
typedef enum
{
    PS_LOG_LEVEL_OFF = 0,   /* 关闭打印     */
    PS_LOG_LEVEL_ERROR,     /* Error级别    */
    PS_LOG_LEVEL_WARNING,   /* Warning级别  */
    PS_LOG_LEVEL_NORMAL,    /* Normal级别   */
    PS_LOG_LEVEL_INFO,      /* Info级别     */
    PS_LOG_LEVEL_BUTT
}PS_LOG_LEVEL_EN;



/*****************************************************************************
   5 STRUCT 
*****************************************************************************/


/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
extern VOS_VOID diag_SocpIndCmdPkt(VOS_UINT8 * pstSocpPackt, VOS_UINT8 ucCmdType, VOS_UINT32 ulCmdParaLen, VOS_UINT16 ulCmdId);
extern VOS_VOID diag_SocpPsLogPkt(VOS_UINT8 * pstSocpPackt, VOS_UINT8 ucMsgType, VOS_UINT32 ulCmdParaLen);


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

#endif /* end of diag_api.h */
