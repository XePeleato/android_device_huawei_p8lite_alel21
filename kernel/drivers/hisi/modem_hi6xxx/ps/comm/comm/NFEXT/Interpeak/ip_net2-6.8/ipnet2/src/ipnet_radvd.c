/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_radvd.c,v $ $Revision: 1.56 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_radvd.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */
/* IPNET router advertisement daemon */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO

#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_hash.h>
#include <ipcom_inet.h>
#include <ipcom_ipd.h>
#include <ipcom_list.h>
#include <ipcom_pqueue.h>
#include <ipcom_sock.h>
#include <ipcom_sock6.h>
#include <ipcom_sysctl.h>
#include <ipcom_syslog.h>
#include <ipcom_sysvar.h>
#include <ipcom_tmo2.h>
#include <ipcom_type.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_ip6.h"
#include "ipnet_radvd.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */
/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */
IPCOM_PROCESS(ipnet_radvd_proc);

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipnet_radvd_start(void);

#ifdef IP_PORT_LAS
IP_PUBLIC Ip_err ipcom_daemon_create(void);
#endif

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                   ipnet_radvd_proc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IPCOM_PROCESS(ipnet_radvd_proc)
{
    ipnet_radvd_main(IP_TRUE);
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_radvd_start
 *===========================================================================
 * Description: Starts the daemon.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipnet_radvd_start(void)
{
#ifdef IPNET_USE_RFC3971
    Ip_u32 stacksize = IPCOM_PROC_STACK_LARGE;
#else
    Ip_u32 stacksize = IPCOM_PROC_STACK_DEFAULT;
#endif
    IPCOM_LOG0(DEBUG2, "radvd: Start");

    return ipcom_proc_create("ipnet_radvd", (Ipcom_proc_func)ipnet_radvd_proc,
                             stacksize, IP_NULL);
}

#ifdef IP_PORT_LAS
/* The original makefile rule did this; its wildcard, and we just undef
 * instead of adding more rules */
#undef ipnet_radvd_create
IP_EXTERN Ip_err ipnet_radvd_create(void);

IP_PUBLIC Ip_err
ipcom_daemon_create(void)
{
    return ipnet_radvd_create();
}

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
