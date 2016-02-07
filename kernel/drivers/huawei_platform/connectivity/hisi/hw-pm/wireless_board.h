/******************************************************************************

          Copyright (C), 2001-2011, huawei technology limited company

 ******************************************************************************
  Filename   : wireless_board.h
  Version    : first draft
  Author     : jwx222439
  Create date: 2014-12-01
  Modify     :
  Func descr : inclue file
  Func list  :
  Modi histoy:
  1.Data     : 2014-12-01
    Author   : jwx222439
    Modify
    content  : Create File

******************************************************************************/

#ifndef __WIRELESS_BOARD_H__
#define __WIRELESS_BOARD_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 Include other Head file
*****************************************************************************/
#include "../hw-ps/hw_bfg_ps.h"

/*****************************************************************************
  2 Define macro
*****************************************************************************/
#define SDIO_RW_NAME_LEN            128
#define INI_NAME_LEN            	128

#define RW_SDIO_PATH                "/system/vendor/firmware/"
#define DTS_COMP_HI1101_RW_NAME     "hisilicon,hi1101-power"


/*****************************************************************************
  3 STRUCT DEFINE
*****************************************************************************/
typedef struct file                 OS_KERNEL_FILE_STRU;

enum return_val
{
    BOARD_SUCC = 0,
    BOADR_EFAIL,
};

typedef struct board_info
{
    int8        rw_file_name[SDIO_RW_NAME_LEN];
    int8        ini_file_name[INI_NAME_LEN];
}BOARD_INFO_STRU;

extern BOARD_INFO_STRU g_st_boardid;
extern int32 get_board_info(BOARD_INFO_STRU * pboard_info);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif
