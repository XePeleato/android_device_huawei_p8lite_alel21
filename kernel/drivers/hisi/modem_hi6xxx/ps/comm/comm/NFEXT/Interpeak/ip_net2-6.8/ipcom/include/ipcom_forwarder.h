/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_forwarder.h,v $ $Revision: 1.44.20.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_forwarder.h,v $
 *   $Author: dlkrejsa $ $Date: 2010-08-10 16:10:24 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 ******************************************************************************
 */
#ifndef IPCOM_FORWARDER_H
#define IPCOM_FORWARDER_H

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

#include "ipcom_config.h"

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_sock.h>
#include "ipcom_netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Uncomment for debug printfs */
/* #define FW_DEBUG 1 */
/* #define FW_IPSEC_DEBUG 1 */

#define IP_FORWARDER_TASK_NAME  "ipfwdr"

#define IPCOM_FORWARDER_PACKET_TYPE_IPV4 (0)
#define IPCOM_FORWARDER_PACKET_TYPE_IPV6 (1)
#define IPCOM_FORWARDER_PACKET_TYPE_NONE (2)
#define IPCOM_FORWARDER_PACKET_TYPE_IPV4_VLAN (3)
#define IPCOM_FORWARDER_PACKET_TYPE_IPV6_VLAN (4)
#define IPCOM_FORWARDER_PACKET_IPSEC_DROP_PKT (5)

#define IPV4 (4)
#define IPV6 (6)
#define IP_IPv4 (0x0800)
#define IP_IPv6 (0x86dd)
#define IP_VLAN (0x8100)

#define LL_ETH_HDR_SZ (14)
#define LL_ETH_ADDR_SZ (6)
#define MAX_FW_LL_HDR_SZ (24)
#define IPCOM_FW_FLOW_SZ (256)
#define LL_ETH_VLAN_ADD (4)
#define LL_ETH_VLAN_TYPE_SZ (2)

#define FW_MSG_CONTROL_BASE      (48)    /* RIME: the value must be larger than the
                                            largest value octeon HW may write, i.e.
                                            >35 */
#define FW_MSG_TRIGGER_PIPE_READ (FW_MSG_CONTROL_BASE + 1)

#define FW_MSG(a)            ((a)->tbl[1])
#define IS_CONTROL_MSG_FW(m) (m->tbl[0] == IPCOM_FORWARDER_PACKET_TYPE_NONE && m->tbl[1] > FW_MSG_CONTROL_BASE)
#define IS_DROP_MSG_FW(m)    (m->tbl[0] == IPCOM_FORWARDER_PACKET_IPSEC_DROP_PKT)

#ifdef IPCOM_USE_OCTEON_FORWARDER
#define MAX_FW_PORTS (35)
#define MAX_FW (14)
#endif

#ifdef IPCOM_USE_NEHALEM_FORWARDER
#define MAX_FW_PORTS (20)
#ifdef IPCOM_USE_NEHALEM_HYPERTHREAD
#define MAX_FW (15)		/* limited by number of TX queues on 82576 */
#define PAUSE() do {_WRS_ASM("pause");} while ((0))
#else
#define MAX_FW (11)
#define PAUSE() do {} while ((0))
#endif /* IPCOM_USE_NEHALEM_HYPERTHREAD */
#else
#define PAUSE() do {} while ((0))
#endif /* IPCOM_USE_NEHALEM_FORWARDER */

/* Supported algorithms */
#define IPCOM_FORWARDER_IPSEC_3DES_MD5    0
#define IPCOM_FORWARDER_IPSEC_3DES_SHA1   1
#define IPCOM_FORWARDER_IPSEC_AES_MD5     2
#define IPCOM_FORWARDER_IPSEC_AES_SHA1    3
#define IPCOM_FORWARDER_IPSEC_ALG_MAX     4

#define IPCOM_FWD_IPSEC_CLEARTXT (0x0)
#define IPCOM_FWD_IPSEC_ENCRYPT  (0x1)
#define IPCOM_FWD_IPSEC_DECRYPT  (0x2)
#define IPCOM_FWD_IPSEC_NOCACHE  (0x3)

#ifdef IPCOM_USE_FORWARDER_IPSEC
#define IPCOM_USE_FORWARDER_UPDATE_CACHE_LOCK
#endif

#ifdef IPCOM_USE_FORWARDER_UPDATE_CACHE_LOCK
/* Lock forwarders before cache update */
#define IPCOM_FWD_LOCK()    ipcom_forwarder_update_cache_lock()
/* Release forwarders after cache update */
#define IPCOM_FWD_RELEASE() ipcom_forwarder_update_cache_unlock()
/* Busy wait until cache is updated */
#define IPCOM_FWD_WAIT()    do { \
                                if (FW_LOCK->update_cache_lock)         \
                                { \
                                    ipcom_atomic_inc(&FW_LOCK->update_cache_count); \
                                    ipcom_mb(); \
                                    while (FW_LOCK->update_cache_lock) \
					PAUSE(); \
                                    ipcom_atomic_dec(&FW_LOCK->update_cache_count); \
                                    ipcom_mb(); \
                                } \
                            } while((0))
#else
#define IPCOM_FWD_LOCK()    IP_NOOP
#define IPCOM_FWD_RELEASE() IP_NOOP
#define IPCOM_FWD_WAIT()    IP_NOOP
#endif

/* port number 1-3 are reserved for MSD/WDB/IPCOM */
#define IPCOM_FORWARDER_MIPC_PORT    4

/*
 * We reserve values 0..15 of pkt->nae_ctx to indicate
 * various non-forwarder packets. Code for different architectures
 * might use these reserved values in different ways. IP_NULL (0) is
 * one of the reserved values.
 */
#define NAE_CTX_MIN     16
#define IS_FORWARDER_PKT(ctx) ((char *)(ctx) >= (char *)NAE_CTX_MIN)

/* Allocation flags for ipcom_fw_cpu_mem_alloc() */
#define IPCOM_FW_CPU_MEM_ALLOC_DMA      1
#define IPCOM_FW_CPU_MEM_ALLOC_SHARED   2


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

typedef struct port_stats_s
{
    Ip_u32      octets_lw;
    Ip_u32      octets_hw;
    Ip_u32      packets;

    Ip_u32  fw_frames;
    Ip_u32  fw_bytes;

    Ip_u32      multicast_packets;
    Ip_u32      broadcast_packets;

    Ip_u32      len_64_packets;
    Ip_u32      len_65_127_packets;
    Ip_u32      len_128_255_packets;
    Ip_u32      len_256_511_packets;
    Ip_u32      len_512_1023_packets;
    Ip_u32      len_1024_1518_packets;
    Ip_u32      len_1519_max_packets;

    Ip_u32      runt_packets;
    Ip_u32      runt_crc_packets;

    Ip_u32      oversize_packets;
    Ip_u32      oversize_crc_packets;

    Ip_u32  dropped_octets;
    Ip_u32  dropped_packets;

    Ip_u32  tx_packets;
    Ip_u32  tx_octets_lw;
    Ip_u32  tx_octets_hw;

} port_stats_t;

#ifdef IPCOM_USE_OCTEON_FORWARDER
typedef Ip_u32 fw_octets_t;
#else
typedef Ip_u64 fw_octets_t;
#endif

typedef struct port_statistics_s
{
    Ip_u32      dropped_octets;
    Ip_u32      dropped_packets;
    Ip_u32      pci_raw_packets;
    fw_octets_t octets;
    Ip_u32      packets;
    Ip_u32      multicast_packets;
    Ip_u32      broadcast_packets;
    Ip_u32      len_64_packets;
    Ip_u32      len_65_127_packets;
    Ip_u32      len_128_255_packets;
    Ip_u32      len_256_511_packets;
    Ip_u32      len_512_1023_packets;
    Ip_u32      len_1024_1518_packets;
    Ip_u32      len_1519_max_packets;
    Ip_u32      fcs_align_err_packets;
    Ip_u32      runt_packets;
    Ip_u32      runt_crc_packets;
    Ip_u32      oversize_packets;
    Ip_u32      oversize_crc_packets;
    Ip_u32  inb_packets;
    Ip_u64  inb_octets;
    Ip_u64      inb_errors;

    Ip_u32  non_fw_frames;
    Ip_u32  non_fw_bytes;

    Ip_u32  tx_packets;
    Ip_u64  tx_octets;

} port_statistics_t;


struct Ip_fwreq
{
    char    eth_name[IP_IFNAMSIZ];    /* if name, e.g. "eth0" */

    union {

        port_stats_t ps;
    }u;

};

typedef int (* Ipcom_forwarder_ipsec_esp_encrypt)(Ip_u16 ekeylen, Ip_u8  *ekey,
                                                  Ip_u16 akeylen, Ip_u8  *akey,
                                                  Ip_u8  *esp,    Ip_u8  *iv,
                                                  Ip_u8  *pkt,    Ip_u16 pktlen,
                                                  Ip_u8  *out,
                                                  Ip_u8  *ictx,   Ip_u8  *octx);

typedef int (* Ipcom_forwarder_ipsec_esp_decrypt)(Ip_u16 ekeylen, Ip_u8  *ekey,
                                                  Ip_u16 akeylen, Ip_u8  *akey,
                                                  Ip_u8  *iv,
                                                  Ip_u8  *pkt,    Ip_u16 pktlen,
                                                  Ip_u8  *out,
                                                  Ip_u8  *ictx,   Ip_u8  *octx);


#include "ipcom_align16.h"
typedef IP_PACKED1 struct Ipcom_fw_ipsec_sa_s
{
    /* offset 0 */
    Ip_u32 spi;      /* Network endian */
    /* offset 4 */
    Ip_u32 seq;
    /* offset 8 */
    Ip_u8 domain;
    Ip_u8 proto;
    Ip_u8 algid;
    Ip_u8 replay;
    /* offset 12 */
    Ip_u8 ivlen;
    Ip_u8 akeylen;
    Ip_u8 ekeylen;
    Ip_u8 blocklen;
    /* offset 16 */
    Ip_u32 src[4];   /* Network endian */
    /* offset 32 */
    Ip_u32 dst[4];   /* Network endian */
    /* offset 48 */
    Ip_u8 ekey[32];  /* Max encryption key is 32 bytes (AES) */
    /* offset 80 */
    Ip_u64 iv[2];    /* Max IV is 16 bytes (AES) */
    /* offset 96 */
    Ip_u8  akey[20]; /* Max authentication key is 20 bytes (SHA1) */
    /* offset 116 */
    Ip_u32 cache_id;
    /* offset 120 */
    Ip_u32 replay_counter;
    /* offset 124 */
    Ip_u32 replay_bitmap;
    /* total 128 bytes */
    Ip_u8 ictx[64];  /* HMAC IPAD context */
    Ip_u8 octx[64];  /* HMAC OPAD context */
    /* total 256 bytes */
}
IP_PACKED2 Ipcom_fw_ipsec_sa_t;
#include "ipcom_align16.h"

typedef struct Ipcom_fw_ipsec_func_s
{
    Ipcom_forwarder_ipsec_esp_encrypt encrypt;
    Ipcom_forwarder_ipsec_esp_decrypt decrypt;
    char *id;
} Ipcom_fw_ipsec_func_t;

typedef struct fw_msg_s
{
    Ip_u8 tbl[2];
#ifdef FW_WIDE_HASH
    Ip_u32 fw_key;
#else
    Ip_u16 fw_key;
#endif
    Ip_u8 *packet;
#ifdef IPCOM_USE_FORWARDER_IPSEC
    Ip_u64 orig_packet;
    int len;
    Ip_u8 ipsec_dec;
#endif
#ifdef IPCOM_USE_FORWARDER_VLAN
    Ip_u16 vlan_id;
#endif
    int ipprt; /* test */
} fw_msg_t;

typedef struct fw_ip4_s
{
    Ip_u32 addr;
    Ip_u32 rci;

} fw_ip4_t;

typedef struct fw_ip6_s
{
    Ip_u64 addr[2];
    Ip_u32 rci;
#ifdef IPCOM_USE_FORWARDER_VLAN
    Ip_u16 type;
    Ip_u16 vlan_id;
#endif
} fw_ip6_t;

typedef struct fw_ip_vlan_s
{
    union {
        Ip_u32 addr_v4;
        Ip_u64 addr_v6[2];
    } a;
    Ip_u32 rci;
    Ip_u16 type;
    Ip_u16 vlan_id;

} fw_ip_vlan_t;

typedef union fw_mac_if_u
{
    Ip_u64 dst_mac64;
    Ip_u32 dst_mac[2];
    Ip_u8  dst_mac_u8[8];
    Ip_u8  ix[8];
    Ip_u8  enc[8];

} fw_mac_if_t;

typedef struct fw_ipsec_replay_s
{
    Ip_u16 replay;
    Ip_u16 key;
    Ip_u32 seq;
} fw_ipsec_replay_t;

typedef struct fw_ipsec_s
{
    Ip_u16 key;
    Ip_u16 mtu;
    Ip_u32 cache_id;
} fw_ipsec_t;

typedef struct fw_cache_entry_v4_s
{
    fw_mac_if_t mac_if;
    fw_ip4_t ip4;
#ifdef IPCOM_USE_FORWARDER_IPSEC
    fw_ipsec_t ipsec;
#endif

} fw_cache_entry_v4_t;

typedef struct fw_cache_entry_v6_s
{
    fw_mac_if_t mac_if;
    fw_ip6_t ip6;
#ifdef IPCOM_USE_FORWARDER_IPSEC
    fw_ipsec_t ipsec;
#endif

} fw_cache_entry_v6_t;

typedef struct fw_cache_entry_vlan_s
{
    fw_mac_if_t mac_if;
    fw_ip_vlan_t ipvlan;
#ifdef IPCOM_USE_FORWARDER_IPSEC
    fw_ipsec_t ipsec;
#endif

} fw_cache_entry_vlan_t;

typedef struct fw_port_s
{
    Ip_u32 src_type[2];
    void *fwd;
    Ip_u32 mtu;

} fw_port_t;

#ifdef IPCOM_USE_FORWARDER_UPDATE_CACHE_LOCK
typedef struct fw_cache_lock_s
{
    volatile int update_cache_lock;     /* shared */
    Ipcom_atomic_t update_cache_count;  /* shared */
} fw_cache_lock_t;

#ifdef IPCOM_FORWARDER_AMP
/* One instance of ipcom_fw per virtual board */
#define FW_LOCK (ipcom_fw.fw_lock)
#else
/* Only one instance of ipcom_fw */
#define FW_LOCK (&ipcom_fw.fw_lock)
#endif
#endif /* IPCOM_USE_FORWARDER_UPDATE_CACHE_LOCK */

