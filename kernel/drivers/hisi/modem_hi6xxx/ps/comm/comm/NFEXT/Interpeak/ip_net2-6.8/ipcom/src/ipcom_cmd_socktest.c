/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_cmd_socktest.c,v $ $Revision: 1.162.18.1 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_cmd_socktest.c,v $
 *     $Author: ulf $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Jonas Rendel <jonas@interpeak.se>
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

#include "ipcom_config.h"

#define IPCOM_SHELL_HISTORY_LENGTH  10


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef IP_PORT_INTEGRITY
#include "ipcom_ipcom2bsd.h"
#endif

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_clib.h"
#include "ipcom_cmd.h"
#include "ipcom_getopt.h"
#include "ipcom_sock.h"
#include "ipcom_sock2.h"
#include "ipcom_sock6.h"
#include "ipcom_inet.h"
#include "ipcom_err.h"
#include "ipcom_os.h"
#include "ipcom_shell.h"
#include "ipcom_strlib.h"
#include "ipcom_file.h"
#include "ipcom_sysvar.h"

#ifdef IPNET
#include "ipnet_qos.h"
#endif

#ifdef IPSCTP
#include "ipsctp_sock.h"
#endif

/* The ipcom_ipcom2bsd.h for VxWorks must be included as the last include
   file */
#ifdef IP_PORT_VXWORKS
#if IP_PORT_VXWORKS >= 65
/* #define IPCOM_CMD_SOCKTEST_USE_BSD_API */
#endif
#endif

#if defined(IPCOM_CMD_SOCKTEST_USE_BSD_API)
#include "ipcom_ipcom2bsd.h"
#endif

#if defined(IPCOM_CMD_SOCKTEST_USE_ZEROCOPY_API)
#include "zbufSockLib.h"
#include "zbufLib.h"
extern Ipcom_pkt* _zbuf_getseg_nolock(int index);
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_SOCKTEST_LEAVE(xerrval)   do { retval = xerrval; goto leave; } while((0))

#define SOCKTEST_PARAM_ENTRIES(sz)  (sizeof(sz)/sizeof(Ipcom_cmd_socktest_param))
#define SOCKTEST_LEVEL_ENTRIES(sz)  (sizeof(sz)/sizeof(Ipcom_cmd_socktest_sock_level))
#define SOCKTEST_SOCKOPT_ENTRIES(sz)(sizeof(sz)/sizeof(Ipcom_cmd_socktest_sockopt))
#define SOCKTEST_IOCTL_ENTRIES(sz)  (sizeof(sz)/sizeof(Ipcom_cmd_socktest_ioctl))
#define SOCKTEST_MAX_SUBCMD         (sizeof(socktest_cmd_cb)/sizeof(Ipcom_cmd_socktest_cb))
#define SOCKTEST_PROTO_ENTRIES      (sizeof(socktest_prot_format)/sizeof(Ipcom_cmd_socktest_proto_format))


#define SOCKTEST_CMSG_LEN           100

#define SOCKTEST_FLOOD_SINK         (1 << 0)
#define SOCKTEST_FLOOD_SOURCE       (1 << 1)
#define SOCKTEST_FLOOD_SHUTDOWN     (1 << 2)
#define SOCKTEST_FLOOD_BURST        (1 << 3)
#define SOCKTEST_FLOOD_VERIFY       (1 << 4)
#define SOCKTEST_FLOOD_SEND         (1 << 5)

#define SOCKTEST_FLOOD_INIT(f)      do { (f)->flood_data = 0; (f)->flood_burst = 0; (f)->flood_flags = 0; (f)->flood_ret = 0; (f)->flood_error = 0; } while (0)

#ifdef IPSCTP
#define SOCKETEST_ADDR_MAX_NUM      20
#endif

/*
 *===========================================================================
 *                         Frame Values
 *===========================================================================
 * Note: Also used for 3:rd argument to when creating packet sockets.
 */
#define IPCOM_ETH_P_ALL        0
#define IPCOM_ETH_P_IP         0x0800
#define IPCOM_ETH_P_ARP        0x0806
#define IPCOM_ETH_P_IPV6       (int)0x86DD
#define IPCOM_ETH_P_PPPOE_DIS  (int)0x8863
#define IPCOM_ETH_P_PPPOE_SES  (int)0x8864


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

typedef struct Ipcom_cmd_socktest_soevent_struct
{
    Ipcom_list                   list;
    /* One of the IP_SOEVENT_CB_xxx constants */
    enum Ipcom_socket_event_type type;
}
Ipcom_cmd_socktest_soevent;


typedef struct Ipcom_cmd_socktest_fd_struct
{
    int                     fd;
    int                     domain;
    int                     type;
    int                     prot;
    /* Is posted once for each socket event in the event queue */
    Ipcom_sem               event_sem;
    /* List of Ipcom_cmd_socktest_soevent entries */
    Ipcom_list              events;
    /* Mutext that must be held while using the 'events' list */
    Ipcom_mutex             events_mutex;

    /* SOURCE/SINK */
    Ip_u16                  flood_flags;
    Ip_u16                  flood_delay;
    Ip_size_t               flood_data;
    Ip_size_t               flood_burst;
    Ipcom_sem               flood_sem;
    Ip_err                  flood_ret;
    int                     flood_error;
    char                    *flood_errdesc;
}
Ipcom_cmd_socktest_fd;

typedef struct Ipcom_cmd_socktest_param_struct
{
    int    par_no;
    char  *par_name;
}
Ipcom_cmd_socktest_param;

typedef struct Ipcom_cmd_socktest_glob_type_struct
{
    char                    *name;
    Ip_socklen_t            len;
    void * (*parse_cb)(char *data, char *obj);
    char * (*format_cb)(void *data);
}
Ipcom_cmd_socktest_glob_type;

typedef struct Ipcom_cmd_socktest_ancillary_data_struct
{
    int                           level;
    int                           type;
    char                         *name;
    Ipcom_cmd_socktest_glob_type *gtype;
}
Ipcom_cmd_socktest_ancillary_data;

typedef struct Ipcom_cmd_socktest_sockopt_struct
{
    int                           no;
    char                         *name;
    Ipcom_cmd_socktest_glob_type *type;
}
Ipcom_cmd_socktest_sockopt;

typedef struct Ipcom_cmd_socktest_sock_level_struct
{
    int                         no;
    char                       *name;
    Ipcom_cmd_socktest_sockopt *opt_list;
    Ip_size_t                   opt_list_len;
}
Ipcom_cmd_socktest_sock_level;

typedef struct Ipcom_cmd_socktest_ioctl_struct
{
    Ip_u32                        no;
    char                         *name;
    Ipcom_cmd_socktest_glob_type *type;
    Ip_bool                       is_get;
}
Ipcom_cmd_socktest_ioctl;

typedef struct Ipcom_cmd_socktest_proto_format_struct
{
    int domain;
    int type;
    int prot;
    Ip_u8 *(*ll_format_cb)(Ip_u8 *data);
    Ip_u8 *(*nl_format_cb)(Ip_u8 *data);
    Ip_u8 *(*tl_format_cb)(Ip_u8 *data, Ip_size_t len);
}
Ipcom_cmd_socktest_proto_format;

union Ipcom_cmd_socktest_addr
{
    struct Ip_sockaddr       sa;
    struct Ip_sockaddr_in    in;
    struct Ip_sockaddr_in6   in6;
    struct Ip_sockaddr_dl    dl;
    struct Ip_sockaddr_ll    ll;
};

#include "ipcom_align16.h" /* ON */
typedef IP_PACKED1 struct Ipcom_cmd_socktest_eth_hdr_struct
{
    Ip_u8    dst[6];     /* Destination address of the frame */
    Ip_u8    src[6];     /* Source address of the fram */
    Ip_u16   type;       /* Ethernet frame type */
}
IP_PACKED2 Ipcom_cmd_socktest_eth_hdr;

typedef IP_PACKED1 struct Ipcom_tcp_hdr_struct
{
    Ip_u16  sport;
    Ip_u16  dport;
    Ip_u16  seq_num[2];
    Ip_u16  ack_num[2];
    Ip_u16  offset_flags;
    Ip_u16  window;
    Ip_u16  chksum;
    Ip_u16  urgent_pointer;
    Ip_u16  options[1];     /* The TCP header (even one including options) is an
                           integral number of 32 bits long*/
    /* data (if any) */
}
IP_PACKED2 Ipcom_tcp_hdr;

typedef IP_PACKED1 struct Ipcom_icmp6_hdr_struct
{
    Ip_u8   icmp6_type;   /* type field */
    Ip_u8   icmp6_code;   /* code field */
    Ip_u16  icmp6_cksum;  /* checksum field */
    union
    {
        Ip_u32  icmp6_un_data32[1]; /* type-specific field */
        Ip_u16  icmp6_un_data16[2]; /* type-specific field */
        Ip_u8   icmp6_un_data8[4];  /* type-specific field */
    } icmp6_dataun;
}
IP_PACKED2 Ipcom_icmp6_hdr;

typedef IP_PACKED1 union Ipcom_socktest_igmp_hdr_union
{
    struct Ipcom_socktest_v1_v2_msg_struct
    {
        Ip_u8    type;          /* 0x11, 0x12, 0x16, 0x17 */
        Ip_u8    code;
        Ip_u16   cksum;
        Ip_u32   maddr;
    } m;

    struct Ipcom_socktest_query_v3_struct
    {
        Ip_u8    type;          /* 0x11 */
        Ip_u8    max_resp_code;
        Ip_u16   cksum;
        Ip_u32   maddr;
        Ip_u8    s_qrv;
        Ip_u8    qqic;
        Ip_u16   number_of_sources;
        Ip_u32   saddr[1];
    } query_v3;

    struct Ipcom_socktest_report_v3_struct
    {
        Ip_u8    type;       /* 0x22 */
        Ip_u8    reserved1;
        Ip_u16   cksum;
        Ip_u16   reserverd2;
        Ip_u16   number_of_group_records;
        struct Ipcom_socktest_group_rec_struct
        {
            Ip_u8  record_type;
            Ip_u8  aux_data_len;
            Ip_u16 number_of_sources;
            Ip_u32 maddr;
            Ip_u32 saddr[1];
        } group_rec[1];
    } report_v3;

}
IP_PACKED2 Ipcom_socktest_igmp_hdr;


typedef IP_PACKED1 union Ipcom_socktest_mld_hdr_union
{
    struct Ipcom_socktest_v1_msg_struct
    {
        Ip_u8              type;          /* 130, 131, 132 */
        Ip_u8              code;
        Ip_u16             cksum;
        Ip_u16             max_resp_delay;
        Ip_u16             reserved;
        struct Ip_in6_addr maddr;
    } m;

    struct Ipcom_socktest_v2_query_struct
    {
        Ip_u8              type;          /* 130 */
        Ip_u8              code;
        Ip_u16             cksum;
        Ip_u16             max_resp_delay;
        Ip_u16             reserved;
        struct Ip_in6_addr maddr;
        Ip_u8              s_qrv;
        Ip_u8              qqic;
        Ip_u16             number_of_sources;
        struct Ip_in6_addr saddr[1];
    } query_v2;

    struct Ipcom_socktest_report_v2_struct
    {
        Ip_u8    type;                /* 143 */
        Ip_u8    reserved1;
        Ip_u16   cksum;
        Ip_u16   reserverd2;
        Ip_u16   number_of_maddr_records;
        struct Ipcom_socktest_maddr_rec_struct
        {
            Ip_u8              record_type;
            Ip_u8              aux_data_len;
            Ip_u16             number_of_sources;
            struct Ip_in6_addr maddr;
            struct Ip_in6_addr saddr[1];
        } mcast_rec[1];
    } report_v2;

}
IP_PACKED2 Ipcom_socktest_mld_hdr;
#include "ipcom_align16.h" /* OFF */


/*
 *===========================================================================
 *                   Global data
 *===========================================================================
 */
#define SOCKTEST_MAX_FD  16

typedef struct Ipcom_cmd_socktest_glob_struct
{
    int exit;
}
Ipcom_cmd_socktest_glob;

typedef struct Ipcom_cmd_socktest_cb_struct
{
    char   *subcmd;
    Ip_err (*cmd_cb)(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char *argv[]);
}
Ipcom_cmd_socktest_cb;

typedef struct Ipcom_cmd_socktest_flood_entry_struct
{
    Ipcom_list              next;
    Ipcom_cmd_socktest_fd   *f;
} Ipcom_cmd_socktest_flood_entry;


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC int ipcom_cmd_socktest(int argc, char **argv);

IP_STATIC void ipcom_cmd_socktest_print_usage(void);

