/* iprohc_config.c - ROHC Component configellete file */

/*
 * Copyright (c) 2008 Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement. No license to Wind River intellectual property rights
 * is granted herein. All rights not licensed by Wind River are reserved
 * by Wind River.
 */


/*
modification history
--------------------
01a,20feb08,tlu  created.
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
#if 0
#include "iprohc_config.h"
#endif

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipcom_cstyle.h"
#include "ipcom_err.h"
#include "ipcom_sysvar.h"
#include "ipcom_shell.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#ifndef ROHC_INTERFACES
#define ROHC_INTERFACES                 ""
#endif

#ifndef ROHC_INTERFACE_PROFILES
#define ROHC_INTERFACE_PROFILES         ""
#endif

#ifndef ROHC_MAX_CID
#define ROHC_MAX_CID                	"15"
#endif

#ifndef ROHC_LARGE_CID
#define ROHC_LARGE_CID          	"0"
#endif

#ifndef ROHC_FEEDBACK_FOR_DEFINED
#define ROHC_FEEDBACK_FOR_DEFINED       "0"
#endif

#ifndef ROHC_FEEDBACK_FOR
#define ROHC_FEEDBACK_FOR               "5"
#endif

#ifndef ROHC_F_MAX_PERIOD
#define ROHC_F_MAX_PERIOD               "256"
#endif

#ifndef ROHC_F_MAX_TIME
#define ROHC_F_MAX_TIME                 "60"
#endif



#if !defined(WRS_IPNET) && !defined(PRJ_BUILD)
#define INCLUDE_IPROHC_CMD
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

#ifdef INCLUDE_IPROHC_CMD
IP_PUBLIC int iprohc_cmd_rohc(int argc, char **argv);
#endif /* INCLUDE_IPROHC_CMD */

#ifdef INCLUDE_IPROHC_PROFILE_IP
IP_PUBLIC Ip_err iprohc_profile_ip_register(void);
#endif /* INCLUDE_IPROHC_PROFILE_IP */

#ifdef INCLUDE_IPROHC_PROFILE_UNCOMPRESSED
IP_PUBLIC Ip_err iprohc_profile_uncompressed_register(void);
#endif /* INCLUDE_IPROHC_PROFILE_UNCOMPRESSED */



/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err iprohc_configure(void);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         SYSVAR CONFIGURATION
 *===========================================================================
 *  Refer to CDF file for detailed explanation.
 */

IP_STATIC IP_CONST Ipcom_sysvar iprohc_conf_sysvar[] =
{
    /* ROHC compression/decompression interface list. e.g. "fei,1" */
    { "iprohc.interface_list", ROHC_INTERFACES },

    /* ROHC compression/decompression interface-profile list. */
    /*	e.g. "fei1,0;fei2,4" */
    { "iprohc.interface_profile_list", ROHC_INTERFACE_PROFILES },

    /* A nonnegative integer, highest context ID number, default 15 */
    { "iprohc.max_cid", ROHC_MAX_CID },

    /* Used CID representation, e.g. "0" */
    { "iprohc.large_cid", ROHC_LARGE_CID },

    /* Define whether feedback for parameter is defined, default 0 */
    { "iprohc.feedback_for_defined", ROHC_FEEDBACK_FOR_DEFINED },

    /* The channel feedback for, refer to RFC 3095, section 5.1.1, e.g. 5 */
    { "iprohc.feedback_for", ROHC_FEEDBACK_FOR },

    /* Maximum number of consecutive compressed headers, */
    /* refer to RFC 2507, section 14 */
    { "iprohc.f_max_period", ROHC_F_MAX_PERIOD },

    /* Maximum time period in seconds of consecutive compressed headers, */
    /* refer to RFC 2507, section 14 */
    { "iprohc.f_max_time", ROHC_F_MAX_TIME },

    { IP_NULL, IP_NULL }
};


/*
 ****************************************************************************
 *                     PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    iprohc_configure
 *===========================================================================
 * Description: ROHC configuration function 
 * Parameters: none
 * Returns: IPCOM_SUCCESS
 *
 */
IP_PUBLIC Ip_err
iprohc_configure(void)
{
    /* Initialize IPROHC system variables. */
    (void)ipcom_sysvar_setv(iprohc_conf_sysvar, 0);

#ifdef INCLUDE_IPROHC_CMD    
    /* Add the ROHC command */
    (void)ipcom_shell_add_cmd("rohc", "rohc [command] <modifier...>",
                              "ROHC", iprohc_cmd_rohc,
                               IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);
#endif /* INCLUDE_IPROHC_CMD */

#ifdef INCLUDE_IPROHC_PROFILE_IP
    /* Register IP compression profile */
    iprohc_profile_ip_register();
#endif /* INCLUDE_IPROHC_IP */

#ifdef INCLUDE_IPROHC_PROFILE_UNCOMPRESSED
    /* Add uncompressed profile, id = 0 */
    iprohc_profile_uncompressed_register();
#endif /* INCLUDE_IPROHC_UNCOMPRESSED */

    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

