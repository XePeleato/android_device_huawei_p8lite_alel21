/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_vxworks_forwarder.c,v $ $Revision: 1.26.22.1 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/Attic/ipcom_vxworks_forwarder.c,v $
 *     $Author: ulf $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * This is the port specific part of the 'ipcom_forwarder'. It is only valid
 * if IPCOM_USE_FORWARDER is defined (typically in ipcom/config/ipcom_config.h)
 * When IPCOM_USE_FORWARDER is not defined the contents of this file is
 * "ifdefed out"
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifdef __GNUC__ /* This file requires GNU Compiler */

#include "ipcom_config.h"

/* Refer to description above*/
#ifdef IPCOM_USE_OCTEON_FORWARDER
/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <vxWorks.h>
#include <string.h>
#include <vxCpuLib.h>
#include <stdio.h>
#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include "ipcom_netif.h"

#include <memLib.h>
#include <muxLib.h>

#define cvmx_shared

#include "cvmx-version.h"
/*
 * SDK version 1.7 includes a command queue helper module which is
 * tied in to the packet output processer (PKO) module. We don't need
 * the command queue routines since we manage our TX command queue
 * directly, but we have to break the linker dependency between cvmx-pko
 * and the cvmx-cmd-queue functions somehow. We do this by using macros
 * to override them with dummy replacements.
 */

#if ((OCTEON_SDK_VERSION_NUM > 105000187) && (OCTEON_SDK_VERSION_NUM < 107020244))
#include "cvmx-cmd-queue.h"
#endif

#include "cvmx-config.h"                /* Must be before cvmx.h include */
#include "executive-config.h"

#include "cvmx.h"
#include "cvmx-atomic.h"
#include "cvmx-ciu.h"
#include "cvmx-pip.h"
#include "cvmx-ipd.h"
#include "cvmx-pko.h"
#include "cvmx-gmx.h"

#include "octeon_eeprom_types.h"
#include "octeonEeprom.h"

/*
 * With the 1.7 version of the SDK, the pko module depends on
 * the cvmx_warn() function.
 */

#if (OCTEON_SDK_VERSION_NUM > 105000187)
#include "cvmx-warn.h"
#endif

#include "ipcom_forwarder.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* #define FW_DEBUG */

#define FORWARDER_TRIGGER_ID (0x5af0)
#define IPCOM_OCTEON_POW_RECEIVE_GRP (15)
#define IPCOM_OCTEON_POW_FORWARDER_GRP (13) /* 14, 15 used by END driver */

/* Tag groups for internally generated tags */
#define IPCOM_FORWARDER_INTERNAL_TAG_SA_GROUP       0
#define IPCOM_FORWARDER_INTERNAL_TAG_PORT_GROUP 65536


/*
 ****************************************************************************
 * 5                    EXTERNAL PROTOTYPES
 ****************************************************************************
 */

IP_EXTERN int ipcom_forwarder_fw_per_port(void);
IP_EXTERN int ipcom_forwarder_config_fw_max(void);
IP_EXTERN int ipcom_forwarder_config_cache_size(void);

IP_PUBLIC void * ipcom_fw_netif_to_end(Ipcom_netif *netif);
IP_EXTERN void * vxbOcteon_get_fwd(void *);
IP_EXTERN END_OBJ * vxbOcteon_port_cookie_to_end(void *);

#ifdef IPCOM_USE_FORWARDER_IPSEC
/* Cavium crypto library */
IP_EXTERN Ip_u16 hw_crc16(Ip_u32 *buf, Ip_u32 num);
IP_EXTERN Ip_u32 hw_crc32(Ip_u32 *buf, Ip_u32 num);
IP_EXTERN int ipcom_forwarder_ipsec_single_output_queue;
#endif

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

typedef struct fwd_s {

    int     octPort;
    int     octQueue;
    UINT64 *octTxChunk;
    int     octTxFree;
    void   *pdrv;

} fwd_t;

typedef struct fw_info_s {
    cvmx_wqe_t *q[2];
    fwd_t *txQs[MAX_FW_PORTS];
    Ip_bool is_master;
    int fw_id;
} fw_info_t;

/*
 ****************************************************************************
 * 5.1                  LOCAL DATA
 ****************************************************************************
 */

#ifdef IPCOM_USE_SMP
fw_info_t *fw_info[MAX_FW+1];
#else
/* In the AMP implementation, each image has a single forwarder */
_WRS_DATA_ALIGN_BYTES(CVMX_CACHE_LINE_SIZE) fw_info_t my_fw_info;
#endif /* IPCOM_USE_SMP */

/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */


/*
 ****************************************************************************
 * 6.1                  Debug  Functions
 ****************************************************************************
 */

#ifdef FW_DEBUG
static __inline__ void hwDebugShow( cvmx_wqe_t * work );

/*
 *===========================================================================
 *                    hwDumpPkt
 *===========================================================================
 * Context:
 * Description: Initialise forwarding properties of a network interface
 * Parameters:
 * Returns:     True: success, False: failure
 *
 */
IP_STATIC void
hwDumpPkt(void *data, int len)
{
    int i;
    int m = 20;
    unsigned char *c;


    for (i = 0, c = data; i < len; i++)
    {
        if (i != 0 && (i % m) == 0)
        {
            m = 20;
            printf("\n");
        }
        printf("%02x ", *c++);
    }
    printf("\n");
}

/*
 *===========================================================================
 *                    hwDebugShow
 *===========================================================================
 * Context:
 * Description: Display contents of work queue entry
 * Parameters:
 * Returns:
 *
 */
static __inline__ void hwDebugShow( cvmx_wqe_t * work )
{
    unsigned char *c;
    static int rime_debug = 1;

    if (rime_debug)
    {
        int i;
        char *v;
        UINT64 p;


        p = ((work->packet_ptr.s.addr >> 7) - work->packet_ptr.s.back) << 7;
        v = cvmx_phys_to_ptr (p);

        /* rime_debug--; */
        printf("\nPolled Packet work %08x\n", work);
        printf("IP:             %s\n", work->word2.s.not_IP ? "no" : "yes");
        printf("TCP or UDP:     %s\n", work->word2.s.tcp_or_udp ? "yes" : "no");
        printf("work.hw_chksum: 0x%x\n", work->hw_chksum);
        printf("work.next_ptr:  0x%x\n", work->next_ptr);
        printf("work.len:       %d\n", work->len);
        printf("work.ipprt:     %x\n", work->ipprt);
        printf("work.qos:       %x\n", work->qos);
        printf("work.grp:       %x\n", work->grp);
        printf("work.tag_type:  %x\n", work->tag_type);
        printf("work.tag:       %x\n", work->tag);
        printf("work.word2\n");
        printf("      bufs:     %x\n", work->word2.s.bufs);
        printf("      ip_offset:%x\n", work->word2.s.ip_offset);
        printf("p:              %x:%x\n", (unsigned long)(p>>32), (unsigned long)p);
        printf("work.packet_ptr\n");
        printf("      s.addr    %x, (phys_to_ptr:%x)\n", work->packet_ptr.s.addr, cvmx_phys_to_ptr(work->packet_ptr.s.addr));
        printf("      s.size    %d\n", work->packet_ptr.s.size);
        printf("      s.back    %d\n", work->packet_ptr.s.back);
        printf("      s.pool    %x\n", work->packet_ptr.s.pool);
        printf("cvmx_wqe_t:     %d\n", sizeof(cvmx_wqe_t));

        if (!work->packet_ptr.s.addr)
        {
            c = work->packet_data;
        }
        else
        {
            c = cvmx_phys_to_ptr(work->packet_ptr.s.addr);
        }

        for (i = 0; i < 60; i++)
        {
            if (i != 0 && (i % 20) == 0)
            {
                printf("\n");
            }
            printf("%02x ", c[i]);
        }
        printf("\n");
    }
}


/*
 *===========================================================================
 *                    ipcom_print_hw_msg
 *===========================================================================
 * Context:
 * Description: Print contents of 'hw_msg'
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_print_hw_msg(Ipcom_pipe_msg_t *hw_msg)
{
    UINT64 p;
    char * v;
    cvmx_wqe_t *work;


    work = hw_msg->priv;

    p = ((work->packet_ptr.s.addr >> 7) - work->packet_ptr.s.back) << 7;
    v = cvmx_phys_to_ptr (p);

    printf("\nFW Packet %08x\n", work);
    printf("IP:             %s\n", work->word2.s.not_IP ? "no" : "yes");
    printf("TCP or UDP:     %s\n", work->word2.s.tcp_or_udp ? "yes" : "no");
    printf("work.hw_chksum: 0x%x\n", work->hw_chksum);
    printf("work.next_ptr:  0x%x\n", work->next_ptr);
    printf("work.len:       %d\n", work->len);
    printf("work.ipprt:     %x\n", work->ipprt);
    printf("work.qos:       %x\n", work->qos);
    printf("work.grp:       %x\n", work->grp);
    printf("work.tag_type:  %x\n", work->tag_type);
    printf("work.tag:       %x\n", work->tag);
    printf("p:              %x:%x\n", (unsigned long)(p>>32), (unsigned long)p);
    printf("work.packet_ptr\n");
    printf("      s.addr    %x, (phys_to_ptr:%x)(v:%x)\n", work->packet_ptr.s.addr, cvmx_phys_to_ptr(work->packet_ptr.s.addr), v);
    printf("      s.size    %d\n", work->packet_ptr.s.size);
    printf("      s.back    %d\n", work->packet_ptr.s.back);
    printf("      s.pool    %x\n", work->packet_ptr.s.pool);
}

#endif /* FW_DEBUG */


/*
 ****************************************************************************
 * 6.2                   LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_forwarder_update_work
 *===========================================================================
 * Context:
 * Description: Update work after IPsec changes offset and length
 * Parameters:  fw_msg : hardware message
 *              work   : work queue entry
 * Returns:
 *
 */
#ifdef IPCOM_USE_FORWARDER_IPSEC
IP_STATIC __inline__ void ipcom_forwarder_update_work(fw_msg_t *fw_msg,
                                                      cvmx_wqe_t *work)
{
    if (fw_msg->orig_packet)
    {
        work->len = fw_msg->len;
        work->packet_ptr.s.addr = cvmx_ptr_to_phys(fw_msg->packet);
        work->packet_ptr.s.back += (work->packet_ptr.s.addr >> 7) - (fw_msg->orig_packet >> 7);
    }
}
#endif


/*
 *===========================================================================
 *                    cvmx_helper_get_ipd_port
 *===========================================================================
 * Context:     any
 * Description: Get HW output base queue. There are
 * Parameters:  interface:
 *              port     :
 * Returns:     base queue
 *
 */
IP_STATIC int
cvmx_helper_get_ipd_port(int interface, int port)
{
    switch (interface)
    {
        case 0: return port;
        case 1: return port + 16;
        case 2: return port + 32;
        case 3: return port + 36;
    }
    return -1;
}
/*
 *===========================================================================
 *                    cvmx_helper_get_number_of_interfaces
 *===========================================================================
 * Context:     any
 * Description: There are three interfaces (0, 1, 2), but we return only 2
 *
 * Parameters:  -
 * Returns:     2
 *
 */
IP_STATIC int
cvmx_helper_get_number_of_interfaces(void)
{
    return 2;
}

/*
 *===========================================================================
 *                    ipcom_set_pip_group
 *===========================================================================
 * Context:
 * Description: Configure PIP to assign base group, 'grp', to incoming packets
 *              A fowarder can be configured to receive packets from any port
 *              or only a specific port. This function assigns group value
 *              accordingly.
 *
 * Parameters:  grp : base group
 *
 */
