/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_pcap.c,v $ $Revision: 1.3 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_pcap.c,v $
 *     $Author: jhorteli $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Writes all received and sent packets on a network interface to a pcap
 * compatible file. The created file can be loaded into Wireshark, which
 * in turn allows for analysis of the stream(s) send and received on the
 * monitored network interface.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipnet_config.h"

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef IPNET_USE_PCAP

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_file.h>
#include <ipcom_hash.h>
#include <ipcom_pkt.h>
#include <ipcom_syslog.h>
#include <ipcom_time.h>

#include "ipnet_pcap.h"
#include "ipnet_h.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_PCAP_MAGIC              0xa1b2c3d4
#define IPNET_PCAP_MAGIC_SWAPPED      0xd4c3b2a1
#define IPNET_PCAP_VERSION_MAJOR      2
#define IPNET_PCAP_VERSION_MINOR      4
#define IPNET_PCAP_THISZONE           0
#define IPNET_PCAP_SIGFIGS            0
#define IPNET_PCAP_SNAPLEN            0xffff

#define IPNET_PCAP_NETWORK_80211      105
#define IPNET_PCAP_NETWORK_DEFAULT    101
#define IPNET_PCAP_NETWORK_ETHERNET   1
#define IPNET_PCAP_NETWORK_LOOP       101
#define IPNET_PCAP_NETWORK_PPP        9

/*
 ****************************************************************************
 * 5
TYPES
 ****************************************************************************
 */

/* Global header for capture file */
typedef struct Ipnet_pcap_hdr_struct
{
    Ip_u32 magic_number;   /* magic number,
                              0xa1b2c3d4 -> all fields in correct byteorder
                              0xd4c3b2a1 -> all fields must be swapped */
    Ip_u16 version_major;  /* major version number */
    Ip_u16 version_minor;  /* minor version number */
    Ip_s32 thiszone;       /* GMT to local correction */
    Ip_u32 sigfigs;        /* accuracy of timestamps */
    Ip_u32 snaplen;        /* max length of captured packets, in octets */
    Ip_u32 network;        /* data link type, IPNET_PCAP_NETWORK_xxx */
}
Ipnet_pcap_hdr_t;

/* Header for every packet record */
typedef struct Ipnet_pcaprec_hdr_struct {
    Ip_u32 ts_sec;         /* timestamp seconds */
    Ip_u32 ts_usec;        /* timestamp microseconds */
    Ip_u32 incl_len;       /* number of octets of packet saved in file */
    Ip_u32 orig_len;       /* actual length of packet */
}
Ipnet_pcaprec_hdr_t;


/* Session stored in a file on the local filesystem */
typedef struct Ipnet_pcap_session_file_struct
{
    char name[IPNET_PCAP_FNAMSIZ];   /* filename of the capture file */
}
Ipnet_pcap_session_file_t;


/* Session sent to a specific address & port */
typedef struct Ipnet_pcap_session_net_struct
{
    int                     socket;
    union Ip_sockaddr_union dst;
}
Ipnet_pcap_session_net_t;

struct Ipnet_pcap_session_struct;

typedef void (*Ipnet_pcap_add_record)(struct Ipnet_pcap_session_struct *session,
                                      Ipcom_pkt *pkt);
typedef void (*Ipnet_pcap_dtor)(struct Ipnet_pcap_session_struct *session);

typedef struct Ipnet_pcap_session_struct
{
    Ip_u32                ifindex;    /* monitored network interface */
    Ipnet_link_input      link_input; /* original link_input handler for 'netif' */
    Ipcom_drv_output      drv_output; /* original drv_output handler for 'netif' */
    Ipnet_pcap_dtor       dtor;       /* destructor for the session */
    Ipnet_pcap_add_record add_record; /* session implementation for adding a record  */
    union {
        Ipnet_pcap_session_file_t file;
        Ipnet_pcap_session_net_t  net;
    } d;
}
Ipnet_pcap_session_t;


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

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC Ipcom_hash *ipnet_pcap_sessions = IP_NULL;

