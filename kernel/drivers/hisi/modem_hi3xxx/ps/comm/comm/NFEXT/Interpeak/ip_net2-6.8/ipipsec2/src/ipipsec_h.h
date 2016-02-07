/*
 ******************************************************************************
 *                     INTERPEAK INTERNAL API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipipsec_h.h,v $ $Revision: 1.51.14.1 $
 *   $Source: /home/interpeak/CVSRoot/ipipsec2/src/ipipsec_h.h,v $
 *   $Author: ulf $ $Date: 2010-07-02 13:21:09 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPIPSEC_H_H
#define IPIPSEC_H_H

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

#include "ipipsec_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipipsec.h"
#include "ipipsec_pfkeyv2.h"

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_netif.h>
#include <ipcom_inet.h>
#include <ipcom_list.h>
#include <ipcom_os.h>
#include <ipcom_hash.h>
#include <ipcom_pipe.h>

#ifdef IPIPSEC_USE_BITARRAY
#include <ipcom_bitarray.h>
#endif

/* openssl crypto */
#ifdef IPIPSEC_USE_MD5
#ifdef IPMCRYPTO
#include <ip/openssl/md5.h>
#else
#include <openssl/md5.h>
#endif
#endif

#ifdef IPIPSEC_USE_SHA
#ifdef IPMCRYPTO
#include <ip/openssl/sha.h>
#else
#include <openssl/sha.h>
#endif
#endif

#ifdef IPIPSEC_USE_RIPEMD
#ifdef IPMCRYPTO
#include <ip/openssl/ripemd.h>
#else
#include <openssl/ripemd.h>
#endif
#endif

#if defined(IPIPSEC_USE_DES) || defined(IPIPSEC_USE_3DES)
#ifdef IPMCRYPTO
#include <ip/openssl/des.h>
#else
#include <openssl/des.h>
#endif
#endif

#ifdef IPIPSEC_USE_BLOWFISH
#ifdef IPMCRYPTO
#include <ip/openssl/blowfish.h>
#else
#include <openssl/blowfish.h>
#endif
#endif

#ifdef IPIPSEC_USE_CAST
#ifdef IPMCRYPTO
#include <ip/openssl/cast.h>
#else
#include <openssl/cast.h>
#endif
#endif

#if defined(IPIPSEC_USE_AES) || defined(IPIPSEC_USE_AESMAC)
#ifdef IPMCRYPTO
#include <ip/openssl/aes.h>
#else
#include <openssl/aes.h>
#endif
#endif

#ifdef IPNET
#include <ipnet_h.h>
#include <ipnet_signal.h>
#include <ipnet_usr_sock.h>
#undef IPCOM_SYSLOG_PRIORITY
#undef IPCOM_SYSLOG_FACILITY
#endif

#ifdef IPHWCRYPTO
#include <iphwcrypto.h>
#endif

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
 *                         HMAC
 *===========================================================================
 */
#define HMAC_IPAD           0x36
#define HMAC_OPAD           0x5C


/*
 *===========================================================================
 *                         misc
 *===========================================================================
 */

#define IPSECPROTONAME(xprot)    ((xprot) == 0               ? "NONE" : \
                                  (xprot) == IP_IPPROTO_AH   ? "AH  " : \
                                  (xprot) == IP_IPPROTO_ESP  ? "ESP " : \
                                              (xprot) == IP_IPPROTO_IPIP ? "IPIP" : \
                                              (xprot) == IP_IPPROTO_IPV6 ? "IPV6" : \
                                  "????")

#define IPSECSATYPENAME(xtype)   ((xtype) == SADB_SATYPE_UNSPEC   ? "NONE" : \
                                  (xtype) == SADB_X_SATYPE_BYPASS ? "BYPS" : \
                                  (xtype) == SADB_SATYPE_AH       ? "AH  " : \
                                  (xtype) == SADB_SATYPE_ESP      ? "ESP " : \
                                              (xtype) == SADB_X_SATYPE_IPIP   ? "IPIP" : "????")

