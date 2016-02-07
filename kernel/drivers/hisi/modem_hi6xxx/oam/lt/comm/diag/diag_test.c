/******************************************************************************

   Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : diag_test.c
  Description     :
  History         :
     1.w00182550       2013-2-4   Draft Enact

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
#include  "diag_common.h"
#include  "diag_debug.h"
#include  "diag_api.h"
#include  "diag_cfg.h"
#if(VOS_OS_VER == VOS_LINUX)
#include  "diag_fw.h"
#endif
#include  "msp_diag.h"
#include  "msp_errno.h"
#include  "SCMProc.h"
#include  "diag_fs_proc.h"
#include  "socp_lfix_chan.h"

#define THIS_FILE_ID  MSP_FILE_ID_DIAG_TEST_C

/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/

/*lint -save -e717*/

/*****************************************************************************
  3 Function
*****************************************************************************/
VOS_UINT32 g_HsoConn = 0;
VOS_UINT32 Diag_VoLTEMessageTest( VOS_UINT32 ulSize);
VOS_VOID Diag_M3_Drx_Mntn(VOS_VOID);

VOS_VOID* diag_HeapAllocD(VOS_UINT32 ulSize);
VOS_VOID* diag_HeapAllocD(VOS_UINT32 ulSize)
{
	VOS_VOID* ret = NULL;

	if((ulSize == 0) || (ulSize > (1024*1024)))
	{
		return NULL;
	}

    ret = (VOS_VOID*)VOS_MemAlloc(MSP_PID_DIAG_FW, (DYNAMIC_MEM_PT), ulSize);

	return ret;
}

VOS_UINT32 Diag_AirMessageTest( VOS_UINT32 ulSize);
VOS_UINT32 Diag_AirMessageTest( VOS_UINT32 ulSize)
{
	DIAG_AIR_MSG_LOG_STRU stRptMessage;
	VOS_UINT8 ucMsg[100] ={1,1,1,1,};
	VOS_UINT32 ret;

	stRptMessage.pData = NULL;
	stRptMessage.ulId = DIAG_ID(PS_PID_ERRC,OS_MSG_UL);
	stRptMessage.ulMessageID = 0x0FC0;
	stRptMessage.ulDestMod = 0xFFFFFFFF;
	stRptMessage.ulSideId = DIAG_SIDE_NET;
	stRptMessage.ulDataSize = ulSize;
	stRptMessage.pData		= ucMsg;

	if(ulSize > 100)
	{
		return ERR_MSP_FAILURE;
	}

	if(g_HsoConn)
	{
		diag_CfgResetAllSwt();

		diag_CfgSetGlobalBitValue(&g_ulDiagCfgInfo,DIAG_CFG_INIT_BIT,DIAG_CFG_SWT_OPEN);

		diag_CfgSetGlobalBitValue(&g_ulDiagCfgInfo,DIAG_CFG_CONN_BIT,DIAG_CFG_SWT_OPEN);

		diag_CfgSetGlobalBitValue(&g_ulDiagCfgInfo,DIAG_CFG_LT_AIR_BIT,DIAG_CFG_SWT_OPEN);
	}
    /*lint -save -e717*/
	diag_printf("DIAG_ReportAirMessageLog !g_ulDiagCfgInfo =%d\n",g_ulDiagCfgInfo);
    /*lint -restore*/
	 ret = DIAG_ReportAirMessageLog(&stRptMessage);
	return ret;
}

VOS_UINT32 Diag_LayerMessageTest( VOS_UINT32 ulSize);
VOS_UINT32 Diag_LayerMessageTest( VOS_UINT32 ulSize)
{
	DIAG_LAYER_MSG_STRU stRptMessage;
	VOS_UINT8 ucMsg[100] ={2,2,2,2,};
	VOS_UINT32 ret;
	DIAG_CMD_LOG_CAT_LAYER_REQ_STRU stLayerReq[5] = {{0},};

	stRptMessage.pData = NULL;
	stRptMessage.ulId = DIAG_ID(150,OS_MSG_UL);
	stRptMessage.ulMessageID = 0x0FC0;
	stRptMessage.ulDestMod = 152;
	stRptMessage.ulSideId = DIAG_SIDE_NET;
	stRptMessage.ulDataSize = ulSize;
	stRptMessage.pData		= ucMsg;

	if(ulSize > 100)
	{
		return ERR_MSP_FAILURE;
	}

	if(g_HsoConn)
	{
		diag_CfgResetAllSwt();

		diag_CfgSetGlobalBitValue(&g_ulDiagCfgInfo,DIAG_CFG_INIT_BIT,DIAG_CFG_SWT_OPEN);

		diag_CfgSetGlobalBitValue(&g_ulDiagCfgInfo,DIAG_CFG_CONN_BIT,DIAG_CFG_SWT_OPEN);

		stLayerReq[0].ulModuleId = 150;
		stLayerReq[0].ulIsDestModule = 0;
		stLayerReq[0].ulSwitch = DIAG_CMD_REQ_LOG_CFG_FILTER_SWT_OPEN;
		diag_CfgSetLayerSwt(&(stLayerReq[0]), sizeof(DIAG_CMD_LOG_CAT_LAYER_REQ_STRU));

		stLayerReq[0].ulModuleId = 152;
		stLayerReq[0].ulIsDestModule = 1;
		stLayerReq[0].ulSwitch = 1;
		diag_CfgSetLayerSwt(stLayerReq, sizeof(DIAG_CMD_LOG_CAT_LAYER_REQ_STRU));
	}

	diag_printf("DIAG_ReportLayerMessageLog !g_ulDiagCfgInfo =%d\n",g_ulDiagCfgInfo);

	 ret = DIAG_ReportLayerMessageLog(&stRptMessage);
	return ret;
}


VOS_UINT32 Diag_VoLTEMessageTest( VOS_UINT32 ulSize)
{
	DIAG_VoLTE_LOG_STRU stRptMessage;
	VOS_UINT8 ucMsg[100] = "VOLTE VOLTE VOLTE";
	VOS_UINT32 ret;
	DIAG_CMD_LOG_CAT_LAYER_REQ_STRU stLayerReq[5] = {{0},};

	stRptMessage.pData = NULL;
	stRptMessage.ulId = DIAG_ID(0xdf,OS_MSG_UL);
	stRptMessage.ulMessageID = 0x0FC0;
	stRptMessage.ulDestMod = 0xdf;
	stRptMessage.ulSideId = DIAG_SIDE_NET;
	stRptMessage.ulDataSize = ulSize;
	stRptMessage.pData		= ucMsg;

	if(ulSize > 100)
	{
		return ERR_MSP_FAILURE;
	}

	if(g_HsoConn)
	{
		diag_CfgResetAllSwt();

		diag_CfgSetGlobalBitValue(&g_ulDiagCfgInfo,DIAG_CFG_INIT_BIT,DIAG_CFG_SWT_OPEN);

		diag_CfgSetGlobalBitValue(&g_ulDiagCfgInfo,DIAG_CFG_CONN_BIT,DIAG_CFG_SWT_OPEN);

		stLayerReq[0].ulModuleId = 0xdf;
		stLayerReq[0].ulIsDestModule = 0;
		stLayerReq[0].ulSwitch = DIAG_CMD_REQ_LOG_CFG_FILTER_SWT_OPEN;
		diag_CfgSetLayerSwt(&(stLayerReq[0]), sizeof(DIAG_CMD_LOG_CAT_LAYER_REQ_STRU));
	}

	diag_printf("DIAG_ReportVoLTELog !g_ulDiagCfgInfo =%d\n",g_ulDiagCfgInfo);

	ret = DIAG_ReportVoLTELog(&stRptMessage);
	return ret;
}