IP_STATIC Ip_err ipcom_cmd_socktest_handle_console (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_exit    (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_open    (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_close   (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_bind    (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_setopt  (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_getopt  (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_ioctl   (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_connect (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_listen  (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_accept  (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_getpeername (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_getsockname (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_send    (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_receive (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_select  (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_shutdown(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_flush   (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_attach  (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_if_nametoindex (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_soevent_monitor(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_flood(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_gethostbyname(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_gethostbyaddr(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);

#ifdef IPSCTP
IP_STATIC Ip_err ipcom_cmd_socktest_handle_sctp_bindx(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_sctp_getpaddrs (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_sctp_freepaddrs(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_sctp_getladdrs (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_sctp_freeladdrs(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_sctp_connectx  (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_sctp_getaddrlen(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_sctp_peeloff(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
#endif



#ifdef IP_PORT_OSE5
IP_STATIC Ip_err ipcom_cmd_socktest_handle_dup(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
extern int efs_donate_fd(int fd, Ip_pid_t to);
extern int efs_receive_fd(Ip_pid_t from);
#if defined(IP_TARGET_SOFT) || defined(softOSE)
extern int zzdup(int fd);
extern int zzdup2(int fd, int newfd);
#else
extern int dup(int fd);
extern int dup2(int fd, int newfd);
#endif
#endif /* IP_PORT_OSE5 */

IP_STATIC Ip_err ipcom_cmd_socktest_flood_closesocket_impl(Ipcom_cmd_socktest_glob *glob,
                                                           Ipcom_cmd_socktest_fd   *f);


#ifdef IPCOM_CMD_SOCKTEST_USE_ZEROCOPY_API
IP_STATIC Ip_err ipcom_cmd_socktest_handle_zerocopy_send(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
IP_STATIC Ip_err ipcom_cmd_socktest_handle_zerocopy_recv(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv);
#endif /* IPCOM_CMD_SOCKTEST_USE_ZEROCOPY_API */

IP_STATIC Ip_u8 * ipcom_cmd_socktest_print_eth   (Ip_u8 * data);
#ifdef IPCOM_USE_INET
IP_STATIC Ip_u8 * ipcom_cmd_socktest_print_ip    (Ip_u8 * data);
IP_STATIC Ip_u8 * ipcom_cmd_socktest_print_icmp  (Ip_u8 * data, Ip_size_t len);
IP_STATIC Ip_u8 * ipcom_cmd_socktest_print_igmp  (Ip_u8 * data, Ip_size_t len);
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
IP_STATIC Ip_u8 * ipcom_cmd_socktest_print_ip6   (Ip_u8 * data);
IP_STATIC Ip_u8 * ipcom_cmd_socktest_print_icmp6 (Ip_u8 * data, Ip_size_t len);
#endif /* #ifdef IPCOM_USE_INET6 */
IP_STATIC Ip_u8 * ipcom_cmd_socktest_print_tcp   (Ip_u8 * data, Ip_size_t len);
IP_STATIC Ip_u8 * ipcom_cmd_socktest_print_udp   (Ip_u8 * data, Ip_size_t len);
#ifdef IPSCTP
IP_STATIC Ip_u8 * ipcom_cmd_socktest_print_sctp  (Ip_u8 * data, Ip_size_t len);
#endif


/* Data types that socktest knowns how to parse and format */

#define IPCOM_CST_TYPE_INT          0
IP_STATIC void * ipcom_cmd_socktest_parse_int     (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_int    (void * data);

#define IPCOM_CST_TYPE_U8           1
IP_STATIC void * ipcom_cmd_socktest_parse_u8      (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_u8     (void * data);

#define IPCOM_CST_TYPE_INADDR       2
IP_STATIC void * ipcom_cmd_socktest_parse_inaddr  (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_inaddr (void * data);

#define IPCOM_CST_TYPE_IP_MREQ      3
IP_STATIC void * ipcom_cmd_socktest_parse_mreq    (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_mreq   (void * data);

#define IPCOM_CST_TYPE_IP6_MREQ     4
IP_STATIC void * ipcom_cmd_socktest_parse_mreq6   (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_mreq6  (void * data);

#define IPCOM_CST_TYPE_ICMP6_FILTER 5
IP_STATIC void * ipcom_cmd_socktest_parse_icmp6_filter(char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_icmp6_filter(void * data);

#define IPCOM_CST_TYPE_IFREQ_INT    6
IP_STATIC void * ipcom_cmd_socktest_parse_ifreq_int       (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_ifreq_int      (void * data);

#define IPCOM_CST_TYPE_IFREQ_ADDR   7
IP_STATIC void * ipcom_cmd_socktest_parse_ifreq_addr      (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_ifreq_addr     (void * data);

#define IPCOM_CST_TYPE_IFREQ_FLAGS  8
IP_STATIC void * ipcom_cmd_socktest_parse_ifreq_flags     (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_ifreq_flags    (void * data);

#define IPCOM_CST_TYPE_ARPREQ       9
IP_STATIC void * ipcom_cmd_socktest_parse_arpreq          (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_arpreq         (void * data);

#define IPCOM_CST_TYPE_IFALIASREQ   10
IP_STATIC void * ipcom_cmd_socktest_parse_ifaliasreq_addr (char * data, char * obj);

#define IPCOM_CST_TYPE_IFREQ_U16    11
IP_STATIC void * ipcom_cmd_socktest_parse_ifreq_u16       (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_ifreq_u16      (void * data);

#define IPCOM_CST_TYPE_TIMEVAL      12
IP_STATIC void * ipcom_cmd_socktest_parse_timeval         (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_timeval        (void * data);

#define IPCOM_CST_TYPE_LINGER       13
IP_STATIC void * ipcom_cmd_socktest_parse_linger          (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_linger         (void * data);

#define IPCOM_CST_TYPE_MD5KEY       14
#define IPCOM_CST_MD5_KEY_MAXLEN    20
IP_STATIC void * ipcom_cmd_socktest_parse_md5key          (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_md5key         (void * data);

#define IPCOM_CST_TYPE_PACKET_MREQ  15
IP_STATIC void * ipcom_cmd_socktest_parse_packet_mreq     (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_packet_mreq    (void * data);

#define IPCOM_CST_TYPE_IFNAME       16
IP_STATIC void * ipcom_cmd_socktest_parse_ifname          (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_ifname         (void * data);

#define IPCOM_CST_TYPE_IN_PKTINFO   17
IP_STATIC char * ipcom_cmd_socktest_format_in_pktinfo(void * data);

#define IPCOM_CST_TYPE_IN6_PKTINFO  18
IP_STATIC char * ipcom_cmd_socktest_format_in6_pktinfo(void * data);

#define IPCOM_CST_TYPE_GROUP_REQ    19
IP_STATIC void * ipcom_cmd_socktest_parse_group_req       (char * data, char * obj);

#define IPCOM_CST_TYPE_GROUP_SOURCE_REQ 20
IP_STATIC void * ipcom_cmd_socktest_parse_group_source_req(char * data, char * obj);

#define IPCOM_CST_TYPE_IP_OPTS      21
IP_STATIC void * ipcom_cmd_socktest_parse_ip_opts(char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_ip_opts(void * data);

#define IPCOM_CST_TYPE_SOVLAN       22
IP_STATIC void * ipcom_cmd_socktest_parse_sovlan          (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_sovlan         (void * data);

#define IPCOM_CST_TYPE_STAT         23
IP_STATIC void * ipcom_cmd_socktest_parse_stat            (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_stat           (void * data);

#define IPCOM_CST_TYPE_GRE_KEY      24
IP_STATIC void * ipcom_cmd_socktest_parse_gre_key          (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_gre_key         (void * data);

#ifdef IPNET
#define IPCOM_CST_TYPE_DS           25
IP_STATIC void * ipcom_cmd_socktest_parse_ds              (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_ds             (void * data);

#define IPCOM_CST_TYPE_DS_MAP       26
IP_STATIC void * ipcom_cmd_socktest_parse_ds_map          (char * data, char * obj);

#ifndef IPNET_DIFFSERV_CLASSIFIER_MODE_BA
#define IPCOM_CST_TYPE_DS_FILTER    27
IP_STATIC void * ipcom_cmd_socktest_parse_ds_filter       (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_ds_filter      (void * data);
#endif
#endif /* IPNET */

#ifdef IPSCTP
/* sctp related */
#define IPCOM_CST_TYPE_RTINFO                28
IP_STATIC void * ipcom_cmd_socktest_parse_rtinfo                   (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_rtinfo                  (void * data);

#define IPCOM_CST_TYPE_ASSOCINFO             29
IP_STATIC void * ipcom_cmd_socktest_parse_associnfo                (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_associnfo               (void * data);

#define IPCOM_CST_TYPE_DEFAULT_SEND_PARAM    30
IP_STATIC void * ipcom_cmd_socktest_parse_default_send_param       (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_default_send_param      (void * data);

#define IPCOM_CST_TYPE_GET_PEER_ADDR_INFO    31
IP_STATIC void * ipcom_cmd_socktest_parse_paddrinfo                (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_paddrinfo               (void * data);

#define IPCOM_CST_TYPE_PRIMARY_ADDR          32
IP_STATIC void * ipcom_cmd_socktest_parse_primary_addr             (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_primary_addr            (void * data);

#define IPCOM_CST_TYPE_PEER_ADDR_PARAMS      33
IP_STATIC void * ipcom_cmd_socktest_parse_peer_addr_params         (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_peer_addr_params        (void * data);

#define IPCOM_CST_TYPE_STATUS                34
IP_STATIC void * ipcom_cmd_socktest_parse_status                   (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_status                  (void * data);

#define IPCOM_CST_TYPE_CONTEXT               35
IP_STATIC void * ipcom_cmd_socktest_parse_assval                   (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_assval                  (void * data);

#define IPCOM_CST_TYPE_EVENT                 36
IP_STATIC void * ipcom_cmd_socktest_parse_event                    (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_event                   (void * data);

#define IPCOM_CST_TYPE_INITMSG               37
IP_STATIC void * ipcom_cmd_socktest_parse_initmsg                  (char * data, char * obj);
IP_STATIC char * ipcom_cmd_socktest_format_initmsg                 (void * data);

/* end of sctp related */
#endif

/* The index of the data type handler MUST be the same as the type id,
   i.e socktest_data_types[IPCOM_CST_TYPE_INT] must return the entry for handling integer */
IP_STATIC Ipcom_cmd_socktest_glob_type socktest_data_types[] = {
    { /* 0 */ "int",                    sizeof(int), ipcom_cmd_socktest_parse_int, ipcom_cmd_socktest_format_int },
    { /* 1 */ "Ip_u8",                  sizeof(Ip_u8), ipcom_cmd_socktest_parse_u8, ipcom_cmd_socktest_format_u8 },
    { /* 2 */ "struct Ip_in_addr",      sizeof(struct Ip_in_addr), ipcom_cmd_socktest_parse_inaddr, ipcom_cmd_socktest_format_inaddr },
    { /* 3 */ "struct Ip_ip_mreq",      sizeof(struct Ip_ip_mreq), ipcom_cmd_socktest_parse_mreq, ipcom_cmd_socktest_format_mreq },
    { /* 4 */ "struct Ip_ip6_mreq",     sizeof(struct Ip_ipv6_mreq), ipcom_cmd_socktest_parse_mreq6, ipcom_cmd_socktest_format_mreq6 },
    { /* 5 */ "struct Ip_icmp6_filter", sizeof(struct Ip_icmp6_filter), ipcom_cmd_socktest_parse_icmp6_filter, ipcom_cmd_socktest_format_icmp6_filter },
    { /* 6 */ "struct Ip_ifreq (int)",  sizeof(struct Ip_ifreq), ipcom_cmd_socktest_parse_ifreq_int, ipcom_cmd_socktest_format_ifreq_int },
    { /* 7 */ "struct Ip_ifreq.addr",   sizeof(struct Ip_ifreq), ipcom_cmd_socktest_parse_ifreq_addr, ipcom_cmd_socktest_format_ifreq_addr },
    { /* 8 */ "struct Ip_ifreq.flags",  sizeof(struct Ip_ifreq), ipcom_cmd_socktest_parse_ifreq_flags, ipcom_cmd_socktest_format_ifreq_flags },
    { /* 9 */ "struct Ip_arpreq",       sizeof(struct Ip_arpreq), ipcom_cmd_socktest_parse_arpreq, ipcom_cmd_socktest_format_arpreq },
    { /* 10 */ "struct Ip_ifaliasreq",  sizeof(struct Ip_ifaliasreq), ipcom_cmd_socktest_parse_ifaliasreq_addr, IP_NULL },
    { /* 11 */ "struct Ip_ifreq (u16)", sizeof(struct Ip_ifreq), ipcom_cmd_socktest_parse_ifreq_u16, ipcom_cmd_socktest_format_ifreq_u16 },
    { /* 12 */ "struct Ip_timeval",     sizeof(struct Ip_timeval), ipcom_cmd_socktest_parse_timeval, ipcom_cmd_socktest_format_timeval },
    { /* 13 */ "struct Ip_linger",      sizeof(struct Ip_linger), ipcom_cmd_socktest_parse_linger, ipcom_cmd_socktest_format_linger },
    { /* 14 */ "char[0..20]",           IPCOM_CST_MD5_KEY_MAXLEN, ipcom_cmd_socktest_parse_md5key, ipcom_cmd_socktest_format_md5key },
    { /* 15 */ "struct Ip_packet_mreq", sizeof(struct Ip_packet_mreq), ipcom_cmd_socktest_parse_packet_mreq, ipcom_cmd_socktest_format_packet_mreq },
    { /* 16 */ "char[0..20]",           IP_IFNAMSIZ, ipcom_cmd_socktest_parse_ifname, ipcom_cmd_socktest_format_ifname },
    { /* 17 */ "struct Ip_in_pktinfo",  sizeof(struct Ip_in_pktinfo), IP_NULL, ipcom_cmd_socktest_format_in_pktinfo },
    { /* 18 */ "struct Ip_in6_pktinfo", sizeof(struct Ip_in6_pktinfo), IP_NULL, ipcom_cmd_socktest_format_in6_pktinfo },
    { /* 19 */ "struct Ip_group_req",   sizeof(struct Ip_group_req), ipcom_cmd_socktest_parse_group_req, IP_NULL },
    { /* 20 */ "struct Ip_group_source_req", sizeof(struct Ip_group_source_req), ipcom_cmd_socktest_parse_group_source_req, IP_NULL },
    { /* 21 */ "char[0..40]",           40, ipcom_cmd_socktest_parse_ip_opts, ipcom_cmd_socktest_format_ip_opts },
    { /* 22 */ "struct Ip_sovlan",      sizeof(struct Ip_sovlan), ipcom_cmd_socktest_parse_sovlan, ipcom_cmd_socktest_format_sovlan },
    { /* 23 */ "struct Ip_stat",        sizeof(struct Ip_stat), ipcom_cmd_socktest_parse_stat, ipcom_cmd_socktest_format_stat },
    { /* 24 */ "struct Ip_gre_key",     sizeof(struct Ip_gre_key), ipcom_cmd_socktest_parse_gre_key, ipcom_cmd_socktest_format_gre_key },
#ifdef IPNET
    { /* 25 */ "struct Ipnet_ds",       sizeof(struct Ipnet_ds), ipcom_cmd_socktest_parse_ds, ipcom_cmd_socktest_format_ds },
    { /* 26 */ "struct Ipnet_ds_map",   sizeof(struct Ipnet_ds_map), ipcom_cmd_socktest_parse_ds_map, IP_NULL },
#ifndef IPNET_DIFFSERV_CLASSIFIER_MODE_BA
    { /* 27 */ "",                      0, IP_NULL, IP_NULL},
#else
    { /* 27 */ "",                      0, IP_NULL, IP_NULL},
#endif

#else
    { /* 25 */ "",                      0, IP_NULL, IP_NULL},
    { /* 26 */ "",                      0, IP_NULL, IP_NULL},
    { /* 27 */ "",                      0, IP_NULL, IP_NULL},
#endif /* IPNET */
#ifdef IPSCTP

    { /* 28 */ "struct Ipsctp_rtoinfo",    sizeof(struct Ipsctp_rtoinfo),ipcom_cmd_socktest_parse_rtinfo,ipcom_cmd_socktest_format_rtinfo},
    { /* 29 */ "struct Ipsctp_assocparams",sizeof(struct Ipsctp_assocparams),ipcom_cmd_socktest_parse_associnfo,ipcom_cmd_socktest_format_associnfo},
    { /* 30 */ "struct Ipsctp_sndrcvinfo", sizeof(struct Ipsctp_sndrcvinfo),ipcom_cmd_socktest_parse_default_send_param,ipcom_cmd_socktest_format_default_send_param},
    { /* 31 */ "struct Ipsctp_paddrinfo",  sizeof(struct Ipsctp_paddrinfo),ipcom_cmd_socktest_parse_paddrinfo,ipcom_cmd_socktest_format_paddrinfo},
    { /* 32 */ "struct Ipsctp_setprim",    sizeof(struct Ipsctp_setprim),ipcom_cmd_socktest_parse_primary_addr,ipcom_cmd_socktest_format_primary_addr},
    { /* 33 */ "struct Ipsctp_paddrparams",sizeof(struct Ipsctp_paddrparams),ipcom_cmd_socktest_parse_peer_addr_params,ipcom_cmd_socktest_format_peer_addr_params},
    { /* 34 */ "struct Ipsctp_status",     sizeof(struct Ipsctp_status),ipcom_cmd_socktest_parse_status,ipcom_cmd_socktest_format_status},
    { /* 35 */ "struct Ipsctp_assoc_value",sizeof(struct Ipsctp_assoc_value),ipcom_cmd_socktest_parse_assval,ipcom_cmd_socktest_format_assval},
    { /* 36 */ "struct Ipsctp_event_subscribe",sizeof(struct Ipsctp_event_subscribe),ipcom_cmd_socktest_parse_event,ipcom_cmd_socktest_format_event},
    { /* 37 */ "struct Ipsctp_initmsg",    sizeof(struct Ipsctp_initmsg),ipcom_cmd_socktest_parse_initmsg,ipcom_cmd_socktest_format_initmsg},
#endif
};


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC Ipcom_cmd_socktest_fd socktest_fd[SOCKTEST_MAX_FD] =
{
    {-1,-1,-1,-1},
    {-1,-1,-1,-1},
    {-1,-1,-1,-1},
    {-1,-1,-1,-1},

    {-1,-1,-1,-1},
    {-1,-1,-1,-1},
    {-1,-1,-1,-1},
    {-1,-1,-1,-1},

    {-1,-1,-1,-1},
    {-1,-1,-1,-1},
    {-1,-1,-1,-1},
    {-1,-1,-1,-1},

    {-1,-1,-1,-1},
    {-1,-1,-1,-1},
    {-1,-1,-1,-1},
    {-1,-1,-1,-1}
};

IP_STATIC Ipcom_cmd_socktest_cb socktest_cmd_cb[] =
{
    {"console",ipcom_cmd_socktest_handle_console },
    {"exit",   ipcom_cmd_socktest_handle_exit    },
    {"open",   ipcom_cmd_socktest_handle_open    },
    {"socket", ipcom_cmd_socktest_handle_open    },
    {"close",  ipcom_cmd_socktest_handle_close   },
    {"bind",   ipcom_cmd_socktest_handle_bind    },
    {"setopt", ipcom_cmd_socktest_handle_setopt  },
    {"getopt", ipcom_cmd_socktest_handle_getopt  },
    {"ioctl",  ipcom_cmd_socktest_handle_ioctl   },
    {"connect",ipcom_cmd_socktest_handle_connect },
    {"listen", ipcom_cmd_socktest_handle_listen  },
    {"accept", ipcom_cmd_socktest_handle_accept  },
    {"getpeer",ipcom_cmd_socktest_handle_getpeername },
    {"getpeername",ipcom_cmd_socktest_handle_getpeername },
    {"getsockname",ipcom_cmd_socktest_handle_getsockname },
    {"receive",ipcom_cmd_socktest_handle_receive },
    {"send",   ipcom_cmd_socktest_handle_send    },
    {"select", ipcom_cmd_socktest_handle_select  },
    {"shutdown", ipcom_cmd_socktest_handle_shutdown },
    {"flush", ipcom_cmd_socktest_handle_flush },
#ifdef IPCOM_CMD_SOCKTEST_USE_ZEROCOPY_API
    {"zerocopy_send", ipcom_cmd_socktest_handle_zerocopy_send },
    {"zerocopy_recv", ipcom_cmd_socktest_handle_zerocopy_recv },
#endif
#ifdef IP_PORT_OSE5
    {"dup",    ipcom_cmd_socktest_handle_dup     },
#endif
    {"attach", ipcom_cmd_socktest_handle_attach  },
    {"if_nametoindex", ipcom_cmd_socktest_handle_if_nametoindex  },
    {"soevent_monitor", ipcom_cmd_socktest_handle_soevent_monitor },
    {"flood",   ipcom_cmd_socktest_handle_flood },
    {"gethostbyname", ipcom_cmd_socktest_handle_gethostbyname },
    {"gethostbyaddr", ipcom_cmd_socktest_handle_gethostbyaddr },
#ifdef IPSCTP
    {"sctp_bindx", ipcom_cmd_socktest_handle_sctp_bindx },
    {"sctp_getpaddrs", ipcom_cmd_socktest_handle_sctp_getpaddrs },
    {"sctp_freepaddrs", ipcom_cmd_socktest_handle_sctp_freepaddrs },
    {"sctp_getladdrs", ipcom_cmd_socktest_handle_sctp_getladdrs },
    {"sctp_freeladdrs", ipcom_cmd_socktest_handle_sctp_freeladdrs },
    {"sctp_connectx", ipcom_cmd_socktest_handle_sctp_connectx },
    {"sctp_getaddrlen",ipcom_cmd_socktest_handle_sctp_getaddrlen },
    {"sctp_peeloff",ipcom_cmd_socktest_handle_sctp_peeloff },
#endif
};

IP_STATIC Ipcom_cmd_socktest_param socktest_domain[] =
{
    {IP_AF_UNSPEC,"unspec"},
    {IP_AF_INET,  "inet"  },
    {IP_AF_ROUTE, "route" },
    {IP_AF_LINK , "link"  },
    {IP_AF_PACKET,"packet"},
    {IP_AF_INET6, "inet6" },
    {IP_AF_UNSPEC,"key"   }
};

IP_STATIC Ipcom_cmd_socktest_param socktest_type[] =
{
    {IP_SOCK_STREAM,    "stream"   },
    {IP_SOCK_DGRAM,     "datagram" },
    {IP_SOCK_DGRAM,     "dgram"    },
    {IP_SOCK_RAW,       "raw"      },
#ifdef IPSCTP
    {IP_SOCK_SEQPACKET, "seqpacket"},
#endif
    {IP_SOCK_PACKET,    "packet"   }
};

IP_STATIC Ipcom_cmd_socktest_param socktest_proto[] =
{
    {IP_IPPROTO_ICMP,       "icmp"      },
    {IP_IPPROTO_IGMP,       "igmp"      },
    {IP_IPPROTO_IPV4,       "ipv4"      },
    {IP_IPPROTO_TCP,        "tcp"       },
    {IP_IPPROTO_UDP,        "udp"       },
    {IP_IPPROTO_IPV6,       "ipv6"      },
    {IP_IPPROTO_ESP,        "esp"       },
    {IP_IPPROTO_AH,         "ah"        },
    {IP_IPPROTO_ICMPV6,     "icmpv6"    },
    {IP_IPPROTO_RAW,        "raw"       },
#ifdef IPSCTP
    {IP_IPPROTO_SCTP,       "sctp"      },
#endif
    {IPCOM_ETH_P_ALL,       "p_all"     },
    {IPCOM_ETH_P_IP,        "p_ip"      },
    {IPCOM_ETH_P_ARP,       "p_arp"     },
    {IPCOM_ETH_P_IPV6,      "p_ipv6"    },
    {IPCOM_ETH_P_PPPOE_DIS, "p_ppoe_dis"},
    {IPCOM_ETH_P_PPPOE_SES, "p_ppoe_ses"}
};


IP_STATIC Ipcom_cmd_socktest_ancillary_data socktest_ancillary_data[] =
{
    {IP_IPPROTO_IP,   IP_IP_TTL,        "ttl",      &socktest_data_types[IPCOM_CST_TYPE_INT]        },
    {IP_IPPROTO_IP,   IP_IP_TOS,        "tos",      &socktest_data_types[IPCOM_CST_TYPE_INT]        },
    {IP_IPPROTO_IP,   IP_IP_RECVDSTADDR,"recvdstaddr",&socktest_data_types[IPCOM_CST_TYPE_INADDR]   },
    {IP_IPPROTO_IP,   IP_IP_PKTINFO,    "pktinfo",  &socktest_data_types[IPCOM_CST_TYPE_IN_PKTINFO] },
    {IP_IPPROTO_IPV6, IP_IPV6_HOPLIMIT, "hoplimit", &socktest_data_types[IPCOM_CST_TYPE_INT]        },
    {IP_IPPROTO_IPV6, IP_IPV6_TCLASS,   "tclass",   &socktest_data_types[IPCOM_CST_TYPE_INT]        },
    {IP_IPPROTO_IPV6, IP_IPV6_PKTINFO,  "pktinfo",  &socktest_data_types[IPCOM_CST_TYPE_IN6_PKTINFO]},
    /* Need this for LAS */
    {IP_IPPROTO_IPV6, IP_IPV6_RECVPKTINFO,  "pktinfo",  &socktest_data_types[IPCOM_CST_TYPE_IN6_PKTINFO]},
#ifdef IPSCTP
    {IP_IPPROTO_SCTP, IPSCTP_SNDRCV,    "sndrcvinfo",  &socktest_data_types[IPCOM_CST_TYPE_DEFAULT_SEND_PARAM]},
    {IP_IPPROTO_SCTP, IPSCTP_INITMSG,   "initmsg",     &socktest_data_types[IPCOM_CST_TYPE_INITMSG]}
#endif
};

/* Socket option format and parse */

IP_STATIC Ipcom_cmd_socktest_sockopt socktest_sol_opt[] =
{
    {IP_SO_ACCEPTCONN,   "acceptconn", &socktest_data_types[IPCOM_CST_TYPE_INT] },
    {IP_SO_BINDTODEVICE, "bindtodevice", &socktest_data_types[IPCOM_CST_TYPE_IFNAME] },
    {IP_SO_BROADCAST,    "broadcast" , &socktest_data_types[IPCOM_CST_TYPE_INT] },
    {IP_SO_DONTROUTE,    "dountroute", &socktest_data_types[IPCOM_CST_TYPE_INT] },
    {IP_SO_ERROR,        "error"     , &socktest_data_types[IPCOM_CST_TYPE_INT] },
    {IP_SO_KEEPALIVE,    "keepalive" , &socktest_data_types[IPCOM_CST_TYPE_INT] },
    {IP_SO_LINGER,       "linger" ,    &socktest_data_types[IPCOM_CST_TYPE_LINGER] },
    {IP_SO_OOBINLINE,    "oobinline" , &socktest_data_types[IPCOM_CST_TYPE_INT] },
    {IP_SO_RCVBUF,       "rcvbuf"    , &socktest_data_types[IPCOM_CST_TYPE_INT] },
    {IP_SO_RCVLOWAT,     "rcvlowat",   &socktest_data_types[IPCOM_CST_TYPE_INT] },
    {IP_SO_RCVTIMEO,     "rcvtimeo"  , &socktest_data_types[IPCOM_CST_TYPE_TIMEVAL] },
    {IP_SO_REUSEADDR,    "reuseaddr" , &socktest_data_types[IPCOM_CST_TYPE_INT] },
    {IP_SO_REUSEPORT,    "reuseport" , &socktest_data_types[IPCOM_CST_TYPE_INT] },
    {IP_SO_SNDBUF,       "sndbuf"    , &socktest_data_types[IPCOM_CST_TYPE_INT] },
    {IP_SO_SNDLOWAT,     "sndlowat",   &socktest_data_types[IPCOM_CST_TYPE_INT] },
    {IP_SO_TYPE,         "type"      , &socktest_data_types[IPCOM_CST_TYPE_INT] },
    {IP_SO_VLAN,         "vlan",       &socktest_data_types[IPCOM_CST_TYPE_SOVLAN] },
    {IP_SO_X_DRAINABLE,  "drainable",  &socktest_data_types[IPCOM_CST_TYPE_INT] },
    {IP_SO_X_GRE_KEY,    "grekey" ,    &socktest_data_types[IPCOM_CST_TYPE_GRE_KEY] },
    {IP_SO_X_PROBEDELAY, "probedelay", &socktest_data_types[IPCOM_CST_TYPE_INT] },
    {IP_SO_X_VR,         "vr"        , &socktest_data_types[IPCOM_CST_TYPE_INT] },
};

IP_STATIC Ipcom_cmd_socktest_sockopt socktest_ip_opt[] =
{
    {IP_IP_OPTIONS,               "options",                  &socktest_data_types[IPCOM_CST_TYPE_IP_OPTS]         },
    {IP_IP_ROUTER_ALERT,          "router_alert",             &socktest_data_types[IPCOM_CST_TYPE_INT]             },
    {IP_IP_HDRINCL,               "hdrincl",                  &socktest_data_types[IPCOM_CST_TYPE_INT]             },
    {IP_IP_TOS,                   "tos",                      &socktest_data_types[IPCOM_CST_TYPE_INT]             },
    {IP_IP_RECVTOS,               "recvtos",                  &socktest_data_types[IPCOM_CST_TYPE_INT]             },
    {IP_IP_TTL,                   "ttl",                      &socktest_data_types[IPCOM_CST_TYPE_INT]             },
    {IP_IP_RECVTTL,               "recvttl",                  &socktest_data_types[IPCOM_CST_TYPE_INT]             },
    {IP_IP_RECVDSTADDR,           "recvdstaddr",              &socktest_data_types[IPCOM_CST_TYPE_INT]             },
    {IP_IP_PKTINFO,               "pktinfo",                  &socktest_data_types[IPCOM_CST_TYPE_INT]             },
    {IP_IP_MULTICAST_IF,          "multicast_if",             &socktest_data_types[IPCOM_CST_TYPE_INADDR]          },
    {IP_IP_MULTICAST_TTL,         "multicast_ttl",            &socktest_data_types[IPCOM_CST_TYPE_U8]              },
    {IP_IP_MULTICAST_LOOP,        "multicast_loop",           &socktest_data_types[IPCOM_CST_TYPE_U8]              },
    {IP_IP_DONTFRAG,              "dontfrag",                 &socktest_data_types[IPCOM_CST_TYPE_U8]              },
    {IP_IP_ADD_MEMBERSHIP,        "add_membership",           &socktest_data_types[IPCOM_CST_TYPE_IP_MREQ]         },
    {IP_IP_JOIN_GROUP,            "join_group" ,              &socktest_data_types[IPCOM_CST_TYPE_IP_MREQ]         },
    {IP_IP_DROP_MEMBERSHIP,       "drop_membership",          &socktest_data_types[IPCOM_CST_TYPE_IP_MREQ]         },
    {IP_IP_LEAVE_GROUP,           "leave_group",              &socktest_data_types[IPCOM_CST_TYPE_IP_MREQ]         },
    {IP_IP_RECVIF,                "recvif",                   &socktest_data_types[IPCOM_CST_TYPE_INT]             },
    {IP_MCAST_JOIN_GROUP,         "mcast_join_group",         &socktest_data_types[IPCOM_CST_TYPE_GROUP_REQ]       },
    {IP_MCAST_BLOCK_SOURCE,       "mcast_block_source" ,      &socktest_data_types[IPCOM_CST_TYPE_GROUP_SOURCE_REQ]},
    {IP_MCAST_UNBLOCK_SOURCE,     "mcast_unblock_source" ,    &socktest_data_types[IPCOM_CST_TYPE_GROUP_SOURCE_REQ]},
    {IP_MCAST_LEAVE_GROUP,        "mcast_leave_group" ,       &socktest_data_types[IPCOM_CST_TYPE_GROUP_REQ]       },
    {IP_MCAST_JOIN_SOURCE_GROUP,  "mcast_join_source_group" , &socktest_data_types[IPCOM_CST_TYPE_GROUP_SOURCE_REQ]},
    {IP_MCAST_LEAVE_SOURCE_GROUP, "mcast_leave_source_group", &socktest_data_types[IPCOM_CST_TYPE_GROUP_SOURCE_REQ]},
};

IP_STATIC Ipcom_cmd_socktest_sockopt socktest_ipv6_opt[] =
{
    {IP_IPV6_UNICAST_HOPS,        "unicast_hops",             &socktest_data_types[IPCOM_CST_TYPE_INT]             },
    {IP_IPV6_MULTICAST_IF,        "multicast_if",             &socktest_data_types[IPCOM_CST_TYPE_INT]             },
    {IP_IPV6_MULTICAST_HOPS,      "multicast_hops",           &socktest_data_types[IPCOM_CST_TYPE_INT]             },
    {IP_IPV6_MULTICAST_LOOP,      "multicast_loop",           &socktest_data_types[IPCOM_CST_TYPE_INT]             },
    {IP_IPV6_JOIN_GROUP,          "join_group",               &socktest_data_types[IPCOM_CST_TYPE_IP6_MREQ]        },
    {IP_IPV6_LEAVE_GROUP,         "leave_group",              &socktest_data_types[IPCOM_CST_TYPE_IP6_MREQ]        },
    {IP_IPV6_ADD_MEMBERSHIP,      "add_membership",           &socktest_data_types[IPCOM_CST_TYPE_IP6_MREQ]        },
    {IP_IPV6_DROP_MEMBERSHIP,     "drop_membership",          &socktest_data_types[IPCOM_CST_TYPE_IP6_MREQ]        },
    {IP_IPV6_RECVIF,              "recvif",                   &socktest_data_types[IPCOM_CST_TYPE_INT]             },
    {IP_IPV6_CHECKSUM,            "checksum",                 &socktest_data_types[IPCOM_CST_TYPE_INT]             },
    {IP_IPV6_RECVPKTINFO,         "recvpktinfo",              &socktest_data_types[IPCOM_CST_TYPE_INT]             },
    {IP_IPV6_RECVHOPLIMIT,        "recvhoplimit",             &socktest_data_types[IPCOM_CST_TYPE_INT]             },
    {IP_IPV6_RECVTCLASS,          "recvtclass",               &socktest_data_types[IPCOM_CST_TYPE_INT]             },
    {IP_IPV6_TCLASS,              "tclass",                   &socktest_data_types[IPCOM_CST_TYPE_INT]             },
    {IP_MCAST_JOIN_GROUP,         "mcast_join_group",         &socktest_data_types[IPCOM_CST_TYPE_GROUP_REQ]       },
    {IP_MCAST_BLOCK_SOURCE,       "mcast_block_source" ,      &socktest_data_types[IPCOM_CST_TYPE_GROUP_SOURCE_REQ]},
    {IP_MCAST_UNBLOCK_SOURCE,     "mcast_unblock_source" ,    &socktest_data_types[IPCOM_CST_TYPE_GROUP_SOURCE_REQ]},
    {IP_MCAST_LEAVE_GROUP,        "mcast_leave_group" ,       &socktest_data_types[IPCOM_CST_TYPE_GROUP_REQ]       },
    {IP_MCAST_JOIN_SOURCE_GROUP,  "mcast_join_source_group" , &socktest_data_types[IPCOM_CST_TYPE_GROUP_SOURCE_REQ]},
    {IP_MCAST_LEAVE_SOURCE_GROUP, "mcast_leave_source_group", &socktest_data_types[IPCOM_CST_TYPE_GROUP_SOURCE_REQ]},
};

IP_STATIC Ipcom_cmd_socktest_sockopt socktest_icmpv6_opt[] =
{
    {IP_ICMP6_FILTER,       "icmp6_filter", &socktest_data_types[IPCOM_CST_TYPE_ICMP6_FILTER] },
};


IP_STATIC Ipcom_cmd_socktest_sockopt socktest_udp_opt[] =
{
    {IP_UDP_NAT_T,               "udpnat",  &socktest_data_types[IPCOM_CST_TYPE_INT]    },
};

IP_STATIC Ipcom_cmd_socktest_sockopt socktest_tcp_opt[] =
{
    {IP_TCP_NODELAY,             "nodelay",  &socktest_data_types[IPCOM_CST_TYPE_INT]    },
    {IP_TCP_MAXSEG,              "maxseg",   &socktest_data_types[IPCOM_CST_TYPE_INT]    },
    {IP_TCP_CORK,                "cork",     &socktest_data_types[IPCOM_CST_TYPE_INT]    },
    {IP_TCP_KEEPIDLE,            "keepidle", &socktest_data_types[IPCOM_CST_TYPE_INT]    },
    {IP_TCP_KEEPINTVL,           "keepintvl",&socktest_data_types[IPCOM_CST_TYPE_INT]    },
    {IP_TCP_KEEPCNT,             "keepcnt",  &socktest_data_types[IPCOM_CST_TYPE_INT]    },
    {IP_TCP_X_SEQ,               "seq",      &socktest_data_types[IPCOM_CST_TYPE_INT]    },
    {IP_TCP_PERSIST_TIMEOUT,     "persist_timeout", &socktest_data_types[IPCOM_CST_TYPE_INT] },
    {IP_TCP_X_MD5_SIGNATURE_KEY, "md5key",   &socktest_data_types[IPCOM_CST_TYPE_MD5KEY] }
};

#ifdef IPSCTP
IP_STATIC Ipcom_cmd_socktest_sockopt socktest_sctp_opt[] =
{
    {IPSCTP_RTOINFO,                  "rtoinfo",                 &socktest_data_types[IPCOM_CST_TYPE_RTINFO]},
    {IPSCTP_ASSOCINFO,                "associnfo",               &socktest_data_types[IPCOM_CST_TYPE_ASSOCINFO]},
    {IPSCTP_DEFAULT_SEND_PARAM,       "default_send_param",      &socktest_data_types[IPCOM_CST_TYPE_DEFAULT_SEND_PARAM]},
    {IPSCTP_GET_PEER_ADDR_INFO,       "get_peer_addr_info",      &socktest_data_types[IPCOM_CST_TYPE_GET_PEER_ADDR_INFO]},
    {IPSCTP_PRIMARY_ADDR,             "primary_addr",            &socktest_data_types[IPCOM_CST_TYPE_PRIMARY_ADDR]},
    {IPSCTP_PEER_ADDR_PARAMS,         "peer_addr_params",        &socktest_data_types[IPCOM_CST_TYPE_PEER_ADDR_PARAMS]},
    {IPSCTP_STATUS,                   "status",                  &socktest_data_types[IPCOM_CST_TYPE_STATUS]},
    {IPSCTP_CONTEXT,                  "context",                 &socktest_data_types[IPCOM_CST_TYPE_CONTEXT]},
    {IPSCTP_EVENTS,                   "event",                   &socktest_data_types[IPCOM_CST_TYPE_EVENT]},
    {IPSCTP_INITMSG,                  "initmsg",                 &socktest_data_types[IPCOM_CST_TYPE_INITMSG]},
    {IPSCTP_DISABLE_FRAGMENTS,        "disable_fragments",       &socktest_data_types[IPCOM_CST_TYPE_CONTEXT]},
    {IPSCTP_MAXSEG,                   "maxseg",                  &socktest_data_types[IPCOM_CST_TYPE_CONTEXT]},
    {IPSCTP_NODELAY,                  "nodelay",                 &socktest_data_types[IPCOM_CST_TYPE_INT]},
    {IPSCTP_SET_PEER_PRIMARY_ADDR,    "peer_primary_addr",       &socktest_data_types[IPCOM_CST_TYPE_PRIMARY_ADDR]}
};
#endif



IP_STATIC Ipcom_cmd_socktest_sockopt socktest_packet_opt[] =
{
    {IP_PACKET_ADD_MEMBERSHIP,   "add_membership",  &socktest_data_types[IPCOM_CST_TYPE_PACKET_MREQ]    },
    {IP_PACKET_DROP_MEMBERSHIP,  "drop_membership", &socktest_data_types[IPCOM_CST_TYPE_PACKET_MREQ] }
};


IP_STATIC Ipcom_cmd_socktest_sock_level socktest_sock_level[] =
{
    {IP_SOL_SOCKET,     "socket", socktest_sol_opt,    SOCKTEST_SOCKOPT_ENTRIES(socktest_sol_opt)    },
    {IP_IPPROTO_IP,     "ip",     socktest_ip_opt,     SOCKTEST_SOCKOPT_ENTRIES(socktest_ip_opt)     },
    {IP_IPPROTO_UDP,    "udp",    socktest_udp_opt,    SOCKTEST_SOCKOPT_ENTRIES(socktest_udp_opt)    },
    {IP_IPPROTO_TCP,    "tcp",    socktest_tcp_opt,    SOCKTEST_SOCKOPT_ENTRIES(socktest_tcp_opt)    },
#ifdef IPSCTP
    {IP_IPPROTO_SCTP,   "sctp",   socktest_sctp_opt,   SOCKTEST_SOCKOPT_ENTRIES(socktest_sctp_opt)   },
#endif
    {IP_IPPROTO_IPV6,   "ipv6",   socktest_ipv6_opt,   SOCKTEST_SOCKOPT_ENTRIES(socktest_ipv6_opt)   },
    {IP_IPPROTO_ICMPV6, "icmpv6", socktest_icmpv6_opt, SOCKTEST_SOCKOPT_ENTRIES(socktest_icmpv6_opt) },
    {IP_SOL_PACKET,     "packet", socktest_packet_opt, SOCKTEST_SOCKOPT_ENTRIES(socktest_packet_opt) }
};


IP_STATIC Ipcom_cmd_socktest_ioctl socktest_ioctl[] =
{
    /*   ID                Name                          Type                                 Is get? */
    {IP_SIOCSPGRP,      "siocspgrp",      &socktest_data_types[IPCOM_CST_TYPE_INT],          IP_FALSE },
    {IP_SIOCGPGRP,      "siocgpgrp",      &socktest_data_types[IPCOM_CST_TYPE_INT],          IP_TRUE  },
    {IP_FIONBIO,        "fionbio",        &socktest_data_types[IPCOM_CST_TYPE_INT],          IP_FALSE },
    {IP_SIOCATMARK,     "siocatmark",     &socktest_data_types[IPCOM_CST_TYPE_INT],          IP_TRUE  },
    {IP_SIOCINQ,        "siocinq",        &socktest_data_types[IPCOM_CST_TYPE_INT],          IP_TRUE  },
    {IP_SIOCOUTQ,       "siocoutq",       &socktest_data_types[IPCOM_CST_TYPE_INT],          IP_TRUE  },
    {IP_SIOCGIFADDR,    "siocgifaddr",    &socktest_data_types[IPCOM_CST_TYPE_IFREQ_ADDR],   IP_TRUE  },
    {IP_SIOCSIFADDR,    "siocsifaddr",    &socktest_data_types[IPCOM_CST_TYPE_IFREQ_ADDR],   IP_FALSE },
    {IP_SIOCGIFBRDADDR, "siocgifbrdaddr", &socktest_data_types[IPCOM_CST_TYPE_IFREQ_ADDR],   IP_TRUE  },
    {IP_SIOCGIFNETMASK, "siocgifnetmask", &socktest_data_types[IPCOM_CST_TYPE_IFREQ_ADDR],   IP_TRUE  },
    {IP_SIOCSIFNETMASK, "siocsifnetmask", &socktest_data_types[IPCOM_CST_TYPE_IFREQ_ADDR],   IP_FALSE },
    {IP_SIOCGIFMTU,     "siocgifmtu",     &socktest_data_types[IPCOM_CST_TYPE_IFREQ_INT],    IP_TRUE  },
    {IP_SIOCSIFMTU,     "siocsifmtu",     &socktest_data_types[IPCOM_CST_TYPE_IFREQ_INT],    IP_FALSE },
    {IP_SIOCGIFFLAGS,   "siocgifflags",   &socktest_data_types[IPCOM_CST_TYPE_IFREQ_FLAGS],  IP_TRUE  },
    {IP_SIOCSIFFLAGS,   "siocsifflags",   &socktest_data_types[IPCOM_CST_TYPE_IFREQ_FLAGS],  IP_FALSE },
#ifdef IP_X_SIOCSINTR
    {IP_X_SIOCSINTR,    "siocsintr",      &socktest_data_types[IPCOM_CST_TYPE_INT],          IP_FALSE },
#endif
#ifdef IP_SIOCGIFMETRIC
    {IP_SIOCGIFMETRIC,  "siocgifmetric",  &socktest_data_types[IPCOM_CST_TYPE_IFREQ_INT],    IP_TRUE  },
#endif
#ifdef IP_SIOCSIFMETRIC
    {IP_SIOCSIFMETRIC,  "siocsifmetric",  &socktest_data_types[IPCOM_CST_TYPE_IFREQ_INT],    IP_FALSE },
#endif
#ifdef IP_SIOCSIFVR
    {IP_SIOCSIFVR,      "siocsifvr",      &socktest_data_types[IPCOM_CST_TYPE_IFREQ_U16],    IP_FALSE },
#endif
#ifdef IP_SIOCGIFVR
    {IP_SIOCGIFVR,      "siocgifvr",      &socktest_data_types[IPCOM_CST_TYPE_IFREQ_U16],    IP_TRUE  },
#endif
    {IP_SIOCGIFDSTADDR, "siocgifdstaddr", &socktest_data_types[IPCOM_CST_TYPE_IFREQ_ADDR],   IP_TRUE  },
    {IP_SIOCAIFADDR,    "siocaifaddr",    &socktest_data_types[IPCOM_CST_TYPE_IFALIASREQ],   IP_FALSE },
    {IP_SIOCDIFADDR,    "siocdifaddr",    &socktest_data_types[IPCOM_CST_TYPE_IFREQ_ADDR],   IP_FALSE },
    {IP_SIOCSARP,       "siocsarp",       &socktest_data_types[IPCOM_CST_TYPE_ARPREQ],       IP_FALSE },
    {IP_SIOCGARP,       "siocgarp",       &socktest_data_types[IPCOM_CST_TYPE_ARPREQ],       IP_TRUE  },
    {IP_SIOCDARP,       "siocdarp",       &socktest_data_types[IPCOM_CST_TYPE_ARPREQ],       IP_FALSE },
#ifdef FIOFSTATGET
    {FIOFSTATGET,       "fiofstatget",    &socktest_data_types[IPCOM_CST_TYPE_STAT],         IP_TRUE  },
#endif
#ifdef IP_X_SIOCSDEBUG
    {IP_X_SIOCSDEBUG,   "siocsdebug",     &socktest_data_types[IPCOM_CST_TYPE_INT],          IP_FALSE },
#endif
#ifdef IP_X_SIOCGDEBUG
    {IP_X_SIOCGDEBUG,   "siocgdebug",     &socktest_data_types[IPCOM_CST_TYPE_INT],          IP_TRUE  },
#endif
#ifdef IPNET
    {IPNET_SIOCXDSCREATE, "siocxdscreate", &socktest_data_types[IPCOM_CST_TYPE_DS],          IP_TRUE  },
    {IPNET_SIOCXDSDESTROY,"siocxdsdestroy",&socktest_data_types[IPCOM_CST_TYPE_INT],         IP_FALSE },
    {IPNET_SIOCXADSMAP,   "siocxadsmap",   &socktest_data_types[IPCOM_CST_TYPE_DS_MAP],      IP_FALSE },
    {IPNET_SIOCXDDSMAP,   "siocxddsmap",   &socktest_data_types[IPCOM_CST_TYPE_DS_MAP],      IP_FALSE },
#ifndef IPNET_DIFFSERV_CLASSIFIER_MODE_BA
    {IPNET_SIOCXADSFILTER,"siocxadsfilter",&socktest_data_types[IPCOM_CST_TYPE_DS_FILTER],   IP_TRUE  },
    {IPNET_SIOCXDDSFILTER,"siocxddsfilter",&socktest_data_types[IPCOM_CST_TYPE_INT],         IP_FALSE },
#endif
#endif /* IPNET */
};


IP_STATIC Ipcom_cmd_socktest_proto_format socktest_prot_format[] =
{
#ifdef IPCOM_USE_INET
    {
        IP_AF_INET,
        IP_SOCK_RAW,
        IP_IPPROTO_IPV4,
        IP_NULL,
        ipcom_cmd_socktest_print_ip,
        IP_NULL
    },
    {
        IP_AF_INET,
        IP_SOCK_RAW,
        IP_IPPROTO_TCP,
        IP_NULL,
        ipcom_cmd_socktest_print_ip,
        ipcom_cmd_socktest_print_tcp
    },
    {
        IP_AF_INET,
        IP_SOCK_RAW,
        IP_IPPROTO_UDP,
        IP_NULL,
        ipcom_cmd_socktest_print_ip,
        ipcom_cmd_socktest_print_udp
    },
    {
        IP_AF_INET,
        IP_SOCK_RAW,
        IP_IPPROTO_ICMP,
        IP_NULL,
        ipcom_cmd_socktest_print_ip,
        ipcom_cmd_socktest_print_icmp
    },
    {
        IP_AF_INET,
        IP_SOCK_RAW,
        IP_IPPROTO_IGMP,
        IP_NULL,
        ipcom_cmd_socktest_print_ip,
        ipcom_cmd_socktest_print_igmp
    },
    {
        IP_AF_PACKET,
        IP_SOCK_RAW,
        IPCOM_ETH_P_IP,
        ipcom_cmd_socktest_print_eth,
        ipcom_cmd_socktest_print_ip,
        IP_NULL
    },
    {
        IP_AF_PACKET,
        IP_SOCK_DGRAM,
        IPCOM_ETH_P_IP,
        IP_NULL,
        ipcom_cmd_socktest_print_ip,
        IP_NULL
    },
    {
        IP_AF_INET,
        IP_SOCK_RAW,
        IP_IPPROTO_RAW,
        IP_NULL,
        ipcom_cmd_socktest_print_ip,
        IP_NULL
    },
#ifdef IPSCTP
    {
        IP_AF_INET,
        IP_SOCK_RAW,
        IP_IPPROTO_SCTP,
        IP_NULL,
        ipcom_cmd_socktest_print_ip,
        ipcom_cmd_socktest_print_sctp,
    },
#endif
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    {
        IP_AF_INET6,
        IP_SOCK_RAW,
        IP_IPPROTO_IPV6,
        IP_NULL,
        IP_NULL,
        IP_NULL
    },
    {
        IP_AF_INET6,
        IP_SOCK_RAW,
        IP_IPPROTO_TCP,
        IP_NULL,
        IP_NULL,
        ipcom_cmd_socktest_print_tcp
    },
    {
        IP_AF_INET6,
        IP_SOCK_RAW,
        IP_IPPROTO_UDP,
        IP_NULL,
        IP_NULL,
        ipcom_cmd_socktest_print_udp
    },
    {
        IP_AF_INET6,
        IP_SOCK_RAW,
        IP_IPPROTO_ICMPV6,
        IP_NULL,
        IP_NULL,
        ipcom_cmd_socktest_print_icmp6
    },
    {
        IP_AF_PACKET,
        IP_SOCK_RAW,
        IPCOM_ETH_P_IPV6,
        ipcom_cmd_socktest_print_eth,
        ipcom_cmd_socktest_print_ip6,
        IP_NULL
    },
    {
        IP_AF_PACKET,
        IP_SOCK_DGRAM,
        IPCOM_ETH_P_IPV6,
        IP_NULL,
        ipcom_cmd_socktest_print_ip6,
        IP_NULL
    },
#ifdef IPSCTP
    {
        IP_AF_INET6,
        IP_SOCK_RAW,
        IP_IPPROTO_SCTP,
        IP_NULL,
        IP_NULL,
        ipcom_cmd_socktest_print_sctp,
    },
#endif
#endif /* IPCOM_USE_INET6 */
    {
        IP_AF_PACKET,
        IP_SOCK_RAW,
        IPCOM_ETH_P_ALL,
        ipcom_cmd_socktest_print_eth,
        IP_NULL,
        IP_NULL
    }
};


/* FLOOD VARIABLES */
static Ipcom_mutex             ipcom_socktest_flood_lock;
static Ipcom_list              ipcom_socktest_flood_list;

#ifdef IPSCTP
IP_STATIC union Ipcom_cmd_socktest_addr   addrgr[SOCKETEST_ADDR_MAX_NUM];
#endif

/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_lfd_to_gfd
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPCOM_USE_FD_DAEMON
#define MAX_DAEMON_FD (10)
IP_STATIC Ip_fd
ipcom_lfd_to_gfd(Ip_fd fd, Ip_fd *l_fd, Ip_fd *g_fd)
{
    int i;

    for (i = 0; i < MAX_DAEMON_FD; i++)
    {
        if (fd == l_fd[i])
        {
            return g_fd[i];
        }
    }
    return -1;
}
#endif


/*
 *===========================================================================
 *                    ipcom_socktest_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_socktest_init(void *unused)
{
    (void)unused;

    ipcom_list_init(&ipcom_socktest_flood_list);
    ipcom_mutex_create(&ipcom_socktest_flood_lock);

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_fd_find
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipcom_cmd_socktest_fd *
ipcom_cmd_socktest_fd_find(Ipcom_cmd_socktest_fd *fdp, int fd)
{
    Ip_size_t i;

    for (i = 0; i < SOCKTEST_MAX_FD; i++)
        if (fdp[i].fd == fd)
            break;

    if (i < SOCKTEST_MAX_FD)
    {
#ifndef IPCOM_USE_FD_DAEMON
#if defined(IP_PORT_OSE) || defined(IP_PORT_OSE5)
        Ip_pid_t ppid;

        if (fd != -1)
        {
            /* Change the socket owner. */
            ppid = ipcom_getpid();
            if (ipcom_socketioctl(fd, IP_SIOCSPGRP, &ppid) < 0)
                ipcom_printf("     ipcom_socketioctl(%d, IP_SIOCPGRP) failed : %s [%d]" IP_LF,
                             fd, ipcom_strerror(ipcom_errno), ipcom_errno);
        }
#endif /* IP_PORT_OSE */
#endif
        return &fdp[i];
    }
    else
        return IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_param_parse
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_param_parse(char * param,
                                Ip_size_t list_row_len,
                                void *list,
                                Ip_size_t list_size)
{
    Ip_size_t i;
    Ipcom_cmd_socktest_param *param_list;

    for (i = 0; i < list_size; i++)
    {
        param_list = (Ipcom_cmd_socktest_param *) list;
        if (ipcom_strcmp(param, param_list->par_name) == 0)
            return list;
        list = (Ip_u8 *) list + list_row_len;
    }

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_param_to_int
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_cmd_socktest_param_to_int(char * param,
                                Ipcom_cmd_socktest_param *list,
                                Ip_size_t list_size)
{
    Ipcom_cmd_socktest_param *stp_list;

    stp_list = (Ipcom_cmd_socktest_param *)
        ipcom_cmd_socktest_param_parse(param,
                                       sizeof(Ipcom_cmd_socktest_param),
                                       list, list_size);
    /* Presume a numeric value if no match was found */
    return (stp_list ? stp_list->par_no : ipcom_atoi(param));
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_param_list_print
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_cmd_socktest_param_list_print(void *list,
                                    Ip_size_t list_len,
                                    Ip_size_t list_row_size)
{
    Ip_size_t i;
    Ipcom_cmd_socktest_param *param_list;

    for (i = 0; i < list_len; i++)
    {
        param_list = (Ipcom_cmd_socktest_param *) list;
        ipcom_printf("          %6d  %s"IP_LF,
                      param_list->par_no, param_list->par_name);
        list = (Ip_u8 *) list + list_row_size;
    }
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_sockopt_list_print
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_cmd_socktest_sockopt_list_print(Ipcom_cmd_socktest_sockopt *list,
                                      Ip_size_t list_len)
{
    Ip_size_t i;

    for (i = 0; i < list_len; i++)
        ipcom_printf("          %-6d  %-24s %s"IP_LF,
                      list[i].no, list[i].name, list[i].type->name);
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_ioctl_list_print
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_cmd_socktest_ioctl_list_print(Ipcom_cmd_socktest_ioctl *list,
                                    Ip_size_t list_len)
{
    Ip_size_t i;

    for (i = 0; i < list_len; i++)
        ipcom_printf("          %#010x  %-20s %s"IP_LF,
                     list[i].no, list[i].name, list[i].type->name);
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_int
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_int(char *data, char *obj)
{
    int *val;

    (void) obj;

    val = ipcom_malloc(sizeof (int));
    if (val == IP_NULL)
        return IP_NULL;

    if (data != IP_NULL)
        *val = ipcom_atoi(data);

    return val;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_int
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char *
ipcom_cmd_socktest_format_int(void * data)
{
    char *val;

    val = ipcom_malloc(12);
    if (val == IP_NULL)
        return IP_NULL;

    ipcom_sprintf(val,"%d",*(int*)data);

    return val;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_u8
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */

IP_STATIC void *
ipcom_cmd_socktest_parse_u8(char *data, char *obj)
{
    Ip_u8 *val;

    (void) obj;

    val = ipcom_malloc(sizeof (Ip_u8));
    if (val == IP_NULL)
        return IP_NULL;

    if (data != IP_NULL)
        *val = (Ip_u8)ipcom_atoi(data);

    return val;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_u8
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */

IP_STATIC char *
ipcom_cmd_socktest_format_u8(void *data)
{
    char *val;

    val = ipcom_malloc(4);
    if (val == IP_NULL)
        return IP_NULL;

    ipcom_sprintf(val,"%u",*(Ip_u8 *)data);

    return val;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_inaddr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_inaddr(char *data, char *obj)
{
    struct Ip_in_addr *val;
    int ret;

    (void) obj;

    val = ipcom_malloc(sizeof (struct Ip_in_addr));
    if (val == IP_NULL)
        return IP_NULL;

    if (data != IP_NULL)
    {
        ret = ipcom_inet_pton(IP_AF_INET,data,val);
        if (ret < 0)
        {
            ipcom_free(val);
            ipcom_printf("     Failed, bad address."IP_LF);
            return IP_NULL;
        }
    }

    return val;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_inaddr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char *
ipcom_cmd_socktest_format_inaddr(void *data)
{
    char * val;

    val = ipcom_malloc(16);
    if (val == IP_NULL)
        return IP_NULL;

    (void)ipcom_inet_ntop(IP_AF_INET,data, val, 16);

    return val;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_ptos
 *===========================================================================
 * Description: Converts a presentation for of a inet or inet6 address to
 *              and struct Ip_sockaddr_storage.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipcom_cmd_socktest_ptos(char *p, struct Ip_sockaddr_storage *s)
{
    ipcom_memset(s, 0, sizeof(struct Ip_sockaddr_storage));

#ifdef IPCOM_USE_INET
    {
        struct Ip_sockaddr_in *i = (struct Ip_sockaddr_in *) s;
        if (ipcom_inet_pton(IP_AF_INET, p, &i->sin_addr) > 0)
        {
            IPCOM_SA_LEN_SET(i, sizeof(struct Ip_sockaddr_in));
            i->sin_family = IP_AF_INET;
            return IP_TRUE;
        }
    }
#endif
#ifdef IPCOM_USE_INET6
    {
        struct Ip_sockaddr_in6 *i = (struct Ip_sockaddr_in6 *) s;;
        if (ipcom_inet_pton(IP_AF_INET6, p, &i->sin6_addr) > 0)
        {
            IPCOM_SA_LEN_SET(i, sizeof(struct Ip_sockaddr_in6));
            i->sin6_family = IP_AF_INET6;
            return IP_TRUE;
        }
    }
#endif
    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_group_req
 *===========================================================================
 * Description: Converts a string to a struct Ip_group_req, the string format
 *              is { ifname, mcast_addr }
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_group_req(char *data, char *obj)
{
    struct Ip_group_req *req;
    char *ifname;
    char *maddr;
    char *arg_str = IP_NULL;
    char *p;

    (void) obj;

    req = ipcom_malloc(sizeof(struct Ip_group_req));
    if (req == IP_NULL)
        return IP_NULL;

    if (data != IP_NULL)
    {
        arg_str = ipcom_strdup(data);
        if (arg_str == IP_NULL)
            goto bad;

        ifname = ipcom_strtok_r(arg_str, "\t ,{", &p);
        /* The ',' is needed since this function should be able to
           parse struct Ip_group_source_req as well */
        maddr = ipcom_strtok_r(IP_NULL, "\t },", &p);

        if (ifname == IP_NULL || maddr == IP_NULL)
        {
            goto bad;
        }

        req->gr_interface = ipcom_if_nametoindex(ifname);

        if (ipcom_cmd_socktest_ptos(maddr, &req->gr_group) == IP_FALSE)
        {
            ipcom_printf("     Failed, bad address format");
            goto bad;
        }
    }

    ipcom_free(arg_str);
    return req;

bad:
    ipcom_free(arg_str);
    ipcom_free(req);
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_group_req
 *===========================================================================
 * Description: Converts a string to a struct Ip_group_req, the string format
 *              is { ifname, maddr, saddr }
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_group_source_req(char * data, char * obj)
{
    struct Ip_group_source_req *req;
    void *ptr;
    char *saddr;
    char *arg_str = IP_NULL;
    char *p;

    (void) obj;

    req = ipcom_cmd_socktest_parse_group_req(data, obj);
    if (req != IP_NULL)
    {
        ptr = ipcom_realloc(req, sizeof(struct Ip_group_source_req));
        if (ptr == IP_NULL)
            goto bad;
        req = ptr;

        arg_str = ipcom_strdup(data);
        if (arg_str == IP_NULL)
            goto bad;

        if (ipcom_strtok_r(arg_str, "\t{ ,", &p) == IP_NULL
            || ipcom_strtok_r(IP_NULL, "\t ,", &p) == IP_NULL
            || (saddr = ipcom_strtok_r(IP_NULL, "\t }", &p)) == IP_NULL)
            goto bad_format;

        if (ipcom_cmd_socktest_ptos(saddr, &req->gsr_source) == IP_FALSE)
        {
            ipcom_printf("     Failed, bad address format");
            goto bad;
        }
    }

    ipcom_free(arg_str);
    return req;

 bad_format:
    ipcom_printf("     Failed, bad format. Should be { ifname, maddr, saddr }");
 bad:
    ipcom_free(arg_str);
    ipcom_free(req);
    return IP_NULL;
}


/*
 *===========================================================================
 *                  ipcom_cmd_socktest_str_to_in_pktinfo
 *===========================================================================
 * Description: Converts a string representation of packet info to its binary form
 * Parameters:  data - string representation of the packet info.
 *              cmsg - buffer where the packet info should be stored.
 * Returns:     Number of bytes copied into data.
 *
 */
IP_STATIC int
ipcom_cmd_socktest_str_to_in_pktinfo(char *data, struct Ip_cmsghdr *cmsg)
{
    struct Ip_in_pktinfo *pktinfo;
    char                 *ifname;
    char                 *src_addr;

    if (data[0] != '{'
        || data[strlen(data) - 1] != '}'
        || ipcom_strchr(data, ',') == IP_NULL)
        /* Invalid format */
        return 0;

    data[strlen(data) - 1] = '\0';

    pktinfo = IP_CMSG_DATA(cmsg);
    ipcom_memset(pktinfo, 0, sizeof(*pktinfo));

    memset(pktinfo, 0, sizeof(*pktinfo));
    ifname = &data[1];
    src_addr = ipcom_strchr(ifname, ',');
    *src_addr++ = '\0';

    pktinfo->ipi_ifindex = ipcom_if_nametoindex(ifname);
    if (ipcom_inet_pton(IP_AF_INET, src_addr, &pktinfo->ipi_addr) != 1)
        pktinfo->ipi_addr.s_addr = IP_INADDR_ANY;

    return sizeof(*pktinfo);
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_in_pktinfo
 *===========================================================================
 * Description: Returns a string representation of the IPv6 packetinfo struct.
 * Parameters:
 * Returns:     The string "{<ifindex>,<sender IPv4 addr>}"
 *
 */
IP_STATIC char *
ipcom_cmd_socktest_format_in_pktinfo(void *data)
{
    struct Ip_in_pktinfo *pktinfo = data;
    char *s;
    char b[IP_INET_ADDRSTRLEN];

    s = ipcom_malloc(IP_INET_ADDRSTRLEN + 10);
    if (s != IP_NULL)
        ipcom_sprintf(s, "{%u, %s}",
                      (unsigned)pktinfo->ipi_ifindex,
                      ipcom_inet_ntop(IP_AF_INET, &pktinfo->ipi_addr, b, sizeof(b)));
    return s;
}


/*
 *===========================================================================
 *                  ipcom_cmd_socktest_str_to_in6_pktinfo
 *===========================================================================
 * Description: Converts a string representation of packet info to its binary form
 * Parameters:  data - string representation of the packet info.
 *              cmsg - buffer where the packet info should be stored.
 * Returns:     Number of bytes copied into data.
 *
 */
IP_STATIC int
ipcom_cmd_socktest_str_to_in6_pktinfo(char *data, struct Ip_cmsghdr *cmsg)
{
    struct Ip_in6_pktinfo *pktinfo;
    char                  *ifname;
    char                  *src_addr;

    if (data[0] != '{'
        || data[strlen(data) - 1] != '}'
        || ipcom_strchr(data, ',') == IP_NULL)
        /* Invalid format */
        return 0;

    data[strlen(data) - 1] = '\0';

    pktinfo = IP_CMSG_DATA(cmsg);
    ipcom_memset(pktinfo, 0, sizeof(*pktinfo));

    ifname = &data[1];
    src_addr = ipcom_strchr(ifname, ',');
    *src_addr++ = '\0';

    pktinfo->ipi6_ifindex = ipcom_if_nametoindex(ifname);
    if (ipcom_inet_pton(IP_AF_INET6, src_addr, &pktinfo->ipi6_addr) != 1)
        ipcom_memset(&pktinfo->ipi6_addr, 0, sizeof(pktinfo->ipi6_addr));
    return sizeof(*pktinfo);
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_in6_pktinfo
 *===========================================================================
 * Description: Returns a string representation of the IPv6 packetinfo struct.
 * Parameters:
 * Returns:     The string "{<ifindex>,<sender IPv6 addr>}"
 *
 */
IP_STATIC char *
ipcom_cmd_socktest_format_in6_pktinfo(void *data)
{
    struct Ip_in6_pktinfo *pktinfo = data;
    char *s;
    char b[IP_INET6_ADDRSTRLEN];

    s = ipcom_malloc(IP_INET6_ADDRSTRLEN + 10);
    if (s != IP_NULL)
        ipcom_sprintf(s, "{%u, %s}",
                      (unsigned)pktinfo->ipi6_ifindex,
                      ipcom_inet_ntop(IP_AF_INET6, &pktinfo->ipi6_addr, b, sizeof(b)));
    return s;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_mreq
 *===========================================================================
 * Description: Convert "{ multiaddr, uniaddr }" to struct Ip_ip_mreq.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_mreq(char *data, char *obj)
{
    struct Ip_ip_mreq *val;
    char * multi_addr;
    char * if_addr;
    char * end;

    (void) obj;

    val = ipcom_malloc(sizeof (struct Ip_ip_mreq));
    if (val == IP_NULL)
        return IP_NULL;

    if (data != IP_NULL)
    {
        char *saveptr;

        end = ipcom_strchr(data,'}');

        if ((data[0] != '{') || (end == IP_NULL))
        {
            ipcom_printf("     Failed, bad data format.");
            goto bad;
        }

        multi_addr = ipcom_strtok_r(&data[1], ",", &saveptr);
        if_addr    = ipcom_strtok_r(IP_NULL, ",", &saveptr);

        if (multi_addr == IP_NULL || if_addr == IP_NULL)
        {
            ipcom_printf("     Failed, bad data format.");
            goto bad;
        }

        if ((ipcom_inet_pton(IP_AF_INET, multi_addr, &val->imr_multiaddr) != 1)
            || (ipcom_inet_pton(IP_AF_INET, if_addr, &val->imr_interface) != 1))
        {
            ipcom_printf("     Failed, bad address format");
            goto bad;
        }
    }

    return val;

bad:
    ipcom_free(val);
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_mreq
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char *
ipcom_cmd_socktest_format_mreq(void * data)
{
    struct Ip_ip_mreq *mreq = (struct Ip_ip_mreq*)data;
    char * val;
    char addrstr[IP_INET_ADDRSTRLEN];

    val = ipcom_malloc(IP_INET_ADDRSTRLEN * 2 + 4);
    if (val == IP_NULL)
        return IP_NULL;

    ipcom_strcpy(val, "{");
    ipcom_strcat(val, ipcom_inet_ntop(IP_AF_INET, &mreq->imr_multiaddr, addrstr, sizeof(addrstr)));
    ipcom_strcat(val, ",");
    ipcom_strcat(val, ipcom_inet_ntop(IP_AF_INET, &mreq->imr_interface, addrstr, sizeof(addrstr)));
    ipcom_strcat(val, "}");

    return val;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_mreq6
 *===========================================================================
 * Description: Convert "{ multiaddr6, ifname }" to struct Ip_ip6_mreq.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_mreq6(char *data, char *obj)
{
    struct Ip_ipv6_mreq *val;
    char                *multi_addr;
    char                *if_name;
    char                *end;

    (void) obj;

    val = ipcom_malloc(sizeof (struct Ip_ipv6_mreq));
    if (val == IP_NULL)
        return IP_NULL;

    if (data != IP_NULL)
    {
        end = ipcom_strchr(data,'}');

        if ((data[0] != '{') || (end == IP_NULL))
        {
            ipcom_printf("     Failed, bad data format.");
            goto bad;
        }

        multi_addr = &data[1];
        data = ipcom_strchr(data,',');

        if (data == IP_NULL)
        {
            ipcom_printf("     Failed, bad data format.");
            goto bad;
        }

        if_name = &data[1];

        /* Add string terminators */
        *end  = 0;
        *data = 0;

        if ((val->ipv6mr_interface = ipcom_if_nametoindex(if_name)) == 0)
        {
            ipcom_printf("     Failed, bad interface name");
            goto bad;
        }

        if (ipcom_inet_pton(IP_AF_INET6, multi_addr, &val->ipv6mr_multiaddr) < 0)
        {
            ipcom_printf("     Failed, bad address format");
            goto bad;
        }
    }

    return val;

bad:
    ipcom_free(val);
    return IP_NULL;

}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_mreq6
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char *
ipcom_cmd_socktest_format_mreq6(void * data)
{
    struct Ip_ipv6_mreq *mreq = (struct Ip_ipv6_mreq*) data;
    char                *val;
    char                 addrstr[IP_INET6_ADDRSTRLEN];

    val = ipcom_malloc(IP_INET6_ADDRSTRLEN + IP_IFNAMSIZ + 4);
    if (val == IP_NULL)
        return IP_NULL;

    ipcom_strcpy(val, "{");
    ipcom_strcat(val, ipcom_inet_ntop(IP_AF_INET6, &mreq->ipv6mr_multiaddr, addrstr, sizeof(addrstr)));
    ipcom_strcat(val, ",");
    ipcom_strcat(val, ipcom_if_indextoname(mreq->ipv6mr_interface, addrstr));
    ipcom_strcat(val, "}");

    return val;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_icmp6_filter
 *===========================================================================
 * Description: Convert "{ Block|Pass type1 type2 ... }" to struct Ip_icmp6_filter.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_icmp6_filter(char * data, char * obj)
{
    struct Ip_icmp6_filter *filter;
    char   *current;
    char   *end;
    int     i;
    Ip_bool block = IP_TRUE;

    (void) obj;

    filter = ipcom_malloc(sizeof (struct Ip_icmp6_filter));
    if (filter == IP_NULL)
        return IP_NULL;

    IP_ICMP6_FILTER_SETPASSALL(filter);

    if (data != IP_NULL)
    {
        end = ipcom_strchr(data, '}');
        current = ipcom_strstr(data, "Block");
        if (current == IP_NULL)
        {
            current = ipcom_strstr(data, "Pass");
            block = IP_FALSE;
            IP_ICMP6_FILTER_SETBLOCKALL(filter);
        }

        if ((data[0] != '{') || (end == IP_NULL) || (current == IP_NULL))
        {
            ipcom_printf("     Failed, bad data format.");
            goto bad;
        }

        for (;;)
        {
            current = ipcom_strchr(current, ' ');

            if (current == IP_NULL)
                break;

            while (*current == ' ')
                ++current;
            if (current >= end)
                break;

            (void)ipcom_sscanf(current, "%d", &i);
            if (i <= 0 || i > 255)
            {
                ipcom_printf("     Failed, bad blocked ICMPv6 type");
                goto bad;
            }

            if (block)
                IP_ICMP6_FILTER_SETBLOCK(i, filter); /*lint !e502 Expected unsigned type */
            else
                IP_ICMP6_FILTER_SETPASS(i, filter); /*lint !e502 Expected unsigned type */
        }
    }

    return filter;

bad:
    ipcom_free(filter);
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_icmp6_filter
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char *
ipcom_cmd_socktest_format_icmp6_filter(void * data)
{
    struct Ip_icmp6_filter *filter = (struct Ip_icmp6_filter*) data;
    char *val;
    int cur;
    int i;
    Ip_size_t num_blocked = 0;

    for (i = 0; i < 256; i++)
        if (IP_ICMP6_FILTER_WILLBLOCK(i, filter))
            num_blocked++;

    val = ipcom_calloc(1, 100 + num_blocked * 4);

    if (val != IP_NULL)
    {
        cur = ipcom_sprintf(val, "{ Block ");

        for (i = 0; i < 256; i++)
            if (IP_ICMP6_FILTER_WILLBLOCK(i, filter))
                cur += ipcom_sprintf(&val[cur], "%d ", i);

        ipcom_sprintf(&val[cur], "}");
    }

    return val;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_ifreq_addr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_ifreq_addr(char * data, char * obj)
{
    struct Ip_ifreq        *ifr;

    ifr = ipcom_malloc(sizeof(struct Ip_ifreq));
    if (ifr == IP_NULL)
        return IP_NULL;

    ipcom_memset(ifr,0,sizeof (struct Ip_ifreq));

    if (obj != IP_NULL)
        ipcom_strncpy(ifr->ifr_name, obj, IP_IFNAMSIZ);
    if (data != IP_NULL)
    {
        if (ipcom_getsockaddrbyaddr(IP_AF_INET, data, &ifr->ip_ifr_addr) < 0)
            goto bad;
    }

    return ifr;

bad:
    ip_assert(ifr != IP_NULL);
    ipcom_free(ifr);

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_ifreq_addr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char *
ipcom_cmd_socktest_format_ifreq_addr(void * data)
{
    struct Ip_ifreq *ifr = (struct Ip_ifreq*)data;
    char                   *buf;

    buf = ipcom_malloc(50);
    if (buf == IP_NULL)
        return IP_NULL;

    (void)ipcom_cmd_sockaddr_to_str(&ifr->ip_ifr_addr,
                               IP_FALSE,
                               IP_TRUE,
                               buf,
                               50);
    return buf;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_ifreq_int
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_ifreq_int(char * data, char * obj)
{
    struct Ip_ifreq *ifr;

    ifr = ipcom_malloc(sizeof (struct Ip_ifreq));
    if (ifr == IP_NULL)
        return IP_NULL;

    ipcom_memset(ifr, 0, sizeof (struct Ip_ifreq));

    if (obj != IP_NULL)
        ipcom_strncpy(ifr->ifr_name, obj, IP_IFNAMSIZ);
    if (data != IP_NULL)
        ifr->ip_ifr_metric = ipcom_atoi(data);

    return ifr;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_ifreq_int
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char *
ipcom_cmd_socktest_format_ifreq_int(void * data)
{
    struct Ip_ifreq *ifr = (struct Ip_ifreq*)data;

    return (ipcom_cmd_socktest_format_int(&ifr->ip_ifr_metric));

}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_ifreq_u16
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_ifreq_u16(char * data, char * obj)
{
    struct Ip_ifreq *ifr;

    ifr = ipcom_malloc(sizeof (struct Ip_ifreq));
    if (ifr == IP_NULL)
        return IP_NULL;

    ipcom_memset(ifr, 0, sizeof (struct Ip_ifreq));

    if (obj != IP_NULL)
        ipcom_strncpy(ifr->ifr_name, obj, IP_IFNAMSIZ);
    if (data != IP_NULL)
        ifr->ip_ifr_vr = (Ip_u16)ipcom_atoi(data);

    return ifr;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_ifreq_u16
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char *
ipcom_cmd_socktest_format_ifreq_u16(void *data)
{
    struct Ip_ifreq *ifr = (struct Ip_ifreq *)data;
    char *val;

    val = ipcom_malloc(12);
    if (val == IP_NULL)
        return IP_NULL;

    ipcom_sprintf(val,"%d", (int)ifr->ip_ifr_vr);
    return val;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_ifreq_flags
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_ifreq_flags(char * data, char * obj)
{
    struct Ip_ifreq *ifr;

    ifr = ipcom_malloc(sizeof (struct Ip_ifreq));
    if (ifr == IP_NULL)
        return IP_NULL;

    ipcom_memset(ifr, 0, sizeof (struct Ip_ifreq));

    if (obj != IP_NULL)
        ipcom_strncpy(ifr->ifr_name, obj, IP_IFNAMSIZ);

    if (data != IP_NULL)
        ifr->ip_ifr_flags = (Ip_u16) ipcom_strtoul(data, IP_NULL, 0);

    return ifr;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_ifreq_flags
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char *
ipcom_cmd_socktest_format_ifreq_flags(void * data)
{
    struct Ip_ifreq *ifr = (struct Ip_ifreq*)data;
    char            *buf;

    buf = ipcom_malloc(100);
    if (buf == IP_NULL)
        return IP_NULL;

    (void)ipcom_cmd_if_flags_to_str(ifr->ip_ifr_flags, buf, 100);

    return buf;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_timeval
 *===========================================================================
 * Description:
 * Parameters:
 *
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_timeval(char *data, char *obj)
{
    struct Ip_timeval *tv;

    (void) obj;

    tv = ipcom_malloc(sizeof(struct Ip_timeval));
    if (tv == IP_NULL)
        return IP_NULL;

    if (data != IP_NULL)
    {
        long msec = ipcom_atol(data);
        tv->tv_sec  = msec / 1000;
        tv->tv_usec = (msec % 1000) * 1000;  /*lint !e647 */
    }

    return tv;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_timeval
 *===========================================================================
 * Description:
 * Parameters:
 *
 * Returns:
 *
 */
IP_STATIC char *
ipcom_cmd_socktest_format_timeval(void * data)
{
    char              *val;
    struct Ip_timeval *tv = (struct Ip_timeval *) data;

    val = ipcom_malloc(12);
    if (val == IP_NULL)
        return IP_NULL;

    ipcom_sprintf(val, "%ld", tv->tv_sec * 1000 + tv->tv_usec / 1000);

    return val;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_linger
 *===========================================================================
 * Description:
 * Parameters:
 *
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_linger(char *data, char *obj)
{
    struct Ip_linger *l;

    (void) obj;

    l = ipcom_calloc(1, sizeof(struct Ip_linger));
    if (l == IP_NULL)
        return IP_NULL;

    if (data != IP_NULL
        && ipcom_strcmp(data, "off") != 0)
    {
        l->l_onoff  = 1;
        l->l_linger = ipcom_atoi(data);
    }

    return l;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_linger
 *===========================================================================
 * Description:
 * Parameters:
 *
 * Returns:
 *
 */
IP_STATIC char *
ipcom_cmd_socktest_format_linger(void * data)
{
    char             *val;
    struct Ip_linger *l = (struct Ip_linger *) data;

    val = ipcom_malloc(12);
    if (val == IP_NULL)
        return IP_NULL;

    if (l->l_onoff)
        ipcom_sprintf(val, "%d", l->l_linger);
    else
        ipcom_sprintf(val, "off");

    return val;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_md5key
 *===========================================================================
 * Description:
 * Parameters:
 *
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_md5key(char *data, char *obj)
{
    char *key;

    (void) obj;

    key = ipcom_calloc(1, IPCOM_CST_MD5_KEY_MAXLEN + 1);
    if (key == IP_NULL)
        return IP_NULL;

    if (data != IP_NULL)
        ipcom_strncpy(key, data, IPCOM_CST_MD5_KEY_MAXLEN);

    return key;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_md5key
 *===========================================================================
 * Description:
 * Parameters:
 *
 * Returns:
 *
 */
IP_STATIC char *
ipcom_cmd_socktest_format_md5key(void * data)
{
    char *val;

    val = ipcom_calloc(1, IPCOM_CST_MD5_KEY_MAXLEN + 1);
    if (val == IP_NULL)
        return IP_NULL;

    if (ipcom_strlen(data) == 0)
        ipcom_strcpy(val, "#DISABLED#");
    else
        ipcom_strncpy(val, data, IPCOM_CST_MD5_KEY_MAXLEN);
    return val;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_ifname
 *===========================================================================
 * Description:
 * Parameters:
 *
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_ifname(char *data, char *obj)
{
    char *key;

    (void) obj;

    key = ipcom_calloc(1, IP_IFNAMSIZ + 1);
    if (key == IP_NULL)
        return IP_NULL;

    if (data != IP_NULL)
        ipcom_strncpy(key, data, IP_IFNAMSIZ);

    return key;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_ifname
 *===========================================================================
 * Description:
 * Parameters:
 *
 * Returns:
 *
 */
IP_STATIC char *
ipcom_cmd_socktest_format_ifname(void * data)
{
    char *val;

    val = ipcom_calloc(1, IP_IFNAMSIZ + 1);
    if (val == IP_NULL)
        return IP_NULL;

    ipcom_strncpy(val, data, IP_IFNAMSIZ);
    return val;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_packet_mreq
 *===========================================================================
 * Description:
 * Parameters:
 *
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_packet_mreq(char * data, char * obj)
{
    struct Ip_packet_mreq *m;
    char                  *d = IP_NULL;
    char                  *pt;
    struct Ip_sockaddr_ll  ll;
    char                  *type;
    char                  *addr;

    (void) obj;

    m = ipcom_calloc(1, sizeof(struct Ip_packet_mreq));
    if (m == IP_NULL)
        return IP_NULL;

    if (data != IP_NULL)
    {
        d = ipcom_strdup(data);
        if (d == IP_NULL || *d != '{')
            goto errout;
        addr = ipcom_strtok_r(&d[1], ",", &pt);
        type = ipcom_strtok_r(IP_NULL, "}", &pt);

        if (type == IP_NULL || addr == IP_NULL)
            goto errout;

        if (ipcom_getsockaddrbyaddr(IP_AF_PACKET, addr, (void *)&ll) < 0)
            goto errout;

        m->mr_type    = (Ip_u8)ipcom_atoi(type);
        m->mr_alen    = ll.sll_halen;
        m->mr_ifindex = ll.sll_ifindex;
        ipcom_memcpy(m->mr_address, ll.sll_addr, ll.sll_halen);
    }

    ipcom_free(d);
    return m;

 errout:
    ipcom_printf("%s has invalid format, should be {link-addr%%ifname,type}"IP_LF, data);
    ipcom_free(d);
    ipcom_free(m);
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_packet_mreq
 *===========================================================================
 * Description:
 * Parameters:
 *
 * Returns:
 *
 */
IP_STATIC char *
ipcom_cmd_socktest_format_packet_mreq(void * data)
{
    char                  *val;
    struct Ip_packet_mreq *mreq = (struct Ip_packet_mreq *) data;
    struct Ip_sockaddr_dl  dl;
    char                   str_addr[40];
    char                   ifname[IP_IFNAMSIZ];

    val = ipcom_malloc(100);
    if (val == IP_NULL)
        return IP_NULL;

    ipcom_memset(&dl, 0, sizeof(dl));
    IPCOM_SA_LEN_SET(&dl, sizeof(dl));
    dl.sdl_family = IP_AF_LINK;
    dl.sdl_index  = (Ip_u16)mreq->mr_ifindex;
    dl.sdl_alen   = mreq->mr_alen;
    ipcom_memcpy(IP_SOCKADDR_DL_LLADDR(&dl), mreq->mr_address, mreq->mr_alen);

    ipcom_sprintf(val, "{%s,%d,%s}",
                  ipcom_if_indextoname(mreq->mr_ifindex, ifname),
                  mreq->mr_type,
                  ipcom_cmd_sockaddr_to_str((void *)&dl, IP_FALSE, IP_TRUE, str_addr, sizeof(str_addr)));

    return val;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_ifaliasreq_addr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_ifaliasreq_addr(char *data, char *obj)
{
    struct Ip_ifaliasreq   *ifa;
    char                   *mask;
    char                   *dst  = IP_NULL;
    char                   *end;

    ifa = ipcom_malloc(sizeof (struct Ip_ifaliasreq));
    if (ifa == IP_NULL)
        return IP_NULL;

    ipcom_memset(ifa, 0, sizeof (struct Ip_ifaliasreq));

    if (obj != IP_NULL)
        ipcom_strncpy(ifa->ifra_name, obj, IP_IFNAMSIZ);

    if (data != IP_NULL)
    {
        end = ipcom_strchr(data,'}');

        if ((data[0] != '{') || (end == IP_NULL))
            goto bad;

        /* Get strings */
        data++;
        *end = 0;
        mask = ipcom_strchr(data,',');

        if (mask != IP_NULL)
        {
            *mask++ = 0;
            dst = ipcom_strchr(mask, ',');

            if (dst != IP_NULL)
                *dst++ = 0;
        }

        if (ipcom_getsockaddrbyaddr(IP_AF_INET, data, &ifa->ifra_addr) < 0)
            goto bad;

        if (mask != IP_NULL)
            if (ipcom_getsockaddrbyaddr(IP_AF_INET, mask, &ifa->ifra_mask) < 0)
                goto bad;

        if (dst != IP_NULL)
            if (ipcom_getsockaddrbyaddr(IP_AF_INET, dst, &ifa->ifra_dstaddr) < 0)
                goto bad;
    }

    return ifa;

bad:
    if (ifa != IP_NULL)
        ipcom_free(ifa);

    ipcom_printf("     Failed, bad format");
    return IP_NULL;

}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_arpreq
 *===========================================================================
 * Description: Parses "{ addr4, link_addr }" OR "addr4" -> struct Ip_arpreq
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_arpreq(char *data, char *obj)
{
    struct Ip_arpreq  *ar;
    char              *end;
    char              *p_lladdr;

    ar = ipcom_malloc(sizeof (struct Ip_arpreq));
    if (ar == IP_NULL)
        goto bad;

    ipcom_memset(ar, 0, sizeof (struct Ip_arpreq));

    if (data != IP_NULL)
    {
        end = ipcom_strchr(data,'}');

        if ((data[0] != '{') || (end == IP_NULL))
        {
            if (ipcom_getsockaddrbyaddr(IP_AF_INET, data, &ar->arp_pa) < 0)
                goto bad;

            ar->arp_ha.sdl_index = (Ip_u16)ipcom_if_nametoindex(obj);
            if (ar->arp_ha.sdl_index == 0)
                goto bad;

            return ar;
        }

        /* Get strings */
        *end = 0;
        p_lladdr = ipcom_strchr(data++, ',');
        if (p_lladdr == IP_NULL)
            goto bad;
        *p_lladdr++ = 0;

        if (ipcom_getsockaddrbyaddr(IP_AF_INET, data, &ar->arp_pa) < 0)
            goto bad;

        if (ipcom_getsockaddrbyaddr(IP_AF_LINK, p_lladdr, (void *)&ar->arp_ha) < 0)
            goto bad;

        ar->arp_ha.sdl_index = (Ip_u16)ipcom_if_nametoindex(obj);
        if (ar->arp_ha.sdl_index == 0)
            goto bad;
    }

    return ar;

bad:
    ipcom_printf("     Failed, bad format");

    if (ar != IP_NULL)
        ipcom_free(ar);

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_arpreq
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char *
ipcom_cmd_socktest_format_arpreq(void * data)
{
    struct Ip_arpreq *ar = (struct Ip_arpreq*)data;
    char             *buf;
    char             pa[20];
    char             ha[20];

    buf = ipcom_malloc(100);
    if (buf == IP_NULL)
        return IP_NULL;

    (void)ipcom_cmd_sockaddr_to_str(&ar->arp_pa, IP_FALSE, IP_TRUE, pa, sizeof (pa));
    (void)ipcom_cmd_sockaddr_to_str((void *)&ar->arp_ha, IP_FALSE, IP_TRUE, ha, sizeof (ha));

    ipcom_snprintf (buf, 100, "{%s,%s,%d}%c", pa, ha, ar->arp_flags, '\0');

    return buf;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_ip_opts
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_ip_opts(char *data, char *obj)
{
    Ip_u8 *o;
    int    i;
    unsigned int b;

    (void) obj;

    if (data[0] != '0' || data[1] != 'x')
        return IP_NULL;

    o = ipcom_calloc(1, 40);
    if (o == IP_NULL)
        return IP_NULL;

    for (i = 0; data[2 * i + 2] != '\0'; i++)
    {
        (void)ipcom_sscanf(&data[2 * i + 2], "%02x", &b);
        o[i] = (Ip_u8) b;
    }

    return o;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_ip_opts
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char *
ipcom_cmd_socktest_format_ip_opts(void * data)
{
    char  *val;
    char   num[3];
    Ip_u8 *o = data;
    int    i = 0;

    val = ipcom_calloc(1, 2 * 40 + 1);
    ipcom_strcpy(val, "0x");
    do
    {
        ipcom_sprintf(num, "%02x", o[i]);
        ipcom_strcat(val, num);
    } while (o[i++] != 0);

    return val;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_sovlan
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_sovlan(char *data, char *obj)
{
    struct Ip_sovlan *sovlan = IP_NULL;
    char             *arg_str;
    char             *p;
    char             *onoff_str;
    char             *vlan_str;
    char             *prio_str;
    char             *ptag_str;

    (void) obj;

    arg_str = ipcom_strdup(data);
    if (arg_str == IP_NULL)
        return IP_NULL;

    onoff_str = ipcom_strtok_r(arg_str, "\t {,", &p);
    ptag_str = ipcom_strtok_r(IP_NULL, "\t ,", &p);
    vlan_str = ipcom_strtok_r(IP_NULL, "\t ,", &p);
    prio_str = ipcom_strtok_r(IP_NULL, "\t ,", &p);
    if (onoff_str == IP_NULL || ptag_str == IP_NULL || vlan_str == IP_NULL || prio_str == IP_NULL)
    {
        ipcom_printf("     Failed, bad format. Format is {<on|off>, <ptagged>, <vlan>, <prio>}");
        goto cleanup;
    }

    sovlan = ipcom_calloc(1, sizeof(struct Ip_sovlan));
    if (sovlan == IP_NULL)
        goto cleanup;

    sovlan->vlan_onoff      = (ipcom_strcmp(onoff_str, "on") == 0);
    sovlan->priority_tagged = ipcom_atoi(ptag_str);
    sovlan->vid             = ipcom_atoi(vlan_str);
    sovlan->upriority       = ipcom_atoi(prio_str);

 cleanup:
    ipcom_free(arg_str);
    return sovlan;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_sovlan
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char *
ipcom_cmd_socktest_format_sovlan(void * data)
{
    struct Ip_sovlan *sovlan = data;
    char             *val;

    val = ipcom_calloc(1, 40);
    if (val == IP_NULL)
        return val;
    ipcom_sprintf(val, "{%s,%u,%u,%u}",
                  sovlan->vlan_onoff ? "on" : "off",
                  sovlan->priority_tagged,
                  sovlan->vid,
                  sovlan->upriority);

    return val;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_stat
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_stat(char *data, char *obj)
{
    (void) data;
    (void) obj;
    return ipcom_calloc(1, sizeof(struct Ip_stat));
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_stat
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char *
ipcom_cmd_socktest_format_stat(void * data)
{
    struct Ip_stat *stat = data;
    char           *val;

    val = ipcom_calloc(1, 1000);
    if (val == IP_NULL)
        return val;
    ipcom_sprintf(val,
                  "{st_mode:%i,st_ino:%u,st_dev:%u,st_nlink:%u,"
                  "st_uid:%u,st_gid:%u,st_size:%u,st_atime:%lu,st_mtime:%lu,st_ctime:%lu}",
                  (int)stat->st_mode,
                  (unsigned)stat->st_ino,
                  (unsigned)stat->st_dev,
                  (unsigned)stat->st_nlink,
                  (unsigned)stat->st_uid,
                  (unsigned)stat->st_gid,
                  (unsigned)stat->st_size,
                  stat->st_atime,
                  stat->st_mtime,
                  stat->st_ctime);

    return val;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_gre_key
 *===========================================================================
 * Description:
 * Parameters:
 *
 * Returns:
 *
 */
IP_STATIC void *
ipcom_cmd_socktest_parse_gre_key(char *data, char *obj)
{
    struct Ip_gre_key *gk;

    (void) obj;

    gk = ipcom_calloc(1, sizeof(struct Ip_gre_key));
    if (gk == IP_NULL)
        return IP_NULL;

    if (data != IP_NULL
        && ipcom_strcmp(data, "off") != 0)
    {
        gk->gk_onoff  = 1;
        gk->gk_key    = ipcom_atoi(data);
    }

    return gk;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_gre_key
 *===========================================================================
 * Description:
 * Parameters:
 *
 * Returns:
 *
 */
IP_STATIC char *
ipcom_cmd_socktest_format_gre_key(void * data)
{
    char              *val;
    struct Ip_gre_key *gk = (struct Ip_gre_key *) data;

    val = ipcom_malloc(16);
    if (val == IP_NULL)
        return IP_NULL;

    if (gk->gk_onoff)
        ipcom_sprintf(val, "%u", (unsigned)gk->gk_key);
    else
        ipcom_sprintf(val, "off");

    return val;
}


#ifdef IPNET
/*
 *===========================================================================
 *                     ipcom_cmd_socktest_parse_ds
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void * ipcom_cmd_socktest_parse_ds(char * data, char * obj)
{
    struct Ipnet_ds *ds;

    (void) obj;

    ds = ipcom_calloc(1, sizeof(*ds));

    if (ipcom_strstr(data, "SimpleMarker"))
    {
        unsigned mask, ds_value, drop_precedence;
        if (ipcom_sscanf(data, "{SimpleMarker,%u,%u,%u}", &mask, &ds_value, &drop_precedence) != 3)
        {
            ipcom_printf("     Failed, bad format. Format is {SimpleMarker,mask,ds_value,drop_precedence}");
            goto errout;
        }
        ipcom_strcpy(ds->name, "SimpleMarker");
        ds->d.sm.mask            = (Ip_u8) mask;
        ds->d.sm.ds_value        = (Ip_u8) ds_value;
        ds->d.sm.drop_precedence = (Ip_u8) drop_precedence;
    }
    else if (ipcom_strstr(data, "srTCM"))
    {
        unsigned mode, cir, cbs, ebs, ds_green, ds_yellow, ds_red;
        if (ipcom_sscanf(data, "{srTCM,%u,%u,%u,%u,%u,%u,%u}", &mode, &cir, &cbs, &ebs, &ds_green, &ds_yellow, &ds_red) != 7)
        {
            ipcom_printf("     Failed, bad format. Format is {srTCM,mode,CIR,CBS,EBS,ds_green,ds_yellow,ds_red}");
            goto errout;
        }
        ipcom_strcpy(ds->name, "srTCM");
        ds->d.srtcm.mode      = (Ip_u8) mode;
        ds->d.srtcm.CIR       = (Ip_u32) cir;
        ds->d.srtcm.CBS       = (Ip_u32) cbs;
        ds->d.srtcm.EBS       = (Ip_u32) ebs;
        ds->d.srtcm.ds_green  = (Ip_u8) ds_green;
        ds->d.srtcm.ds_yellow = (Ip_u8) ds_yellow;
        ds->d.srtcm.ds_red    = (Ip_u8) ds_red;
    }
    else
    {
        ipcom_printf("     Failed, unsupported DS. Must be SimpleMarker|srTCM");
        goto errout;
    }

    return ds;

 errout:
    ipcom_free(ds);
    return IP_NULL;
}


/*
 *===========================================================================
 *                     ipcom_cmd_socktest_format_ds
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char * ipcom_cmd_socktest_format_ds(void * data)
{
    /* Print the first integer in the Ipnet_ds structure */
    return ipcom_cmd_socktest_format_int(data);
}


/*
 *===========================================================================
 *                     ipcom_cmd_socktest_parse_ds_map
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void * ipcom_cmd_socktest_parse_ds_map(char * data, char * obj)
{
    struct Ipnet_ds_map *ds_map = IP_NULL;
    unsigned filter_id;
    unsigned ds_id;

    (void) obj;

    if (ipcom_sscanf(data, "{%u,%u}", &filter_id, &ds_id) != 2)
    {
        ipcom_printf("     Failed, bad format. Should be {filter_id,ds_id}");
        goto errout;
    }

    ds_map = ipcom_malloc(sizeof(*ds_map));
    if (ds_map != IP_NULL)
    {
        ds_map->filter_id = (Ip_u8) filter_id;
        ds_map->ds_id     = (Ip_u8) ds_id;
    }

    return ds_map;

 errout:
    ipcom_free(ds_map);
    return IP_NULL;
}

#endif /* IPNET */

#ifdef IPSCTP
/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_rtinfo
 *===========================================================================
 * Description:RW
 * Parameters:
 * Returns:
 *
 */

IP_STATIC void * ipcom_cmd_socktest_parse_rtinfo(char * data, char * obj)
{
    struct Ipsctp_rtoinfo *rtinfo;

    rtinfo = ipcom_calloc(1, sizeof(struct Ipsctp_rtoinfo));
    if (IP_NULL == rtinfo)
    {
        return IP_NULL;
    }

    if (IP_TRUE == *obj)
    {
        rtinfo->srto_assoc_id = ipcom_atoi(data);
    }
    else
    {
        if (ipcom_sscanf(data, "{%u,%u,%u,%u}",
                         &rtinfo->srto_assoc_id,
                         &rtinfo->srto_initial,
                         &rtinfo->srto_max,
                         &rtinfo->srto_min) != 4)
        {
            ipcom_printf("     Failed, bad format. Format is {srto_assoc_id,srto_initial,srto_max,srto_min}");
            ipcom_free(rtinfo);
            return IP_NULL;
        }
    }
    return rtinfo;
}
/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_rtinfo
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */

IP_STATIC char * ipcom_cmd_socktest_format_rtinfo(void * data)
{
    struct Ipsctp_rtoinfo *rtinfo = data;
    char           *val;

    val = ipcom_calloc(1, 1000);
    if (val == IP_NULL)
        return val;
    ipcom_sprintf(val,
                  "{%u,%u,%u,%u}",
                  rtinfo->srto_assoc_id,
                  rtinfo->srto_initial,
                  rtinfo->srto_max,
                  rtinfo->srto_min);
    return val;
}

/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_associnfo
 *===========================================================================
 * Description:RW
 * Parameters:
 * Returns:
 *
 */

IP_STATIC void * ipcom_cmd_socktest_parse_associnfo(char * data, char * obj)
{
    struct Ipsctp_assocparams *assparam;

    assparam = ipcom_calloc(1, sizeof(struct Ipsctp_assocparams));
    if (IP_NULL == assparam)
    {
        return IP_NULL;
    }
    if (IP_TRUE == *obj)
    {
        assparam->sasoc_assoc_id = ipcom_atoi(data);
    }
    else
    {
        if (ipcom_sscanf(data, "{%u,%u,%u,%u,%u,%u}",
                         &assparam->sasoc_assoc_id,
                         &assparam->sasoc_asocmaxrxt,
                         &assparam->sasoc_number_peer_destinations,
                         &assparam->sasoc_peer_rwnd,
                         &assparam->sasoc_local_rwnd,
                         &assparam->sasoc_cookie_life) != 6)
        {
            ipcom_printf("     Failed, bad format.\
                               Format is {sasoc_assoc_id,sasoc_asocmaxrxt,\
                               sasoc_number_peer_destinations,sasoc_peer_rwnd\
                               sasoc_local_rwnd,sasoc_cookie_life}");
            ipcom_free(assparam);
            return IP_NULL;
        }
    }

    return assparam;
}
/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_associnfo
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */

IP_STATIC char * ipcom_cmd_socktest_format_associnfo(void * data)
{
    struct Ipsctp_assocparams *assparam = data;
    char           *val;

    val = ipcom_calloc(1, 1000);
    if (val == IP_NULL)
        return val;
    ipcom_sprintf(val,
                  "{%u,%u,%u,%u,%u,%u}",
                  assparam->sasoc_assoc_id,
                  assparam->sasoc_asocmaxrxt,
                  assparam->sasoc_number_peer_destinations,
                  assparam->sasoc_peer_rwnd,
                  assparam->sasoc_local_rwnd,
                  assparam->sasoc_cookie_life);
    return val;
}

/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_default_send_param
 *===========================================================================
 * Description:RW
 * Parameters:
 * Returns:
 *
 */

IP_STATIC void * ipcom_cmd_socktest_parse_default_send_param(char * data, char * obj)
{
    struct Ipsctp_sndrcvinfo *sndrcvinfo;

    sndrcvinfo = ipcom_calloc(1, sizeof(struct Ipsctp_assocparams));
    if (IP_NULL == sndrcvinfo)
    {
        return IP_NULL;
    }

    if (IP_FALSE == *obj)
    {

        if (ipcom_sscanf(data, "{%u,%u,%u,%u,%u,%u,%u,%u,%u,%u}",
                         &sndrcvinfo->sinfo_stream,
                         &sndrcvinfo->sinfo_ssn,
                         &sndrcvinfo->sinfo_flags,
                         &sndrcvinfo->sinfo_pr_policy,
                         &sndrcvinfo->sinfo_ppid,
                         &sndrcvinfo->sinfo_context,
                         &sndrcvinfo->sinfo_pr_value,
                         &sndrcvinfo->sinfo_tsn,
                         &sndrcvinfo->sinfo_cumtsn,
                         &sndrcvinfo->sinfo_assoc_id)!= 10)
        {
            ipcom_printf("     Failed, bad format.\
                               Format is {sinfo_stream,\
                                          sinfo_ssn,\
                                          sinfo_flags,\
                                          sinfo_pr_policy,\
                                          sinfo_ppid,\
                                          sinfo_context,\
                                          sinfo_pr_value,\
                                          sinfo_tsn,\
                                          sinfo_cumtsn,\
                                          sinfo_assoc_id}");

            ipcom_free(sndrcvinfo);
            return IP_NULL;
        }
    }

    return sndrcvinfo;
}
/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_default_send_param
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */

IP_STATIC char * ipcom_cmd_socktest_format_default_send_param(void * data)
{
    struct Ipsctp_sndrcvinfo *sndrcvinfo = data;
    char           *val;

    val = ipcom_calloc(1, 1000);
    if (val == IP_NULL)
        return val;
    ipcom_sprintf(val,"{%u,%u,%u,%u,%u,%u,%u,%u,%u,%u}",
                  sndrcvinfo->sinfo_stream,
                  sndrcvinfo->sinfo_ssn,
                  sndrcvinfo->sinfo_flags,
                  sndrcvinfo->sinfo_pr_policy,
                  sndrcvinfo->sinfo_ppid,
                  sndrcvinfo->sinfo_context,
                  sndrcvinfo->sinfo_pr_value,
                  sndrcvinfo->sinfo_tsn,
                  sndrcvinfo->sinfo_cumtsn,
                  sndrcvinfo->sinfo_assoc_id);
    return val;
}

/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_paddrinfo
 *===========================================================================
 * Description:READ-ONLY
 * Parameters:
 * Returns:
 *
 */

IP_STATIC void * ipcom_cmd_socktest_parse_paddrinfo(char *data, char *obj)
{
    struct Ipsctp_paddrinfo *paddrinfo;
    char *spinfo_assoc_id;
    char *paddr;
    char *arg_str = IP_NULL;
    char *p;

    paddrinfo = ipcom_calloc(1, sizeof(struct Ipsctp_paddrinfo));
    if (IP_NULL == paddrinfo)
    {
        return IP_NULL;
    }

    arg_str = ipcom_strdup(data);
    if (arg_str == IP_NULL)
        goto bad;

    spinfo_assoc_id = ipcom_strtok_r(arg_str, "\t{ ,", &p);
    paddr = ipcom_strtok_r(IP_NULL, "\t, }", &p);

    if (spinfo_assoc_id == IP_NULL || paddr == IP_NULL)
    {
        goto bad_format;
    }

    paddrinfo->spinfo_assoc_id = *spinfo_assoc_id;

    if (ipcom_cmd_socktest_ptos(paddr, &paddrinfo->spinfo_address) == IP_FALSE)
    {
        ipcom_printf("     Failed, bad address format");
        goto bad;
    }

    if (IP_FALSE == *obj)
    {
        if (ipcom_sscanf(p, "%u,%u,%u,%u,%u}",
                         &paddrinfo->spinfo_state,
                         &paddrinfo->spinfo_cwnd,
                         &paddrinfo->spinfo_srtt,
                         &paddrinfo->spinfo_rto,
                         &paddrinfo->spinfo_mtu)!= 5)
        {
            goto bad_format_set;
        }
    }

    ipcom_free(arg_str);
    return paddrinfo;

 bad_format:
    ipcom_printf("     Failed, bad format. Should be { spinfo_assoc_id, spinfo_address}");
    goto bad;
 bad_format_set:
    ipcom_printf("     Failed, bad format. Should be { spinfo_assoc_id, spinfo_address,\
                                                       spinfo_state, spinfo_cwnd,\
                                                       spinfo_srtt, spinfo_rto\
                                                       spinfo_mtu}");
 bad:
    ipcom_free(arg_str);
    ipcom_free(paddrinfo);
    return IP_NULL;
}

/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_paddrinfo
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */

IP_STATIC char * ipcom_cmd_socktest_format_paddrinfo(void * data)
{
    struct Ipsctp_paddrinfo *paddrinfo = data;
    char   *val;
    char   paddr[128];
    IP_CONST char   *tmp;
    struct Ip_sockaddr *sockaddr;

    val = ipcom_calloc(1, 1000);
    if (val == IP_NULL)
        return val;

    sockaddr = (struct Ip_sockaddr *)&paddrinfo->spinfo_address;
    tmp = ipcom_cmd_sockaddr_to_str(sockaddr, IP_FALSE, IP_TRUE, paddr, sizeof (paddr));
    if (IP_NULL == tmp)
    {
        ipcom_free(val);
        return IP_NULL;
    }

    ipcom_sprintf(val,"{%u,%s,%u,%u,%u,%u,%u}",
                  paddrinfo->spinfo_assoc_id,
                  paddr,
                  paddrinfo->spinfo_state,
                  paddrinfo->spinfo_cwnd,
                  paddrinfo->spinfo_srtt,
                  paddrinfo->spinfo_rto,
                  paddrinfo->spinfo_mtu);
    return val;
}

/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_primary_addr
 *===========================================================================
 * Description:RW
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void * ipcom_cmd_socktest_parse_primary_addr(char *data, char *obj)
{
    struct Ipsctp_setprim *setprim;
    char *arg_str = IP_NULL;
    char *ssp_assoc_id;
    char *paddr;
    char *p;

    setprim = ipcom_calloc(1, sizeof(struct Ipsctp_setprim));
    if (IP_NULL == setprim)
    {
        return IP_NULL;
    }

    arg_str = ipcom_strdup(data);
    if (arg_str == IP_NULL)
        goto bad;

    if (IP_TRUE == *obj)
    {
        setprim->ssp_assoc_id = ipcom_atoi(arg_str);
        return setprim;
    }

    ssp_assoc_id = ipcom_strtok_r(arg_str, "\t{ ,", &p);
    paddr = ipcom_strtok_r(IP_NULL, "\t }", &p);
    if (paddr == IP_NULL || ssp_assoc_id == IP_NULL)
    {
        goto bad_format;
    }
    setprim->ssp_assoc_id = *ssp_assoc_id;

    if (ipcom_cmd_socktest_ptos(paddr, &setprim->ssp_addr) == IP_FALSE)
    {
        ipcom_printf("     Failed, bad address format");
        goto bad;
    }
    ipcom_free(arg_str);
    return setprim;

bad_format:
    ipcom_printf("     Failed, bad format. Should be {ssp_assoc_id,ssp_addr}");

bad:
    ipcom_free(arg_str);
    ipcom_free(setprim);
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_primary_addr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char * ipcom_cmd_socktest_format_primary_addr(void * data)
{
    struct Ipsctp_setprim *setprim = data;
    char   *val;
    char   paddr[128];
    IP_CONST char   *tmp;
    struct Ip_sockaddr *sockaddr;

    val = ipcom_calloc(1, 1000);
    if (val == IP_NULL)
        return val;

    sockaddr = (struct Ip_sockaddr *)&setprim->ssp_addr;
    tmp = ipcom_cmd_sockaddr_to_str(sockaddr, IP_FALSE, IP_TRUE, paddr, sizeof (paddr));
    if (IP_NULL == tmp)
    {
        ipcom_free(val);
        return IP_NULL;
    }

    ipcom_sprintf(val,"{%u,%s}",
                  setprim->ssp_assoc_id,
                  paddr);

    return val;
}

/*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_peer_addr_params
 *===========================================================================
 * Description:RW
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void * ipcom_cmd_socktest_parse_peer_addr_params(char *data, char *obj)
{
    struct Ipsctp_paddrparams *paddrparams;
    char *spp_assoc_id;
    char *paddr;
    char *arg_str = IP_NULL;
    char *p;

    paddrparams = ipcom_calloc(1, sizeof(struct Ipsctp_paddrparams));
    if (IP_NULL == paddrparams)
    {
        return IP_NULL;
    }

    arg_str = ipcom_strdup(data);
    if (arg_str == IP_NULL)
        goto bad;

    spp_assoc_id = ipcom_strtok_r(arg_str, "\t{ ,", &p);
    paddr = ipcom_strtok_r(IP_NULL, "\t, }", &p);

    if (spp_assoc_id == IP_NULL || paddr == IP_NULL)
    {
        goto bad_format;
    }

    paddrparams->spp_assoc_id = *spp_assoc_id;

    if (ipcom_cmd_socktest_ptos(paddr, &paddrparams->spp_address) == IP_FALSE)
    {
        ipcom_printf("     Failed, bad address format");
        goto bad;
    }

    if (IP_FALSE == *obj)
    {
        if (ipcom_sscanf(p, "%u,%u,%u,%u,%u,%u}",
                         paddrparams->spp_hbinterval,
                         paddrparams->spp_pathmaxrxt,
                         paddrparams->spp_pathmtu,
                         paddrparams->spp_flags,
                         paddrparams->spp_ipv6_flowlabel,
                         paddrparams->spp_ipv4_tos)!= 5)
        {
            goto bad_format_set;
        }
    }

    ipcom_free(arg_str);
    return paddrparams;

 bad_format:
    ipcom_printf("     Failed, bad format. Should be { spinfo_assoc_id, spinfo_address}");
    goto bad;
 bad_format_set:
    ipcom_printf("     Failed, bad format. Should be { spinfo_assoc_id, spinfo_address,\
                                                       spp_hbinterval,spp_pathmaxrxt,\
                                                       spp_pathmtu,spp_flags,\
                                                       spp_ipv6_flowlabel,spp_ipv4_tos}");
 bad:
    ipcom_free(arg_str);
    ipcom_free(paddrparams);
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_peer_addr_params
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char * ipcom_cmd_socktest_format_peer_addr_params(void * data)
{
    struct Ipsctp_paddrparams *paddrparams = data;
    char   *val;
    char   paddr[128];
    IP_CONST char   *tmp;
    struct Ip_sockaddr *sockaddr;

    val = ipcom_calloc(1, 1000);
    if (val == IP_NULL)
        return val;

    sockaddr = (struct Ip_sockaddr *)&paddrparams->spp_address;
    tmp = ipcom_cmd_sockaddr_to_str(sockaddr, IP_FALSE, IP_TRUE, paddr, sizeof (paddr));
    if (IP_NULL == tmp)
    {
        ipcom_free(val);
        return IP_NULL;
    }

    ipcom_sprintf(val,"{%u,%s,%u,%u,%u,%u,%u,%u}",
                  paddrparams->spp_assoc_id,
                  paddr,
                  paddrparams->spp_hbinterval,
                  paddrparams->spp_pathmaxrxt,
                  paddrparams->spp_pathmtu,
                  paddrparams->spp_flags,
                  paddrparams->spp_ipv6_flowlabel,
                  paddrparams->spp_ipv4_tos);

    return val;
}

 /*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_status
 *===========================================================================
 * Description:READ ONLY
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void * ipcom_cmd_socktest_parse_status(char * data, char * obj)
{
    struct Ipsctp_status *status;
    (void) obj;

    status = ipcom_calloc(1, sizeof(struct Ipsctp_status));
    if (IP_NULL == status)
    {
        return IP_NULL;
    }
    if (data != IP_NULL)
    {
        status->sstat_assoc_id = ipcom_atoi(data);
    }
    return status;
}

 /*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_status
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char * ipcom_cmd_socktest_format_status(void * data)
{
    struct Ipsctp_status *status = data;
    char   *val;
    char   paddr[128];
    IP_CONST char   *tmp;
    struct Ip_sockaddr *sockaddr;
    struct Ipsctp_paddrinfo *paddrinfo;

    val = ipcom_calloc(1, 1000);
    if (val == IP_NULL)
     return val;

    paddrinfo = &status->sstat_primary;
    sockaddr = (struct Ip_sockaddr *)&paddrinfo->spinfo_address;
    tmp = ipcom_cmd_sockaddr_to_str(sockaddr, IP_FALSE, IP_TRUE, paddr, sizeof (paddr));
    if (IP_NULL == tmp)
    {
        ipcom_free(val);
        return IP_NULL;
    }

    ipcom_sprintf(val,"{%u,%u,%u,%u,%u,%u,%u,%u,{%u,%s,%u,%u,%u,%u,%u}}",
                  status->sstat_assoc_id,
                  status->sstat_state,
                  status->sstat_rwnd,
                  status->sstat_unackdata,
                  status->sstat_penddata,
                  status->sstat_instrms,
                  status->sstat_outstrms,
                  status->sstat_fragmentation_point,
                  paddrinfo->spinfo_assoc_id,
                  paddr,
                  paddrinfo->spinfo_state,
                  paddrinfo->spinfo_cwnd,
                  paddrinfo->spinfo_srtt,
                  paddrinfo->spinfo_rto,
                  paddrinfo->spinfo_mtu);

     return val;
}

 /*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_assval
 *===========================================================================
 * Description:RW
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void * ipcom_cmd_socktest_parse_assval(char * data, char * obj)
{
     struct Ipsctp_assoc_value *context;
     (void) obj;

     context = ipcom_calloc(1, sizeof(struct Ipsctp_assoc_value));
     if (IP_NULL == context)
     {
         return IP_NULL;
     }

     if (IP_TRUE == *obj)
     {
         context->assoc_id = ipcom_atoi(data);
         return context;
     }

     if (ipcom_sscanf(data, "{%u,%u}",
                     &context->assoc_id,
                     &context->assoc_value) != 2)
     {
         ipcom_printf("     Failed, bad format. Should be {assoc_id, assoc_value}");

     }
     return context;
}

 /*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_assval
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char * ipcom_cmd_socktest_format_assval(void * data)
{
    char   *val;
    struct Ipsctp_assoc_value *context = data;

    val = ipcom_calloc(1, 1000);
    if (val == IP_NULL)
    {
        return val;
    }
    ipcom_sprintf(val,
                  "{%u,%u}",
                  context->assoc_id,
                  context->assoc_value);
    return val;
}

 /*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_event
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */

IP_STATIC void * ipcom_cmd_socktest_parse_event(char * data, char * obj)
{
    struct Ipsctp_event_subscribe *event;

    event = ipcom_calloc(1, sizeof(struct Ipsctp_event_subscribe));
    if (IP_NULL == event)
    {
        return IP_NULL;
    }
    if (IP_TRUE == *obj)
    {
        return event;
    }
    if (ipcom_sscanf(data, "{%u,%u,%u,%u,%u,%u,%u,%u,%u}",
                     &event->Ipsctp_data_io_event,
                     &event->Ipsctp_association_event,
                     &event->Ipsctp_address_event,
                     &event->Ipsctp_send_failure_event,
                     &event->Ipsctp_peer_error_event,
                     &event->Ipsctp_shutdown_event,
                     &event->Ipsctp_partial_delivery_event,
                     &event->Ipsctp_adaptation_layer_event,
                     &event->Ipsctp_authentication_event)!= 10)
    {
        ipcom_printf("     Failed, bad format.\
                           Format is {data_io_event,\
                                      association_event,\
                                      address_event,\
                                      send_failure_event,\
                                      peer_error_event,\
                                      shutdown_event,\
                                      partial_delivery_event,\
                                      adaptation_layer_event,\
                                      authentication_event");

        ipcom_free(event);
        return IP_NULL;
    }
    return event;
}

 /*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_event
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char * ipcom_cmd_socktest_format_event(void * data)
{
    struct Ipsctp_event_subscribe *event = data;
    char   *val;

    val = ipcom_calloc(1, 1000);
    if (val == IP_NULL)
    {
        return val;
    }
    ipcom_sprintf(val,
                  "{%u,%u,%u,%u,%u,%u,%u,%u,%u}",
                  event->Ipsctp_data_io_event,
                  event->Ipsctp_association_event,
                  event->Ipsctp_address_event,
                  event->Ipsctp_send_failure_event,
                  event->Ipsctp_peer_error_event,
                  event->Ipsctp_shutdown_event,
                  event->Ipsctp_partial_delivery_event,
                  event->Ipsctp_adaptation_layer_event,
                  event->Ipsctp_authentication_event);
    return val;
}


 /*
 *===========================================================================
 *                    ipcom_cmd_socktest_parse_initmsg
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */

 IP_STATIC void * ipcom_cmd_socktest_parse_initmsg(char * data, char * obj)
 {
    struct Ipsctp_initmsg *initmsg;

    initmsg = ipcom_calloc(1, sizeof(struct Ipsctp_initmsg));
    if (IP_NULL == initmsg)
    {
        return IP_NULL;
    }
    if (IP_TRUE == *obj)
    {
        return initmsg;
    }
    if (ipcom_sscanf(data, "{%u,%u,%u,%u}",
                     initmsg->sinit_num_ostreams,
                     initmsg->sinit_max_instreams,
                     initmsg->sinit_max_attempts,
                     initmsg->sinit_max_init_timeo)!= 4)
    {
        ipcom_printf("     Failed, bad format.\
                           Format is {sinit_num_ostreams,\
                           sinit_max_instreams,\
                           sinit_max_attempts,\
                           sinit_max_init_timeo");

        ipcom_free(initmsg);
        return IP_NULL;
    }
    return initmsg;
 }
 /*
 *===========================================================================
 *                    ipcom_cmd_socktest_format_initmsg
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char * ipcom_cmd_socktest_format_initmsg(void * data)
{
     struct Ipsctp_initmsg *initmsg = data;
     char   *val;

     val = ipcom_calloc(1, 1000);
     if (val == IP_NULL)
     {
         return val;
     }
     ipcom_sprintf(val,
                   "{%u,%u,%u,%u}",
                   initmsg->sinit_num_ostreams,
                   initmsg->sinit_max_instreams,
                   initmsg->sinit_max_attempts,
                   initmsg->sinit_max_init_timeo);
     return val;
}
#endif

/*
 *===========================================================================
 *                    ipcom_cmd_socktest_portinfo_add
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_portinfo_add(int domain, struct Ip_sockaddr *sa, char * p_port)
{
    Ip_err retval = IPCOM_SUCCESS;
    Ip_u16 port;

    if (p_port != IP_NULL)
    {
        port = (Ip_u16) ipcom_atoi (p_port);

        if (domain == IP_AF_INET)
        {
            struct Ip_sockaddr_in *p_sin = (struct Ip_sockaddr_in*)sa;

            p_sin->sin_port = ip_htons (port);
        }
        else if (domain == IP_AF_INET6)
        {
            struct Ip_sockaddr_in6 *p_sin6 = (struct Ip_sockaddr_in6*)sa;

            p_sin6->sin6_port = ip_htons (port);
        }
        else
        {
            ipcom_printf("     Failed, port argument not applicable for socket type"IP_LF);
            retval = IPCOM_ERR_INVALID_ARG;
        }
    }

    return retval;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_print_eth
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_u8 *
ipcom_cmd_socktest_print_eth(Ip_u8 * data)
{
    Ipcom_cmd_socktest_eth_hdr *hdr = (Ipcom_cmd_socktest_eth_hdr *)data;

    ipcom_printf("     ETHERNET HEADER"IP_LF);
    ipcom_printf("     Destination : 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x"IP_LF,
                  hdr->dst[0],
                  hdr->dst[1],
                  hdr->dst[2],
                  hdr->dst[3],
                  hdr->dst[4],
                  hdr->dst[5]);
    ipcom_printf("     Source      : 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x"IP_LF,
                  hdr->src[0],
                  hdr->src[1],
                  hdr->src[2],
                  hdr->src[3],
                  hdr->src[4],
                  hdr->src[5]);
    ipcom_printf("     Type        : 0x%04x"IP_LF,ip_ntohs(hdr->type));

    return data + sizeof (Ipcom_cmd_socktest_eth_hdr);
}

#ifdef IPCOM_USE_INET
/*
 *===========================================================================
 *                    ipcom_cmd_socktest_print_ip
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_u8 *
ipcom_cmd_socktest_print_ip(Ip_u8 * data)
{
    Ipcom_ip_header *hdr = (Ipcom_ip_header*)data;
    char buf[20];
    int ip_options_len;

    ipcom_printf("     IPv4 HEADER"IP_LF);
    ipcom_printf("     VHL         : 0x%x"IP_LF, hdr->vers_len);
    ipcom_printf("     TOS         : 0x%x"IP_LF, hdr->tos);
    ipcom_printf("     Total len   : 0x%x"IP_LF, ip_ntohs (hdr->length));
    ipcom_printf("     ID          : 0x%x"IP_LF, ip_ntohs (hdr->id));
    ipcom_printf("     Fragm offs  : 0x%x"IP_LF, ip_ntohs (hdr->frag));
    ipcom_printf("     TTL         : 0x%x"IP_LF, hdr->ttl);
    ipcom_printf("     Proto       : 0x%x"IP_LF, hdr->protocol);
    ipcom_printf("     Checksum    : 0x%x"IP_LF, ip_ntohs (hdr->checksum));

    (void)ipcom_inet_ntop (IP_AF_INET, &hdr->src, buf, sizeof (buf));
    ipcom_printf("     Source      : %s"IP_LF,buf);

    (void)ipcom_inet_ntop (IP_AF_INET, &hdr->dst, buf, sizeof (buf));
    ipcom_printf("     Destination : %s"IP_LF,buf);

    ip_options_len = ((hdr->vers_len & 0xf) << 2) - sizeof(Ipcom_ip_header);
    if (ip_options_len)
    {
        int i;

        ipcom_printf("     IP options  : ");
        for (i = 0; i < ip_options_len; i++)
            ipcom_printf("0x%02x ", ((Ip_u8 *)(hdr + 1))[i]);
        ipcom_printf(IP_LF);
    }

    return data + ((hdr->vers_len & 0xf) << 2);
}
#endif /* IPCOM_USE_INET */


#ifdef IPCOM_USE_INET6
/*
 *===========================================================================
 *                    ipcom_cmd_socktest_print_ip6
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_u8 *
ipcom_cmd_socktest_print_ip6(Ip_u8 * data)
{
    Ipcom_ip6_header *hdr = (Ipcom_ip6_header*)data;
    char buf[50];

    ipcom_printf("     IPv6 HEADER"IP_LF);
    ipcom_printf("     VCF         : 0x%lx"IP_LF, ip_ntohl(hdr->ver_class_flow));
    ipcom_printf("     Length      : 0x%x"IP_LF, ip_ntohs(hdr->plen));
    ipcom_printf("     Next Header : 0x%x"IP_LF, hdr->nxt);
    ipcom_printf("     Hop limit   : 0x%x"IP_LF, hdr->hlim);

    (void)ipcom_inet_ntop (IP_AF_INET6, &hdr->src, buf, sizeof (buf));
    ipcom_printf("     Source      : %s"IP_LF, buf);

    (void)ipcom_inet_ntop (IP_AF_INET6, &hdr->dst, buf, sizeof (buf));
    ipcom_printf("     Destination : %s"IP_LF, buf);

    return data + sizeof (Ipcom_ip6_header);
}
#endif /* IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_print_tcp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_u8 *
ipcom_cmd_socktest_print_tcp(Ip_u8 * data, Ip_size_t alen)
{
    Ipcom_tcp_hdr *hdr = (Ipcom_tcp_hdr *)data;
    Ip_u16 len;
    Ip_u16 flags;
    Ip_u16 i;
    Ip_u32 num_n;

    (void)alen;
    flags = ip_ntohs (hdr->offset_flags);
    len   = (Ip_u16) ((flags & 0xf000) >> 12);
    flags = (flags & 0x002f);

    ipcom_printf("     TCP HEADER"IP_LF);
    ipcom_printf("     Source port : %d"IP_LF, ip_ntohs(hdr->sport));
    ipcom_printf("     Dest port   : %d"IP_LF, ip_ntohs(hdr->dport));
    ipcom_memcpy(&num_n, hdr->seq_num, sizeof(num_n));
    ipcom_printf("     Seq. no     : %ld"IP_LF, ip_ntohl(num_n));
    ipcom_memcpy(&num_n, hdr->ack_num, sizeof(num_n));
    ipcom_printf("     Ack. no     : %ld"IP_LF, ip_ntohl(num_n));
    ipcom_printf("     Header len  : %d"IP_LF, len);
    ipcom_printf("     Flags       : 0x%02x"IP_LF, flags);
    ipcom_printf("     Window      : %d"IP_LF, ip_ntohs (hdr->window));
    ipcom_printf("     Urgent Ptr  : 0x%x"IP_LF, ip_ntohs (hdr->urgent_pointer));

    if (len > 20)
        for (i = 0; i < (len - 20); i++)
            ipcom_printf("     Option[%d]  : 0x%x"IP_LF, i, ip_ntohs(hdr->options[i]));

    return data + (Ip_size_t)len;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_print_udp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_u8 *
ipcom_cmd_socktest_print_udp(Ip_u8 * data, Ip_size_t len)
{
    Ipcom_udp_header *hdr = (Ipcom_udp_header*)data;

    (void) len;
    ipcom_printf("     UDP HEADER"IP_LF);
    ipcom_printf("     Source port : %d"IP_LF, ip_ntohs (hdr->src_port));
    ipcom_printf("     Dest port   : %d"IP_LF, ip_ntohs (hdr->dst_port));
    ipcom_printf("     Length      : %d"IP_LF, ip_ntohs (hdr->length));
    ipcom_printf("     Checksum    : 0x%x"IP_LF, ip_ntohs (hdr->checksum));

    return data + sizeof (Ipcom_udp_header);
}

#ifdef IPSCTP
/*
 *===========================================================================
 *                    ipcom_cmd_socktest_print_sctp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_u8 *
ipcom_cmd_socktest_print_sctp(Ip_u8 * data, Ip_size_t len)
{
    Ipcom_sctp_header *hdr = (Ipcom_sctp_header *)data;

    (void) len;
    ipcom_printf("     SCTP HEADER"IP_LF);
    ipcom_printf("     Source port : %d"IP_LF, ip_ntohs (hdr->sport));
    ipcom_printf("     Dest port   : %d"IP_LF, ip_ntohs (hdr->dport));
    ipcom_printf("     Vtag        : %d"IP_LF, ip_ntohs (hdr->vtag));
    ipcom_printf("     Crc         : 0x%x"IP_LF, ip_ntohs (hdr->crc));

    return data + sizeof (Ipcom_sctp_header);
}
#endif

/*
 *===========================================================================
 *                    ipcom_cmd_socktest_rec_type_to_str
 *===========================================================================
 * Description: Returns a string representation of the record type
 * Parameters:  rec_type - A record type
 * Returns:     A string representation.
 *
 */
IP_STATIC IP_CONST char *
ipcom_cmd_socktest_rec_type_to_str(Ip_u8 rec_type)
{
    switch (rec_type)
    {
    case 1:
        return "MODE_IS_INCLUDE";
    case 2:
        return "MODE_IS_EXCLUDE";
    case 3:
        return "CHANGE_TO_INCLUDE_MODE";
    case 4:
        return "CHANGE_TO_EXCLUDE_MODE";
    case 5:
        return "ALLOW_NEW_SOURCES";
    case 6:
        return "BLOCK_OLD_SOURCES";
    default:
        return "*** invalid record type";
    }
}


#ifdef IPCOM_USE_INET
/*
 *===========================================================================
 *                    ipcom_cmd_socktest_print_icmp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_u8 *
ipcom_cmd_socktest_print_icmp(Ip_u8 * data, Ip_size_t len)
{
    Ipcom_icmp_header *hdr = (Ipcom_icmp_header*)data;

    (void) len;
    ipcom_printf("     ICMP HEADER"IP_LF);
    ipcom_printf("     Type        : 0x%02x"IP_LF, hdr->type);
    ipcom_printf("     Code        : 0x%02x"IP_LF, hdr->code);
    ipcom_printf("     Checksum    : 0x%02x"IP_LF, ip_ntohs (hdr->checksum));

    ipcom_printf("     ICMP MESSAGE"IP_LF);

    if ((hdr->type == 0) || (hdr->type == 8))
    {
        ipcom_printf("     ID          : 0x%x"IP_LF, hdr->data.echo.id);
        ipcom_printf("     Sequence    : %d"IP_LF, hdr->data.echo.seq);
        ipcom_printf("     Data        : 0x%lx"IP_LF, IP_GET_NTOHL(hdr->data.echo.data)); /*lint !e647 */
    }
    else if (hdr->type == 5)
    {
        char buf[20];

        (void)ipcom_inet_ntop (IP_AF_INET,
                         &hdr->data.redirect.gateway_addr,
                         buf,
                         sizeof (buf));

        ipcom_printf("     Router addr   : %s"IP_LF,buf);
        ipcom_printf("     EMBEDDED ");
        (void)ipcom_cmd_socktest_print_ip ((Ip_u8 *)hdr->data.redirect.ip);
        ipcom_printf("     Transp hdr    : 0x%04x 0x%04x 0x%04x 0x%04x"IP_LF,
                      ip_ntohs (hdr->data.redirect.transport_hdr[0]),
                      ip_ntohs (hdr->data.redirect.transport_hdr[1]),
                      ip_ntohs (hdr->data.redirect.transport_hdr[2]),
                      ip_ntohs (hdr->data.redirect.transport_hdr[3]));
    }
    else if (hdr->type == 3)
    {
        ipcom_printf("     EMBEDDED ");
        (void)ipcom_cmd_socktest_print_ip ((Ip_u8 *)hdr->data.failing_pkt.ip);
        ipcom_printf("     Transp hdr    : 0x%04x 0x%04x 0x%04x 0x%04x"IP_LF,
                      ip_ntohs (hdr->data.failing_pkt.transport_hdr[0]),
                      ip_ntohs (hdr->data.failing_pkt.transport_hdr[1]),
                      ip_ntohs (hdr->data.failing_pkt.transport_hdr[2]),
                      ip_ntohs (hdr->data.failing_pkt.transport_hdr[3]));
    }
    else
    {
        ipcom_printf("     Could not parse data"IP_LF);
    }

    return data + sizeof (Ipcom_icmp_header);
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_print_igmp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_u8 *
ipcom_cmd_socktest_print_igmp(Ip_u8 * data, Ip_size_t len)
{
    int  version;
    int  i;
    char b[IP_INET_ADDRSTRLEN];
    Ipcom_socktest_igmp_hdr *hdr = (Ipcom_socktest_igmp_hdr*)data;

    ipcom_printf("     IGMP HEADER"IP_LF);

    switch (hdr->m.type)
    {
    case 0x11:
        if (len > 8)
            version = 3;
        else if (hdr->m.code)
            version = 2;
        else
            version = 1;
        ipcom_printf("     Type        : Membership Query v%d"IP_LF, version);
        ipcom_printf("     Max resp    : 0x%02x"IP_LF, hdr->m.code);
        (void)ipcom_inet_ntop(IP_AF_INET, &hdr->m.maddr, b, sizeof(b));
        ipcom_printf("     Group       : %s"IP_LF, b);
        if (version == 3)
        {
            ipcom_printf("     S flag      : %s"IP_LF, (hdr->query_v3.s_qrv & 0x08) ? "on" : "off");
            ipcom_printf("     QRV         : 0x%02x"IP_LF, hdr->query_v3.s_qrv & 0x07);
            ipcom_printf("     QQIC        : 0x%02x"IP_LF, hdr->query_v3.qqic);
            ipcom_printf("     N           : 0x%04x"IP_LF, ip_ntohs(hdr->query_v3.number_of_sources));
            for (i = 0; i < ip_ntohs(hdr->query_v3.number_of_sources); i++)
            {
                (void)ipcom_inet_ntop(IP_AF_INET, &hdr->query_v3.saddr[i], b, sizeof(b));
                ipcom_printf("     Source#%02d   : %s"IP_LF, i, b);
            }
        }
        break;
    case 0x12:
        ipcom_printf("     Type        : Membership Report v1"IP_LF);
        (void)ipcom_inet_ntop(IP_AF_INET, &hdr->m.maddr, b, sizeof(b));
        ipcom_printf("     Group       : %s"IP_LF, b);
        break;
    case 0x16:
        ipcom_printf("     Type        : Membership Report v2"IP_LF);
        (void)ipcom_inet_ntop(IP_AF_INET, &hdr->m.maddr, b, sizeof(b));
        ipcom_printf("     Group       : %s"IP_LF, b);
        break;
    case 0x17:
        ipcom_printf("     Type        : Leave Group v2"IP_LF);
        (void)ipcom_inet_ntop(IP_AF_INET, &hdr->m.maddr, b, sizeof(b));
        ipcom_printf("     Group       : %s"IP_LF, b);
        break;
    case 0x22:
        ipcom_printf("     Type        : Membership Report v3"IP_LF);
        for (i = 0; i < ip_ntohs(hdr->report_v3.number_of_group_records); i++)
        {
            struct Ipcom_socktest_group_rec_struct *rec = &hdr->report_v3.group_rec[i];
            int j;

            ipcom_printf("     Record type : %s"IP_LF,
                         ipcom_cmd_socktest_rec_type_to_str(rec->record_type));
            ipcom_printf("     Aux len     : %#02x"IP_LF, rec->aux_data_len);
            (void)ipcom_inet_ntop(IP_AF_INET, &rec->maddr, b, sizeof(b));
            ipcom_printf("     Group       : %s"IP_LF, b);
            for (j = 0; j < ip_ntohs(rec->number_of_sources); j++)
            {
                (void)ipcom_inet_ntop(IP_AF_INET, &rec->saddr[j], b, sizeof(b));
                ipcom_printf("     Source#%02d   : %s"IP_LF, j, b);
            }
        }
        break;
    default:
        ipcom_printf("     Type        : 0x%02x"IP_LF, hdr->m.type);
        break;
    }

    return data + len;
}
#endif /* IPCOM_USE_INET */


#ifdef IPCOM_USE_INET6
/*
 *===========================================================================
 *                    ipcom_cmd_socktest_print_icmp6
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_u8 *
ipcom_cmd_socktest_print_icmp6(Ip_u8 * data, Ip_size_t len)
{
    int  version;
    int  i, j;
    char b[IP_INET6_ADDRSTRLEN];
    Ipcom_socktest_mld_hdr *hdr = (Ipcom_socktest_mld_hdr*)data;
    Ipcom_icmp6_hdr *ghdr = (Ipcom_icmp6_hdr *)data;

    ipcom_printf("     ICMPv6 HEADER"IP_LF);
    switch (hdr->m.type)
    {
    case 130:
        version = (len > 24 ? 2 : 1);
        ipcom_printf("     Type        : Membership Query v%d"IP_LF, version);
        ipcom_printf("     Max resp    : 0x%04x"IP_LF, ip_ntohs(hdr->query_v2.max_resp_delay));
        (void)ipcom_inet_ntop(IP_AF_INET6, &hdr->query_v2.maddr, b, sizeof(b));
        ipcom_printf("     Group       : %s"IP_LF, b);
        if (version == 2)
        {
            ipcom_printf("     S flag      : %s"IP_LF, (hdr->query_v2.s_qrv & 0x08) ? "on" : "off");
            ipcom_printf("     QRV         : 0x%02x"IP_LF, hdr->query_v2.s_qrv & 0x07);
            ipcom_printf("     QQIC        : 0x%02x"IP_LF, hdr->query_v2.qqic);
            ipcom_printf("     N           : 0x%04x"IP_LF, ip_ntohs(hdr->query_v2.number_of_sources));
            for (i = 0; i < ip_ntohs(hdr->query_v2.number_of_sources); i++)
            {
                (void)ipcom_inet_ntop(IP_AF_INET6, &hdr->query_v2.saddr[i], b, sizeof(b));
                ipcom_printf("     Source#%02d   : %s"IP_LF, i, b);
            }
        }
        break;
    case 131:
        ipcom_printf("     Type        : Membership Report v1"IP_LF);
        (void)ipcom_inet_ntop(IP_AF_INET6, &hdr->m.maddr, b, sizeof(b));
        ipcom_printf("     Group       : %s"IP_LF, b);
        break;
    case 132:
        ipcom_printf("     Type        : Leave Group"IP_LF);
        (void)ipcom_inet_ntop(IP_AF_INET6, &hdr->m.maddr, b, sizeof(b));
        ipcom_printf("     Group       : %s"IP_LF, b);
        break;
    case 143:
        ipcom_printf("     Type        : Membership Report v2"IP_LF);
        for (i = 0; i < ip_ntohs(hdr->report_v2.number_of_maddr_records); i++)
        {
            struct Ipcom_socktest_maddr_rec_struct *rec = &hdr->report_v2.mcast_rec[i];

            ipcom_printf("     Record type : %s"IP_LF,
                         ipcom_cmd_socktest_rec_type_to_str(rec->record_type));
            ipcom_printf("     Aux len     : %#02x"IP_LF, rec->aux_data_len);
            (void)ipcom_inet_ntop(IP_AF_INET6, &rec->maddr, b, sizeof(b));
            ipcom_printf("     Group       : %s"IP_LF, b);
            for (j = 0; j < ip_ntohs(rec->number_of_sources); j++)
            {
                (void)ipcom_inet_ntop(IP_AF_INET6, &rec->saddr[j], b, sizeof(b));
                ipcom_printf("     Source#%02d   : %s"IP_LF, j, b);
            }
        }
        break;
    case 135: /* Neighbor solicitation */
        ipcom_printf("     Type        : Neighbor Solicitation"IP_LF);
        ipcom_printf("     Target      : %s"IP_LF,
                     ipcom_inet_ntop(IP_AF_INET6, ghdr + 1, b, sizeof(b)));
        break;
    default:
        ipcom_printf("     Type        : 0x%02x"IP_LF, ghdr->icmp6_type);
        ipcom_printf("     Code        : 0x%02x"IP_LF, ghdr->icmp6_code);
        ipcom_printf("     Checksum    : 0x%02x"IP_LF, ip_ntohs (ghdr->icmp6_cksum));
        ipcom_printf("     Data        : 0x%02x 0x%02x 0x%02x 0x%02x"IP_LF,
                     ghdr->icmp6_dataun.icmp6_un_data8[0],
                     ghdr->icmp6_dataun.icmp6_un_data8[1],
                     ghdr->icmp6_dataun.icmp6_un_data8[2],
                     ghdr->icmp6_dataun.icmp6_un_data8[3]);
        break;
    }

    return data + len;
}
#endif /* IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_print_formatted
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_cmd_socktest_print_formatted(Ip_u8 *data,
                                   Ip_size_t len,
                                   Ipcom_cmd_socktest_fd *fd)
{
    Ip_size_t i;
    Ip_size_t j;
    Ip_u8    *start;

    for (i = 0; i < SOCKTEST_PROTO_ENTRIES; i++)
        if ((fd->domain == socktest_prot_format[i].domain) &&
            (fd->type   == socktest_prot_format[i].type) &&
            (fd->prot   == socktest_prot_format[i].prot))
            break;


    if (i < SOCKTEST_PROTO_ENTRIES)
    {
        start = data;

        if (socktest_prot_format[i].ll_format_cb != IP_NULL)
        {
            /* Print Link Layer Header */
            data = socktest_prot_format[i].ll_format_cb(data);
        }

        if (socktest_prot_format[i].nl_format_cb != IP_NULL)
        {
            /* Print Network Layer Header */
            data = socktest_prot_format[i].nl_format_cb(data);
        }

        len -= IP_MIN((Ip_size_t)((Ip_ptrdiff_t)data - (Ip_ptrdiff_t)start), len);
        start = data;

        if (socktest_prot_format[i].tl_format_cb != IP_NULL)
        {
            /* Print Transport Layer Header */
            data = socktest_prot_format[i].tl_format_cb(data, len);
        }

        len -= IP_MIN((Ip_size_t)((Ip_ptrdiff_t)data - (Ip_ptrdiff_t)start), len);
    }

    if (len > 0)
    {
        ipcom_printf("     PAYLOAD");

        for (i = 0; i < len; i += 14)
        {
            ipcom_printf(IP_LF"     ");
            for (j = 0; ((j < 14) && (j + i) < len); j++)
                ipcom_printf("0x%02x ", data[i+j]);
        }
        ipcom_printf(IP_LF);
    }
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_console
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_console(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *unused, int unused2, char **unused3)
{
    char                  command[512];
    Ipcom_cmd_socktest_fd local_socktest_fd[SOCKTEST_MAX_FD];
    Ipcom_getline        *gl;
    Ip_size_t             i;
    Ip_err                retval;
    int                   argc, offset;
    char                **argv = IP_NULL;

    (void)unused;
    (void)unused2;
    (void)unused3;

    /* Use a brand new set of local descriptor place holders. */
    ipcom_memset(local_socktest_fd, -1, sizeof(socktest_fd));

    gl = ipcom_getline_init(IPCOM_SHELL_HISTORY_LENGTH);
    if (gl == IP_NULL)
    {
        ipcom_printf("out of readline memory."IP_LF);
        return IPCOM_SUCCESS;
    }

    /* Command loop. */
    for (;glob->exit == 0;)
    {
        /* Print prompt. */
        (void)ipcom_fflush(ip_stdout);
        ipcom_printf("socktest> ");
        (void)ipcom_fflush(ip_stdout);

        /* Get command line. */
        if (ipcom_getline(IP_INVALID_SOCKET, command, sizeof(command), IP_TRUE, gl, 10 /* promptlen */) < 0)
        {
            ipcom_printf("end of line, aborting."IP_LF);
            break;
        }

        if (!*command)
            continue;

        argc = 0;
        if (argv != IP_NULL)
        {
            ipcom_free(argv);
            argv = IP_NULL;
        }

        retval = ipcom_parse_argstr(command, &argc, &argv);
        if (retval != IPCOM_SUCCESS)
        {
            ipcom_printf("argument error."IP_LF);
            continue;
        }

        if (argc == 0 || argv == IP_NULL)
            continue;

        if (ipcom_strcmp(argv[0], "socktest") == 0)
            offset = 1;
        else
            offset = 0;

        if (argc - offset <= 0)
        {
            ipcom_cmd_socktest_print_usage();
            continue;
        }

        for (i = 0; i < SOCKTEST_MAX_SUBCMD; i++)
        {
            if (ipcom_strcmp(argv[offset], socktest_cmd_cb[i].subcmd) == 0)
            {
                retval = socktest_cmd_cb[i].cmd_cb(glob, local_socktest_fd, argc - offset, &argv[offset]);
                break;
            }
        }

        if (i == SOCKTEST_MAX_SUBCMD)
        {
            ipcom_cmd_socktest_print_usage();
            continue;
        }

        if (retval == IPCOM_ERR_FAILED)
            ipcom_printf("     Failed, errno: %s" IP_LF, ipcom_strerror(ipcom_errno));
        else if (retval == IPCOM_ERR_NO_MEMORY)
            ipcom_printf("     Failed, out of memory."IP_LF);
        ipcom_printf(IP_LF);

        /* FREE argv?? */
    }

    ipcom_getline_exit(gl);
    if (argv != IP_NULL)
        ipcom_free(argv);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_exit
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_exit(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    (void)fdp;
    (void)argc;
    (void)argv;
    glob->exit = 1;
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_get_sockno
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_cmd_socktest_get_sockno(char *value)
{
    char  sysbuf[128];

    ipcom_sprintf(sysbuf, "ipcom.socktest.socket.%s", value);

    return ipcom_sysvar_get_as_int(sysbuf, ipcom_atoi(value));
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_open
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_open(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    Ip_err             retval;
    Ipcom_getopt       o;
    Ipcom_cmd_socktest_fd *f;
    int                domain = -1;
    int                type   = -1;
    int                proto  = 0;
    int                fd;
    int                opt;
    char               sysbuf[128];
    char               intbuf[12];

    (void)glob;
    sysbuf[0] = '\0';

    if (argc == 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "d:t:p:N:s:", &o)) != -1)
    {
        switch (opt)
        {
        case 'd':
            domain = ipcom_cmd_socktest_param_to_int(o.optarg,
                                                    socktest_domain,
                                                    SOCKTEST_PARAM_ENTRIES(socktest_domain));
            break;

        case 't':
            type   = ipcom_cmd_socktest_param_to_int(o.optarg,
                                                     socktest_type,
                                                     SOCKTEST_PARAM_ENTRIES(socktest_type));
            break;

        case 'p':
            proto  = ipcom_cmd_socktest_param_to_int(o.optarg,
                                                     socktest_proto,
                                                     SOCKTEST_PARAM_ENTRIES(socktest_proto));
            break;

        case 'N':
            break;

        case 's':
            ipcom_sprintf(sysbuf, "ipcom.socktest.socket.%s", o.optarg);
            break;

        default:
            goto print_help;
        }
    }

    /* Get free fd entry */
    f = ipcom_cmd_socktest_fd_find(fdp, -1);
    if (f != IP_NULL)
    {
        fd = ipcom_socket(domain,
                          type,
                          domain == IP_AF_PACKET ? ip_ntohs(proto) : proto);
        if (fd >= 0)
        {
#ifdef IPCOM_USE_FD_DAEMON
            f->fd        = ipcom_fd_daemon_translate_new_fd(fd);
#else
            f->fd        = fd;
#endif
            f->domain    = domain;
            f->type      = type;
            f->prot      = proto;
            f->event_sem = IPCOM_SEM_INVALID;
            SOCKTEST_FLOOD_INIT(f);

#ifndef IPCOM_USE_FD_DAEMON
#if defined(IP_PORT_OSE) || defined(IP_PORT_OSE5)
            /* Detach socket from creating process */
            {
                Ip_pid_t pid = 0;
                if (ipcom_socketioctl(fd, IP_SIOCSPGRP, &pid) < 0)
                {
                    ipcom_printf("     ipcom_socketioctl(IP_SIOCSPGRP,0) failed : %s", ipcom_strerror(ipcom_errno));
                }
            }
#endif
#endif
        }
        else
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
    }
    else
    {
        ipcom_printf("     Failed, socket descriptor list full."IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_CONFIG);
    }

    ipcom_printf("     Socket = %d"IP_LF, f->fd);
    retval = IPCOM_SUCCESS;
    ipcom_sprintf(intbuf, "%d", f->fd);
    (void)ipcom_sysvar_set(sysbuf, intbuf, IPCOM_SYSVAR_FLAG_OVERWRITE);

leave:
    return retval;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest open -d <domain> -t <type> -p <protocol>"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -d <domain>"IP_LF);
    ipcom_cmd_socktest_param_list_print(socktest_domain, SOCKTEST_PARAM_ENTRIES(socktest_domain), sizeof(Ipcom_cmd_socktest_param));
    ipcom_printf(IP_LF);
    ipcom_printf("     -t <type>"IP_LF);
    ipcom_cmd_socktest_param_list_print(socktest_type, SOCKTEST_PARAM_ENTRIES(socktest_type), sizeof(Ipcom_cmd_socktest_param));
    ipcom_printf(IP_LF);
    ipcom_printf("     -p <protocol>"IP_LF);
    ipcom_cmd_socktest_param_list_print(socktest_proto, SOCKTEST_PARAM_ENTRIES(socktest_proto), sizeof(Ipcom_cmd_socktest_param));
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_close
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_close(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    Ip_err             retval;
    Ipcom_getopt       o;
    Ipcom_cmd_socktest_fd *f;
    int                sockno = -1;
    int                opt;
    int                ret;
    Ip_bool            close_forced = IP_FALSE;
    int fd;

    (void)glob;

    if (argc == 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "s:fN:", &o)) != -1)
    {
        switch (opt)
        {
        case 's':
            sockno  = ipcom_cmd_socktest_get_sockno(o.optarg);
            break;
        case 'f':
            close_forced = IP_TRUE;
            break;
        case 'N':
            break;
        default:
            goto print_help;
        }
    }

    if (sockno < 0)
    {
        ipcom_printf("     Failed, no socket specified"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    /* Get fd entry */
    f = ipcom_cmd_socktest_fd_find(fdp, sockno);

#ifdef IPCOM_USE_FD_DAEMON
    fd = ipcom_fd_daemon_close_local_fd(sockno);
#else
    fd = sockno;
#endif

    if (f != IP_NULL)
    {
        if (f->flood_flags)
        {
            ipcom_errno = IP_ERRNO_EBUSY;
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
        }

        ret = ipcom_socketclose(fd);
        while (f->event_sem != IPCOM_SEM_INVALID)
            ipcom_sleep(1);
        f->fd = -1;

        if (ret < 0)
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
    }
    else if (close_forced == IP_TRUE)
    {
        ret = ipcom_socketclose (fd);

        if (ret < 0)
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
    }
    else
    {
        ipcom_printf("     Failed, bad socket number");
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    ipcom_printf("     Socket %d closed"IP_LF, sockno);
    retval = IPCOM_SUCCESS;

leave:
    return retval;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest close -s <socket> -f"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -s specify socket number"IP_LF);
    ipcom_printf("     -f close forced"IP_LF);
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_flush
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_flush(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    int                     i;
    int                     fd;

    (void)glob;
    (void)argv;
    (void)argc;

    for (i = 0; i < SOCKTEST_MAX_FD; i++)
        if (fdp[i].fd != IP_INVALID_SOCKET)
        {
            if (fdp[i].flood_flags)
                ipcom_cmd_socktest_flood_closesocket_impl(glob, &fdp[i]);

#ifdef IPCOM_USE_FD_DAEMON
            fd = ipcom_fd_daemon_close_local_fd(fdp[i].fd);
#else
            fd = fdp[i].fd;
#endif

            ipcom_socketclose(fd);
            while (fdp[i].event_sem != IPCOM_SEM_INVALID)
                ipcom_sleep(1);
            ipcom_printf("     Socket %d closed"IP_LF, fdp[i].fd);
            fdp[i].fd = -1;
        }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_bind
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_bind(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    Ip_err                          retval;
    Ipcom_getopt                    o;
    Ipcom_cmd_socktest_fd           *f;
    union Ipcom_cmd_socktest_addr   addr;
    char                            *p_addr = IP_NULL;
    char                            *p_port = IP_NULL;
    char                            *p_if   = IP_NULL;
    int                             sockno = -1;
    int                             opt;
    int                             fd = -1;

    (void)glob;

    if (argc == 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "s:a:p:i:N:", &o)) != -1)
    {
        switch (opt)
        {
        case 's':
            sockno  = ipcom_cmd_socktest_get_sockno(o.optarg);
            break;
        case 'a':
            p_addr = o.optarg;
            break;
        case 'p':
            p_port = o.optarg;
            break;
        case 'i':
            p_if   = o.optarg;
            break;
        case 'N':
            break;
        default:
            goto print_help;
        }
    }

    if (sockno >= 0)
    {
        /* Get fd entry */
        f = ipcom_cmd_socktest_fd_find(fdp, sockno);

        if (f != IP_NULL)
        {
            /* Setup socket address */
            if (ipcom_getsockaddrbyaddr(f->domain, p_addr, (void *) &addr.sa) < 0)
            {
                ipcom_printf("     Failed, invalid address %s"IP_LF,p_addr);
                IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
            }

            (void)ipcom_cmd_socktest_portinfo_add (f->domain, &addr.sa, p_port);

            if (f->domain == IP_AF_PACKET)
            {
                /* Setup pack sock specific stuff */
                addr.ll.sll_protocol = (Ip_u16) ip_htons (f->prot);
                if (p_if != IP_NULL)
                    addr.ll.sll_ifindex = ipcom_if_nametoindex (p_if);
            }
        }
        else
        {
            ipcom_printf("     Failed, bad socket number"IP_LF);
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
        }

#ifdef IPCOM_USE_FD_DAEMON
        fd = ipcom_fd_daemon_get_local_fd(f->fd);
#else
        fd = f->fd;
#endif

        /* Try to bind... */
        if (ipcom_bind (fd, &addr.sa, IPCOM_SA_LEN_GET(&addr.sa)) < 0)
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
    }
    else
    {
        ipcom_printf("     Failed, no socket number specified");
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    ipcom_printf("     Socket %d bound"IP_LF, f->fd);
    retval = IPCOM_SUCCESS;

leave:
#ifdef IPCOM_USE_FD_DAEMON
    if (fd != -1)
        ipcom_socketclose(fd);
#endif
    return retval;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest bind -s <socket> [-a <address>] [-p <port>] [-i <interface name>]"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -s specify socket number"IP_LF);
    ipcom_printf("     -a specify binding address"IP_LF);
    ipcom_printf("     -p specify binding port"IP_LF);
    ipcom_printf("     -p specify binding interface name"IP_LF);
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_setopt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_setopt(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    Ip_err                        retval;
    Ipcom_getopt                  o;
    Ipcom_cmd_socktest_fd        *f;
    Ipcom_cmd_socktest_sockopt   *opt;
    Ipcom_cmd_socktest_sock_level *level = IP_NULL;
    int                       sockno = -1;
    int                       cmd_opt;
    int                       ret;
    Ip_size_t                 i;
    char                     *p_opt = IP_NULL;
    char                     *delimiter;
    void                     *val;
    int                       fd = -1;

    (void)glob;

    if (argc == 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((cmd_opt = ipcom_getopt_r(argc, argv, "s:l:o:N:", &o)) != -1)
    {
        switch (cmd_opt)
        {
        case 's':
            sockno  = ipcom_cmd_socktest_get_sockno(o.optarg);
            break;
        case 'l':
            level = ipcom_cmd_socktest_param_parse(o.optarg,
                sizeof(Ipcom_cmd_socktest_sock_level),
                socktest_sock_level,
                SOCKTEST_LEVEL_ENTRIES(socktest_sock_level));

            if (level == IP_NULL)
            {
                ipcom_printf("     Failed, invalid level"IP_LF);
                goto print_help;
            }
            break;
        case 'o':
            p_opt = o.optarg;
            break;
        case 'N':
            break;
        default:
            goto print_help;
        }
    }

    if (sockno < 0)
    {
        ipcom_printf("     Failed, no socket specified"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }
    if (level == IP_NULL)
    {
        ipcom_printf("     Failed, no socket level specified"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    /* Get fd entry */
    f = ipcom_cmd_socktest_fd_find(fdp, sockno);

    if (f != IP_NULL)
    {
        char getopt = IP_FALSE;
        if (p_opt == IP_NULL)
        {
            ipcom_printf("     Failed, no option specified"IP_LF);
            goto print_help;
        }

        delimiter = ipcom_strchr(p_opt,'=');

        if (delimiter == IP_NULL)
        {
            ipcom_printf("     Failed, bad data format"IP_LF);
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
        }

        /* Add string terminator */
        delimiter[0] = 0;

        opt = ipcom_cmd_socktest_param_parse(p_opt,
            sizeof(Ipcom_cmd_socktest_sockopt),
            level->opt_list,
            level->opt_list_len);
        if (opt == IP_NULL)
        {
            ipcom_printf("     Failed, invalid option"IP_LF);
            goto print_help;
        }

        /* Parse option value */
        val = opt->type->parse_cb(&delimiter[1], &getopt);

        if (val == IP_NULL)
        {
            ipcom_printf("     Failed."IP_LF);
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
        }

        /* Try to set option...*/
#ifdef IPCOM_USE_FD_DAEMON
        fd = ipcom_fd_daemon_get_local_fd(f->fd);
#else
        fd = f->fd;
#endif
        ret = ipcom_setsockopt (fd, level->no, opt->no, val, opt->type->len);
        ipcom_free(val);

        if (ret < 0)
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
    }
    else
    {
        ipcom_printf("     Failed, bad socket number"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    ipcom_printf("     Socket %d options set"IP_LF, f->fd);
    retval = IPCOM_SUCCESS;

leave:
#ifdef IPCOM_USE_FD_DAEMON
    if (fd != -1)
        ipcom_socketclose(fd);
#endif
    return retval;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest setopt -s <socket> -l <level> -o <option>=<data>"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -s specify socket number"IP_LF);
    ipcom_printf("     -l specify level"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_cmd_socktest_param_list_print(socktest_sock_level,
        SOCKTEST_LEVEL_ENTRIES(socktest_sock_level),
        sizeof(Ipcom_cmd_socktest_sock_level));
    ipcom_printf(IP_LF);
    ipcom_printf("     -o specify opion and data"IP_LF);
    for (i = 0; i < SOCKTEST_LEVEL_ENTRIES(socktest_sock_level); i++)
    {
        ipcom_printf("        level: %s"IP_LF, socktest_sock_level[i].name);
        ipcom_cmd_socktest_sockopt_list_print(socktest_sock_level[i].opt_list,
            socktest_sock_level[i].opt_list_len);
        ipcom_printf(IP_LF);
    }

    return IPCOM_ERR_INVALID_ARG;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_getopt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_getopt(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    Ip_err                         retval;
    Ipcom_getopt                   o;
    Ipcom_cmd_socktest_fd         *f;
    Ipcom_cmd_socktest_sock_level *level = IP_NULL;
    Ipcom_cmd_socktest_sockopt    *opt;
    Ip_socklen_t                   optlen;
    int                            cmd_opt;
    int                            sockno = -1;
    int                            ret;
    Ip_size_t                      i;
    char                          *p_opt = IP_NULL;
    void                          *val = IP_NULL;
    char                          *val_format;
    char                          *delimiter;
    int                            fd = -1;

    (void)glob;

    if (argc == 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((cmd_opt = ipcom_getopt_r(argc, argv, "s:l:o:N:", &o)) != -1)
    {
        switch (cmd_opt)
        {
        case 's':
            sockno = ipcom_cmd_socktest_get_sockno(o.optarg);
            break;
        case 'l':
            level = ipcom_cmd_socktest_param_parse(o.optarg,
                sizeof(Ipcom_cmd_socktest_sock_level),
                socktest_sock_level,
                SOCKTEST_LEVEL_ENTRIES(socktest_sock_level));

            if (level == IP_NULL)
            {
                ipcom_printf("     Failed, invalid level"IP_LF);
                goto print_help;
            }
            break;
        case 'o':
            p_opt = o.optarg;
            break;
        case 'N':
            break;
        default:
            goto print_help;
        }
    }

    if (sockno < 0)
    {
        ipcom_printf("     Failed, no socket specified"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }
    if (level == IP_NULL)
    {
        ipcom_printf("     Failed, no socket level specified"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    /* Get fd entry */
    f = ipcom_cmd_socktest_fd_find(fdp, sockno);

    if (f != IP_NULL)
    {
        if (p_opt == IP_NULL)
        {
            ipcom_printf("     Failed, no option specified"IP_LF);
            goto print_help;
        }

        opt = ipcom_cmd_socktest_param_parse(p_opt,
            sizeof(Ipcom_cmd_socktest_sockopt),
            level->opt_list,
            level->opt_list_len);
        if (opt == IP_NULL)
        {
            ipcom_printf("     Failed, invalid option"IP_LF);
            goto print_help;
        }

        /* if there is input param in <data> */
        delimiter = ipcom_strchr(p_opt,'=');
        if (delimiter != IP_NULL)
        {
            char getopt = IP_TRUE;
            /* Add string terminator */
            delimiter[0] = 0;

            /* Parse option value */
            val = opt->type->parse_cb(&delimiter[1], &getopt);
            if (val == IP_NULL)
            {
                ipcom_printf("     Failed."IP_LF);
                IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
            }
        }
        else
        {
            val = ipcom_calloc(1, opt->type->len);
            if (val == IP_NULL)
                IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_NO_MEMORY);
        }

        /* Try to get option...*/
        optlen = opt->type->len;
#ifdef IPCOM_USE_FD_DAEMON
        fd = ipcom_fd_daemon_get_local_fd(f->fd);
#else
        fd = f->fd;
#endif
        ret = ipcom_getsockopt (fd, level->no, opt->no, val, &optlen);
        if (ret < 0)
        {
            ipcom_printf("     Failed, errno = %d"IP_LF,ipcom_errno);
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
        }

        /* Print formatted value */
        val_format = opt->type->format_cb(val);
        ipcom_printf("     %s"IP_LF,val_format);
        ipcom_free(val_format);
    }
    else
    {
        ipcom_printf("     Failed, bad socket number"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    retval = IPCOM_SUCCESS;

leave:

    if (val)
        ipcom_free(val);

#ifdef IPCOM_USE_FD_DAEMON
    if (fd != -1)
        ipcom_socketclose(fd);
#endif
    return retval;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest getopt -s <socket> -l <level> -o <option>"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -s specify socket number"IP_LF);
    ipcom_printf("     -l specify level"IP_LF);
    ipcom_cmd_socktest_param_list_print(socktest_sock_level,
        SOCKTEST_LEVEL_ENTRIES(socktest_sock_level),
        sizeof(Ipcom_cmd_socktest_sock_level));
    ipcom_printf(IP_LF);
    ipcom_printf("     -o specify option"IP_LF);
    for (i = 0; i < SOCKTEST_LEVEL_ENTRIES(socktest_sock_level); i++)
    {
        ipcom_printf("        level: %s"IP_LF, socktest_sock_level[i].name);
        ipcom_cmd_socktest_sockopt_list_print(socktest_sock_level[i].opt_list,
            socktest_sock_level[i].opt_list_len);
        ipcom_printf(IP_LF);
    }

    return IPCOM_ERR_INVALID_ARG;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_ioctl
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_ioctl(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    Ip_err                    retval;
    Ipcom_getopt              o;
    Ipcom_cmd_socktest_fd    *f;
    Ipcom_cmd_socktest_ioctl *ioctl_var = IP_NULL;
    char                     *p_value = IP_NULL;
    char                     *p_if    = IP_NULL;
    char                     *val_format;
    void                     *val     = IP_NULL;
    int                       sockno = -1;
    int                       cmd_opt;
    int                       fd = -1;

    (void)glob;

    if (argc == 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((cmd_opt = ipcom_getopt_r(argc, argv, "s:i:o:N:", &o)) != -1)
    {
        switch (cmd_opt)
        {
        case 's':
            sockno = ipcom_cmd_socktest_get_sockno(o.optarg);
            break;
        case 'o':

            p_value = ipcom_strchr(o.optarg,'=');

            if (p_value != IP_NULL)
                *p_value++ = 0;

            ioctl_var = ipcom_cmd_socktest_param_parse(o.optarg,
                                                       sizeof(Ipcom_cmd_socktest_ioctl),
                                                       socktest_ioctl,
                                                       SOCKTEST_IOCTL_ENTRIES(socktest_ioctl));
            if (ioctl_var == IP_NULL)
            {
                ipcom_printf("     Failed, invalid ioctl"IP_LF);
                goto print_help;
            }

            break;
        case 'i':
            p_if = o.optarg;
            break;
        case 'N':
            break;
        default:
            goto print_help;
        }
    }

    if (ioctl_var == IP_NULL)
    {
        ipcom_printf("     Failed, no ioctl specified"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    if (sockno < 0)
    {
        ipcom_printf("     Failed, no socket specified"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    /* Get fd entry */
    f = ipcom_cmd_socktest_fd_find(fdp, sockno);
    if (f == IP_NULL)
    {
        ipcom_printf("     Failed, bad socket number"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    val = ioctl_var->type->parse_cb(p_value, p_if);

    if (val == IP_NULL)
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_NO_MEMORY);

    /* Try the ioctl */
#ifdef IPCOM_USE_FD_DAEMON
        fd = ipcom_fd_daemon_get_local_fd(f->fd);
#else
        fd = f->fd;
#endif

    if (ipcom_socketioctl(fd, ioctl_var->no, val) < 0)
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);

    if (ioctl_var->is_get)
    {
        /* This is a "get" ioctl */
        val_format = ioctl_var->type->format_cb (val);

        if (val_format == IP_NULL)
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_NO_MEMORY);

        ipcom_printf("     %s"IP_LF,val_format);
        ipcom_free(val_format);
    }

    retval = IPCOM_SUCCESS;

leave:
#ifdef IPCOM_USE_FD_DAEMON
    if (fd != -1)
        ipcom_socketclose(fd);
#endif
    if (val != IP_NULL)
        ipcom_free(val);
    return retval;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest ioctl -s <socket> -o <ioctl option> [-i <interface name>] "IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -s specify socket number"IP_LF);
    ipcom_printf("     -o specify ioctl"IP_LF);
    ipcom_cmd_socktest_ioctl_list_print(socktest_ioctl,
        SOCKTEST_IOCTL_ENTRIES(socktest_ioctl));
    ipcom_printf(IP_LF);
    ipcom_printf("     -i specify interface name"IP_LF);
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_connect
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_connect(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    Ip_err                          retval;
    Ipcom_getopt                    o;
    Ipcom_cmd_socktest_fd           *f;
    union Ipcom_cmd_socktest_addr   addr;
    char                            *p_addr = IP_NULL;
    char                            *p_port = IP_NULL;
    int                             opt;
    int                             domain = -1;
    int                             sockno = -1;
    int                             fd = -1;

    (void)glob;

    if (argc == 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "s:a:p:d:N:", &o)) != -1)
    {
        switch (opt)
        {
        case 's':
            sockno = ipcom_cmd_socktest_get_sockno(o.optarg);
            break;
        case 'a':
            p_addr = o.optarg;
            break;
        case 'p':
            p_port = o.optarg;
            break;
        case 'd':
            domain = ipcom_cmd_socktest_param_to_int(o.optarg,
                socktest_domain,
                SOCKTEST_PARAM_ENTRIES(socktest_domain));
            break;
        case 'N':
            break;
        default:
            goto print_help;
        }
    }

    if (p_addr == IP_NULL || p_port == IP_NULL)
    {
        goto print_help;
    }


    if (sockno >= 0)
    {
        /* Get fd entry */
        f = ipcom_cmd_socktest_fd_find(fdp, sockno);

        if (f != IP_NULL)
        {
            /* Setup socket address */
            if (domain == -1)
                domain = f->domain;
            if (ipcom_getsockaddrbyaddr(domain, p_addr, (void*) &addr.sa) < 0)
            {
                ipcom_printf("     Failed, invalid address %s"IP_LF,p_addr);
                IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
            }

            if (ipcom_cmd_socktest_portinfo_add (f->domain, &addr.sa, p_port) != IPCOM_SUCCESS)
                IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);

            /* Try to connect...*/
#ifdef IPCOM_USE_FD_DAEMON
            fd = ipcom_fd_daemon_get_local_fd(f->fd);
#else
            fd = f->fd;
#endif
            if (ipcom_connect (fd, &addr.sa, IPCOM_SA_LEN_GET(&addr.sa)) < 0)
                IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
        }
        else
        {
            ipcom_printf("     Failed, bad socket number"IP_LF);
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
        }
    }
    else
    {
        ipcom_printf("     Failed, no socket specified"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }


    ipcom_printf("     Socket %d connected"IP_LF, f->fd);
    retval = IPCOM_SUCCESS;

leave:
#ifdef IPCOM_USE_FD_DAEMON
    if (fd != -1)
        ipcom_socketclose(fd);
#endif
    return retval;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest connect -s <socket> -a <peer address> -p <peer port> [-d <domain>]"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -s specify socket number"IP_LF);
    ipcom_printf("     -a specify peer address"IP_LF);
    ipcom_printf("     -p specify peer port"IP_LF);
    ipcom_printf("     -d specify domain"IP_LF);
    ipcom_cmd_socktest_param_list_print(socktest_domain,
                                         SOCKTEST_PARAM_ENTRIES(socktest_domain),
                                         sizeof(Ipcom_cmd_socktest_param));
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_listen
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_listen(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    Ip_err             retval;
    Ipcom_getopt       o;
    Ipcom_cmd_socktest_fd *f;
    int                opt;
    int                backlog = 0;
    int                sockno  = -1;
    int                fd = -1;

    (void)glob;

    if (argc == 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "s:b:N:", &o)) != -1)
    {
        switch (opt)
        {
        case 's':
            sockno = ipcom_cmd_socktest_get_sockno(o.optarg);
            break;
        case 'b':
            backlog = ipcom_atoi (o.optarg);
            break;
        case 'N':
            break;
        default:
            goto print_help;
        }
    }

    if (sockno >= 0)
    {
        /* Get fd entry */
        f = ipcom_cmd_socktest_fd_find(fdp, sockno);
        if (f != IP_NULL)
        {
            /* Try listen...*/
#ifdef IPCOM_USE_FD_DAEMON
            fd = ipcom_fd_daemon_get_local_fd(f->fd);
#else
            fd = f->fd;
#endif
            if (ipcom_listen (fd, backlog) < 0)
                IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
        }
        else
        {
            ipcom_printf("     Failed, bad socket number"IP_LF);
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
        }
    }
    else
    {
        ipcom_printf("     Failed, no socket specified"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    ipcom_printf("     Socket %d listens"IP_LF, f->fd);
    retval = IPCOM_SUCCESS;

leave:
#ifdef IPCOM_USE_FD_DAEMON
    if (fd != -1)
        ipcom_socketclose(fd);
#endif
    return retval;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest listen -s <socket> -b <back log>"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -s specify socket number"IP_LF);
    ipcom_printf("     -b specify number of backlog entries"IP_LF);
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_accept
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_accept(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    Ip_err             retval;
    Ipcom_getopt       o;
    Ipcom_cmd_socktest_fd *f;
    Ipcom_cmd_socktest_fd *new_f;
    int                sockno = -1;
    int                opt;
    Ip_fd              fdg = -1;
    Ip_fd              fdl;
    Ip_fd              fd;
    char               sysbuf[128];
    char               intbuf[12];

    (void)glob;

    if (argc == 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "s:N:z:", &o)) != -1)
    {
        switch (opt)
        {
        case 's':
            sockno = ipcom_cmd_socktest_get_sockno(o.optarg);
            break;
        case 'N':
            break;
        case 'z':
            ipcom_sprintf(sysbuf, "ipcom.socktest.socket.%s", o.optarg);
            break;
        default:
            goto print_help;
        }
    }

    if (sockno >= 0)
    {
        /* Get fd entry */
        f = ipcom_cmd_socktest_fd_find(fdp, sockno);
        if (f != IP_NULL)
        {
#ifdef IPCOM_USE_FD_DAEMON
            fdg = ipcom_fd_daemon_get_local_fd(f->fd);
#else
            fdg = f->fd;
#endif

            fdl = ipcom_accept(fdg, IP_NULL, IP_NULL);
            if (fdl == -1)
                IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);

            new_f = ipcom_cmd_socktest_fd_find(fdp, -1);
            if (new_f == IP_NULL)
            {
                ipcom_printf("     Failed, socktest socket list full"IP_LF);
                IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
            }

#ifdef IPCOM_USE_FD_DAEMON
            fd = ipcom_fd_daemon_translate_new_fd(fdl);
#else
            fd = fdl;
#endif
            new_f->fd        = fd;
            new_f->domain    = f->domain;
            new_f->type      = f->type;
            new_f->prot      = f->prot;
            new_f->event_sem = IPCOM_SEM_INVALID;
            SOCKTEST_FLOOD_INIT(new_f);

            ipcom_sprintf(intbuf, "%d", new_f->fd);
            (void)ipcom_sysvar_set(sysbuf, intbuf, IPCOM_SYSVAR_FLAG_OVERWRITE);

            ipcom_printf("     Socket = %d",new_f->fd);
        }
        else
        {
            ipcom_printf("     Failed, bad socket number"IP_LF);
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
        }
    }
    else
    {
        ipcom_printf("     Failed, no socket specified"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    ipcom_printf("     Socket %d accepted"IP_LF, f->fd);
    retval = IPCOM_SUCCESS;

leave:
#ifdef IPCOM_USE_FD_DAEMON
    if (fdg != -1)
        ipcom_socketclose(fdg);
#endif
    return retval;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest accept -s <socket>"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -s specify socket number"IP_LF);
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_getname
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_getname(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv, Ip_bool is_peer)
{
    Ip_err                          retval;
    Ipcom_getopt                    o;
    Ipcom_cmd_socktest_fd           *f;
    union Ipcom_cmd_socktest_addr   addr;
    Ip_socklen_t                    socklen;
    char                            buf[50];
    int                             sockno = -1;
    int                             opt;
    int                             fd = -1;

    (void)glob;

    if (argc == 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "s:N:", &o)) != -1)
    {
        switch (opt)
        {
        case 's':
            sockno = ipcom_cmd_socktest_get_sockno(o.optarg);
            break;
        case 'N':
            break;
        default:
            goto print_help;
        }
    }

    if (sockno >= 0)
    {
        /* Get fd entry */
        f = ipcom_cmd_socktest_fd_find(fdp, sockno);
        if (f != IP_NULL)
        {
            /* Try getpeer...*/
#ifdef IPCOM_USE_FD_DAEMON
            fd = ipcom_fd_daemon_get_local_fd(f->fd);
#else
            fd = f->fd;
#endif
            socklen = sizeof (addr);
            if (is_peer)
            {
                if (ipcom_getpeername(fd, &addr.sa, &socklen) < 0)
                    IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
            }
            else
            {
                if (ipcom_getsockname(fd, &addr.sa, &socklen) < 0)
                    IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
            }

            if (ipcom_cmd_sockaddr_to_str((void *) &addr.sa,
                                          IP_TRUE,
                                          IP_TRUE,
                                          buf,
                                          sizeof (buf)) == IP_NULL)
            {
                ipcom_printf("     Failed, unable to get %s info."IP_LF, is_peer ? "peername" : "sockname");
                IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
            }

            ipcom_printf("     %s: %s"IP_LF, is_peer ? "Peer" : "Name", buf);
        }
        else
        {
            ipcom_printf("     Failed, bad socket number"IP_LF);
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
        }
    }
    else
    {
        ipcom_printf("     Failed, no socket specified"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    retval = IPCOM_SUCCESS;

leave:
#ifdef IPCOM_USE_FD_DAEMON
    if (fd != -1)
        ipcom_socketclose(fd);
#endif
    return retval;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest %s -s <socket>"IP_LF, is_peer ? "getpeer" : "getsockname");
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -s specify socket number"IP_LF);
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_getpeer
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_getsockname(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    return ipcom_cmd_socktest_handle_getname(glob, fdp, argc, argv, IP_FALSE);
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_getpeer
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_getpeername(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    return ipcom_cmd_socktest_handle_getname(glob, fdp, argc, argv, IP_TRUE);
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_select
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_select(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    struct Ip_timeval tv = {0, 0};
    struct Ip_timeval *tvp = IP_NULL;
    Ip_fd_set rset;
    Ip_fd_set wset;
    Ip_fd_set xset;
    Ip_fd_set *set;
    Ip_fd maxfd = 0;
    Ipcom_getopt o;
    Ip_err retval = IPCOM_SUCCESS;
    int ret;
    int opt;

#ifdef IPCOM_USE_FD_DAEMON
    int i;
    int fdc = 0;
    Ip_fd l_fd[MAX_DAEMON_FD];
    Ip_fd g_fd[MAX_DAEMON_FD];

    ipcom_memset(l_fd, 0, sizeof(l_fd));
    ipcom_memset(g_fd, 0, sizeof(g_fd));
#endif

    (void)glob;
    (void)fdp;

    if (argc <= 1)
        goto print_help;

    /* Clear sets */
    IP_FD_ZERO(&rset);
    IP_FD_ZERO(&wset);
    IP_FD_ZERO(&xset);

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "r:w:x:t:N:", &o)) != -1)
    {
        Ip_fd fd = 0;

        switch (opt)
        {
        case 'r':
            fd = ipcom_cmd_socktest_get_sockno(o.optarg);
#ifdef IPCOM_USE_FD_DAEMON
            g_fd[fdc] = fd;
            fd = ipcom_fd_daemon_get_local_fd(fd);
            l_fd[fdc++] = fd;
#endif
            IP_FD_SET(fd, &rset);
            break;
        case 'w':
            fd = ipcom_cmd_socktest_get_sockno(o.optarg);
#ifdef IPCOM_USE_FD_DAEMON
            g_fd[fdc] = fd;
            fd = ipcom_fd_daemon_get_local_fd(fd);
            l_fd[fdc++] = fd;
#endif
            IP_FD_SET(fd, &wset);
            break;
        case 'x':
            fd = ipcom_cmd_socktest_get_sockno(o.optarg);
#ifdef IPCOM_USE_FD_DAEMON
            g_fd[fdc] = fd;
            fd = ipcom_fd_daemon_get_local_fd(fd);
            l_fd[fdc++] = fd;
#endif
            IP_FD_SET(fd, &xset);
            break;
        case 't':
            {
                int t = ipcom_atoi(o.optarg);
                tv.tv_sec  = t / 1000;
                tv.tv_usec = (t % 1000) * 1000; /*lint !e647 */
                tvp = &tv;
                break;
            }
        case 'N':          break;
        default:
            goto print_help;
        }

        maxfd = IP_MAX(maxfd, fd);
    }

    /* Do the select */
    if ((ret = ipcom_socketselect(maxfd + 1, &rset, &wset, &xset, tvp)) < 0)
    {
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
    }

    /* Show result */
    for (set = &rset;
         set != IP_NULL;
         set = (set == &rset ? &wset : (set == &wset ? &xset : IP_NULL)))
    {
        int got = 0;
        int i;

        ipcom_printf("%c:", (set == &rset ? 'R' : (set == &wset ? 'W' : 'X')));

        for (i = 0; i <= maxfd; i++)
        {
            if (IP_FD_ISSET(i, set))
            {
#ifdef IPCOM_USE_FD_DAEMON
                ipcom_printf("%s%d", (got++ ? "," : ""), ipcom_lfd_to_gfd(i, l_fd, g_fd));
#else
                ipcom_printf("%s%d", (got++ ? "," : ""), i);
#endif
            }
        }

        ipcom_printf("-"IP_LF);
    }

    ipcom_printf("Returned %d"IP_LF, ret);

 leave:
#ifdef IPCOM_USE_FD_DAEMON
    for (i = 0; i < fdc; i++)
        ipcom_socketclose(l_fd[i]);
#endif
    return retval;

 print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest select -r <rsock> -w <wsock> -x <xsock>"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -r specify read socket (may be repeated)"IP_LF);
    ipcom_printf("     -w specify write socket (may be repeated)"IP_LF);
    ipcom_printf("     -x specify exception socket (may be repeated)"IP_LF);
    ipcom_printf("     -t specify timeout in milliseconds"IP_LF);
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_shutdown
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_shutdown(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    int                    fd;
    Ipcom_getopt           o;
    int                    opt;
    int                    how = IP_SHUT_RDWR;
    int                    sockno = -1;
    Ip_err                 retval;
    Ipcom_cmd_socktest_fd *f;

    (void)glob;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "rs:w", &o)) != -1)
    {
        switch (opt)
        {
        case 's':
            sockno = ipcom_cmd_socktest_get_sockno(o.optarg);
            break;
        case 'r':
            how = IP_SHUT_RD;
            break;
        case 'w':
            how = IP_SHUT_WR;
            break;
        default:
            goto print_help;
        }
    }

#ifdef IP_PORT_INTEGRITY
    ipcom_shutdown_libsocket();
#else
    if (sockno < 0)
    {
        ipcom_printf("     Failed, no socket specified"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    /* Get fd entry */
    f = ipcom_cmd_socktest_fd_find(fdp, sockno);
    if (f == IP_NULL)
    {
        ipcom_printf("     Failed, bad socket number"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

#ifdef IPCOM_USE_FD_DAEMON
            fd = ipcom_fd_daemon_get_local_fd(f->fd);
#else
            fd = f->fd;
#endif

    if (ipcom_shutdown(fd, how) >= 0)
        ipcom_printf("     Success"IP_LF);
    else
    {
        ipcom_printf("     Failed, shutdown returned '%s'"IP_LF, ipcom_strerror(ipcom_errno));
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
    }
#endif

    retval = IPCOM_SUCCESS;

 leave:
    return retval;

 print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest shutdown [-r] [-w]"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}
/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_getaddrs
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPSCTP

IP_STATIC Ip_err ipcom_cmd_socktest_handle_getaddrs (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv, Ip_bool is_peer)
{
    Ip_err                          retval;
    Ipcom_getopt                    o;
    Ipcom_cmd_socktest_fd           *f;
    char                            buf[50];
    int                             sockno = -1;
    int                             opt;
    int                             assoc_id = 0;
    int                             i;
    IP_STATIC union Ipcom_cmd_socktest_addr   *addr;
    int                             addrnum;

    (void)glob;

    if (argc == 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "s:a:N:", &o)) != -1)
    {
        switch (opt)
        {
        case 's':
            sockno = ipcom_atoi(o.optarg);
            break;
        case 'a':
            assoc_id = ipcom_atoi(o.optarg);
        case 'N':
            break;
        default:
            goto print_help;
        }
    }

    if (sockno >= 0)
    {
        /* Get fd entry */
        f = ipcom_cmd_socktest_fd_find(fdp, sockno);
        if (f != IP_NULL)
        {
            /* Try getpeer...*/
            if (is_peer)
            {
                addrnum = ipsctp_getpaddrs(f->fd, assoc_id, (struct Ip_sockaddr **)&addr);
                if (addrnum < 0)
                    IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
            }
            else
            {
                addrnum = ipsctp_getladdrs(f->fd, assoc_id, (struct Ip_sockaddr **)&addr);
                if (addrnum < 0)
                    IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
            }
            if (0 == addrnum)
            {
                ipcom_printf("     No %s addresses specified,"IP_LF, is_peer ? "Peer" : "Local", buf);
                return IPCOM_SUCCESS;
            }
            else
            {
                ipcom_printf("     %s address list(%p) are:"IP_LF, is_peer ? "Peer" : "Local", addr);
            }

            for (i = 0;i < addrnum;i++)
            {
                if (ipcom_cmd_sockaddr_to_str((void *) &addr[i].sa,
                                              IP_TRUE,
                                              IP_TRUE,
                                              buf,
                                              sizeof (buf)) == IP_NULL)
                {
                    retval = IPCOM_SUCCESS;
                    IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
                }
                ipcom_printf("     %s"IP_LF, buf);
            }
        }
        else
        {
            ipcom_printf("     Failed, bad socket number"IP_LF);
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
        }
    }
    else
    {
        ipcom_printf("     Failed, no socket specified or no association setup"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    retval = IPCOM_SUCCESS;

leave:
    return retval;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest %s -s <socket> [-a <assoc_id>]"IP_LF, is_peer ? "sctp_getpaddrs" : "sctp_getladdrs");
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -s specify socket number"IP_LF);
    ipcom_printf("     -a specify association id"IP_LF);
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}

/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_freeaddrs
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */

IP_STATIC Ip_err ipcom_cmd_socktest_handle_freeaddrs (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv, Ip_bool is_peer)
{
    Ipcom_getopt                    o;
    int                             opt;
    struct sockaddr                 *address = IP_NULL;

    (void)glob;

    if (argc == 1)
    {
        goto print_help;
    }

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "a:N:", &o)) != -1)
    {
        switch (opt)
        {
            case 'a':
                address = (struct sockaddr *)ipcom_atoi(o.optarg);
            case 'N':
                break;
            default:
                goto print_help;
        }
    }
    if (is_peer)
    {
        ipsctp_freepaddrs((struct Ip_sockaddr *)address);
    }
    else
    {
        ipsctp_freeladdrs((struct Ip_sockaddr *)address);
    }

    ipcom_printf("    address list(%p) is freed"IP_LF,address);
    return IPCOM_SUCCESS;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest sctp_freepaddrs -a <address_pointer>"IP_LF, is_peer ? "sctp_freepaddrs" : "sctp_freeladdrs");
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -a specify pointer of the address array to be freed "IP_LF);
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}
/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_sctp_getpaddrs
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */

IP_STATIC Ip_err ipcom_cmd_socktest_handle_sctp_getpaddrs (Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    return ipcom_cmd_socktest_handle_getaddrs(glob, fdp, argc, argv, IP_TRUE);
}
/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_sctp_freepaddrs
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */

IP_STATIC Ip_err ipcom_cmd_socktest_handle_sctp_freepaddrs(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    return ipcom_cmd_socktest_handle_freeaddrs(glob, fdp, argc, argv, IP_TRUE);
}

/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_sctp_getladdrs
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */

IP_STATIC Ip_err ipcom_cmd_socktest_handle_sctp_getladdrs(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    return ipcom_cmd_socktest_handle_getaddrs(glob, fdp, argc, argv, IP_FALSE);
}

/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_sctp_freeladdrs
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */

IP_STATIC Ip_err ipcom_cmd_socktest_handle_sctp_freeladdrs(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    return ipcom_cmd_socktest_handle_freeaddrs(glob, fdp, argc, argv, IP_FALSE);
}

/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_sctp_addrx
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_sctp_addrx(int domain,
                                     char *straddr,
                                     struct Ip_sockaddr **addrx,
                                     char *port,
                                     int *addrcnt)
{
    while (*straddr != '\0' && *addrcnt < SOCKETEST_ADDR_MAX_NUM)
    {
        while (*straddr == ' ')
        {
            straddr++;
        }
        /* Setup socket address */
        if (ipcom_getsockaddrbyaddr(domain, straddr, *addrx) < 0)
        {
            ipcom_printf("     Failed, invalid address %s"IP_LF,straddr);
            return IPCOM_ERR_INVALID_ARG;
        }
        if (ipcom_cmd_socktest_portinfo_add (domain, *addrx, port) < 0)
        {
            ipcom_printf("     Failed, invalid port"IP_LF);
            return IPCOM_ERR_INVALID_ARG;
        }

        (*addrcnt)++;
        *addrx = (struct Ip_sockaddr *)((Ip_u8 *)(*addrx) + IPCOM_SA_LEN_GET(*addrx));

        while (*straddr != ','&& *straddr != '\0')
        {
            straddr++;
        }
        if (*straddr == '\0')
        {
            break;
        }
        straddr++;
    }
    return IPCOM_SUCCESS;
}

/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_sctp_bindx
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_sctp_bindx(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    Ip_err                          retval;
    Ipcom_getopt                    o;
    Ipcom_cmd_socktest_fd           *f;
    char                            *p_addr = IP_NULL;
    char                            *p_addr6 = IP_NULL;
    char                            *p_port = IP_NULL;
    char                            flag;
    int                             sockno = -1;
    int                             opt;
    int                             addrcnt = 0;

    (void)glob;

    if (argc == 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "s:a:A:p:f:i:m:N:", &o)) != -1)
    {
        switch (opt)
        {
        case 's':
            sockno = ipcom_atoi(o.optarg);
            break;
        case 'a':
            p_addr = o.optarg;
            break;
        case 'A':
            p_addr6 = o.optarg;
            break;
        case 'p':
            p_port = o.optarg;
            break;
        case 'f':
            flag = o.optarg ? *o.optarg : (char)IPSCTP_OPTNAME_MAX;
            break;
        case 'N':
            break;
        default:
            goto print_help;
        }
    }
    if (sockno >= 0)
    {
        /* Get fd entry */
        f = ipcom_cmd_socktest_fd_find(fdp, sockno);

        if (f != IP_NULL)
        {
            struct Ip_sockaddr *addrx = (struct Ip_sockaddr *)addrgr;
            if (IP_NULL != p_addr)
            {
                retval = ipcom_cmd_socktest_handle_sctp_addrx(IP_AF_INET,
                                                              p_addr,
                                                              &addrx,
                                                              p_port,
                                                              &addrcnt);
                if (retval < 0)
                {
                    IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
                }
            }
            if (IP_NULL != p_addr6)
            {
                retval = ipcom_cmd_socktest_handle_sctp_addrx(IP_AF_INET6,
                                                              p_addr6,
                                                              &addrx,
                                                              p_port,
                                                              &addrcnt);
                if (retval < 0)
                {
                    IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
                }
            }

            if (SOCKETEST_ADDR_MAX_NUM == addrcnt)
            {
                ipcom_printf("     Only %d addresses are allowed to be bound"IP_LF,SOCKETEST_ADDR_MAX_NUM);
            }
            if (addrcnt == 0)
            {
                ipcom_printf("     No address"IP_LF);
                IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
            }
        }
        else
        {
            ipcom_printf("     Failed, bad socket number"IP_LF);
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
        }
        switch(flag)
        {
            case 'A':
                flag = (char)IPSCTP_BINDX_ADD_ADDR;
                break;
            case 'R':
                flag = (char)IPSCTP_BINDX_REM_ADDR;
                break;
            default:
                IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
        }
        /* Try to bind... */
        if (ipsctp_bindx (f->fd, (struct Ip_sockaddr *)addrgr, addrcnt, flag) < 0)
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
    }
    else
    {
        ipcom_printf("     Failed, no socket number specified");
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    ipcom_printf("     Socket %d bound %d addresses"IP_LF, f->fd,addrcnt);
    retval = IPCOM_SUCCESS;

leave:
    return retval;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest sctp_bindx -s <socket> [-a <address0>,<address1>, .... <addressN>] [-A <address0>,<address1>, .... <addressN>] -p <port> -f <flag>"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -s specify socket number"IP_LF);
    ipcom_printf("     -a specify binding ipv4 addresses"IP_LF);
    ipcom_printf("     -A specify binding ipv6 addresses"IP_LF);
    ipcom_printf("     -p specify binding port"IP_LF);
    ipcom_printf("     -f specify opt mode add(A) or remove(R)"IP_LF);
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_sctp_connectx
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */

IP_STATIC Ip_err ipcom_cmd_socktest_handle_sctp_connectx(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    Ip_err                          retval;
    Ipcom_getopt                    o;
    Ipcom_cmd_socktest_fd           *f;
    char                            *p_addr = IP_NULL;
    char                            *p_addr6 = IP_NULL;
    char                            *p_port = IP_NULL;
    int                             opt;
    int                             sockno = -1;
    int                             addrcnt = 0;
    Ipsctp_assoc_t                  assoc_id = 0;

    (void)glob;

    if (argc == 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "s:a:A:p:N:", &o)) != -1)
    {
        switch (opt)
        {
        case 's':
            sockno = ipcom_atoi(o.optarg);
            break;
        case 'a':
            p_addr = o.optarg;
            break;
        case 'A':
            p_addr6 = o.optarg;
            break;
        case 'p':
            p_port = o.optarg;
            break;
        case 'N':
            break;
        default:
            goto print_help;
        }
    }

    if (sockno >= 0)
    {
        /* Get fd entry */
        f = ipcom_cmd_socktest_fd_find(fdp, sockno);

        if (f != IP_NULL)
        {
            struct Ip_sockaddr *addrx = (struct Ip_sockaddr *)addrgr;
            if (IP_NULL != p_addr)
            {
                retval = ipcom_cmd_socktest_handle_sctp_addrx(IP_AF_INET,
                                                              p_addr,
                                                              &addrx,
                                                              p_port,
                                                              &addrcnt);
                if (retval < 0)
                {
                    IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
                }
            }
            if (IP_NULL != p_addr6)
            {
                retval = ipcom_cmd_socktest_handle_sctp_addrx(IP_AF_INET6,
                                                              p_addr6,
                                                              &addrx,
                                                              p_port,
                                                              &addrcnt);
                if (retval < 0)
                {
                    IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
                }
            }
            if (addrcnt == SOCKETEST_ADDR_MAX_NUM)
            {
                ipcom_printf("     Only %d addresses are allowed to be connected"IP_LF,SOCKETEST_ADDR_MAX_NUM);
            }

            /* Try to connect...*/
            if (ipsctp_connectx(f->fd, (struct Ip_sockaddr *)addrgr, addrcnt, &assoc_id) < 0)
                IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
        }
        else
        {
            ipcom_printf("     Failed, bad socket number"IP_LF);
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
        }
    }
    else
    {
        ipcom_printf("     Failed, no socket specified"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    ipcom_printf("     Socket %d associate id %d connected "IP_LF, f->fd, assoc_id);
    retval = IPCOM_SUCCESS;

leave:
    return retval;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest sctp_connectx -s <socket> -a <peer address0>,[<peer address1>, ... <peer addressN>][-A <address0>,<address1>, .... <addressN>] -p <peer port>"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -s specify socket number"IP_LF);
    ipcom_printf("     -a specify ipv4 peer addresses"IP_LF);
    ipcom_printf("     -A specify ipv6 peer addresses"IP_LF);
    ipcom_printf("     -p specify peer port"IP_LF);
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}

/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_getaddrlen
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */

IP_STATIC Ip_err ipcom_cmd_socktest_handle_sctp_getaddrlen(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    Ip_err                          retval;
    Ipcom_getopt                    o;
    int                             opt;
    int                             family = -1;
    int                             addrlen;

    (void)glob;

    if (argc == 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "d:N:", &o)) != -1)
    {
        switch (opt)
        {
        case 'd':
            family = ipcom_atoi(o.optarg);
            break;
        case 'N':
            break;
        default:
            goto print_help;
        }
    }
    /* Try to getaddrlen...*/
    addrlen = ipsctp_getaddrlen (family);
    if (addrlen < 0)
    {
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
    }

    retval = IPCOM_SUCCESS;
    ipcom_printf("     Getaddrlen is %d"IP_LF,addrlen);
leave:
    return retval;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest sctp_getaddrlen -d <family>"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -d specify sa_family"IP_LF);
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}
/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_sctp_peeloff
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_sctp_peeloff(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    Ip_err                 retval;
    Ipcom_getopt           o;
    Ipcom_cmd_socktest_fd *f        = IP_NULL;
    Ipcom_cmd_socktest_fd *new_f    = IP_NULL;
    int                    sockno   = -1;
    int                    opt      = -1;
    Ip_fd                  fd       = -1;
    Ipsctp_assoc_t         assoc_id = 0;

    (void)glob;

    if (argc == 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "a:s:N:", &o)) != -1)
    {
        switch (opt)
        {
        case 'a':
            assoc_id = (Ipsctp_assoc_t)ipcom_strtoul(o.optarg, IP_NULL, 10);
            break;
        case 's':
            sockno   = ipcom_atoi(o.optarg);
            break;
        case 'N':
            break;
        default:
            goto print_help;
        }
    }

    /* association id must be specified */
    if (0 == assoc_id)
    {
        ipcom_printf("     Failed, no assoc_id specified"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    if (sockno >= 0)
    {
        /* Get fd entry */
        f = ipcom_cmd_socktest_fd_find(fdp, sockno);
        if (f != IP_NULL)
        {
            fd = ipsctp_peeloff(f->fd, assoc_id);
            if (fd == -1)
            {
                ipcom_printf("     Failed, peel off error"IP_LF);
                IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
            }

            new_f = ipcom_cmd_socktest_fd_find(fdp, -1);
            if (new_f == IP_NULL)
            {
                ipcom_printf("     Failed, socktest socket list full"IP_LF);
                IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
            }

            new_f->fd        = fd;
            new_f->domain    = f->domain;
            new_f->type      = f->type;
            new_f->prot      = f->prot;
            new_f->event_sem = IPCOM_SEM_INVALID;
            SOCKTEST_FLOOD_INIT(new_f);

            ipcom_printf("     Peel off socket = %d", new_f->fd);
        }
        else
        {
            ipcom_printf("     Failed, bad socket number"IP_LF);
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
        }
    }
    else
    {
        ipcom_printf("     Failed, no socket specified"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    ipcom_printf("     Socket %d peels off"IP_LF, f->fd);
    retval = IPCOM_SUCCESS;

leave:
    return retval;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest peeloff -s <socket> -a <assoc_id>"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -s specify socket number"IP_LF);
    ipcom_printf("     -a specify association identifier"IP_LF);
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}

#endif

/*
 *===========================================================================
 *                    ipcom_cmd_socktest_print_ancillary_data
 *===========================================================================
 * Description: Prints ancillary data entry.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_cmd_socktest_print_ancillary_data(struct Ip_cmsghdr *cmsg)
{
    unsigned i, j;
    unsigned alen = sizeof(socktest_ancillary_data) / sizeof(Ipcom_cmd_socktest_ancillary_data);
    char    *s;

    ipcom_printf("       length=%d, ", (int)cmsg->cmsg_len);
    for (i = 0; i < alen; i++)
        if (socktest_ancillary_data[i].level == cmsg->cmsg_level
            && socktest_ancillary_data[i].type == cmsg->cmsg_type)
        {
            for (j = 0; j < SOCKTEST_LEVEL_ENTRIES(socktest_sock_level); j++)
                if (socktest_sock_level[j].no == cmsg->cmsg_level)
                    break;

            if (j == SOCKTEST_LEVEL_ENTRIES(socktest_sock_level))
                ipcom_printf("level=%d", cmsg->cmsg_level);
            else
                ipcom_printf("level=%s", socktest_sock_level[j].name);

            ipcom_printf(", type=%s", socktest_ancillary_data[i].name);
            s = socktest_ancillary_data[i].gtype->format_cb(IP_CMSG_DATA(cmsg));
            if (s != IP_NULL)
            {
                ipcom_printf(", data=%s", s);
                ipcom_free(s);
            }
            ipcom_printf(IP_LF);
            return;
        }

    /* Unknown (for socktest) ancillary data */
    ipcom_printf("level=%d, type=%d"IP_LF,
                 cmsg->cmsg_level, cmsg->cmsg_type);
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_print_all_ancillary_data
 *===========================================================================
 * Description: Prints all ancillary data that was received.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_cmd_socktest_print_all_ancillary_data(struct Ip_msghdr *m)
{
    struct Ip_cmsghdr *cmsg = IP_CMSG_FIRSTHDR(m);

    if (cmsg == IP_NULL)
        /* No ancillary data present */
        return;

    ipcom_printf("     Ancillary data"IP_LF);
    do {
        ipcom_cmd_socktest_print_ancillary_data(cmsg);
    } while (IP_NULL != (cmsg = (void *) IP_CMSG_NXTHDR((void*) m, (void*)cmsg))
             && cmsg->cmsg_len > 0);
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_recv_impl
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_recv_impl(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv, Ip_bool use_zerocopy)
{
    Ip_err                          retval = 0;
    Ipcom_getopt                    o;
    Ipcom_cmd_socktest_fd          *f;
    union Ip_sockaddr_union         from;
    Ip_socklen_t                    from_len = sizeof(from);
    int                             sockno = -1;
    int                             length = 100;
    int                             flags  = 0;
    int                             i;
    int                             use_recvfrom = 0;
    int                             j;
    int                             opt;
    int                             ret;
    int                             n = 1;
    Ip_bool                         print_raw = IP_FALSE;
    Ip_bool                         show_from_addr = IP_FALSE;
#ifdef IPCOM_CMD_SOCKTEST_USE_ZEROCOPY_API
    Ip_bool                         detach_buf = IP_FALSE;
#endif
    char                            str_addr[50];
    Ip_u8                          *buf = IP_NULL;
    struct Ip_cmsghdr              *cmsg = IP_NULL;
    int                             fd = -1;

    (void)glob;

    if (argc == 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "s:ADNPl:f:n:rwom", &o)) != -1)
    {
        switch (opt)
        {
        case 's':
            sockno = ipcom_cmd_socktest_get_sockno(o.optarg);
            break;
        case 'A':
            show_from_addr = IP_TRUE;
            break;
#ifdef IPCOM_CMD_SOCKTEST_USE_ZEROCOPY_API
        case 'D':
            detach_buf = IP_TRUE;
            break;
#endif
        case 'l':
            length = ipcom_atoi(o.optarg);
            break;
        case 'f':
            flags = ipcom_atoi(o.optarg);
            break;
        case 'n':
            n = ipcom_atoi(o.optarg);
            break;
        case 'r':
            print_raw = IP_TRUE;
            break;
        case 'N':
            IP_BIT_SET(flags, IP_MSG_DONTWAIT);
            break;
        case 'P':
            IP_BIT_SET(flags, IP_MSG_PEEK);
            break;
        case 'o':
            IP_BIT_SET(flags, IP_MSG_OOB);
            break;
        case 'w':
            IP_BIT_SET(flags, IP_MSG_WAITALL);
            break;
        case 'm':
            use_recvfrom = 1;
            break;
        default:
            goto print_help;
        }
    }

    if (sockno >= 0)
    {
        /* Get fd entry */
        f = ipcom_cmd_socktest_fd_find(fdp, sockno);
        if (f != IP_NULL)
        {
            /* Setup socket address */
            if (length < 0)
            {
                ipcom_printf("     Failed, max length not specified"IP_LF);
                IPCOM_SOCKTEST_LEAVE (IPCOM_ERR_INVALID_ARG);
            }

            buf = ipcom_malloc(length + 1);
            cmsg = ipcom_malloc(SOCKTEST_CMSG_LEN);
            if (buf == IP_NULL || cmsg == IP_NULL)
                IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_NO_MEMORY);

#ifdef IPCOM_USE_FD_DAEMON
            fd = ipcom_fd_daemon_get_local_fd(f->fd);
#else
            fd = f->fd;
#endif

            while (n-- > 0)
            {
                ipcom_memset(buf, 0, length + 1);
                ipcom_memset(&from, 0, sizeof(from));

                /* Try receive...*/
                ipcom_printf("     Waiting to receive"IP_LF);

                if (use_recvfrom)
                {
                    ret = ipcom_recvfrom(fd,
                                         (void *) buf,
                                         length,
                                         flags,
                                         (void *) &from.sa,
                                         &from_len);
                }
                else if (!use_zerocopy)
                {
                    struct Ip_msghdr m;
                    struct Ip_iovec  iov;

                    iov.iov_base = (void *) buf;
                    iov.iov_len  = length;

                    m.msg_name       = &from.sa;
                    m.msg_namelen    = from_len;
                    m.msg_iov        = &iov;
                    m.msg_iovlen     = 1;
                    m.msg_control    = cmsg;
                    m.msg_controllen = SOCKTEST_CMSG_LEN;
                    m.msg_flags      = 0;
                    ret = ipcom_recvmsg(fd, &m, flags);
                    if (ret != IP_SOCKERR)
                        ipcom_cmd_socktest_print_all_ancillary_data(&m);
                    if (0 != m.msg_flags)
                    {
                        ipcom_printf("msg_flags : %d"IP_LF,m.msg_flags);
                    }
                }
                else
#ifndef IPCOM_CMD_SOCKTEST_USE_ZEROCOPY_API
                    ret = -1;
#else
                {
                    ZBUF_ID zbuf_id = zbufSockRecvfrom(
                            fd,     /* socket to receive from */
                            flags,      /* flags to underlying protocols */
                            (int*)&from_len,    /* number of bytes requested/returned */
                            &from.sa,   /* where to copy sender's addr */
                            &ret        /* value/result length of <from> */
                            );

                    if (ret >= 0)
                    {
                        Ipcom_pkt* pkt = _zbuf_getseg_nolock(zbuf_id);
                        ipcom_memcpy(buf, (char *)&pkt->data[pkt->start], ret );
                        zbufDelete(zbuf_id);
                    }
                }
#endif /* IPCOM_CMD_SOCKTEST_USE_ZEROCOPY_API */
                if (ret >= 0)
                {
                    ipcom_printf("     Received %d bytes"IP_LF,ret);
                    if (show_from_addr)
                    {
                        ipcom_printf("     From ");
                        switch (f->domain)
                        {
#ifdef IPCOM_USE_INET
                        case IP_AF_INET:
                            ipcom_printf("%s : %d"IP_LF,
                                          ipcom_inet_ntop(IP_AF_INET,
                                                          &from.sin.sin_addr,
                                                          str_addr, sizeof(str_addr)),
                                          ip_ntohs(from.sin.sin_port));
                            break;
#endif
#ifdef IPCOM_USE_INET6
                        case IP_AF_INET6:
                            ipcom_printf("%s : %d"IP_LF,
                                          ipcom_inet_ntop(IP_AF_INET6,
                                                          &from.sin6.sin6_addr,
                                                          str_addr, sizeof(str_addr)),
                                          ip_ntohs(from.sin6.sin6_port));
                            break;
#endif
                        default:
                            break;
                        }
                    }
                    if (print_raw == IP_TRUE)
                    {
                        ipcom_printf("     Data:");
                        for (i = 0; i < ret; i+=14)
                        {
                            for (j = 0; ((j < 14) && (j + i) < ret); j++)
                                ipcom_printf(" 0x%02x", buf[i+j]);
                            ipcom_printf(IP_LF"          ");
                        }
                    }
                    else
                    {
                        if ((f->domain == IP_AF_PACKET) ||
                            (f->type   == IP_SOCK_RAW))
                            ipcom_cmd_socktest_print_formatted (buf, ret, f);
                        else
                            ipcom_printf("     Data: %s",buf);
                    }
                    ipcom_printf(IP_LF);

                    retval = IPCOM_SUCCESS;
                }
                else
                {
                    retval = IPCOM_ERR_FAILED;
                    break;
                }
            }
        }
        else
        {
            ipcom_printf("     Failed, bad socket number"IP_LF);
            IPCOM_SOCKTEST_LEAVE (IPCOM_ERR_INVALID_ARG);
        }
    }
    else
    {
        ipcom_printf("     Failed, no socket specified"IP_LF);
        IPCOM_SOCKTEST_LEAVE (IPCOM_ERR_INVALID_ARG);
    }

leave:
#ifdef IPCOM_USE_FD_DAEMON
    if (fd != -1)
        ipcom_socketclose(fd);
#endif
    ipcom_free(cmsg);
    ipcom_free(buf);
    return retval;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest %s -s <socket> -l <max length> [-NPmorw] ",
                  use_zerocopy ? "zerocopy_recv" : "receive" IP_LF);
    ipcom_printf("     [-n <packets>]"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -A print the 'from' address"IP_LF);
    if (use_zerocopy)
        ipcom_printf("     -D detach the zero copy buffer from the stack"IP_LF);
    ipcom_printf("     -N set the IP_MSG_DONT_WAIT flag"IP_LF);
    ipcom_printf("     -P set the IP_MSG_PEEK flag"IP_LF);
    ipcom_printf("     -l <max length> specify max receive length"IP_LF);
    ipcom_printf("     -m use recvfrom() instead of recvmsg()"IP_LF);
    ipcom_printf("     -n <packets> specify number of packets"IP_LF);
    ipcom_printf("     -o set the IP_MSG_OOB flag"IP_LF);
    ipcom_printf("     -r print output as raw data"IP_LF);
    ipcom_printf("     -s <socket> specify socket number"IP_LF);
    ipcom_printf("     -w set the IP_MSG_WAITALL flag"IP_LF);
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_flood_shutdown_impl
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_flood_shutdown_impl(Ipcom_cmd_socktest_fd    *f,
                                       char                     *buf,
                                       Ip_size_t                sz)
{
    struct Ip_timeval   tv;
    Ip_fd_set           rfds;
    int                 ret;
    int                 fd;
    Ip_err retval = IPCOM_ERR_FAILED;

    tv.tv_sec   = f->flood_delay;
    tv.tv_usec  = 0;

#ifdef IPCOM_USE_FD_DAEMON
    fd = ipcom_fd_daemon_get_local_fd(f->fd);
#else
    fd = f->fd;
#endif

    if (ipcom_shutdown(fd, IP_SHUT_WR) != 0)
    {
        f->flood_error = ipcom_errno;
        ipcom_snprintf(buf, sz, "     Failed, shutdown error %d"IP_LF, f->flood_error);
        f->flood_errdesc    = ipcom_strdup(buf);
        goto leave;
    }

    for (;;)
    {
        IP_FD_ZERO(&rfds);
        IP_FD_SET(fd, &rfds);

        if (fd == IP_INVALID_SOCKET)
        {
            f->flood_error = IP_ERRNO_ETIMEDOUT;
            ipcom_snprintf(buf, sz, "     Failed, shutdown invalid socket handle"IP_LF);
            f->flood_errdesc    = ipcom_strdup(buf);
            goto leave;
        }

        ret = ipcom_socketselect(fd + 1, &rfds, IP_NULL, IP_NULL, &tv);
        if (ret > 0)
        {
            if (IP_FD_ISSET(fd, &rfds))
            {
                ret = ipcom_recv(fd, buf, sz, 0);
                if (ret < 0)
                {
                    f->flood_error = ipcom_errno;
                    ipcom_snprintf(buf, sz, "     Failed, shutdown receive error %u"IP_LF, f->flood_error);
                    f->flood_errdesc    = ipcom_strdup(buf);
                    goto leave;
                }

                if (ret == 0)
                {
                    retval = IPCOM_SUCCESS;
                    goto leave;
                }

                f->flood_data += ret;
            }
        }
        else if (ret < 0)
        {
            f->flood_error = ipcom_errno;
            ipcom_snprintf(buf, sz, "     Failed, shutdown select error %u"IP_LF, f->flood_error);
            f->flood_errdesc    = ipcom_strdup(buf);
            goto leave;
        }
        else
        {
            f->flood_error = IP_ERRNO_ETIMEDOUT;
            ipcom_snprintf(buf, sz, "     Failed, shutdown timeout error"IP_LF);
            f->flood_errdesc    = ipcom_strdup(buf);
            goto leave;
        }
    }

 leave:
#ifdef IPCOM_USE_FD_DAEMON
    if (fd != -1)
        ipcom_socketclose(fd);
#endif
    return retval;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_flood_acquire
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipcom_cmd_socktest_fd   *
ipcom_cmd_socktest_flood_acquire(void)
{
    Ipcom_cmd_socktest_flood_entry  *entry;
    Ipcom_cmd_socktest_fd           *f  = IP_NULL;

    ipcom_mutex_lock(ipcom_socktest_flood_lock);
    entry = IPCOM_LIST_FIRST(&ipcom_socktest_flood_list);
    if (entry)
    {
        ipcom_list_remove(&entry->next);
        f = entry->f;
    }

    ipcom_mutex_unlock(ipcom_socktest_flood_lock);

    if (f != IP_NULL)
        ipcom_sem_post(f->flood_sem);

    return f;
}

/*
 *===========================================================================
 *                    ipcom_cmd_socktest_flood_proc_impl
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC
IPCOM_PROCESS(ipcom_cmd_socktest_flood_proc_impl)
{
    Ipcom_cmd_socktest_fd   *f;
    char                    *buf;
    Ip_size_t               sz      = 1500;
    int                     fd;
    ipcom_proc_init();

    buf = ipcom_malloc(sz);

    if (buf != IP_NULL && (f = ipcom_cmd_socktest_flood_acquire()) != IP_NULL)
    {
        Ip_err              retval = IPCOM_SUCCESS;
#ifdef IPCOM_USE_FD_DAEMON
        fd = ipcom_fd_daemon_get_local_fd(f->fd);
#else
        fd = f->fd;
#endif

        for (;;)
        {
            Ip_fd_set           rfds;
            Ip_fd_set           wfds;
            struct Ip_timeval   tv;
            int                 ret;

            if (f->fd == IP_INVALID_SOCKET)
            {
                f->flood_error     = IP_ERRNO_ETIMEDOUT;
                ipcom_snprintf(buf, sz, "     Failed, invalid file descriptor"IP_LF);
                f->flood_errdesc    = ipcom_strdup(buf);
                retval      = IPCOM_ERR_FAILED;
                break;
            }

            IP_FD_ZERO(&rfds);
            IP_FD_ZERO(&wfds);
            tv.tv_sec   = 0;
            tv.tv_usec  = 1000;

            IP_FD_SET(fd, &rfds);
            if (IP_BIT_ISSET(f->flood_flags, SOCKTEST_FLOOD_BURST|SOCKTEST_FLOOD_SOURCE))
            {
                ipcom_memset(buf, 0xa, sz);

                IP_FD_SET(fd, &wfds);
                if (IP_BIT_ISSET(f->flood_flags, SOCKTEST_FLOOD_BURST))
                {
                    /* Only BURST; no verification; send in the timeout. */
                    if (IP_BIT_ISSET(f->flood_flags, SOCKTEST_FLOOD_SEND|SOCKTEST_FLOOD_VERIFY))
                    {
                        ret = ipcom_socketselect(fd + 1, &rfds, &wfds, IP_NULL, &tv);
                    }
                    else
                    {
                        tv.tv_usec = 200000;
                        ret = ipcom_socketselect(fd + 1, &rfds, IP_NULL, IP_NULL, &tv);
                        IP_BIT_SET(f->flood_flags, SOCKTEST_FLOOD_SEND);
                    }
                }
                else
                {
                    ret = ipcom_socketselect(fd + 1, &rfds, &wfds, IP_NULL, &tv);
                }

                if (ret < 0)
                {
                    f->flood_error = ipcom_errno;
                    ipcom_snprintf(buf, sz, "     Failed, source select error %u"IP_LF, f->flood_error);
                    f->flood_errdesc    = ipcom_strdup(buf);
                    IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
                }

                if (IP_BIT_ISSET(f->flood_flags, SOCKTEST_FLOOD_SHUTDOWN) || IP_FD_ISSET(fd, &rfds))
                    IPCOM_SOCKTEST_LEAVE(ipcom_cmd_socktest_flood_shutdown_impl(f, buf, sz));

                if (ret == 0)
                    continue;

                if (IP_FD_ISSET(fd, &wfds))
                {
                    IP_BIT_CLR(f->flood_flags, SOCKTEST_FLOOD_SEND);
                    ret = ipcom_send(fd, buf, sz, IP_MSG_DONTWAIT);
                    if (ret < 0)
                    {
                        if (ipcom_errno == IP_ERRNO_EWOULDBLOCK)
                            continue;
                        else
                        {
                            f->flood_error = ipcom_errno;
                            ipcom_snprintf(buf, sz, "     Failed, source send error %u"IP_LF, f->flood_error);
                            f->flood_errdesc    = ipcom_strdup(buf);
                            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
                        }
                    }
                    else
                    {
                        /* CHECK ANY VERIFICATION */
                        if (IP_BIT_ISSET(f->flood_flags, SOCKTEST_FLOOD_VERIFY))
                        {
                            f->flood_burst -= IP_MIN(f->flood_burst, (Ip_size_t) ret);
                            if (f->flood_burst == 0)
                                IP_BIT_CLR(f->flood_flags, SOCKTEST_FLOOD_VERIFY);
                        }

                        f->flood_data += ret;
                    }
                }
            }
            else
            {
                ret = ipcom_socketselect(fd + 1, &rfds, IP_NULL, IP_NULL, &tv);
                if (ret < 0)
                {
                    f->flood_error = ipcom_errno;
                    ipcom_snprintf(buf, sz, "     Failed, sink select error %u"IP_LF, f->flood_error);
                    f->flood_errdesc    = ipcom_strdup(buf);
                    IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
                }

                if (IP_BIT_ISSET(f->flood_flags, SOCKTEST_FLOOD_SHUTDOWN))
                    IPCOM_SOCKTEST_LEAVE(ipcom_cmd_socktest_flood_shutdown_impl(f, buf, sz));

                if (ret == 0)
                    continue;

                if (IP_FD_ISSET(fd, &rfds))
                {
                    ret = ipcom_recv(fd, buf, sz, 0);
                    if (ret < 0)
                    {
                        f->flood_error = ipcom_errno;
                        ipcom_snprintf(buf, sz, "     Failed, sink recv error %u"IP_LF, f->flood_error);
                        f->flood_errdesc    = ipcom_strdup(buf);
                        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
                    }

                    if (ret == 0)
                    {
                        if (ipcom_shutdown(fd, IP_SHUT_WR) != 0)
                        {
                            f->flood_error = ipcom_errno;
                            ipcom_snprintf(buf, sz, "     Failed, sink shutdown error %u"IP_LF, f->flood_error);
                            f->flood_errdesc    = ipcom_strdup(buf);
                            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
                        }
                        break;
                    }

                    /* CHECK ANY VERIFICATION */
                    if (IP_BIT_ISSET(f->flood_flags, SOCKTEST_FLOOD_VERIFY))
                    {
                        f->flood_burst -= IP_MIN(f->flood_burst, (Ip_size_t) ret);
                        if (f->flood_burst == 0)
                            IP_BIT_CLR(f->flood_flags, SOCKTEST_FLOOD_VERIFY);
                    }

                    f->flood_data += ret;
                }

            }
        }

leave:
#ifdef IPCOM_USE_FD_DAEMON
        ipcom_socketclose(fd);
#endif

        f->flood_ret    = retval;
        ipcom_sem_post(f->flood_sem);
    }

    if (buf != IP_NULL)
        ipcom_free(buf);

    ipcom_proc_exit();
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_flood_closesocket_impl
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_flood_closesocket_impl(Ipcom_cmd_socktest_glob *glob,
                                          Ipcom_cmd_socktest_fd   *f)
{
    (void) glob;

    if (f->flood_flags == 0)
    {
        ipcom_printf("     Failed, socket is not sink/source"IP_LF);
        return IPCOM_ERR_INVALID_ARG;
    }

    if (IP_BIT_ISSET(f->flood_flags, SOCKTEST_FLOOD_SHUTDOWN))
    {
        ipcom_printf("     Failed, sink/source already shutdown"IP_LF);
        return IPCOM_ERR_INVALID_ARG;
    }

    IP_BIT_SET(f->flood_flags, SOCKTEST_FLOOD_SHUTDOWN);
    ipcom_sem_wait(f->flood_sem);

    /* Clear the flood flags */
    f->flood_flags  = 0;
    if (f->flood_errdesc)
    {
        ipcom_printf(f->flood_errdesc);
        ipcom_free(f->flood_errdesc);
    }
    ipcom_sem_delete(&f->flood_sem);
    return IPCOM_SUCCESS;
}

/*
 *===========================================================================
 *                    ipcom_cmd_socktest_recv_impl
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_flood_impl(Ipcom_cmd_socktest_glob *glob,
                              Ipcom_cmd_socktest_fd   *fdp,
                              int                     argc,
                              char                    **argv)
{
    Ip_err                          retval          = IPCOM_SUCCESS;
    Ipcom_getopt                    o;
    Ipcom_cmd_socktest_fd          *f               = IP_NULL;
    int                             sockno          = -1;
    int                             opt;
    int                             what            = 0;
    Ip_size_t                       burst           = 10 * 1024;    /* 10 K */
    int                             delay           = 5;
    Ip_bool                         delay_updated   = IP_FALSE;
    char                            *wdesc          = "start|stop|burst|verify";

    (void)glob;

    if (argc < 3)
        goto print_help;

    if (ipcom_strcasecmp(argv[1], "source") == 0)
    {
        what    = SOCKTEST_FLOOD_SOURCE;
        wdesc   = "source";
    }
    else if (ipcom_strcasecmp(argv[1], "sink") == 0)
    {
        what    = SOCKTEST_FLOOD_SINK;
        wdesc   = "sink";
    }
    else if (ipcom_strcasecmp(argv[1], "stop") == 0)
    {
        what    = SOCKTEST_FLOOD_SHUTDOWN;
        wdesc   = "stop";
    }
    else if (ipcom_strcasecmp(argv[1], "burst") == 0)
    {
        what    = SOCKTEST_FLOOD_BURST|SOCKTEST_FLOOD_SOURCE;
        wdesc   = "burst";
    }
    else if (ipcom_strcasecmp(argv[1], "verify") == 0)
    {
        what    = SOCKTEST_FLOOD_VERIFY;
        wdesc   = "verify";
    }
    else
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc - 1, argv + 1, "s:w:b:", &o)) != -1)
    {
        switch (opt)
        {
        case 's':
            sockno = ipcom_cmd_socktest_get_sockno(o.optarg);
            break;
        case 'w':
            delay           = ipcom_atoi(o.optarg);
            delay_updated   = IP_TRUE;
            break;
        case 'b':
            burst = ipcom_atoi(o.optarg);
            break;
        default:
            break;
        }
    }

    if (sockno >= 0)
    {
        /* Get fd entry */
        f = ipcom_cmd_socktest_fd_find(fdp, sockno);
        if (f != IP_NULL)
        {
            if (IP_BIT_ISFALSE(what, SOCKTEST_FLOOD_VERIFY))
            {
                if (IP_BIT_ISSET(what, SOCKTEST_FLOOD_SINK|SOCKTEST_FLOOD_SOURCE))
                {
                    if (f->flood_flags != 0)
                    {
                        ipcom_printf("     Failed, sink/source already started"IP_LF);
                        IPCOM_SOCKTEST_LEAVE (IPCOM_ERR_INVALID_ARG);
                    }
                    else
                    {
                        Ipcom_cmd_socktest_flood_entry  entry;
                        char        floodname[128];
                        Ip_pid_t    pid;

                        f->flood_delay      = delay;
                        f->flood_burst      = burst;
                        f->flood_errdesc    = IP_NULL;

                        ipcom_mutex_lock(ipcom_socktest_flood_lock);
                        entry.f = f;
                        ipcom_list_insert_first(&ipcom_socktest_flood_list, &entry.next);
                        ipcom_mutex_unlock(ipcom_socktest_flood_lock);

                        /* Always start with verification */
                        f->flood_flags = what|SOCKTEST_FLOOD_VERIFY;
                        ipcom_sem_create(&f->flood_sem, 0);
                        ipcom_sprintf(floodname, "socktest_flood_%u", f->fd);

                        /* Spawn session handler */
                        if (ipcom_proc_create(floodname, ipcom_cmd_socktest_flood_proc_impl,
                                              IPCOM_PROC_STACK_LARGE, &pid) != IPCOM_SUCCESS)
                        {
                            int err = ipcom_errno;
                            ipcom_printf("Cannot spawn flood handler: %s"IP_LF,
                                       ipcom_strerror(ipcom_errno));

                            ipcom_mutex_lock(ipcom_socktest_flood_lock);
                            ipcom_list_remove(&entry.next);
                            ipcom_mutex_unlock(ipcom_socktest_flood_lock);

                            ipcom_errno = err;
                            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
                        }

                        ipcom_sem_wait(f->flood_sem);
                    }
                }
                else
                {
                    /* Change delay flag */
                    if (delay_updated)
                        f->flood_delay = delay;

                    retval = ipcom_cmd_socktest_flood_closesocket_impl(glob, f);
                    if (retval == IPCOM_SUCCESS)
                    {
                        /* Store the return values */
                        retval          = f->flood_ret;
                        ipcom_errno     = f->flood_error;
                    }
                }
            }
            else
            {
                if (f->flood_flags == 0)
                {
                    ipcom_printf("     Failed, socket is not sink/source"IP_LF);
                    IPCOM_SOCKTEST_LEAVE (IPCOM_ERR_INVALID_ARG);
                }

                if (IP_BIT_ISSET(f->flood_flags, SOCKTEST_FLOOD_SHUTDOWN))
                {
                    ipcom_printf("     Failed, sink/source already shutdown"IP_LF);
                    IPCOM_SOCKTEST_LEAVE (IPCOM_ERR_INVALID_ARG);
                }

                /* Set the info */
                if (IP_BIT_ISFALSE(f->flood_flags, SOCKTEST_FLOOD_VERIFY))
                {
                    f->flood_burst = burst;
                    IP_BIT_SET(f->flood_flags, SOCKTEST_FLOOD_VERIFY);
                }

                /* Wait until verify flag has been cleared */
                while (f->flood_ret == 0 && delay-- > 0)
                {
                    ipcom_sleep(1);
                    if (IP_BIT_ISFALSE(f->flood_flags, SOCKTEST_FLOOD_VERIFY))
                        IPCOM_SOCKTEST_LEAVE (IPCOM_SUCCESS);
                }

                ipcom_printf("     Failed, sink/source seems dead"IP_LF);
                IPCOM_SOCKTEST_LEAVE (IPCOM_ERR_TIMEOUT);
            }
        }
        else
        {
            ipcom_printf("     Failed, bad socket number"IP_LF);
            IPCOM_SOCKTEST_LEAVE (IPCOM_ERR_INVALID_ARG);
        }
    }
    else
    {
        ipcom_printf("     Failed, no socket specified"IP_LF);
        IPCOM_SOCKTEST_LEAVE (IPCOM_ERR_INVALID_ARG);
    }

leave:
    if (retval == IPCOM_SUCCESS && f != IP_NULL)
    {
        if (IP_BIT_ISSET(what, SOCKTEST_FLOOD_VERIFY|SOCKTEST_FLOOD_SHUTDOWN))
            ipcom_printf("     Success, %u bytes processed"IP_LF, f->flood_data);
    }

    return retval;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest flood %s -s <socket> [-w <delay>] [-b <burst size>]"IP_LF, wdesc);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -s Socket descriptor"IP_LF);
    ipcom_printf("     -w How long to wait for an operation to complete"IP_LF);
    ipcom_printf("     -b burst size when using bursts and verification"IP_LF);
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_flood
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_flood(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    return ipcom_cmd_socktest_flood_impl(glob, fdp, argc, argv);
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_receive
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_receive(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    return ipcom_cmd_socktest_recv_impl(glob, fdp, argc, argv, IP_FALSE);
}


#ifdef IPCOM_CMD_SOCKTEST_USE_ZEROCOPY_API
/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_zerocopy_recv
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_zerocopy_recv(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    return ipcom_cmd_socktest_recv_impl(glob, fdp, argc, argv, IP_TRUE);
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_pkt_freefunc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_cmd_socktest_pkt_freefunc(void *data, void *cookie)
{
    (void)data;
#ifdef IP_PORT_OSE
    ipcom_free(cookie);
#else
    ipcom_pkt_buffer_free(cookie);
#endif
}
#endif /* IPCOM_CMD_SOCKTEST_USE_ZEROCOPY_API */


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_send_impl
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_send_impl(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv, Ip_bool use_zerocopy)
{
    Ip_err                          retval;
    Ipcom_getopt                    o;
    Ipcom_cmd_socktest_fd          *f;
    union Ipcom_cmd_socktest_addr   addr;
    struct Ip_sockaddr             *p_sa;
    Ip_socklen_t                    sa_len;
    char                           *p_addr = IP_NULL;
    char                           *p_port = IP_NULL;
    char                           *buf    = IP_NULL;
    int                             buf_len = 0;
    Ip_u8                           *hex_data = IP_NULL;
    int                             sockno = -1;
    int                             flags  =  0;
    int                             opt;
    int                             ret = -1;
    int                             repeat_data = 1;
#ifdef IPCOM_CMD_SOCKTEST_USE_ZEROCOPY_API
    char                           *zerocopy_buf = IP_NULL;
#endif
    char                           *next_hop = IP_NULL;
#ifdef IPCOM_USE_INET6
    Ip_u8                           cmsg_buf[IP_CMSG_SPACE(sizeof(union Ip_sockaddr_union))
                                             + IP_CMSG_SPACE(sizeof(struct Ip_in6_pktinfo))];
#else
    Ip_u8                           cmsg_buf[IP_CMSG_SPACE(sizeof(union Ip_sockaddr_union))
                                             + IP_CMSG_SPACE(sizeof(struct Ip_in_pktinfo))];
#endif
    struct Ip_cmsghdr              *cmsg = (struct Ip_cmsghdr *) cmsg_buf;
    Ip_socklen_t                    cmsg_len = 0;
    char                           *pktinfo = IP_NULL;
    int                             fd = -1;
#ifdef IPSCTP
    char                           *sndrcvbuf = IP_NULL;
    struct Ipsctp_sndrcvinfo       *sndrcvinfo = IP_NULL;
#endif


    (void)glob;

    if (argc == 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "s:a:p:d:f:n:omr:P:", &o)) != -1)
    {
        switch (opt)
        {
        case 's':
            sockno = ipcom_cmd_socktest_get_sockno(o.optarg);
            break;
        case 'a':
            p_addr = o.optarg;
            break;
        case 'p':
            p_port = o.optarg;
            break;
        case 'd':
            buf = o.optarg;
            buf_len = ipcom_strlen(buf);
            break;
        case 'f':
            flags = ipcom_atoi(o.optarg);
            break;
        case 'n':
            next_hop = o.optarg;
            break;
        case 'm':
            IP_BIT_SET(flags, IP_MSG_MORE);
            break;
        case 'o':
            IP_BIT_SET(flags, IP_MSG_OOB);
            break;
        case 'r':
            repeat_data = ipcom_atoi(o.optarg);
            break;
        case 'P':
            pktinfo = o.optarg;
            break;
#ifdef IPSCTP
        case 'i':
            sndrcvbuf = o.optarg;
            break;
#endif
        default:
            goto print_help;
        }
    }

    if (sockno >= 0)
    {
        /* Get fd entry */
        f = ipcom_cmd_socktest_fd_find(fdp, sockno);
        if (f != IP_NULL)
        {
            /* Setup socket address */
            if (p_addr != IP_NULL
                && ipcom_getsockaddrbyaddr(f->domain, p_addr, (void *) &addr.sa) < 0)
            {
                ipcom_printf("     Failed, invalid address %s"IP_LF,p_addr);
                IPCOM_SOCKTEST_LEAVE (IPCOM_ERR_INVALID_ARG);
            }

            if (ipcom_cmd_socktest_portinfo_add (f->domain, &addr.sa, p_port) != IPCOM_SUCCESS)
                IPCOM_SOCKTEST_LEAVE (IPCOM_ERR_INVALID_ARG);

            if (buf == IP_NULL)
            {
                ipcom_printf("     Failed, data not specified"IP_LF);
                IPCOM_SOCKTEST_LEAVE (IPCOM_ERR_INVALID_ARG);
            }

            if (buf_len > 2 && buf[0] == '0' && buf[1] == 'x')
            {
                int i;
                int r;
                unsigned int b;
                Ip_u16 *inchecksum_ptr = IP_NULL;

                buf_len = (buf_len - 2) / 2;

                /* Treat data as hex numbers */
                hex_data = ipcom_malloc(buf_len * repeat_data);
                if (hex_data == IP_NULL && buf_len * repeat_data > 0)
                {
                    ipcom_printf("     Failed, out of memory"IP_LF);
                    IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_NO_MEMORY);
                }

                for (r = 0; r < repeat_data; r++)
                    for (i = 0; i < buf_len; i++)
                    {
                        if (buf[2 * i + 2] != '#')
                        {
                            /* RTCore requires the string to be null-terminated... */
                            char bbuf[3];
                            bbuf[0] = buf[2 * i + 2];
                            bbuf[1] = buf[2 * i + 3];
                            bbuf[2] = '\0';
                            (void)ipcom_sscanf(bbuf, "%02x", &b);
                        }
                        else
                        {
                            if (inchecksum_ptr == IP_NULL)
                                inchecksum_ptr = (Ip_u16 *) &hex_data[i + r * buf_len];
                            b = 0;
                        }
                        hex_data[i + r * buf_len] = (Ip_u8) b;
                    }
                buf = (char *) hex_data;
                buf_len *= repeat_data;
                if (inchecksum_ptr != IP_NULL)
                    *inchecksum_ptr = ipcom_in_checksum(buf, (Ip_size_t)buf_len);
            }

            if (pktinfo != IP_NULL)
            {
                int pktinfo_len;

                if (f->domain == IP_AF_INET)
                {
                    pktinfo_len = ipcom_cmd_socktest_str_to_in_pktinfo(pktinfo, cmsg);
                    cmsg->cmsg_level = IP_IPPROTO_IP;
                    cmsg->cmsg_type  = IP_IP_PKTINFO;
                }
                else
                {
                    pktinfo_len = ipcom_cmd_socktest_str_to_in6_pktinfo(pktinfo, cmsg);
                    cmsg->cmsg_level = IP_IPPROTO_IPV6;
                    cmsg->cmsg_type  = IP_IPV6_PKTINFO;
                }

                if (pktinfo_len)
                {
                    cmsg->cmsg_len = IP_CMSG_LEN(pktinfo_len);
                    cmsg_len += IP_CMSG_SPACE(pktinfo_len);
                    cmsg = (struct Ip_cmsghdr*) ((Ip_u8*) cmsg + IP_CMSG_SPACE(pktinfo_len));
                }
            }

            if (next_hop != IP_NULL)
            {
                cmsg->cmsg_len = IP_CMSG_LEN(sizeof(union Ip_sockaddr_union));
                if (f->domain == IP_AF_INET)
                {
                    cmsg->cmsg_level = IP_IPPROTO_IP;
                    cmsg->cmsg_type  = IP_IP_NEXTHOP;
                }
                else
                {
                    cmsg->cmsg_level = IP_IPPROTO_IPV6;
                    cmsg->cmsg_type  = IP_IPV6_NEXTHOP;
                }
                if (ipcom_getsockaddrbyaddr(f->domain, o.optarg, IP_CMSG_DATA(cmsg)) != IPCOM_SUCCESS)
                {
                    ipcom_printf("     Failed, invalid address %s"IP_LF, o.optarg);
                    IPCOM_SOCKTEST_LEAVE (IPCOM_ERR_INVALID_ARG);
                }
                cmsg = (struct Ip_cmsghdr *) ((Ip_u8 *) cmsg + IP_CMSG_SPACE(sizeof(union Ip_sockaddr_union)));
                cmsg_len += IP_CMSG_SPACE(sizeof(union Ip_sockaddr_union));
            }
#ifdef IPSCTP
            /* for sctp only */
            if (sndrcvbuf != IP_NULL)
            {
                Ipcom_cmd_socktest_sockopt    *sockopt;
                Ipcom_cmd_socktest_sock_level *level = IP_NULL;
                char                           getopt = IP_FALSE;

                cmsg->cmsg_len = IP_CMSG_LEN(sizeof(struct Ipsctp_sndrcvinfo));
                level = ipcom_cmd_socktest_param_parse("sctp",
                            sizeof(Ipcom_cmd_socktest_sock_level),
                            socktest_sock_level,
                            SOCKTEST_LEVEL_ENTRIES(socktest_sock_level));

                if (level == IP_NULL)
                {
                    ipcom_printf("     Failed, invalid level"IP_LF);
                    goto print_help;
                }
                sockopt = ipcom_cmd_socktest_param_parse("default_send_param",
                           sizeof(Ipcom_cmd_socktest_sockopt),
                           level->opt_list,
                           level->opt_list_len);
                if (sockopt == IP_NULL)
                {
                    ipcom_printf("     Failed, invalid option"IP_LF);
                    goto print_help;
                }

                /* Parse option value */
                sndrcvinfo = sockopt->type->parse_cb(sndrcvbuf, &getopt);

                if (sndrcvinfo == IP_NULL)
                {
                    ipcom_printf("     Failed."IP_LF);
                    IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
                }
                cmsg->cmsg_level = IP_IPPROTO_SCTP;
                cmsg->cmsg_type  = IP_IP_NEXTHOP;
                cmsg_len += IP_CMSG_SPACE(sizeof(struct Ipsctp_sndrcvinfo));
                (void)ipcom_memcpy(IP_CMSG_DATA(cmsg),sndrcvinfo,sizeof(struct Ipsctp_sndrcvinfo));
                ipcom_free(sndrcvinfo);
            }
#endif
            p_sa = p_addr == IP_NULL ? IP_NULL : &addr.sa;
            sa_len = p_addr == IP_NULL ? 0 : IPCOM_SA_LEN_GET(&addr.sa);
#ifdef IPCOM_USE_FD_DAEMON
            fd = ipcom_fd_daemon_get_local_fd(f->fd);
#else
            fd = f->fd;
#endif

            if (!use_zerocopy)
            {
                struct Ip_msghdr m;
                struct Ip_iovec  iov;

                iov.iov_base = buf;
                iov.iov_len  = buf_len;

                m.msg_name       = p_sa;
                m.msg_namelen    = sa_len;
                m.msg_iov        = &iov;
                m.msg_iovlen     = 1;
                m.msg_control    = cmsg_len ? cmsg_buf : IP_NULL;
                m.msg_controllen = cmsg_len;
                m.msg_flags      = 0;

                ret = ipcom_sendmsg(fd, &m, flags);
            }
#ifdef IPCOM_CMD_SOCKTEST_USE_ZEROCOPY_API
            else
            {
                /* Use the zero copy API */
                zerocopy_buf = ipcom_malloc(buf_len);
                if (zerocopy_buf == IP_NULL)
                {
                    ipcom_printf("     Failed, out of memory"IP_LF);
                    IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_NO_MEMORY);
                }
                ipcom_memcpy(zerocopy_buf, buf, buf_len);

                ret = zbufSockBufSend(fd, zerocopy_buf,
                                      buf_len, 0, 0, 0);
                if (ret >= 0) /* Will be release by the stack */
                    zerocopy_buf = IP_NULL;
            }
#else
            else
                ret = -1;

#endif /* IPCOM_CMD_SOCKTEST_USE_ZEROCOPY_API */

            if (ret < 0)
                IPCOM_SOCKTEST_LEAVE (IPCOM_ERR_FAILED);
            else if (ret != buf_len)
            {
                ipcom_printf("     Only %d bytes sent."IP_LF, ret);
                IPCOM_SOCKTEST_LEAVE (IPCOM_ERR_FAILED);
            }
            else
                ipcom_printf("     Sent %d bytes"IP_LF, ret);
        }
        else
        {
            ipcom_printf("     Failed, bad socket number"IP_LF);
            IPCOM_SOCKTEST_LEAVE (IPCOM_ERR_INVALID_ARG);
        }
    }
    else
    {
        ipcom_printf("     Failed, no socket specified"IP_LF);
        IPCOM_SOCKTEST_LEAVE (IPCOM_ERR_INVALID_ARG);
    }

    retval = IPCOM_SUCCESS;

 leave:
#ifdef IPCOM_USE_FD_DAEMON
    if (fd != -1)
        ipcom_socketclose(fd);
#endif

    if (hex_data)
        ipcom_free(hex_data);
#ifdef IPCOM_CMD_SOCKTEST_USE_ZEROCOPY_API
    if (zerocopy_buf)
        ipcom_free(zerocopy_buf);
/*        ipcom_pkt_buffer_free(zerocopy_buf); */
#endif

    return retval;

 print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest %s -s <socket> -d <data> [-f <flags>] [-a <to address>] [-p <to port>]"IP_LF, use_zerocopy ? "zerocopy_send" : "send");
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -s specify socket number"IP_LF);
    ipcom_printf("     -d specify data to send"IP_LF);
    ipcom_printf("     -f specify flags"IP_LF);
    ipcom_printf("     -a specify to address"IP_LF);
    ipcom_printf("     -p specify to port"IP_LF);
    ipcom_printf("     -n specify next hop address"IP_LF);
    ipcom_printf("     -m set the IP_MSG_MORE flag"IP_LF);
    ipcom_printf("     -o set the IP_MSG_OOB flag"IP_LF);
    ipcom_printf("     -r <N> repeat the data N times"IP_LF);
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_attach
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_attach(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    Ipcom_getopt       o;
    Ipcom_cmd_socktest_fd *f;
    int                sockno = -1;
    int                domain = -1;
    int                type   = -1;
    int                proto  = -1;
    int                opt;
    int                val;
    int                ret;
    Ip_err             retval;
    Ip_socklen_t       optlen;
#ifdef IP_PORT_OSE5
    int                old_fd, new_fd;
#endif

    (void)glob;

    if (argc == 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "d:t:p:N:s:", &o)) != -1)
    {
        switch (opt)
        {
        case 's':
            sockno = ipcom_cmd_socktest_get_sockno(o.optarg);
            break;

        case 'd':
            domain = ipcom_cmd_socktest_param_to_int(o.optarg,
                                                    socktest_domain,
                                                     SOCKTEST_PARAM_ENTRIES(socktest_domain));
            break;

        case 't':
            type   = ipcom_cmd_socktest_param_to_int(o.optarg,
                                                     socktest_type,
                                                     SOCKTEST_PARAM_ENTRIES(socktest_type));
            break;

        case 'p':
            proto  = ipcom_cmd_socktest_param_to_int(o.optarg,
                                                     socktest_proto,
                                                     SOCKTEST_PARAM_ENTRIES(socktest_proto));
            break;

        case 'N':
            break;

        default:
            goto print_help;
        }
    }

#ifdef IP_PORT_OSE5
    ipcom_printf("Waiting for socket to pid = 0x%lx"IP_LF, ipcom_getpid());
    old_fd = efs_receive_fd(0);
    ipcom_printf("     Received socket %d -> %d from other process"IP_LF, old_fd, sockno);
#if defined(IP_TARGET_SOFT) || defined(softOSE)
    new_fd = zzdup2(old_fd, sockno);
#else
    new_fd = dup2(old_fd, sockno);
#endif
    (void)ipcom_socketclose(old_fd);
    if (new_fd != sockno)
    {
        ipcom_printf("     Failed, socket %d already exists"IP_LF, sockno);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }
#endif /* IP_PORT_OSE5 */

    f = ipcom_cmd_socktest_fd_find(fdp, sockno);
    if (f)
    {
        ipcom_printf("     Failed, socket %d already exists"IP_LF, sockno);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    optlen = sizeof(int);
    ret = ipcom_getsockopt(sockno, IP_SOL_SOCKET, IP_SO_TYPE, (void *) &val, &optlen);
    if (ret < 0)
    {
        ipcom_printf("     Failed, invalid socket descriptor"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);
    }

    f = ipcom_cmd_socktest_fd_find(fdp, -1);
    if (f == IP_NULL)
    {
        ipcom_printf("     Failed, socktest socket list full"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    f->fd       = sockno;
    f->domain   = domain;
    f->type     = type;
    f->prot     = proto;
    SOCKTEST_FLOOD_INIT(f);

    ipcom_printf("     Attached %d"IP_LF, f->fd);
    retval = IPCOM_SUCCESS;

leave:
    return retval;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest attach -s <socket> -d <domain> -t <type> -p <protocol>"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -s specify socket number"IP_LF);
    ipcom_printf("     -d <domain>"IP_LF);
    ipcom_cmd_socktest_param_list_print(socktest_domain, SOCKTEST_PARAM_ENTRIES(socktest_domain), sizeof(Ipcom_cmd_socktest_param));
    ipcom_printf(IP_LF);
    ipcom_printf("     -t <type>"IP_LF);
    ipcom_cmd_socktest_param_list_print(socktest_type, SOCKTEST_PARAM_ENTRIES(socktest_type), sizeof(Ipcom_cmd_socktest_param));
    ipcom_printf(IP_LF);
    ipcom_printf("     -p <protocol>"IP_LF);
    ipcom_cmd_socktest_param_list_print(socktest_proto, SOCKTEST_PARAM_ENTRIES(socktest_proto), sizeof(Ipcom_cmd_socktest_param));
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_if_nametoindex
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_if_nametoindex(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    (void) glob;
    (void) fdp;
    if (argc != 2)
        goto print_help;

    ipcom_printf("%d"IP_LF, (int)ipcom_if_nametoindex(argv[1]));
    return 0;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest if_nametoindex <ifname>"IP_LF);
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_socket_eventcb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_cmd_socktest_socket_eventcb(struct Ipcom_socket_struct *sock,
                                  IP_CONST Ipcom_pkt *pkt,
                                  enum Ipcom_socket_event_type event)
{
    Ipcom_cmd_socktest_fd      *f;
    Ipcom_cmd_socktest_soevent *soevent;

    IPCOM_UNUSED_ARG(pkt);

    f = ipcom_cmd_socktest_fd_find(socktest_fd, sock->fd);
    if (f == IP_NULL)
        IP_PANIC();

    soevent = ipcom_malloc(sizeof(*soevent));
    if (soevent != IP_NULL)
    {
        soevent->type = event;
        ipcom_mutex_lock(f->events_mutex);
        ipcom_list_insert_last(&f->events, &soevent->list);
        ipcom_mutex_unlock(f->events_mutex);
        (void)ipcom_sem_post(f->event_sem);
    }
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_soevent_monitor
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_soevent_monitor(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    Ipcom_getopt           o;
    int                    opt;
    Ipcom_cmd_socktest_fd *f = IP_NULL;
    int                    sockno = -1;
    Ip_bool                done = IP_FALSE;
    Ip_err                 retval;
    Ipcom_socket_eventcb   eventcb = ipcom_cmd_socktest_socket_eventcb;
    int                    fd = -1;

    (void)glob;

    if (argc == 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "s:a:p:d:f:n:omr:", &o)) != -1)
    {
        switch (opt)
        {
        case 's':
            sockno = ipcom_cmd_socktest_get_sockno(o.optarg);
            break;
        default:
            goto print_help;
        }
    }

    if (sockno < 0)
    {
        ipcom_printf("     Failed, no socket specified"IP_LF);
        IPCOM_SOCKTEST_LEAVE (IPCOM_ERR_INVALID_ARG);
    }

    /* Get fd entry */
    f = ipcom_cmd_socktest_fd_find(fdp, sockno);
    if (f == IP_NULL)
    {
        ipcom_printf("     Failed, bad socket number"IP_LF);
        IPCOM_SOCKTEST_LEAVE (IPCOM_ERR_INVALID_ARG);
    }

    ipcom_sem_create(&f->event_sem, 0);
    ipcom_mutex_create(&f->events_mutex);
    ipcom_list_init(&f->events);

#ifdef IPCOM_USE_FD_DAEMON
    fd = ipcom_fd_daemon_get_local_fd(f->fd);
#else
    fd = f->fd;
#endif

    if (ipcom_setsockopt(fd,
                         IP_SOL_SOCKET,
                         IP_SO_X_EVENTCB_RO,
                         (void *) &eventcb,
                         sizeof(eventcb)) < 0)
    {
        ipcom_printf("     Failed, socket event callbacks not supported"IP_LF);
        IPCOM_SOCKTEST_LEAVE (IPCOM_ERR_INVALID_ARG);
    }

    while (!done)
    {
        Ipcom_cmd_socktest_soevent *soevent;
        const char                 *event_desc = "";

        if (ipcom_sem_wait(f->event_sem) != IPCOM_SUCCESS)
            IP_PANIC();
        ipcom_mutex_lock(f->events_mutex);
        soevent = IPCOM_LIST_FIRST(&f->events);
        if (soevent == IP_NULL)
            IPCOM_SOCKTEST_LEAVE (IPCOM_ERR_INVALID_ARG);
        ipcom_list_remove(&soevent->list);
        ipcom_mutex_unlock(f->events_mutex);

        switch (soevent->type)
        {
        case IP_SOEVENT_CB_INPKT:
            event_desc = "INPKT"IP_LF;
            break;
        case IP_SOEVENT_CB_OUTPKT:
            event_desc = "OUTPKT"IP_LF;
            break;
        case IP_SOEVENT_CB_CLOSE:
            event_desc = "CLOSE"IP_LF;
            done = IP_TRUE;
            break;
        case IP_SOEVENT_CB_ERRNO:
            event_desc = "ERRNO"IP_LF;
            break;
        case IP_SOEVENT_CB_ACCEPT:
            event_desc = "ACCEPT"IP_LF;
            break;
        case IP_SOEVENT_CB_CONNECT:
            event_desc = "CONNECT"IP_LF;
            break;
        case IP_SOEVENT_CB_TIMED_OUT:
            event_desc = "TIMED_OUT"IP_LF;
            break;
        case IP_SOEVENT_CB_ACK:
            event_desc = "ACK"IP_LF;
            break;
        case IP_SOEVENT_CB_PKTAVAIL:
            event_desc = "PKTAVAIL"IP_LF;
            break;
        case IP_SOEVENT_CB_TCP_STATE:
            event_desc = "TCP_STATE"IP_LF;
            break;
        case IP_SOEVENT_CB_OOB:
            event_desc = "OOB"IP_LF;
            break;
        case IP_SOEVENT_CB_SELREAD:
            event_desc = "SELREAD"IP_LF;
            break;
        case IP_SOEVENT_CB_SELWRITE:
            event_desc = "SELWRITE"IP_LF;
            break;
        case IP_SOEVENT_CB_SELEXCEPT:
            event_desc = "SELEXCEPT"IP_LF;
            break;
        default:
            IP_PANIC();
            break;
        }
        ipcom_printf(event_desc);
        ipcom_free(soevent);
    }

    retval = IPCOM_SUCCESS;

 leave:
#ifdef IPCOM_USE_FD_DAEMON
    if (fd != -1)
        ipcom_socketclose(fd);
#endif
    if (f != IP_NULL)
    {
        ipcom_sem_delete(&f->event_sem);
        ipcom_mutex_delete(&f->events_mutex);
        f->event_sem = IPCOM_SEM_INVALID;
    }

    return retval;

 print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest soevent_monitor -s <socket>"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -s specify socket number"IP_LF);
    ipcom_printf(IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_gethostbyname
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_gethostbyname(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    struct Ip_hostent   *he;
    Ipcom_getopt         o;
    int                  opt;
    int                  use_r = 0;
    struct Ip_hostent    result;
    char                 buffer[512];
    int                  h_errno;

    (void)glob;
    (void)fdp;

    if (argc <= 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "r", &o)) != -1)
    {
        switch (opt)
        {
        case 'r':
            use_r = 1;
            break;
        default:
            goto print_help;
        }
    }

    if (use_r)
        he = ipcom_gethostbyname_r(argv[o.optind], &result, buffer, sizeof(buffer), &h_errno);
    else
        he = ipcom_gethostbyname(argv[o.optind]);
    if (he != IP_NULL)
    {
        struct Ip_in_addr  inaddr;

        inaddr.s_addr = *(Ip_u32 *)*he->h_addr_list;

        ipcom_printf("%s [af=%d len=%d] = %s"IP_LF,
                     he->h_name,
                     he->h_addrtype,
                     he->h_length,
                     ipcom_inet_ntoa(inaddr));
    }
    else
        ipcom_printf("%s = <not found>"IP_LF, o.optarg);

    return 0;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest gethostbyname [-r] <name>"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -r use rentrant version of function"IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_gethostbyaddr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_gethostbyaddr(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    struct Ip_hostent   *he;
    Ipcom_getopt         o;
    int                  opt;
    int                  use_r = 0;
    struct Ip_hostent    result;
    char                 buffer[512];
    int                  h_errno;
    struct Ip_in_addr    inaddr;

    (void)glob;
    (void)fdp;

    if (argc <= 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "r", &o)) != -1)
    {
        switch (opt)
        {
        case 'r':
            use_r = 1;
            break;
        default:
            goto print_help;
        }
    }

    if (ipcom_inet_pton(IP_AF_INET, argv[o.optind], &inaddr) != 1)
        goto print_help;

    if (use_r)
        he = ipcom_gethostbyaddr_r((const char *)&inaddr, sizeof(inaddr), IP_AF_INET, &result, buffer, sizeof(buffer), &h_errno);
    else
        he = ipcom_gethostbyaddr((const char *)&inaddr, sizeof(inaddr), IP_AF_INET);
    if (he != IP_NULL)
    {
        inaddr.s_addr = *(Ip_u32 *)*he->h_addr_list;

        ipcom_printf("%s [af=%d len=%d] = %s"IP_LF,
                     ipcom_inet_ntoa(inaddr),
                     he->h_addrtype,
                     he->h_length,
                     he->h_name);
    }
    else
        ipcom_printf("%s = <not found>"IP_LF, o.optarg);

    return 0;

print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest gethostbyaddr [-r] <a.b.c.d>"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -r use rentrant version of function"IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_dup
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IP_PORT_OSE5
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_dup(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    Ipcom_getopt       o;
    Ipcom_cmd_socktest_fd *f;
    Ipcom_cmd_socktest_fd *new_f;
    int                sockno = -1;
    int                opt;
    Ip_fd              new_fd;
    Ip_err             retval = IPCOM_ERR_FAILED;
    Ip_pid_t           pid = 0;

    (void)glob;

    if (argc == 1)
        goto print_help;

    /* Fetch arguments */
    ipcom_getopt_clear_r(&o);
    while ((opt = ipcom_getopt_r(argc, argv, "s:P:", &o)) != -1)
    {
        switch (opt)
        {
        case 's':
            sockno = ipcom_cmd_socktest_get_sockno(o.optarg);
            break;
        case 'P':
            pid = ipcom_strtoul(o.optarg, IP_NULL, 16);
            break;
        default:
            goto print_help;
        }
    }

    f = ipcom_cmd_socktest_fd_find(fdp, sockno);
    if (f == IP_NULL)
    {
        ipcom_printf("     Failed, bad socket number");
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

#if defined(IP_TARGET_SOFT) || defined(softOSE)
    new_fd = zzdup(f->fd);
#else
    new_fd = dup(f->fd);
#endif
    if (new_fd < 0)
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_FAILED);

    if (pid)
    {
        retval = (Ip_err)efs_donate_fd(new_fd, pid);
        if (retval == 0)
        {
            ipcom_printf("     Socket = %d donated to 0x%lx"IP_LF, (int)new_fd, pid);
            IPCOM_SOCKTEST_LEAVE(IPCOM_SUCCESS);
        }
        else
        {
            ipcom_printf("     Failed, efs_dup_to() failed, retval = %d", retval);
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
        }
    }

    new_f = ipcom_cmd_socktest_fd_find(fdp, -1);
    if (new_f == IP_NULL)
    {
        ipcom_printf("     Failed, socktest socket list full"IP_LF);
        IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
    }
    new_f->fd        = new_fd;
    new_f->domain    = f->domain;
    new_f->type      = f->type;
    new_f->prot      = f->prot;
    SOCKTEST_FLOOD_INIT(f);

    ipcom_printf("     Socket = %d"IP_LF, new_f->fd);
    retval = IPCOM_SUCCESS;

leave:
    return retval;

 print_help:
    ipcom_printf("     USAGE"IP_LF);
    ipcom_printf("     socktest dup -s <socket> -P [pid]"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     OPTIONS"IP_LF);
    ipcom_printf("     -s specify socket number"IP_LF);

    return IPCOM_ERR_INVALID_ARG;
}
#endif /* IP_PORT_OSE5 */


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_send
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_send(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    return ipcom_cmd_socktest_send_impl(glob, fdp,argc, argv, IP_FALSE);
}


#ifdef IPCOM_CMD_SOCKTEST_USE_ZEROCOPY_API
/*
 *===========================================================================
 *                    ipcom_cmd_socktest_handle_zerocopy_send
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_socktest_handle_zerocopy_send(Ipcom_cmd_socktest_glob *glob, Ipcom_cmd_socktest_fd *fdp, int argc, char **argv)
{
    return ipcom_cmd_socktest_send_impl(glob, fdp, argc, argv, IP_TRUE);
}
#endif


/*
 *===========================================================================
 *                    ipcom_cmd_socktest_print_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_cmd_socktest_print_usage(void)
{
    ipcom_printf(IP_LF);
    ipcom_printf("NAME"IP_LF);
    ipcom_printf("     socktest - socket testing utility"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("SYNOPSIS"IP_LF);
    ipcom_printf("     socktest console"IP_LF);
    ipcom_printf("     [socktest] exit"IP_LF);
    ipcom_printf("     socktest open -d <domain> -t <type> -p <protocol>"IP_LF);
    ipcom_printf("     socktest close -s <socket> -f"IP_LF);
    ipcom_printf("     socktest bind -s <socket> [-a <address>] [-p <port>] [-i <interface name>]"IP_LF);
    ipcom_printf("     socktest setopt -s <socket> -l <level> -o <option>=<data>"IP_LF);
    ipcom_printf("     socktest getopt -s <socket> -l <level> -o <option>"IP_LF);
    ipcom_printf("     socktest ioctl -s <socket> -o <ioctl>[=<data>] -i <interface name>"IP_LF);
    ipcom_printf("     socktest connect -s <socket> -a <peer address> -p <peer port> [-d <domain>]"IP_LF);
    ipcom_printf("     socktest listen -s <socket> -b <back log>"IP_LF);
    ipcom_printf("     socktest accept -s <socket>"IP_LF);
    ipcom_printf("     socktest getsockname -s <socket>"IP_LF);
    ipcom_printf("     socktest getpeername -s <socket>"IP_LF);
    ipcom_printf("     socktest gethostbyname [-r] <name>"IP_LF);
    ipcom_printf("     socktest gethostbyaddr [-r] <address>"IP_LF);
    ipcom_printf("     socktest receive -s <socket> [-l <max length>] [-n <packets>] [-NPmorw]"IP_LF);
    ipcom_printf("     socktest send -s <socket> -d <data> [-f <flags>] [-a  <to address>] [-p <to port>]"IP_LF);
#ifdef IPCOM_CMD_SOCKTEST_USE_ZEROCOPY_API
    ipcom_printf("     socktest zerocopy_recv -s <socket> [-l <max length>] [-n <packets>]"IP_LF);
    ipcom_printf("                      [-r] [-a <from address>] [-p <from port>]"IP_LF);
    ipcom_printf("     socktest zerocopy_send -s <socket> -d <data> [-f <flags>] [-a  <to address>] [-p <to port>]"IP_LF);
#endif /* IPCOM_CMD_SOCKTEST_USE_ZEROCOPY_API */
    ipcom_printf("     socktest select -r <rsock> -w <wsock> -x <xsock>"IP_LF);
    ipcom_printf("     socktest shutdown [-r] [-w]"IP_LF);
#ifdef IP_PORT_OSE5
    ipcom_printf("     socktest dup -s <socket> [-P pid]"IP_LF);
#endif
    ipcom_printf("     socktest attach -s <socket> -d <domain> -t <type> -p <protocol>"IP_LF);
    ipcom_printf("     socktest if_nametoindex <ifname>"IP_LF);
    ipcom_printf("     socktest soevent_monitor -s <socket>"IP_LF);
#ifdef IPSCTP
    ipcom_printf("     socktest sctp_bindx -s <socket> [-a <address0>,<address1>, .... <addressN>] [-A <address0>,<address1>, .... <addressN>] -p <port> -f <flag> "IP_LF);
    ipcom_printf("     socktest sctp_getpaddrs -s <socket> [-a <assoc_id>] "IP_LF);
    ipcom_printf("     socktest sctp_freepaddrs -a <address_pointer>"IP_LF);
    ipcom_printf("     socktest sctp_getladdrs -s <socket> [-a <assoc_id>] "IP_LF);
    ipcom_printf("     socktest sctp_freeladdrs -a <address_pointer>"IP_LF);
    ipcom_printf("     socktest sctp_connectx -s <socket> -a <peer address0>,[<peer address1>, ... <peer addressN>][-A <address0>,<address1>, .... <addressN>] -p <peer port> [-d <domain>]"IP_LF);
    ipcom_printf("     socktest sctp_getaddrlen -d <family>"IP_LF);
    ipcom_printf("     socktest peeloff -s <socket> -a <assoc_id>"IP_LF);
#endif
    ipcom_printf(IP_LF);
    ipcom_printf("OPTIONS"IP_LF);
    ipcom_printf("     -N <n>  repeat the same command <n> times"IP_LF);
    ipcom_printf(IP_LF);
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_cmd_socktest
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_cmd_socktest(int argc, char **argv)
{
    Ip_err    retval = 0;
    int       no = 1;
    int       n;
    int       i;
    Ipcom_cmd_socktest_glob  glob;
    static Ipcom_once_t             once = IPCOM_ONCE_INIT;

    retval = ipcom_once(&once, ipcom_socktest_init, IP_NULL);
    if (retval != IPCOM_SUCCESS)
        return retval;

    ipcom_memset(&glob, 0, sizeof(Ipcom_cmd_socktest_glob));



#ifdef IP_PORT_INTEGRITY
    ipcom_init_libsocket();
#endif

    if (argc == 1)
    {
        ipcom_cmd_socktest_print_usage();
        return IPCOM_ERR_INVALID_ARG;
    }

    /* Get general options */
    for (i = 2; i < argc; i++)
    {
        if (ipcom_strcmp(argv[i], "-N") == 0 && i < (argc - 1))
        {
            no = ipcom_atoi(argv[++i]);
            break;
        }
    }

    /* Repeat the command n times, with errors and everything */
    for (n = 0; n < no; n++)
    {
        for (i = 0; i < (int) SOCKTEST_MAX_SUBCMD; i++)
        {
            if (ipcom_strcmp(argv[1], socktest_cmd_cb[i].subcmd) == 0)
            {
                retval = socktest_cmd_cb[i].cmd_cb(&glob, socktest_fd, argc-1, &argv[1]);
                break;
            }
        }

        if (i == SOCKTEST_MAX_SUBCMD)
        {
            ipcom_cmd_socktest_print_usage();
            IPCOM_SOCKTEST_LEAVE(IPCOM_ERR_INVALID_ARG);
        }

leave:
        if (retval == IPCOM_ERR_FAILED)
#ifdef IP_PORT_OSE5
        {
            int err = ipcom_errno;
            ipcom_printf("     Failed, errno: %s" IP_LF, ipcom_strerror(err > 0 ? err : -err));
        }
#else
            ipcom_printf("     Failed, errno: %s" IP_LF,
                          ipcom_strerror(ipcom_errno));
#endif
        else if (retval == IPCOM_ERR_NO_MEMORY)
            ipcom_printf("     Failed, out of memory."IP_LF);
        ipcom_printf(IP_LF);
    }

#ifdef IP_PORT_INTEGRITY
    if (ipcom_strcmp(argv[1], "shutdown") != 0)
    {
        /* Perform normal client shutdown */
        ipcom_shutdown_libsocket();
    }
    else if (no == 0)
    {
        /*
         * This was a shutdown command, exit from this process if N == 0
         * to create a permanent connection from this address space to
         * the socket server. If not, all sockets are closed when calling
         * shutdown the next time.
         */
        ipcom_printf("EXIT SHELL PROCESS\n");
        ipcom_exit(0);
    }
#endif

    return retval;
}

#if defined(IPCOM_CMD_SOCKTEST_USE_BSD_API)
#include "ipcom_ipcom2bsd_finish.h"
#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
