/*
 ******************************************************************************
 *                     IPCOM CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_config.c,v $ $Revision: 1.34.16.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/ipcom_config.c,v $
 *   $Author: gzadoyan $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
02n,31aug10,ggz  Added SHA256 hash support
02m,21jan10,dlk  Don't hardcode master forwarder core number.
02l,21jan10,dlk  Nehalem S-AMP forwarder support.
02k,01may09,ggz  Added configuration capability for password hash format
02j,14feb09,dlk  Rename INCLUDE_OCTEON_RGMII_VXB_FORWARDER to
                 INCLUDE_OCTEON_VXB_FORWARDER.
02i,11oct07,rme  Defect:WIND00107249:Fix:WIND00107665 ipcom commands not
                 present on console
02h,03oct07,uol  Fixing defect WIND00092273, cast required to avoid compiler
                 warning.
02g,27jul07,spw  Add IPWLAN commands
02f,02aug07,dlk  Added ipnet_cmd_qc_hook for ipnet_cmd_qc scalability in GPP.
02e,27jun07,mcarlste  Add Mib2 shell command
02d,04may07,v_r  Added back call to shellCmdTopicAdd() since defect
                 CQ:WIND00093739 is fixed.
02e,27apr07,tkf  Add IPv6Only build support.
02d,03mar07,jmt  Update prototype to be correct.
02c,27feb07,kch  Renamed Interpeak product versions command from 'version' to
                 'ipversion'. Also added 'getenv', 'date' and 'time' IPCOM
                 shell commands registration to ipcom_register_shell_commands.
                 (WIND00089286). Also fixed compiler warnings for ipcom
                 authentication (WIND00086024).
02b,30jan07,tkf  ipcom_register_shell_commands() should be called when
                 INCLUDE_IPCOM_SHELL is defined.
02a,29jan07,tkf  Updated to make ipcom shell commands binary scalable.
01z,22jan07,kch  Updated sockperf and socktest descriptions.
01y,22jan07,kch  Renamed radius client configuration macros for ipcom
                 authentication (WIND00083711).
01x,19jan07,tkf  Added ipcom_init_config_vars(). Added RAM disk initialization
                 support. Also pass NULL to ipcom_setlogfile() if
                 INCLUDE_IPCOM_SYSLOGD_USE_LOG_FILE is not included
                 (WIND00085813).
01w,08jan07,kch  Moved ipcom_ipd_products[] to ipcom_ipd_config.c.
01v,07jan07,tkf  Adjust INCLUDE_IPCOM_USE_VXSHELL_CMD initialization to
                 accomodate changes in the latest Stockholm code drop.
01u,28dec06,tkf  Define IPCOM_USE_INET6 if INCLUDE_IPCOM_USE_INET6 is
                 defined.
01t,27dec06,kch  Added ipcom_auth.h include. Also fixed Pentium4 exception
                 when ipcom_auth_useradd() is called with username and/or
                 password sets to NULL.
01s,13dec06,tkf  Initialize vxshell_parser_control if INCLUDE_IPCOM_USE_VXSHELL
                 is defined (to decouple IPNET from INCLUDE_SHELL).
01r,01dec06,kch  Updated CVS revision log entry.
01q,13nov06,kch  Removed IPCOM_USE_CLIB_PROTO define.
01p,31oct06,kch  Corrected typo for ipecho components.
01o,24oct06,kch  Renamed INCLUDE_IPCOM_USE_SYSLOGD to INCLUDE_IPCOM_SYSLOGD_CMD.
01n,06oct06,kch  Removed netInit boolean initialization.
01m,27sep06,kch  Renamed IPMPLS to INCLUDE_IPMPLS, IPEAP to INCLUDE_IPEAP and
                 IPLDAPC to INCLUDE_IPLDAPC.
01l,23sep06,kch  Renamed INCLUDE_IPNET_USE_VRRPD to INCLUDE_IPVRRPD.
01k,22sep06,jfb  Added ipipsec, ipcci, iphwcrypto
01j,16sep06,kch  Refresh with Sept 08, 2006 code drop. Comment out the old
                 unsupported INCLUDE_IPPPPOE.
01i,12sep06,tkf  Added IPDHCPC6 and IPDHCPS6 support.
01h,16jul06,tlu  Added RIPng
01h,28aug06,kch  Update based on Aug 22, 2006 code drop.
01g,28aug06,kch  Corrected IPCom authentication configuration support.
01f,21aug06,kch  Renamed INCLUDE_IPVRRP to INCLUDE_IPNET_USE_VRRPD.
01e,11aug06,kch  Corrected ipsntp startup issue.
01d,10aug06,kch  Renamed IPCOM_USE_SYSVAR to INCLUDE_IPCOM_SYSVAR_CMD and
                 IPCOM_USE_SYSLOG to INCLUDE_IPCOM_SYSLOGD_CMD.
01c,03aug06,kch  Renamed INCLUDE_Xxx_CMD to INCLUDE_IPXxx_CMD for consistency.
01b,22jul06,kch  Renamed IPRIP to INCLUDE_IPRIP.
01a,03jul06,dlk  Fixed typo, added WRS copyright notice and modification
                 history.
*/

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 2                    INCLUDE FILES
 ****************************************************************************
 */

#if !defined(IP_PORT_INTEGRITY)
#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#endif

#ifdef IPCOM_USE_CLIB_PROTO
#undef IPCOM_USE_CLIB_PROTO
#endif

#ifdef INCLUDE_IPCOM_USE_INET6
#ifndef IPCOM_USE_INET6
#define IPCOM_USE_INET6
#endif
#endif

#include "ipcom_config.h"
#include "ipcom_err.h"
#include "ipcom_auth.h"
#if 0
#include "ipcom_clib.h"
#include "ipcom_syslog.h"
#include "ipcom_sock.h"
#include "ipcom_sock2.h"
#endif
#include "ipcom_shell.h"
#include "ipcom_ipd.h"
#include "ipcom_os.h"
#include "ipcom_sysvar.h"

#include "ipcom_h.h" /* for ipcom_create() */
#include "ipnet.h"
#include <shellInterpCmdLib.h>
#include <private/vxCpuLibP.h>
#include <private/rtpLibP.h>
#include <taskHookLib.h>

#ifdef IPCOM_FORWARDER_AMP
#include <wrhv/vbi.h>
#include <ipcom_shared_mem.h>
#endif


/*
 ****************************************************************************
 * 3                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipcom_configure(void);
IP_PUBLIC Ip_err ipcom_configure_reserved_cpus(int num);


/*
 ****************************************************************************
 * 4                    EXTERN PROTOTYPES
 ****************************************************************************
 */

IP_EXTERN int ipcom_sethostname();
IP_EXTERN int ipcom_setlogfile(const char *file);

#ifdef INCLUDE_IPCOM_SYSLOGD_CMD
IP_EXTERN int ipcom_cmd_syslog(int argc, char **argv);
#endif

#ifdef INCLUDE_IPCOM_SYSVAR_CMD
IP_EXTERN int ipcom_cmd_sysvar(int argc, char **argv);
#endif

IP_EXTERN int ipcom_cmd_user(int argc, char **argv);

#ifdef INCLUDE_IPMEM_DEBUG_CMD
IP_EXTERN int ipcom_cmd_mem(int argc, char **argv);
#endif

#ifdef INCLUDE_IPD_CMD
IP_EXTERN int ipcom_cmd_ipd(int argc, char **argv);
#endif

#ifdef INCLUDE_IPVERSION_CMD
IP_EXTERN int ipcom_cmd_version(int argc, char **argv);
#endif

#ifdef INCLUDE_IPECHO_CLIENT_CMD
IP_EXTERN int ipcom_cmd_echoclient(int argc, char **argv);
#endif

#ifdef INCLUDE_IPECHO_SERVER_CMD
IP_EXTERN int ipcom_cmd_echoserver(int argc, char **argv);
#endif

#ifdef INCLUDE_IPSOCKPERF_CMD
IP_EXTERN int ipcom_cmd_sockperf(int argc, char **argv);
#endif

#ifdef INCLUDE_IPSOCKTEST_CMD
IP_EXTERN int ipcom_cmd_socktest(int argc, char **argv);
#endif

#ifdef INCLUDE_IPTRACE_ROUTE_CMD
IP_EXTERN int ipcom_cmd_tracert(int argc, char **argv);
#endif

#ifdef INCLUDE_IPTCP_TEST_CMD
IP_EXTERN int ipcom_cmd_ttcp(int argc, char **argv);
#endif

#if IPCOM_USE_ENV != IPCOM_ENV_NONE
IP_EXTERN int ipcom_cmd_setenv(int argc, char **argv);
IP_EXTERN int ipcom_cmd_getenv(int argc, char **argv);
#endif

#ifdef INCLUDE_IPCOM_USE_TIME_CMD
IP_EXTERN int ipcom_cmd_date(int argc, char **argv);
IP_EXTERN int ipcom_cmd_time(int argc, char **argv);
#endif

#if defined(INCLUDE_IPCOM_USE_VXSHELL_CMD) && IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
IP_EXTERN int ipcom_cmd_vxshell(int argc, char **argv);
#endif

#ifdef INCLUDE_IPCOM_USE_MIB2_CMD
IP_EXTERN int ipcom_cmd_mib2(int argc, char **argv);
#endif

#ifdef INCLUDE_IPWLAN_CMD
IP_EXTERN int ipwlan_cmd_wlan(int argc, char **argv);
#endif

#ifdef INCLUDE_IPCOM_CPU_CMD
IP_EXTERN int ipcom_cmd_cpu(int argc, char **argv);
#endif

#ifdef INCLUDE_IPCOM_KEY_DB_CMD
IP_GLOBAL int ipcom_cmd_keydb(int argc, char** argv);
#endif
IP_GLOBAL char ipcom_key_db_get_char(int in_chr);

#ifdef INCLUDE_IPCOM_USE_RAM_DISK
IP_EXTERN void vxfs_ramdisk_init();
#endif

IP_EXTERN STATUS ipcom_mcmd_route(int argc, char ** arg);
IP_EXTERN STATUS ipcom_mcmd_ifconfig(int argc, char ** arg);


/*
 ****************************************************************************
 * 4                    DEFINE
 ****************************************************************************
 */

/* Translates a macro from its symbolic name to a string containing its
   numerical for.
   Must evaluate x in 2 steps so that the pre-processor replaces
   the macro "x" with its numerical form
   ex: IPASCII(IPCOM_LOG_INFO) -> IPASCII2(5) -> "5"
*/
#define IPASCII(x)   IPASCII2(x)
#define IPASCII2(x) #x

#ifdef INCLUDE_IPCOM_SYSLOGD_USE_LOG_FILE
#define IPCOM_LOG_FILE_NAME IPCOM_SYSLOGD_LOG_FILE
#else
#define IPCOM_LOG_FILE_NAME NULL
#endif

/*
 ****************************************************************************
 * 5                    SYSTEM VARIABLES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 6                    AUTHENTICATION
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         IPCOM AUTH - RADIUS AUTHENTICATION
 *===========================================================================
 * RADIUS client configuration for IPCOM authentication - ipcom_auth.h.
 */
#ifdef INCLUDE_IPCOM_USE_AUTH_RADIUS

/* RADIUS server domain: IP_AF_INET=2 or IP_AF_INET6=24. */
IP_GLOBAL IP_CONST int ipcom_auth_radius_server_domain =
                                    IPCOM_AUTH_RADIUS_SERVER_DOMAIN;

/* RADIUS server IPv4/IPv6 address: */
IP_GLOBAL IP_CONST char *ipcom_auth_radius_server_address =
                                    IPCOM_AUTH_RADIUS_SERVER_IPV4_ADDRESS;

#if 0
#ifdef INCLUDE_IPCOM_USE_INET6
IP_GLOBAL IP_CONST char *ipcom_auth_radius_server_address6 =
                                    IPCOM_AUTH_RADIUS_SERVER_IPV6_ADDRESS;
#endif /* INCLUDE_IPCOM_USE_INET6 */
#endif

/* RADIUS server port in host endian: */
IP_GLOBAL IP_CONST Ip_u16 ipcom_auth_radius_server_port =
                                    IPCOM_AUTH_RADIUS_SERVER_PORT;

/* RADIUS server shared secret: */
IP_GLOBAL IP_CONST char *ipcom_auth_radius_server_secret =
                                    IPCOM_AUTH_RADIUS_SERVER_SHARE_SECRET;


/* RADIUS server password type (1=PAP, 2=CHAP): */
#define IPRADIUS_PAP_Password       1
#define IPRADIUS_CHAP_Password      2
IP_GLOBAL IP_CONST Ip_u32 ipcom_auth_radius_server_pwdtype =
                                    IPCOM_AUTH_RADIUS_SERVER_PASSWD_TYPE;

#endif /* INCLUDE_IPCOM_USE_AUTH_RADIUS */

/*
 *===========================================================================
 *                         IPCOM Config Function pointers
 *===========================================================================
 * This function pointer variables are used to scale in and out modules at
 * VxWorks image build time.
 */