IP_STATIC void
ipcom_set_pip_fw_group(int grp)
{
    int iface, n_ifaces;
    int port;
    int instance = 0;
    uint32_t ports;
    int n_forwarders;

    n_forwarders = ipcom_forwarder_get_fw_cnt();

#ifdef FW_DEBUG
    printf("%s: n_forwarders:%d\n", __FUNCTION__, n_forwarders);
#endif

    cvmx_ipd_disable();

    n_ifaces = cvmx_helper_get_number_of_interfaces();
    for (iface = 0; iface < n_ifaces; iface++)
    {
        port = cvmx_helper_get_ipd_port(iface, 0);

        for (ports = ipcom_forwarder_get_ports_for_interface(iface);
             ports != 0;
             ports >>= 1, ++port)
        {
            cvmx_pip_prt_tagx_t pip_prt_tagx;

            if ((ports & 1) == 0)  /* skip unconfigured or non-existing ports */
                continue;

            pip_prt_tagx.u64 = cvmx_read_csr(CVMX_PIP_PRT_TAGX(port));
            pip_prt_tagx.s.grp = grp;

            if (ipcom_forwarder_fw_per_port())
            {
                /*
                 * Note, ipcom_forwarder_get_fw_cnt() ensures that
                 * n_forwarders <= IPCOM_OCTEON_POW_FORWARDER_GRP
                 * in this case.
                 */

                if (instance >= n_forwarders)
                    instance = 0;

                ++instance;

                if (grp == IPCOM_OCTEON_POW_FORWARDER_GRP)
                {
                    pip_prt_tagx.s.grp = grp - instance; /* 12 .. 0 */
                }
            }
            cvmx_write_csr(CVMX_PIP_PRT_TAGX(port), pip_prt_tagx.u64);
        }
    }
    cvmx_ipd_enable();
}

/*
 ****************************************************************************
 * 6.3                   PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_forwarder_atomic_lock
 *===========================================================================
 * Context:
 * Description:
 * Parameters:
 *
 * Returns:
 *
 */
IP_PUBLIC fw_lock_key_t
ipcom_forwarder_atomic_lock(int index)
{
    cvmx_pow_tag_sw((CVMX_TAG_SW_BITS_INTERNAL << CVMX_TAG_SW_SHIFT)
                    + IPCOM_FORWARDER_INTERNAL_TAG_SA_GROUP
                    + index, CVMX_POW_TAG_TYPE_ATOMIC);

    /* flush pending writes */
    CVMX_SYNCWS;
    cvmx_pow_tag_sw_wait();
    return (fw_lock_key_t)0;
}


/*
 *===========================================================================
 *                    ipcom_forwarder_atomic_unlock
 *===========================================================================
 * Context:
 * Description:
 * Parameters:
 *
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_forwarder_atomic_unlock(int index, fw_lock_key_t unused)
{
    (void)unused;
    /* flush pending writes */
    CVMX_SYNCWS;
    cvmx_pow_tag_sw((CVMX_TAG_SW_BITS_INTERNAL << CVMX_TAG_SW_SHIFT)
                    + IPCOM_FORWARDER_INTERNAL_TAG_SA_GROUP
                    + index, CVMX_POW_TAG_TYPE_ORDERED);

    cvmx_pow_tag_sw_wait();
}


/*
 *===========================================================================
 *                    ipcom_forwarder_hw_crc16
 *===========================================================================
 * Context:
 * Description: Calculate crc16 on a buffer
 * Parameters:  buf : pointer to buffer
 *              num : length of buffer
 * Returns:
 *
 */
#ifdef IPCOM_USE_FORWARDER_IPSEC
IP_PUBLIC Ip_u16
ipcom_forwarder_hw_crc16(Ip_u32 *buf, Ip_u32 num)
{
    return hw_crc16(buf, num);
}
#endif /* IPCOM_USE_FORWARDER_IPSEC */


/*
 *===========================================================================
 *                    ipcom_forwarder_hw_crc32
 *===========================================================================
 * Context:
 * Description: Calculate crc32 on a buffer
 * Parameters:  buf : pointer to buffer
 *              num : length of buffer
 * Returns:
 *
 */
#ifdef IPCOM_USE_FORWARDER_IPSEC
IP_PUBLIC Ip_u32
ipcom_forwarder_hw_crc32(Ip_u32 *buf, Ip_u32 num)
{
    return hw_crc32(buf, num);
}
#endif /* IPCOM_USE_FORWARDER_IPSEC */


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

    p = memalign(alignment, size);
    if (p)
        memset(p, 0, size);
    return p;
}


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
 * Returns: The forwarder port index, or -1 if the port
 *          does not support forwarding acceleration
 */
IP_PUBLIC int
ipcom_fw_register_netif(Ipcom_netif *netif)
{
    fwd_t * fwp = ipcom_forwarder_netif_to_port_cookie(netif);
    END_OBJ * pEnd;
    Ip_u8 mac_addr[8];
    Ip_u32 mtu;

    if (fwp == IP_NULL)
    {
        netif->hw_ix = -1;
        return -1;
    }

    netif->hw_ix = fwp->octPort;

    /*
     * We expect the next three calls to all succeed since
     * ipcom_forwarder_netif_to_port_cookie() succeeded above.
     */
    pEnd = ipcom_fw_netif_to_end(netif);
    (void)muxIoctl (pEnd, EIOCGADDR, (caddr_t)mac_addr);
    (void)muxIoctl (pEnd, EIOCGIFMTU, (caddr_t)&mtu);

    ipcom_forwarder_register_port(fwp->octPort, fwp, mtu, mac_addr);

    ipcom_forwarder_reset_statistics(fwp);

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
    int i;

    i = netif->hw_ix;
    if (i < 0)
        return;

    /* Register NULL as port cookie to deregister */
    ipcom_forwarder_register_port(i, IP_NULL, 0, IP_NULL);

    /* TODO: anything else? */

    netif->hw_ix = -1;
    return;
}


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
    /* Temporary method */
    Ip_u32 * pkey = (Ip_u32 *)pkt->nae_ctx;
    return *pkey;
}

/*
 *===========================================================================
 *                    ipcom_forwarder_pkt_set_key
 *===========================================================================
 * Context:     nae forwarder slow path (ipsec)
 * Description: Changes the forwarder hash key for the packet.
 *              Used in slow path when IPsec modifies the lookup key.
 * Parameters:  pkt : the forwarded packet
 *              key : the new key.
 * Returns:     Pointer to memory or NULL.
 *
 * Note, this routine is only called if IS_FORWARDER_PKT(pkt->nae_ctx) is
 * true.
 */
IP_PUBLIC void
ipcom_forwarder_pkt_set_key(Ipcom_pkt *pkt, Ip_u32 key)
{
    /* Temporary method */
    Ip_u32 * pkey = (Ip_u32 *)pkt->nae_ctx;
    *pkey = key;
}

/*
 *===========================================================================
 *                    ipcom_forwarder_slow_path
 *===========================================================================
 * Context:
 * Description: Append a work queue entry to the driver receive queue
 * Parameters:  hw_msg : contains packet buffer a reference to 'work queue entry'
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_forwarder_slow_path(fw_msg_t *fw_msg, void *fw_cookie)
{
    Ip_u32 *msg;
    cvmx_wqe_t *work = (cvmx_wqe_t *)((Ip_u8 *)fw_msg - ip_offsetof(cvmx_wqe_t, packet_data));

    IPCOM_UNUSED_ARG(fw_cookie);

#ifdef IPCOM_USE_FORWARDER_IPSEC
    ipcom_forwarder_update_work(fw_msg, work);
#endif

#ifdef IPCOM_OCTEON_USE_SMALL_PACKET
    if (!work->word2.s.bufs)
    {
        int offset;

        msg = (Ip_u32 *)cvmx_fpa_alloc(CVMX_FPA_PACKET_POOL);
        if (cvmx_unlikely(msg == NULL))
        {
            /* TODO: maintain statistic */
            cvmx_fpa_free (work, CVMX_FPA_WQE_POOL, 0);
            return;
        }

        work->word2.s.bufs = 1;
        work->packet_ptr.s.back = 1;
        work->packet_ptr.s.addr = cvmx_ptr_to_phys((Ip_u8 *)msg + CVMX_CACHE_LINE_SIZE + 2);
        work->packet_ptr.s.size = CVMX_FPA_PACKET_POOL_SIZE - CVMX_CACHE_LINE_SIZE - 2;

        if (work->word2.s.not_IP)
        {
            offset = 4;
        }
        else
        {
            if (work->word2.s.is_v6)
            {
                offset = 24 - LL_ETH_HDR_SZ;
            }
            else
            {
                offset = 24 - LL_ETH_HDR_SZ + 4;
            }
        }
        memcpy((Ip_u8 *)msg + CVMX_CACHE_LINE_SIZE + 2, work->packet_data + offset, work->len);
    }
    else
#endif
    {
        msg = cvmx_phys_to_ptr (((work->packet_ptr.s.addr >> 7) - work->packet_ptr.s.back) << 7);
    }
    *msg = (Ip_u32)(work->tag & 0xffff);

    /*
     * The 0x01000000 OR'ed with the tag indicates to octEndRxHandle() that
     * this packet was passed by the forwarder for slow path processing;
     * octEndRxHandle() then sets pkt->nae_ctx to point at the packet buffer
     * base instead of NULL.
     * Note that the PIP hardware always clears bits 24-31 of the 32-bit tag.
     */
    cvmx_pow_work_submit(work,
                         work->tag | 0x01000000,
                         CVMX_POW_TAG_TYPE_NULL,
                         work->qos,
                         IPCOM_OCTEON_POW_RECEIVE_GRP);
}


/*
 *===========================================================================
 *                    ipcom_forwarder_reset_statistics
 *===========================================================================
 * Context:     shell
 * Description: Reset the statistics counters for the specified port
 * Parameters:  port_cookie : the port cookie for the port
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_forwarder_reset_statistics(void *port_cookie)
{
    fwd_t *fwd = port_cookie;
    port_statistics_t ps;
    cvmx_pko_port_status_t pko_ps;

    cvmx_pip_get_port_status(fwd[0].octPort, 1, (cvmx_pip_port_status_t *)&ps);
    ipcom_forwarder_get_nonfw_stats(fwd[0].pdrv, &ps.non_fw_frames, &ps.non_fw_bytes, 1);
    cvmx_pko_get_port_status(fwd[0].octPort, 1, (cvmx_pko_port_status_t *)&pko_ps);
}

/*
 *===========================================================================
 *                    ipcom_forwarder_get_statistics
 *===========================================================================
 * Context:     shell
 * Description: Get the statistics counters for the specified port
 * Parameters:  port_cookie : the port cookie for the port
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_forwarder_get_statistics(void *port_cookie, port_statistics_t *ps)
{
    fwd_t *fwd = port_cookie;
    cvmx_pko_port_status_t pko_ps;

    cvmx_pip_get_port_status(fwd[0].octPort, 0, (cvmx_pip_port_status_t *)ps);
    ipcom_forwarder_get_nonfw_stats(fwd[0].pdrv, &ps->non_fw_frames, &ps->non_fw_bytes, 0);
    cvmx_pko_get_port_status(fwd[0].octPort, 0, (cvmx_pko_port_status_t *)&pko_ps);
    ps->tx_packets = pko_ps.packets;
    ps->tx_octets = pko_ps.octets;
}

IP_PUBLIC int
ipcom_forwarder_port_name_get(void *port_cookie,
                              char ifname[IP_IFNAMSIZ])
{
    END_OBJ *pEnd;

    if (port_cookie == IP_NULL)
        return -1;

    pEnd = vxbOcteon_port_cookie_to_end(port_cookie);
    snprintf (ifname, IP_IFNAMSIZ, "%s%u",
              pEnd->devObject.name, pEnd->devObject.unit);
    return 0;
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
    cvmx_pip_tag_secret_t tag_secret;

    tag_secret.s.dst = (Ip_u16)(seed >> 16);
    tag_secret.s.src = (Ip_u16)seed;

    cvmx_write_csr(CVMX_PIP_TAG_SECRET, tag_secret.u64);
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
    return;
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
     cvmx_pip_tag_mask_t tag_mask;
     int csize = ipcom_forwarder_config_cache_size();

#ifdef IPCOM_FORWARDER_USE_UPPER_TAG_BITS
     tag_mask.s.mask = ~0xffffu;

     {
         int i;

         for (i = 0; i < 16; i++)
         {
             if (!((csize << i) & 0xffffu))
             {
                 break;
             }
             tag_mask.s.mask <<= 1;
             tag_mask.s.mask |= 0x1;
         }
         for (i = 0; i < 16; i++)
         {
             if ((IPCOM_FORWARDER_TRUE_CACHE_WAYS >> i) & 0x1)
             {
                 break;
             }
             tag_mask.s.mask <<= 1;
             tag_mask.s.mask |= 0x1;
         }
     }
#else
    /* Limit the generated 'tag' value to fit the specified cache size */
    /* OCTEON_FORWARDER_CACHE_SIZE is defined by a BSP CDF-file */
    tag_mask.s.mask = ~(csize - 1);
     {
         int i;

         for (i = 0; i < 16; i++)
         {
             if ((IPCOM_FORWARDER_TRUE_CACHE_WAYS >> i) & 0x1)
             {
                 break;
             }
         }
         tag_mask.s.mask <<= i;
         tag_mask.s.mask |= (IPCOM_FORWARDER_TRUE_CACHE_WAYS - 1);
     }

