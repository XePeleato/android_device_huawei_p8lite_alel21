/******************************************************************************

   Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : drx_msp_api.c
  Description     :
  History         :
     1.y00171698       2013-8-20   Draft Enact

******************************************************************************/
/*lint --e{537,713,701,767,830,525,717}*/

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
#if(VOS_OS_VER == VOS_LINUX)
#include  <linux/wakelock.h>
#endif

#define    THIS_FILE_ID        MSP_FILE_ID_DRX_API_C


/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/
VOS_UINT32  gbl_drx_msp_vote_status = 0;

VOS_SEM  g_drx_vote_sem = (VOS_SEM)0;

#if(VOS_OS_VER == VOS_LINUX)
struct wake_lock msp_wakelock;
#endif

static drx_msp_debug_vote_s gbl_drx_cur_vote_status;

/*****************************************************************************
  3 Function
*****************************************************************************/
/*****************************************************************************
 Function Name	 : drx_msp_init
 Description	 : debug func
 Input			 :
 Output 		 : None
 Return 		 : None

 History		 :

*****************************************************************************/
VOS_VOID drx_msp_init(VOS_VOID)
{
#if(VOS_OS_VER == VOS_LINUX)
    wake_lock_init(&msp_wakelock, WAKE_LOCK_SUSPEND, "msp_wakeup");
#endif
	/* set global variable */
	VOS_MemSet(&gbl_drx_cur_vote_status, 0, sizeof(drx_msp_debug_vote_s));
}

/*****************************************************************************
 Function Name   : drx_msp_fid_vote
 Description     : msp fids reject vote to sleep
 Input           :drx_msp_fid_vote_e e_drx_vote
 Output          : None
 Return          : None

 History         :

*****************************************************************************/
VOS_VOID drx_msp_fid_vote_lock(drx_msp_fid_vote_e e_drx_vote)
{
	VOS_INT32 vote_status_temp = 0;
	VOS_INT32 vote_status_bit_shift = 0;

	/* create sem */
	if(0 ==g_drx_vote_sem)
	{
		if ( VOS_OK != VOS_SmBCreate( "DRX", 1, VOS_SEMA4_FIFO, &g_drx_vote_sem))
	    {
	    	vos_printf("%s: create sem failed!\n",__FUNCTION__);
	        return;
	    }
	}

	if(e_drx_vote >=DRX_MSP_VOTE_BUTT)
	{
		vos_printf("%s: e_drx_vote si too larger!\n",__FUNCTION__);
		return ;
	}

	/* modify current status */
	if(VOS_OK != VOS_SmP(g_drx_vote_sem, 0))
    {
    	vos_printf("%s: post sem failed!\n",__FUNCTION__);
        return ;
    }

	/* save current status */
	vote_status_temp = gbl_drx_msp_vote_status;

	vote_status_bit_shift = 0x1<<((VOS_INT32)e_drx_vote);
	set_drx_status_bit(vote_status_bit_shift);

	if((DRX_MSP_VOTE_SLEEP_OK == vote_status_temp) &&
		(DRX_MSP_VOTE_SLEEP_OK != gbl_drx_msp_vote_status))
	{
#if((VOS_OS_VER == VOS_VXWORKS) || (VOS_OS_VER == VOS_RTOSCK))
		mdrv_pm_wake_lock(PWRCTRL_SLEEP_OAM);
#endif
#if(VOS_OS_VER == VOS_LINUX)
        wake_lock(&msp_wakelock);
#endif
        gbl_drx_cur_vote_status.drx_msp_cur_vote_status |= vote_status_bit_shift;
	}

	VOS_SmV(g_drx_vote_sem);

}

/*****************************************************************************
 Function Name   : drx_msp_fid_unvote
 Description     : msp fids vote to sleep
 Input           :drx_msp_fid_vote_e e_drx_vote
 Output          : None
 Return          : None

 History         :

*****************************************************************************/
VOS_VOID drx_msp_fid_vote_unlock(drx_msp_fid_vote_e e_drx_vote)
{
	VOS_INT32 vote_status_temp = 0;
	VOS_UINT32 vote_status_bit_shift = 0;

	if(e_drx_vote >=DRX_MSP_VOTE_BUTT)
	{
		vos_printf("%s: e_drx_vote si too larger!\n",__FUNCTION__);
		return ;
	}

	/* modify current status */
	if(VOS_OK != VOS_SmP(g_drx_vote_sem, 0))
    {
    	vos_printf("%s: post sem failed!\n",__FUNCTION__);
        return ;
    }

	/* save current status */
	vote_status_temp = gbl_drx_msp_vote_status;

	vote_status_bit_shift = 0x1<<((VOS_UINT32)e_drx_vote);
	clear_drx_status_bit(vote_status_bit_shift);

	if((DRX_MSP_VOTE_SLEEP_OK != vote_status_temp) &&
		(DRX_MSP_VOTE_SLEEP_OK == gbl_drx_msp_vote_status))
	{
#if((VOS_OS_VER == VOS_VXWORKS) || (VOS_OS_VER == VOS_RTOSCK))
		mdrv_pm_wake_unlock(PWRCTRL_SLEEP_OAM);
#endif

#if(VOS_OS_VER == VOS_LINUX)
        wake_unlock(&msp_wakelock);
#endif
        gbl_drx_cur_vote_status.drx_msp_cur_vote_status &= (~vote_status_bit_shift);
	}

	VOS_SmV(g_drx_vote_sem);

}

/*****************************************************************************
 Function Name   : drx_msp_global_debug
 Description     : debug func
 Input           :
 Output          : None
 Return          : None

 History         :

*****************************************************************************/
void drx_msp_global_debug(void)
{
	vos_printf("----drx debug info begin: \n");
	vos_printf("gbl_drx_msp_vote_status is 0x%x!\n",gbl_drx_msp_vote_status);
    vos_printf("drx_msp_cur_vote_status is 0x%x!\n",gbl_drx_cur_vote_status.drx_msp_cur_vote_status);
	vos_printf("----drx debug info end!\n");
}

/* module_init(drx_msp_init); */

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


