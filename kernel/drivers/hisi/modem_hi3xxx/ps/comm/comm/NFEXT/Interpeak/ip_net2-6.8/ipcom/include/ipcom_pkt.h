/* ipcom_pkt.h - API of IPCOM packets */

/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_pkt.h,v $ $Revision: 1.102.18.1.2.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_pkt.h,v $
 *   $Author: ulf $ $Date: 2010-07-02 13:20:55 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_PKT_H
#define IPCOM_PKT_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Definition of IPNET and IPLITE packet structure - Ipcom_pkt.
 */

/*
DESCRIPTION
This library contains the API for IPCOM packets.
INCLUDE FILES: ipcom_atomic.h, ipcom_cstyle.h, ipcom_type.h
*/

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_atomic.h>
#include <ipcom_cstyle.h>
#include <ipcom_type.h>

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
 *                      Driver layer macros
 *===========================================================================
 * Set pkt->start to a correct minimum value with alignment 'align' on 'data'.
 */
#define IPCOM_DRV_SET_PKT_START(pkt, link_hdr_size, align) \
    do { \
        Ip_ptrdiff_t a = (align) - 1; \
        (pkt)->start  = (int)(ipcom_conf_max_link_hdr_size - (link_hdr_size)); \
        (pkt)->start += (int)(((align) - ((Ip_ptrdiff_t)&(pkt)->data[(pkt)->start] & a)) & a); \
    } while((0))


/* Maximum number of times this packet can go through the IP-output layer
   in the same context. */
#define IPCOM_PKT_MAX_RECURSE_LEVEL   4

#define IPCOM_PKT_INFO_LEN  400

/* packet info things */
#define IPCOM_PKT_INFO_COPY  0x80 /* All types with this bit set
                                          must be be copied to clones */

#define IPCOM_PKT_INFO_OPRIO           (1 | IPCOM_PKT_INFO_COPY)
#define IPCOM_PKT_INFO_DROP_PRECEDENCE (2 | IPCOM_PKT_INFO_COPY)

/*
 * Special ipcom_pkt_malloc() fcflags values.  These must not use
 * bits used by the IP_FLAG_FC_* macros in ipcom_cstyle.h.
 * Note that the bits (1 << 16) to (1 << 31) are used internally
 * by ipnet_pkt_pool.c and are not available either.  We start
 * at (1<<15) and allocate towards less-significant bits until
 * we hit those in ipcom_cstyle.h.
 */

/*
 * Don't allocate a larger buffer if the best fit buffer size for
 * the specified nonzero MTU is not available. Setting this flag
 * in fcflags in ipcom_pkt_malloc(), for a given mtu value the
 * same memory buffer cache will always be used to allocate the buffer.
 */
#define IPCOM_FLAG_FC_EXACT 0x8000

#ifndef IP_FASTTEXT
#define IP_FASTTEXT
#endif

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

struct Ipcom_pkt_struct;     /* dummy forward declaration. */
struct Ipcom_netif_struct;   /* dummy forward declaration. */

/*
 *===========================================================================
 *                         Ipcom_pkt_trace_buffer
 *===========================================================================
 * A trace is a notification of who owned the packet and where in the
 * code the trace was made from. Can be usefull in debugging to figure
 * out where in the stack this packet has been
 */
#ifdef IP_DEBUG
#define IPCOM_PKT_TRACE_BUFFER_LEN 10

/*
 *===========================================================================
 *                         IPCOM_PKT_ID_XXX
 *===========================================================================
 *
 */
enum Ipcom_pkt_id {
    IPCOM_PKT_ID_INIT,
    IPCOM_PKT_ID_MALLOC,           /* Packet has just been allocated */
    IPCOM_PKT_ID_RX,               /* In driver RX */
    IPCOM_PKT_ID_TX,               /* In driver TX */
    IPCOM_PKT_ID_FREEQ,            /* In free queue */
    IPCOM_PKT_ID_ADDREF,           /* Decreasae reference */
    IPCOM_PKT_ID_DELREF,           /* Decreasae reference */
    IPCOM_PKT_ID_INPUT,            /* Packed is processed by ipcom_pkt_input(). */
    IPCOM_PKT_ID_INQ,              /* In input-queue */
    IPCOM_PKT_ID_OUTQ,             /* In input output-queue */
    IPCOM_PKT_ID_NETIF_OUTQ,       /* In interface output-queue */
    IPCOM_PKT_ID_SOCKRECVQ,        /* In socket receive queue */
    IPCOM_PKT_ID_TCP,              /* Allocated by TCP. */
    IPCOM_PKT_ID_TCP_INPUT,        /* Packet is processed by TCP input */
    IPCOM_PKT_ID_TCP_OUTPUT,       /* Packet is processed by TCP output */
    IPCOM_PKT_ID_TCP_SOCKOOORECVQ, /* In TCP out-of-order receive queue */
    IPCOM_PKT_ID_TCP_RETRANSQ,     /* In TCP retransmission queue */
    IPCOM_PKT_ID_IP4_INPUT,        /* Packet is processed by IPv4 input */
    IPCOM_PKT_ID_IP4_OUTPUT,       /* Packet is processed by IPv4 output */
    IPCOM_PKT_ID_IP4_REASSEMBLY,   /* Packet is a IPv4 fragment in the reassembly queue */
    IPCOM_PKT_ID_IP6_INPUT,        /* Packet is processed by IPv6 input */
    IPCOM_PKT_ID_IP6_OUTPUT,       /* Packet is processed by IPv6 output */
    IPCOM_PKT_ID_IP6_REASSEMBLY,   /* Packet is a IPv6 fragment in the reassembly queue */
    IPCOM_PKT_ID_REMOVE,           /* The packet is beeing removed from its queue */
    IPCOM_PKT_ID_IPSEC_INPUT,      /* Packet is processed by ipsec input */
    IPCOM_PKT_ID_IPSEC_OUTPUT,     /* New ipsec packet for output */
    IPCOM_PKT_ID_IPSEC_OUTPUT_ORG, /* Packet is processed by ipsec output */
    IPCOM_PKT_ID_IPSEC_PF_KEY_IN,  /* PF_KEY input */
    IPCOM_PKT_ID_PPP,              /* Allocated by PPP. */
    IPCOM_PKT_ID_PPP_ORGOUT,       /* Original output packet (unescaped). */
    IPCOM_PKT_ID_PPP_INPUT,        /* Packet is processed by PPP input */
    IPCOM_PKT_ID_PPP_OUTPUT,       /* Packet is processed by PPP output */
    IPCOM_PKT_ID_RTSOCK,           /* Allocated by routesock. */
    IPCOM_PKT_ID_UDP_INPUT,        /* UDP input. */
    IPCOM_PKT_ID_NETIF_DEQUEUE,    /* Dequeued from interface queue */
    IPCOM_PKT_ID_SOCKSENDQ,        /* Datagram socket send queue */
    IPCOM_PKT_ID_SENT_PIPE,        /* Sent into stack pipe */
    IPCOM_PKT_ID_RECV_PIPE,        /* Received from stack pipe */
    IPCOM_PKT_ID_FRAG,             /* New fragment */
    IPCOM_PKT_ID_PRE_SEND_SIG,     /* Pending Secure NDP signature verification/creation */
    IPCOM_PKT_ID_POST_SEND_SIG,    /* After Secure NDP signature verification/creation */
    IPCOM_PKT_ID_PER_CPU_CACHE,    /* Packet is freed and sitting in the per-cpu cache */
    IPCOM_PKT_ID_SCTP_INPUT        /* Packet is processed by SCTP input */
};

typedef struct Ipcom_pkt_trace_buffer_struct
{
    enum Ipcom_pkt_id id;
    Ip_pid_t       pid;  /* Process ID of the task owning this packet
                            when the trace entry was stored */
    IP_CONST char *file; /* The file from where the trace entry was
                            made */
    int            line; /* The line from where the trace entry was
                            made */
}
Ipcom_pkt_trace_buffer;


#define IPCOM_PKT_TRACE_FL(pkt, id_arg, file_arg, line_arg) \
    do { \
        Ip_u32 tbval = (pkt)->next_trace++; \
        Ipcom_pkt_trace_buffer *tb; \
        if (tbval >= IPCOM_PKT_TRACE_BUFFER_LEN) \
            tbval = 0; \
        tb = &(pkt)->trace_buffer[tbval]; \
        if (pkt->next_trace >= IPCOM_PKT_TRACE_BUFFER_LEN) \
            pkt->next_trace = 0; \
        tb->id   = id_arg; \
        tb->pid  = ipcom_getpid();\
        tb->file = file_arg; \
        tb->line = line_arg; \
    } while (0);

#else
#define IPCOM_PKT_TRACE_FL(pkt, id_arg, file, line)
#endif

#define IPCOM_PKT_TRACE(pkt, id_arg) IPCOM_PKT_TRACE_FL(pkt, id_arg, __FILE__, __LINE__)

/* Emulates the old pkt->id member */
#define IPCOM_GET_PKT_ID(pkt) \
    ((pkt)->trace_buffer[(pkt)->next_trace == 0 \
                         ? IPCOM_PKT_TRACE_BUFFER_LEN - 1   \
                         : pkt->next_trace - 1].id)


/*
 *===========================================================================
 *                         Ipcom_pkt_freefunc
 *===========================================================================
 * Function to free the pkt->data buffer.
 */
typedef void (*Ipcom_pkt_freefunc)(void *data, void *cookie);


/*
 *===========================================================================
 *                         Ipcom_pkt_dtor
 *===========================================================================
 * Function that will be run just before the packet is freed.
 */
typedef void (*Ipcom_pkt_dtor)(struct Ipcom_pkt_struct *pkt);


/*
 *===========================================================================
 *                       Ipcom_pkt_opt_t
 *===========================================================================
 */
#ifdef IPCOM_USE_PKT_OPS
/* TCP Segmentation Offload specific functions */
typedef void (*Ipcom_pkt_set_tso)(struct Ipcom_pkt_struct *self, int mss, int tcp_hdr_len);
typedef Ip_bool (*Ipcom_pkt_is_tso_active)(struct Ipcom_pkt_struct *self);

