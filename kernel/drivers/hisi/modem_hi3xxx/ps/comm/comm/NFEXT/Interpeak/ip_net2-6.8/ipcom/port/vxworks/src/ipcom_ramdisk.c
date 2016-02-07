/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_ramdisk.c,v $ $Revision: 1.3 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_ramdisk.c,v $
 *     $Author: lob $ $Date: 2007-02-27 11:41:05 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * ipcom_ramdisk.c contains source code for initializing RAM disk support. The
 * module is intended to de-couple RAM disk support from IPCOM so that RAM
 * disk support can be scaled in/out at link time.
 *
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"

#if IPCOM_USE_FILE == IPCOM_FILE_NATIVE && defined(IPCOM_USE_RAM_DISK)

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipcom_type.h"
#include "ipcom_file.h"
#include "ipcom_clib.h"
#include "ipcom_syslog.h"
#include "ipcom_err.h"

#include <string.h>
#include <ramDrv.h>
#include <dosFsLib.h>
#if IP_PORT_VXWORKS >= 62
#include <xbdRamDisk.h>
#else
#include <cbioLib.h>
#endif


/*
 ****************************************************************************
 * 4                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    DEFINES
 ****************************************************************************
 */

#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#undef IPCOM_SYSLOG_FACILITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM_DRV_ETH


/*
 ****************************************************************************
 * 6                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                    FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    vxfs_ramdisk_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
vxfs_ramdisk_init(void)
{
    char rd_dev_name [sizeof(IPCOM_FILE_ROOT) + 4];
    int  rd_dev_name_len;

    strlcpy (rd_dev_name, IPCOM_FILE_ROOT, sizeof(IPCOM_FILE_ROOT)+1);
    rd_dev_name_len = strlen (rd_dev_name);

    /* Remove the trailing slash */
    if (rd_dev_name[rd_dev_name_len-1] == '/' )
        rd_dev_name[rd_dev_name_len-1] = '\0';

    if (xbdRamDiskDevCreate(IPCOM_RAM_DISK_BLOCK_SIZE,
                            (IPCOM_RAM_DISK_NO_BLOCK * IPCOM_RAM_DISK_BLOCK_SIZE),
                            FALSE, rd_dev_name) == NULLDEV)
    {
        IPCOM_LOG1(ERR, "ipcom_ramdisk.c: vxfs_ramdisk_init, "
                   "xbdRamDiskDevCreate() failed, errno=%d",
                   ipcom_errno);
        return;
    }

    if (dosFsVolFormat (rd_dev_name, DOS_OPT_BLANK, NULL) != IPCOM_SUCCESS )
    {
        IPCOM_LOG1(ERR, "ipcom_ramdisk.c: vxfs_ramdisk_init, "
                   "dosFsVolFormat() failed, errno=%d",
                   ipcom_errno);
        return;
    }
}

#else
int ipcom_ramdisk_empty_file;
#endif /* #if IPCOM_USE_FILE == IPCOM_FILE_NATIVE
          && defined(IPCOM_USE_RAM_DISK) */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */


