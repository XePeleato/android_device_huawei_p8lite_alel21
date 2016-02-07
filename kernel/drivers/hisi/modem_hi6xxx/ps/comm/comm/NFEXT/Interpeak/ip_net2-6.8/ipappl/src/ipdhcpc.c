/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipdhcpc.c,v $ $Revision: 1.104.6.1 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/src/ipdhcpc.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 *           and Jonas Rendel <jonas@interpeak.se>
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

#include "ipdhcpc_config.h"

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_clib.h>
#include <ipcom_syslog.h>
#include <ipcom_os.h>
#include <ipcom_sock.h>
#include <ipcom_netif.h>
#include <ipcom_err.h>
#include <ipcom_tmo.h>
#include <ipcom_sysvar.h>
#include <ipcom_ipd.h>
#include <ipcom_time.h>
#include <ipnet_routesock.h>
#include <ipcom_sysctl.h>
#include "ipdhcpc.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Syslog priority and facility. */
#ifdef IPCOM_SYSLOG_PRIORITY
#undef IPCOM_SYSLOG_PRIORITY
#endif
#define IPCOM_SYSLOG_PRIORITY    IPCOM_LOG_DEBUG2

#ifdef IPCOM_SYSLOG_FACILITY
#undef IPCOM_SYSLOG_FACILITY
#endif
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPDHCPC

/* DHCP pkt opcode: */
#define IPDHCPC_OP_BOOTREQUEST     1
#define IPDHCPC_OP_BOOTREPLY       2

/* DHCP pkt htype & hlen: */
#define IPDHCPC_HTYPE_ETHERNET     1
#define IPDHCPC_HLEN_ETHERNET      6

/* DHCP pkt message types:  */
#define IPDHCPC_MSG_INVALID        0
#define IPDHCPC_MSG_DISCOVER       1
#define IPDHCPC_MSG_OFFER          2
#define IPDHCPC_MSG_REQUEST        3
#define IPDHCPC_MSG_DECLINE        4
#define IPDHCPC_MSG_ACK            5
#define IPDHCPC_MSG_NAK            6
#define IPDHCPC_MSG_RELEASE        7
#define IPDHCPC_MSG_INFORM         8

/* DHCP client message bitvalues: */
#define IPDHCPC_MB_DISCOVER        0x01
#define IPDHCPC_MB_OFFER           0x02
#define IPDHCPC_MB_REQUEST         0x04
#define IPDHCPC_MB_DECLINE         0x08
#define IPDHCPC_MB_ACK             0x10
#define IPDHCPC_MB_NAK             0x20
#define IPDHCPC_MB_RELEASE         0x40
#define IPDHCPC_MB_INFORM          0x80

/* DHCP client states: */
#define IPDHCPC_STATE_INIT         1
#define IPDHCPC_STATE_INIT_REBOOT  2
#define IPDHCPC_STATE_SELECTING    3
#define IPDHCPC_STATE_REBOOTING    4
#define IPDHCPC_STATE_REQUESTING   5
#define IPDHCPC_STATE_BOUND        6
#define IPDHCPC_STATE_RENEWING     7
#define IPDHCPC_STATE_REBINDING    8
#define IPDHCPC_STATE_STOPPED      9

/* DHCP ports. */
#define IPDHCPC_SERVER_PORT        67
#define IPDHCPC_CLIENT_PORT        68

/* DHCP Header size without options, as per RFC 2131 */
#define IPDHCPC_NO_OPTIONS_SIZE    264

/* DHCP Max size of one option.  Arbitrary.  Used to alloc buffer holding options from sysvar struct */
#define IPDHCPC_MAX_OPTION_SIZE    1024

/* Minimum BOOTP header length. RFC1542:p5 */
#define IPDHCPC_PKT_MINLEN         300

/* MACROS */
#define IPDHCPC_LEAVE(xerrval)   do { retval = xerrval; goto leave; } while(0)

#define IPDHCPC_ISLOCKED_ASSERT()    ip_assert(ipdhcpc.code_mutex_count == 1)
#define IPDHCPC_LOCK()           do { ipcom_mutex_lock(ipdhcpc.code_mutex); \
                                      ip_assert(ipdhcpc.code_mutex_count == 0); \
                                      ipdhcpc.code_mutex_count++; \
                                 } while(0)

#define IPDHCPC_UNLOCK()         do { ip_assert(ipdhcpc.code_mutex_count == 1); \
                                      ipdhcpc.code_mutex_count--; \
                                      ipcom_mutex_unlock(ipdhcpc.code_mutex); \
                                 } while(0)



/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipdhcpc_handle_priv
 *===========================================================================
 */
typedef struct Ipdhcpc_handle_priv_struct
{
    struct Ipdhcpc_handle_struct *next;

    Ipcom_tmo   tmo;
    int         tmo_ignore;
    Ipcom_tmo   tmo_t2;
    int         tmo2_ignore;
    int         retrans_counter;
    int         mode;
    int         state;
    Ip_u32      xid_n;
    Ip_u16      in_optlen;
    Ip_u8       msgtype;
    Ip_u8       pad;

    /* Cached server info. */
    Ip_u32      serverid_n;                   /* Server IP address */
    Ip_u32      gw_n;                         /* Gateway IP address */ 
    Ip_u32      yiaddr;                       /* 'your' (client) IP address */
}
Ipdhcpc_handle_priv;


/*
 *===========================================================================
 *                         Ipdhcpc_options
 *===========================================================================
 */
typedef struct Ipdhcpc_options_struct
{
    Ip_u8 optcode;
    Ip_u8 minlen;
    Ip_u8 maxlen;
    Ip_u8 msgmust;
    Ip_u8 msgmustnot;
}
Ipdhcpc_options;


/*
 *===========================================================================
 *                         Ipdhcpc_pkt
 *===========================================================================
 */
IP_PACKED typedef struct Ipdhcpc_pkt_struct
{
    Ip_u8  op;
    Ip_u8  htype;
    Ip_u8  hlen;
    Ip_u8  hops;
    Ip_u32 xid;
    Ip_u16 secs;
    Ip_u16 flags;
    Ip_u32 ciaddr;      /* Client IP address                             */
    Ip_u32 yiaddr;		/* 'your' (client) IP address.                   */
    Ip_u32 siaddr;      /* IP address of next server to use in bootstrap */
    Ip_u32 giaddr;      /* Relay agent IP address                        */
    Ip_u8 chaddr[16];   /* Client hardware address.                      */
    char  sname[64];    /* Optional server host name                     */
    char  file[128];    /* Boot file name                                */
    Ip_u8 options[1];
}
Ipdhcpc_pkt;



/*
 *===========================================================================
 *                         Ipdhcpc_lease
 *===========================================================================
 */
typedef struct Ipdhcpc_lease_struct
{
    Ip_time_t t1;
    Ip_time_t t2;
    Ip_time_t tlease;
}
Ipdhcpc_lease;


/*
 *===========================================================================
 *                         Ipdhcpc_if
 *===========================================================================
 */
typedef struct Ipdhcpc_if_struct
{
    struct Ipdhcpc_if_struct   *next;
    Ip_u32                     ifindex;
    char                       ifname[IP_IFNAMSIZ];
    int                        num_opened;

    /* ifopen configuration. */

    /*  Time stuff.	 */
    Ip_time_t   time_addr_req;	/* same as time when ifopen() was made. */

    /* Interface stuff. */
    Ip_u8       chaddr[16];	/* Client hardware address. */
    Ip_u8       hlen;
    Ip_u8       pad;

    /* Config. */
    char       *clientid;

    /* Interface lease. */
    Ipdhcpc_handle   *handle;
    Ipdhcpc_lease     lease;

    Ip_bool           isdown;
}
Ipdhcpc_if;


/*
 *===========================================================================
 *                         Ipdhcpc_data
 *===========================================================================
 */
typedef struct Ipdhcpc_data_struct
{
    Ip_bool               opened;

    Ipcom_mutex           code_mutex;	/* Mutex to protect IPDHCPC global data. */
    int                   code_mutex_count;

    Ip_fd                 fd;
    Ip_fd                 ipd_fd;
    Ip_fd                 rt_fd;

    char                  buf[16];
    struct Ip_sockaddr_in addr;
    Ip_u32                last_brd_ifindex;
    int                   open_interfaces;

    Ipdhcpc_pkt          *in_pkt;
    Ip_ssize_t            in_pkt_len;
    Ipdhcpc_pkt          *out_pkt;
    Ipdhcpc_if           *ift_head;
    Ipdhcpc_handle       *handle_head;
    Ip_u16                max_message_size;
    Ip_bool               shut_down;
}
Ipdhcpc_data;


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

IP_STATIC Ip_err  ipdhcpc_init(void);
IP_STATIC void    ipdhcpc_exit(void);

IP_STATIC void    ipdhcpc_state_init(Ipdhcpc_if *ift, Ipdhcpc_handle *handle);
IP_STATIC void    ipdhcpc_state_reboot(Ipdhcpc_if *ift, Ipdhcpc_handle *handle);
IP_STATIC void    ipdhcpc_state_restart(Ipdhcpc_handle *handle);
IP_STATIC void    ipdhcpc_wakeup(Ipdhcpc_handle *handle, Ip_err status, 
                                 int event);

IP_STATIC void    ipdhcpc_output(Ipdhcpc_if *ift, Ipdhcpc_handle *handle,
			         struct Ip_sockaddr_in *addr);
IP_STATIC void    ipdhcpc_output_request(Ipdhcpc_if *ift,
				         Ipdhcpc_handle *handle,
				         Ip_u8 msgtype);

IP_STATIC void    ipdhcpc_reply_input(Ipdhcpc_if *ift, Ipdhcpc_handle *handle);
IP_STATIC void    ipdhcpc_offer_input(Ipdhcpc_if *ift, Ipdhcpc_handle *handle, 
                                      Ip_u8 *opt_serverid);
IP_STATIC void    ipdhcpc_ack_input(Ipdhcpc_if *ift, Ipdhcpc_handle *handle, 
                                    Ip_u8 *opt_serverid);
IP_STATIC void    ipdhcpc_nak_input(Ipdhcpc_if *ift, Ipdhcpc_handle *handle, 
                                    Ip_u8 *opt_serverid);
IP_STATIC void    ipdhcpc_bootp_input(Ipdhcpc_if *ift, Ipdhcpc_handle *handle);

IP_STATIC Ip_err  ipdhcpc_ifunlease_locked(Ipdhcpc_if *ift, 
                                           Ipdhcpc_handle **handlep);

IP_STATIC Ip_u8  *ipdhcpc_koption_get(Ip_u8 *options, int options_length, 
                                      Ip_u8 code);
IP_STATIC void    ipdhcpc_daemon(void);
IP_STATIC Ip_err  ipdhcpc_run(void);
IP_STATIC void    ipdhcpc_if_eventcb(char *ifname, Ip_u32 ifindex, 
                                     Ip_u32 ifflags, int event, void *data);

IP_STATIC void    ipdhcpc_ifclose_all(void);
IP_STATIC void    ipdhcpc_handle_free_pending(void);

IP_STATIC void    ipdhcpc_timer_cancel(Ipdhcpc_handle *handle, int which);
IP_STATIC void    ipdhcpc_timer_set(Ipdhcpc_handle *handle, Ip_u32 msec, 
                                    int which);
IP_STATIC Ip_bool ipdhcpc_if_has_addr(Ip_u32 ifindex, Ip_u32 ipaddr_n);

IP_GLOBAL Ip_err  ipdhcpc_option_add(Ipdhcpc_handle *handle, 
                                     Ip_u8 opt_code, 
                                     Ip_u8 *opt_data, 
                                     Ip_u32 opt_len);

IP_PUBLIC Ip_err      ipdhcpc_create(void);
IP_PUBLIC Ip_err      ipdhcpc_start(void);
IP_PUBLIC const char *ipdhcpc_version(void);

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC const char *ipdhcpc_msg_name[9] =
{
    "INVALID",
    "DISCOVER",
    "OFFER",
    "REQUEST",
    "DECLINE",
    "ACK",
    "NAK",
    "RELEASE",
    "INFORM"
};

IP_STATIC const char *ipdhcpc_state_name[10] =
{
    "INVALID",
    "INIT",
    "INIT_REBOOT",
    "REBOOTING",
    "SELECTING",
    "REQUESTING",
    "BOUND",
    "RENEWING",
    "REBINDING",
    "STOPPED"
};

IP_STATIC Ipdhcpc_data ipdhcpc;

IP_STATIC const Ipdhcpc_options ipdhcpc_options[] =
{
    /*
     * optcode,
     * minlen, maxlen,
     * msgmust,
     * msgmustnot,
     */

    /*
     * 9.1. Requested IP Address
     */
    {
        IPDHCPC_OPTCODE_DHCP_REQUESTED_ADDRESS,
        4, 4,
        IPDHCPC_MB_DECLINE,
        IPDHCPC_MB_RELEASE | IPDHCPC_MB_INFORM
    },

    /*
     * 9.2. IP Address Lease Time
     */
    {
        IPDHCPC_OPTCODE_DHCP_LEASE_TIME,
        4, 4,
        0,
        IPDHCPC_MB_INFORM | IPDHCPC_MB_DECLINE | IPDHCPC_MB_RELEASE
    },

    /*
     * 9.5. Server Identifier
     */
    {
        IPDHCPC_OPTCODE_DHCP_SERVER_IDENTIFIER,
        4, 4,
        IPDHCPC_MB_DECLINE | IPDHCPC_MB_RELEASE,
        IPDHCPC_MB_DISCOVER | IPDHCPC_MB_INFORM
    },

    /*
     * 9.8. Maximum DHCP Message Size
     */
    {
        IPDHCPC_OPTCODE_DHCP_MAX_MESSAGE_SIZE,
        2, 2,
        0,
        IPDHCPC_MB_DECLINE | IPDHCPC_MB_RELEASE
    },


    /*
     * 9.11. Class-identifier
     */
    {
        IPDHCPC_OPTCODE_VENDOR_CLASS_IDENTIFIER,
        1, 255,
        0,
        IPDHCPC_MB_DECLINE | IPDHCPC_MB_RELEASE
    },

    {
        IPDHCPC_OPTCODE_END, 1, 1,
        0,
        0
    }
};


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipdhcpc_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipdhcpc_init(void)
{
    int    num = 0;
    Ip_u16 port;
    int    max_msg_size;

    ipcom_memset(&ipdhcpc, 0, sizeof(Ipdhcpc_data));
    ipdhcpc.fd     = IP_INVALID_SOCKET;
    ipdhcpc.ipd_fd = IP_INVALID_SOCKET;
    
    max_msg_size = ipcom_sysvar_get_as_int("ipdhcpc.option.max_message_size", 
                                           IPDHCPC_MAX_MESSAGE_SIZE);
    
    if (max_msg_size < IPDHCPC_MAX_MESSAGE_SIZE)
    {
    	IPCOM_LOG1(WARNING, "ipdhcpc_init :: configured max message size (%d) too low", max_msg_size);
    	max_msg_size = IPDHCPC_MAX_MESSAGE_SIZE; 	
    }
    
    ipdhcpc.max_message_size = max_msg_size;

    (void)ipcom_mutex_create(&ipdhcpc.code_mutex);

    ipdhcpc.out_pkt = ipcom_malloc(ipdhcpc.max_message_size);
    if (ipdhcpc.out_pkt == IP_NULL)
        goto fail;

    ipdhcpc.in_pkt = ipcom_malloc(ipdhcpc.max_message_size);
    if (ipdhcpc.in_pkt == IP_NULL)
        goto fail;

    /* Must have a working timer starting at non-zero.*/
    while (ipcom_time(0) == 0)
    {
        (void)ipcom_sleep(1);
        num++;
        ip_assert(num < 10);
    }

    /* Get default DHCP UDP client socket. */
    ipdhcpc.fd = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
    if (ipdhcpc.fd == IP_INVALID_SOCKET)
    {
        IPCOM_LOG1(ERR, "ipdhcpc_open :: ipcom_socket() failed, errno =%d", ipcom_errno);
        goto fail;
    }

    /* Bind the socket. */
    ipcom_memset(&ipdhcpc.addr, 0, sizeof(struct Ip_sockaddr_in));
    IPCOM_SA_LEN_SET(&ipdhcpc.addr, sizeof(struct Ip_sockaddr_in));
    ipdhcpc.addr.sin_family = IP_AF_INET;
    port = (Ip_u16)ipcom_sysvar_get_as_int("ipdhcpc.client_port", IPDHCPC_CLIENT_PORT);
    ipdhcpc.addr.sin_port = ip_htons(port);
    ipdhcpc.addr.sin_addr.s_addr = ip_htonl(IP_INADDR_ANY);
    if (ipcom_bind(ipdhcpc.fd, (struct Ip_sockaddr *)&ipdhcpc.addr, sizeof(ipdhcpc.addr)) == IP_SOCKERR)
    {
        /* Failed to bind socket */
        IPCOM_LOG1(ERR, "ipdhcpc_open :: ipcom_bind() failed, errno =%d", ipcom_errno);
        goto fail;
    }

    /* Get routing socket */
    ipdhcpc.rt_fd = ipcom_socket(IP_AF_ROUTE, IP_SOCK_RAW, 0);
    if (ipdhcpc.rt_fd == IP_INVALID_SOCKET)
    {
        IPCOM_LOG1(ERR, "ipdhcpc_open :: ipcom_socket(routing) failed, errno =%d", ipcom_errno);
        goto fail;
    }

    {
        int i = 1;
        if (ipcom_socketioctl(ipdhcpc.rt_fd, IP_FIONBIO, (char *)&i) != 0)
        {
            IPCOM_LOG1(WARNING, "ipdhcpc_open ::Failed to set routing socket to non blocking, errno = %d",
                       ipcom_errno);
        }
    }

    /* Success. */
    IPCOM_LOG0(DEBUG2, "ipdhcpc_open() :: opened");
    ipdhcpc.opened = IP_TRUE;
    return IPCOM_SUCCESS;

 fail:
    return IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    ipdhcpc_exit
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipdhcpc_exit(void)
{
    IPDHCPC_LOCK();

    IPCOM_LOG0(DEBUG2, "ipdhcpc_exit() :: exiting");

    ipdhcpc.opened = IP_FALSE;

    if (ipdhcpc.fd != IP_INVALID_SOCKET)
    {
        /* Close Routing socket socket */
        (void)ipcom_socketclose(ipdhcpc.rt_fd);
        ipdhcpc.rt_fd = IP_INVALID_SOCKET;

        /* Close DHCP socket */
        (void)ipcom_socketclose(ipdhcpc.fd);
        ipdhcpc.fd = IP_INVALID_SOCKET;
    }

    /* Free memory */
    if (ipdhcpc.out_pkt != IP_NULL)
        ipcom_free(ipdhcpc.out_pkt);
    if (ipdhcpc.in_pkt != IP_NULL)
        ipcom_free(ipdhcpc.in_pkt);

    IPDHCPC_UNLOCK();
    (void)ipcom_mutex_delete(&ipdhcpc.code_mutex);
}


/*
 *===========================================================================
 *                    ipdhcpc_number
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_u32
ipdhcpc_number(int from, int to)
{
    Ip_u32 val;
    ip_assert(from < to);

    val = ipcom_random();
    val %= (Ip_u32)(to - from + 1);
    val += from;

    return val;
}


/*
 *===========================================================================
 *                    ipdhcpc_wakeup
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipdhcpc_wakeup(Ipdhcpc_handle *handle, Ip_err status, int event)
{
    handle->info.status = status;

    /* Wakeup blocking request and call the callback. */
    if (handle->reply_cb != IP_NULL)
    {
        IPDHCPC_UNLOCK();
        (void)handle->reply_cb(handle, event);
        IPDHCPC_LOCK();
    }
}


/*
 *===========================================================================
 *                    ipdhcpc_print_packet
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipdhcpc_print_packet(Ipdhcpc_if *ift, Ipdhcpc_handle *handle,
                     Ipdhcpc_pkt *pkt, Ip_bool input, Ip_ssize_t length)
{
#ifdef IP_DEBUG
    static char buf[2000];
    Ip_u8 *option;
    int    i, len, optlen, tlen;
    Ip_u8  msgtype = 0;
    char   addr1[16], addr2[16];
    int    bootp_mode;
    int    priority;
    
    optlen = (int) length - ip_offsetof(Ipdhcpc_pkt, options);
    option = ipdhcpc_koption_get(&pkt->options[4] /*lint -e416 */, optlen,
                                 IPDHCPC_OPTCODE_DHCP_MESSAGE_TYPE);
    
    if (option != IP_NULL && option[1] == 1 && option[2] < 9)
        msgtype = option[2];

    bootp_mode = ipcom_sysvar_get_as_int("ipdhcpc.bootp_mode", IPDHCPC_BOOTP_MODE);

    len = ipcom_sprintf(buf, IP_LF "*******************" IP_LF);
    
    if (!bootp_mode)
    {
        len += ipcom_sprintf(buf + len,
                             "ipdhcpc[%s] :: %s in %s state (len %d)" IP_LF,
                             ift->ifname,
                             input ? "Receiving DHCP Reply" :
                             "Sending DHCP Request",
                             ipdhcpc_state_name[handle->priv->state],
                             length);

        len += ipcom_sprintf(buf + len,
                             "[ DHCP %s ] ",
                             ipdhcpc_msg_name[msgtype]);
    }
    else
    {
        len += ipcom_sprintf(buf + len,
                             "ipdhcpc[%s] :: %s" IP_LF,
                             ift->ifname,
                             input ? "Receiving BOOTP Reply" :
                             "Sending BOOTP Request",
                             ipdhcpc_state_name[handle->priv->state]);	
    }
    
    len += ipcom_sprintf(buf + len,                        
                         "[ xid: 0x%lx ] [ secs: %d ] [ flags: %d ]" IP_LF,
                         pkt->xid,
                         (int) ip_ntohs(pkt->secs),
                         (int) ip_ntohs(pkt->flags));
    
    len += ipcom_sprintf(buf + len,
                         "[ ciaddr: %s ] [ yiaddr: %s ]" IP_LF,
                         ipcom_inet_ntop(IP_AF_INET, &pkt->ciaddr,
                                         addr1,
                                         sizeof(addr1)),
                         ipcom_inet_ntop(IP_AF_INET, &pkt->yiaddr,
                                         addr2,
                                         sizeof(addr2)));
    
    len += ipcom_sprintf(buf + len,
                         "[ siaddr: %s ] [ giaddr: %s ]" IP_LF,                     		 
                         ipcom_inet_ntop(IP_AF_INET, &pkt->siaddr,
                                         addr1,
                                         sizeof(addr1)),
                         ipcom_inet_ntop(IP_AF_INET, &pkt->giaddr,
                                         addr2,
                                         sizeof(addr2)));

    if (ipcom_strlen(pkt->sname) > 0)
        len += ipcom_sprintf(buf + len,
                             "[ sname: %s ]" IP_LF, &pkt->sname);
   
    if (ipcom_strlen(pkt->file) > 0)
        len += ipcom_sprintf(buf + len,
                             "[ file: %s ]" IP_LF, &pkt->file);
    
    len += ipcom_sprintf(buf + len, "[ Options (len %d): ]" IP_LF, optlen);
    option = pkt->options;
    
    for (i = 4; i < optlen; i++)
    {
        if (option[i] == IPDHCPC_OPTCODE_PAD)
            continue;
        if (option[i] == IPDHCPC_OPTCODE_END)
            break;
        
        if (i + 1 + option[i + 1] >= optlen)
        {
            len += ipcom_sprintf(buf + len,
                                 "  %2d: length = %d, truncated[%d/%d]"
                                 IP_LF, option[i], option[i + 1],
                                 i + 1 + option[i + 1], optlen);
            break;
        }

        tlen = ipdhcpc_option_sprintf(&option[i],
                                      buf + len,
                                      sizeof(buf) - len);
        if (tlen < 0)
            break;
        
        len += tlen;

        i += 1 + option[i + 1];
        continue;
    }

    ipcom_sprintf(buf + len, "*******************" IP_LF);
    
    priority = ipcom_getlogmask_on(IPCOM_LOG_IPDHCPC);
    
    if (priority >= IPCOM_LOG_DEBUG)
    	ipcom_printf("%s", buf);

#else
    
    (void)ift;
    (void)handle;
    (void)pkt;
    (void)input;
    (void)length;
    (void)ipdhcpc_msg_name;
    (void)ipdhcpc_state_name;

#endif
}


/*
 *===========================================================================
 *                    ipdhcpc_sec_to_msec
 *===========================================================================
 * Description: Converts seconds to milliseconds and check for overflow.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_u32
ipdhcpc_sec_to_msec(Ip_u32 seconds)
{
    if (seconds > (Ip_u32) (0xffffffff / 1000))
        return 0xffffffff;
    return seconds * 1000;
}


/*
 *===========================================================================
 *                    ipdhcpc_tmo_cb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_s32
ipdhcpc_tmo_cb(struct Ipcom_tmo_struct *tmo, void *cookie)
{
    Ipdhcpc_handle *handle = cookie;
    Ipdhcpc_if     *ift;

    IPDHCPC_LOCK();

    /* Check for timeouts that were ready to run when cancelled */
    if (tmo == &handle->priv->tmo && handle->priv->tmo_ignore == 1)
    {
        handle->priv->tmo_ignore = 0;
        goto leave;
    }
    if (tmo == &handle->priv->tmo_t2 && handle->priv->tmo2_ignore == 1)
    {
        handle->priv->tmo2_ignore = 0;
        goto leave;
    }

    /* Get interface. */
    for (ift = ipdhcpc.ift_head; ift != IP_NULL; ift = ift->next)
        if (ift->ifindex == (Ip_u32)handle->ifindex)
            break;

    /* Interface gone, release the timeout. */
    if (ift == IP_NULL)
    {
        ipdhcpc_wakeup(handle, IPCOM_ERR_INVALID_INTERFACE,
                       IPDHCPC_REPLY_CB_EVENT_FAIL_IFLOST);
        goto leave;
    }

    IPCOM_LOG2(DEBUG2, "ipdhcpc[%s] :: DHCP timeout in %s state",
               ift->ifname,
               ipdhcpc_state_name[handle->priv->state]);

    /* Act on the state.*/
    switch(handle->priv->state)
    {
    case IPDHCPC_STATE_REBOOTING:
        ip_assert(tmo == &handle->priv->tmo);
        ip_assert(!IPCOM_TMO_PENDING(&handle->priv->tmo_t2));
        ipdhcpc_state_restart(handle);
        break;
    case IPDHCPC_STATE_INIT:
        ip_assert(tmo == &handle->priv->tmo);
        ip_assert(!IPCOM_TMO_PENDING(&handle->priv->tmo_t2));
        ipdhcpc_state_init(ift, handle);
        break;
    case IPDHCPC_STATE_INIT_REBOOT:
        ip_assert(tmo == &handle->priv->tmo);
        ip_assert(!IPCOM_TMO_PENDING(&handle->priv->tmo_t2));
        ipdhcpc_state_reboot(ift, handle);
        break;

    case IPDHCPC_STATE_SELECTING:
        {
            int   num_retries;

            ip_assert(tmo == &handle->priv->tmo);
            ip_assert(!IPCOM_TMO_PENDING(&handle->priv->tmo_t2));

            num_retries = ipcom_sysvar_get_as_int("ipdhcpc.discover_retries",
                                                  IPDHCPC_DISCOVER_RETRIES);

            /* Increment to double the timeout for retransmission. */
            /* retrans_counter = 4 means 64 seconds timeout, which will be repeated. */
            if (++handle->priv->retrans_counter > num_retries)
            {
                ipdhcpc_wakeup(handle, IPCOM_ERR_TIMEOUT, IPDHCPC_REPLY_CB_EVENT_FAIL_RETRY);
            }
            else
            {           	
                ipdhcpc_output_request(ift, handle, IPDHCPC_MSG_DISCOVER);
            }

        }
        break;

    case IPDHCPC_STATE_REBINDING:
        ip_assert(ift->handle == handle);
        if (tmo == &handle->priv->tmo_t2)
        {
            /* Lease expired -> restart. */
            IPCOM_LOG1(DEBUG2, "ipdhcpc[%s] :: Lease expired, restarting", ift->ifname);

            if (handle->reply_cb != IP_NULL)
                (void)handle->reply_cb(handle, IPDHCPC_REPLY_CB_EVENT_EXPIRE);

            ipdhcpc_timer_cancel(ift->handle, 0);
            
            ipdhcpc_ifunlease_locked(ift, IP_NULL);
 
            ift->handle = handle;
                       
            ipdhcpc_state_restart(handle);  
            break;
        }
        goto send_request;

    case IPDHCPC_STATE_RENEWING:
        ip_assert(ift->handle == handle);
        if (tmo == &handle->priv->tmo_t2)
        {
            /* T2 expired -> try any DHCP server in rebinding state. */
            IPCOM_LOG1(DEBUG2, "ipdhcpc[%s] :: Entering REBINDING state", ift->ifname);

            handle->priv->state = IPDHCPC_STATE_REBINDING;
            handle->priv->serverid_n = 0;
            ipdhcpc_timer_set(handle, ipdhcpc_sec_to_msec(ift->lease.tlease - ift->lease.t2), 2);
            handle->priv->retrans_counter = 0;
            ipdhcpc_timer_cancel(handle, 1);
            
            handle->priv->next = ipdhcpc.handle_head;
			ipdhcpc.handle_head = handle;
        }
        goto send_request;

    case IPDHCPC_STATE_REQUESTING:
    send_request:
        if (++handle->priv->retrans_counter > 4)
        {
            /* Never received reply from selected server. Reboot state machine. */
            ipdhcpc_timer_cancel(handle, 2);
            handle->priv->state = IPDHCPC_STATE_INIT;
            ipdhcpc_state_init(ift, handle);
        }
        else
        {
            /*
             * Timeout waiting for DHCP request packet. Resend the REQUEST
             * packet.
             */
            ipdhcpc_output_request(ift, handle, IPDHCPC_MSG_REQUEST);
        }
        break;

    case IPDHCPC_STATE_BOUND:
        ip_assert(tmo == &handle->priv->tmo);
        ip_assert(ift->handle == handle);
        ip_assert(!IPCOM_TMO_PENDING(&handle->priv->tmo_t2));

        IPCOM_LOG1(DEBUG2, "ipdhcpc[%s] :: Entering RENEWING state", ift->ifname);

        handle->priv->tmo2_ignore = 0;
        handle->priv->state = IPDHCPC_STATE_RENEWING;
        ipdhcpc_timer_set(handle, ipdhcpc_sec_to_msec(ift->lease.t2 - ift->lease.t1), 2);
        handle->priv->retrans_counter = 0;
        ipdhcpc_output_request(ift, handle, IPDHCPC_MSG_REQUEST);
        break;

    default:
        IP_PANIC();
        break;
    }

 leave:
    IPDHCPC_UNLOCK();
    return 0;
}


/*
 *===========================================================================
 *                    ipdhcpc_state_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipdhcpc_state_init(Ipdhcpc_if *ift, Ipdhcpc_handle *handle)
{
    ip_assert(handle->priv->state <= IPDHCPC_STATE_INIT_REBOOT);
    ip_assert(ift->handle == handle);

    /* !!if not remember active lease, go to reboot. */
    /* !!clean data on socket */

    /* Init new request data. */
    handle->priv->state = IPDHCPC_STATE_SELECTING;
    handle->priv->xid_n =(Ip_u32) ipcom_random();
    handle->priv->serverid_n = 0;
    handle->priv->yiaddr = 0;
    handle->priv->retrans_counter = 0;

    /* Send the discover message. */
    ipdhcpc_output_request(ift, handle, IPDHCPC_MSG_DISCOVER);
}


/*
 *===========================================================================
 *                    ipdhcpc_state_reboot
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipdhcpc_state_reboot(Ipdhcpc_if *ift, Ipdhcpc_handle *handle)
{
    ip_assert(handle->priv->state <= IPDHCPC_STATE_INIT_REBOOT);
    ip_assert(ift->handle == handle);

    handle->priv->state = IPDHCPC_STATE_REBOOTING;
    ipdhcpc_output_request(ift, handle, IPDHCPC_MSG_REQUEST);
}


/*
 *===========================================================================
 *                    ipdhcpc_state_restart
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipdhcpc_state_restart(Ipdhcpc_handle *handle)
{
    char    ifname[IP_IFNAMSIZ + 1];
    int     rfc_init_delay;
    Ip_u32  msec;

    ipdhcpc_timer_cancel(handle, 0);

    rfc_init_delay = ipcom_sysvar_get_as_int("ipdhcpc.rfc2131_init_delay",
                                             IPDHCPC_RFC2131_INIT_DELAY);
    if (rfc_init_delay)
        msec = ipdhcpc_number(9000, 11000);
    else
        msec = 0;

    (void)ipcom_if_indextoname(handle->ifindex, ifname);
    IPCOM_LOG2(INFO,
               "ipdhcpc[%s] :: restarting after %d milliseconds",
               ifname, msec);

    handle->priv->state = IPDHCPC_STATE_INIT;

    ipdhcpc_timer_set(handle, msec, 1);
}

IP_STATIC void
ipdhcpc_state_init_reboot(Ipdhcpc_handle *handle)
{
    char    ifname[IP_IFNAMSIZ + 1];
    int     rfc_init_delay;
    Ip_u32  msec;

    ipdhcpc_timer_cancel(handle, 0);

    rfc_init_delay = ipcom_sysvar_get_as_int("ipdhcpc.rfc2131_init_delay",
                                             IPDHCPC_RFC2131_INIT_DELAY);
    if (rfc_init_delay)
        msec = ipdhcpc_number(9000, 11000);
    else
        msec = 0;

    (void)ipcom_if_indextoname(handle->ifindex, ifname);
    IPCOM_LOG2(INFO,
               "ipdhcpc[%s] :: restarting after %d milliseconds",
               ifname, msec);

    handle->priv->state = IPDHCPC_STATE_INIT_REBOOT;

    ipdhcpc_timer_set(handle, msec, 1);
}


/*
 *===========================================================================
 *                    ipdhcpc_output_request
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipdhcpc_output_request(Ipdhcpc_if *ift, Ipdhcpc_handle *handle, Ip_u8 msgtype)
{
    struct Ip_sockaddr_in addr;
    Ip_u32 msec;
    Ip_u32 offer_timout;
    int    rfc_backoff_delay;

    ipcom_memset(&addr, 0, sizeof(struct Ip_sockaddr_in));
    IPCOM_SA_LEN_SET(&addr, sizeof(struct Ip_sockaddr_in));
    addr.sin_family = IP_AF_INET;
    addr.sin_port = handle->serverport_n;

    if (handle->serveraddr_n != IP_INADDR_BROADCAST)
        addr.sin_addr.s_addr = handle->serveraddr_n;
    else if (handle->priv->state == IPDHCPC_STATE_RENEWING)
    {
        ip_assert(handle->priv->serverid_n != 0);
        addr.sin_addr.s_addr = handle->priv->serverid_n;
    }
    else
        addr.sin_addr.s_addr = handle->serveraddr_n;

    /* If broadcast, adjust outgoing interface. */
    if (ipdhcpc.last_brd_ifindex != ift->ifindex)
    {
        struct Ip_in_addr in_addr;

        IPCOM_LOG2(DEBUG2, "ipdhcpc[%s] :: setting broadcast interface to %d", ift->ifname, ift->ifindex);

        in_addr.s_addr = ip_htonl(ift->ifindex);

        /* Sent outgoing multicast interface for IPv4 */
        if (ipcom_setsockopt(ipdhcpc.fd, IP_IPPROTO_IP, IP_IP_MULTICAST_IF, &in_addr, sizeof(in_addr)) == 0)
            ipdhcpc.last_brd_ifindex = ift->ifindex;
    }

    /* Send the discover message. */
    handle->priv->msgtype = msgtype;
    ipdhcpc_output(ift, handle, &addr);

    /* Request a new timeout */
    rfc_backoff_delay = ipcom_sysvar_get_as_int("ipdhcpc.rfc2131_exponential_backoff",
                                                IPDHCPC_RFC2131_EXP_BACKOFF_DELAY);
    offer_timout = (Ip_u32)ipcom_sysvar_get_as_int("ipdhcpc.offer_timeout",
                                                   IPDHCPC_OFFER_TIMEOUT);
    if (rfc_backoff_delay)
        /* Exponential backoff (4,8,16,32 and 64 seconds) */
        msec = offer_timout * (2 << handle->priv->retrans_counter);
    else
        /* Exponential back-off disabled */
        msec = offer_timout;

    /* Use T1 for retransmission timer */
    msec = IP_MAX(msec, 200);
    ipdhcpc_timer_cancel(handle, 1);
    ipdhcpc_timer_set(handle, ipdhcpc_number((int)msec - 100, (int)msec + 100), 1);
}


/*
 *===========================================================================
 *                    ipdhcpc_output
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipdhcpc_output(Ipdhcpc_if *ift, Ipdhcpc_handle *handle,
               struct Ip_sockaddr_in *addr)
{
    Ip_ssize_t               wrote;
    Ip_ssize_t               length;
    int                      n, i, len;
    Ip_time_t                now;
    Ip_u16                   secs;
    int                      bootp_mode;

    ip_assert(handle->priv->msgtype != IPDHCPC_MSG_INVALID);

    ipcom_memset(ipdhcpc.out_pkt, 0, ipdhcpc.max_message_size);

    ipdhcpc.out_pkt->op = IPDHCPC_OP_BOOTREQUEST;
    ipdhcpc.out_pkt->htype = IPDHCPC_HTYPE_ETHERNET;
    ipdhcpc.out_pkt->hlen = ift->hlen;
    ipdhcpc.out_pkt->xid = handle->priv->xid_n;

    if (handle->priv->retrans_counter == 0)
    {
        ift->time_addr_req = ipcom_time(0);
        secs = 0;
    }
    else
    {
        now = ipcom_time(0);
        if (now - ift->time_addr_req > 65535)
            secs = 0xffff;
        else if (now < ift->time_addr_req)
        {
            /* Old clock, reset time. */
            secs = 0;
            ift->time_addr_req = ipcom_time(0);
        }
        else
            secs =(Ip_u16)(now - ift->time_addr_req);
    }
    ipdhcpc.out_pkt->secs = ip_htons(secs);

    /*
     * Only specify IP address if we own it. See RFC1542:p11 Discussion,
     * interpretation #1.
     */
    if (handle->priv->msgtype == IPDHCPC_MSG_INFORM)
    {
        ip_assert(handle->clientaddr_n != 0);
        ipdhcpc.out_pkt->ciaddr = handle->clientaddr_n;
    }
    else if (handle->priv->msgtype == IPDHCPC_MSG_RELEASE ||
             (handle->priv->msgtype == IPDHCPC_MSG_REQUEST &&
              (handle->priv->state == IPDHCPC_STATE_BOUND ||
               handle->priv->state == IPDHCPC_STATE_RENEWING ||
               handle->priv->state == IPDHCPC_STATE_REBINDING)))
    {
        ip_assert(handle->priv->yiaddr != 0);
        ipdhcpc.out_pkt->ciaddr = handle->priv->yiaddr;
    }

    ipcom_memcpy(ipdhcpc.out_pkt->chaddr, ift->chaddr, ift->hlen);

    if (handle->file != IP_NULL)
    	ipcom_memcpy(ipdhcpc.out_pkt->file,handle->file,128);
    
    if (handle->sname != IP_NULL)
    	ipcom_memcpy(ipdhcpc.out_pkt->sname,handle->sname,64);
    
    /* Add option cookie. */
    /*lint -e416 */
    ipdhcpc.out_pkt->options[0] =(Ip_u8)((handle->magic_cookie >> 24) & 0xff);
    /*lint -e415 */
    ipdhcpc.out_pkt->options[1] =(Ip_u8)((handle->magic_cookie >> 16) & 0xff);
    /*lint -e415 */
    ipdhcpc.out_pkt->options[2] =(Ip_u8)((handle->magic_cookie >> 8) & 0xff);
    /*lint -e415 */
    ipdhcpc.out_pkt->options[3] =(Ip_u8)((handle->magic_cookie >> 0) & 0xff);
    i = 4;

    bootp_mode = ipcom_sysvar_get_as_int("ipdhcpc.bootp_mode",IPDHCPC_BOOTP_MODE);
    
    if (!bootp_mode)
    {
    	/* Add mandatory DHCP Message Type option. */	
    	
    	ipdhcpc.out_pkt->options[i++] = IPDHCPC_OPTCODE_DHCP_MESSAGE_TYPE;
    	ipdhcpc.out_pkt->options[i++] = 1;
    	ipdhcpc.out_pkt->options[i++] = handle->priv->msgtype;

    	/* Add client-identifier option. */
    	if (ift->clientid != IP_NULL)
    	{
    	    len = ipcom_strlen(ift->clientid);
    		ipdhcpc.out_pkt->options[i++] =	IPDHCPC_OPTCODE_DHCP_CLIENT_IDENTIFIER;
    		ipdhcpc.out_pkt->options[i++] =(Ip_u8) (1 + len);
    		ipdhcpc.out_pkt->options[i++] = 0;  /*!! HW type: none */
    		ipcom_memcpy(&ipdhcpc.out_pkt->options[i], ift->clientid, len);
    	    i += len;
    	}
    	else
    	{
    	    len = ift->hlen;
    		ipdhcpc.out_pkt->options[i++] = IPDHCPC_OPTCODE_DHCP_CLIENT_IDENTIFIER;
    		ipdhcpc.out_pkt->options[i++] =(Ip_u8)(1 + len);
    		ipdhcpc.out_pkt->options[i++] = 1;  /*!! HW type: Ethernet */
    		ipcom_memcpy(&ipdhcpc.out_pkt->options[i], ift->chaddr, len);
    	    i += len;
    	}

    	/* Optionally add server-identifier option. */
    	if (handle->priv->msgtype == IPDHCPC_MSG_DECLINE ||
    	    handle->priv->msgtype == IPDHCPC_MSG_RELEASE ||
            (handle->priv->msgtype == IPDHCPC_MSG_REQUEST &&
             handle->priv->state == IPDHCPC_STATE_REQUESTING))
    	{
            ip_assert(handle->priv->serverid_n != 0);
    		ipdhcpc.out_pkt->options[i++] = IPDHCPC_OPTCODE_DHCP_SERVER_IDENTIFIER;
    		ipdhcpc.out_pkt->options[i++] = 4;
    		ipcom_memcpy(&ipdhcpc.out_pkt->options[i],
                         &handle->priv->serverid_n,
                         4);
            i += 4;
    	}

    	/* Optionally add Requested IP address option.*/
    	if (handle->clientaddr_n != 0 &&
            (handle->priv->msgtype == IPDHCPC_MSG_DISCOVER ||
             (handle->priv->msgtype == IPDHCPC_MSG_REQUEST &&
              handle->priv->state == IPDHCPC_STATE_REBOOTING)))
    	{
    		ipdhcpc.out_pkt->options[i++] =
            IPDHCPC_OPTCODE_DHCP_REQUESTED_ADDRESS;
    		ipdhcpc.out_pkt->options[i++] = 4;
    		ipcom_memcpy(&ipdhcpc.out_pkt->options[i],
                         &handle->clientaddr_n,
                         4);
            i += 4;
    	}
	    else if (handle->priv->msgtype == IPDHCPC_MSG_DECLINE ||
             (handle->priv->msgtype == IPDHCPC_MSG_REQUEST &&
              handle->priv->state == IPDHCPC_STATE_REQUESTING))
    	{
            ip_assert(handle->priv->yiaddr != 0);
    		ipdhcpc.out_pkt->options[i++] = IPDHCPC_OPTCODE_DHCP_REQUESTED_ADDRESS;
    		ipdhcpc.out_pkt->options[i++] = 4;
    		ipcom_memcpy(&ipdhcpc.out_pkt->options[i],
                         &handle->priv->yiaddr, 4);
            i += 4;
    	}

    	/* Add Maximum message size. */
        if (handle->priv->msgtype == IPDHCPC_MSG_DISCOVER ||
            handle->priv->msgtype == IPDHCPC_MSG_REQUEST ||
            handle->priv->msgtype == IPDHCPC_MSG_INFORM)
    	{
    		ipdhcpc.out_pkt->options[i++] = IPDHCPC_OPTCODE_DHCP_MAX_MESSAGE_SIZE;
    		ipdhcpc.out_pkt->options[i++] = 2;
    		ipdhcpc.out_pkt->options[i++] =(Ip_u8)((ipdhcpc.max_message_size >> 8) & 0xff);
    		ipdhcpc.out_pkt->options[i++] =(Ip_u8)((ipdhcpc.max_message_size >> 0) & 0xff);
    	}
    }

    /* Copy over valid user options. */
    for (n = 0; n < handle->optlen; n++)
    {
        if (handle->options[n] == IPDHCPC_OPTCODE_PAD)
            ipdhcpc.out_pkt->options[i++] = IPDHCPC_OPTCODE_PAD;
        else if (handle->options[n] == IPDHCPC_OPTCODE_END)
            break;	/* end -> strip it and breakout. */
        else if (n + 1 + handle->options[n + 1] >= handle->optlen)
        {
            /* check if option is truncated, if so - skip it and breakout. */
            IPCOM_LOG2(ERR, "ipdhcpc[%s] :: USER: truncated option %d",
                       ift->ifname,
                       (int) handle->options[n]);
            break;
        }
        else
        {
            int msg;

            /* Check the option.*/
            for (msg = 0; ipdhcpc_options[msg].optcode != IPDHCPC_OPTCODE_END; msg++)
                if (ipdhcpc_options[msg].optcode == handle->options[n])
                {
#ifdef IP_ASSERT
                    /* These options are handled by IPDHCPC.*/
                    if (handle->options[n] == IPDHCPC_OPTCODE_DHCP_MESSAGE_TYPE ||
                        handle->options[n] == IPDHCPC_OPTCODE_DHCP_CLIENT_IDENTIFIER || 
                        handle->options[n] == IPDHCPC_OPTCODE_DHCP_MAX_MESSAGE_SIZE)
                            IP_PANIC();
#endif

                    /* Verify min length field.*/
                    if (ipdhcpc_options[msg].minlen && handle->options[n + 1] < ipdhcpc_options[msg].minlen)
                    {
                        IPCOM_LOG2(ERR, "ipdhcpc[%s] :: USER: too short option %d - not sent",
                                   ift->ifname,
                                   (int) handle->options[n]);
                        goto next_opt;
                    }

                    /* Verify max length field. */
                    if (ipdhcpc_options[msg].maxlen && handle->options[n + 1] > ipdhcpc_options[msg].maxlen)
                    {
                        IPCOM_LOG2(ERR, "ipdhcpc[%s] :: USER: too long option %d - not sent",
                                   ift->ifname,
                                   (int) handle->options[n]);
                        goto next_opt;
                    }

                    /* Option must not be included with this message type, skip it. */
                    if (IP_BIT_GET(ipdhcpc_options[msg].msgmustnot, handle->priv->msgtype - 1))
                    {
                        IPCOM_LOG2(DEBUG2, "ipdhcpc[%s] :: USER: ignoring option %d due to msgtype - not sent",
                                   ift->ifname,
                                   (int) handle->options[n]);
                        goto next_opt;
                    }

                    /*
                     * RFC2131:p38 - Not many options used with DHCP
                     * decline & release.
                     */
                    if (handle->priv->msgtype == IPDHCPC_MSG_DECLINE ||
                        handle->priv->msgtype == IPDHCPC_MSG_RELEASE)
                    {
                        if (handle->options[n] != IPDHCPC_OPTCODE_DHCP_REQUESTED_ADDRESS &&
                            handle->options[n] != IPDHCPC_OPTCODE_DHCP_SERVER_IDENTIFIER &&
                            handle->options[n] != IPDHCPC_OPTCODE_DHCP_MESSAGE)
                        {
                            IPCOM_LOG2(DEBUG2, "ipdhcpc[%s] :: USER: ignoring option %d due to dec/rel - not sent",
                                       ift->ifname,
                                       (int)handle->options[n]);
                            goto next_opt;
                        }
                    }

                    /* Option valid. */
                    break;
                }

            /* Copy over option. */
            ipcom_memcpy(&ipdhcpc.out_pkt->options[i],
                         &handle->options[n],
                         2 + handle->options[n + 1]);
            
            i +=(2 + handle->options[n + 1]);

            /* fall through to next option. */
        }

    next_opt:
        n += 1 + handle->options[n + 1];
        continue;
    }

    /* End of options. 	 */
    ipdhcpc.out_pkt->options[i++] = IPDHCPC_OPTCODE_END;

    /* Send the DHCP client packet. */
    ip_assert(i < 312);

    length = ip_offsetof(Ipdhcpc_pkt, options) + i;
    length = IP_MAX(IPDHCPC_PKT_MINLEN, length);
#ifdef IPNET
    {
        struct Ip_in_pktinfo    *pktinfo    = IP_NULL;
        struct Ip_iovec         vec;
        struct Ip_msghdr        msg;
        struct Ip_cmsghdr       *c;
        Ip_u8                   cmsg_buf[IP_CMSG_SPACE(sizeof(struct Ip_in_pktinfo))];

        ipcom_memset(&msg, 0, sizeof(msg));

        /* Initialize vector */
        vec.iov_base        = ipdhcpc.out_pkt;
        vec.iov_len         = length;

        /* Initialize name */
        msg.msg_name        = addr;
        msg.msg_namelen     = sizeof(struct Ip_sockaddr);

        /* Point to the IO vector */
        msg.msg_iov       = &vec;
        msg.msg_iovlen    = 1;

        /* Use ancillary data to specify outgoing interface. */
        c = (struct Ip_cmsghdr *) cmsg_buf;
        c->cmsg_len   = IP_CMSG_LEN(sizeof(struct Ip_in_pktinfo));
        c->cmsg_level = IP_IPPROTO_IP;
        c->cmsg_type  = IP_IP_PKTINFO;

        pktinfo = IP_CMSG_DATA(c);
        ipcom_memset(pktinfo, 0, sizeof(*pktinfo));
        pktinfo->ipi_addr.s_addr    = 0;
        pktinfo->ipi_ifindex        = ift->ifindex;

        msg.msg_control    = c;
        msg.msg_controllen = sizeof(cmsg_buf);

        /* Send advertisement. */
        wrote = ipcom_sendmsg(ipdhcpc.fd, &msg, 0);
    }
#else
    wrote = ipcom_sendto(ipdhcpc.fd,
                         ipdhcpc.out_pkt,
                         length,
                         0,
                         (struct Ip_sockaddr *) addr,
                         sizeof(struct Ip_sockaddr));
#endif

    if (length != wrote)
    {
        IPCOM_LOG3(WARNING, "ipdhcpc[%s] :: sendto(%d) failed, errno = %d",
                   ift->ifname,
                   length, wrote == -1 ? ipcom_errno : wrote);
    }
    else
        ipdhcpc_print_packet(ift, handle, ipdhcpc.out_pkt, IP_FALSE,
                             length);
}


/*
 *===========================================================================
 *                    ipdhcpc_reply_input
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipdhcpc_reply_input(Ipdhcpc_if *ift, Ipdhcpc_handle *handle)
{
    Ip_u8 *option;
    Ip_u8 *opt_serverid;
    int    bootp_mode;
    
    /* hlen - hardware address length must match. */
    if (ift->hlen != ipdhcpc.in_pkt->hlen)
        return;

    /* chaddr - hardware address must match. */
    if (ipcom_memcmp(ipdhcpc.in_pkt->chaddr, ift->chaddr, ift->hlen) != 0)
        return;

    /* Optional options[0-4] - magic cookie must match.	 */
    if (ipdhcpc.in_pkt->options[0] !=   (Ip_u8)((handle->magic_cookie >> 24) & 0xff)
        || ipdhcpc.in_pkt->options[1] !=(Ip_u8)((handle->magic_cookie >> 16) & 0xff)
        || ipdhcpc.in_pkt->options[2] !=(Ip_u8)((handle->magic_cookie >> 8) & 0xff)
        || ipdhcpc.in_pkt->options[3] !=(Ip_u8)((handle->magic_cookie >> 0) & 0xff))
    {
        IPCOM_LOG1(DEBUG2, "ipdhcpc[%s] :: input discard - bad magic cookie", ift->ifname);
        return;
    }

    handle->priv->in_optlen =(Ip_u16)(ipdhcpc.in_pkt_len - ip_offsetof(Ipdhcpc_pkt, options) - 4);
    option = ipdhcpc_koption_get(&ipdhcpc.in_pkt->options[4],
                                 handle->priv->in_optlen,
                                 IPDHCPC_OPTCODE_DHCP_MESSAGE_TYPE);
    
    bootp_mode = ipcom_sysvar_get_as_int("ipdhcpc.bootp_mode",IPDHCPC_BOOTP_MODE);
    
    if (bootp_mode && ipdhcpc.in_pkt->op == IPDHCPC_OP_BOOTREPLY)
    {
    	handle->priv->msgtype = IP_NULL;
    	ipdhcpc_bootp_input(ift, handle);
    	return;
    }
    
    /***** Get mandatory reply message type. */
    
    if (option == IP_NULL || option[2] == 0 || option[2] > IPDHCPC_MSG_INFORM)
    {
        IPCOM_LOG1(WARNING, "ipdhcpc[%s] :: input discard - missing/illegal message type option", ift->ifname);
        return;
    }
    handle->priv->msgtype = option[2];

    /***** Get mandatory server id. *****/
    opt_serverid = ipdhcpc_koption_get(&ipdhcpc.in_pkt->options[4],
                                       handle->priv->in_optlen,
                                       IPDHCPC_OPTCODE_DHCP_SERVER_IDENTIFIER);
    if (opt_serverid == IP_NULL || opt_serverid[1] != 4)
    {
        IPCOM_LOG1(WARNING, "ipdhcpc[%s] :: input discard - missing/illegal server id option", ift->ifname);
        return;
    }

    /*
     * Check that no "MUST NOT" options are included in the NAK.
     * RFC2131:p29
     */
    option = &ipdhcpc.in_pkt->options[4];
    if (handle->priv->msgtype == IPDHCPC_MSG_NAK)
    {
        int i;
        for (i = 0; i < handle->priv->in_optlen; i++)
        {
            if (option[i] == IPDHCPC_OPTCODE_PAD)
                continue;	/* padding, skip. */
            if (option[i] == IPDHCPC_OPTCODE_END)
                break;	/* option end code. */
            if (i + 1 + option[i + 1] >= handle->priv->in_optlen)
                break;	/* truncated option. */
            if (option[i] != IPDHCPC_OPTCODE_DHCP_MAX_MESSAGE_SIZE &&
                option[i] != IPDHCPC_OPTCODE_DHCP_MESSAGE &&
                option[i] != IPDHCPC_OPTCODE_DHCP_CLIENT_IDENTIFIER &&
                option[i] != IPDHCPC_OPTCODE_VENDOR_CLASS_IDENTIFIER &&
                option[i] != IPDHCPC_OPTCODE_DHCP_SERVER_IDENTIFIER)
            {
                IPCOM_LOG1(DEBUG2, "ipdhcpc[%s] :: input discard - illegal option included in NAK", ift->ifname);
                return;
            }
        }
    }
    /*
     * Check that no "MUST NOT" options are included in the OFFER or ACK.
     * RFC2131:p29
     */
    else
    {
        if (ipdhcpc_koption_get(option, handle->priv->in_optlen,
                                IPDHCPC_OPTCODE_DHCP_REQUESTED_ADDRESS) ||
            ipdhcpc_koption_get(option, handle->priv->in_optlen,
                                IPDHCPC_OPTCODE_DHCP_PARAMETER_REQUEST_LIST) ||
            ipdhcpc_koption_get(option, handle->priv->in_optlen,
                                IPDHCPC_OPTCODE_DHCP_MAX_MESSAGE_SIZE) ||
            ipdhcpc_koption_get(option, handle->priv->in_optlen,
                                IPDHCPC_OPTCODE_DHCP_CLIENT_IDENTIFIER))
        {
            IPCOM_LOG1(DEBUG2, "ipdhcpc[%s] :: input discard - illegal option included in OFFER/ACK", ift->ifname);
            return;
        }
    }

    /* Act on the reply msgtype. */
    IPCOM_LOG2(DEBUG2, "ipdhcpc[%s] :: parsing %s reply message",
               ift->ifname,
               ipdhcpc_msg_name[handle->priv->msgtype]);
    
    switch(handle->priv->msgtype)
    {
    case IPDHCPC_MSG_OFFER:
        ipdhcpc_offer_input(ift, handle, opt_serverid);
        break;
    case IPDHCPC_MSG_ACK:
        ipdhcpc_ack_input(ift, handle, opt_serverid);
        break;
    case IPDHCPC_MSG_NAK:
        ipdhcpc_nak_input(ift, handle, opt_serverid);
        break;

        /* Client messages, should not happen. */
    default:
        IPCOM_LOG2(WARNING, "ipdhcpc[%s] :: input discard - bad reply message %s",
                   ift->ifname,
                   ipdhcpc_msg_name[handle->priv->msgtype]);
        return;
    }
}


/*
 *===========================================================================
 *                    ipdhcpc_koption_get
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_u8 *
ipdhcpc_koption_get(Ip_u8 *options, int options_length, Ip_u8 code)
{
    int i;

    /* Scan options for msgtype and check client-identifier. */
    for (i = 0; i < options_length; i++)
    {
        /* padding, skip. */
        if (options[i] == IPDHCPC_OPTCODE_PAD)
            continue;

        /* option end code. */
        if (options[i] == IPDHCPC_OPTCODE_END)
            break;

        /* check that option is not truncated. */
        if (i + 1 + options[i + 1] >= options_length)
        {
            IPCOM_LOG1(WARNING, "ipdhcpc_koption_get() - truncated option %d",
                       (int) options[i]);
            break;
        }

        /* Found option we are looking for. */
        if (options[i] == code)
            return &options[i];

        /* Next option. */
        i += 1 + options[i + 1];
        continue;
    }

    /* Not found. */
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipdhcpc_offer_input
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipdhcpc_offer_input(Ipdhcpc_if *ift, Ipdhcpc_handle *handle,
                    Ip_u8 *opt_serverid)
{
    /* Only collect OFFER replies in selecting state. */
    if (handle->priv->state != IPDHCPC_STATE_SELECTING)
    {
        IPCOM_LOG1(DEBUG2, "ipdhcpc[%s] :: input discard - only collecting offers in selecting state", ift->ifname);
        return;
    }

    /* Save server ID. */
    handle->priv->serverid_n =(Ip_u32)((opt_serverid[2] << 24) +
                                       (opt_serverid[3] << 16) +
                                       (opt_serverid[4] << 8) +
                                       (opt_serverid[5] << 0));
    handle->priv->serverid_n = IP_SWAP32(handle->priv->serverid_n);
    if (handle->priv->serverid_n == 0)
    {
        IPCOM_LOG1(WARNING, "ipdhcpc[%s] :: input discard - zero server id option", ift->ifname);
        return;
    }

    handle->priv->state = IPDHCPC_STATE_REQUESTING;

    /* Ok packet, save some parameters. */
    IPCOM_LOG1(DEBUG2, "ipdhcpc[%s] :: Valid OFFER -> REQUESTING state", ift->ifname);

    /* 'your'(client) IP address. The address the server offers us. */
    handle->priv->yiaddr = ipdhcpc.in_pkt->yiaddr;

    /* Send DHCP REQUEST */
    handle->priv->retrans_counter = 0;

    /* Update USER info handle structure.*/
    handle->info.ipaddr_n = ipdhcpc.in_pkt->yiaddr;
    handle->info.serveraddr_n = handle->priv->serverid_n;
    handle->info.optlen = handle->priv->in_optlen;
    
    ipcom_memcpy(handle->info.options,
                 &ipdhcpc.in_pkt->options[4],
                 handle->info.optlen);
    ipcom_memcpy(handle->info.sname,
                 ipdhcpc.in_pkt->sname,
                 sizeof(ipdhcpc.in_pkt->sname));
    handle->info.bootaddr_n = ipdhcpc.in_pkt->siaddr;
    ipcom_memcpy(handle->info.file,
                 ipdhcpc.in_pkt->file,
                 sizeof(ipdhcpc.in_pkt->file));   
    
    if (handle->reply_cb != IP_NULL)
    {
        if(handle->reply_cb(handle, IPDHCPC_REPLY_CB_EVENT_OFFER) == IPDHCPC_REPLY_CB_ERROR)
        {
            ipdhcpc_output_request(ift, handle, IPDHCPC_MSG_DECLINE);
            handle->priv->state = IPDHCPC_STATE_SELECTING;
            return;
        }
    }
    
    ipdhcpc_output_request(ift, handle, IPDHCPC_MSG_REQUEST);
}


