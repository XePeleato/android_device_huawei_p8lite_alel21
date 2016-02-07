/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_vxworks_nehalem_forwarder.h,v $ $Revision: 1.1.4.1.2.4 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/include/ipcom_vxworks_nehalem_forwarder.h,v $
 *     $Author: dlkrejsa $ $Date: 2010-08-10 16:10:25 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Roger Boden <roger@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_VXWORKS_NEHALEM_FORWARDER_H
#define IPCOM_VXWORKS_NEHALEM_FORWARDER_H

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

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_config.h>

#include <ipcom_cstyle.h>
#include <ipcom_type.h>

#include <ipcom_pkt.h>
#include <ipcom_forwarder.h>
#include <ipcom_ipc_spinlock.h>
#include <ipcom_shared_mem.h>
#include <ipcom_ifproxy_proto.h>

#include <vxWorks.h>
#include <jobQueueLib.h>
#include <endLib.h>
#include <cpuset.h>
#include <vxCpuLib.h>

#ifdef IPCOM_FORWARDER_AMP
#include <multios_ipc/mipc.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_FW_DEBUG
#define IPCOM_FW_LOG_SIZE	(0x4000)

#define IPCOM_FORWARDER_SHOW

#define IPCOM_USE_FW_STATS

/*
 * One arbitrary limit on the number of forwarder cores in the
 * system.  The number of entries in the array ipcom_fw_info
 * of Ipcom_forwarder_info_t pointers.
 *
 * Probably limited by the number of TX queues on an 82576.
 */
#define IPCOM_MAX_FORWARDERS    MAX_FW  /* from ipcom_forwarder.h */

#ifdef IPCOM_FORWARDER_AMP
#ifdef IPCOM_USE_NEHALEM_HYPERTHREAD
#define IPCOM_FW_MAX_CORES()    (24)
#else
#define IPCOM_FW_MAX_CORES()    (12)
#endif
#else
#define IPCOM_FW_MAX_CORES()    (vxCpuConfiguredGet())
#endif

/*
 * Maximum number of RX queues that a single forwarder can poll.
 * More or less arbitrary. This number times the number of forwarder
 * cores is one upper bound on the number of ports from which any
 * forwarder receives packets.
 */
#define IPCOM_FW_RX_QUEUES_MAX  8

/*
 * Maximum number of ports in the system, to which forwarders
 * can output packets. This is the same as the number of output queues
 * that a single forwarder can send to (since each forwarder outputs
 * to one queue on each usable port).
 *
 * More or less arbitrary.
 */
#define IPCOM_FW_TX_QUEUES_MAX  MAX_FW_PORTS  /* from ipcom_forwarder.h */
#define IPCOM_FW_PORTS_MAX IPCOM_FW_TX_QUEUES_MAX

#define IPCOM_FW_MAX_PKTS_ALLOC_PER_TXQ 32
#define IPCOM_FW_MAX_PKTS_ALLOC_PER_RXQ (2 * 512)

/*
 * To avoid having to potentially change the size of the
 * packet pointer array used in the forwarder's packet pool,
 * we make the array of the largest possible size.
 * Current value 20 * 32 + 8 * 2 * 128 = 4736 pointers.
 * Small potatoes compared to packet buffer memory usage.
 */
#define IPCOM_FW_MAX_PKTS \
    (IPCOM_FW_TX_QUEUES_MAX * IPCOM_FW_MAX_PKTS_ALLOC_PER_TXQ + \
     IPCOM_FW_RX_QUEUES_MAX * IPCOM_FW_MAX_PKTS_ALLOC_PER_RXQ)

/*
 * Maximum number of consecutive packets that a forwarder may
 * retrieve from a single RX queue before switching to the next
 * RX queue that it polls (if any).
 */
#define IPCOM_FW_MAX_1Q 32

/*
 * Maximum amount of RX work to do before checking low-frequency
 * work: TX cleanup, or slow path packet return or trigger events.
 */
#define IPCOM_FW_MAX_RX 64

#define IPCOM_FW_MAX_DRIVERS 2  /* gei and tei. Increase if more needed. */

/* Initial offset from start of Ipcom_fw_pkt_t to start of packet data */

/*
 * We align the start of the MAC header with the start of the cache
 * line for better DMA performance even though this requires some
 * unaligned accesses to the IP header.  The former has more of an
 * impact than the later at high loads. We might revisit this when
 * we support IPSec or other operations.
 */
#define IPCOM_FW_RXPKT_OFFSET  (256)    /* align start of MAC header */

#define IPCOM_FW_VIRT_TO_PHYS(x) ((unsigned long)(x))

/*
 * Special ipcom_fw_rx_poll() return values.
 */
#define IPCOM_FW_RX_POLL_BADPKT ((Ipcom_fw_pkt_t *)1)
#define IPCOM_FW_RX_POLL_NOBUFS ((Ipcom_fw_pkt_t *)2)

#undef FW_PKT_VAR_FREE
#ifdef FW_PKT_VAR_FREE
typedef void (*Ipcom_fw_pkt_free_rtn) (Ipcom_fw_pkt_t *, void *);
#define FW_PKT_FREE(pkt, pool) \
    do  {                               \
        Ipcom_fw_pkt_t * __p__ = (pkt); \
        __p__->free (__p__, (pool));    \
    } while ((0))
#else
#define FW_PKT_FREE(pkt, pool) ipcom_fw_pkt_free((pkt), (pool))
#endif

#define FW_MSG_TO_PKT(pMsg)  \
    ((Ipcom_fw_pkt_t *)((char *)(pMsg) - offsetof (Ipcom_fw_pkt_t, fw_msg)))

#define FW_PKT_TO_MSG(pkt) (&(pkt)->fw_msg)

#define FW_SLOW_JOB_TO_PKT(pJob) \
    ((Ipcom_fw_pkt_t *)((char *)(pJob) - offsetof (Ipcom_fw_pkt_t, job)))



/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

#ifdef IPCOM_USE_FORWARDER_IPSEC

typedef struct Ipcom_fw_sa_lock_s {
    ipc_spinlock_t lock;
#undef SA_LOCK_SIZE     /* Define as 128 for utmost performance and memory usage */
#ifdef SA_LOCK_SIZE
    Ip_u8 pad [SA_LOCK_SIZE - sizeof(ipc_spinlock_t)];
#endif
} Ipcom_fw_sa_lock_t;

#endif /* IPCOM_USE_FORWARDER_IPSEC */

/* Software-defined structures & types */

#ifdef IPCOM_FORWARDER_AMP
typedef Ip_ptrdiff_t Ipcom_fw_offset_t;

#define IPCOM_FW_MIPC_BUS_NAME         "main" /* MIPC bus used by ctrl nae / ifproxy */

/* (M)IPC message queues used by the forwarder. */

#define IPCOM_IPC_MSGQ_STATS

#ifdef IPCOM_IPC_MSGQ_STATS
#define IPCQ_STATS(x) do {x; } while ((0))
#else
#define IPCQ_STATS(x) do {} while ((0))
#endif

#define IPCOM_IPC_MSGQ_JOBS_MAX 64  /* max jobs per single MIPC callback */
typedef Ip_u32 Ipcom_ipc_msgq_cmd_num_t;

typedef struct Ipcom_ipc_msg_s
{
    Ipcom_fw_offset_t offset_next;
    Ipcom_ipc_msgq_cmd_num_t cmd;
} Ipcom_ipc_msg_t;

typedef void (*Ipcom_ipc_msgq_cmd) (Ipcom_ipc_msg_t *);

typedef struct Ipcom_ipc_msgq_s
{
    union
        {
        Ipcom_ipc_spinlock_t lock;
        char pad [128];
        } u;

    uint64_t express_data;
    struct mipc_sockaddr to;
    int sock;

    Ipcom_fw_offset_t msg_offset_head;  /* offset of first queued message */
    Ipcom_fw_offset_t msg_offset_tail;  /* offset of last queued message */

    Ipcom_fw_offset_t cached;

    Ipcom_ipc_msgq_cmd * cmds;
    Ip_u32 n_cmds;

    Ip_u32 badcmds;

#ifdef IPCOM_IPC_MSGQ_STATS
    Ip_u32 goodcmds;
    Ip_u32 reposts;
#endif

} Ipcom_ipc_msgq_t;

/* Shared memory anchor type */
typedef struct Ipcom_fw_anchor_s
{
    Ipcom_ipc_msgq_t msg_q;

    char pad [128];
    
    /* initial heartbeats */
    volatile Ip_u32 master_heartbeat;
    volatile Ip_u32 heartbeat [32]; /* Need one per core */

} Ipcom_fw_anchor_t;

#endif /* IPCOM_FORWARDER_AMP */

typedef struct Ipcom_fw_pkt_s Ipcom_fw_pkt_t;

typedef struct Ipcom_forwarder_info_s Ipcom_forwarder_info_t;

typedef struct fw_rx_queue_s fw_rx_queue_t;

typedef struct fw_tx_queue_s fw_tx_queue_t;

typedef struct Ipcom_fw_port_s Ipcom_fw_port_t;

typedef struct Ipcom_fw_port_drvinfo_s Ipcom_fw_port_drvinfo_t;

typedef struct Ipcom_fw_data_funcs_s  Ipcom_fw_data_funcs_t;

/*
 * For now, I'm assuming that each packet data buffer is associated with
 * a Ipcom_fw_pkt_t permanently, at a fixed offset from the Ipcom_fw_pkt_t.
 *
 * Hmm, if we don't make the Ipcom_fw_pkt_t control structure contiguous
 * with the packet's data buffer, then we have the possibility of
 * recycling the Ipcom_fw_pkt_ts faster than the data buffers (which have
 * to fifo through the RX ring), keeping a few of them in cache always.
 */
/* DATA */
struct Ipcom_fw_pkt_s
{
    fw_msg_t fw_msg;    /* how the generic forwarder represents a packet */

    int start;          /* offset of start of packet data */
    int len;            /* saved length of packet */

    Ip_u32 errsts;      /* errsts bits from RX descriptor; used in slow
                           path for RX checksum offload. Still little endian. */

#ifdef IPCOM_USE_FORWARDER_IPSEC
    Ip_u32 orighash;    /* saved original hash of destination IP address */
#endif

    union
    {
        /* Fast path: */
        fw_rx_queue_t * pRxQ;  /* What RX queue was the packet received on? */

        /* Slow path: */
        Ipcom_fw_port_t * fw_port;
    } rxu;
    
#ifdef FW_PKT_VAR_FREE
    Ipcom_fw_pkt_free_rtn free; /* how to free this packet buffer */
#endif
    void * pool;                /* points to Ipcom_forwarder_info_t for now */

#ifdef IPCOM_FORWARDER_AMP
#else
    QJOB job;                   /* For queueing packets to the slow path */
#endif
};


typedef void * Ipcom_fw_foreign_ptr;  /* somebody else's virtual address */

