/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_init.c,v $ $Revision: 1.88 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_init.c,v $
 *   $Author: uolofsso $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */


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

#if !defined(IP_PORT_INTEGRITY)
#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#endif
#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_INIT_C
#define IPCOM_USE_CLIB_PROTO
#include "ipcom_type.h"
#include "ipcom_err.h"
#include "ipcom_os.h"
#ifdef IPCOM_USE_ROUTE
#include "ipcom_route.h"
#endif
#include "ipcom_prof.h"
#include "ipcom_sock.h"
#include "ipcom_clib.h"
#include "ipcom_time.h"
#include "ipcom_syslog.h"
#include "ipcom_h.h"
#include "ipcom_ipd.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#if !defined(IP_PORT_LKM) && !defined(IP_PORT_LAS) && !defined(IP_PORT_OSE5) && !defined(OSE_REFSYS)
#define IPCOM_CREATE_IPPRODUCT
#endif


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

IP_PUBLIC Ip_err ipcom_configure(void);

IP_GLOBAL Ip_err ipcom_random_init(void);

IP_PUBLIC Ip_err ipcom_usr_start(void);

#if IPCOM_USE_SHELL == IPCOM_SHELL_MINI
IP_GLOBAL Ip_err ipcom_mshell_init(void);
#endif


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

#ifdef IPCOM_CREATE_IPPRODUCT
IP_STATIC void ipcom_create_ipproduct(void);
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
 *                    ipcom_create_ipproduct
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPCOM_CREATE_IPPRODUCT
IP_STATIC void
ipcom_create_ipproduct(void)
{
    int     i;
    Ip_u8   disabled[IPCOM_IPD_ID_MAX];
    Ip_err  ret;

    ipcom_memset(disabled, 0, sizeof(disabled));

    /* Create products. */
    for(i = 0; ipcom_ipd_products[i].name != IP_NULL; i++)
        if (ipcom_ipd_products[i].create != IP_NULL)
        {
            IPCOM_LOG1(INFO, "creating '%s'",
                       ipcom_ipd_products[i].version());
            ret = ipcom_ipd_products[i].create();
            if(ret != IPCOM_SUCCESS)
            {
                IPCOM_LOG2(CRIT, "ipcom_create_ipproduct :: failed to create '%s', err = %ld",
                       ipcom_ipd_products[i].name, ret);
                disabled[ipcom_ipd_products[i].id] = 1;
            }
        }

#ifdef IPCOM_USE_SYSLOG
    /* Open syslogs */
    for(i = 0; ipcom_ipd_products[i].name != IP_NULL; i++)
        if (ipcom_ipd_products[i].syslog_id != IPCOM_LOG_NONE)
            ipcom_openlog(ipcom_ipd_products[i].name, 0, ipcom_ipd_products[i].syslog_id);
#endif

    /* Configure products. */
    for(i = 0; ipcom_ipd_products[i].name != IP_NULL; i++)
        if (ipcom_ipd_products[i].configure != IP_NULL)
        {
            if(disabled[ipcom_ipd_products[i].id] == 0)
            {
                IPCOM_LOG1(INFO, "configuring '%s'", ipcom_ipd_products[i].name);
                ret = ipcom_ipd_products[i].configure();
                if(ret != IPCOM_SUCCESS)
                {
                    IPCOM_LOG2(CRIT, "ipcom_create_ipproduct :: failed to configure '%s', err = %ld",
                               ipcom_ipd_products[i].name, ret);
                    disabled[ipcom_ipd_products[i].id] = 1;
                }
            }
        }

#if IPCOM_USE_SOCK != IPCOM_SOCK_NONE
    /* Start products. */
    for(i = 0; ipcom_ipd_products[i].name != IP_NULL; i++)
        if (ipcom_ipd_products[i].start != IP_NULL)
        {
            if(disabled[ipcom_ipd_products[i].id] == 0
               && IP_BIT_ISFALSE(ipcom_ipd_products[i].flags, IPCOM_IPD_FLAG_HALTED))
            {
                IPCOM_LOG1(INFO, "starting '%s'", ipcom_ipd_products[i].name);
                if(IP_BIT_ISSET(ipcom_ipd_products[i].flags, IPCOM_IPD_FLAG_IPD_START))
                    ret = ipcom_ipd_start(ipcom_ipd_products[i].name);
                else
                    ret = ipcom_ipd_products[i].start();
                if(ret != IPCOM_SUCCESS)
                {
                    IPCOM_LOG2(CRIT, "ipcom_create_ipproduct :: failed to start '%s', err = %ld",
                               ipcom_ipd_products[i].name, ret);
                }
            }
        }
#endif /* IPCOM_USE_SOCK != IPCOM_SOCK_NONE */
}
#endif


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
 *                    ipcom_start
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC Ip_err
ipcom_start(void)
{
    Ip_err   err;

    /***** Create IPCOM usr lib *****/
    err = ipcom_usr_create();
    if(err != IPCOM_SUCCESS)
        goto fail;


    /****** Configure IPCOM *****/
#if !defined(IP_PORT_LKM)
    err = ipcom_configure();
    if(err != IPCOM_SUCCESS)
        goto fail;
#endif


    /***** Start IPCOM boot *****/
#ifdef IPCOM_BUILD_DATE
    IPCOM_LOG1(INFO, "Build date: %s", IPCOM_BUILD_DATE);
#endif

    IPCOM_LOG2(INFO, "Number of CPUs: configured %d online %d",
               ipcom_num_configured_cpus(),
               ipcom_num_online_cpus());

    /***** Start IPCOM boot *****/
    IPCOM_LOG0(DEBUG, "ipcom_start :: begin");
#if IPCOM_USE_FILE == IPCOM_FILE_IPCOM
    IPCOM_LOG0(DEBUG, "ipcom_start :: using IPCOM memory file system");
#endif
#if IPCOM_USE_FILE == IPCOM_FILE_NATIVE
    IPCOM_LOG0(DEBUG, "ipcom_start :: using NATIVE file system");
#endif


    /***** Init random *****/
#if !defined(IPCOM_USE_REMOTE_RANDOM)
    IPCOM_LOG0(DEBUG, "ipcom_start :: initializing IPCOM random");
    err = ipcom_random_init();
    if(err != IPCOM_SUCCESS)
        goto fail;
#endif


    /****** Init IPCOM MINI SHELL *****/
#if IPCOM_USE_SHELL == IPCOM_SHELL_MINI
    (void)ipcom_mshell_init();
#endif

#if defined(IPCOM_USE_FD_DAEMON)
    (void)ipcom_fd_daemon_start();
#endif

    /****** Create Interpeak products *****/
#ifdef IPCOM_CREATE_IPPRODUCT
    IPCOM_LOG0(DEBUG, "ipcom_start :: creating Interpeak products");
    ipcom_create_ipproduct();
#endif


    /****** SUCCESS *****/
    IPCOM_LOG0(DEBUG, "ipcom_start :: end");
    return IPCOM_SUCCESS;


    /****** START FAILED *****/
 fail:
    IPCOM_LOG1(EMERG, "ipcom_start :: failed, err = %ld", err);
    IP_PANIC();
    return err;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