/* ramdisk init routine. Set to NULL if use of RAMDISK is disabled (default) */
FUNCPTR vxfs_ramdisk_init_hook = NULL;

/* VxWorks Shell command add routine. NULL if no VxWorks shell (default) */
FUNCPTR ipcom_vxshell_add_cmd_hook = NULL;

/* Shell command add routine. Set to NULL if no shell (default) */
FUNCPTR ipcom_shell_find_cmd_hook = NULL;

/* IPNET route command routine. Set to ipcom_mcmd_route() if no shell cmd
   (default) */
FUNCPTR ipnet_cmd_route_hook = (FUNCPTR)NULL;

/* IPNET ifconfig command routine. Set to ipcom_mcmd_ifconfig() if no shell
   cmd (default) */
FUNCPTR ipnet_cmd_ifconfig_hook = (FUNCPTR)NULL;

/* IPNET qc command routine. */
FUNCPTR ipnet_cmd_qc_hook = (FUNCPTR)NULL;

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

#ifdef INCLUDE_OCTEON_VXB_FORWARDER
IP_PUBLIC int ipcom_forwarder_ipsec_sa_cache_size = OCTEON_FORWARDER_IPSEC_SA_CACHE_SIZE;
IP_PUBLIC int ipcom_forwarder_ipsec_single_output_queue = OCTEON_FORWARDER_IPSEC_SINGLE_OUTPUT_QUEUE;
#elif defined (INCLUDE_NEHALEM_VXB_FORWARDER)

#include <cpuset.h>
#include <vxCpuLib.h>

IP_PUBLIC int ipcom_forwarder_ipsec_sa_cache_size = NEHALEM_FORWARDER_IPSEC_SA_CACHE_SIZE;
IP_PUBLIC int ipcom_forwarder_ipsec_single_output_queue = 0;

IP_PUBLIC cpuset_t  ipcom_forwarder_cpus;
#ifdef IPCOM_FORWARDER_AMP
IP_PUBLIC unsigned int ipcom_forwarder_init(void);
#endif
#else
IP_PUBLIC int ipcom_forwarder_ipsec_sa_cache_size = 0;
IP_PUBLIC int ipcom_forwarder_ipsec_single_output_queue = 0;
#endif


/*
 ****************************************************************************
 * 7                   FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_register_shell_commands
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef INCLUDE_IPCOM_SHELL_CMD
/*lint -efunc(429,ipcom_shell_add_cmd)  Custodial pointer 'ptr' (line X) has not been freed or returned*/
IP_STATIC void
ipcom_register_shell_commands(void)
{
#if IPCOM_USE_SHELL != IPCOM_SHELL_NONE

#ifdef INCLUDE_IPVERSION_CMD
    (void)ipcom_shell_add_cmd("ipversion", "ipversion",
                              "Show interpeak product versions",
                              ipcom_cmd_version,
                              IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_SMALL);
#endif /* INCLUDE_IPVERSION_CMD */

#ifdef INCLUDE_IPD_CMD
    (void)ipcom_shell_add_cmd("ipd", "ipd <command> [ -options ]",
                               "ipd - Interpeak daemon control",
                              ipcom_cmd_ipd,
                              IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_DEFAULT);
#endif /* INCLUDE_IPD_CMD */

#ifdef INCLUDE_IPCOM_SYSLOGD_CMD
    (void)ipcom_shell_add_cmd("syslog", "syslog <command> [ -options ]",
                              "syslog",
                              ipcom_cmd_syslog,
                              IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_DEFAULT);
#endif /* INCLUDE_IPCOM_SYSLOGD_CMD */

#ifdef INCLUDE_IPCOM_SYSVAR_CMD
    (void)ipcom_shell_add_cmd("sysvar", "sysvar <command> [name] [value]",
                              "System variable tool",
                              ipcom_cmd_sysvar,
                              IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_DEFAULT);
#endif /* INCLUDE_IPCOM_SYSVAR_CMD */

#ifdef INCLUDE_IPMEM_DEBUG_CMD
    (void)ipcom_shell_add_cmd("ipmem", "ipmem <command> [ options ]",
                              "IPCOM memory debug tool",
                              ipcom_cmd_mem,
                              IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_DEFAULT);
#endif /* INCLUDE_IPMEM_DEBUG_CMD */

#ifdef INCLUDE_IPTCP_TEST_CMD
    (void)ipcom_shell_add_cmd("ttcp", "ttcp [ options ] <modifier...>",
                              "ttcp - standard performance test",
                              ipcom_cmd_ttcp,
                              IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_DEFAULT);
    (void)ipcom_shell_add_cmd("ttcp1", "ttcp1 [ options ] <modifier...>",
                              "ttcp - min priority",
                              ipcom_cmd_ttcp,
                              IPCOM_PRIORITY_MIN, IPCOM_PROC_STACK_DEFAULT);
    (void)ipcom_shell_add_cmd("ttcp7", "ttcp7 [ options ] <modifier...>",
                              "ttcp - max priority",
                              ipcom_cmd_ttcp,
                              IPCOM_PRIORITY_MAX, IPCOM_PROC_STACK_DEFAULT);
#endif /* INCLUDE_IPTCP_TEST_CMD */

#ifdef INCLUDE_IPSOCKPERF_CMD
    (void)ipcom_shell_add_cmd("sockperf", "sockperf [options] [addr]",
                              "Network performance test tool",
                              ipcom_cmd_sockperf,
                              IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_DEFAULT);
#endif /* INCLUDE_IPSOCKPERF_CMD */

#ifdef INCLUDE_IPSOCKTEST_CMD
    (void)ipcom_shell_add_cmd("socktest", "socktest [ options ] <modifier...>",
                              "Socket API test tool",
                              ipcom_cmd_socktest,
                              IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_LARGE);
#endif /* INCLUDE_IPSOCKTEST_CMD */

#ifdef INCLUDE_IPTRACE_ROUTE_CMD
    (void)ipcom_shell_add_cmd("traceroute", "traceroute <peer>",
                              "Trace route command for IPv4",
                              ipcom_cmd_tracert,
                              IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_DEFAULT);
#endif /* INCLUDE_IPTRACE_ROUTE_CMD */

#ifdef INCLUDE_IPECHO_SERVER_CMD
    (void)ipcom_shell_add_cmd("echoserver", "echoserver [ options ]",
                              "TCP/UDP echo server",
                              ipcom_cmd_echoserver,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);
#endif /* INCLUDE_IPECHO_SERVER_CMD */

#ifdef INCLUDE_IPECHO_CLIENT_CMD
    /* echoclient */
    (void)ipcom_shell_add_cmd("echoclient", "send [ options ]",
                              "TCP/UDP echo client",
                              ipcom_cmd_echoclient,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);
#endif /* INCLUDE_IPECHO_CLIENT_CMD */

#if IPCOM_USE_ENV != IPCOM_ENV_NONE
    (void)ipcom_shell_add_cmd("setenv", "setenv [name] [value]",
                              "Set an environment variable",
                              ipcom_cmd_setenv,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);

    (void)ipcom_shell_add_cmd("getenv", "getenv [name]",
                              "Get an environment variable",
                              ipcom_cmd_getenv,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);
#endif /* IPCOM_USE_ENV != IPCOM_ENV_NONE */

#ifdef INCLUDE_IPCOM_USE_TIME_CMD
    (void)ipcom_shell_add_cmd("date", "date [yyyy-mm-dd]",
                              "Show/Set current date",
                              ipcom_cmd_date,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);

    (void)ipcom_shell_add_cmd("time", "time [hh:mm:ss]",
                              "Show/Set current time",
                              ipcom_cmd_time,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);
#endif /* INCLUDE_IPCOM_USE_TIME_CMD */

#if defined(INCLUDE_IPCOM_USE_VXSHELL_CMD) && IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    (void)ipcom_shell_add_cmd("vxshell", "vxshell",
                              "Enter VxWorks target shell",
                              ipcom_cmd_vxshell, IPCOM_PRIORITY_DEFAULT,
                              IPCOM_PROC_STACK_DEFAULT);
#endif /* INCLUDE_IPCOM_USE_VXSHELL_CMD */

#ifdef INCLUDE_IPCOM_USE_MIB2_CMD
    (void)ipcom_shell_add_cmd("mib2", "mib2 <cmd>", "Print MIB-2 statistics",
                              ipcom_cmd_mib2,
                              IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_DEFAULT);
#endif /* INCLUDE_IPCOM_MIB2_CMD */

#ifdef INCLUDE_IPWLAN_CMD
    (void)ipcom_shell_add_cmd("wifi", "wifi",
                              "Wireless LAN config command",
                              ipwlan_cmd_wlan, IPCOM_PRIORITY_DEFAULT,
                              IPCOM_PROC_STACK_DEFAULT);
#endif /* INCLUDE_IPWLAN_CMD */

#ifdef INCLUDE_IPCOM_CPU_CMD
    (void)ipcom_shell_add_cmd("cpu", "cpu", "Set/Get CPU affinity",
                              ipcom_cmd_cpu,
                              IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_DEFAULT);
#endif /* INCLUDE_IPCOM_CPU_CMD */

#ifdef INCLUDE_IPCOM_KEY_DB_CMD
    (void)ipcom_shell_add_cmd("keydb", "keydb <command> [options]", "Key DB admin command",
                              ipcom_cmd_keydb,
                              IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_DEFAULT);
#endif

#ifdef INCLUDE_USER_CMD
    (void)ipcom_shell_add_cmd("user", "user <command> [name] [value]", "User admin command",
                              ipcom_cmd_user,
                              IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_DEFAULT);
#endif

#endif /* IPCOM_USE_SHELL != IPCOM_SHELL_NONE */
}
#endif /* #ifdef INCLUDE_IPCOM_SHELL_CMD */


#ifdef INCLUDE_IPCOM_USE_KEY_DB
/*
 *===========================================================================
 *                    ipcom_key_db_cb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipcom_key_db_cb(char *buf, int size, int rwflag, void *userdata)
{
    char last = 17;
    int i;

    (void) rwflag;
    (void) userdata;

    for (i=0; i<(int)strlen(IPCOM_KEY_DB_KEY_ENCRYPTING_KEY) && i<size; i++)
    {
        buf[i] = ipcom_key_db_get_char((int)IPCOM_KEY_DB_KEY_ENCRYPTING_KEY[i]+(int)last);
        last = buf[i];
    }
    buf[i] = 0;

    return strlen(buf);
}
#endif


#ifdef INCLUDE_IPCOM
/*
 *===========================================================================
 *                         ipcom_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_configure(void)
{
    (void)ipcom_sethostname("iptarget", 8);

#ifdef INCLUDE_IPCOM_SHELL_CMD
    ipcom_register_shell_commands();
#endif /* INCLUDE_USE_IPCOM_SHELL */