typedef struct Ipcom_pkt_ops_struct
{
#ifdef IPCOM_USE_TSO
    Ipcom_pkt_set_tso       set_tso;
    Ipcom_pkt_is_tso_active is_tso_active;
#endif /* IPCOM_USE_TSO */
}
Ipcom_pkt_ops_t;
#endif /* IPCOM_USE_PKT_OPS */

#ifdef IP_PORT_VXWORKS
/*
 *===========================================================================
 *                         Ipcom_pkt_done_func
 *===========================================================================
 * Function to free a packet from VxWorks code, possibly calling an output
 * done function.
 */
#ifdef IP_DEBUG
typedef void (*Ipcom_pkt_done_func)(struct Ipcom_pkt_struct *pkt,
                                    char * file, int line);
#define IPCOM_PKT_DONE_DEFAULT ipcom_pkt_dfree
#else
typedef void (*Ipcom_pkt_done_func)(struct Ipcom_pkt_struct *pkt);
#define IPCOM_PKT_DONE_DEFAULT ipcom_pkt_free
#endif /* IP_DEBUG */
#endif /* IP_PORT_VXWORKS */


/*
 *===========================================================================
 *                         Ipcom_pkt
 *===========================================================================
 */

typedef struct Ipcom_pkt_struct
{
    /* Public structures */
    int          fd;            /* Socket descriptor. */
    Ip_u16       vr_index;      /* The virtual router index */
    Ip_u16       offset;        /* IP fragment offset */
    Ip_u32       ifindex;       /* Interface index. */
    Ip_u32       flags;         /* Packet flags. */

    int          ipstart;       /* Start of IP (v4/v6) header */
    int          tlstart;       /* Start of transport header */
    int          start;         /* Start of layer data */
    int          end;           /* End of layer data */

    /* 32 bytes for cache alignment */
    /***** Private structures *****/
#if defined (IPCOM_NAE) || defined (IPCOM_USE_FORWARDER)
    void        *nae_ctx;       /* NAE context specific
                                   information. Its meaning might be
                                   different on different
                                   achitectures.*/
#endif
#ifdef IPCOM_STORE_PHYSICAL_IF
    Ip_u32       physical_ifindex; /* The physical rx interface */
    struct Ipcom_netif_struct *physical_netif; /* The physical rx netif */
#endif

    Ip_ptrdiff_t link_cookie;   /* Link specific cookie. */
    Ip_u32       chk;           /* Checksum place holder. */
    Ip_u8        recurs_level;  /* Number of times the IP-layer output function has been
                                   called in the current call-chain */
    Ip_u8        vrid;          /* VRRP ID of this packet */

    Ip_u16       lla_size;      /* Size of Source MAC LLA */
    int          lla_off;       /* Source MAC LLA */

    Ip_u8        ipsec_pad;     /* IPSEC unused */
    Ip_u8        ipsec_skip;    /* IPSEC already done on this output packet */
    Ip_u16       ipsec_hlen;    /* IPSEC output packet header and trailer space (forwarded packets only) */

    Ip_u16       ipsec_seq;     /* IPSEC sequence tag */
    Ip_u8        ipsec_input;   /* IPSEC input */
    Ip_u8        ipsec_domain;  /* IPSEC domain */

    /* All fields after and including maxlen is not copied when a packet is cloned */
    int          maxlen;        /* Maximum length of data buffer. */
#ifdef IPCOM_USE_SMP
    Ipcom_atomic_t atomic_ref_count;   /* The reference count for this packet */
#else
    int          ref_count;
#endif
    /* Data space. Note: 'data' _must_ start at 32-bit alignemnt! */
    Ip_u8       *data;
#ifdef IP_PORT_VXWORKS
    Ipcom_pkt_done_func done; /* (send) done function */
    void        *done_arg;       /* extra info for done() function */

    int          net_svc;        /* network service type for benefit of
                                    the MUX */
#endif
    void                   *data_freefunc_cookie;   /* Free function cookie */
    Ipcom_pkt_freefunc      data_freefunc;          /* Function to free the pkt->data buffer */
    Ipcom_pkt_dtor          dtor;

    /* 32 bytes for cache alignment, if IPCOM_STORE_PHYSICAL_IF is not defined */

    struct Ipcom_pkt_struct  *next;          /* Next packet in a list */
    struct Ipcom_pkt_struct  *next_fragment; /* Next fragment in a datagram */
    struct Ipcom_pkt_struct  *next_original; /* Pointer to original pkt. */
    struct Ipcom_pkt_struct  *next_part;     /* Next packet if this
                                                packet is distributed
                                                over more than one
                                                packet (scatter/gather) */

#ifdef IPCOM_USE_PKT_OPS
    IP_CONST Ipcom_pkt_ops_t *ops;
#endif

    /* Extra information accociated with this packet. Must only be
       accessed through the ipcom_pkt_*_info() functions. */
    unsigned                 next_extra_data;
    Ip_ptrdiff_t             extra_data[IPCOM_PKT_INFO_LEN / sizeof(Ip_ptrdiff_t)];

#ifdef IPROHC
    /* Store pkt's destination address for ethernet multicast */
    Ip_bool     flag_rohc;
    Ip_u32	dst[4];
#endif /* IPROHC */

    /* Force link layer to output pkt with this frametype set */
    Ip_u16 force_frame_type;

#ifdef IPCOM_USE_MUX_DRV
    void *route_info;
#endif

#ifdef IP_DEBUG
    struct Ipcom_pkt_struct *next_alloc;    /* List of allocated
                                               packets */
    Ip_u32                   num_allocated; /* Number of time this
                                               packet has been
                                               allocated */
    unsigned                 next_trace;
    Ipcom_pkt_trace_buffer   trace_buffer[IPCOM_PKT_TRACE_BUFFER_LEN];
#endif /* IP_DEBUG */


}
Ipcom_pkt;


/* packet flags 'pkt->flags'. */

/* Generic flags */
#define IPCOM_PKT_FLAG_ALLOC          (1 << 0)    /* The packet is allocated */
#define IPCOM_PKT_FLAG_LOOPED_BACK    (1 << 1)    /* This (incoming) packet was sent and received on the same interface */
#define IPCOM_PKT_FLAG_DELAYED        (1 << 2)    /* This packet was queued for delayed output from inputd/tickd. */

#define IPCOM_PKT_FLAG_ZBUF           (1 << 3)    /* ZeroCopy. Used to remember not to perform checksum of UDP payload. */

/* Socket layer flags */
#define IPCOM_PKT_FLAG_NONBLOCKING    (1 << 4)    /* Packet was sent using a socket in non-blocking mode */
#define IPCOM_PKT_FLAG_DONTROUTE      (1 << 5)    /* Send this on the interface that match the network portion of
                                                     the destionation address */
#define IPCOM_PKT_FLAG_NO_RX_CACHE    (1 << 6)    /* This packet cannot be used to create a RX cache entry */

/* Network layer flags */
#define IPCOM_PKT_FLAG_NAT            (1 << 7)    /* NAT translated packet */
#define IPCOM_PKT_FLAG_BROADCAST      (1 << 8)    /* Destination address is a broadcast address */
#define IPCOM_PKT_FLAG_MULTICAST      (1 << 9)    /* Destination address is a multicast address */
#define IPCOM_PKT_FLAG_LOOP_MCAST     (1 << 10)   /* Multicast packets should be looped back */
#define IPCOM_PKT_FLAG_FORWARDED      (1 << 11)   /* Forwarded packet. */
#define IPCOM_PKT_FLAG_FRAGMENT       (1 << 12)   /* This packet is an IP fragment */
#define IPCOM_PKT_FLAG_MORE_FRAG      (1 << 13)   /* There is more fragments (but 'next_fragment' can still be IP_NULL) */
#define IPCOM_PKT_FLAG_HAS_IP_HDR     (1 << 14)   /* This outgoing packet has an IP header */
#define IPCOM_PKT_FLAG_ROUTER_ALERT   (1 << 15)   /* This package has a router alert option */
#define IPCOM_PKT_FLAG_IPV4           (1 << 16)   /* Used for incoming packets, this is an IPv4 packet */
#define IPCOM_PKT_FLAG_IPV6           (1 << 17)   /* Used for incoming packets, this is an IPv6 packet */
#define IPCOM_PKT_FLAG_TUNNELED       (1 << 18)   /* Used for incoming packets, this packet has been tunneled */
#define IPCOM_PKT_FLAG_OUTPUT         (1 << 19)   /* Set for outgoing packets */
#define IPCOM_PKT_FLAG_IPSEC_DONE     (1 << 20)   /* Incoming ESP/AH packet authenticated by IPSEC */
#define IPCOM_PKT_FLAG_PROGRESS       (1 << 21)   /* The transport layer is making forward progress, so the
                                                     neighbor used to communicat with the peer is reachable */
#define IPCOM_PKT_FLAG_HW_CHECKSUM    (1 << 22)   /* Output: the hardware must calculate the checksum,
                                                     the checksum will be calculated from &pkt->data[pkt->tlstart]
                                                     to &pkt->data[pkt->end] and stored at pkt->data[pkt->tlstart + pkt->chk]
                                                     Input: Transport header checksum has been verified */
#define IPCOM_PKT_FLAG_TL_CHECKSUM    (1 << 23)   /* Input: The pkt->chk field contains the checksum of the transport data */
#define IPCOM_PKT_FLAG_VLAN_TAG       (1 << 24)   /* Output: The pkt->link_cookie field contains vlan tag in host-byte
                                                     order, hardware is expected to insert the tag to frame on transmit.
                                                     Input:  The pkt->link_cookie field contains vlan tag in host-byte
                                                     order. The received ethernet header is untagged.
                                                   */

/* Link layer flags */
#define IPCOM_PKT_FLAG_LINK_BROADCAST (1 << 25)   /* The packet was sent to a link layer broadcast address */
#define IPCOM_PKT_FLAG_LINK_MULTICAST (1 << 26)   /* The packet was sent to a link layer multicast address */
#define IPCOM_PKT_FLAG_LINK_OTHER     (1 << 27)   /* The packet was sent to a link layer address not assigned to this host */
#define IPCOM_PKT_FLAG_LINK1          (1 << 28)   /* Private link layer flag #1 */
#define IPCOM_PKT_FLAG_LINK2          (1 << 29)   /* Private link layer flag #2 */
#define IPCOM_PKT_FLAG_LINK3          (1 << 30)   /* Private link layer flag #3 */
#define IPCOM_PKT_FLAG_LINK4          (1 << 31)   /* Private link layer flag #4 */

/* packet malloc priorities */
#define IPCOM_PKT_MPRIO_MIN           0
#define IPCOM_PKT_MPRIO_MAX           10
#define IPCOM_PKT_MPRIO_DEFAULT       (IPCOM_PKT_MPRIO_MIN + 1)
#define IPCOM_PKT_MPRIO_STACK         IPCOM_PKT_MPRIO_MAX
#define IPCOM_PKT_MPRIO_DRV           IPCOM_PKT_MPRIO_MAX

/* packet output priorities */
#define IPCOM_PKT_GET_OPRIO(pkt)     ipcom_pkt_get_int(pkt, IPCOM_PKT_INFO_OPRIO, Ip_u8)
#define IPCOM_PKT_SET_OPRIO(pkt, op) ipcom_pkt_set_int(pkt, IPCOM_PKT_INFO_OPRIO, op, Ip_u8)
#define IPCOM_PKT_OPRIO_BEST_EFFORT       0x0  /* Default priority */
#define IPCOM_PKT_OPRIO_FILLER            0x1  /* Background tasks (ex: e-mail) */
#define IPCOM_PKT_OPRIO_BULK              0x2  /* Bulk transfer (ex: FTP) */
#define IPCOM_PKT_OPRIO_INTERACTIVE       0x3  /* Interactive traffic (ex: SSH) */
#define IPCOM_PKT_OPRIO_INTERACTIVE_BULK  0x4  /* Interactive bulk traffic (ex: video conferences) */
#define IPCOM_PKT_OPRIO_CONTROL           0x5  /* Control traffic (ex: ) */
#define IPCOM_PKT_OPRIO_MAX               0xf

#define IPCOM_PKT_GET_DROP_PRECEDENCE(pkt)     ipcom_pkt_get_int(pkt, IPCOM_PKT_INFO_DROP_PRECEDENCE, Ip_u8)
#define IPCOM_PKT_SET_DROP_PRECEDENCE(pkt, dp) ipcom_pkt_set_int(pkt, IPCOM_PKT_INFO_DROP_PRECEDENCE, dp, Ip_u8)
#define IPCOM_PKT_DROP_PRECEDENCE_UNDEF   0x0  /* Used for traffic that is forwarded using "best effort" policy */
#define IPCOM_PKT_DROP_PRECEDENCE_LOW     0x1  /* Drop only if absolutely neccessary */
#define IPCOM_PKT_DROP_PRECEDENCE_MEDIUM  0x2  /* Drop to avoid very long latency and very high memory consumption */
#define IPCOM_PKT_DROP_PRECEDENCE_HIGH    0x3  /* Drop packet in the case of congestion */

/* Set/get virtual router ID */
#define IPCOM_PKT_SET_VRID(pkt, vrid_) ((pkt)->vrid = vrid_)
#define IPCOM_PKT_GET_VRID(pkt)        ((Ip_u8) (pkt)->vrid)

/* Packet reference count macros */
#ifdef IPCOM_USE_SMP
# define IPCOM_PKT_INIT_REF_COUNT(pkt)       (ipcom_atomic_set(&(pkt)->atomic_ref_count, 1))
# define IPCOM_PKT_GET_REF_COUNT(pkt)        (ipcom_atomic_get(&(pkt)->atomic_ref_count))
# define IPCOM_PKT_ADD_REF(pkt)              IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_ADDREF); \
    ipcom_atomic_inc(&(pkt)->atomic_ref_count)
#define IPCOM_PKT_DEL_REF(pkt)              (ipcom_atomic_sub_and_return(&(pkt)->atomic_ref_count,1))
# ifdef IP_PORT_LKM
#  define IPCOM_PKT_HAS_MULTIPLE_OWNERS(pkt) (ipcom_atomic_get(&(pkt)->atomic_ref_count) > 1 || ipcom_lkm_skb_get_ref_count(pkt) > 1)
# else
#  define IPCOM_PKT_HAS_MULTIPLE_OWNERS(pkt) (ipcom_atomic_get(&(pkt)->atomic_ref_count) > 1)
# endif
#else /* !IPCOM_USE_SMP */
# define IPCOM_PKT_INIT_REF_COUNT(pkt)      ((pkt)->ref_count = 1)
# define IPCOM_PKT_GET_REF_COUNT(pkt)       ((pkt)->ref_count)
# define IPCOM_PKT_ADD_REF(pkt)              IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_ADDREF); \
    (++(pkt)->ref_count)
#define IPCOM_PKT_DEL_REF(pkt)              (--(pkt)->ref_count)
# ifdef IP_PORT_LKM
#  define IPCOM_PKT_HAS_MULTIPLE_OWNERS(pkt) ((pkt)->ref_count > 1 || ipcom_lkm_skb_get_ref_count(pkt) > 1)
# else
#  define IPCOM_PKT_HAS_MULTIPLE_OWNERS(pkt) ((pkt)->ref_count > 1)
# endif
#endif /* IPCOM_USE_SMP */

