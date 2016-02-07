/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name$ $RCSfile$ $Revision$
 *     $Source$
 *     $Author$
 *     $State$ $Locker$
 *
 *     INTERPEAK_COPYRIGHT_STRING
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 *
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#define IPCOM_FORWARDER_CTRL_C

#include <ipcom_config.h>

#ifdef IPCOM_USE_FORWARDER

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

/* #define FW_DEBUG */

/* Ipcom include files */
#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_err.h>
#include <ipcom_os.h>
#include <ipcom_syslog.h>

#include <ipcom_cstyle.h>
#include <ipcom_netif.h>
#include <ipcom_pkt.h>
#include <ipcom_syslog.h>
#include <ipcom_type.h>
#include <ipcom_pipe.h>
#include <ipcom_shell.h>
#include <ipcom_getopt.h>
#include <ipcom_atomic.h>


#include <ipnet.h>
#include <ipnet_config.h>
#include <ipnet_netif.h>
#include <ipnet_eth.h>
#include <ipnet_route.h>
#include <ipnet_usr_sock.h>

#include <ipcom_shared_mem.h>
#include <ipcom_ipc_spinlock.h>
#include <ipcom_forwarder.h>

#ifdef IPIPSEC2
#include <ipipsec_config.h>
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_FORWARDER_PIPE_SIZE (50)
#define IPCOM_FORWARDER_MAX_TASKNAME_LEN (10)
#define IPCOM_FORWARDER_OVERWRITE_MAX (10000)
#define IPCOM_FORWARDER_MAX_PACKETS_QUEUED (256 * 4)

#define SADDR_V4_OFFSET(p) ((Ip_u32 *)p + 3)
#define DADDR_V4_OFFSET(p) ((Ip_u32 *)p + 3 + 1)
#define ADDR_V4_OFFSET(p) ((Ip_u64 *)(SADDR_V4_OFFSET(p)))

#define SADDR_V6_OFFSET(p) ((Ip_u8 *)p + 8)
#define DADDR_V6_OFFSET(p) ((Ip_u8 *)p + 8 + 16)
#define DADDR_V6_OFFSET0(p) ((Ip_u64 *)((Ip_u8 *)p + 8 + 16))
#define DADDR_V6_OFFSET1(p) ((Ip_u64 *)((Ip_u8 *)p + 8 + 16 + 8))

/* VLAN */
#define VLAN_TAG(p)             (((Ip_u16 *)(p))[0])

#ifdef IP_PORT_LKM
extern void printk(const char *fmt, ...);
#define ipcom_printf printk
#endif


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */


/* In ipcom_config.c */
IP_EXTERN void ipcom_forwarder_startup_delay(void);
IP_EXTERN int ipcom_forwarder_get_fwcache_util_level_limit(void);
IP_EXTERN void *ipcom_allocate_fw_cache(int entry_size, int *entries);

/* In ipcom_cmd_fwctrl.c */
IP_EXTERN void ipcom_forwarder_cmd_add(void);
#ifdef IPIPSEC_USE_DAEMONS
IP_GLOBAL void       ipipsec_resume_daemons(Ipnet_sig *sig);
IP_GLOBAL Ipnet_sig *ipipsec_suspend_daemons(void);
#endif


#ifdef IPCOM_FORWARDER_AMP
IP_EXTERN Ipcom_shared_mem_pool_t * ipcom_shared_memory_pools_create(void);
IP_EXTERN Ipcom_shared_mem_pool_t * ipcom_shared_mem_pool_for_caller(void);
IP_EXTERN void * ipcom_forwarder_msgq_init(void);
IP_EXTERN int ipcom_forwarder_ifproxy_init(void);
#endif

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

#ifndef IPCOM_FORWARDER_AMP
IP_STATIC Ip_err ipcom_create_rx_fw_tx(int fw_i);
#endif
IP_STATIC int ipcom_forwarder_set_fw_if(Ipcom_pkt *pkt, int port_index);

IPCOM_PROCESS(ipcom_forwarder);

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_PUBLIC int ipcom_forwarder_fw_enable = 1;        /* Start off enabled */
IP_PUBLIC int ipcom_forwarder_fw_started = 0;
IP_PUBLIC Ip_u32 ipcom_forwarder_seed = 0x5a014310; /* Start seed for tag generation */

IP_STATIC Ip_u32 overwrite_cnt = 0;
IP_STATIC Ip_u32 skip_packet_cnt = 0;

#ifdef IPCOM_FORWARDER_AMP
IP_PUBLIC void * fw_msg_q;
#endif

IP_EXTERN Ipcom_fw_t ipcom_fw;

IP_EXTERN int ipcom_forwarder_ipsec_sa_cache_size;


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

#ifdef IPCOM_USE_CALCULATE_KEY
/*
 *===========================================================================
 *                    ipcom_forwarder_calculate_pkt_key
 *===========================================================================
 * Context:     any
 * Description: Calculate key based on 'ipcom_pkt'
 * Parameters:
 * Returns:
 *
 */
IP_STATIC __inline__ Ip_u16
ipcom_forwarder_calculate_pkt_key(Ipcom_pkt *pkt)
{
    Ip_u32 k;
    Ip_u16 *p = (Ip_u16 *)(pkt->data + pkt->start + LL_ETH_HDR_SZ);

    k = ((p[6] + p[7]) << 1) + p[8] + p[9];

    return k + (k >> 16);
}

/*
 *===========================================================================
 *                    ipcom_forwarder_calculate_packet_key
 *===========================================================================
 * Context:     any
 * Description: Calculate key based on 'packet'
 * Parameters:
 * Returns:
 *
 */
IP_STATIC __inline__ Ip_u16
ipcom_forwarder_calculate_packet_key(void *packet)
{
    Ip_u32 k;
    Ip_u16 *p = packet;

    k = ((p[7 + 6] + p[7 + 7]) << 1) + p[7 + 8] + p[7 + 9];

    return k + (k >> 16);
}
#endif /* IPCOM_USE_CALCULATE_KEY */



/*
 *===========================================================================
 *                    ipcom_forwarder_get_v4_update_entry
 *===========================================================================
 * Context:     ipnetd
 * Description: Obtain a cache entry
 * Parameters:
 * Returns:     Pointer to claimed entry
 *
 */
#ifdef IPCOM_USE_FORWARDER_CACHE_WAYS
IP_STATIC fw_cache_entry_v4_t *
ipcom_forwarder_get_v4_update_entry(fw_cache_entry_v4_t *e4, Ip_u32 addr)
{
    int i;


    for (i = 0; i < IPCOM_FORWARDER_CACHE_WAYS; i++)
    {
        if (e4[i].ip4.rci == *ipcom_fw.rt_cache_id && e4[i].ip4.addr == addr)
        {
            /* A valid entry for this flow already exists */
            return e4 + i;
        }
    }

    for (i = 0; i < IPCOM_FORWARDER_CACHE_WAYS; i++)
    {
        if (e4[i].ip4.rci != *ipcom_fw.rt_cache_id)
        {
            /* First unused slot */
#ifdef FW_DEBUG
            if (i)
                ipcom_printf("set %d unused\n", i);
#endif
            return e4 + i;
        }
    }
    /* All slots in set are in use => use random slot */
    i = (ipcom_random() % IPCOM_FORWARDER_CACHE_WAYS);
#ifdef FW_DEBUG
    ipcom_printf("set %d used\n", i);
#endif
    return e4 + i;
}
#endif

