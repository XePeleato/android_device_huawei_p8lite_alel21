/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: iptftp_h.h,v $ $Revision: 1.8 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/src/iptftp_h.h,v $
 *   $Author: lob $ $Date: 2005-11-17 13:04:01 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */
#ifndef IPTFTP_H_H
#define IPTFTP_H_H

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

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_clib.h>
#include <ipcom_err.h>
#include <ipcom_os.h>
#include <ipcom_syslog.h>
#include <ipcom_sock.h>
#ifdef IPCOM_USE_INET6
#include <ipcom_sock6.h>
#endif
#include <ipcom_sysvar.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Syslog facility */
#ifdef IPCOM_SYSLOG_FACILITY
#undef IPCOM_SYSLOG_FACILITY
#endif
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPTFTP

/* Syslog priority */
#ifdef IPCOM_SYSLOG_PRIORITY
#undef IPCOM_SYSLOG_PRIORITY
#endif
#ifdef IP_DEBUG
#define IPCOM_SYSLOG_PRIORITY    IPCOM_LOG_DEBUG2
#else
#define IPCOM_SYSLOG_PRIORITY    IPCOM_LOG_WARNING
#endif

/* Directory separator character */
#define IPTFTP_DIR_SEP          '/'

/* tftp port */
#define IPTFTP_PORT             69

/* tftp maximum packet size */
#define IPTFTP_MAX_PACKET       516

/* TFTP opcodes */
#define IPTFTP_OPCODE_RRQ       1
#define IPTFTP_OPCODE_WRQ       2
#define IPTFTP_OPCODE_DATA      3
#define IPTFTP_OPCODE_ACK       4
#define IPTFTP_OPCODE_ERR       5

/* TFTP Error codes */
#define IPTFTP_ERRCODE_NOTDEFINED               0
#define IPTFTP_ERRCODE_FILENOTFOUND             1
#define IPTFTP_ERRCODE_ACCESSVIOLATION          2
#define IPTFTP_ERRCODE_DISKFULL                 3
#define IPTFTP_ERRCODE_ILLEGALTFTPOP            4
#define IPTFTP_ERRCODE_UNKNKOWNTRANSFERID       5
#define IPTFTP_ERRCODE_FILEALREADYEXISTS        6
#define IPTFTP_ERRCODE_NOSUCHUSER               7

/* TFTP Error messages */
#define IPTFTP_ERRMSG_INCOMPLETEMSG             "Incomplete message."
#define IPTFTP_ERRMSG_INVALIDOPCODE             "Invalid opcode."
#define IPTFTP_ERRMSG_INVALIDMODE               "Invalid mode."
#define IPTFTP_ERRMSG_COULDNOTOPENFILE          "Could not open file."
#define IPTFTP_ERRMSG_COULDNOTCREATEFILE        "Could not create file."
#define IPTFTP_ERRMSG_FILEREADERROR             "File read error."
#define IPTFTP_ERRMSG_INCORRECTSRCPORT          "Incorrect source port."
#define IPTFTP_ERRMSG_FAILEDTOCHANGEDIR         "Failed to change directory."
#define IPTFTP_ERRMSG_INVALIDLINEDELIMITER      "Invalid line delimiter received."
#define IPTFTP_ERRMSG_FILEWRITEERROR            "File write error."


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

typedef union
{
    struct Ip_in_addr addr;
#ifdef IPCOM_USE_INET6
    struct Ip_in6_addr addr6;
#endif
}
Iptftp_addr;


typedef struct Iptftp_struct
{
    Ip_s32 family;              /* Internet address family */
#ifdef IPCOM_USE_INET6
    Ip_u32 ifindex;             /* Interface index for link local v6 addresses */
#endif
    Iptftp_addr peer_addr;      /* Peer address (network endian) */
    Ip_u16 peer_port;           /* Peer port */
    Ip_u16 opcode;              /* Read or Write request */
    Ip_bool binary;             /* True for octet mode */
    Ip_bool verbose;            /* True for statistics and error message stdio printing */
    Ip_u32 nbytes;              /* The number of bytes transfer in the connection */
    Ip_s32 dirlen;              /* Length of the server working directory */
    char file[256];             /* Filename with absolute path */
    /* ancillary data buffer */
#ifdef IPCOM_USE_INET6
    char controlbuf[IP_CMSG_SPACE(sizeof(struct Ip_in6_pktinfo))];
#else
    char controlbuf[IP_CMSG_SPACE(sizeof(struct Ip_in_pktinfo))];
#endif
    Ip_socklen_t controllen;    /* ancillary data len */
}
Iptftp;


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

IP_GLOBAL Ip_err iptftp_sendto(Ip_fd sock, Iptftp *tftp, char *buf, Ip_s32 len);
IP_GLOBAL Ip_err iptftp_bind_zero(Ip_fd sock, Ip_s32 family);
IP_GLOBAL Ip_s32 iptftp_recvfrom(Ip_fd sock, Iptftp *tftp, char *buf, Ip_s32 bufsize);
IP_GLOBAL void iptftp_send_err(Ip_fd sock, Iptftp *tftp, Ip_u16 err_code, char *err_msg);
IP_GLOBAL Ip_err iptftp_receive_loop(Ip_fd sock, IP_FILE *fp, char **err_msg, Ip_u16 *err_code, Iptftp *tftp);
IP_GLOBAL Ip_err iptftp_send_loop(Ip_fd sock, IP_FILE *fp, char **err_msg, Ip_u16 *err_code, Iptftp *tftp);
IP_GLOBAL void iptftp_strip_path(char *path);
IP_GLOBAL int iptftp_cmd_tftp(int argc, char **argv);


#ifdef __cplusplus
}
#endif

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
