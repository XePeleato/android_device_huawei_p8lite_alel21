#ifndef __MDRV_ACORE_H__
#define __MDRV_ACORE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*公共头文件*/
#include "mdrv_public.h"
#include "mdrv_version.h"

/*soc*/
#include "mdrv_int.h"
#include "mdrv_wdt.h"
#include "mdrv_timer.h"
#include "mdrv_edma.h"
#include "mdrv_ipc.h"
#include "mdrv_icc.h"
#include "mdrv_memory.h"
#include "mdrv_rtc.h"
#include "mdrv_sysctrl.h"
#include "mdrv_sync.h"
#include "mdrv_pm.h"

/*devcie ip*/
#include "mdrv_hkadc.h"
#include "mdrv_efuse.h"
#include "mdrv_misc.h"
#include "mdrv_ipf.h"

/*封装层*/
#include "mdrv_udi.h"
#include "mdrv_om.h"
#include "mdrv_rfile.h"

/*solution*/
#include "mdrv_sysboot.h"
#include "mdrv_dload.h"
#include "mdrv_chg.h"
#include "mdrv_wifi.h"
#include "mdrv_sd.h"
#if !defined(CHIP_BB_HI6210)
#include "mdrv_mbb_channel.h"
#endif
#include "mdrv_socp.h"
#include "mdrv_usb.h"

/*stub*/
#include "mdrv_stub.h"

#if defined(CHIP_BB_HI6210)
#ifndef _lint
#include "MemoryMap.h"
#endif
#include "drv_mailbox.h" 
#else
#include "drv_mailbox_cfg.h"    /* 多核ICC项目后此头文件删除 */
#endif

#if !defined(CHIP_BB_HI6210)
#include "mdrv_hsuart.h"
#endif

#include "mdrv_crypto.h"

#ifdef __cplusplus
}
#endif
#endif