/*
 *===========================================================================
 *                    ipdhcpc_ack_input
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipdhcpc_ack_input(Ipdhcpc_if *ift, Ipdhcpc_handle *handle,
                  Ip_u8 *opt_serverid)
{
    int    prev_state = handle->priv->state;
    struct Ip_in_aliasreq   ifalias;
    struct Ip_sockaddr_in *addrp;
    struct Ip_sockaddr_in *maskp;

    ipcom_memset(&ifalias, 0, sizeof(ifalias));

    /* Discard ACK in the following states: */
    if (handle->priv->state <= IPDHCPC_STATE_INIT_REBOOT ||
        handle->priv->state == IPDHCPC_STATE_SELECTING ||
        handle->priv->state == IPDHCPC_STATE_BOUND)
    {
        IPCOM_LOG2(DEBUG2, "ipdhcpc[%s] :: discarding ACK in %s state",
                   ift->ifname,
                   ipdhcpc_state_name[handle->priv->state]);
        return;
    }

    if (handle->priv->state == IPDHCPC_STATE_REBINDING)
    {
        /* Found server that can extend the lease. Save its id */
        handle->priv->serverid_n =(Ip_u32)((opt_serverid[2] << 24) +
                                           (opt_serverid[3] << 16) +
                                           (opt_serverid[4] << 8) +
                                           (opt_serverid[5] << 0));
        handle->priv->serverid_n = IP_SWAP32(handle->priv->serverid_n);
        if (handle->priv->serverid_n == 0)
        {
            IPCOM_LOG1(WARNING, "ipdhcpc[%s] :: input discard - zero server id option",
                       ift->ifname);
            return;
        }
    }
    else if (ipcom_memcmp
             (&opt_serverid[2], &handle->priv->serverid_n, 4))
    {
        /* Ignore ACK if not from the chosen server. */
        IPCOM_LOG1(DEBUG2, "ipdhcpc[%s] :: input ACK discard - not my server", ift->ifname);
        return;
    }

    /* ACK on existing lease. Update lease. */
    ip_assert(ift->handle == IP_NULL || handle->priv->state >= IPDHCPC_STATE_REBOOTING);
    ip_assert(ift->handle != IP_NULL || handle->priv->state < IPDHCPC_STATE_RENEWING);

    /* Set IPv4 address on this interface.*/

    if (handle->priv->state < IPDHCPC_STATE_RENEWING)
    {
        Ip_u8                 *option;

        /* Act on the DHCP options we got from the server.*/
        for (option = ipdhcpc_option_get_first(&ipdhcpc.in_pkt->options[4]);
             option != IP_NULL;
             option = ipdhcpc_option_get_next(option))
        {   	
            /* Let user application look at/act on the option */
            if (ipdhcpc_option_callback(handle, option) == IP_FALSE)
                continue;

            if (*option == IPDHCPC_OPTCODE_SUBNET_MASK)
            {
            	/* Set the mask, as the address may be in use while/if we 
            	 * perform duplicate address detection
            	 */
            	
                maskp = (struct Ip_sockaddr_in *)&ifalias.ifra_mask;
                maskp->sin_family      = IP_AF_INET;
                IPCOM_SA_LEN_SET(maskp, sizeof(struct Ip_sockaddr_in));
                maskp->sin_addr.s_addr = IP_GET_32ON8(&option[2]);
              
                break;
            }
        }

        addrp = &ifalias.ifra_addr;
    	(void)ipcom_if_indextoname(handle->ifindex, ifalias.ifra_name);
    	addrp->sin_family = IP_AF_INET;
    	IPCOM_SA_LEN_SET(addrp, sizeof(struct Ip_sockaddr_in));
    	addrp->sin_addr.s_addr = ipdhcpc.in_pkt->yiaddr;
    	if (ipcom_socketioctl(ipdhcpc.fd, IP_SIOCAIFADDR, &ifalias) < 0)
    	{
            IPCOM_LOG2(ERR, "ipdhcpc[%s] :: ipdhcpc_ack_input(): ipcom_ioctl(IP_SIOCSIFADDR) failed, errno = %d",
                       ifalias.ifra_name, ipcom_errno);
            return;
    	}

#ifdef IPNET
    	{
            /*
             * RFC2131:p17. Perform gratuitious ARP and send DHCPDECLINE
             * if address is busy. If busy, wait minimum 10 seconds to avoid
             * excessive network traffic.
             */
            int sec;

            sec = ipcom_sysvar_get_as_int("ipnet.inet.BaseRetransmitTime", 0);
            if (sec > 0)
            {
                int n;
                IPCOM_LOG2(INFO,
                           "ipdhcpc[%s] :: ipdhcpc_ack_input(): Sleeping %d seconds for gratuitous ARP",
                           ift->ifname, sec);

                for (n = 0; n < sec; n++)
                {
                    ipcom_sleep(1);

                    /* Verify that the address is set */
    				
    				if (ipdhcpc_if_has_addr(handle->ifindex,ipdhcpc.in_pkt->yiaddr) != IP_TRUE)
                    {
                        IPCOM_LOG1(WARNING,
                                   "ipdhcpc[%s] :: ipdhcpc_ack_input(): Offered DHCP address declined (duplicate detected)",
                                   ift->ifname);

                        /* Send a decline to the server */
                        ipdhcpc_output_request(ift, handle, IPDHCPC_MSG_DECLINE);

                        /* Restart */
                        ipdhcpc_state_restart(handle);
                        return;
                     }
                }
   
            }
    	}
#endif /* IPNET */
    }
    
    /* We are now in bound state. */
    handle->priv->state = IPDHCPC_STATE_BOUND;
    handle->priv->yiaddr = ipdhcpc.in_pkt->yiaddr;
    handle->clientaddr_n = handle->priv->yiaddr;

    /* Update USER info handle structure.*/
    ip_assert(handle->ifindex == (int)ift->ifindex);
    handle->info.ipaddr_n = ipdhcpc.in_pkt->yiaddr;
    handle->info.serveraddr_n = handle->priv->serverid_n;
    handle->info.optlen = handle->priv->in_optlen;

    ipcom_memcpy(handle->info.options,
                 &ipdhcpc.in_pkt->options[4],
                 handle->info.optlen);
    ipcom_memcpy(handle->info.sname,
                 ipdhcpc.in_pkt->sname,
                 sizeof(ipdhcpc.in_pkt->sname));
    handle->info.bootaddr_n = ipdhcpc.in_pkt->siaddr;
    ipcom_memcpy(handle->info.file,
                 ipdhcpc.in_pkt->file,
                 sizeof(ipdhcpc.in_pkt->file));

    /* Cancel the timers. */
    ipdhcpc_timer_cancel(handle, 0);

    if (prev_state == IPDHCPC_STATE_RENEWING)
    {
        IPCOM_LOG1(DEBUG2, "ipdhcpc[%s] :: Successful ACK, renewing lease", ift->ifname);
        ipdhcpc_timer_set(handle, ipdhcpc_sec_to_msec(ift->lease.t1), 1);         
    }
    else
    {
        IPCOM_LOG1(DEBUG2, "ipdhcpc[%s] :: Successful ACK, ready to lease", ift->ifname);

        /* Wakeup blocking request or call the callback. */
        ipdhcpc_wakeup(handle,
                       IPCOM_SUCCESS,
                       IPDHCPC_REPLY_CB_EVENT_ACK);
    }
}


/*
 *===========================================================================
 *                    ipdhcpc_nak_input
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipdhcpc_nak_input(Ipdhcpc_if *ift, Ipdhcpc_handle *handle, Ip_u8 *opt_serverid)
{
    (void)ift;
    (void)opt_serverid;	/* !!should it be checked? */

    if (handle->reply_cb != IP_NULL)
        (void)handle->reply_cb(handle, IPDHCPC_REPLY_CB_EVENT_NAK);

    /* Discard NAK in the following states: */
    if (handle->priv->state <= IPDHCPC_STATE_INIT_REBOOT ||
        handle->priv->state == IPDHCPC_STATE_SELECTING ||
        handle->priv->state == IPDHCPC_STATE_BOUND)
    {
        IPCOM_LOG2(DEBUG2, "ipdhcpc[%s] :: input discard - ignoring NAK in %s state",
                   ift->ifname, ipdhcpc_state_name[handle->priv->state]);
        return;
    }

    switch(handle->priv->state)
    {
    case IPDHCPC_STATE_RENEWING:
    case IPDHCPC_STATE_REBINDING:
        /* NAK on this lease, unlease. */
        if (handle == ift->handle)
            (void)ipdhcpc_ifunlease_locked(ift, IP_NULL);
        /* fall through  */

    case IPDHCPC_STATE_REQUESTING:
    case IPDHCPC_STATE_REBOOTING:
        ip_assert(ift->handle != handle);

        IPCOM_LOG2(DEBUG2, "ipdhcpc[%s] :: DHCP NAK -> restarting in %s state",
                   ift->ifname,
                   ipdhcpc_state_name[handle->priv->state]);

        ipdhcpc_state_restart(handle);
        break;

    default:
        IP_PANIC();
        break;
    }
}

/*
 *===========================================================================
 *                    ipdhcpc_bootp_input
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipdhcpc_bootp_input(Ipdhcpc_if *ift, Ipdhcpc_handle *handle)
{    
    /* ip_assert(ift->handle == IP_NULL); */
    
    /* We are now in bound state. */
    handle->priv->state = IPDHCPC_STATE_BOUND;
    handle->priv->yiaddr = ipdhcpc.in_pkt->yiaddr;

    /* Update USER info handle structure.*/
    ip_assert(handle->ifindex == (int)ift->ifindex);
    handle->info.ipaddr_n = ipdhcpc.in_pkt->yiaddr;
    handle->info.serveraddr_n = handle->priv->serverid_n;
    handle->info.optlen = handle->priv->in_optlen;

    ipcom_memcpy(handle->info.options,
                 &ipdhcpc.in_pkt->options[4],
                 handle->info.optlen);
    ipcom_memcpy(handle->info.sname,
                 ipdhcpc.in_pkt->sname,
                 sizeof(ipdhcpc.in_pkt->sname));
    handle->info.bootaddr_n = ipdhcpc.in_pkt->siaddr;
    ipcom_memcpy(handle->info.file,
                 ipdhcpc.in_pkt->file,
                 sizeof(ipdhcpc.in_pkt->file));

    handle->priv->tmo_ignore = 1;

    IPCOM_LOG1(DEBUG2, "ipdhcpc[%s] :: Successful ACK, ready to lease", ift->ifname);  
    
    ipdhcpc_wakeup(handle,
                   IPCOM_SUCCESS,
                   IPDHCPC_REPLY_CB_EVENT_BOOTREPLY);
}


/*
 *===========================================================================
 *                    ipdhcpc_ifunlease_locked
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipdhcpc_ifunlease_locked(Ipdhcpc_if *ift, Ipdhcpc_handle **handlep)
{
    struct Ip_ifreq  ifreq;
    struct Ip_sockaddr_in *addrp;	
    char   buf[16];
    int    remove_addr = 0;
    
    /* No lease. */
    if (ift->handle == IP_NULL)
        return IPCOM_ERR_NOT_INSTALLED;

    /* Cancel timers */
    
    ipdhcpc_timer_cancel(ift->handle, 0);

    if (ipdhcpc_if_has_addr(ift->ifindex, ift->handle->info.ipaddr_n) != IP_TRUE)
    {
    	return IPCOM_ERR_FAILED;	
    }

	remove_addr = ipcom_sysvar_get_as_int("ipdhcpc.remove_leased_address",
                                           IPDHCPC_REMOVE_LEASED_ADDRESS);
	
        if (remove_addr > 0)
        {
	    /* Remove address. */		
		
            ipcom_memset(&ifreq, 0, sizeof(struct Ip_ifreq));
            ipcom_strcpy(ifreq.ifr_name, ift->ifname);
            addrp             = (struct Ip_sockaddr_in *)&ifreq.ip_ifr_addr;
            addrp->sin_family = IP_AF_INET;
            addrp->sin_len    = sizeof(struct Ip_sockaddr_in);
            addrp->sin_addr.s_addr = ift->handle->info.ipaddr_n;

            if (addrp->sin_addr.s_addr == 0)
                return IPCOM_ERR_FAILED;

            IPCOM_LOG2(DEBUG2, "ipdhcpc[%s] :: removed address = %s",
                       ift->ifname,
                       ipcom_inet_ntop(IP_AF_INET,
                       &addrp->sin_addr.s_addr, 
                       buf, 
                       sizeof(buf)));
	
            if (ipcom_socketioctl(ipdhcpc.fd, IP_SIOCDIFADDR, &ifreq) < 0)
            {
                IP_PANIC2();
                IPCOM_LOG2(ERR, "ipdhcpc[%s] :: ipdhcpc_ifunlease_locked(): ipcom_ioctl(IP_SIOCDIFADDR) failed, errno = %d",
                           ifreq.ifr_name, ipcom_errno);
                return IPCOM_ERR_FAILED;;
            }      	
        }
    
    /* Remove lease. */
    if (handlep)
        *handlep = ift->handle;
    ift->handle = IP_NULL;

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipdhcpc_proc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC
IPCOM_PROCESS(ipdhcpc_proc)
{
    Ip_err ret;

    ipcom_proc_init();

    ret = ipdhcpc_init();
    if (ret != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "ipdhcpc_proc :: ipdhcpc_init() failed, ret=%ld", ret);
        goto exit;
    }

    ret = ipdhcpc_run();
    if (ret != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "ipdhcpc_proc :: ipdhcpc_run() failed, ret=%ld", ret);
        goto exit;
    }

    /* IPD init */
    ret = ipcom_ipd_init("ipdhcpc", IPCOM_SUCCESS, &ipdhcpc.ipd_fd);
    if (ret != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "ipdhcpc_proc :: ipcom_ipd_init() failed, ret=%ld", ret);
        goto exit;
    }

    /* Run daemon */
    ipdhcpc_daemon();

    /* Clean up */
    ipdhcpc_ifclose_all();

    /* Remove all interfaces */
    {
        Ipdhcpc_if *ift;

        while ((ift = ipdhcpc.ift_head) != IP_NULL)
            ipdhcpc_ifclose(ift->ifindex);
    }

    ipdhcpc_handle_free_pending();

 exit:
    IPCOM_LOG0(INFO,"ipdhcpc_proc :: shutting down");

    if (ipdhcpc.ipd_fd == IP_INVALID_SOCKET)
        (void)ipcom_ipd_init("ipdhcpc", ret, &ipdhcpc.ipd_fd);

    ipdhcpc_exit();

    ip_assert(ipdhcpc.ipd_fd != IP_INVALID_SOCKET);
    (void)ipcom_ipd_exit("ipdhcpc", ipdhcpc.ipd_fd);

    ipcom_proc_exit();
}


/*
 *===========================================================================
 *                  ipdhcpc_daemon
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipdhcpc_daemon(void)
{
    struct Ip_sockaddr_in addr;
    Ip_socklen_t          socklen;
    Ipdhcpc_handle       *handle;
    Ipdhcpc_if           *ift;
    int                   num;
    int                   max_fd;
    Ip_fd_set             read_set;

    ip_assert(ipdhcpc.opened == IP_TRUE);

    for (;;)
    {
        IP_FD_ZERO(&read_set);
        max_fd = IP_MAX(ipdhcpc.fd, ipdhcpc.ipd_fd);
        max_fd = IP_MAX(max_fd, ipdhcpc.rt_fd);
        
        IP_FD_SET(ipdhcpc.rt_fd, &read_set);
        IP_FD_SET(ipdhcpc.ipd_fd, &read_set);
        IP_FD_SET(ipdhcpc.fd, &read_set);

        num = ipcom_socketselect(max_fd + 1, &read_set, IP_NULL, IP_NULL, IP_NULL);
        if (num < 0)
        {
            IPCOM_LOG1(WARNING, "ipcom_socketselect() failed, errno = %d", ipcom_errno);
            (void)ipcom_sleep(1);
            continue;
        }

        if (num > 0 && IP_FD_ISSET(ipdhcpc.ipd_fd, &read_set))
        {
            int event;

            event = ipcom_ipd_input(ipdhcpc.ipd_fd);

            num--;
            switch(event)
            {
            case IPCOM_IPD_EVENT_RECONFIGURE:
                IPCOM_LOG0(INFO, "received IPD reconfigure message");
                break;

            case IPCOM_IPD_EVENT_KILL:
                IPCOM_LOG0(INFO, "received IPD kill message");
                return;

            default:
                /* Ignore all other messages */
                break;
            }
        }

        if (num > 0 && IP_FD_ISSET(ipdhcpc.rt_fd, &read_set))
        {
            char         ifname[IP_IFNAMSIZ + 1];
            Ip_ssize_t   length;
            union rtmsg_union
            {
                struct Ipnet_rt_msghdr          *rthdr;
                struct Ipnet_if_msghdr          *ifmsg;
                struct Ipnet_ifa_msghdr         *ifamsg;
                struct Ipnet_if_announcemsghdr  *ifannmsg;
                void                            *buffer;
            } rt_type;
            rt_type.buffer = ipdhcpc.out_pkt;

            length = ipcom_recv(ipdhcpc.rt_fd, rt_type.buffer, sizeof(Ipdhcpc_pkt), 0);
            if (length == IP_SOCKERR)
            {
                if (ipcom_errno == IP_ERRNO_EWOULDBLOCK)
                    goto more;
                IPCOM_LOG1(WARNING, "ipcom_recv(routing) failed, errno = %d", ipcom_errno);
            }
            else
            {
                if ((Ip_size_t)length < sizeof(struct Ipnet_rs_msghdr))
                    goto more;

                switch(rt_type.rthdr->rtm_type)
                {
                case IPNET_RTM_IFINFO:
                    if ((Ip_size_t)length < sizeof(struct Ipnet_ifa_msghdr))
                        goto more;
                    if (ipcom_if_indextoname(rt_type.ifamsg->ifam_index, ifname) == IP_NULL)
                        goto more;

                    IPCOM_LOG1(DEBUG2, "received RT IFINFO for interface %s", ifname);
                    ipdhcpc_if_eventcb(ifname, rt_type.ifamsg->ifam_index, rt_type.ifamsg->ifam_flags, IP_EIOXCHANGE, IP_NULL);
                    break;

                case IPNET_RTM_IFANNOUNCE:
                    if ((Ip_size_t)length < sizeof(struct Ipnet_if_announcemsghdr))
                        goto more;

                    if (ipcom_if_indextoname(rt_type.ifannmsg->ifan_index, ifname) == IP_NULL)
                        goto more;

                    if (rt_type.ifannmsg->ifan_what == IPNET_IFAN_DEPARTURE)
                    {
                        IPCOM_LOG1(DEBUG2, "received RT IFANNOUNCE=departure for interface %s", ifname);
                        if (ipdhcpc_if_isopen(rt_type.ifannmsg->ifan_index))
                            (void)ipdhcpc_ifclose(rt_type.ifannmsg->ifan_index);
                    }
                    else
                    {
                        IPCOM_LOG1(DEBUG2, "received RT IFANNOUNCE=arrival for interface %s", ifname);
                        /*!! do what? */
                    }
                    break;

                default:
                    break;
                }
            }
        }
    more:

        if (num == 0 || !IP_FD_ISSET(ipdhcpc.fd, &read_set))
            continue;

        /* Received DHCP packet. */
        socklen = sizeof(struct Ip_sockaddr);
        ipdhcpc.in_pkt_len = ipcom_recvfrom(ipdhcpc.fd,
                                            ipdhcpc.in_pkt,
                                            ipdhcpc.max_message_size,
                                            0,
                                            (struct Ip_sockaddr *)&addr,
                                            &socklen);

        /* read error. */
        if (ipdhcpc.in_pkt_len < 0)
        {
            IPCOM_LOG1(DEBUG2, "ipdhcpc_daemon :: recvfrom() failed, errno = %d", ipcom_errno);
            continue;
        }

        /* too short packet. */
        if (ipdhcpc.in_pkt_len < sizeof(Ipdhcpc_pkt))
            continue;

        /* not a bootp/dhcp reply. */
        if (ipdhcpc.in_pkt->op != IPDHCPC_OP_BOOTREPLY)
            continue;

        IPDHCPC_LOCK();

        /* Check our outstanding list of requests. */
        for (handle = ipdhcpc.handle_head; handle != IP_NULL;
             handle = handle->priv->next)
        {
            /* xid must match.  */
            if (handle->priv->xid_n != ipdhcpc.in_pkt->xid)
                continue;

            /* interface must still be active. */
            for (ift = ipdhcpc.ift_head; ift != IP_NULL;
                 ift = ift->next)
                if (ift->ifindex == (Ip_u32)handle->ifindex)
                    break;
            
            if (ift == IP_NULL)
                continue;

            /* Possible reply match, parse it.*/
            ipdhcpc_print_packet(ift,
                                 handle,
                                 ipdhcpc.in_pkt,
                                 IP_TRUE,
                                 ipdhcpc.in_pkt_len);
            ipdhcpc_reply_input(ift, handle);
            goto next;
        }

        /* Check our interfaces for current leases. */
        for (ift = ipdhcpc.ift_head; ift != IP_NULL; ift = ift->next)
            if (ift->handle && ift->handle->priv->xid_n == ipdhcpc.in_pkt->xid)
            {
                /* Possible reply match, parse it. */
                ipdhcpc_print_packet(ift,
                                     ift->handle,
                                     ipdhcpc.in_pkt,
                                     IP_TRUE,
                                     ipdhcpc.in_pkt_len);
                ipdhcpc_reply_input(ift, ift->handle);
                goto next;
            }

    next:
        IPDHCPC_UNLOCK();
    }
}


