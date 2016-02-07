/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_vrrpd.c,v $ $Revision: 1.24 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_vrrpd.c,v $
 *   $Author: n $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Implementation of RFC3768 - Virtual Router Redundancy Protocol (VRRP)
 * This is the VRRP daemon.
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

#define IPNET_VRRP_BUF_SIZE 1000


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO

#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_ipd.h>
#include <ipcom_os.h>
#include <ipcom_sysctl.h>
#include <ipcom_syslog.h>
#include <ipcom_sysvar.h>
#include <ipcom_tmo2.h>
#include <ipcom_type.h>

#if defined(IPNET_USE_VRRP) && defined(IPCOM_USE_INET)

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_routesock.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_VRRPD_IPD_NAME "ipnet_vrrpd"

#define IPNET_VRRPD_CFG_DELIMITER " ,"

#define IPNET_VRRPD_PROTO       112
#define IPNET_VRRPD_IPV4_ADDR  "224.0.0.18"

/* VRRP states */
#define IPNET_VRRPD_STATE_UNINITIALIZED  0
#define IPNET_VRRPD_STATE_MASTER         1
#define IPNET_VRRPD_STATE_BACKUP         2

/* Current VRRP protcol version */
#define IPNET_VRRP_VERSION   2

/* VRRP message types */
#define IPNET_VRRP_TYPE_ADV  1

/* Authentication mechanisms */
#define IPNET_VRRP_AUTH_TYPE_0 0
#define IPNET_VRRP_AUTH_TYPE_1 1  /* simple text password rfc2338#section-5.3.6 */

#define IPNET_VRRP_AUTH_DATA_SIZE 8

#define IPNET_VRRP_MSG_GET_VERSION(ver_type) ((ver_type) >> 4)
#define IPNET_VRRP_MSG_GET_TYPE(ver_type) ((ver_type) & 0x0f)

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
  VRRP Packet Format

   This section defines the format of the VRRP packet and the relevant
   fields in the IP header.

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |Version| Type  | Virtual Rtr ID|   Priority    | Count IP Addrs|
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |   Auth Type   |   Adver Int   |          Checksum             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                         IP Address (1)                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                            .                                  |
   |                            .                                  |
   |                            .                                  |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                         IP Address (n)                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                     Authentication Data (1)                   |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                     Authentication Data (2)                   |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
#include "ipcom_align16.h"
typedef IP_PACKED1 struct Ipnet_vrrp_pkt_struct
{
    Ip_u8  ver_type;
    Ip_u8  vrid;
    Ip_u8  priority;
    Ip_u8  count_ip_addrs;
    Ip_u8  auth_type;
    Ip_u8  adver_int;
    Ip_u16 checksum;
    struct Ip_in_addr addrs[1];
    /* Authentication data follows */
}
IP_PACKED2 Ipnet_vrrp_pkt_t;
#include "ipcom_align16.h"


/* Search key for the 'conf_head' hash table */
typedef struct Ipnet_vrrpd_key_struct
{
    char        ifname[IP_IFNAMSIZ]; /* Interface name */
    Ip_u8       vrid;                /* VRID 1-255 */
}
Ipnet_vrrpd_key_t;


typedef struct Ipnet_vrrpd_struct
{
    int           ipd;
    int           sock;
    Ipcom_hash   *conf_head;
    Ipcom_pqueue *tmo_head;
    char          buf[IPNET_VRRP_BUF_SIZE];
}
Ipnet_vrrpd_t;

/* Forward declaration */
struct Ipnet_vrrpd_conf_struct;

typedef void (*Ipnet_vrrpd_get_virtual_link_addr)
     (struct Ipnet_vrrpd_conf_struct *conf, struct Ip_sockaddr_dl *dl);


/* Configuration parameters for one interface/VRID pair */
typedef struct Ipnet_vrrpd_conf_struct
{
    Ipnet_vrrpd_t *v;                /* The virtual router daemon this entry is part of */
    char        ifname[IP_IFNAMSIZ]; /* Interface name */
    Ip_u8       vrid;                /* VRID 1-255 */
    Ip_u8       priority;            /* Priority 1-254 or 255 if this host owns the address */
    Ip_u8       num_addresses;       /* Number of elements in the 'addrs' array */
    Ip_u8       state;               /* The current state (one for the IPNET_VRRPD_STATE_xxx) */
    Ip_u8       adv_interval_sec;    /* Interval between advertisements when running as master */
    Ipcom_tmo2  tmo;                 /* Handle to the current running timeout */
    Ip_bool     preempt;             /* IP_TRUE if we should go to backup state when a higher prio is seen */
    Ip_bool     promisc;             /* Promiscuous mode has been entered due to this virtual ID */
    Ipnet_vrrpd_get_virtual_link_addr get_virtual_link_addr;
    int         auth_type;
    char        auth_data[IPNET_VRRP_AUTH_DATA_SIZE];
    struct Ip_in_addr addrs[1];      /* Array of addresses associated with this interface/VRID */
}
Ipnet_vrrpd_conf_t;


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

IPCOM_PROCESS(ipnet_vrrpd_proc);

#endif /* IPNET_USE_VRRP && IPCOM_USE_INET */

/*
 *===========================================================================
 *                    ipnet_vrrpd_create
 *===========================================================================
 * Description: IPD hook to allocate global resources for the VRRP daemon.
 * Parameters:
 * Returns:     IPCOM_SUCCESS if the operation was successful.
 *
 */
IP_PUBLIC Ip_err ipnet_vrrpd_create(void);


/*
 *===========================================================================
 *                    ipnet_vrrpd_start
 *===========================================================================
 * Description: IPD hook to start the VRRP daemon.
 * Parameters:
 * Returns:     IPCOM_SUCCESS if the operation was successful.
 *
 */
IP_PUBLIC Ip_err ipnet_vrrpd_start(void);


/*
 *===========================================================================
 *                    ipnet_vrrpd_version
 *===========================================================================
 * Description: Returns a version string for the VRRP daemon.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC const char * ipnet_vrrpd_version(void);


#if defined(IPNET_USE_VRRP) && defined(IPCOM_USE_INET)

IP_STATIC Ip_err ipnet_vrrpd_set_state(Ipnet_vrrpd_conf_t *conf, Ip_u8 state);

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC Ipnet_vrrpd_t ipnet_vrrpd;

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_vrrpd_obj_hash
 *===========================================================================
 * Description: Computes a hash value for an interface configuration entry.
 * Parameters:  conf - A vrrp configuration entry.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_vrrpd_obj_hash(Ipnet_vrrpd_conf_t *conf)
{
    unsigned v;

    v =  ipcom_hash_update(conf->ifname, ipcom_strlen(conf->ifname), 0);
    return ipcom_hash_update(&conf->vrid, sizeof(conf->vrid), v);
}


/*
 *===========================================================================
 *                    ipnet_vrrpd_key_hash
 *===========================================================================
 * Description: Computes a hash value an interface name.
 * Parameters:  key - A vrrpd search key.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_vrrpd_key_hash(Ipnet_vrrpd_key_t *key)
{
    unsigned v;

    v =  ipcom_hash_update(key->ifname, ipcom_strlen(key->ifname), 0);
    return ipcom_hash_update(&key->vrid, sizeof(key->vrid), v);
}


/*
 *===========================================================================
 *                    ipnet_vrrpd_hash_cmp
 *===========================================================================
 * Description: Compares the name of interface configuration interface and
 *              the given interface name
 * Parameters:  conf - VRRP configuration entry.
 *              ifname - An interface name.
 * Returns:     IP_TRUE if the key matches the configuration entry.
 *
 */
IP_STATIC Ip_bool
ipnet_vrrpd_hash_cmp(Ipnet_vrrpd_conf_t *conf, Ipnet_vrrpd_key_t *key)
{
    return conf->vrid == key->vrid
        && ipcom_strcmp(conf->ifname, key->ifname) == 0;
}


