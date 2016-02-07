/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipipsec_pfkeyv2.h,v $ $Revision: 1.23 $
 *   $Source: /home/interpeak/CVSRoot/ipipsec2/include/ipipsec_pfkeyv2.h,v $
 *   $Author: lob $ $Date: 2009-04-23 09:00:23 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef __PFKEY_V2_H
#define __PFKEY_V2_H 1

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * This file defines structures and symbols for the PF_KEY Version 2
 * key management interface. It was written at the U.S. Naval Research
 * Laboratory. This file is in the public domain. The authors ask that
 * you leave this credit intact on any copies of this file.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipipsec_config.h"


/*
 ****************************************************************************
 * 3                    REVISION
 ***************************************************************************
 */
#define PF_KEY_V2            2
#define PFKEYV2_REVISION     200901L


/*
 ****************************************************************************
 * 4                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 ****************************************************************************
 * 5                    DEFINES
 ****************************************************************************
 */

#define SADB_SATYPE_UNSPEC           0
#define SADB_SATYPE_AH               2
#define SADB_SATYPE_ESP              3
#define SADB_SATYPE_RSVP             5
#define SADB_SATYPE_OSPFV2           6
#define SADB_SATYPE_RIPV2            7
#define SADB_SATYPE_MIP              8
#define SADB_X_SATYPE_IPIP           9
#define SADB_X_SATYPE_TCPSIGNATURE  10
#define SADB_X_SATYPE_BYPASS        11
#define SADB_SATYPE_MAX             11


/*
 *===========================================================================
 *                         SADB_EXT_
 *===========================================================================
 */
#define SADB_EXT_RESERVED             0    /* _Extension_data_type_ */
#define SADB_EXT_SA                   1    /* struct sadb_sa */
#define SADB_EXT_LIFETIME_CURRENT     2    /* struct sadb_lifetime */
#define SADB_EXT_LIFETIME_HARD        3    /* struct sadb_lifetime */
#define SADB_EXT_LIFETIME_SOFT        4    /* struct sadb_lifetime */
#define SADB_EXT_ADDRESS_SRC          5    /* struct sadb_address */
#define SADB_EXT_ADDRESS_DST          6    /* struct sadb_address */
#define SADB_EXT_ADDRESS_PROXY        7    /* struct sadb_address */
#define SADB_EXT_KEY_AUTH             8    /* struct sadb_key */
#define SADB_EXT_KEY_ENCRYPT          9    /* struct sadb_key */
#define SADB_EXT_IDENTITY_SRC         10   /* struct sadb_ident */
#define SADB_EXT_IDENTITY_DST         11   /* struct sadb_ident */
#define SADB_EXT_SENSITIVITY          12   /* struct sadb_sens */
#define SADB_EXT_PROPOSAL             13   /* struct sadb_prop */
#define SADB_EXT_SUPPORTED            14   /* struct sadb_supported */
#define SADB_EXT_SPIRANGE             15   /* struct sadb_spirange */
#define SADB_X_EXT_SRC_MASK           16   /* struct sadb_address */
#define SADB_X_EXT_DST_MASK           17   /* struct sadb_address */
#define SADB_X_EXT_PROTOCOL           18   /* struct sadb_protocol */
#define SADB_X_EXT_SA2                19   /* struct sadb_sa */
#define SADB_X_EXT_SRC_FLOW           20   /* struct sadb_address */
#define SADB_X_EXT_DST_FLOW           21   /* struct sadb_address */
#define SADB_X_EXT_DST2               22   /* struct sadb_address */
#define SADB_X_EXT_FLOW_OPTIONS       23   /* struct sadb_x_flowopt */
#define SADB_X_EXT_UDPENCAP           24   /* struct sadb_x_udpencap */
#define SADB_X_EXT_SRCMAX_FLOW        25   /* struct sadb_address */
#define SADB_X_EXT_DSTMAX_FLOW        26   /* struct sadb_address */
#define SADB_X_EXT_SA_OPTIONS         27   /* struct sadb_x_saopt */
#define SADB_X_EXT_KMADDRESS          28   /* struct sadb_x_kmaddress */
#define SADB_EXT_MAX                  28


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

#if !defined(IP_PORT_VXWORKS) || IP_PORT_VXWORKS < 65 || !defined(__INCvxTypesh)
#ifndef IPIPSEC_HAVE_UINT_TYPE
typedef Ip_u8   uint8_t;
typedef Ip_u16  uint16_t;
typedef Ip_u32  uint32_t;
#endif
#ifndef IPIPSEC_HAVE_UINT64_TYPE
typedef Ip_u64  uint64_t;
#endif
#endif /* __INCvxTypesBase */


/*
 *===========================================================================
 *                         sadb_msg
 *===========================================================================
 */