#ifdef INCLUDE_IPCOM_USE_AUTH

    /* Add some default users. */
    {
#if defined(INCLUDE_IPCOM_AUTH_1) || defined(INCLUDE_IPCOM_AUTH_2) || \
    defined(INCLUDE_IPCOM_AUTH_3) || defined(INCLUDE_IPCOM_AUTH_4) || \
    defined(INCLUDE_IPCOM_AUTH_5) || defined(INCLUDE_IPCOM_AUTH_6)
    char *username = NULL;
    char *passwd = NULL;
    BOOL pwdhash = IP_FALSE;
#endif

#if (defined(IPCOM_AUTH_HASH_HOOK_INSTALL) && (IPCOM_AUTH_HASH_HOOK_INSTALL == TRUE))
    (void)ipcom_auth_hash_rtn_install(IPCOM_AUTH_HASH_HOOK, 
                            IPCOM_AUTH_HASH_HOOK_DIGEST);
#else
    /* Default hash algorithm is "SHA-256". The digest length is 32 byte. */ 
    (void)ipcom_auth_hash_rtn_install(ipcom_auth_default_hash_rtn, 32);
#endif

#ifdef INCLUDE_IPCOM_AUTH_1
    username = AUTH_USER_NAME1;
    passwd = AUTH_USER_PASSWD1;
    pwdhash = AUTH_USER_PASSWD1_HASH;

    if ((username != NULL) && (passwd != NULL))
    {  
       if (pwdhash == IP_FALSE)
          (void)ipcom_auth_useradd(AUTH_USER_NAME1, AUTH_USER_PASSWD1,
                                                       AUTH_USER_ID1);
       else
          (void)ipcom_auth_useradd_hash(AUTH_USER_NAME1, AUTH_USER_PASSWD1,
                                                    AUTH_USER_ID1, IP_TRUE);
    } 
#endif /* INCLUDE_IPCOM_AUTH_1 */

#ifdef INCLUDE_IPCOM_AUTH_2
    username = AUTH_USER_NAME2;
    passwd = AUTH_USER_PASSWD2;
    pwdhash = AUTH_USER_PASSWD2_HASH;

    if ((username != NULL) && (passwd != NULL))
    { 
       if (pwdhash == IP_FALSE)
          (void)ipcom_auth_useradd(AUTH_USER_NAME2, AUTH_USER_PASSWD2,
                                                       AUTH_USER_ID2);
       else
          (void)ipcom_auth_useradd_hash(AUTH_USER_NAME2, AUTH_USER_PASSWD2,
                                                   AUTH_USER_ID2, IP_TRUE);
    }  
#endif /* INCLUDE_IPCOM_AUTH_2 */

#ifdef INCLUDE_IPCOM_AUTH_3
    username = AUTH_USER_NAME3;
    passwd = AUTH_USER_PASSWD3;
    pwdhash = AUTH_USER_PASSWD3_HASH;

    if ((username != NULL) && (passwd != NULL))
    { 
       if (pwdhash == IP_FALSE)
          (void)ipcom_auth_useradd(AUTH_USER_NAME3, AUTH_USER_PASSWD3,
                                                       AUTH_USER_ID3);
       else
          (void)ipcom_auth_useradd_hash(AUTH_USER_NAME3, AUTH_USER_PASSWD3,
                                                   AUTH_USER_ID3, IP_TRUE);
    }
#endif /* INCLUDE_IPCOM_AUTH_2 */

#ifdef INCLUDE_IPCOM_AUTH_4
    username = AUTH_USER_NAME4;
    passwd = AUTH_USER_PASSWD4;
    pwdhash = AUTH_USER_PASSWD4_HASH;

    if ((username != NULL) && (passwd != NULL))
    { 
       if (pwdhash == IP_FALSE)
          (void)ipcom_auth_useradd(AUTH_USER_NAME4, AUTH_USER_PASSWD4,
                                                       AUTH_USER_ID4);
      else
          (void)ipcom_auth_useradd_hash(AUTH_USER_NAME4, AUTH_USER_PASSWD4,
                                                   AUTH_USER_ID4, IP_TRUE);
    } 
#endif /* INCLUDE_IPCOM_AUTH_4 */

#ifdef INCLUDE_IPCOM_AUTH_5
    username = AUTH_USER_NAME5;
    passwd = AUTH_USER_PASSWD5;
    pwdhash = AUTH_USER_PASSWD5_HASH;

    if ((username != NULL) && (passwd != NULL))
    {
       if (pwdhash == IP_FALSE)
          (void)ipcom_auth_useradd(AUTH_USER_NAME5, AUTH_USER_PASSWD5,
                                                       AUTH_USER_ID5);
       else
          (void)ipcom_auth_useradd_hash(AUTH_USER_NAME5, AUTH_USER_PASSWD5,
                                                   AUTH_USER_ID5, IP_TRUE);
    }
#endif /* INCLUDE_IPCOM_AUTH_5 */

#ifdef INCLUDE_IPCOM_AUTH_6
    username = AUTH_USER_NAME6;
    passwd = AUTH_USER_PASSWD6;
    pwdhash = AUTH_USER_PASSWD6_HASH;

    if ((username != NULL) && (passwd != NULL))
    { 
       if (pwdhash == IP_FALSE)
          (void)ipcom_auth_useradd(AUTH_USER_NAME6, AUTH_USER_PASSWD6,
                                                       AUTH_USER_ID6);
       else
           (void)ipcom_auth_useradd_hash(AUTH_USER_NAME6, AUTH_USER_PASSWD6,
                                                    AUTH_USER_ID6, IP_TRUE);
    } 
#endif /* INCLUDE_IPCOM_AUTH_6 */
    }
#endif /* INCLUDE_IPCOM_USE_AUTH */


#ifdef INCLUDE_IPCOM_SYSLOGD_CMD
    (void)ipcom_sysvar_set("ipcom.syslogd.queue.max",
                           IPASCII(IPCOM_SYSLOGD_QUEUE_MAX), 0);

    /* Default runtime syslog priority (change with shell command syslog) */

    (void)ipcom_sysvar_set("ipcom.syslogd.default_priority",
                           IPASCII(IPCOM_SYSLOGD_DEFAULT_PRIORITY), 0);

    /* Set logfile name. Note: Set filename to IP_NULL to disable. */
    (void)ipcom_setlogfile(IPCOM_LOG_FILE_NAME);

    /* Set UDP log host and port. Note: Set port to 0 to disable.
#ifdef INCLUDE_IPCOM_USE_INET
    (void)ipcom_setlogudp("255.255.255.255", 000);
#else
    (void)ipcom_setlogudp("FF02::1%2", 4000);
#endif
    */


#endif /* INCLUDE_IPCOM_SYSLOGD_CMD */

    return IPCOM_SUCCESS;
}

/*
 *===========================================================================
 *                         ipcom_init_config_vars
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_init_config_vars()
{
#if defined(INCLUDE_IPCOM_USE_RAM_DISK)
    vxfs_ramdisk_init_hook = (FUNCPTR)vxfs_ramdisk_init;
#endif

#if defined(INCLUDE_IPCOM_SHELL_CMD)
    ipcom_shell_find_cmd_hook = (FUNCPTR)ipcom_shell_find_cmd;

#if IPCOM_USE_SHELL != IPCOM_SHELL_NONE
    /* Add the "network" topic. */
    shellCmdTopicAdd ("network", "Network commands");
    ipcom_vxshell_add_cmd_hook = shellCmdAdd;
#endif

#endif /* INCLUDE_IPCOM_SHELL_CMD */

#if defined(INCLUDE_IPROUTE_CMD)
    ipnet_cmd_route_hook = ipnet_cmd_route;
#else
    ipnet_cmd_route_hook = (FUNCPTR)ipcom_mcmd_route;
#endif

#if defined(INCLUDE_IPIFCONFIG_CMD)
    ipnet_cmd_ifconfig_hook = ipnet_cmd_ifconfig;
#else
    ipnet_cmd_ifconfig_hook = (FUNCPTR)ipcom_mcmd_ifconfig;
#endif

#if defined(INCLUDE_IPQUEUE_CONFIG_CMD)
    ipnet_cmd_qc_hook = (FUNCPTR)ipnet_cmd_qc;
#endif
}
#endif /* INCLUDE_IPCOM */


/*
 *===========================================================================
 *                    ipcom_configure_reserved_cpus
 *===========================================================================
 * Description: Configure the set of CPUs to be reserved and excluded from
 *              normal scheduling. Must be called prior to schedule start.
 * Parameters:  num - number of CPUs to exclude
 * Returns:     IPCOM_SUCCESS    = success
 *              IPCOM_ERR_FAILED = failure
 */