#define IPSECFLOWTYPENAME(xtype) ((xtype) == SADB_X_FLOW_TYPE_USE ? "use" : \
                                  (xtype) == SADB_X_FLOW_TYPE_ACQUIRE ? "acquire" : \
                                  (xtype) == SADB_X_FLOW_TYPE_REQUIRE ? "require" : \
                                  (xtype) == SADB_X_FLOW_TYPE_BYPASS ? "bypass" : \
                                  (xtype) == SADB_X_FLOW_TYPE_DENY ? "deny" : \
                                              "????")

#define IPIPSEC_SADB_GET_PROTO(s) ((Ip_u8)( \
                                   (s) == SADB_SATYPE_AH       ? IP_IPPROTO_AH : \
                                   (s) == SADB_SATYPE_ESP      ? IP_IPPROTO_ESP : \
                                   (s) == SADB_X_SATYPE_BYPASS ? 0 : \
                                                                 IP_IPPROTO_IPIP ))

/* SA/flow directions */
#define IPIPSEC_DIR_IN     0
#define IPIPSEC_DIR_OUT    1
#define IPIPSEC_DIR_ANY    2

/* Array index values: */
#define IPIPSEC_INET   0
#define IPIPSEC_INET6  1

  /* expire types */
#define IPIPSEC_EXPIRE_TYPE_HARD    0
#define IPIPSEC_EXPIRE_TYPE_SOFT    1


/*
 *===========================================================================
 *                         IPIPSEC CODE/DATA LOCKS
 *===========================================================================
 */
#ifdef IPIPSEC_USE_DAEMONS
# define IPIPSEC_DATA_LOCK(sl)            ipcom_spinlock_lock(sl)
# define IPIPSEC_DATA_UNLOCK(sl)          ipcom_spinlock_unlock(sl)
#else
# define IPIPSEC_DATA_LOCK(x)             IP_NOOP
# define IPIPSEC_DATA_UNLOCK(x)           IP_NOOP
#endif


#define IPIPSEC_LIST_ENTRY(node, type, member)   ((node) ? ipcom_list_entry(node, type, member) : 0)


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         AUTHENTICATION
 *===========================================================================
 */

#define IPIPSEC_AUTH_MAXKEYLEN         20    /* max key/digest length */
#define IPIPSEC_HMAC_AUTHLEN           12    /* 96 bits authenticator */

#ifndef AES_BLOCK_SIZE
#define AES_BLOCK_SIZE 16
#endif

#ifdef IPIPSEC_USE_AESMAC
typedef struct AESMAC_CTX_struct
{
    Ip_u8  key[AES_BLOCK_SIZE];
    Ip_u8  k1[AES_BLOCK_SIZE];
    Ip_u8  k2[AES_BLOCK_SIZE];
    Ip_u8  k3[AES_BLOCK_SIZE];
    Ip_u8  e[AES_BLOCK_SIZE];
    Ip_u8  msg[AES_BLOCK_SIZE];
    Ip_u32 msgLen;
}
AESMAC_CTX;
#endif

/* Ipipsec_auth_ctx */
typedef union Ipipsec_auth_ctx_union
{
#ifdef IPIPSEC_USE_MD5
    MD5_CTX         md5;
#endif
#ifdef IPIPSEC_USE_SHA
    SHA_CTX         sha1;
#endif
#ifdef IPIPSEC_USE_RIPEMD
    RIPEMD160_CTX   rmd160;
#endif
#ifdef IPIPSEC_USE_AESMAC
    AESMAC_CTX      aes;
#endif
}
Ipipsec_auth_ctx;

/* Ipipsec_auth_method */
typedef int  (*Hash_init)  (Ipipsec_auth_ctx *ctx);
typedef void (*Hash_update)(Ipipsec_auth_ctx *ctx, IP_CONST Ip_u8 *data, Ip_u32 length);
typedef void (*Hash_final) (Ip_u8 *digest, Ipipsec_auth_ctx *ctx);

typedef struct Ipipsec_auth_method
{
    Ip_u16         minkey_bytes;
    Ip_u16         maxkey_bytes;
    Ip_u16         hash_bytes;     /* hash size */
    Ip_u16         auth_bytes;     /* number of bytes in AH header */

    Hash_init      init;
    Hash_update    update;
    Hash_final     final;
}
Ipipsec_auth_method;


