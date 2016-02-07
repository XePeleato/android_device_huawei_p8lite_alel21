/******************************************************************************

    Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : bbp_datalog.h
  Description     : bbp_datalog.c header file
  History         :
     1.w00182550       2012-12-20     Draft Enact
     2. 
******************************************************************************/

#ifndef __BBP_DATALOG_H__
#define __BBP_DATALOG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile                            
*****************************************************************************/
#include  "vos.h" 
#include  "diag_common.h" 
#include  "diag_cmd_param_plugin.h"
#include  "lte_sleepflow.h" 
#include  <diag_message_id.h>

#pragma pack(4)

/*****************************************************************************
  2 macro
*****************************************************************************/
#define BBP_DATA_SAMPLE_TIMEROUT_VALUE 5
#define BBP_LOG_SAMPLE_TIMEOUT_VALUE 1000

#define BBP_LOG_SAMPLE_MAX_CHAN_NUM 8


/*****************************************************************************
  3 Massage Declare 
*****************************************************************************/


/*****************************************************************************
  4 Enum 
*****************************************************************************/
enum ENUM_BBP_REPORT_TYPE
{
   BBP_REPORT_FROM_LAST_BBP_OFF = 0,
   BBP_REPORT_FROM_BBP_ON,
   BBP_REPORT_FROM_BUTT
};

typedef VOS_UINT32 ENUM_BBP_REPORT_TYPE_U32 ; 

/*****************************************************************************
   5 STRUCT 
*****************************************************************************/


/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/
extern VOS_SEM g_ulDataSampleIndSem ;
extern VOS_UINT32 g_blBbpAgentSleepFlag;
extern DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU *g_pDataSampleRecordBuf;
extern DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU *g_pLogSampleRecordBuf[BBP_LOG_SAMPLE_MAX_CHAN_NUM];
extern DIAG_CMD_DRX_REG_WR_IND_STRU *g_pstDataSampleIndVal ;
extern DIAG_CMD_DRX_REG_WR_IND_STRU *g_pstLogSampleIndVal[BBP_LOG_SAMPLE_MAX_CHAN_NUM] ;
extern VOS_BOOL g_blDataSampleIsOver ;
extern HTIMER g_DataSampleTimer;



/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/


/*****************************************************************************
  9 OTHERS
*****************************************************************************/



extern VOS_BOOL bbp_AgentGetSleepFlag(VOS_VOID);
extern VOS_VOID bbp_AgentMsgProc(MsgBlock* pMsgBlock);
extern VOS_UINT32 bbp_AgentMsgProcInit(enum VOS_INIT_PHASE_DEFINE ip);
extern VOS_VOID bbp_AgentSetSleepFlag(VOS_BOOL blAgentSleepFlag);
extern VOS_VOID bbp_AgentTimeOutProcEntry(VOS_VOID* pstPara);
extern VOS_VOID bbp_DataLogIndSelfTask(VOS_VOID);
extern VOS_UINT32 bbp_DataLogInitGlobal( VOS_VOID );
extern VOS_BOOL bbp_DataSampleIsOver(DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU*psDataSample);
extern VOS_UINT32 bbp_DataSampleTimerOutProc(VOS_VOID);
extern VOS_UINT32 bbp_DrxDataSampleStartEntry(DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU *pstDataSample);
extern VOS_UINT32 bbp_DrxDataSampleStopEntry(DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU *pstDataSample);
extern VOS_UINT32 bbp_DrxLogSampleStartEntry(DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU *pstLogSample);
extern VOS_UINT32 bbp_DrxLogSampleStopEntry(DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU *pstLogSample);
extern VOS_VOID bbp_GetDataSampleIndValue(DIAG_CMD_DRX_REG_WR_PARA_STRU * pIndRegVal,DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU*psDataSample);
extern VOS_VOID bbp_GetLogSampleIndValue(DIAG_CMD_DRX_REG_WR_PARA_STRU * pIndRegVal,DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU*pstLogSample);
extern VOS_UINT32 bbp_LogSampleTimerOutProc(VOS_VOID);
extern VOS_UINT32 bbp_ReportDataSampleInd(ENUM_BBP_REPORT_TYPE_U32 enBbpReportFlag);
extern VOS_UINT32 bbp_ReportLogSampleInd(ENUM_BBP_REPORT_TYPE_U32 enBbpReportFlag,VOS_UINT32 ulChanIndex);
extern VOS_UINT32 bbp_SetDataSampleOver(DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU*psDataSample);
extern VOS_UINT32 bbp_SetDataSampleStart(DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU*psDataSample);
extern VOS_UINT32 bbp_SetLogSampleOver(DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU*pstLogSample);
extern VOS_UINT32 bbp_SetLogSampleStart(DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU*pstLogSample);
extern VOS_UINT32 bbp_StartDataSampleCallBack( VOS_VOID );
extern VOS_UINT32 bbp_StartLogSampleCallBack( VOS_VOID );
extern VOS_UINT32 bbp_StopDataSampleCallBack( VOS_VOID );
extern VOS_UINT32 bbp_StopLogSampleCallBack( VOS_VOID );

/* yangzhi 2014-07-17 Begin:*/
extern VOS_UINT32 bbp_RegRead(const VOS_VOID* pReadAdd, VOS_VOID* pOutBuff);
extern VOS_UINT32 bbp_RegWrite(const VOS_VOID* pInBuff, VOS_VOID* pWriteAdd);
/* yangzhi 2014-07-17 End!*/

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

#endif /* end of bbp_datalog.h */
