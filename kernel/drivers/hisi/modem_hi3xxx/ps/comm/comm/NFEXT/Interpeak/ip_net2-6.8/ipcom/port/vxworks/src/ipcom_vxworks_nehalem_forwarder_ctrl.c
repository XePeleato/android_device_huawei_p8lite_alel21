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
 * This is the port specific part of the 'ipcom_forwarder'. It is only valid
 * if IPCOM_USE_NEHALEM_FORWARDER is defined (typically in
 * ipcom/config/ipcom_config.h). When IPCOM_USE_NEHALEM_FORWARDER is not
 * defined, the contents of this file is "ifdefed out".
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"

/* Refer to description above*/
#ifdef IPCOM_USE_NEHALEM_FORWARDER

#define FW_DEBUG

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <vxWorks.h>
#include <string.h>
#include <ctype.h>
#include <vxCpuLib.h>
#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_netif.h>
#include <ipcom_vxworks.h>

#include <ipcom_forwarder.h>
#include <ipcom_vxworks_nehalem_forwarder.h>

#include <vxmux_pkt.h> /* TODO: temporary */

#ifdef IPCOM_FORWARDER_AMP
#include <ipcom_shared_mem.h>

#include <wrhv/vbi.h>
#endif

#include <stdio.h>
#include <memLib.h>
#include <sysLib.h>

#include <netLib.h>

#include <end.h>
#include <muxLib.h>
#include <net/ethernet.h>
#include <hwif/vxbus/vxBus.h>
#undef BSP_VERSION


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define FW_MAX_LINKHDR  18      /* 14-byte ethernet header + 1 VLAN tag */

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

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_EXTERN int  ipcom_forwarder_get_fw_cnt(void);

IP_STATIC void ipcom_ipc_msgq_cmd_getinfo(Ipcom_ipc_msg_t *ipc_msg);

IP_PUBLIC Ip_u32 ipcom_fw_pkt_size(void);

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Local data
 *===========================================================================
 */

IP_STATIC Ip_u32 * ipcom_forwarder_secret_seed;

IP_STATIC Ipcom_fw_port_info_t ipcom_fw_ports_static [IPCOM_FW_PORTS_MAX];

IP_STATIC Ip_bool ipcom_fw_polling = IP_FALSE;

#ifdef IPCOM_USE_FORWARDER_IPSEC
/* Array of locks, one for each SA */
/* target array needs to be in shared memory! */
/*
 * TODO - the control side doesn't access these locks,
 * but presently it initializes them.
 */
IP_STATIC Ipcom_fw_sa_lock_t * ipcom_fw_sa_locks;
#endif

IP_STATIC atomic_t ipcom_fw_ports_allocated;

/*
 *===========================================================================
 *                         Global data
 *===========================================================================
 */


IP_PUBLIC Ipcom_fw_port_info_t * ipcom_fw_port_info = &ipcom_fw_ports_static[0];

#ifndef IPCOM_FORWARDER_AMP
IP_PUBLIC Ipcom_forwarder_info_t * ipcom_fw_info[IPCOM_MAX_FORWARDERS] = {IP_NULL};
#endif

#if defined (IPCOM_FORWARDER_AMP)
IP_PUBLIC rxq_statistics_t * ipcom_fw_rxq_stats[IPCOM_FW_MAX_CORES()];
#endif

IP_PUBLIC Ipcom_fw_event_ptrs_t ipcom_fw_events[IPCOM_MAX_FORWARDERS] =
        {{
          IP_NULL
#ifdef IPCOM_FW_HIGH_EVENTS
          , IP_NULL
#endif
        }};

IP_PUBLIC int ipcom_fw_num = -1;
IP_PUBLIC cpuset_t  ipcom_forwarder_cpus;  /* Make an Ipcom_cpu_set_t ? */

IP_PUBLIC Ip_u32 ipcom_fw_pkt_size_val;

IP_PUBLIC Ipcom_ipc_msgq_cmd ipcom_fw_ipc_msgq_cmds [IPCOM_FW_MSGQ_NUM_CMDS] =
{
    ipcom_ipc_msgq_cmd_getinfo,
#ifdef IPCOM_FORWARDER_LOCAL_STACK
    IP_NULL,   /* IPCOM_FW_MSGQ_CMD_SLOWPKT_GEI; set by driver */
    IP_NULL,   /* IPCOM_FW_MSGQ_CMD_SLOWPKT_TEI; set by driver */
#else
#endif
};

/*
 ****************************************************************************
 * 9                     DEBUG FUNCTIONS
 ****************************************************************************
 */

#ifdef FW_DEBUG

/* Debug */
#ifdef IPCOM_FORWARDER_LOCAL_STACK
IP_PUBLIC void
ipcom_fw_netif_info(Ipcom_netif *netif)
{
    void *port_cookie;
    if (netif == IP_NULL)
        return;

    port_cookie = ipcom_fw.fw_port[netif->hw_ix].fwd;

    printf ("hw_ix=%d, cookie=%p\n", netif->hw_ix, port_cookie);
}
#endif

#endif /* FW_DEBUG */


/*
 ****************************************************************************
 * 10                    LOCAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                  ipcom_forwarder_aggregate_rxq_stats
 *===========================================================================
 * Context:     shell
 * Description: Aggregate RX queue specific statistics to port
 * Parameters:  'port' : the forwarder port cookie
 * Returns:     N/A
 *
 */

/* int get_stats_debug = 0; */
IP_STATIC void
ipcom_forwarder_aggregate_rxq_stats(Ipcom_fw_port_t *port)
{
    Ip_u32 ix;
    Ip_u32 qix;
    rxq_statistics_t rxq_stats;
    rxq_statistics_t *pStats;
    unsigned int cpu;

    memset (&rxq_stats, 0, sizeof (rxq_stats));

    for (ix = 0; ix < port->nRxPollers; ++ix)
    {
	cpu = port->rxqi[ix].cpuid;
        pStats = ipcom_fw_rxq_stats[cpu];
        if (pStats == 0) /* shouldn't really happen */
            continue;
	qix = port->rxqi[ix].rxq_stats_ix;
        if (qix >= IPCOM_FW_RX_QUEUES_MAX) /* shouldn't happen */
            continue;

        pStats += qix;
#if 0
        if (get_stats_debug)
        {
            printf ("port %u polled by %u (index %u): %u  %u  %u  %u\n",
                    port->port_index, cpu, qix, 
                    pStats->non_fw_frames, pStats->non_fw_bytes,
                    pStats->dropped_packets, pStats->dropped_octets);
        }
#endif
        /* Agregate queue-specific slow-path statistics to port */
        rxq_stats.non_fw_frames += pStats->non_fw_frames;
        rxq_stats.non_fw_bytes += pStats->non_fw_bytes;
        rxq_stats.dropped_packets += pStats->dropped_packets;
        rxq_stats.dropped_octets += pStats->dropped_octets;

    }

    port->stats.non_fw_frames += (rxq_stats.non_fw_frames - 
                                  port->rxq_stats_aggr_old.non_fw_frames);
    port->stats.non_fw_bytes += (rxq_stats.non_fw_bytes -
                                 port->rxq_stats_aggr_old.non_fw_bytes);
    port->stats.dropped_packets += (rxq_stats.dropped_packets -
                                    port->rxq_stats_aggr_old.dropped_packets);
    port->stats.dropped_octets += (rxq_stats.dropped_octets -
                                   port->rxq_stats_aggr_old.dropped_octets);

    port->rxq_stats_aggr_old = rxq_stats;
}

/*
 ****************************************************************************
 * 11                   GLOBAL_FUNCTIONS
 ****************************************************************************
 */


/*
 ****************************************************************************
 * 12                   PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_forwarder_alloc_aligned
 *===========================================================================
 * Context:
 * Description: Allocate memory aligned on a boundary
 * Parameters:  alignment : boundary to align to (power of 2)
 *              size      : number of bytes to allocate
 * Returns:     Pointer to memory or NULL.
 *
 */
IP_PUBLIC void *
ipcom_forwarder_alloc_aligned(unsigned alignment, unsigned size)
{
    void *p;

#ifdef IPCOM_FORWARDER_AMP
    unsigned int cpu = ipcom_forwarder_current_core();
    p = ipcom_fw_cpu_mem_alloc (cpu, size, alignment,
                                IPCOM_FW_CPU_MEM_ALLOC_SHARED);
#else
    p = memalign(alignment, size);
#endif
    if (p)
        memset(p, 0, size);
    return p;
}

/*
 *===========================================================================
 *                    ipcom_forwarder_setup_hw
 *===========================================================================
 * Context:     Master forwarder core initialization task.
 * Description: Ensure network drivers & devices are started.
 *
 */
IP_PUBLIC void
ipcom_forwarder_setup_hw(void)
{
}


#ifdef IPCOM_FORWARDER_LOCAL_STACK
/*
 *===========================================================================
 *                    ipcom_forwarder_netif_to_port_cookie
 *===========================================================================
 * Context:     arbitrary task / initialization
 * Description: get the forwarder port object associated with the specified
 *              network interface
 * Parameters:  netif : the network interface for which to obtain the
 *                      port object
 * Returns: Pointer to the port object, or IP_NULL if the port does not
 *          support forwarding acceleration
 */
IP_EXTERN void *
ipcom_forwarder_netif_to_port_cookie(Ipcom_netif *netif)
{
    END_OBJ * pEnd;
    void * fw_port;

    /* TODO: check that netif is an ethernet interface */

    pEnd = ipcom_fw_netif_to_end(netif);
    if (pEnd == NULL)
        return IP_NULL;

    if (muxIoctl (pEnd, EIOCGFWPORT, (char *)&fw_port) != OK ||
        fw_port == NULL)
        return IP_NULL;

    return fw_port;
}
#endif /* IPCOM_FORWARDER_LOCAL_STACK */


IP_PUBLIC int
ipcom_fw_port_index_allocate(void)
{
    uint32_t ports;  /* presently limited to at most 32 ports */
    atomicVal_t old;
    int i;

    old = vxAtomicGet(&ipcom_fw_ports_allocated);
    while (1)
    {
        ports = ~(uint32_t)old; /* available ports */
        ports = ports & -ports; /* lowest numbered available port */
        if (ports == 0 || ports > (1 << (MAX_FW_PORTS - 1)))
            return -1;

        /*
         * If the old value of the bit was zero when we atomically OR
         * in the bit, we succeeded in allocating the bit.
         */
        old = vxAtomicOr(&ipcom_fw_ports_allocated, (atomicVal_t)ports);
        if (((uint32_t)old & ports) == 0)
            break;
    }

    for (i = 0; (ports & 1) == 0; ++i)
        ports >>= 1;

    return i;
}

IP_PUBLIC void
ipcom_fw_port_index_release(int port)
{
    atomicVal_t val;

    if (port < 0 || port >= MAX_FW_PORTS)
        return;

    val = ~(1 << port);

    vxAtomicAnd (&ipcom_fw_ports_allocated, val);
}


/*
 *===========================================================================
 *                    ipcom_fw_port_install
 *===========================================================================
 * Context:     arbitrary task / initialization (driver ..InstInit2())
 * Description: Install a forwarder-capable port, obtaining the forwarder
 *              port index (0 .. IPCOM_FW_PORTS_MAX-1).
 * Parameters:  fw_port : the forwarder port control-side object
 *              mtu : the port's maximum supported MTU.
 *              hwaddr : pointer to the port's ethernet station address.
 * Returns: The port's allocated index value.
 */
IP_PUBLIC int
ipcom_fw_port_install(Ipcom_fw_port_t *fw_port, Ip_u32 mtu, const Ip_u8 *hwaddr)
{
    int i;
    char buf [80];

    if (fw_port == IP_NULL)
        return -1;

#ifdef IPCOM_FORWARDER_AMP
    (void)ipcom_forwarder_init();
#endif

    i = ipcom_fw_port_index_allocate();
    sprintf (buf, "registering %p, got slot %d\n", fw_port, i);
    vbiKputs (buf);

    if (i >= 0 && i < MAX_FW_PORTS)
    {
        fw_port->port_index = i;
        ipcom_forwarder_register_port(i, fw_port, mtu, hwaddr);
        return i;
    }

    vbiKputs ("Cannot install forwarder port!\n");
    return -1;
}

/*
 *===========================================================================
 *                    ipcom_fw_port_remove
 *===========================================================================
 * Context:     arbitrary task
 * Description: deregister a forwarder-capable port
 * Parameters:  fw_port : the forwarder port control-side object
 * Returns: The port's previously allocated index value
 *          (0 .. IPCOM_FW_PORTS_MAX-1).
 */
IP_PUBLIC int
ipcom_fw_port_remove(Ipcom_fw_port_t *fw_port)
{
    int i;

    /*
     * TODO: not sure we need this; will need more work
     * to safely remove a forwarder port.
     */

    i = fw_port->port_index;
    if (fw_port == IP_NULL || i < 0 || i >= MAX_FW_PORTS)
        return -1;

    ipcom_forwarder_register_port(i, IP_NULL, 0, IP_NULL);
    return i;
}

#ifdef IPCOM_FORWARDER_LOCAL_STACK
/*
 *===========================================================================
 *                    ipcom_fw_register_netif
 *===========================================================================
 * Context:     arbitrary task / initialization
 * Description: get the forwarder port cookie associated with the specified
 *              network interface. If the port supports forwarding offload,
 *              register it as a forwarder port.  It is harmless but useless
 *              to register the same port (cookie) more than once.
 * Parameters:  netif : the network interface for which to obtain the
 *                      port cookie
 * Returns: The index of the forwarder port, or zero if the port does
 *          not support forwarding.
 */
IP_PUBLIC int
ipcom_fw_register_netif(Ipcom_netif *netif)
{
    Ipcom_fw_port_t *fw_port;

    fw_port = ipcom_forwarder_netif_to_port_cookie(netif);

    if (fw_port == IP_NULL)
    {
        netif->hw_ix = -1;
        return 0;
    }

    netif->hw_ix = fw_port->port_index;

    ipcom_forwarder_reset_statistics(fw_port);
    fw_port->funcs->fw_redirect(fw_port, ipcom_fw_polling);

    return netif->hw_ix;
}

/*
 *===========================================================================
 *                    ipcom_fw_deregister_netif
 *===========================================================================
 * Context:     arbitrary task during interface detach
 * Description: Mark the port no longer usable as a forwarder port.
 *              No effect if the port is not currently a forwarder port.
 * Parameters:  netif : the network interface to be deregistered as
 *                      a forwarder port.
 * Returns:     N/A
 */
IP_PUBLIC void
ipcom_fw_deregister_netif(Ipcom_netif *netif)
{
    Ipcom_fw_port_t *fw_port;

    netif->hw_ix = -1;

    fw_port = ipcom_forwarder_netif_to_port_cookie(netif);

    if (fw_port == IP_NULL)
        return;

    fw_port->funcs->fw_redirect(fw_port, IP_FALSE);

    return;
}
#endif /* IPCOM_FORWARDER_LOCAL_STACK */


/*
 *===========================================================================
 *                    ipcom_forwarder_port_name_get
 *===========================================================================
 * Context:     shell
 * Description: obtain the port's name
 * Parameters:  'fwd' : the forwarder port cookie
 *              'ifname' : array to be filled in with port's driver-level
 *                       interface name
 * Returns: 0 if successful; -1 if 'fwd' or 'ifname' is IP_NULL.
 *
 */
IP_PUBLIC int
ipcom_forwarder_port_name_get(void *fwd, char ifname[IP_IFNAMSIZ])
{
    Ipcom_fw_port_t * port = fwd;
    END_OBJ *pEnd;

    if (port == IP_NULL || ifname == IP_NULL || (pEnd = port->pEnd) == NULL)
        return -1;

    (void)snprintf(ifname, IP_IFNAMSIZ, "%s%u",
                   pEnd->devObject.name, pEnd->devObject.unit);
    return 0;
}

/*
 *===========================================================================
 *                    ipcom_forwarder_reset_statistics
 *===========================================================================
 * Context:     shell
 * Description: Reset the statistics counters for the specified port
 * Parameters:  'fwd' : the forwarder port cookie
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_forwarder_reset_statistics(void *fwd)
{
    Ipcom_fw_port_t * port = fwd;

    if (fwd == IP_NULL)
        return;

    ipcom_forwarder_aggregate_rxq_stats(port);

    port->funcs->stats_update(port, IP_TRUE);
}


/*
 *===========================================================================
 *                    ipcom_forwarder_get_statistics
 *===========================================================================
 * Context:     shell
 * Description: Get the statistics counters for the specified port
 * Parameters:  'fwd' : the forwarder port cookie
 * Returns:
 *
 */

IP_PUBLIC void
ipcom_forwarder_get_statistics(void *fwd, port_statistics_t *ps)
{
    Ipcom_fw_port_t * port = fwd;

    if (fwd == IP_NULL)
        return;

    ipcom_forwarder_aggregate_rxq_stats(port);

    port->funcs->stats_update(port, IP_FALSE);

    memcpy (ps, &port->stats, sizeof (*ps));
}

/*
 *===========================================================================
 *                    ipcom_forwarder_set_tag_seed
 *===========================================================================
 * Context:     any
 * Description: This determines how packet tags are calculated by the HW
 * Parameters:  'seed': 32 bit tag seed value
 *
 */
IP_PUBLIC void
ipcom_forwarder_set_tag_seed(Ip_u32 seed)
{
    /*
     * The 82575 RSS hash is unlikely to be appropriate
     * as a route lookup hash, as it may contain undesired
     * information (e.g. IP source address, layer 4 port numbers).
     * We will likely have to caluclate a hash in software, possibly
     * using CRC32.  Here we can simply store the secret seed
     * in a static duration variable.
     */

    /*
     * Note, we need to store the seed in shared memory so
     * that all the forwarders can see it when we change it.
     * We could use some message-based method
     * (e.g. an event) to let the forwarder know that the seed
     * has changed. How quickly do they need to find out?
     */
    *ipcom_forwarder_secret_seed = seed;
}

/*
 *===========================================================================
 *                    ipcom_forwarder_backend_init
 *===========================================================================
 * Context:     any; initialization
 * Description: called once at startup time (from ipcom_forwarder_create())
 *              to do any control plane initialization in the forwarder back
 *              end (port/hardware specific part of forwarder; that's us)
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_forwarder_backend_init(void)
{
    ipcom_forwarder_secret_seed =
        ipcom_fw_cpu_mem_alloc(ipcom_forwarder_master_core(),
                               sizeof (Ip_u32),
                               _CACHE_ALIGN_SIZE,
                               IPCOM_FW_CPU_MEM_ALLOC_SHARED);

#ifdef IPCOM_USE_FORWARDER_IPSEC
    int i;
    /*
     * TODO - the control side doesn't use these locks at all,
     * but presently it initializes them, as they are shared
     * by all forwarders. Not ideal.
     */
    if (ipcom_fw_sa_locks == IP_NULL)
    {
        int n = ipcom_fw.fw_ipsec_entries;

        ipcom_fw_sa_locks =
            ipcom_fw_cpu_mem_alloc(ipcom_forwarder_master_core(),
                                   n * sizeof(Ipcom_fw_sa_lock_t),
                                   _CACHE_ALIGN_SIZE,
                                   IPCOM_FW_CPU_MEM_ALLOC_SHARED);
        if (ipcom_fw_sa_locks != NULL)
        {
            for (i = 0; i < n; ++i)
            {
                ipcom_ipc_spinlock_init(&ipcom_fw_sa_locks[i]);
            }
        }
        else
            printf ("Cannot allocate SA lock array.\n");
    }
#endif
}

/*
 *===========================================================================
 *                    ipcom_forwarder_enter_poll
 *===========================================================================
 * Context:
 * Description: Redirect input packets via forwarders
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_forwarder_enter_poll(void)
{
    int i;

    /*
     * Set the fw_key hash mask based upon the size of the
     * forwarder cache.
     */
    ipcom_fw_polling = IP_TRUE;

    /*
     * For all ports on which forwarding is configured, modify the
     * RSS indirection table such that packets go to non-zero index
     * queue(s) that forwarder cores will poll.  Each used RX queue
     * is polled by no more than one forwarding core, to avoid mutual
     * exclusion overhead.  Won't enable RX interrupts to the forwarding
     * cores.  A forwarding core can (manually) poll more than one RX
     * queue, however. For example, if we reserve 7 of the 8
     * cores as forwarders, we could have each core poll two
     * interfaces, for a total of 14 interfaces.
     *
     * It may or may not make sense to distribute packets from
     * a given port across more than one RX queue. (Note, 82575
     * supports up to 4 RX queues per port. 82598 supports 64
     * RX queues per port -- but the RSS indirection table provides
     * only 4 (8?) bits. How specifically does the RSS indirection
     * table index output determine the RX queue used?)
     */

    for (i = 0; i < IPCOM_FW_PORTS_MAX; ++i)
    {
        Ipcom_fw_port_t * port = ipcom_fw.fw_port[i].fwd;
        if (port == IP_NULL)
            continue;
        port->funcs->fw_redirect(port, IP_TRUE);
    }
}

/*
 *===========================================================================
 *                    ipcom_forwarder_exit_poll
 *===========================================================================
 * Context:
 * Description: Insert input packets directly into END driver receive queue
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_forwarder_exit_poll(void)
{
    int i;

    ipcom_fw_polling = IP_FALSE;

    /*
     * For all ports on which forwarding is configured, modify the
     * RSS indirection table such that packets go to RX queue zero,
     * which is managed by tNet0 executing driver code.
     */

    for (i = 0; i < IPCOM_FW_PORTS_MAX; ++i)
    {
        Ipcom_fw_port_t * port = ipcom_fw.fw_port[i].fwd;
        if (port == IP_NULL)
            continue;
        port->funcs->fw_redirect(port, IP_FALSE);
    }
}

#ifndef IPCOM_FORWARDER_AMP
/*
 * Other devices that actually have some hardware init to do might
 * still need an ipcom_forwarder_init_rx() call even in an AMP system,
 * but we don't.
 */
/*
 *===========================================================================
 *                    ipcom_forwarder_init_rx
 *===========================================================================
 * Context:  Called in the forwarder task before entering its main loop.
 * Description: Set up the "group poll mask" for the calling forwarder
 * Parameters:  fw_i: id of the forwarder (1-7)
 * Returns:
 *
 */
IP_PUBLIC void *
ipcom_forwarder_init_rx(int fw_i)
{
    Ip_u32 ix = fw_i - 1;
    /* for now */

    if (ix >= IPCOM_MAX_FORWARDERS)
        return NULL;

    return ipcom_fw_info [ix];
}
#endif /* ! IPCOM_FORWARDER_AMP */

/*
 *===========================================================================
 *                    ipcom_forwarder_pkt_get_key
 *===========================================================================
 * Context:     slow path transmission / routing cache update
 * Description: Returns the saved forwarding cache hash key for a packet
 *              being forwarded in the slow path.
 * Parameters:  pkt - the packet being transmitted in the slow path.
 * Returns:     The saved forwarding cache lookup key
 *
 * Note, this routine is only called if IS_FORWARDER_PKT(pkt->nae_ctx) is
 * true.
 */
IP_PUBLIC Ip_u32
ipcom_forwarder_pkt_get_key(Ipcom_pkt *pkt)
{
#ifdef IPCOM_FORWARDER_AMP
    /* Initial release: we get packets from ifproxy, and update the
     * routing cache based upon those packets. There's no fw_msg
     * associated with the packets that come from ifproxy.
     */
    Ip_u32 key = (Ip_u32)pkt->nae_ctx;
    return (key & ~0x80000000U);        /* see ipcom_fw_packet_send() */
#else
    fw_msg_t * fw_msg = pkt->nae_ctx;
    return (Ip_u32)fw_msg->fw_key;
#endif
}

/*
 *===========================================================================
 *                    ipcom_forwarder_get_fw_cnt
 *===========================================================================
 * Context:
 * Description: Get the number of explicitly or implicitly specified forwarders
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_forwarder_get_fw_cnt(void)
{
    return ipcom_fw_num;  /* for now */
}

/*
 *===========================================================================
 *                            ipcom_fw_packet_send
 *===========================================================================
 * Context:     ifproxy MIPC message handling context
 * Description: Transmit a packet on a physical port in response to an
 *              IFPROXY_CMD_PACKET_OUT request.
 * Parameters:  hdr - ifproxy command header in packet buffer
 *              free_arg - arbitrary pointer to be passed to
 *                         ipcom_fw_packet_done()
 * Returns:     0 if successful
 *              -IP_ERRNO_EWOULDBLOCK if the packet cannot be sent due to
 *               a temporary lack of TX resources
 *
 * Note: When ipcom_fw_packet_send() returns 0, it takes ownership of the packet
 * and arranges to call ipcom_fw_packet_done() to release the packet when
 * transmission completes.  ipcom_fw_packet_done() may in some circumstances
 * be called during the call to ipcom_fw_packet_send(), or more usually may
 * be called later in a different context.
 *
 * ipcom_fw_packet_done() has the following prototype:
 *
 *    void ipcom_fw_packet_done(ifproxy_cmd_hdr_t *hdr, void *free_arg);
 *
 * When called, it is passed the same arguments as ipcom_fw_packet_tx().
 */
IP_PUBLIC int
ipcom_fw_packet_send(ifproxy_cmd_hdr_t *hdr, void *free_arg)
{
    Ipcom_pkt *pkt;
    Ipcom_fw_port_t *fw_port = ipcom_forwarder_get_port_cookie(hdr->PortIndex);
    Ip_u8 *p;
    Ip_u32 hash_words;
    Ip_u32 type;

    if (fw_port == IP_NULL)
        goto fail_quietly;

    /*
     * TODO: eventually, we may want to use space in the packet buffer
     * for a temporary Ipcom_pkt.
     */
    pkt = vxipcom_pkt_malloc(0, 0);
    if (pkt == IP_NULL)
        goto fail_quietly;

    pkt->data = (Ip_u8 *)hdr; /* We don't have to worry about overwriting
                                the ifproxy command header; we won't. */
    /*
     * TODO: The length argument should indicate the actual
     * available length of the packet buffer beyond the ifproxy
     * header, not just the portion filled with data. But
     * for our purposes here, it does not matter much.
     */
    pkt->maxlen = hdr->EndOffset + sizeof(*hdr);
    pkt->start = hdr->StartOffset + sizeof(*hdr);
    pkt->end = hdr->EndOffset + sizeof(*hdr);

    /*
     * TODO: modify the calling convention for ipcom_fw_packet_done()
     * somewhat so that it's not necessary to use a wrapper function,
     * and so that we could pass an additional argument.
     */
    pkt->data_freefunc = (Ipcom_pkt_freefunc)ipcom_fw_packet_done;
    pkt->data_freefunc_cookie = free_arg;
    pkt->done = VXIPCOM_PKT_FREE; /* done_arg not needed */

    /* Note, we cannot handle TX checksum offload when the forwarder is
       enabled yet. */

    /*
     * Need to calculate the hash key if it's IP, and update the
     * routing cache.
     */
    p = pkt->data + pkt->start;

    if (*p & 1)
        goto sendit;    /* Don't cache link-level multicast/broadcast */

    p += LL_ETH_HDR_SZ;
    type = *((Ip_u16 *)p - 1);
    hash_words = 0;
    if (type == ip_htons(IP_VLAN))
    {
        p += LL_ETH_VLAN_ADD;
        type = *((Ip_u16 *)p - 1);
    }
    if (type == ip_htons(IP_IPv4))
    {
        p += 16;  /* offset in IPv4 header to destination address */
        if (*p < 0xe0)  /* Don't cache IPv4 multicast */
            hash_words = 1;
    }
    else if (type == ip_htons(IP_IPv6))
    {
        p += 8 + 16;   /* offset in IPv6 header to destination address */
        if (*p != 0xff) /* Don't cache IPv6 multicast */
            hash_words = 4;
    }
    if (hash_words > 0)
    {
        Ip_u32 fw_key;
        fw_key = (ipcom_fw.fw_v4_entries - 1) &
            ipcom_lookup3_hashword((uint32_t *)p, hash_words,
                                   *ipcom_forwarder_secret_seed);
#if IPCOM_FORWARDER_TRUE_CACHE_WAYS_SHIFT > 0
        fw_key <<= IPCOM_FORWARDER_TRUE_CACHE_WAYS_SHIFT;
#endif
        pkt->nae_ctx = (void *)(0x80000000U | fw_key);

        (void)ipcom_forwarder_transmit_pkt_ext(pkt, hdr->PortIndex);
    }

sendit:
    if (IP_LIKELY (mux2Send(fw_port->pEnd, pkt) != -IP_ERRNO_EWOULDBLOCK))
        return 0;

    /* Ugh, free pkt, but leave hdr untouched */

    pkt->data = IP_NULL;  /* don't try to free buffer! */
    pkt->data_freefunc = IP_NULL;
    pkt->data_freefunc_cookie = IP_NULL;

    VXIPCOM_PKT_DONE(pkt);

    return -IP_ERRNO_EWOULDBLOCK;

fail_quietly:
    ipcom_fw_packet_done(hdr, free_arg);
    return 0;
}

#ifndef IPCOM_FORWARDER_AMP
/*
 *===========================================================================
 *                    ipcom_forwarder_cpuid_get
 *===========================================================================
 * Context:     Called in the forwarder task before setting CPU affinity
 * Description: Get the index of the CPU on which the forwarder should run
 * Parameters:  fw_i: id (index) of the forwarder (1 - ...)
 * Returns:     The CPU ID of the core on which forwarder fw_i should run
 *
 */
IP_PUBLIC int
ipcom_forwarder_cpuid_get(int fw_id)
{
    Ip_u32 ix = (Ip_u32)fw_id - 1;
    Ipcom_forwarder_info_t * info;

    if (ix >= IPCOM_MAX_FORWARDERS)
        return -1;

    info = ipcom_fw_info [ix];
    if (info == NULL)
        return -1;

    return (int)info->cpuId;
}
#endif /* ! IPCOM_FORWARDER_AMP */

#ifdef IPCOM_FW_EVT_TRIGGER_PIPE_READ
/*
 *===========================================================================
 *                    ipcom_forwarder_trigger_pipe_read
 *===========================================================================
 * Context:
 * Description: Trigger a forwarder to poll the control pipe
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_forwarder_trigger_pipe_read(int fw_id)
{
    Ipcom_fw_low_events_t * low;

    --fw_id;

    if ((unsigned)fw_id >= IPCOM_MAX_FORWARDERS ||
        (low = ipcom_fw_events[fw_id].low) == IP_NULL)
        return;

    (void)vxAtomicOr(&low->events, IPCOM_FW_EVT_TRIGGER_PIPE_READ);
}
#endif /* IPCOM_FW_EVT_TRIGGER_PIPE_READ */

#ifdef IPCOM_FW_HIGH_EVENTS
#ifdef IPCOM_FW_EVT_JOB
IP_PUBLIC void
ipcom_fw_job_post(Ipcom_fw_high_events_t *high, Ipcom_fw_job_t *job)
{
    Ipcom_int_key_t key;
    Ipcom_fw_offset_t *ptail;
    Ipcom_fw_offset_t job_off = (char *)job - (char *)high;

    key = ipcom_ipc_spinlock_take(&high->jobq.lock);
    ptail = (Ipcom_fw_offset_t *)((char *)high + high->jobq.ptail_off);
    *ptail = job_off;
    high->jobq.ptail_off = job_off + offsetof(Ipcom_fw_job_t, next_off);
    ipcom_ipc_spinlock_give(&high->jobq.lock, key);
    (void)vxAtomicOr (&high->events, IPCOM_FW_EVT_JOB);
}
#endif /* IPCOM_FW_EVT_JOB */
#endif /* IPCOM_FW_HIGH_EVENTS */

/*
 * Functions called by the gei and tei drivers with forwarding support.
 */

#ifdef IPCOM_FW_EVT_PKTS_TO_FREE
/*
 *===========================================================================
 *                    ipcom_fw_pkt_remote_free
 *===========================================================================
 * Context:        called from tNet0, possibly other tasks
 * Description:    This routine is called to free a packet belonging to a
 *                 forwarder from a task different than the forwarder (usually
 *                 tNet0, in the slow path).  Since a forwarder normally uses
 *                 no mutual exclusion to access its packet pool, we need to
 *                 put the packet on a separate list and set a flag that the
 *                 forwarder will (eventually) see, to know that it should
 *                 get these packets.
 * Parameters:     pkt - the packet buffer to be freed
 *                 info - the forwarder that owns the packet
 * Returns:        N/A
 */
IP_PUBLIC void
ipcom_fw_pkt_remote_free(void *pktbuf, Ipcom_fw_low_events_t * low)
{
    void * old;
    int key;
    Ipcom_fw_pkt_t * pkt;

#ifdef IPCOM_FORWARDER_LOCAL_STACK
    /* geiFwRxSlowPath() preserves the fw_msg_t, compensate here */
    pkt = (Ipcom_fw_pkt_t *)((char *)pktbuf - sizeof(fw_msg_t));
#else
    pkt = pktbuf;
#endif

    key = ipcom_ipc_spinlock_take (&low->pktStack.lock);
    IPCOM_FW_PKT_STACK_PUSH(&low->pktStack, pkt, old);
    if (old == 0)
        (void)vxAtomicOr(&low->events, IPCOM_FW_EVT_PKTS_TO_FREE);
    ipcom_ipc_spinlock_give (&low->pktStack.lock, key);
}
#endif /* IPCOM_FW_EVT_PKTS_TO_FREE */