typedef struct Ipcom_fw_pkt_block_s
{
    /* all offsets relative to address of this structure */
    Ipcom_fw_offset_t pkt_off;    /* offset of first packet in block */
    Ip_u32 n_pkts;                /* number of packets in the block */
    Ip_u32 pktsize;               /* actual size  (period) for each packet */
} Ipcom_fw_pkt_block_t;


/* Configuration info about a TX queue passed to a forwarder */
typedef struct Ipcom_fw_txq_config_info_s {
    Ip_u64 txq_regs; /* Host phys. addr. for TXQ regs in PCI mem. space.
		      * Actually always a 32-bit quantity for now, and is
		      * the same for all guests.
		      */
    char ifname[IP_IFNAMSIZ];
} Ipcom_fw_txq_config_info_t;

/* Info about an RX queue held by the port */
typedef struct Ipcom_fw_rxq_port_info_s {
    Ip_s16 cpuid;   /* logical CPU number of forwarder polling the RX queue */
    Ip_u16 rxq_stats_ix; /* Index in the RX queue statistics for this queue */
} Ipcom_fw_rxq_port_info_t;

/* Configuration info about an RX queue passed to a forwarder */
typedef struct Ipcom_fw_rxq_config_info_s {
    Ip_u64 rxq_regs; /* Host phys. addr. for RXQ regs in PCI mem. space.
		      * Actually always a 32-bit quantity for now, and is
		      * the same for all guests.
		      */
    char ifname[IP_IFNAMSIZ];
    struct mipc_sockaddr ifproxy_addr;  /* socket address for slow-path */
    Ip_u16 port_index;
    Ip_u16 rxq_stats_ix; /* Index in RX queue statistics array for queue */
} Ipcom_fw_rxq_config_info_t;

struct fw_tx_queue_s
{
    int (*fw_tx) (fw_tx_queue_t *txQ, Ipcom_fw_pkt_t * pkt);
    void (*fw_tx_clean) (fw_tx_queue_t * txQ, Ipcom_forwarder_info_t * info,
                         Ip_bool send_deferred);
    Ipcom_fw_data_funcs_t * funcs;
    Ip_bool deferred; /* If true, deferred transmit */
};

typedef struct rxq_statistics_s
{
    Ip_u32 non_fw_frames;
    Ip_u32 non_fw_bytes;
    Ip_u32 dropped_packets;
    Ip_u32 dropped_octets;

} rxq_statistics_t;

struct fw_rx_queue_s
{
    Ipcom_fw_pkt_t * (*rx_poll) (fw_rx_queue_t * rxQ);
    Ipcom_fw_data_funcs_t * funcs;
#ifdef IPCOM_FORWARDER_AMP
    rxq_statistics_t *stats;
    struct mipc_sockaddr ifproxy_addr;
#endif
};

/* low priority */
#if 0
#define IPCOM_FW_EVT_PKTS_TO_FREE       (0x01U)
#endif
#define IPCOM_FW_EVT_TRIGGER_PIPE_READ  (0x02U)

/* high priority */
#define IPCOM_FW_EVT_JOB                (0x10000)

#undef IPCOM_FW_HIGH_EVENTS

#ifdef IPCOM_FW_EVT_PKTS_TO_FREE
/*
 * Stack of packets freed in slow path by tNet0, to be (eventually)
 * returned by the forwarder to its own pool.
 *
 * A possibility to consider is instead of using a separate
 * packet stack (for slow-path packet frees) for each forwarder,
 * use a shared stack (could use chunking), accessed by forwarders
 * only if their own private packet supplies run out. One would
 * not need to notify a forwarder when freeing a packet in that
 * case.
 */
/*
 * DATA and CTRL; in shared memory. One instance per forwarder.
 * Part of Ipcom_fw_low_events_t, below.
 */
typedef struct Ipcom_fw_pkt_stack_s
{
    Ipcom_ipc_spinlock_t lock;
#ifdef IPCOM_FORWARDER_AMP
    Ipcom_fw_offset_t pkt_off;
#else
    Ipcom_fw_pkt_t * pkts;
#endif
} Ipcom_fw_pkt_stack_t;
#endif /* IPCOM_FW_EVT_PKTS_TO_FREE */

typedef struct Ipcom_fw_low_events_s
{
    atomic_t events; /* MULTIOS_ATOMIC ? */

#ifdef IPCOM_FW_EVT_PKTS_TO_FREE
    Ipcom_fw_pkt_stack_t pktStack;
#endif

} Ipcom_fw_low_events_t;

#ifdef IPCOM_FW_HIGH_EVENTS
#ifdef IPCOM_FW_EVT_JOB
typedef struct Ipcom_fw_job_s  Ipcom_fw_job_t;

struct Ipcom_fw_job_s
{
    /* TODO: review whether making an ifproxy command header into a
       job this way makes sense ... */
    /* (Ipcom_fw_job_t *); offset relative to Ipcom_fw_high_events_t */
    Ipcom_fw_offset_t next_off;
    Ipcom_fw_offset_t aux; /* presently unused */
    ifproxy_cmd_hdr_t hdr;
};

typedef struct Ipcom_fw_tx_job_s
{
    Ipcom_fw_job_t job;
    Ipcom_fw_pkt_t pkt;
} Ipcom_fw_tx_job_t;

typedef struct Ipcom_fw_job_queue_s
{
    Ipcom_ipc_spinlock_t lock;
    /* Offsets relative to encapsulating Ipcom_fw_high_events_t */
    /* (Ipcom_fw_job_t *) */
    Ipcom_fw_offset_t head_off;
    /* (Ipcom_fw_job_t **) */
    Ipcom_fw_offset_t ptail_off;

} Ipcom_fw_job_queue_t;

