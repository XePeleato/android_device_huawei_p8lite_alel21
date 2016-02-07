/* ipcom_syslog.h - IPCOM syslog library */

/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_syslog.h,v $ $Revision: 1.63.24.1 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_syslog.h,v $
 *     $Author: dbush $ $Date: 2010-07-22 20:42:11 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_SYSLOG_H
#define IPCOM_SYSLOG_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
DESCRIPTION
Public API for ipcom syslog.
INCLUDE FILES: ipcom_type.h, ipcom_cstyle.h, ipcom_sock.h
*/

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"

/* Maximum size of a syslog entry. */
#define IPCOM_SYSLOG_MAX_SIZE       512


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_sock.h"

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
 *                         syslog application macro API
 *===========================================================================
 * How to use:
 * ===========
 * The following two macros must be defined by the application, e.g.:
 *
 *    #define IPCOM_SYSLOG_PRIORITY    IPCOM_LOG_DEBUG
 *    #define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_FTP
 *
 * To change the output look, redefine IPCOM_SYSLOG_BODY e.g:
 *
 *    #undef IPCOM_SYSLOG_BODY
 *    #define IPCOM_SYSLOG_BODY(x) "at " __FILE__ "(" __IP_LINESTR__ "): " x
 *
 * To include local runtime priority checks, redefine IPCOM_SYSLOG_ENABLE, e.g.:
 *
 *    #undef IPCOM_SYSLOG_ENABLE
 *    #define IPCOM_SYSLOG_ENABLE(x) (ipftpd.syslog_level >= IPCOM_LOG_ ## x)
 *
 * Source code usage example:
 *
 *    IPCOM_LOG(WARNING, "Login attempt with user: %s, pw: %s"),
 *              session->username, password IPCOM_END;
 */

/* application macros (may be redefined) */
#if defined(_lint) || !defined(IPCOM_USE_SYSLOG)
#define IPCOM_SYSLOG_ENABLE(x)    (0)
#else
#define IPCOM_SYSLOG_ENABLE(x)    (1)
#endif
#define IPCOM_SYSLOG_BODY(fm)     fm