VOS_UINT32 Diag_PrintMessageTest( VOS_UINT32 ulLevelFilter);
VOS_UINT32 Diag_PrintMessageTest( VOS_UINT32 ulLevelFilter)
{
	VOS_UINT32 ret =0;
	DIAG_CMD_LOG_CAT_PRINT_REQ_STRU* pstPrintSwtReq = NULL;

	if(g_HsoConn)
	{
		diag_CfgResetAllSwt();

		ret = diag_CfgSetGlobalBitValue(&g_ulDiagCfgInfo,DIAG_CFG_INIT_BIT,DIAG_CFG_SWT_OPEN);
		if(ret != ERR_MSP_SUCCESS)
		{
			diag_printf("DIAG_PrintfV 111 !\n");
		}
		ret = diag_CfgSetGlobalBitValue(&g_ulDiagCfgInfo,DIAG_CFG_CONN_BIT,DIAG_CFG_SWT_OPEN);
		if(ret != ERR_MSP_SUCCESS)
		{
			diag_printf("DIAG_PrintfV! 222 \n");
		}

		diag_printf("DIAG_PrintfV !g_ulDiagCfgInfo =%d\n",g_ulDiagCfgInfo);

		pstPrintSwtReq = (DIAG_CMD_LOG_CAT_PRINT_REQ_STRU*)diag_HeapAllocD(sizeof(DIAG_CMD_LOG_CAT_PRINT_REQ_STRU));   /* [false alarm]:屏蔽Fortify */
        if(NULL == pstPrintSwtReq)
        {
			diag_printf("DIAG_PrintfV! 333 \n");
            return ERR_MSP_FAILURE;
        }
        /*lint -save -e539*/
		pstPrintSwtReq->ulModuleId = 152;
        /*lint -restore*/
		pstPrintSwtReq->ulLevelFilter = DIAG_CMD_LOG_CFG_PRINT_LEVEL_ERROR;

		ret = diag_CfgSetPrintSwt(pstPrintSwtReq, sizeof(DIAG_CMD_LOG_CAT_PRINT_REQ_STRU));
        if(ret != ERR_MSP_SUCCESS)
        {
			diag_printf("DIAG_PrintfV! 444 \n");
        }
	}
	ret = DIAG_PrintfV(DIAG_ID(152 ,ulLevelFilter),"aa", 100, ("%s, Debug1: %d, %d, %d, %d, %d"),"ssss",0,0,0,0,0);
	return ret;
}
VOS_UINT32 Diag_UserPlaneMessageTest( VOS_UINT32 ulMsgId,VOS_UINT32 ulSwitch);
VOS_UINT32 Diag_UserPlaneMessageTest( VOS_UINT32 ulMsgId,VOS_UINT32 ulSwitch)
{
	VOS_UINT32 ret =0;
	DIAG_USERPLANE_MSG_STRU stRptMessage = {0};
	DIAG_CMD_LOG_CAT_USERPLANE_REQ_STRU stUserPlaneSwtReq[DIAG_CFG_USER_PLANE_ID_NUM] = {{0},};
	VOS_UINT8 ucMsg[100] ={2,2,2,2,};

	stRptMessage.ulMessageID = 0xd02;
	stRptMessage.ulDataSize = 100;
	stRptMessage.pData = ucMsg;

	if(g_HsoConn)
	{
		diag_CfgResetAllSwt();

		diag_CfgSetGlobalBitValue(&g_ulDiagCfgInfo,DIAG_CFG_INIT_BIT,DIAG_CFG_SWT_OPEN);

		diag_CfgSetGlobalBitValue(&g_ulDiagCfgInfo,DIAG_CFG_CONN_BIT,DIAG_CFG_SWT_OPEN);

		stUserPlaneSwtReq[0].ulMsgId = ulMsgId;
		stUserPlaneSwtReq[0].ulSwitch = ulSwitch;
		ret = diag_CfgSetUserPlaneSwt(&(stUserPlaneSwtReq[0]),sizeof(DIAG_CMD_LOG_CAT_USERPLANE_REQ_STRU));
        if(ret != ERR_MSP_SUCCESS)
        {
			diag_printf("Diag_UserPlaneMessageTest! 111 \n");
        }
	}
	diag_printf("DIAG_ReportUserPlaneMessageLog !g_ulDiagCfgInfo =%d\n",g_ulDiagCfgInfo);

	ret = DIAG_ReportUserPlaneMessageLog(&stRptMessage);
	return ret;
}
VOS_UINT32 Diag_EventMessageTest( VOS_UINT32 id,VOS_UINT32 ulEventID);
VOS_UINT32 Diag_EventMessageTest( VOS_UINT32 id,VOS_UINT32 ulEventID)
{
	VOS_UINT32 ret;

	if(g_HsoConn)
	{
		diag_CfgResetAllSwt();

		diag_CfgSetGlobalBitValue(&g_ulDiagCfgInfo,DIAG_CFG_INIT_BIT,DIAG_CFG_SWT_OPEN);

		diag_CfgSetGlobalBitValue(&g_ulDiagCfgInfo,DIAG_CFG_CONN_BIT,DIAG_CFG_SWT_OPEN);

		diag_CfgSetGlobalBitValue(&g_ulDiagCfgInfo,DIAG_CFG_EVENT_BIT,DIAG_CFG_SWT_OPEN);
	}
	diag_printf("DIAG_ReportEventLog !g_ulDiagCfgInfo =%d\n",g_ulDiagCfgInfo);

	ret = DIAG_ReportEventLog(id,ulEventID);
	return ret;
}
VOS_VOID Diag_BufTest2(VOS_VOID);
VOS_VOID Diag_BufTest2(VOS_VOID)
{
    /*lint -save -e438 -e329 -e550*/
	VOS_UINT_PTR ulRealAddr=0;
	VOS_UINT8* pucBuf= NULL;

    /* coverity[returned_pointer] */
	pucBuf = (VOS_UINT8*)VOS_UnCacheMemAlloc((VOS_UINT32)100*32, &ulRealAddr);
    if(NULL != pucBuf)
    {
	    DIAG_FLUSH_CACHE(pucBuf,32);
    }
    /*lint -restore*/
}
VOS_VOID Diag_BufTest(VOS_VOID);
VOS_VOID Diag_BufTest(VOS_VOID)
{
	VOS_UINT32 i =0;
	for(i=0;i <= 4096;i++)
	{
		Diag_EventMessageTest(0,0);
	}
}
VOS_UINT32 Diag_CmdMessageTest( VOS_UINT32 id,VOS_UINT32 ulSize);
VOS_UINT32 Diag_CmdMessageTest( VOS_UINT32 id,VOS_UINT32 ulSize)
{
	VOS_UINT32 ret =0;
	VOS_UINT8 ucMsg[100] ={3,3,3,3,};

	if(ulSize > 100)
	{
		return ERR_MSP_FAILURE;
	}

	if(g_HsoConn)
	{
		diag_CfgResetAllSwt();

		diag_CfgSetGlobalBitValue(&g_ulDiagCfgInfo,DIAG_CFG_INIT_BIT,DIAG_CFG_SWT_OPEN);

		diag_CfgSetGlobalBitValue(&g_ulDiagCfgInfo,DIAG_CFG_CONN_BIT,DIAG_CFG_SWT_OPEN);
	}
    /*lint -save -e717*/
	diag_printf("DIAG_ReportCommand !g_ulDiagCfgInfo =%d\n",g_ulDiagCfgInfo);
    /*lint -restore*/
	ret = DIAG_ReportCommand((VOS_UINT16)id, ulSize, ucMsg);
	return ret;
}

VOS_VOID DIAG_API_TEST(VOS_VOID);
VOS_VOID DIAG_API_TEST(VOS_VOID)
{
	vos_printf("Diag_AirMessageTest(ulSize)!\n");
	vos_printf("Diag_GuAirMessageTest(ulSize)!\n");
	vos_printf("Diag_LayerMessageTest(ulSize)!\n");
	vos_printf("Diag_PrintMessageTest(ulLevelFilter)!\n");
	vos_printf("Diag_UserPlaneMessageTest(ulSwitch)!\n");
	vos_printf("Diag_EventMessageTest(id,ulEventID)!\n");
	vos_printf("Diag_CmdMessageTest(id,ulSize)!\n");
}

#if(VOS_OS_VER == VOS_LINUX)
VOS_CHAR g_pDirName[] = "/system/om";
VOS_CHAR g_pFileName[] = "/system/om/diag_Test.txt";
VOS_CHAR g_pFileName2[] = "/system/om/diag_Test2.txt";

VOS_VOID Diag_CreatDir(VOS_VOID)
{

#if(VOS_OS_VER == VOS_LINUX)
		mm_segment_t old_fs;

		old_fs = get_fs();
		set_fs(KERNEL_DS);
#endif

	 vos_printf("%s\n",__FUNCTION__);
	 (VOS_VOID)DIAG_FS_MKDIR("/system/om/",0755);

#if(VOS_OS_VER == VOS_LINUX)
		set_fs(old_fs);
#endif

	return;
}

