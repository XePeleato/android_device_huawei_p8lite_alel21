/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipipsec_pfkeyv2.c,v $ $Revision: 1.87 $
 *   $Source: /home/interpeak/CVSRoot/ipipsec2/src/ipipsec_pfkeyv2.c,v $
 *   $Author: lob $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * General RFC support:
 * RFC4301 s4.4.1p23: "For every IPsec implementation, there MUST be a management interface that allows a user or system administrator to manage the SPD."
 * RFC4301 s4.4.1p24: "The management interface for the SPD MUST allow creation of entries consistent with the selectors defined in Section 4.4.1.1"
 * RFC4301 s4.4.1p24: "... and MUST support (total) ordering of these entries"
 * RFC4301 s4.4.1p24: "However, this document does specify a standard set of SPD elements that all IPsec implementations MUST support."
*/

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipipsec_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_sock.h>
#include <ipcom_syslog.h>
#include <ipcom_clib.h>
#include <ipcom_inet.h>
#include <ipcom_sysvar.h>
#include <ipcom_err.h>

#include "ipipsec_h.h"
#include "ipipsec_pfkeyv2.h"


/*
 ****************************************************************************
 * 4                    EXTERN PROTOTYPES
 ****************************************************************************
 */

IP_EXTERN Ipipsec_data  ipipsec;

IP_EXTERN IP_CONST char *ipipsec_pfkeyv2_command[];

#ifdef IPHWCRYPTO
IP_EXTERN int iphwcrypto_ipsec_esp_input(Ip_u8 *data, Ip_u32 length, Ipipsec_sa *sa, Ipipsec_param *param, Ip_u8 *digest, Ipcom_pkt *pkt);
IP_EXTERN int iphwcrypto_ipsec_esp_output(Ip_u8 *data, Ip_u32 length, Ipipsec_sa *sa, Ipipsec_param *param, Ip_u8 *digest, Ipcom_pkt *pkt);
#endif


/*
 ****************************************************************************
 * 5                    DEFINES
 ****************************************************************************
 */

#define IPCOM_SYSLOG_PRIORITY    IPIPSEC_SYSTEM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPIPSEC

#define EXT_SADBADDR_SIZE_MIN   (int)(sizeof(struct sadb_address) + IP_ROUNDUP(sizeof(struct Ip_sockaddr_in),8))
#define EXT_SADBADDR_SIZE_IN    24 /* same expression as above */
#define EXT_SADBADDR_SIZE_IN6   40 /* 8+sizeof(struct Ip_sockaddr_in6) = 8+28 = 36 -> rounded up = 40 */

#define IPIPSEC_EXPIRE_TYPE_HARD    0
#define IPIPSEC_EXPIRE_TYPE_SOFT    1


/*
 *===========================================================================
 *                         SADB_EXT_ BITMAP
 *===========================================================================
 */
#define SEBIT_SA                (1 << SADB_EXT_SA)
#define SEBIT_LIFETIME_CURRENT  (1 << SADB_EXT_LIFETIME_CURRENT)
#define SEBIT_LIFETIME_HARD     (1 << SADB_EXT_LIFETIME_HARD)
#define SEBIT_LIFETIME_SOFT     (1 << SADB_EXT_LIFETIME_SOFT)
#define SEBIT_ADDRESS_SRC       (1 << SADB_EXT_ADDRESS_SRC)
#define SEBIT_ADDRESS_DST       (1 << SADB_EXT_ADDRESS_DST)
#define SEBIT_ADDRESS_PROXY     (1 << SADB_EXT_ADDRESS_PROXY)
#define SEBIT_KEY_AUTH          (1 << SADB_EXT_KEY_AUTH)
#define SEBIT_KEY_ENCRYPT       (1 << SADB_EXT_KEY_ENCRYPT)
#define SEBIT_IDENTITY_SRC      (1 << SADB_EXT_IDENTITY_SRC)
#define SEBIT_IDENTITY_DST      (1 << SADB_EXT_IDENTITY_DST)
#define SEBIT_SENSITIVITY       (1 << SADB_EXT_SENSITIVITY)
#define SEBIT_PROPOSAL          (1 << SADB_EXT_PROPOSAL)
#define SEBIT_SUPPORTED         (1 << SADB_EXT_SUPPORTED)
#define SEBIT_SPIRANGE          (1 << SADB_EXT_SPIRANGE)
#define SEBIT_X_SRC_MASK        (1 << SADB_X_EXT_SRC_MASK)
#define SEBIT_X_DST_MASK        (1 << SADB_X_EXT_DST_MASK)
#define SEBIT_X_PROTOCOL        (1 << SADB_X_EXT_PROTOCOL)
#define SEBIT_X_SA2             (1 << SADB_X_EXT_SA2)
#define SEBIT_X_SRC_FLOW        (1 << SADB_X_EXT_SRC_FLOW)
#define SEBIT_X_DST_FLOW        (1 << SADB_X_EXT_DST_FLOW)
#define SEBIT_X_DST2            (1 << SADB_X_EXT_DST2)
#define SEBIT_X_FLOW_OPTIONS    (1 << SADB_X_EXT_FLOW_OPTIONS)
#define SEBIT_X_UDPENCAP        (1 << SADB_X_EXT_UDPENCAP)
#define SEBIT_X_SRCMAX_FLOW     (1 << SADB_X_EXT_SRCMAX_FLOW)
#define SEBIT_X_DSTMAX_FLOW     (1 << SADB_X_EXT_DSTMAX_FLOW)
#define SEBIT_X_EXT_SA_OPTIONS  (1 << SADB_X_EXT_SA_OPTIONS)
#define SEBIT_X_EXT_KMADDRESS   (1 << SADB_X_EXT_KMADDRESS)

/* Groups of bits */
#define SEBIT_LIFETIME  (SEBIT_LIFETIME_CURRENT | SEBIT_LIFETIME_HARD | SEBIT_LIFETIME_SOFT)
#define SEBIT_ADDRESS   (SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST | SEBIT_ADDRESS_PROXY)
#define SEBIT_KEY       (SEBIT_KEY_AUTH | SEBIT_KEY_ENCRYPT)
#define SEBIT_IDENTITY  (SEBIT_IDENTITY_SRC | SEBIT_IDENTITY_DST)
#define SEBIT_X_MAXFLOW (SEBIT_X_SRCMAX_FLOW | SEBIT_X_DSTMAX_FLOW)
#define SEBIT_X_FLOW    (SEBIT_X_SRC_FLOW | SEBIT_X_DST_FLOW)
#define SEBIT_X_MASK    (SEBIT_X_SRC_MASK | SEBIT_X_DST_MASK)


/*
 ****************************************************************************
 * 6                    TYPES
 ****************************************************************************
 */


/*
 *===========================================================================
 *                      Ipipsec_pfkeyv2_sadb
 *===========================================================================
 */
typedef struct Ipipsec_pfkeyv2_sadb_struct
{
    struct sadb_msg        *msg;
    struct sadb_sa         *sa;
    struct sadb_lifetime   *lifetime_current;
    struct sadb_lifetime   *lifetime_hard;
    struct sadb_lifetime   *lifetime_soft;
    struct sadb_address    *address_src;
    struct sadb_address    *address_dst;
    struct sadb_address    *address_proxy;
    struct sadb_key        *key_auth;
    struct sadb_key        *key_encrypt;
    struct sadb_ident      *ident_src;
    struct sadb_ident      *ident_dst;
    struct sadb_sens       *sens;
    struct sadb_prop       *prop;
    struct sadb_supported  *supported;
    struct sadb_spirange   *spirange;
    struct sadb_address    *src_mask;
    struct sadb_address    *dst_mask;
    struct sadb_protocol   *protocol;
    struct sadb_sa         *sa2;
    struct sadb_address    *src_flow;
    struct sadb_address    *dst_flow;
    struct sadb_address    *dst2;
    struct sadb_x_flowopt  *flowopt;
    struct sadb_x_udpencap *udpencap;
    struct sadb_address    *srcmax_flow;
    struct sadb_address    *dstmax_flow;
    struct sadb_x_saopt    *saopt;
}
Ipipsec_pfkeyv2_sadb;


/*
 *===========================================================================
 *                         Ipipsec_pfkeyv2_sadb_extbits
 *===========================================================================
 */
typedef struct Ipipsec_pfkeyv2_sadb_extbits_struct
{
    Ip_u32   in_req;
    Ip_u32   in_opt;
    Ip_u32   out_req;
    Ip_u32   out_opt;
}
Ipipsec_pfkeyv2_sadb_extbits;


/*
 *===========================================================================
 *                         Ipipsec_sadb_address_full
 *===========================================================================
 */
struct Ipipsec_sadb_address_full
{
    struct sadb_address       addr;
    union Ip_sockaddr_union   sock;
};


/*
 *===========================================================================
 *                         Ipipsec_sadb_prop_full
 *===========================================================================
 */
struct Ipipsec_sadb_prop_full
{
    struct sadb_prop   prop;
    struct sadb_comb   comb;
};


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC int    ipipsec_pfkeyv2_verify_satype(Ip_u8 satype);
IP_STATIC Ipipsec_sa *ipipsec_pfkeyv2_sadb_sa_lookup(Ipipsec_pfkeyv2_sadb *sadb, int sadir, Ip_bool exact);


IP_STATIC int    ipipsec_pfkeyv2_output(Ipcom_socket *sock, void *exthdr[SADB_EXT_MAX+1], Ip_u32 sndmode, Ip_u32 saerrno);
IP_STATIC int    ipipsec_pfkeyv2_output_sa(Ipcom_socket *sock, Ipipsec_sa *sa, Ipipsec_pfkeyv2_sadb *sadb, Ip_u32 sndmode);
IP_STATIC int
ipipsec_pfkeyv2_output_flow(Ipcom_socket *sock, Ipipsec_flow *flow, Ipipsec_pfkeyv2_sadb *sadb);

IP_STATIC void   ipipsec_pfkeyv2_ipaddr_to_extaddr(struct sadb_address *address, union Ip_in_addr_union *ipaddr, Ip_u16 port_n, int domain);
IP_STATIC int ipipsec_pfkeyv2_sadb_to_sa(Ipipsec_pfkeyv2_sadb *sadb, Ipipsec_sa *sa, Ip_u8 msgtype);
IP_STATIC int ipipsec_pfkeyv2_sadb_to_flowsel(Ipipsec_pfkeyv2_sadb *sadb, Ipipsec_flowsel *flowsel);
IP_STATIC int ipipsec_pfkeyv2_sadb_to_flowdir(Ipipsec_pfkeyv2_sadb *sadb, int default_flowdir);
IP_STATIC int ipipsec_pfkeyv2_sadb_to_sadir(Ipipsec_pfkeyv2_sadb *sadb, int default_dir);
IP_STATIC int ipipsec_pfkeyv2_sadb_to_key(Ipipsec_pfkeyv2_sadb *sadb, Ipipsec_key *key, int default_dir);
IP_STATIC int ipipsec_pfkeyv2_sadb_to_key2(Ipipsec_pfkeyv2_sadb *sadb, Ipipsec_key *key);

IP_STATIC int ipipsec_pfkeyv2_msg_sadb_dump(Ipcom_socket *sock, Ipipsec_pfkeyv2_sadb *sadb);
IP_STATIC int ipipsec_pfkeyv2_msg_sadb_flush(Ipipsec_pfkeyv2_sadb *sadb);
IP_STATIC int ipipsec_pfkeyv2_msg_sadb_add(Ipipsec_pfkeyv2_sadb *sadb);
IP_STATIC int ipipsec_pfkeyv2_msg_sadb_update(Ipipsec_pfkeyv2_sadb *sadb);
IP_STATIC int ipipsec_pfkeyv2_msg_sadb_getspi(Ipipsec_pfkeyv2_sadb *sadb, struct sadb_sa *samem);
IP_STATIC int ipipsec_pfkeyv2_msg_sadb_delete(Ipipsec_pfkeyv2_sadb *sadb);
IP_STATIC int ipipsec_pfkeyv2_msg_sadb_get(Ipcom_socket *sock, Ipipsec_pfkeyv2_sadb *sadb);
IP_STATIC int ipipsec_pfkeyv2_msg_sadb_x_addflow(Ipipsec_pfkeyv2_sadb *sadb);
IP_STATIC int ipipsec_pfkeyv2_msg_sadb_x_delflow(Ipipsec_pfkeyv2_sadb *sadb);
IP_STATIC int ipipsec_pfkeyv2_msg_sadb_x_findflow(Ipcom_socket *sock, Ipipsec_pfkeyv2_sadb *sadb);
IP_STATIC int ipipsec_pfkeyv2_msg_sadb_x_dumpflow(Ipcom_socket *sock, Ipipsec_pfkeyv2_sadb *sadb);
IP_STATIC int ipipsec_pfkeyv2_msg_sadb_x_grpspis(Ipipsec_pfkeyv2_sadb *sadb);
IP_STATIC int ipipsec_pfkeyv2_msg_sadb_register(Ipcom_socket *sock, Ipipsec_pfkeyv2_sadb *sadb);

IP_STATIC void ipipsec_pfkeyv2_copy_lifetimes(struct sadb_lifetime *lt, Ipipsec_sa_lifetime *sa_lt);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC int ipipsec_pfkeyv2_ext_sizes[SADB_EXT_MAX+1] =
{
    0,
    sizeof(struct sadb_sa),
    sizeof(struct sadb_lifetime), sizeof(struct sadb_lifetime), sizeof(struct sadb_lifetime),
    -EXT_SADBADDR_SIZE_MIN, -EXT_SADBADDR_SIZE_MIN, -EXT_SADBADDR_SIZE_MIN,
    -(int)sizeof(struct sadb_key), -(int)sizeof(struct sadb_key),
    -(int)sizeof(struct sadb_ident), -(int)sizeof(struct sadb_ident),
    -(int)sizeof(struct sadb_sens),
    -(int)sizeof(struct sadb_prop),
    -(int)sizeof(struct sadb_supported),
    sizeof(struct sadb_spirange),
    -EXT_SADBADDR_SIZE_MIN, -EXT_SADBADDR_SIZE_MIN, /* src_mask & dst_mask */
    sizeof(struct sadb_protocol),
    sizeof(struct sadb_sa),               /* sa2 */
    -EXT_SADBADDR_SIZE_MIN, -EXT_SADBADDR_SIZE_MIN, /* src_flow & dst_flow */
    -EXT_SADBADDR_SIZE_MIN,                    /* dst2 */
    sizeof(struct sadb_x_flowopt), /* flowopt */
    sizeof(struct sadb_x_udpencap),
    -EXT_SADBADDR_SIZE_MIN, -EXT_SADBADDR_SIZE_MIN, /* srcmax_flow & dstmax_flow */
    sizeof(struct sadb_x_saopt), /* saopt */
    (int)-sizeof(struct sadb_x_kmaddress), /*sadb_x_kmaddress*/
};