struct sadb_msg
{
    uint8_t    sadb_msg_version;  /* Set to PF_KEY_V2 */
    uint8_t    sadb_msg_type;     /* See SADB_xxx types below */
    uint8_t    sadb_msg_errno;
    uint8_t    sadb_msg_satype;   /* SADB_SATYPE_ESP, SADB_SATYPE_AH etc. */
    uint16_t   sadb_msg_len;
    uint16_t   sadb_msg_reserved; /* Must be set to 0 */
    uint32_t   sadb_msg_seq;
    uint32_t   sadb_msg_pid;      /* Process ID, must be non-zero */
};

/* sadb_msg_type */         /* Mandatory extensions: */
#define SADB_RESERVED    0
#define SADB_GETSPI      1  /* base,spirange,address_src,address_dst */
#define SADB_UPDATE      2  /* sa,address_src,address_dst */
#define SADB_ADD         3  /* sa,address_src,address_dst */
#define SADB_DELETE      4  /* sa,address_dst */
#define SADB_GET         5  /* sa,address_src,address_dst */
#define SADB_ACQUIRE     6
#define SADB_REGISTER    7
#define SADB_EXPIRE      8
#define SADB_FLUSH       9
#define SADB_DUMP        10
#define SADB_X_PROMISC   11
#define SADB_X_ADDFLOW   12 /* src_mask,dst_mask,src_flow,dst_flow */
#define SADB_X_DELFLOW   13 /* src_mask,dst_mask,src_flow,dst_flow */
#define SADB_X_GRPSPIS   14 /* sa,sa2,dst,dst2,protocol */
#define SADB_X_BINDSA    15 /* sa,sa2,dst,dst2,protocol */
#define SADB_X_FINDFLOW  16 /* src_flow,dst_flow,protocol */
#define SADB_X_DUMPFLOW  17
#define SADB_X_MIGRATE   18 /* migrate message for mip and ike comm */
#define SADB_MAX         18


/*
 *===========================================================================
 *                         sadb_ext
 *===========================================================================
 */
struct sadb_ext
{
    uint16_t   sadb_ext_len;
    uint16_t   sadb_ext_type;
};


/*
 *===========================================================================
 *                         sadb_sa
 *===========================================================================
 */
struct sadb_sa
{
    uint16_t   sadb_sa_len;
    uint16_t   sadb_sa_exttype;
    uint32_t   sadb_sa_spi;
    uint8_t    sadb_sa_replay;
    uint8_t    sadb_sa_state;
    uint8_t    sadb_sa_auth;
    uint8_t    sadb_sa_encrypt;
    uint32_t   sadb_sa_flags;
};


/* sadb_sa_state */
#define SADB_SASTATE_LARVAL          0
#define SADB_SASTATE_MATURE          1
#define SADB_SASTATE_DYING           2
#define SADB_SASTATE_DEAD            3
#define SADB_SASTATE_MAX             3

/* sadb_sa_auth */
#define SADB_AALG_NONE               0
#define SADB_AALG_MD5HMAC            1
#define SADB_AALG_SHA1HMAC           2
#define SADB_AALG_MD5HMAC96          3
#define SADB_AALG_SHA1HMAC96         4
#define SADB_X_AALG_RIPEMD160HMAC96  5
#define SADB_X_AALG_MD5              6
#define SADB_X_AALG_SHA1             7
#define SADB_X_AALG_AESXCBCMAC96     8
#define SADB_AALG_MAX                8

/* sadb_sa_encrypt */
#define SADB_EALG_NONE               0
#define SADB_EALG_DESCBC             1
#define SADB_EALG_3DESCBC            2
#define SADB_X_EALG_BLF              3
#define SADB_X_EALG_CAST             4
#define SADB_X_EALG_SKIPJACK         5    /* not supported */
#define SADB_X_EALG_AES              6    /* AESCBC */
#define SADB_X_EALG_AESCBC           6
#define SADB_X_EALG_AESCTR           7
#define SADB_EALG_MAX                7

