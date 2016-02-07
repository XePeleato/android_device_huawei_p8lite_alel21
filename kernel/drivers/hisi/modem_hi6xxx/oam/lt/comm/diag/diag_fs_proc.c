/******************************************************************************

   Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : diag_fs_proc.c
  Description     :
  History         :
     1.w00182550       2013-1-28   Draft Enact

******************************************************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
/*lint -save -e537*/
#include  "vos.h"
#include  "msp_errno.h"
#include  "diag_fs_proc.h"
#include  "diag_debug.h"
#include  "diag_common.h"
#include  "diag_cmd_id.h"
#include  "diag_cmd_param_plugin.h"
/*lint -restore*/

/*lint -save -e767 原因:Log打印*/
#define    THIS_FILE_ID        MSP_FILE_ID_DIAG_FS_PROC_C
/*lint -restore +e767*/
/*lint -save -e718 -e746 -e628*/
/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/
#if(VOS_OS_VER == VOS_LINUX)
const VOS_CHAR g_acDiagRootPath[] = "/";
const VOS_CHAR g_acDiagLockedRootPath[] = "/modem_log"; /* 锁定状态下的根目录 */
const VOS_CHAR g_acDiagDumpDir[] = "/modem_log";
const VOS_CHAR g_acDiagLogDir[] = "/modem_log";
#else
const VOS_CHAR g_acDiagRootPath[] = "/yaffs0";
const VOS_CHAR g_acDiagDumpDir[] = "/yaffs2/exc";
const VOS_CHAR g_acDiagLogDir[] = "/yaffs2/log";
#endif

DIAG_FILE_INFO_STRU g_stDiagFileInfo = {DIAG_FILE_NULL, DIAG_FS_FOLDER_BUTT, VOS_NULL_PTR};

#if(VOS_OS_VER == VOS_LINUX)
extern VOS_BOOL g_bAtDataLocked;
#endif
/*****************************************************************************
  3 Function
*****************************************************************************/

#if(VOS_OS_VER == VOS_LINUX)
#define diag_fs_log()\
	printk("diag_fs_log: %d\n", __LINE__)
#else
#define diag_fs_log()
#endif
/*lint -save -e40 -e63*/ /* 内核操作接口不识别*/

/*****************************************************************************
 Function Name   : diag_FsClose
 Description     : 关闭文件
 Input           :VOS_VOID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2013-1-29  Draft Enact

*****************************************************************************/
VOS_VOID diag_FsClose(VOS_VOID)
{
#if(VOS_OS_VER == VOS_LINUX)
    mm_segment_t old_fs;

    old_fs = get_fs();
    set_fs(KERNEL_DS);
#endif
    if (DIAG_FILE_NULL != g_stDiagFileInfo.lFile)
    {
#if(VOS_OS_VER == VOS_LINUX)
        DIAG_FS_CLOSE(g_stDiagFileInfo.lFile);
#else
        DIAG_FS_CLOSE((FILE *)g_stDiagFileInfo.lFile);
#endif
        g_stDiagFileInfo.lFile = DIAG_FILE_NULL;
    }
    
#if(VOS_OS_VER == VOS_LINUX)
    (VOS_VOID)DIAG_FS_FILE_SYNC(g_stDiagFileInfo.lFile);
    set_fs(old_fs);
#endif
}

/*****************************************************************************
 Function Name   : diag_FsGetDirInfo
 Description     : 获取文件、文件夹数目
 Input           :VOS_CHAR *pDirName
                VOS_UINT32 *pulTotalNum
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2013-1-29  Draft Enact

*****************************************************************************/
#if(VOS_OS_VER == VOS_LINUX)
VOS_UINT32  diag_FsGetDirInfo(VOS_CHAR *pDirName, VOS_UINT32 *pulTotalNum)
{
    VOS_INT32 				dir_handle =0;
	VOS_INT32				nRead =0;
	VOS_INT32				nCount =0;
	VOS_UINT32				i=0;
	VOS_UINT8*				buf =NULL;

    DIAG_DIRENT_STRU        *pstTmpDirent;
    mm_segment_t old_fs;

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    /*打开目录*/
    if((dir_handle = DIAG_FS_OPENDIR((VOS_CHAR*)pDirName,DIAG_FS_RDONLY|DIAG_FS_DIRECTORY,0))< 0)
    {
        set_fs(old_fs);
		printk( "[%s]DIAG_FS_OPENDIR error!",__FUNCTION__);
        return VOS_ERR;
    }

	buf =(VOS_UINT8*)VOS_MemAlloc(diag_GetAgentPid(), DYNAMIC_MEM_PT,MSP_DF_DIR_BUF_LEN);
    if(NULL == buf)
    {
        printk( "[%s]Alloc mem error!",__FUNCTION__);
		DIAG_FS_CLOSEDIR(dir_handle);
		set_fs(old_fs);
        return VOS_ERR;
    }
	nRead = DIAG_FS_GETDENTS(dir_handle, (struct linux_dirent __user *)buf, MSP_DF_DIR_BUF_LEN);

	if(-1 == nRead)
	{
		printk("[%s]dents error,nRead=%d!\n",__FUNCTION__,(VOS_INT)nRead);
		*pulTotalNum =0;
		VOS_MemFree(diag_GetAgentPid(), buf);
		DIAG_FS_CLOSEDIR(dir_handle);
		set_fs(old_fs);
		return VOS_ERR;
	}

	if(0 == nRead)
	{
        /*lint -save -e717*/
		diag_printf("[%s]dents zero!\n",__FUNCTION__);
        /*lint -restore*/
		*pulTotalNum = 0;
		VOS_MemFree(diag_GetAgentPid(), buf);
		DIAG_FS_CLOSEDIR(dir_handle);
		set_fs(old_fs);
		return VOS_OK;
	}

	for(i=0; i<(VOS_UINT32)nRead;)
	{
	    pstTmpDirent = (DIAG_DIRENT_STRU*)(buf + i);
		i += pstTmpDirent->d_reclen;

		if((0 == VOS_StrCmp((char *) pstTmpDirent->d_name, "."))
			||(0 == VOS_StrCmp ((char *) pstTmpDirent->d_name, "..")))
        {
            /*lint -save -e717*/
        	diag_printf("diag_FsGetDirInfo:d_name=%S!\n",pstTmpDirent->d_name);
            /*lint -restore*/
            continue;
        }
#if 0
	    if (nCount>=FILENAME_NUM_MAX)
        {
        	DIAG_FS_CLOSEDIR(dir_handle);
	        set_fs(old_fs);
            return VOS_ERR;
        }
#endif
		nCount++;
	}

	*pulTotalNum = nCount;

     /*关闭目录*/
    if (DIAG_FS_ERROR == DIAG_FS_CLOSEDIR(dir_handle))
    {
        VOS_MemFree(diag_GetAgentPid(), buf);
    	printk("[%s]DIAG_FS_CLOSEDIR zero!\n",__FUNCTION__);
        set_fs(old_fs);
        return VOS_ERR;
    }

    VOS_MemFree(diag_GetAgentPid(), buf);

    set_fs(old_fs);
    return VOS_OK;
}

/*****************************************************************************
 Function Name   : diag_FsGetItemInfo
 Description     : 获取文件或文件夹信息
               VOS_CHAR *pDirName
               DIAG_DIR_FILE_INFO_STRU *pstDirFileInfo
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2013-1-29  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_FsGetItemInfo(VOS_CHAR *pDirectory,DIAG_DIR_FILE_INFO_STRU *pstDirFileInfo)
{
    DIAG_DIRENT_STRU        *pstTmpDirent;
    DIAG_STAT_STRU           stStat;
    VOS_UINT8               ucDirName[MSP_DF_DIR_MAX_LEN];
    VOS_INT32 				dir_handle =0;
	VOS_INT32				nRead =0;
	VOS_INT32				i=0;
	VOS_UINT8*				buf =NULL;
    mm_segment_t old_fs;

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    /*再次打开目录*/
    if((dir_handle = DIAG_FS_OPENDIR(pDirectory,DIAG_FS_RDONLY|DIAG_FS_DIRECTORY,0))< 0)
    {
    	printk( "[%s]DIAG_FS_OPENDIR error!",__FUNCTION__);
        set_fs(old_fs);
        return VOS_ERR;

    }

	buf =(VOS_UINT8*)VOS_MemAlloc(diag_GetAgentPid(), DYNAMIC_MEM_PT,MSP_DF_DIR_BUF_LEN);
    if(NULL == buf)
    {
        printk( "[%s]Alloc mem error!",__FUNCTION__);
		DIAG_FS_CLOSEDIR(dir_handle);
		set_fs(old_fs);
        return VOS_ERR;
    }
	nRead = DIAG_FS_GETDENTS(dir_handle, (struct linux_dirent __user *)buf, MSP_DF_DIR_BUF_LEN);
	if((-1 == nRead)||(0 == nRead))
	{
		printk("[%s]dents error,nRead=%d!\n",__FUNCTION__,(VOS_INT)nRead);
		VOS_MemFree(diag_GetAgentPid(), buf);
		DIAG_FS_CLOSEDIR(dir_handle);
		set_fs(old_fs);
		return VOS_ERR;
	}

	/*轮询文件夹将所有文件名保存至全局变量*/
	for(i=0; i<nRead; )
	{
		pstTmpDirent = (DIAG_DIRENT_STRU*)(buf + i);
		i += pstTmpDirent->d_reclen;

		if((0 == VOS_StrCmp((char *) pstTmpDirent->d_name, "."))
			||(0 == VOS_StrCmp ((char *) pstTmpDirent->d_name, "..")))
		{
            /*lint -save -e717*/
			diag_printf("[%s]:d_name=%S!\n",__FUNCTION__,pstTmpDirent->d_name);
            /*lint -restore*/
			continue;
		}

		VOS_StrCpy((VOS_CHAR *)ucDirName, pDirectory);
		strncat((VOS_CHAR *)ucDirName, "/",sizeof(VOS_CHAR));
        strncat((VOS_CHAR *)ucDirName, pstTmpDirent->d_name,strlen(pstTmpDirent->d_name));

        /*通过stat获取文件或文件夹的信息*/
        if (DIAG_FS_ERROR == DIAG_FS_STAT((VOS_CHAR *)ucDirName, &stStat))
        {
        	printk("DIAG_FS_STAT: error!\n");
            pstDirFileInfo->ulItemType = DIAG_FS_ITEM_FILE;
            pstDirFileInfo->st_size    = 0;
            pstDirFileInfo->st_mode    = 0;
            pstDirFileInfo->st_atime   = 0;
            pstDirFileInfo->st_mtime   = 0;
            pstDirFileInfo->st_ctime   = 0;
        }
        else
        {
            /*目录*/

            if (0 != (DIAG_IF_DIR&stStat.mode))
            {

                pstDirFileInfo->ulItemType = DIAG_FS_ITEM_FOLDER;
            }
            /*文件*/
            else
            {
                pstDirFileInfo->ulItemType = DIAG_FS_ITEM_FILE;
            }
            pstDirFileInfo->st_size   = (stStat.size & 0xFFFFFFFF);/* 目前文件大小不会超过32位大小 */
            pstDirFileInfo->st_atime  = stStat.atime.tv_sec;
            pstDirFileInfo->st_mtime  = stStat.mtime.tv_sec;
            pstDirFileInfo->st_ctime  = stStat.ctime.tv_sec;
            pstDirFileInfo->st_mode   = stStat.mode;
        }

        /*文件或文件夹名*/
        VOS_MemSet(pstDirFileInfo->aucDirName, 0,DIAG_CMD_FILE_NAME_LEN);
        VOS_StrCpy(pstDirFileInfo->aucDirName, pstTmpDirent->d_name);

        pstDirFileInfo++;

 	}

    VOS_MemFree(diag_GetAgentPid(), buf);
    DIAG_FS_CLOSEDIR(dir_handle);
    set_fs(old_fs);

    return VOS_OK;

}
#else
VOS_UINT32  diag_FsGetDirInfo(VOS_CHAR *pDirName, VOS_UINT32 *pulTotalNum)
{
    DIAG_DIRENT_STRU	*pstTmpDirent = NULL;
    VOS_UINT32           ulTotalNum = 0;
    VOS_INT32 			 dir_handle =0;
    /*打开目录*/
    /*lint -save -e628 -e64*/
    dir_handle = (VOS_INT32)DIAG_FS_OPENDIR((VOS_CHAR*)pDirName,DIAG_FS_RDONLY|DIAG_FS_DIRECTORY,0);  /* [false alarm]:屏蔽Fortify */
    if(dir_handle < 0)
    {
        return VOS_ERR;
    }

    do
    {
        /*lint -save -e740*/
        pstTmpDirent = (DIAG_DIRENT_STRU *)DIAG_FS_READDIR((VOS_VOID*)dir_handle);
        /*lint -restore*/
        /*readdir的返回值为空，表明目录浏览完毕*/
        if (VOS_NULL_PTR != pstTmpDirent)
        {
            ulTotalNum++;
        }
    }while (VOS_NULL_PTR != pstTmpDirent);

    *pulTotalNum = ulTotalNum;/* [false alarm]: 屏蔽Fortify 错误 */

    /*关闭目录*/
    if (DIAG_FS_ERROR == DIAG_FS_CLOSEDIR((VOS_VOID*)dir_handle))
    {
        return VOS_ERR;
    }
    /*lint -restore*/
    return VOS_OK;
}