/*
 *===========================================================================
 *                    ipcom_forwarder_get_v6_update_entry
 *===========================================================================
 * Context:     ipnetd
 * Description: Obtain a cache entry
 * Parameters:
 * Returns:     Pointer to claimed entry
 *
 */

#ifdef IPCOM_USE_FORWARDER_CACHE_WAYS
IP_STATIC fw_cache_entry_v6_t *
ipcom_forwarder_get_v6_update_entry(fw_cache_entry_v6_t *e6, Ip_u64 *addr)
{
    int i;

    for (i = 0; i < IPCOM_FORWARDER_CACHE_WAYS; i++)
    {
        if (e6[i].ip6.rci == *ipcom_fw.rt_cache_id &&
#ifdef IPCOM_USE_FORWARDER_VLAN
            e6->ip6.type == IPCOM_FORWARDER_PACKET_TYPE_IPV6 &&
#endif
            e6[i].ip6.addr[0] == addr[0] && e6[i].ip6.addr[1] == addr[1])
        {
            /* A valid entry for this flow already exists */
            return e6 + i;
        }
    }

    for (i = 0; i < IPCOM_FORWARDER_CACHE_WAYS; i++)
    {
        if (e6[i].ip6.rci != *ipcom_fw.rt_cache_id)
        {
            /* First unused slot */
#ifdef FW_DEBUG
            if (i)
                ipcom_printf("set %d unused\n", i);
#endif
            return e6 + i;
        }
    }
    /* All slots in set are in use => use random slot */
    i = (ipcom_random() % IPCOM_FORWARDER_CACHE_WAYS);
#ifdef FW_DEBUG
    ipcom_printf("set %d used\n", i);
#endif
    return e6 + i;
}
#endif


/*
 *===========================================================================
 *                    ipcom_forwarder_get_vlan_update_entry
 *===========================================================================
 * Context:     ipnetd
 * Description: Obtain a cache entry
 * Parameters:
 * Returns:     Pointer to claimed entry
 *
 */

#ifdef IPCOM_USE_FORWARDER_CACHE_WAYS
#ifdef IPCOM_USE_FORWARDER_VLAN
IP_STATIC fw_cache_entry_vlan_t *
ipcom_forwarder_get_vlan_update_entry(fw_cache_entry_vlan_t *ev, int vlan_id, Ip_u32 v4addr, Ip_u64 *v6addr)
{
    int i;

    for (i = 0; i < IPCOM_FORWARDER_CACHE_WAYS; i++)
    {

        if (ev[i].ipvlan.rci == *ipcom_fw.rt_cache_id)
        {
            if (!v6addr &&
                ev->ipvlan.type == IPCOM_FORWARDER_PACKET_TYPE_IPV4_VLAN &&
                ev->ipvlan.vlan_id == vlan_id &&
                ev[i].ipvlan.a.addr_v4 == v4addr)
            {
                /* A valid entry for this flow already exists */
                return ev + i;
            }
            else if (v6addr &&
                     ev->ipvlan.type == IPCOM_FORWARDER_PACKET_TYPE_IPV6_VLAN &&
                     ev->ipvlan.vlan_id == vlan_id &&
                     ev[i].ipvlan.a.addr_v6[0] == v6addr[0] &&
                     ev[i].ipvlan.a.addr_v6[1] == v6addr[1])
            {
                /* A valid entry for this flow already exists */
                return ev + i;
            }
        }
    }

    for (i = 0; i < IPCOM_FORWARDER_CACHE_WAYS; i++)
    {
        if (ev[i].ipvlan.rci != *ipcom_fw.rt_cache_id)
        {
            /* First unused slot */
#ifdef FW_DEBUG
            if (i)
                ipcom_printf("set %d unused\n", i);
#endif
            return ev + i;
        }
    }
    /* All slots in set are in use => use random slot */
    i = (ipcom_random() % IPCOM_FORWARDER_CACHE_WAYS);
#ifdef FW_DEBUG
    ipcom_printf("set %d used\n", i);
#endif
    return ev + i;
}
#endif
#endif


/*
 *===========================================================================
 *                    ipcom_forwarder_set_fw_if
 *===========================================================================
 * Context:     ipnetd
 * Description: Add/update an entry in the IP forwarders cache
 * Parameters:
 * Returns:     non-zero if a possibly valid entry was overwritten
 *
 */
IP_STATIC int
ipcom_forwarder_set_fw_if(Ipcom_pkt *pkt, int port_index)
{
    Ip_u32 ix;
    Ip_u8 *p;
    fw_cache_entry_v4_t *e4;
    fw_cache_entry_v6_t *e6;
#ifdef IPCOM_USE_FORWARDER_VLAN
    fw_cache_entry_vlan_t *ev;
#endif
    int fw_ix = port_index;
    Ip_u8 ipsec;
#ifdef IPCOM_USE_FORWARDER_IPSEC
    fw_ipsec_t fw_ipsec;
#endif
    int ret = 0;

#ifdef IPIPSEC_USE_DAEMONS
    Ipnet_sig *suspend;

    /*
     * This function is called by IPNET. The forwarders lock/unlock mechansim requires
     * the IPSec daemons to be suspended since they may try to lock the forwarders too.
     */
    suspend = ipipsec_suspend_daemons();
    if (suspend == IP_NULL)
        return 0;
#endif
    IPCOM_FWD_LOCK();

    p = &pkt->data[pkt->start + LL_ETH_HDR_SZ]; /* Point after an initial Ethernet header */

#ifdef IPCOM_USE_FORWARDER_IPSEC
    ipsec = ipcom_forwarder_ipsec_set_fw_if(p, pkt, ipcom_fw.fw_port[fw_ix].mtu, &fw_ipsec);
    if (ipsec == IPCOM_FWD_IPSEC_NOCACHE)
    {
        IPCOM_FWD_RELEASE();
#ifdef IPIPSEC_USE_DAEMONS
        ipipsec_resume_daemons(suspend);
#endif
        return 0;
    }
    if (ipsec == IPCOM_FWD_IPSEC_CLEARTXT)
        ipcom_memset(&fw_ipsec, 0, sizeof(fw_ipsec));
#else
    ipsec = IPCOM_FWD_IPSEC_CLEARTXT;
#endif /* IPCOM_USE_FORWARDER_IPSEC */

#define IPCOM_FW_PKT_TYPE(p) *((Ip_u16 *)p - 1)

    ix = ipcom_forwarder_pkt_get_key(pkt);

#ifdef FW_DEBUG
    ipcom_printf("ipcom_forwarder_set_fw_if: Adding entry: ix: %d\n", ix);
#endif

    switch(IPCOM_FW_PKT_TYPE(p))
    {
    case ip_htons(IP_IPv4):

        e4 = &ipcom_fw.fw_cache_tbl_v4[ix];

#ifdef IPCOM_USE_FORWARDER_CACHE_WAYS
        e4 = ipcom_forwarder_get_v4_update_entry(e4, *DADDR_V4_OFFSET(p));
#endif

        if (e4->ip4.rci == *ipcom_fw.rt_cache_id && e4->ip4.addr != *DADDR_V4_OFFSET(p))
        {
            ret = 4; /* Potential collision */
        }

#ifdef FW_DEBUG
        ipcom_printf("added IPv4 entry at index %x(%d)\n", ix, ix);
#endif
        e4->ip4.addr = 0;
        e4->ip4.rci = *ipcom_fw.rt_cache_id;
        e4->mac_if.dst_mac[0] = *(Ip_u32 *)(p - LL_ETH_HDR_SZ - 2);
        e4->mac_if.dst_mac[1] = *(Ip_u32 *)(p - LL_ETH_HDR_SZ + 2);
        e4->mac_if.ix[0] = fw_ix;
#if 0
        ipcom_fw.fw_port[fw_ix].src_type[0] = *(Ip_u32 *)(p - 8);
#ifdef IP_BIG_ENDIAN
        ipcom_fw.fw_port[fw_ix].src_type[1] = (*(Ip_u16 *)(p - 4) << 16) + ip_htons(IP_IPv6);
#else
        ipcom_fw.fw_port[fw_ix].src_type[1] =  (ip_htons(IP_IPv6) << 16) + *(Ip_u16 *)(p - 4);
#endif
#endif
        e4->mac_if.enc[1] = ipsec;
#ifdef IPCOM_USE_FORWARDER_IPSEC
        ipcom_memcpy(&e4->ipsec, &fw_ipsec, sizeof(fw_ipsec));
#endif
        e4->ip4.addr = *DADDR_V4_OFFSET(p);
        break;

    case ip_htons(IP_IPv6):
        e6 = &ipcom_fw.fw_cache_tbl_v6[ix];
#ifdef IPCOM_USE_FORWARDER_CACHE_WAYS
        e6 = ipcom_forwarder_get_v6_update_entry(e6, DADDR_V6_OFFSET0(p));
#endif
        e6->ip6.addr[0] = 0;
        e6->ip6.addr[1] = 0;
        if (e6->ip6.rci == *ipcom_fw.rt_cache_id)
        {
            ret = 6;
        }

#ifdef FW_DEBUG
        ipcom_printf("added IPv6 entry at index %x(%d)\n", ix, ix);
#endif

        e6->ip6.rci = *ipcom_fw.rt_cache_id;
        e6->mac_if.dst_mac[0] = *(Ip_u32 *)(p - LL_ETH_HDR_SZ - 2);
        e6->mac_if.dst_mac[1] = *(Ip_u32 *)(p - LL_ETH_HDR_SZ + 2);
        e6->mac_if.ix[0] = fw_ix;
#if 0
        ipcom_fw.fw_port[fw_ix].src_type[0] = *(Ip_u32 *)(p - 8);
        ipcom_fw.fw_port[fw_ix].src_type[1] = *(Ip_u32 *)(p - 4);
#endif
        e6->mac_if.enc[1] = ipsec;
#ifdef IPCOM_USE_FORWARDER_IPSEC
        ipcom_memcpy(&e6->ipsec, &fw_ipsec, sizeof(fw_ipsec));
#endif
#ifdef IPCOM_USE_FORWARDER_VLAN
        e6->ip6.type = IPCOM_FORWARDER_PACKET_TYPE_IPV6;
#endif
        e6->ip6.addr[0] = *DADDR_V6_OFFSET0(p);
        e6->ip6.addr[1] = *DADDR_V6_OFFSET1(p);
        break;

#ifdef IPCOM_USE_FORWARDER_VLAN

    case ip_htons(IP_VLAN):
        ev = (fw_cache_entry_vlan_t *)&ipcom_fw.fw_cache_tbl_v6[ix];
        p += LL_ETH_VLAN_ADD;

        if (IPCOM_FW_PKT_TYPE(p) == ip_htons(IP_IPv4))
        {
#ifdef IPCOM_USE_FORWARDER_CACHE_WAYS
            ev = ipcom_forwarder_get_vlan_update_entry(ev, VLAN_TAG(p - LL_ETH_VLAN_ADD), *DADDR_V4_OFFSET(p), IP_NULL);
#endif
            ev->ipvlan.a.addr_v4 = 0;
            if (ev->ipvlan.rci == *ipcom_fw.rt_cache_id)
            {
                ret = 6;
            }
            ev->ipvlan.rci = *ipcom_fw.rt_cache_id;
            ev->mac_if.dst_mac[0] = *(Ip_u32 *)(p - LL_ETH_HDR_SZ - LL_ETH_VLAN_ADD - 2);
            ev->mac_if.dst_mac[1] = *(Ip_u32 *)(p - LL_ETH_HDR_SZ - LL_ETH_VLAN_ADD + 2);
            ev->mac_if.ix[0] = fw_ix;
#if 0
            ipcom_fw.fw_port[fw_ix].src_type[0] = *(Ip_u32 *)(p - LL_ETH_VLAN_ADD - 8);
#ifdef IP_BIG_ENDIAN
            ipcom_fw.fw_port[fw_ix].src_type[1] = (*(Ip_u16 *)(p - LL_ETH_VLAN_ADD - 4) << 16) + ip_htons(IP_IPv6);
#else
            ipcom_fw.fw_port[fw_ix].src_type[1] =  (ip_htons(IP_IPv6) << 16) + *(Ip_u16 *)(p - LL_ETH_VLAN_ADD - 4);
#endif
#endif
            ev->mac_if.enc[1] = ipsec;
#ifdef IPCOM_USE_FORWARDER_IPSEC
            ipcom_memcpy(&ev->ipsec, &fw_ipsec, sizeof(fw_ipsec));
#endif
            ev->ipvlan.type = IPCOM_FORWARDER_PACKET_TYPE_IPV4_VLAN;
            ev->ipvlan.vlan_id = VLAN_TAG(p - LL_ETH_VLAN_ADD);
            ev->ipvlan.a.addr_v4 = *DADDR_V4_OFFSET(p);
        }

        else if (IPCOM_FW_PKT_TYPE(p) == ip_htons(IP_IPv6))
        {
#ifdef IPCOM_USE_FORWARDER_CACHE_WAYS
            ev = ipcom_forwarder_get_vlan_update_entry(ev, VLAN_TAG(p - LL_ETH_VLAN_ADD), 0, DADDR_V6_OFFSET0(p));
#endif
            ev->ipvlan.a.addr_v6[0] = 0;
            ev->ipvlan.a.addr_v6[1] = 0;
            if (ev->ipvlan.rci == *ipcom_fw.rt_cache_id)
            {
                ret = 6;
            }
            ev->ipvlan.rci = *ipcom_fw.rt_cache_id;
            ev->mac_if.dst_mac[0] = *(Ip_u32 *)(p - LL_ETH_HDR_SZ - LL_ETH_VLAN_ADD - 2);
            ev->mac_if.dst_mac[1] = *(Ip_u32 *)(p - LL_ETH_HDR_SZ - LL_ETH_VLAN_ADD + 2);
            ev->mac_if.ix[0] = fw_ix;
#if 0
            ipcom_fw.fw_port[fw_ix].src_type[0] = *(Ip_u32 *)(p - LL_ETH_VLAN_ADD - 8);
#ifdef IP_BIG_ENDIAN
            ipcom_fw.fw_port[fw_ix].src_type[1] = (*(Ip_u16 *)(p - LL_ETH_VLAN_ADD - 4) << 16) + ip_htons(IP_IPv6);
#else
            ipcom_fw.fw_port[fw_ix].src_type[1] =  (ip_htons(IP_IPv6) << 16) + *(Ip_u16 *)(p - LL_ETH_VLAN_ADD - 4);
#endif
#endif
            ev->mac_if.enc[1] = ipsec;
#ifdef IPCOM_USE_FORWARDER_IPSEC
            ipcom_memcpy(&ev->ipsec, &fw_ipsec, sizeof(fw_ipsec));
#endif
            ev->ipvlan.type = IPCOM_FORWARDER_PACKET_TYPE_IPV6_VLAN;
            ev->ipvlan.vlan_id = VLAN_TAG(p - LL_ETH_VLAN_ADD);
            ev->ipvlan.a.addr_v6[0] = *DADDR_V6_OFFSET0(p);
            ev->ipvlan.a.addr_v6[1] = *DADDR_V6_OFFSET1(p);
        }

        break;
#endif
    default:

        break;
    }

    IPCOM_FWD_RELEASE();
#ifdef IPIPSEC_USE_DAEMONS
    ipipsec_resume_daemons(suspend);
#endif
    return ret;
}


#ifndef IPCOM_FORWARDER_AMP
/*
 *===========================================================================
 *                    ipcom_create_rx_fw_tx
 *===========================================================================
 * Context:
 * Description:     Creates a combined receiver, forwarder, and transmitter
 *                  daemon.
 * Parameters:      fw_i: Index of forwarder (0 - 9)
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_create_rx_fw_tx(int fw_i)
{
    int retval;
    Ip_pid_t pid;
    Ipcom_proc_attr attr;
    char name[IPCOM_FORWARDER_MAX_TASKNAME_LEN];


    ipcom_proc_attr_init(&attr);
    attr.priority  = IPCOM_PRIORITY_MIN;
    attr.stacksize = IPCOM_PROC_STACK_LARGE;

    /* Create the Forwarder task name */
    ipcom_sprintf(name, "%s%d", IP_FORWARDER_TASK_NAME, fw_i + 1);

    /* Spawn a new forwarder task */
    retval = ipcom_proc_acreate(name, (Ipcom_proc_func)ipcom_forwarder, &attr, &pid);

    return retval;
}
#endif /* IPCOM_FORWARDER_AMP */

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#ifdef IPCOM_USE_FORWARDER_UPDATE_CACHE_LOCK
/*
 *===========================================================================
 *                    ipcom_forwarder_update_cache_lock
 *===========================================================================
 * Context:
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_forwarder_update_cache_lock(void)
{
    int fw_cnt = ipcom_forwarder_get_fw_cnt();

    /* Tell forwarders to wait */
    FW_LOCK->update_cache_lock = 1;
    /* Sync */
    ipcom_mb();
    /* Busy wait until all forwarders have seen the lock */
    while (ipcom_atomic_get(&FW_LOCK->update_cache_count) < fw_cnt)
    {
    }
}


/*
 *===========================================================================
 *                    ipcom_forwarder_update_cache_unlock
 *===========================================================================
 * Context:
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_forwarder_update_cache_unlock(void)
{
    /* Force cache update to memory */
    ipcom_mb();
    /* Tell forwarders to go */
    FW_LOCK->update_cache_lock = 0;
    /* Sync */
    ipcom_mb();
    /* Busy wait until all forwarders have seen the release */
    while (ipcom_atomic_get(&FW_LOCK->update_cache_count) > 0)
    {
    }
}
#endif /* IPCOM_USE_FORWARDER_UPDATE_CACHE_LOCK */


#ifdef IPCOM_FORWARDER_LOCAL_STACK
/*
 *===========================================================================
 *                    ipcom_forwarder_ioctl
 *===========================================================================
 * Context:
 * Description:
 * Parameters:
 *
 */
#ifndef IP_PORT_LKM
IP_PUBLIC Ip_err
ipcom_forwarder_ioctl(Ipcom_netif *netif, int code, struct Ip_fwreq *fwreq)
{
    port_statistics_t ps;
    void * port_cookie;

    if (netif->hw_ix < 0)
    {
        return -1;
    }
    port_cookie = ipcom_fw.fw_port[netif->hw_ix].fwd;

    switch (code)
    {
    case IP_SIOCGHWSTATS:

#ifdef IP_PORT_VXWORKS
        ipcom_forwarder_get_statistics(port_cookie, &ps);

        fwreq->u.ps.packets = ps.packets;
        fwreq->u.ps.octets_hw = (Ip_u32)(ps.inb_octets >> 32);
        fwreq->u.ps.octets_lw = (Ip_u32)ps.inb_octets;

        fwreq->u.ps.multicast_packets = ps.multicast_packets;
        fwreq->u.ps.broadcast_packets = ps.broadcast_packets;

        fwreq->u.ps.len_64_packets = ps.len_64_packets;
        fwreq->u.ps.len_65_127_packets = ps.len_65_127_packets;
        fwreq->u.ps.len_128_255_packets = ps.len_128_255_packets;
        fwreq->u.ps.len_256_511_packets = ps.len_256_511_packets;
        fwreq->u.ps.len_512_1023_packets = ps.len_512_1023_packets;
        fwreq->u.ps.len_1024_1518_packets = ps.len_1024_1518_packets;
        fwreq->u.ps.len_1519_max_packets = ps.len_1519_max_packets;

        fwreq->u.ps.dropped_packets = ps.dropped_packets;
        fwreq->u.ps.dropped_octets = ps.dropped_octets;

        fwreq->u.ps.tx_packets = ps.tx_packets;
        fwreq->u.ps.tx_octets_hw = (Ip_u32)(ps.tx_octets >> 32);
        fwreq->u.ps.tx_octets_lw = (Ip_u32)ps.tx_octets;

        break;

    case IP_SIOCSRESETHWSTATS:

        ipcom_forwarder_reset_statistics(port_cookie);
        break;
#endif
    default:

        break;
    }

    return 0;
}
#endif
#endif /* IPCOM_FORWARDER_LOCAL_STACK */

/*
 *===========================================================================
 *                    ipcom_forwarder_start_stop_fw
 *===========================================================================
 * Context:     shell
 * Description: Start/stop forwarders by send messages and trigger them
 * Parameters:  sig: IPCOM_SIG_STOP_FW or IPCOM_SIG_START_FW
 *
 */
IP_GLOBAL void
ipcom_forwarder_start_stop_fw(int sig)
{
#ifdef IPCOM_FORWARDER_AMP
    /* TODO */
#else
    int i;
    int fw_cnt;
    static int stop_msg = IPCOM_SIG_STOP_FW;
    static int start_msg = IPCOM_SIG_START_FW;
    if (sig == IPCOM_SIG_START_FW)
    {
        ipcom_forwarder_fw_started = 1;
    }

    fw_cnt = ipcom_forwarder_get_fw_cnt();
    for (i = 0; i < fw_cnt; i++)
    {
        ipcom_pipe_send(ipcom_fw.control_pipe[i], (sig == IPCOM_SIG_STOP_FW) ? (int *)&stop_msg : (int *)&start_msg);
    }
    if (sig == IPCOM_SIG_STOP_FW)
    {
        for (i = 0; i < fw_cnt; i++)
        {
            ipcom_forwarder_trigger_pipe_read(i+1); /* i + 1 is forwarder index */
        }
        ipcom_forwarder_fw_started = 0;
    }
#endif /* IPCOM_FORWARDER_AMP */
}

/*
 *===========================================================================
 *                    ipcom_forwarder_get_fwdebug_mask
 *===========================================================================
 * Context:     shell
 * Description:
 * Returns:     current value of 'FW_DEBUG'
 *
 */
IP_GLOBAL int
ipcom_forwarder_get_fwdebug_mask(void)
{
#ifdef FW_DEBUG
    return FW_DEBUG;
#else
    return 0;
#endif
}

/*
 *===========================================================================
 *                    ipcom_forwarder_get_fw_enable
 *===========================================================================
 * Context:     shell
 * Description:
 * Returns:     current value of ipcom_forwarder_fw_enable
 *
 */
IP_GLOBAL int
ipcom_forwarder_get_fw_enable(void)
{
    return ipcom_forwarder_fw_enable;
}

/*
 *===========================================================================
 *                    ipcom_forwarder_get_fw_started
 *===========================================================================
 * Context:     shell
 * Description:
 * Returns:     current value of fw_started
 *
 */
IP_GLOBAL int
ipcom_forwarder_get_fw_started(void)
{
    return ipcom_forwarder_fw_started;
}

/*
 *===========================================================================
 *                    ipcom_forwarder_cache_util
 *===========================================================================
 * Context:     shell
 * Description: Retrieve the current fw cache usage in percent
 * Return:      Fw cache usage in percent
 *
 */
IP_PUBLIC int
ipcom_forwarder_cache_util(int v4_v6)
{
    int i;
    int used = 0;
    int entries = 0;
    fw_cache_entry_v4_t *fw4;
    fw_cache_entry_v6_t *fw6;
    int limit;

    if (v4_v6 & IPCOM_FWD_V4)
    {
#if IPCOM_FORWARDER_TRUE_CACHE_WAYS_SHIFT > 0
        limit = ipcom_fw.fw_v4_entries << IPCOM_FORWARDER_TRUE_CACHE_WAYS_SHIFT;
#else
        limit = ipcom_fw.fw_v4_entries + IPCOM_FORWARDER_CACHE_WAYS - 1;
#endif
        for (i = 0, fw4 = ipcom_fw.fw_cache_tbl_v4; i < limit; i++)
        {
            entries++;
            if (fw4[i].ip4.rci == *ipcom_fw.rt_cache_id)
            {
                used++;
            }
        }
    }

    if (v4_v6 & IPCOM_FWD_V6)
    {
#if IPCOM_FORWARDER_TRUE_CACHE_WAYS_SHIFT > 0
        limit = ipcom_fw.fw_v6_entries << IPCOM_FORWARDER_TRUE_CACHE_WAYS_SHIFT;
#else
        limit = ipcom_fw.fw_v6_entries + IPCOM_FORWARDER_CACHE_WAYS - 1;
#endif
        for (i = 0, fw6 = ipcom_fw.fw_cache_tbl_v6; i < limit; i++)
        {
            entries++;
            if (fw6[i].ip6.rci == *ipcom_fw.rt_cache_id)
            {
                used++;
            }
        }
    }

    if (entries)
    {
        return 100 * used / entries;
    }
    return 0;
}

/*
 *===========================================================================
 *                    ipcom_forwarder_route_cache_invalidate
 *===========================================================================
 * Context:     control NAE
 * Description: quick, global invalidate of all forwarding cache entries
 * Parameters:
 *
 * This routine works by incrementing the shared route_cache_id counter.
 * Note that if the 32-bit route cache ID wraps around, it is possible
 * though unlikely that an old invalid route cache entry becomes valid
 * again; to prevent this, we call the slower ipcom_forwarder_invalidate()
 * in case of wraparound.
 */
IP_PUBLIC void
ipcom_forwarder_route_cache_invalidate(void)
{
    volatile int *rt_cache_id = ipcom_fw.rt_cache_id;
    int id = *rt_cache_id;

    if (++id == 0)
    {
        id = 1;
        ipcom_forwarder_invalidate(-1, IPCOM_FWD_V4|IPCOM_FWD_V6);
    }

    *rt_cache_id = id;
}

/*
 *===========================================================================
 *                    ipcom_forwarder_invalidate
 *===========================================================================
 * Context:     any
 * Description: Invalidate fw cache entries
 * Parameters:  id : index of entry to invalidate;
 *                   a negative value means all entries
 *
 * When IPCOM_FORWARDER_TRUE_CACHE_WAYS_SHIFT is
 * greater than zero, then the index should be the set
 * index rather than the entry index. In this case, all ways
 * of the set will be invalidated.
 */
IP_PUBLIC void
ipcom_forwarder_invalidate(int id, int v4_v6)
{
    int i;
    fw_cache_entry_v4_t *fw4;
    fw_cache_entry_v6_t *fw6;
    int limit;

    if (v4_v6 & IPCOM_FWD_V4)
    {
#if IPCOM_FORWARDER_TRUE_CACHE_WAYS_SHIFT > 0
        limit = ipcom_fw.fw_v4_entries << IPCOM_FORWARDER_TRUE_CACHE_WAYS_SHIFT;
#else
        limit = ipcom_fw.fw_v4_entries + IPCOM_FORWARDER_CACHE_WAYS - 1;
#endif
        for (i = 0, fw4 = ipcom_fw.fw_cache_tbl_v4; i < limit; i++)
        {
            if (id < 0 || id == i >> IPCOM_FORWARDER_TRUE_CACHE_WAYS_SHIFT)
            {
                fw4[i].ip4.rci = 0;
            }
        }
    }
    if (v4_v6 & IPCOM_FWD_V6)
    {
#if IPCOM_FORWARDER_TRUE_CACHE_WAYS_SHIFT > 0
        limit = ipcom_fw.fw_v6_entries << IPCOM_FORWARDER_TRUE_CACHE_WAYS_SHIFT;
#else
        limit = ipcom_fw.fw_v6_entries + IPCOM_FORWARDER_CACHE_WAYS - 1;
#endif
        for (i = 0, fw6 = ipcom_fw.fw_cache_tbl_v6; i < limit; i++)
        {
            if (id < 0 || id == i >> IPCOM_FORWARDER_TRUE_CACHE_WAYS_SHIFT)
            {
                fw6[i].ip6.rci = 0;
            }
        }
    }
}

/*
 *===========================================================================
 *                    ipcom_forwarder_set_seed
 *===========================================================================
 * Context:     shell
 * Description: Set the packet 'tag' calculation seed (used to generate flow index)
 * Parameters:  'seed': 32 bit "random" value
 *
 */
IP_PUBLIC void
ipcom_forwarder_set_seed(Ip_u32 seed)
{
    ipcom_forwarder_seed = seed;
    ipcom_forwarder_set_tag_seed(seed);
    ipcom_forwarder_invalidate(-1, IPCOM_FWD_V4_V6);
    skip_packet_cnt = IPCOM_FORWARDER_MAX_PACKETS_QUEUED;
}

/*
 *===========================================================================
 *                    ipcom_forwarder_get_seed
 *===========================================================================
 * Context:     shell
 * Description: Get the current packet 'tag' calculation seed
 * Return:      32 bit "random" value
 *
 */
IP_PUBLIC Ip_u32
ipcom_forwarder_get_seed(void)
{
    return ipcom_forwarder_seed;
}


/*
 *===========================================================================
 *                    ipcom_forwarder_print_cache
 *===========================================================================
 * Context:     shell
 * Description: Print cache statistics
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_forwarder_print_cache(int v)
{
    ipcom_printf("No cache info available\n");
}

/*
 *===========================================================================
 *                    ipcom_forwarder_transmit_pkt_ext
 *===========================================================================
 * Context:     ipnetd
 * Description: Forwarder output processing (update forwarding cache)
 * Parameters:  pkt - the packet being sent
 *              port_index - the forwarder output port index
 * Returns: 0 : packet was handled by this function
 *          1 : packet was not handled by this function and should be transmitted by ipnet
 *
 * Note: currently, always returns 1.
 */
IP_PUBLIC int
ipcom_forwarder_transmit_pkt_ext(Ipcom_pkt *pkt, int port_index)
{
    int v;

    if (!ipcom_forwarder_fw_started)
        return 1;

    if (!ipcom_forwarder_fw_enable)
        return 1;

    if (!skip_packet_cnt)
    {
        if ((v = ipcom_forwarder_set_fw_if(pkt, port_index)) > 0)
        {
            /* Overwrote a valid entry */
            if (++overwrite_cnt > IPCOM_FORWARDER_OVERWRITE_MAX)
            {
                if (ipcom_forwarder_cache_util((v == 4) ? IPCOM_FWD_V4 : IPCOM_FWD_V6) < ipcom_forwarder_get_fwcache_util_level_limit())
                {
                    struct Ip_timeval now;

                    ipcom_microtime(&now);
                    ipcom_forwarder_seed ^= (Ip_u32)(now.tv_usec + (now.tv_usec << 17) + (now.tv_usec >> 5));
#ifdef FW_DEBUG
                    ipcom_printf("Updated tag seed: %x\n", ipcom_forwarder_seed);
#endif
                    ipcom_forwarder_set_tag_seed(ipcom_forwarder_seed);
                    skip_packet_cnt = IPCOM_FORWARDER_MAX_PACKETS_QUEUED;
                    ipcom_forwarder_invalidate(-1, IPCOM_FWD_V4_V6);
                }
                overwrite_cnt = 0;
            }
        }
    }
    else
    {
        skip_packet_cnt--;
    }

    return 1;
}

#ifdef IPCOM_FORWARDER_LOCAL_STACK
/*
 *===========================================================================
 *                    ipcom_forwarder_transmit_pkt
 *===========================================================================
 * Context:     ipnetd
 * Description: Send packet via forwarder
 * Parameters:
 * Returns: 0 : packet was handled by this function
 *          1 : packet was not handled by this function and should be transmitted by ipnet
 *
 */
IP_PUBLIC int
ipcom_forwarder_transmit_pkt(Ipcom_pkt *pkt, Ipcom_netif *netif)
{

#ifdef IPCOM_USE_FORWARDER_IPSEC
    if (pkt->ipsec_hlen && pkt->next_original)
    {
        int offset = LL_ETH_HDR_SZ;

        if (netif->type == IP_IFT_L2VLAN)
            offset += LL_ETH_VLAN_ADD;
        /* Restore original packet start pointer */
        pkt->next_original->start = pkt->next_original->ipstart - offset;
        /* Copy link layer header to original packet */
        ipcom_memcpy(&pkt->next_original->data[pkt->next_original->start],
                     &pkt->data[pkt->start],
                     offset);
        pkt->next_original->ipsec_hlen = pkt->ipsec_hlen;
        pkt = pkt->next_original;

        if ((pkt->data[pkt->start + offset] & 0xf0) == 0x40)
            IP_SET_HTONS(&pkt->data[pkt->start + offset - 2], IP_IPv4);
        else if ((pkt->data[pkt->start + offset] & 0xf0) == 0x60)
            IP_SET_HTONS(&pkt->data[pkt->start + offset - 2], IP_IPv6);
        else
            return 1;
    }
#endif /* IPCOM_USE_FORWARDER_IPSEC */

#ifdef IPCOM_USE_FORWARDER_VLAN
    if (netif->type == IP_IFT_L2VLAN)
        netif = (Ipcom_netif *)(((Ipnet_netif *)netif)->eth->vlan.parent);
#endif

    if (netif->hw_ix < 0)
        return 1;

    /* TODO: LKM setting pkt->nae_ctx */

    if (!IS_FORWARDER_PKT(pkt->nae_ctx))
        return 1;

    if (!IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_FORWARDED))
        return 1;

#ifdef IPCOM_USE_NEHALEM_FORWARDER
    /*
     * Temporary, for test purposes. Eventually, this routine won't
     * be called at all for the Nehalem AMP forwarder initial release
     * (without the NAE stack).
     */
    return 1;
