

/*****************************************************************************
  1 头文件包含
**************************************************************************** */
#include "SCMProc.h"
#include "ombufmngr.h"
#include "cpm.h"
#include "omsdlog.h"
#include "OmFsPpm.h"
#include "NVIM_Interface.h"
#include "SysNvId.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* lint -e767  */
#define    THIS_FILE_ID        PS_FILE_ID_OM_FS_PPM_C
/* lint +e767  */

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */


/*****************************************************************************
  3 外部引用声明
*****************************************************************************/
extern NV_FLASH_LOG_RECORD_STRU            g_stFlashLogCfg;
extern OM_FLASH_DEBUG_INFO_STRU            g_stFSLogFileInfo;
extern VOS_UINT32                          g_ulLogMaxCout;
/*****************************************************************************
  4 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  :
 功能描述  :
 输入参数  :
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年5月31日
    作    者   : XXXXXXXX
    修改内容   : V8R1 OM_Optimize项目新增

*****************************************************************************/
#if (VOS_LINUX == VOS_OS_VER)


VOS_UINT32 PPM_FSStartLog(VOS_VOID)
{
    /*如果GU配置保存空间配置为0，则不需要进行保存*/
    if (0 == g_stFlashLogCfg.ulGULogFileSize)
    {
        return VOS_OK;
    }

    if (VOS_OK != OM_FSInitCfgFile())
    {
        return VOS_ERR;
    }

    if (VOS_OK != OM_FSCheckSpace())
    {
        return VOS_ERR;
    }

    g_stFSLogFileInfo.bIsWritten    = VOS_TRUE;
    g_stFSLogFileInfo.lFileHandle   = -1;
    g_stFSLogFileInfo.ulRemainCount = g_ulLogMaxCout;
    g_stFSLogFileInfo.ulFileSize    = OM_FLASH_DEFAULT_FILE_SIZE;
    g_stFSLogFileInfo.ulFileMaxSize = OM_FLASH_DEFAULT_FILE_SIZE;

    return VOS_OK;
}
VOS_UINT32 PPM_FSWriteLogProc(VOS_UINT8 *pucVirAddr, VOS_UINT8 *pucPhyAddr, VOS_UINT32 ulLength)
{
#if (FEATURE_ON == FEATURE_COMPRESS_WRITE_LOG_FILE)
    return OM_CompressRcvLog(pucVirAddr, ulLength);
#else
    return OM_FSWriteLogFile(pucVirAddr, ulLength);
#endif
}

/*****************************************************************************
 函 数 名  : PPM_FSInitLogFile
 功能描述  : 初始化LOG文件
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_ERR
             VOS_OK
  1.日    期   : 2014年5月31日
    作    者   : h59254
    修改内容   : V8R1 OM_Optimize项目新增
*****************************************************************************/
VOS_UINT32 PPM_FSInitLogFile(VOS_VOID)
{
    /*读取Flash保存trace配置*/
    if(NV_OK != NV_Read(en_NV_Item_FLASH_Log_Record_CFG, &g_stFlashLogCfg, sizeof(NV_FLASH_LOG_RECORD_STRU)))
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "PPM_FSInitLogFile:Read NV Config fail!");

        return VOS_OK;
    }

    g_stFSLogFileInfo.bIsWritten    = VOS_FALSE;

    /*如果GU配置保存空间配置为0，则不需要进行保存*/
    if (0 == g_stFlashLogCfg.ulGULogFileSize)
    {
        return VOS_OK;
    }

    CPM_PhySendReg(CPM_FS_PORT, (CPM_SEND_FUNC)PPM_FSWriteLogProc);

    if (VOS_OK != PPM_FSStartLog())
    {
        return VOS_ERR;
    }

    return VOS_OK;
}

#endif

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif




