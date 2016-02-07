

/*****************************************************************************
  1 头文件包含
**************************************************************************** */
#include "OmSdPpm.h"
#include "cpm.h"
#include "NVIM_Interface.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* lint -e767  */
#define    THIS_FILE_ID        PS_FILE_ID_OM_SD_PPM_C
/* lint +e767  */

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */


/*****************************************************************************
  3 外部引用声明
*****************************************************************************/


/*****************************************************************************
  4 函数实现
*****************************************************************************/

VOS_UINT32 PPM_LogFileInit(VOS_VOID)
{
    OM_CHANNLE_PORT_CFG_STRU            stPortCfg;

    /*读取SD Log在NV中的配置*/
    if(NV_OK != NV_Read(en_NV_Item_Om_Port_Type, &stPortCfg, sizeof(OM_CHANNLE_PORT_CFG_STRU)))
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "PPM_LogFileInit:Read NV Config fail!");

        return VOS_OK;
    }

    /* 读取SD Log在NV中的配置为输出到SD卡的时候才使能 */
    if (CPM_OM_PORT_TYPE_SD == stPortCfg.enPortNum)
    {
        OM_SDInit();

        return VOS_OK;
    }
#if (VOS_OS_VER == VOS_LINUX)
    else if (CPM_OM_PORT_TYPE_FS == stPortCfg.enPortNum)
    {
        PPM_FSInitLogFile();

        return VOS_OK;
    }
#endif

    return VOS_OK;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif




