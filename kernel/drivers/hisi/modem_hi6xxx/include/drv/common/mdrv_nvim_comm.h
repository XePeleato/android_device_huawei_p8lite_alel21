#ifndef __MDRV_NVIM_COMMON_H__
#define __MDRV_NVIM_COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <product_config.h>

#if !defined (CHIP_BB_HI6210)

/******************************** NV return value, need to be cleared *********************************/
#define NV_OK                           0
#define NV_ID_NOT_EXIST                 1
#define NV_BUFFER_TOO_LONG              2
#define NV_BUFFER_NOT_EXIST             4
#define NV_WRITE_SECURE_FAIL            18
#define NV_RESTORE_RUNNING              0x5A5A55AA


/******************************** NV恢复表类型 *********************************/
enum NV_RESUME_ITEM
{
    NV_MANUFACTURE_ITEM = 0,
    NV_USER_ITEM,
    NV_SECURE_ITEM,
    NV_ITEM_BUTT
};
typedef unsigned long NV_RESUME_ITEM_ENUM_UINT32;

/*****************************************************************************
 *  结构名    : NV_LIST_INFO_STRU
 *  协议表格  :
 *  ASN.1描述 :
 *  结构说明  : 获取NV列表信息
 ******************************************************************************/
typedef struct tag_NV_LIST_INFO_STRU
{
	unsigned short usNvId;                  /*NVID值*/
	unsigned char  ucNvModemNum;            /*NV是否存在多份*/
	unsigned char  ucRsv;
}NV_LIST_INFO_STRU;  /*back*/

/* 获取NV项数量 */
unsigned int mdrv_nv_get_nvid_num();

/* 获取NV列表 */
unsigned int mdrv_nv_get_nvid_list(NV_LIST_INFO_STRU *pstNvIdList);

/* 读不同Modem NV项数据 */
unsigned int mdrv_nv_readex(unsigned int modemid, unsigned int itemid, void *pdata, unsigned int ulLength);

/* 写不同Modem NV项数据 */
unsigned int mdrv_nv_writeex(unsigned int modemid, unsigned int itemid,void *pdata, unsigned int ulLength);

/* 读NV项数据 */
unsigned int mdrv_nv_read_partex(unsigned int modemid, unsigned int itemid, unsigned int ulOffset, void *pdata, unsigned int ulLength);

/* 写部分NV项数据 */
unsigned int mdrv_nv_write_partex(unsigned int modemid, unsigned int itemid, unsigned int ulOffset, void *pdata, unsigned int ulLength);

/* 将内存中的nv数据刷到flash 文件系统中 */
unsigned int mdrv_nv_flush();

/* 获取NV长度 */
unsigned int mdrv_nv_get_length(unsigned int itemid, unsigned int *pulLength);

/* 读NV项数据 */
unsigned int mdrv_nv_read(unsigned int itemid, void *pdata, unsigned int ulLength);

/* 写NV项数据 */
unsigned int mdrv_nv_write(unsigned int itemid, void *pdata, unsigned int ulLength);

/* 读NV项数据 */
unsigned int mdrv_nv_readpart(unsigned int itemid, unsigned int ulOffset, void *pdata, unsigned int ulLength);

/* 写部分NV项数据 */
unsigned int mdrv_nv_writepart(unsigned int itemid, unsigned int ulOffset, void *pdata, unsigned int ulLength);

/* 获取NV列表 */
unsigned int mdrv_nv_get_resume_nvid_list(unsigned int enNvItem, unsigned short *pusNvList, unsigned int ulNvNum);

/* 获取需要恢复恢复NV项数量 */
unsigned int mdrv_nv_get_resume_nvid_num(unsigned int enNvItem);

/* 备份NV数据 */
unsigned int mdrv_nv_backup();

/* 恢复结果检查 */
unsigned int mdrv_nv_restore_result();

/* 恢复生产NV项 */
unsigned int mdrv_nv_restore();

/*备份出厂NV项*/
unsigned int mdrv_nv_backup_factorynv();

/*恢复出厂NV项*/
unsigned int mdrv_nv_revert_factorynv();

/************************************适配GU对外接口*****************************************/
unsigned int mdrv_nv_revert_manufacturnv(void);

#define NV_GetNVIdListNum()		                    mdrv_nv_get_nvid_num()
#define NV_GetNVIdList(list)		                mdrv_nv_get_nvid_list(list)
#define NV_ReadEx(modemid, id, item, len)	        mdrv_nv_readex(modemid, id, item, len)
#define NV_WriteEx(modemid, id, item, len) 	        mdrv_nv_writeex(modemid, id, item, len)
#define NV_ReadPartEx(modemid, id, off, item, len)	mdrv_nv_read_partex(modemid, id, off, item, len)
#define NV_WritePartEx(modemid, id, off, item, len)	mdrv_nv_write_partex(modemid, id, off, item, len)
#define NV_Flush()	                                mdrv_nv_flush()
#define NV_GetLength(id, len)	                    mdrv_nv_get_length(id, len)
#define NV_GetResumeNvIdList(item, list, num)       mdrv_nv_get_resume_nvid_list(item, list, num)
#define NV_GetResumeNvIdNum(item)	                mdrv_nv_get_resume_nvid_num(item)
#define NV_Backup()	                                mdrv_nv_backup()
#define NV_RestoreResult()	                        mdrv_nv_restore_result()
#define NV_Restore()	                            mdrv_nv_restore()
#define NV_RestoreAll()                             mdrv_nv_restore()
#define NV_QueryRestoreResult()                     mdrv_nv_restore_result()
#define NVM_BackUpFNV()                             mdrv_nv_backup_factorynv()
#define NVM_RevertFNV()                             mdrv_nv_revert_factorynv()
#define NV_RestoreManufactureDefault()              mdrv_nv_revert_factorynv()
#define NV_RestoreManufacture()                     mdrv_nv_revert_manufacturnv()
#define NV_SpecialNvIdBackup(a,b,c)                 (0)
#define NV_RestoreManufactureExt()                  (1)
#define NV_BackupCheck()                            (0)

#if defined (INSTANCE_1)
#define NV_Read(id, item, len)	                    mdrv_nv_readex(MODEM_ID_1, id, item, len)
#define NV_Write(id, item, len)                     mdrv_nv_writeex(MODEM_ID_1, id, item, len)
#define NV_ReadPart(id, off, item, len)             mdrv_nv_read_partex(MODEM_ID_1, id, off, item, len)
#define NV_WritePart(id, off, item, len)            mdrv_nv_write_partex(MODEM_ID_1, id, off, item, len)
#else
#define NV_Read(id, item, len)	                    mdrv_nv_readex(MODEM_ID_0, id, item, len)
#define NV_Write(id, item, len)                     mdrv_nv_writeex(MODEM_ID_0, id, item, len)
#define NV_ReadPart(id, off, item, len)             mdrv_nv_read_partex(MODEM_ID_0, id, off, item, len)
#define NV_WritePart(id, off, item, len)            mdrv_nv_write_partex(MODEM_ID_0, id, off, item, len)
#endif


/************************************适配TL对外接口*****************************************/
#define NVM_Flush()                                 mdrv_nv_flush()
#define NVM_GetItemLength(id, len)	                mdrv_nv_get_length(id, len)
#define NVM_UpgradeBackup(uloption)	                mdrv_nv_backup()
#define NVM_UpgradeRestore()	                    mdrv_nv_restore()

#if defined (INSTANCE_1)
#define NVM_Read(id, item, len)                     mdrv_nv_readex(MODEM_ID_1, id, item, len)
#define NVM_Write(id, item, len)                    mdrv_nv_writeex(MODEM_ID_1, id, item, len)
#else
#define NVM_Read(id, item, len)                     mdrv_nv_readex(MODEM_ID_0, id, item, len)
#define NVM_Write(id, item, len)                    mdrv_nv_writeex(MODEM_ID_0, id, item, len)
#endif

#endif
#ifdef __cplusplus
}
#endif
#endif //__MDRV_NVIM_COMMON_H__
