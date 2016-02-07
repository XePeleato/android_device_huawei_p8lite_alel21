/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipftps_h.h,v $ $Revision: 1.10 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/src/ipftps_h.h,v $
 *   $Author: ulf $ $Date: 2009-06-08 11:33:05 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Ulf Olofsson <ulf@interpeak.se>
 ******************************************************************************
 */
#ifndef IPFTPS_H_H
#define IPFTPS_H_H

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

#include "ipftps_config.h"
#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipftps.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Telnet */
#define IPFTPS_SE       240
#define IPFTPS_NOP      241
#define IPFTPS_DM       242
#define IPFTPS_BRK      243
#define IPFTPS_IP       244
#define IPFTPS_AO       245
#define IPFTPS_AYT      246
#define IPFTPS_EC       247
#define IPFTPS_EL       248
#define IPFTPS_GA       249
#define IPFTPS_SB       250
#define IPFTPS_WILL     251
#define IPFTPS_WONT     252
#define IPFTPS_DO       253
#define IPFTPS_DONT     254
#define IPFTPS_IAC      255

/* Line terminator */
#define CL "\r\n"

/* Source code line string */
#define __LINESTR(line) #line
#define __LINESTR_(line) __LINESTR(line)
#define __LINESTR__ __LINESTR_(__LINE__)

/* Syslog facility code */
#define IPCOM_SYSLOG_FACILITY IPCOM_LOG_IPFTP

/* Create log messages */
#undef IPCOM_SYSLOG_BODY
#ifdef IP_DEBUG
#define IPCOM_SYSLOG_BODY(x) "%s at " __FILE__ "(" __LINESTR__ "): " x, \
                             (session == IP_NULL ? "ipftps" : session->name)
#else
#define IPCOM_SYSLOG_BODY(x) "%s: " x, \
                             (session == IP_NULL ? "ipftps" : session->name)
#endif

/* Time values */
#define IPFTPS_MONTHS  (30L * IPFTPS_DAYS)
#define IPFTPS_DAYS    (24L * IPFTPS_HOURS)
#define IPFTPS_HOURS   (60L * IPFTPS_MINUTES)
#define IPFTPS_MINUTES (60L)

/* Max length of IP address string */
#define IPFTPS_IPV4_MAXLEN \
        sizeof("255.255.255.255")
#define IPFTPS_IPV6_MAXLEN \
        sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")

/* IPv4/IPv6 macros */
#ifdef IPCOM_USE_INET6

#define IPFTPS_AFMAX 2
#define IPFTPS_AFSTR "(1,2)"
#define IPFTPS_IP_MAXLEN IPFTPS_IPV6_MAXLEN
#define IPFTPS_PORT(s) *((s)->sa.sa_family == IP_AF_INET ? \
                       &(s)->sin.sin_port : \
                       &(s)->sin6.sin6_port)
#define IPFTPS_SOCKADDRLEN(s) ((s)->sa.sa_family == IP_AF_INET ? \
                              sizeof((s)->sin) : \
                              sizeof((s)->sin6))
#define IPFTPS_ADDRCMP(a1,a2) ((a1)->sa.sa_family != (a2)->sa.sa_family || \
                              ((a1)->sa.sa_family == IP_AF_INET ? \
                              ipcom_memcmp(&(a1)->sin.sin_addr, &(a2)->sin.sin_addr, \
                              sizeof((a1)->sin.sin_addr)) : \
                              ipcom_memcmp(&(a1)->sin6.sin6_addr, &(a2)->sin6.sin6_addr, \
                              sizeof((a1)->sin6.sin6_addr))) != 0)
#else

#define IPFTPS_AFMAX 1
#define IPFTPS_AFSTR "(1)"
#define IPFTPS_IP_MAXLEN IPFTPS_IPV4_MAXLEN
#define IPFTPS_PORT(s) ((s)->sin.sin_port)
#define IPFTPS_SOCKADDRLEN(s) (sizeof((s)->sin))
#define IPFTPS_ADDRCMP(a1,a2) ((a1)->sa.sa_family != (a2)->sa.sa_family || \
                              ipcom_memcmp(&(a1)->sin.sin_addr, &(a2)->sin.sin_addr, \
                              sizeof((a1)->sin.sin_addr)) != 0)

