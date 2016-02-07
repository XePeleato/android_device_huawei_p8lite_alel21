/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipl2tp_example.c,v $ $Revision: 1.1.12.1.2.5 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipl2tp_example.c,v $
 *     $Author: tneale $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *
 ******************************************************************************
 */

/*
 ****************************************************************************
 *      DESCRIPTION
 ****************************************************************************
 *
 * L2TP example file.
 */

/*
 ****************************************************************************
 *      INCLUDE FILES
 ****************************************************************************
 */

#ifdef IPCOM_USE_CLIB_PROTO
#undef IPCOM_USE_CLIB_PROTO
#endif

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif

/* Synchronize build with configuration definitions */
#include "ipcom_config.h"
#ifdef INCLUDE_IPPPP
#define IPPPP
#endif
#ifdef INCLUDE_IPL2TP
#define IPL2TP
#endif
#ifdef IPPPP
#include "ipppp_config.h"
#include "ipppp.h"
#endif

#include <ipcom_hash.h>
#include <ipl2tp_api.h>
#include <ipl2tp_attr.h>
#include <ipl2tp_example.h>
#include <ipl2tp_example_v2.h>
#include <ipl2tp_example_v3.h>

#include <ipcom_netif.h>
#include <ipcom_syslog.h>
#include <ipcom_clib.h>
#include <ipcom_err.h>

#include "ipl2tp.h"
#include "ipl2tp_api_session.h"


/*
 ****************************************************************************
 *      DEFINES
 ****************************************************************************
 */

/* L2TP control port */
#define IPL2TP_PORT_NUMBER 1701

/* Extra header space allocated before L2TP header */
#define HEADER_SPACE 0

#define LF "\n"
#define ARGC_NAME -1
#define ARGC_DESCR -2

/* PPP protocols */
#define PPPP_LCP        0xc021  /* Link Control Protocol */
#define PPPP_IPCP       0x8021  /* IP Control Protocol */
#define PPPP_PAP        0xc023  /* Password Authentication Protocol */
#define PPPP_CHAP       0xc223  /* Cryptographic Handshake Auth. Prot. */
#define PPPP_IP         0x0021  /* Internet Protocol */

/* PPP CP (LCP, IPCP, etc) codes */
#define PPP_CP_CONFREQ  1       /* Configuration Request */
#define PPP_CP_CONFACK  2       /* Configuration Ack */
#define PPP_CP_CONFNAK  3       /* Configuration Nak */
#define PPP_CP_CONFREJ  4       /* Configuration Reject */
#define PPP_CP_TERMREQ  5       /* Termination Request */
#define PPP_CP_TERMACK  6       /* Termination Ack */
#define PPP_CP_CODEREJ  7       /* Code Reject */
#define PPP_CP_PROTREJ  8       /* Protocol Reject, */
#define PPP_CP_ECHOREQ  9       /* Echo Request */
#define PPP_CP_ECHOREP  10      /* Echo Reply */
#define PPP_CP_DISCREQ  11      /* Discard Request */

#ifndef IP_IPPROTO_L2TP
#define IP_IPPROTO_L2TP 115
#endif


/*
 ****************************************************************************
 *      TYPES
 ****************************************************************************
 */

typedef struct Ipl2tp_cache_head
{
    struct Ipl2tp_cache_head *next;
}
Ipl2tp_cache_head;

/*
 ****************************************************************************
 *      LOCAL DATA
 ****************************************************************************
 */

/* List of free cache buffers */
IP_STATIC Ipl2tp_cache_head *free_list = IP_NULL;

/* Main socket fd */
IP_STATIC Ip_fd udp_fd  = IP_INVALID_SOCKET;
IP_STATIC Ip_fd ip_fd   = IP_INVALID_SOCKET;

/* Output flag */
IP_STATIC int l2tp_output = 0;

/*
 ****************************************************************************
 *      PUBLIC DATA
 ****************************************************************************
 */

/*
int          ipl2tp_buffer_free = 0;
*/
int          ipl2tp_buffer_free = 1;

Ipcom_hash   *ipl2tp_example_sessions = IP_NULL;
Ipcom_list   ipl2tp_example_fds;


/*
 ****************************************************************************
 *      EXTERNAL DATA
 ****************************************************************************
 */

/* Poke vars */
IP_EXTERN int poke_count;
IP_EXTERN int poke_mtype;
IP_EXTERN int poke_avp;
IP_EXTERN int poke_offset;
IP_EXTERN int poke_value;
IP_EXTERN int poke_skip;

IP_EXTERN Ip_u16 session_table[];
IP_EXTERN Ip_u16 session_table_size;

/*
 ****************************************************************************
 *      PROTOTYPES
 ****************************************************************************
 */
IP_PUBLIC Ip_err ipl2tp_create(void);
IP_PUBLIC Ip_err ipl2tp_start(void);
#if 0
/* PPP/L2TPv2 integration for third-party PPP */
IP_PUBLIC int ppp_input_hook(int          ifindex, 
                             void         *pppbuf, 
                             Ip_size_t    length, 
                             Ip_size_t    offset);
IP_PUBLIC int ppp_output_hook(Ip_u32  ifindex, 
                              void    *pppbuf, 
                              Ip_u32  length, 
                              Ip_u32  offset);
IP_PUBLIC int ppp_open_hook(int   ifindex, 
                            int   protocol);
IP_PUBLIC int ppp_writedone_hook(int ifindex, 
                                 void *pppbuf, 
                                 void *cookie);
#endif

/*
 ****************************************************************************
 *      LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                      ipl2tp_data_obj_hash_tunnel_assigned
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC unsigned
ipl2tp_example_obj_hash_key(Ipl2tp_key  *key)
{
    /* Hash the version first */
    unsigned val = ipcom_hash_update(&key->version,
                                     sizeof(key->version), 
                                     0);

    /* Hash the key ID */
    return ipcom_hash_update(&key->ident,
                             sizeof(key->ident), 
                             val);
}

/*
 *===========================================================================
 *                    ipl2tp_data_cmp_tunnel_assigned
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC Ip_bool
ipl2tp_example_cmp_key(Ipl2tp_key  *obj_key,
                       Ipl2tp_key  *key)
{
    /* Addr, Version & ID must match */
    return obj_key->version == key->version && obj_key->ident == key->ident;
}


/*
 *===========================================================================
 *                    ipl2tp_cache_malloc
 *===========================================================================
 * Description: Allocate buffer from local cache
 * Parameters:  
 * Returns:     
 *
 */
IP_STATIC void *
ipl2tp_cache_malloc(Ip_u32 size)
{
    Ipl2tp_cache_head *head;
    Ip_u32 msr;

    msr = ipcom_interrupt_disable();

    if (free_list != IP_NULL)
    {
        head = free_list;
        free_list = head->next;
        ipcom_interrupt_enable(msr);
    }
    else
    {
        ipcom_interrupt_enable(msr);

        if ((head = ipcom_malloc(sizeof(*head) + size)) == IP_NULL)
        {
            return IP_NULL;
        }
    }

    return head + 1;
}


/*
 *===========================================================================
 *                    ipl2tp_cache_free
 *===========================================================================
 * Description: Return buffer to local cache. Allows data to be freed
 *              within interrupt context.
 * Parameters:  
 * Returns:     
 *
 */
IP_STATIC void
ipl2tp_cache_free(void *buf)
{
    Ipl2tp_cache_head *head = buf;
    Ip_u32 msr;

    head--;
    msr = ipcom_interrupt_disable();
    head->next = free_list;
    free_list = head;
    ipcom_interrupt_enable(msr);
}


/*
 *===========================================================================
 *                    ipl2tp_ppp_data_cb
 *===========================================================================
 * Description: Data callback.
 * Parameters:  user_data   - User callback data.
 *              buffer      - Original data buffer.
 *              payload     - Payload pointer in data buffer.
 *              length      - Length of payload.
 * Returns:     
 *
 */
IP_STATIC void
ipl2tp_ppp_data_cb(void *user_data, Ipl2tp_key *id,
                   void *buffer, void *payload, Ip_u16 length)
{
    int ifindex = (int)user_data;

    (void)id;
    (void)buffer;
    (void)payload;
    (void)length;

    /* Do the write */
    if (ifindex != -1)
    {
        /*
         * Buffer will be used by PPP for a while,
         * and then released in ppp_writedone_hook().
         */
/*
        ipl2tp_buffer_free = 0;
*/

        l2tp_output = 1;

        /*
        ** Insert PPP write routine here, e.g.:
        ** ppplink_write(ifindex, payload, length, HEADER_SPACE, buffer);
        */
#ifdef IPPPP_USE_PPPL2TP

        if (user_data == (void *)0x0)
        {
#ifdef IPPPP_USE_PPPL2TP_LNS
            if ((ipcom_sysvar_get_conf_as_int("ipppp.l2tp.server", IP_NULL, IP_NULL) != 1) || 
                ((ifindex = ipppp_pppl2tp_if_create_server(id)) < 0))
            {
                ipcom_printf ("Failed to create new server interface."IP_LF);
                return;
            }
#else
            ipcom_printf ("Data callback: no ifindex."IP_LF);
            return;
#endif
        }

        ipppp_pppl2tp_input (ifindex, payload, length);
#endif

        l2tp_output = 0;
    }
}


/*
 *===========================================================================
 *                    ipl2tp_l2tp_read
 *===========================================================================
 * Description: Read a L2TP control message via UDP.
 * Parameters:  
 * Returns:     
 *
 */
IP_STATIC int
ipl2tp_l2tp_read(Ip_fd                      fd,
                 Ip_u8                      **buf, 
                 union Ip_sockaddr_union    *ipp)
{
    Ip_socklen_t            faddrsize = sizeof(*ipp);
    int                     length;
    Ip_u8                   *buffer;

    /* Get buffer */
    if ((buffer = ipl2tp_cache_malloc(HEADER_SPACE +
                                      IPL2TP_MAX_MESSAGE_SIZE)) == IP_NULL)
    {
        ipcom_printf("Out of memory when reading L2TP socket"IP_LF);
        return 0;
    }

    /* Receive an UDP packet */
    if ((length = ipcom_recvfrom(fd, buffer, IPL2TP_MAX_MESSAGE_SIZE, 0,
                                 &ipp->sa,
                                 &faddrsize)) < 0)
    {
        ipcom_printf("ipl2tp_l2tp_read() failed with errno: %d"IP_LF,
                     ipcom_errno);
        ipl2tp_cache_free(buffer);
        return 0;
    }

    *buf = buffer;
    return length;
}

/*
 *===========================================================================
 *                    ipl2tp_l2tp_write
 *===========================================================================
 * Description: Send a L2TP control message via UDP.
 * Parameters:  message - Message to send.
 *              addr    - Destination IP address.
 * Returns:     
 *
 */
IP_STATIC int
ipl2tp_l2tp_write(void                      *user_data, 
                  Ipl2tp_key                *key,
                  struct Ip_iovec           *iov,
                  int                       iovlen,
                  union Ip_sockaddr_union   *addr,
                  int                       transport)
{
    int                 sz;
    int                 fd;
    struct Ip_msghdr    msg;

    (void)user_data;
    (void)key;

    if (transport == IPL2TP_ATTR_TRANSPORT_UDP)
    {
        fd = udp_fd;
    }
    else if (transport == IPL2TP_ATTR_TRANSPORT_IP)
    {
        fd = ip_fd;
    }
    else
    {
        ipcom_printf("Bad transport %u!!"IP_LF, transport);
        return -1;
    }

    ipcom_memset(&msg, 0, sizeof(msg));

    /* Set it up */
    msg.msg_iov     = iov;
    msg.msg_iovlen  = iovlen;
    msg.msg_name    = &addr->sa;
    msg.msg_namelen = addr->sa.sa_len;

    /* Transmit the UDP packet */
    if ((sz = ipcom_sendmsg(fd, &msg, 0)) < 0)
    {
        ipcom_printf("ipl2tp_l2tp_write() failed with errno: %d"IP_LF,
                     ipcom_errno);
        return 0;
    }

    return sz;
}

#if 0
/*  L2TP Debugging */
/*
 *===========================================================================
 *                    peek_poke
 *===========================================================================
 * Description: Peek and optionally poke at incoming L2TP data.
 * Parameters:  data    - Data buffer.
 * Returns:     
 *
 */
