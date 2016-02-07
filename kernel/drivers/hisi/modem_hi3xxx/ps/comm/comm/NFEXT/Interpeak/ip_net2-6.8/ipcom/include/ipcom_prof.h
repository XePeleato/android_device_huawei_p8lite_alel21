/*
******************************************************************************
*                     HEADER FILE
*
*     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_prof.h,v $ $Revision: 1.12 $
*     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_prof.h,v $
*     $Author: ulf $ $Date: 2006-05-30 16:54:49 $
*     $State: Exp $ $Locker:  $
*
*     INTERPEAK_COPYRIGHT_STRING
*
******************************************************************************
*/
#ifndef IPCOM_PROF_H
#define IPCOM_PROF_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Interpeak profiler API.
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
#include "ipcom_cstyle.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Flags for the enter function */
#define IPCOM_PROF_CONTINUED 0x01

/* Flags for the display functions */
#define IPCOM_PROF_ABSOLUTE  0x01
#define IPCOM_PROF_LINENUM   0x02
#define IPCOM_PROF_COUNT     0x04
#define IPCOM_PROF_OVERHEAD  0x08

#ifdef IPCOM_USE_PROFILING_ENABLE
#define IPCOM_PROF_TYPE(t)    Ipcom_prof_entry t
#define IPCOM_PROF(s)         ipcom_prof_enter(s, __FILE__, __LINE__, 0)
#define IPCOM_PROF_PUSH(e,s)  do { \
                                  ipcom_prof_push(&(e)); \
		                          ipcom_prof_enter(s, __FILE__, __LINE__, 0); \
                              } while ((0))
#define IPCOM_PROF_POP(e)     ipcom_prof_pop(&(e))
#else
#define IPCOM_PROF_TYPE(t)    IP_NOOP
#define IPCOM_PROF(s)         IP_NOOP
#define IPCOM_PROF_PUSH(e,s)  IP_NOOP
#define IPCOM_PROF_POP(e)     IP_NOOP
#endif


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

typedef struct
{
    Ip_u32 us;
    const char *msg;
    const char *file;
    Ip_u16 line;
    Ip_u8 type;
    Ip_u8 flags;
}
Ipcom_prof_entry;


/*
 ****************************************************************************
 * 6                    STANDARD FUNCTIONS
 ****************************************************************************
 */

IP_PUBLIC void
ipcom_prof_clear(void);

IP_PUBLIC int
ipcom_prof_stopped(int new_prof);

IP_PUBLIC void
ipcom_prof_enter(const char *msg, const char *file, int line, int flags);

IP_PUBLIC void
ipcom_prof_push(Ipcom_prof_entry *e);

IP_PUBLIC void
ipcom_prof_pop(Ipcom_prof_entry *e);

IP_PUBLIC void
ipcom_prof_dump(int start, int stop, int flags);

IP_PUBLIC void
ipcom_prof_summary(int start, int stop, int flags);

IP_PUBLIC void
ipcom_prof_sequence(int start, int stop, int flags);

IP_PUBLIC void
ipcom_prof_void(void);

IP_PUBLIC int
ipcom_cmd_prof(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */


