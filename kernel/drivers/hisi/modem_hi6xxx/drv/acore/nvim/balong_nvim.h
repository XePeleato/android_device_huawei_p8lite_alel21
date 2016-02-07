
#ifndef _BALONG_NVIM_H_
#define _BALONG_NVIM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define NV_ERROR               -1
#define NV_OK                   0
#define NV_ID_NOT_EXIST         1
#define NV_BUFFER_TOO_LONG      2
#define NV_PART_DATA_ERR        3
#define NV_BUFFER_NOT_EXIST     4
#define NV_NEED_AUTH            5
#define NV_WRITE_NUMBER_END     6
#define NV_AUTH_NUMBER_END      7
#define NV_AUTH_ERR             8
#define NV_READ_UNABLE          9
#define NV_WRITE_UNABLE         10
#define NV_ALLOC_BUFFER_FAIL    11
#define NV_OPEN_FILE_FAIL       12
#define NV_WRITE_FLASH_FAIL     13
#define NV_NO_BACKUP            14
#define NV_FOLDER_CREAT_FAIL    15
#define NV_FILE_RESTORE_FAIL    16
#define NV_FOLDER_DEL_FAIL      17
#define NV_IFC_ERROR            253
#define NV_NOT_READY            254
#define NV_READ_TIMEOUT         255
#define NV_UNKNOWN_ERROR        256


/**************************************************************************
  º¯ÊýÉùÃ÷
**************************************************************************/
int DR_NV_Read(unsigned short usID,void *pItem,unsigned int ulLength);
int DR_NV_Write(unsigned short usID,void *pItem,unsigned int ulLength);

#ifdef __cplusplus
}
#endif

#endif