typedef struct Ipcom_fw_s
{
    volatile int *rt_cache_id;
#ifdef IPCOM_USE_FORWARDER_UPDATE_CACHE_LOCK
#ifdef IPCOM_FORWARDER_AMP
    fw_cache_lock_t *fw_lock;
#else
    fw_cache_lock_t fw_lock;
#endif
#endif
    fw_cache_entry_v4_t *fw_cache_tbl_v4;
    fw_cache_entry_v6_t *fw_cache_tbl_v6;
    int fw_v4_entries;
    int fw_v6_entries;
#ifdef IPCOM_USE_FORWARDER_IPSEC
    Ipcom_fw_ipsec_sa_t *fw_cache_tbl_enc;
    Ipcom_fw_ipsec_sa_t *fw_cache_tbl_dec;
    int fw_ipsec_entries;
    Ipcom_fw_ipsec_func_t fw_ipsec_func[IPCOM_FORWARDER_IPSEC_ALG_MAX];
#endif
#ifdef IPCOM_FORWARDER_USE_UPPER_TAG_BITS
    int fw_key_shift;
#endif
#ifdef IPCOM_FORWARDER_AMP
    fw_port_t *fw_port; /* array of length MAX_FW_PORTS */
#else
    fw_port_t fw_port[MAX_FW_PORTS];
#endif

#ifndef IPCOM_FORWARDER_AMP
    Ipcom_pipe *fw_pipe;                        /* SMP only */
    Ipcom_pipe *control_pipe[MAX_FW];           /* SMP only */
#endif
    int fw_tick_cnt;
    int fw_map;           /* bitmap of cores with running forwarders */

} Ipcom_fw_t;



enum Ipcom_forward_sig
{
    IPCOM_SIG_ETH_PKT = 0x8833,
    IPCOM_SIG_STOP_FW,
    IPCOM_SIG_START_FW,
    IPCOM_SIG_ACK_FW
};

/*
 * TODO: remove or at least clean up this structure.
 */
typedef struct Ipcom_pipe_msg_s
{
    int msg_id;
    union {
            char * packet;                      /* Used for raw packet */
            Ipcom_pkt * pkt;            /* Forward an Ipcom_pkt */
    } p;
    union {
            void *nif;
            Ipcom_netif *netif;
    } n;
    unsigned int fw_key;
    void *priv;
    Ip_u32 tag_seed;
    Ip_pid_t pid;
    int nix;                   /* index of input interface */

} Ipcom_pipe_msg_t;

typedef unsigned long fw_lock_key_t;


#define IPCOM_FWD_V4 (0x1)
#define IPCOM_FWD_V6 (0x2)
#define IPCOM_FWD_V4_V6 (IPCOM_FWD_V4 | IPCOM_FWD_V6)


/*
 ****************************************************************************
 * 5                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_EXTERN int ipcom_forwarder_fw_enable;
IP_EXTERN int ipcom_forwarder_fw_started;
IP_EXTERN Ip_u32 ipcom_forwarder_seed;
IP_EXTERN Ipcom_fw_t ipcom_fw;

#ifdef IPCOM_USE_FORWARDER_IPSEC
IP_EXTERN unsigned long long ipsec_out_ok;
IP_EXTERN unsigned long long ipsec_out_skip;
IP_EXTERN unsigned long long ipsec_out_fail;
IP_EXTERN unsigned long long ipsec_in_ok;
IP_EXTERN unsigned long long ipsec_in_skip;
IP_EXTERN unsigned long long ipsec_in_fail;
IP_EXTERN unsigned long long ipsec_recrypt_ok;
IP_EXTERN unsigned long long ipsec_recrypt_fail;
#endif /* IPCOM_USE_FORWARDER_IPSEC */

/*
 ****************************************************************************
 * 6                    EXTERNAL FUNCTIONS
 ****************************************************************************
 */

#ifndef ipcom_forwarder_create_cf
IP_EXTERN int ipcom_forwarder_create_cf(void);
#endif
#ifndef ipcom_forwarder_transmit_pkt_cf
IP_EXTERN int ipcom_forwarder_transmit_pkt_cf(Ipcom_pkt *pkt, Ipcom_netif *netif);
#endif
IP_EXTERN void ipcom_forwarder_cmd_add(void);
IP_EXTERN void ipcom_forwarder_master_tick_cf(void);


/*
 ****************************************************************************
 * 7                    PUBLIC FUNCTIONS
 ****************************************************************************
 */

/* ipcom_<os/hw>_forwarder.c */