/* Ipipsec_sa_auth */
typedef struct Ipipsec_sa_auth_struct
{
    IP_CONST Ipipsec_auth_method   *alg;
    Ip_u8                *key;     /* For OLD AH */
    Ip_u32                keyLen;  /* key-length in bytes */

    Ipipsec_auth_ctx   i_ctx;
    Ipipsec_auth_ctx   o_ctx;
}
Ipipsec_sa_auth;


/*
 *===========================================================================
 *                         ENCRYPTION
 *===========================================================================
 */

/* Maximum IV size */
#define IPIPSEC_MAX_IV_SIZE        16

/* specific encryption defines: */
#ifndef BF_ROUNDS
#define BF_ROUNDS       16
#endif
#define IPIPSEC_BLF_MAXKEYLEN ((BF_ROUNDS-2)*4)     /* 448 bits (56 bytes) */

/* Ipipsec_encrypt_key */
typedef union Ipipsec_encrypt_key_union
{
#ifdef IPIPSEC_USE_BLOWFISH
    BF_KEY                    bf;
#endif

#ifdef IPIPSEC_USE_DES
    des_key_schedule          des;
#endif

#ifdef IPIPSEC_USE_3DES
    struct des3_struct
    {
        des_key_schedule key1;
        des_key_schedule key2;
        des_key_schedule key3;
    }                         des3;
#endif

#ifdef IPIPSEC_USE_CAST
    CAST_KEY                  cast;
#endif

#if defined(IPIPSEC_USE_AES) || defined(IPIPSEC_USE_AESMAC)
    AES_KEY                   aes[2];  /* 0 encrypt key, 1 decrypt key */
#endif

#ifdef IPIPSEC_USE_AESCTR
    struct aesctr_struct
    {
        AES_KEY         key;
        unsigned char   nonce[4];
        unsigned char   ecount_buf[AES_BLOCK_SIZE];
        unsigned int    num;
    }                         aesctr;
#endif

#if !defined(IPIPSEC_USE_CAST) && !defined(IPIPSEC_USE_BLOWFISH) \
    && !defined(IPIPSEC_USE_3DES) && !defined(IPIPSEC_USE_DES) \
    && !defined(IPIPSEC_USE_AES)
    int     dummy;
#endif
}
Ipipsec_encrypt_key;

/* Ipipsec_encrypt_method */
typedef struct Ipipsec_encrypt_method
{
    int       key_size;
    Ip_u16    minkey_bytes;
    Ip_u16    maxkey_bytes;
    Ip_u16    block_bytes;
    Ip_u16    iv_bytes;

    void (*encrypt)(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv);
    void (*decrypt)(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv);
    Ip_s32  (*setkey) (Ip_u8 *secret, Ip_u32 length, Ipipsec_encrypt_key *key);
}
Ipipsec_encrypt_method;

/* Ipipsec_sa_encrypt */
typedef struct Ipipsec_sa_encrypt_struct
{
    IP_CONST Ipipsec_encrypt_method   *alg;

    Ip_u32                    bytes;   /* key-length in bytes */
    Ip_u8                     iv[IPIPSEC_MAX_IV_SIZE];
    Ip_u16                    iv_length;
    Ip_u16                    pad;
    Ip_u32                    pad1;    /* CAVIUM SEEMS TO REQUIRE 8byte ALIGNMENT FOR
                                          THE BLOWFISH KEY, OR IT'LL FAIL INTERMITTENTLY */
    Ipipsec_encrypt_key       key;
}
Ipipsec_sa_encrypt;


/*
 *===========================================================================
 *                  Ipipsec_sa_lifetime
 *===========================================================================
 * RFC4301s4.4.2.1 "A compliant implementation MUST support both types of lifetimes, and MUST support a simultaneous use of both." (time or byte count)
 */
typedef struct Ipipsec_sa_lifetime_struct
{
    /* NOTE: entries MUST be in same order as sadb_lifetime!*/
    Ip_u32          allocations;
    Ip_u64          bytes;
    Ip_u32          addtime;   /* current=time added, else seconds to expire */
    Ip_u32          usetime;   /* current=time first used, else seconds to expire */
}
Ipipsec_sa_lifetime;


/*
 *===========================================================================
 *                         Ipipsec_sa
 *===========================================================================
 */
struct Ipipsec_flow_struct; /* forward dummy */

typedef struct Ipipsec_sa_struct
{
    Ipcom_list      sa_list;          /* List entry for global lists */
    Ipcom_list      flow_head[2];     /* INPUT-filter/OUTPUT-route flow list heads */

    Ipipsec_key     key;              /* Destination, source, SPI, domain, protocol, direction */

    void           *tmo;              /* Timeout handle, != IP_NULL when a timeout is active */
    Ip_time_t       larval_timeout;   /* 0 not active, else absolute timeout */

    union Ip_in_addr_union   proxy;   /* Proxy address */

    /* SADB values */
    Ip_u32          sadb_sa_flags;       /* SADB_SAFLAGS and SADB_X_SAFLAGS */
    Ip_u8           sadb_sa_auth;
    Ip_u8           sadb_sa_encrypt;
    Ip_u8           sadb_sa_replay;      /* Between 0 and 255 (0 = none) */
    Ip_u8           sadb_sa_state;       /* larval,mature,dying,dead */
    Ip_u8           sadb_msg_satype;     /* Same as SA protocol! */

    /* Cached internal data */
    Ip_u8           iflags;              /* Internal: Not to confuse with sadb_sa_flags */
    Ip_u16          in_addr_size;        /* Size of IP addresses (4 or 16) */
    Ip_u16          sock_in_addr_offset; /* in_addr offset in sockaddr (2 or 6) */
    Ip_s16          header_size;         /* AH or ESP header size */
    Ip_u16          header_space;
    Ip_u16          trailer_space;

    /* Output */
    struct Ipipsec_sa_struct   *group_head;     /* 0 if not grouped, else group head */
    struct Ipipsec_sa_struct   *group_next;     /* Next SA in a group */

    /* Encryption/Authentication data */
    Ipipsec_sa_encrypt   *encrypt;
    Ipipsec_sa_auth      *auth;

    /* Hardware crypto support (!!todo: move to encrypt space to save AH memory) */
#ifdef IPIPSEC_USE_ESP
#if defined(IPHWCRYPTO) || defined(IPCOM_USE_FORWARDER_IPSEC)
    Ip_u8 esp_authkey[20];
    Ip_u8 esp_enckey[32];
#endif
#ifdef IPHWCRYPTO
    int (*esp_input)(Ip_u8 *data, Ip_u32 length, struct Ipipsec_sa_struct *sa, Ipipsec_param *param, Ip_u8 *digest, Ipcom_pkt *pkt);
    int (*esp_output)(Ip_u8 *data, Ip_u32 length, struct Ipipsec_sa_struct *sa, Ipipsec_param *param, Ip_u8 *digest, Ipcom_pkt *pkt);
#endif
#endif /* IPIPSEC_USE_ESP */

    /* Replay protection
     * Seql is stored in old 32 bit field seq, old replay_counter is used
     * for heuristic checks and replay_bitmap stores the bitarray size if
     * bitarray is used
     */
    Ip_u32                 seql;        /* (Low) sequence number */
    Ip_u32                 replay_counter;
    Ip_u32                 replay_bitmap;
#ifdef IPIPSEC_USE_ESN
    Ip_u32                 replay_Tl;   /* Seql validated so far */
    Ip_u32                 replay_Th;   /* Seqh validated so far */
    Ip_u32                 seqh;        /* High sequence number */
    Ip_u32                 replay_failures;
#ifdef IPIPSEC_USE_BITARRAY
    Ipcom_bitarray        *replay_bitarray;
#endif
#endif /* IPIPSEC_USE_ESN */

    /* lifetimes, counters and times */
    Ip_u32                 time_first_used;
    Ip_u64                 bytes[2];    /* 0=input,1=output */
    Ip_u64                 packets[2];  /* 0=input,1=output */
    Ipipsec_sa_lifetime    ltcurrent;
    Ipipsec_sa_lifetime    lthard;
    Ipipsec_sa_lifetime    ltsoft;

    /* NAT Traversal (UDP Encap ports) */
    Ip_u16                 encap_srcport_n;
    Ip_u16                 encap_dstport_n;

#ifdef IPIPSEC_USE_TFC
    void                  *tfc_tmo; /* TFC timeout handle */
#endif

#ifdef IPIPSEC_USE_TRAPS
    Ip_u32          sent_trap[IPIPSEC_TRAP_NUM];
#endif

    /* SA MIB counters */
#if defined(IPIPSEC_USE_MIB) || defined(IPIPSEC_USE_TRAPS)
    Ip_u32          err_in_decrypt;
    Ip_u32          err_in_auth;
    Ip_u32          err_in_replay;
    Ip_u32          err_in_policy;
    Ip_u32          err_in_pad;
    Ip_u32          err_in_other;
    Ip_u32          err_out_send;
#endif /* defined(IPIPSEC_USE_MIB) || defined(IPIPSEC_USE_TRAPS) */
}
Ipipsec_sa;


