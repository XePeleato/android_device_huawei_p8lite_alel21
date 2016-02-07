/* 头文件包含 */
#include "omsdlog.h"
#include "OmFsPpm.h"
#include "omnvinterface.h"

extern NV_FLASH_LOG_RECORD_STRU            g_stFlashLogCfg;

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_FSStartLog_case1(VOS_VOID)
{
    g_stFlashLogCfg.ulGULogFileSize = 0;

    return PPM_FSStartLog();
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
VOS_UINT32 uttest_PPM_FSStartLog_case2(VOS_VOID)
{
    g_stFlashLogCfg.ulGULogFileSize = 1000;

    return PPM_FSStartLog();
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
VOS_UINT32 uttest_PPM_FSStartLog_case3(VOS_VOID)
{
    g_stFlashLogCfg.ulGULogFileSize = 1000;

    return PPM_FSStartLog();
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
VOS_UINT32 uttest_PPM_FSStartLog_case4(VOS_VOID)
{
    g_stFlashLogCfg.ulGULogFileSize = 1000;

    return PPM_FSStartLog();
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
VOS_UINT32 uttest_PPM_FSWriteLogProc_case1(VOS_VOID)
{
    VOS_UINT8                           aucVirAddr[100];
    VOS_UINT8                           aucPhyAddr[100];

    return PPM_FSWriteLogProc(aucVirAddr, aucPhyAddr, sizeof(aucPhyAddr));
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
VOS_UINT32 uttest_PPM_FSInitLogFile_case1(VOS_VOID)
{
    return PPM_FSInitLogFile();
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
VOS_UINT32 NV_ReadExStub(MODEM_ID_ENUM_UINT16           enModemID,
                            VOS_UINT16                  usID,
                            VOS_VOID                   *pItem,
                            VOS_UINT32                  ulLength)
{
    NV_FLASH_LOG_RECORD_STRU *pstNv;

    pstNv = (NV_FLASH_LOG_RECORD_STRU *)pItem;

    pstNv->ulGULogFileSize = 0;

    return VOS_OK;
}

VOS_UINT32 uttest_PPM_FSInitLogFile_case2(VOS_VOID)
{
    return PPM_FSInitLogFile();
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
VOS_UINT32 NV_ReadExStub1(MODEM_ID_ENUM_UINT16           enModemID,
                            VOS_UINT16                  usID,
                            VOS_VOID                   *pItem,
                            VOS_UINT32                  ulLength)
{
    NV_FLASH_LOG_RECORD_STRU *pstNv;

    pstNv = (NV_FLASH_LOG_RECORD_STRU *)pItem;

    pstNv->ulGULogFileSize = 1000;

    return VOS_OK;
}

VOS_UINT32 uttest_PPM_FSInitLogFile_case3(VOS_VOID)
{
    return PPM_FSInitLogFile();
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
VOS_UINT32 uttest_PPM_FSInitLogFile_case4(VOS_VOID)
{
    return PPM_FSInitLogFile();
}