IP_PUBLIC Ip_err
ipcom_configure_reserved_cpus(int num)
{
    (void)num;
#ifdef IPCOM_USE_SMP
#if 0
    cpuset_t cpuset;
    int      i, count = 0;

    /* Get the current set of reserved CPUs */
    cpuset = vxCpuReservedGet();

    /* Traverse the set of CPUs from the top and select unreserved CPUs.
       Avoid CPU#0 */
    if (num > 0)
    {
        for (i = ipcom_num_configured_cpus() - 1; i > 0; i--)
        {
            if (!CPUSET_ISSET(cpuset, i))
            {
                CPUSET_SET(cpuset, i);
                if (++count == num)
                    break;
            }
        }
    }

    /* Check that CPUs were available */
    if (count < num)
        return IPCOM_ERR_FAILED;

    /* Update the set of reserved CPUs */
    if (vxCpuReservedSet(cpuset) == ERROR)
       return IPCOM_ERR_FAILED;
#endif
    return IPCOM_SUCCESS;
#else
    return IPCOM_ERR_FAILED;
#endif
}

/*
 *===========================================================================
 *                    ipcom_forwarder support
 *===========================================================================
 */

#if defined (INCLUDE_OCTEON_VXB_FORWARDER) || \
    defined (INCLUDE_NEHALEM_VXB_FORWARDER)
IP_EXTERN int ipcom_forwarder_transmit_pkt(void *pkt, void *netif);
IP_EXTERN int ipcom_forwarder_create(void);

IP_EXTERN int ipcom_fw_register_netif(void *netif);
IP_EXTERN void ipcom_fw_deregister_netif(void *netif);
IP_EXTERN Ip_err ipcom_forwarder_ioctl(void *netif, int code, void *fwreq);

IP_EXTERN void * ipcom_fw_cpu_mem_alloc(unsigned int cpu, Ip_size_t size,
                                        Ip_size_t align, unsigned int flags);
#define IPCOM_FW_CPU_MEM_ALLOC_SHARED   2
#endif

#ifdef INCLUDE_OCTEON_VXB_FORWARDER
IP_EXTERN int ipcom_forwarder_get_fw_cnt(void);
IP_EXTERN void vxbOcteon_get_nonfw_stats(void *pdrv, Ip_u32 *frames, Ip_u32 *bytes, int reset);
IP_EXTERN uint32_t vxbOcteon_get_ports_for_interface (uint32_t iface);
#endif

#ifndef FWCACHE_USE_LEVEL_LIMIT
#define FWCACHE_USE_LEVEL_LIMIT (25)
#endif

IP_PUBLIC int
ipcom_forwarder_get_fwcache_util_level_limit(void)
{
    return FWCACHE_USE_LEVEL_LIMIT;
}


IP_PUBLIC Ip_u32
ipcom_forwarder_get_ports_for_interface (Ip_u32 iface)
{
#ifdef INCLUDE_OCTEON_VXB_FORWARDER
    return vxbOcteon_get_ports_for_interface(iface);
#else
    return 0;
#endif
}

IP_PUBLIC void
ipcom_forwarder_get_nonfw_stats(void *end, Ip_u32 *frames, Ip_u32 *bytes, int reset)
{
#ifdef INCLUDE_OCTEON_VXB_FORWARDER
    vxbOcteon_get_nonfw_stats(end, frames, bytes, reset);
#else
    (void)end;
    (void)frames;
    (void)bytes;
    (void)reset;
#endif
}

#if defined(IPCOM_USE_FORWARDER) && defined(IPCOM_FORWARDER_LOCAL_STACK)
IP_PUBLIC int
ipcom_forwarder_transmit_pkt_cf(void *pkt, void *netif)
{
#if defined (INCLUDE_OCTEON_VXB_FORWARDER) || \
    defined (INCLUDE_NEHALEM_VXB_FORWARDER)
    return ipcom_forwarder_transmit_pkt(pkt, netif);
#else
    return 1; /* Did not transmit packet */
#endif
}
#endif /* IPCOM_USE_FORWARDER && IPCOM_FORWARDER_LOCAL_STACK */

IP_PUBLIC int
ipcom_forwarder_create_cf(void)
{
    /* Called from ipnet_start() */
#if defined (INCLUDE_OCTEON_VXB_FORWARDER) || \
    defined (INCLUDE_NEHALEM_VXB_FORWARDER)
    return ipcom_forwarder_create();
#else
    return 0;
#endif
}

IP_PUBLIC void
ipcom_forwarder_startup_delay(void)
{
#ifdef VX_ENABLE_CPU_TIMEOUT
    ipcom_sleep(VX_ENABLE_CPU_TIMEOUT);
#endif
}

IP_PUBLIC int
ipcom_reserve_fw_cpu(void)
{
#ifdef INCLUDE_OCTEON_VXB_FORWARDER
    return ipcom_configure_reserved_cpus(ipcom_forwarder_get_fw_cnt());
#elif defined (INCLUDE_NEHALEM_VXB_FORWARDER)
#ifndef IPCOM_FORWARDER_AMP
    cpuset_t obtained;
    CPUSET_ZERO(obtained);
    if (vxCpuReserve (ipcom_forwarder_cpus, &obtained) != OK)
        {
        if (_func_logMsg)
            _func_logMsg ("Failed to reserve CPUs 0x%x for forwarding!\n",
                          ipcom_forwarder_cpus);
        return IPCOM_ERR_FAILED;
        }
#endif
    return IPCOM_SUCCESS;
#else
    return IPCOM_SUCCESS;
#endif
}

#define IPCOM_TAG_BITS (16)

IP_PUBLIC int
ipcom_forwarder_config_cache_size(void)
{
#ifdef INCLUDE_OCTEON_VXB_FORWARDER
    Ip_u32 nsize = OCTEON_FORWARDER_CACHE_SIZE;
#elif defined (INCLUDE_NEHALEM_VXB_FORWARDER)
    Ip_u32 nsize = NEHALEM_FORWARDER_CACHE_SIZE;
#else
    return 0;
#endif

#if defined (INCLUDE_OCTEON_VXB_FORWARDER) || \
    defined (INCLUDE_NEHALEM_VXB_FORWARDER)
    int i;


    /*
     * At this point nsize has a nominal number of cache entries.
     * In case IPCOM_FORWARDER_TRUE_CACHE_WAYS is greater than 1,
     * this routine returns the number of cache _sets_.  It returns
     * N, the smallest power of two no less than 16 such that either
     * the total number of entries is at least the requested number,
     * or else N == 65536.
     */

    nsize += IPCOM_FORWARDER_TRUE_CACHE_WAYS - 1; /* round up */

    for (i = 4; i < IPCOM_TAG_BITS; i++)
    {
        if (((nsize / IPCOM_FORWARDER_TRUE_CACHE_WAYS - 1) >> i) == 0)
        {
            break;
        }
    }
    return 1 << i;
#endif
}