IP_STATIC Ipipsec_pfkeyv2_sadb_extbits ipipsec_pfkeyv2_sadbextbits[SADB_MAX+1] =
{
  /* FIELDS: (SADB_RESERVED) */
  { 0, /* in_req */
    0, /* in_opt */
    0, /* out_req */
    0  /* out_opt */
  },
  /* SADB_GETSPI */
  { SEBIT_SPIRANGE | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST,
    SEBIT_SPIRANGE | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST | SEBIT_SA | SEBIT_X_EXT_SA_OPTIONS,
    SEBIT_SA       | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST,
    SEBIT_SA       | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST | SEBIT_X_EXT_SA_OPTIONS
  },
  /* SADB_UPDATE */
  { SEBIT_SA | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST,
    SEBIT_SA | SEBIT_LIFETIME | SEBIT_ADDRESS | SEBIT_KEY | SEBIT_IDENTITY | SEBIT_SENSITIVITY | SEBIT_X_EXT_SA_OPTIONS,
    SEBIT_SA | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST,
    SEBIT_SA | SEBIT_LIFETIME | SEBIT_ADDRESS | SEBIT_IDENTITY | SEBIT_SENSITIVITY | SEBIT_X_EXT_SA_OPTIONS
  },
  /* SADB_ADD */
  { SEBIT_SA | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST,
    SEBIT_SA | SEBIT_LIFETIME | SEBIT_ADDRESS | SEBIT_KEY | SEBIT_IDENTITY | SEBIT_SENSITIVITY | SEBIT_X_UDPENCAP | SEBIT_X_EXT_SA_OPTIONS,
    SEBIT_SA | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST,
    SEBIT_SA | SEBIT_ADDRESS | SEBIT_LIFETIME | SEBIT_IDENTITY | SEBIT_SENSITIVITY | SEBIT_X_UDPENCAP | SEBIT_X_EXT_SA_OPTIONS
  },
  /* SADB_DELETE */
  { SEBIT_SA | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST,
    SEBIT_SA | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST | SEBIT_X_EXT_SA_OPTIONS,
    SEBIT_SA | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST,
    SEBIT_SA | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST | SEBIT_X_EXT_SA_OPTIONS
  },
  /* SADB_GET */
  { SEBIT_SA | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST,
    SEBIT_SA | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST | SEBIT_X_EXT_SA_OPTIONS,
    SEBIT_SA | SEBIT_LIFETIME_CURRENT | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST,
    SEBIT_SA | SEBIT_LIFETIME | SEBIT_ADDRESS | SEBIT_KEY | SEBIT_IDENTITY | SEBIT_SENSITIVITY | SEBIT_X_EXT_SA_OPTIONS
  },
  /* SADB_ACQUIRE */
  { 0,
    SEBIT_ADDRESS | SEBIT_IDENTITY | SEBIT_SENSITIVITY | SEBIT_PROPOSAL ,
    0,
    SEBIT_ADDRESS | SEBIT_IDENTITY | SEBIT_SENSITIVITY | SEBIT_PROPOSAL
  },
  /* SADB_REGISTER */
  { 0,
    0,
    SEBIT_SUPPORTED,
    SEBIT_SUPPORTED
  },
  /* SADB_EXPIRE */
  { 0,
    0,
    SEBIT_SA | SEBIT_LIFETIME_CURRENT | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST,
    SEBIT_SA | SEBIT_LIFETIME | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST | SEBIT_X_EXT_SA_OPTIONS,
  },
  /* SADB_FLUSH */
  { 0,
    0,
    0,
    0
  },
  /* SADB_DUMP */
  { 0,
    0,
    0,
    SEBIT_SA | SEBIT_LIFETIME | SEBIT_ADDRESS | SEBIT_IDENTITY | SEBIT_SENSITIVITY | SEBIT_X_UDPENCAP | SEBIT_X_EXT_SA_OPTIONS,
  },
  /* SADB_X_PROMISC */
  { 0,
    0,
    0,
    0
  },
  /* SADB_X_ADDFLOW */
  { SEBIT_X_FLOW,
    SEBIT_SA | SEBIT_ADDRESS_DST | SEBIT_X_MASK | SEBIT_X_FLOW | SEBIT_ADDRESS_SRC | SEBIT_X_PROTOCOL | SEBIT_X_FLOW_OPTIONS | SEBIT_X_MAXFLOW | SEBIT_X_EXT_SA_OPTIONS,
    SEBIT_X_FLOW,
    SEBIT_SA | SEBIT_ADDRESS_DST | SEBIT_X_MASK | SEBIT_X_FLOW | SEBIT_ADDRESS_SRC | SEBIT_X_PROTOCOL | SEBIT_X_FLOW_OPTIONS | SEBIT_X_MAXFLOW | SEBIT_X_EXT_SA_OPTIONS
  },
  /* SADB_X_DELFLOW */
  { 0,
    SEBIT_SA | SEBIT_X_MASK | SEBIT_X_FLOW | SEBIT_X_PROTOCOL | SEBIT_ADDRESS_DST | SEBIT_X_FLOW_OPTIONS | SEBIT_X_MAXFLOW | SEBIT_X_EXT_SA_OPTIONS,
    0,
    SEBIT_SA | SEBIT_X_MASK | SEBIT_X_FLOW | SEBIT_X_PROTOCOL | SEBIT_ADDRESS_DST | SEBIT_X_FLOW_OPTIONS | SEBIT_X_MAXFLOW | SEBIT_X_EXT_SA_OPTIONS
  },
  /* SADB_X_GRPSPIS */
  { SEBIT_SA | SEBIT_X_SA2 | SEBIT_X_DST2 | SEBIT_ADDRESS_DST | SEBIT_X_PROTOCOL,
    SEBIT_SA | SEBIT_X_SA2 | SEBIT_X_DST2 | SEBIT_ADDRESS_DST | SEBIT_X_PROTOCOL | SEBIT_X_EXT_SA_OPTIONS | SEBIT_ADDRESS_SRC,
    SEBIT_SA | SEBIT_X_SA2 | SEBIT_X_DST2 | SEBIT_ADDRESS_DST | SEBIT_X_PROTOCOL,
    SEBIT_SA | SEBIT_X_SA2 | SEBIT_X_DST2 | SEBIT_ADDRESS_DST | SEBIT_X_PROTOCOL | SEBIT_X_EXT_SA_OPTIONS | SEBIT_ADDRESS_SRC
  },
  /* SADB_X_BINDSA */
  { SEBIT_SA | SEBIT_X_SA2 | SEBIT_X_DST2 | SEBIT_ADDRESS_DST | SEBIT_X_PROTOCOL,
    SEBIT_SA | SEBIT_X_SA2 | SEBIT_X_DST2 | SEBIT_ADDRESS_DST | SEBIT_X_PROTOCOL,
    SEBIT_SA | SEBIT_X_SA2 | SEBIT_X_DST2 | SEBIT_ADDRESS_DST | SEBIT_X_PROTOCOL,
    SEBIT_SA | SEBIT_X_SA2 | SEBIT_X_DST2 | SEBIT_ADDRESS_DST | SEBIT_X_PROTOCOL
  },
  /* SADB_X_FINDFLOW */
  { SEBIT_X_FLOW | SEBIT_X_PROTOCOL,
    SEBIT_X_FLOW | SEBIT_X_PROTOCOL | SEBIT_SA | SEBIT_ADDRESS_DST | SEBIT_X_FLOW_OPTIONS | SEBIT_X_EXT_SA_OPTIONS,
    SEBIT_X_FLOW | SEBIT_X_PROTOCOL | SEBIT_X_MASK,
    SEBIT_X_FLOW | SEBIT_X_MAXFLOW | SEBIT_X_FLOW_OPTIONS | SEBIT_X_PROTOCOL | SEBIT_X_MASK | SEBIT_SA | SEBIT_X_EXT_SA_OPTIONS
  },
  /* SADB_X_DUMPFLOW */
  { 0,
    SEBIT_SA | SEBIT_X_FLOW_OPTIONS,
    SEBIT_X_FLOW | SEBIT_X_PROTOCOL | SEBIT_X_MASK,
    SEBIT_X_FLOW | SEBIT_X_MAXFLOW | SEBIT_X_FLOW_OPTIONS | SEBIT_X_PROTOCOL | SEBIT_X_MASK | SEBIT_SA | SEBIT_X_EXT_SA_OPTIONS
  },
    /* SADB_X_MIGRATE */
  { SEBIT_X_EXT_KMADDRESS,
    SEBIT_X_EXT_KMADDRESS,
    SEBIT_X_EXT_KMADDRESS,
    SEBIT_X_EXT_KMADDRESS
  }
};