/*
 *===========================================================================
 *                    ipdhcpc_if_eventcb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipdhcpc_if_eventcb(char *ifname, Ip_u32 ifindex, Ip_u32 ifflags, int event, void *data)
{
    Ip_err                 ret;
    Ipdhcpc_handle        *handle;
    Ip_bool                isopen;
    struct Ip_ifreq        ifreq;
    struct Ip_sockaddr_in *addrp;
    char                   buf[50];
    int                    bootp_mode;
    int                    remove_addr;
    Ipdhcpc_if            *ift = IP_NULL;

    (void)ifname;
    (void)data;

    switch (event)
    {
    case IP_EIOXUP:
    case IP_EIOXDOWN:
    case IP_EIOXCHANGE:
        break;
    default:
        return;
    }

    IPDHCPC_LOCK();

    if (ipdhcpc.opened == IP_FALSE)
        isopen = IP_FALSE;
    else
    {
        for (ift = ipdhcpc.ift_head; ift != IP_NULL; ift = ift->next)
            if (ift->ifindex == ifindex)
                break;

        isopen = (ift != IP_NULL);
    }

    IPDHCPC_UNLOCK();


    if ((ifflags & (IP_IFF_UP | IP_IFF_X_DHCPRUNNING)) == (IP_IFF_UP | IP_IFF_X_DHCPRUNNING))
    {
        Ipdhcpc_ifopen   conf;

        /* UP with DHCP */
        if (isopen)
        {
            if (ift != IP_NULL && ift->isdown)
            {
                IPCOM_LOG1(INFO,"ipdhcpc_if_eventcb :: ipdhcpc_ifrequest() interface '%d' is up."IP_LF, ifindex);
                ift->isdown = IP_FALSE;

                /* Do verify immediately */
                if (ift->handle != IP_NULL)
                {
                    /**/
                    if (ift->handle->priv->state > IPDHCPC_STATE_SELECTING)
                        ipdhcpc_state_init_reboot(ift->handle);
                    else
                        ipdhcpc_state_restart(ift->handle);
                }
            }
            return;
        }

        ipcom_memset(&conf, 0, sizeof(Ipdhcpc_ifopen));
        conf.api_version = 1;
        conf.conf[0] = IP_TAG_END;

        ret = ipdhcpc_ifopen(ifindex, &conf);
        if (ret != IPCOM_SUCCESS)
        {
            IPCOM_LOG2(ERR,"ipdhcpc_if_eventcb :: ipdhcpc_ifopen() failed on interface '%s', err = %d."IP_LF, ifname, ret);
            return;
        }

        /* Allocate DHCP client handle. */
        ret = ipdhcpc_handle_malloc(ifindex, IPDHCPC_MODE_ALLOC_DYNAMIC, &handle);
        if (ret != IPCOM_SUCCESS)
        {
            IPCOM_LOG2(ERR,"ipdhcpc_if_eventcb :: ipdhcpc_handle_malloc() failed on interface '%d', err = %d."IP_LF, ifindex, ret);
            goto fail;
        }

        /* Initiate DHCP client request */
        handle->reply_cb = ipdhcpc_reply_callback;
        ret = ipdhcpc_ifrequest(ifindex, handle);
        if (ret != IPCOM_SUCCESS)
        {
            IPCOM_LOG2(ERR,"ipdhcpc_if_eventcb :: ipdhcpc_ifrequest() failed on interface '%d', err = %d."IP_LF, ifindex, ret);
            (void)ipdhcpc_handle_free(ifindex, handle);
            goto fail;
        }

        /* Success. */
        return;

    fail:
        ret = ipdhcpc_ifclose(ifindex);
        if (ret != IPCOM_SUCCESS)
        {
            IPCOM_LOG2(ERR,"ipdhcpc_if_eventcb :: ipdhcpc_ifclose() failed on interface '%d', err = %d."IP_LF, ifindex, ret);
        }
        return;
    }

    else if (isopen)
    {
        if ((ifflags & IP_IFF_X_DHCPRUNNING) == IP_IFF_X_DHCPRUNNING)
        {
            if (ift != IP_NULL && ift->handle != IP_NULL)
            {
                IPCOM_LOG1(INFO,"ipdhcpc_if_eventcb :: ipdhcpc_ifrequest() interface '%d' is down."IP_LF, ifindex);
                ift->isdown = IP_TRUE;
            }
            return;
        }

        bootp_mode = ipcom_sysvar_get_as_int("ipdhcpc.bootp_mode",IPDHCPC_BOOTP_MODE);
        
        /* Send out DHCP RELEASE if interface is valid and we are not in BOOTP mode */
        
        if (ift == IP_NULL || ift->handle == IP_NULL || bootp_mode)
    	{   		
            ret = ipdhcpc_ifclose(ifindex);

            if (ret != IPCOM_SUCCESS)
            {
                IPCOM_LOG2(ERR,"ipdhcpc_if_eventcb :: ipdhcpc_ifclose() failed on interface '%d', err = %d."IP_LF, ifindex, ret);
            }
    		
    	    return;
    	}
    	else
    	{
    	    if (ipcom_tmo_cancel(&ift->handle->priv->tmo) == IPCOM_ERR_PENDING)
    	        ift->handle->priv->tmo_ignore = 1;
    		
            if (ift->handle->priv->state > IPDHCPC_STATE_SELECTING)
                ipdhcpc_output_request(ift, ift->handle, IPDHCPC_MSG_RELEASE);
    	}

    	remove_addr = ipcom_sysvar_get_as_int("ipdhcpc.remove_leased_address",
                                               IPDHCPC_REMOVE_LEASED_ADDRESS);
    	
    	if (remove_addr > 0)
    	{
            /* We have released the address.  Remove it on the interface. */		
    		
            ipcom_strcpy(ifreq.ifr_name, ift->ifname);
            addrp             = (struct Ip_sockaddr_in *)&ifreq.ip_ifr_addr;
            addrp->sin_family = IP_AF_INET;
            addrp->sin_len    = sizeof(struct Ip_sockaddr_in);
            addrp->sin_addr.s_addr = ift->handle->info.ipaddr_n;

            if (addrp->sin_addr.s_addr == 0)
                goto fail;

            IPCOM_LOG2(DEBUG2, "ipdhcpc[%s] :: removed address = %s",
                       ift->ifname,
                       ipcom_inet_ntop(IP_AF_INET,
                                       &addrp->sin_addr.s_addr, 
                                       buf, 
                                       sizeof(buf)));
    	
            if (ipcom_socketioctl(ipdhcpc.fd, IP_SIOCDIFADDR, &ifreq) < 0)
            {
                IP_PANIC2();
                IPCOM_LOG2(ERR, "ipdhcpc[%s] :: ipdhcpc_ifopen(): ipcom_ioctl(IP_SIOCDIFADDR) failed, errno = %d",
                           ifreq.ifr_name, ipcom_errno);
                goto fail;
            }      	
    	}

        ret = ipdhcpc_ifclose(ifindex);

        if (ret != IPCOM_SUCCESS)
        {
            IPCOM_LOG2(ERR,"ipdhcpc_if_eventcb :: ipdhcpc_ifclose() failed on interface '%d', err = %d."IP_LF, ifindex, ret);
        }
    }
}


/*
 *===========================================================================
 *                    ipdhcpc_run
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipdhcpc_run(void)
{
    struct Ip_if_nameindex *oifrs, *ifrs;
    struct Ip_ifreq         ifreq;
    Ip_u16                  ifru_flags;

    /* Search for interfaces preconfigured for dhcp */
    ifrs = ipcom_if_nameindex();
    if (ifrs == IP_NULL)
        return IPCOM_ERR_FAILED;

    for (oifrs = ifrs; ifrs && ifrs->if_name != IP_NULL; ifrs++)
    {
        /* Check if interface is up. */
        strcpy(ifreq.ifr_name, ifrs->if_name);
        if (ipcom_socketioctl(ipdhcpc.fd, IP_SIOCGIFFLAGS,(char *)&ifreq) < 0)
        {
            IPCOM_LOG2(ERR,
                       "ipdhcpc[%s] :: ipcom_ioctl(SIOCGIFFLAGS) failed, errno =%d",
                       ifrs->if_name, ipcom_errno);
            continue;
        }
        if (IP_BIT_ISFALSE(ifreq.ifr_ifru.ifru_flags, IP_IFF_UP))
            continue;
        ifru_flags = ifreq.ifr_ifru.ifru_flags;

        /* Check if DHCP is enabled for this interface. */
        if (ipcom_socketioctl(ipdhcpc.fd, IP_SIOCXGDHCPRUNNING,(char *)&ifreq) < 0)
        {
            IPCOM_LOG2(ERR,
                       "ipdhcpc[%s] :: ipcom_ioctl(IP_SIOCXGDHCPRUNNING) failed, errno =%d",
                       ifrs->if_name, ipcom_errno);
            continue;
        }

        if (ifreq.ifr_ifru.ifru_opt == 1)
        {
            /* Start DHCP client on interface that has DHCPRUNNING flag set. */
            ipdhcpc_if_eventcb((char *)ifrs->if_name, ifrs->if_index,
                               ifru_flags | IP_IFF_X_DHCPRUNNING,
                               IP_EIOXCHANGE, IP_NULL);
        }
    }

    ipcom_if_freenameindex(oifrs);

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipdhcpc_reply_cb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipdhcpc_reply_cb(Ipdhcpc_handle *handle, int event)
{
    Ip_err ret = IPDHCPC_REPLY_CB_SUCCESS;
        
    switch(event)
    {
    case IPDHCPC_REPLY_CB_EVENT_OFFER:
        /* Informational only in this implementation. */
        IPCOM_LOG1(INFO, "ipdhcpc_reply_cb :: received OFFER on interface '%d'.", handle->ifindex);
        break;
    
    case IPDHCPC_REPLY_CB_EVENT_ACK:
    case IPDHCPC_REPLY_CB_EVENT_BOOTREPLY:
        ret = ipdhcpc_iflease(handle->ifindex, handle);
        if (ret != IPCOM_SUCCESS)
        {
            IPCOM_LOG2(ERR, "ipdhcpc_reply_cb :: ipdhcpc_iflease() failed on interface '%d', err = %d.",
                       handle->ifindex, ret);
            
            ret = IPDHCPC_REPLY_CB_ERROR;
        }
        break;

    case IPDHCPC_REPLY_CB_EVENT_NAK:
        /* Informational only in this implementation. */
        IPCOM_LOG1(INFO, "ipdhcpc_reply_cb :: received NAK on interface '%d'.", handle->ifindex);
        break;

    case IPDHCPC_REPLY_CB_EVENT_EXPIRE:
        /* Informational only in this implementation. */
        IPCOM_LOG1(INFO, "ipdhcpc_reply_cb :: Lease expired on interface '%d'.", handle->ifindex);
        break;

    case IPDHCPC_REPLY_CB_EVENT_FAIL_RETRY:
    	
        IPCOM_LOG1(INFO, "ipdhcpc_reply_cb :: Retries expired on interface '%d'.", handle->ifindex);

        (void)ipdhcpc_ifclose(handle->ifindex);
        (void)ipdhcpc_handle_free(handle->ifindex, handle);
    	
    	break;

    case IPDHCPC_REPLY_CB_EVENT_FAIL_IFLOST:
        IPCOM_LOG1(INFO, "ipdhcpc_reply_cb :: Interface '%d' lost.", handle->ifindex);
        break;
        
    default:
        (void)ipdhcpc_handle_free(handle->ifindex, handle);
        break;
    }

    return ret;
}


/*
 *===========================================================================
 *                    ipdhcpc_route_mod
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipdhcpc_route_mod(Ip_u32 ipaddr_n, Ip_bool add)
{
    struct Ip_ortentry      ort;
    struct Ip_sockaddr_in  *sin;
    int                     ret;

    memset(&ort, 0, sizeof(ort));
    
    /*!!ort.rt_flags = IPNET_RTF_UP | IPNET_RTF_GATEWAY | IPNET_RTF_STATIC;*/
    ort.rt_flags = 0x1 | 0x2 | 0x800;
    ort.rt_dst.sa_family = IP_AF_INET;
    ort.rt_gateway.sa_family = IP_AF_INET;
    sin = (struct Ip_sockaddr_in *)&ort.rt_gateway;
    IPCOM_SA_LEN_SET(sin, sizeof(struct Ip_sockaddr_in));
    sin->sin_addr.s_addr = ipaddr_n;

    ret = ipcom_socketioctl(ipdhcpc.fd, add ? IP_SIOCADDRT : IP_SIOCDELRT, &ort);
    if (ret != 0 && ipcom_errno == IP_ERRNO_EEXIST)
        ret = 0; /* ignore route already exists error. */
    return ret;
}


/*
 *===========================================================================
 *                    ipdhcpc_timer_cancel
 *===========================================================================
 * Description:   Cancel timers
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipdhcpc_timer_cancel(Ipdhcpc_handle *handle, int which)
{
    char    ifname[IP_IFNAMSIZ + 1];

    ip_assert(handle != IP_NULL);
    ip_assert(handle->priv != IP_NULL);
    ip_assert(which == 0 || which == 1 || which == 2);

    (void)ipcom_if_indextoname(handle->ifindex, ifname);

    if (which == 0 || which == 1)
    {
        handle->priv->tmo_ignore = 0;
        if (IPCOM_TMO_PENDING(&handle->priv->tmo))
        {
            IPCOM_LOG2(DEBUG2,
                       "ipdhcpc[%s] :: cancelling timer T1 (0x%lx)",
                       ifname, &handle->priv->tmo);
            if (ipcom_tmo_cancel(&handle->priv->tmo) == IPCOM_ERR_PENDING)
                handle->priv->tmo_ignore = 1;
        }
    }
    if (which == 0 || which == 2)
    {
        handle->priv->tmo2_ignore = 0;
        if (IPCOM_TMO_PENDING(&handle->priv->tmo_t2))
        {
            IPCOM_LOG2(DEBUG2,
                       "ipdhcpc[%s] :: cancelling timer T2 (0x%lx)",
                       ifname, &handle->priv->tmo_t2);
            if (ipcom_tmo_cancel(&handle->priv->tmo_t2) == IPCOM_ERR_PENDING)
                handle->priv->tmo2_ignore = 1;
        }
    }
}


/*
 *===========================================================================
 *                    ipdhcpc_timer_set
 *===========================================================================
 * Description:   Set T1 timer
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipdhcpc_timer_set(Ipdhcpc_handle *handle, Ip_u32 msec, int which)
{
    char    ifname[IP_IFNAMSIZ + 1];

    ip_assert(which == 1 || which == 2);

    (void)ipcom_if_indextoname(handle->ifindex, ifname);
    IPCOM_LOG4(DEBUG2,
               "ipdhcpc[%s] :: setting timer %s (0x%lx) to %ld msec",
               ifname,
               which == 1 ? "T1" : "T2",
               which == 1 ? &handle->priv->tmo : &handle->priv->tmo_t2,
               msec);

    (void)ipcom_tmo_request(which == 1 ? &handle->priv->tmo : &handle->priv->tmo_t2,
                            ipdhcpc_tmo_cb, handle,
                            msec);
}

/*
 *===========================================================================
 *                    ipdhcpc_ifclose_all
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipdhcpc_ifclose_all(void)
{
    Ipdhcpc_if      *ift;
    Ipdhcpc_if      *ift_next;
    struct Ip_ifreq  ifreq;

    for (ift = ipdhcpc.ift_head; ift != IP_NULL; ift = ift_next)
    {
        ift_next = ift->next;

        /* Disable DHCP running on this interface. */
        strcpy(ifreq.ifr_name, ift->ifname);
        ifreq.ifr_ifru.ifru_opt = 0;

        if (ipcom_socketioctl(ipdhcpc.fd, IP_SIOCXSDHCPRUNNING, &ifreq) < 0)
        {
            IPCOM_LOG2(ERR, "ipdhcpc[%s] :: ipcom_ioctl(IP_SIOCXSDHCPRUNNING) failed, errno =%d",
                       ift->ifname, ipcom_errno);
            continue;

        }
    }
}

/*
 *===========================================================================
 *                    ipdhcpc_if_has_addr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipdhcpc_route_init_addrs(void *msg, Ip_u32 addrs, struct Ip_sockaddr **sas)
{
    int       i;
    Ip_size_t offset = 0;

    for (i = 0; i < IPNET_RTAX_MAX; i++)
    {
        if (IP_BIT_ISSET(addrs, 1 << i))
        {
            sas[i] = (struct Ip_sockaddr *) &((Ip_u8 *) msg)[offset];
            offset += IPCOM_SA_LEN_GET(sas[i]);
        }
        else
            sas[i] = IP_NULL;
    }
}

/*
 *===========================================================================
 *                    ipdhcpc_if_has_addr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipdhcpc_if_has_addr(Ip_u32 ifindex, Ip_u32 ipaddr_n)
{
    int                      name[6];
    int                      ret = IP_FALSE;
    Ip_u8                   *if_data = IP_NULL;
    struct Ipnet_ifa_msghdr *ifa;
    struct Ipnet_if_msghdr  *ifm;
    Ip_size_t                if_data_len;
    Ip_size_t                if_data_offset;
    Ip_size_t                offset;
    struct Ip_sockaddr_in   *addrs[IPNET_RTAX_MAX];

    name[0] = IP_CTL_NET;
    name[1] = IP_AF_ROUTE;
    name[2] = 0;
    name[3] = IP_AF_INET;
    name[4] = IP_NET_RT_IFLIST;
    name[5] = ifindex;

    /* Get length of data */
	
    if (ipcom_sysctl(name, 6, IP_NULL, &if_data_len, IP_NULL, 0) < 0)
    {
        IPCOM_LOG1(ERR, "ipdhcpc_if_has_addr: ipcom_sysctl() failed: %s",
                   ipcom_strerror(ipcom_errno));
        goto cleanup;
    }

    /* Allocate space for data */
	
    if_data = ipcom_malloc(if_data_len);
    if (if_data == IP_NULL)
    {
        IPCOM_LOG0(ERR, "ipdhcpc_if_has_addr: out of memory");
        goto cleanup;
    }

    /* Get the actual data */

    if (ipcom_sysctl(name, 6, if_data, &if_data_len, IP_NULL, 0) < 0)
    { 
       IPCOM_LOG1(ERR, "ipdhcpc_if_has_addr: ipcom_sysctl() failed: %s",
                  ipcom_strerror(ipcom_errno));
       ret = -1;
       goto cleanup;
    }

    ifm = (struct Ipnet_if_msghdr *) &if_data[0];
    if_data_offset = ifm->ifm_msglen;

    offset = 0;
    while (offset < if_data_len - if_data_offset)
    {
        ifa = (struct Ipnet_ifa_msghdr *) &if_data[if_data_offset + offset];

        ipdhcpc_route_init_addrs(ifa + 1, ifa->ifam_addrs, (struct Ip_sockaddr **) addrs);

        /* Only AF_INET addrs */
        
        if (ifa->ifam_type != IPNET_RTM_NEWADDR || addrs[IPNET_RTAX_IFA]->sin_family != IP_AF_INET)
            break;
		
        if (addrs[IPNET_RTAX_IFA]->sin_addr.s_addr == ipaddr_n)
        {
            /* Found the requested address */
        	
            ret = IP_TRUE;
        }
        offset += ifa->ifam_msglen;
    }