/* sadb_sa_flags */
#define SADB_SAFLAGS_PFS                0x00000001L    /* perfect forward secrecy */
#define SADB_X_SAFLAGS_HALFIV           0x00000002L    /* Used for ESP-old */
#define SADB_X_SAFLAGS_TUNNEL           0x00000004L    /* Force tunneling */
#define SADB_X_SAFLAGS_CHAINDEL         0x00000008L    /* Delete whole SA chain */
#define SADB_X_SAFLAGS_REPLACEFLOW      0x00000020L    /* Replace existing flow */
#define SADB_X_SAFLAGS_INGRESS_FLOW     0x00000040L    /* Ingress flow entry */
#define SADB_X_SAFLAGS_INGRESS_SA       0x00000040L    /* Ingress SA entry */
#define SADB_X_SAFLAGS_RANDOMPADDING    0x00000080L    /* Random ESP padding */
#define SADB_X_SAFLAGS_NOREPLAY         0x00000100L    /* No replay counter */
#define SADB_X_SAFLAGS_UDPENCAP         0x00000200L    /* ESP in UDP */
#define SADB_X_SAFLAGS_EGRESS_FLOW      0x00000400L    /* Egress flow entry */
#define SADB_X_SAFLAGS_EGRESS_SA        0x00000400L    /* Egress (output) SA entry */
#define SADB_X_SAFLAGS_TFC              0x00000800L    /* Enable TFC on SA */
#define SADB_X_SAFLAGS_ESN              0x00001000L    /* Enable ESN on SA */
#define SADB_X_SAFLAGS_SRCREQ           0x00002000L    /* RFC4301p13: source IP must match */
#define SADB_X_SAFLAGS_CLRDF            0x00004000L    /* Clear DF on outer IPv4 header */
#define SADB_X_SAFLAGS_SETDF            0x00008000L    /* Set DF on outer IPv4 header */

#define SADB_KEY_FLAGS_MAX    0


/*
 *===========================================================================
 *                         sadb_lifetime
 *===========================================================================
 */
struct sadb_lifetime
{
    uint16_t   sadb_lifetime_len;
    uint16_t   sadb_lifetime_exttype;
    uint32_t   sadb_lifetime_allocations;
    uint64_t   sadb_lifetime_bytes;
    uint64_t   sadb_lifetime_addtime;
    uint64_t   sadb_lifetime_usetime;
};


/*
 *===========================================================================
 *                         sadb_address
 *===========================================================================
 */
struct sadb_address
{
    uint16_t   sadb_address_len;
    uint16_t   sadb_address_exttype;
    uint8_t    sadb_address_proto;      /* unused, set to 0 */
    uint8_t    sadb_address_prefixlen;  /* unused, set to 0 */
    uint16_t   sadb_address_reserved;
    /* Followed by the IP address stored in a socket structure */
};


/*
 *===========================================================================
 *                         sadb_key
 *===========================================================================
 */
struct sadb_key
{
    uint16_t   sadb_key_len;
    uint16_t   sadb_key_exttype;
    uint16_t   sadb_key_bits;
    uint16_t   sadb_key_reserved;
};


/*
 *===========================================================================
 *                         sadb_ident
 *===========================================================================
 */
struct sadb_ident
{
    uint16_t   sadb_ident_len;
    uint16_t   sadb_ident_exttype;
    uint16_t   sadb_ident_type;
    uint16_t   sadb_ident_reserved;
    uint64_t   sadb_ident_id;
};


/* sadb_ident_type */
#define SADB_IDENTTYPE_RESERVED   0
#define SADB_IDENTTYPE_PREFIX     1
#define SADB_IDENTTYPE_FQDN       2
#define SADB_IDENTTYPE_USERFQDN   3
#define SADB_IDENTTYPE_MAX        3


/*
 *===========================================================================
 *                         sadb_sens
 *===========================================================================
 */
struct sadb_sens
{
    uint16_t   sadb_sens_len;
    uint16_t   sadb_sens_exttype;
    uint32_t   sadb_sens_dpd;
    uint8_t    sadb_sens_sens_level;
    uint8_t    sadb_sens_sens_len;
    uint8_t    sadb_sens_integ_level;
    uint8_t    sadb_sens_integ_len;
    uint32_t   sadb_sens_reserved;
};


/*
 *===========================================================================
 *                         sadb_prop
 *===========================================================================
 */
struct sadb_prop
{
    uint16_t   sadb_prop_len;
    uint16_t   sadb_prop_exttype;
    uint8_t    sadb_prop_replay;
    uint8_t    sadb_prop_reserved[3];
};


/*
 *===========================================================================
 *                         sadb_comb
 *===========================================================================
 */
struct sadb_comb
{
    uint8_t    sadb_comb_auth;
    uint8_t    sadb_comb_encrypt;
    uint16_t   sadb_comb_flags;
    uint16_t   sadb_comb_auth_minbits;
    uint16_t   sadb_comb_auth_maxbits;
    uint16_t   sadb_comb_encrypt_minbits;
    uint16_t   sadb_comb_encrypt_maxbits;
    uint32_t   sadb_comb_reserved;
    uint32_t   sadb_comb_soft_allocations;
    uint32_t   sadb_comb_hard_allocations;
    uint64_t   sadb_comb_soft_bytes;
    uint64_t   sadb_comb_hard_bytes;
    uint64_t   sadb_comb_soft_addtime;
    uint64_t   sadb_comb_hard_addtime;
    uint64_t   sadb_comb_soft_usetime;
    uint64_t   sadb_comb_hard_usetime;
};