/* HW specific functions */
IP_PUBLIC fw_lock_key_t ipcom_forwarder_atomic_lock(int index);
IP_PUBLIC void   ipcom_forwarder_atomic_unlock(int index, fw_lock_key_t key);
IP_PUBLIC void   ipcom_forwarder_backend_init(void);
IP_PUBLIC void * ipcom_fw_cpu_mem_alloc(unsigned int cpu, Ip_size_t size,
                                        Ip_size_t align, unsigned int flags);
IP_PUBLIC void   ipcom_fw_cpu_mem_free(void * ptr);
IP_PUBLIC int    ipcom_forwarder_cpuid_get(int fw_id);
IP_PUBLIC void   ipcom_forwarder_drop_fw_msg(fw_msg_t *msg, void *fw_cookie);
IP_PUBLIC void   ipcom_forwarder_enter_poll(void);
IP_PUBLIC void   ipcom_forwarder_exit_poll(void);
IP_PUBLIC void   ipcom_forwarder_free_trigger_msg(fw_msg_t *msg, void *fw_cookie);
IP_PUBLIC int    ipcom_forwarder_get_fw_cnt(void);
IP_PUBLIC int    ipcom_forwarder_get_port_cnt(void);
IP_PUBLIC void   ipcom_forwarder_get_statistics(void *port_cookie, port_statistics_t *ps);
#ifdef IPCOM_USE_FORWARDER_IPSEC
IP_PUBLIC Ip_u16 ipcom_forwarder_hw_crc16(Ip_u32 *buf, Ip_u32 num);
IP_PUBLIC Ip_u32 ipcom_forwarder_hw_crc32(Ip_u32 *buf, Ip_u32 num);
#endif
IP_PUBLIC void  *ipcom_forwarder_init_rx(int fw_id);
IP_PUBLIC int    ipcom_forwarder_ipnet_msg_len(fw_msg_t *msg);
IP_PUBLIC Ip_u8 *ipcom_forwarder_ipnet_msg_packet(fw_msg_t *msg);
IP_PUBLIC Ip_u32 ipcom_forwarder_pkt_get_key(Ipcom_pkt *pkt);
IP_PUBLIC void   ipcom_forwarder_pkt_set_key(Ipcom_pkt *pkt, Ip_u32 key);
IP_PUBLIC int    ipcom_forwarder_port_name_get(void *port_cookie,
                                               char ifname[IP_IFNAMSIZ]);
IP_PUBLIC void   ipcom_forwarder_reset_statistics(void *port_cookie);
IP_PUBLIC fw_msg_t *ipcom_forwarder_rx(void *fw_cookie);
IP_PUBLIC void   ipcom_forwarder_set_tag_seed(Ip_u32 seed);
IP_PUBLIC void   ipcom_forwarder_slow_path(fw_msg_t *fw_msg, void *fw_cookie);
IP_PUBLIC void   ipcom_forwarder_trigger_pipe_read(int fw_id);
IP_PUBLIC int    ipcom_forwarder_tx(int port_index, fw_msg_t *msg, void *fw_cookie);



/* OS specific functions */
IP_PUBLIC void ipcom_forwarder_setup_hw(void);
IP_PUBLIC void *ipcom_forwarder_alloc_aligned(unsigned alignment, unsigned size);

/* ipcom_forwarder_ctrl.c */
IP_PUBLIC int ipcom_forwarder_cache_util(int v4_v6);
IP_PUBLIC int    ipcom_forwarder_create(void);
IP_PUBLIC int    ipcom_forwarder_get_fw_enable(void);
IP_PUBLIC int    ipcom_forwarder_get_fw_started(void);
IP_PUBLIC int    ipcom_forwarder_get_fwdebug_mask(void);
IP_PUBLIC void  *ipcom_forwarder_get_port_cookie(int port);
IP_PUBLIC Ip_u32 ipcom_forwarder_get_seed(void);
IP_PUBLIC unsigned int ipcom_forwarder_init(void);