cleanup:
    ipcom_free(if_data);	

    return(ret);
}

/*
****************************************************************************
* 10                   GLOBAL FUNCTIONS
****************************************************************************
*/

/*
 *===========================================================================
 *                    ipdhcpc_ifopen
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipdhcpc_ifopen(Ip_u32 ifindex, Ipdhcpc_ifopen * config)
{
    Ip_err                 retval;
    Ipdhcpc_if            *ift = IP_NULL;
    char                   buf[50];
    struct Ip_ifreq        ifreq;
    struct Ip_sockaddr_in *addrp;
    int                    remove_addrs;

    IPDHCPC_LOCK();

    if (ipdhcpc.opened == IP_FALSE)
        IPDHCPC_LEAVE(IPCOM_ERR_NOT_OPENED);
    if (config->api_version > 1)
        IPDHCPC_LEAVE(IPCOM_ERR_INVALID_VERSION);
    if (config->conf[0] != IP_TAG_END)
        IPDHCPC_LEAVE(IPCOM_ERR_UNSUPPORTED_TAG);

    for (ift = ipdhcpc.ift_head; ift != IP_NULL; ift = ift->next)
        if (ift->ifindex == ifindex)
        {
            ift->num_opened++;
            ift = IP_NULL;	/* trick to not release the interface. */
            IPDHCPC_LEAVE(IPCOM_SUCCESS);
        }

    ift = ipcom_malloc(sizeof(Ipdhcpc_if));
    if (ift == IP_NULL)
        IPDHCPC_LEAVE(IPCOM_ERR_NO_MEMORY);

    /* Init interface structure. */
    ipcom_memset(ift, 0, sizeof(Ipdhcpc_if));
    ift->ifindex = ifindex;
    (void)ipcom_if_indextoname(ift->ifindex, ift->ifname);

    /* Check if we have a client identifier */
    ipcom_snprintf(buf,sizeof(buf), "ipdhcpc.%s.option.client_identifier", ift->ifname);
    if ((ift->clientid = ipcom_sysvar_get(buf,IP_NULL,IP_NULL)) == IP_NULL)
        ift->clientid = ipcom_sysvar_get("ipdhcpc.option.client_identifier", IP_NULL, IP_NULL);

    /* Make sure that IPv4 address auto configuration is disabled on this
     * interface.
     */
    {
        const char* str_ac = "inet.AutoConf";
        char *name;
        int val;
        const Ipcom_cmd_int_str_map map[] = {
            { IP_TRUE , "yes"      },
            { IP_FALSE, "no"       },
            { IP_TRUE , "enable"   },
            { IP_TRUE , "enabled"  },
            { IP_FALSE, "disable"  },
            { IP_FALSE, "disabled" },
            { IP_TRUE , "on"       },
            { IP_FALSE, "off"      },
            { IP_FALSE, "false"    },
            { IP_FALSE, "true"     },
            { IP_NULL,  0          }
        };

        name = ipcom_malloc(ipcom_strlen(str_ac) + IP_IFNAMSIZ + 1);
        if (name == IP_NULL)
            IPDHCPC_LEAVE(IPCOM_ERR_NO_MEMORY);

        /* Check interface specific autoconf setting */
        ipcom_sprintf(name, "%s.%s", ift->ifname, str_ac);
        if (ipcom_sysvar_get_descriptive_int0(name, 0, &val, map) < 0)
        {
            /* Check global autoconf setting */
            ipcom_sprintf(name, "%s.%s", "ipnet", str_ac);
            if (ipcom_sysvar_get_descriptive_int0(name, 0, &val, map) < 0)
                goto done;
        }

        if (val == 1)
        {
            /* AutoConf is enabled. Turn it off */
            IPCOM_LOG1(INFO, "ipdhcpc[%s] :: Disabling IPv4 address auto configuration", ift->ifname);

            ipcom_sprintf(name, "%s.%s", ift->ifname, str_ac);
            ipcom_sysvar_set(name,"no",IPCOM_SYSVAR_FLAG_OVERWRITE);

            /* Turn interface down/up to make the setting take effect */
            ipcom_memset(&ifreq, 0, sizeof(ifreq));
            ipcom_strcpy(ifreq.ifr_name, ift->ifname);
            if (ipcom_socketioctl(ipdhcpc.fd, IP_SIOCGIFFLAGS, &ifreq) < 0)
            {
                IP_PANIC2();
                IPCOM_LOG2(ERR, "ipdhcpc[%s] :: ipdhcpc_ifopen(): ipcom_ioctl(IP_SIOCGIFFLAGS) failed, errno = %d",
                           ifreq.ifr_name, ipcom_errno);
                IPDHCPC_LEAVE(IPCOM_ERR_FAILED);
            }

            IP_BIT_CLR(ifreq.ip_ifr_flags,IP_IFF_UP);
            if (ipcom_socketioctl(ipdhcpc.fd, IP_SIOCSIFFLAGS, &ifreq) < 0)
            {
                IP_PANIC2();
                IPCOM_LOG2(ERR, "ipdhcpc[%s] :: ipdhcpc_ifopen(): ipcom_ioctl(IP_SIOCSIFFLAGS,down) failed, errno = %d",
                           ifreq.ifr_name, ipcom_errno);
                IPDHCPC_LEAVE(IPCOM_ERR_FAILED);
            }

            IP_BIT_SET(ifreq.ip_ifr_flags,IP_IFF_UP);
            if (ipcom_socketioctl(ipdhcpc.fd, IP_SIOCSIFFLAGS, &ifreq) < 0)
            {
                IP_PANIC2();
                IPCOM_LOG2(ERR, "ipdhcpc[%s] :: ipdhcpc_ifopen(): ipcom_ioctl(IP_SIOCSIFFLAGS,up) failed, errno = %d",
                           ifreq.ifr_name, ipcom_errno);
                IPDHCPC_LEAVE(IPCOM_ERR_FAILED);
            }
        }
    done:
        ipcom_free(name);
    }

    remove_addrs = ipcom_sysvar_get_as_int("ipdhcpc.remove_existing_addresses",
                                           IPDHCPC_REMOVE_EXISTING_ADDRESSES);
    
    if (remove_addrs > 0)
    {
    	/* Remove all unicast address on interface. */
    
    	for (;;)
    	{
            ipcom_strcpy(ifreq.ifr_name, ift->ifname);
            addrp             = (struct Ip_sockaddr_in *)&ifreq.ip_ifr_addr;
            addrp->sin_family = IP_AF_INET;
            IPCOM_SA_LEN_SET(addrp, sizeof(struct Ip_sockaddr_in));
            if (ipcom_socketioctl(ipdhcpc.fd, IP_SIOCGIFADDR, &ifreq) < 0)
            {
                if (ipcom_errno == IP_ERRNO_EADDRNOTAVAIL)
                    break;
                IP_PANIC2();
                IPCOM_LOG2(ERR, 
                           "ipdhcpc[%s] :: ipdhcpc_ifopen(): ipcom_ioctl(IP_SIOCGIFADDR) failed, errno = %d",
                           ifreq.ifr_name, 
                           ipcom_errno);
                IPDHCPC_LEAVE(IPCOM_ERR_FAILED);
            }

            if (addrp->sin_addr.s_addr == 0)
                break;

        
            ip_assert(addrp->sin_family == IP_AF_INET);
#ifdef IPCOM_USE_SA_LEN
            ip_assert(addrp->sin_len == sizeof(struct Ip_sockaddr_in));
#endif
            IPCOM_LOG2(DEBUG2, "ipdhcpc[%s] :: removed address = %s",
                       ift->ifname,
                       ipcom_inet_ntop(IP_AF_INET, 
                                       &addrp->sin_addr.s_addr, 
                                       buf, 
                                       sizeof(buf)));
    		
            if (ipcom_socketioctl(ipdhcpc.fd, IP_SIOCDIFADDR, &ifreq) < 0)
            { 
                IP_PANIC2();
                IPCOM_LOG2(ERR, "ipdhcpc[%s] :: ipdhcpc_ifopen(): ipcom_ioctl(IP_SIOCDIFADDR) failed, errno = %d",
                           ifreq.ifr_name, 
                           ipcom_errno);
                IPDHCPC_LEAVE(IPCOM_ERR_FAILED);
            }
        } 
    }

    /* Get interface hardware address and length. */
    (void)ipcom_if_indextoname(ift->ifindex, ifreq.ifr_name);
    if (ipcom_socketioctl(ipdhcpc.fd, IP_SIOCGIFLLADDR, &ifreq) < 0)
    {
        IPCOM_LOG2(ERR, "ipdhcpc[%s] :: ipdhcpc_ifopen(): ipcom_ioctl(IP_SIOCGIFLLADDR) failed, errno = %d",
                   ifreq.ifr_name, ipcom_errno);
        IPDHCPC_LEAVE(IPCOM_ERR_FAILED);
    }

#ifdef IPCOM_USE_SA_LEN
    ift->hlen = IP_MIN(sizeof(ift->chaddr), ifreq.ip_ifr_addr.sa_len);
#else
    ift->hlen = 6;
#endif
    
    if (ift->hlen > sizeof(ifreq.ip_ifr_addr.sa_data))
    	IPDHCPC_LEAVE(IPCOM_ERR_FAILED);
    	
    ipcom_memcpy(ift->chaddr, ifreq.ip_ifr_addr.sa_data, ift->hlen);

    /* Insert first into interface list. */
    ift->next = ipdhcpc.ift_head;
    ipdhcpc.ift_head = ift;

    /* Increment interface counter */
    ipdhcpc.open_interfaces++;

    IPCOM_LOG1(DEBUG2, "ipdhcpc[%s] :: DHCP opened", ift->ifname);

    /* Success. */
    retval = IPCOM_SUCCESS;
    ift = IP_NULL;		/* trick to not release the interface. */

 leave:
    if (ift != IP_NULL)
    {
        if (ift->clientid != IP_NULL)
            ipcom_free(ift->clientid);
        ipcom_free(ift);
    }
    IPDHCPC_UNLOCK();
    return retval;
}


/*
 *===========================================================================
 *                    ipdhcpc_ifclose
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipdhcpc_ifclose(Ip_u32 ifindex)
{
    Ip_err          retval;
    Ipdhcpc_if    **iftp;
    Ipdhcpc_if     *ift;
    Ipdhcpc_handle *handle;

    if (ipdhcpc.opened == IP_FALSE)
        IPDHCPC_LEAVE(IPCOM_ERR_NOT_OPENED);

    for (ift = IP_NULL, iftp = &ipdhcpc.ift_head;(*iftp) != IP_NULL;
         iftp = &(*iftp)->next)
        if ((*iftp)->ifindex == ifindex)
        {
            ift = *iftp;
            break;
        }

    if (ift == IP_NULL)
        IPDHCPC_LEAVE(IPCOM_ERR_INVALID_INTERFACE);

    if (ift->num_opened > 0)
    {
        ift->num_opened--;
        IPDHCPC_LEAVE(IPCOM_SUCCESS);
    }

    IPCOM_LOG1(DEBUG2, "ipdhcpc[%s] :: DHCP closed", ift->ifname);

    /* Remove lease if any. */
    retval = ipdhcpc_ifunlease_locked(ift, &handle);
    if (retval == IPCOM_SUCCESS)
        (void)ipdhcpc_handle_free(ifindex, handle);

    /* Remove Client ID entry */
    if (ift->clientid != IP_NULL)
        ipcom_free(ift->clientid);

    /* Remove entry from list and free it. */
    *iftp = ift->next;
    ipcom_free(ift);

    /* Decrement interface counter */
    ipdhcpc.open_interfaces--;

    retval = IPCOM_SUCCESS;

 leave:
    return retval;
}

