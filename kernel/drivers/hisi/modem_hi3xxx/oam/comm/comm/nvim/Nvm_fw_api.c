#include "msp_errno.h"
#include "msp_nvim.h"
#include "product_config.h"

#if (FEATURE_SOCP_SYNC_ENABLE == FEATURE_ON)

VOS_UINT32 NVM_RevertFNV(VOS_VOID)
{
    return 0;
}

VOS_UINT32 NVM_BackUpFNV(VOS_VOID)
{
    return 0;
}

VOS_UINT32 NVM_SetFactoryDefault(VOS_VOID)
{
    return 0;
}
#if 0
VOS_UINT32 nvim_GetInitExportFileInfo(NVIM_EXPORT_FILE_INFO_STRU*pExportFileInfo)
{
    return 0;
}

VOS_UINT32 nvim_GetWorkExportFileInfo(NVIM_EXPORT_FILE_INFO_STRU *pExportFileInfo)
{
    return 0;
}
#endif
VOS_UINT32 nvim_DiagDownLoad( VOS_VOID* pFile,VOS_UINT32 ulFileLen)
{
    return 0;
}

VOS_BOOL Lnvm_IsRestoreOK(VOS_VOID)
{
    return 0;
}

VOS_UINT32 NVM_AutoRestoreNV(VOS_VOID)
{
    return 0;
}

VOS_BOOL NVM_IsReStoreFunAll(VOS_VOID)
{
    return 0;
}

VOS_UINT32 NVIM_WriteFactoryForce(VOS_UINT32 ulId, const VOS_VOID* pData,VOS_UINT16 usDataLen)
{
    return 0;
}

VOS_UINT32 NVM_UpgradeBackup(VOS_UINT32 ulOption)
{
    return 0;
}

#if 0
NVIM_REVERT_STATE_EN nvim_GetRevertState(VOS_VOID )
{
    return 0;
}

#endif

VOS_UINT32 NVM_UpgradeRestore(VOS_VOID)
{
    return 0;
}

VOS_VOID nvim_GetAuthorityType(VOS_UINT32 *pData)
{
    return ;
}

VOS_VOID nvim_GetAuthorityVer(VOS_UINT32 *pData)
{
    return ;
}

VOS_UINT32 nvim_RestoreVersionSetting(VOS_VOID)
{
    return 0;
}

#endif

