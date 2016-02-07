/* 头文件包含 */
#include "SOCPInterface.h"
#include "SCMProc.h"
#include "SCMSoftDecode.h"
#include "ombufmngr.h"
#include "OmHdlcInterface.h"
#include "OmAppRl.h"

VOS_UINT32 uttest_SCM_SoftDecodeCfgDataRcv_case1(VOS_VOID)
{
	VOS_UINT8 aucBuffer[1000] = {0}; 
	VOS_UINT32 ulLen = 1000;

	return SCM_SoftDecodeCfgDataRcv(aucBuffer, ulLen);
}

VOS_UINT32 uttest_SCM_SoftDecodeDataRcv_case1(VOS_VOID)
{
	VOS_UINT8 aucBuffer[1000] = {0}; 
	VOS_UINT32 ulLen = 1000;

	return SCM_SoftDecodeDataRcv(aucBuffer, ulLen);
}

VOS_UINT32 uttest_SCM_SoftDecodeDataRcv_case2(VOS_VOID)
{
	VOS_UINT8 aucBuffer[1000] = {0}; 
	VOS_UINT32 ulLen = 1024;

	return SCM_SoftDecodeDataRcv(aucBuffer, ulLen);
}

VOS_UINT32 uttest_SCM_SoftDecodeDataRcv_case3(VOS_VOID)
{
	VOS_UINT8 aucBuffer[1000] = {0}; 
	VOS_UINT32 ulLen = 1000;

	return SCM_SoftDecodeDataRcv(aucBuffer, ulLen);
}

VOS_UINT32 Stub_RcvData(SOCP_DECODER_DST_ENUM_U32 enChanlID,VOS_UINT8 *pucData, VOS_UINT32 ulSize, VOS_UINT8 *pucRBData, VOS_UINT32 ulRBSize)
{
	return VOS_OK;
}

VOS_VOID uttest_SCM_RcvDataDispatch_case1(VOS_VOID)
{
	OM_HDLC_STRU                        stHdlcCtrl;
	VOS_UINT8                           ucDataType = SCM_DATA_TYPE_TL;

	g_astSCMDecoderCbFunc[SOCP_DECODER_DST_CB_TL_OM] = Stub_RcvData;

	SCM_RcvDataDispatch(&stHdlcCtrl, ucDataType);
}

VOS_VOID uttest_SCM_RcvDataDispatch_case2(VOS_VOID)
{
	OM_HDLC_STRU                        stHdlcCtrl;
	VOS_UINT8                           ucDataType = SCM_DATA_TYPE_GU;

	g_astSCMDecoderCbFunc[SOCP_DECODER_DST_CB_GU_OM] = Stub_RcvData;

	SCM_RcvDataDispatch(&stHdlcCtrl, ucDataType);
}

VOS_UINT32 uttest_SCM_SoftDecodeAcpuRcvData_case1(VOS_VOID)
{
	OM_HDLC_STRU                        stHdlcCtrl;
	VOS_UINT8                           aucData[10];
	VOS_UINT32                          ulLen = 3;
	VOS_UINT8                           aucTmp[10];


	stHdlcCtrl.pucDecapBuff = aucTmp;
	stHdlcCtrl.pucDecapBuff[0] = SCM_DATA_TYPE_GU;

	return SCM_SoftDecodeAcpuRcvData(&stHdlcCtrl, aucData, ulLen);
}

VOS_UINT32 uttest_SCM_SoftDecodeCfgHdlcInit_case1(VOS_VOID)
{
	OM_HDLC_STRU                        stHdlcCtrl;

	return SCM_SoftDecodeCfgHdlcInit(&stHdlcCtrl);
}

VOS_UINT32 uttest_SCM_SoftDecodeCfgRcvTaskInit_case1(VOS_VOID)
{
	return SCM_SoftDecodeCfgRcvTaskInit();
}