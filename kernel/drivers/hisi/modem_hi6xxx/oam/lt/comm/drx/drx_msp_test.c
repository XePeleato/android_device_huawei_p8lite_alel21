/******************************************************************************

   Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : drx_msp_test.c
  Description     : 
  History         :
     1.y00171698       2013-8-20   Draft Enact

******************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include HeadFile                            
*****************************************************************************/
#include  "vos.h" 
#include  "msp_errno.h"
#include  "msp_drx.h"
#include  "drx_msp_api.h"
#include  "mdrv.h"

#define    THIS_FILE_ID        MSP_FILE_ID_DRX_TEST_C

/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/
extern VOS_UINT32  gbl_drx_msp_vote_status;

extern void drx_msp_global_print(void);

/* yangzhi 2014-07-17 */
#if((VOS_OS_VER == VOS_VXWORKS) || (VOS_OS_VER == VOS_RTOSCK))
/* diag fw */
VOS_VOID st_drx_vote_001(void);
VOS_VOID st_drx_vote_002(void);
VOS_VOID st_drx_vote_003(void);
VOS_VOID st_drx_vote_004(void);

#endif
/* yangzhi 2014-07-17 */
/*****************************************************************************
  3 Function 
*****************************************************************************/

#if(VOS_OS_VER == VOS_LINUX)
/* diag fw */
VOS_VOID st_drx_vote_001(void)
{
	drx_msp_fid_vote_lock(DRX_DIAG_FW_VOTE);
	VOS_TaskDelay(100);
	drx_msp_global_debug();
	VOS_TaskDelay(100);
	drx_msp_fid_vote_unlock(DRX_DIAG_FW_VOTE);
}

/* diag app agent */
VOS_VOID st_drx_vote_002(void)
{
	drx_msp_global_debug();
	VOS_TaskDelay(100);
	drx_msp_fid_vote_lock(DRX_DIAG_APP_AGENT_VOTE);
	drx_msp_global_debug();
	VOS_TaskDelay(100);
	drx_msp_fid_vote_unlock(DRX_DIAG_APP_AGENT_VOTE);
	drx_msp_global_debug();
	VOS_TaskDelay(100);
}
#endif

#if((VOS_OS_VER == VOS_VXWORKS) || (VOS_OS_VER == VOS_RTOSCK))
/* diag fw */
VOS_VOID st_drx_vote_001(void)
{
	/* drx_msp_global_debug(); */
	
	drx_msp_fid_vote_lock(DRX_DIAG_MODEM_AGENT_VOTE);
	VOS_TaskDelay(100);
	drx_msp_global_debug();
	VOS_TaskDelay(100);
	drx_msp_fid_vote_unlock(DRX_DIAG_MODEM_AGENT_VOTE);
	/* drx_msp_global_debug();*/
}

/* diag app agent */
VOS_VOID st_drx_vote_002(void)
{
	drx_msp_global_debug();
	VOS_TaskDelay(10);
	drx_msp_fid_vote_lock(DRX_L4A_VOTE);
	drx_msp_global_debug();
	VOS_TaskDelay(10);
	drx_msp_fid_vote_unlock(DRX_L4A_VOTE);
	drx_msp_global_debug();
	VOS_TaskDelay(10);
}

VOS_VOID st_drx_vote_003(void)
{
	drx_msp_global_debug();
	VOS_TaskDelay(10);
	drx_msp_fid_vote_lock(DRX_FTM_VOTE);
	drx_msp_global_debug();
	VOS_TaskDelay(10);
	drx_msp_fid_vote_unlock(DRX_FTM_VOTE);
	drx_msp_global_debug();
	VOS_TaskDelay(10);
}

VOS_VOID st_drx_vote_004(void)
{
	drx_msp_global_debug();
	VOS_TaskDelay(10);
	drx_msp_fid_vote_lock(DRX_FTM_MAILBOX_VOTE);
	drx_msp_global_debug();
	VOS_TaskDelay(10);
	drx_msp_fid_vote_unlock(DRX_FTM_MAILBOX_VOTE);
	drx_msp_global_debug();
}

#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