#else
    return ipcom_forwarder_transmit_pkt_ext(pkt, netif->hw_ix);
#endif
}

#else  /*  IPCOM_FORWARDER_LOCAL_STACK */
/*
 * TODO: In the initial Nehalem forwarder release, there will be
 * no IPNET stack on the NAEs, and IPsec is not supported either.
 *
 * Any packet sent over ifproxy by the management stack is examined;
 * if it is an IP packet, we use it to update the routing cache
 * before sending it.  We need to write a routine semi-analogous
 * to ipcom_forwarder_transmit_pkt() / ipcom_forwarder_set_fw_if()
 * above to handle the routing cache update.
 *
 * When the NAE stack is added, the NAE stack
 * forwarding information base is updated based upon NETLINK messages
 * from the NAD, and packets that miss in the routing cache are
 * forwarded (usually) through the NAE stack and ipcom_forwarder_transmit_pkt()
 * above is called to update the routing cache.  But without the NAE
 * stack, we just invalidate the routing cache upon receipt of any NETLINK
 * message.
 */
#endif /* IPCOM_FORWARDER_LOCAL_STACK */

/*
 *===========================================================================
 *                    ipcom_forwarder_register_port
 *===========================================================================
 * Context:      any (initialization; no mutual exclusion)
 * Description:  set the forwarder port cookie for a specified
 *               port index
 * Parameters:   port - the forwarder port index
 *               port_cookie - the forwarder port cookie
 *               mtu - the port's MTU
 *               hwaddr - the ports ethernet station address
 */
IP_PUBLIC void
ipcom_forwarder_register_port(int port, void *port_cookie,
                              Ip_u32 mtu, const Ip_u8 *hwaddr)
{
    fw_port_t *fw_port;

    if (port >= 0 && port < MAX_FW_PORTS)
    {
        fw_port = &ipcom_fw.fw_port[port];
        fw_port->fwd = port_cookie;
        fw_port->mtu = mtu;
        if (hwaddr != IP_NULL)
        {
            ipcom_memcpy(&fw_port->src_type, hwaddr, LL_ETH_ADDR_SZ);
            *((Ip_u16 *)fw_port->src_type + 3) = ip_htons(IP_IPv6);
        }
    }
}


/*
 *===========================================================================
 *                    ipcom_forwarder_set_port_cookie
 *===========================================================================
 * Context:      any (initialization; no mutual exclusion)
 * Description:  set the forwarder port cookie for a specified
 *               port index
 */
IP_PUBLIC void
ipcom_forwarder_set_port_cookie(int port, void *port_cookie)
{
    if (port >= 0 && port < MAX_FW_PORTS)
    {
        ipcom_fw.fw_port[port].fwd = port_cookie;
    }
}

/*
 *===========================================================================
 *                    ipcom_forwarder_port_mac_update
 *===========================================================================
 * Context:      any
 * Description:  Change the MAC station address of a forwarder port.
 * Parameters:   port - the port index
 *               hwaddr - pointer to the first byte of the port's new
 *                        ethernet station address.
 */
IP_PUBLIC void
ipcom_forwarder_port_mac_update(int port, Ip_u8 *hwaddr)
{
    fw_port_t *fw_port;

    if (port >= 0 && port < MAX_FW_PORTS && hwaddr != IP_NULL)
    {
        fw_port = &ipcom_fw.fw_port[port];
        ipcom_memcpy(&fw_port->src_type, hwaddr, LL_ETH_ADDR_SZ);
        *((Ip_u16 *)fw_port->src_type + 3) = ip_htons(IP_IPv6);
    }
}

/*
 *===========================================================================
 *                    ipcom_forwarder_port_mtu_update
 *===========================================================================
 * Context:      any
 * Description:  Change the MTU of a forwarder port.
 * Parameters:   port - the port index
 *               mtu - the port's new mtu
 */
IP_PUBLIC void
ipcom_forwarder_port_mtu_update(int port, Ip_u32 mtu)
{
    fw_port_t *fw_port;

    if (port >= 0 && port < MAX_FW_PORTS)
    {
        fw_port = &ipcom_fw.fw_port[port];
        fw_port->mtu = mtu;
    }
}

/*
 *===========================================================================
 *                    ipcom_forwarder_get_port_cookie
 *===========================================================================
 * Context:         any
 * Description:
 *
 */
IP_PUBLIC void *
ipcom_forwarder_get_port_cookie(int port)
{
    if (port >= 0 && port < MAX_FW_PORTS)
    {
        return ipcom_fw.fw_port[port].fwd;
    }
    return IP_NULL;
}


#ifdef IPCOM_FORWARDER_AMP
IP_PUBLIC unsigned int
ipcom_forwarder_init(void)
{
    static Ip_bool ipcom_fw_initialized = IP_FALSE;
    unsigned int cpu;

    /*
     * Note, this routine is called not only from ipcom_forwarder_create(),
     * but (indirectly) from driver ...InstInit2() routines, to ensure that
     * the shared memory pools have been initialized when the driver uses them.
     * It must be idempotent.
     */
    cpu = ipcom_forwarder_master_core();

    if (ipcom_fw_initialized) /* only do it once. TODO: use ipcom_once() */
        return cpu;

    ipcom_shared_memory_pools_create(); /* may need this much earlier
                                           for use by driver startup?
                                           If more than one VB manages
                                           and brings up network ports,
                                           who does
                                             ipcom_shared_memory_pools_create()?
                                           Ignore that possibility for now.
                                         */

    ipcom_fw.fw_port =
        ipcom_fw_cpu_mem_alloc(cpu,
                               MAX_FW_PORTS * sizeof(fw_port_t),
                               ipcom_cache_line_size(),
                               IPCOM_FW_CPU_MEM_ALLOC_SHARED);


    ipcom_fw_initialized = IP_TRUE;
    ipcom_mb();
    return cpu;
}
#endif /* IPCOM_FORWARDER_AMP */

/*
 *===========================================================================
 *                    ipcom_forwarder_create
 *===========================================================================
 * Context:         init, master forwarder core only.
 * Description:     Create the number of forwarders requested by the kernel
 *                  configuration
 *
 */
