/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_radvd_lib.c,v $ $Revision: 1.6.24.2 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_radvd_lib.c,v $
 *   $Author: jhorteli $
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
 */
/* IPNET router advertisement daemon */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipnet_config.h"

/* Time between RA message just when an interface starts to send RA:s */
#define IPNET_RADVD_MAX_INITIAL_RTR_ADVER_INTERVAL  16000

/* Number of fast unsolicitated messages to be sent when an interface
 * is becomming an advertising interface */
#define IPNET_RADVD_MAX_INITIAL_RTR_ADVERTISEMENTS  3

#define IPNET_RADVD_MIN_DELAY_BETWEEN_RAS           3000

#define IPNET_RADVD_MIN_MIN_ROUTER_INTERVAL           30
#define IPNET_RADVD_MAX_MIN_ROUTER_INTERVAL           70



/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO

#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_hash.h>
#include <ipcom_inet.h>
#include <ipcom_ipd.h>
#include <ipcom_list.h>
#include <ipcom_pqueue.h>
#include <ipcom_sock.h>
#include <ipcom_sock6.h>
#include <ipcom_sysctl.h>
#include <ipcom_syslog.h>
#include <ipcom_sysvar.h>
#include <ipcom_tmo2.h>
#include <ipcom_type.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_ip6.h"
#include "ipnet_radvd.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_RADVD_CMD_ADD     1
#define IPNET_RADVD_CMD_DELETE  2
#define IPNET_RADVD_CMD_ENABLE  3
#define IPNET_RADVD_CMD_DISABLE 4

#define IPNET_RADVD_CFG_DELIMITER " ,"

#define IPNET_RADVD_MAGIC        0xADADADAD
#define IPNET_RADVD_PREFIX_MAGIC 0xAEAEAEAE

#define IPNET_RADVD_LOCK()   ipcom_mutex_lock(IPNET_RADVD_DATA()->mutex)
#define IPNET_RADVD_UNLOCK() ipcom_mutex_unlock(IPNET_RADVD_DATA()->mutex)


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* Prefix option configuration parameters (record) */
typedef struct Ipnet_radvd_if_prefix_record_struct
{
    Ipcom_list                 list;
    Ip_u32                     magic;
    char                       ifname[IP_IFNAMSIZ];
    Ip_time_t                  create_time;
    Ipnet_radvd_if_prefix_data data;
}
Ipnet_radvd_if_prefix_record;

/* Network interface configuration parameters (record) */
typedef struct Ipnet_radvd_if_data_record_struct
{
    Ipcom_list          list;                   /* List of configure structures for one if */
    Ip_u32              magic;                  /* Magic cookie */
    Ipcom_list          prefix_list;            /* List of prefixes selected with this if */
    Ipnet_radvd_if_data data;                   /* The configuration data */
}
Ipnet_radvd_if_data_record;

/* Network interface configuration parameters (union for hash table) */
typedef struct Ipnet_radvd_if_data_parent_struct
{
    Ipcom_list          child_list;             /* List of all configure structures for this if */
    Ipcom_list          prefix_list;
    Ip_u32              earliest_sol_ra;        /* Absolute time when the next solicited RA can be sent */
    Ipcom_tmo2          tmo;
    int                 accept_ra;
    int                 initial_rtr_adv;
    Ipnet_radvd_if_data data;                   /* The configuration data */
}
Ipnet_radvd_if_data_parent;

/* Client-side network interface configuration parameters */
typedef struct Ipnet_radvd_if_conf_struct
{
    Ipcom_list            list;
    Ipnet_radvd_if_handle handle;
}
Ipnet_radvd_if_conf;

/* Router advertisement daemon parameters */
typedef struct Ipnet_radvd_struct
{
    int           ipd;
    int           icmp6_sock;
    Ipcom_hash   *ifs;
    Ipcom_pqueue *if_tmo_head;
    Ipcom_list    if_conf_list;
    Ipcom_mutex   mutex;
}
Ipnet_radvd;


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */
IPCOM_PROCESS(ipnet_radvd_proc);

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC Ip_bool ipnet_radvd_tmo_send_advertisement(Ipnet_radvd_if_data_parent *parent);


/*
 *===========================================================================
 *                    ipnet_radvd_create
 *===========================================================================
 * Description: IPD hook to allocate global resources for the RA daemon.
 * Parameters:
 * Returns:     IPCOM_SUCCESS if the operation was successful.
 *
 */
IP_PUBLIC Ip_err ipnet_radvd_create(void);


/*
 *===========================================================================
 *                    ipnet_radvd_start
 *===========================================================================
 * Description: IPD hook to start the RA daemon.
 * Parameters:
 * Returns:     IPCOM_SUCCESS if the operation was successful.
 *
 */
IP_PUBLIC Ip_err ipnet_radvd_start(void);


/*
 *===========================================================================
 *                    ipnet_radvd_version
 *===========================================================================
 * Description: Returns a version string for the RA daemon.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC const char * ipnet_radvd_version(void);


IP_GLOBAL int ipnet_radvd_get_sysvar(IP_CONST char *ifname, IP_CONST char *key,
                                     IP_CONST char *prefix, int default_val);

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

#ifndef IP_PORT_LAS
IP_STATIC Ipnet_radvd ipnet_radvd[IPCOM_VR_MAX];

/* Modify this macro to add multiple radvd instances */
#define IPNET_RADVD_DATA() (&ipnet_radvd[ipcom_proc_vr_get()])
#else
IP_STATIC Ipnet_radvd ipnet_radvd[1];

/* Modify this macro to add multiple radvd instances */
#define IPNET_RADVD_DATA() (&ipnet_radvd[0])
#endif

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipnet_radvd_msec_now
 *===========================================================================
 * Description: Returns a number that is increased by 1 for each milliseconds.
 * Parameters:
 * Returns:     A value with millisecond resolution.
 *
 */
IP_STATIC Ip_u32
ipnet_radvd_msec_now(void)
{
    struct Ip_timeval tv;

    ipcom_microtime(&tv);
#ifdef IP_DEBUG
    {
        static Ip_u32 boot_time = 0;

        if (boot_time == 0)
            boot_time = IPNET_TIMEVAL_TO_MSEC(tv);

        return IPNET_TIMEVAL_TO_MSEC(tv) - boot_time;
    }
#else
    return IPNET_TIMEVAL_TO_MSEC(tv);
#endif
}


/*
 *===========================================================================
 *                    ipnet_radvd_obj_hash
 *===========================================================================
 * Description: Computes a hash value for an interface configuration entry.
 * Parameters:  if_conf - An interface configuration entry.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_radvd_obj_hash(Ipnet_radvd_if_data_parent *parent)
{
    return ipcom_hash_update(parent->data.ifname,
                             ipcom_strlen(parent->data.ifname),
                             0);
}


/*
 *===========================================================================
 *                    ipnet_radvd_key_hash
 *===========================================================================
 * Description: Computes a hash value an interface name.
 * Parameters:  ifname - An interface name.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_radvd_key_hash(char *ifname)
{
    return ipcom_hash_update(ifname, ipcom_strlen(ifname), 0);
}


/*
 *===========================================================================
 *                    ipnet_radvd_hash_cmp
 *===========================================================================
 * Description: Compares the name of interface configuration interface and
 *              the given interface name
 * Parameters:  if_conf - An interface configuration entry.
 *              ifname - An interface name.
 * Returns:     IP_TRUE if the interface names matches.
 *
 */
IP_STATIC Ip_bool
ipnet_radvd_hash_cmp(Ipnet_radvd_if_data_parent *parent, char *ifname)
{
    return ipcom_strcmp(parent->data.ifname, ifname) == 0;
}


/*
 *===========================================================================
 *                    ipnet_radvd_if_initiate
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_radvd_if_initiate(Ipnet_radvd_if_handle handle)
{
    Ipnet_radvd_if_data_record   *record = handle;
    Ipnet_radvd_if_data_parent   *parent;
    Ipnet_radvd                  *radvd = IPNET_RADVD_DATA();
    Ip_bool                      success = IP_FALSE;

    IPNET_RADVD_LOCK();

    if (record == IP_NULL || record->magic != IPNET_RADVD_MAGIC)
    {
        IPCOM_LOG0(ERR, "radvd: Invalid handle passed to if_start");
        goto cleanup;
    }

    /* Get parent */
    parent = ipcom_hash_get(radvd->ifs, record->data.ifname);
    if (parent == IP_NULL)
    {
        IPCOM_LOG1(ERR, "radvd: Cannot find parent for if %s", record->data.ifname);
        goto cleanup;
    }

    if (parent->initial_rtr_adv == 1)
    {
        (void)ipcom_tmo2_request(radvd->if_tmo_head,
                                 &parent->tmo,
                                 (Ipcom_tmo2_func)ipnet_radvd_tmo_send_advertisement,
                                 parent,
                                 0);
        success = IP_TRUE;
    }


cleanup:
    IPNET_RADVD_UNLOCK();
    return success;
}


/*
 *===========================================================================
 *                    ipnet_radvd_add_prefix_info
 *===========================================================================
 * Description: Adds a prefix option from the prefix configuration info.
 * Parameters:  prefix_conf - The configuration parameters for this prefix.
 *              prefix_opt - The buffer the prefix info should be written in.
 * Returns:
 */