VOS_VOID Diag_CreatDir2(VOS_VOID)
{

#if(VOS_OS_VER == VOS_LINUX)
	mm_segment_t old_fs;

	old_fs = get_fs();
	set_fs(KERNEL_DS);
#endif

	 vos_printf("%s\n",__FUNCTION__);
	 (VOS_VOID)DIAG_FS_MKDIR("/system/om2",0755);

#if(VOS_OS_VER == VOS_LINUX)
	set_fs(old_fs);
#endif

	return;
}

extern VOS_INT32 diag_FsOpen(const VOS_CHAR *pcFileName, VOS_INT lFlag);
VOS_INT32 g_Testfd =0,g_Testfd2 =0;

extern VOS_VOID Diag_CreatFile(VOS_VOID);
extern VOS_INT Diag_CloseFile(VOS_VOID);
extern VOS_INT Diag_WriteFile(VOS_VOID);

VOS_VOID Diag_FileTest(VOS_VOID)
{
	Diag_CreatFile();
	Diag_WriteFile();
	Diag_CloseFile();
}

VOS_VOID Diag_CreatFile(VOS_VOID)
{

#if(VOS_OS_VER == VOS_LINUX)
			mm_segment_t old_fs;

			old_fs = get_fs();
			set_fs(KERNEL_DS);
#endif
	g_Testfd = diag_FsOpen(g_pFileName,DIAG_FS_CREAT|DIAG_FS_APPEND|DIAG_FS_RDWR);
	vos_printf("%s%d\n",__FUNCTION__,g_Testfd);

#if(VOS_OS_VER == VOS_LINUX)
			set_fs(old_fs);
#endif
	return;
}

VOS_INT Diag_WriteFile(VOS_VOID)
{
	VOS_INT lResult=0;

#if(VOS_OS_VER == VOS_LINUX)
	mm_segment_t old_fs;

	old_fs = get_fs();
	set_fs(KERNEL_DS);
#endif
    lResult = DIAG_FS_WRITE(g_Testfd, "aaa", VOS_StrLen("aaa"));
	vos_printf("%s%d\n",__FUNCTION__,lResult);

#if(VOS_OS_VER == VOS_LINUX)
	set_fs(old_fs);
#endif
	return lResult;
}

VOS_INT Diag_CloseFile(VOS_VOID)
{
	VOS_INT lResult=0;
#if(VOS_OS_VER == VOS_LINUX)
		mm_segment_t old_fs;

		old_fs = get_fs();
		set_fs(KERNEL_DS);
#endif

	lResult =  DIAG_FS_CLOSE(g_Testfd);

#if(VOS_OS_VER == VOS_LINUX)
		set_fs(old_fs);
#endif
	return lResult;
}

VOS_VOID Diag_RmDir(VOS_VOID)
{
#if(VOS_OS_VER == VOS_LINUX)
			mm_segment_t old_fs;

			old_fs = get_fs();
			set_fs(KERNEL_DS);
#endif
	DIAG_FS_RMDIR(g_pDirName);
#if(VOS_OS_VER == VOS_LINUX)
			set_fs(old_fs);
#endif

	return;
}