#ifndef IP_PORT_LKM
IP_PUBLIC Ip_err ipcom_forwarder_ioctl(Ipcom_netif *netif, int code, struct Ip_fwreq *fwreq);
#endif
IP_PUBLIC void   ipcom_forwarder_invalidate(int id, int v4_v6);
IP_PUBLIC void   ipcom_forwarder_port_mac_update(int port_index, Ip_u8 *hwaddr);
IP_PUBLIC void   ipcom_forwarder_port_mtu_update(int port, Ip_u32 mtu);
IP_PUBLIC void   ipcom_forwarder_print_cache(int v);
IP_PUBLIC void   ipcom_forwarder_register_port(int port, void *port_cookie,
                                               Ip_u32 mtu, const Ip_u8 *hwaddr);
IP_PUBLIC void   ipcom_forwarder_route_cache_invalidate(void);
IP_PUBLIC void   ipcom_forwarder_set_port_cookie(int port, void *port_cookie);
IP_PUBLIC void   ipcom_forwarder_set_seed(Ip_u32 seed);
IP_PUBLIC void   ipcom_forwarder_start_stop_fw(int sig);
IP_PUBLIC int    ipcom_forwarder_transmit_pkt(Ipcom_pkt *pkt, Ipcom_netif *netif);
IP_PUBLIC int    ipcom_forwarder_transmit_pkt_ext(Ipcom_pkt *pkt, int port_index);
#ifdef IPCOM_USE_FORWARDER_UPDATE_CACHE_LOCK
IP_PUBLIC void   ipcom_forwarder_update_cache_lock(void);
IP_PUBLIC void   ipcom_forwarder_update_cache_unlock(void);
#endif

/* ipcom_forwarder_data.c */
IP_PUBLIC void   ipcom_forwarder_loop(void * fw_cookie, void * pipe);


/* ipcom_config.c */
IP_PUBLIC int    ipcom_forwarder_config_cache_size(void);
IP_PUBLIC int    ipcom_forwarder_config_fw_max(void);
IP_PUBLIC unsigned int ipcom_forwarder_current_core(void);
IP_PUBLIC int    ipcom_forwarder_fw_per_port(void);
IP_PUBLIC int    ipcom_forwarder_get_key_shift(void);
IP_PUBLIC void   ipcom_forwarder_get_nonfw_stats(void *end, Ip_u32 *frames, Ip_u32 *bytes, int reset);
IP_PUBLIC Ip_u32 ipcom_forwarder_get_ports_for_interface (Ip_u32 iface);
IP_PUBLIC int    ipcom_forwarder_is_master(void);
IP_PUBLIC unsigned int ipcom_forwarder_master_core(void);


/* port/<os>/src/ipcom_os.c */

/* ipcom_forwarder_ipsec.c */
IP_PUBLIC int    ipcom_forwarder_ipsec_check_replay_window(Ipcom_fw_ipsec_sa_t *fw_ipsec_sa, Ip_u32 seq);
IP_PUBLIC int    ipcom_forwarder_ipsec_input(Ip_u8 *domain, fw_ipsec_replay_t *fw_replay,
                                             Ip_u8 *p, unsigned int length,
                                             unsigned int *headers, unsigned int *trailers);
IP_PUBLIC int    ipcom_forwarder_ipsec_output(fw_ipsec_t *ipsec,
                                              Ip_u8 *p, unsigned int length,
                                              unsigned int *headers, unsigned int *trailers);
IP_PUBLIC void   ipcom_forwarder_ipsec_populate_func_table(void);
IP_PUBLIC int    ipcom_forwarder_ipsec_recrypt(Ip_u8 *p, unsigned int length);
IP_PUBLIC void   ipcom_forwarder_ipsec_sprint_sa(Ip_bool in, char *buf, Ipcom_fw_ipsec_sa_t *fw_ipsec_sa);
IP_PUBLIC Ip_u8  ipcom_forwarder_ipsec_set_fw_if(Ip_u8 *p, Ipcom_pkt *pkt,
                                                 Ip_u32 mtu, fw_ipsec_t *fw_ipsec);


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