/*
 *===========================================================================
 *                    ipdhcpc_if_isopen
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_bool
ipdhcpc_if_isopen(Ip_u32 ifindex)
{
    Ipdhcpc_if *ift;
    Ip_bool retval;

    IPDHCPC_LOCK();

    if (ipdhcpc.opened == IP_FALSE)
        IPDHCPC_LEAVE(IP_FALSE);

    for (ift = ipdhcpc.ift_head; ift != IP_NULL; ift = ift->next)
        if (ift->ifindex == ifindex)
            break;

    if (ift == IP_NULL)
        IPDHCPC_LEAVE(IP_FALSE);

    retval = IP_TRUE;

 leave:
    IPDHCPC_UNLOCK();
    return retval;
}


/*
 *===========================================================================
 *                    ipdhcpc_handle_malloc
 *===========================================================================
 * Description:   Mallocate and initialize a DHCP client request packet for
 *                use with ipdhcpc_ifrequest().
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipdhcpc_handle_malloc(Ip_u32 ifindex, int mode, Ipdhcpc_handle **handlep)
{
    Ipdhcpc_if     *ift;
    Ipdhcpc_handle *handle;
    Ip_err          retval = IPCOM_ERR_FAILED;
    char            ifname[IP_IFNAMSIZ];
    char           *opt;
    char           *opt_ptr;
    char           *next;
    char           *last;
    char            buf[50];
    Ip_u32          opt_len;
    Ip_u16          port;
    Ip_bool         has_options;
    Ip_u32          bootp_mode;
    char            opt_buf[IPDHCPC_MAX_OPTION_SIZE];  
    Ip_u32          ipaddress_lease_time = 0;    
    Ip_u32          htonl_tmp = 0;    
    
    ip_assert(handlep != IP_NULL);

    if (mode < IPDHCPC_MODE_ALLOC_DYNAMIC ||
        mode > IPDHCPC_MODE_INFORM_STATIC)
        return IPCOM_ERR_INVALID_ARG;
    
    IPDHCPC_LOCK();

    has_options = IP_FALSE;
    
    /* Look up the interface. */
    for (ift = ipdhcpc.ift_head; ift != IP_NULL; ift = ift->next)
        if (ift->ifindex == ifindex)
            break;
    if (ift == IP_NULL)
        IPDHCPC_LEAVE(IPCOM_ERR_INVALID_INTERFACE);

    /* Mallocate DHCP handle.*/
    handle = ipcom_malloc(sizeof(Ipdhcpc_handle) +
                          sizeof(Ipdhcpc_handle_priv));
    if (handle == IP_NULL)
        IPDHCPC_LEAVE(IPCOM_ERR_NO_MEMORY);

    /* Initialize DHCP handle. */
    ipcom_memset(handle, 0, sizeof(Ipdhcpc_handle) + sizeof(Ipdhcpc_handle_priv));
    
    /* Mallocate space for info options */
    
    handle->info.options = ipcom_malloc((ipdhcpc.max_message_size - IPDHCPC_NO_OPTIONS_SIZE));
    if (handle->info.options == IP_NULL)
    {
    	ipcom_free(handle);
        IPDHCPC_LEAVE(IPCOM_ERR_NO_MEMORY);
    }
    ipcom_memset(handle->info.options, 0, (ipdhcpc.max_message_size - IPDHCPC_NO_OPTIONS_SIZE));
    
    /* Initialize DHCP handle. */

    handle->priv = (Ipdhcpc_handle_priv *)((Ip_u8 *) handle +
                                           sizeof(Ipdhcpc_handle));
    handle->ifindex = ifindex;
    handle->serveraddr_n = IP_INADDR_BROADCAST;
    port = (Ip_u16)ipcom_sysvar_get_as_int("ipdhcpc.server_port", IPDHCPC_SERVER_PORT);
    handle->serverport_n = ip_htons(port);
    handle->magic_cookie = IPDHCPC_MAGIC_COOKIE;

    IPCOM_TMO_INIT(&handle->priv->tmo);
    IPCOM_TMO_INIT(&handle->priv->tmo_t2);
    handle->priv->mode = mode;
    
    /* Fill in valid BOOTP/DHCP options */
       
    opt = ipcom_sysvar_get("ipdhcpc.option.hostname",IP_NULL,IP_NULL);     
    opt_len = opt? ipcom_strlen(opt) : 0;
       
    if (opt_len > 0)
    {
        if (opt_len < IPDHCPC_MAX_OPTION_SIZE)
            ipcom_strncpy((char *)opt_buf, opt, opt_len);
           
        retval = ipdhcpc_option_add(handle, 
                                    IPDHCPC_OPTCODE_HOST_NAME, 
                                    (Ip_u8 *)opt_buf, 
                                    opt_len);

        if (retval == IPCOM_SUCCESS)
            has_options = IP_TRUE;
           
        ipcom_free(opt);
        opt = IP_NULL;
        ipcom_memset(opt_buf, 0, IPDHCPC_MAX_OPTION_SIZE);
    }
       
    opt = ipcom_sysvar_get("ipdhcpc.option.netmask",IP_NULL,IP_NULL);
    opt_len = opt? ipcom_strlen(opt) : 0;   
    
    if (opt_len > 0)
    {
        opt_len = 4;
           
        if (ipcom_inet_pton(IP_AF_INET, opt, opt_buf) != 1)
        {
        	ipcom_free(opt);
            IPDHCPC_LEAVE(IPCOM_ERR_FAILED);
        }
           
        retval = ipdhcpc_option_add(handle, 
                                    IPDHCPC_OPTCODE_SUBNET_MASK, 
                                    (Ip_u8 *)opt_buf, 
                                    opt_len);

        if (retval == IPCOM_SUCCESS)
            has_options = IP_TRUE;
           
        ipcom_free(opt);
        opt = IP_NULL;
        ipcom_memset(opt_buf, 0, IPDHCPC_MAX_OPTION_SIZE);
    }   
    
    /* Fill in DHCP only options */
    
    bootp_mode = ipcom_sysvar_get_as_int("ipdhcpc.bootp_mode",IPDHCPC_BOOTP_MODE);

    if (!bootp_mode)
    {
        (void)ipcom_if_indextoname(ifindex,ifname);
        ipcom_snprintf(buf, sizeof(buf), "ipdhcpc.%s.option.requested_options", ifname);
        
        if ((opt = ipcom_sysvar_get(buf,IP_NULL,IP_NULL)) == IP_NULL)
             opt = ipcom_sysvar_get("ipdhcpc.option.requested_options",IP_NULL,IP_NULL);
        
        opt_len = opt? ipcom_strlen(opt) : 0;
        
        if (opt_len > 0)
        {   
        	opt_ptr = opt;
        	
            for (opt_len = 0; opt != IP_NULL && *opt != 0; opt = next, opt_len++)
            {
                next = ipcom_strchr(opt, ',');
                if (next != IP_NULL)
                    *next++ = 0;
                
                opt_buf[opt_len] = (Ip_u8)ipcom_atoi(opt);
            }
            
            retval = ipdhcpc_option_add(handle, 
                                        IPDHCPC_OPTCODE_DHCP_PARAMETER_REQUEST_LIST, 
                                        (Ip_u8 *)opt_buf, 
                                        opt_len);
            
            if (retval == IPCOM_SUCCESS)
            	has_options = IP_TRUE;

            ipcom_free(opt_ptr);
            opt = IP_NULL;
            ipcom_memset(opt_buf, 0, IPDHCPC_MAX_OPTION_SIZE);
        }
        
        opt = ipcom_sysvar_get("ipdhcpc.option.vendor_class_id",IP_NULL,IP_NULL);
        opt_len = opt? ipcom_strlen(opt) : 0;
        
        if (opt_len > 0)
        {                          
            if (opt_len < IPDHCPC_MAX_OPTION_SIZE)
            {
                ipcom_strncpy((char *)opt_buf, opt, opt_len);
            	
                retval = ipdhcpc_option_add(handle, 
                                            IPDHCPC_OPTCODE_VENDOR_CLASS_IDENTIFIER, 
                                            (Ip_u8 *)opt_buf, 
                                            opt_len);

                if (retval == IPCOM_SUCCESS)
            	    has_options = IP_TRUE;
            }
            
            ipcom_free(opt);
            opt = IP_NULL;
            ipcom_memset(opt_buf, 0, IPDHCPC_MAX_OPTION_SIZE);
        }
    
        /* fill in vendor identifying vendor class option */

        opt = ipcom_sysvar_get("ipdhcpc.option.vendor_identifying_vendor_class",IP_NULL,IP_NULL);
        opt_len = opt? ipcom_strlen(opt) : 0;
        
        if (opt_len > 0)
        {  	
            Ip_u8          *vivc_options = (Ip_u8 *)opt_buf;
            char           *vivc_opt     = opt;
            char           *vivc_data;
            Ip_u32          vivc_data_len;
            Ip_u32          vivc_enterprise_number;
            char           *vivc_enterprise_string;
            Ip_u32          vivc_enterprise_string_len;
  	
            /*
             * Expected format: <enterprise number>=<data1>;<enterprise number>=<data2>;...
             */
        
            for (opt_len = 0; opt != IP_NULL && *opt != 0; opt = next)
            {
                /* get enterprise number */
          
                vivc_enterprise_string = ipcom_strtok_r(opt, "=;", &last);
                if(vivc_enterprise_string == IP_NULL)
                    break;
            
                vivc_enterprise_string_len = ipcom_strlen(vivc_enterprise_string);
                if (vivc_enterprise_string_len == 0)
                    break;
                    
                vivc_enterprise_number = ip_htonl(ipcom_atoi(vivc_enterprise_string)); 
            
                /* get option data */
            
                vivc_data = ipcom_strtok_r(IP_NULL, "=;", &last);          
                if (vivc_data == IP_NULL)
                    break;
     
                vivc_data_len = ipcom_strlen(vivc_data);
                if (vivc_data_len == 0)
                    break;
            
                /* fill in enterprise number */               
                    
                ipcom_memcpy(vivc_options, &vivc_enterprise_number, sizeof(Ip_u32));
                vivc_options += sizeof(Ip_u32);
                                   
                /* fill in data length */
                    
                ipcom_memcpy(vivc_options, &vivc_data_len, sizeof(Ip_u8)); 
                vivc_options += sizeof(Ip_u8);
            
                /* fill in vendor specific data */
                    
                ipcom_strncpy((char *)vivc_options, vivc_data, vivc_data_len);
                vivc_options += vivc_data_len; 
                   
                /* bump total option length (enterprise # + data length field + data) */
                    
                opt_len += sizeof(Ip_u32) + sizeof(Ip_u8) + vivc_data_len;
                    
                /* next option string (current + data + enterprise # + delimeters) */
            
                next = opt + vivc_data_len + vivc_enterprise_string_len + 2;
            }

            if (opt_len < IPDHCPC_MAX_OPTION_SIZE)
            {
                retval = ipdhcpc_option_add(handle, 
                                            IPDHCPC_OPTCODE_VI_VENDOR_CLASS, 
                                            (Ip_u8 *)opt_buf, 
                                            opt_len);

                if (retval == IPCOM_SUCCESS)
                    has_options = IP_TRUE;
            }      
            
            ipcom_free(vivc_opt);
            opt = IP_NULL;
            ipcom_memset(opt_buf, 0, IPDHCPC_MAX_OPTION_SIZE);
        }
        
        /* Fill in any user specified options.  Parse this sysvar just before adding
         * the END option so that in the case of duplicate options, the fully supported 
         * options specified above take precidence.
         */

        opt = ipcom_sysvar_get("ipdhcpc.option.user_specified",IP_NULL,IP_NULL);
        opt_len = opt? ipcom_strlen(opt) : 0;
        
        if (opt_len > 0)
        {
            Ip_u8           opt_data_len;
        	char           *opt_data_string;
        	Ip_u32          opt_data_string_len;
        	Ip_u8           opt_code;
        	char           *opt_code_string;
        	Ip_u32          opt_code_string_len;
        	char           *opt_ptr;
        	int             i;
        	unsigned int    k;
        	
        	opt_ptr = opt;
        	opt_len = ipcom_strlen(opt);
        	        	         		
        	/*
        	 * Expected format: <option code1>=<option data1>;<option code2>=<option data2>;...
        	 */
        		
        	for (next = IP_NULL; opt != IP_NULL && *opt != 0; opt = next)  	    	
        	{
        	    /* get option code */
        	          
        	    opt_code_string = ipcom_strtok_r(opt, "=;", &last);
        	    if(opt_code_string == IP_NULL)
        	        break;
        	        
                opt_code_string_len = ipcom_strlen(opt_code_string);
                if (opt_code_string_len <= 0)
                    break;
                    
        	    opt_code = (Ip_u8)ipcom_atoi(opt_code_string);
        	        
        	    /* get option data */
        	            
        	    opt_data_string = ipcom_strtok_r(IP_NULL, "=;", &last);          
        	    if (opt_data_string == IP_NULL)
        	        break;
        	        
        	    opt_data_string_len = ipcom_strlen(opt_data_string);         
        	    if (opt_data_string_len <= 0)
        	        break;  
        	        
        	    /* get option length */
        	        
        	    opt_data_len = (Ip_u8)(opt_data_string_len / 2);
        	            
        	    /* copy over option data */
      	        	
        	    for (i = 0; ipcom_sscanf(opt_data_string + 2*i, "%02x", &k); i++)
        	        opt_buf[i] = (Ip_u8)k;
        	              	        
            	retval = ipdhcpc_option_add(handle, 
            	                            opt_code, 
            	                            (Ip_u8 *)opt_buf, 
            	                            opt_data_len);
            	    
            	if (retval == IPCOM_SUCCESS)
            	    has_options = IP_TRUE;
            	else
            	    break;
     	        
                ipcom_memset(opt_buf, 0, IPDHCPC_MAX_OPTION_SIZE);
        	        
        	    /* next option string (current + option code length + option data length +
        	     * delimeters) 
        	     */
        	            
        	    next = opt + opt_code_string_len + opt_data_string_len + 2; 
            		
                if ((next - opt_ptr) >= opt_len)
            	    break;
        	}   
            
        	
            ipcom_free(opt_ptr);
            opt = IP_NULL;
            
        } /* End of user specified options */
        
        ipaddress_lease_time = (Ip_u32) ipcom_sysvar_get_as_int("ipdhcpc.option.lease_time", 0);       
        if (ipaddress_lease_time != 0)
        {
            opt_len = 4;
            htonl_tmp = ip_htonl(ipaddress_lease_time);
            retval = ipdhcpc_option_add(handle, 
                                        IPDHCPC_OPTCODE_DHCP_LEASE_TIME, 
                                        (Ip_u8 *) &htonl_tmp, 
                                         opt_len);

           if (retval == IPCOM_SUCCESS)
              has_options = IP_TRUE;
        } 
    } /* End of DHCP only options */
       
    /* fill in end option */
    
    if (has_options == IP_TRUE)
    {
        retval = ipdhcpc_option_add(handle, 
                                    IPDHCPC_OPTCODE_END, 
                                    IP_NULL, 
                                    0);  	
    }

    /* fill in filename / hostname */
    
    opt = ipcom_sysvar_get("ipdhcpc.boot_filename", IP_NULL, IP_NULL); 	
    opt_len = opt? ipcom_strlen(opt) : 0;
    
    if (opt_len > 0)
    {
    	if (opt_len < sizeof(handle->file))
    	    ipcom_strncpy((char *)handle->file, opt, opt_len);
    	
    	ipcom_free(opt);
    }
    
    opt = ipcom_sysvar_get("ipdhcpc.server_hostname", IP_NULL, IP_NULL);
    opt_len = opt? ipcom_strlen(opt) : 0;
    
    if (opt_len > 0)
    {
    	if (opt_len < sizeof(handle->sname))
    	    ipcom_strncpy((char *)handle->sname, opt, opt_len);
    	
    	ipcom_free(opt);
    }  
    
    switch(mode)
    {
    case IPDHCPC_MODE_ALLOC_DYNAMIC:
        handle->priv->state = IPDHCPC_STATE_INIT;
        break;
    case IPDHCPC_MODE_REUSE_DYNAMIC:
        break;
    case IPDHCPC_MODE_RELEASE_DYNAMIC:
        break;
    case IPDHCPC_MODE_INFORM_STATIC:
        break;
    }

    /* Insert first into DHCP handle list. */
    handle->priv->next = ipdhcpc.handle_head;
    ipdhcpc.handle_head = handle;

    /* Success. */
    *handlep = handle;
    retval = IPCOM_SUCCESS;
    
 leave:
    IPDHCPC_UNLOCK();
    return retval;
}


/*
 *===========================================================================
 *                    ipdhcpc_handle_free
 *===========================================================================
 * Description:   Free a DHCP client handle.
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipdhcpc_handle_free(Ip_u32 ifindex, Ipdhcpc_handle *handle)
{
    Ipdhcpc_handle  **handlep;
    Ipdhcpc_if      *ift;

    (void)ifindex;

    for (ift = ipdhcpc.ift_head; ift != IP_NULL; ift = ift->next)
        if (ift->handle == handle)
            ift->handle = IP_NULL;

    ip_assert(handle != IP_NULL);

    IPDHCPC_LOCK();

    /* Remove entry from list and free it. */
    for (handlep = &ipdhcpc.handle_head;(*handlep) != IP_NULL;
         handlep = &(*handlep)->priv->next)
        if ((*handlep) == handle)
        {
            *handlep = handle->priv->next;
            break;
        }

    /* Free user option buffer */
    if (handle->info.options != IP_NULL)
    	ipcom_free(handle->info.options);
    
    /* Free handle */
    ipcom_free(handle);

    IPDHCPC_UNLOCK();

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipdhcpc_handle_free_pending
 *===========================================================================
 * Description:   Free all DHCP client handles.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipdhcpc_handle_free_pending(void)
{
    Ipdhcpc_handle *handle;
    Ipdhcpc_handle *handle_next;

    IPDHCPC_LOCK();

    /* Remove entry from list and free it. */
    for (handle = ipdhcpc.handle_head; handle != IP_NULL; handle = handle_next)
    {
        handle_next = handle->priv->next;

        /* Cancel timers */
        ipdhcpc_timer_cancel(handle, 0);

        /* Free handle */
        ipcom_free(handle);
    }

    IPDHCPC_UNLOCK();
}


/*
 *===========================================================================
 *                    ipdhcpc_ifrequest
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipdhcpc_ifrequest(Ip_u32 ifindex, Ipdhcpc_handle *handle)
{
    Ip_err retval;
    Ipdhcpc_if *ift;
    Ip_time_t time_diff;

    ip_assert(handle != IP_NULL);

    IPDHCPC_LOCK();

    for (ift = ipdhcpc.ift_head; ift != IP_NULL; ift = ift->next)
        if (ift->ifindex == ifindex)
            break;

    if (ift == IP_NULL)
        IPDHCPC_LEAVE(IPCOM_ERR_INVALID_INTERFACE);
    else
        time_diff = ipcom_time(0) - ift->time_addr_req;

    if (ift->handle != IP_NULL)
        IPDHCPC_LEAVE(IPCOM_ERR_ALREADY_STARTED);

    /* Save handle in interface structure.*/
    ift->handle = handle;

    IPDHCPC_UNLOCK();

    IPCOM_LOG2(DEBUG2, "ipdhcpc[%s] :: Initiating DHCP request, mode=%d",
               ift->ifname,
               handle->priv->mode);

    if (handle->priv->state <= IPDHCPC_STATE_INIT_REBOOT && time_diff < 10)
    {
        ipdhcpc_state_restart(handle);
    }
    else
    {
        /* Output the request immediately by triggering instant timeout. */
        (void)ipdhcpc_tmo_cb(&handle->priv->tmo, handle);
    }

    /* Non blocking mode, reply instant success. */
    if (handle->reply_cb != IP_NULL)
        return IPCOM_SUCCESS;

    IPDHCPC_LOCK();

    retval = IPCOM_SUCCESS;

 leave:
    IPDHCPC_UNLOCK();
    return retval;
}


/*
 *===========================================================================
 *                    ipdhcpc_iflease
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipdhcpc_iflease(Ip_u32 ifindex, Ipdhcpc_handle *handle)
{
    Ip_err                 retval;
    Ipdhcpc_if            *ift;
    Ipdhcpc_handle       **handlep;
    Ip_u32                 addr_n;
    Ip_u8                  end = IPDHCPC_OPTCODE_END;
    Ip_u8                 *option;
    Ip_u8                  option_overload = 0;
    char                  *buf = IP_NULL;
    Ip_bool                this_is_primary = IP_TRUE;
    struct Ip_ifreq        ifreq;
    struct Ip_sockaddr_in *addrp;
    Ip_u32                 gw_n;
    char                   addr1[16];
    int                    bootp_mode;
    
    ip_assert(handle != IP_NULL);

    IPDHCPC_LOCK();

    if (ipdhcpc.opened == IP_FALSE)
        IPDHCPC_LEAVE(IPCOM_ERR_NOT_OPENED);

    buf = ipcom_malloc(255);
    if (buf == IP_NULL)
        IPDHCPC_LEAVE(IPCOM_ERR_NO_MEMORY);

    if (handle->priv->state != IPDHCPC_STATE_BOUND)
        IPDHCPC_LEAVE(IPCOM_ERR_FAILED);

    for (ift = ipdhcpc.ift_head; ift != IP_NULL; ift = ift->next)
        if (ift->ifindex == ifindex)
            break;
    if (ift == IP_NULL)
        IPDHCPC_LEAVE(IPCOM_ERR_INVALID_INTERFACE);

    IPCOM_LOG1(DEBUG2, "ipdhcpc[%s] :: Starting lease", ift->ifname);

    if (ipdhcpc.open_interfaces > 1)
    {
        Ipdhcpc_if *ift2;

        this_is_primary = IP_TRUE;
        for (ift2 = ipdhcpc.ift_head; ift2 != IP_NULL; ift2 = ift2->next)
            if (ift2->ifindex < ift->ifindex)
            {
                this_is_primary = IP_FALSE;
                break;
            }
    }

    /* Remove entry from pending handle list. */
    for (handlep = &ipdhcpc.handle_head;(*handlep) != IP_NULL;
         handlep = &(*handlep)->priv->next)
        if ((*handlep) == handle)
            break;
    if ((*handlep) != handle)
        IPDHCPC_LEAVE(IPCOM_ERR_INVALID_ARG);
    *handlep = handle->priv->next;


    bootp_mode = ipcom_sysvar_get_as_int("ipdhcpc.bootp_mode",IPDHCPC_BOOTP_MODE);

    /* If we're in bootp mode, set IPv4 address on this interface.*/
    if (bootp_mode) 
    { 
        struct Ip_in_aliasreq   ifalias;

        ipcom_memset(&ifalias, 0, sizeof(ifalias));

        (void)ipcom_if_indextoname(ifindex, ifalias.ifra_name);
        addrp             = &ifalias.ifra_addr;
        addrp->sin_family = IP_AF_INET;
        addrp->sin_len    = sizeof(struct Ip_sockaddr_in);
        addrp->sin_addr.s_addr = handle->info.ipaddr_n;
        if (ipcom_socketioctl(ipdhcpc.fd, IP_SIOCAIFADDR, &ifalias) < 0)
        {
            IPCOM_LOG2(ERR, "ipdhcpc[%s] :: ipdhcpc_iflease(): ipcom_ioctl(IP_SIOCSIFADDR) failed, errno = %d",
                   ifalias.ifra_name, ipcom_errno);
            IPDHCPC_LEAVE(IPCOM_ERR_FAILED);
        }
    }

    /* Reset options to default values */
    ift->lease.t1 = 0;
    ift->lease.t2 = 0;
    ift->lease.tlease = (Ip_time_t)0xffffffff;

    /* Notify the user about the boot server address in dhcp header */
    if (handle->info.bootaddr_n)
    {
        Ip_u8 *addr_buf;

        addr_buf = ipcom_malloc(sizeof(Ip_u32) + 2 * sizeof(Ip_u8));
        if (addr_buf == IP_NULL)
            IPDHCPC_LEAVE(IPCOM_ERR_NO_MEMORY);

        addr_buf[0] = IPDHCPC_OPTCODE_TFTP_SERVER_ADDRESS;
        addr_buf[1] = sizeof(Ip_u32);
        ipcom_memcpy(&addr_buf[2],&handle->info.bootaddr_n, sizeof(Ip_u32));

        (void)ipdhcpc_option_callback(handle, addr_buf);
        ipcom_free(addr_buf);
    }


    /* Act on the DHCP options we got from the server.*/
    for (option = ipdhcpc_option_get_first(handle->info.options);
         option != IP_NULL;
         option = ipdhcpc_option_get_next(option))
    {   	
        /* Let user application look at/act on the option */
        if (ipdhcpc_option_callback(handle, option) == IP_FALSE)
            continue;
        
        switch(*option)
        {
        case IPDHCPC_OPTCODE_SUBNET_MASK:
            /* Set subnet mask */

            (void)ipcom_if_indextoname(handle->ifindex, ifreq.ifr_name);
            addrp = (struct Ip_sockaddr_in *)&ifreq.ip_ifr_addr;
            addrp->sin_family      = IP_AF_INET;
            IPCOM_SA_LEN_SET(addrp, sizeof(struct Ip_sockaddr_in));
            addrp->sin_addr.s_addr = IP_GET_32ON8(&option[2]);

            if (ipcom_socketioctl(ipdhcpc.fd, IP_SIOCSIFNETMASK, &ifreq) < 0)
            {
                IPCOM_LOG2(ERR, "ipdhcpc[%s] :: ipdhcpc_iflease(): ipcom_ioctl(IP_SIOCSIFNETMASK), errno = %d",
                           ift->ifname, ipcom_errno);
                IPDHCPC_LEAVE(IPCOM_ERR_FAILED);
            }
            break;

        case IPDHCPC_OPTCODE_ROUTERS:
            /* Skip if not primary interface */
            if (this_is_primary == IP_FALSE)
                break;

            /* Only pick first default route.*/
            gw_n = IP_GET_32ON8(&option[2]);

            /* Ignore router == 0.0.0.0 */
            if (gw_n == 0)
            {
                IPCOM_LOG1(DEBUG2, "ipdhcpc[%s] :: Got bogus default router (0.0.0.0), skipping", ift->ifname);
                break;
            }

            if (ift->handle->priv->gw_n != 0)
            {
                /* Delete previous DHCP default route. */
                if (ipdhcpc_route_mod(ift->handle->priv->gw_n, IP_FALSE) != 0)
                {
                    IPCOM_LOG2(ERR, "ipdhcpc[%s] :: ipdhcpc_iflease(): ipcom_ioctl(SIOCDELRT) failed, errno = %d",
                               ift->ifname, ipcom_errno);
                }
                else
                {
                    IPCOM_LOG2(DEBUG2, "ipdhcpc[%s] :: deleted default gateway = %s",
                               ift->ifname,
                               ipcom_inet_ntop(IP_AF_INET, &ift->handle->priv->gw_n, addr1, sizeof(addr1)));
                    (void)addr1;
                }
                ift->handle->priv->gw_n = 0;
            }

            /* Add default gateway. */
            if (ipdhcpc_route_mod(gw_n, IP_TRUE) != 0)
            {
                IPCOM_LOG2(ERR, "ipdhcpc[%s] :: ipdhcpc_iflease(): ipcom_ioctl(SIOCADDRT) failed, errno = %d",
                           ift->ifname, ipcom_errno);
                IPDHCPC_LEAVE(IPCOM_ERR_FAILED);
            }
            ift->handle->priv->gw_n = gw_n;
            IPCOM_LOG2(DEBUG2, "ipdhcpc[%s] :: added default gateway = %s",
                       ift->ifname,
                       ipcom_inet_ntop(IP_AF_INET, &gw_n, addr1, sizeof(addr1)));
            (void)addr1;
            break;

        case IPDHCPC_OPTCODE_DHCP_RENEWAL_TIME:
            ift->lease.t1 = IP_GET_NTOHL(&option[2]);
            break;

        case IPDHCPC_OPTCODE_DHCP_REBINDING_TIME:
            ift->lease.t2 = IP_GET_NTOHL(&option[2]);
            break;

        case IPDHCPC_OPTCODE_DHCP_LEASE_TIME:
            ift->lease.tlease = IP_GET_NTOHL(&option[2]);
            break;

        case IPDHCPC_OPTCODE_DOMAIN_NAME:
        	
            /* Skip if not primary interface */
            if (this_is_primary == IP_FALSE)
                break;

            ipcom_strncpy(buf,(char *) &option[2], option[1]);
            buf[option[1]] = 0;
            (void)ipcom_sysvar_set("ipdnsc.domainname", buf, IPCOM_SYSVAR_FLAG_OVERWRITE);
            
            break;

        case IPDHCPC_OPTCODE_DOMAIN_NAME_SERVERS:
        	            	
            /* Skip if not primary interface */
            if (this_is_primary == IP_FALSE)
                break;

            /* Set primary NS */
            addr_n = IP_GET_32ON8(&option[2]);
                       
            (void)ipcom_inet_ntop(IP_AF_INET, &addr_n, buf, 255);
            (void)ipcom_sysvar_set("ipdnsc.primaryns", buf, IPCOM_SYSVAR_FLAG_OVERWRITE);

            /* Set optional secondary NS */
            if (option[1] >= 8)
            {
                addr_n = IP_GET_32ON8(&option[6]);
                (void)ipcom_inet_ntop(IP_AF_INET, &addr_n, buf, 255);
                (void)ipcom_sysvar_set("ipdnsc.secondaryns", buf, IPCOM_SYSVAR_FLAG_OVERWRITE);
            }

            /* Set optional tertiary NS */
            if (option[1] >= 12)
            {
                addr_n = IP_GET_32ON8(&option[10]);
                (void)ipcom_inet_ntop(IP_AF_INET, &addr_n, buf, 255);
                (void)ipcom_sysvar_set("ipdnsc.tertiaryns", buf, IPCOM_SYSVAR_FLAG_OVERWRITE);
            }

            /* Set optional quaternary NS */
            if (option[1] >= 16)
            {
                addr_n = IP_GET_32ON8(&option[14]);
                (void)ipcom_inet_ntop(IP_AF_INET, &addr_n, buf, 255);
                (void)ipcom_sysvar_set("ipdnsc.quaternaryns", buf, IPCOM_SYSVAR_FLAG_OVERWRITE);
            }
            break;

        case IPDHCPC_OPTCODE_NTP_SERVERS:
           	      	
            /* Skip if not primary interface */
            if (this_is_primary == IP_FALSE)
                break;

            addr_n = IP_GET_32ON8(&option[2]);
            (void)ipcom_inet_ntop(IP_AF_INET, &addr_n, buf, 255);

            (void)ipcom_sysvar_set("ipsntp.client.primary.addr", buf, IPCOM_SYSVAR_FLAG_OVERWRITE);
            break;

        case IPDHCPC_OPTCODE_OPTION_OVERLOAD:
            /* file and/or sname field in dhcp header overloaded with options */
            option_overload = option[2];
            break;

        default:
            break;
        }
    }


    /* Notify the user about the boot server name in dhcp header */
    if (option_overload != 2 &&
        option_overload != 3 &&
        handle->info.sname &&
        handle->info.sname[0] != 0)
    {
        Ip_u8     *name_buf;
        Ip_size_t  len = ipcom_strlen((const char*)handle->info.sname);

        name_buf = ipcom_malloc(len + 2 * sizeof(Ip_u8));
        if (name_buf == IP_NULL)
            IPDHCPC_LEAVE(IPCOM_ERR_NO_MEMORY);

        name_buf[0] = IPDHCPC_OPTCODE_TFTP_SERVER_NAME;
        name_buf[1] = len;
        ipcom_memcpy(&name_buf[2],handle->info.sname,len);

        (void)ipdhcpc_option_callback(handle, name_buf);
        ipcom_free(name_buf);
    }

    /* Notify the user about the boot file name in DHCP header */
    if (option_overload != 1 &&
        option_overload != 3 &&
        handle->info.file &&
        handle->info.file[0] != 0)
    {
        Ip_u8     *name_buf;
        Ip_size_t  len = ipcom_strlen((const char*)handle->info.file);

        name_buf = ipcom_malloc(len + 2 * sizeof(Ip_u8));
        if (name_buf == IP_NULL)
            IPDHCPC_LEAVE(IPCOM_ERR_NO_MEMORY);

        name_buf[0] = IPDHCPC_OPTCODE_BOOTFILE_NAME;
        name_buf[1] = len;
        ipcom_memcpy(&name_buf[2],handle->info.file,len);

        (void)ipdhcpc_option_callback(handle, name_buf);
        ipcom_free(name_buf);
    }

    /* Tell the DHCP option callback that there are no more options */
    (void)ipdhcpc_option_callback(handle, &end);
    
    if (!bootp_mode)
    {
    	ip_assert(!IPCOM_TMO_PENDING(&handle->priv->tmo));
    	ip_assert(!IPCOM_TMO_PENDING(&handle->priv->tmo_t2));

    	if (ift->lease.t1 == 0)
            ift->lease.t1 = ift->lease.tlease / 2; /* 50% */
    	if (ift->lease.t2 == 0)
            ift->lease.t2 = ift->lease.tlease * 7 / 8; /* 87,5% */

        /* Update user info with timeout information */
        handle->info.t1 = ift->lease.t1;
        handle->info.t2 = ift->lease.t2;

    	/* Set T1 timeout for renewing. */
    	if (ift->lease.t1 > 0 && ift->lease.tlease != (Ip_time_t)0xffffffff)
    	{
       	    ipdhcpc_timer_set(handle, ipdhcpc_sec_to_msec(ift->lease.t1), 1);
    	}
    }

    retval = IPCOM_SUCCESS;

 leave:
    if (buf != IP_NULL)
        ipcom_free(buf);
    IPDHCPC_UNLOCK();
    return retval;
}