#if defined(IPCOM_USE_FORWARDER) && defined(IPCOM_FORWARDER_LOCAL_STACK)
IP_PUBLIC int
ipcom_fw_register_netif_cf(void *netif)
{
#if defined (INCLUDE_OCTEON_VXB_FORWARDER) || \
    defined (INCLUDE_NEHALEM_VXB_FORWARDER)
    return ipcom_fw_register_netif(netif);
#else
    return 0;
#endif
}

IP_PUBLIC void
ipcom_fw_deregister_netif_cf(void *netif)
{
#if defined (INCLUDE_OCTEON_VXB_FORWARDER) || \
    defined (INCLUDE_NEHALEM_VXB_FORWARDER)
    ipcom_fw_deregister_netif(netif);
#endif
}

IP_PUBLIC Ip_err
ipcom_forwarder_ioctl_cf(void *netif, int code, void *fwreq)
{
#if defined (INCLUDE_OCTEON_VXB_FORWARDER) || \
    defined (INCLUDE_NEHALEM_VXB_FORWARDER)
    return ipcom_forwarder_ioctl(netif, code, fwreq);
#else
    return -1;
#endif
}
#endif /* IPCOM_USE_FORWARDER && IPCOM_FORWARDER_LOCAL_STACK */

/* Limits nominal MTU for all forwarder network devices */
IP_PUBLIC int
ipcom_forwarder_mtu(void)
{
#if defined (INCLUDE_NEHALEM_VXB_FORWARDER)
    return NEHALEM_FORWARDER_MTU;
#else
    return 1500;
#endif
}

IP_PUBLIC unsigned int
ipcom_forwarder_current_core(void)
{
    /*
     * Note that under SMP, this result of this function may
     * be obsolete unless CPU migration is prevented.
     */
#ifdef IPCOM_FORWARDER_AMP
#if defined (INCLUDE_NEHALEM_VXB_FORWARDER) || \
    defined (INCLUDE_NEHALEM_FORWARDER_AMP_DATAPLANE)
    unsigned int cpu;

    /*
     * We presently depend upon the sysProcNum boot
     * parameter being set in the bootline for this VB by the
     * hypervisor configuration to indicate the physical
     * CPU core that this board runs on. Would like a better
     * method.
     */
    if(!_procNumWasSet)
    {
        BOOT_PARAMS p;
        char *pEndStr;
        pEndStr = bootStringToStruct(sysBootLine, &p);
        if (*pEndStr != EOS)
        {
            errnoSet(EINVAL);
            bootParamsErrorPrint(sysBootLine, pEndStr);
            return 0;
        }
        sysProcNumSet(p.procNum);
        _procNumWasSet = TRUE;
    }
    cpu = sysProcNumGet();

    return cpu;
#else
    return 0; /* TODO - non-Nehalem AMP forwarder */
#endif
#else /* SMP case */
    return vxCpuIndexGet();
#endif /* !defined(IPCOM_FORWARDER_AMP) */
}

#ifdef IPCOM_FORWARDER_AMP
/*
 * Return CPU ID of master forwarder core / control NAE
 */
IP_PUBLIC unsigned int
ipcom_forwarder_master_core(void)
{
#if defined (INCLUDE_NEHALEM_VXB_FORWARDER) || \
    defined (INCLUDE_NEHALEM_FORWARDER_AMP_DATAPLANE)
    char * name;
    char * endptr;
    unsigned int i;
    static unsigned int master_core = ~0U;

    if (master_core != ~0U)
	return master_core;

    name = (char *)VBI_BOARD_NAME_GET();

    /*
     * Format of the board name is prefix:cpu:mgmt_cpu:ctrl_nae_cpu,
     * where the prefix is "mgmt", "ctrl", or "nae" for the
     * management board, control NAE, or regular NAE respectively.
     * The cpus are decimal numbers.
     */
    i = 0;
    while (*name != '\0')
	if (*name++ == ':' && ++i == 3)
	    break;
    i = (unsigned int)strtoul(name, &endptr, 10);
    if (name == endptr)
	i = 1; /* default */

    master_core = i;
    return i;
#else
    return 1;
#endif
}

IP_PUBLIC Ip_bool
ipcom_forwarder_is_master(void)
{
#if defined (INCLUDE_OCTEON_VXB_MASTER_FORWARDER) || \
    defined (INCLUDE_NEHALEM_VXB_FORWARDER)
    return IP_TRUE;
#else
    return IP_FALSE;
#endif
}