#define IPCOM_PKT_RESET_CHECKSUM(pkt)   \
        do { \
          IP_BIT_CLR((pkt)->flags, IPCOM_PKT_FLAG_HW_CHECKSUM|IPCOM_PKT_FLAG_TL_CHECKSUM); \
          (pkt)->chk = 0; \
        } while (0)

/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

IP_PUBLIC int
ipcom_pkt_get_length(Ipcom_pkt *pkt);

IP_PUBLIC void *
ipcom_pkt_get_data(Ipcom_pkt *pkt, int offset);

IP_PUBLIC Ipcom_pkt *
ipcom_pkt_make_linear(Ipcom_pkt *pkt);


/******************************************************************************
*
* ipcom_pktbuf_free - free packet buffer disassociated with Ipcom_pkt packet
*
* This routine frees a buffer that was previously allocated along with
* an Ipcom_pkt structure using a ipcom_pkt_malloc() call with non-zero
* <size> argument.
*
* The <buf> argument to this routine is the <data> member from the previously
* allocated Ipcom_pkt packet, and the <cookie> argument is the packet's
* <data_freefunc_cookie> member.  After recording the original packet's
* <data> and <data_freefunc_cookie> members, the packet's <data> member
* should be cleared to disassociate it from the packet buffer and prevent a
* double-free of the buffer when the original Ipcom_pkt is freed using
* ipcom_pkt_free().

* This function is intended to support limited cases in which it is
* convenient to allocate both an Ipcom_pkt packet header and a packet buffer
* in an ipcom_pkt_malloc() call, but to separate the parts for different
* uses with different lifetimes.  This kind of separation can increase
* cache efficiency in some network drivers that allocate RX packets out of
* the ipnet packet pool; the disassociated buffer can be put into the device's
* RX ring to receive a packet off the wire at some time in the future,
* while the disassociated and still cache-hot Ipcom_pkt can be reassociated
* with a filled buffer that holds a received packet for immediate delivery to
* the stack.
*
* Parameters:
* \is
* \i <buf>
* A pointer to the disassociated buffer to free; the <data> member
* of the formerly associated Ipcom_pkt with which the buffer was allocated.
* \i <cookie>
* The <data_freefunc_cookie> member of the formerly associated packet.
* \ie
*
* RETURNS: \&N/A.
*
* ERRNO:
*/
IP_PUBLIC void ipcom_pktbuf_free
(
    Ip_u8 *buf,
    void * cookie
);


#ifdef IP_DEBUG
/*
 *===========================================================================
 *                    ipcom_pkt_dfree
 *===========================================================================
 * Description: Frees a packet that was allocated with ipcom_pkt_malloc().
 * Parameters:  pkt - The packet to free.
 *              file - The file the packet was freed in.
 *              line - The line the packet was freed at.
 * Returns:
 *
 */
IP_PUBLIC void ipcom_pkt_dfree(Ipcom_pkt *pkt, char *file, int line);
#define ipcom_pkt_free(pkt) ipcom_pkt_dfree(pkt, __FILE__, __LINE__)


/*
 *===========================================================================
 *                    ipcom_pkt_dmalloc
 *===========================================================================
 * Description: Allocates the requested amount of MTU data. MTU data is
 *              defined as IP + transport + application (not link header).
 * Parameters:  size  - The size of the data MTU in bytes.
 *              fcflags - function call flags (see ipcom_cstyle.h)
 *              file - The file the packet was allocated in.
 *              line - The line the packet was allocated at.
 * Returns:     A IPCOM packet.
 *
 */
IP_PUBLIC Ipcom_pkt *ipcom_pkt_dmalloc(Ip_size_t size, int fcflags, char *file, int line);
#define ipcom_pkt_malloc(size, fcflags) ipcom_pkt_dmalloc(size, fcflags, __FILE__, __LINE__)

#else /* ifndef IP_DEBUG */

/******************************************************************************
*
* ipcom_pkt_free - free memory used by the 'Ipcom_pkt' structure
*
* This routine frees the memory used by the 'Ipcom_pkt' structure.  If the
* packet's data_freefunc() function is non-NULL, it is called to free the
* data buffer attached to the packet.  Otherwise, if the packet has an
* attached data buffer, the buffer is assumed to have been allocated as part
* of ipcom_pkt_malloc(), and is freed in the default way.
*
* Parameters:
* \is
* \i <pkt>
* A pointer to the packet to free.
* \ie
*
* RETURNS: \&N/A.
*
* ERRNO:
*/
IP_FASTTEXT IP_PUBLIC void ipcom_pkt_free
(
    Ipcom_pkt *pkt
);