#endif

#ifdef FW_DEBUG
     printf("tag mask: %04x, ways: %d, buckets: %d\n",
            tag_mask.s.mask,
            IPCOM_FORWARDER_TRUE_CACHE_WAYS,
            csize
         );
#endif

    cvmx_write_csr(CVMX_PIP_TAG_MASK, tag_mask.u64);

    ipcom_set_pip_fw_group(IPCOM_OCTEON_POW_FORWARDER_GRP);
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
    ipcom_set_pip_fw_group(IPCOM_OCTEON_POW_RECEIVE_GRP);
}


/*
 *===========================================================================
 *                    ipcom_forwarder_cpuid_get
 *===========================================================================
 * Context:     Called in the forwarder task before setting CPU affinity
 * Description: Get the index of the CPU on which the forwarder should run
 * Parameters:  fw_i: id of the forwarder (1-7)
 * Returns:     The CPU ID of the core on which forwarder fw_i should run
 *
 */
IP_PUBLIC int
ipcom_forwarder_cpuid_get(int fw_i)
{
    return ipcom_cpu_reserved_alloc();
}

/*
 *===========================================================================
 *                    ipcom_forwarder_init_rx
 *===========================================================================
 * Context:
 * Description: Set up the "group poll mask" for the calling forwarder
 * Parameters:  fw_id: id of the forwarder (1-7)
 * Returns:
 *
 */

IP_PUBLIC void *
ipcom_forwarder_init_rx(int fw_id)
{
    fw_info_t *info;
    int i;

#ifdef IPCOM_OCTEON_USE_SMALL_PACKET
    cvmx_pip_gbl_cfg_t pip_gbl_cfg;
    cvmx_pip_ip_offset_t pip_ip_offset;
#endif
    int core = cvmx_get_core_num();

    if (ipcom_forwarder_fw_per_port())
    {
        cvmx_write_csr (CVMX_POW_PP_GRP_MSKX(core),
                        (1 << (IPCOM_OCTEON_POW_FORWARDER_GRP - fw_id)) + (1 << IPCOM_OCTEON_POW_FORWARDER_GRP));
    }
    else
    {
        cvmx_write_csr (CVMX_POW_PP_GRP_MSKX(core), 1 << IPCOM_OCTEON_POW_FORWARDER_GRP);
    }

#ifdef IPCOM_OCTEON_USE_SMALL_PACKET
    /* RIME */
    pip_gbl_cfg.u64 = cvmx_read_csr(CVMX_PIP_GBL_CFG);
    pip_gbl_cfg.s.nip_shf = 4;
    cvmx_write_csr(CVMX_PIP_GBL_CFG, pip_gbl_cfg.u64);

    pip_ip_offset.u64 = cvmx_read_csr(CVMX_PIP_IP_OFFSET);
    pip_ip_offset.s.offset = 3; /* 24 byte offset */
    cvmx_write_csr(CVMX_PIP_IP_OFFSET, pip_ip_offset.u64);
#endif

    info = memalign (CVMX_CACHE_LINE_SIZE, sizeof(*info));
    fw_info[fw_id] = info;

    info->q[0] = IP_NULL;
    info->q[1] = IP_NULL;
    /* info->is_master = ipcom_forwarder_is_master(); */
    info->fw_id = fw_id;
    /* TODO: make sure ipcom_fw.fw_port[] is initialized by now */
    for (i = 0; i < MAX_FW_PORTS; ++i)
    {
        fwd_t *pTx = ipcom_fw.fw_port[i].fwd;
        if (pTx == IP_NULL)
            continue;
        /*
         * TODO: The TX queue structures ought to be isolated
         * in their own cache lines since each queue is owned
         * and used by a different forwarder. Presently they
         * are all adjacent, embedded in the END_OBJ.
         * (We could embed a forwarder's TX queues in the fw_info_t.)
         */
        info->txQs[i] = pTx + fw_id;
    }
    return (info);
}