#ifdef INCLUDE_NEHALEM_FORWARDER_AMP_DATAPLANE
IP_PUBLIC void
ipcom_forwarder_core_start(void)
{
    char tname [32];
    unsigned int cpu = ipcom_forwarder_current_core();
    extern int ipcom_fw_task (void);

    sprintf (tname, "ipfwdr%u", cpu);

    taskSpawn (tname, 100, 0, 20000, (FUNCPTR)ipcom_fw_task,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}
#endif

int * ipcom_route_cache_id_ptr = IP_NULL;

IP_PUBLIC int *
ipcom_route_cache_id_alloc(void)
{
    if (ipcom_route_cache_id_ptr == IP_NULL)
    {
#if defined (INCLUDE_NEHALEM_VXB_FORWARDER)
        if (ipcom_forwarder_is_master())
        {
            /* ensure shared memory pools are created */
            (void)ipcom_forwarder_init();
            ipcom_route_cache_id_ptr =
                ipcom_fw_cpu_mem_alloc(ipcom_forwarder_master_core(),
                                       sizeof (int),
                                       _CACHE_ALIGN_SIZE,
                                       IPCOM_FW_CPU_MEM_ALLOC_SHARED);
        }
        else /* TODO: do we care if non-master forwarder stacks get
                a different route cache ID than the master? */
#endif
        {
            ipcom_route_cache_id_ptr =
                memalign(_CACHE_ALIGN_SIZE, _CACHE_ALIGN_SIZE);
        }
        *ipcom_route_cache_id_ptr = 0;
    }
    return ipcom_route_cache_id_ptr;
}

IP_PUBLIC int
ipcom_shared_memory_pool_cfg_get(unsigned int ix, Ipcom_shared_mem_pool_cfg_t * pcfg)
{
#if defined (INCLUDE_NEHALEM_VXB_FORWARDER) || \
    defined (INCLUDE_NEHALEM_FORWARDER_AMP_DATAPLANE)
    void *vbPhys;
    uint32_t length;
    uint32_t attr;
    char * name;
    char buf [80];

    if (pcfg == NULL)
        return -1;

    if (ix == 0)
        name = "numa1";
    else if (ix == 1)
        name = "numa2";
    else
        return -1;
    
    /* assume identity virt-to-phys mapping in VB for shared memory regions */
    vbiShmemRegionFind((int8_t *)name, &vbPhys, &length, &attr);

    if (vbPhys == NULL)
    {
        sprintf (buf, "Shared memory region %s not found\n", name);
        vbiKputs (buf);
        return -1;
    }

#define IPC_ANCHOR_RESERVE 0x1000 /* reserve space for anchor in 1st pool */
    if (ix == 0)
    {
        vbPhys = (void *)((char *)vbPhys + IPC_ANCHOR_RESERVE);
        length -= IPC_ANCHOR_RESERVE;
    }

    pcfg->addr = (Ip_shared_mem_virt_addr)vbPhys;
    pcfg->len = (Ip_size_t)length;

    return 0;
#else
    return -1; /* TODO */
#endif
}

IP_PUBLIC void
ipcom_shared_memory_pools_create(void)
{
#if defined (INCLUDE_NEHALEM_VXB_FORWARDER)
    Ipcom_shared_mem_pool_cfg_t cfg;
    int i;
    char buf [80];

    for (i = 0; ; ++i)
    {
        if (ipcom_shared_memory_pool_cfg_get(i, &cfg) < 0)
            break;

        sprintf (buf, "shared mem addr=0x%x, len=0x%x\n", cfg.addr, cfg.len);
        vbiKputs (buf);

        if (ipcom_shared_mem_pool_init((void *)cfg.addr, cfg.len) == 0)
        {
            sprintf (buf, "shared mem addr=0x%x initialized\n", cfg.addr);
            vbiKputs (buf);
        }
    }
#endif
}

IP_PUBLIC Ipcom_shared_mem_pool_t *
ipcom_shared_mem_pool_for_core(unsigned int coreid)
{
#if defined (INCLUDE_NEHALEM_VXB_FORWARDER) || \
    defined (INCLUDE_NEHALEM_FORWARDER_AMP_DATAPLANE)
    Ipcom_shared_mem_pool_cfg_t cfg;
    int index;

    /*
     * For now: even logical cores in one NUMA node, odd in another.
     * We should pass the logical core's APIC ID from the hypervisor,
     * since the hypervisor doesn't let the VB access the local APIC
     * registers directly.
     */
    index = coreid & 1;

    if (ipcom_shared_memory_pool_cfg_get(index, &cfg) < 0)
        return IP_NULL;

    return (Ipcom_shared_mem_pool_t *)cfg.addr;
#else
    return IP_NULL; /* todo */
#endif
}

void * ipcom_fw_anchor_get(void)
{
#if defined (INCLUDE_NEHALEM_VXB_FORWARDER) || \
    defined (INCLUDE_NEHALEM_FORWARDER_AMP_DATAPLANE)

    Ipcom_shared_mem_pool_cfg_t cfg;

    if (ipcom_shared_memory_pool_cfg_get(0, &cfg) < 0)
        return IP_NULL;

    return (void *)(cfg.addr - IPC_ANCHOR_RESERVE);
#else
    return IP_NULL;
#endif
}

#endif /* defined (IPCOM_FORWARDER_AMP) */

IP_PUBLIC void *
ipcom_allocate_fw_cache(int entry_size, int *entries)
{
#if defined (INCLUDE_OCTEON_VXB_FORWARDER) || \
    defined (INCLUDE_NEHALEM_VXB_FORWARDER)
#define IPCOM_CACHELINE_SZ (128)

    void *p;
    int csize = ipcom_forwarder_config_cache_size();
    size_t memsize;

    printf("Cache entries: %d, buckets: %d, ways: %d\n",
           csize * IPCOM_FORWARDER_TRUE_CACHE_WAYS,
           csize,
           IPCOM_FORWARDER_TRUE_CACHE_WAYS);

    memsize = 
        entry_size * (csize * IPCOM_FORWARDER_TRUE_CACHE_WAYS +
                      IPCOM_FORWARDER_CACHE_WAYS);

#ifdef IPCOM_FORWARDER_AMP
    p = ipcom_fw_cpu_mem_alloc (ipcom_forwarder_master_core(), /* for now */
                                memsize, IPCOM_CACHELINE_SZ,
                                IPCOM_FW_CPU_MEM_ALLOC_SHARED);
    memset(p, 0, memsize);
#else
    memsize += IPCOM_CACHELINE_SZ; /* for alignment */
    p = malloc(memsize);
    memset(p, 0, memsize);

    p = (void *)((((Ip_ptrdiff_t)p) + IPCOM_CACHELINE_SZ - 1) & ~(IPCOM_CACHELINE_SZ - 1));
#endif

    if (entries)
    {
        *entries = csize;
    }
    return p;
#else
    (void)entry_size;
    (void)entries;
    return NULL;
#endif
}

IP_PUBLIC int
ipcom_forwarder_config_fw_max(void)
{
#ifdef INCLUDE_OCTEON_VXB_FORWARDER
    return OCTEON_NUM_FORWARDERS;
#else
    return 0;
#endif
}

IP_PUBLIC int
ipcom_forwarder_fw_per_port(void)
{
#if OCTEON_BIND_INPUT_PORT_TO_FORWARDER == TRUE
    return 1;
#else
    return 0;
#endif
}

/*
 * For a hash key of width IPCOM_TAG_BITS,  this routine tells
 * how many bits should we shift right if we want to use the
 * uppermost bits of the hash key rather than the lowermost bits.
 */
IP_PUBLIC int ipcom_forwarder_get_key_shift(void)
{
    int n;
    int csize = ipcom_forwarder_config_cache_size();

    for (n = 4; n < IPCOM_TAG_BITS; n++)
    {
        if (csize & (1 << n))
        {
           return IPCOM_TAG_BITS - n;
        }
    }
    return 0;
}


/*
 *===========================================================================
 *                       ipcom_vxworks_rtp_del_hook_add
 *===========================================================================
 * Description: Add RTP delete hook if RTP support is enabled in the VIP.
 *              This has to reside in a configlette to handle the case
 *              when _WRS_CONFIG_RTP is defined and INCLUDE_RTP is not.
 * Parameters:  hook    - hook to install
 *              to_head - add hook to head of list
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_vxworks_rtp_del_hook_add(RTP_DELETE_HOOK hook, BOOL to_head)
{
#ifdef INCLUDE_RTP
    rtpDeleteHookAdd(hook, to_head);
#endif
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