/* 8-bit internal flags: */
#define IPIPSEC_IFLAGS_IN_SALIST    0x1


/*
 *===========================================================================
 *                         Ipipsec_hash
 *===========================================================================
 */
typedef struct Ipipsec_hash_struct
{
    Ipipsec_key    key;
    Ipcom_list     hash_list;   /* List entry for HASH timeout */

    void          *entry;       /* Flow or SA pointer */
}
Ipipsec_hash;


/*
 *===========================================================================
 *                         Ipipsec_hash_head
 *===========================================================================
 */
typedef struct Ipipsec_hash_head_struct
{
    Ipcom_hash    *hash;       /* Hash table */
    Ipcom_list     list;       /* List entry head for HASH timeouts */
    int            size;       /* Current size of hash table */
    int            maxsize;    /* Maximum size of hash table */
}
Ipipsec_hash_head;


/*
 *===========================================================================
 *                         Ipipsec_flowsel
 *===========================================================================
 * RFC4301s4.4.1 p26-28: "The following selector parameters MUST be supported by all IPsec implementations to facilitate control of SA granularity":
 * - Remote IP Address(es) (IPv4 or IPv6)
 * - Local IP Address(es) (IPv4 or IPv6)
 * - Next Layer Protocol
 *   - If the Next Layer Protocol uses two ports (as do TCP, UDP, SCTP, and others), then there are selectors for Local and Remote Ports.
 *   - If the Next Layer Protocol is a Mobility Header, then there is a selector for IPv6 Mobility Header message type (MH type)
 *   - If the Next Layer Protocol value is ICMP, then there is a 16-bit selector for the ICMP message type and code.
 */
typedef struct Ipipsec_flowsel_struct
{
    Ipipsec_key   key;

    struct flowsel_mask
    {
        union Ip_in_addr_union   src;     /* Source mask in network endian. */
        union Ip_in_addr_union   dst;     /* Destination mask in network endian. */
        Ip_u32                   srcscope_n;
        Ip_u32                   dstscope_n;
    }         mask;

    /* Port and address range */
    union Ip_in_addr_union   src_max;
    union Ip_in_addr_union   dst_max;
    Ip_u16    src_max_port;
    Ip_u16    dst_max_port;
}
Ipipsec_flowsel;


/*
 *===========================================================================
 *                         Ipipsec_flow
 *===========================================================================
 */
typedef struct Ipipsec_flow_struct
{
    Ipcom_list         flow_list;   /* Global flow list next pointer */
    Ipcom_list         sa_list;     /* SA flow list next pointer */

    Ipipsec_sa        *sa;
    Ip_u32             id;

    Ip_u8              flowtype;    /* SADB_X_FLOW_TYPE_xx */
    Ip_u8              magic;       /* IPIPSEC_FLOW_MAGIC */
    Ip_u8              iflags;      /* Internal flags */
    Ip_u8              pad;

    /* Flow selectors */
    Ipipsec_flowsel    flowsel;
}
Ipipsec_flow;

#define IPIPSEC_FLOW_MAGIC  0xab


/*
 *===========================================================================
 *                         Statistics
 *===========================================================================
 */