typedef struct Ipcom_fw_job_pool_s
{
    Ipcom_ipc_spinlock_t lock;
    /* Offsets relative to Ipcom_fw_job_pool_t */
    /* (Ipcom_fw_job_t *) */
    Ipcom_fw_offset_t head_off;

} Ipcom_fw_job_pool_t;

#endif /* IPCOM_FW_EVT_JOB */

typedef struct Ipcom_fw_high_events_s
{
    atomic_t events; /* MULTIOS_ATOMIC ? */

#ifdef IPCOM_FW_EVT_JOB
    Ipcom_fw_job_queue_t jobq;
#endif

} Ipcom_fw_high_events_t;
#endif /*  IPCOM_FW_HIGH_EVENTS */

/* Used by Control NAE */
typedef struct Ipcom_fw_event_ptrs_s
{
    Ipcom_fw_low_events_t * low;
#ifdef IPCOM_FW_HIGH_EVENTS
    Ipcom_fw_high_events_t * high;
#endif
} Ipcom_fw_event_ptrs_t;

#ifdef IPCOM_FORWARDER_AMP
#define IPCOM_FW_PKT_STACK_CLEAR(pStack, pkt) \
    do  { Ipcom_fw_offset_t off_ = (pStack)->pkt_off; \
          (pStack)->pkt_off = 0; \
          if (off_ == 0) \
              pkt = IP_NULL; \
          else \
              pkt = (Ipcom_fw_pkt_t *)((char *)pStack + off_);  \
    } while ((0))
#define IPCOM_FW_PKT_STACK_NEXT(pkt, next) \
    do {Ipcom_fw_offset_t off_ = *(Ipcom_fw_offset_t *)(pkt); \
        if (off_ == 0) \
            next = IP_NULL; \
        else \
            next = ((Ipcom_fw_pkt_t *)((char *)(pkt) + off_)); \
    } while ((0))
#define IPCOM_FW_PKT_STACK_PUSH(pStack, pkt, old)                        \
    do {Ipcom_fw_offset_t off_ = (char *)pkt - (char *)(pStack); \
        Ipcom_fw_offset_t oldoff_ = (pStack)->pkt_off; \
        (pStack)->pkt_off = off_; \
        if (oldoff_ == 0) \
            *(Ipcom_fw_offset_t *)pkt = 0; \
        else \
            *(Ipcom_fw_offset_t *)pkt = oldoff_ - off_; \
        old = (void *)oldoff_; \
    } while ((0))
#define IPCOM_FW_PKT_STACK_ZERO(pStack) \
    do {(pStack)->pkt_off = 0;} while ((0))
#else

#define IPCOM_FW_PKT_STACK_CLEAR(pStack, pkt)                           \
    do { pkt = (pStack)->pkts; (pStack)->pkts = IP_NULL; } while ((0))
#define IPCOM_FW_PKT_STACK_NEXT(pkt, next) \
    do {next = *(Ipcom_fw_pkt_t **)(pkt);} while ((0))
#define IPCOM_FW_PKT_STACK_PUSH(pStack, pkt, old)         \
    do {old = (pStack)->pkts; \
        *(Ipcom_fw_pkt_t **)pkt = old;  \
        (pStack)->pkts = pkt; \
    } while ((0))
#define IPCOM_FW_PKT_STACK_ZERO(pStack) \
    do {(pStack)->pkts = IP_NULL;} while ((0))
#endif /* IPCOM_FORWARDER_AMP */

#define FW_PREFETCH_Q_SIZE 1

enum fw_q_type {
    FW_Q_RX,
    FW_Q_TX
};


/* DATA */
struct Ipcom_forwarder_info_s
{
    /* packet pool */
    Ip_u32 pkt_pool_index;     /* TOS: index of 1st (empty) slot;
                                   number of filled slots. */
    /* Polling state machine counter */
    Ip_u32 counter;     /*
                         * 0: Poll for TX cleanup, slow path packet return,
                         *    or trigger message.
                         * 1 .. IEI_FW_MAX_RX: Do RX polling.
                         * Note, TX cleanup polling is not critical,
                         * since we do some TX cleanup during transmission.
                         * It may, however, help return transmitted packet
                         * buffers faster, which _might_ help performance.
                         * TBD.
                         */

    Ipcom_fw_pkt_t * prefetchQ [FW_PREFETCH_Q_SIZE];
    Ip_u32 rxQreps; /* could combine with rxQcur... */

    /* One RX queue for each interface that we should poll (default 1) */
    Ip_u32 nRxQs;
    Ip_u32 rxQcur;
    fw_rx_queue_t * rxQs [IPCOM_FW_RX_QUEUES_MAX];

    Ip_u32 fw_key_mask;  /* For masking forwarder hash key. */
    Ip_u32 *fw_secret_seed;  /* For creating hash. Points to shared memory.
                              * TODO: message notification when seed changes,
                              * so we don't need to do a dereference per packet?
                              */

    /* One output queue per destination port */
    Ip_u32 nTxQs;
    Ip_u32 txQcur;
    fw_tx_queue_t * txQs [IPCOM_FW_TX_QUEUES_MAX];
    
#ifdef IPCOM_FW_HIGH_EVENTS
    Ipcom_fw_high_events_t * high_events;
#endif
    Ipcom_fw_low_events_t * low_events;

    Ipcom_fw_anchor_t * anchor; /* the shared memory anchor */
    int sock;       /* MIPC socket descriptor for slow path */

    fw_msg_t triggerMsg;

    unsigned int cpuId;