IP_STATIC int
peek_poke(Ip_u8 *data)
{
    if (poke_count != 0 &&              /* Poke active */
        (data[0] & 0x80) != 0 &&        /* Control msg */
        (poke_mtype == 0 ||             /* Wildcard */
         (data[3] > 12 &&               /* Non-ZLB */
          poke_mtype == data[19])))     /* Msg type match */
    {
        poke_count--;
        if (poke_skip != 0)
        {
            /* Skip message */
            return 0;
        }

        if (poke_avp != 0)
        {
            int mindex;
            int length;

            /* Find correct AVP */
            for (mindex = 12, length = 1;
                 length != 0 && mindex < data[3];
                 mindex += length)
            {
                /* Only allow 8-bit length */
                length = data[mindex + 1];
                if (data[mindex + 5] == poke_avp)
                {
                    data[mindex + poke_offset] = (Ip_u8)poke_value;
                    break;
                }
            }
        }
        else
        {
            data[poke_offset] = (Ip_u8)poke_value;
        }
    }

    return 1;
}
#endif


/*
 *===========================================================================
 *                    ipl2tps
 *===========================================================================
 * Description: 
 * Parameters:  
 * Returns:     
 *
 */
IP_STATIC
IPCOM_PROCESS(ipl2tps)
{
    Ip_fd                   ipd_fd = IP_INVALID_SOCKET;
    Ip_bool                 ipd_init = 0;
#ifdef IPPPP_USE_PPPL2TP
    Ip_fd                   ipl2tp_ppp_ipc_recv_fd = IP_INVALID_SOCKET;
#endif

    ipcom_proc_init();

#ifdef IPPPP_USE_PPPL2TP
    if ((ipl2tp_ppp_ipc_recv_fd = ipppp_pppl2tp_pkt_que_open()) == IP_INVALID_SOCKET)
    {
        ipcom_printf("Cannot allocate data packet output socket, errno: %d"IP_LF, ipcom_errno);
        goto leave;
    }
#endif

    /* Get socket */
    if ((udp_fd = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, IP_IPPROTO_UDP)) < 0)
    {
        ipcom_printf("Cannot allocate socket, errno: %d"IP_LF,
                     ipcom_errno);
        goto leave;
    }
    else
    {
        union Ip_sockaddr_union uaddr;

        /* Bind to L2TP port */
        ipcom_memset(&uaddr, 0, sizeof(uaddr));

        uaddr.sin.sin_family        = IP_AF_INET;
        uaddr.sin.sin_addr.s_addr   = IP_INADDR_ANY;
        uaddr.sin.sin_port          = ip_htons(IPL2TP_PORT_NUMBER);
        uaddr.sin.sin_len           = sizeof(struct Ip_sockaddr_in);
        if (ipcom_bind(udp_fd, &uaddr.sa, sizeof(struct Ip_sockaddr_in)) < 0)
        {
            ipcom_printf("Cannot bind socket, errno: %d"IP_LF,
                         ipcom_errno);
            ipcom_socketclose(udp_fd);
            goto leave;
        }
    }

    /* Get socket */
    if ((ip_fd = ipcom_socket(IP_AF_INET, IP_SOCK_RAW, IP_IPPROTO_L2TP)) < 0)
    {
        ipcom_printf("Cannot allocate socket, errno: %d"IP_LF,
                     ipcom_errno);
        ipcom_socketclose(udp_fd);
        goto leave;
    }


    /* Init IPD */
    if (ipd_init == 0 &&
        ipcom_ipd_init("ipl2tp", IPCOM_SUCCESS, &ipd_fd) != IPCOM_SUCCESS)
    {
        ipcom_printf("Cannot initialize ipd: %s"IP_LF,
                     ipcom_strerror(ipcom_errno));
        goto leave;
    }

    ipd_init = 1;

    /* Enter read loop (never return) */
    for (;;)
    {
        Ip_fd_set                   fds;            
        Ipl2tp_example_fd_entry_t   *fd;
        int                         fd_max;
        struct Ip_timeval           tv;
        int                         ret;

        IP_FD_ZERO(&fds);

        IP_FD_SET(udp_fd, &fds);
        IP_FD_SET(ip_fd, &fds);
        IP_FD_SET(ipd_fd, &fds);
        fd_max = IP_MAX(udp_fd, ip_fd);
        fd_max = IP_MAX(fd_max, ipd_fd);

#ifdef IPPPP_USE_PPPL2TP
        IP_FD_SET(ipl2tp_ppp_ipc_recv_fd, &fds); 
        fd_max = IP_MAX(fd_max, ipl2tp_ppp_ipc_recv_fd);
#endif

        fd = IPCOM_LIST_FIRST(&ipl2tp_example_fds);
        while (fd != IP_NULL)
        {
            Ipl2tp_example_fd_entry_t   *nfd = IPCOM_LIST_NEXT(&fd->next);
            if (fd->data != IP_NULL)
            {
                IP_FD_SET(fd->fd, &fds);
                fd_max = IP_MAX(fd_max, fd->fd);
            }
            else
            {
                ipcom_list_remove(&fd->next);
                ipcom_socketclose(fd->fd);
                ipcom_free(fd);
            }

            fd = nfd;
        }

        tv.tv_sec   = 0;
        tv.tv_usec  = 10000;
        if ((ret = ipcom_socketselect(fd_max + 1, &fds, IP_NULL, IP_NULL, &tv)) > 0)
        {
            union Ip_sockaddr_union uaddr;
            int                     length;
            Ip_u8                   *buffer;

            if (IP_FD_ISSET(ipd_fd, &fds))
            {
                int event;

                event = ipcom_ipd_input(ipd_fd);
                if (event == IPCOM_IPD_EVENT_RECONFIGURE)
                    goto leave;
            }


#ifdef IPPPP_USE_PPPL2TP
            if (IP_FD_ISSET(ipl2tp_ppp_ipc_recv_fd, &fds))
            {
                Ipppp_pppl2tp_pkt_que_entry que_entry;
                Ipcom_netif *netif;
                Ipcom_pkt *pkt;
                char addrstr[IP_INET_ADDRSTRLEN];

                /* Read message and feed it to L2TP main input */
                if (ipppp_pppl2tp_pkt_que_recv(&que_entry, &uaddr) == IPCOM_SUCCESS)
                {
                    if ((que_entry.netif == IP_NULL) ||
                        (que_entry.pkt == IP_NULL))
                    {
                        ipcom_printf("ipl2tps: L2TPv2 pkt from %s:%d contains null ptr(s)."IP_LF,
                            ipcom_inet_ntop(IP_AF_INET, &uaddr.sin.sin_addr, addrstr, sizeof(addrstr)),
                            ipcom_ntohs(uaddr.sin.sin_port));
                    }
                    else
                    {
                        pkt = que_entry.pkt;
                        netif = que_entry.netif;
                        ipppp_pppl2tp_output(netif, pkt);
                    }
                }
                else
                    ipcom_printf("ipl2tps: PPP pkt recv failed."IP_LF);
            }
#endif /* IPPPP_USE_PPPL2TP */

            if (IP_FD_ISSET(udp_fd, &fds))
            {
                /* Read message and feed it to L2TP main input */
                if ((length = ipl2tp_l2tp_read(udp_fd, &buffer, &uaddr)) > 0)
                {
                    ipl2tp_api_l2tp_input(buffer, 
                                          buffer + HEADER_SPACE,
                                          (Ip_u16)length, 
                                          &uaddr, 
                                          IPL2TP_ATTR_TRANSPORT_UDP);

                    /* Free buffer if not in PPP output queue */
                    if (ipl2tp_buffer_free)
                    {
                        ipl2tp_cache_free(buffer);
                    }
                }
            }

            if (IP_FD_ISSET(ip_fd, &fds))
            {
                /* Read message and feed it to L2TP main input */
                if ((length = ipl2tp_l2tp_read(ip_fd, &buffer, &uaddr)) > 0)
                {
                    /* Remove the IP header before invoking */
                    ipl2tp_api_l2tp_input(buffer, 
                                          buffer + HEADER_SPACE + 20,
                                          (Ip_u16)(length - 20), 
                                          &uaddr, 
                                          IPL2TP_ATTR_TRANSPORT_IP);

                    /* Free buffer if not in PPP output queue */
                    if (ipl2tp_buffer_free)
                    {
                        ipl2tp_cache_free(buffer);
                    }
                }
            }

            fd = IPCOM_LIST_FIRST(&ipl2tp_example_fds);
            while (fd != IP_NULL)
            {
                Ipl2tp_example_fd_entry_t   *nfd = IPCOM_LIST_NEXT(&fd->next);
                if (fd->data!= IP_NULL && IP_FD_ISSET(fd->fd, &fds))
                {
                    /* Read message and feed it to L2TP main input */
                    if ((length = ipl2tp_l2tp_read(fd->fd, &buffer, &uaddr)) > 0)
                    {
                        if (fd->data != IP_NULL)
                        {
                            (*fd->data) (fd,
                                         buffer, 
                                         buffer + HEADER_SPACE, 
                                         length);
                        }

                        /* Free buffer if not in PPP output queue */
                        if (ipl2tp_buffer_free)
                        {
                            ipl2tp_cache_free(buffer);
                        }
                    }
                }

                fd = nfd;
            }
        }
        else if (ret < 0)
        {
            ipcom_printf("select error"IP_LF);
        }        
    }