/*
 *===========================================================================
 *                    ipcom_forwarder_get_port_cnt
 *===========================================================================
 * Context:
 * Description: Get the total number of ports available for forwarding
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_forwarder_get_port_cnt(void)
{
    int n_ports = 0;
    int i;
    uint32_t ports;

    for (i = 0; i < cvmx_helper_get_number_of_interfaces(); ++i)
    {
        ports = ipcom_forwarder_get_ports_for_interface(i);
        while (ports != 0)
            {
            if (ports & 1)
                ++n_ports;
            ports >>= 1;
            }
    }
    return n_ports;
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
    int fw_cnt;
    int max_fw = vxCpuConfiguredGet() - 1; /* at least CPU 0 reserved for tNet0 */

    fw_cnt = ipcom_forwarder_config_fw_max();

    if (ipcom_forwarder_fw_per_port())
    {
        /* Treat OCTEON_NUM_FORWARDERS as a maximum in this case. */
        if (max_fw > fw_cnt)
            max_fw = fw_cnt;

        /*
         * The forwarders each have to listen to a different
         * packet input POW group.
         */
        if (max_fw > IPCOM_OCTEON_POW_FORWARDER_GRP)
            max_fw = IPCOM_OCTEON_POW_FORWARDER_GRP;

        /*
         * If possible, we want up to one forwarder per port. We don't
         * want more, since we want to preserve ordering.
         */
        fw_cnt = ipcom_forwarder_get_port_cnt();

    }

    if (fw_cnt > max_fw)
    {
        /*
         * Don't printf(), as we are called very early at startup before
         * the kernel is fully initialized.  Also, we are called several
         * times.
         */
        /* printf ("Warning: forwarder count decreased to %d\n", max_fw); */
        fw_cnt = max_fw;
    }

    return fw_cnt;
}

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
    char *p;
    cvmx_wqe_t *work;

    (void)fw_id; /* ignored - ipcom_forwarder_start_stop_fw() calls us
                    once per forwarder, and we rely on being stopped
                    long enough that each forwarder handles one work
                    item, but don't care which is which. */

    do
    {
        work = cvmx_fpa_alloc (CVMX_FPA_PACKET_POOL);
    } while(!work);

    p = (char *)work + 2 * CVMX_CACHE_LINE_SIZE;
    memset(work, 0, CVMX_FPA_PACKET_POOL_SIZE);

    work->packet_ptr.s.addr = cvmx_ptr_to_phys(p);
    work->packet_ptr.s.back = 1;
    /*
     * ipcom_forwarder_rx() will transfer work->ipprt for non-IP
     * packets to work->packet_data[1] (i.e., fw_msg->tbl[1]).
     */
    work->ipprt = FW_MSG_TRIGGER_PIPE_READ;
    work->word2.s.not_IP = 1;

    cvmx_pow_work_submit(work,
                         (FORWARDER_TRIGGER_ID << 16),
                         CVMX_POW_TAG_TYPE_NULL,
                         0,
                         IPCOM_OCTEON_POW_FORWARDER_GRP);
}

/*
 *===========================================================================
 *                    ipcom_forwarder_free_trigger_msg
 *===========================================================================
 * Context:
 * Description: Free the "trigger" message posted by
 *              ipcom_forwarder_trigger_pipe_read()
 * Parameters:  msg - the message to be dropped
 *              fw_cookie - the forwarder cookie (unused)
 * Returns:     N/A
 *
 */
IP_PUBLIC void
ipcom_forwarder_free_trigger_msg(fw_msg_t *msg, void *fw_cookie)
{
    cvmx_wqe_t *work = (cvmx_wqe_t *)((Ip_u8 *)msg - ip_offsetof(cvmx_wqe_t, packet_data));

    (void)fw_cookie;
    /*
     * Note, the work queue entry was allocated from the packet pool,
     * and there is no separate packet buffer.
     */
    cvmx_fpa_free (work, CVMX_FPA_PACKET_POOL, 0);
}