    int index; /* forwarder index */

    void * msg_sync; /* reusable message buffer for synchronous IPC messages */

    rxq_statistics_t *rxq_stats_base;

    Ip_u16 pkt_buf_size; /* Full size of packet buffers */
    Ip_u16 nominal_mtu;

    Ip_u32 neededPkts;  /* Number of packets needed by this forwarder to
			   support currently assigned queues */
    Ip_u32 maxPkts;	/* Actual number of packets owned by this forwarder */

    /*
     * The forwarder's packet pool, accessed only by the forwarder task.
     *
     * Note that all the packets in a forwarder's pool are the same size.
     * If a forwarder receives from more than one port, its packet buffers
     * should be large enough for all of the ports that it receives from.
     * We presently enforce that all forwarder-capable ports use the same
     * MTU. It's problematic in any case to enable jumbo frames on some ports
     * but not others -- any fragmentation would likely mean taking the
     * slow path.
     *
     * Must be last.
     */
    Ipcom_fw_pkt_t * pkts [IPCOM_FW_MAX_PKTS]; /* packet pool */
};

/*
 * Driver-specific functions called by the forwarder NAE.
 * Does not include data-path rx_poll, fw_tx, and fw_tx_clean
 * functions; these are called in the control plane (but not
 * by the control NAE).
 */
struct Ipcom_fw_data_funcs_s
{
    char * drvname;  /* e.g. "gei", "tei" */
#ifdef IPCOM_FORWARDER_LOCAL_STACK
    Ipcom_ipc_msgq_cmd_num_t rx_slow_cmd;
#endif
    int (*fw_txq_add) (Ipcom_forwarder_info_t * info, void * txq_regs,
		       Ip_u32 port_index, char ifname[IP_IFNAMSIZ]);
    int (*fw_txq_remove) (Ipcom_forwarder_info_t * info, Ip_u32 port_index);
    int (*fw_rxq_add) (Ipcom_forwarder_info_t * info, void * rxq_regs,
		       Ip_u32 port_index,  rxq_statistics_t *stats,
		       struct mipc_sockaddr *ifproxy_addr,
		       char ifname[IP_IFNAMSIZ]);
    int (*fw_rxq_remove) (Ipcom_forwarder_info_t *info,
			  fw_rx_queue_t * pRxQ);
#ifdef IPCOM_FORWARDER_SHOW
    void (*fw_txq_show) (Ipcom_forwarder_info_t *info, int ix);
    void (*fw_rxq_show) (Ipcom_forwarder_info_t *info, int ix);
#endif
};

/* CTRL */
typedef struct Ipcom_fw_port_funcs_s
{
    /*
     * Update port->fw_stats from hardware. If 'reset',
     * clear statistics.
     */
    void (*stats_update) (Ipcom_fw_port_t *port, Ip_bool reset);
    /*
     * If 'enable', redirect IP packets to forwarders. Otherwise,
     * redirect packets to normal path processing.
     */
    void (*fw_redirect) (Ipcom_fw_port_t * port, Ip_bool enable);

    void (*port_drvinfo) (Ipcom_fw_port_t * port,
                          Ipcom_fw_port_drvinfo_t * drvinfo);

    Ip_u64 (*fw_qaddr) (Ipcom_fw_port_t * port,
			Ip_u32 q_index,
			enum fw_q_type q_type);
} Ipcom_fw_port_funcs_t;


/* This is embedded in the driver control structure.
   It is not in shared memory. */
/* CTRL.  Data uses Ipcom_fw_port_data_t, above. */
struct Ipcom_fw_port_s
{
    END_OBJ * pEnd;
    Ip_u32 pktsize;     /* actual size of each packet buffer */

    Ip_bool fw_capable;
    Ip_u32  port_index; /* forwarder port index */

    Ipcom_fw_rxq_port_info_t rxqi [IPCOM_MAX_FORWARDERS];

    Ip_u32              numTxQs; /* Not counting the default queue */
    Ip_u32              numRxQs; /* Not counting the default queue */

    Ip_bool             redirState; /* Is traffic redirected to forwarders? */
    Ip_u32              nRxPollers; /* Number of active RX queues polled
                                        by forwarders (1 forwarder/queue)
                                        on this port. */
#ifdef IPCOM_FORWARDER_AMP
    struct mipc_sockaddr ifproxy_addr; /* where to send slow path packets */
#endif

    Ipcom_fw_port_funcs_t * funcs;  /* used only by master NAE or mgmt core */
    port_statistics_t   stats;  /* used only by master NAE or mgmt core */

    /* Old aggregated RX queue statistics */
    rxq_statistics_t rxq_stats_aggr_old;
};

/* CTRL */
struct Ipcom_fw_port_drvinfo_s
{
    Ip_u32              maxMtu;
    /*
     * The following two members are hints from the driver on how
     * many packet buffers the forwarder should allocate per RX queue
     * and per TX queue on this port.
     *
     * Typically, bufsPerRxQ is the number of descriptors in a forwarder RX
     * ring, and bufsPerTxQ is the TX cleanup threshold for a TX ring.
     * The Nehalem forwarder code will multiply these by a sharing factor
     * (2 at present).
     */
    Ip_u32              bufsPerRxQ;
    Ip_u32              bufsPerTxQ;
};

/* CTRL */
typedef struct Ipcom_fw_port_info_s
{
    char ifname[IP_IFNAMSIZ];  /* e.g. "gei3", "tei0". Null terminated. */
    cpuset_t pollers;          /* cores that poll this port for RX packets */
} Ipcom_fw_port_info_t;