typedef struct Ipipsec_stats_struct
{
    Ip_u32  dummy;

#ifdef IPIPSEC_USE_MIB
    Ip_u32 ipsecEspCurrentInboundSAs;
    Ip_u32 ipsecEspTotalInboundSAs;
    Ip_u32 ipsecEspCurrentOutboundSAs;
    Ip_u32 ipsecEspTotalOutboundSAs;
    Ip_u32 ipsecAhCurrentInboundSAs;
    Ip_u32 ipsecAhTotalInboundSAs;
    Ip_u32 ipsecAhCurrentOutboundSAs;
    Ip_u32 ipsecAhTotalOutboundSAs;

    Ip_u32 ipsecDecryptionErrors;
    Ip_u32 ipsecAuthenticationErrors;
    Ip_u32 ipsecReplayErrors;
    Ip_u32 ipsecPolicyErrors;
    Ip_u32 ipsecOtherReceiveErrors;
    Ip_u32 ipsecSendErrors;
    Ip_u32 ipsecUnknownSpiErrors;
#endif /* IPIPSEC_USE_MIB */

#ifdef IPIPSEC_USE_STATS
    Ip_s32 sa_match;
    Ip_s32 sa_miss;
    Ip_s32 sa_list_match;
    Ip_s32 sa_hash_match;
    Ip_s32 sa_hash_add;
    Ip_s32 sa_hash_full;
    Ip_s32 sa_infilter_match;
    Ip_s32 sa_infilter_miss;
    Ip_s32 sa_infilter_drop;
    Ip_s32 sa_outflow_match;
    Ip_s32 sa_outflow_miss;
    Ip_s32 sa_outflow_drop;
    Ip_s32 sa_outflow_require;
    Ip_s32 sa_outflow_acquire;
    Ip_s32 sa_outflow_bypass;
    Ip_s32 sa_outflow_deny;
    Ip_s32 sa_inreplay_inorder;
    Ip_s32 sa_inreplay_outoforder;
    Ip_s32 sa_inreplay_wrap;
    Ip_s32 sa_inreplay_seen;
    Ip_s32 sa_inreplay_zeroseq;
    Ip_s32 flow_list_match;
    Ip_s32 flow_hash_match;
    Ip_s32 flow_hash_add;
    Ip_s32 flow_hash_full;
    Ip_s32 output_socket_none;
    Ip_s32 output_socket_have;
    Ip_s32 output_socket_cache_flow;
    Ip_s32 output_socket_flow_cache_match;
#ifdef IPIPSEC_USE_TFC
    Ip_s32 sa_dummy_generated;
    Ip_s32 sa_dummy_send_errors;
#endif
#endif /* IPIPSEC_USE_STATS */
}
Ipipsec_stats;

#ifdef IPIPSEC_USE_MIB
#define IPSEC_MIB(xxmib)      xxmib
#else
#define IPSEC_MIB(xxmib)      IP_NOOP
#endif

#ifdef IPIPSEC_USE_STATS
#define IPSEC_STATS(xxstats)  xxstats
#else
#define IPSEC_STATS(xxstats)  IP_NOOP
#endif


/*
 *===========================================================================
 *                         Ipipsec_data
 *===========================================================================
 */
