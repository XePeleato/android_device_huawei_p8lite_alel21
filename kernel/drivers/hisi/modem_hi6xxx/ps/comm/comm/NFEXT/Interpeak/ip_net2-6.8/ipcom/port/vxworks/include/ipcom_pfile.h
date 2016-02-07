/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_pfile.h,v $ $Revision: 1.8 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/include/ipcom_pfile.h,v $
 *     $Author: lob $ $Date: 2006-05-04 16:56:47 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_PFILE_H
#define IPCOM_PFILE_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipcom_type.h"

#if IPCOM_USE_FILE != IPCOM_FILE_NATIVE
#error ipcom_pfile_h_only_used_with_native_file_system
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         file open modes
 *===========================================================================
 */

#undef IP_O_CREAT
#define IP_O_CREAT      0x0200


/*
 *===========================================================================
 *                    object types
 *===========================================================================
 * Bitvalues for object types.
 */

#undef IP_S_IFREG
#define IP_S_IFREG 0x8000   /*  regular */


/*
 *===========================================================================
 *                    Bitvalues for object access
 *===========================================================================
 */
#undef IP_S_IREAD
#undef IP_S_IWRITE
#undef IP_S_IEXEC

#undef IP_S_IARCH
#undef IP_S_IRUSR
#undef IP_S_IWUSR
#undef IP_S_IXUSR
#undef IP_S_IRGRP
#undef IP_S_IWGRP
#undef IP_S_IXGRP
#undef IP_S_IROTH
#undef IP_S_IWOTH
#undef IP_S_IXOTH

#define IP_S_IREAD       (IP_S_IRUSR | IP_S_IRGRP | IP_S_IROTH)
#define IP_S_IWRITE      (IP_S_IWUSR | IP_S_IWGRP | IP_S_IWOTH)
#define IP_S_IEXEC       (IP_S_IXUSR | IP_S_IXGRP | IP_S_IXOTH)

#define IP_S_IRUSR  0x0100         /* read permission, owner */
#define IP_S_IWUSR  0x0080         /* write permission, owner */
#define IP_S_IXUSR  0x0040         /* execute/search permission, owner */
#define IP_S_IRGRP  0x0020         /* read permission, group */
#define IP_S_IWGRP  0x0010         /* write permission, group */
#define IP_S_IXGRP  0x0008         /* execute/search permission, group */
#define IP_S_IROTH  0x0004         /* read permission, other */
#define IP_S_IWOTH  0x0002         /* write permission, other */
#define IP_S_IXOTH  0x0001         /* execute/search permission, other */


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