#ifdef IPCOM_FORWARDER_AMP

/* Big enough for all sychronous commands: */
#define IPCOM_FW_SYNC_MSG_SIZE  2048

/* Forwarder IPC message queue command types */

enum Ipcom_fw_msgq_cmd_e
{
    IPCOM_FW_MSGQ_CMD_GETINFO = 0,
    /* IPCOM_FW_MSGQ_CMD_LOGMSG, */

#ifdef IPCOM_FORWARDER_LOCAL_STACK
    /*
     * We use separate RX slow path commands for different supported
     * (or eventually supported) drivers, to avoid an extra level
     * of function call for each slow path packet.
     */
    IPCOM_FW_MSGQ_CMD_SLOWPKT_GEI,
    IPCOM_FW_MSGQ_CMD_SLOWPKT_TEI,
#else
    IPCOM_FW_MSGQ_CMD_SLOWPKT_NOSTACK, /* send direct to mgmt OS */
#endif

    /* Must be last. Number of commands. */
    IPCOM_FW_MSGQ_NUM_CMDS
};

typedef struct Ipcom_fw_cmd_getinfo_s
{
    Ipcom_ipc_msg_t ipc_msg;
    unsigned int cpu;           /* sender's system CPU core index (0..N) */

    /*
     * The remaining fields set by message recipient (tNet0 on management
     * core).
     */

    int fw_ix;          /* Forwarder index, 1..IPCOM_MAX_FORWARDERS.
                         * Zero if this core should not act as a forwarder.
                         */

    /* Information to set up ipcom_fw variable */
    Ipcom_fw_offset_t rt_cache_id_off;
    Ipcom_fw_offset_t fw_lock_off;
    Ipcom_fw_offset_t fw_cache_tbl_v4_off;
    Ipcom_fw_offset_t fw_cache_tbl_v6_off;
    int fw_v4_entries;
    int fw_v6_entries;
    Ipcom_fw_offset_t fw_port_off;
#ifdef IPCOM_USE_FORWARDER_IPSEC
    Ipcom_fw_offset_t fw_cache_tbl_enc_off;
    Ipcom_fw_offset_t fw_cache_tbl_dec_off;
    int fw_ipsec_entries;
    /* forwarder has to set up fw_ipsec_func for itself */
    Ipcom_fw_offset_t fw_sa_locks_off;
#endif
#ifdef IPCOM_FORWARDER_USE_UPPER_TAG_BITS
    int fw_key_shift;                           /* OK dup (same for all) */
#endif
    

    Ip_u32 fw_key_mask;  /* For masking forwarder hash key */
    Ipcom_fw_offset_t fw_secret_seed_off;  /* For creating hash. */

#ifdef IPCOM_FW_HIGH_EVENTS
    /* Message-relative offset to Ipcom_fw_high_events_t */
    Ipcom_fw_offset_t high_events_off;
#endif

    /* Message-relative offset to Ipcom_fw_low_events_t */
    Ipcom_fw_offset_t low_events_off;

    void * low_events_mgmt; /* master NAE (or management core) virtual address
                               for Ipcom_fw_low_events_t (packet stack). */
    Ipcom_fw_rxq_config_info_t rxqc [IPCOM_FW_RX_QUEUES_MAX];

    Ipcom_fw_offset_t rxq_stats_off;

    Ip_u16 pkt_buf_size; /* Full size of packet buffers */
    Ip_u16 nominal_mtu;
    
    /*
     * Message-relative offsets to Ipcom_fw_tx_queue_shared_t structures
     * for this forwarder, for all ports. Zero offsets indicate unused slots.
     *
     * MUST BE LISTED IN FORWARDER PORT-INDEX ORDER!
     */
    Ipcom_fw_txq_config_info_t txqc [IPCOM_FW_PORTS_MAX];

    char pad [256];
    Ip_bool response_flag; /* set by management tNet0 after it has
                              set above information (& executed a memory
                              barrier) to indicate that response is ready. */
    
} Ipcom_fw_cmd_getinfo_t;

/*
 * Note, the command message structure for the SLOWPKT commands
 * is just Ipcom_fw_pkt_t.
 */

#endif /* IPCOM_FORWARDER_AMP */

/*
 ****************************************************************************
 * 6                    GLOBAL DATA
 ****************************************************************************
 */

#if 0
IP_EXTERN const char ipcom_fw_ports_cfg [];
#endif

IP_EXTERN Ipcom_fw_port_info_t * ipcom_fw_port_info;

IP_EXTERN Ipcom_forwarder_info_t * ipcom_fw_info[IPCOM_MAX_FORWARDERS];

IP_EXTERN int ipcom_fw_num;

IP_EXTERN Ipcom_ipc_msgq_cmd ipcom_fw_ipc_msgq_cmds [IPCOM_FW_MSGQ_NUM_CMDS];
    
/*
 ****************************************************************************
 * 7                    FUNCTIONS
 ****************************************************************************
 */



/*
 *===========================================================================
 *                    ipcom_fw_pkt_alloc
 *===========================================================================
 * Context:        called from forwarder task for <info>
 * Description:    Allocate a forwarder packet buffer from the forwarder's pool
 * Parameters:     info - the calling forwarder
 * Returns:        Pointer to the allocated packet buffer, or IP_NULL.
 */