typedef struct Ipipsec_data_struct
{
    Ip_u32         policy_id;         /* Current policy id (used for both SA and flow) */

#ifdef IPIPSEC_USE_DAEMONS
    Ipcom_spinlock         spl;      /* Lock for SMP */
    int                    numd;     /* Number of ipsec daemons */
    Ipnet_timeout         *atmo;     /* Affinity timeout handle */
    int                    nextpipe; /* Index to next pipe */
    Ipcom_atomic_t         mypipe;   /* Index to daemon pipe */
    Ipcom_pipe           **pipes;    /* Pointer to array of pipes */
    Ip_pid_t              *pids;     /* Pointer to array of ipsec daemon pids */
    int                   *cpus;     /* Pointer to array of ipsec daemon cpus */
    Ipcom_pipe            *ctrlpipe; /* Control pipe for ipsec daemon */
    Ip_u16                 seq;
    Ip_u16                 lastseq;
    Ip_bool                pktq_flush;
    Ipcom_pqueue          *pktq;
#endif /* #if IPIPSEC_USE_DAEMONS */

    /* Security associations */
    Ipipsec_sa     sa_bypass;       /* INPUT/OUTPUT bypass SA */
    Ipcom_list     sa_head[2][2];   /* [domain,0=inet,1=inet6][dir,0=in,1=out]  */
    Ipipsec_hash_head  sa_hash;

    /* RFC4301 section 4.4.1p20: "An IPsec implementation MUST have at least one SPD"
     * Note: The SPD consist of flows. A policy entry may consist of multiple flows
     */
    Ip_u32         flow_id;           /* Flow id */
    Ipcom_list     flow_head[2][2];   /* [domain,0=inet,1=inet6][dir,0=in,1=out]  */
    Ipipsec_hash_head  flow_hash;

    /* Crypt */
    Ip_u8          zeroes[64];
    Ip_u8          o_buffer[64];    /* 64 '0x5C' */
    Ip_u8          i_buffer[64];    /* 64 '0x36' */

    /* PFKEY sockets */
    Ip_u32         registered_num;
    Ip_u32         pfkeyv2_seq;

#ifdef IPIPSEC_USE_AH
    Ip_u8 calc_digest[IPIPSEC_AUTH_MAXKEYLEN];
#endif

#ifdef IPIPSEC_USE_TRAPS
    Ipipsec_trap_func      trap_cb;
    Ip_u32         enable_trap[IPIPSEC_TRAP_NUM];
    Ip_tag         trap_tags[11];  /* Max 5 tags */
#endif

    Ipipsec_stats  stats;

    char  sysbuf[256];      /* temporary storage for debug output */
}
Ipipsec_data;


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         MISC
 *===========================================================================
 *
 */
IP_GLOBAL char *ipipsec_key_name(Ipcom_pkt *pkt, Ipipsec_param *param);
IP_GLOBAL void ipipsec_output_add_ip_header(Ipcom_pkt *pkt, Ipipsec_param *param);
IP_GLOBAL void ipipsec_create_consecutive_pkt(Ipcom_pkt *pkt, Ipipsec_param *param);
IP_GLOBAL int ipipsec_lifetime_bytes(Ipipsec_sa *sa, Ipcom_pkt *pkt, Ip_u32 dir, Ip_u32 bytes);
IP_GLOBAL int ipipsec_inet6_header_length(Ipipsec_param *param, Ip_bool first);
IP_GLOBAL int ipipsec_check_replay_window(Ipipsec_sa *sa, Ip_u32 seq);
IP_GLOBAL Ip_err ipipsec_update_replay_window(Ipipsec_sa *sa, Ip_u32 seq);

IP_GLOBAL int ipipsec_check_replay_window_esn(Ipipsec_sa *sa, Ip_u32 seq, Ip_u32 *diff,Ip_bool *incTh);
IP_GLOBAL Ip_err ipipsec_update_replay_window_esn(Ipipsec_sa *sa, Ip_u32 seq, Ip_u32 diff,Ip_bool incTh);

IP_GLOBAL void ipipsec_tfc_reset_timeout(Ipipsec_sa *sa);

IP_GLOBAL void ipipsec_calc_digest(Ipipsec_sa *sa, Ipcom_pkt *pkt, Ip_u32 esn, Ip_u8 *digest );
IP_GLOBAL int  ipipsec_addrcmp(int domain, union Ip_in_addr_union *addr1, union Ip_in_addr_union *addr2);

IP_GLOBAL int ipipsec_key_update(Ipcom_pkt *pkt, Ipipsec_param *param);


/*
 *===========================================================================
 *                         SA
 *===========================================================================
 *
 */
IP_GLOBAL char *ipipsec_sa_name(Ipipsec_sa *sa);
IP_GLOBAL Ipipsec_sa *ipipsec_sa_group(Ipipsec_sa *sa1, Ipipsec_sa *sa2);
IP_GLOBAL Ipipsec_sa *ipipsec_sa_malloc(Ip_u32 spi_n, int sadir, int domain);
IP_GLOBAL void        ipipsec_sa_free(Ipipsec_sa *sa, Ip_bool remove_group, Ip_bool addlog);
IP_GLOBAL void        ipipsec_sa_insert(Ipipsec_sa *this);