/******************************************************************************
*
* ipcom_pkt_malloc - allocate memory for 'Ipcom_pkt' structures
*
* This routine is used to allocate memory for 'Ipcom_pkt' structures. The
* <size> parameter specifies the number of octets to allocate for the IP
* header, transport header and application data. The TCP/IP stack adds space
* for the link-layer header. This means that the full size of the data buffer
* will be greater than <size>.  If <size> is zero, the call allocates only
* an Ipcom_pkt structure with no data buffer attached.
*
* The total amount of memory allocated by ipcom_pkt_malloc() can never exceed
* the predefined TCP/IP stack packet buffer pool <maxsize>.
*
* The <fcflags> parameter must be set based on blocking mode and the context of
* the process that calls ipcom_pkt_malloc(). Available flags are:
* \is
* \i 'IP_FLAG_FC_BLOCKOK'
* Ok for function call to block (used when default is non-blocking).
* \i 'IP_FLAG_FC_NOBLOCK'
* Function call may not block (used when default is blocking).
* \ie
*
* Parameters:
* \is
* \i <size>
* Either zero, in which case no data buffer is allocated; or else the size in
* bytes of the portion of the packet data buffer reserved for the IP datagram
* proper, not counting the link header: the MTU.
* \i <fcflags>
* Function call flags.
* \ie
*
* RETURNS: A pointer to a newly allocated packet structure, or 'IP_NULL', if no
* buffer was available.
*
* ERRNO:
*/
IP_FASTTEXT IP_PUBLIC Ipcom_pkt *ipcom_pkt_malloc
(
    Ip_size_t size,
    int fcflags
);

#endif /* IP_DEBUG */


/*******************************************************************************
*
* ipcom_pkt_create - create a valid 'ipcom_pkt' structure from user allocated
* memory
*
* This routine creates a valid 'ipcom_pkt' structure from user allocated memory.
* This routine can be used to avoid doing memory copy of data from a device
* driver buffer into an 'ipcom_pkt' structure.
*
* Parameters:
* \is
* \i <pkt>
* Pointer to a buffer which is large enough to hold an 'ipcom_pkt' structure.
* \i <data>
* Pointer to the buffer which contains the packet <data>.
* \i <data_len>
* The length of the data buffer.
* \i <freefunc>
* A routine that is called when the stack is finished using the packet.
* \i <cookie>
* User-defined data that is passed as the second argument to <freefunc>.
* \ie
*
* RETURNS: \&N/A.
*
* ERRNO:
*/
IP_FASTTEXT IP_PUBLIC void ipcom_pkt_create
(
    Ipcom_pkt *pkt,
    void *data,
    Ip_size_t data_len,
    Ipcom_pkt_freefunc freefunc,
    void *cookie
);


/*******************************************************************************
*
* ipcom_is_stack_task -  check if the current task is used to input packet
*
* This routine is used to check if the current task is used to input packets
* into the stack
*
* ex: The function will return IP_TRUE when running in the context of the
*     tNetTask on VxWorks or ipnetX on Linux
*
*
* Parameters:
*
* RETURNS: IP_TRUE if running in a OS stack task
*
* ERRNO:
*/
IP_PUBLIC Ip_bool ipcom_is_stack_task
(
    void
);


/*******************************************************************************
*
* ipcom_pkt_input -  input packets when called from a non-interrupt context
*
* This routine is used to input packets when called from a non-interrupt
* context.
*
* 'pkt-\>start' must be the offset of the link-layer header and 'pkt-\>end' must
* be the offset to the end of the link-layer data.
*
* The ipcom_is_stack_task() function must return IP_TRUE when running the the
* context of the OS task that calls this function.
*
* The TCP/IP stack controls the lifetime of the packet when ipcom_pkt_input() is
* called, even if the call fails.
*
* Parameters:
* \is
* \i <netif>
* Interface pointer.
* \i <pkt>
* Packet pointer.
* \ie
*
* RETURNS: \&N/A.
*
* ERRNO:
*/
IP_FASTTEXT IP_PUBLIC void ipcom_pkt_input
(
    struct Ipcom_netif_struct *netif,
    Ipcom_pkt *pkt
);


/*******************************************************************************
*
* ipcom_pkt_input_queue - adds packets to the TCP/IP stack\'s input queue
*
* This routine is used by driver processes that do not want to parse incoming
* packets in their own context, for example, interrupt processes. The routine
* adds packets to the TCP/IP stack\'s packet input queue.
*
* 'pkt-\>start' must be the offset of the link-layer header and 'pkt-\>end' must
* be the offset to the end of the link-layer data.
*
* The stack controls the lifetime of the packet when this function is called,
* even if the call fails.
*
* Parameters:
* \is
* \i <netif>
* Interface pointer.
* \i <pkt>
* Packet pointer.
* \ie
*
* RETURNS: '0' if the packet was successfully processed. A negative ERRNO code
* is returned if an error occurs.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_pkt_input_queue
(
    struct Ipcom_netif_struct *netif,
    Ipcom_pkt *pkt
);


/*******************************************************************************
*
* ipcom_pkt_output_done - Return outgoing packet to the TCP/IP stack
*
* This routine must be called for each IPNET packet that has been sent by the
* driver.
*
* The fcflags parameter is set based on blocking mode and the context of the
* process that calls ipcom_pkt_output_done(). Available flags are:
* \is
* \i 'IP_FLAG_FC_BLOCKOK'
* Ok for function call to block (used when default is non-blocking).
* \i 'IP_FLAG_FC_NOBLOCK'
* Function call may not block (used when default is blocking.
* \i 'IP_FLAG_FC_ISRCONTEXT'
* The function is called from ISR (interrupt context).
* \i 'IP_FLAG_FC_STACKCONTEXT'
* The function is called from the stack context and code-locked.
* \ie
*
* ipcom_pkt_output_done() calls any callback that the application has registered
* to IPNET through ipnet_setsockopt().
*
* IPNET takes care of freeing the memory allocated by the packet, even if the
* routine returns an error.
*
* An important secondary task of ipcom_pkt_output_done() is to wake up processes
* waiting to output more packets on the interface. Therefore, it is valid to
* call ipcom_pkt_output_done() with 'pkt ==' 'IP_NULL' just to trigger more
* output (if' -IP_ERRNO_EWOULDBLOCK' was previously returned by
* ipnet_drv_output().
*
* Parameters:
* \is
* \i <netif>
* Interface pointer.
* \i <pkt>
* Packet pointer.
* \i <fcflags>
* Function call flags.
* \ie
*
* RETURNS: '0' if the packet was successfully processed. A negative ERRNO code
* if an error occurs.
*
* ERRNO:
*/
IP_FASTTEXT IP_PUBLIC int ipcom_pkt_output_done
(
    struct Ipcom_netif_struct *netif,
    Ipcom_pkt *pkt,
    int fcflags
);