/* log macros (may not be redefined) */
#define IPCOM_LOGX(x,fm)  if (IPCOM_SYSLOG_PRIORITY >= IPCOM_LOG_ ## x && \
			     IPCOM_SYSLOG_ENABLE(x)) \
			 ipcom_syslog(IPCOM_SYSLOG_FACILITY | IPCOM_LOG_ ## x, \
			              IPCOM_SYSLOG_BODY(fm)
#define IPCOM_ENDX )


/*
 *===========================================================================
 *                         IPCOM_LOGX
 *===========================================================================
 * Same as IPCOM_LOG() except it does not require IPCOM_END.
 */
#if defined(_lint) || !defined(IPCOM_USE_SYSLOG)
#define IPCOM_LOG0(x,fm)                   IP_NOOP
#define IPCOM_LOG1(x,fm,a)                 IP_NOOP
#define IPCOM_LOG2(x,fm,a,b)               IP_NOOP
#define IPCOM_LOG3(x,fm,a,b,c)             IP_NOOP
#define IPCOM_LOG4(x,fm,a,b,c,d)           IP_NOOP
#define IPCOM_LOG5(x,fm,a,b,c,d,e)         IP_NOOP
#define IPCOM_LOG6(x,fm,a,b,c,d,e,f)       IP_NOOP
#define IPCOM_LOG7(x,fm,a,b,c,d,e,f,g)     IP_NOOP
#define IPCOM_LOG8(x,fm,a,b,c,d,e,f,g,i)   IP_NOOP
#define IPCOM_LOG9(x,fm,a,b,c,d,e,f,g,i,j) IP_NOOP
#else
#define IPCOM_LOG0(x,fm)           \
    do { if ((IPCOM_SYSLOG_PRIORITY >= IPCOM_LOG_ ## x) && IPCOM_SYSLOG_ENABLE(x)) \
        ipcom_syslog(IPCOM_SYSLOG_FACILITY | IPCOM_LOG_ ## x, IPCOM_SYSLOG_BODY(fm)); } while((0))
#define IPCOM_LOG1(x,fm,a)         \
    do { if ((IPCOM_SYSLOG_PRIORITY >= IPCOM_LOG_ ## x) && IPCOM_SYSLOG_ENABLE(x)) \
        ipcom_syslog(IPCOM_SYSLOG_FACILITY | IPCOM_LOG_ ## x, IPCOM_SYSLOG_BODY(fm),a); } while((0))
#define IPCOM_LOG2(x,fm,a,b)       \
    do { if ((IPCOM_SYSLOG_PRIORITY >= IPCOM_LOG_ ## x) && IPCOM_SYSLOG_ENABLE(x)) \
        ipcom_syslog(IPCOM_SYSLOG_FACILITY | IPCOM_LOG_ ## x, IPCOM_SYSLOG_BODY(fm),a,b); } while((0))
#define IPCOM_LOG3(x,fm,a,b,c)     \
    do { if ((IPCOM_SYSLOG_PRIORITY >= IPCOM_LOG_ ## x) && IPCOM_SYSLOG_ENABLE(x)) \
        ipcom_syslog(IPCOM_SYSLOG_FACILITY | IPCOM_LOG_ ## x, IPCOM_SYSLOG_BODY(fm),a,b,c); } while((0))
#define IPCOM_LOG4(x,fm,a,b,c,d)   \
    do { if ((IPCOM_SYSLOG_PRIORITY >= IPCOM_LOG_ ## x) && IPCOM_SYSLOG_ENABLE(x)) \
        ipcom_syslog(IPCOM_SYSLOG_FACILITY | IPCOM_LOG_ ## x, IPCOM_SYSLOG_BODY(fm),a,b,c,d); } while((0))
#define IPCOM_LOG5(x,fm,a,b,c,d,e) \
    do { if ((IPCOM_SYSLOG_PRIORITY >= IPCOM_LOG_ ## x) && IPCOM_SYSLOG_ENABLE(x)) \
        ipcom_syslog(IPCOM_SYSLOG_FACILITY | IPCOM_LOG_ ## x, IPCOM_SYSLOG_BODY(fm),a,b,c,d,e); } while((0))
#define IPCOM_LOG6(x,fm,a,b,c,d,e,f) \
    do { if ((IPCOM_SYSLOG_PRIORITY >= IPCOM_LOG_ ## x) && IPCOM_SYSLOG_ENABLE(x)) \
        ipcom_syslog(IPCOM_SYSLOG_FACILITY | IPCOM_LOG_ ## x, IPCOM_SYSLOG_BODY(fm),a,b,c,d,e,f); } while((0))
#define IPCOM_LOG7(x,fm,a,b,c,d,e,f,g) \
    do { if ((IPCOM_SYSLOG_PRIORITY >= IPCOM_LOG_ ## x) && IPCOM_SYSLOG_ENABLE(x)) \
        ipcom_syslog(IPCOM_SYSLOG_FACILITY | IPCOM_LOG_ ## x, IPCOM_SYSLOG_BODY(fm),a,b,c,d,e,f,g); } while((0))
#define IPCOM_LOG8(x,fm,a,b,c,d,e,f,g,i) \
    do { if ((IPCOM_SYSLOG_PRIORITY >= IPCOM_LOG_ ## x) && IPCOM_SYSLOG_ENABLE(x)) \
        ipcom_syslog(IPCOM_SYSLOG_FACILITY | IPCOM_LOG_ ## x, IPCOM_SYSLOG_BODY(fm),a,b,c,d,e,f,g,i); } while((0))
#define IPCOM_LOG9(x,fm,a,b,c,d,e,f,g,i,j) \
    do { if ((IPCOM_SYSLOG_PRIORITY >= IPCOM_LOG_ ## x) && IPCOM_SYSLOG_ENABLE(x)) \
        ipcom_syslog(IPCOM_SYSLOG_FACILITY | IPCOM_LOG_ ## x, IPCOM_SYSLOG_BODY(fm),a,b,c,d,e,f,g,i,j); } while((0))
#endif


/*
 *===========================================================================
 *                    user or kernel functions
 *===========================================================================
 */
#if (!defined(IPCOM_KERNEL) && defined(IPCOM_USRLIB)) || defined(IPCOM_USE_REMOTE_SYSLOG)
#define ipcom_openlog         ipcom_openlog_usr
#define ipcom_closelog        ipcom_closelog_usr
#define ipcom_setlogmask_on   ipcom_setlogmask_on_usr
#define ipcom_getlogmask_on   ipcom_getlogmask_on_usr
#define ipcom_vsyslog         ipcom_vsyslog_usr
#define ipcom_syslog          ipcom_syslog_usr
#define ipcom_setlogfile      ipcom_setlogfile_usr
#define ipcom_setlogudp       ipcom_setlogudp_usr
#endif


/*
 *===========================================================================
 *               Facility codes and priorities
 *===========================================================================
 * Priorities/facilities are encoded into a single 32-bit quantity, where the
 * bottom 3 bits are the priority (0-7) and the top 28 bits are the facility
 * (0-big number).  Both the priorities and the facilities map roughly
 * one-to-one to strings in the syslogd(8) source code.  This mapping is
 * included in this file.
 */

/* priorities (internal) - bottom 3 bits */
#define	IPCOM_LOG_EMERG	         0  /* system is unusable */
#define	IPCOM_LOG_CRIT	         1  /* critical conditions */
#define	IPCOM_LOG_ERR	         2  /* error conditions */
#define	IPCOM_LOG_WARNING        3  /* warning conditions */
#define	IPCOM_LOG_NOTICE         4  /* normal but signification condition */
#define	IPCOM_LOG_INFO	         5  /* informational */
#define	IPCOM_LOG_DEBUG	         6  /* debug-level messages */
#define	IPCOM_LOG_DEBUG2         7  /* interpeak debug-level messages */

#define	IPCOM_LOG_NONE	         -1 /* Not a bit (for compiling without logs) */

#define	IPCOM_LOG_PRIMASK        0x7    /* mask to extract priority */
#define IPCOM_LOG_PRI(p)         ((p) & IPCOM_LOG_PRIMASK)
#define IPCOM_LOG_PRI_MAXLEN     9 /* strlen("Warning: ") */

/* facility codes - top 28 bits */
#define IPCOM_LOG_KERN           (0 << 3)  /* kernel messages */
#define IPCOM_LOG_USER           (1 << 3)  /* random user-level messages */
#define IPCOM_LOG_DAEMON         (2 << 3)  /* unix system daemons. */
#define IPCOM_LOG_AUTH           (3 << 3)  /* security/authorization messages */
#define IPCOM_LOG_SYSLOG         (4 << 3)  /* messages generated internally by syslogd */
#define IPCOM_LOG_IPCOM          (5 << 3)
#define IPCOM_LOG_IPCOM_SHELL    (6 << 3)
#define IPCOM_LOG_IPCOM_TELNET   (7 << 3)
#define IPCOM_LOG_IPCRYPTO       (8 << 3)
#define IPCOM_LOG_IPIPSEC        (9 << 3)
#define IPCOM_LOG_IPIKE          (10 << 3)
#define IPCOM_LOG_IPL2TP         (11 << 3)
#define IPCOM_LOG_IPLDAPC        (12 << 3)
#define IPCOM_LOG_IPLITE         (13 << 3)
#define IPCOM_LOG_IPNAT          (14 << 3)
#define IPCOM_LOG_IPPPPOE        (15 << 3)
#define IPCOM_LOG_IPRADIUS       (16 << 3)
#define IPCOM_LOG_IPRIP          (17 << 3)
#define IPCOM_LOG_IPSSH          (18 << 3)
#define IPCOM_LOG_IPSSL          (19 << 3)
#define IPCOM_LOG_IPSSLPROXY     (20 << 3)
#define IPCOM_LOG_IPFTP          (21 << 3)
#define IPCOM_LOG_IPFIREWALL     (22 << 3)
#define IPCOM_LOG_IPDHCPD        (23 << 3)
#define IPCOM_LOG_IPDHCPC        (24 << 3)
#define IPCOM_LOG_IPWEBS         (25 << 3)
#define IPCOM_LOG_IPNET          (26 << 3)
#define IPCOM_LOG_IPTFTP         (27 << 3)
#define IPCOM_LOG_IPSNTP         (28 << 3)
#define IPCOM_LOG_IPDHCPS        (29 << 3)
#define IPCOM_LOG_IPDHCPS6       (30 << 3)
#define IPCOM_LOG_IPSNMP         (31 << 3)
#define IPCOM_LOG_IPDHCPR        (32 << 3)
#define IPCOM_LOG_IPCOM_DRV_ETH  (33 << 3)
#define IPCOM_LOG_IPPPP          (34 << 3)
#define IPCOM_LOG_IPMIP          (35 << 3)
#define IPCOM_LOG_IPAPPL         (36 << 3)
#define IPCOM_LOG_IPTCP          (37 << 3)
#define IPCOM_LOG_IPMLDS         (38 << 3)
#define IPCOM_LOG_IPEMANATE      (39 << 3)
#define IPCOM_LOG_IPFREESCALE    (40 << 3)
#define IPCOM_LOG_IPMCP          (41 << 3)
#define IPCOM_LOG_IPPRISM        (42 << 3)
#define IPCOM_LOG_IP8021X        (43 << 3)
#define IPCOM_LOG_IPEAP          (44 << 3)
#define IPCOM_LOG_IPSAFENET      (45 << 3)
#define IPCOM_LOG_IPHWCRYPTO     (46 << 3)
#define IPCOM_LOG_IPNETSNMP      (47 << 3)
#define IPCOM_LOG_IPQUAGGA       (48 << 3)
#define IPCOM_LOG_IPDHCPC6       (49 << 3)
#define IPCOM_LOG_IPCCI          (50 << 3)
#define IPCOM_LOG_IPDIAMETER     (51 << 3)
#define IPCOM_LOG_IPROHC     	 (52 << 3)
#define IPCOM_LOG_IPMPLS     	 (53 << 3)
#define IPCOM_LOG_IPCAVIUM     	 (54 << 3)
#define IPCOM_LOG_IPSCTP         (55 << 3)
#define IPCOM_LOG_IPIFPROXY      (56 << 3)
#define IPCOM_LOG_IPCOM_KEY_DB   (57 << 3)
#define IPCOM_LOG_IPWPS     	 (58 << 3)
#define IPCOM_LOG_WRNAD          (59 << 3)

/* Keep updated! (also update <IPCOM>/src/ipcom_syslog_data.c: ipcom_syslogd_facility_names[] */
#define IPCOM_LOG_FACILITY_MAX   (60)

/* Mask to extract facility part facility of pri. */
#define IPCOM_LOG_FACMASK        0xfff8
#define IPCOM_LOG_FAC(p)         (((p) & IPCOM_LOG_FACMASK) >> 3)

#define IPCOM_LOG_UPTO(pri)      (pri)


/*
 *===========================================================================
 *                         openlog option flags
 *===========================================================================
 */
#define IP_LOG_PID    0x01    /* log the process ID with each message */
#define IP_LOG_CONS   0x02    /* [UNUSED] Defined for compilation reasons only. */


/*
 ****************************************************************************
 * 5                    DATA
 ****************************************************************************
 */

IP_EXTERN const char *ipcom_syslog_priority_names[8];

IP_EXTERN const char *ipcom_syslog_facility_names[IPCOM_LOG_FACILITY_MAX+1];


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_openlog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_openlog(const char *ident, Ip_u32 option, Ip_u32 facility);


/*
 *===========================================================================
 *                    ipcom_closelog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_closelog(void);


/*
 *===========================================================================
 *                    ipcom_syslog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_syslog(Ip_u32 priority, const char *format, ...);


/*
 *===========================================================================
 *                    ipcom_vsyslog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 * IP_PUBLIC void
 * ipcom_vsyslog (Ip_u32 priority, const char *format, va_list args)
 */


/*******************************************************************************
*
* ipcom_setlogmask_on - set the log level for a facility
*
* DESCRIPTION
* Sets the log level for a facility
*
* Parameters:
* \is
* \i <maskpri>
* New priority. All entries with this priority and higher will be output to the
* syslog
* \i <facility>
* The facility to set the log level for (IPCOM_LOG_IPXX).
* \ie
*
* RETURNS:
* Old priority if the priority was successfully updated, or -1 if facility
* does not exist.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_setlogmask_on(
				  int maskpri,
				  Ip_u32 facility
				  );

#define IPCOM_LOG_UPTO(pri)  (pri)


/*******************************************************************************
*
* ipcom_getlogmask_on - get the log level for a facility
*
* DESCRIPTION
* Gets the log level for a facility.
*
* Parameter:
* \is
* \i <facility>
* The facility to get the log level for (IPCOM_LOG_IPXX).
* \ie
*
* RETURNS:
* Current priority of the facility, or -1 if facility does not exist.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_getlogmask_on(
				  Ip_u32 facility
				  );


/*******************************************************************************
*
* ipcom_setlogfile - set the path to the syslog file
*
* DESCRIPTION
* Sets the path to the syslog file.
*
* Parameter:
* \is
* \i <file>
* New path to the syslog file.
* \ie
*
* RETURNS:
* IPCOM_SUCCESS if the path was successfully set, or -1 if the path could not
* be updated.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_setlogfile(
			       const char *file
			       );


/*******************************************************************************
*
* ipcom_setlogudp - set the UDP address and port to send the syslog entries to
*
* DESCRIPTION
* Sets the UDP address and port to which to send the syslog entries.
*
* Parameters:
* \is
* \i <ipaddr>
* Destination IP address.
* \i <port>
* Destination UDP port.
* \ie
*
* RETURNS:
* IPCOM_SUCCESS if the address and port were successfully set, or -1 if the
* address and port could not be updated.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_setlogudp(
			      const char *ipaddr,
			      Ip_u16 port
			      );


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