IP_GLOBAL void ipipsec_sa_reset_timeout(Ipipsec_sa  *sa);


/*
 *===========================================================================
 *                         LOOKUP
 *===========================================================================
 *
 */
IP_GLOBAL Ipipsec_sa *ipipsec_sa_lookup(Ipipsec_key *key, Ip_u16 nocheck);
IP_GLOBAL Ipipsec_sa *ipipsec_sa_lookup_exact(Ipipsec_key *key);

IP_GLOBAL Ipipsec_flow *ipipsec_flow_lookup(Ipipsec_key *key, Ipipsec_sa *sa, Ip_u16 nocheck);
IP_GLOBAL Ipipsec_flow *ipipsec_flow_lookup_exact(Ipipsec_flowsel *flowsel, Ipipsec_sa *sa, Ip_u32 flowid);
IP_GLOBAL int ipipsec_flow_lookup_input(Ipipsec_sa *sa, Ipcom_pkt *pkt, Ipipsec_param *param);

/* Lookup flags used for e.g. input policy filter: */
#define IPIPSEC_LFLAGS_USEHASH             0x0001
#define IPIPSEC_LFLAGS_NOCHECK_PORTS       0x0010 /* flow lookup only */
#define IPIPSEC_LFLAGS_NOCHECK_SRC         0x0020 /* flow lookup only */
#define IPIPSEC_LFLAGS_NOCHECK_PROTO       0x0040 /* flow lookup only */


/*
 *===========================================================================
 *                         FLOW
 *===========================================================================
 *
 */
IP_GLOBAL char *ipipsec_flow_name(Ipipsec_flow *flow, Ipipsec_sa *sa);
IP_GLOBAL Ipipsec_flow *ipipsec_flow_malloc(Ipipsec_flowsel *flowsel, Ipipsec_sa *sa, Ip_u8 flowtype);
IP_GLOBAL void ipipsec_flow_free(Ipipsec_flow *flow);


/*
 *===========================================================================
 *                         AUTH
 *===========================================================================
 */
IP_GLOBAL Ip_s32 ipipsec_auth_setkey(Ipipsec_sa *sa, Ip_u8 aalg, Ip_u8 *key, Ip_u32 bytes);
IP_GLOBAL Ip_s32 ipipsec_encrypt_setkey(Ipipsec_sa *sa, Ip_u8 ealg, Ip_u8 *key, Ip_u32 bytes);
IP_GLOBAL void   ipipsec_crypt_get_algs(struct sadb_supported *supported);


/*
 *===========================================================================
 *                         PFKEYv2
 *===========================================================================
 */
IP_GLOBAL void ipipsec_pfkeyv2_expire(Ipipsec_sa *sa, Ip_u32 type);
IP_GLOBAL void ipipsec_pfkeyv2_acquire(Ipipsec_key *key);


/*
 *===========================================================================
 *                         CTRL
 *===========================================================================
 */
IP_GLOBAL Ip_err ipipsec_conf_internal(Ip_tag *ctrl_tags);
IP_GLOBAL Ip_err ipipsec_ctrl_internal(Ip_tag *ctrl_tags);
IP_GLOBAL Ip_err ipipsec_sactrl_internal(Ipipsec_sa_handle *sah, Ip_tag *ctrl_tags);


/*
 *===========================================================================
 *                         CRYPT RAND
 *===========================================================================
 */
IP_GLOBAL void ipipsec_crypt_random_fill(Ip_u8 *buf, int length);


/*
 *===========================================================================
 *                         SMP
 *===========================================================================
 */
IP_GLOBAL void       ipipsec_process_signal(Ipnet_sig *sig);
IP_GLOBAL void       ipipsec_poll_control_pipe(void);
IP_GLOBAL void       ipipsec_resume_daemons(Ipnet_sig *sig);
IP_GLOBAL Ipnet_sig *ipipsec_suspend_daemons(void);

IP_PUBLIC void       ipipsec_ioevent_eioxflushnetif(void);
IP_PUBLIC int        ipipsec_ioevent_sock_key_close(Ipcom_socket *sock);


#ifdef __cplusplus
}
#endif

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