/* DATA */
IP_INLINE Ipcom_fw_pkt_t *
ipcom_fw_pkt_alloc(Ipcom_forwarder_info_t * info)
{
    Ipcom_fw_pkt_t * pkt;
    Ip_u32 index = info->pkt_pool_index;

    if (IP_UNLIKELY(index == 0))
        return IP_NULL;

    /*
     * We manage the pool as a stack of packet pointers.
     * Between calls, pkt_pool_index is the number of packets
     * in the pool, and info->pkts[info->pkt_pool_index] is the first
     * slot of the array not holding a packet.
     *
     * There is no mutual exclusion since only the forwarder
     * task accesses the packet stack.
     */

    info->pkt_pool_index = --index;
    pkt = info->pkts[index];

    return pkt;
}


/*
 *===========================================================================
 *                    ipcom_fw_pkt_free
 *===========================================================================
 * Context:        called from forwarder task for <info>
 * Description:    free a forwarder packet buffer from the forwarder's pool
 * Parameters:     pkt - the packet buffer to be freed
 *                 info - the calling forwarder
 * Returns:        N/A
 */
/* DATA */
IP_INLINE void
ipcom_fw_pkt_free(Ipcom_fw_pkt_t *pkt, Ipcom_forwarder_info_t *info)
{
    /*
     * In a debug version we might check for overflow
     * (i.e., more frees than allocs), but normally
     * we assume that won't happen.
     */
    info->pkts[info->pkt_pool_index++] = pkt;
}

/* TODO: make some of these IP_GLOBAL rather than IP_PUBLIC */

IP_GLOBAL void ipcom_fw_packet_done(ifproxy_cmd_hdr_t *buf, void *cookie);

IP_PUBLIC int ipcom_fw_packet_send(ifproxy_cmd_hdr_t *hdr, void *free_arg);
IP_PUBLIC void ipcom_fw_pkt_remote_free(void *pktbuf,
                                        Ipcom_fw_low_events_t * low);

IP_PUBLIC int ipcom_fw_pkt_blk_alloc(Ipcom_fw_pkt_block_t * pkt_block,
                                     unsigned int cpuid, Ip_u32 n_pkts);
IP_PUBLIC void ipcom_fw_pkt_blk_free(Ipcom_fw_pkt_block_t * pkt_block);

IP_PUBLIC void * ipcom_fw_netif_to_end(Ipcom_netif *netif);

IP_PUBLIC int ipcom_fw_port_install(Ipcom_fw_port_t *fw_port,
                                    Ip_u32 mtu, const Ip_u8 *hwaddr);
IP_PUBLIC int ipcom_fw_port_remove(Ipcom_fw_port_t *fw_port);

IP_PUBLIC int ipcom_fw_data_funcs_register(Ipcom_fw_data_funcs_t *dfuncs);
IP_PUBLIC Ip_u32 ipcom_fw_pkt_size(void);

IP_PUBLIC int ipcom_fw_pkts_add(Ipcom_forwarder_info_t *info, Ip_u32 num_pkts);
IP_PUBLIC void ipcom_fw_pkts_release(Ipcom_forwarder_info_t *info,
				     Ip_u32 num_pkts);
    
/* In ipcom_vxworks_nehalem_forwarder_mipc.c */
IP_PUBLIC int ipcom_ifproxy_mipc_bus(void);
IP_GLOBAL void ipcom_ipc_msgq_cmd_slowpkt(Ipcom_ipc_msg_t *qjob);
IP_PUBLIC int ipcom_ipc_msgq_init(Ipcom_ipc_msgq_t *msgq, unsigned short busno,
                                  uint64_t express_data,
                                  Ip_u32 n_cmds, Ipcom_ipc_msgq_cmd *cmds);

/* In ipcom_config.c */
IP_PUBLIC int ipcom_forwarder_mtu(void);
IP_PUBLIC Ipcom_shared_mem_pool_t * ipcom_shared_mem_pool_for_core(unsigned int coreid);
void * ipcom_fw_anchor_get(void);

#ifdef IPCOM_FW_DEBUG
IP_PUBLIC int ipcom_fw_logmsg(char *fmt, ...);
IP_PUBLIC void ipcom_fw_log_dump(void);
/* Darn C99 variadic macros require at least one argument in the ... position */
#define FW_LOGMSG0(fmt) ipcom_fw_logmsg(fmt)
#define FW_LOGMSG(fmt, ...) \
    ipcom_fw_logmsg(fmt, __VA_ARGS__)
#else
#define FW_LOGMSG0(fmt) (0)
#define FW_LOGMSG(fmt, ...) (0)
#endif /* IPCOM_FW_DEBUG */

/*
 * The hashing code used here is adapted from Bob Jenkins' public-domain
 * lookup3 hash code.
 *
 * http://burtleburtle.net/bob/c/lookup3.c
 */
/*
-------------------------------------------------------------------------------
lookup3.c, by Bob Jenkins, May 2006, Public Domain.

These are functions for producing 32-bit hashes for hash table lookup.
hashword(), hashlittle(), hashlittle2(), hashbig(), mix(), and final() 
are externally useful functions.  Routines to test the hash are included 
if SELF_TEST is defined.  You can use this free for any purpose.  It's in
the public domain.  It has no warranty.

You probably want to use hashlittle().  hashlittle() and hashbig()
hash byte arrays.  hashlittle() is is faster than hashbig() on
little-endian machines.  Intel and AMD are little-endian machines.
On second thought, you probably want hashlittle2(), which is identical to
hashlittle() except it returns two 32-bit hashes for the price of one.  
You could implement hashbig2() if you wanted but I haven't bothered here.

If you want to find a hash of, say, exactly 7 integers, do
  a = i1;  b = i2;  c = i3;
  mix(a,b,c);
  a += i4; b += i5; c += i6;
  mix(a,b,c);
  a += i7;
  final(a,b,c);
then use c as the hash value.  If you have a variable length array of
4-byte integers to hash, use hashword().  If you have a byte array (like
a character string), use hashlittle().  If you have several byte arrays, or
a mix of things, see the comments above hashlittle().  

Why is this so big?  I read 12 bytes at a time into 3 4-byte integers, 
then mix those integers.  This is fast (you can do a lot more thorough
mixing with 12*3 instructions on 3 integers than you can with 3 instructions
on 1 byte), but shoehorning those bytes into integers efficiently is messy.
-------------------------------------------------------------------------------
*/