VOS_VOID Diag_RmFile(VOS_VOID)
{
#if(VOS_OS_VER == VOS_LINUX)
			mm_segment_t old_fs;

			old_fs = get_fs();
			set_fs(KERNEL_DS);
#endif

	 DIAG_FS_RMFILE(g_pFileName);
#if(VOS_OS_VER == VOS_LINUX)
			set_fs(old_fs);
#endif

	return;
}

VOS_VOID Diag_Stat(VOS_VOID)
{
	DIAG_STAT_STRU stStat = {0};

#if(VOS_OS_VER == VOS_LINUX)
			mm_segment_t old_fs;

			old_fs = get_fs();
			set_fs(KERNEL_DS);
#endif
	if (DIAG_FS_ERROR == DIAG_FS_STAT(g_pDirName, &stStat))
    {
        vos_printf("stat error!\n");
    }

	vos_printf("stat stStat:!,size=%d,st_atime=%d,st_mtime=%d,st_ctime=%d\n",stStat.size,stStat.atime.tv_sec,stStat.mtime.tv_sec,stStat.ctime.tv_sec);

#if(VOS_OS_VER == VOS_LINUX)
			set_fs(old_fs);
#endif
	return;
}
#endif

/* test case for diag drx mntn */
VOS_VOID Diag_M3_Drx_Mntn(VOS_VOID)
{
	/* */

}
typedef enum
{
	DIAG_ENUM_MODEM_BBPDMA = DIAG_CMD_DRX_SAMPLE_ADDR_REQ,
	DIAG_ENUM_MODEM_BBPDBG,
	DIAG_ENUM_MODEM_BBPSRC,
	DIAG_ENUM_MODEM_SYSCLK,
	DIAG_ENUM_MODEM_SOCP,
	DIAG_ENUM_BUTT
}DIAG_ENUM_TYPE_E;

/*test */
#if(VOS_OS_VER == VOS_LINUX)
VOS_VOID Test_Diag_Mntn_Drx(DIAG_ENUM_TYPE_E eDiagEnumType)
{
	VOS_UINT8 *pHsoMsg = NULL;
	VOS_UINT32 u32HsoMsgLg = 0;
	VOS_UINT32 ret = ERR_MSP_SUCCESS;
	VOS_UINT32 u32DiagId=eDiagEnumType;

	MSP_SOCP_HEAD_STRU sSocpHeader = {MSP_SOCP_SID_DIAG_SERVER,(MSP_SOCP_HIGH4BIT_SSID_MCPU << 4)
                                      ,MSP_SOCP_SERVER_SESSION_ID,MSP_SOCP_MSG_TYPE_REQ,0,{0,}};
	MSP_DIAG_HEAD_STRU *pDiagHeadStru = NULL;
	MSP_DIAG_DATA_REQ_STRU *pDiagReqHeadStru = NULL;


	/* count length */
	u32HsoMsgLg = sizeof(MSP_SOCP_HEAD_STRU)+sizeof(MSP_DIAG_HEAD_STRU)+sizeof(MSP_DIAG_DATA_REQ_STRU);
	/* malloc buffer */
	pHsoMsg = VOS_MemAlloc(MSP_PID_DIAG_FW, DYNAMIC_MEM_PT, u32HsoMsgLg);
    if(NULL == pHsoMsg)
    {
        return;
    }

    memcpy((VOS_UINT8 *)pHsoMsg,(VOS_UINT8*)&sSocpHeader,sizeof(MSP_SOCP_HEAD_STRU));
	pDiagHeadStru = (MSP_DIAG_HEAD_STRU *)(pHsoMsg + sizeof(MSP_SOCP_HEAD_STRU));

	pDiagHeadStru->ulID = (MSP_STRU_ID_28_31_GROUP_MSP << 28) | (MSP_STRU_ID_16_23_MSP_CMD<< 16) | u32DiagId;

	diag_printf("ulID is 0x%x!\n",pDiagHeadStru->ulID);

	pDiagHeadStru->ulDataSize = u32HsoMsgLg - sizeof(MSP_SOCP_HEAD_STRU)-sizeof(MSP_DIAG_HEAD_STRU);
   /* para->time_stamp         = 0;\*/

	pDiagReqHeadStru = (MSP_DIAG_DATA_REQ_STRU *)(((VOS_UINT8*)pDiagHeadStru)+sizeof(MSP_DIAG_HEAD_STRU));
    pDiagReqHeadStru->ulAuid = 0;
	pDiagReqHeadStru->ulSn= 0;

    /*参数检查，确认HSO下发数据是否正确*/
    ret = diag_FwDecodeDataCheck(pHsoMsg);
    if(ret != ERR_MSP_SUCCESS)
    {
        diag_printf("diag_FwDecodeDataCheck ERROR, ret is 0x%x",ret);
        return;
    }

    /*发送给DIAG FW进行处理*/

	ret = diag_SendMsg(MSP_PID_DIAG_FW, MSP_PID_DIAG_AGENT, ID_MSG_DIAG_CMD_REQ_FW_TO_AGENT, pHsoMsg, u32HsoMsgLg);

    if(ret != ERR_MSP_SUCCESS)
    {
       DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_DECODE_PACKET_RCVE_ERROR, 0, 0, 6);
    }

	VOS_MemFree(MSP_PID_DIAG_FW,pHsoMsg);

    return;

}