/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_verify_satype
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_verify_satype(Ip_u8 satype)
{
    switch(satype)
    {
#ifdef IPIPSEC_USE_AH
    case SADB_SATYPE_AH :
        if (ipipsec_conf_ah_enable != 1)
            return -IP_ERRNO_EOPNOTSUPP;
        break;
#endif
#ifdef IPIPSEC_USE_ESP
    case SADB_SATYPE_ESP :
        if (ipipsec_conf_esp_enable != 1)
            return -IP_ERRNO_EOPNOTSUPP;
        break;
#endif
#ifdef IPIPSEC_USE_IPIP
    case SADB_X_SATYPE_IPIP :
        if (ipipsec_conf_ipip_allow != 1)
            return -IP_ERRNO_EOPNOTSUPP;
        break;
#endif
    default:
        return -IP_ERRNO_EOPNOTSUPP;
    }

    return 0;  /* ok */
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_sadb_to_key
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_sadb_to_key(Ipipsec_pfkeyv2_sadb *sadb, Ipipsec_key *key, int default_dir)
{
    struct Ip_sockaddr *sockaddr;
    int in_addr_size, sock_in_addr_offset;
    int sadir;
    Ip_u8 domain;

    ip_assert(sadb->msg);

    ipcom_memset(key, 0, sizeof(Ipipsec_key));

    /* Domain, default to IPv4 */
    if (sadb->address_dst)
    {
        sockaddr = (struct Ip_sockaddr *)((Ip_u8 *)sadb->address_dst + sizeof(struct sadb_address));
        domain   = (Ip_u8)sockaddr->sa_family;
        if (domain != IP_AF_INET && domain != IP_AF_INET6)
            return -IP_ERRNO_EINVAL;
    }
    else
        domain = IP_AF_INET;

    /* Direction */
    sadir = ipipsec_pfkeyv2_sadb_to_sadir(sadb, default_dir);
    if (sadir < 0)
        return sadir;

    /* General info */
    key->domain    = domain;
    key->proto     = IPIPSEC_SADB_GET_PROTO(sadb->msg->sadb_msg_satype);
    key->direction = (Ip_u8)sadir;
    /* Priority */
    if (sadb->saopt)
        key->priority = sadb->saopt->sadb_saopt_priority;

    in_addr_size        = 4;
    sock_in_addr_offset = 2;
#ifdef IPCOM_USE_INET6
    if (domain == IP_AF_INET6)
    {
        in_addr_size        = 16;
        sock_in_addr_offset = 6;
    }
#endif

    /* SPI */
    if (sadb->sa)
        key->ports.espah_spi_n = sadb->sa->sadb_sa_spi;

    /* Destination address */
    if (sadb->address_dst)
    {
        sockaddr = (struct Ip_sockaddr *)((Ip_u8 *)sadb->address_dst + sizeof(struct sadb_address));
        ipcom_memcpy(&key->dst, &sockaddr->sa_data[sock_in_addr_offset], in_addr_size);
    }

    /* Source address */
    if (sadb->address_src)
    {
        sockaddr = (struct Ip_sockaddr *)((Ip_u8 *)sadb->address_src + sizeof(struct sadb_address));
        ipcom_memcpy(&key->src, &sockaddr->sa_data[sock_in_addr_offset], in_addr_size);
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_sadb_to_key2
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_sadb_to_key2(Ipipsec_pfkeyv2_sadb *sadb, Ipipsec_key *key)
{
    struct Ip_sockaddr *sockaddr;
    int in_addr_size, sock_in_addr_offset;
    Ip_u8 domain;

    ip_assert(sadb->msg);
    ip_assert(sadb->protocol);

    ipcom_memset(key, 0, sizeof(Ipipsec_key));

    /* Domain */
    if (sadb->dst2)
    {
        sockaddr = (struct Ip_sockaddr *)((Ip_u8 *)sadb->dst2 + sizeof(struct sadb_address));
        domain   = (Ip_u8)sockaddr->sa_family;
    }
    else
        return -IP_ERRNO_EINVAL;
    if (domain != IP_AF_INET && domain != IP_AF_INET6)
        return -IP_ERRNO_EINVAL;

    /* General info */
    key->domain    = domain;
    key->proto     = IPIPSEC_SADB_GET_PROTO(sadb->protocol->sadb_protocol_proto);
    key->direction = IPIPSEC_DIR_OUT;

    in_addr_size        = 4;
    sock_in_addr_offset = 2;
#ifdef IPCOM_USE_INET6
    if (domain == IP_AF_INET6)
    {
        in_addr_size        = 16;
        sock_in_addr_offset = 6;
    }
#endif

    /* SPI */
    if (sadb->sa2)
        key->ports.espah_spi_n = sadb->sa2->sadb_sa_spi;

    /* Destination address */
    if (sadb->dst2)
    {
        sockaddr = (struct Ip_sockaddr *)((Ip_u8 *)sadb->dst2 + sizeof(struct sadb_address));
        ipcom_memcpy(&key->dst, &sockaddr->sa_data[sock_in_addr_offset], in_addr_size);
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_sadb_sa_lookup
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipipsec_sa *
ipipsec_pfkeyv2_sadb_sa_lookup(Ipipsec_pfkeyv2_sadb *sadb, int sadir, Ip_bool exact)
{
    Ipipsec_key  key;

    ip_assert(sadb->msg);

    if (sadb->msg->sadb_msg_satype == SADB_X_SATYPE_BYPASS)
        return &ipipsec.sa_bypass;

    if (sadb->sa == IP_NULL)
        return IP_NULL;

    if (ipipsec_pfkeyv2_sadb_to_key(sadb, &key, sadir) != 0)
        return IP_NULL;

    if (exact == IP_TRUE)
        return ipipsec_sa_lookup_exact(&key);
    else
        return ipipsec_sa_lookup(&key, 0);
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_ipaddr_to_extaddr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipipsec_pfkeyv2_ipaddr_to_extaddr(struct sadb_address *address, union Ip_in_addr_union *ipaddr, Ip_u16 port, int domain)
{
    ip_assert(address);
    ip_assert(domain == IP_AF_INET || domain == IP_AF_INET6);

    address->sadb_address_len = sizeof(struct sadb_address);

#ifdef IPCOM_USE_INET
    if (domain == IP_AF_INET)
    {
        struct Ip_sockaddr_in   *sockaddr_in;

        address->sadb_address_len += IP_ROUNDUP(sizeof(struct Ip_sockaddr_in), 8);

        sockaddr_in = (struct Ip_sockaddr_in *)((Ip_u8 *)address + sizeof(struct sadb_address));
        IPCOM_SA_LEN_SET(sockaddr_in, sizeof(struct Ip_sockaddr_in));
        sockaddr_in->sin_family = IP_AF_INET;
        sockaddr_in->sin_port = ip_htons(port);
        if (ipaddr != IP_NULL)
            sockaddr_in->sin_addr.s_addr = ipaddr->in.s_addr;
    }
#endif

#ifdef IPCOM_USE_INET6
    if (domain == IP_AF_INET6)
    {
        struct Ip_sockaddr_in6   *sockaddr_in6;

        address->sadb_address_len += IP_ROUNDUP(sizeof(struct Ip_sockaddr_in6), 8);

        sockaddr_in6 = (struct Ip_sockaddr_in6 *)((Ip_u8 *)address + sizeof(struct sadb_address));
        IPCOM_SA_LEN_SET(sockaddr_in6, sizeof(struct Ip_sockaddr_in6));
        sockaddr_in6->sin6_family = IP_AF_INET6;
        if (ipaddr != IP_NULL)
            ipcom_memcpy(&sockaddr_in6->sin6_addr, ipaddr, 16);
        sockaddr_in6->sin6_port = ip_htons(port);
        sockaddr_in6->sin6_scope_id = 0;
    }
#endif

    ip_assert((address->sadb_address_len & 0x7) == 0);
    address->sadb_address_len >>= 3 ;
    address->sadb_address_proto = 0;
    ip_assert(address->sadb_address_prefixlen == 0);
    ip_assert(address->sadb_address_reserved == 0);
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_output_sa
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_output_sa(Ipcom_socket *sock, Ipipsec_sa *sa, Ipipsec_pfkeyv2_sadb *sadb, Ip_u32 sndmode)
{
    Ip_size_t  length;
    int        retval;
    Ip_u8     *orgbuf, *msgbuf;
    int        addrsize;

#if defined(IPCOM_USE_INET) && defined(IPCOM_USE_INET6)
    addrsize = sa->key.domain == IP_AF_INET ? EXT_SADBADDR_SIZE_IN : EXT_SADBADDR_SIZE_IN6;
#elif defined(IPCOM_USE_INET6)
    addrsize = EXT_SADBADDR_SIZE_IN6;
#else
    addrsize = EXT_SADBADDR_SIZE_IN;
#endif

    length = sizeof(struct sadb_sa) + (3 * sizeof(struct sadb_lifetime))
        + (3 * addrsize)
        + sizeof(struct sadb_x_udpencap)
        + sizeof(struct sadb_x_saopt);
    /*!! ignore key(s) (or send empty keys to tell keylength?) */
    /* note: no support for identity & sensitivity */

    orgbuf = (Ip_u8 *)ipcom_malloc(length + sizeof(Ip_u64));
    if (orgbuf == IP_NULL)
    {
        IPCOM_LOG1(ERR, "ipipsec_pfkeyv2_output_sa() :: out of memory ('msgbuf' %ld bytes)",
                   length + sizeof(Ip_u64));
        return -IP_ERRNO_ENOMEM;
    }
    msgbuf = IP_PTR_ALIGN(orgbuf, 8);   /* 64-bit align msgbuf */
    ip_assert(((Ip_ptrdiff_t)msgbuf & 0x7L) == 0);
    ipcom_memset(msgbuf, 0, length);

    /* Use 'msg' from the incoming buffer. Use 'msgbuf' for the rest */
    ipcom_memset(&sadb->sa, 0, SADB_EXT_MAX * sizeof(void*)); /*lint !e419 */

    /* SADB_MSG */
    ip_assert(sadb->msg->sadb_msg_version == PF_KEY_V2);
    sadb->msg->sadb_msg_satype = sa->sadb_msg_satype;
    ip_assert(sadb->msg->sadb_msg_reserved == 0);

    /* SADB_EXT_SA */
    sadb->sa = (struct sadb_sa *)msgbuf;
    msgbuf += sizeof(struct sadb_sa);
    sadb->sa->sadb_sa_len = sizeof(struct sadb_sa) / sizeof(Ip_u64);
    sadb->sa->sadb_sa_exttype = SADB_EXT_SA;
    sadb->sa->sadb_sa_spi     = sa->key.ports.espah_spi_n;  /* [2.3.1] already in network endian */
    sadb->sa->sadb_sa_replay  = sa->sadb_sa_replay;
    sadb->sa->sadb_sa_state   = sa->sadb_sa_state;
    sadb->sa->sadb_sa_auth    = sa->sadb_sa_auth;
    sadb->sa->sadb_sa_encrypt = sa->sadb_sa_encrypt;
    sadb->sa->sadb_sa_flags   = sa->sadb_sa_flags;

    /* SADB_EXT_ADDRESS_SRC */
    sadb->address_src = (struct sadb_address *)msgbuf;
    msgbuf += addrsize;
    ipipsec_pfkeyv2_ipaddr_to_extaddr(sadb->address_src, &sa->key.src, 0, sa->key.domain);
    sadb->address_src->sadb_address_exttype = SADB_EXT_ADDRESS_SRC;

    /* SADB_EXT_ADDRESS_DST */
    sadb->address_dst = (struct sadb_address *)msgbuf;
    msgbuf += addrsize;
    ipipsec_pfkeyv2_ipaddr_to_extaddr(sadb->address_dst, &sa->key.dst, 0, sa->key.domain);
    sadb->address_dst->sadb_address_exttype = SADB_EXT_ADDRESS_DST;

    /* SADB_EXT_ADDRESS_PROXY */
    if (ipcom_memcmp(ipipsec.zeroes, &sa->proxy, sa->in_addr_size))
    {
        sadb->address_proxy = (struct sadb_address *)msgbuf;
        msgbuf += addrsize;
        ipipsec_pfkeyv2_ipaddr_to_extaddr(sadb->address_proxy, &sa->proxy, 0, sa->key.domain);
        sadb->address_proxy->sadb_address_exttype = SADB_EXT_ADDRESS_PROXY;
    }

    /* SADB_EXT_LIFETIME_CURRENT */
    sadb->lifetime_current = (struct sadb_lifetime *)msgbuf;
    msgbuf += sizeof(struct sadb_lifetime);
    sadb->lifetime_current->sadb_lifetime_len = sizeof(struct sadb_lifetime) / sizeof(Ip_u64);
    sadb->lifetime_current->sadb_lifetime_exttype = SADB_EXT_LIFETIME_CURRENT;
    ipipsec_pfkeyv2_copy_lifetimes(sadb->lifetime_current, &sa->ltcurrent);

    /* SADB_EXT_LIFETIME_HARD */
    if (IP_U64_GETLO(sa->lthard.bytes)
       || sa->lthard.addtime
       || sa->lthard.usetime)
    {
        sadb->lifetime_hard = (struct sadb_lifetime *)msgbuf;
        msgbuf += sizeof(struct sadb_lifetime);
        sadb->lifetime_hard->sadb_lifetime_len = sizeof(struct sadb_lifetime) / sizeof(Ip_u64);
        sadb->lifetime_hard->sadb_lifetime_exttype = SADB_EXT_LIFETIME_HARD;
        ipipsec_pfkeyv2_copy_lifetimes(sadb->lifetime_hard, &sa->lthard);
    }

    /* SADB_EXT_LIFETIME_SOFT */
    if (IP_U64_GETLO(sa->ltsoft.bytes)
       || sa->ltsoft.addtime
       || sa->ltsoft.usetime)
    {
        sadb->lifetime_soft = (struct sadb_lifetime *)msgbuf;
        msgbuf += sizeof(struct sadb_lifetime);
        sadb->lifetime_soft->sadb_lifetime_len = sizeof(struct sadb_lifetime) / sizeof(Ip_u64);
        sadb->lifetime_soft->sadb_lifetime_exttype = SADB_EXT_LIFETIME_SOFT;
        ipipsec_pfkeyv2_copy_lifetimes(sadb->lifetime_soft, &sa->ltsoft);
    }

    /* SADB_X_EXT_UDPENCAP */
    if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_UDPENCAP))
    {
        sadb->udpencap = (struct sadb_x_udpencap *)msgbuf;
        msgbuf += sizeof(struct sadb_x_udpencap);
        sadb->udpencap->sadb_x_udpencap_len     = sizeof(struct sadb_x_udpencap) / sizeof(Ip_u64);
        sadb->udpencap->sadb_x_udpencap_exttype = SADB_X_EXT_UDPENCAP;
        sadb->udpencap->sadb_x_udpencap_port    = sa->encap_srcport_n;
    }

    /* SADB_X_EXT_SA_OPTIONS */
    if (sa->key.priority != 0)
    {
        sadb->saopt = (struct sadb_x_saopt *)msgbuf;
        /*msgbuf += sizeof(struct sadb_x_saopt);*/
        sadb->saopt->sadb_saopt_len      = sizeof(struct sadb_x_saopt) / sizeof(Ip_u64);
        sadb->saopt->sadb_saopt_exttype  = SADB_X_EXT_SA_OPTIONS;
        sadb->saopt->sadb_saopt_priority = sa->key.priority;
    }

    /* Send the message */
    retval = ipipsec_pfkeyv2_output(sock, (void *)sadb, sndmode, 0);

    ipcom_free(orgbuf);

    return retval;
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_sadb_to_flowdir
 *===========================================================================
 * Description:   Get flow direction from key message
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_sadb_to_flowdir(Ipipsec_pfkeyv2_sadb *sadb, int default_flowdir)
{
    int flowdir;

    flowdir = default_flowdir;

    if (sadb->sa)
    {
        /* Check sadb_sa_flags first to not change behaviour for older applications */
        flowdir = IP_BIT_ISSET(sadb->sa->sadb_sa_flags, SADB_X_SAFLAGS_INGRESS_FLOW) ?
            IPIPSEC_DIR_IN : IPIPSEC_DIR_OUT;
    }
    else if (sadb->flowopt && sadb->flowopt->sadb_flowopt_direction > 0)
    {
        flowdir = sadb->flowopt->sadb_flowopt_direction - 1;
    }

    if (flowdir != IPIPSEC_DIR_IN && flowdir != IPIPSEC_DIR_OUT && flowdir != IPIPSEC_DIR_ANY)
        return -IP_ERRNO_EINVAL;

    return flowdir;
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_output_flow
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_output_flow(Ipcom_socket *sock, Ipipsec_flow *flow, Ipipsec_pfkeyv2_sadb *sadb)
{
    Ip_size_t  length;
    int        retval;
    Ip_u8     *orgbuf, *msgbuf;
    int        addrsize;
    Ip_u16     src_port, dst_port;

    ip_assert(flow != IP_NULL);

#if defined(IPCOM_USE_INET) && defined(IPCOM_USE_INET6)
    addrsize = flow->flowsel.key.domain == IP_AF_INET ? EXT_SADBADDR_SIZE_IN : EXT_SADBADDR_SIZE_IN6;
#elif defined(IPCOM_USE_INET6)
    addrsize = EXT_SADBADDR_SIZE_IN6;
#else
    addrsize = EXT_SADBADDR_SIZE_IN;
#endif

    switch (flow->flowsel.key.proto)
    {
    case IP_IPPROTO_TCP:
    case IP_IPPROTO_UDP:
        src_port = ip_ntohs(flow->flowsel.key.ports.udptcp_srcdst_n[0]);
        dst_port = ip_ntohs(flow->flowsel.key.ports.udptcp_srcdst_n[1]);
        break;
    default:
        src_port = dst_port = 0;
        break;
    }

    length = 6 * addrsize + sizeof(struct sadb_protocol) + sizeof(struct sadb_x_flowopt);

    orgbuf = (Ip_u8 *)ipcom_malloc(length + sizeof(Ip_u64));
    if (orgbuf == IP_NULL)
    {
        IPCOM_LOG1(ERR, "ipipsec_pfkeyv2_output_flow() :: out of memory ('msgbuf' %ld bytes)",
                   length + sizeof(Ip_u64));
        return -IP_ERRNO_ENOMEM;
    }
    msgbuf = IP_PTR_ALIGN(orgbuf, 8);   /* 64-bit align msgbuf */
    ip_assert(((Ip_ptrdiff_t)msgbuf & 0x7L) == 0);
    ipcom_memset(msgbuf, 0, length);

    /* Use 'msg' from the incoming buffer. Use 'msgbuf' for the rest */
    ipcom_memset(&sadb->sa, 0, SADB_EXT_MAX * sizeof(void*)); /*lint !e419 */

    /* SADB_MSG */
    ip_assert(sadb->msg->sadb_msg_version == PF_KEY_V2);
    sadb->msg->sadb_msg_satype = flow->sa ? flow->sa->sadb_msg_satype : SADB_SATYPE_UNSPEC;
    ip_assert(sadb->msg->sadb_msg_reserved == 0);

    /* SADB_X_EXT_SRC_MASK */
    sadb->src_mask = (struct sadb_address *)msgbuf;
    msgbuf += addrsize;
    ipipsec_pfkeyv2_ipaddr_to_extaddr(sadb->src_mask,
                                      &flow->flowsel.mask.src,
                                      0,
                                      flow->flowsel.key.domain);
    sadb->src_mask->sadb_address_exttype = SADB_X_EXT_SRC_MASK;

    /* SADB_X_EXT_DST_MASK */
    sadb->dst_mask = (struct sadb_address *)msgbuf;
    msgbuf += addrsize;
    ipipsec_pfkeyv2_ipaddr_to_extaddr(sadb->dst_mask,
                                      &flow->flowsel.mask.dst,
                                      0,
                                      flow->flowsel.key.domain);
    sadb->dst_mask->sadb_address_exttype = SADB_X_EXT_DST_MASK;

    /* SADB_X_EXT_PROTOCOL */
    sadb->protocol = (struct sadb_protocol *)msgbuf;
    msgbuf += sizeof(struct sadb_protocol);
    sadb->protocol->sadb_protocol_len      = sizeof(struct sadb_protocol) >> 3;
    sadb->protocol->sadb_protocol_exttype  = SADB_X_EXT_PROTOCOL;
    sadb->protocol->sadb_protocol_proto    = flow->flowsel.key.proto; /* TCP, UDP, etc */
    sadb->protocol->sadb_protocol_ports0   = flow->flowsel.key.ports.icmp_typecode[0];
    sadb->protocol->sadb_protocol_ports1   = flow->flowsel.key.ports.icmp_typecode[1];

    /* SADB_X_EXT_FLOW_OPTIONS */
    sadb->flowopt = (struct sadb_x_flowopt *)msgbuf;
    msgbuf += sizeof(struct sadb_x_flowopt);
    sadb->flowopt->sadb_flowopt_len       = sizeof(struct sadb_x_flowopt) >> 3;
    sadb->flowopt->sadb_flowopt_exttype   = SADB_X_EXT_FLOW_OPTIONS;
    sadb->flowopt->sadb_flowopt_type      = flow->flowtype;
    sadb->flowopt->sadb_flowopt_direction = flow->flowsel.key.direction + 1; /* 0=any, 1=in, 2=out */
    ip_assert(sadb->flowopt->sadb_flowopt_direction <= 2);
    sadb->flowopt->sadb_flowopt_priority  = flow->flowsel.key.priority;
    sadb->flowopt->sadb_flowopt_dscp      = flow->flowsel.key.dscp;
    sadb->flowopt->sadb_flowopt_id        = flow->id;

    /* SADB_X_EXT_SRC_FLOW */
    sadb->src_flow = (struct sadb_address *)msgbuf;
    msgbuf += addrsize;
    ipipsec_pfkeyv2_ipaddr_to_extaddr(sadb->src_flow,
                                      &flow->flowsel.key.src,
                                      src_port,
                                      flow->flowsel.key.domain);
    sadb->src_flow->sadb_address_exttype = SADB_X_EXT_SRC_FLOW;

    /* SADB_X_EXT_DST_FLOW */
    sadb->dst_flow = (struct sadb_address *)msgbuf;
    msgbuf += addrsize;
    ipipsec_pfkeyv2_ipaddr_to_extaddr(sadb->dst_flow,
                                      &flow->flowsel.key.dst,
                                      dst_port,
                                      flow->flowsel.key.domain);
    sadb->dst_flow->sadb_address_exttype = SADB_X_EXT_DST_FLOW;

    /* Optional SADB_X_EXT_SRCMAX_FLOW */
    if (ip_ntohs(flow->flowsel.key.ports.udptcp_srcdst_n[0])
        != flow->flowsel.src_max_port
        || ipcom_memcmp(&flow->flowsel.key.src, &flow->flowsel.src_max,
                        flow->flowsel.key.domain == IP_AF_INET ? 4 : 16))
    {
        sadb->srcmax_flow = (struct sadb_address *)msgbuf;
        msgbuf += addrsize;
        ipipsec_pfkeyv2_ipaddr_to_extaddr(sadb->srcmax_flow,
                                          &flow->flowsel.src_max,
                                          flow->flowsel.src_max_port,
                                          flow->flowsel.key.domain);
        sadb->srcmax_flow->sadb_address_exttype = SADB_X_EXT_SRCMAX_FLOW;
    }

    /* Optional SADB_X_EXT_DSTMAX_FLOW */
    if (ip_ntohs(flow->flowsel.key.ports.udptcp_srcdst_n[1])
        != flow->flowsel.dst_max_port
        || ipcom_memcmp(&flow->flowsel.key.dst, &flow->flowsel.dst_max,
                        flow->flowsel.key.domain == IP_AF_INET ? 4 : 16))
    {
        sadb->dstmax_flow = (struct sadb_address *)msgbuf;
        msgbuf += addrsize;
        ipipsec_pfkeyv2_ipaddr_to_extaddr(sadb->dstmax_flow,
                                          &flow->flowsel.dst_max,
                                          flow->flowsel.dst_max_port,
                                          flow->flowsel.key.domain);
        sadb->dstmax_flow->sadb_address_exttype = SADB_X_EXT_DSTMAX_FLOW;
    }

    /* Send the message */
    retval = ipipsec_pfkeyv2_output(sock, (void *)sadb, IPIPSEC_SNDMODE_UNICAST, 0);

    ipcom_free(orgbuf);

    return retval;
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_msg_sadb_dump
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_msg_sadb_dump(Ipcom_socket *sock, Ipipsec_pfkeyv2_sadb *sadb)
{
    Ipipsec_sa   *sa, *last;
    int     retval;
    int     sadir, domain_index;
    Ip_u8   satype;

    ip_assert(sadb->msg);
    ip_assert(sadb->msg->sadb_msg_type == SADB_DUMP);

    satype = sadb->msg->sadb_msg_satype;

    last = IP_NULL;
    for (domain_index = 0; domain_index < 2; domain_index++)
        for (sadir = 0; sadir < 2; sadir++)
            for(sa = IPCOM_LIST_FIRST(&ipipsec.sa_head[domain_index][sadir]);
                sa != IP_NULL;
                sa = IPCOM_LIST_NEXT(&sa->sa_list))
                if (satype == sa->sadb_msg_satype || satype == SADB_SATYPE_UNSPEC)
                {
                    if (last)
                    {
                        retval = ipipsec_pfkeyv2_output_sa(sock, last, sadb, IPIPSEC_SNDMODE_UNICAST);
                        if (retval != 0)
                            return retval;
                    }
                    last = sa;
                }

    /* Write the last one with seq field of zero */
    if (last)
    {
        sadb->msg->sadb_msg_seq = 0;
        retval = ipipsec_pfkeyv2_output_sa(sock, last, sadb, IPIPSEC_SNDMODE_UNICAST);
        if (retval != 0)
            return retval;
    }
    else
        return -IP_ERRNO_ESRCH;

    return 0;
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_output
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_output(Ipcom_socket *sock, void *exthdr[SADB_EXT_MAX+1], Ip_u32 sndmode, Ip_u32 saerrno)
{
    int    i;
    Ip_size_t  length;
    Ipipsec_pfkeyv2_sadb *sadb;
    Ipcom_pkt  *pkt;
    Ip_u32   extmap;
    Ipipsec_eio_output  eio_output;

    sadb = (Ipipsec_pfkeyv2_sadb *)exthdr;
    ip_assert(sadb);
    ip_assert(sadb->msg);
    ip_assert(saerrno <= 255);

    if (saerrno != 0)
    {
        /* Pass errno in message, clear all extensions */
        ipcom_memset(&exthdr[1], 0, SADB_EXT_MAX * sizeof(void*));
    }
    else
    {
        /* Only send extensions that are ok */
        for (i = 1, extmap = 0; i <= SADB_EXT_MAX; i++)
            if (exthdr[i])
            {
                if (IP_BIT_ISSET(ipipsec_pfkeyv2_sadbextbits[sadb->msg->sadb_msg_type].out_opt, (1 << i)))
                    extmap |= (1 << i);
                else
                {
                    exthdr[i] = IP_NULL;
                }
            }

        /* Check if the required OUT extensions are there */
        if ((extmap & ipipsec_pfkeyv2_sadbextbits[sadb->msg->sadb_msg_type].out_req) !=
           ipipsec_pfkeyv2_sadbextbits[sadb->msg->sadb_msg_type].out_req)
        {
            IPCOM_LOG3(ERR, "ipipsec_pfkeyv2_output() :: missing mandatory extension (msgtype=%d  sndmap=0x%lx  reqmap=0x%lx",
                       sadb->msg->sadb_msg_type,
                       extmap,
                       ipipsec_pfkeyv2_sadbextbits[sadb->msg->sadb_msg_type].out_req);

            /* Pass internal errno in message, clear all extensions */
            saerrno = IP_ERRNO_EINVAL;
            ipcom_memset(&exthdr[1], 0, SADB_EXT_MAX * sizeof(void*));
        }
    }

    for (length = sizeof(struct sadb_msg), i = 1; i <= SADB_EXT_MAX; i++)
        if (exthdr[i])
            length += (((struct sadb_ext *)exthdr[i])->sadb_ext_len * sizeof(Ip_u64));

    /* Allocate packet */
    pkt = ipcom_pkt_malloc(length + 7, 0);
    if (pkt == IP_NULL)
        return -IP_ERRNO_ENOMEM;
    pkt->start   = (int)((Ip_ptrdiff_t)pkt->data & 0x7);
    pkt->ipstart = pkt->start;
    pkt->end     = pkt->start;

    /* SADB_MSG */
    ip_assert(sadb->msg->sadb_msg_version == PF_KEY_V2);
    sadb->msg->sadb_msg_len = (Ip_u16)length / sizeof(Ip_u64);
    sadb->msg->sadb_msg_errno = (Ip_u8)saerrno;

    /* verify */
    ip_assert(sadb->msg->sadb_msg_errno < 100);  /* detect unsigned wrap */
    ip_assert(exthdr[SADB_EXT_KEY_AUTH] == IP_NULL);
    ip_assert(exthdr[SADB_EXT_KEY_ENCRYPT] == IP_NULL);

    /* Copy over all headers */
    ipcom_memcpy(&pkt->data[pkt->end], sadb->msg, sizeof(struct sadb_msg));
    pkt->end += sizeof(struct sadb_msg);
    for (i = 1; i <= SADB_EXT_MAX; i++)
        if (exthdr[i] != IP_NULL)
        {
            ipcom_memcpy(&pkt->data[pkt->end], exthdr[i],
                         ((struct sadb_ext *)exthdr[i])->sadb_ext_len * sizeof(Ip_u64));
            pkt->end += ((struct sadb_ext *)exthdr[i])->sadb_ext_len * sizeof(Ip_u64);
        }

    /* Output packet to key socket */
    eio_output.sndmode = sndmode;
    eio_output.sock = sock;
    eio_output.pkt = pkt;
    eio_output.sadb_msg_type = (Ip_u16)sadb->msg->sadb_msg_type;
    eio_output.sadb_msg_satype = (Ip_u16)sadb->msg->sadb_msg_satype;
    (void)ipcom_ipsec_ioevent(IPIPSEC_EIOXOUTPUT, &eio_output);

    return 0;
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_msg_sadb_flush
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_msg_sadb_flush(Ipipsec_pfkeyv2_sadb *sadb)
{
    Ipipsec_sa   *sa, *next;
    Ipipsec_flow *flow;
    int      direction, domain_index;
    Ip_u8    satype;

    ip_assert(sadb->msg);
    ip_assert(sadb->msg->sadb_msg_type == SADB_FLUSH);

    satype = sadb->msg->sadb_msg_satype;

    switch (satype)
    {
    case SADB_X_SATYPE_BYPASS:
        /* Remove all flows from the bypass SA */
        ipipsec_sa_free(&ipipsec.sa_bypass, IP_FALSE, IP_TRUE);
        return 0;

    default:
        for (domain_index = 0; domain_index < 2; domain_index++)
            for (direction = 0; direction < 2; direction++)
            {
                /* Remove all SA's */
                for(sa = IPCOM_LIST_FIRST(&ipipsec.sa_head[domain_index][direction]);
                    sa != IP_NULL;
                    sa = next)
                {
                    next = IPCOM_LIST_NEXT(&sa->sa_list);
                    if (satype == sa->sadb_msg_satype || satype == SADB_SATYPE_UNSPEC)
                    {
                        ipipsec_sa_free(sa, IP_FALSE, IP_TRUE);
                    }
                }

                /* Remove deny,acquire,require flows */
                while ((flow = IPCOM_LIST_FIRST(&ipipsec.flow_head[domain_index][direction])) != IP_NULL)
                    ipipsec_flow_free(flow);
            }
        break;
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_sadb_to_sadir
 *===========================================================================
 * Description:   Get flow direction from key message
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_sadb_to_sadir(Ipipsec_pfkeyv2_sadb *sadb, int default_sadir)
{
    int sadir;

    if (sadb->sa && IP_BIT_ISSET(sadb->sa->sadb_sa_flags, SADB_X_SAFLAGS_INGRESS_SA))
        sadir = IPIPSEC_DIR_IN;
    else if (sadb->sa && IP_BIT_ISSET(sadb->sa->sadb_sa_flags, SADB_X_SAFLAGS_EGRESS_SA))
        sadir = IPIPSEC_DIR_OUT;
    else if (sadb->sa2 && IP_BIT_ISSET(sadb->sa2->sadb_sa_flags, SADB_X_SAFLAGS_INGRESS_SA))
        sadir = IPIPSEC_DIR_IN;
    else if (sadb->sa2 && IP_BIT_ISSET(sadb->sa2->sadb_sa_flags, SADB_X_SAFLAGS_EGRESS_SA))
        sadir = IPIPSEC_DIR_OUT;
    else
        sadir = default_sadir;

    if (sadir != IPIPSEC_DIR_IN && sadir != IPIPSEC_DIR_OUT)
        return -IP_ERRNO_EINVAL;

    return sadir;
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_sadb_to_sa
 *===========================================================================
 * Description:  Retrieve information from KEY structure to SA
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_sadb_to_sa(Ipipsec_pfkeyv2_sadb *sadb, Ipipsec_sa *sa, Ip_u8 msgtype)
{
    int   retval;
    struct Ip_sockaddr   *sockaddr;

    ip_assert(sa->key.domain == IP_AF_INET || sa->key.domain == IP_AF_INET6);
    ip_assert(sa->key.direction == IPIPSEC_DIR_IN || sa->key.direction == IPIPSEC_DIR_OUT);


    /***** ADD or UPDATE LARVAL *****/
    if (msgtype == SADB_ADD || msgtype == SADB_UPDATE)
    {
        Ip_u32   bytes;

        /* SA options */
        if (sadb->saopt != IP_NULL)
            sa->key.priority = sadb->saopt->sadb_saopt_priority;

        /* SADB_EXT_SA */
        ip_assert(sadb->sa != IP_NULL);
        if (IP_BIT_ISFALSE(sadb->sa->sadb_sa_flags, SADB_X_SAFLAGS_NOREPLAY))
            sa->sadb_sa_replay = IP_ROUNDUP(sadb->sa->sadb_sa_replay, 8);
        sa->sadb_sa_state = SADB_SASTATE_MATURE;
        sa->sadb_sa_flags = sadb->sa->sadb_sa_flags;
        if ((sadb->msg->sadb_msg_satype == SADB_SATYPE_ESP
             || sadb->msg->sadb_msg_satype == SADB_SATYPE_AH)
            && ipcom_sysvar_get_as_int("ipipsec.pfkey.esn_is_default", 1) != 0)
            IP_BIT_SET(sa->sadb_sa_flags, SADB_X_SAFLAGS_ESN);

        /* Bitmap allocation only on ADD of new SA */
        if (msgtype == SADB_ADD)
        {
#ifdef IPIPSEC_USE_BITARRAY
            /* Create replay bitmap if required */
            if (sa->sadb_sa_replay > 32)
            {
                if (IP_BIT_ISFALSE(sa->sadb_sa_flags, SADB_X_SAFLAGS_ESN))
                    return -IP_ERRNO_EINVAL;
                ip_assert(sa->replay_bitarray == IP_NULL);
                sa->replay_bitarray = ipcom_bitarray_new(sa->sadb_sa_replay / 8);
                if (sa->replay_bitarray == IP_NULL)
                {
                    IPCOM_LOG1(ERR,
                               "ipipsec_pfkeyv2_sadb_to_sa() :: out of memory for SA bitarray, size (%d)",
                               sa->sadb_sa_replay);
                    return -IP_ERRNO_ENOMEM;
                }
            }
#else
            if (sa->sadb_sa_replay > 32)
                return -IP_ERRNO_EINVAL;
#endif /* IPIPSEC_USE_BITARRAY */
        }

        /*** ESP/AH/IPIP specifics ***/
        switch(sadb->msg->sadb_msg_satype)
        {
            /* ESP - Encryption/Authentication */
#ifdef IPIPSEC_USE_ESP
            case SADB_SATYPE_ESP:
                /* Do not accept both NULL auth and NULL encrypt.
                 * RFC4301 s4.2p17: "A compliant implementation MUST NOT allow instantiation of an ESP SA that employs both NULL encryption and no integrity algorithm." */
                ip_assert(sadb->sa != IP_NULL);
                if (sadb->sa->sadb_sa_auth == SADB_AALG_NONE
                    && sadb->sa->sadb_sa_encrypt == SADB_EALG_NONE)
                    return -IP_ERRNO_EINVAL;

                sa->sadb_sa_auth = sadb->sa->sadb_sa_auth;
                sa->sadb_sa_encrypt = sadb->sa->sadb_sa_encrypt;

                sa->header_size = 4 /* spi field */;
                if (IP_BIT_ISFALSE(sa->sadb_sa_flags, SADB_X_SAFLAGS_NOREPLAY))
                    sa->header_size += 4;   /* seq mandatory for 'NEW' ESP */

                /* ESP header/trailer lengths */
                sa->header_space = sa->header_size;
                sa->trailer_space = IPIPSEC_HMAC_AUTHLEN + 2;   /* length & protocol field */
                if (sadb->sa->sadb_sa_encrypt == SADB_EALG_NONE)
                    sa->trailer_space += 4;   /* ESP null cipher can get padded upto 4 bytes */
                break;
#endif /* IPIPSEC_USE_ESP */

            /* AH - Authentication */
#ifdef IPIPSEC_USE_AH
            case SADB_SATYPE_AH:
                /* Do not accept a NULL AH algorithm */
                ip_assert(sadb->sa != IP_NULL);
                if (sadb->sa->sadb_sa_auth == SADB_AALG_NONE)
                    return -IP_ERRNO_EINVAL;

                sa->sadb_sa_auth = sadb->sa->sadb_sa_auth;
                ip_assert(sa->sadb_sa_encrypt == SADB_EALG_NONE);

                /* AH header length */
                sa->header_size = 8; /* [NH,payload len,reserved] + SPI */
                if (IP_BIT_ISFALSE(sa->sadb_sa_flags, SADB_X_SAFLAGS_NOREPLAY))
                    sa->header_size += 4;   /* seq mandatory for 'NEW' AH */

                /* Space for the digest/checksum in AH header is added in ipipsec_auth_setkey() */
                sa->header_space = sa->header_size;
                break;
#endif /* IPIPSEC_USE_AH */

            /* IP in IP encapsulation */
#ifdef IPIPSEC_USE_IPIP
            case SADB_X_SATYPE_IPIP:
                /* Do not accept auth or encrypt */
                if (sadb->sa->sadb_sa_auth != SADB_AALG_NONE
                    || sadb->sa->sadb_sa_encrypt != SADB_EALG_NONE)
                    return -IP_ERRNO_EINVAL;

                ip_assert(sa->sadb_sa_auth == SADB_AALG_NONE);
                ip_assert(sa->sadb_sa_encrypt == SADB_EALG_NONE);

                /* Force tunnel mode */
                IP_BIT_SET(sa->sadb_sa_flags, SADB_X_SAFLAGS_TUNNEL);
                break;
#endif

            default:
                IPCOM_LOG1(ERR, "ipipsec_pfkeyv2_sadb_to_sa() :: unknown satype (%d)",
                           sadb->msg->sadb_msg_satype);
                IP_PANIC();
                break;
        }

        /* SADB_EXT_KEY_AUTH */
        ip_assert(sadb->sa != IP_NULL);
        if (sadb->sa->sadb_sa_auth != SADB_AALG_NONE)
        {
            if (sadb->key_auth == IP_NULL)
                return -IP_ERRNO_EINVAL;

            bytes = sadb->key_auth->sadb_key_bits >> 3;
            if ((bytes + sizeof(struct sadb_key)) > (sadb->key_auth->sadb_key_len * sizeof(Ip_u64)))
                return -IP_ERRNO_EINVAL;

            /* Set the key */
            retval = ipipsec_auth_setkey(sa, sadb->sa->sadb_sa_auth,
                (Ip_u8*)sadb->key_auth + sizeof(struct sadb_key), bytes);
            if (retval != 0)
                return retval;

#if defined(IPHWCRYPTO) || defined(IPCOM_USE_FORWARDER_IPSEC)
            /* Hardware crypto. Note, we must store raw key */
            ipcom_memcpy(sa->esp_authkey,
                         (Ip_u8 *)sadb->key_auth + sizeof(struct sadb_key),
                         IP_MIN(sadb->key_auth->sadb_key_bits >> 3, sizeof(sa->esp_authkey)));
#endif
            /* For AH, add header space for the digest/checksum */
#ifdef IPIPSEC_USE_AH
            if (sadb->msg->sadb_msg_satype == SADB_SATYPE_AH)
            {
                ip_assert(sa->auth);
                sa->header_space += sa->auth->alg->auth_bytes;
            }
#endif
        }

#ifdef IPIPSEC_USE_ESP
        /* SADB_EXT_KEY_ENCRYPT */
        ip_assert(sadb->sa != IP_NULL);
        if (sadb->sa->sadb_sa_encrypt != SADB_EALG_NONE)
        {
            if (sadb->key_encrypt == IP_NULL)
                return -IP_ERRNO_EINVAL;
            if (sadb->msg->sadb_msg_satype != SADB_SATYPE_ESP)
                return -IP_ERRNO_EINVAL;

            /* Check key length */
            bytes = sadb->key_encrypt->sadb_key_bits >> 3;
            if ((bytes + sizeof(struct sadb_key)) > (sadb->key_encrypt->sadb_key_len * sizeof(Ip_u64)))
                return -IP_ERRNO_EINVAL;

            /* Set the key */
            retval = ipipsec_encrypt_setkey(sa, sadb->sa->sadb_sa_encrypt,
                (Ip_u8 *)sadb->key_encrypt + sizeof(struct sadb_key), bytes);
            if (retval != 0)
                return retval;

#if defined(IPHWCRYPTO) || defined(IPCOM_USE_FORWARDER_IPSEC)
            /* Hardware crypto. Note, we must store raw key */
            ipcom_memcpy(sa->esp_enckey,
                        (Ip_u8 *)sadb->key_encrypt + sizeof(struct sadb_key),
                        IP_MIN(sadb->key_encrypt->sadb_key_bits >> 3, sizeof(sa->esp_enckey)));
#endif
        }

#ifdef IPHWCRYPTO
        sa->esp_input  = iphwcrypto_ipsec_esp_input;
        sa->esp_output = iphwcrypto_ipsec_esp_output;
#endif
#endif    /* #ifdef IPIPSEC_USE_ESP */
    }


    /* Lifetime. Not for GETSPI */
    if (msgtype != SADB_GETSPI)
    {
        /* SADB_EXT_LIFETIME_HARD */
        if (sadb->lifetime_hard)
        {
            sa->lthard.allocations = sadb->lifetime_hard->sadb_lifetime_allocations;
            ipcom_memcpy(&sa->lthard.bytes,
                &sadb->lifetime_hard->sadb_lifetime_bytes, sizeof(Ip_u64));
            sa->lthard.addtime = IP_U64_GETLO(sadb->lifetime_hard->sadb_lifetime_addtime);
            sa->lthard.usetime = IP_U64_GETLO(sadb->lifetime_hard->sadb_lifetime_usetime);
        }

        /* SADB_EXT_LIFETIME_SOFT */
        if (sadb->lifetime_soft)
        {
            sa->ltsoft.allocations = sadb->lifetime_soft->sadb_lifetime_allocations;
            ipcom_memcpy(&sa->ltsoft.bytes,
                &sadb->lifetime_soft->sadb_lifetime_bytes, sizeof(Ip_u64));
            sa->ltsoft.addtime = IP_U64_GETLO(sadb->lifetime_soft->sadb_lifetime_addtime);
            sa->ltsoft.usetime = IP_U64_GETLO(sadb->lifetime_soft->sadb_lifetime_usetime);
        }

        /* Reset SA timeout if lifetime changed */
        if (sadb->lifetime_hard || sadb->lifetime_soft)
            ipipsec_sa_reset_timeout(sa);
    }


    /* Addresses. Only for GETSPI, ADD and UPDATE LARVAL */
    if (msgtype != SADB_MAX)
    {
        /* SADB_MSG */
        sa->sadb_msg_satype = sadb->msg->sadb_msg_satype;
        sa->key.proto = IPIPSEC_SADB_GET_PROTO(sadb->msg->sadb_msg_satype);

        /* SADB_EXT_ADDRESS_DST */
        if (sadb->address_dst)
        {
            sockaddr = (struct Ip_sockaddr *)((Ip_u8 *)sadb->address_dst + sizeof(struct sadb_address));
            if (sockaddr->sa_family != sa->key.domain)
                return -IP_ERRNO_EINVAL;
            ipcom_memcpy(&sa->key.dst, &sockaddr->sa_data[sa->sock_in_addr_offset], sa->in_addr_size);
        }

        /* SADB_EXT_ADDRESS_SRC */
        if (sadb->address_src)
        {
            sockaddr = (struct Ip_sockaddr *)((Ip_u8 *)sadb->address_src + sizeof(struct sadb_address));
            if (sockaddr->sa_family != sa->key.domain)
                return -IP_ERRNO_EINVAL;
            ipcom_memcpy(&sa->key.src, &sockaddr->sa_data[sa->sock_in_addr_offset], sa->in_addr_size);
        }

        /* SADB_EXT_ADDRESS_PROXY */
        if (sadb->address_proxy)
        {
            sockaddr = (struct Ip_sockaddr *)((Ip_u8 *)sadb->address_proxy + sizeof(struct sadb_address));
            if (sockaddr->sa_family != sa->key.domain)
                return -IP_ERRNO_EINVAL;
            ipcom_memcpy(&sa->proxy, &sockaddr->sa_data[sa->sock_in_addr_offset], sa->in_addr_size);
        }

        /* SADB_X_EXT_UDPENCAP */
        if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_UDPENCAP))
        {
            Ip_u16  port;

            if (sadb->udpencap)
                port = sadb->udpencap->sadb_x_udpencap_port;
            else
                port = ip_htons(4500);
            sa->encap_srcport_n = port;
            sa->encap_dstport_n = port;
        }
    }


    /* Counters. Only for ADD and UPDATE LARVAL */
    if (msgtype == SADB_ADD || msgtype == SADB_UPDATE)
    {
        if (sa->key.direction == IPIPSEC_DIR_IN)
        {
#ifdef IPIPSEC_USE_MIB
            /* MIB counters */
#ifdef IPIPSEC_USE_ESP
            if (sa->sadb_msg_satype == SADB_SATYPE_ESP)
            {
                ipipsec.stats.ipsecEspCurrentInboundSAs++;
                ipipsec.stats.ipsecEspTotalInboundSAs++;
            }
#endif
#ifdef IPIPSEC_USE_AH
            if (sa->sadb_msg_satype == SADB_SATYPE_AH)
            {
                ipipsec.stats.ipsecAhCurrentInboundSAs++;
                ipipsec.stats.ipsecAhTotalInboundSAs++;
            }
#endif
#endif /* IPIPSEC_USE_MIB */
        }

        if (sa->key.direction == IPIPSEC_DIR_OUT)
        {
#ifdef IPIPSEC_USE_MIB
#ifdef IPIPSEC_USE_ESP
            if (sa->sadb_msg_satype == SADB_SATYPE_ESP)
            {
                ipipsec.stats.ipsecEspCurrentOutboundSAs++;
                ipipsec.stats.ipsecEspTotalOutboundSAs++;
            }
#endif
#ifdef IPIPSEC_USE_AH
            if (sa->sadb_msg_satype == SADB_SATYPE_AH)
            {
                ipipsec.stats.ipsecAhCurrentOutboundSAs++;
                ipipsec.stats.ipsecAhTotalOutboundSAs++;
            }
#endif

#endif /* IPIPSEC_USE_MIB */

            /*!!FIXME: bad place */
#ifdef IPIPSEC_USE_TFC
            if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_TFC)
                && msgtype == SADB_ADD
                && sa->sadb_msg_satype == SADB_SATYPE_ESP)
            {
                ipipsec_tfc_reset_timeout(sa);
            }
#endif /* IPIPSEC_USE_TFC */
        }
    }

    /* Insert in ordered global list */
    ipipsec_sa_insert(sa);

    /* Success */
    return 0;
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_msg_sadb_add
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_msg_sadb_add(Ipipsec_pfkeyv2_sadb *sadb)
{
    Ipipsec_sa      *sa;
    struct Ip_sockaddr   *sockaddr;
    int   retval;
    int   sadir;

    ip_assert(sadb->msg);
    ip_assert(sadb->sa);
    ip_assert(sadb->address_src);
    ip_assert(sadb->address_dst);

    /* [3.1.3] Only accept mature SA's */
    if (sadb->sa->sadb_sa_state != SADB_SASTATE_MATURE)
        return -IP_ERRNO_EINVAL;

    /* Do not allow SPI 0 since it is reserved and used by bypass SA */
    if (sadb->sa->sadb_sa_spi == IPIPSEC_SPI_LOCAL_USE)
        return -IP_ERRNO_EINVAL;

    /* Can only add ESP, AH or IPIP SA's */
    if (ipipsec_pfkeyv2_verify_satype(sadb->msg->sadb_msg_satype) != 0)
        return -IP_ERRNO_EOPNOTSUPP;

    /* Check if there is an identical SA */
    sa = ipipsec_pfkeyv2_sadb_sa_lookup(sadb, IPIPSEC_DIR_OUT, IP_TRUE);
    if (sa != IP_NULL)
    {
        IPCOM_LOG1(ERR, "ipipsec_pfkeyv2_msg_sadb_add() :: SA exist already (spi=%d)",
            sadb->sa->sadb_sa_spi);
        return -IP_ERRNO_EEXIST;
    }

    /* Get SA direction */
    sadir = ipipsec_pfkeyv2_sadb_to_sadir(sadb, IPIPSEC_DIR_OUT);
    if (sadir < 0)
        return sadir;

    /* Create and initialize a new SA */
    sockaddr = (struct Ip_sockaddr *)((Ip_u8*)sadb->address_dst + sizeof(struct sadb_address));
    sa = ipipsec_sa_malloc(sadb->sa->sadb_sa_spi, sadir, sockaddr->sa_family);
    if (sa == IP_NULL)
        return -IP_ERRNO_ENOMEM;

    /* Add the info from the PFKEY to the SA */
    retval = ipipsec_pfkeyv2_sadb_to_sa(sadb, sa, SADB_ADD);
    if (retval != 0)
    {
        ipipsec_sa_free(sa, IP_FALSE, IP_FALSE);
    }
    else
    {
        IPCOM_LOG2(INFO, "Adding SA: %s, replay=%d",
                   ipipsec_sa_name(sa),
                   (int)sa->sadb_sa_replay);
    }

    return retval;
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_msg_sadb_update
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_msg_sadb_update(Ipipsec_pfkeyv2_sadb *sadb)
{
    Ipipsec_sa      *sa;
    int   retval;

    ip_assert(sadb->msg);
    ip_assert(sadb->sa);
    ip_assert(sadb->address_dst);

    /* [3.1.2] Can only update if state of new SA is mature */
    if (sadb->sa->sadb_sa_state != SADB_SASTATE_MATURE)
        return -IP_ERRNO_EINVAL;

    /* Do not allow SPI 0 since it is reserved and used by bypass SA */
    if (sadb->sa->sadb_sa_spi == IPIPSEC_SPI_LOCAL_USE)
        return -IP_ERRNO_EINVAL;

    /* Check if there is a SA to update */
    sa = ipipsec_pfkeyv2_sadb_sa_lookup(sadb, IPIPSEC_DIR_OUT, IP_TRUE);
    if (sa == IP_NULL)
        return -IP_ERRNO_ESRCH;


    /* Check what state the SA is in */
    switch(sa->sadb_sa_state)
    {
    case SADB_SASTATE_DEAD:
        /* Can't update DEAD SA, must return EINVAL */
        return -IP_ERRNO_EINVAL;

        /* LARVAL SA, can update everything */
    case SADB_SASTATE_LARVAL:
        /* Can only update ESP, AH or IPIP SA's */
        if (ipipsec_pfkeyv2_verify_satype(sadb->msg->sadb_msg_satype) != 0)
            return -IP_ERRNO_EOPNOTSUPP;

        /* Add the info from the SA to the SA */
        retval = ipipsec_pfkeyv2_sadb_to_sa(sadb, sa, SADB_UPDATE);
        if (retval != 0)
        {
            /* remain in larval state so we can update the SA again */
            sa->sadb_sa_state = SADB_SASTATE_LARVAL;
            return retval;
        }

        IPCOM_LOG1(INFO, "Updating larval SA: %s", ipipsec_sa_name(sa));

        /* Cancel any pending getspi timeout */
        sa->larval_timeout = 0;
        ipipsec_sa_reset_timeout(sa);
        break;

        /* SADB_SASTATE_MATURE or SADB_SASTATE_DYING */
    default:
        ip_assert(sa->sadb_sa_state == SADB_SASTATE_MATURE
                  || sa->sadb_sa_state == SADB_SASTATE_DYING);

        /* Not allowed to change keys, identify, sensitivity or proxy */
        if (sadb->ident_src || sadb->ident_dst
           || sadb->sens
           || sadb->address_proxy
           || sadb->key_auth || sadb->key_encrypt)
        {
            return -IP_ERRNO_EINVAL;
        }

        IPCOM_LOG1(INFO, "Updating SA: %s", ipipsec_sa_name(sa));

        /* Only ok to update state and lifetime */
        retval = ipipsec_pfkeyv2_sadb_to_sa(sadb, sa, SADB_MAX /* clutch, use max */);
        if (retval == 0)
            sa->sadb_sa_state = sadb->sa->sadb_sa_state;

        break;
    }

    return retval;
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_msg_sadb_getspi
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_msg_sadb_getspi(Ipipsec_pfkeyv2_sadb *sadb, struct sadb_sa *samem)
{
    Ipipsec_sa     *sa;
    Ipipsec_key key;
    Ip_u32 spi_min;
    int   retval;
    int   seconds;

    ip_assert(sadb->msg);
    ip_assert(sadb->spirange);
    ip_assert(sadb->address_dst);

    if (ipipsec_pfkeyv2_verify_satype(sadb->msg->sadb_msg_satype) != 0)
        return -IP_ERRNO_EOPNOTSUPP;

    retval = ipipsec_pfkeyv2_sadb_to_key(sadb, &key, IPIPSEC_DIR_OUT);
    if (retval != 0)
        return retval;

    /* Get SPI */
    for (spi_min = IP_MAX(sadb->spirange->sadb_spirange_min, IPIPSEC_SPI_RESERVED_MAX+1);
         spi_min <= sadb->spirange->sadb_spirange_max;
         spi_min++)
    {
        key.ports.espah_spi_n = ip_htonl(spi_min);
        sa = ipipsec_sa_lookup(&key, 0);
        if (sa == IP_NULL)
            break;
    }

    if (spi_min > sadb->spirange->sadb_spirange_max)
        return -IP_ERRNO_EEXIST;

    /* Create and init a new SA */
    sa = ipipsec_sa_malloc(ip_htonl(spi_min), key.direction, key.domain);
    if (sa == IP_NULL)
        return -IP_ERRNO_ENOMEM;

    /* Set source and destination address + satype */
    retval = ipipsec_pfkeyv2_sadb_to_sa(sadb, sa, SADB_GETSPI);
    if (retval != 0)
    {
        ipipsec_sa_free(sa, IP_FALSE, IP_FALSE);
    }
    else
    {
        IPCOM_LOG1(DEBUG, "<GETSPI>: SA: %s", ipipsec_sa_name(sa));

        /* Set larval timeout to auto-clean unused SAs */
        seconds = ipcom_sysvar_get_as_int("ipipsec.larval_sa_tmo_seconds", 600);
        seconds = IP_MAX(1, seconds);
        sa->larval_timeout = ipipsec_seconds_now() + (Ip_time_t)seconds;
        ipipsec_sa_reset_timeout(sa);

        /* Add a SADB_EXT_SA header to the reply */
        if (sadb->sa == IP_NULL)
            sadb->sa = samem;
        sadb->sa->sadb_sa_len     = sizeof(struct sadb_sa) / sizeof(Ip_u64);
        sadb->sa->sadb_sa_exttype = SADB_EXT_SA;
        sadb->sa->sadb_sa_spi     = sa->key.ports.espah_spi_n;  /* [2.3.1] in network endian */
        sadb->sa->sadb_sa_replay  = sa->sadb_sa_replay;
        sadb->sa->sadb_sa_state   = sa->sadb_sa_state;
        sadb->sa->sadb_sa_auth    = sa->sadb_sa_auth;
        sadb->sa->sadb_sa_encrypt = sa->sadb_sa_encrypt;
        sadb->sa->sadb_sa_flags   = sa->sadb_sa_flags;
    }

    return retval;
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_msg_sadb_delete
 *===========================================================================
 * Description:   Delete a SA (and optionally entire group for linked SAs)
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_msg_sadb_delete(Ipipsec_pfkeyv2_sadb *sadb)
{
    Ipipsec_sa   *sa;

    ip_assert(sadb->msg);
    ip_assert(sadb->sa);
    ip_assert(sadb->address_dst);

    /* Allow SPI 0 because that is one way to flush the bypass flows */

    /* Lookup the SA (exact match) */
    sa = ipipsec_pfkeyv2_sadb_sa_lookup(sadb, IPIPSEC_DIR_OUT, IP_TRUE);
    if (sa == IP_NULL)
        return -IP_ERRNO_ESRCH;

    /* Delete SA */
    ipipsec_sa_free(sa, IP_BIT_ISSET(sadb->sa->sadb_sa_flags, SADB_X_SAFLAGS_CHAINDEL) ? IP_TRUE : IP_FALSE, IP_TRUE);

    return 0;
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_msg_sadb_get
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_msg_sadb_get(Ipcom_socket *sock, Ipipsec_pfkeyv2_sadb *sadb)
{
    Ipipsec_sa      *sa;

    ip_assert(sadb->msg);
    ip_assert(sadb->msg->sadb_msg_type == SADB_GET);
    ip_assert(sadb->sa);

    /* Lookup the SA (note: exact lookup) */
    sa = ipipsec_pfkeyv2_sadb_sa_lookup(sadb, IPIPSEC_DIR_OUT, IP_TRUE);
    if (sa == IP_NULL)
        return -IP_ERRNO_ESRCH;

    /* [3.1.5] Return SADB_GET message to the socket that sent the message */
    return ipipsec_pfkeyv2_output_sa(sock, sa, sadb, IPIPSEC_SNDMODE_UNICAST);
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_sadb_to_flowsel
 *===========================================================================
 * Description:  Used for flow commands to extract info from sadb to flowsel
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_sadb_to_flowsel(Ipipsec_pfkeyv2_sadb *sadb, Ipipsec_flowsel *flowsel)
{
    union Ip_sockaddr_union  *sockaddr_src;
    union Ip_sockaddr_union  *sockaddr_dst;
    union Ip_sockaddr_union  *sockaddr;

    ip_assert(sadb->src_flow);
    ip_assert(sadb->dst_flow);

    sockaddr_src = (union Ip_sockaddr_union *)((Ip_u8 *)sadb->src_flow + sizeof(struct sadb_address));
    sockaddr_dst = (union Ip_sockaddr_union *)((Ip_u8 *)sadb->dst_flow + sizeof(struct sadb_address));

    if (sockaddr_src->sa.sa_family != sockaddr_dst->sa.sa_family)
        return -IP_ERRNO_EINVAL;
    if (sockaddr_dst->sa.sa_family != IP_AF_INET
        && sockaddr_dst->sa.sa_family != IP_AF_INET6)
        return -IP_ERRNO_EINVAL;

    ipcom_memset(flowsel, 0, sizeof(Ipipsec_flowsel));

    /* Domain */
    flowsel->key.domain = (Ip_u8)sockaddr_dst->sa.sa_family;

    /* Flow options in SADB_X_EXT_FLOW_OPTIONS: */
    if (sadb->flowopt)
    {
        /* Flow priority (-128 to 127, -128=highest) */
        flowsel->key.priority = sadb->flowopt->sadb_flowopt_priority;
        /* DSCP value (in high order 6-bits */
        flowsel->key.dscp     = sadb->flowopt->sadb_flowopt_dscp;
    }

    /* Optional transport protocol in SADB_X_EXT_PROTOCOL, e.g. UDP or TCP */
    if (sadb->protocol)
    {
        flowsel->key.proto = sadb->protocol->sadb_protocol_proto;

        switch (flowsel->key.proto)
        {
        case IP_IPPROTO_TCP:
        case IP_IPPROTO_UDP:
            /* Source TCP/UDP ports (at same offset for both IPv4 and IPv6) */
            flowsel->key.ports.udptcp_srcdst_n[0] = sockaddr_src->sin.sin_port;
            flowsel->src_max_port                 = ip_ntohs(sockaddr_src->sin.sin_port);
            if (sadb->srcmax_flow != IP_NULL)
            {
                sockaddr = (union Ip_sockaddr_union *)((Ip_u8*)sadb->srcmax_flow + sizeof(struct sadb_address));
                flowsel->src_max_port = ip_ntohs(sockaddr->sin.sin_port);
                if (flowsel->src_max_port < ip_ntohs(flowsel->key.ports.udptcp_srcdst_n[0]))
                    return -IP_ERRNO_EINVAL;
            }

            /* Destination TCP/UDP ports (at same offset for both IPv4 and IPv6) */
            flowsel->key.ports.udptcp_srcdst_n[1] = sockaddr_dst->sin.sin_port;
            flowsel->dst_max_port                 = ip_ntohs(sockaddr_dst->sin.sin_port);
            if (sadb->dstmax_flow != IP_NULL)
            {
                sockaddr = (union Ip_sockaddr_union *)((Ip_u8*)sadb->dstmax_flow + sizeof(struct sadb_address));
                flowsel->dst_max_port = ip_ntohs(sockaddr->sin.sin_port);
                if (flowsel->dst_max_port < ip_ntohs(flowsel->key.ports.udptcp_srcdst_n[1]))
                    return -IP_ERRNO_EINVAL;
            }
            break;

        case IP_IPPROTO_ICMP:
        case IP_IPPROTO_ICMPV6:
            flowsel->key.ports.icmp_typecode[0] = sadb->protocol->sadb_protocol_ports0;
            flowsel->key.ports.icmp_typecode[1] = sadb->protocol->sadb_protocol_ports1;
            break;

        case IP_IPPROTO_MH:
            flowsel->key.ports.mh_type[0] = sadb->protocol->sadb_protocol_ports0;
            break;

        default:
            break;
        }
    }

    /* Source and destination addresses */
#ifdef IPCOM_USE_INET
    if (flowsel->key.domain == IP_AF_INET)
    {
        /* Source address and mask */
        flowsel->key.src.in.s_addr = sockaddr_src->sin.sin_addr.s_addr;
        if (sadb->src_mask != IP_NULL)
        {
            flowsel->mask.src.in.s_addr = ((struct Ip_sockaddr_in *)((Ip_u8*)sadb->src_mask + sizeof(struct sadb_address)))->sin_addr.s_addr;
            flowsel->key.src.in.s_addr &= flowsel->mask.src.in.s_addr;
        }
        else
            flowsel->mask.src.in.s_addr = 0xffffffff;
        if (sadb->srcmax_flow != IP_NULL)
        {
            sockaddr = (union Ip_sockaddr_union *)((Ip_u8*)sadb->srcmax_flow + sizeof(struct sadb_address));
            flowsel->src_max.in.s_addr = sockaddr->sin.sin_addr.s_addr;
            if (ip_ntohl(flowsel->src_max.in.s_addr) < ip_ntohl(flowsel->key.src.in.s_addr))
                return -IP_ERRNO_EINVAL;
            if (flowsel->mask.src.in.s_addr != 0xffffffff)
                return -IP_ERRNO_EINVAL;
        }
        else
            flowsel->src_max.in.s_addr = flowsel->key.src.in.s_addr;

        /* Destination address and mask */
        flowsel->key.dst.in.s_addr = sockaddr_dst->sin.sin_addr.s_addr;
        if (sadb->dst_mask != IP_NULL)
        {
            flowsel->mask.dst.in.s_addr = ((struct Ip_sockaddr_in *)((Ip_u8*)sadb->dst_mask + sizeof(struct sadb_address)))->sin_addr.s_addr;
            flowsel->key.dst.in.s_addr &= flowsel->mask.dst.in.s_addr;
        }
        else
            flowsel->mask.dst.in.s_addr = 0xffffffff;
        if (sadb->dstmax_flow != IP_NULL)
        {
            sockaddr = (union Ip_sockaddr_union *)((Ip_u8*)sadb->dstmax_flow + sizeof(struct sadb_address));
            flowsel->dst_max.in.s_addr = sockaddr->sin.sin_addr.s_addr;
            if (ip_ntohl(flowsel->dst_max.in.s_addr) < ip_ntohl(flowsel->key.dst.in.s_addr))
                return -IP_ERRNO_EINVAL;
            if (flowsel->mask.dst.in.s_addr != 0xffffffff)
                return -IP_ERRNO_EINVAL;
        }
        else
            flowsel->dst_max.in.s_addr = flowsel->key.dst.in.s_addr;
    }
#endif /* IPCOM_USE_INET */


#ifdef IPCOM_USE_INET6
    if (flowsel->key.domain == IP_AF_INET6)
    {
        struct Ip_sockaddr_in6  *sockaddr6;
        int i;

        /* Source address and mask */
        ipcom_memcpy(&flowsel->key.src, &sockaddr_src->sin6.sin6_addr, sizeof(struct Ip_in6_addr));
        flowsel->key.srcscope_n = ip_htonl(sockaddr_src->sin6.sin6_scope_id);
        if (flowsel->key.srcscope_n != 0)
            flowsel->mask.srcscope_n = 0xffffffff;
        if (sadb->src_mask != IP_NULL)
        {
            sockaddr6 = (struct Ip_sockaddr_in6 *)((Ip_u8*)sadb->src_mask + sizeof(struct sadb_address));
            ipcom_memcpy(&flowsel->mask.src, &sockaddr6->sin6_addr, sizeof(struct Ip_in6_addr));
            for (i = 0; i < 4; i++)
                flowsel->key.src.in6.in6.addr32[i] &= flowsel->mask.src.in6.in6.addr32[i];
        }
        else
            ipcom_memset(&flowsel->mask.src, 0xff, sizeof(struct Ip_in6_addr));
        if (sadb->srcmax_flow != IP_NULL)
        {
            sockaddr6 = (struct Ip_sockaddr_in6 *)((Ip_u8*)sadb->srcmax_flow + sizeof(struct sadb_address));
            ipcom_memcpy(&flowsel->src_max, &sockaddr6->sin6_addr, sizeof(struct Ip_in6_addr));
            if (ipipsec_addrcmp(IP_AF_INET6, &flowsel->src_max, &flowsel->key.src) == -1)
                return -IP_ERRNO_EINVAL;
            if (ipcom_mask_to_prefixlen(&flowsel->mask.src, 128) != 128)
                return -IP_ERRNO_EINVAL;
        }
        else
            ipcom_memcpy(&flowsel->src_max, &flowsel->key.src, sizeof(struct Ip_in6_addr));

        /* Destination address and mask */
        ipcom_memcpy(&flowsel->key.dst, &sockaddr_dst->sin6.sin6_addr, sizeof(struct Ip_in6_addr));
        flowsel->key.dstscope_n = ip_htonl(sockaddr_dst->sin6.sin6_scope_id);
        if (flowsel->key.dstscope_n != 0)
            flowsel->mask.dstscope_n = 0xffffffff;
        if (sadb->dst_mask != IP_NULL)
        {
            sockaddr6 = (struct Ip_sockaddr_in6 *)((Ip_u8*)sadb->dst_mask + sizeof(struct sadb_address));
            ipcom_memcpy(&flowsel->mask.dst, &sockaddr6->sin6_addr, sizeof(struct Ip_in6_addr));
            for (i = 0; i < 4; i++)
                flowsel->key.dst.in6.in6.addr32[i] &= flowsel->mask.dst.in6.in6.addr32[i];
        }
        else
            ipcom_memset(&flowsel->mask.dst, 0xff, sizeof(struct Ip_in6_addr));
        if (sadb->dstmax_flow != IP_NULL)
        {
            sockaddr6 = (struct Ip_sockaddr_in6 *)((Ip_u8*)sadb->dstmax_flow + sizeof(struct sadb_address));
            ipcom_memcpy(&flowsel->dst_max, &sockaddr6->sin6_addr, sizeof(struct Ip_in6_addr));
            if (ipipsec_addrcmp(IP_AF_INET6, &flowsel->dst_max, &flowsel->key.dst) == -1)
                return -IP_ERRNO_EINVAL;
            if (ipcom_mask_to_prefixlen(&flowsel->mask.dst, 128) != 128)
                return -IP_ERRNO_EINVAL;
        }
        else
            ipcom_memcpy(&flowsel->dst_max, &flowsel->key.dst, sizeof(struct Ip_in6_addr));
    }
#endif /* IPCOM_USE_INET6 */

    return 0;
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_msg_sadb_x_addflow
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_msg_sadb_x_addflow(Ipipsec_pfkeyv2_sadb *sadb)
{
    Ipipsec_flowsel   flowsel;
    Ipipsec_sa    *sa;
    Ipipsec_flow   *flow;
    int             flowdir, retval;
    Ip_u8           flowtype;

    ip_assert(sadb->msg);
    ip_assert(sadb->src_flow);
    ip_assert(sadb->dst_flow);

    /* Get flow direction */
    flowdir = ipipsec_pfkeyv2_sadb_to_flowdir(sadb, IPIPSEC_DIR_OUT);
    if (flowdir < 0)
        return flowdir;

    /* Get flowtype */
    flowtype = SADB_X_FLOW_TYPE_UNSPEC;
    if (sadb->msg->sadb_msg_satype == SADB_X_SATYPE_BYPASS)
    {
        /* bypass flow */
        flowtype = SADB_X_FLOW_TYPE_BYPASS;
    }
    else if (sadb->flowopt)
    {
        /* flowtype specified in extension */
        flowtype = sadb->flowopt->sadb_flowopt_type;
    }
    else if (sadb->sa != IP_NULL
             && sadb->sa->sadb_sa_spi == 0
             && sadb->msg->sadb_msg_satype == SADB_SATYPE_UNSPEC)
    {
        /* spi == 0 and satype == 0 -> flow without SA (used for require) */
        flowtype = SADB_X_FLOW_TYPE_REQUIRE;
    }

    /* Default to 'use' flowtype for security reasons */
    if (flowtype == SADB_X_FLOW_TYPE_UNSPEC)
        flowtype = SADB_X_FLOW_TYPE_USE;

    /* Act on flowtype */
    switch (flowtype)
    {
    case SADB_X_FLOW_TYPE_BYPASS:
        sa = &ipipsec.sa_bypass;
        break;

    case SADB_X_FLOW_TYPE_ACQUIRE:
    case SADB_X_FLOW_TYPE_REQUIRE:
    case SADB_X_FLOW_TYPE_DENY:
        sa = IP_NULL;
        break;

    case SADB_X_FLOW_TYPE_USE:
        /* Get SA */
        sa = ipipsec_pfkeyv2_sadb_sa_lookup(sadb, flowdir, IP_FALSE);
        if (sa == IP_NULL || sa == &ipipsec.sa_bypass)
            return -IP_ERRNO_ESRCH;
        ip_assert(sa->key.direction == flowdir);
        break;

    default:
        IPCOM_LOG1(DEBUG, "ipipsec_pfkeyv2_msg_sadb_x_addflow() :: invalid flowtype '%d'", (int)flowtype);
        return -IP_ERRNO_EINVAL;
    }

    /* Fill in flowsel */
    retval = ipipsec_pfkeyv2_sadb_to_flowsel(sadb, &flowsel);
    if (retval != 0)
        return retval;
    flowsel.key.direction = (Ip_u8)flowdir;

    /* Check for duplicate flow (only submit SA for input flows) */
    flow = ipipsec_flow_lookup_exact(&flowsel,
                                     flowdir == IPIPSEC_DIR_IN ? sa : IP_NULL,
                                     0);
    if (flow != IP_NULL)
    {
        /* Do not accept duplicate flows unless we are replacing them */
        if (sadb->sa && IP_BIT_ISSET(sadb->sa->sadb_sa_flags, SADB_X_SAFLAGS_REPLACEFLOW))
            ipipsec_flow_free(flow);
        else
            return -IP_ERRNO_EEXIST;
    }

    /* Create and init new flow */
    flow = ipipsec_flow_malloc(&flowsel, sa, flowtype);
    if (flow == IP_NULL)
        return -IP_ERRNO_ENOMEM;

    if (sadb->flowopt != IP_NULL)
    {
        sadb->flowopt->sadb_flowopt_type      = flow->flowtype;
        sadb->flowopt->sadb_flowopt_direction = flow->flowsel.key.direction + 1;
        ip_assert(sadb->flowopt->sadb_flowopt_direction <= 2);
        sadb->flowopt->sadb_flowopt_priority  = flow->flowsel.key.priority;
        sadb->flowopt->sadb_flowopt_dscp      = flow->flowsel.key.dscp;
        sadb->flowopt->sadb_flowopt_id        = flow->id;
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_msg_sadb_x_delflow
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_msg_sadb_x_delflow(Ipipsec_pfkeyv2_sadb *sadb)
{
    Ipipsec_flowsel      flowsel;
    Ipipsec_sa          *sa;
    Ipipsec_flow        *flow;
    int                  flowdir, flowid, retval;

    /* Get flow direction */
    flowdir = ipipsec_pfkeyv2_sadb_to_flowdir(sadb, IPIPSEC_DIR_OUT);
    if (flowdir < 0)
        return flowdir;

    flowid = sadb->flowopt ? sadb->flowopt->sadb_flowopt_id : 0;
    if (flowid != 0)
    {
        /* Lookup the exact flow using index only */
        flow = ipipsec_flow_lookup_exact(IP_NULL, IP_NULL, flowid);
    }
    else
    {
        /* Lookup using source and destination address */
        if (sadb->src_flow == IP_NULL || sadb->dst_flow == IP_NULL
            || sadb->src_mask == IP_NULL || sadb->dst_mask == IP_NULL)
            return -IP_ERRNO_EINVAL;

        /* Lookup optional SA */
        sa = ipipsec_pfkeyv2_sadb_sa_lookup(sadb, flowdir, IP_FALSE);

        /* Fill in flowsel */
        retval = ipipsec_pfkeyv2_sadb_to_flowsel(sadb, &flowsel);
        if (retval != 0)
            return retval;
        flowsel.key.direction = (Ip_u8)flowdir;

        /* Lookup the exact flow */
        flow = ipipsec_flow_lookup_exact(&flowsel, sa, 0);
    }

    if (flow == IP_NULL)
        return -IP_ERRNO_ESRCH;

    if (sadb->flowopt != IP_NULL)
    {
        sadb->flowopt->sadb_flowopt_type      = flow->flowtype;
        sadb->flowopt->sadb_flowopt_direction = flow->flowsel.key.direction + 1;
        ip_assert(sadb->flowopt->sadb_flowopt_direction <= 2);
        sadb->flowopt->sadb_flowopt_priority  = flow->flowsel.key.priority;
        sadb->flowopt->sadb_flowopt_dscp      = flow->flowsel.key.dscp;
        sadb->flowopt->sadb_flowopt_id        = flow->id;
    }

    /* Delete the flow */
    ipipsec_flow_free(flow);

    return 0;
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_msg_sadb_x_findflow
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_msg_sadb_x_findflow(Ipcom_socket *sock, Ipipsec_pfkeyv2_sadb *sadb)
{
    Ipipsec_flowsel   flowsel;
    Ipipsec_flow     *flow;
    Ipipsec_sa       *sa;
    int               retval, flowdir;

    /* Get flow direction */
    flowdir = ipipsec_pfkeyv2_sadb_to_flowdir(sadb, IPIPSEC_DIR_OUT);
    if (flowdir < 0)
        return flowdir;

    /* Try lookup SA */
    sa = ipipsec_pfkeyv2_sadb_sa_lookup(sadb, flowdir, IP_FALSE);

    /* Fill in flowsel */
    retval = ipipsec_pfkeyv2_sadb_to_flowsel(sadb, &flowsel);
    if (retval != 0)
        return retval;
    flowsel.key.direction = (Ip_u8)flowdir;

    /* Lookup flow just like a output/input operation was made */
    flow = ipipsec_flow_lookup(&flowsel.key, sa, 0);
    if (flow == IP_NULL)
        return -IP_ERRNO_ESRCH;

    /* [3.1.5] Return SADB_X_FINDFLOW message to the socket that sent the message */
    return ipipsec_pfkeyv2_output_flow(sock, flow, sadb);
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_msg_sadb_x_dumpflow
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_msg_sadb_x_dumpflow(Ipcom_socket *sock, Ipipsec_pfkeyv2_sadb *sadb)
{
    Ipipsec_flow   *flow, *last;
    int     retval;
    int     flowtype;
    int     flowdir, flow_index, domain_index;

    ip_assert(sadb->msg);
    ip_assert(sadb->msg->sadb_msg_type == SADB_X_DUMPFLOW);

    /* Get flow direction */
    flowdir = ipipsec_pfkeyv2_sadb_to_flowdir(sadb, IPIPSEC_DIR_ANY);
    if (flowdir < 0)
        return flowdir;

    if (sadb->flowopt)
        flowtype = sadb->flowopt->sadb_flowopt_type;
    else
        flowtype = SADB_X_FLOW_TYPE_UNSPEC;

    last = IP_NULL;
    for (domain_index = 0; domain_index < 2; domain_index++)
        for (flow_index = 0; flow_index < 2; flow_index++)
        {
            if (flowdir != IPIPSEC_DIR_ANY
                && flowdir != flow_index)
                continue;

            for (flow = IPCOM_LIST_FIRST(&ipipsec.flow_head[domain_index][flow_index]);
                 flow != IP_NULL;
                 flow = IPCOM_LIST_NEXT(&flow->flow_list))
                if (flowtype == SADB_X_FLOW_TYPE_UNSPEC || flowtype == flow->flowtype)
                {
                    if (last)
                    {
                        retval = ipipsec_pfkeyv2_output_flow(sock, last, sadb);
                        if (retval != 0)
                            return retval;
                    }
                    last = flow;
                }
        }

    /* Write the last one with seq field of zero */
    if (last)
    {
        sadb->msg->sadb_msg_seq = 0;
        retval = ipipsec_pfkeyv2_output_flow(sock, last, sadb);
        if (retval != 0)
            return retval;
    }
    else
        return -IP_ERRNO_ESRCH;

    return 0;
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_msg_sadb_x_grpspis
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_msg_sadb_x_grpspis(Ipipsec_pfkeyv2_sadb *sadb)
{
    Ipipsec_sa   *sa, *sa2;
    Ipipsec_key   key2;
    int    retval;

    ip_assert(sadb->sa);
    ip_assert(sadb->address_dst);
    ip_assert(sadb->sa2);
    ip_assert(sadb->dst2);

    /* Get first SA (lookup exact first, then best match) */
    sa = ipipsec_pfkeyv2_sadb_sa_lookup(sadb, IPIPSEC_DIR_OUT, IP_TRUE);
    if (sa == IP_NULL)
        sa = ipipsec_pfkeyv2_sadb_sa_lookup(sadb, IPIPSEC_DIR_OUT, IP_FALSE);
    if (sa == IP_NULL || sa == &ipipsec.sa_bypass)
        return -IP_ERRNO_ESRCH;

    /* Get second SA (which always must be an output SA) */
    retval = ipipsec_pfkeyv2_sadb_to_key2(sadb, &key2);
    if (retval != 0)
        return retval;
    sa2 = ipipsec_sa_lookup_exact(&key2);
    if (sa2 == IP_NULL)
        sa2 = ipipsec_sa_lookup(&key2, 0);
    if (sa2 == IP_NULL || sa2 == &ipipsec.sa_bypass)
        return -IP_ERRNO_ESRCH;

    /* Group them together */
    (void)ipipsec_sa_group(sa, sa2);

    return 0;
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_msg_sadb_register
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_pfkeyv2_msg_sadb_register(Ipcom_socket *sock, Ipipsec_pfkeyv2_sadb *sadb)
{
    Ip_size_t  length;
    int        retval;
    Ip_u8     *orgbuf, *msgbuf;

    ip_assert(sadb->msg);

    /* Can only register for satype <= 31 since stored in 32bit bit-field */
    if (sadb->msg->sadb_msg_satype > 31)
        return -IP_ERRNO_EINVAL;

    /* Reply with a sadb_supported message */
    length = sizeof(struct sadb_supported) + (SADB_AALG_MAX + SADB_EALG_MAX) * sizeof(struct sadb_alg);

    orgbuf = (Ip_u8 *)ipcom_malloc(length + sizeof(Ip_u64));
    if (orgbuf == IP_NULL)
    {
        IPCOM_LOG1(ERR, "ipipsec_pfkeyv2_msg_sadb_register() :: out of memory ('msgbuf' %ld bytes)",
                   length + sizeof(Ip_u64));
        return -IP_ERRNO_ENOMEM;
    }
    msgbuf = IP_PTR_ALIGN(orgbuf, 8);   /* 64-bit align msgbuf */
    ip_assert(((Ip_ptrdiff_t)msgbuf & 0x7L) == 0);
    ipcom_memset(msgbuf, 0, length);

    /* Only count unregistered sockets (they may be registered for multiple types) */
    if (sock->ipsec_registered == 0)
        ipipsec.registered_num++;
    IP_BIT_SET(sock->ipsec_registered, 1 << sadb->msg->sadb_msg_satype);


    /***** Send REGISTER reply to all registered sockets *****/

    /* Use 'msg' from the incoming buffer. Use 'msgbuf' for sadb_supported */
    ipcom_memset(&sadb->sa, 0, SADB_EXT_MAX * sizeof(void *)); /*lint !e419 */
    sadb->supported = (struct sadb_supported *)msgbuf;

    /* SADB_MSG */
    ip_assert(sadb->msg->sadb_msg_version == PF_KEY_V2);
    ip_assert(sadb->msg->sadb_msg_reserved == 0);

    /* SADB_EXT_SUPPORTED */
    ipipsec_crypt_get_algs(sadb->supported);

    /* Send the message to all registered sockets (sock != IP_NULL) */
    retval = ipipsec_pfkeyv2_output(sock, (void *)sadb, IPIPSEC_SNDMODE_REGISTERED, 0);

    ipcom_free(orgbuf);

    return retval;
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_copy_lifetimes
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipipsec_pfkeyv2_copy_lifetimes(struct sadb_lifetime *lt, Ipipsec_sa_lifetime *sa_lt)
{
    lt->sadb_lifetime_allocations = sa_lt->allocations;
    ipcom_memcpy(&lt->sadb_lifetime_bytes, &sa_lt->bytes, sizeof(lt->sadb_lifetime_bytes));

    IP_U64_SETHI(lt->sadb_lifetime_addtime, 0);
    IP_U64_SETLO(lt->sadb_lifetime_addtime, sa_lt->addtime);

    IP_U64_SETHI(lt->sadb_lifetime_usetime, 0);
    IP_U64_SETLO(lt->sadb_lifetime_usetime, sa_lt->usetime);
}


/*
 ****************************************************************************
 * 10                    PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_acquire
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipipsec_pfkeyv2_acquire(Ipipsec_key *key)
{
    struct acquire_msg
    {
        struct sadb_msg                   msg;
        struct Ipipsec_sadb_prop_full     prop;
        struct Ipipsec_sadb_address_full  src;
        struct Ipipsec_sadb_address_full  dst;
    } acquire;
    void   *exthdr[SADB_EXT_MAX+1];

    if (ipipsec.registered_num == 0)
        return;

    ip_assert(key);
    ip_assert(key->domain == IP_AF_INET || key->domain == IP_AF_INET6);

    /* Clear structures */
    ipcom_memset(exthdr, 0, sizeof(exthdr));
    ipcom_memset(&acquire, 0, sizeof(struct acquire_msg));

    /* Message header */
    acquire.msg.sadb_msg_version = PF_KEY_V2;
    acquire.msg.sadb_msg_type    = SADB_ACQUIRE;
    acquire.msg.sadb_msg_seq     = ipipsec.pfkeyv2_seq++;
    exthdr[0] = &acquire.msg;

    /* SADB_EXT_ADDRESS_SRC */
    exthdr[SADB_EXT_ADDRESS_SRC] = &acquire.src;
    ipipsec_pfkeyv2_ipaddr_to_extaddr(&acquire.src.addr,
                                      &key->src,
                                      ip_ntohs(key->ports.udptcp_srcdst_n[0]),
                                      key->domain);
    acquire.src.addr.sadb_address_proto = key->proto;
    acquire.src.addr.sadb_address_exttype = SADB_EXT_ADDRESS_SRC;

    /* SADB_EXT_ADDRESS_DST */
    exthdr[SADB_EXT_ADDRESS_DST] = &acquire.dst;
    ipipsec_pfkeyv2_ipaddr_to_extaddr(&acquire.dst.addr,
                                      &key->dst,
                                      ip_ntohs(key->ports.udptcp_srcdst_n[1]),
                                      key->domain);
    acquire.dst.addr.sadb_address_proto = key->proto;
    acquire.dst.addr.sadb_address_exttype = SADB_EXT_ADDRESS_DST;

    /* Proposal */
    exthdr[SADB_EXT_PROPOSAL] = &acquire.prop;
    acquire.prop.prop.sadb_prop_len = sizeof(acquire.prop) >> 3;
    acquire.prop.prop.sadb_prop_exttype = SADB_EXT_PROPOSAL;

    IPCOM_LOG1(DEBUG, "Sending acquire message for dst=%s",
               ipcom_inet_ntop(acquire.dst.sock.sa.sa_family,
                               &key->dst,
                               ipipsec.sysbuf, sizeof(ipipsec.sysbuf)));

    /* Send to all registered listeners that match the specified satype */
    (void)ipipsec_pfkeyv2_output(IP_NULL, exthdr, IPIPSEC_SNDMODE_REGISTERED, 0);
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_expire
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipipsec_pfkeyv2_expire(Ipipsec_sa *sa, Ip_u32 type)
{
    struct expire_msg
    {
        struct sadb_msg msg;
        struct sadb_sa  sa;
        struct sadb_lifetime current;
        struct sadb_lifetime soft_or_hard;
        struct Ipipsec_sadb_address_full   src;
        struct Ipipsec_sadb_address_full   dst;
        struct sadb_x_saopt  saopt;
    } expire;
    void   *exthdr[SADB_EXT_MAX+1];

    ipcom_memset(exthdr, 0, sizeof(exthdr));
    ipcom_memset(&expire, 0, sizeof(struct expire_msg));

    ip_assert(type == IPIPSEC_EXPIRE_TYPE_HARD || type == IPIPSEC_EXPIRE_TYPE_SOFT);

    expire.msg.sadb_msg_version = PF_KEY_V2;
    expire.msg.sadb_msg_type = SADB_EXPIRE;
    expire.msg.sadb_msg_satype = sa->sadb_msg_satype;
    expire.msg.sadb_msg_seq = ipipsec.pfkeyv2_seq++;
    exthdr[0] = &expire.msg;

    /* Security association */
    expire.sa.sadb_sa_len = sizeof(expire.sa) / 8;
    expire.sa.sadb_sa_exttype = SADB_EXT_SA;
    expire.sa.sadb_sa_spi = sa->key.ports.espah_spi_n;
    expire.sa.sadb_sa_replay = sa->sadb_sa_replay;
    expire.sa.sadb_sa_state = sa->sadb_sa_state;
    expire.sa.sadb_sa_auth = sa->sadb_sa_auth;
    expire.sa.sadb_sa_encrypt = sa->sadb_sa_encrypt;
    expire.sa.sadb_sa_flags = sa->sadb_sa_flags;
    exthdr[SADB_EXT_SA] = &expire.sa;

    /* Lifetime */
    expire.current.sadb_lifetime_len = sizeof(expire.current) / 8;
    expire.current.sadb_lifetime_exttype = SADB_EXT_LIFETIME_CURRENT;
    ipipsec_pfkeyv2_copy_lifetimes(&expire.current, &sa->ltcurrent);
    exthdr[SADB_EXT_LIFETIME_CURRENT] = &expire.current;

    expire.soft_or_hard.sadb_lifetime_len = sizeof(expire.soft_or_hard) / 8;
    if (type == IPIPSEC_EXPIRE_TYPE_HARD)
    {
        expire.soft_or_hard.sadb_lifetime_exttype = SADB_EXT_LIFETIME_HARD;
        ipipsec_pfkeyv2_copy_lifetimes(&expire.soft_or_hard, &sa->lthard);
        exthdr[SADB_EXT_LIFETIME_HARD] = &expire.soft_or_hard;
    }
    else
    {
        expire.soft_or_hard.sadb_lifetime_exttype = SADB_EXT_LIFETIME_SOFT;
        ipipsec_pfkeyv2_copy_lifetimes(&expire.soft_or_hard, &sa->ltsoft);
        exthdr[SADB_EXT_LIFETIME_SOFT] = &expire.soft_or_hard;
    }

    /* SADB_EXT_ADDRESS_SRC */
    exthdr[SADB_EXT_ADDRESS_SRC] = &expire.src;
    ipipsec_pfkeyv2_ipaddr_to_extaddr(&expire.src.addr, &sa->key.src, 0, sa->key.domain);
    expire.src.addr.sadb_address_proto = 0;
    expire.src.addr.sadb_address_exttype = SADB_EXT_ADDRESS_SRC;

    /* SADB_EXT_ADDRESS_DST */
    exthdr[SADB_EXT_ADDRESS_DST] = &expire.dst;
    ipipsec_pfkeyv2_ipaddr_to_extaddr(&expire.dst.addr, &sa->key.dst, 0, sa->key.domain);
    expire.dst.addr.sadb_address_proto = 0;
    expire.dst.addr.sadb_address_exttype = SADB_EXT_ADDRESS_DST;

    IPCOM_LOG1(DEBUG, "Sending expire message for dst=%s",
               ipcom_inet_ntop(expire.dst.sock.sa.sa_family,
                               &sa->key.dst,
                               ipipsec.sysbuf, sizeof(ipipsec.sysbuf)));

    /* SADB_X_EXT_SA_OPTIONS */
    if (sa->key.priority != 0)
    {
        exthdr[SADB_X_EXT_SA_OPTIONS] = &expire.saopt;
        expire.saopt.sadb_saopt_len      = sizeof(expire.saopt) / 8;
        expire.saopt.sadb_saopt_exttype  = SADB_X_EXT_SA_OPTIONS;
        expire.saopt.sadb_saopt_priority = sa->key.priority;
    }

    /* Send to all Key sockets */
    (void)ipipsec_pfkeyv2_output(IP_NULL, exthdr, IPIPSEC_SNDMODE_BROADCAST, 0);

    if (type == IPIPSEC_EXPIRE_TYPE_SOFT)
    {
        /* Disable soft checking */
        IP_U64_CLEAR(sa->ltsoft.bytes);
        sa->ltsoft.addtime = 0;
        sa->ltsoft.usetime = 0;
        ipipsec_sa_reset_timeout(sa);
    }
}


/*
 *===========================================================================
 *                    ipipsec_pfkeyv2_input
 *===========================================================================
 * Description: Handles writes to IP_PF_KEY sockets.
 * Parameters:  sock - the socket used when writing
 *              to - unused by IP_PF_KEY sockets
 *              pkt - the data written
 * Returns:     0 = success, <0 = error code
 *
 */
IP_GLOBAL int
ipipsec_pfkeyv2_input(Ipcom_socket *sock, IP_CONST struct Ip_msghdr *msghdr, Ipcom_pkt *pkt)
{
    void                  *exthdr[SADB_EXT_MAX+1];
    struct sadb_msg       *msg;
    char                  *buf;
    struct sadb_ext       *ext;
    int                    retval = -IP_ERRNO_EINVAL;
    Ip_u32                 extmap, extdiff;
    Ip_u32                 sndmode = IPIPSEC_SNDMODE_BROADCAST;
    struct sadb_sa         samem;
    int                    nbyte = pkt->end - pkt->start;

#ifdef IPIPSEC_USE_DAEMONS
    Ipnet_sig *sig;

    sig = ipipsec_suspend_daemons();
    if (sig == IP_NULL)
    {
        retval = -IP_ERRNO_ENOMEM;
        goto leave;
    }
    ipipsec_poll_control_pipe();
#endif

    (void)msghdr;

    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_IPSEC_PF_KEY_IN);
    /* Prepare extension header array */
    ipcom_memset(exthdr, 0, sizeof(exthdr));
    msg = (struct sadb_msg *)&pkt->data[pkt->start];
    buf = (char *)msg;
    exthdr[0] = msg;

    /*!!TODO: send copy of message to all PFKEYV2_SOCKETFLAGS_PROMISC sockets */

    /* verify sadb_msg message header */

    if (nbyte < (int)sizeof(struct sadb_msg))
    {
        IPCOM_LOG1(DEBUG, "ipipsec_pfkeyv2_input() :: too short (%d) 'sadb_msg' header", nbyte);
        goto leave;
    }

    if ((msg->sadb_msg_version != PF_KEY_V2)
        || (msg->sadb_msg_type == SADB_RESERVED)
        || (msg->sadb_msg_type > SADB_MAX)
        /* [2.1] - sadb_msg_errno should be set to 0, i.e. not must */
        || (msg->sadb_msg_len * sizeof(Ip_u64) != (Ip_u32) nbyte)
        || (msg->sadb_msg_reserved != 0)
        || (msg->sadb_msg_pid == 0))   /* trivial pid check */
    {
        IPCOM_LOG0(DEBUG, "ipipsec_pfkeyv2_input() :: bad 'sadb_msg' header");
        goto einval;
    }

    IPCOM_LOG1(DEBUG, "ipipsec_pfkeyv2_input() :: receiving '%s' command",
               ipipsec_pfkeyv2_command[msg->sadb_msg_type]);

    /* Step over the sadb_msg header */
    buf += sizeof(struct sadb_msg);
    nbyte -= sizeof(struct sadb_msg);

    /* Step through the extension headers */
    for (extmap = 0, ext = (struct sadb_ext *)buf; nbyte >= (int)sizeof(struct sadb_ext);
         ext = (struct sadb_ext *)buf)
    {
        int   ext_len;

        /* extension headers must start at 32-bit alignment (acctually 64-bit) */
        if (((Ip_ptrdiff_t)ext & 0x3L) != 0)
        {
            IPCOM_LOG1(DEBUG, "ipipsec_pfkeyv2_input() :: bad extension alignment(0x%lx)",
                       (Ip_ptrdiff_t)ext);
            goto einval;
        }

        ext_len = (int)(ext->sadb_ext_len * sizeof(Ip_u64));

        if (ext_len == 0 || nbyte < ext_len
            || ext->sadb_ext_type == SADB_EXT_RESERVED)
        {
            IPCOM_LOG1(DEBUG, "ipipsec_pfkeyv2_input() :: bad extension length(%ld) or type(0)", ext_len);
            goto einval;
        }

        /* [2.3] - Ignore unknown (that includes > MAX) extensions */
        if (ext->sadb_ext_type <= SADB_EXT_MAX)
        {
            extmap |= (1 << ext->sadb_ext_type);

            /* [2.3] Only one instance of an extension header */
            if (exthdr[ext->sadb_ext_type])
            {
                IPCOM_LOG1(DEBUG, "ipipsec_pfkeyv2_input() :: duplicate extension header [ext=%d]",
                    ext->sadb_ext_type);
                goto einval;
            }
            else
                exthdr[ext->sadb_ext_type] = ext;

            /* _Exact_ extension header size check */
            if (ipipsec_pfkeyv2_ext_sizes[ext->sadb_ext_type] > 0 &&
                ipipsec_pfkeyv2_ext_sizes[ext->sadb_ext_type] != ext_len)
            {
                IPCOM_LOG2(DEBUG, "ipipsec_pfkeyv2_input() :: illegal extension header length(%d) [ext=%d]",
                           ext_len, ext->sadb_ext_type);
                goto einval;
            }

            /* _Minimum_ extension header size check */
            if (ipipsec_pfkeyv2_ext_sizes[ext->sadb_ext_type] < 0 &&
                ext_len < -ipipsec_pfkeyv2_ext_sizes[ext->sadb_ext_type])
            {
                IPCOM_LOG2(DEBUG, "ipipsec_pfkeyv2_input() :: too small extension header length(%d) [ext=%d]",
                           ext_len, ext->sadb_ext_type);
                goto einval;
            }

            /* Extension specific checks */
            switch(ext->sadb_ext_type)
            {
            case SADB_EXT_SA :
            case SADB_X_EXT_SA2 :
                {
                    struct sadb_sa *sa;
                    sa = (struct sadb_sa *)ext;

                    if (sa->sadb_sa_state == SADB_SASTATE_DEAD
                        || sa->sadb_sa_state > SADB_SASTATE_MAX
                        || sa->sadb_sa_auth > SADB_AALG_MAX
                        || sa->sadb_sa_encrypt > SADB_EALG_MAX)
                    {
                        IPCOM_LOG0(DEBUG, "ipipsec_pfkeyv2_input() :: invalid SA (sadb_sa) values");
                        goto einval;
                    }
                }
                break;
            case SADB_EXT_ADDRESS_SRC :
                /*!! [2.3.3] source address must be unicast or 0 */
            case SADB_X_EXT_DST2 :
            case SADB_EXT_ADDRESS_DST :
            case SADB_EXT_ADDRESS_PROXY :
                /* Ports must be 0 since not used with SRC,DST and PROXY */
                if (((struct Ip_sockaddr_in *)((Ip_u8 *)ext + sizeof(struct sadb_address)))->sin_port != 0)
                {
                    IPCOM_LOG0(DEBUG, "ipipsec_pfkeyv2_input() :: non-zero ports");
                    goto einval;
                }

                /* Bypass destination SA is domain-less */
                if (ext->sadb_ext_type == SADB_EXT_ADDRESS_DST
                   && msg->sadb_msg_satype == SADB_X_SATYPE_BYPASS)
                    break;
                goto check_address;

            case SADB_X_EXT_SRC_MASK :
            case SADB_X_EXT_DST_MASK :
            case SADB_X_EXT_SRC_FLOW :
            case SADB_X_EXT_DST_FLOW :
            case SADB_X_EXT_SRCMAX_FLOW :
            case SADB_X_EXT_DSTMAX_FLOW :
                {
                    struct sadb_address *address;
                    struct Ip_sockaddr  *sockaddr;

                    address  = (struct sadb_address *)ext;
                    if (address->sadb_address_reserved || address->sadb_address_prefixlen != 0)
                    {
                        IPCOM_LOG0(DEBUG, "ipipsec_pfkeyv2_input() :: non-zero sadb_address reserved or prefixlen");
                        goto einval;
                    }

                check_address:
                    sockaddr = (struct Ip_sockaddr *)((Ip_u8 *)ext + sizeof(struct sadb_address));
#if defined(IPCOM_USE_INET) && defined(IPCOM_USE_INET6)
                    if (sockaddr->sa_family != IP_AF_INET && sockaddr->sa_family != IP_AF_INET6)
                    {
                        IPCOM_LOG1(DEBUG, "ipipsec_pfkeyv2_input() :: bad socket address family (%d)",
                                   (int)sockaddr->sa_family);
                        goto einval;
                    }
#elif defined(IPCOM_USE_INET6)
                    if (sockaddr->sa_family != IP_AF_INET6)
                        goto einval;
#else
                    if (sockaddr->sa_family != IP_AF_INET)
                        goto einval;
#endif

                    /* [2.3.3] ignore sa_len check because it is not MUST */
                }
                break;
            case SADB_EXT_KEY_AUTH :
            case SADB_EXT_KEY_ENCRYPT :
                {
                    struct sadb_key   *key;

                    key = (struct sadb_key *)ext;
                    if (key->sadb_key_bits == 0
                        || key->sadb_key_reserved != 0
                        || ((key->sadb_key_bits + 63) / 64) * sizeof(Ip_u64) != (ext_len - sizeof(struct sadb_key)))
                    {
                        IPCOM_LOG0(DEBUG, "ipipsec_pfkeyv2_input() :: invalid KEY (sadb_key) values");
                        goto einval;
                    }
                }
                break;
            case SADB_EXT_SPIRANGE :
                {
                    struct sadb_spirange   *spirange;
                    spirange = (struct sadb_spirange *)ext;

                    /* SPI range check */
                    if (spirange->sadb_spirange_min > spirange->sadb_spirange_max
                        || spirange->sadb_spirange_max <= IPIPSEC_SPI_RESERVED_MAX)
                    {
                        IPCOM_LOG0(DEBUG, "ipipsec_pfkeyv2_input() :: invalid SPI range");
                        goto einval;
                    }
                }
                break;
                /* no checks
                   case SADB_X_EXT_PROTOCOL :
                   case SADB_X_EXT_FLOW_OPTIONS :
                   case SADB_X_EXT_UDPENCAP :
                */
                /* only size checks:
                case SADB_X_EXT_PROTOCOL :
                case SADB_EXT_LIFETIME_CURRENT :
                case SADB_EXT_LIFETIME_HARD :
                case SADB_EXT_LIFETIME_SOFT :
                */
                /* currently unsupported:
                   case SADB_EXT_IDENTITY_SRC :
                   case SADB_EXT_IDENTITY_DST :
                   case SADB_EXT_SENSITIVITY :
                   case SADB_EXT_PROPOSAL :
                   case SADB_EXT_SUPPORTED :
                */
            default:
                break;
            }
        }

        /* next extension header */
        nbyte -= ext_len;
        buf += ext_len;
    }

    /* Do not accept trailing extra bytes */
    if (nbyte != 0)
    {
        IPCOM_LOG1(DEBUG, "ipipsec_pfkeyv2_input() :: bad length, additional trailing bytes (%d)", nbyte);
        goto einval;
    }

    /* Check if the required IN extensions are there */
    extdiff = extmap & ipipsec_pfkeyv2_sadbextbits[msg->sadb_msg_type].in_req;
    if (extdiff != ipipsec_pfkeyv2_sadbextbits[msg->sadb_msg_type].in_req)
    {
        IPCOM_LOG1(DEBUG, "ipipsec_pfkeyv2_input() :: missing mandatory extension(s), diff=0x%lx",
                   (extdiff ^ ipipsec_pfkeyv2_sadbextbits[msg->sadb_msg_type].in_req));
        goto einval;
    }

    /* Check that the optional IN extensions are ok */
    extdiff = extmap | ipipsec_pfkeyv2_sadbextbits[msg->sadb_msg_type].in_opt;
    if ((extdiff ^ ipipsec_pfkeyv2_sadbextbits[msg->sadb_msg_type].in_opt) != 0)
    {
        IPCOM_LOG1(DEBUG, "ipipsec_pfkeyv2_input() :: illegal optional extension(s), diff=0x%lx",
                   extdiff ^ ipipsec_pfkeyv2_sadbextbits[msg->sadb_msg_type].in_opt);
        goto einval;
    }

    /* Act on the message type */
    switch(msg->sadb_msg_type)
    {
    case SADB_GETSPI :        /* reply to all */
        retval = ipipsec_pfkeyv2_msg_sadb_getspi((Ipipsec_pfkeyv2_sadb *)exthdr, &samem);
        break;
    case SADB_UPDATE :        /* reply to all */
        retval = ipipsec_pfkeyv2_msg_sadb_update((Ipipsec_pfkeyv2_sadb *)exthdr);
    break;
    case SADB_ADD :           /* reply to all */
        retval = ipipsec_pfkeyv2_msg_sadb_add((Ipipsec_pfkeyv2_sadb *)exthdr);
        break;
    case SADB_DELETE :        /* reply to all */
        retval = ipipsec_pfkeyv2_msg_sadb_delete((Ipipsec_pfkeyv2_sadb *)exthdr);
        break;
    case SADB_GET :           /* reply to sender */
        retval = ipipsec_pfkeyv2_msg_sadb_get(sock, (Ipipsec_pfkeyv2_sadb *)exthdr);
        /* If success, a sadb has already been sent */
        if (retval == 0)
            goto leave;
        break;
    case SADB_FLUSH :         /* reply to all */
        retval = ipipsec_pfkeyv2_msg_sadb_flush((Ipipsec_pfkeyv2_sadb *)exthdr);
        break;
    case SADB_DUMP :          /* reply to sender */
        retval = ipipsec_pfkeyv2_msg_sadb_dump(sock, (Ipipsec_pfkeyv2_sadb *)exthdr);
        /* If success, one or multiple messages have already been sent */
        if (retval == 0)
            goto leave;
        break;
    case SADB_EXPIRE :
    case SADB_ACQUIRE :
        retval = 0;  /* do nothing */
        break;
    case SADB_X_ADDFLOW :
        retval = ipipsec_pfkeyv2_msg_sadb_x_addflow((Ipipsec_pfkeyv2_sadb *)exthdr);
        break;
    case SADB_X_DELFLOW :
        retval = ipipsec_pfkeyv2_msg_sadb_x_delflow((Ipipsec_pfkeyv2_sadb *)exthdr);
        break;
    case SADB_X_FINDFLOW :
        retval = ipipsec_pfkeyv2_msg_sadb_x_findflow(sock, (Ipipsec_pfkeyv2_sadb *)exthdr);
        /* If success, a flow has already been sent */
        if (retval == 0)
            goto leave;
        break;
    case SADB_X_DUMPFLOW :          /* reply to sender */
        retval = ipipsec_pfkeyv2_msg_sadb_x_dumpflow(sock, (Ipipsec_pfkeyv2_sadb *)exthdr);
        /* If success, one or multiple messages have already been sent */
        if (retval == 0)
            goto leave;
        break;
    case SADB_X_GRPSPIS :
        retval = ipipsec_pfkeyv2_msg_sadb_x_grpspis((Ipipsec_pfkeyv2_sadb *)exthdr);
        break;
        /* UNIMPLEMENTED */
    case SADB_REGISTER :        /* reply to registered */
        retval = ipipsec_pfkeyv2_msg_sadb_register(sock, (Ipipsec_pfkeyv2_sadb *)exthdr);
        /* If success, a register message was sent to all registered sockets */
        if (retval == 0)
            goto leave;
        break;
    case SADB_X_PROMISC :
    case SADB_X_BINDSA :
        retval = -IP_ERRNO_ENOSYS;
        break;
    case SADB_X_MIGRATE:
	/* ignore this message in the ipsec.. reply to all */
	retval = 0;
	break;
    default:
        IPCOM_LOG1(DEBUG, "ipipsec_pfkeyv2_input() :: unknown msg_type (%d)",
                   msg->sadb_msg_type);
        IP_PANIC2();
        retval = -IP_ERRNO_EINVAL;
        break;
    }

    ip_assert(retval <= 0);
    ip_assert(exthdr[0] == msg);

    /** Error **/
    if (retval)
    {
        if (retval == -IP_ERRNO_EINVAL
            || retval == -IP_ERRNO_ENOMEM
            || retval == -IP_ERRNO_ENOBUFS)
        {
            /* EINVAL, ENOMEM and ENOBUFS may be returned immediately, lets not */
            goto einval;
        }

    }

    /* [1.4] - All properly-formed messages sent to the kernel are
    * returned to all open PF_KEY sockets, including the sender.
    */
 einval:
    (void)ipipsec_pfkeyv2_output(sock, exthdr, sndmode, -retval);
    retval = 0;

 leave:
    ipcom_pkt_free(pkt);
#ifdef IP_DEBUG
    if (retval)
        IPCOM_LOG1(DEBUG, "ipipsec_pfkeyv2_input() :: failed, errno = %d", -retval);
    else
        IPCOM_LOG0(DEBUG, "ipipsec_pfkeyv2_input() :: succeeded");
#endif
#ifdef IPIPSEC_USE_DAEMONS
    if (sig != IP_NULL)
        ipipsec_resume_daemons(sig);
#endif
    return retval;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