/*
 *===========================================================================
 *                    ipdhcpc_ifunlease
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipdhcpc_ifunlease(Ip_u32 ifindex, Ipdhcpc_handle **handlep)
{
    Ip_err      retval;
    Ipdhcpc_if *ift;

    IPDHCPC_LOCK();

    if (ipdhcpc.opened == IP_FALSE)
        IPDHCPC_LEAVE(IPCOM_ERR_NOT_OPENED);

    for (ift = ipdhcpc.ift_head; ift != IP_NULL; ift = ift->next)
        if (ift->ifindex == ifindex)
            break;
    if (ift == IP_NULL)
        IPDHCPC_LEAVE(IPCOM_ERR_INVALID_INTERFACE);

    /* No lease. */
    if (ift->handle == IP_NULL)
        IPDHCPC_LEAVE(IPCOM_ERR_NOT_INSTALLED);

    retval = ipdhcpc_ifunlease_locked(ift, handlep);
    if (retval == IPCOM_SUCCESS)
    {
        IPCOM_LOG1(DEBUG2, "ipdhcpc[%s] :: Stopping lease", ift->ifname);
    }

 leave:
    IPDHCPC_UNLOCK();
    return retval;
}


/*
 *===========================================================================
 *                    ipdhcpc_option_sprintf
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipdhcpc_option_sprintf(Ip_u8 *option, char *buf, int buflen)
{
    Ip_size_t len = 0;
    int       n;
    char      addr1[16];
    Ip_u32    val32;
    Ip_u16    val16;
    Ip_u8     val8;
    
    len = ipcom_snprintf(buf, buflen, "  [ Option #%3d: ", *option);
    
    switch(*option)
    {
    case IPDHCPC_OPTCODE_DHCP_MESSAGE_TYPE:
    	val8 = option[2];
    	len += ipcom_snprintf(buf + len, buflen - len, "Message Type: ");
    	switch (val8)
    	{
    	case IPDHCPC_MSG_DISCOVER:
            len += ipcom_snprintf(buf + len, buflen - len, "DISCOVER");
            break;
    	case IPDHCPC_MSG_OFFER:
            len += ipcom_snprintf(buf + len, buflen - len, "OFFER");
            break;
    	case IPDHCPC_MSG_REQUEST:
            len += ipcom_snprintf(buf + len, buflen - len, "REQUEST");
            break;
    	case IPDHCPC_MSG_DECLINE:
            len += ipcom_snprintf(buf + len, buflen - len, "DECLINE");
            break;
    	case IPDHCPC_MSG_ACK:
            len += ipcom_snprintf(buf + len, buflen - len, "ACK");
            break;
    	case IPDHCPC_MSG_NAK:
            len += ipcom_snprintf(buf + len, buflen - len, "NACK");
            break;
    	case IPDHCPC_MSG_RELEASE:
            len += ipcom_snprintf(buf + len, buflen - len, "RELEASE");
            break;
    	case IPDHCPC_MSG_INFORM:
            len += ipcom_snprintf(buf + len, buflen - len, "INFORM");
            break;
    	case IPDHCPC_MSG_INVALID:
    	default:
            len += ipcom_snprintf(buf + len, buflen - len, "Invalid");
    	}
    	len += ipcom_snprintf(buf + len, buflen - len, " ]" IP_LF);
        break;

    case IPDHCPC_OPTCODE_DHCP_CLIENT_IDENTIFIER:
        len += ipcom_snprintf(buf + len, buflen - len, "Client Identifier: ");
        if (option[1] >= 2 && option[2] == 1)
        {
            len += ipcom_snprintf(buf + len,
                                  buflen - len,
                                  "Ethernet: ");
            for (n = 0; n < option[1] - 1; n++)
                len += ipcom_snprintf(buf + len, buflen - len, "%.2x:", option[3 + n]);
        }
        else
        {
            for (n = 0; n < option[1]; n++)
                len += ipcom_snprintf(buf + len,
                                      buflen - len,
                                      "%c",
                                      option[2 + n]);
        }
        len += ipcom_snprintf(buf + len, buflen - len, " ]" IP_LF);
        break;

    case IPDHCPC_OPTCODE_DHCP_MAX_MESSAGE_SIZE:
        val16 = IP_GET_NTOHS(&option[2]);
        len += ipcom_snprintf(buf + len,
                             buflen - len,
                             "Maximum DHCP Message Size: %d ]" IP_LF,
                             (int) val16);
        break;

    case IPDHCPC_OPTCODE_DHCP_SERVER_IDENTIFIER:
        val32 = IP_GET_32ON8(&option[2]);
        len += ipcom_snprintf(buf + len,
                              buflen - len,
                              "Server Identifier: %s ]" IP_LF,
                              ipcom_inet_ntop(IP_AF_INET, 
                            		          &val32, 
                            		          addr1, 
                            		          sizeof(addr1)));
        break;

    case IPDHCPC_OPTCODE_DHCP_REQUESTED_ADDRESS:
        val32 = IP_GET_32ON8(&option[2]);
        len += ipcom_snprintf(buf + len, buflen - len,
                              "Requested IP Address: %s ]" IP_LF,
                              ipcom_inet_ntop(IP_AF_INET, 
                            		          &val32, 
                            		          addr1, 
                            		          sizeof(addr1)));
        break;

    case IPDHCPC_OPTCODE_DHCP_LEASE_TIME:
        val32 = IP_GET_NTOHL(&option[2]);
        len += ipcom_snprintf(buf + len, buflen - len,"IP Address Lease Time: %ld ]" IP_LF,
        		              val32);
        break;

    case IPDHCPC_OPTCODE_SUBNET_MASK:
        val32 = IP_GET_32ON8(&option[2]);
        len += ipcom_snprintf(buf + len,
                              buflen - len,
                              "Subnet Mask: %s ]" IP_LF,
                              ipcom_inet_ntop(IP_AF_INET, 
                            		          &val32, 
                            		          addr1, 
                            		          sizeof(addr1)));
        break;

    case IPDHCPC_OPTCODE_IP_FORWARDING:
        len += ipcom_snprintf(buf + len, buflen - len, "Forwarding: %s ]"IP_LF,
        		             option[2] == 1 ? "enable" : "disable");
        break;

    case IPDHCPC_OPTCODE_ROUTERS:
    case IPDHCPC_OPTCODE_NTP_SERVERS:
    case IPDHCPC_OPTCODE_DOMAIN_NAME_SERVERS:
        switch(*option)
        {
        case IPDHCPC_OPTCODE_ROUTERS:
            len += ipcom_snprintf(buf + len, buflen - len, "Routers:");
            break;
        case IPDHCPC_OPTCODE_NTP_SERVERS:
            len += ipcom_snprintf(buf + len, buflen - len, "NTP Servers:");
            break;
        case IPDHCPC_OPTCODE_DOMAIN_NAME_SERVERS:
            len += ipcom_snprintf(buf + len, buflen - len, "Domain Name Server:");
            break;
        default:
            IP_PANIC();
            break;
        }

        for (n = 0; n < option[1] / 4; n++)
        {
            val32 = IP_GET_32ON8(&option[2 + n * 4]);
            len += ipcom_snprintf(buf + len,
                                  buflen - len,
                                  " %s",
                                  ipcom_inet_ntop(IP_AF_INET,	&val32, addr1, sizeof(addr1)));
        }
        len += ipcom_snprintf(buf + len, buflen - len, " ]" IP_LF);
        break;

    case IPDHCPC_OPTCODE_DHCP_RENEWAL_TIME:
        val32 = IP_GET_NTOHL(&option[2]);
        len += ipcom_snprintf(buf + len, buflen - len,
        		             "Renewal(T1) Time Value: %ld ]" IP_LF, val32);
        break;

    case IPDHCPC_OPTCODE_DHCP_REBINDING_TIME:
        val32 = IP_GET_NTOHL(&option[2]);
        len += ipcom_snprintf(buf + len, buflen - len,
        		             "Renewal(T1) Time Value: %ld ]" IP_LF, val32);
        break;

    case IPDHCPC_OPTCODE_DOMAIN_NAME:
        len += ipcom_snprintf(buf + len,
                             buflen - len,
                             "Domain Name: ");
        for (n = 0; n < option[1]; n++)
            len += ipcom_snprintf(buf + len, buflen - len, "%c", option[2 + n]);
        len += ipcom_snprintf(buf + len, buflen - len, " ]" IP_LF);
        break;

    case IPDHCPC_OPTCODE_DHCP_PARAMETER_REQUEST_LIST:
        len += ipcom_snprintf(buf + len,
                             buflen - len,
                             "Parameter Request List: ");
        for (n = 0; n < option[1]; n++)
            len += ipcom_snprintf(buf + len, buflen - len, "%d ", option[2 + n]);
        len += ipcom_snprintf(buf + len, buflen - len, " ]" IP_LF);
        break;

    default:
    	
        len += ipcom_snprintf(buf + len,
                             buflen - len,
                             "length = %d, Val = ",
                             option[1]);
        for (n = 0; n < option[1]; n++)
            len += ipcom_snprintf(buf + len, buflen - len, "0x%x ",(int)option[2 + n]);
        len += ipcom_snprintf(buf + len, buflen - len, "]" IP_LF);
        
        break;
    }

    return len;
}


/*
 *===========================================================================
 *                    ipdhcpc_option_find
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_u8 *
ipdhcpc_option_find(Ipdhcpc_handle *handle, Ip_u8 code)
{
    return ipdhcpc_koption_get(handle->info.options, handle->info.optlen, code);
}


/*
 *===========================================================================
 *                    ipdhcpc_option_get_first
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_u8 *
ipdhcpc_option_get_first(Ip_u8 *options)
{
    return options;
}


/*
 *===========================================================================
 *                    ipdhcpc_option_get_next
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_u8 *
ipdhcpc_option_get_next(Ip_u8 *options)
{
    Ip_u8 optlen;

    optlen = options[1];
    options += optlen + 2;

    while (*options == IPDHCPC_OPTCODE_PAD)
        options++;

    if (*options == IPDHCPC_OPTCODE_END)
        return IP_NULL;

    return options;
}

/*
 *===========================================================================
 *                    ipdhcpc_option_add
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipdhcpc_option_add(Ipdhcpc_handle *handle, Ip_u8 opt_code, Ip_u8 *opt_data, Ip_u32 opt_len)
{
    Ip_u8  *option = IP_NULL;
    Ip_err  retval;
    Ip_bool add_end_option;

    /* Ensure we've got enough space for the new option */
    if ((opt_len + handle->optlen) > IPDHCPC_MAX_USER_OPT_SIZE)
    {
    	IPDHCPC_LEAVE(IPCOM_ERR_NO_MEMORY);	
    }
    
    if (opt_code == IPDHCPC_OPTCODE_END)
    {
    	handle->options[handle->optlen] = IPDHCPC_OPTCODE_END;
    	IPDHCPC_LEAVE(IPCOM_SUCCESS);	
    }
    
    /* Ensure the option is not one supported inherently by IPDHCPC */
    if (opt_code == IPDHCPC_OPTCODE_DHCP_MESSAGE_TYPE ||
    	opt_code == IPDHCPC_OPTCODE_DHCP_CLIENT_IDENTIFIER ||
    	opt_code == IPDHCPC_OPTCODE_DHCP_MAX_MESSAGE_SIZE)
    {
        IPCOM_LOG1(WARNING, "User addition of option %d is restricted\n", opt_code);
    	IPDHCPC_LEAVE(IPCOM_ERR_INVALID_ARG);
    }
       
    /* Ensure the option is not already present */
    option = ipdhcpc_koption_get(handle->options, handle->optlen, opt_code);   
    if (option != IP_NULL && opt_code != IPDHCPC_OPTCODE_PAD)
    {
    	IPDHCPC_LEAVE(IPCOM_ERR_INVALID_ARG);   	
    }
    
    /* If we have an existing END option, insert new option before it. Otherwise,
     * place new option at the end of the option list
     */
    if (handle->options[handle->optlen] != IPDHCPC_OPTCODE_END)
    {
    	option = &handle->options[handle->optlen];
        add_end_option = IP_FALSE;	
    }
    else
    {
    	add_end_option = IP_TRUE;	
    }
            
    /* Copy the option over */
    if (opt_data != IP_NULL && option != IP_NULL)
    {
        *option++ = opt_code;
        *option++ = opt_len;
        ipcom_memcpy(option, opt_data, opt_len);
    }
   
    /* Bump option length by the size of the option data, option size, and option code fields */
    if (opt_len > 0)
        handle->optlen += opt_len + 2;
    else
    	handle->optlen += opt_len + 1;
    
    if (add_end_option == IP_TRUE)
    {
	    handle->options[handle->optlen] = IPDHCPC_OPTCODE_END;
    }
           
    retval = IPCOM_SUCCESS;

leave:
    return retval;
}

/*
****************************************************************************
* 11                   PUBLIC FUNCTIONS
****************************************************************************
*/

/*
 *===========================================================================
 *                  ipdhcpc_create
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipdhcpc_create(void)
{
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                   ipdhcpc_start
 *===========================================================================
 * Description:   Start DHCP client process.
 * Parameters:
 * Returns:
 */
IP_PUBLIC Ip_err
ipdhcpc_start(void)
{
    Ipcom_proc_attr attr;

    ipcom_proc_attr_init(&attr);
    attr.stacksize = IPCOM_PROC_STACK_LARGE;

    return ipcom_proc_acreate("ipdhcpc", ipdhcpc_proc, &attr, IP_NULL);
}


/*
 *===========================================================================
 *                    ipdhcpc_version
 *===========================================================================
 * Description::
 * Parameters::
 * Returns:
 *
 */
IP_PUBLIC const char *
ipdhcpc_version(void)
{
    return "@(#) IPDHCPC $Name: VXWORKS_ITER18A_FRZ10 $ - INTERPEAK_COPYRIGHT_STRING";
}


/*
****************************************************************************
*                      END OF FILE
****************************************************************************
*/