IP_PUBLIC Ip_u32
ipcom_fw_pkt_size(void)
{
    Ip_u32 mtu = ipcom_forwarder_mtu();
    Ip_u32 size;

    /*
     * Points A, B, and F are cache-line aligned (64 bytes on Nehalem).
     * 'size' will be F - A, and should be an odd multiple of
     * a cache line.  D - C is the MTU.
     *
     * +----------------+ A
     * | Ipcom_fw_pkt_t |   Note that Ipcom_fw_pkt_t embeds the fw_msg_t.
     * |                |   The Ipcom_fw_pkt_t is probably no more than 64 bytes.
     * +----------------+
     * |    Pad         |   Pad should be large enough for prepending
     * |                |   additional headers during tunnelling.
     * ~                ~
     * |                |   We choose (B - A) = 256 bytes.
     * +----------------+ B Align the ethernet header on a cache boundary
     * |  Maximum       |   to improve system I/O throughput.
     * |  Ethernet      |
     * |   Header       |
     * |                |
     * +----------------+ C
     * |                |
     * |  Ethernet      |    D - C = 1500 or 9000
     * |  payload       |
     * ~                ~
     * |                |
     * +----------------+ D
     * |    4 bytes     |    For ethernet CRC, in case HW doesn't strip it.
     * +----------------+ E
     * |   Pad          |    Add at least 1 cache line of pad, and then
     * |                |    round up to next odd multiple of cache line size.
     * +----------------+ F
     */

    size = IPCOM_FW_RXPKT_OFFSET + FW_MAX_LINKHDR + mtu + 4 +
      _CACHE_ALIGN_SIZE;

    size = ROUND_UP (size, _CACHE_ALIGN_SIZE);

    /* Make size an odd multiple of _CACHE_ALIGN_SIZE */
    if ((size & _CACHE_ALIGN_SIZE) == 0)
        size += _CACHE_ALIGN_SIZE;

    /*
     * mtu = 1500 ==> size = 0x740 = 1856.
     * mtu = 9000 ==> size = 0x24C0 = 9408.
     */

    ipcom_fw_pkt_size_val = size; /* stash it in a global for slow path */
    return size;
}


#ifndef IPCOM_FORWARDER_AMP
/*
 * ipcom_fw_ports_config() and ipcom_forwarders_config() are
 * no longer used in the AMP version of the Nehalem forwarder system.
 * They might possibly still be useful if the SMP version were
 * to be supported.
 */
int ipcom_fw_ports_config(const char *cfg, Ipcom_fw_port_info_t *pInfo, int maxports)
{
    int i, n, len, val;
    int ncpus;
    const char * c;
    char buf [IP_IFNAMSIZ * 2];
    char * something;

    if (cfg == IP_NULL || pInfo == IP_NULL || maxports < 0)
        return -1;

    ncpus = (int)IPCOM_FW_MAX_CORES();

    for (i = 0; i < maxports; ++i)
    {
        memset (pInfo->ifname, 0, sizeof (pInfo->ifname));
        CPUSET_ZERO(pInfo->pollers);
    }

    /*
     * cfg is expected to be a string such as:
     *  "gei2 3; gei3 3; gei4 3; gei5 7; gei6 7; gei7 7; tei0 5 6"
     *
     * that indicates a series of network interfaces, listing the
     * CPU cores that poll each interface.  In the above example,
     * gei2, gei3, and gei4 are polled by CPU 3; gei5, gei6, and gei7
     * are polled by cpu 7; and tei0 is polled by cpu CPUs 5 and 6.
     *
     * The allowed syntax is:
     *
     * cfg_string := <ws>* [<ifpollspec> (';' <ws>* <ifpollspec>)* [';'] <ws>*]
     * ifpollspec := <ifname> (<ws>+ <cpuid>)* <ws>*
     * ifname := <alpha>+ <decimal>+
     * cpuid := <decimal>+
     * ws := any whitespace character (isspace())
     * alpha := any alphabetic character (A-Za-z : isalpha())
     * decimal := any decimal numeral (0-9 : isdigit())
     *
     * This routine does not check that the specified ifname's correspond
     * to actual devices, since the routine may be called before the
     * devices are created. An ifname must consist of fewer than IP_IFNAMSIZ
     * characters.
     *
     * If an ifname occurs more than once, a warning
     * is emitted, and the sets of CPUs from each occurrence are merged.
     * If there are more ifname's than maxports, a warning is emitted and
     * processing stops.
     *
     * The cpuid values must correspond to valid configured CPU indices,
     * in the range 0 <= cpuid < IPCOM_FW_MAX_CORES(), and must in addition
     * avoid cores belonging to the management board (core 0, for now).
     * cpuid values outside that range cause a warning to be emitted, but
     * are otherwise ignored.
     * The order in which the cpuid values are listed for a given interface
     * does not matter.  A repeated cpuid value for the same interface causes
     * a warning.
     *
     * If an unexpected character is encountered, a warning is emitted and
     * processing stops.
     *
     * The return value when processing stops is the number of slots filled
     * in the <pInfo> array.
     */

    c = cfg;
    n = 0;

    while (TRUE)
    {

        while (isspace(*c))
            ++c;

        cfg = c;
        if (*c == '\0')
            return n;

        if (!isalpha(*c))
            goto bad_ifname;

        do {++c;} while (isalpha(*c));

        if (!isdigit(*c))
            goto bad_ifname;

        do {++c;} while (isdigit(*c));

        if (!isspace(*c) && *c != '\0' && *c != ';')
            goto bad_ifname;

        if (c - cfg >= IP_IFNAMSIZ)
            goto bad_ifname2;

        /* check if the interface name already occurs in the list */

        len = c - cfg;

        for (i = 0; i < n; ++i)
        {
            if (memcmp(pInfo[i].ifname, cfg, len) == 0 &&
                pInfo[i].ifname[len] == '\0')
                break;
        }
        if (i == n)
        {
            if (n == maxports)
            {
                FW_LOGMSG("WARNING: Too many forwarder ports configured, maximum is %d\n", maxports);
                return n;
            }
            memcpy(pInfo[i].ifname, cfg, len);
            memset(pInfo[i].ifname + len, 0, IP_IFNAMSIZ - len); /* at least 1 */
            ++n;
        }

        while (IP_TRUE) /* read forwarder cpuids for this interface */
        {
            while (isspace(*c))
                ++c;

            if (*c == '\0')
                return n;

            if (*c == ';')
            {
                ++c;
                break; /* look for another ifname */
            }

            cfg = c;

            if (!isdigit(*c))
                goto bad_cpuid;

            val = 0;

            while (isdigit(*c))
            {
                if (val < ncpus)
                    val = val * 10 + (*c) - '0';
                ++c;
            }
            if (!isspace(*c) && *c != '\0' && *c != ';')
                goto bad_cpuid;

            /* The val == 0 check intends to exclude the management core... */
            if (val == 0 || val >= ncpus)
            {
                FW_LOGMSG("WARNING: Forwarder CPU index %d for %s out of range\n",
                         val, pInfo[i].ifname);
                continue;
            }

            if (CPUSET_ISSET(pInfo[i].pollers, val))
                FW_LOGMSG("WARNING: Forwarder CPU id %d repeated for %s\n",
                         val, pInfo[i].ifname);

            CPUSET_SET(pInfo[i].pollers, val);
        } /* loop over cpuids */
    } /* loop over ifpollspecs */

bad_ifname:
    ++c;
bad_ifname2:
    something = "interface name";
    goto bad_something;

bad_cpuid:
    ++c;
    something = "cpu index";

bad_something:
    len = c - cfg;
    if (len >= sizeof(buf))
        len = sizeof(buf) - 1;
    memcpy(buf, cfg, len);
    buf[len] = '\0';

    FW_LOGMSG("WARNING: Bad forwarder %s '%s...'\n", something, buf);

    return n;
}

IP_PUBLIC void
ipcom_forwarders_config(void)
{
    int nports;
    cpuset_t fw_cpus;
    int i, j;
    int maxcpus = IPCOM_FW_MAX_CORES();
    int ncpus;
    int ports_for_cpu;

    nports = ipcom_fw_ports_config(ipcom_fw_ports_cfg,
                                   ipcom_fw_port_info,
                                   IPCOM_FW_PORTS_MAX);

    CPUSET_ZERO(fw_cpus);
    for (i = 0; i < nports; ++i)
    {
        CPUSET_ADD(fw_cpus, ipcom_fw_port_info[i].pollers);
    }

    ncpus = 0;

    for (i = 0; i < maxcpus; ++i)
    {
        if (CPUSET_ISSET(fw_cpus, i))
        {
            ++ncpus;
            ports_for_cpu = 0;
            for (j = 0; j < nports; ++j)
            {
                if (CPUSET_ISSET(ipcom_fw_port_info[j].pollers, i))
                    ++ports_for_cpu;
            }
            if (ports_for_cpu > IPCOM_FW_RX_QUEUES_MAX)
                FW_LOGMSG("ERROR: Forwarder core %d polls too many ports\n", i);

            if (ncpus <= IPCOM_MAX_FORWARDERS)
            {
                CPUSET_SET(ipcom_forwarder_cpus, i);
            }
        }
    }
    if (ncpus > IPCOM_MAX_FORWARDERS)
    {
        FW_LOGMSG("ERROR: Too many configured forwarders (%d > %d)\n",
               ncpus, IPCOM_MAX_FORWARDERS);
        ncpus = IPCOM_MAX_FORWARDERS;
    }

    ipcom_fw_num = ncpus;

    /* FW_LOGMSG ("Configured %d forwarders and %d ports\n", ncpus, nports); */
}
#endif /* !defined (IPCOM_FORWARDER_AMP) */

IP_PUBLIC void
ipcom_fw_master_heartbeat(Ipcom_fw_anchor_t * anchor)
{
    unsigned int i;
    unsigned int maxCores = IPCOM_FW_MAX_CORES();

    for (i = 0; i < maxCores; ++i)
        anchor->heartbeat[i] = 0;

    printf ("Master nae waiting for forwarder heartbeats...\n");
again:
    ++anchor->master_heartbeat;
    VX_MEM_BARRIER_RW();
    taskDelay (1);

    for (i = 0; i < maxCores; ++i)
    {
        /* skip non-forwarder cores */
        if (!CPUSET_ISSET(ipcom_forwarder_cpus, i))
            continue;
        /*
         * If any forwarder core has not responded yet,
         * try again.
         */
        if (anchor->heartbeat[i] == 0)
            goto again;
    }

    printf ("Heard from all forwarder cores!\n");
    return;
}

/* Do we really need this? */
IP_PUBLIC int
ipcom_core_num_to_fw_index(unsigned int core)
{
    int i;
    int n;

    if (!CPUSET_ISSET(ipcom_forwarder_cpus, core))
        return 0;

    n = 1;
    for (i = 0; i < IPCOM_FW_MAX_CORES(); ++i)
    {
        if (i == core)
            return n;

        if (CPUSET_ISSET(ipcom_forwarder_cpus, i))
            ++n;
    }
    return 0;
}


/*
 * The message queue callback function for the forwarder NAEs'
 * initial getinfo requests.
 */