IP_STATIC void
ipnet_radvd_add_prefix_info(Ipnet_radvd_if_prefix_record *prefix_record,
                            Ipnet_pkt_nd_opt_prefix_info *prefix_opt)
{
    Ip_u32 valid_lifetime = prefix_record->data.valid_lifetime;
    Ip_u32 preferred_lifetime = prefix_record->data.preferred_lifetime;
    Ip_time_t create_time = prefix_record->create_time;
    Ip_time_t now = ipcom_time(IP_NULL);

    ipcom_memset(prefix_opt, 0, sizeof(Ipnet_pkt_nd_opt_prefix_info));
    prefix_opt->nd_opt_pi_type       = IPNET_ND_OPT_PREFIX_INFORMATION;
    prefix_opt->nd_opt_pi_len        = 4;
    prefix_opt->nd_opt_pi_prefix_len = (Ip_u8) prefix_record->data.prefixlen;

    if (prefix_record->data.on_link)
        IP_BIT_SET(prefix_opt->nd_opt_pi_flags_reserved, IPNET_ND_OPT_PI_FLAG_ONLINK);
    if (prefix_record->data.autonomous)
        IP_BIT_SET(prefix_opt->nd_opt_pi_flags_reserved, IPNET_ND_OPT_PI_FLAG_AUTO);
    if (prefix_record->data.router_address)
        IP_BIT_SET(prefix_opt->nd_opt_pi_flags_reserved, IPNET_ND_OPT_PI_FLAG_ROUTER_ADDRESS);

    if (prefix_record->data.valid_lifetime_decrement)
    {
        if (now - create_time > valid_lifetime)
            valid_lifetime = 0;
        else
            valid_lifetime -= now - create_time;
    }

    if (prefix_record->data.preferred_lifetime_decrement)
    {
        if (now - create_time > preferred_lifetime)
            preferred_lifetime = 0;
        else
            preferred_lifetime -= now - create_time;
    }

    IP_SET_HTONL(&prefix_opt->nd_opt_pi_valid_time, valid_lifetime);
    IP_SET_HTONL(&prefix_opt->nd_opt_pi_preferred_time, preferred_lifetime);

    /* Store the prefix */
    IPNET_IP6_SET_ADDR(&prefix_opt->nd_opt_pi_prefix, &prefix_record->data.prefix);
}


/*
 *===========================================================================
 *                 ipnet_radvd_flags_expansion_option_len
 *===========================================================================
 * Description: Returns the size of the RFC 5175 flags expansion option
 * Parameters:  ifdata - interface configuration
 * Returns:     0 if the option should not be included
 *              8 if it should be included
 *
 */
IP_STATIC int
ipnet_radvd_flags_expansion_option_len(Ipnet_radvd_if_data *ifdata)
{
    if (ifdata->private_experimentation_0
        || ifdata->private_experimentation_1)
        return sizeof(Ipnet_pkt_nd_opt_flags_expansion);
    return 0;
}


/*
 *===========================================================================
 *                 ipnet_radvd_add_flags_expansion_option
 *===========================================================================
 * Description: Adds a RFC 5175 flags expansion option.
 * Parameters:  ifdata - interface configuration
 *              buf - buffer to where the option should be written
 * Returns:
 *
 */
IP_STATIC void
ipnet_radvd_add_flags_expansion_option(Ipnet_radvd_if_data *ifdata, void *buf)
{
    Ipnet_pkt_nd_opt_flags_expansion *opt = buf;

    opt->nd_opt_fe_type = IPNET_ND_OPT_FLAGS_EXPANSION;
    opt->nd_opt_fe_len  = 1;

    /* This are the flags defined in RFC 5175 */
    if (ifdata->private_experimentation_0)
        IPNET_ND_OPT_FE_BIT_SET(opt, IPNET_ND_OPT_FE_PRIV_0);
    if (ifdata->private_experimentation_1)
        IPNET_ND_OPT_FE_BIT_SET(opt, IPNET_ND_OPT_FE_PRIV_1);
}


/*
 *===========================================================================
 *                  ipnet_radvd_send_advertisement
 *===========================================================================
 * Description: Sends a router advertisements message on an interface.
 * Parameters:  radvd - The router advertisement daemon parameters.
 *              if_conf - The configuration parameters for the interface
 *              the router advertisement should be sent on.
 *              to - The unicast address the message should be sent to
 *              or IP_NULL if an unsolicited router advertisements should
 *              be sent.
 * Returns:
 *
 */
IP_STATIC void
ipnet_radvd_send_advertisement(Ipnet_radvd *radvd,
                               Ipnet_radvd_if_data_parent *parent,
                               struct Ip_sockaddr_in6 *to)
{
    Ipnet_nd_router_advert       *ra = IP_NULL;
    int                           len;
    int                           flags_expansion_opt_len;
    Ipnet_radvd_if_prefix_record *prefix_record;
    Ipnet_pkt_nd_opt_prefix_info *prefix_opt;
    struct Ip_sockaddr_in6        to_buf;
    Ip_u32                        now = ipnet_radvd_msec_now();
    Ip_u32                        next_timeout;
    char                          *ra_ptr;
    int                           ret   = 0;

    if (IPCOM_IS_LT(now, parent->earliest_sol_ra))
        /* Is starting up or too short time between RA:s */
        goto cleanup;

    len =   sizeof(Ipnet_nd_router_advert);
    len +=  parent->prefix_list.size * sizeof(Ipnet_pkt_nd_opt_prefix_info);
    if (parent->data.link_mtu)
        len += sizeof(Ipnet_pkt_nd_opt_mtu);
    if (parent->data.adv_interval_opt)
        len += sizeof(Ipnet_pkt_nd_opt_adv_interval);
    if (parent->data.adv_home_agent_opt)
        len += sizeof(Ipnet_pkt_nd_opt_home_agent);
    /* The length of RFC 5175 Flags Expansion Option will be zero if
       it is not needed */
    flags_expansion_opt_len = ipnet_radvd_flags_expansion_option_len(&parent->data);
    len += flags_expansion_opt_len;

    ra = ipcom_calloc(1, len);
    if (ra == IP_NULL)
    {
        IPCOM_LOG1(ERR, "radvd: Failed to allocate send buffer (%d bytes)", len);
        return;
    }

    /* Store the pointer */
    ra_ptr = (char *) ra;
    ra_ptr += sizeof(*ra);

    ra->ipnet_nd_ra_curhoplimit     = parent->data.hoplimit;
    ra->ipnet_nd_ra_router_lifetime = (Ip_u16) ip_ntohs((parent->data.default_lifetime/1000));
    ra->ipnet_nd_ra_flags_reserved  = 0;
    if (parent->data.managed)
        IP_BIT_SET(ra->ipnet_nd_ra_flags_reserved, IPNET_ND_RA_FLAG_MANAGED);
    if (parent->data.other_config)
        IP_BIT_SET(ra->ipnet_nd_ra_flags_reserved, IPNET_ND_RA_FLAG_OTHER);
    if (parent->data.home_agent)
        IP_BIT_SET(ra->ipnet_nd_ra_flags_reserved, IPNET_ND_RA_FLAG_HOME_AGENT);

    IP_SET_HTONL(&ra->nd_ra_reachable, parent->data.reachable_time);
    IP_SET_HTONL(&ra->nd_ra_retransmit, parent->data.retrans_timer);

    if (flags_expansion_opt_len > 0)
    {
        ipnet_radvd_add_flags_expansion_option(&parent->data, ra_ptr);
        ra_ptr += flags_expansion_opt_len;
    }

    if (parent->data.link_mtu != 0)
    {
        Ipnet_pkt_nd_opt_mtu *mtu_opt;
        /* MTU option */
        mtu_opt = (Ipnet_pkt_nd_opt_mtu *) (ra_ptr);
        ra_ptr += sizeof(*mtu_opt);

        mtu_opt->nd_opt_mtu_type     = IPNET_ND_OPT_MTU;
        mtu_opt->nd_opt_mtu_len      = 1;
        mtu_opt->nd_opt_mtu_reserved = 0;
        IP_SET_HTONL(&mtu_opt->nd_opt_mtu_mtu, parent->data.link_mtu);
    }

    if (parent->data.adv_interval_opt != 0)
    {
        Ipnet_pkt_nd_opt_adv_interval *adv_int_opt;
        /* Advertisement interval option */
        adv_int_opt = (Ipnet_pkt_nd_opt_adv_interval *) (ra_ptr);
        ra_ptr += sizeof(*adv_int_opt);

        adv_int_opt->nd_opt_adv_int_type        = IPNET_ND_OPT_ADV_INTERVAL;
        adv_int_opt->nd_opt_adv_int_len         = 1;
        adv_int_opt->nd_opt_adv_int_reserved    = 0;
        IP_SET_HTONL(&adv_int_opt->nd_opt_adv_int_interval, parent->data.max_rtr_adv_interval);
    }

    if (parent->data.adv_home_agent_opt != 0)
    {
        Ipnet_pkt_nd_opt_home_agent *adv_ha_opt;
        /* Home Agent information option */
        adv_ha_opt = (Ipnet_pkt_nd_opt_home_agent *) (ra_ptr);
        ra_ptr += sizeof(*adv_ha_opt);

        adv_ha_opt->nd_opt_ha_type        = IPNET_ND_OPT_HOME_AGENT_INFO;
        adv_ha_opt->nd_opt_ha_len         = 1;
        adv_ha_opt->nd_opt_ha_reserved    = 0;
        IP_SET_HTONS(&adv_ha_opt->nd_opt_ha_preference, ((Ip_u16)parent->data.adv_home_agent_opt_preference));
        IP_SET_HTONS(&adv_ha_opt->nd_opt_ha_lifetime,   ((Ip_u16)(parent->data.adv_home_agent_opt_lifetime)));
    }

    prefix_opt = (Ipnet_pkt_nd_opt_prefix_info *) (ra_ptr);
    for (prefix_record = IPCOM_LIST_FIRST(&parent->prefix_list);
         prefix_record != IP_NULL;
         prefix_record = IPCOM_LIST_NEXT(&prefix_record->list))
    {
        ipnet_radvd_add_prefix_info(prefix_record, prefix_opt++);
    }

    /* Fill in the ICMPv6 header */
    ra->nd_ra_hdr.icmp6_type  = IPNET_ND_ROUTER_ADVERT;
    ra->nd_ra_hdr.icmp6_code  = 0;
    ra->nd_ra_hdr.icmp6_cksum = 0;

    if (to == IP_NULL)
    {
        /* No specific response address, send an unsolicited router advertisement */
        to = &to_buf;
        ipcom_memset(to, 0, sizeof(*to));
        (void)ipcom_inet_pton(IP_AF_INET6, "ff02::1", &to->sin6_addr);
        IPCOM_SA_LEN_SET(to, sizeof(to_buf));
        to->sin6_family   = IP_AF_INET6;
        to->sin6_scope_id = ipcom_if_nametoindex(parent->data.ifname);
        if (to->sin6_scope_id == 0)
        {
            IPCOM_LOG1(WARNING, "radvd: Interface %s is not attached", parent->data.ifname);
            goto cleanup;
        }
    }