#ifndef IP_AF_INET6
#define IP_AF_INET6 -1 /* Dummy to avoid #ifdefs */
#endif

#endif

/* Liberal free */
#define ipftps_free(x) \
    if ((x) != IP_NULL) do {ipcom_free(x); (x) = IP_NULL; (void)(x);} while (0)

/* A string and its length */
#define ipftps_strandlen(x) x, ipcom_strlen(x)

/*!! to work with ipcom410. */
#ifndef IP_ERRNO_ENOTDIR
#define IP_ERRNO_ENOTDIR 20
#endif

/* IPLITE/IPNET does not support write select */
#if !defined(IPCOM_SOCK_IPCOM)
#define IPFTPS_USE_WRITESELECT
#endif


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 6                    LOCAL FUNCTIONS
 ****************************************************************************
 */

#ifdef IPFTPS_C

/*
 *===========================================================================
 *      ipftps_filewrite_ascii
 *===========================================================================
 * Description:
 *      Write ASCII buffer to file.
 *
 * Parameters:
 *      fd      File descriptor.
 *      buf     Buffer.
 *      nbuf    New buffer.
 *      bufsize Buffer size.
 *      orphant True if last buffer was terminated with '\r'.
 *
 * Returns:
 *      0 if success, 1 otherwise.
 */
IP_STATIC int
ipftps_filewrite_ascii(Ip_fd fd, char *buf, char *nbuf, int bufsize,
                       Ip_bool *orphant);


/*
 *===========================================================================
 *      ipftps_socketwrite_ascii
 *===========================================================================
 * Description:
 *      Write ASCII buffer to socket.
 *
 * Parameters:
 *      fd      File descriptor.
 *      buf     Buffer.
 *      nbuf    New buffer.
 *      bufsize Buffer size.
 *
 * Returns:
 *      0 if success, 1 otherwise.
 */
IP_STATIC int
ipftps_socketwrite_ascii(Ip_fd fd, char *buf, char *nbuf, int bufsize)
;


/*
 *===========================================================================
 *      ipftps_skipdrive
 *===========================================================================
 * Description:
 *      Skip possible drive part of path.
 *
 * Parameters:
 *      path    Directory path.
 *
 * Returns:
 *      New path.
 */
IP_STATIC char *
ipftps_skipdrive(char *path);


/*
 *===========================================================================
 *      ipftps_dirindex
 *===========================================================================
 * Description:
 *      Calculate level of directory path.
 *
 * Parameters:
 *      path    Directory path.
 *      idomin  Min flag.
 *
 * Returns:
 *      Final subdirectory level if idomin == 0,
 *      minimum subdirectory level if idomin == 1.
 */
IP_STATIC int
ipftps_dirindex(char *path, int idomin);


/*
 *===========================================================================
 *      ipftps_dircmp
 *===========================================================================
 * Description:
 *      Check if dir is OK wrt root.
 *
 * Parameters:
 *      root    Directory root.
 *      path    Directory path.
 *
 * Returns:
 *      0 if OK, 1 otherwise.
 */
IP_STATIC int
ipftps_dircmp(char *root, char *path);


/*
 *===========================================================================
 *      ipftps_root_check
 *===========================================================================
 * Description:
 *      Check if path valid, and prepare for its use in next command.
 *
 * Parameters:
 *      session Session descriptor.
 *      path    Directory path.
 *
 * Returns:
 *      0 if path OK, 1 otherwise.
 */
IP_STATIC int
ipftps_root_check(Ipftps_session *session, char **path);


/*
 *===========================================================================
 *      ipftps_ls
 *===========================================================================
 * Description:
 *      List file (long format).
 *
 * Parameters:
 *      session Session descriptor.
 *      fd      File descriptor.
 *      path1   1st part of path.
 *      path2   2nd part of path.
 *
 * Returns:
 */
IP_STATIC int
ipftps_ls(Ipftps_session *session, Ip_fd fd, char *path1, char *path2);


/*
 *===========================================================================
 *      ipftps_abor
 *===========================================================================
 * Description:
 *      ABOR command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_abor(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_acct
 *===========================================================================
 * Description:
 *      ACCT command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_acct(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_appe
 *===========================================================================
 * Description:
 *      APPE command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_appe(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_cdup
 *===========================================================================
 * Description:
 *      CDUP command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_cdup(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_cwd
 *===========================================================================
 * Description:
 *      CWD command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_cwd(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_allo
 *===========================================================================
 * Description:
 *      ALLO command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_allo(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_dele
 *===========================================================================
 * Description:
 *      DELE command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_dele(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_eprt
 *===========================================================================
 * Description:
 *      EPRT command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_eprt(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_epsv
 *===========================================================================
 * Description:
 *      EPSV command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_epsv(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_help
 *===========================================================================
 * Description:
 *      HELP command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_help(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_list
 *===========================================================================
 * Description:
 *      LIST command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_list(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_mkd
 *===========================================================================
 * Description:
 *      MKD command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_mkd(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_mode
 *===========================================================================
 * Description:
 *      MODE command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_mode(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_nlst
 *===========================================================================
 * Description:
 *      NLST command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_nlst(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_noop
 *===========================================================================
 * Description:
 *      NOOP command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_noop(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_pass
 *===========================================================================
 * Description:
 *      PASS command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_pass(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_pasv
 *===========================================================================
 * Description:
 *      PASV command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_pasv(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_port
 *===========================================================================
 * Description:
 *      PORT command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_port(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_pwd
 *===========================================================================
 * Description:
 *      PWD command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_pwd(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_quit
 *===========================================================================
 * Description:
 *      QUIT command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_quit(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_retr
 *===========================================================================
 * Description:
 *      RETR command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_retr(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_rmd
 *===========================================================================
 * Description:
 *      RMD command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_rmd(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_rnfr
 *===========================================================================
 * Description:
 *      RNFR command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_rnfr(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_rnto
 *===========================================================================
 * Description:
 *      RNTO command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_rnto(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_stat
 *===========================================================================
 * Description:
 *      STAT command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_stat(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_stor
 *===========================================================================
 * Description:
 *      STOR command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_stor(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_stru
 *===========================================================================
 * Description:
 *      STRU command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_stru(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_syst
 *===========================================================================
 * Description:
 *      SYST command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_syst(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_type
 *===========================================================================
 * Description:
 *      TYPE command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_type(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_user
 *===========================================================================
 * Description:
 *      USER command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_user(Ipftps_session *session, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftps_xftp
 *===========================================================================
 * Description:
 *      XFTP command.
 *
 *      WARNING--This function is designed to be used for test
 *      purposes and contains a number of commands that may be
 *      used to compromise security. It is of utmost importance
 *      that this function is disabled before deploying the FTP
 *      server. This is done by defining IPFTPS_USE_TEST to 0.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
#ifdef IPFTPS_USE_TEST
IP_STATIC void
ipftps_xftp(Ipftps_session *session, int argc, char *argv[]);
#endif


/*
 *===========================================================================
 *      ipftps_ntop
 *===========================================================================
 * Description:
 *      Print IP address using ntop.
 *
 * Parameters:
 *      addr    IP address.
 *      dest    Result buffer.
 *      len     Size of result buffer.
 *
 * Returns:
 *      Pointer to buffer if success, 0 otherwise.
 */
IP_STATIC const
char *ipftps_ntop(union Ip_sockaddr_union *addr, char *dest, int len);


/*
 *===========================================================================
 *      ipftps_perror
 *===========================================================================
 * Description:
 *      Send errno based error message to control socket.
 *
 * Parameters:
 *      session Session descriptor.
 *      no      FTP error number.
 *
 * Returns:
 *      Nothing.
 */
IP_STATIC void
ipftps_perror(Ipftps_session *session, int no);