IP_STATIC void
ipcom_ipc_msgq_cmd_getinfo(Ipcom_ipc_msg_t *ipc_msg)
{
    Ipcom_fw_cmd_getinfo_t *msg = (Ipcom_fw_cmd_getinfo_t *)ipc_msg;
    unsigned int cpu;
    int fw_ix;
    Ipcom_fw_port_t *fw_port;
    int i;
    Ip_u32 qnum;
    Ip_u32 rxqnum;
    Ipcom_fw_low_events_t * low;
#ifdef IPCOM_FW_HIGH_EVENTS
    Ipcom_fw_high_events_t * high;
#endif
    rxq_statistics_t *stats_a;

    cpu = msg->cpu;

    fw_ix = ipcom_core_num_to_fw_index(cpu);
    msg->fw_ix = fw_ix;

    if (fw_ix == 0)
    {

        printf ("CPU %u is not assigned as a forwarder.\n",
                      cpu);
        goto done;
    }

    msg->rt_cache_id_off = (char *)ipcom_fw.rt_cache_id - (char *)msg;
#ifdef IPCOM_USE_FORWARDER_UPDATE_CACHE_LOCK
    msg->fw_lock_off = (char *)ipcom_fw.fw_lock - (char *)msg;
#endif
    msg->fw_cache_tbl_v4_off = (char *)ipcom_fw.fw_cache_tbl_v4 - (char *)msg;
    msg->fw_cache_tbl_v6_off = (char *)ipcom_fw.fw_cache_tbl_v6 - (char *)msg;
    msg->fw_v4_entries = ipcom_fw.fw_v4_entries;
    msg->fw_v6_entries = ipcom_fw.fw_v6_entries;
#ifdef IPCOM_USE_FORWARDER_IPSEC
    msg->fw_cache_tbl_enc_off = (char *)ipcom_fw.fw_cache_tbl_enc - (char *)msg;
    msg->fw_cache_tbl_dec_off = (char *)ipcom_fw.fw_cache_tbl_dec - (char *)msg;
    msg->fw_ipsec_entries = ipcom_fw.fw_ipsec_entries;
    msg->fw_sa_locks_off = (char *)ipcom_fw_sa_locks - (char *)msg;
#endif
    msg->fw_port_off = (char *)ipcom_fw.fw_port - (char *)msg;
#ifdef IPCOM_FORWARDER_USE_UPPER_TAG_BITS
    msg->fw_key_shift = ipcom_fw.fw_key_shift;
#endif

    msg->fw_key_mask = (ipcom_forwarder_config_cache_size() - 1);
    msg->fw_secret_seed_off = (char *)ipcom_forwarder_secret_seed - (char *)msg;

    printf ("Forwarder GETINFO: CPU %u polls", cpu);

    rxqnum = 0;
    for (i = 0; i < IPCOM_FW_PORTS_MAX; ++i)
    {
        fw_port = ipcom_fw.fw_port[i].fwd;
        if (fw_port == IP_NULL)
            continue;

        /* Check this port's RX queues */
        for (qnum = 0; qnum < fw_port->nRxPollers; ++qnum)
        {
	    if (fw_port->rxqi[qnum].cpuid != cpu)
		continue;

	    snprintf(msg->rxqc[rxqnum].ifname, IP_IFNAMSIZ, "%s%d",
		     fw_port->pEnd->devObject.name,
		     fw_port->pEnd->devObject.unit);
	    FW_LOGMSG ("%s\n", msg->rxqc[rxqnum].ifname);

	    msg->rxqc[rxqnum].rxq_regs =
		fw_port->funcs->fw_qaddr(fw_port, qnum, FW_Q_RX);

	    msg->rxqc[rxqnum].ifproxy_addr = fw_port->ifproxy_addr;

	    msg->rxqc[rxqnum].port_index = i;
	    msg->rxqc[rxqnum].rxq_stats_ix = fw_port->rxqi[qnum].rxq_stats_ix;

            ++rxqnum;
            break;
        }
    }

    if (rxqnum == 0) /* might possibly happen if geiFwReservedUnits != 0 */
        {
        /* TODO: should we really stop the forwarder, or should we just
         * let it poll for non-RX work?
         */
        printf(" no RX queues!\n");
        msg->fw_ix = 0;
        ipcom_fw_num--;  /* TODO - forwarder indices don't change! */
        goto done;
        }
    else if (rxqnum > IPCOM_FW_RX_QUEUES_MAX)
        printf(" ... Too many RX Queues!\n");
    else
        printf(".\n");

    stats_a = 
        ipcom_fw_cpu_mem_alloc(cpu,
                               sizeof(rxq_statistics_t) * 
                               IPCOM_FW_RX_QUEUES_MAX,
                               _CACHE_ALIGN_SIZE,
                               (IPCOM_FW_CPU_MEM_ALLOC_DMA |
                                IPCOM_FW_CPU_MEM_ALLOC_SHARED));
    if (stats_a == IP_NULL)
    {
        printf ("Cannot allocate RX queue statistics for core %u\n",
                cpu);
        ipcom_fw_num--;  /* TODO - forwarder indices don't change! */
        msg->fw_ix = 0;
        goto done;
    }
    ipcom_fw_rxq_stats[cpu] = stats_a;
    msg->rxq_stats_off = (char *)stats_a - (char *)msg;
    memset (stats_a, 0, sizeof(rxq_statistics_t) * IPCOM_FW_RX_QUEUES_MAX);

    msg->pkt_buf_size = ipcom_fw_pkt_size();
    msg->nominal_mtu = ipcom_forwarder_mtu();

    for (i = 0; i < IPCOM_FW_PORTS_MAX; ++i)
    {
        fw_port = ipcom_fw.fw_port[i].fwd;
        if (fw_port == IP_NULL)
            continue;

	snprintf(msg->txqc[i].ifname, IP_IFNAMSIZ, "%s%d",
		 fw_port->pEnd->devObject.name,
		 fw_port->pEnd->devObject.unit);
	/* Forwarder fw_ix gets TX queue number fw_ix - 1 on every port */
	msg->txqc[i].txq_regs =
	    fw_port->funcs->fw_qaddr(fw_port, fw_ix - 1, FW_Q_TX);
    }

    low = ipcom_fw_cpu_mem_alloc(cpu, sizeof(*low),
                                 _CACHE_ALIGN_SIZE,
                                 IPCOM_FW_CPU_MEM_ALLOC_SHARED);
    if (low == IP_NULL)
    {
        printf ("Cannot allocate low-priority event structure for forwarder core %u\n",
                cpu);
        ipcom_fw_num--;  /* TODO - forwarder indices don't change! */
        msg->fw_ix = 0;
        goto done;
    }

    vxAtomicSet (&low->events, 0);

#ifdef IPCOM_FW_EVT_PKTS_TO_FREE
    IPCOM_FW_PKT_STACK_ZERO(&low->pktStack);
    ipcom_ipc_spinlock_init(&low->pktStack.lock);
#endif

    msg->low_events_off = (char *)low - (char *)msg;
    msg->low_events_mgmt = low;

    ipcom_fw_events[fw_ix - 1].low = low;

#ifdef IPCOM_FW_HIGH_EVENTS
    high = ipcom_fw_cpu_mem_alloc(cpu, sizeof(*high),
                                  _CACHE_ALIGN_SIZE,
                                  IPCOM_FW_CPU_MEM_ALLOC_SHARED);
    if (high == IP_NULL)
    {
        printf ("Cannot allocate high-priority event structure for forwarder core %u\n",
                cpu);
        ipcom_fw_num--;  /* TODO - forwarder indices don't change! */
        msg->fw_ix = 0;
        goto done;
    }

    vxAtomicSet (&high->events, 0);

    msg->high_events_off = (char *)high - (char *)msg;

    ipcom_fw_events[fw_ix - 1].high = high;
#endif

done:
    VX_MEM_BARRIER_W(); /* barriers probably not necessary for nehalem */
    msg->response_flag = IP_TRUE;
}

/*
 * Note, the driver controlling the port provides the slow-path receive
 * routine that is called for the IPCOM_FW_MSGQ_CMD_SLOWPKT command.
 */


IP_PUBLIC void *
ipcom_forwarder_msgq_init(void)
{
    Ipcom_fw_anchor_t * anchor = ipcom_fw_anchor_get();

    memset (anchor, 0, sizeof (*anchor));

    if (ipcom_ipc_msgq_init(&anchor->msg_q,
                            ipcom_ifproxy_mipc_bus(),
                            0ULL,
                            IPCOM_FW_MSGQ_NUM_CMDS,
                            ipcom_fw_ipc_msgq_cmds) < 0)
    {
        printf ("Cannot initialize forwarding message queue!\n");
        return IP_NULL;
    }

    ipcom_fw_master_heartbeat(anchor);

    return &anchor->msg_q;
}

#else
char ipcom_vxworks_nehalem_forwarder_ctrl_empty_file;
#endif /* IPCOM_USE_NEHALEM_FORWARDER */

/*
 ******************************************************************************
 * End of file
 ******************************************************************************
 */