    if ((ret = ipcom_sendto(radvd->icmp6_sock, ra, len, 0, (struct Ip_sockaddr *) to, sizeof(*to)))
        == IP_SOCKERR)
    {
        IPCOM_LOG3(WARNING, "radvd: Failed to send router advertisement on %s: %s(%d)",
                   parent->data.ifname,
                   ipcom_strerror(ipcom_errno),
                   ipcom_errno);
    }

 cleanup:
    ipcom_tmo2_cancel(&parent->tmo);
    if (IPCOM_IS_LT(now, parent->earliest_sol_ra))
        next_timeout = parent->earliest_sol_ra - now;
    else
    {
        if (parent->initial_rtr_adv >= IPNET_RADVD_MAX_INITIAL_RTR_ADVERTISEMENTS)
        {
            parent->earliest_sol_ra = now + parent->data.min_delay_between_ras;
            /* RFC 4861, chapter 6.2.4
               ...
               Unsolicited Router Advertisements are not strictly periodic: the
               interval between subsequent transmissions is randomized to reduce the
               probability of synchronization with the advertisements from other
               routers on the same link [SYNC].  Each advertising interface has its
               own timer.  Whenever a multicast advertisement is sent from an
               interface, the timer is reset to a uniformly distributed random value
               between the interface's configured MinRtrAdvInterval and
               MaxRtrAdvInterval; expiration of the timer causes the next
               advertisement to be sent and a new random value to be chosen.
               ...
            */
            next_timeout = parent->data.min_rtr_adv_interval
                + (Ip_u32) ipcom_random() % ((parent->data.max_rtr_adv_interval - parent->data.min_rtr_adv_interval));
        }
        else
        {
            if (ret >= 0)
            {
                parent->earliest_sol_ra = now + parent->data.min_delay_between_ras;
                parent->initial_rtr_adv++;
                next_timeout = IP_MIN(IPNET_RADVD_MAX_INITIAL_RTR_ADVER_INTERVAL, parent->data.min_rtr_adv_interval);
            }
            else
            {
                parent->earliest_sol_ra = now;
                next_timeout = 1000;
            }
        }
    }

    (void)ipcom_tmo2_request(radvd->if_tmo_head,
                             &parent->tmo,
                             (Ipcom_tmo2_func) ipnet_radvd_tmo_send_advertisement,
                             parent,
                             next_timeout);
    ipcom_free(ra);
}


/*
 *===========================================================================
 *                  ipnet_radvd_tmo_send_advertisement
 *===========================================================================
 * Description: Timeout handler that will send a RA message.
 * Parameters:  if_conf - The configuration parameters for the interface
 *              the router advertisement should be sent on.
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_radvd_tmo_send_advertisement(Ipnet_radvd_if_data_parent *parent)
{
    ipnet_radvd_send_advertisement(IPNET_RADVD_DATA(), parent, IP_NULL);
    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_radvd_validate_options_len
 *===========================================================================
 * Description: Checks that all included options has a valid length field
 * Parameters:  first_opt - Points to the first option in the packet.
 *              tot_opt_len - Length in byte of ICMPv6 header + data.
 * Returns:     IP_FALSE if some option was invalid, the whole IP packet must
 *              be discared.
 *
 */
IP_STATIC Ip_bool
ipnet_radvd_validate_options_len(Ipnet_pkt_nd_opt_hdr *first_opt,
                                 int tot_opt_len,
                                 Ip_bool unspecified_src_addr)
{
    int                   opt_len;
    Ipnet_pkt_nd_opt_hdr *opt = first_opt;

    while (tot_opt_len > 0)
    {
        opt_len = opt->nd_opt_len;
        tot_opt_len -= opt_len << 3;
        if (opt_len == 0 || tot_opt_len < 0)
            return IP_FALSE;

        if (unspecified_src_addr
            && opt->nd_opt_type == IPNET_ND_OPT_SOURCE_LINKADDR)
            return IP_FALSE;

        opt = (Ipnet_pkt_nd_opt_hdr *) ((Ip_u16*) opt + (opt_len << 2));
    }
    return IP_TRUE;
}


/*
 *===========================================================================
 *                  ipnet_radvd_process_router_solicitation
 *===========================================================================
 * Description: Processes a router solicitation message.
 * Parameters:  radvd - The router advertisement daemon parameters.
 * Returns:
 *
 */
IP_STATIC void
ipnet_radvd_process_router_solicitation(Ipnet_radvd *radvd)
{
    char                   ifname[IP_IFNAMSIZ];
#ifdef IPNET_USE_RFC3971
    Ip_ptrdiff_t           icmp6_buf[1500 / sizeof(Ip_ptrdiff_t)];
#else
    Ip_ptrdiff_t           icmp6_buf[25 + sizeof(Ipnet_pkt_icmp6) / sizeof(Ip_ptrdiff_t)];
#endif
    Ipnet_pkt_icmp6       *icmp6 = (Ipnet_pkt_icmp6 *) icmp6_buf;
    struct Ip_iovec        iov;
    struct Ip_msghdr       msg;
    struct Ip_cmsghdr     *cmsg;
    struct Ip_sockaddr_in6 from;
    struct Ip_in6_pktinfo *pktinfo;
    int                   *hopcount = IP_NULL;
    int                    bytes_recv;
    Ip_bool                got_ifname = IP_FALSE;
    Ip_u8 ancillary_data_buf[IP_CMSG_SPACE(sizeof(struct Ip_in6_pktinfo)) + IP_CMSG_SPACE(sizeof(int))];

    iov.iov_base = icmp6_buf;
    iov.iov_len  = sizeof(icmp6_buf);

    msg.msg_name       = &from;
    msg.msg_namelen    = sizeof(from);
    msg.msg_iov        = &iov;
    msg.msg_iovlen     = 1;
    msg.msg_control    = ancillary_data_buf;
    msg.msg_controllen = sizeof(ancillary_data_buf);
    msg.msg_flags      = 0;

    bytes_recv = ipcom_recvmsg(radvd->icmp6_sock, &msg, 0);
    if (bytes_recv == IP_SOCKERR)
    {
        Ip_socklen_t   errlen;
        int so_errno;

        IPCOM_LOG2(ERR, "radvd: ipcom_recvmsg() on ICMPv6 socket failed: %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
        /* Clear socket error. */
        errlen = sizeof(int);
        (void)ipcom_getsockopt(radvd->icmp6_sock, IP_SOL_SOCKET, IP_SO_ERROR, &so_errno, &errlen);
        return;
    }

    if ((unsigned)bytes_recv < sizeof(*icmp6)
        || icmp6->icmp6_type != IPNET_ND_ROUTER_SOLICIT
        || icmp6->icmp6_code != 0
        || (IP_IN6_IS_ADDR_LINK_LOCAL(&from.sin6_addr) == IP_FALSE
            && IP_IN6_IS_ADDR_UNSPECIFIED(&from.sin6_addr) == IP_FALSE)
        || !ipnet_radvd_validate_options_len((void*) (icmp6 + 1),
                                             bytes_recv - sizeof(Ipnet_pkt_icmp6),
                                             IP_IN6_IS_ADDR_UNSPECIFIED(&from.sin6_addr)))
            /* Not a (valid) router solicitation */
            return;

    for (cmsg = IP_CMSG_FIRSTHDR(&msg);
         cmsg != IP_NULL;
         cmsg = IP_CMSG_NXTHDR(&msg, cmsg))
    {
        if (cmsg->cmsg_level != IP_IPPROTO_IPV6)
            continue;
        switch (cmsg->cmsg_type)
        {
        case IP_IPV6_HOPLIMIT:
            hopcount = IP_CMSG_DATA(cmsg);
            if (*hopcount != 255)
            {
                IPCOM_LOG1(NOTICE,
                           "radvd: router solicitation message dropped since hopcount != 255 (%d)",
                           *hopcount);
                return;
            }
            break;

        case IP_IPV6_PKTINFO:
            pktinfo = IP_CMSG_DATA(cmsg);
            if (ipcom_if_indextoname(pktinfo->ipi6_ifindex, ifname) == IP_NULL)
            {
                IPCOM_LOG1(WARNING,
                           "radvd: no name found for the interface the router solicitation was received at. ifindex:%d",
                           pktinfo->ipi6_ifindex);
            }
            else
            {
                got_ifname = IP_TRUE;
            }
            break;

        default:
            break;
        }
    }

    if (got_ifname && hopcount)
    {
        Ipnet_radvd_if_data_parent *parent;

        IPNET_RADVD_LOCK();
        parent = ipcom_hash_get(radvd->ifs, ifname);
        if (parent != IP_NULL)
            ipnet_radvd_send_advertisement(radvd, parent, parent->data.unicast_only ? &from : IP_NULL);
        IPNET_RADVD_UNLOCK();
    }
    /* else: Should not send router advertisement on this interface, just drop the message */
}


/*
 *===========================================================================
 *                  ipnet_radvd_sock_open
 *===========================================================================
 * Description: Initializes the ICMPv6 socket used by the daemon.
 * Parameters:  radvd - The router advertisement daemon parameters.
 * Returns:     IPCOM error code.
 *
 */
IP_STATIC Ip_err
ipnet_radvd_sock_open(Ipnet_radvd *radvd)
{
    struct Ip_icmp6_filter filter;
    struct Ip_sockaddr_in6 name;
    int                    hoplimit = 255;
    int                    on = 1;

    radvd->icmp6_sock = ipcom_socket(IP_AF_INET6, IP_SOCK_RAW, IP_IPPROTO_ICMPV6);
    if (radvd->icmp6_sock == IP_INVALID_SOCKET)
    {
        IPCOM_LOG2(ERR, "radvd: Failed to create socket: %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
        return IPCOM_ERR_FAILED;
    }

    IP_ICMP6_FILTER_SETBLOCKALL(&filter);
    IP_ICMP6_FILTER_SETPASS(IPNET_ND_ROUTER_SOLICIT, &filter);
    if (ipcom_setsockopt(radvd->icmp6_sock, IP_IPPROTO_ICMPV6, IP_ICMP6_FILTER,
                         &filter, sizeof(filter)) == IP_SOCKERR)
    {
        IPCOM_LOG2(NOTICE, "radvd: Failed to set ICMPv6 filter: %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
    }

    if (ipcom_setsockopt(radvd->icmp6_sock, IP_IPPROTO_IPV6, IP_IPV6_RECVPKTINFO,
                         &on, sizeof(on)) == IP_SOCKERR)
    {
        IPCOM_LOG2(ERR, "radvd: Failed to activate IPV6_RECVPKTINFO: %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
        return IPCOM_ERR_FAILED;
    }

    if (ipcom_setsockopt(radvd->icmp6_sock, IP_IPPROTO_IPV6, IP_IPV6_RECVHOPLIMIT,
                         &on, sizeof(on)) == IP_SOCKERR)
    {
        IPCOM_LOG2(ERR, "radvd: Failed to activate IPV6_RECVHOPLIMIT: %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
        return IPCOM_ERR_FAILED;
    }

    if (ipcom_setsockopt(radvd->icmp6_sock, IP_IPPROTO_IPV6, IP_IPV6_UNICAST_HOPS,
                         &hoplimit, sizeof(hoplimit)) == IP_SOCKERR)
    {
        IPCOM_LOG2(ERR, "radvd: Failed set unicast hop limit: %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
        return IPCOM_ERR_FAILED;
    }

    if (ipcom_setsockopt(radvd->icmp6_sock, IP_IPPROTO_IPV6, IP_IPV6_MULTICAST_HOPS,
                         &hoplimit, sizeof(hoplimit)) == IP_SOCKERR)
    {
        IPCOM_LOG2(ERR, "radvd: Failed set multicast hop limit: %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
        return IPCOM_ERR_FAILED;
    }

    if (ipcom_setsockopt(radvd->icmp6_sock, IP_IPPROTO_IPV6, IP_IPV6_MULTICAST_LOOP,
                         &on, sizeof(on)) == IP_SOCKERR)
    {
        IPCOM_LOG2(ERR, "radvd: Failed to turn on loopbacking of multicast: %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
        return IPCOM_ERR_FAILED;
    }

    ipcom_memset(&name, 0, sizeof(name));
    IPCOM_SA_LEN_SET(&name, sizeof(name));
    name.sin6_family = IP_AF_INET6;
    if (ipcom_bind(radvd->icmp6_sock, (struct Ip_sockaddr *) &name, sizeof(name)) == IP_SOCKERR)
    {
        IPCOM_LOG2(ERR, "radvd: Failed to bind: %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
        return IPCOM_ERR_FAILED;
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_radvd_ra_off_restore
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipnet_radvd_ra_off_restore(const char *ifname, int *value, Ip_bool off_restore)
{
    Ip_size_t vlen = sizeof(*value);
    int       name[5];
    int       off = 0;

    name[0] = IP_CTL_NET;
    name[1] = IP_AF_INET6;
    name[2] = IP_IPPROTO_IPV6;
    name[3] = IP_IPV6CTL_ACCEPT_RTADV;
    name[4] = ipcom_if_nametoindex(ifname);
    if ((off_restore == IP_FALSE ?
         ipcom_sysctl(name, 5, value, &vlen, &off, sizeof(off)) :
         ipcom_sysctl(name, 5, IP_NULL, IP_NULL, value, sizeof(*value))) < 0)
    {
        IPCOM_LOG3(ERR, "radvd: Failed to set RA message processing for %s: %s(%d)",
                   ifname, ipcom_strerror(ipcom_errno), ipcom_errno);
        return IPCOM_ERR_FAILED;
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_radvd_all_routers_leave_join
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipnet_radvd_all_routers_leave_join(Ipnet_radvd *radvd, const char *ifname, Ip_bool leave_join)
{
    struct Ip_ipv6_mreq mreq6;

    /* Join the all-routers address on this interface */
    mreq6.ipv6mr_interface = ipcom_if_nametoindex(ifname);
    (void)ipcom_inet_pton(IP_AF_INET6, "ff02::2", &mreq6.ipv6mr_multiaddr);
    if (ipcom_setsockopt(radvd->icmp6_sock, IP_IPPROTO_IPV6,
                         (leave_join == IP_FALSE ? IP_IPV6_LEAVE_GROUP : IP_IPV6_JOIN_GROUP),
                         &mreq6, sizeof(mreq6)) == IP_SOCKERR)
    {
        IPCOM_LOG3(INFO, "radvd: Failed to join/leave the all-routers group on %s: %s(%d)",
                   ifname, ipcom_strerror(ipcom_errno), ipcom_errno);
        return IPCOM_ERR_FAILED;
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_radvd_if_validate
 *===========================================================================
 * Description: Validates an interface configuration.
 * Parameters:  data -  Configuration data for the interface.
 * Returns:     IPCOM error code.
 *
 */
IP_STATIC Ip_err
ipnet_radvd_if_validate(Ipnet_radvd_if_data *data)
{
    /* Verify the configured values */
    if (data->min_rtr_adv_interval < IPNET_RADVD_MIN_MIN_ROUTER_INTERVAL
        || data->max_rtr_adv_interval < IPNET_RADVD_MAX_MIN_ROUTER_INTERVAL
        || 3 * data->min_rtr_adv_interval / 4 > data->max_rtr_adv_interval)
    {
        IPCOM_LOG2(ERR, "radvd: ipnet.inet6.radvd.[min|max]_rtr_adv_interval not set or invalid. min=%d, max=%d",
                   data->min_rtr_adv_interval, data->max_rtr_adv_interval);
        return IPCOM_ERR_INVALID_CONFIG;
    }

    /* Verify the home agent values */
    if (data->home_agent != IP_FALSE)
    {
        /* They've specified that we should use a home agent option? */
        if (data->adv_home_agent_opt != IP_FALSE)
        {
            /* Sanity */
            if (data->adv_home_agent_opt_lifetime == 0)
            {
                IPCOM_LOG0(ERR, "radvd: ipnet.inet6.AdvHomeAgentOptLifetime is zero, which is forbidden");
                return IPCOM_ERR_INVALID_CONFIG;
            }

            /* Default values? */
            if (data->adv_home_agent_opt_lifetime == (data->default_lifetime / 1000)
                && data->adv_home_agent_opt_preference == 0)
            {
                IPCOM_LOG0(ERR, "radvd: ipnet.inet6.AdvHomeAgentOpt has been disabled since all values are defaults.");
                data->adv_home_agent_opt = IP_FALSE;
            }
        }
    }
    else if (data->adv_home_agent_opt != IP_FALSE)
    {
        IPCOM_LOG0(ERR, "radvd: ipnet.inet6.AdvHomeAgentOpt is set, but AdvHomeAgentFlag is not");
        return IPCOM_ERR_INVALID_CONFIG;
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_radvd_if_rescan
 *===========================================================================
 * Description: Validates an interface configuration.
 * Parameters:  data -  Configuration data for the interface.
 * Returns:     IPCOM error code.
 *
 */
IP_STATIC Ip_err
ipnet_radvd_if_rescan(const char *ifname)
{
    Ipnet_radvd                  *radvd = IPNET_RADVD_DATA();
    Ipnet_radvd_if_data_parent   *parent;
    Ipnet_radvd_if_data_record   *record;
    Ipnet_radvd_if_prefix_record *prefix_record;
    Ip_err                        err = IPCOM_SUCCESS;

    IPNET_RADVD_LOCK();

    /* Get parent */
    parent = ipcom_hash_get(radvd->ifs, (char *)ifname);
    if (parent == IP_NULL)
    {
        IPCOM_LOG1(ERR, "radvd: Could not find parent record for %s", ifname);
        err = IPCOM_ERR_NOT_FOUND;
        goto cleanup;
    }

    /* Remove existing prefix list */
    while ((prefix_record = IPCOM_LIST_FIRST(&parent->prefix_list)) != IP_NULL)
    {
        ipcom_list_remove(&prefix_record->list);
        prefix_record->magic = 0;
        ipcom_free(prefix_record);
    }

    /* Loop through all children and gather the config to use */
    for (record = IPCOM_LIST_FIRST(&parent->child_list), parent->data = record->data;
         record != IP_NULL;
         record = IPCOM_LIST_NEXT(&record->list))
    {
        ip_assert(ipcom_strcmp(parent->data.ifname, record->data.ifname) == 0);

#define IPNET_RADVD_ASSIGN_IF(parent, field, compared_to, record, override)     \
        if (IP_BIT_ISSET(parent->data.override_bitmask, override) == 0 &&       \
            IP_BIT_ISSET(record->data.override_bitmask, override) != 0)         \
        {                                                                       \
            parent->data.field = record->data.field;                            \
            parent->data.override_bitmask |= override;                          \
        }                                                                       \
        else if (IP_BIT_ISSET(parent->data.override_bitmask, override) ==       \
                 IP_BIT_ISSET(record->data.override_bitmask, override) &&       \
                 parent->data.field compared_to record->data.field)             \
        {                                                                       \
            parent->data.field = record->data.field;                            \
        }

        /* Select a common configuration for this interface */
        IPNET_RADVD_ASSIGN_IF(parent, min_rtr_adv_interval,          >, record,
                              IPNET_RADVD_OVERRIDE_MIN_RTR_ADV_INTERVAL);
        IPNET_RADVD_ASSIGN_IF(parent, max_rtr_adv_interval,          >, record,
                              IPNET_RADVD_OVERRIDE_MAX_RTR_ADV_INTERVAL);
        IPNET_RADVD_ASSIGN_IF(parent, min_delay_between_ras,         >, record,
                              IPNET_RADVD_OVERRIDE_MIN_DELAY_BETWEEN_RAS);
        IPNET_RADVD_ASSIGN_IF(parent, default_lifetime,              >, record,
                              IPNET_RADVD_OVERRIDE_DEFAULT_LIFETIME);
        IPNET_RADVD_ASSIGN_IF(parent, reachable_time,                >, record,
                              IPNET_RADVD_OVERRIDE_REACHABLE_TIME);
        IPNET_RADVD_ASSIGN_IF(parent, retrans_timer,                 >, record,
                              IPNET_RADVD_OVERRIDE_RETRANS_TIMER);
        IPNET_RADVD_ASSIGN_IF(parent, link_mtu,                      >, record,
                              IPNET_RADVD_OVERRIDE_LINK_MTU);
        IPNET_RADVD_ASSIGN_IF(parent, adv_home_agent_opt_preference, <, record,
                              IPNET_RADVD_OVERRIDE_ADV_HOME_AGENT_OPT_PREFERENCE);
        IPNET_RADVD_ASSIGN_IF(parent, adv_home_agent_opt_lifetime,   <, record,
                              IPNET_RADVD_OVERRIDE_ADV_HOME_AGENT_OPT_LIFETIME);
        IPNET_RADVD_ASSIGN_IF(parent, hoplimit,                      <, record,
                              IPNET_RADVD_OVERRIDE_HOPLIMIT);
        IPNET_RADVD_ASSIGN_IF(parent, unicast_only,                  <, record,
                              IPNET_RADVD_OVERRIDE_UNICAST_ONLY);
        IPNET_RADVD_ASSIGN_IF(parent, managed,                       <, record,
                              IPNET_RADVD_OVERRIDE_MANAGED);
        IPNET_RADVD_ASSIGN_IF(parent, other_config,                  <, record,
                              IPNET_RADVD_OVERRIDE_OTHER_CONFIG);
        IPNET_RADVD_ASSIGN_IF(parent, home_agent,                    <, record,
                              IPNET_RADVD_OVERRIDE_HOME_AGENT);
        IPNET_RADVD_ASSIGN_IF(parent, adv_interval_opt,              <, record,
                              IPNET_RADVD_OVERRIDE_ADV_INTERVAL_OPT);
        IPNET_RADVD_ASSIGN_IF(parent, adv_home_agent_opt,            <, record,
                              IPNET_RADVD_OVERRIDE_ADV_HOME_AGENT_OPT);
        IPNET_RADVD_ASSIGN_IF(parent, private_experimentation_0,     <, record,
                              IPNET_RADVD_OVERRIDE_PRIVATE_EXPERIMENTION_0);
        IPNET_RADVD_ASSIGN_IF(parent, private_experimentation_1,     <, record,
                              IPNET_RADVD_OVERRIDE_PRIVATE_EXPERIMENTION_1);

        /* Create a list of unique prefixes */
        for (prefix_record = IPCOM_LIST_FIRST(&record->prefix_list);
             prefix_record != IP_NULL;
             prefix_record = IPCOM_LIST_NEXT(&prefix_record->list))
        {
            Ipnet_radvd_if_prefix_record *parent_pr;

            /* Check if prefix already present */
            for (parent_pr = IPCOM_LIST_FIRST(&parent->prefix_list);
                 parent_pr != IP_NULL;
                 parent_pr = IPCOM_LIST_NEXT(&parent_pr->list))
            {
                if (ipcom_memcmp(&prefix_record->data, &parent_pr->data, sizeof(parent_pr->data)) == 0)
                {
                    /* Exactly the same prefix record already exists */
                    IPCOM_LOG1(ERR, "radvd: Trying to add exact duplicate of prefix for %s", ifname);
                    err = IPCOM_ERR_DUPLICATE;
                    break;
                }
                if (IP_IN6_ARE_ADDR_EQUAL(&prefix_record->data.prefix, &parent_pr->data.prefix)
                    && prefix_record->data.prefixlen == parent_pr->data.prefixlen)
                {
                    /* Same prefix but different characteristics */
                    IPCOM_LOG1(ERR, "radvd: Trying to add already existing prefix for %s", ifname);
                    err = IPCOM_ERR_ALREADY_INSTALLED;
                    break;
                }
            }

            /* Create new prefix if not found */
            if (parent_pr == IP_NULL)
            {
                parent_pr = ipcom_calloc(1, sizeof(*parent_pr));
                if (parent_pr == IP_NULL)
                {
                    IPCOM_LOG0(ERR, "radvd: Failed to allocate memory for a new prefix");
                    err = IPCOM_ERR_NO_MEMORY;
                    goto cleanup;
                }
                parent_pr->data = prefix_record->data;
                ipcom_list_insert_last(&parent->prefix_list, &parent_pr->list);
            }

            /*
             * Set the time when this entry last initialized.
             */
            parent_pr->create_time = ipcom_time(IP_NULL);

        }
    }

cleanup:
    IPNET_RADVD_UNLOCK();
    return err;
}



/*
 *===========================================================================
 *                    ipnet_radvd_prefix_conf_add
 *===========================================================================
 * Description: Adds configuration for a prefix option to the specifed interface
 * Parameters:  ifname - The name of the interface to configure.
 *              prefix - The name of the prefix option to configure.
 * Returns:     IPCOM error code.
 *
 */
IP_STATIC Ip_err
ipnet_radvd_prefix_conf_add(Ipnet_radvd_if_conf *if_conf, IP_CONST char *prefix)
{
    Ipnet_radvd_if_data_record *record = if_conf->handle;
    Ipnet_radvd_if_prefix_data  prefix_data;
    char                       *str_prefix;
    char                        sysvar_name[IPNET_RADVD_SYSVAR_NAME_LEN];
    IP_CONST char              *ifname = record->data.ifname;
    Ip_err                      err = IPCOM_ERR_FAILED;

    ipcom_memset(&prefix_data, 0, sizeof(prefix_data));

    ipcom_snprintf(sysvar_name, sizeof(sysvar_name), "ipnet.inet6.radvd.%s.prefix.%s", ifname, prefix);
    if ((str_prefix = ipcom_sysvar_get(sysvar_name, IP_NULL, IP_NULL)) == IP_NULL)
    {
        IPCOM_LOG2(ERR, "radvd: Could not find prefix definition for %s on %s",
                   prefix, ifname);
        goto cleanup;
    }

    if (ipcom_getinaddrprefixbyaddr(IP_AF_INET6, str_prefix, &prefix_data.prefix, &prefix_data.prefixlen) < 0)
    {
        IPCOM_LOG1(ERR, "radvd: %s is not a valid IPv6 prefix and prefixlen in the form prefix/prefixlen.", str_prefix);
        goto cleanup;
    }

    /*
     * The [valid|preferred]_lifetime_decrement is whether the
     * lifetime is static (same value in each RA) or dynamic (lifetime
     * will be decreased towards zero during the lifetime).
     */
    prefix_data.valid_lifetime_decrement
        = (Ip_bool)ipnet_radvd_get_sysvar(ifname, "AdvValidLifetimeDecrement", prefix, IP_FALSE);
    prefix_data.preferred_lifetime_decrement
        = (Ip_bool)ipnet_radvd_get_sysvar(ifname, "AdvPreferredLifetimeDecrement", prefix, IP_FALSE);

    prefix_data.valid_lifetime     = (Ip_u32)ipnet_radvd_get_sysvar(ifname, "AdvValidLifetime", prefix, 2592000);
    prefix_data.preferred_lifetime = (Ip_u32)ipnet_radvd_get_sysvar(ifname, "AdvPreferredLifetime", prefix, 604800);
    prefix_data.on_link            = ipnet_radvd_get_sysvar(ifname, "AdvOnLinkFlag", prefix, 1) != 0;
    prefix_data.autonomous         = ipnet_radvd_get_sysvar(ifname, "AdvAutonomousFlag", prefix, 1) != 0;
    prefix_data.router_address     = ipnet_radvd_get_sysvar(ifname, "AdvRouterAddressFlag", prefix, 0) != 0;

    err = ipnet_radvd_if_prefix_open(if_conf->handle,
                                     IP_NULL,
                                     &prefix_data);


cleanup:
    ipcom_free(str_prefix);
    return err;
}


/*
 *===========================================================================
 *                    ipnet_radvd_if_conf_add
 *===========================================================================
 * Description: Adds configuration for the specifed interface
 * Parameters:  radvd - The router advertisement daemon parameters.
 *              ifname - The name of the interface to configure.
 * Returns:     IPCOM error code.
 *
 */
IP_STATIC Ip_err
ipnet_radvd_if_conf_add(Ipnet_radvd *radvd, IP_CONST char *ifname)
{
    Ipnet_radvd_if_conf *if_conf;
    Ipnet_radvd_if_data  if_data;
    char                *prefixes;
    char                *last;
    char                *prefix;
    char                 sysvar_name[IPNET_RADVD_SYSVAR_NAME_LEN];
    Ip_err               err;

    if_conf = ipcom_calloc(1, sizeof(Ipnet_radvd_if_conf));
    if (if_conf == IP_NULL)
    {
        IPCOM_LOG0(ERR, "radvd: Failed to allocate memory for a new interface");
        return IPCOM_ERR_NO_MEMORY;
    }

    ipcom_list_insert_last(&radvd->if_conf_list, &if_conf->list);

    ipcom_memset(&if_data, 0, sizeof(if_data));
    ipcom_strncpy(if_data.ifname, ifname, IP_IFNAMSIZ);

    if_data.max_rtr_adv_interval =
        ipnet_radvd_get_sysvar(ifname, "MaxRtrAdvInterval", IP_NULL, 600000);

    if_data.min_rtr_adv_interval =
        ipnet_radvd_get_sysvar(ifname, "MinRtrAdvInterval", IP_NULL, if_data.max_rtr_adv_interval / 3);

    /* Use the min value of the protocol constant of 3000ms or the min router value */
    if_data.min_delay_between_ras =
        ipnet_radvd_get_sysvar(ifname, "MinDelayBetweenRAs", IP_NULL,
        IP_MIN(if_data.min_rtr_adv_interval, IPNET_RADVD_MIN_DELAY_BETWEEN_RAS));

    if_data.default_lifetime =
        ipnet_radvd_get_sysvar(ifname, "AdvDefaultLifetime", IP_NULL, IP_MAX(1000, (3 * if_data.max_rtr_adv_interval)));
    if_data.link_mtu = (Ip_u32)
        ipnet_radvd_get_sysvar(ifname, "AdvLinkMTU", IP_NULL, 0);
    if_data.reachable_time = (Ip_u32)
        ipnet_radvd_get_sysvar(ifname, "AdvReachableTime", IP_NULL, 0);
    if_data.retrans_timer = (Ip_u32)
        ipnet_radvd_get_sysvar(ifname, "AdvRetransTimer", IP_NULL, 0);
    if_data.hoplimit = (Ip_u8)
        ipnet_radvd_get_sysvar(ifname, "AdvCurHopLimit", IP_NULL, 64);
    if_data.managed =
        ipnet_radvd_get_sysvar(ifname, "AdvManagedFlag", IP_NULL, 0);
    if_data.other_config =
        ipnet_radvd_get_sysvar(ifname, "AdvOtherConfigFlag", IP_NULL, 0);
    if_data.home_agent =
        ipnet_radvd_get_sysvar(ifname, "AdvHomeAgentFlag", IP_NULL, 0);
    if_data.adv_interval_opt =
        ipnet_radvd_get_sysvar(ifname, "AdvIntervalOpt", IP_NULL, 0);
    if_data.adv_home_agent_opt =
        ipnet_radvd_get_sysvar(ifname, "AdvHomeAgentOpt", IP_NULL, 0);
    if_data.adv_home_agent_opt_lifetime  =
        ipnet_radvd_get_sysvar(ifname, "AdvHomeAgentOptLifetime", IP_NULL, if_data.default_lifetime/1000);
    if_data.adv_home_agent_opt_preference  =
        ipnet_radvd_get_sysvar(ifname, "AdvHomeAgentOptPreference", IP_NULL, 0);
    if_data.disable_accept_rtadv  =
        ipnet_radvd_get_sysvar(ifname, "DisableAcceptRtAdv", IP_NULL, 0);
    if_data.private_experimentation_0 =
        ipnet_radvd_get_sysvar(ifname, "AdvPrivateExperimentation0Flag", IP_NULL, 0);
    if_data.private_experimentation_1 =
        ipnet_radvd_get_sysvar(ifname, "AdvPrivateExperimentation1Flag", IP_NULL, 0);

    err = ipnet_radvd_if_open(&if_conf->handle, &if_data);
    if (err != IPCOM_SUCCESS)
    {
        goto cleanup;
    }

    ipcom_snprintf(sysvar_name, sizeof(sysvar_name), "ipnet.inet6.radvd.%s.AdvPrefixList", ifname);
    last = prefixes = ipcom_sysvar_get(sysvar_name, IP_NULL, IP_NULL);
    if (prefixes)
    {
        while (err == IPCOM_SUCCESS
               && IP_NULL != (prefix = ipcom_strtok_r(last == prefixes ? prefixes : IP_NULL,
                                                      IPNET_RADVD_CFG_DELIMITER,
                                                      &last)))
        {
            err = ipnet_radvd_prefix_conf_add(if_conf, prefix);
        }
        ipcom_free(prefixes);
        if (err != IPCOM_SUCCESS)
            goto cleanup;
    }

    ipnet_radvd_if_initiate(if_conf->handle);
    return IPCOM_SUCCESS;

cleanup:
    ipcom_list_remove(&if_conf->list);
    ipcom_free(if_conf);

    return IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    ipnet_radvd_shutdown
 *===========================================================================
 * Description: Flushes the old configuration.
 * Parameters:  radvd - The router advertisement daemon parameters.
 * Returns:     IPCOM error code.
 *
 */
IP_STATIC Ip_err
ipnet_radvd_shutdown(Ipnet_radvd *radvd)
{
    Ipnet_radvd_if_conf *if_conf;

    /* Remove the current configuration */
    while ((if_conf = IPCOM_LIST_FIRST(&radvd->if_conf_list)) != IP_NULL)
    {
        /* Close entry */
        (void)ipnet_radvd_if_close(if_conf->handle);

        /* Remove the entry from the list */
        ipcom_list_remove(&if_conf->list);

        /* Free the memory */
        ipcom_free(if_conf);
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_radvd_reconfigure
 *===========================================================================
 * Description: Flushes the old configuration and reads the new.
 * Parameters:  radvd - The router advertisement daemon parameters.
 * Returns:     IPCOM error code.
 *
 */
IP_STATIC Ip_err
ipnet_radvd_reconfigure(Ipnet_radvd *radvd)
{
    char  *ifnames;
    char  *last;
    char  *ifname;
    Ip_err err = IPCOM_SUCCESS;

    /* Remove the current configuration */
    (void)ipnet_radvd_shutdown(radvd);

    /* Find all interfaces for which router advertisement should be sent */
    last = ifnames = ipcom_sysvar_get("ipnet.inet6.radvd.interfaces", IP_NULL, IP_NULL);
    if (ifnames == IP_NULL)
    {
        IPCOM_LOG0(INFO, "radvd: No interfaces found in ipnet.inet6.radvd.interfaces");
        return IPCOM_SUCCESS;
    }

    /* Add interface */
    while (err == IPCOM_SUCCESS
           && IP_NULL != (ifname = ipcom_strtok_r(last == ifnames ? ifnames : IP_NULL,
                                                  IPNET_RADVD_CFG_DELIMITER,
                                                  &last)))
    {
        err = ipnet_radvd_if_conf_add(radvd, ifname);
    }

    ipcom_free(ifnames);
    return err;
}


/*
 *===========================================================================
 *                    ipnet_radvd_process_ipd_msg
 *===========================================================================
 * Description: Processes received IPD messages.
 * Parameters:  radvd - The configuration for the router adv daemon.
 *              shutdown - Set to IP_TRUE if the daemon should shutdown.
 * Returns:
 *
 */
#ifdef IPCOM_IPD_ID_IPNET_RADVD
IP_STATIC Ip_err
ipnet_radvd_process_ipd_msg(Ipnet_radvd *radvd, Ip_bool *shutdown)
{
    *shutdown = IP_FALSE;

    switch (ipcom_ipd_input(radvd->ipd))
    {
    case IPCOM_IPD_EVENT_NONE:
        break;
    case IPCOM_IPD_EVENT_RECONFIGURE:
        return ipnet_radvd_reconfigure(radvd);
    case IPCOM_IPD_EVENT_KILL:
        *shutdown = IP_TRUE;
        break;
    default:
        /* Just ignore this message */
        break;
    }
    return IPCOM_SUCCESS;
}
#endif /* IPCOM_IPD_ID_IPNET_RADVD */


/*
 *===========================================================================
 *                  ipnet_radvd_run
 *===========================================================================
 * Description: The router advertisement daemon main loop.
 * Parameters:  radvd - The router advertisement daemon parameters.
 * Returns:
 *
 */
IP_STATIC void
ipnet_radvd_run(Ipnet_radvd *radvd)
{
    Ip_fd_set read_set;
    Ip_bool   shutdown = IP_FALSE;

    while (!shutdown)
    {
        struct Ip_timeval tv;
        int               ret;

        tv.tv_sec    = 0;
        tv.tv_usec   = 100 * 1000;

        IP_FD_ZERO(&read_set);
        IP_FD_SET(radvd->icmp6_sock, &read_set);
        if (radvd->ipd != IP_INVALID_SOCKET)
            IP_FD_SET(radvd->ipd, &read_set);
        if ((ret = ipcom_tmo2_select(radvd->if_tmo_head,
                                     IP_MAX(radvd->icmp6_sock, radvd->ipd) + 1,
                                     &read_set,
                                     IP_NULL,
                                     IP_NULL,
                                     &tv)) == IP_SOCKERR)
        {
            if (ipcom_errno != IP_ERRNO_EAGAIN)
            {
                IPCOM_LOG2(ERR, "radvd: ipcom_socketselect() failed: %s(%d)",
                           ipcom_strerror(ipcom_errno), ipcom_errno);
                shutdown = IP_TRUE;
            }
            continue;
        }

        if (ret == 0)
            continue;

        if (IP_FD_ISSET(radvd->icmp6_sock, &read_set))
            ipnet_radvd_process_router_solicitation(radvd);

        if (radvd->ipd != IP_INVALID_SOCKET && IP_FD_ISSET(radvd->ipd, &read_set))
        {
            Ip_err err;
            err = ipnet_radvd_process_ipd_msg(radvd, &shutdown);
            if (err != IPCOM_SUCCESS)
            {
                IPCOM_LOG1(ERR, "radvd: failed to process IPD message (%d)", err);
                shutdown = IP_TRUE;
            }
        }
    }

    /* Remove the current configuration */
    (void)ipnet_radvd_shutdown(radvd);
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                   ipnet_radvd_proc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_radvd_main(Ip_bool do_ipd)
{
    Ipnet_radvd *radvd = IPNET_RADVD_DATA();

    ipcom_proc_init();

    if (ipnet_radvd_sock_open(radvd) != IPCOM_SUCCESS)
        goto failed;

    if (ipnet_radvd_reconfigure(radvd) != IPCOM_SUCCESS)
        goto failed;

    radvd->ipd = IP_INVALID_SOCKET;
    if (do_ipd && ipcom_ipd_init(IPNET_RADVD_IPD_NAME, IPCOM_SUCCESS, &radvd->ipd) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(CRIT, "Failed to initialize IPD");
        goto failed;
    }

    ipnet_radvd_run(radvd);

    if (do_ipd)
        (void)ipcom_ipd_exit(IPNET_RADVD_IPD_NAME, radvd->ipd);

 failed:
    if (radvd->icmp6_sock != IP_INVALID_SOCKET)
        (void)ipcom_socketclose(radvd->icmp6_sock);
    ipcom_proc_exit();
}


/*
 *===========================================================================
 *                    ipnet_radvd_get_sysvar
 *===========================================================================
 * Description: Returns the value for a router advertisement daemon parameter.
 * Parameters:  ifname - The interface the parameter is valid for.
 *              key - The router advertisement value to return.
 *              prefix - The prefix to return data for or IP_NULL if interface info
 *              should be returned.
 *              default_val - The default value to return if no user value
 *              has been set.
 * Returns:     The value for the specified parameter.
 *
 */
IP_GLOBAL int
ipnet_radvd_get_sysvar(IP_CONST char *ifname, IP_CONST char *key,
                       IP_CONST char *prefix, int default_val)
{
    char sysvar_name[80];

    ipcom_strcpy(sysvar_name, "ipnet.inet6.radvd.");
    ipcom_strcat(sysvar_name, ifname);
    ipcom_strcat(sysvar_name, ".");
    if (prefix == IP_NULL)
        ipcom_strcat(sysvar_name, key);
    else
        ipcom_sprintf(sysvar_name + ipcom_strlen(sysvar_name),
                      "prefix.%s.%s", prefix, key);
    return ipcom_sysvar_get_as_int(sysvar_name, default_val);
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_radvd_create
 *===========================================================================
 * Description: Initializes static data for the RADV daemon.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipnet_radvd_create(void)
{
    Ipnet_radvd *radvd; 
    Ip_err err;
    int i;

    IPCOM_LOG0(DEBUG2, "radvd: Create");

#ifndef IP_PORT_LAS
    for (i = 0; i < IPCOM_VR_MAX; i++)
#else
    for (i = 0; i < 1; i++)
#endif

    {
        radvd = &ipnet_radvd[i];

        ipcom_memset(radvd, 0, sizeof(*radvd));

        err = ipcom_mutex_create(&radvd->mutex);
        if (err != IPCOM_SUCCESS)
            return err;

        radvd->ifs = ipcom_hash_new((Ipcom_hash_obj_func) ipnet_radvd_obj_hash,
                                    (Ipcom_hash_key_func) ipnet_radvd_key_hash,
                                    (Ipcom_hash_cmp_func) ipnet_radvd_hash_cmp);
        if (radvd->ifs == IP_NULL)
            return IPCOM_ERR_NO_MEMORY;

        radvd->if_tmo_head = ipcom_tmo2_new();
        if (radvd->if_tmo_head == IP_NULL)
            return IPCOM_ERR_NO_MEMORY;

        radvd->icmp6_sock = IP_INVALID_SOCKET;

        ipcom_list_init(&radvd->if_conf_list);
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_radvd_if_open
 *===========================================================================
 * Description: Adds configuration entry for the specifed interface.
 * Parameters:  handle - Pointer to returned handle.
 *              data -  Configuration data for the interface.
 * Returns:     IPCOM error code.
 *
 */
IP_PUBLIC Ip_err
ipnet_radvd_if_open(Ipnet_radvd_if_handle *handle,
                    const Ipnet_radvd_if_data *data)
{
    Ipnet_radvd_if_data_parent *parent = IP_NULL;
    Ipnet_radvd_if_data_record *record = IP_NULL;
    Ipnet_radvd                *radvd = IPNET_RADVD_DATA();
    Ip_err                      err = IPCOM_SUCCESS;

    IPCOM_LOG1(DEBUG2, "radvd: Open handle for %s", data->ifname);

    /* Create data record */
    record = ipcom_calloc(1, sizeof(*record));
    if (record == IP_NULL)
    {
        IPCOM_LOG0(ERR, "radvd: Failed to allocate memory for a new interface");
        err = IPCOM_ERR_NO_MEMORY;
        goto cleanup;
    }
    ipcom_list_init(&record->prefix_list);
    record->magic = IPNET_RADVD_MAGIC;
    record->data = *data;

    /* Validate configuration */
    err = ipnet_radvd_if_validate(&record->data);
    if (err != IPCOM_SUCCESS)
    {
        goto cleanup;
    }

    IPNET_RADVD_LOCK();

    /* Check if first entry for this interface */
    parent = ipcom_hash_get(radvd->ifs, (char *)data->ifname);
    if (parent == IP_NULL)
    {
        /* First entry, create parent */
        parent = ipcom_calloc(1, sizeof(*parent));
        if (parent == IP_NULL)
        {
            IPCOM_LOG0(ERR, "radvd: Failed to allocate memory for a new interface");
            err = IPCOM_ERR_NO_MEMORY;
            goto cleanup_lock;
        }
        ipcom_list_init(&parent->child_list);
        ipcom_list_init(&parent->prefix_list);
        parent->data = record->data;

        err = ipcom_hash_add(radvd->ifs, parent);
        if (err != IPCOM_SUCCESS)
        {
            IPCOM_LOG2(ERR, "radvd: Out of memory (%d), could not add %s to hash table", err, data->ifname);
            goto cleanup_lock;
        }

        /* Turn off RA message processing */
        if (parent->data.disable_accept_rtadv == 0)
        {
            err = ipnet_radvd_ra_off_restore(parent->data.ifname, &parent->accept_ra, IP_FALSE);
            if (err != IPCOM_SUCCESS)
            {
                (void)ipcom_hash_remove(radvd->ifs, parent);
                goto cleanup_lock;
            }
        }

        /* Join the all-routers address on this interface */
        err = ipnet_radvd_all_routers_leave_join(radvd, parent->data.ifname, IP_TRUE);
        if (err != IPCOM_SUCCESS)
        {
            (void)ipcom_hash_remove(radvd->ifs, parent);
            if (parent->data.disable_accept_rtadv == 0)
                (void)ipnet_radvd_ra_off_restore(parent->data.ifname, &parent->accept_ra, IP_TRUE);
            goto cleanup_lock;
        }

        /* Schedule an RA message */
        parent->earliest_sol_ra = ipnet_radvd_msec_now();
        parent->initial_rtr_adv = 1;
    }

    ipcom_list_insert_last(&parent->child_list, &record->list);

    IPNET_RADVD_UNLOCK();

    /* Rescan interface */
    err = ipnet_radvd_if_rescan(data->ifname);
    if (err != IPCOM_SUCCESS)
        ipnet_radvd_if_close(record);

    if (err == IPCOM_SUCCESS && handle != IP_NULL)
        *handle = record;

    return err;

cleanup_lock:
    IPNET_RADVD_UNLOCK();

cleanup:
    if (record != IP_NULL)
    {
        record->magic = 0;
        ipcom_free(record);
    }
    if (parent != IP_NULL)
    {
        ipcom_free(parent);
    }

    return err;
}


/*
 *===========================================================================
 *                    ipnet_radvd_if_start
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipnet_radvd_if_start(Ipnet_radvd_if_handle handle)
{
    /* We're external, do start it */
    ipnet_radvd_if_initiate(handle);
}


/*
 *===========================================================================
 *                    ipnet_radvd_if_close
 *===========================================================================
 * Description: Removes configuration entry for the specifed interface.
 * Parameters:  handle - Configuration handle.
 * Returns:     IPCOM error code.
 *
 */
IP_PUBLIC Ip_err
ipnet_radvd_if_close(Ipnet_radvd_if_handle handle)
{
    Ipnet_radvd_if_data_record   *record = handle;
    Ipnet_radvd_if_prefix_record *prefix_record;
    Ipnet_radvd                  *radvd = IPNET_RADVD_DATA();
    Ip_err                        err = IPCOM_SUCCESS;

    IPCOM_LOG2(DEBUG2, "radvd: Close handle %p for %s", handle, record->data.ifname);

    IPNET_RADVD_LOCK();

    if (record == IP_NULL || record->magic != IPNET_RADVD_MAGIC)
    {
        IPCOM_LOG0(ERR, "radvd: Invalid handle passed to if_close");
        err = IPCOM_ERR_INVALID_ARG;
        goto cleanup;
    }

    /* Last child in list? */
    if (ipcom_list_prev(&record->list) == IP_NULL && ipcom_list_next(&record->list) == IP_NULL)
    {
        Ipnet_radvd_if_data_parent *parent;

        ipcom_list_remove(&record->list);

        /* Get parent */
        parent = ipcom_hash_get(radvd->ifs, record->data.ifname);
        if (parent == IP_NULL)
        {
            IPCOM_LOG1(ERR, "radvd: Cannot find parent for if %s", record->data.ifname);
            err = IPCOM_ERR_NOT_FOUND;
            goto cleanup;
        }

        /* Announce our departure */
        parent->data.default_lifetime = 0;
        parent->earliest_sol_ra = ipnet_radvd_msec_now();
        ipnet_radvd_send_advertisement(radvd, parent, IP_NULL);

        /* Also remove the parent */
        (void)ipcom_hash_remove(radvd->ifs, parent);

        /* Cancel outstanding TMO's */
        ipcom_tmo2_cancel(&parent->tmo);

        /* Restore RA message processing */
        if (parent->data.disable_accept_rtadv == 0)
            (void)ipnet_radvd_ra_off_restore(parent->data.ifname, &parent->accept_ra, IP_TRUE);

        /* Leave the all-routers address on this interface */
        (void)ipnet_radvd_all_routers_leave_join(radvd, parent->data.ifname, IP_FALSE);

        IPNET_RADVD_UNLOCK();

        /* Remove parent prefix entries */
        while ((prefix_record = IPCOM_LIST_FIRST(&parent->prefix_list)) != IP_NULL)
        {
            ipcom_list_remove(&prefix_record->list);
            prefix_record->magic = 0;
            ipcom_free(prefix_record);
        }

        ip_assert(IPCOM_LIST_FIRST(&parent->child_list) == IP_NULL);

        ipcom_free(parent);
    }
    else
    {

        ipcom_list_remove(&record->list);
        IPNET_RADVD_UNLOCK();
        (void)ipnet_radvd_if_rescan(record->data.ifname);
    }

    /* Remove prefix entries */
    while ((prefix_record = IPCOM_LIST_FIRST(&record->prefix_list)) != IP_NULL)
    {
        ipcom_list_remove(&prefix_record->list);
        prefix_record->magic = 0;
        ipcom_free(prefix_record);
    }

    record->magic = 0;
    ipcom_free(record);

    return err;

cleanup:
    IPNET_RADVD_UNLOCK();
    return err;
}


/*
 *===========================================================================
 *                    ipnet_radvd_if_get
 *===========================================================================
 * Description: Get configuration data for the specifed handle.
 * Parameters:  handle - Configuration handle.
 *              data -   Pointer to returned configuration data.
 * Returns:     IPCOM error code.
 *
 */
IP_PUBLIC Ip_err
ipnet_radvd_if_get(Ipnet_radvd_if_handle handle,
                   Ipnet_radvd_if_data *data)
{
    Ipnet_radvd_if_data_record *record = handle;
    Ip_err                      err = IPCOM_SUCCESS;

    IPNET_RADVD_LOCK();

    if (record == IP_NULL || record->magic != IPNET_RADVD_MAGIC)
    {
        IPCOM_LOG0(ERR, "radvd: Invalid handle passed to if_get");
        err = IPCOM_ERR_INVALID_ARG;
        goto cleanup;
    }

    if (data != IP_NULL)
        *data = record->data;

cleanup:
    IPNET_RADVD_UNLOCK();
    return err;
}


/*
 *===========================================================================
 *                    ipnet_radvd_if_set
 *===========================================================================
 * Description: Set configuration data for the specifed handle.
 * Parameters:  handle - Configuration handle.
 *              data -   Pointer to new configuration data.
 * Returns:     IPCOM error code.
 *
 */
IP_PUBLIC Ip_err
ipnet_radvd_if_set(Ipnet_radvd_if_handle handle,
                   const Ipnet_radvd_if_data *data)
{
    Ipnet_radvd_if_data_record *record = handle;
    Ipnet_radvd_if_data         old;
    Ipnet_radvd_if_data         tmp;
    Ip_err                      err = IPCOM_SUCCESS;

    IPCOM_LOG2(DEBUG2, "radvd: Set data for handle %p at %s", handle, record->data.ifname);

    IPNET_RADVD_LOCK();

    if (record == IP_NULL || record->magic != IPNET_RADVD_MAGIC)
    {
        IPCOM_LOG0(ERR, "radvd: Invalid handle passed to if_set");
        err = IPCOM_ERR_INVALID_ARG;
        goto cleanup;
    }

    /* Have to have writable copy of data */
    tmp = *data;
    err = ipnet_radvd_if_validate(&tmp);
    if (err != IPCOM_SUCCESS)
    {
        goto cleanup;
    }

    /* Save copy of data if we have to restore */
    old = record->data;
    record->data = tmp;

    IPNET_RADVD_UNLOCK();

    /* Rescan interface */
    err = ipnet_radvd_if_rescan(record->data.ifname);
    if (err != IPCOM_SUCCESS)
    {
        /* Error in configuration so restore old one */
        IPNET_RADVD_LOCK();
        record->data = old;
        IPNET_RADVD_UNLOCK();
        (void)ipnet_radvd_if_rescan(record->data.ifname);
    }

    return err;

cleanup:
    IPNET_RADVD_UNLOCK();
    return err;
}


/*
 *===========================================================================
 *                    ipnet_radvd_if_active_get
 *===========================================================================
 * Description: Get currently active configuration data for the specifed if.
 * Parameters:  ifname - Interface name.
 *              data - Pointer to returned configuration data.
 * Returns:     IPCOM error code.
 *
 */
IP_PUBLIC Ip_err
ipnet_radvd_if_active_get(const char *ifname, Ipnet_radvd_if_data *data)
{
    Ipnet_radvd_if_data_parent   *parent;
    Ipnet_radvd                  *radvd = IPNET_RADVD_DATA();
    Ip_err                        err = IPCOM_SUCCESS;

    IPNET_RADVD_LOCK();

    /* Get parent */
    parent = ipcom_hash_get(radvd->ifs, (char *)ifname);
    if (parent == IP_NULL)
    {
        err = IPCOM_ERR_NOT_FOUND;
    }
    else if (data != IP_NULL)
    {
        *data = parent->data;
    }

    IPNET_RADVD_UNLOCK();
    return err;
}


/*
 *===========================================================================
 *                    ipnet_radvd_if_prefix_open
 *===========================================================================
 * Description: Adds prefix configuration entry for the specifed interface handle.
 * Parameters:  handle - Pointer to returned handle.
 *              prefix_handle - Pointer to returned handle.
 *              data - Prefix configuration data for the interface.
 * Returns:     IPCOM error code.
 *
 */
IP_PUBLIC Ip_err
ipnet_radvd_if_prefix_open(Ipnet_radvd_if_handle handle,
                           Ipnet_radvd_if_prefix_handle *prefix_handle,
                           const Ipnet_radvd_if_prefix_data *data)
{
    Ipnet_radvd_if_data_record   *record = handle;
    Ipnet_radvd_if_prefix_record *prefix_record;
    Ip_err                        err = IPCOM_SUCCESS;

    IPCOM_LOG1(DEBUG2, "radvd: Open prefix handle for %s", record->data.ifname);

    IPNET_RADVD_LOCK();

    if (record == IP_NULL || record->magic != IPNET_RADVD_MAGIC)
    {
        IPCOM_LOG0(ERR, "radvd: Invalid handle passed to if_prefix_open");
        err = IPCOM_ERR_INVALID_ARG;
        goto cleanup;
    }

    prefix_record = ipcom_calloc(1, sizeof(*prefix_record));
    if (prefix_record == IP_NULL)
    {
        IPCOM_LOG0(ERR, "radvd: Failed to allocate memory for a new prefix");
        err = IPCOM_ERR_NO_MEMORY;
        goto cleanup;
    }

    prefix_record->magic = IPNET_RADVD_PREFIX_MAGIC;
    prefix_record->data = *data;
    ipcom_strncpy(prefix_record->ifname, record->data.ifname, sizeof(prefix_record->ifname));
    ipcom_list_insert_last(&record->prefix_list, &prefix_record->list);
    IPNET_RADVD_UNLOCK();

    /* Rescan interface */
    err = ipnet_radvd_if_rescan(record->data.ifname);
    if (err != IPCOM_SUCCESS)
    {
        (void)ipnet_radvd_if_prefix_close(prefix_record);
    }

    if (err == IPCOM_SUCCESS && prefix_handle != IP_NULL)
        *prefix_handle = prefix_record;

    return err;

cleanup:
    IPNET_RADVD_UNLOCK();
    return err;
}


/*
 *===========================================================================
 *                    ipnet_radvd_if_prefix_close
 *===========================================================================
 * Description: Removes prefix configuration entry.
 * Parameters:  handle - Prefix configuration handle.
 * Returns:     IPCOM error code.
 *
 */
IP_PUBLIC Ip_err
ipnet_radvd_if_prefix_close(Ipnet_radvd_if_prefix_handle handle)
{
    Ipnet_radvd_if_prefix_record *prefix_record = handle;
    Ip_err                        err = IPCOM_SUCCESS;

    IPCOM_LOG2(DEBUG2, "radvd: Close prefix handle %p for %s", handle, prefix_record->ifname);

    IPNET_RADVD_LOCK();

    if (prefix_record == IP_NULL || prefix_record->magic != IPNET_RADVD_PREFIX_MAGIC)
    {
        IPCOM_LOG0(ERR, "radvd: Invalid handle passed to if_prefix_close");
        err = IPCOM_ERR_INVALID_ARG;
        goto cleanup;
    }

    ipcom_list_remove(&prefix_record->list);
    IPNET_RADVD_UNLOCK();

    (void)ipnet_radvd_if_rescan(prefix_record->ifname);
    prefix_record->magic = 0;
    ipcom_free(prefix_record);

    return err;

cleanup:
    IPNET_RADVD_UNLOCK();
    return err;
}


/*
 *===========================================================================
 *                    ipnet_radvd_if_prefix_get
 *===========================================================================
 * Description: Get prefix configuration data for the specifed handle.
 * Parameters:  handle - Prefix configuration handle.
 *              data -   Pointer to returned configuration data.
 * Returns:     IPCOM error code.
 *
 */
IP_PUBLIC Ip_err
ipnet_radvd_if_prefix_get(Ipnet_radvd_if_prefix_handle handle,
                          Ipnet_radvd_if_prefix_data *data)
{
    Ipnet_radvd_if_prefix_record *prefix_record = handle;
    Ip_err                        err = IPCOM_SUCCESS;

    IPNET_RADVD_LOCK();

    if (prefix_record == IP_NULL || prefix_record->magic != IPNET_RADVD_PREFIX_MAGIC)
    {
        IPCOM_LOG0(ERR, "radvd: Invalid handle passed to if_prefix_get");
        err = IPCOM_ERR_INVALID_ARG;
        goto cleanup;
    }

    if (data != IP_NULL)
        *data = prefix_record->data;

cleanup:
    IPNET_RADVD_UNLOCK();
    return err;
}


/*
 *===========================================================================
 *                    ipnet_radvd_if_prefix_set
 *===========================================================================
 * Description: Set prefix configuration data for the specifed handle.
 * Parameters:  handle - Prefix configuration handle.
 *              data -   Pointer to new configuration data.
 * Returns:     IPCOM error code.
 *
 */
IP_PUBLIC Ip_err
ipnet_radvd_if_prefix_set(Ipnet_radvd_if_prefix_handle handle,
                          const Ipnet_radvd_if_prefix_data *data)
{
    Ipnet_radvd_if_prefix_record *prefix_record = handle;
    Ipnet_radvd_if_prefix_data    old;
    Ip_err                        err = IPCOM_SUCCESS;

    IPCOM_LOG3(DEBUG2, "radvd: Set prefix %s for handle %p at %s",
               data->prefix, handle, prefix_record->ifname);

    IPNET_RADVD_LOCK();

    if (prefix_record == IP_NULL || prefix_record->magic != IPNET_RADVD_PREFIX_MAGIC)
    {
        IPCOM_LOG0(ERR, "radvd: Invalid handle passed to if_prefix_set");
        err = IPCOM_ERR_INVALID_ARG;
        goto cleanup;
    }

    /* Save copy of data if we have to restore */
    old = prefix_record->data;
    prefix_record->data = *data;

    IPNET_RADVD_UNLOCK();

    /* Rescan interface */
    err = ipnet_radvd_if_rescan(prefix_record->ifname);
    if (err != IPCOM_SUCCESS)
    {
        /* Error in configuration so restore old one */
        IPNET_RADVD_LOCK();
        prefix_record->data = old;
        IPNET_RADVD_UNLOCK();
        (void)ipnet_radvd_if_rescan(prefix_record->ifname);
    }

    return err;

cleanup:
    IPNET_RADVD_UNLOCK();
    return err;
}

/*
 *===========================================================================
 *                    ipnet_radvd_version
 *===========================================================================
 * Description: Returns the daemon version.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC const char *
ipnet_radvd_version(void)
{
    return "@(#) IPNET router advertisement daemon $Name: VXWORKS_ITER18A_FRZ10 $ - INTERPEAK_COPYRIGHT_STRING";
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