/*
 ****************************************************************************
 * 10                   STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                      ipnet_pcap_session_key_func
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_pcap_session_key_func(Ip_u32 *ifindex)
{
    return ipcom_hash_update(ifindex, sizeof(*ifindex), 0);
}


/*
 *===========================================================================
 *                      ipnet_pcap_session_obj_func
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_pcap_session_obj_func(Ipnet_pcap_session_t *session)
{
    return ipnet_pcap_session_key_func(&session->ifindex);
}


/*
 *===========================================================================
 *                      ipnet_pcap_session_cmp_func
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_pcap_session_cmp_func(Ipnet_pcap_session_t *session, Ip_u32 *ifindex)
{
    return session->ifindex == *ifindex;
}


/*
 *===========================================================================
 *                    ipnet_pcap_fill_header
 *===========================================================================
 * Description: Fills the fields of the global pcap header.
 * Parameters:  h - A pcap header buffer.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pcap_fill_header(Ipnet_pcap_hdr_t *h, Ipnet_pcap_session_t *session)
{
    Ipnet_netif *netif = ipnet_if_indextonetif(IPCOM_VR_ANY, session->ifindex);

    h->magic_number  = IPNET_PCAP_MAGIC;
    h->version_major = IPNET_PCAP_VERSION_MAJOR;
    h->version_minor = IPNET_PCAP_VERSION_MINOR;
    h->thiszone      = IPNET_PCAP_THISZONE;
    h->sigfigs       = IPNET_PCAP_SIGFIGS;
    h->snaplen       = IPNET_PCAP_SNAPLEN;
    switch (netif->ipcom.type)
    {
    case IP_IFT_LOOP:
        h->network = IPNET_PCAP_NETWORK_LOOP;
        break;
    case IP_IFT_ETHER:
    case IP_IFT_L2VLAN:
        h->network = IPNET_PCAP_NETWORK_ETHERNET;
        break;
    case IP_IFT_IEEE80211:
        h->network = IPNET_PCAP_NETWORK_80211;
        break;
    case IP_IFT_PPP:
        h->network = IPNET_PCAP_NETWORK_PPP;
        break;
    default:
        h->network = IPNET_PCAP_NETWORK_DEFAULT;
        break;
    }

}


/*
 *===========================================================================
 *                    ipnet_pcap_new_record
 *===========================================================================
 * Description: Creates a new pcap record for a packet, the record must be
 *              freed with ipcom_free() by the caller.
 * Parameters:  pkt - The packet to add a record for.
 * Returns:     The new record or IP_NULL if out of memory.
 *
 */
IP_STATIC Ipnet_pcaprec_hdr_t *
ipnet_pcap_new_record(Ipcom_pkt *pkt)
{
    Ipnet_pcaprec_hdr_t *rec;
    int                  pkt_len = pkt->end - pkt->start;

    rec = ipcom_malloc(sizeof(*rec) + pkt_len);
    if (rec != IP_NULL)
    {
        struct Ip_timeval tv;

        ipcom_gettimeofday(&tv, IP_NULL);
        rec->ts_sec   = tv.tv_sec;
        rec->ts_usec  = tv.tv_usec;
        rec->incl_len = (Ip_u32) pkt_len;
        rec->orig_len = (Ip_u32) pkt_len;
        ipcom_memcpy(rec + 1, &pkt->data[pkt->start], pkt_len);
    }

    return rec;
}


/*
 *===========================================================================
 *                    ipnet_pcap_file_add_record
 *===========================================================================
 * Description: Adds a pcap record by appending it to the file associated
 *              with the session.
 * Parameters:  session - A pcap file session.
 *              pkt - The packet to add a record for.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pcap_file_add_record(Ipnet_pcap_session_t *session, Ipcom_pkt *pkt)
{
    IP_FILE             *f;
    Ipnet_pcaprec_hdr_t *rec;

    if (IP_NULL != (f = ipcom_fopen(session->d.file.name, "a")))
    {
        if (IP_NULL != (rec = ipnet_pcap_new_record(pkt)))
        {
            (void)ipcom_fwrite(rec, sizeof(*rec) + rec->incl_len, 1, f);
            ipcom_free(rec);
        }
        ipcom_fclose(f);
    }
}


/*
 *===========================================================================
 *                    ipnet_pcap_add_record
 *===========================================================================
 * Description: Adds a packet to the pcap backend storage.
 * Parameters:  netif - The network interface the packet was capture on.
 *              pkt - The packet to capture.
 * Returns:
 *
 */
IP_STATIC Ipnet_pcap_session_t *
ipnet_pcap_add_record(Ipcom_netif   *netif, 
                      Ipcom_pkt     *pkt, 
                      int           pcap_dir, 
                      Ip_u16        frame_type,
                      int           mac,
                      int           payload)
{
    Ipnet_pcap_session_t *session;

    (void) pcap_dir;
    (void) frame_type;
    (void) mac;
    (void) payload;

    session = ipcom_hash_get(ipnet_pcap_sessions, &netif->ifindex);
    ip_assert(session != IP_NULL);
    session->add_record(session, pkt);
    return session;
}


/*
 *===========================================================================
 *                    ipnet_pcap_link_input
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:     0 = success
 *             <0 = error code
 *
 */
IP_STATIC int
ipnet_pcap_link_input(Ipnet_netif *netif, Ipcom_pkt *pkt)
{
    return ipnet_pcap_add_record(&netif->ipcom, pkt, IP_PACKET_HOST, 0, -1, -1)->link_input(netif, pkt);
}


/*
 *===========================================================================
 *                    ipnet_pcap_drv_output
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:     0 = success
 *             <0 = error code
 *
 */
IP_STATIC int
ipnet_pcap_drv_output(Ipcom_netif *netif, Ipcom_pkt *pkt)
{
    return ipnet_pcap_add_record(netif, pkt, IP_PACKET_OUTGOING, 0, -1, -1)->drv_output(netif, pkt);
}


/*
 *===========================================================================
 *                    ipnet_pcap_file_init
 *===========================================================================
 * Description: Adds a pcap record by appending it to the file associated
 *              with the session.
 * Parameters:  session - A pcap file session.
 *              pkt - The packet to add a record for.
 * Returns:     0 = success
 *             <0 = error code
 *
 */