/*
 *===========================================================================
 *                         sadb_suppported
 *===========================================================================
 * original sadb_supported struct:
 *    struct sadb_supported
 *    {
 *       uint16_t   sadb_supported_len;
 *       uint16_t   sadb_supported_exttype;
 *       uint32_t   sadb_supported_reserved;
 *    };
 */
struct sadb_supported
{
    uint16_t  sadb_supported_len;
    uint16_t  sadb_supported_exttype;
    uint8_t   sadb_supported_nauth;
    uint8_t   sadb_supported_nencrypt;
    uint16_t  sadb_supported_reserved;
};


/*
 *===========================================================================
 *                         sadb_alg
 *===========================================================================
 */
struct sadb_alg
{
    uint8_t    sadb_alg_id;
    uint8_t    sadb_alg_ivlen;
    uint16_t   sadb_alg_minbits;
    uint16_t   sadb_alg_maxbits;
    uint16_t   sadb_alg_reserved;
};


/*
 *===========================================================================
 *                         sadb_spirange
 *===========================================================================
 */
struct sadb_spirange
{
    uint16_t   sadb_spirange_len;
    uint16_t   sadb_spirange_exttype;
    uint32_t   sadb_spirange_min;
    uint32_t   sadb_spirange_max;
    uint32_t   sadb_spirange_reserved;
};


/*
 *===========================================================================
 *                         sadb_protocol
 *===========================================================================
 */
struct sadb_protocol
{
    uint16_t   sadb_protocol_len;
    uint16_t   sadb_protocol_exttype;
    uint8_t    sadb_protocol_proto;
    uint8_t    sadb_protocol_ports0;   /* See supported _ports0 below */
    uint8_t    sadb_protocol_ports1;   /* See supported _ports1 below */
    uint8_t    sadb_protocol_reserved2;
};

/* Supported _ports0 and _ports1 selectors with _proto:
 * proto = IP_IPPROTO_ICMP    _ports0=ICMP type and _ports1=ICMP code
 * proto = IP_IPPROTO_MH      _ports0=Mobility Header type
 */


/*
 *===========================================================================
 *                         sadb_x_flowopt
 *===========================================================================
 */
struct sadb_x_flowopt
{
    uint16_t   sadb_flowopt_len;
    uint16_t   sadb_flowopt_exttype;     /* SADB_X_EXT_FLOW_OPTIONS */
    uint8_t    sadb_flowopt_type;        /* See SADB_X_FLOW_TYPE_xxx below */
	uint8_t    sadb_flowopt_direction;   /* 0=any, 1=in, 2=out */

    char       sadb_flowopt_priority;    /* Flow priority -128=highest, 127=lowest */
    uint8_t    sadb_flowopt_dscp;        /* DSCP value */
    uint32_t   sadb_flowopt_id;          /* Flow id != 0*/
    uint32_t   sadb_flowopt_reserved2;
};

/* SADB_X_EXT_FLOW_TYPE sadb_flowopt_type: */
#define SADB_X_FLOW_TYPE_UNSPEC        0
#define SADB_X_FLOW_TYPE_USE           1   /* Apply security (standard flow) */
#define SADB_X_FLOW_TYPE_ACQUIRE       2   /* Generate acquire message, send packet in cleartext */
#define SADB_X_FLOW_TYPE_REQUIRE       3   /* Generate require message, discard packet */
#define SADB_X_FLOW_TYPE_BYPASS        4   /* Send without applying security, i.e. cleartext */
#define SADB_X_FLOW_TYPE_DENY          5   /* Silent discard */


/*
 *===========================================================================
 *                         sadb_x_saopt
 *===========================================================================
 */
struct sadb_x_saopt
{
    uint16_t   sadb_saopt_len;
    uint16_t   sadb_saopt_exttype;     /* SADB_X_EXT_SA_OPTIONS */
    char       sadb_saopt_priority;    /* Sa priority -128=highest, 127=lowest */
    uint8_t    sadb_saopt_reserved;
    uint16_t   sadb_saopt_reserved2;
};


/*
 *===========================================================================
 *                         sadb_x_udpencap
 *===========================================================================
 */

struct sadb_x_udpencap
{
    uint16_t sadb_x_udpencap_len;
    uint16_t sadb_x_udpencap_exttype;
    uint16_t sadb_x_udpencap_port;      /* port in network endian */
    uint16_t sadb_x_udpencap_reserved;
};


/*
 *===========================================================================
 *                         sadb_x_kmaddress
 *===========================================================================
 */

/* sizeof(struct sadb_x_kmaddress) == 8 */
struct sadb_x_kmaddress
{
    uint16_t sadb_x_kmaddress_len;
    uint16_t sadb_x_kmaddress_exttype;
    uint32_t sadb_x_kmaddress_reserved;
};


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
#ifdef __cplusplus
}
#endif

#endif
/* __PFKEY_V2_H */