#define hashsize(n) ((uint32_t)1<<(n))
#define hashmask(n) (hashsize(n)-1)
#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

/*
-------------------------------------------------------------------------------
mix -- mix 3 32-bit values reversibly.

This is reversible, so any information in (a,b,c) before mix() is
still in (a,b,c) after mix().

If four pairs of (a,b,c) inputs are run through mix(), or through
mix() in reverse, there are at least 32 bits of the output that
are sometimes the same for one pair and different for another pair.
This was tested for:
* pairs that differed by one bit, by two bits, in any combination
  of top bits of (a,b,c), or in any combination of bottom bits of
  (a,b,c).
* "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
  the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
  is commonly produced by subtraction) look like a single 1-bit
  difference.
* the base values were pseudorandom, all zero but one bit set, or 
  all zero plus a counter that starts at zero.

Some k values for my "a-=c; a^=rot(c,k); c+=b;" arrangement that
satisfy this are
    4  6  8 16 19  4
    9 15  3 18 27 15
   14  9  3  7 17  3
Well, "9 15 3 18 27 15" didn't quite get 32 bits diffing
for "differ" defined as + with a one-bit base and a two-bit delta.  I
used http://burtleburtle.net/bob/hash/avalanche.html to choose 
the operations, constants, and arrangements of the variables.

This does not achieve avalanche.  There are input bits of (a,b,c)
that fail to affect some output bits of (a,b,c), especially of a.  The
most thoroughly mixed value is c, but it doesn't really even achieve
avalanche in c.

This allows some parallelism.  Read-after-writes are good at doubling
the number of bits affected, so the goal of mixing pulls in the opposite
direction as the goal of parallelism.  I did what I could.  Rotates
seem to cost as much as shifts on every machine I could lay my hands
on, and rotates are much kinder to the top and bottom bits, so I used
rotates.
-------------------------------------------------------------------------------
*/
#define mix(a,b,c) \
{ \
  a -= c;  a ^= rot(c, 4);  c += b; \
  b -= a;  b ^= rot(a, 6);  a += c; \
  c -= b;  c ^= rot(b, 8);  b += a; \
  a -= c;  a ^= rot(c,16);  c += b; \
  b -= a;  b ^= rot(a,19);  a += c; \
  c -= b;  c ^= rot(b, 4);  b += a; \
}

/*
-------------------------------------------------------------------------------
final -- final mixing of 3 32-bit values (a,b,c) into c

Pairs of (a,b,c) values differing in only a few bits will usually
produce values of c that look totally different.  This was tested for
* pairs that differed by one bit, by two bits, in any combination
  of top bits of (a,b,c), or in any combination of bottom bits of
  (a,b,c).
* "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
  the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
  is commonly produced by subtraction) look like a single 1-bit
  difference.
* the base values were pseudorandom, all zero but one bit set, or 
  all zero plus a counter that starts at zero.

These constants passed:
 14 11 25 16 4 14 24
 12 14 25 16 4 14 24
and these came close:
  4  8 15 26 3 22 24
 10  8 15 26 3 22 24
 11  8 15 26 3 22 24
-------------------------------------------------------------------------------
*/
#define final(a,b,c) \
{ \
  c ^= b; c -= rot(b,14); \
  a ^= c; a -= rot(c,11); \
  b ^= a; b -= rot(a,25); \
  c ^= b; c -= rot(b,16); \
  a ^= c; a -= rot(c,4);  \
  b ^= a; b -= rot(a,14); \
  c ^= b; c -= rot(b,24); \
}

/*
 *===========================================================================
 *                    ipcom_lookup3_hashword
 *===========================================================================
 * Context:
 * Description: Hash a key that is an array of 32-bit words
 *              Based upon lookup3's  hashword() function.
 * Parameters:  k : pointer to the hash key, an array of 32-bit words
 *              length : the number of 32-bit words in the key
 *              initval : an arbitrary value to initialize the hash.
 *
 * Returns: N/A
 *
 */
/* DATA */

IP_INLINE uint32_t
ipcom_lookup3_hashword(const uint32_t *k, size_t length, uint32_t initval)
{
    uint32_t a,b,c;

    /* Set up the internal state */
    a = b = c = 0xdeadbeef + (((uint32_t)length)<<2) + initval;

    /*--------------------------------------------- handle most of the key */
    while (length > 3)
    {
        a += k[0];
        b += k[1];
        c += k[2];
        mix(a,b,c);
        length -= 3;
        k += 3;
    }

    /*--------------------------------------- handle the last 3 uint32_t's */
    switch(length)                 /* all the case statements fall through */
    { 
    case 3 : c+=k[2];
    case 2 : b+=k[1];
    case 1 : a+=k[0];
        final(a,b,c);
    case 0:     /* case 0: nothing left to add */
        break;
    }
    /*-------------------------------------------------- report the result */
    return c;
}

#ifdef __cplusplus
}
#endif

#endif /* IPCOM_VXWORKS_NEHALEM_FORWARDER_H */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