IP_STATIC int
ipnet_pcap_file_init(Ipnet_pcap_session_t *session, const char *filename)
{
    Ipnet_pcap_hdr_t hdr;
    IP_FILE         *f;

    session->add_record  = ipnet_pcap_file_add_record;
    session->dtor        = IP_NULL;
    ipcom_strncpy(session->d.file.name, filename, IPNET_PCAP_FNAMSIZ);

    f = ipcom_fopen(filename, "w");
    if (f == IP_NULL)
    {
        IPCOM_LOG2(ERR, "ipnet_pcap_file_init: failed to create file %s: %s",
                   filename,
                   ipcom_strerror(ipcom_errno));
        return -ipcom_errno;
    }

    ipnet_pcap_fill_header(&hdr, session);
    (void)ipcom_fwrite(&hdr, sizeof(hdr), 1, f);

    ipcom_fclose(f);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_pcap_net_init
 *===========================================================================
 * Description: Adds a pcap record by appending it to the file associated
 *              with the session.
 * Parameters:  session - A pcap file session.
 *              pkt - The packet to add a record for.
 * Returns:     0 = success
 *             <0 = error code
 *
 */
IP_STATIC int
ipnet_pcap_net_init(Ipnet_pcap_session_t *session, union Ip_sockaddr_union *dst)
{
    (void) session;
    (void) dst;
    /* NOT IMPLEMENTED YET */
    IP_PANIC();
    return 0;
}


/*
 ****************************************************************************
 * 11                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_pcap_ioctl
 *===========================================================================
 * Description:
 * Parameters:  session - A pcap file session.
 *              pkt - The packet to add a record for.
 * Returns:     0 = success
               <0 = error code
 *
 */
IP_GLOBAL int
ipnet_pcap_ioctl(Ipnet_netif *netif, Ip_u32 command, void *data)
{
    struct Ip_ifreq      *ifreq = data;
    Ipnet_pcap_ioctl_t   *pcap = ifreq->ip_ifr_data;
    Ipnet_pcap_session_t *session;
    int                   ret = -IP_ERRNO_EINVAL;

    (void) command;
    ip_assert(command == IP_SIOCXPCAP);

    if (pcap == IP_NULL)
        return -IP_ERRNO_EINVAL;

    if (ipnet_pcap_sessions == IP_NULL)
    {
        ipnet_pcap_sessions
            = ipcom_hash_new((Ipcom_hash_obj_func) ipnet_pcap_session_key_func,
                             (Ipcom_hash_key_func) ipnet_pcap_session_obj_func,
                             (Ipcom_hash_cmp_func) ipnet_pcap_session_cmp_func);
    }

    session = ipcom_hash_get(ipnet_pcap_sessions, &netif->ipcom.ifindex);

    switch (pcap->op)
    {
    case IPNET_PCAP_OP_START:
        /* Enable capture */
        if (session != IP_NULL)
            return -IP_ERRNO_EEXIST;

        session = ipcom_calloc(1, sizeof(*session));
        if (session == IP_NULL)
            return -IP_ERRNO_ENOMEM;

        session->ifindex = netif->ipcom.ifindex;
        if (ipcom_hash_add(ipnet_pcap_sessions, session) != IPCOM_SUCCESS)
        {
            ipcom_free(session);
            return -IP_ERRNO_ENOMEM;
        }

        switch (pcap->type)
        {
        case IPNET_PCAP_TYPE_FILE:
            ret = ipnet_pcap_file_init(session, pcap->d.filename);
            break;
        case IPNET_PCAP_TYPE_NET:
            ret = ipnet_pcap_net_init(session, &pcap->d.dst);
            break;
        default:
            break;
        }

        if (ret != 0)
        {
            (void)ipcom_hash_remove(ipnet_pcap_sessions, session);
            ipcom_free(session);
        }
        else
        {
            session->link_input = netif->link_input;
            session->drv_output = netif->ipcom.drv_output;

            switch (netif->ipcom.type)
            {
            case IP_IFT_LOOP:
                netif->ipcom.drv_output = ipnet_pcap_drv_output;
                break;
            case IP_IFT_L2VLAN:
            case IP_IFT_TUNNEL:
                netif->ipcom.link_tap = (Ipcom_link_tap) ipnet_pcap_add_record;
                break;
            default:
                netif->link_input       = ipnet_pcap_link_input;
                netif->ipcom.drv_output = ipnet_pcap_drv_output;
                break;
            }
        }
        break;

    case IPNET_PCAP_OP_STOP:
        /* Disable capture */
        if (session == IP_NULL)
            return -IP_ERRNO_ENOENT;

        if (ipcom_hash_remove(ipnet_pcap_sessions, session) != IPCOM_SUCCESS)
            IP_PANIC();
        if (session->dtor != IP_NULL)
            session->dtor(session);

        netif->link_input       = session->link_input;
        netif->ipcom.drv_output = session->drv_output;
        netif->ipcom.link_tap   = IP_NULL;
        ipcom_free(session);
        ret = 0;
        break;

    default:
        break;
    }

    return ret;
}

#endif /* IPNET_USE_PCAP */
