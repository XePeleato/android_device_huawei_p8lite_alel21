/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_signal.h,v $ $Revision: 1.34 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_signal.h,v $
 *   $Author: zruan $ $Date: 2009-09-18 19:15:37 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth.jonsson@windriver.com>
 ******************************************************************************
 */
#ifndef IPNET_SIGNAL_H
#define IPNET_SIGNAL_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */
/*
   Definition of all signals that can be sent and received from the
   IPNET socket daemon.
*/

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_atomic.h>
#include <ipcom_cstyle.h>
#include <ipcom_netif.h>
#include <ipcom_pipe.h>
#include <ipcom_pkt.h>
#include <ipcom_sock.h>
#include <ipcom_type.h>

#ifdef IPIPSEC2
#include <ipipsec.h>
#endif

#include "ipnet_timeout.h"

#ifdef __cplusplus
extern "C" {
#endif

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

enum Ipnet_sock_poll
{
    IPNET_SOCK_POLL_READ,
    IPNET_SOCK_POLL_WRITE,
    IPNET_SOCK_POLL_EXCEPT
};

enum Ipnet_sig_type
{
    IPNET_SIG_ACCEPT,
    IPNET_SIG_BIND,
    IPNET_SIG_CLOSE,
    IPNET_SIG_CONNECT,
    IPNET_SIG_GETANCILLARYDATA,
    IPNET_SIG_GETNAME,
    IPNET_SIG_GETSOCKOPT,
    IPNET_SIG_IF_INDEXTONAME,
    IPNET_SIG_IF_NAMETOINDEX,
    IPNET_SIG_IOCTL,
    IPNET_SIG_LISTEN,
    IPNET_SIG_POLL,
    IPNET_SIG_POLL_ASYNC,
    IPNET_SIG_POLL_DONE,
    IPNET_SIG_POLL_TIMEOUT,
    IPNET_SIG_RESUME,
    IPNET_SIG_SENDMSG,
    IPNET_SIG_SETSOCKOPT,
    IPNET_SIG_SOCKET,
    IPNET_SIG_SOCKETFREE,
    IPNET_SIG_SOCKETFREE_ASYNC,
    IPNET_SIG_SHUTDOWN,
    IPNET_SIG_SUSPEND,
    IPNET_SIG_SYSCTL,
    IPNET_SIG_WAKEUP,
    IPNET_SIG_WINDOW_UPDATE,
    IPNET_SIG_IPSEC,
    IPNET_SIG_IF_ATTACH,
    IPNET_SIG_IF_CHANGEMTU,
    IPNET_SIG_IF_CHANGENAME,
    IPNET_SIG_IF_DETACH,
    IPNET_SIG_IF_CHANGELINKADDR,
    IPNET_SIG_IF_FREE,
    IPNET_SIG_IF_MALLOC,
    IPNET_SIG_IF_NAMETONETIF,
    IPNET_SIG_PORT_SPECIFIC,
    IPNET_SIG_STATS,
#ifdef IPIFPROXY
    IPNET_SIG_CTRL_SOCK_RX,
    IPNET_SIG_DATA_SOCK_RX,
#endif
    IPNET_SIG_SCTP_SEND_INIT,
    IPNET_SIG_SCTP_SET_STATE,
    IPNET_SIG_SCTP_WINDOW_UPDATE
};

struct Ipnet_sig_accept
{
    Ip_fd                      fd;
    struct Ip_sockaddr_storage from;
    Ip_socklen_t               fromlen;
};

struct Ipnet_sig_ancillary_data
{
    Ip_fd             fd;
    struct Ip_msghdr *msg;
    Ipcom_pkt        *pkt;
};

struct Ipnet_sig_bind
{
    Ip_fd                      fd;
    struct Ip_sockaddr_storage name;
    Ip_socklen_t               namelen;
};

struct Ipnet_sig_connect
{
    Ip_fd                      fd;
    struct Ip_sockaddr_storage to;
    Ip_socklen_t               tolen;
};

struct Ipnet_sig_getname
{
    Ip_fd                      fd;
    Ip_bool                    get_local_name;
    struct Ip_sockaddr_storage name;
};

struct Ipnet_sig_if_indexname
{
    /* Total number of attached interfaces */
    Ip_u32 num_attached;
    /* The highest ifindex assigned to any interface */
    Ip_u32 highest_assigned_ifindex;
    Ip_u32 ifindex;
    char   ifname[IP_IFNAMSIZ+1];
};


struct Ipnet_sig_netif
{
    Ipcom_netif *netif;
    Ip_u16       type;
    Ip_u16       vr;
    Ip_u32       mtu;
    Ip_bool      locked;
    Ip_u8       *link_addr;
    Ip_u16       size;
    char         ifname[IP_IFNAMSIZ+1];
};

struct Ipnet_sig_ioctl
{
    Ip_fd         fd;
    unsigned long request;
    void         *argp;
};

struct Ipnet_sig_listen
{
    Ip_fd fd;
    int   backlog;
};

struct Ipnet_sig_reply
{
    int return_code;
    int sock_errno;
};

struct Ipnet_sig_poll
{
    /* File descriptor for the socket this poll is done for */
    Ip_fd                fd;
    /* What to poll for */
    enum Ipnet_sock_poll what;
    /* Minimum number of bytes the caller want to receive/send. 0 will
       use the lo watermark setting from the socket. */
    int                  lowat;
    /* Maximum time the client want to wait for the socket to become
       ready */
    struct Ip_timeval   *timeout;
    /* Timeout handle */
    Ipnet_timeout       **ptmo;
    /* List of receivers/senders waiting to receive/send on the same
       socket. The list contains Ipnet_sig instances, one entry for
       each waiting (task/process, what) combination. The head of this
       list is Ipnet_socket::poll_list for socket identified by "fd". */
    Ipcom_list           list;
    /* List of outstanding poll requests that all belong to the same
       session. The head of this list is in the
       Ipnet_sig_poll_done::session_list instance for the task/process
       doing this poll. */
    Ipcom_list           session_list;
    /* Number of responses that should have been sent into the
       response pipe. An actual response must only be sent once, when
       this variable is 0. */
    Ipcom_atomic_t      *responses_sent;
    /* Must be the last element in this structure, used to figure out
       the size needed to create an IPNET_SIG_POLL signal */
    int                  last;
};

struct Ipnet_sig_poll_done
{
    /* Storage of the 'responses_sent' variable that all struct
       Ipnet_sig_poll structures referes to */
    Ipcom_atomic_t responses_sent;
    /* Storage of the timeout handle variable that all struct
       Ipnet_sig_poll structures referes to via 'ptmo' */
    Ipnet_timeout *tmo;
    /* List of all poll requests in this session. struct
       Ipnet_sig_poll elements are stored using their 'session_list'
       memeber */
    Ipcom_list     session_list_head;
};

struct Ipnet_sig_sendmsg
{
    Ip_fd                      fd;
    Ipcom_pkt                 *pkt;
    struct Ip_msghdr           msg;
    struct Ip_sockaddr_storage to;
    struct Ipnet_sig_struct   *rsig; /* Response signal for synchronous
                                        calls */
};

struct Ipnet_sig_socket
{
    int domain;
    int type;
    int protocol;
};

struct Ipnet_sig_sockopt
{
    Ip_fd        fd;
    int          level;
    int          optname;
    void        *optval;
    Ip_socklen_t optlen;
};

struct Ipnet_sig_shutdown
{
    Ip_fd fd;
    int   how;
};

struct Ipnet_sig_sysctl
{
    int         *name;
    unsigned int namelen;
    void        *oldp;
    Ip_size_t   *oldlenp;
    void        *newp;
    Ip_size_t    newlen;
};

struct Ipnet_sig_window_update
{
    Ip_fd  fd;
};
#ifdef IPSCTP
struct Ipnet_sig_sctp_send_init
{
    Ip_fd  fd;
    Ip_u32 vtag;
};

struct Ipnet_sig_sctp_set_state
{
    Ip_u32 vtag;
    int    state;
};

struct Ipnet_sig_sctp_wu
{
    void  *tcb;
    Ip_u32 delay_msec;
    Ip_u32 send_tsn_next;
    Ip_u32 send_peerwnd;
    Ip_u32 recv_tsn_next;
};
#endif

#ifdef IPIPSEC2
enum Ipipsec_sig_type
{
    IPIPSEC_SIG_DECRYPT,       /* IPSEC daemon pipe */
    IPIPSEC_SIG_ENCRYPT,       /* IPSEC daemon pipe */
    IPIPSEC_SIG_LOCK,          /* IPSEC daemon pipe */
    IPIPSEC_SIG_UNLOCK,        /* IPSEC daemon pipe */
    IPIPSEC_SIG_ACK,           /* Response pipe */
    IPIPSEC_SIG_PKTINPUT,      /* IPNET pipe */
    IPIPSEC_SIG_PKTOUTPUT,     /* IPNET pipe */
    IPIPSEC_SIG_PKTFREE,       /* IPNET pipe */
    IPIPSEC_SIG_CONTROL,       /* IPNET pipe */
    IPIPSEC_SIG_SOFTEXPIRE,    /* IPSEC control pipe */
    IPIPSEC_SIG_HARDEXPIRE,    /* IPSEC control pipe */
    IPIPSEC_SIG_REGISTERUSE,   /* IPSEC control pipe */
    IPIPSEC_SIG_TRAP           /* IPSEC control pipe */
};

struct Ipnet_sig_ipsec
{
    enum Ipipsec_sig_type ipsec_sig_type;
    union
    {
        struct Ipnet_sig_ipsec_packet
        {
            Ipcom_pkt *pkt;
            Ip_u8      domain;
            Ip_u8      proto;
        } packet;
        struct Ipnet_sig_ipsec_crypto
        {
            Ipcom_pkt     *pkt;
            Ipipsec_param  param;
        } crypto;
        struct Ipnet_sig_ipsec_expire
        {
            void *sa;
        } expire;
        struct Ipnet_sig_ipsec_trap
        {
            void          *sa;
            Ipipsec_param  param;
            Ip_u32         traptype;
            Ip_u32         errors;
        } trap;
    } t;
};
#endif /* #ifdef IPIPSEC2 */

struct Ipnet_sig_stats
{
    int proto;
    int sig_errno;
    union
    {
        struct
        {
            Ip_u32 in_receives;
            Ip_u32 in_hdr_errors;
            Ip_u32 in_addr_errors;
            Ip_u32 forw_datagrams;
            Ip_u32 in_unknown_protos;
            Ip_u32 in_discards;
            Ip_u32 in_delivers;
            Ip_u32 out_requests;
            Ip_u32 out_discards;
            Ip_u32 out_no_routes;
            Ip_s32 reasm_timeout;
            Ip_u32 reasm_reqds;
            Ip_u32 reasm_oks;
            Ip_u32 reasm_fails;
            Ip_u32 frag_oks;
            Ip_u32 frag_fails;
            Ip_u32 frag_creates;
        } ip;
        struct
        {
            Ip_u32 in_msgs;
            Ip_u32 in_errors;
            Ip_u32 in_dest_unreachs;
            Ip_u32 in_time_excds;
            Ip_u32 in_parm_probs;
            Ip_u32 in_src_quenchs;
            Ip_u32 in_redirects;
            Ip_u32 in_echos;
            Ip_u32 in_echo_reps;
            Ip_u32 in_timestamps;
            Ip_u32 in_timestamp_reps;
            Ip_u32 in_addr_masks;
            Ip_u32 in_addr_mask_reps;
            Ip_u32 out_msgs;
            Ip_u32 out_errors;
            Ip_u32 out_dest_unreachs;
            Ip_u32 out_time_excds;
            Ip_u32 out_parm_probs;
            Ip_u32 out_src_quenchs;
            Ip_u32 out_redirects;
            Ip_u32 out_echos;
            Ip_u32 out_echo_reps;
            Ip_u32 out_timestamps;
            Ip_u32 out_timestamp_reps;
            Ip_u32 out_addr_masks;
            Ip_u32 out_addr_mask_reps;
        } icmp;
        struct
        {
            Ip_u32 active_opens;
            Ip_u32 passive_opens;
            Ip_u32 attempt_fails;
            Ip_u32 estab_resets;
            Ip_u32 curr_estab;
            Ip_u32 in_segs;
            Ip_u32 out_segs;
            Ip_u32 retrans_segs;
            Ip_u32 in_errs;
            Ip_u32 out_rsts;
        } tcp;
        struct
        {
            Ip_u32 in_datagrams;
            Ip_u32 no_ports;
            Ip_u32 in_errors;
            Ip_u32 out_datagrams;
        } udp;
#ifdef IPSCTP
        struct
        {
            /* Begin: mib in RFC3873 */
            Ip_u32      sctp_Curr_Estab;             /* Gauge32 */
            Ip_u32      sctp_Active_Estabs;
            Ip_u32      sctp_Passive_Estabs;
            Ip_u32      sctp_Aborteds;
            Ip_u32      sctp_Shutdowns;
            Ip_u32      sctp_OutOfBlues;
            Ip_u32      sctp_Checksum_Errors;
            Ip_u64      sctp_Out_Ctrl_Chunks;
            Ip_u64      sctp_Out_Order_Chunks;
            Ip_u64      sctp_Out_Unorder_Chunks;
            Ip_u64      sctp_In_Ctrl_Chunks;
            Ip_u64      sctp_In_Order_Chunks;
            Ip_u64      sctp_In_Unorder_Chunks;
            Ip_u64      sctp_Frag_UsrMsgs;
            Ip_u64      sctp_Reasm_UsrMsgs;
            Ip_u64      sctp_Out_SCTP_Packs;
            Ip_u64      sctp_In_SCTP_Packs;
            Ip_u32      sctp_Discontinuity_Time;
            /* End: mib in RFC3873 */
        }sctp;
#endif
       struct
       {
            Ip_u32    ip6_input;
            Ip_u32    ip6_input_err;
            Ip_u32    ip6_input_hdr_trunc;
            Ip_u32    ip6_input_inval_len;
            Ip_u32    ip6_input_inval_exthdr;
            Ip_u32    ip6_input_not_to_me;
            Ip_u32    ip6_input_too_big;
            Ip_u32    ip6_input_time_exceeded;
            Ip_u32    ip6_input_too_short;
            Ip_u32    ip6_input_tcp_multicast;
            Ip_u32    ip6_input_prot_err;
            Ip_u32    ip6_input_hdr_no_next;
            Ip_u32    ip6_input_raw;
            Ip_u32    ip6_output;
            Ip_u32    ip6_output_err;
            Ip_u32    ip6_output_blackhole;
            Ip_u32    ip6_output_hdr_space;
            Ip_u32    ip6_output_noroute;
            Ip_u32    ip6_output_nomem;
            Ip_u32    ip6_output_enobufs;
            Ip_u32    ip6_output_err_probe;
            Ip_u32    ip6_output_link_err;
       }ip6;

       struct
       {
            Ip_u32    icmp6_input;
            Ip_u32    icmp6_input_err;
            Ip_u32    icmp6_input_badchksum;
            Ip_u32    icmp6_input_nomem;
            Ip_u32    icmp6_send_nomem;
       }
       icmp6;
    } p;
};

typedef struct Ipnet_sig_struct
{
    enum Ipnet_sig_type sig_type;
    Ipcom_atomic_t      garbage_tag;

    void (*free)(struct Ipnet_sig_struct *sig); /* Function that
                                                   knows how to free
                                                   this signal */
    struct Ipnet_sig_struct *freesig;   /* Argument to "free" */

    Ipcom_pipe         *response_pipe;  /* The pipe the response must
                                           be sent through. This
                                           pointer might be IP_NULL
                                           in which case the global
                                           IPNET pipe will be used */
    Ip_pid_t            sender_pid;     /* PID of signal sender */
    Ip_pid_t            receiver_pid;   /* PID of signal receiver */
#if IPCOM_VR_MAX > 1
    Ip_u16              sender_vr;
#endif

    struct Ipnet_sig_reply reply;

    union
    {
        Ip_fd                           close;
        struct Ipnet_sig_accept         accept;
        struct Ipnet_sig_ancillary_data ancillary_data;
        struct Ipnet_sig_bind           bind;
        struct Ipnet_sig_connect        connect;
        struct Ipnet_sig_getname        getname;
        struct Ipnet_sig_netif          netif;
        struct Ipnet_sig_if_indexname   indexname;
        struct Ipnet_sig_ioctl          ioctl;
        struct Ipnet_sig_listen         listen;
        struct Ipnet_sig_poll           poll;
        struct Ipnet_sig_poll_done      poll_done;
        struct Ipnet_sig_sendmsg        sendmsg;
        struct Ipnet_sig_socket         socket;
        struct Ipnet_sig_sockopt        sockopt;
        struct Ipnet_sig_shutdown       shutdown;
        struct Ipnet_sig_sysctl         sysctl;
        struct Ipnet_sig_window_update  window_update;
#ifdef IPIPSEC2
        struct Ipnet_sig_ipsec          ipsec;
#endif
        struct Ipnet_sig_stats          stats;
#ifdef IPSCTP
        struct Ipnet_sig_sctp_send_init sctp_send_init;
        struct Ipnet_sig_sctp_set_state sctp_set_state;
        struct Ipnet_sig_sctp_wu        sctp_wu;
#endif

    } d;
}
Ipnet_sig;

/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

IP_GLOBAL void ipnet_sig_free(Ipnet_sig *sig);

#ifdef IPCOM_USE_PORT_SPECIFIC_SIGNALS
#define IPCOM_PORT_SIG_NO_REPLY 0
#define IPCOM_PORT_SIG_DO_REPLY 1
IP_PUBLIC int ipcom_handle_port_specific_sig(void* sig);

#endif

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
