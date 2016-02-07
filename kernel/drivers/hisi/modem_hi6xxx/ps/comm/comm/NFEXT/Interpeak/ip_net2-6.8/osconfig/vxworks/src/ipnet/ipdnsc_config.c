/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipdnsc_config.c,v $ $Revision: 1.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipdnsc_config.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 ******************************************************************************
 */

/*
modification history
--------------------
01i,11sep07,dlk  Change parameter names for tertiary and quaternary name
                 servers to match those used in 00comp_ipnet_apps.cdf.
01h,28feb07,dlk  Replace _gethostbyname, _gethostbyaddr function pointers
                 with reentrant versions _ipcom_gethostbyname_r,
		 _ipcom_gethostbyaddr_r.
01g,03jan07,kch  Updated CVS revision log entry for Jan 03 code drop.
01f,01dec06,kch  Added support for additional configuration parameters.
01e,13sep06,kch  Refresh with Sept 08, 2006 code drop
01d,28aug06,kch  Corrected ipcom_gethostbyaddr() prototype declaration.
01c,28aug06,tkf  Remove dependency on hostLib.
01b,23aug06,tkf  Add a call to gethostbynameInit() and gethostbynameInit()
01a,03aug06,kch  Renamed INCLUDE_NSLOOKUP_CMD to INCLUDE_IPNSLOOKUP_CMD.
*/

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * DNS client configuration.
 */

/*
 ****************************************************************************
 * 2                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_sysvar.h>
#include <ipcom_shell.h>
#include <ipcom_sock2.h>
#include <ipdnsc_config.h>

/*
 ****************************************************************************
 * 3                    DEFINES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 4                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    EXTERN PROTOTYPES
 ****************************************************************************
 */
#ifdef INCLUDE_IPNSLOOKUP_CMD
IP_EXTERN int ipdnsc_cmd_nslookup(int argc, char **argv);
#endif

IP_EXTERN FUNCPTR _ipcom_gethostbyname_r;
IP_EXTERN FUNCPTR _ipcom_gethostbyaddr_r;

/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipdnsc_configure(void);


/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         ipdnsc_conf_sysvar
 *===========================================================================
 * DNS Client Configuration Variables
 */
IP_CONST Ipcom_sysvar ipdnsc_conf_sysvar[] =
{
	/* DNS server listening port */
	{ "ipdnsc.port", DNSC_SERVER_PORT},

	/* Timeout in seconds when waiting for responses to DNS queries */
	{ "ipdnsc.timeout", DNSC_TIMEOUT},

	/* Number of retries for DNS queries */
	{ "ipdnsc.retries", DNSC_RETRIES},

	/* DNS domain name */
	{ "ipdnsc.domainname", DNSC_DOMAIN_NAME},

	/* DNS primary name server */
	{ "ipdnsc.primaryns", DNSC_PRIMARY_NAME_SERVER},

	/* DNS secondary name server */
	{ "ipdnsc.secondaryns", DNSC_SECONDARY_NAME_SERVER},

	/* DNS tertiary name server */
	{ "ipdnsc.tertiaryns",  DNSC_TERTIARY_NAME_SERVER},

	/* DNS quaternary name server */
	{ "ipdnsc.quaternaryns", DNSC_QUATERNARY_NAME_SERVER},

	/* Zone for ipv4 address to name lookups */
	{ "ipdnsc.ip4.zone",   DNSC_IP4_ZONE},

    /* Zone for ipv6 address to name lookups */
    { "ipdnsc.ip6.zone",   DNSC_IP6_ZONE},     /* RFC1886: "ip6.int" RFC3152: "ip6.arpa" */
#ifdef IPCOM_DNSC_TC_SUPPORT
    /* default setting to use TCP to resolver DNS or not*/
    { "ipdnsc.use_tcp",   DNSC_USE_TCP},    
    /*能同时使用DNS client的用户最大数*/
    { "ipdnsc.max_users",   DNSC_MAX_USERS}, 
#endif

	{ IP_NULL, IP_NULL }
};


/*
 ****************************************************************************
 * 8                   STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                  PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipdnsc_configure
 *===========================================================================
 * Description: Configure DNS client.
 * Parameters:  None.
 * Returns:     IPCOM_SUCCESS or IPCOM_ERR_XXX if failed.
 */
IP_PUBLIC Ip_err
ipdnsc_configure(void)
{
    Ip_err ret;

    ret = ipcom_sysvar_setv(ipdnsc_conf_sysvar, 0);
    if(ret != IPCOM_SUCCESS)
        return ret;

    /* Initialize _gethostby* func pointers */
    _ipcom_gethostbyname_r = (FUNCPTR)ipcom_gethostbyname_r;
    _ipcom_gethostbyaddr_r = (FUNCPTR)ipcom_gethostbyaddr_r;

#ifdef INCLUDE_IPNSLOOKUP_CMD
    ret = ipcom_shell_add_cmd("nslookup", "nslookup [ options ] <modifier...>",
                              "Query Internet name servers interactively",
                               ipdnsc_cmd_nslookup,
                               IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);
#endif /* INCLUDE_IPNSLOOKUP_CMD */

    return ret;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
