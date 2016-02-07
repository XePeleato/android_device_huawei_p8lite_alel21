/* 头文件包含 */
#include "OmSdPpm.h"
#include "omnvinterface.h"


/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_LogFileInit_case1(VOS_VOID)
{
    return PPM_LogFileInit();
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 NV_ReadExPPMLogFileInitStub(MODEM_ID_ENUM_UINT16           enModemID,
                            VOS_UINT16                  usID,
                            VOS_VOID                   *pItem,
                            VOS_UINT32                  ulLength)
{
    OM_CHANNLE_PORT_CFG_STRU *pstNv;

    pstNv = (OM_CHANNLE_PORT_CFG_STRU *)pItem;

    pstNv->enPortNum = CPM_OM_PORT_TYPE_SD;

    return VOS_OK;
}


VOS_UINT32 uttest_PPM_LogFileInit_case2(VOS_VOID)
{
    return PPM_LogFileInit();
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 NV_ReadExPPMLogFileInitStub1(MODEM_ID_ENUM_UINT16           enModemID,
                            VOS_UINT16                  usID,
                            VOS_VOID                   *pItem,
                            VOS_UINT32                  ulLength)
{
    OM_CHANNLE_PORT_CFG_STRU *pstNv;

    pstNv = (OM_CHANNLE_PORT_CFG_STRU *)pItem;

    pstNv->enPortNum = CPM_OM_PORT_TYPE_FS;

    return VOS_OK;
}


VOS_UINT32 uttest_PPM_LogFileInit_case3(VOS_VOID)
{
    return PPM_LogFileInit();
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 NV_ReadExPPMLogFileInitStub2(MODEM_ID_ENUM_UINT16           enModemID,
                            VOS_UINT16                  usID,
                            VOS_VOID                   *pItem,
                            VOS_UINT32                  ulLength)
{
    OM_CHANNLE_PORT_CFG_STRU *pstNv;

    pstNv = (OM_CHANNLE_PORT_CFG_STRU *)pItem;

    pstNv->enPortNum = CPM_OM_PORT_TYPE_USB;

    return VOS_OK;
}


VOS_UINT32 uttest_PPM_LogFileInit_case4(VOS_VOID)
{
    return PPM_LogFileInit();
}