/* 以下测试低功耗数采*/
/*drx sample test*/
VOS_UINT32 Test_Diag_Sample_Drx_Common(DIAG_ENUM_TYPE_E eDiagEnumCore,
VOS_UINT32 ulSerId, VOS_UINT32 ulStrId, VOS_UINT32 ulMailboxId, VOS_UINT32 ulOmSamPleId)
{
	VOS_UINT8 *pHsoMsg = NULL;
	VOS_UINT32 u32HsoMsgLg = 0;
	VOS_UINT32 ret = ERR_MSP_SUCCESS;

	MSP_SOCP_HEAD_STRU sSocpHeader = {MSP_SOCP_SID_DIAG_SERVER,(ulSerId<< 4)
                                      ,MSP_SOCP_SERVER_SESSION_ID,MSP_SOCP_MSG_TYPE_REQ,0,{0,}}; /* ulSerId normal :MSP_SOCP_HIGH4BIT_SSID_MCPU*/
	MSP_DIAG_HEAD_STRU *pDiagHeadStru = NULL;
	MSP_DIAG_DATA_REQ_STRU *pDiagReqHeadStru = NULL;
	DIAG_CMD_DRX_SAMPLE_REG_WR_REQ_STRU *psDrxSample = NULL;


	/* count length */
	u32HsoMsgLg = sizeof(MSP_SOCP_HEAD_STRU)+sizeof(MSP_DIAG_HEAD_STRU)+sizeof(MSP_DIAG_DATA_REQ_STRU)+sizeof(DIAG_CMD_DRX_SAMPLE_REG_WR_REQ_STRU);
	/* malloc buffer */
	pHsoMsg = VOS_MemAlloc(MSP_PID_DIAG_FW, DYNAMIC_MEM_PT, u32HsoMsgLg);
    if(NULL == pHsoMsg)
    {
        return ERR_MSP_MALLOC_FAILUE;
    }

    memcpy((VOS_UINT8 *)pHsoMsg,(VOS_UINT8*)&sSocpHeader,sizeof(MSP_SOCP_HEAD_STRU));
	pDiagHeadStru = (MSP_DIAG_HEAD_STRU *)(pHsoMsg + sizeof(MSP_SOCP_HEAD_STRU));

	/* normal: DIAG_CMD_DRX_SAMPLE_REG_WR */
	pDiagHeadStru->ulID = (MSP_STRU_ID_28_31_GROUP_MSP << 28) | (MSP_STRU_ID_16_23_MSP_CMD<< 16) | ulStrId;

	diag_printf("ulID is 0x%x!\n",pDiagHeadStru->ulID);

	pDiagHeadStru->ulDataSize = u32HsoMsgLg - sizeof(MSP_SOCP_HEAD_STRU)-sizeof(MSP_DIAG_HEAD_STRU);
   /* para->time_stamp         = 0;\*/

	pDiagReqHeadStru = (MSP_DIAG_DATA_REQ_STRU *)(((VOS_UINT8*)pDiagHeadStru)+sizeof(MSP_DIAG_HEAD_STRU));
    pDiagReqHeadStru->ulAuid = 0;
	pDiagReqHeadStru->ulSn= 0;

	psDrxSample = (DIAG_CMD_DRX_SAMPLE_REG_WR_REQ_STRU *)(((VOS_UINT8*)pDiagReqHeadStru) +sizeof(MSP_DIAG_DATA_REQ_STRU));
	psDrxSample->ulOmDrxSampleId = ulOmSamPleId; /* normal : 0--data sample; 1--log sample*/
	psDrxSample->usOpid = 0; /*start */
	psDrxSample->usPowrDomainBitmap = 0;

    /*参数检查，确认HSO下发数据是否正确*/
    ret = diag_FwDecodeDataCheck(pHsoMsg);
    if(ret != ERR_MSP_SUCCESS)
    {
        diag_printf("diag_FwDecodeDataCheck ERROR, ret is 0x%x",ret);
        return ret;
    }

    /*发送给DIAG FW进行处理*/

	ret = diag_SendMsg(MSP_PID_DIAG_FW, MSP_PID_DIAG_AGENT, ID_MSG_DIAG_CMD_REQ_FW_TO_AGENT, pHsoMsg, u32HsoMsgLg);

    if(ret != ERR_MSP_SUCCESS)
    {
       DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_DECODE_PACKET_RCVE_ERROR, 0, 0, 6);
    }

	VOS_MemFree(MSP_PID_DIAG_FW,pHsoMsg);

    return ret;

}

