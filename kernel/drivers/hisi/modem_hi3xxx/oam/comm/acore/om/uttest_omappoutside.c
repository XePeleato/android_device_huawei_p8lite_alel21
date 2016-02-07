#include "omprivate.h"

extern VOS_VOID OM_AcpuCallBackMsgProc(MsgBlock* pMsg);


VOS_VOID uttest_OM_AcpuCallBackMsgProc_case1(VOS_VOID)
{
	MsgBlock          *pstMsg;
	OM_REQ_PACKET_STRU stAppMsg;

	stAppMsg.ucModuleId = 35;

	pstMsg = (MsgBlock*)malloc(sizeof(MsgBlock)+sizeof(OM_REQ_PACKET_STRU));

	memcpy(pstMsg->aucValue, &stAppMsg, sizeof(OM_REQ_PACKET_STRU));

	OM_AcpuCallBackMsgProc(pstMsg);

	free(pstMsg);
}


VOS_VOID uttest_OM_AcpuCallBackMsgProc_case2(VOS_VOID)
{
	MsgBlock          *pstMsg;
	OM_REQ_PACKET_STRU stAppMsg;

	stAppMsg.ucModuleId = 65;

	pstMsg = (MsgBlock*)malloc(sizeof(MsgBlock)+sizeof(OM_REQ_PACKET_STRU));

	memcpy(pstMsg->aucValue, &stAppMsg, sizeof(OM_REQ_PACKET_STRU));

	OM_AcpuCallBackMsgProc(pstMsg);

	free(pstMsg);
}


VOS_VOID uttest_OM_AcpuCallBackMsgProc_case3(VOS_VOID)
{
	MsgBlock          *pstMsg;
	OM_REQ_PACKET_STRU stAppMsg;

	stAppMsg.ucModuleId = 50;

	pstMsg = (MsgBlock*)malloc(sizeof(MsgBlock)+sizeof(OM_REQ_PACKET_STRU));

	memcpy(pstMsg->aucValue, &stAppMsg, sizeof(OM_REQ_PACKET_STRU));

	g_astAcpuOmFuncIdProcTbl[50].pOmFuncProc = VOS_NULL_PTR;

	OM_AcpuCallBackMsgProc(pstMsg);

	free(pstMsg);
}


VOS_VOID TestSample(OM_REQ_PACKET_STRU *pstRspPacket, OM_RSP_FUNC *pstRspFuncPtr)
{
	return;
}

VOS_VOID uttest_OM_AcpuCallBackMsgProc_case4(VOS_VOID)
{
	MsgBlock          *pstMsg;
	OM_REQ_PACKET_STRU stAppMsg;

	stAppMsg.ucModuleId = 50;

	pstMsg = (MsgBlock*)malloc(sizeof(MsgBlock)+sizeof(OM_REQ_PACKET_STRU));

	memcpy(pstMsg->aucValue, &stAppMsg, sizeof(OM_REQ_PACKET_STRU));

	g_astAcpuOmFuncIdProcTbl[50].pOmFuncProc = TestSample;

	OM_AcpuCallBackMsgProc(pstMsg);

	free(pstMsg);
}


VOS_UINT32 uttest_OM_AcpuCallBackFidInit_case1(VOS_VOID)
{
    if (VOS_ERR == OM_AcpuCallBackFidInit(VOS_IP_LOAD_CONFIG))
    {
        return VOS_OK;
    }

    return VOS_ERR;
}


VOS_UINT32 uttest_OM_AcpuCallBackFidInit_case2(VOS_VOID)
{
	if (VOS_ERR == OM_AcpuCallBackFidInit(VOS_IP_LOAD_CONFIG))
	{
	    return VOS_OK;
	}

	return VOS_ERR;
}


VOS_UINT32 uttest_OM_AcpuCallBackFidInit_case3(VOS_VOID)
{
	if (VOS_OK == OM_AcpuCallBackFidInit(VOS_IP_LOAD_CONFIG))
	{
	    return VOS_OK;
	}

	return VOS_ERR;
}