/*
 *===========================================================================
 *                    ipcom_forwarder_drop_fw_msg
 *===========================================================================
 * Context:
 * Description: Free resources associated with a packet
 * Parameters:  msg - the message to be dropped
 *              fw_cookie - the forwarder cookie (unused)
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_forwarder_drop_fw_msg(fw_msg_t *msg, void *fw_cookie)
{
    cvmx_wqe_t *work = (cvmx_wqe_t *)((Ip_u8 *)msg - ip_offsetof(cvmx_wqe_t, packet_data));
    void *pkt = (void *)((((Ip_ptrdiff_t)msg->packet) & ~(CVMX_CACHE_LINE_SIZE - 1)) - CVMX_CACHE_LINE_SIZE);

    (void)fw_cookie;
    cvmx_fpa_free (pkt, CVMX_FPA_PACKET_POOL, 0);
    cvmx_fpa_free (work, CVMX_FPA_WQE_POOL, 0);
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipnet_msg_len
 *===========================================================================
 * Context:
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_forwarder_ipnet_msg_len(fw_msg_t *msg)
{
    cvmx_wqe_t *work = (cvmx_wqe_t *)((Ip_u8 *)msg - ip_offsetof(cvmx_wqe_t, packet_data));

    return work->len;
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipnet_msg_packet
 *===========================================================================
 * Context:
 * Description: Retrieve packet data corrresponding to a 'fw_msg_t'
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_u8 *
ipcom_forwarder_ipnet_msg_packet(fw_msg_t *msg)
{
    cvmx_wqe_t *work = (cvmx_wqe_t *)((Ip_u8 *)msg - ip_offsetof(cvmx_wqe_t, packet_data));

    return cvmx_phys_to_ptr(work->packet_ptr.s.addr);
}


/******************************************************************************
*
* ipcom_forwarder_rx - Poll for input packets on any port
*
* This function is only called by the "forwarder" core, which has been locked
* to one physical core. The (cvmx_wqe_t ** q) is used for each forwarder to
* hold the previous wqe entries it has retrieved from the POW. The <q> array
* is two 64 bit pointers one for each wqe read. As the hwRxPoll routine is
* called by a forwarder the fifo is shifted to down till q[0] contains a
* valid wqe pointer. The local <work> variable is set to q[0] if valid.
*
*  work q[0] q[1]
* |  0 |  0 |  0 |  Starts out zero no wqe entries.
* |  0 |  0 |  A |  'A' is the first wqe pointer.
* |  0 |  A |  B |  'A' is shifted down and 'B' is in q[1] now.
* |  A |  B |  C |  'A' Shifted down, 'B' shifted down, 'C' is in q[1].
*
* Note: <work> now contains the first wqe read and will be returned.
*
* The reason for doing the fifo above is to allow the cache fetches to have
* time to load the wqe entry and first 128 bytes of packet into L2 cache.
* We do not want to halt the CPU waiting for a cache line fill.
*
* Takes three calls to hwRxPoll to receive the first wqe pointer returned
* in the variable <work>. Note: Because the routine does not wait for a wqe
* pointer the small fifo could have NULL pointers between valid ones.
*
* The function return a Cavium "work" entry, or NULL if no packet is available
*
* RETURNS: <work>, or NULL
*
* ERRNO: N/A
*/
IP_PUBLIC fw_msg_t *
ipcom_forwarder_rx(void *fw_cookie)
{
    void *p;
    fw_msg_t *msg;
    cvmx_wqe_t *work;
#ifdef IPCOM_USE_FORWARDER_IPSEC
    UINT64 w2;

    /* Do not use work queue fifo for IPSec as the tag type needs to be ORDERED */
    work = cvmx_pow_work_request_sync(CVMX_POW_NO_WAIT);
    if (cvmx_likely(work))
    {
        CVMX_PREFETCH0(work);
        CVMX_PREFETCH0(cvmx_phys_to_ptr(work->packet_ptr.s.addr));
        CVMX_PREFETCH0(&ipcom_fw.fw_cache_tbl_v4[work->tag]);
    }
    else
    {
        msg = NULL;
        goto wait;
    }
#else
    fw_info_t *info = fw_cookie;
    cvmx_wqe_t ** q = info->q;

    /* Setup a set of wqe's to be create a prefetch effect on a wqe */
    work = q[0];                /* push the wqe pointers down in the fifo */
    q[0] = q[1];
    q[1] = cvmx_pow_work_request_sync (CVMX_POW_NO_WAIT);

    /* Prefetch the cache entry for the wqe pointer we just got. */
    CVMX_PREFETCH0(q[1]);
#if (CVMX_FPA_POOL_1_SIZE > CVMX_CACHE_LINE_SIZE)
    CVMX_PREFETCH128(q[1]);
#endif

    /* Prefetch the first cacheline for the packet data of the previous wqe */
    if (cvmx_likely(q[0]))
    {
#ifdef IPCOM_OCTEON_USE_SMALL_PACKET
        if (cvmx_unlikely(q[0]->word2.s.bufs))
        {
            CVMX_PREFETCH0( cvmx_phys_to_ptr(q[0]->packet_ptr.s.addr) );
        }
#else
        CVMX_PREFETCH0( cvmx_phys_to_ptr(q[0]->packet_ptr.s.addr) );
#endif
        CVMX_PREFETCH0(&ipcom_fw.fw_cache_tbl_v4[q[0]->tag]);
    }

    /* When work is NULL we do not have any work to do */
    if (cvmx_unlikely(work == NULL))
    {
        msg = NULL;
        goto wait;
    }

#endif /* IPCOM_USE_FORWARDER_IPSEC */

    /* Need to check for receive errors in wqe.word2.s.rcv_error */
    if (cvmx_unlikely(work->word2.s.rcv_error))
    {
        if (cvmx_likely(work->word2.s.bufs))
        {
            msg = cvmx_phys_to_ptr (((work->packet_ptr.s.addr >> 7) - work->packet_ptr.s.back) << 7);
            cvmx_fpa_free (msg, CVMX_FPA_PACKET_POOL, 0);
        }
        cvmx_fpa_free (work, CVMX_FPA_WQE_POOL, 0);
        msg = NULL;
        goto wait;
    }

    if (cvmx_unlikely(work->word2.s.not_IP))
    {
        work->packet_data[0] = IPCOM_FORWARDER_PACKET_TYPE_NONE;
        work->packet_data[1] = work->ipprt;

        msg = (fw_msg_t *)&work->packet_data;
#ifdef IPCOM_USE_FORWARDER_IPSEC
        msg->len = work->len;
        msg->orig_packet = 0;
        msg->ipsec_dec = 0;
#endif
    }
    else
    {
#ifdef IPCOM_OCTEON_USE_SMALL_PACKET
        if(cvmx_likely(work->word2.s.bufs == 0))
        {
            p = work->packet_data + LL_ETH_HDR_SZ;
            work->packet_ptr.s.addr = cvmx_ptr_to_phys(p);
            work->packet_ptr.s.pool = CVMX_FPA_WQE_POOL;
            work->packet_ptr.s.size = work->len;
        }
        else
#endif
        {
            p = cvmx_phys_to_ptr(work->packet_ptr.s.addr);
        }
#ifdef IPCOM_FORWARDER_USE_UPPER_TAG_BITS
        *(Ip_u64 *)work->packet_data = ((Ip_u64)work->word2.s.is_v6 << (32 + 24)) + ((Ip_u64)(((Ip_u16)work->tag) >> ipcom_fw.fw_key_shift) << 32) + (Ip_u32)p;
#else
        *(Ip_u64 *)work->packet_data = ((Ip_u64)work->word2.s.is_v6 << (32 + 24)) + ((Ip_u64)(work->tag) << 32) + (Ip_u32)p;
#endif
        msg = (fw_msg_t *)&work->packet_data;

#ifdef IPCOM_USE_FORWARDER_VLAN
        if (work->word2.s.vlan_valid)
        {
            msg->tbl[0] = work->word2.s.is_v6 ? IPCOM_FORWARDER_PACKET_TYPE_IPV6_VLAN : IPCOM_FORWARDER_PACKET_TYPE_IPV4_VLAN;
            msg->vlan_id = work->word2.s.vlan_id;
        }
#endif

#ifdef IPCOM_USE_FORWARDER_IPSEC
        msg->len = work->len;
        msg->orig_packet = work->packet_ptr.s.addr;
        /* Attempt IPsec decryption if dec_ipsec bit is set and exception bits are cleared */
        w2 = work->word2.u64 & 0x00037f00;
        if (w2 == 0x00020000)
            msg->ipsec_dec = IP_AF_INET;
        else if (w2 == 0x00030000)
            msg->ipsec_dec = IP_AF_INET6;
        else
            msg->ipsec_dec = 0;
#endif
    }

wait:
    /* Check for potential cache update */
    IPCOM_FWD_WAIT();

    return msg;
}