IP_PUBLIC int
ipcom_forwarder_create(void)
{
#ifdef IPCOM_FORWARDER_AMP
    unsigned int cpu;
#else
    int i;
    int fw_cnt;
    Ipcom_pipe_attr pattr;
#endif

#ifdef IPCOM_FORWARDER_AMP
    if (!ipcom_forwarder_is_master())
    {
        /* Just in case we somehow get called from some non-master
           forwarder core... */
        return IPCOM_ERR_PERMISSION_DENIED;
    }
#endif

#ifdef IP_PORT_LKM
    ipcom_memset(&ipcom_fw, 0, sizeof(ipcom_fw));
#endif

#ifdef IPCOM_FORWARDER_AMP
#ifdef IPCOM_USE_NEHALEM_FORWARDER
    ipcom_forwarder_ifproxy_init();
#else
    ipcom_forwarder_setup_hw(); /* start network devices, etc. */
#endif
    cpu = ipcom_forwarder_init();
#endif /* IPCOM_FORWARDER_AMP */

    ipcom_fw.rt_cache_id  = ipnet_usr_sock_get_route_cache_id();

#ifdef IPCOM_USE_FORWARDER_UPDATE_CACHE_LOCK
#ifdef IPCOM_FORWARDER_AMP
    ipcom_fw.fw_lock = ipcom_fw_cpu_mem_alloc(cpu,
                                              sizeof(fw_cache_lock_t),
                                              ipcom_cache_line_size(),
                                              IPCOM_FW_CPU_MEM_ALLOC_SHARED);
#endif
    FW_LOCK->update_cache_lock = 0;
    ipcom_atomic_set(&FW_LOCK->update_cache_count, 0);
#endif

    /* for now, only use a single forwarding cache for all cores... */
    ipcom_fw.fw_cache_tbl_v4 = ipcom_allocate_fw_cache(sizeof(fw_cache_entry_v4_t), &ipcom_fw.fw_v4_entries);
    ipcom_fw.fw_cache_tbl_v6 = ipcom_allocate_fw_cache(sizeof(fw_cache_entry_v6_t), &ipcom_fw.fw_v6_entries);

#ifdef IPCOM_USE_FORWARDER_IPSEC
    /*
     * Hmm, this algorithm will result in ipcom_fw.fw_ipsec_entries smaller
     * than the requested ipcom_forwarder_ipsec_sa_cache_size if the latter
     * is not a power of two...
     */
    for (i=15; i>=0; i--)
    {
        if (IP_BIT_ISSET(ipcom_forwarder_ipsec_sa_cache_size, (1 << i)))
        {
            ipcom_fw.fw_ipsec_entries = 1 << i;
            break;
        }
    }

#ifdef IPCOM_FORWARDER_AMP
    ipcom_fw.fw_cache_tbl_enc =
        ipcom_fw_cpu_mem_alloc(cpu,
                               sizeof(Ipcom_fw_ipsec_sa_t) * ipcom_fw.fw_ipsec_entries,
                               sizeof(Ipcom_fw_ipsec_sa_t),
                               IPCOM_FW_CPU_MEM_ALLOC_SHARED);
    ipcom_fw.fw_cache_tbl_dec =
        ipcom_fw_cpu_mem_alloc(cpu,
                               sizeof(Ipcom_fw_ipsec_sa_t) * ipcom_fw.fw_ipsec_entries,
                               sizeof(Ipcom_fw_ipsec_sa_t),
                               IPCOM_FW_CPU_MEM_ALLOC_SHARED);
#else  /* non-AMP */
    ipcom_fw.fw_cache_tbl_enc =
        ipcom_forwarder_alloc_aligned(sizeof(Ipcom_fw_ipsec_sa_t),
                                      sizeof(Ipcom_fw_ipsec_sa_t) * ipcom_fw.fw_ipsec_entries);
    ipcom_fw.fw_cache_tbl_dec =
        ipcom_forwarder_alloc_aligned(sizeof(Ipcom_fw_ipsec_sa_t),
                                      sizeof(Ipcom_fw_ipsec_sa_t) * ipcom_fw.fw_ipsec_entries);
#endif /* non-AMP */

#ifdef FW_IPSEC_DEBUG
    ipcom_printf("FW_IPSEC :: allocated ipsec encrypt cache at %p, total bytes = %d, entry = %d bytes\n",
                 ipcom_fw.fw_cache_tbl_enc, sizeof(Ipcom_fw_ipsec_sa_t) * ipcom_fw.fw_ipsec_entries, sizeof(Ipcom_fw_ipsec_sa_t));
    ipcom_printf("FW_IPSEC :: allocated ipsec decrypt cache at %p, total bytes = %d, entry = %d bytes\n",
                 ipcom_fw.fw_cache_tbl_dec, sizeof(Ipcom_fw_ipsec_sa_t) * ipcom_fw.fw_ipsec_entries, sizeof(Ipcom_fw_ipsec_sa_t));
#endif
    ipcom_forwarder_ipsec_populate_func_table();
#endif /* IPCOM_USE_FORWARDER_IPSEC */

#ifndef IPCOM_FORWARDER_AMP
    pattr.msg_limit = IPCOM_FORWARDER_PIPE_SIZE;
    ipcom_fw.fw_pipe = ipcom_pipe_new(IPCOM_PIPE_GENERIC, &pattr);
#endif

#ifdef IPCOM_FORWARDER_USE_UPPER_TAG_BITS
    /* Set up key shift value (based on cache size and number of sets)*/
    ipcom_fw.fw_key_shift = ipcom_forwarder_get_key_shift();
#ifdef FW_DEBUG
    ipcom_printf("Key shift value: %d\n", ipcom_fw.fw_key_shift);
#endif
#endif

    ipcom_forwarder_backend_init();

#ifdef IPCOM_FORWARDER_AMP
    /*
     * Initialize and advertise readiness of management core's
     * IPC message queue. The network job queue will handle
     * the requests asynchronously...
     *
     * Do we have to wait for ports to be ready?
     *
     * Message commands:
     *  'I'm forwarder core #n. Give me all the information that I need'
     *  (offsets corresponding to each pointer in ipcom_fw;
     *  what's the list of all forwarder-offload capable ports?
     *  which of those ports should I poll for input packets, using
     *  what input queues? What output queue do I use for each port?
     *  Where is my packet return/event stack?
     *
     * 'OK, I'm ready to begin forwarding'
     */
    fw_msg_q = ipcom_forwarder_msgq_init();
#endif

    /* Indicate FW started */
    ipcom_forwarder_fw_started = 1;

    /* Switch off receive interrupts */
    ipcom_forwarder_enter_poll();
    ipcom_forwarder_set_tag_seed(ipcom_forwarder_seed);

#ifndef IPCOM_FORWARDER_AMP
    /* Start forwarders */
    fw_cnt = ipcom_forwarder_get_fw_cnt();
    for (i = 0; i < fw_cnt; i++)
    {
        /* Create forwarders */
        ipcom_create_rx_fw_tx(i);

        /* Send start message to forwarder */
        ipcom_fw.control_pipe[i] = ipcom_pipe_new(IPCOM_PIPE_GENERIC, &pattr);
        ipcom_pipe_send(ipcom_fw.fw_pipe, &ipcom_fw.control_pipe[i]);
    }
#endif

#ifndef IP_PORT_LKM
    ipcom_forwarder_cmd_add();
#endif

    return IPCOM_SUCCESS;
}

#else
int ipcom_forwarder_ctrl_empty_file;
#endif /* IPCOM_USE_FORWARDER */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