leave:
    /* Have to call ipd init if not done yet */
    if (ipd_init == 0 &&
        ipcom_ipd_init("ipl2tp", IPCOM_ERR_FAILED, &ipd_fd) == IPCOM_SUCCESS)
    {
        ipd_init = 1;
    }
    if (ipd_init != 0)
    {
        (void)ipcom_ipd_exit("ipl2tp", ipd_fd);
    }

    ipcom_proc_exit();
}


/*
 *===========================================================================
 *                    ipl2tp_create
 *===========================================================================
 * Description: 
 * Parameters:  
 * Returns:     
 *
 */
IP_PUBLIC Ip_err
ipl2tp_create(void)
{
    return ipl2tp_api_init() == IPL2TP_STATUS_OK ? IPCOM_SUCCESS : IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    ipl2tp_start
 *===========================================================================
 * Description: Start L2TP daemon.
 * Parameters:  
 * Returns:     
 *
 */
IP_PUBLIC Ip_err
ipl2tp_start(void)
{
    Ip_u16  pw_list[]   = 
    { 
        IPL2TP_ATTR_PW_TYPE_ETHERNET, 
        IPL2TP_ATTR_PW_TYPE_ETHERNET_VLAN        
    };

    Ipl2tp_attrobj attr      = IP_NULL;
    Ipl2tp_attrobj tattr2    = IP_NULL;
    Ipl2tp_attrobj sattr2    = IP_NULL;
    Ipl2tp_attrobj tattr3    = IP_NULL;
    Ipl2tp_attrobj sattr3    = IP_NULL;

    Ip_u32 enable_v2;
    Ip_u32 enable_v3;

    ipl2tp_example_sessions = ipcom_hash_new((Ipcom_hash_obj_func)ipl2tp_example_obj_hash_key,
                                             (Ipcom_hash_key_func)ipl2tp_example_obj_hash_key,
                                             (Ipcom_hash_cmp_func)ipl2tp_example_cmp_key);

    ipcom_list_init(&ipl2tp_example_fds);

    /* Initialize */
    ipl2tp_attr_strappend(&attr, 
                          IPL2TP_ATTR_SET_HOST_NAME, 
                          L2TP_HOST_NAME);
    ipl2tp_attr_pappend(&attr, 
                        IPL2TP_ATTR_SET_L2TP_WRITE_CALLBACK,
                        (void *)ipl2tp_l2tp_write);

    ipl2tp_attr_iappend(&attr, 
                        IPL2TP_ATTR_SET_ENABLE_OUTGOING,
                        IPL2TP_MIB_YES);

#if L2TP_VERSION_2_ENABLE == TRUE
    enable_v2 = IPL2TP_MIB_YES;
#else
    enable_v2 = IPL2TP_MIB_NO;
#endif

    ipl2tp_attr_iappend(&attr, 
                        IPL2TP_ATTR_SET_ENABLE_L2TP_V2,
                        enable_v2);

#if L2TP_VERSION_3_ENABLE == TRUE
    enable_v3 = IPL2TP_MIB_YES;
#else
    enable_v3 = IPL2TP_MIB_NO;
#endif

    ipl2tp_attr_iappend(&attr, 
                        IPL2TP_ATTR_SET_ENABLE_L2TP_V3,
                        enable_v3);

    ipl2tp_attr_pappend(&tattr2, 
                        IPL2TP_TATTR_SET_STATUS_CALLBACK,
                        (void *)ipl2tp_example_tstatus_v2);
    ipl2tp_attr_pappend(&tattr3, 
                        IPL2TP_TATTR_SET_STATUS_CALLBACK,
                        (void *)ipl2tp_example_tstatus_v3);
    ipl2tp_attr_vappend(&tattr3, 
                        IPL2TP_TATTR_SET_LOCAL_PSEUDOWIRE_CAPABILITIES_LIST,
                        (void *)pw_list,
                        sizeof(pw_list));

    ipl2tp_attr_pappend(&sattr2, 
                        IPL2TP_SATTR_SET_STATUS_CALLBACK,
                        (void *)ipl2tp_example_sstatus_v2);
    ipl2tp_attr_pappend(&sattr2, 
                        IPL2TP_SATTR_SET_DATA_CALLBACK,
                        (void *)ipl2tp_ppp_data_cb);
    ipl2tp_attr_pappend(&sattr3, 
                        IPL2TP_SATTR_SET_STATUS_CALLBACK,
                        (void *)ipl2tp_example_sstatus_v3);
    ipl2tp_attr_iappend(&sattr3, 
                        IPL2TP_SATTR_SET_PSEUDOWIRE_TYPE,
                        IPL2TP_ATTR_PW_TYPE_ETHERNET_VLAN);

    ipl2tp_api_open(attr, tattr2, tattr3, sattr2, sattr3);

    ipl2tp_attr_delete(&attr);
    ipl2tp_attr_delete(&tattr2);
    ipl2tp_attr_delete(&sattr2);
    ipl2tp_attr_delete(&tattr3);
    ipl2tp_attr_delete(&sattr3);

    /* Spawn new daemon process */
    return ipcom_proc_acreate("l2tps", ipl2tps, IP_NULL, IP_NULL);
}


#if 0
/* PPP/L2TPv2 integration for third-party PPP */

/*
 *===========================================================================
 *                    ppp_input_hook
 *===========================================================================
 * Description: PPP packet input hook.
 * Parameters:  ifindex - Interface index.
 *              pppbuf  - PPP packet.
 *              length  - Length of PPP packet.
 *              offset  . ???
 * Returns:     0 to let PPP frame be parsed, else silently discarded.
 *
 */
IP_PUBLIC int
ppp_input_hook(int          ifindex, 
               void         *pppbuf, 
               Ip_size_t    length, 
               Ip_size_t    offset)
{
    Ipl2tp_key  *key    = ipl2tp_cmd_session_key(ifindex);

    /* Did we locate a session key for this index? */
    if (key == IP_NULL)
        return 0;

    ipl2tp_api_session_ppp_input(key, pppbuf, length, offset, 0, IP_TRUE);

    return 1;
}


/*
 *===========================================================================
 *                    ppp_output_hook
 *===========================================================================
 * Description: PPP packet output hook. This hook may be called reentrantly 
 *              in interrupt context. Such calls will be ignored, which means 
 *              that an occasional CONFREQ may be lost. However, this only 
 *              happens in the case of PPP congestion, which should never 
 *              occur during LCP negotiation.
 * Parameters:  ifindex     - Interface index.
 *              pppbuf      - PPP packet.
 *              length      - Length of PPP packet.
 *              offset      - ???
 * Returns:     0 to let PPP frame be parsed, else silently discarded.
 *
 */
IP_PUBLIC int
ppp_output_hook(Ip_u32  ifindex, 
                void    *pppbuf, 
                Ip_u32  length, 
                Ip_u32  offset)
{
    (void)ifindex;
    (void)pppbuf;
    (void)length;
    (void)offset;

    return l2tp_output == 0;
}


/*
 *===========================================================================
 *                    ppp_open_hook
 *===========================================================================
 * Description: PPP link 
 * Parameters:  ifindex  - Interface index.
 *              protocol - Protocol code.
 * Returns:     
 *
 */
IP_PUBLIC int
ppp_open_hook(int   ifindex, 
              int   protocol)
{
    (void)ifindex;

    switch (ip_ntohs(protocol))
    {
    case PPPP_LCP:
    case PPPP_PAP:
    case PPPP_CHAP:
    case PPPP_IPCP:
        break;

    default:
        return 1;
    }

    return 0;
}


/*
 *===========================================================================
 *                    ppp_writedone_hook
 *===========================================================================
 * Description: Release buffer when not required any more.
 * Parameters:  ifindex - Interface index.
 *              pppbuf  - PPP buffer.
 *              cookie  - Original buffer pointer.
 * Returns:     
 *
 */
IP_PUBLIC int
ppp_writedone_hook(int  ifindex, 
                   void *pppbuf, 
                   void *cookie)
{
    (void)ifindex;
    (void)pppbuf;

    ipl2tp_cache_free(cookie);

    return 0;
}
#endif /* 0 */

/*
 ****************************************************************************
 *      END OF FILE
 ****************************************************************************
 */