VOS_UINT32 diag_FsGetItemInfo(VOS_CHAR *pDirectory,DIAG_DIR_FILE_INFO_STRU *pstDirFileInfo)
{
#if 0
    /*lint -save -e958*/
    DIAG_DIRENT_STRU        *pstTmpDirent;
    DIAG_STAT_STRU           stStat;
    VOS_UINT8              *pDirName;
    VOS_UINT16              usDirLen;
    VOS_UINT32              ultemp_len = 0;
    VOS_INT32 				dir_handle =0;
    /*lint -restore*/
    /*再次打开目录*/
    /*lint -save -e64*/
    dir_handle = (VOS_INT32)DIAG_FS_OPENDIR(pDirectory,DIAG_FS_RDONLY|DIAG_FS_DIRECTORY,0);  /* [false alarm]:屏蔽Fortify */
    if(dir_handle < 0)
    {
        return VOS_ERR;
    }

    /*分配空间用来存放文件或文件夹的路径，加1是因为后面需要添加斜杠*/
    pDirName = (VOS_UINT8*)VOS_MemAlloc(diag_GetAgentPid(), DYNAMIC_MEM_PT,MSP_DF_DIR_MAX_LEN);
    if (VOS_NULL_PTR == pDirName)
    {
        DIAG_FS_CLOSEDIR((VOS_VOID*)dir_handle);
        return VOS_ERR;

    }

    /*将目录路径拷贝进文件路径中*/
    usDirLen = (VOS_UINT16)VOS_StrLen((VOS_CHAR*)pDirectory);
    VOS_MemCpy(pDirName, pDirectory, usDirLen);

    /*由于目录路径是不以斜杠结束，在与文件名结合时，需要添加斜杠*/
    pDirName[usDirLen] = '/';
    usDirLen++;

    do
    {
        /*遍历整个pstDir指向路径中的所有文件和文件夹*/
        pstTmpDirent = (DIAG_DIRENT_STRU *)DIAG_FS_READDIR((VOS_VOID*)dir_handle);
        /*lint -restore*/
        /*readdir的返回值为空，表明目录浏览完毕*/
        if (VOS_NULL_PTR != pstTmpDirent)
        {
            /*得到文件或文件夹名的长度*/
            ultemp_len = (VOS_UINT16)VOS_StrLen(pstTmpDirent->d_name);  /* [false alarm]:屏蔽Fortify */

            /*由于文件或文件夹路径需要以'\0'作为结束，所以在Copy时，长度加1*/
            VOS_MemCpy(pDirName + usDirLen,pstTmpDirent->d_name, ultemp_len + 1);

            /*通过stat获取文件或文件夹的信息*/
            if (DIAG_FS_ERROR == DIAG_FS_STAT((VOS_CHAR*)pDirName, &stStat))
            {
                pstDirFileInfo->ulItemType = DIAG_FS_ITEM_FILE;
                pstDirFileInfo->st_size    = 0;
                pstDirFileInfo->st_mode    = 0;
                pstDirFileInfo->st_atime   = 0;
                pstDirFileInfo->st_mtime   = 0;
                pstDirFileInfo->st_ctime   = 0;
            }
            else
            {
                /*目录*/
                if (0 != (DIAG_IF_DIR&stStat.st_mode))
                {

                    pstDirFileInfo->ulItemType = DIAG_FS_ITEM_FOLDER;
                }
                /*文件*/
                else
                {
                    pstDirFileInfo->ulItemType = DIAG_FS_ITEM_FILE;
                }
                pstDirFileInfo->st_size   = (stStat.st_size & 0xFFFFFFFF);/* 目前文件大小不会超过32位大小 */
                pstDirFileInfo->st_atime  = (VOS_INT32)(stStat.st_atime);
                pstDirFileInfo->st_mtime  = (VOS_INT32)(stStat.st_mtime);
                pstDirFileInfo->st_ctime  = (VOS_INT32)(stStat.st_ctime);
                pstDirFileInfo->st_mode   = stStat.st_mode;
            }

            /*文件或文件夹名*/
            VOS_MemCpy(pstDirFileInfo->aucDirName, pstTmpDirent->d_name, ultemp_len +1);
            /* coverity[suspicious_pointer_arithmetic] */
            pstDirFileInfo = pstDirFileInfo + sizeof(DIAG_DIR_FILE_INFO_STRU );
    /*lint -save -e525*/
      }
    }while(VOS_NULL_PTR != pstTmpDirent);
    /*lint -restore*/
    /*lint -save -e50 -e64*/
    VOS_MemFree(diag_GetAgentPid(), pDirName);
    DIAG_FS_CLOSEDIR((VOS_VOID*)dir_handle);
    /*lint -restore*/
#endif
    return VOS_OK;

}

#endif

VOS_INT32 diag_FsOpen(const VOS_CHAR *pcFileName, VOS_INT lFlag)
{
#if((VOS_OS_VER == VOS_VXWORKS) || (VOS_OS_VER == VOS_RTOSCK))
//    VOS_CHAR    *pucMode;
#endif
    VOS_INT32  fp = 0;
#if(VOS_OS_VER == VOS_LINUX)
    mm_segment_t old_fs;

    old_fs = get_fs();
    set_fs(KERNEL_DS);
#endif
#if((VOS_OS_VER == VOS_VXWORKS) || (VOS_OS_VER == VOS_RTOSCK))
#if 0
    switch(lFlag)
    {
        case (DIAG_FS_CREAT|DIAG_FS_APPEND|DIAG_FS_RDWR):
            pucMode = "ab+";
            break;

        case (DIAG_FS_CREAT|DIAG_FS_APPEND|DIAG_FS_WRONLY):
            pucMode = "ab";
            break;

        case (DIAG_FS_CREAT|DIAG_FS_TRUNC|DIAG_FS_RDWR):
            pucMode = "wb+";
            break;

        case (DIAG_FS_CREAT|DIAG_FS_TRUNC|DIAG_FS_WRONLY):
            pucMode = "wb";
            break;

        case DIAG_FS_RDWR:
            pucMode = "rb+";
            break;

        case DIAG_FS_RDONLY:
            pucMode = "rb";
            break;

        default:
            return DIAG_FILE_NULL;
    }
#endif
#endif
#if(VOS_OS_VER == VOS_LINUX)
    fp = DIAG_FS_OPEN(pcFileName, lFlag,0755);
#endif
#if((VOS_OS_VER == VOS_VXWORKS) || (VOS_OS_VER == VOS_RTOSCK))
//    fp = (VOS_INT32)DIAG_FS_OPEN(pcFileName, lFlag, pucMode);
#endif
	diag_fs_log();

#if(VOS_OS_VER == VOS_LINUX)
        set_fs(old_fs);
#endif

    return (VOS_INT32)fp;
}

/*****************************************************************************
 Function Name   : diag_FsQueryProc
 Description     : 查询根目录
 Input           :VOS_UINT8* pstReq
                VOS_UINT32 ulCmdId
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2013-1-29  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_FsQueryProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId)
{
    DIAG_CMD_FS_QUERY_REQ* pstFsReq = NULL;
    DIAG_CMD_FS_QUERY_CNF stFsCnf = {0};
    VOS_UINT32 ret = ERR_MSP_SUCCESS;

    pstFsReq = (DIAG_CMD_FS_QUERY_REQ*)(DIAG_OFFSET_HEAD_GET_DATA(pstReq));

    if (DIAG_FS_ROOT_FOLDER == pstFsReq->ulFolderType)
    {
#if(VOS_OS_VER == VOS_LINUX)
        /* 锁定状态下，根目录受限制 */
        if(g_bAtDataLocked)
        {
            stFsCnf.ulLength = VOS_StrLen((VOS_CHAR*)g_acDiagLockedRootPath);
            VOS_MemCpy(stFsCnf.aucDirPath,g_acDiagLockedRootPath, stFsCnf.ulLength);
        }
        else
        {
            stFsCnf.ulLength = VOS_StrLen((VOS_CHAR*)g_acDiagRootPath);
            VOS_MemCpy(stFsCnf.aucDirPath,g_acDiagRootPath, stFsCnf.ulLength);
        }
#else
            stFsCnf.ulLength = VOS_StrLen((VOS_CHAR*)g_acDiagRootPath);
            VOS_MemCpy(stFsCnf.aucDirPath,g_acDiagRootPath, stFsCnf.ulLength);
#endif
        g_stDiagFileInfo.ulFileType = DIAG_FS_ROOT_FOLDER;

    }
    else if (DIAG_FS_LOG_FOLDER == pstFsReq->ulFolderType)
    {
        stFsCnf.ulLength = VOS_StrLen((VOS_CHAR*)g_acDiagLogDir);
        VOS_MemCpy(stFsCnf.aucDirPath,g_acDiagLogDir, stFsCnf.ulLength);
        g_stDiagFileInfo.ulFileType = DIAG_FS_LOG_FOLDER;
    }
    /*临终遗言文件所在的路径*/
    else if (DIAG_FS_DUMP_FOLDER == pstFsReq->ulFolderType)
    {
        stFsCnf.ulLength = VOS_StrLen((VOS_CHAR*)g_acDiagDumpDir);
        VOS_MemCpy(stFsCnf.aucDirPath, g_acDiagDumpDir, stFsCnf.ulLength);
        g_stDiagFileInfo.ulFileType = DIAG_FS_DUMP_FOLDER;
    }
    else
    {
        ret = ERR_MSP_FAILURE;
    }
    stFsCnf.ulRet = ret;

    /*打包回复给FW*/
    ret = diag_AgentCnfFun((VOS_UINT8*)&stFsCnf,ulCmdId,sizeof(DIAG_CMD_FS_QUERY_CNF));
    return ret;

}

/*****************************************************************************
 Function Name   : diag_FsScanProc
 Description     : 扫描所有文件或文件夹信息
 Input           :VOS_UINT8* pstReq
                VOS_UINT32 ulCmdId
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2013-1-29  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_FsScanProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId)
{
    DIAG_CMD_FS_SCAN_DIR_REQ* pstFsReq = NULL;
    DIAG_CMD_FS_SCAN_DIR_CNF* pstFsCnf = NULL;
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
    VOS_UINT32              ulTotalNum;
    VOS_UINT32              ulTotalSize;

    pstFsReq = (DIAG_CMD_FS_SCAN_DIR_REQ*)(DIAG_OFFSET_HEAD_GET_DATA(pstReq));

   /*得到目录内，文件和文件夹的总个数、总的名字长度*/
    if(VOS_OK != diag_FsGetDirInfo(pstFsReq->szDirectory,&ulTotalNum))
    {
        return VOS_ERR;
    }

    /*计算返回给工具侧消息包的长度*/
    ulTotalSize = (ulTotalNum* sizeof(DIAG_DIR_FILE_INFO_STRU)+ sizeof(DIAG_CMD_FS_SCAN_DIR_CNF));

    pstFsCnf = (DIAG_CMD_FS_SCAN_DIR_CNF*)VOS_MemAlloc(diag_GetAgentPid(), DYNAMIC_MEM_PT, ulTotalSize);
    if (VOS_NULL_PTR == pstFsCnf)
    {
        return VOS_ERR;
    }

    /*获取每个文件和文件夹的信息*/
    /*lint -save -e539*/
	if(ulTotalNum>0)
	{
    	ret = diag_FsGetItemInfo(pstFsReq->szDirectory,pstFsCnf->stDirInfo);
	}
    /*lint -restore*/

    pstFsCnf->ulRet = ret;
    pstFsCnf->ulDirNum= ulTotalNum;
    pstFsCnf->ulDirInfoLen= ulTotalNum * sizeof(DIAG_DIR_FILE_INFO_STRU);

    /*打包回复给FW*/
    ret = diag_AgentCnfFun((VOS_UINT8*)pstFsCnf,ulCmdId,ulTotalSize);
    /*lint -save -e50*/
    VOS_MemFree(diag_GetAgentPid(), pstFsCnf);
    /*lint -restore*/
    return ret;

}

/*****************************************************************************
 Function Name   : diag_FsMkdirProc
 Description     : 创建目录
 Input           :VOS_UINT8* pstReq
                VOS_UINT32 ulCmdId
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2013-1-29  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_FsMkdirProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId)
{
    DIAG_CMD_FS_MAKE_DIR_REQ* pstFsReq = NULL;
    DIAG_CMD_FS_MAKE_DIR_CNF stFsCnf = {0};
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
#if(VOS_OS_VER == VOS_LINUX)
    mm_segment_t old_fs;

    old_fs = get_fs();
    set_fs(KERNEL_DS);
#endif

    pstFsReq = (DIAG_CMD_FS_MAKE_DIR_REQ*)(DIAG_OFFSET_HEAD_GET_DATA(pstReq));

    ret = (VOS_UINT32)DIAG_FS_MKDIR((VOS_CHAR*)pstFsReq->szDirectory,0755);

    stFsCnf.ulRet = ret;
#if(VOS_OS_VER == VOS_LINUX)
    set_fs(old_fs);
#endif

    /*打包回复给FW*/
    ret = diag_AgentCnfFun((VOS_UINT8*)&stFsCnf,ulCmdId,sizeof(DIAG_CMD_FS_MAKE_DIR_CNF));

    return ret;

}

/*****************************************************************************
 Function Name   : diag_FsOpenProc
 Description     : 打开或创建文件
 Input           :VOS_UINT8* pstReq
                VOS_UINT32 ulCmdId
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2013-1-29  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_FsOpenProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId)
{
    DIAG_CMD_FS_OPEN_REQ* pstFsReq = NULL;
    DIAG_CMD_FS_OPEN_CNF stFsCnf = {0};
    VOS_UINT32 ret = ERR_MSP_SUCCESS;

    pstFsReq = (DIAG_CMD_FS_OPEN_REQ*)(DIAG_OFFSET_HEAD_GET_DATA(pstReq));

    /*不支持操作文件的重入操作*/
    if (DIAG_FILE_NULL != g_stDiagFileInfo.lFile)
    {
        diag_printf("warning: last File import not finished\n");
        diag_FsClose();
    }
    /*lint -save -e539*/
	diag_fs_log();
    /*lint -restore*/
    g_stDiagFileInfo.lFile = diag_FsOpen(pstFsReq->szDirectory, (VOS_INT)(pstFsReq->ulMode));

    if (g_stDiagFileInfo.lFile < 0)
    {
    	g_stDiagFileInfo.lFile = DIAG_FILE_NULL;
        ret = VOS_ERR;
    }
	else
	{
	    /*启动定时器*/
	    g_stDiagFileInfo.hTimer = VOS_NULL_PTR;

	    ret = VOS_StartRelTimer(&g_stDiagFileInfo.hTimer, diag_GetAgentPid(), DIAG_CMD_FILE_OPS_TIME_OUT_LEN, 0, DIAG_TMR_FILE_OP_TIMEOUT_PARAM,VOS_RELTIMER_NOLOOP, VOS_TIMER_NO_PRECISION);
        if(ret != ERR_MSP_SUCCESS)
        {
            /*lint -save -e717*/
            diag_printf("VOS_StartRelTimer fail [%s]\n",__func__);
            /*lint -restore*/
        }
        diag_fs_log();
	}
    stFsCnf.ulRet = ret;

    /*打包回复给FW*/
    ret = diag_AgentCnfFun((VOS_UINT8*)&stFsCnf,ulCmdId,sizeof(DIAG_CMD_FS_OPEN_CNF));
	diag_fs_log();

    return ret;

}
/*****************************************************************************
 Function Name   : diag_FsImportProc
 Description     : 导入文件
 Input           :VOS_UINT8* pstReq
                VOS_UINT32 ulCmdId
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2013-1-29  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_FsImportProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId)
{
    DIAG_CMD_FS_IMPORT_REQ* pstFsReq = NULL;
    DIAG_CMD_FS_IMPORT_CNF stFsCnf = {0};
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
    VOS_INT32 lResult =0;
#if(VOS_OS_VER == VOS_LINUX)
    mm_segment_t old_fs;
#endif

    pstFsReq = (DIAG_CMD_FS_IMPORT_REQ*)(DIAG_OFFSET_HEAD_GET_DATA(pstReq));

    if (DIAG_FILE_NULL == g_stDiagFileInfo.lFile)
    {
        return ERR_MSP_FAILURE;
    }
    /*lint -save -e539*/
	diag_fs_log();
    /*lint -restore*/
    /*停止定时器*/
    VOS_StopRelTimer(&g_stDiagFileInfo.hTimer);

    /*写文件完毕，关闭文件*/
    if (0 == pstFsReq->ulSize)
    {

		diag_fs_log();
        diag_FsClose();
        /*打包回复给FW*/
        stFsCnf.ulRet = ERR_MSP_SUCCESS;
        ret = diag_AgentCnfFun((VOS_UINT8*)&stFsCnf,ulCmdId,sizeof(DIAG_CMD_FS_IMPORT_CNF));
        return ret;
    }

#if(VOS_OS_VER == VOS_LINUX)
    old_fs = get_fs();
    set_fs(KERNEL_DS);
#endif

#if(VOS_OS_VER == VOS_LINUX)
    lResult = DIAG_FS_WRITE(g_stDiagFileInfo.lFile, pstFsReq->acContent, pstFsReq->ulSize);
#else
    lResult = DIAG_FS_WRITE((FILE *)g_stDiagFileInfo.lFile, pstFsReq->acContent, pstFsReq->ulSize);
#endif

	diag_fs_log();

#if(VOS_OS_VER == VOS_LINUX)
    set_fs(old_fs);
#endif

    /*写文件操作失败或者写入长度不正确*/
    if ((DIAG_FS_ERROR == lResult)||(lResult != (VOS_INT32)(pstFsReq->ulSize)))
    {
		diag_fs_log();
        /*lint -save -e717*/
		diag_printf("[%s]!,lResult=%d\n",__FUNCTION__,lResult);
        /*lint -restore*/
		diag_FsClose();
        return VOS_ERR;
    }

    /*启动定时器*/
    ret = VOS_StartRelTimer(&g_stDiagFileInfo.hTimer, diag_GetAgentPid(),DIAG_CMD_FILE_OPS_TIME_OUT_LEN,0,DIAG_TMR_FILE_OP_TIMEOUT_PARAM,VOS_RELTIMER_NOLOOP, VOS_TIMER_NO_PRECISION);
    if(ret != ERR_MSP_SUCCESS)
    {
        /*lint -save -e717*/
        diag_printf("VOS_StartRelTimer fail [%s]\n",__func__);
        /*lint -restore*/
    }

    stFsCnf.ulRet = ret;

    /*打包回复给FW*/
    ret = diag_AgentCnfFun((VOS_UINT8*)&stFsCnf,ulCmdId,sizeof(DIAG_CMD_FS_IMPORT_CNF));
    return ret;

}

/*****************************************************************************
 Function Name   : diag_FsExportProc
 Description     : 导出文件
 Input           :VOS_UINT8* pstReq
                VOS_UINT32 ulCmdId
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2013-1-29  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_FsExportProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId)
{
    //DIAG_CMD_FS_EXPORT_REQ* pstFsReq = NULL;
    DIAG_CMD_FS_EXPORT_CNF *pstFsCnf = NULL;
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
    VOS_UINT32 ulReadSize = 0;
#if(VOS_OS_VER == VOS_LINUX)
    mm_segment_t old_fs;
#endif


    if (DIAG_FILE_NULL == g_stDiagFileInfo.lFile)
    {

		diag_fs_log();
         return ERR_MSP_FAILURE;
    }

    /*停止定时器*/
    VOS_StopRelTimer(&g_stDiagFileInfo.hTimer);
	diag_fs_log();

    pstFsCnf = (DIAG_CMD_FS_EXPORT_CNF*)VOS_MemAlloc(diag_GetAgentPid(), DYNAMIC_MEM_PT, sizeof(DIAG_CMD_FS_EXPORT_CNF)+DIAG_TRANSFER_BLOCK_MAX_SIZE);
    if (VOS_NULL_PTR == pstFsCnf)
    {

		diag_fs_log();
        diag_FsClose();
        return ERR_MSP_FAILURE;

    }

#if(VOS_OS_VER == VOS_LINUX)
    old_fs = get_fs();
    set_fs(KERNEL_DS);
#endif

#if(VOS_OS_VER == VOS_LINUX)
    ulReadSize = (VOS_UINT32)DIAG_FS_READ(g_stDiagFileInfo.lFile,(VOS_CHAR*)pstFsCnf + sizeof(DIAG_CMD_FS_EXPORT_CNF), DIAG_TRANSFER_BLOCK_MAX_SIZE);
#else
    ulReadSize = (VOS_UINT32)DIAG_FS_READ((FILE *)g_stDiagFileInfo.lFile,(VOS_CHAR*)pstFsCnf + sizeof(DIAG_CMD_FS_EXPORT_CNF), DIAG_TRANSFER_BLOCK_MAX_SIZE);
#endif

	diag_fs_log();

#if(VOS_OS_VER == VOS_LINUX)
    set_fs(old_fs);
#endif

    /*读取文件出现错误*/
    if (DIAG_FS_ERROR == (VOS_INT32)ulReadSize)
    {

		diag_fs_log();
        /*lint -save -e50*/
        VOS_MemFree(diag_GetAgentPid(), pstFsCnf);
        /*lint -restore*/
        diag_FsClose();
        return ERR_MSP_FAILURE;

    }

    /*表明已经没有内容可以读取，文件内容全部读完*/
    if (0 == ulReadSize)
    {

		diag_fs_log();
        diag_FsClose();
    }
    else
    {

		diag_fs_log();
        /*启动定时器*/
        g_stDiagFileInfo.hTimer = VOS_NULL_PTR;
        ret = VOS_StartRelTimer(&g_stDiagFileInfo.hTimer, diag_GetAgentPid(), DIAG_CMD_FILE_OPS_TIME_OUT_LEN, 0, DIAG_TMR_FILE_OP_TIMEOUT_PARAM, VOS_RELTIMER_NOLOOP, VOS_TIMER_NO_PRECISION);
        if(ret != ERR_MSP_SUCCESS)
        {
            /*lint -save -e717*/
            diag_printf("VOS_StartRelTimer fail [%s]\n", __func__);
            /*lint -restore*/
        }

	}

    pstFsCnf->ulRet = ret;
    pstFsCnf->ulSize = ulReadSize;

    /*打包回复给FW*/
    ret = diag_AgentCnfFun((VOS_UINT8*)pstFsCnf,ulCmdId,sizeof(DIAG_CMD_FS_EXPORT_CNF)+ulReadSize);
    /*lint -save -e50*/
    VOS_MemFree(diag_GetAgentPid(), pstFsCnf);
    /*lint -restore*/
	diag_fs_log();
    return ret;

}

/*****************************************************************************
 Function Name   : diag_FsDeleteProc
 Description     : 删除文件或文件夹
 Input           :VOS_UINT8* pstReq
                VOS_UINT32 ulCmdId
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2013-1-29  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_FsDeleteProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId)
{
    DIAG_CMD_FS_DELETE_REQ* pstFsReq = NULL;
    DIAG_CMD_FS_DELETE_CNF stFsCnf = {0};
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
#if(VOS_OS_VER == VOS_LINUX)
    mm_segment_t old_fs;

    old_fs = get_fs();
    set_fs(KERNEL_DS);
#endif

    pstFsReq = (DIAG_CMD_FS_DELETE_REQ*)(DIAG_OFFSET_HEAD_GET_DATA(pstReq));

    if (DIAG_FS_ITEM_FOLDER == pstFsReq->ulItemType)
    {
        ret = (VOS_UINT32)DIAG_FS_RMDIR(pstFsReq->szDirectory);
    }
    else if (DIAG_FS_ITEM_FILE == pstFsReq->ulItemType)
    {
        ret = (VOS_UINT32)DIAG_FS_RMFILE(pstFsReq->szDirectory);
    }

#if(VOS_OS_VER == VOS_LINUX)
    set_fs(old_fs);
#endif

    stFsCnf.ulRet = ret;

    /*打包回复给FW*/
    ret = diag_AgentCnfFun((VOS_UINT8*)&stFsCnf,ulCmdId,sizeof(DIAG_CMD_FS_DELETE_CNF));


    return ret;

}

/*****************************************************************************
 Function Name   : diag_FsSpaceProc
 Description     : 查询使用及可用空间
 Input           :VOS_UINT8* pstReq
                VOS_UINT32 ulCmdId
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2013-1-29  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_FsSpaceProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId)
{
    DIAG_CMD_FS_SPACE_REQ* pstFsReq = NULL;
    DIAG_CMD_FS_SPACE_CNF stFsCnf = {0};
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
#if(VOS_OS_VER == VOS_LINUX)
   mm_segment_t old_fs;

    old_fs = get_fs();
    set_fs(KERNEL_DS);
#endif
    pstFsReq = (DIAG_CMD_FS_SPACE_REQ*)(DIAG_OFFSET_HEAD_GET_DATA(pstReq));

    ret = (VOS_UINT32)DRV_FILE_GET_DISKSPACE((VOS_CHAR*)(pstFsReq->szDirectory),(VOS_UINT*)&stFsCnf.ulDiskSpace,
                    (VOS_UINT*)&stFsCnf.ulUsedSpace,(VOS_UINT*)&stFsCnf.ulValidSpace);

    stFsCnf.ulRet = ret;
#if(VOS_OS_VER == VOS_LINUX)
    set_fs(old_fs);
#endif

    /*打包回复给FW*/
    ret = diag_AgentCnfFun((VOS_UINT8*)&stFsCnf,ulCmdId,sizeof(DIAG_CMD_FS_SPACE_CNF));


    return ret;

}

/*****************************************************************************
 Function Name   : diag_FsProcEntry
 Description     : 文件处理入口
 Input           :VOS_UINT8* pstReq
                VOS_UINT32 ulCmdId
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2013-1-29  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_FsProcEntry (VOS_UINT8* pstReq , VOS_UINT32 ulCmdId)
{
    VOS_UINT32 ret = ERR_MSP_SUCCESS;

    switch(MSP_STRU_ID_0_15_CMD_CATEGORY(ulCmdId))
    {
        case DIAG_CMD_FS_QUERY_DIR:
            ret = diag_FsQueryProc(pstReq,ulCmdId);
            break;
        case DIAG_CMD_FS_SCAN_DIR:
            ret = diag_FsScanProc(pstReq,ulCmdId);
            break;
        case DIAG_CMD_FS_MAKE_DIR:
            ret = diag_FsMkdirProc(pstReq,ulCmdId);
            break;
        case DIAG_CMD_FS_OPEN:
            ret = diag_FsOpenProc(pstReq,ulCmdId);
            break;
        case DIAG_CMD_FS_IMPORT:
            ret = diag_FsImportProc(pstReq,ulCmdId);
            break;
        case DIAG_CMD_FS_EXPORT:
            ret = diag_FsExportProc(pstReq,ulCmdId);
            break;
        case DIAG_CMD_FS_DELETE:
            ret = diag_FsDeleteProc(pstReq,ulCmdId);
            break;
        case DIAG_CMD_FS_SPACE:
            ret = diag_FsSpaceProc(pstReq,ulCmdId);
            break;
        default:
            break;

    }
    return ret;
}
/*lint -restore*/







#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