/* DRX.SAMPLE.DRX.001: data sample */
VOS_VOID Test_Diag_Sample_Drx_001(VOS_VOID)
{
	Test_Diag_Sample_Drx_Common(DIAG_ENUM_MODEM_BBPDMA, MSP_SOCP_HIGH4BIT_SSID_MCPU, DIAG_CMD_DRX_SAMPLE_ADDR_REQ, 4, 0); /*4: MBX_ADDRESSER_LTE_PHY_HS_DIAG*/
}

/* DRX.SAMPLE.DRX.002: log sample */
VOS_VOID Test_Diag_Sample_Drx_002(VOS_VOID)
{
	Test_Diag_Sample_Drx_Common(DIAG_ENUM_MODEM_BBPDBG, MSP_SOCP_HIGH4BIT_SSID_MCPU, DIAG_CMD_DRX_SAMPLE_CHNSIZE_REQ, 4, 1);
}

/* DRX.SAMPLE.DRX.003: log sample */
VOS_VOID Test_Diag_Sample_Drx_003(VOS_VOID)
{
	Test_Diag_Sample_Drx_Common(DIAG_ENUM_MODEM_BBPSRC, MSP_SOCP_HIGH4BIT_SSID_MCPU, DIAG_CMD_DRX_SAMPLE_GET_VERSION_REQ, 4, 1);
}

VOS_VOID Test_Diag_Sample_Drx_004(VOS_VOID)
{
	Test_Diag_Sample_Drx_Common(DIAG_ENUM_MODEM_SYSCLK, MSP_SOCP_HIGH4BIT_SSID_MCPU, DIAG_CMD_DRX_SAMPLE_ABLE_CHN_REQ, 4, 1);
}

VOS_VOID Test_Diag_Sample_Drx_005(VOS_VOID)
{
	Test_Diag_Sample_Drx_Common(DIAG_ENUM_MODEM_SOCP, MSP_SOCP_HIGH4BIT_SSID_MCPU, DIAG_CMD_DRX_SAMPLE_ABLE_CHN_REQ, 4, 1);
}
/*lint -restore*/

#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