/******************************************************************************
*
* ipcom_forwarder_tx - Transmit a Cavium packet
*
* Cavium uses arrays of command words to present outgoing packets to the HW. In
* this case two command word describe each packet. Once the words have been
* written to the array the HW is triggered. The arrays are linked together via
* the "last" entry and the HW will automatically free the array when the link
* has been read.
*
* In the forwarding case no "transmit done" interrupt is needed. Therefore,
* two array words per packet are required. The standard output routine,
* 'octEndSend' using 'octEndEncap', which does require a "transmit done"
* indication uses three.
*
* Because the forwarders and the END have different output queues, these
* differences do not cause any problems.
*
* ERRNO: N/A
*/
IP_PUBLIC int
ipcom_forwarder_tx(int port_index, fw_msg_t *fw_msg, void * fw_cookie)
{
    UINT64         *u;
    UINT64         *p;
    cvmx_wqe_t *work = (cvmx_wqe_t *)((Ip_u8 *)fw_msg - ip_offsetof(cvmx_wqe_t, packet_data));
    fw_info_t * info = fw_cookie;
    fwd_t *pTx = info->txQs[port_index];

#ifdef IPCOM_USE_FORWARDER_IPSEC
    if (cvmx_unlikely(ipcom_forwarder_ipsec_single_output_queue))
    {
        cvmx_pow_tag_sw((CVMX_TAG_SW_BITS_INTERNAL << CVMX_TAG_SW_SHIFT)
                        + IPCOM_FORWARDER_INTERNAL_TAG_PORT_GROUP
                        + pTx->octPort, CVMX_POW_TAG_TYPE_ATOMIC);
        /* flush pending writes */
        CVMX_SYNCWS;
        cvmx_pow_tag_sw_wait();
    }
#endif

    u = pTx->octTxChunk;
    pTx->octTxChunk += 2;
    pTx->octTxFree  -= 2;

#ifdef IPCOM_USE_FORWARDER_IPSEC
    ipcom_forwarder_update_work(fw_msg, work);
#endif

    /*
     * Invariant: We always have at least 3 slots worth of space
     * left in the current chunk.  When we have exactly 3 slots,
     * we must allocate a new chunk and store the next chunk pointer
     * in the current chunk.
     */

    /* Shift the number of segments by 16 and add in the length */
    u[0]        = (UINT64)((1 << 16) + work->len);
    u[1]        = work->packet_ptr.u64;

    /*
     * If we originally had more than 3 slots left, we don't need
     * to allocate a new chunk. (Note, we must store the new
     * chunk pointer before ringing the doorbell for the last
     * packet in the chunk.)
     */
    if (cvmx_likely( pTx->octTxFree > 1 ))
    {
        cvmx_pko_doorbell (pTx->octPort, pTx->octQueue, 2);
#ifdef IPCOM_OCTEON_USE_SMALL_PACKET
        if (cvmx_unlikely(!work->packet_ptr.s.pool))
        {
            cvmx_fpa_free(work, CVMX_FPA_WQE_POOL, 0);
        }
#else
        cvmx_fpa_free(work, CVMX_FPA_WQE_POOL, 0);
#endif
        return OK;
    }

    /* Slow path must allocate a new chunk */
    p = (UINT64 *)cvmx_fpa_alloc (CVMX_FPA_OUTPUT_BUFFER_POOL);
    if (cvmx_unlikely(p == NULL))
    {
        if (cvmx_likely(!work->packet_ptr.s.pool))
        {
            fw_msg = cvmx_phys_to_ptr (((work->packet_ptr.s.addr >> 7) -
                                        work->packet_ptr.s.back) << 7);
            cvmx_fpa_free (fw_msg, CVMX_FPA_PACKET_POOL, 0);
        }
        cvmx_fpa_free(work, CVMX_FPA_WQE_POOL, 0);

        /* Restore the bumped values for the next time */
        pTx->octTxChunk -= 2;
        pTx->octTxFree  += 2;

        return EAGAIN;
    }

    /* Set up octTxFree for the new chunk. */
    pTx->octTxFree  = CVMX_FPA_OUTPUT_BUFFER_POOL_ENTRIES;
    pTx->octTxChunk = p;

    /* Store the next-chunk pointer */
    u[2]            = cvmx_ptr_to_phys (p);

    cvmx_pko_doorbell (pTx->octPort, pTx->octQueue, 2);

#ifdef IPCOM_OCTEON_USE_SMALL_PACKET
    if (cvmx_unlikely(!work->packet_ptr.s.pool))
    {
        cvmx_fpa_free(work, CVMX_FPA_WQE_POOL, 0);
    }
#else
    cvmx_fpa_free(work, CVMX_FPA_WQE_POOL, 0);
#endif

    return OK;
}

#endif /* IPCOM_USE_OCTEON_FORWARDER */

#endif /* __GNUC__ :This file requires GNU Compiler */

/*
 ******************************************************************************
 * End of file
 ******************************************************************************
*/