/*
 *===========================================================================
 *      ipftps_dsock_open
 *===========================================================================
 * Description:
 *      Open data socket.
 *
 * Parameters:
 *      session         Session descriptor.
 *      passive_open    True if opening a passive socket.
 *
 * Returns:
 *      0 if success, 1 otherwise.
 */
IP_STATIC int
ipftps_dsock_open(Ipftps_session *session, int passive_open);


/*
 *===========================================================================
 *      ipftps_dsock_close
 *===========================================================================
 * Description:
 *      Close data socket.
 *
 * Parameters:
 *      session Session descriptor.
 *
 * Returns:
 *      Nothing.
 */
IP_STATIC void
ipftps_dsock_close(Ipftps_session *session);


/*
 *===========================================================================
 *      ipftps_write
 *===========================================================================
 * Description:
 *      Write to socket, terminate session if control socket down.
 *
 * Parameters:
 *      session Session descriptor.
 *      fd      Socket descriptor.
 *      buf     Buffer.
 *      len     Buffer length.
 *
 * Returns:
 *      0 if success, 1 otherwise.
 */
IP_STATIC int
ipftps_write(Ipftps_session *session, Ip_fd fd, void *buf, int len);


/*
 *===========================================================================
 *      ipftps_reply_sock
 *===========================================================================
 * Description:
 *      Reply to peer.
 *
 * Parameters:
 *      session Session descriptor.
 *      fd      Socket descriptor.
 *      fmt     Format string.
 *      ...     Additional parms.
 *
 * Returns:
 *      Nothing.
 */
IP_STATIC void
ipftps_reply_sock(Ipftps_session *session, Ip_fd fd, const char *fmt, ...);


/*
 *===========================================================================
 *      ipftps_reply
 *===========================================================================
 * Description:
 *      Reply to the peer PI.
 *
 * Parameters:
 *      session Session descriptor.
 *      fmt     Format string.
 *      ...     Additional parms.
 *
 * Returns:
 *      Nothing.
 */
IP_STATIC void
ipftps_reply(Ipftps_session *session, const char *fmt, ...);


/*
 *===========================================================================
 *      ipftps_telnet
 *===========================================================================
 * Description:
 *      Handle telnet stuff.
 *
 * Parameters:
 *      session Session descriptor.
 *
 * Returns:
 *      0 if success, 1 otherwise.
 */
IP_STATIC int
ipftps_telnet(Ipftps_session *session);


/*
 *===========================================================================
 *      ipftps_get_command
 *===========================================================================
 * Description:
 *      Get a complete command from the PI socket.
 *
 * Parameters:
 *      session Session descriptor.
 *
 * Returns:
 *      0 if success, 1 otherwise.
 */
IP_STATIC int
ipftps_get_command(Ipftps_session *session);


/*
 *===========================================================================
 *      ipftps_parse_command
 *===========================================================================
 * Description:
 *      Parse a command from the PI socket.
 *
 * Parameters:
 *      session Session descriptor.
 *
 * Returns:
 */
IP_STATIC int
ipftps_parse_command(Ipftps_session *session);


/*
 *===========================================================================
 *      ipftps_async
 *===========================================================================
 * Description:
 *      Handle asynchronous commands during file transfer.
 *
 * Parameters:
 *      session Session descriptor.
 *
 * Returns:
 *      1 if an ABOR command is found, 0 otherwise.
 */
IP_STATIC int
ipftps_async(Ipftps_session *session);

#endif

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *      ipftps_session
 *===========================================================================
 * Description:
 *      FTP session handler subprocess.
 *
 * Parameters:
 *
 * Returns:
 */
IPCOM_PROCESS(ipftps_session);


/*
 *===========================================================================
 *      ipftps_main
 *===========================================================================
 * Description:
 *      FTP main process.
 *
 * Parameters:
 *
 * Returns:
 */
IPCOM_PROCESS(ipftps_main);


/*
 *===========================================================================
 *      ipftps_create
 *===========================================================================
 * Description:
 *      Start FTP daemon.
 *
 * Parameters:
 *
 * Returns:
 */
IP_PUBLIC Ip_err
ipftps_create(void);

#ifdef __cplusplus
}
#endif

#endif
/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