/*
 *===========================================================================
 *                    Packet information
 *===========================================================================
 *
 */
IP_PUBLIC void *ipcom_pkt_alloc_info(Ipcom_pkt *pkt,
                                     Ip_u8 info_type,
                                     Ip_size_t info_size);
IP_PUBLIC void ipcom_pkt_set_info(Ipcom_pkt *pkt,
                                  Ip_u8 info_type,
                                  Ip_size_t info_size,
                                  void *new_info);
IP_PUBLIC void *ipcom_pkt_get_info(IP_CONST Ipcom_pkt *pkt,
                                   Ip_u8 info_type);
IP_PUBLIC void *ipcom_pkt_get_info_safe(IP_CONST Ipcom_pkt *pkt,
                                        Ip_u8 info_type);
IP_PUBLIC void ipcom_pkt_copy_info(Ipcom_pkt *dst,
                                   IP_CONST Ipcom_pkt *src);

#define ipcom_pkt_set_int(pkt, info_type, i, int_type) \
    do { \
        int_type i_ =  i; \
        ipcom_pkt_set_info(pkt, info_type, sizeof(int_type), &i_); \
    } while (0)

#define ipcom_pkt_get_int(pkt, info_type, int_type) \
    (*(int_type*)ipcom_pkt_get_info_safe(pkt, info_type))


/*
 *===========================================================================
 *                    USER/KERNEL copy
 *===========================================================================
 *
 */
#ifdef IP_PORT_LKM
IP_PUBLIC void *ipcom_copy_to_user(void *s1, const void *s2, Ip_size_t n);
IP_PUBLIC void *ipcom_copy_from_user(void *s1, const void *s2, Ip_size_t n);
#else
#define ipcom_copy_to_user(s1,s2,n)    ipcom_memcpy(s1,s2,n)
#define ipcom_copy_from_user(s1,s2,n)  ipcom_memcpy(s1,s2,n)
#endif
/*
 *===========================================================================
 *                    SKB reference counter
 *===========================================================================
 *
 */
#ifdef IP_PORT_LKM
IP_PUBLIC int ipcom_lkm_skb_get_ref_count(Ipcom_pkt *pkt);
#endif

/*
 ****************************************************************************
 * 8                    VARIABLES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                        ipcom_port_pkt_hdr_extra_size
 *===========================================================================
 * The amount of extra space reserved for port-specific usage at the end of
 * each Ipcom_pkt structure.
 */
IP_EXTERN Ip_size_t ipcom_port_pkt_hdr_extra_size;


/*
 *===========================================================================
 *                    ipcom_conf_pkt_pool_buffer_alignment
 *===========================================================================
 * The minimal packet data buffer alignment for packets allocated using
 * ipcom_pkt_malloc(), as required by the ipcom port.  Only has effect if
 * set before packet pool creation.  The actual alignment guarantee will be
 * the maximum of this variable and the alignment imposed by the packet
 * pool implementation. For example, the IPNET packet pool imposes at least
 * cache-line alignment for packet buffers.
 *
 * The value must be a power of two, or zero.  Defaults to zero if not
 * modified by the port.
 */
IP_EXTERN Ip_size_t ipcom_conf_pkt_pool_buffer_alignment;


/*
 *===========================================================================
 *                    ipcom_conf_pkt_pool_buffer_extra_space
 *===========================================================================
 * The minimal additional space in packet buffers beyond the nominal size
 * (MTU) requested in ipcom_pkt_malloc(), as required by the ipcom port.
 * Only has effect if set before packet pool creation.  The actual additional
 * space will be the maximum of this variable and the extra space provided
 * by the pool implementation. For example, the IPNET packet pool provides
 * additional space in each packet data buffer of at least the calculated
 * maximum link header space plus a constant (64 bytes at present).
 *
 * This variable defaults to zero if not modifed by the port.
 */
IP_EXTERN Ip_size_t ipcom_conf_pkt_pool_buffer_extra_space;

#ifdef __cplusplus
}
#endif

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