/*
 *===========================================================================
 *                    ipnet_vrrpd_get_virtual_ethernet_addr
 *===========================================================================
 * Description: Returns the virtual MAC that matches the virtual router entry
 * Parameters:  conf - A virtual router entry.
 *              dl - Will contain the MAC address.
 * Returns:
 *
 */
IP_STATIC void
ipnet_vrrpd_get_virtual_ethernet_addr(Ipnet_vrrpd_conf_t *conf, struct Ip_sockaddr_dl *dl)
{
    ipcom_memset(dl, 0, sizeof(*dl));

    IPCOM_SA_LEN_SET(dl, sizeof(*dl));
    dl->sdl_family  = IP_AF_LINK;
    dl->sdl_index   = (Ip_u16) ipcom_if_nametoindex(conf->ifname);
    dl->sdl_type    = IP_IFT_ETHER;
    dl->sdl_alen    = 6;
    dl->sdl_data[2] = 0x5e;
    dl->sdl_data[4] = 0x01;
    dl->sdl_data[5] = conf->vrid;
}


/*
 *===========================================================================
 *                    ipnet_vrrpd_get_virtual_vlan_addr
 *===========================================================================
 * Description: Returns the virtual MAC that matches the virtual router entry
 * Parameters:  conf - A virtual router entry.
 *              dl - Will contain the MAC address.
 * Returns:
 *
 */
IP_STATIC void
ipnet_vrrpd_get_virtual_vlan_addr(Ipnet_vrrpd_conf_t *conf, struct Ip_sockaddr_dl *dl)
{
    ipnet_vrrpd_get_virtual_ethernet_addr(conf, dl);
    dl->sdl_type = IP_IFT_L2VLAN;
}


/*
 *===========================================================================
 *                    ipnet_vrrpd_get_link_addr_func
 *===========================================================================
 * Description: Returns a function that knows how to map a VRID to the link
 *              layer address used by the specified interface type.
 * Parameters:  type - IP_IFT_xxx type.
 * Returns:
 *
 */
IP_STATIC Ipnet_vrrpd_get_virtual_link_addr
ipnet_vrrpd_get_link_addr_func(Ip_u8 type)
{
    switch (type)
    {
#ifdef _WIN32
        /* Returning the address of a function is buggy in (at least version 6) MSVC */
    case IP_IFT_ETHER:
        return &ipnet_vrrpd_get_virtual_ethernet_addr;
    case IP_IFT_L2VLAN:
        return &ipnet_vrrpd_get_virtual_vlan_addr;
#else
    case IP_IFT_ETHER:
        return ipnet_vrrpd_get_virtual_ethernet_addr;
    case IP_IFT_L2VLAN:
        return ipnet_vrrpd_get_virtual_vlan_addr;
#endif
    default:
        IPCOM_LOG0(ERR, "VRRPD: %d is an unknown interface type");
        return IP_NULL;
    }
}


/*
 *===========================================================================
 *                         ipnet_vrrpd_skew_time
 *===========================================================================
 * Description: Calculates skew time according to RFC3768
 *              Skew_Time       Time to skew Master_Down_Interval in seconds.
 *                              Calculated as:
 *                              ( (256 - Priority) / 256 )
 * Parameters:  conf - VRRP configuration
 * Returns:     Skew time in milliseconds.
 *
 */
IP_STATIC Ip_u32
ipnet_vrrpd_skew_time(Ipnet_vrrpd_conf_t *conf)
{
    return ((1000 * (256 - conf->priority)) / 256);
}


/*
 *===========================================================================
 *                    ipnet_vrrpd_pkt_len
 *===========================================================================
 * Description: Returns the length of a VRRP packet given the number of addresses.
 * Parameters:  num_addresses - The number of addresses in the packet.
 * Returns:
 *
 */
IP_STATIC int
ipnet_vrrpd_pkt_len(Ip_u8 num_addresses)
{
    return sizeof(Ipnet_vrrp_pkt_t)
        + (num_addresses - 1) * sizeof(struct Ip_in_addr)
        + sizeof(Ip_u32) * 2;
}


/*
 *===========================================================================
 *                    ipnet_vrrpd_send_adv
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:     IP_TRUE - i.e. reschedule this timeout
 *
 */
IP_STATIC Ip_bool
ipnet_vrrpd_send_adv(Ipnet_vrrpd_conf_t *conf)
{
    Ipnet_vrrp_pkt_t     *vrrp = (Ipnet_vrrp_pkt_t *) conf->v->buf;
    int                   i;
    struct Ip_msghdr      m;
    struct Ip_iovec       iov;
    struct Ip_sockaddr_in to;
    Ip_u8                 cmsg_buf[IP_CMSG_SPACE(sizeof(struct Ip_in_pktinfo))
                                   + IP_CMSG_SPACE(sizeof(Ip_u8))];
    struct Ip_cmsghdr    *c;
    struct Ip_in_pktinfo *pktinfo;
    char                 *auth_start;

    IPCOM_LOG2(DEBUG2, "VRRPD: Sending advertisment on %s for VRID %d",
               conf->ifname, (int)conf->vrid);

    iov.iov_base = vrrp;
    iov.iov_len  = ipnet_vrrpd_pkt_len(conf->num_addresses);

    auth_start = (char *) vrrp + iov.iov_len - IPNET_VRRP_AUTH_DATA_SIZE;
    ipcom_memcpy(auth_start, conf->auth_data, IPNET_VRRP_AUTH_DATA_SIZE);

    if (IPNET_VRRP_BUF_SIZE < iov.iov_len)
    {
        IPCOM_LOG3(ERR, "VRRPD: message too long for the buffer, increase the value of IPNET_VRRP_BUF_SIZE",
                   conf->ifname, ipcom_strerror(ipcom_errno), ipcom_errno);

    }

    for (i = 0; i < (int) conf->num_addresses; i++)
        vrrp->addrs[i] = conf->addrs[i];

    if (conf->auth_type == 0)
        vrrp->ver_type       = (IPNET_VRRP_VERSION << 4) + IPNET_VRRP_TYPE_ADV;
    else
        vrrp->ver_type       = (1 << 4) + IPNET_VRRP_TYPE_ADV;
    vrrp->vrid           = conf->vrid;
    vrrp->priority       = conf->priority;
    vrrp->count_ip_addrs = conf->num_addresses;
    vrrp->auth_type      = conf->auth_type;
    vrrp->adver_int      = conf->adv_interval_sec;
    vrrp->checksum       = 0;
    vrrp->checksum       = ipcom_in_checksum(vrrp, iov.iov_len);

    m.msg_name       = &to;
    m.msg_namelen    = sizeof(to);
    m.msg_iov        = &iov;
    m.msg_iovlen     = 1;
    m.msg_control    = cmsg_buf;
    m.msg_controllen = sizeof(cmsg_buf);
    m.msg_flags      = 0;

    ipcom_memset(&to, 0, sizeof(to));

    IPCOM_SA_LEN_SET(&to, sizeof(to));
    to.sin_family = IP_AF_INET;
    (void)ipcom_inet_pton(IP_AF_INET, IPNET_VRRPD_IPV4_ADDR, &to.sin_addr);

    /* Specify the outgoing interface */
    c = (struct Ip_cmsghdr *) cmsg_buf;
    c->cmsg_len   = IP_CMSG_LEN(sizeof(struct Ip_in_pktinfo));
    c->cmsg_level = IP_IPPROTO_IP;
    c->cmsg_type  = IP_IP_PKTINFO;
    pktinfo = IP_CMSG_DATA(c);
    ipcom_memset(pktinfo, 0, sizeof(*pktinfo));
    pktinfo->ipi_ifindex = ipcom_if_nametoindex(conf->ifname);

    /* Specify the VRID so that IPNET uses the corresponding VRID
       link layer address as source addr */
    c = (struct Ip_cmsghdr *) (cmsg_buf + IP_CMSG_SPACE(sizeof(struct Ip_in_pktinfo)));
    c->cmsg_len   = IP_CMSG_LEN(sizeof(Ip_u8));
    c->cmsg_level = IP_IPPROTO_IP;
    c->cmsg_type  = IP_IP_X_VRID;
    *(Ip_u8 *) IP_CMSG_DATA(c) = conf->vrid;

    if (ipcom_sendmsg(conf->v->sock, &m, 0) < 0)
    {
        Ip_socklen_t l = sizeof(i);
        IPCOM_LOG2(ERR, "VRRPD: Failed to send advertisement message: %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
        (void)ipcom_getsockopt(conf->v->sock, IP_SOL_SOCKET, IP_SO_ERROR, &i, &l);
    }

    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipnet_vrrpd_become_master
 *===========================================================================
 * Description: Timeout handler that moves the router from backup state
 *              to master state.
 * Parameters:  conf - VRRP configuration entry to become master for.
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_vrrpd_become_master(Ipnet_vrrpd_conf_t *conf)
{
    if (ipnet_vrrpd_set_state(conf, IPNET_VRRPD_STATE_MASTER) != IPCOM_SUCCESS)
        /* Try again later */
        return IP_TRUE;
    return IP_FALSE;
}


/*
 *===========================================================================
 *                      ipnet_vrrpd_state_to_str
 *===========================================================================
 * Description: Returns a string representation of the state.
 * Parameters:  state - one of the IPNET_VRRPD_STATE_xxx constants.
 * Returns:     A string representation.
 *
 */
IP_STATIC IP_CONST char *
ipnet_vrrpd_state_to_str(Ip_u8 state)
{
    switch (state)
    {
    case IPNET_VRRPD_STATE_UNINITIALIZED:
        return "UNINITIALIZED";
    case IPNET_VRRPD_STATE_MASTER:
        return "MASTER";
    case IPNET_VRRPD_STATE_BACKUP:
        return "BACKUP";
    default:
        return "unknown state";
    }
}


/*
 *===========================================================================
 *                    ipnet_vrrpd_set_state
 *===========================================================================
 * Description: Sets the router in master state for the specified interface/VRID.
 * Parameters:  conf - VRRP configuration entry.
 *              state - The state this virtual router should enter.
 * Returns:
 *
 */
IP_STATIC Ip_err
ipnet_vrrpd_set_state(Ipnet_vrrpd_conf_t *conf, Ip_u8 state)
{
    struct Ip_ifreq   ifreq;
    struct Ip_ip_mreq mreq;
    struct Ip_arpreq  arpreq;
    int               i;

    if (conf->state == state)
    {
        if (state == IPNET_VRRPD_STATE_BACKUP)
            return ipcom_tmo2_reset(&conf->tmo);
        else
            return IPCOM_SUCCESS;
    }

    ipcom_memset(&arpreq, 0, sizeof(arpreq));

    IPCOM_SA_LEN_SET(&arpreq.arp_pa, sizeof(arpreq.arp_pa));
    arpreq.arp_pa.sa_family = IP_AF_INET;
    conf->get_virtual_link_addr(conf, &arpreq.arp_ha);

    ipcom_strcpy(ifreq.ifr_name, conf->ifname);
    if (ipcom_socketioctl(conf->v->sock, IP_SIOCGIFADDR, &ifreq) < 0)
    {
        IPCOM_LOG1(ERR, "VRRPD: Failed to get primary address for interface %s", conf->ifname);
        return IPCOM_ERR_FAILED;
    }
    mreq.imr_interface = ((struct Ip_sockaddr_in *) &ifreq.ip_ifr_addr)->sin_addr;
    mreq.imr_multiaddr.s_addr = 0;

    /* This is the state that is beeing exited */
    (void)ipcom_tmo2_cancel(&conf->tmo);
    switch (conf->state)
    {
    case IPNET_VRRPD_STATE_UNINITIALIZED:
        break;

    case IPNET_VRRPD_STATE_MASTER:
        for (i = 0; i < (int) conf->num_addresses; i++)
        {
            ((struct Ip_sockaddr_in*) &arpreq.arp_pa)->sin_addr = conf->addrs[i];
            /* This can fail since the ARP cache might have been manually flushed */
            (void)ipcom_socketioctl(conf->v->sock, IP_SIOCDARP, &arpreq);
        }

        /* Exit promiscuous mode on this interface */
        if (conf->promisc)
            (void)ipcom_setsockopt(conf->v->sock, IP_IPPROTO_IP, IP_IP_LEAVE_GROUP,
                                   &mreq, sizeof(mreq));
        conf->promisc = IP_FALSE;
        break;

    case IPNET_VRRPD_STATE_BACKUP:
        break;

    default:
        IPCOM_LOG1(CRIT, "VRRPD: exiting an invalid state (%d)", conf->state);
        IP_PANIC();
        return IPCOM_ERR_FAILED;
    }

    IPCOM_LOG5(INFO, "VRRPD: exit state %s and enter state %s, VRID %d at %s address %s",
               ipnet_vrrpd_state_to_str(conf->state),
               ipnet_vrrpd_state_to_str(state),
               (int)conf->vrid,
               conf->ifname,
               ipcom_inet_ntop(IP_AF_INET, &conf->addrs[0], conf->v->buf, sizeof(conf->v->buf)));

    /* This is the state beeing entered */
    conf->state = state;
    switch (conf->state)
    {
    case IPNET_VRRPD_STATE_UNINITIALIZED:
        break;

    case IPNET_VRRPD_STATE_MASTER:
        arpreq.arp_flags = IP_ATF_COM | IP_ATF_PERM | IP_ATF_PUBL | IP_ATF_GRAT;
        for (i = 0; i < (int) conf->num_addresses; i++)
        {
            ((struct Ip_sockaddr_in*) &arpreq.arp_pa)->sin_addr = conf->addrs[i];
            if (ipcom_socketioctl(conf->v->sock, IP_SIOCSARP, &arpreq) < 0)
            {
                IPCOM_LOG3(ERR, "VRRPD: IOCTL IP_SIOCSARP failed on %s: %s(%d)",
                           conf->ifname, ipcom_strerror(ipcom_errno), ipcom_errno);
                return IPCOM_ERR_FAILED;
            }
        }

        if (ipcom_tmo2_request(conf->v->tmo_head,
                               &conf->tmo,
                               (Ipcom_tmo2_func) ipnet_vrrpd_send_adv,
                               conf,
                               conf->adv_interval_sec * (950 + (Ip_u32) (100 * ipcom_rand()) / IP_RAND_MAX))
            != IPCOM_SUCCESS)
        {
            IPCOM_LOG0(ERR, "VRRPD: Failed to initialize the VRRP advertisment timer");
            return IPCOM_ERR_FAILED;
        }

        /* Enter promiscuous mode on this interface */
        if (ipcom_setsockopt(conf->v->sock, IP_IPPROTO_IP, IP_IP_JOIN_GROUP,
                             &mreq, sizeof(mreq)) == IP_SOCKERR)
        {
            IPCOM_LOG3(INFO, "VRRPD: Failed to set the interface in promiscuous mode on %s: %s(%d)",
                       conf->ifname, ipcom_strerror(ipcom_errno), ipcom_errno);
            return IPCOM_ERR_FAILED;
        }
        conf->promisc = IP_TRUE;

        /* Send an adv directly so that switches learns that the virtual MAC is
           owned by this host now */
        (void)ipnet_vrrpd_send_adv(conf);

        break;

    case IPNET_VRRPD_STATE_BACKUP:
        /* Schedule a timeout after Master_Down_Interval seconds
           RFC3768:
           Master_Down_Interval  Time interval for Backup to declare Master
                                 down (seconds).  Calculated as:
                                 (3 * Advertisement_Interval) + Skew_time
         */
        if (ipcom_tmo2_request(conf->v->tmo_head,
                               &conf->tmo,
                               (Ipcom_tmo2_func) ipnet_vrrpd_become_master,
                               conf,
                               ((3 * conf->adv_interval_sec * 1000) + ipnet_vrrpd_skew_time(conf)))
            != IPCOM_SUCCESS)
        {
            IPCOM_LOG0(ERR, "VRRPD: Failed to initialize the VRRP advertisment timer");
            return IPCOM_ERR_FAILED;
        }
        break;

    default:
        IPCOM_LOG1(CRIT, "VRRPD: entering an invalid state (%d)", conf->state);
        IP_PANIC();
        return IPCOM_ERR_FAILED;
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_vrrpd_num_addrs
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_vrrpd_num_addrs(IP_CONST char *addrs)
{
    int     num = 0;
    int     i = 0;
    Ip_bool in_delimiter = IP_TRUE;

    while (addrs[i] != '\0')
    {
        if (ipcom_strchr(IPNET_VRRPD_CFG_DELIMITER, addrs[i]))
            in_delimiter = IP_TRUE;
        else if (in_delimiter)
        {
            num++;
            in_delimiter = IP_FALSE;
        }
        i++;
    }
    return num;
}



/*
 *===========================================================================
 *                    ipnet_vrrpd_get_interface_type
 *===========================================================================
 * Description: Returns the type of the specified interface.
 * Parameters:  ifname - A network interface name.
 * Returns:     One of the IP_IFT_xxx constants or 0 in case of error.
 *
 */
IP_STATIC Ip_u8
ipnet_vrrpd_get_interface_type(char *ifname)
{
    int                     name[6];
    struct Ipnet_if_msghdr *ifinfo;
    Ip_size_t               ifinfo_size;
    Ip_u8                   iftype;

    name[0] = IP_CTL_NET;
    name[1] = IP_AF_ROUTE;
    name[2] = 0;
    name[3] = 0;
    name[4] = IP_NET_RT_IFLIST;
    name[5] = ipcom_if_nametoindex(ifname);
    if (ipcom_sysctl(name, 6, IP_NULL, &ifinfo_size, IP_NULL, IP_NULL) < 0)
    {
        IPCOM_LOG3(ERR, "VRRPD: failed to get interface type for %s: %s(%d)",
                   ifname, ipcom_strerror(ipcom_errno), ipcom_errno);
        return 0;
    }
    ifinfo = ipcom_malloc(ifinfo_size);
    if (ifinfo == IP_NULL)
    {
        IPCOM_LOG1(ERR, "VRRPD: failed to allocate %d bytes", ifinfo_size);
        return 0;
    }
    if (ipcom_sysctl(name, 6, ifinfo, &ifinfo_size, IP_NULL, IP_NULL) < 0)
    {
        IPCOM_LOG3(ERR, "VRRPD: failed to get interface type for %s: %s(%d)",
                   ifname, ipcom_strerror(ipcom_errno), ipcom_errno);
        ipcom_free(ifinfo);
        return 0;
    }
    iftype = ifinfo->ifm_data.ifi_type;
    ipcom_free(ifinfo);
    return iftype;
}


/*
 *===========================================================================
 *                    ipnet_vrrpd_add_conf_entry
 *===========================================================================
 * Description: Creates an ifname/VRID entry.
 * Parameters:  v - The configuration for the VRRP daemon.
 *              ifname - The interface to create.
 *              vrid - The virtual ID.
 *              sysvar_basename - The sysvar basename for all configuration.
 * Returns:     IPCOM error code.
 *
 */
IP_STATIC Ip_err
ipnet_vrrpd_add_conf_entry(Ipnet_vrrpd_t *v, char *ifname, Ip_u8 vrid, char *sysvar_basename)
{
    char               *addrs;
    char               *last;
    char               *addr;
    Ipnet_vrrpd_conf_t *conf = IP_NULL;
    char               *sysvar_basename_end;
    struct Ip_in_addr  *in_addr;
    int                 i;

    sysvar_basename_end = sysvar_basename + ipcom_strlen(sysvar_basename);
    ipcom_strcpy(sysvar_basename_end, ".IP_Addresses");
    last = addrs = ipcom_sysvar_get(sysvar_basename, IP_NULL, IP_NULL);
    if (addrs == IP_NULL)
    {
        IPCOM_LOG2(ERR, "VRRPD: No addresses specified for %s and VRID %d",
                   ifname, (int) vrid);
        return IPCOM_ERR_FAILED;
    }

    i = ipnet_vrrpd_num_addrs(addrs);
    if (i == 0)
    {
        IPCOM_LOG2(ERR, "VRRPD: no virtual addresses defined for %s and VRID %d",
                   ifname, (int) vrid);
        goto errout;
    }

    conf = ipcom_calloc(1, sizeof(Ipnet_vrrpd_conf_t) + sizeof(struct Ip_in_addr) * (i - 1));
    if (conf == IP_NULL)
    {
        IPCOM_LOG0(CRIT, "VRRPD: failed to allocate memory");
        goto errout;
    }

    conf->v             = v;
    conf->num_addresses = (Ip_u8) i;
    conf->vrid          = vrid;
    ipcom_strcpy(conf->ifname, ifname);

    ipcom_strcpy(sysvar_basename_end, ".Priority");
    i = ipcom_sysvar_get_as_int(sysvar_basename, 100);
    if (i < 1 || i > 255)
    {
        IPCOM_LOG3(ERR, "VRRPD: %d is an invalid priority, specified for VRID %d on %s",
                   i, (int) vrid, ifname);
        goto errout;
    }
    conf->priority = (Ip_u8) i;

    ipcom_strcpy(sysvar_basename_end, ".Advertisement_Interval");
    i = ipcom_sysvar_get_as_int(sysvar_basename, 1);
    if (i < 1 && i > 255)
    {
        IPCOM_LOG3(ERR, "VRRPD: advertisement interval must be [1..255] seconds, "
                   "%d seconds specified for VRID %d on %s",
                   i, (int) vrid, ifname);
        goto errout;
    }
    conf->adv_interval_sec = (Ip_u8) i;

    ipcom_strcpy(sysvar_basename_end, ".Preempt_Mode");
    if (ipcom_sysvar_get_as_int(sysvar_basename, 1))
        conf->preempt = IP_TRUE;

    ipcom_strcpy(sysvar_basename_end, ".Authentication_Type");
    i = ipcom_sysvar_get_as_int(sysvar_basename, 0);
    if (i != 0 && i != 1)
    {
        IPCOM_LOG3(ERR, "VRRPD: %d is an invalid authentication type, specified for VRID %d on %s",
                   i, (int) vrid, ifname);
        goto errout;
    }
    conf->auth_type = (Ip_u8) i;

    {
        char *auth_data;
        Ip_size_t len;
        ipcom_strcpy(sysvar_basename_end, ".Authentication_Data");
        auth_data = ipcom_sysvar_get(sysvar_basename, IP_NULL, IP_NULL);
        /* default to zeroes */
        ipcom_memset(conf->auth_data, 0, IPNET_VRRP_AUTH_DATA_SIZE);
        if (auth_data != IP_NULL)
        {
            len = ipcom_strlen(auth_data);
            if (len > 0)
            {
                if (len > IPNET_VRRP_AUTH_DATA_SIZE)
                    IPCOM_LOG2(WARNING, "VRRPD: provided %s is too long, truncating to %d.",
                               sysvar_basename,
                               IPNET_VRRP_AUTH_DATA_SIZE);
                ipcom_memcpy(conf->auth_data,
                             auth_data,
                             len > IPNET_VRRP_AUTH_DATA_SIZE ? IPNET_VRRP_AUTH_DATA_SIZE : len);
            }
        }
        ipcom_free(auth_data);
    }


    in_addr = conf->addrs;
    while (IP_NULL != (addr = ipcom_strtok_r(last == addrs ? addrs : IP_NULL,
                                             IPNET_VRRPD_CFG_DELIMITER,
                                             &last)))
    {
        if (ipcom_inet_pton(IP_AF_INET, addr, in_addr++) != 1)
        {
            IPCOM_LOG3(ERR, "VRRPD: %s is not a valid IPv4 address, specified for VRID %d on %s",
                       addr, (int) vrid, ifname);
            goto errout;
        }
    }
    ipcom_free(addrs);
    addrs = IP_NULL;

    conf->get_virtual_link_addr = ipnet_vrrpd_get_link_addr_func(ipnet_vrrpd_get_interface_type(ifname));
    if (conf->get_virtual_link_addr == IP_NULL)
        goto errout;

    if (ipcom_hash_add(v->conf_head, conf) != IPCOM_SUCCESS)
    {
        IPCOM_LOG2(ERR, "VRRPD: Failed to add conf entry for VRID %d on %s",
                   (int) vrid, ifname);
        goto errout;
    }

    if (conf->priority == 255)
        return ipnet_vrrpd_set_state(conf, IPNET_VRRPD_STATE_MASTER);
    return ipnet_vrrpd_set_state(conf, IPNET_VRRPD_STATE_BACKUP);

 errout:
    ipcom_free(conf);
    ipcom_free(addrs);
    return IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    ipnet_vrrpd_add_vrids
 *===========================================================================
 * Description: Create entries for all configure VRID:s on the specified
 *              interface.
 * Parameters:  v - The configuration for the VRRP daemon.
 *              ifname - Name of the interface to configure.
 * Returns:     IPCOM error code.
 *
 */
IP_STATIC Ip_err
ipnet_vrrpd_add_vrids(Ipnet_vrrpd_t *v, char *ifname)
{
    char  *vrids;
    char  *last;
    char  *vrid;
    int    vrid_num;
    Ip_err err = IPCOM_SUCCESS;

    ipcom_strcpy(v->buf, "ipnet.vrrp.");
    ipcom_strcat(v->buf, ifname);
    ipcom_strcat(v->buf, ".vrids");
    last = vrids = ipcom_sysvar_get(v->buf, IP_NULL, IP_NULL);
    if (vrids == IP_NULL)
    {
        IPCOM_LOG1(INFO, "VRRPD: No VRIDS found for interface %s", ifname);
        return IPCOM_SUCCESS;
    }

    while (IP_NULL != (vrid = ipcom_strtok_r(last == vrids ? vrids : IP_NULL,
                                               IPNET_VRRPD_CFG_DELIMITER,
                                               &last)))
    {
        vrid_num = ipcom_atoi(vrid);
        if (vrid_num < 1 || vrid_num > 255)
        {
            IPCOM_LOG1(INFO, "VRRPD: VRID %s is invalid, must be 1-255", vrid_num);
            ipcom_free(vrids);
            return IPCOM_ERR_FAILED;
        }

        ipcom_strcpy(v->buf, "ipnet.vrrp.");
        ipcom_strcat(v->buf, ifname);
        ipcom_strcat(v->buf, ".");
        ipcom_strcat(v->buf, vrid);
        err = ipnet_vrrpd_add_conf_entry(v, ifname, (Ip_u8) vrid_num, v->buf);
        if (err != IPCOM_SUCCESS)
            break;
    }
    ipcom_free(vrids);
    return err;
}


/*
 *===========================================================================
 *                    ipnet_vrrpd_reconfigure
 *===========================================================================
 * Description: Flushes the old configuration and reads the new.
 * Parameters:  v - The configuration for the VRRP daemon.
 * Returns:     IPCOM error code.
 *
 */
IP_STATIC Ip_err
ipnet_vrrpd_reconfigure(Ipnet_vrrpd_t *v)
{
    char  *ifnames;
    char  *last;
    char  *ifname;
    Ip_err err = IPCOM_ERR_NOT_FOUND;

    /* Find all interfaces for which VRRP advertisement should be sent */
    last = ifnames = ipcom_sysvar_get("ipnet.vrrp.interfaces", IP_NULL, IP_NULL);
    if (ifnames == IP_NULL)
    {
        IPCOM_LOG0(INFO, "VRRPD: No interfaces found in ipnet.vrrp.interfaces");
        return IPCOM_SUCCESS;
    }

    while (IP_NULL != (ifname = ipcom_strtok_r(last == ifnames ? ifnames : IP_NULL,
                                               IPNET_VRRPD_CFG_DELIMITER,
                                               &last)))
    {
        struct Ip_ip_mreq mreq;
        struct Ip_ifreq   ifreq;

        ipcom_strcpy(ifreq.ifr_name, ifname);
        if (ipcom_socketioctl(v->sock, IP_SIOCGIFADDR, &ifreq) < 0)
        {
            if (ipcom_if_nametoindex(ifname) == 0)
                IPCOM_LOG1(ERR, "VRRPD: Interface %s does not exists", ifname);
            else
                IPCOM_LOG2(ERR, "VRRPD: Failed to get primary address for interface %s: %s",
                           ifname, ipcom_strerror(ipcom_errno));
            ipcom_free(ifnames);
            return IPCOM_ERR_FAILED;
        }
        mreq.imr_interface = ((struct Ip_sockaddr_in *) &ifreq.ip_ifr_addr)->sin_addr;

        err = ipnet_vrrpd_add_vrids(v, ifname);
        if (err != IPCOM_SUCCESS)
            break;

        /* Join all-vrrp-routers address on this interface */
        (void) ipcom_inet_pton(IP_AF_INET, IPNET_VRRPD_IPV4_ADDR, &mreq.imr_multiaddr);
        if (ipcom_setsockopt(v->sock, IP_IPPROTO_IP, IP_IP_JOIN_GROUP,
                             &mreq, sizeof(mreq)) == IP_SOCKERR)
        {
            IPCOM_LOG4(INFO, "VRRPD: Failed to join the %s group on %s: %s(%d)",
                       IPNET_VRRPD_IPV4_ADDR, ifname, ipcom_strerror(ipcom_errno), ipcom_errno);
            err = IPCOM_ERR_FAILED;
            break;
        }
    }

    ipcom_free(ifnames);
    if (err == IPCOM_ERR_NOT_FOUND)
        IPCOM_LOG0(NOTICE, "VRRPD: Shutting down since no VRRP operations is defined");

    return err;
}


/*
 *===========================================================================
 *                    ipnet_vrrpd_process_ipd_msg
 *===========================================================================
 * Description: Processes received IPD messages.
 * Parameters:  v - The configuration for the VRRP daemon.
 *              shutdown - Set to IP_TRUE if the daemon should shutdown.
 * Returns:     IPCOM_SUCCESS if successful or IPCOM_ERR_FAILED if
 *              reconfigure could not be done.
 */
IP_STATIC Ip_err
ipnet_vrrpd_process_ipd_msg(Ipnet_vrrpd_t *v, Ip_bool *shutdown)
{
    *shutdown = IP_FALSE;

    switch (ipcom_ipd_input(v->ipd))
    {
    case IPCOM_IPD_EVENT_RECONFIGURE:
        IPCOM_LOG0(DEBUG, "VRRPD: Got IPD reconfigure message");
        return ipnet_vrrpd_reconfigure(v);
    case IPCOM_IPD_EVENT_KILL:
        IPCOM_LOG0(DEBUG, "VRRPD: Got IPD kill message");
        *shutdown = IP_TRUE;
        break;
    default:
        /* Just ignore this message */
        break;
    }
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                   ipnet_vrrpd_is_addr_gt_local_addr
 *===========================================================================
 * Description: Returns whether the specified address is numerically greater
 *              than the primary address assigned to "ifname".
 * Parameters:  sock - a socket
 *              ifname - name of the interface to compare primary address to
 *              src - IPv4 address of a non local VRRP node
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_vrrpd_is_addr_gt_local_addr(int sock,
                                  char *ifname,
                                  struct Ip_sockaddr_in *src)
{
    struct Ip_ifreq ifreq;

    ipcom_strcpy(ifreq.ifr_name, ifname);
    if (ipcom_socketioctl(sock, IP_SIOCGIFADDR, &ifreq) < 0)
        /* Could not get the primary address, probably something wrong
           with the configuration, assume the other host has a greater
           numerical value on the address. */
        return IP_TRUE;

    return src->sin_addr.s_addr
        > ((struct Ip_sockaddr_in *) &ifreq.ip_ifr_addr)->sin_addr.s_addr;
}


/*
 *===========================================================================
 *                   ipnet_vrrpd_process_msg_as_master
 *===========================================================================
 * Description: Process an advertisement message that has passed sanity
 *              checks when acting as master
 * Parameters:  conf - this nodes VRRP configuration and state
 *              vrrp - VRRP advertisement message
 *              sender_addr_is_gt - IP_TRUE if the sender of this message
 *                                  has a primary address that is numerically
 *                                  greater than the local primary address
 * Returns:     IPCOM_SUCCESS
 *
 */
IP_STATIC Ip_err
ipnet_vrrpd_process_msg_as_master(Ipnet_vrrpd_conf_t *conf,
                                  Ipnet_vrrp_pkt_t *vrrp,
                                  Ip_bool sender_addr_is_gt)
{
    /* RFC3768, chapter 6.4.3. Master
       ...
       If the Priority in the ADVERTISEMENT is Zero, then:
           o  Send an ADVERTISEMENT
           o  Reset the Adver_Timer to Advertisement_Interval
       else:
           If the Priority in the ADVERTISEMENT is greater than the
           local Priority,
           or
           If the Priority in the ADVERTISEMENT is equal to the local
           Priority and the primary IP Address of the sender is greater
           than the local primary IP Address, then:
               o Cancel Adver_Timer
               o Set Master_Down_Timer to Master_Down_Interval
               o Transition to the {Backup} state
           else:
               o Discard ADVERTISEMENT
           endif
       endif
     */
    if (vrrp->priority == 0)
        return ipnet_vrrpd_set_state(conf, IPNET_VRRPD_STATE_MASTER);

    if (vrrp->priority > conf->priority
        || (vrrp->priority == conf->priority && sender_addr_is_gt))
        return ipnet_vrrpd_set_state(conf, IPNET_VRRPD_STATE_BACKUP);

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                   ipnet_vrrpd_process_msg_as_backup
 *===========================================================================
 * Description: Process an advertisement message that has passed sanity
 *              checks when acting as backup
 * Parameters:  conf - this nodes VRRP configuration and state
 *              vrrp - VRRP advertisement message
 * Returns:     IPCOM_SUCCESS
 *
 */
IP_STATIC Ip_err
ipnet_vrrpd_process_msg_as_backup(Ipnet_vrrpd_conf_t *conf,
                                  Ipnet_vrrp_pkt_t *vrrp)
{
    /* RFC3768, chapter 6.4.2. Backup
       ...
       If the Priority in the ADVERTISEMENT is Zero, then:
           o  Set the Master_Down_Timer to Skew_Time
       else:
           If Preempt_Mode is False, or If the Priority in the
           ADVERTISEMENT is greater than or equal to the local
           Priority, then:
               o Reset the Master_Down_Timer to Master_Down_Interval
           else:
               o Discard the ADVERTISEMENT
           endif
        endif
        ...
    */
    if (vrrp->priority == 0)
    {
        (void)ipcom_tmo2_cancel(&conf->tmo);
        return ipcom_tmo2_request(conf->v->tmo_head,
                                  &conf->tmo,
                                  (Ipcom_tmo2_func) ipnet_vrrpd_become_master,
                                  conf,
                                  ipnet_vrrpd_skew_time(conf));
    }

    if (conf->preempt == IP_FALSE
        || vrrp->priority >= conf->priority)
        return ipnet_vrrpd_set_state(conf, IPNET_VRRPD_STATE_BACKUP);

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_vrrpd_process_msg
 *===========================================================================
 * Description: Processes VRRP messages.
 * Parameters:  v - The configuration for the VRRP daemon.
 * Returns:     IPCOM_SUCCESS
 *
 */
IP_STATIC Ip_err
ipnet_vrrpd_process_msg(Ipnet_vrrpd_t *v)
{
    int                   ver;
    Ipcom_ip_header      *ip;
    int                   ip_len;
    Ipnet_vrrp_pkt_t     *vrrp;
    int                   bytes;
    struct Ip_msghdr      m;
    struct Ip_iovec       iov;
    Ip_u8                 cmsg_buf[IP_CMSG_SPACE(sizeof(struct Ip_in_pktinfo))
                                   + IP_CMSG_SPACE(sizeof(Ip_u8))];
    struct Ip_cmsghdr    *c;
    struct Ip_in_pktinfo *pktinfo;
    Ipnet_vrrpd_key_t     key;
    Ipnet_vrrpd_conf_t   *conf;
    struct Ip_sockaddr_in src;

    iov.iov_base = v->buf;
    iov.iov_len  = sizeof(v->buf);

    m.msg_name       = &src;
    m.msg_namelen    = sizeof(src);
    m.msg_iov        = &iov;
    m.msg_iovlen     = 1;
    m.msg_control    = cmsg_buf;
    m.msg_controllen = sizeof(cmsg_buf);
    m.msg_flags      = 0;

    bytes = ipcom_recvmsg(v->sock, &m, 0);
    if (bytes < 0)
    {
        IPCOM_LOG2(ERR, "VRRPD: Failed read VRRP message %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
        return IPCOM_ERR_FAILED;
    }

    c = IP_CMSG_FIRSTHDR(&m);
    if (c == IP_NULL
        || c->cmsg_level != IP_IPPROTO_IP
        || c->cmsg_type != IP_IP_PKTINFO)
    {
        IPCOM_LOG0(ERR, "VRRPD: no packet info received");
        return IPCOM_ERR_FAILED;
    }
    pktinfo = IP_CMSG_DATA(c);

    /*  RFC3768
        7.1. Receiving VRRP Packets

        Performe the following functions when a VRRP packet is received:
        -  MUST verify that the IP TTL is 255.
        -  MUST verify the VRRP version is 2.
        -  MUST verify that the received packet contains the complete VRRP
           packet (including fixed fields, IP Address(es), and Authentication
           Data).
        -  MUST verify the VRRP checksum.
        -  MUST verify that the VRID is configured on the receiving interface
           and the local router is not the IP Address owner (Priority equals
           255 (decimal)).
        -  MUST verify that the Auth Type matches the locally configured
           authentication method for the virtual router and perform that
           authentication method.

        If any one of the above checks fails, the receiver MUST discard the
        packet, SHOULD log the event and MAY indicate via network management
        that an error occurred.
     */

    /* The IP header is already verified by the stack */
    ip = (Ipcom_ip_header *) v->buf;

    if (ip->ttl != 255)
    {
        IPCOM_LOG1(NOTICE,
                   "VRRPD: got message with TTL=%d, TTL must be 255",
                   ip->ttl);
        return IPCOM_SUCCESS;
    }

    ip_len = (ip->vers_len & 0x0f) * sizeof(Ip_u32);
    vrrp = (Ipnet_vrrp_pkt_t *) (v->buf + ip_len);
    if (bytes < (int) (ip_len + sizeof(Ipnet_vrrp_pkt_t))
        || bytes < ip_len + ipnet_vrrpd_pkt_len(vrrp->count_ip_addrs))
    {
        IPCOM_LOG0(NOTICE, "VRRPD: message corrupted (too short)");
        return IPCOM_SUCCESS;
    }

    ver = IPNET_VRRP_MSG_GET_VERSION(vrrp->ver_type);
    if (ver != IPNET_VRRP_VERSION
        && ver != 1)
    {
        IPCOM_LOG2(NOTICE,
                   "VRRPD: received vrrp version %d, supported version is 1 and %d",
                   ver,
                   IPNET_VRRP_VERSION);
        return IPCOM_SUCCESS;
    }

    if (ver == IPNET_VRRP_VERSION
        && vrrp->auth_type != 0)
    {
        IPCOM_LOG2(NOTICE,
                   "VRRPD: received vrrp version %d and auth_type %d, this combination is not supported.",
                   ver,
                   vrrp->auth_type);
        return IPCOM_SUCCESS;
    }

    if (ipcom_in_checksum(vrrp, bytes - ip_len) != 0)
    {
        IPCOM_LOG0(INFO, "VRRPD: checksum verification failed");
        return IPCOM_SUCCESS;
    }

    if (IPNET_VRRP_MSG_GET_TYPE(vrrp->ver_type) != IPNET_VRRP_TYPE_ADV)
    {
        IPCOM_LOG2(NOTICE,
                   "VRRPD: got message of type %d, expected a VRRP advertisement message (type=%d)",
                   IPNET_VRRP_MSG_GET_TYPE(vrrp->ver_type),
                   IPNET_VRRP_TYPE_ADV);
        return IPCOM_SUCCESS;
    }

    if (ipcom_if_indextoname(pktinfo->ipi_ifindex, key.ifname) == IP_NULL)
    {
        /* Interface got detached between the point where the VRRP
           message was received and up to now. Just ignore this
           message */
        return IPCOM_SUCCESS;
    }

    key.vrid = vrrp->vrid;
    conf = ipcom_hash_get(v->conf_head, &key);

    IPCOM_LOG3(DEBUG2, "VRRPD: Got VRRP advertisment on %s for VRID %d, %s",
               key.ifname, (int)vrrp->vrid,
               conf == IP_NULL ? "no VRRP operation defined" : "running as backup");

    if (conf == IP_NULL)
        /* No VRRP operation defined on this interface */
        return IPCOM_SUCCESS;

    if (vrrp->auth_type != conf->auth_type)
    {
        IPCOM_LOG4(NOTICE,
                   "VRRPD: message authentication type is %d, but interface %s with VRID=%d is configured to use %d",
                   vrrp->auth_type,
                   key.ifname,
                   vrrp->vrid,
                   conf->auth_type);
        return IPCOM_SUCCESS;
    }
    /* RFC3768, chapter 5.3.6.1. Authentication Type 0 - No Authentication

       The use of this authentication type means that VRRP protocol
       exchanges are not authenticated.  The contents of the Authentication
       Data field should be set to zero on transmission and ignored on
       reception.
    */

    if (conf->auth_type == IPNET_VRRP_AUTH_TYPE_1)
    {
        char *auth_start = (char *) vrrp
            + ipnet_vrrpd_pkt_len(vrrp->count_ip_addrs)
            - IPNET_VRRP_AUTH_DATA_SIZE;
        if (ipcom_memcmp(conf->auth_data, auth_start, IPNET_VRRP_AUTH_DATA_SIZE) != 0)
        {
            char buf0[IPNET_VRRP_AUTH_DATA_SIZE + 1];
            char buf1[IPNET_VRRP_AUTH_DATA_SIZE + 1];
            ipcom_snprintf(buf0, IPNET_VRRP_AUTH_DATA_SIZE + 1, "%s", conf->auth_data);
            ipcom_snprintf(buf1, IPNET_VRRP_AUTH_DATA_SIZE + 1, "%s", auth_start);
            IPCOM_LOG2(DEBUG2,
                       "VRRPD: Invalid auth_data sent. Expected: %s. Got: %s.",
                       buf0,
                       buf1);
            return IPCOM_SUCCESS;
        }
    }

    if (conf->priority == 255)
    {
        if (vrrp->priority == 255)
            IPCOM_LOG2(ERR,
                       "VRRPD: Multiple masters defined for VRID=%d on the link %s is attached to",
                       vrrp->vrid,
                       key.ifname);
        else
            /* This might happen even on a correctly configured
               network if this node just resumed its VRRP operation */
            IPCOM_LOG3(INFO,
                       "VRRPD: Received VRRP advertisement message on %s for VRID=%d "
                       "and prio=%d despite this node being active and address owner",
                       key.ifname,
                       vrrp->vrid,
                       vrrp->priority);
        return IPCOM_SUCCESS;
    }

    /* RFC 3768, chapter 7.1
      -  MAY verify that "Count IP Addrs" and the list of IP Address
         matches the IP_Addresses configured for the VRID

      If the above check fails, the receiver SHOULD log the event and MAY
      indicate via network management that a misconfiguration was detected.
      If the packet was not generated by the address owner (Priority does
      not equal 255 (decimal)), the receiver MUST drop the packet,
      otherwise continue processing.
     */
    if (vrrp->count_ip_addrs != conf->num_addresses)
    {
        IPCOM_LOG4(NOTICE,
                   "VRRPD: VRID=%d on %s is configured with %d addresses, but the received message states %d addresses",
                   vrrp->vrid,
                   key.ifname,
                   conf->num_addresses,
                   vrrp->count_ip_addrs);
        if (vrrp->priority != 255)
            return IPCOM_SUCCESS;
    }

    /* RFC 3768, chapter 7.1
       -  MUST verify that the Adver Interval in the packet is the same as
          the locally configured for this virtual router

       If the above check fails, the receiver MUST discard the packet,
       SHOULD log the event and MAY indicate via network management that a
       misconfiguration was detected.
    */
    if (vrrp->adver_int != conf->adv_interval_sec)
    {
        IPCOM_LOG4(NOTICE,
                   "VRRPD: VRID=%d on %s is configured with an advertisment interval of %d seconds, but the received message specifies an interval of %d seconds",
                   vrrp->vrid,
                   key.ifname,
                   conf->adv_interval_sec,
                   vrrp->adver_int);
        return IPCOM_SUCCESS;
    }

    if (conf->state == IPNET_VRRPD_STATE_BACKUP)
        return ipnet_vrrpd_process_msg_as_backup(conf, vrrp);

    return ipnet_vrrpd_process_msg_as_master(conf,
                                             vrrp,
                                             ipnet_vrrpd_is_addr_gt_local_addr(v->sock,
                                                                               conf->ifname,
                                                                               &src));
}


/*
 *===========================================================================
 *                   ipnet_vrrpd_run
 *===========================================================================
 * Description: The VRRP daemon main loop.
 * Parameters:  v - The virtual router redundancy protocol structure.
 * Returns:     IPCOM_SUCCCESS if everything was successful
 *              IPCOM_FAILED otherwise
 *
 */
IP_STATIC void
ipnet_vrrpd_run(Ipnet_vrrpd_t *v)
{
    Ip_fd_set rd_set;
    Ip_bool   shutdown = IP_FALSE;
    int       width;

    while (!shutdown)
    {
        IP_FD_ZERO(&rd_set);
        IP_FD_SET(v->ipd, &rd_set);
        IP_FD_SET(v->sock, &rd_set);
        width = IP_MAX(v->ipd, v->sock);
        if (ipcom_tmo2_select(v->tmo_head, width + 1, &rd_set, IP_NULL, IP_NULL, IP_NULL) < 0)
        {
            IPCOM_LOG2(ERR, "VRRPD: select failed: %s(%d)",
                       ipcom_strerror(ipcom_errno), ipcom_errno);
            break;
        }

        if (IP_FD_ISSET(v->ipd, &rd_set)
            && ipnet_vrrpd_process_ipd_msg(v, &shutdown) != IPCOM_SUCCESS)
        {
            IPCOM_LOG0(ERR, "VRRPD: failed to process IPD message");
            break;
        }

        if (IP_FD_ISSET(v->sock, &rd_set)
            && ipnet_vrrpd_process_msg(v) != IPCOM_SUCCESS)
        {
            IPCOM_LOG0(ERR, "VRRPD: failed to process VRRP message");
            break;
        }
    }
}


/*
 *===========================================================================
 *                   ipnet_vrrpd_startup
 *===========================================================================
 * Description: Initializes the VRRP daemon.
 * Parameters:  v - The virtual router redundancy protocol structure.
 * Returns:     IPCOM_SUCCCESS if everything was successful
 *              IPCOM_FAILED otherwise
 *
 */
IP_STATIC Ip_err
ipnet_vrrpd_startup(Ipnet_vrrpd_t *v)
{
    int                   val;
    Ip_u8                 val8;
    struct Ip_sockaddr_in name;

    IPCOM_LOG0(INFO, "VRRPD: starting");
    ipcom_memset(v, 0, sizeof(Ipnet_vrrpd_t));
    v->ipd  = IP_INVALID_SOCKET;

    v->sock = ipcom_socket(IP_AF_INET, IP_SOCK_RAW, IPNET_VRRPD_PROTO);
    if (v->sock == IP_INVALID_SOCKET)
    {
        IPCOM_LOG0(CRIT, "VRRPD: Failed to create VRRPD advertisement socket");
        return IPCOM_ERR_FAILED;
    }

    /* TTL must be 255 on all sent VRRP messages */
    val8 = 255;
    if (ipcom_setsockopt(v->sock, IP_IPPROTO_IP, IP_IP_MULTICAST_TTL, &val8, sizeof(val8)) < 0)
    {
        IPCOM_LOG2(ERR, "VRRPD: Failed to set TTL %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
        return IPCOM_ERR_FAILED;
    }

    /* No reason to loop back the VRRP messages */
    val8 = 0;
    if (ipcom_setsockopt(v->sock, IP_IPPROTO_IP, IP_IP_MULTICAST_LOOP, &val8, sizeof(val8)) < 0)
        IPCOM_LOG2(NOTICE, "VRRPD: Failed to disable looping back multicast packets: %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);

    /* Must receive packet info */
    val = 1;
    if (ipcom_setsockopt(v->sock, IP_IPPROTO_IP, IP_IP_PKTINFO, &val, sizeof(val)) < 0)
    {
        IPCOM_LOG2(ERR, "VRRPD: Failed to enable packet info on receive %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
        return IPCOM_ERR_FAILED;
    }

    ipcom_memset(&name, 0, sizeof(name));
    IPCOM_SA_LEN_SET(&name, sizeof(name));
    name.sin_family = IP_AF_INET;
    (void) ipcom_inet_pton(IP_AF_INET, IPNET_VRRPD_IPV4_ADDR, &name.sin_addr);
    if (ipcom_bind(v->sock, (struct Ip_sockaddr *) &name, sizeof(name)) < 0)
    {
        IPCOM_LOG3(ERR, "VRRPD: Failed to bind to %s: %s(%d)",
                   IPNET_VRRPD_IPV4_ADDR, ipcom_strerror(ipcom_errno), ipcom_errno);
        return IPCOM_ERR_FAILED;
    }

    if (ipcom_ipd_init(IPNET_VRRPD_IPD_NAME, IPCOM_SUCCESS, &v->ipd) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(CRIT, "VRRPD: Failed to initialize IPD");
        return IPCOM_ERR_FAILED;
    }

    v->tmo_head = ipcom_tmo2_new();
    if (v->tmo_head == IP_NULL)
    {
        IPCOM_LOG0(CRIT, "VRRPD: Failed to create timeout queue");
        return IPCOM_ERR_FAILED;
    }

    v->conf_head = ipcom_hash_new((Ipcom_hash_obj_func)ipnet_vrrpd_obj_hash,
                                  (Ipcom_hash_key_func)ipnet_vrrpd_key_hash,
                                  (Ipcom_hash_cmp_func)ipnet_vrrpd_hash_cmp);
    if (v->conf_head == IP_NULL)
    {
        IPCOM_LOG0(CRIT, "VRRPD: Failed to create hash table");
        return IPCOM_ERR_FAILED;
    }
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                   ipnet_vrrpd_shutdown_conf
 *===========================================================================
 * Description: Sets the state to UNINITIALIZED on all configured virtual addresses.
 * Parameters:  conf - Configuration for a virtual address.
 * Returns:
 *
 */
IP_STATIC void
ipnet_vrrpd_shutdown_conf(Ipnet_vrrpd_conf_t *conf, Ipcom_hash *conf_head)
{
    /* RFC3768, 6.4.3. Master
       ...
       -  If a Shutdown event is received, then:
          o  Cancel the Adver_Timer
          o  Send an ADVERTISEMENT with Priority = 0
          o  Transition to the {Initialize} state
         endif
       ...
    */
    if (conf->state == IPNET_VRRPD_STATE_MASTER)
    {
        conf->priority = 0;
        (void)ipnet_vrrpd_send_adv(conf);
    }

    /* RFC3768, 6.4.2. Backup
       ...
       -  If a Shutdown event is received, then:
          o  Cancel the Master_Down_Timer
          o  Transition to the {Initialize} state
         endif
      ...
    */

    /* This will cancel any running timer and change state */
    (void)ipnet_vrrpd_set_state(conf, IPNET_VRRPD_STATE_UNINITIALIZED);
    (void)ipcom_hash_remove(conf_head, conf);
    ipcom_free(conf);
}


/*
 *===========================================================================
 *                   ipnet_vrrpd_shutdown
 *===========================================================================
 * Description: Shutdowns the VRRP daemon.
 * Parameters:  v - The virtual router redundancy protocol structure.
 * Returns:
 *
 */
IP_STATIC void
ipnet_vrrpd_shutdown(Ipnet_vrrpd_t *v)
{
    IPCOM_LOG0(INFO, "VRRPD: shutting down");
    if (v->conf_head != IP_NULL)
        ipcom_hash_for_each(v->conf_head,
                            (Ipcom_hash_foreach_cb_func) ipnet_vrrpd_shutdown_conf,
                            v->conf_head);

    (void)ipcom_socketclose(v->sock);
    (void)ipcom_ipd_exit(IPNET_VRRPD_IPD_NAME, v->ipd);

    if (v->tmo_head != IP_NULL)
        ipcom_tmo2_delete(v->tmo_head);

    if (v->conf_head != IP_NULL)
        ipcom_hash_delete(v->conf_head);
}


/*
 *===========================================================================
 *                   ipnet_vrrpd_proc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IPCOM_PROCESS(ipnet_vrrpd_proc)
{
    ipcom_proc_init();

    if (ipnet_vrrpd_startup(&ipnet_vrrpd) != IPCOM_SUCCESS)
        goto failed;

    if (ipnet_vrrpd_reconfigure(&ipnet_vrrpd) != IPCOM_SUCCESS)
        goto failed;

    ipnet_vrrpd_run(&ipnet_vrrpd);
failed:
    ipnet_vrrpd_shutdown(&ipnet_vrrpd);
    ipcom_proc_exit();
}

#endif /* IPNET_USE_VRRP && IPCOM_USE_INET */

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                     ipnet_vrrpd_create
 *===========================================================================
 * Description: Initializes static data for the VRRP daemon.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipnet_vrrpd_create(void)
{
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_vrrpd_start
 *===========================================================================
 * Description: Starts the daemon.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipnet_vrrpd_start(void)
{
#if defined(IPNET_USE_VRRP) && defined(IPCOM_USE_INET)
    return ipcom_proc_create("ipnet_vrrpd", (Ipcom_proc_func)ipnet_vrrpd_proc,
                             IPCOM_PROC_STACK_DEFAULT, IP_NULL);
#else
    return IPCOM_SUCCESS;
#endif
}


/*
 *===========================================================================
 *                    ipnet_vrrpd_version
 *===========================================================================
 * Description: Returns the daemon version.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC const char *
ipnet_vrrpd_version(void)
{
#if defined(IPNET_USE_VRRP) && defined(IPCOM_USE_INET)
    return "@(#) IPNET VRRP daemon $Name: VXWORKS_ITER18A_FRZ10 $ - INTERPEAK_COPYRIGHT_STRING";
#else
    return "";
#endif
}

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

