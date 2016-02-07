/*
 *
 * Copyright (C), 2001-2021, Huawei Tech. Co., Ltd.
 *
 * File Name     : hisi_ini.h
 * Version	     : Initial Draft
 * Created	     : 2012/8/26
 * Last Modified :
 * Description   : hisi_ini.c header file
 * Function List :
 * History 	     :
 * 1.Date		 : 2013/10/21
 *   Modification: Created file
 *
 */

#ifndef __OAM_DRV_H__
#define __OAM_DRV_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
 * 1 Other Header File Including
 */


/*
 * 2 Macro Definition
 */

#define INI_KO_MODULE
//#define INI_TEST

//#define INI_FILE_PATH			("./hisi_cfg.ini")

#ifdef INI_KO_MODULE
#define INI_TIME_TEST

#define INI_DEBUG(fmt, arg...) do { \
	printk(KERN_DEBUG "INI_DRV::[%s:%d]"fmt"\n",__FUNCTION__,__LINE__, ##arg); \
} while(0)

#define INI_INFO(fmt, arg...) do { \
	printk(KERN_INFO "INI_DRV::[%s:%d]"fmt"\n",__FUNCTION__,__LINE__,##arg); \
} while(0)

#define INI_WARNING(fmt, arg...) do { \
	printk(KERN_WARNING "INI_DRV::[%s:%d]"fmt"\n",__FUNCTION__,__LINE__,##arg); \
} while(0)

#define INI_ERROR(fmt, arg...) do { \
	printk(KERN_ERR "INI_DRV:: error [%s:%d]"fmt"\n\n\n",__FUNCTION__,__LINE__,##arg); \
} while(0)
#else

#include <cutils/log.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "INI_SO"

#define INI_DEBUG(fmt, arg...) do {\
	LOGD("[%s:%d]"fmt"\n",__FUNCTION__,__LINE__, ##arg); \
} while(0)

#define INI_INFO(fmt, arg...) do {\
	LOGI("[%s:%d]"fmt"\n",__FUNCTION__,__LINE__,##arg); \
} while(0)

#define INI_WARNING(fmt, arg...) do {\
	LOGW("[%s:%d]"fmt"\n",__FUNCTION__,__LINE__,##arg); \
} while(0)

#define INI_ERROR(fmt, arg...) do {\
	LOGE("[%s:%d]"fmt"\n",__FUNCTION__,__LINE__,##arg); \
} while(0)

#define INI_FILE        FILE
typedef unsigned char                   uint8;
typedef char                            int8;
typedef unsigned short                  uint16;
typedef signed short		            int16;
typedef unsigned int                    uint32;
typedef signed int                      int32;
#endif 

#define INI_MODU_WIFI			(0x1)
#define INI_MODU_GNSS			(0x2)
#define INI_MODU_BT				(0x4)
#define INI_MODU_FM				(0x8)
#define INI_MODU_WIFI_PLAT		(0x10)
#define INI_MODU_BFG_PLAT		(0x20)
#define INI_MODU_PLAT			(0x40)


#define INI_MODE_VAR_WIFI			("wifi_ini_mode")
#define INI_MODE_VAR_GNSS			("gnss_ini_mode")
#define INI_MODE_VAR_BT				("bt_ini_mode")
#define INI_MODE_VAR_FM				("fm_ini_mode")
#define INI_MODE_VAR_WIFI_PLAT		("wifi_plat_ini_mode")
#define INI_MODE_VAR_BFG_PLAT		("bfg_plat_ini_mode")

#define INI_VAR_PLAT_BOARD		    ("g_board_version.board_version")
#define INI_VAR_PLAT_PARAM		    ("g_param_version.param_version")

#define INI_STR_WIFI_NORMAL0		("[HOST_WIFI_NORMAL]")
#define INI_STR_DEVICE_WIFI		    ("[DEVICE_WIFI]")

#ifdef INI_KO_MODULE

#define INI_STR_WIFI_NORMAL				("[HOST_WIFI_NORMAL]")
#define INI_STR_WIFI_PERFORMANCE		("[HOST_WIFI_PERFORMANCE]")
#define INI_STR_WIFI_CERTIFY			("[HOST_WIFI_CERTIFY]")
#define INI_STR_WIFI_CERTIFY_CE			("[HOST_WIFI_CERTIFY_CE_ENHANCE]")
#define INI_STR_GNSS_GPSGLONASS			("[HOST_GNSS_GPSGLONASS]")
#define INI_STR_GNSS_BDGPS				("[HOST_GNSS_BDGPS]")
#define INI_STR_BT_NORMAL				("[HOST_BT_NORMAL]")
#define INI_STR_FM_NORMAL				("[HOST_FM_NORMAL]")
#define INI_STR_WIFI_PLAT_NORMAL		("[HOST_WIFI_PLAT_NORMAL]")
#define INI_STR_BFG_PLAT_NORMAL			("[HOST_BFG_PLAT_NORMAL]")
#define INI_STR_PLAT					("[HOST_PLAT]")
#define INI_INIT_MUTEX(mutex)           mutex_init(mutex)
#define INI_MUTEX_LOCK(mutex)           mutex_lock(mutex)
#define INI_MUTEX_UNLOCK(mutex)         mutex_unlock(mutex)

#else

#define INI_SDIOBIN_DEFAULT				("/system/vendor/firmware/SDIO_RW.bin")
#define INI_SDIOBIN_NORMAL				("/system/vendor/firmware/SDIO_RW_Normal.bin")
#define INI_SDIOBIN_PERFORMANCE			("/system/vendor/firmware/SDIO_RW_Performance.bin")
#define INI_SDIOBIN_CERTIFY				("/system/vendor/firmware/SDIO_RW_Certification.bin")

#define INI_STR_WIFI_NORMAL				("[HOST_WIFI_NORMAL]\n")
#define INI_STR_WIFI_PERFORMANCE		("[HOST_WIFI_PERFORMANCE]\n")
#define INI_STR_WIFI_CERTIFY			("[HOST_WIFI_CERTIFY]\n")
#define INI_STR_WIFI_CERTIFY_CE			("[HOST_WIFI_CERTIFY_CE_ENHANCE]\n")
#define INI_STR_GNSS_GPSGLONASS			("[HOST_GNSS_GPSGLONASS]\n")
#define INI_STR_GNSS_BDGPS				("[HOST_GNSS_BDGPS]\n")
#define INI_STR_BT_NORMAL				("[HOST_BT_NORMAL]\n")
#define INI_STR_FM_NORMAL				("[HOST_FM_NORMAL]\n")
#define INI_STR_WIFI_PLAT_NORMAL		("[HOST_WIFI_PLAT_NORMAL]\n")
#define INI_STR_BFG_PLAT_NORMAL			("[HOST_BFG_PLAT_NORMAL]\n")
#define INI_STR_PLAT					("[HOST_PLAT]\n")
#define INI_INIT_MUTEX(mutex, para)     pthread_mutex_init(mutex, para)
#define INI_MUTEX_LOCK(mutex)           pthread_mutex_lock(mutex)
#define INI_MUTEX_UNLOCK(mutex)         pthread_mutex_unlock(mutex)

#endif

#define INI_STR_MODU_LEN		(40)

#define INI_MODE_GPSGLONASS		(0)
#define INI_MODE_BDGPS			(1)
#define INI_MODE_NORMAL			(0)
#define INI_MODE_PERFORMANCE	(1)
#define INI_MODE_CERTIFY 		(2)
#define INI_MODE_CERTIFY_CE 	(3)

#define MAX_READ_LINE_NUM		(192)

#define INI_SUCC_MODE_VAR			(1)
#define INI_SUCC					(0)
#define INI_FAILED					(-1)
/*Hi1101 and Hi1151 version micro definition*/

/*
 * 4 Message Header Definition
 */


/*
 * 5 Message Definition
 */


/*
 * 6 STRUCT Type Definition
 */

#ifdef INI_KO_MODULE
typedef struct ini_board_vervion
{
	unsigned char board_version[16];
}INI_BOARD_VERSION_STRU;

typedef struct ini_param_vervion
{
	unsigned char param_version[16];
}INI_PARAM_VERSION_STRU;
typedef struct file INI_FILE;



/*
 * 7 Global Variable Declaring
 */

extern INI_BOARD_VERSION_STRU g_board_version;
extern INI_PARAM_VERSION_STRU g_param_version;
#endif
/*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */


/*
 * 10 Function Declare
 */

#ifdef INI_KO_MODULE
extern int ini_find_var_value(int modu, char * puc_var, char *puc_value);
#else
extern int32 ini_find_var_value(int modu, char * puc_var, char *puc_value);
extern int32 ini_write_var_value(int32 modu, int8 * puc_var, int8 * puc_value);
#endif

#ifdef __cplusplus
	#if __cplusplus
		}
	#endif
#endif

#endif 
