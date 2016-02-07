/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_send.c,v $ $Revision: 1.19 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_send.c,v $
 *   $Author: rboden $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Roger Boden
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * This file implements Secure Neighbor Discovery (SEND) defined in RFC 3971.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipnet_config.h"

#ifdef IPNET_USE_RFC3971

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_clib.h>
#include <ipcom_time.h>
#include <ipcom_ipd.h>
#include <ipcom_err.h>
#include <ipcom_os.h>
#include <ipcom_syslog.h>
#include <ipcom_hash.h>
#include <ipcom_list.h>
#include <ipcom_tmo2.h>
#include <ipcom_key_db.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_cga.h"
#include "ipnet_cga_h.h"
#include "ipnet_icmp6.h"
#include "ipnet_ip6.h"
#include "ipnet_send.h"

#include <openssl/rsa.h>
#include <openssl/bn.h>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/x509.h>
#include <openssl/safestack.h>
#include <openssl/err.h>

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* The number of nonce bytes to include in sent messages. The actual number
 * of nonce bytes is calculated by IPNET_SEND_NONCE_BYTES*8-2 */
#ifndef IPNET_SEND_NONCE_SIZE
#define IPNET_SEND_NONCE_SIZE 1
#endif
#define IPNET_SEND_NONCE_BYTES (IPNET_SEND_NONCE_SIZE*8-2)

/* The maximum accepted size of ICMPv6 nonce options from peers. */
#ifndef IPNET_SEND_NONCE_SIZE_MAX
#define IPNET_SEND_NONCE_SIZE_MAX 4
#endif
#define IPNET_SEND_NONCE_BYTES_MAX (IPNET_SEND_NONCE_SIZE_MAX*8-2)

#define IPNET_SEND_MODE_OFF         0
#define IPNET_SEND_MODE_ON          1
#define IPNET_SEND_MODE_TRANSITION  2

#define IPNET_SEND_CERT_PATH_MODE_OFF     0
#define IPNET_SEND_CERT_PATH_MODE_HOST    1
#define IPNET_SEND_CERT_PATH_MODE_ROUTER  2

#define IPNET_SEND_CERT_PATH_ALL_COMPONENTS  0xFFFF

#define IPNET_SEND_ICMP6_OPT_CERT_TYPE_X509 1

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */
typedef struct Ipnet_send_conf_st
{
    Ip_fd     netlink_sock;
    Ip_fd     netlink_sock2;
    Ip_fd     icmp6_sock;
    EVP_PKEY *pkey;
    RSA      *rsa_key;
    Ip_u8    *der_rsa_key;
    int       der_len;
    int       timestamp_delta;
    int       cert_path_mode;
    X509     *cert;
    X509     *ca_cert;
    int       min_sec;
    STACK_OF(X509) *intermediate_certs;
    STACK_OF(X509) *cert_chain;
    STACK_OF(X509_CRL) *crl_st;
} Ipnet_send_conf;


struct Ip_nlmsg_sec_nd_sign
{
    Ip_u32              ifindex;
    Ip_u32              vr;
    Ip_u32              hlim;
    struct Ip_in6_addr  dst;
    struct Ip_in6_addr  src;
    struct Ip_in6_addr  nc_key;  /* Key used to lookup neighbor cache entry */
    Ip_err              status;  /* Used in netlink replies from the ipnet_sendd daemon */
};

typedef struct Ipnet_send_nlmsg_st
{
    struct Ip_nlmsghdr          nlmsghdr;
    struct Ip_nlmsg_sec_nd_sign sec_nd_sign;
    Ipnet_pkt_ip6               ip6_hdr;
    Ipnet_pkt_icmp6             icmp6;
} Ipnet_send_nlmsg;

typedef struct Ipnet_send_nonce_entry_st
{
    struct Ip_in6_addr peer;
    Ip_u8              nonce[IPNET_SEND_NONCE_BYTES_MAX];
    int                len;
    Ip_u8              icmp6_type;
    Ipcom_tmo2         tmo;
} Ipnet_send_nonce_entry;

typedef struct Ipnet_send_nonce_key_st
{
    struct Ip_in6_addr *peer;
    Ip_u8               icmp6_type;
} Ipnet_send_nonce_key;

typedef struct Ipnet_send_prefix_map_entry_st
{
    Ipcom_list list;

    /* Parameter from router adv message */
    struct Ip_in6_addr prefix;
    Ip_u32 prefered_time;
    Ip_u32 valid_time;

    Ip_bool cga_addr_generated;
    Ip_u8 cga_modifier[16];
    struct Ip_in6_addr cga_addr;
    struct Ip_in6_addr router_addr;
    int ifindex;

    /* Parameters related to certificate validation */
    STACK_OF(X509)    *cert_chain;
    X509              *router_cert;
    RSA               *rsa;
    Ip_bool            cert_validated;
    Ip_u16             id;  /* Id field in cert path messages */
    Ip_u16             next_component; /* Next expected component in cert
                                        * path messages. Used to detect lost msgs */
    Ip_u16             all_components; /* No of certs in the complete chain */

    Ipcom_tmo2         tmo;
} Ipnet_send_prefix_map_entry;


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */
IP_PUBLIC Ip_err
ipnet_send_cert_chain_verify_cb(X509 *ca_cert,
                                STACK_OF(X509) *cert_chain,
                                X509 *router_cert,
                                struct Ip_in6_addr *prefix);

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC void
ipnet_send_process_sign_create_req(Ipnet_send_conf *conf, Ip_u8 *nl_msg, int len);
IP_STATIC void
ipnet_send_process_sign_verify_req(Ipnet_send_conf *conf, Ip_u8 *nl_msg, int len);


IPCOM_PROCESS(ipnet_sendd);
IP_PUBLIC Ip_err ipnet_send_create(void);
IP_PUBLIC Ip_err ipnet_send_start(void);
IP_PUBLIC const char * ipnet_send_version(void);

IP_STATIC Ip_err
ipnet_send_cert_path_solicit_output(Ipnet_send_conf *conf,
                                    Ip_u16 component,
                                    Ip_u16 id,
                                    struct Ip_sockaddr_in* addr,
                                    Ip_socklen_t addr_len);

IP_STATIC Ip_err
ipnet_send_cert_chain_verify(X509 *ca_cert,
                             STACK_OF(X509) *cert_chain,
                             X509 *router_cert,
                             STACK_OF(X509_CRL) *crls,
                             STACK_OF(X509) **verified_cert_chain);

IP_STATIC void
ipnet_send_nonce_entry_free(Ipnet_send_nonce_entry *nonce_entry);

IP_STATIC void
ipnet_send_prefix_map_entry_free(Ipnet_send_prefix_map_entry *map_entry);

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */
IP_STATIC Ip_u8 ipnet_send_cga_msg_type_tag[16] =
    { 0x08,0x6F,0xCA,0x5E,0x10,0xB2,0x00,0xC9,
      0x9C,0x8C,0xE0,0x01,0x64,0x27,0x7C,0x08 };

IP_STATIC Ipcom_hash *ipnet_send_nonce_hash_tbl;

IP_STATIC Ipcom_list ipnet_send_prefix_map;

IP_STATIC Ipcom_pqueue *ipnet_send_tmo;

IP_STATIC Ip_u16 ipnet_send_cert_path_id;

IP_STATIC char   ipnet_send_log_buf[256];

IP_STATIC Ip_u8 ipnet_send_nl_buf[2048];

IP_STATIC Ipcom_cmd_int_str_map ipnet_send_mode_map[] =
    {
        { IPNET_SEND_MODE_OFF, "off" },
        { IPNET_SEND_MODE_ON, "on" },
        { IPNET_SEND_MODE_TRANSITION, "transition" },
        { -1, IP_NULL }
    };

IP_STATIC Ipcom_cmd_int_str_map ipnet_send_cert_path_mode_map[] =
    {
        { IPNET_SEND_CERT_PATH_MODE_OFF, "off" },
        { IPNET_SEND_CERT_PATH_MODE_HOST, "host" },
        { IPNET_SEND_CERT_PATH_MODE_ROUTER, "router" },
        { -1, IP_NULL }
    };

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_send_load_crl_file
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipnet_send_load_crl_file(const char *path, STACK_OF(X509_CRL) **crl_st)
{
    IP_FILE *fp;
    X509_CRL* crl;

    fp = ipcom_fopen(path, "r");
    if (fp == IP_NULL)
    {
        IPCOM_LOG1(ERR, "ipnet_send_load_crl_file :: unable to open CRL file: %s", path);
        return IPCOM_ERR_FAILED;
    }

    while ((crl = PEM_read_X509_CRL(fp, IP_NULL, IP_NULL, IP_NULL)) != IP_NULL)
    {
        sk_X509_CRL_push(*crl_st, crl);
    }

    ipcom_fclose(fp);

    if (sk_X509_CRL_num(*crl_st) == 0)
    {
        IPCOM_LOG1(ERR, "ipnet_send_load_crl_file :: failed to load CRLs from  file: %s", path);
        return IPCOM_ERR_FAILED;
    }

    return IPCOM_SUCCESS;
}

/*
 *===========================================================================
 *                    ipnet_send_load_x509_file
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC X509*
ipnet_send_load_x509_file(const char* path)
{
    IP_FILE *fp;
    X509* ret;

    fp = ipcom_fopen(path, "r");
    if (fp == IP_NULL)
    {
        IPCOM_LOG1(ERR, "ipnet_send_load_x509_file :: unable to open certificate file: %s", path);
        return IP_NULL;
    }

    ret = PEM_read_X509(fp, IP_NULL, IP_NULL, IP_NULL);
    if (!ret)
    {
        IPCOM_LOG1(ERR, "ipnet_send_load_x509_file :: failed to load certificate from file %s", path);
        goto cleanup;
    }

 cleanup:
    ipcom_fclose(fp);

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_send_load_cert_st_file
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipnet_send_load_cert_st_file(char* file, STACK_OF(X509)* cert_stack)
{
    IP_FILE *fp;
    X509  *cert;

    fp = ipcom_fopen(file, "r");
    if (fp == IP_NULL)
        return IPCOM_ERR_FAILED;

    while ((cert = PEM_read_X509(fp, IP_NULL, IP_NULL, IP_NULL)) != IP_NULL)
    {
        sk_X509_push(cert_stack, cert);
    }

    ERR_clear_error();
    ERR_remove_state(0);
    ipcom_fclose(fp);

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_send_log_cert
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_send_log_cert(X509 *cert, int _index)
{
    X509_NAME *issuer;
    X509_NAME *subject;
    char log_buf[128];

    issuer = X509_get_issuer_name(cert);
    if (!issuer)
        return;
    X509_NAME_oneline(issuer, log_buf, sizeof(log_buf));
    IPCOM_LOG2(INFO, "[%d]Issuer:  %s", _index, log_buf);
    subject = X509_get_subject_name(cert);
    if (!subject)
        return;
    X509_NAME_oneline(subject, log_buf, sizeof(log_buf));
    IPCOM_LOG2(INFO, "[%d]Subject: %s", _index, log_buf);
}


/*
 *===========================================================================
 *                    ipnet_send_log_cert_stack
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_send_log_cert_stack(STACK_OF(X509)* cert_stack)
{
    int i;
    X509 *cert;

    for (i=0; i<(sk_X509_num(cert_stack)); i++)
    {
        cert = sk_X509_value(cert_stack, i);
        ipnet_send_log_cert(cert, i);
    }
}


/*
 *===========================================================================
 *                    ipnet_send_prefix_map_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_send_prefix_map_init(void)
{
    ipcom_list_init(&ipnet_send_prefix_map);
}


/*
 *===========================================================================
 *                    ipnet_send_prefix_map_free
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_send_prefix_map_free(void)
{
    Ipnet_send_prefix_map_entry *map_entry;

    for (map_entry = IPCOM_LIST_FIRST(&ipnet_send_prefix_map);
         map_entry;
         map_entry = IPCOM_LIST_FIRST(&ipnet_send_prefix_map))
    {
        ipnet_send_prefix_map_entry_free(map_entry);
    }
}


/*
 *===========================================================================
 *                    ipnet_send_prefix_map_entry_free
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_send_prefix_map_entry_free(Ipnet_send_prefix_map_entry *map_entry)
{
    ipcom_list_remove(&map_entry->list);

    sk_X509_pop_free(map_entry->cert_chain, X509_free);
    if (map_entry->router_cert)
        X509_free(map_entry->router_cert);
    if(IPCOM_TMO2_PENDING(&map_entry->tmo))
        ipcom_tmo2_cancel(&map_entry->tmo);
    if (map_entry->rsa)
        RSA_free(map_entry->rsa);

    ipcom_free(map_entry);
}


/*
 *===========================================================================
 *                    ipnet_send_nonce_tmo_cb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_send_nonce_tmo_cb(void* cookie)
{
    Ipnet_send_nonce_entry *nonce_entry = cookie;

    IPCOM_LOG2(DEBUG, "ipnet_send_nonce_tmo_cb :: peer: %s, icmp type: %d",
               ipcom_inet_ntop(IP_AF_INET6,
                               &nonce_entry->peer,
                               ipnet_send_log_buf,
                               sizeof(ipnet_send_log_buf)),
               nonce_entry->icmp6_type);
    (void)ipcom_hash_remove(ipnet_send_nonce_hash_tbl, nonce_entry);
    ipnet_send_nonce_entry_free(nonce_entry);

    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_send_prefix_map_tmo_cb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_send_prefix_map_tmo_cb(void* cookie)
{
    Ipnet_send_prefix_map_entry *map_entry = cookie;

    IPCOM_LOG2(DEBUG, "ipnet_send_prefix_map_tmo_cb :: prefix: %s, router addr: %s",
               ipcom_inet_ntop(IP_AF_INET6,
                               &map_entry->prefix,
                               ipnet_send_log_buf,
                               IP_INET6_ADDRSTRLEN),
               ipcom_inet_ntop(IP_AF_INET6,
                               &map_entry->router_addr,
                               ipnet_send_log_buf+IP_INET6_ADDRSTRLEN,
                               sizeof(ipnet_send_log_buf)-IP_INET6_ADDRSTRLEN));


    ipnet_send_prefix_map_entry_free(map_entry);

    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_send_prefix_map_get_entry
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipnet_send_prefix_map_entry*
ipnet_send_prefix_map_get_entry(struct Ip_in6_addr *router_addr,
                                struct Ip_in6_addr *prefix,
                                Ip_bool create)
{
    Ipnet_send_prefix_map_entry *map_entry;

    for (map_entry = IPCOM_LIST_FIRST(&ipnet_send_prefix_map);
         map_entry;
         map_entry = IPCOM_LIST_NEXT(&map_entry->list))
    {
        if (ipcom_memcmp(&map_entry->router_addr, router_addr, 16) == 0)
        {
            if (ipcom_memcmp(&map_entry->prefix, prefix, 8) == 0)
                return map_entry;
        }
    }

    if (!create)
        return IP_NULL;

    map_entry = ipcom_calloc(1, sizeof(Ipnet_send_prefix_map_entry));
    if (!map_entry)
        return IP_NULL;
    ipcom_memcpy(&map_entry->router_addr, router_addr, 16);
    ipcom_memcpy(&map_entry->prefix, prefix, 16);
    ipcom_list_insert_first(&ipnet_send_prefix_map, &map_entry->list);
    map_entry->cert_chain = sk_X509_new_null();

    return map_entry;
}


/*
 *===========================================================================
 *                    ipnet_send_prefix_map_get_entry_by_id
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipnet_send_prefix_map_entry*
ipnet_send_prefix_map_get_entry_by_id(Ip_u16 id)
{
    Ipnet_send_prefix_map_entry *map_entry;

    for (map_entry = IPCOM_LIST_FIRST(&ipnet_send_prefix_map);
         map_entry;
         map_entry = IPCOM_LIST_NEXT(&map_entry->list))
    {
        if (map_entry->id == id)
                return map_entry;
    }

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_send_bin2hex
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char
ipnet_send_bin2hex(Ip_u8 val)
{
    if (val <= 9)
        return '0' + val;
    else
        return  'a' + val-10;
}


/*
 *===========================================================================
 *                    ipnet_send_nonce_obj_func
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_send_nonce_obj_func(Ipnet_send_nonce_entry *obj)
{
    unsigned tmp;

    tmp = ipcom_hash_update(&obj->peer, sizeof(obj->peer), 0);
    return ipcom_hash_update(&obj->icmp6_type, sizeof(obj->icmp6_type), tmp);

}


/*
 *===========================================================================
 *                    ipnet_send_nonce_key_func
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_send_nonce_key_func(Ipnet_send_nonce_key *key)
{
    unsigned tmp;

    tmp = ipcom_hash_update(key->peer, sizeof(struct Ip_in6_addr), 0);
    return ipcom_hash_update(&key->icmp6_type, sizeof(key->icmp6_type), tmp);
}


/*
 *===========================================================================
 *                    ipnet_send_nonce_entry_free
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_send_nonce_entry_free(Ipnet_send_nonce_entry *nonce_entry)
{
    if (IPCOM_TMO2_PENDING(&nonce_entry->tmo))
        ipcom_tmo2_cancel(&nonce_entry->tmo);

    ipcom_free(nonce_entry);
}


/*
 *===========================================================================
 *                    ipnet_send_nonce_cmp_func
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipnet_send_nonce_cmp_func(Ipnet_send_nonce_entry *obj, Ipnet_send_nonce_key *key)
{
    if (obj->icmp6_type != key->icmp6_type)
        return IP_FALSE;
    if (ipcom_memcmp(&obj->peer, key->peer, sizeof(obj->peer)) != 0)
        return IP_FALSE;

    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipnet_send_nonce_cache_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipnet_send_nonce_cache_init(void)
{
    ipnet_send_nonce_hash_tbl = ipcom_hash_new((Ipcom_hash_obj_func) ipnet_send_nonce_obj_func,
                                               (Ipcom_hash_key_func) ipnet_send_nonce_key_func,
                                               (Ipcom_hash_cmp_func) ipnet_send_nonce_cmp_func);
    if (!ipnet_send_nonce_hash_tbl)
    {
        return IPCOM_ERR_FAILED;
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_send_nonce_cache_for_each_cb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_send_nonce_cache_for_each_cb(void *obj, void *data)
{
    Ipnet_send_nonce_entry *nonce_entry;

    (void)data;

    nonce_entry = obj;
    ipnet_send_nonce_entry_free(nonce_entry);
}


/*
 *===========================================================================
 *                    ipnet_send_nonce_cache_free
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_send_nonce_cache_free(void)
{
    ipcom_hash_for_each(ipnet_send_nonce_hash_tbl, ipnet_send_nonce_cache_for_each_cb, IP_NULL);
    ipcom_hash_delete(ipnet_send_nonce_hash_tbl);
}


/*
 *===========================================================================
 *                    ipnet_send_nonce_cache_get
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipnet_send_nonce_cache_get(struct Ip_in6_addr *peer, Ip_u8 icmp6_type, Ip_u8 *buf, int *len)
{
    Ipnet_send_nonce_entry *nonce_entry;
    Ipnet_send_nonce_key key;

    key.peer = peer;
    key.icmp6_type = icmp6_type;

    nonce_entry = ipcom_hash_get(ipnet_send_nonce_hash_tbl, &key);
    if (!nonce_entry)
        return IPCOM_ERR_FAILED;

    if (nonce_entry->len > *len)
        return IPCOM_ERR_FAILED;

    ipcom_memcpy(buf, nonce_entry->nonce, nonce_entry->len);
    *len = nonce_entry->len;

#ifdef IP_DEBUG
    {
        int i;
        Ip_u8 tmp_buf[2*(IPNET_SEND_NONCE_BYTES)+1];

        for (i=0; i<IPNET_SEND_NONCE_BYTES; i++)
        {
            tmp_buf[2*i] = ipnet_send_bin2hex(buf[i] >> 4);
            tmp_buf[2*i+1] = ipnet_send_bin2hex(buf[i] & 15);
        }
        tmp_buf[2*i] = 0;
        IPCOM_LOG3(DEBUG, "nonce_cache_get :: peer: %s, icmp type: %d, nonce: 0x%s",
                   ipcom_inet_ntop(IP_AF_INET6, peer, ipnet_send_log_buf, sizeof(ipnet_send_log_buf)),
                   icmp6_type,
                   tmp_buf);
    }
#endif

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_send_nonce_cache_add
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipnet_send_nonce_cache_add(struct Ip_in6_addr *peer, Ip_u8 icmp6_type, Ip_u8 *nonce, int len)
{
    Ipnet_send_nonce_entry *nonce_entry;
    Ipnet_send_nonce_key key;

    key.peer = peer;
    key.icmp6_type = icmp6_type;

    nonce_entry = ipcom_hash_get(ipnet_send_nonce_hash_tbl, &key);
    if (nonce_entry)
    {
        if (ipcom_hash_remove(ipnet_send_nonce_hash_tbl, nonce_entry) == IPCOM_SUCCESS)
            ipnet_send_nonce_entry_free(nonce_entry);
        else
            IPCOM_LOG0(ERR, "Failed to remove nonce cache entry");
    }
    else
    {
            IPCOM_LOG0(DEBUG, "nonce cache add, no previous entry");
    }
    nonce_entry = ipcom_calloc(1, sizeof(Ipnet_send_nonce_entry));
    if (!nonce_entry)
        return IPCOM_ERR_FAILED;

    ipcom_memcpy(&nonce_entry->peer, peer, sizeof(struct Ip_in6_addr));
    ipcom_memcpy(nonce_entry->nonce, nonce, len);
    nonce_entry->len = len;
    nonce_entry->icmp6_type = icmp6_type;
    if (ipcom_hash_add(ipnet_send_nonce_hash_tbl, nonce_entry) != IPCOM_SUCCESS)
    {
        ipcom_free(nonce_entry);
        return IPCOM_ERR_FAILED;
    }

    if (IPCOM_TMO2_PENDING(&nonce_entry->tmo))
        ipcom_tmo2_cancel(&nonce_entry->tmo);
    ipcom_tmo2_request(ipnet_send_tmo,
                       &nonce_entry->tmo,
                       ipnet_send_nonce_tmo_cb,
                       nonce_entry,
                       30000);

    {
        int i;
        Ip_u8 tmp_buf[2*(IPNET_SEND_NONCE_BYTES_MAX)+1];

        for (i=0; i<len; i++)
        {

            tmp_buf[2*i] = ipnet_send_bin2hex(nonce[i] >> 4);
            tmp_buf[2*i+1] = ipnet_send_bin2hex(nonce[i] & 15);
        }
        tmp_buf[2*i]=0;
        IPCOM_LOG3(DEBUG, "nonce_cache_add :: peer: %s, icmp type: %d, nonce: 0x%s",
                   ipcom_inet_ntop(IP_AF_INET6, peer, ipnet_send_log_buf, sizeof(ipnet_send_log_buf)),
                   icmp6_type,
                   tmp_buf);
    }
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_send_icmp6_opt_cert_add
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipnet_send_icmp6_opt_cert_add(Ip_u8 *buf, Ip_size_t *buf_len, X509* cert)
{
    Ipnet_pkt_nd_opt_cert *opt_cert;
    int len;
    int pad_len;
    Ip_u8 *ptr;

    opt_cert = (Ipnet_pkt_nd_opt_cert*)buf;
    opt_cert->nd_opt_cert = IPNET_ND_OPT_CERTIFICATE;
    opt_cert->nd_opt_cert_type = IPNET_SEND_ICMP6_OPT_CERT_TYPE_X509;
    opt_cert->nd_opt_cert_reserved = 0;
    ptr = opt_cert->nd_opt_cert_value;
    len = i2d_X509(cert, &ptr);
    if (len < 0)
    {
        IPCOM_LOG0(ERR, "ipnet_send_icmp6_opt_cert_add :: i2d_X509() failed");
        return IPCOM_ERR_FAILED;
    }
    len += 4; /* Include opt header */
    if (len%8)
    {
        pad_len = 8-(len%8);
        ipcom_memset(ptr, 0, pad_len); /* Set pad field to zero */
    }
    else
        pad_len = 0;
    opt_cert->nd_opt_cert_len = (len+pad_len)/8;
    ip_assert((Ip_size_t)opt_cert->nd_opt_cert_len*8 <= *buf_len);

    *buf_len = opt_cert->nd_opt_cert_len*8;

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_send_icmp6_opt_trust_anchor_add
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipnet_send_icmp6_opt_trust_anchor_add(Ip_u8 *buf, int *buf_len, X509* cert)
{
    X509_NAME *x509_name;
    Ipnet_pkt_nd_opt_trust_anchor *opt;
    int len;
    Ip_u8 *ptr;

    opt = (Ipnet_pkt_nd_opt_trust_anchor*)buf;
    x509_name = X509_get_issuer_name(cert);
    if (!x509_name)
        return IPCOM_ERR_FAILED;
    len = i2d_X509_NAME(x509_name, IP_NULL);
    if (len + (8-len%8) > *buf_len - 4 - 4)
    {
        return IPCOM_ERR_INVALID_ARG;
    }
    ptr = opt->nd_opt_ta_value;
    (void)i2d_X509_NAME(x509_name, &ptr);
    if ((len+4)%8)
        opt->nd_opt_ta_pad_len = 8-(len+4)%8;
    else
        opt->nd_opt_ta_pad_len = 0;
    opt->nd_opt_ta_len = (len+4+opt->nd_opt_ta_pad_len)/8;
    ip_assert((len+4+opt->nd_opt_ta_pad_len)%8 ==0);
    opt = (Ipnet_pkt_nd_opt_trust_anchor*)buf;
    opt->nd_opt_ta_name_type = IPNET_SEND_ICMP6_OPT_CERT_TYPE_X509;
    opt->nd_opt_ta = IPNET_ND_OPT_TRUST_ANCHOR;

    *buf_len = opt->nd_opt_ta_len*8;

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_send_add_nonce_opt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipnet_send_add_nonce_opt(struct Ip_in6_addr *dst, Ip_u8 icmp6_type, Ip_u8* buf, int *len)
{
    Ipnet_pkt_nd_opt_hdr *opt;
    Ip_u8 nonce[IPNET_SEND_NONCE_BYTES_MAX];
    int nonce_len = sizeof(nonce);

    switch (icmp6_type)
    {
    case IPNET_ND_ROUTER_SOLICIT:
    case IPNET_ND_NEIGHBOR_SOLICIT:
        RAND_bytes(nonce, IPNET_SEND_NONCE_BYTES);
        if (ipnet_send_nonce_cache_add(dst, icmp6_type, nonce, IPNET_SEND_NONCE_BYTES) != IPCOM_SUCCESS)
            return IPCOM_ERR_FAILED;
        nonce_len = IPNET_SEND_NONCE_BYTES;
        break;

    case IPNET_ND_ROUTER_ADVERT:
        /* If ipnet_send_nonce_cache_get succeeds, this is a response to a router
         * solicitation message. Otherwise it is an unsolicitated router advertisement
         * message. */
        if (ipnet_send_nonce_cache_get(dst, IPNET_ND_ROUTER_SOLICIT, nonce, &nonce_len) != IPCOM_SUCCESS)
        {
            RAND_bytes(nonce, IPNET_SEND_NONCE_BYTES);
            nonce_len = IPNET_SEND_NONCE_BYTES;
        }
        break;

    case IPNET_ND_NEIGHBOR_ADVERT:
        if (ipnet_send_nonce_cache_get(dst, IPNET_ND_NEIGHBOR_SOLICIT, nonce, &nonce_len) != IPCOM_SUCCESS)
            return IPCOM_ERR_FAILED;
        break;

    case IPNET_ND_REDIRECT:
        RAND_bytes(nonce, IPNET_SEND_NONCE_BYTES);
        nonce_len = IPNET_SEND_NONCE_BYTES;
        break;
    }

    opt = (Ipnet_pkt_nd_opt_hdr*)buf;
    opt->nd_opt_type = IPNET_ND_OPT_NONCE;
    opt->nd_opt_len = (nonce_len+2)/8;
    ipcom_memcpy(opt->nd_opt_data, nonce, nonce_len);

    *len = nonce_len + 2;

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_send_verify_nonce_opt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipnet_send_verify_nonce_opt(struct Ip_in6_addr *peer, Ip_u8 icmp_type, Ipnet_pkt_nd_opt_hdr *opt)
{
    Ip_u8 nonce[IPNET_SEND_NONCE_BYTES_MAX];
    int nonce_len = sizeof(nonce);

    switch (icmp_type)
    {
    case IPNET_ND_ROUTER_SOLICIT:
    case IPNET_ND_NEIGHBOR_SOLICIT:
        ipnet_send_nonce_cache_add(peer, icmp_type, opt->nd_opt_data, opt->nd_opt_len*8-2);
        return IPCOM_SUCCESS;

    case IPNET_ND_ROUTER_ADVERT:
        if (ipnet_send_nonce_cache_get(peer, IPNET_ND_ROUTER_SOLICIT, nonce, &nonce_len) == IPCOM_SUCCESS)
        {
            if (nonce_len != opt->nd_opt_len*8-2)
                return IPCOM_ERR_FAILED;

            if (ipcom_memcmp(opt->nd_opt_data, nonce, nonce_len) == 0)
                return IPCOM_SUCCESS;
            else
                return IPCOM_ERR_FAILED;
        }

        /* Unsolicitated advert, no nonce verification to be performed */
        return IPCOM_SUCCESS;

    case IPNET_ND_NEIGHBOR_ADVERT:
        ipnet_send_nonce_cache_get(peer, IPNET_ND_NEIGHBOR_SOLICIT, nonce, &nonce_len);
        if (nonce_len != opt->nd_opt_len*8-2)
            return IPCOM_ERR_FAILED;
        if (ipcom_memcmp(opt->nd_opt_data, nonce, nonce_len) == 0)
            return IPCOM_SUCCESS;
        else
            return IPCOM_ERR_FAILED;

    case IPNET_ND_REDIRECT:
        break;

    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_send_add_timestamp_opt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_send_add_timestamp_opt(Ip_u8* buf)
{
    Ipnet_pkt_nd_opt_time_stamp *opt;
    Ip_time_t secs;
    Ip_u8 *tmp;

    opt = (Ipnet_pkt_nd_opt_time_stamp*)buf;
    opt->nd_opt_time_stamp = IPNET_ND_OPT_TIMESTAMP;
    opt->nd_opt_time_stamp_len = 2;
    ipcom_memset(&opt->nd_opt_time_stamp_reserved,0, 14);
    secs = ipcom_time(IP_NULL);
    /* Time stamp is 64 bits. The least significant bits are
     * fractions of seconds and the remaining 48 bits are seconds
     * since epoch start. Since 'our' time is represented as 32 bits
     * and no fractions, these shall be put in the middle, e.g.
     * bytes 2-5. Remaining bytes are set to zero. */
    tmp = (Ip_u8*)&opt->nd_opt_time_stamp_value;
    tmp += 2;
    IP_SET_32ON8(tmp, ip_htonl(secs));
}


/*
 *===========================================================================
 *                    ipnet_send_verify_timestamp_opt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipnet_send_verify_timestamp_opt(Ipnet_send_conf *conf, Ipnet_pkt_nd_opt_hdr *opt)
{
    Ipnet_pkt_nd_opt_time_stamp *ts_opt;
    Ip_time_t recv_secs;
    Ip_time_t local_secs;
    Ip_u8    *tmp;

    ts_opt = (Ipnet_pkt_nd_opt_time_stamp*)opt;
    if (ts_opt->nd_opt_time_stamp_len != 2)
        return IPCOM_ERR_INVALID_ARG;

    local_secs = ipcom_time(IP_NULL);

    tmp = (Ip_u8*)&ts_opt->nd_opt_time_stamp_value;
    tmp += 2;
    recv_secs = ip_ntohl(IP_GET_32ON8(tmp));

    if (IP_ABS(recv_secs-local_secs)>conf->timestamp_delta)
    {
        IPCOM_LOG3(DEBUG, "Invalid time stamp: %d, localtime %d, diff %d",
                   recv_secs, local_secs, IP_ABS(recv_secs-local_secs));
        return IPCOM_ERR_FAILED;
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_send_verify_rsa_sign_opt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipnet_send_verify_rsa_sign_opt(Ipcom_pkt *pkt, int min_sec)
{
	EVP_MD_CTX                   ctx;
    EVP_PKEY                    *pkey = IP_NULL;
    RSA                         *rsa = IP_NULL;
    Ipnet_pkt_ip6               *ip6_hdr;
    Ipnet_pkt_icmp6             *icmp6_hdr;
    Ip_u32                       chk_sum;
    int                          icmp6_tmp_len;
    Ipnet_pkt_nd_opt_hdr        *first_opt;
    Ipnet_pkt_nd_opt_rsa_sign   *opt_rsa;
    Ipnet_pkt_nd_opt_cga        *opt_cga;
    struct Ip_in6_addr          *cga_addr;
    Ip_err                       rc = IPCOM_ERR_FAILED;
    int                          tot_opt_len;
    int                          sec;

    ip6_hdr = (Ipnet_pkt_ip6*)&pkt->data[pkt->ipstart];
    icmp6_hdr = (Ipnet_pkt_icmp6*)&ip6_hdr[1];

    first_opt = ipnet_icmp6_find_opts(pkt, &tot_opt_len);
    if (!first_opt)
        return IPCOM_ERR_FAILED;
    opt_rsa = (Ipnet_pkt_nd_opt_rsa_sign*)ipnet_icmp6_get_option(first_opt,
                                                                 tot_opt_len,
                                                                 IPNET_ND_OPT_RSA_SIGNATURE,
                                                                 0);
    if (!opt_rsa)
        return IPCOM_ERR_FAILED;
    opt_cga = (Ipnet_pkt_nd_opt_cga*)ipnet_icmp6_get_option(first_opt,
                                                            tot_opt_len,
                                                            IPNET_ND_OPT_CGA,
                                                            0);
    if (!opt_cga)
        return IPCOM_ERR_FAILED;

    if (ipcom_memcmp(&ip6_hdr->src, &ip_in6addr_any, 16) == 0 &&
        icmp6_hdr->icmp6_type == IPNET_ND_NEIGHBOR_SOLICIT)
    {
        Ipnet_pkt_nd_neighbor_solicit *sol;
        sol = (Ipnet_pkt_nd_neighbor_solicit*)icmp6_hdr;
        cga_addr = &sol->nd_ns_target;
    }
    else
        cga_addr = &ip6_hdr->src;

    sec = ipnet_cga_get_sec(cga_addr);
    if (sec < min_sec)
    {
        IPCOM_LOG3(ERR, "Too small sec (%d, %d required) in addr %s",
                   sec, min_sec,
                   ipcom_inet_ntop(IP_AF_INET6,
                                   cga_addr,
                                   ipnet_send_log_buf,
                                   sizeof(ipnet_send_log_buf)));
        goto cleanup;
    }

    if (ipnet_cga_icmp6_opt_verify(opt_cga, cga_addr) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(DEBUG, "Failed to verify CGA option");
        goto cleanup;
    }

    pkey = EVP_PKEY_new();
    if (!pkey)
        return IPCOM_ERR_FAILED;

    rsa = ipnet_cga_icmp6_opt_get_rsa_key(opt_cga);
    if (!rsa)
        goto cleanup;
    if (!EVP_PKEY_set1_RSA(pkey, rsa))
        goto cleanup;

    /* Create temporary checksum. Needed in signature verification below. */
    icmp6_tmp_len = pkt->end - pkt->start - opt_rsa->nd_opt_rsa_sign_len*8; /* Length of icmp packet, excl RSA sign opt */
    icmp6_hdr->icmp6_cksum = 0;

    chk_sum = ipcom_in_checksum_update(icmp6_hdr, icmp6_tmp_len);
    chk_sum += ipnet_ip6_pseudo_header_checksum_update(&ip6_hdr->src,
                                                       &ip6_hdr->dst,
                                                       ip6_hdr->nxt,
                                                       icmp6_tmp_len);
    icmp6_hdr->icmp6_cksum = ipcom_in_checksum_finish(chk_sum);

    /* TODO: check key hash */

    /* Verify signature */
    EVP_MD_CTX_init(&ctx);
	if (EVP_VerifyInit(&ctx, EVP_sha1()) != 1)
    {
        IPCOM_LOG0(DEBUG, "Failed to verify NDP message, EVP_VerifyInit failed");
        goto cleanup;
    }
    if (EVP_VerifyUpdate(&ctx,ipnet_send_cga_msg_type_tag, sizeof(ipnet_send_cga_msg_type_tag)) != 1)
    {
        IPCOM_LOG0(DEBUG, "Failed to verify NDP message, EVP_VerifyUpdate failed (msg type tag)");
        goto cleanup;
    }
    IPCOM_LOG0(DEBUG2, "Sign verify, CGA msg type tag:");
    ipnet_send_log_hex_buffer(ipnet_send_cga_msg_type_tag, sizeof(ipnet_send_cga_msg_type_tag));

    if (EVP_VerifyUpdate(&ctx, &ip6_hdr->src, sizeof(ip6_hdr->src)) != 1)
    {
        IPCOM_LOG0(DEBUG, "Failed to verify NDP message, EVP_VerifyUpdate failed (src addr)");
        goto cleanup;
    }
    IPCOM_LOG0(DEBUG2, "Sign verify, src addr:");
    ipnet_send_log_hex_buffer((Ip_u8*)&ip6_hdr->src, sizeof(ip6_hdr->src));

    if (EVP_VerifyUpdate(&ctx, &ip6_hdr->dst, sizeof(ip6_hdr->dst)) != 1)
    {
        IPCOM_LOG0(DEBUG, "Failed to verify NDP message, EVP_VerifyUpdate failed (dst addr)");
        goto cleanup;
    }
    IPCOM_LOG0(DEBUG2, "Sign verify, dst addr:");
    ipnet_send_log_hex_buffer((Ip_u8*)&ip6_hdr->dst, sizeof(ip6_hdr->dst));

    if (EVP_VerifyUpdate(&ctx,icmp6_hdr, icmp6_tmp_len) != 1)
    {
        IPCOM_LOG0(DEBUG, "Failed to verify NDP message, EVP_VerifyUpdate failed (icmp6 hdr)");
        goto cleanup;
    }
    IPCOM_LOG0(DEBUG2, "Sign verify, icmp header:");
    ipnet_send_log_hex_buffer((Ip_u8*)icmp6_hdr, icmp6_tmp_len);

    if (EVP_PKEY_size(pkey) > opt_rsa->nd_opt_rsa_sign_len*8-20)
    {
        IPCOM_LOG1(DEBUG, "Failed to verify NDP message, too short signature: %d bytes",
                   opt_rsa->nd_opt_rsa_sign_len*8-20);
        goto cleanup;
    }
    if (EVP_VerifyFinal(&ctx,opt_rsa->nd_opt_rsa_sign, EVP_PKEY_size(pkey), pkey) != 1)
    {
        IPCOM_LOG0(DEBUG, "Failed to verify NDP message, EVP_VerifyFinal failed");
        goto cleanup;
    }
    (void)EVP_MD_CTX_cleanup(&ctx);

    rc =IPCOM_SUCCESS;

 cleanup:
    if (pkey)
        EVP_PKEY_free(pkey);
    if (rsa)
        RSA_free(rsa);

    return rc;
}


/*
 *===========================================================================
 *                    ipnet_send_add_rsa_sign_key_hash
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_send_add_rsa_sign_key_hash(Ipnet_pkt_nd_opt_rsa_sign* opt_rsa_sign, Ip_u8* der_rsa_key, int der_key_len)
{
    Ip_u8 tmp_buf[SHA_DIGEST_LENGTH];

    SHA1(der_rsa_key, der_key_len, tmp_buf);

    ipcom_memcpy(opt_rsa_sign->nd_opt_rsa_sign_key_hash,
                 tmp_buf,
                 sizeof(opt_rsa_sign->nd_opt_rsa_sign_key_hash));
}


/*
 *===========================================================================
 *                    ipnet_send_conf_free
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_send_conf_free(Ipnet_send_conf *conf)
{
    if (conf->cert_chain)
        sk_X509_pop_free(conf->cert_chain, X509_free);
    if (conf->netlink_sock != IP_INVALID_SOCKET)
        ipcom_socketclose(conf->netlink_sock);
    if (conf->netlink_sock2 != IP_INVALID_SOCKET)
        ipcom_socketclose(conf->netlink_sock2);
    if (conf->icmp6_sock != IP_INVALID_SOCKET)
        ipcom_socketclose(conf->icmp6_sock);
    if (conf->rsa_key)
        RSA_free(conf->rsa_key);
    if (conf->pkey)
        EVP_PKEY_free(conf->pkey);
    if (conf->der_rsa_key)
        ipcom_free(conf->der_rsa_key);
    if (conf->ca_cert)
        X509_free(conf->ca_cert);
    if (conf->cert)
        X509_free(conf->cert);
    if (conf->intermediate_certs)
        sk_X509_pop_free(conf->intermediate_certs, X509_free);
    if (conf->crl_st)
        sk_X509_CRL_pop_free(conf->crl_st, X509_CRL_free);

    ipcom_free(conf);
}


/*
 *===========================================================================
 *                    ipnet_send_conf_new
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipnet_send_conf*
ipnet_send_conf_new(void)
{
    Ipnet_send_conf* new_conf;

    new_conf = ipcom_calloc(1,sizeof(Ipnet_send_conf));
    if (!new_conf)
        return IP_NULL;

    new_conf->netlink_sock = IP_INVALID_SOCKET;
    new_conf->netlink_sock2 = IP_INVALID_SOCKET;
    new_conf->icmp6_sock = IP_INVALID_SOCKET;

    new_conf->intermediate_certs = sk_X509_new_null();
    if (new_conf->intermediate_certs == IP_NULL)
        goto err_out;

    new_conf->crl_st = sk_X509_CRL_new_null();
    if (new_conf->crl_st == IP_NULL)
        goto err_out;

    return new_conf;

 err_out:
    if (new_conf->intermediate_certs)
        sk_X509_free(new_conf->intermediate_certs);

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_send_addr_add
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipnet_send_addr_add(Ipnet_send_conf *conf,
                    struct Ip_in6_addr *addr,
                    Ip_u8 prefix_len,
                    Ip_u8 *modifier,
                    int ifindex,
                    Ip_u32 prefered_time,
                    Ip_u32 valid_time)
{
    struct Ip_rtattr             *rtattr;
    struct Ip_sockaddr_nl         nl_addr;
    struct Ip_ifa_cacheinfo*      cache_info;
    struct
    {
        struct Ip_nlmsghdr nlh;
        struct Ip_ifaddrmsg ifaddrmsg;
        Ip_u8 attr_buf[500];
    } req;
    char sysvar_name[80+IP_IFNAMSIZ];
    char if_name_buf[IP_IFNAMSIZ];
    char cga_modifier[33];

    /* Set modifier sysvar for this address */
    (void)ipcom_if_indextoname(ifindex, if_name_buf);
    ipcom_snprintf(sysvar_name, sizeof(sysvar_name), "ipnet.inet6.cga.%s.modifier.%s",
                   if_name_buf,
                   ipcom_inet_ntop(IP_AF_INET6, addr, ipnet_send_log_buf, sizeof(ipnet_send_log_buf)));
    ipnet_cga_modifier_bin2hex(modifier, 16, cga_modifier);
    if (ipcom_sysvar_set(sysvar_name, cga_modifier, IPCOM_SYSVAR_FLAG_OVERWRITE))
    {
        IPCOM_LOG2(ERR, "ipnet_send_addr_add :: failed to set CGA modifier sysvar: %s=%s",
                   sysvar_name, cga_modifier);
        return IPCOM_ERR_FAILED;
    }

    /* Create and send netlink msg */
    IPCOM_LOG1(DEBUG, "RA: Adding CGA addr: %s ",
               ipcom_inet_ntop(IP_AF_INET6, addr, ipnet_send_log_buf, sizeof(ipnet_send_log_buf)));
    ipcom_memset(&req, 0, sizeof(req));
    req.nlh.nlmsg_len = IP_NLMSG_LENGTH(sizeof(struct Ip_ifaddrmsg));
    req.nlh.nlmsg_flags = IP_NLM_F_REQUEST;
    req.nlh.nlmsg_type = IP_RTM_NEWADDR;

    req.ifaddrmsg.ifa_family = IP_AF_INET6;
    req.ifaddrmsg.ifa_prefixlen = prefix_len;
    req.ifaddrmsg.ifa_index = ifindex;
    req.ifaddrmsg.ifa_flags = IP_IFA_F_TENTATIVE | IP_IFA_F_CGA;

    rtattr = (struct Ip_rtattr *)(((char *) &req) + IP_NLMSG_ALIGN(req.nlh.nlmsg_len));
    rtattr->rta_type = IP_IFA_LOCAL;
    rtattr->rta_len = IP_RTA_LENGTH(sizeof(struct Ip_in6_addr));
    req.nlh.nlmsg_len = IP_NLMSG_ALIGN(req.nlh.nlmsg_len) + IP_RTA_LENGTH(sizeof(struct Ip_in6_addr));
    ipcom_memcpy(IP_RTA_DATA(rtattr), addr, sizeof(*addr));

    rtattr = (struct Ip_rtattr *)(((char *) &req) + IP_NLMSG_ALIGN(req.nlh.nlmsg_len));
    rtattr->rta_type = IP_IFA_CACHEINFO;
    rtattr->rta_len = IP_RTA_LENGTH(sizeof(struct Ip_ifa_cacheinfo));
    req.nlh.nlmsg_len = IP_NLMSG_ALIGN(req.nlh.nlmsg_len) + IP_RTA_LENGTH(sizeof(struct Ip_ifa_cacheinfo));
    cache_info = (struct Ip_ifa_cacheinfo*)IP_RTA_DATA(rtattr);
    cache_info->ifa_prefered = prefered_time;
    cache_info->ifa_valid = valid_time;

    ipcom_memset(&nl_addr, 0, sizeof(nl_addr));
    nl_addr.nl_len = sizeof(nl_addr);
    nl_addr.nl_family = IP_AF_NETLINK;
    ipcom_sendto(conf->netlink_sock2,
                 &req,
                 req.nlh.nlmsg_len,
                 0,
                 (struct Ip_sockaddr*)&nl_addr,
                 sizeof(nl_addr));

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_send_reconfigure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipnet_send_reconfigure(Ipnet_send_conf **ppconf)
{
    Ipnet_send_conf *old_conf = *ppconf;
    Ipnet_send_conf *new_conf;
    struct Ip_sockaddr_nl sockaddr_nl;
    const char *sysvar_key = "ipnet.inet6.send.rsa_key_id";
    char *key_id = IP_NULL;
    BIO *bio = IP_NULL;
    Ip_err rc = IPCOM_ERR_INVALID_CONFIG;
    Ip_bool switch_config = IP_FALSE;
    char *path = IP_NULL;
    char *path2 = IP_NULL;
    char *path3 = IP_NULL;

    new_conf = ipnet_send_conf_new();
    if (!new_conf)
    {
        IPCOM_LOG0(ERR, "ipnet_send_reconfigure :: out of memory");
        goto cleanup;
    }

    new_conf->netlink_sock = ipcom_socket(IP_AF_NETLINK, IP_SOCK_RAW, 0);
    new_conf->netlink_sock2 = ipcom_socket(IP_AF_NETLINK, IP_SOCK_RAW, 0);
    if (new_conf->netlink_sock == IP_INVALID_SOCKET || new_conf->netlink_sock2 == IP_INVALID_SOCKET)
    {
        IPCOM_LOG2(ERR, "ipnet_send_reconfigure :: failed to create netlink sockets,"
                   "errno=%d, %s", ipcom_errno, ipcom_strerror(ipcom_errno));
        goto cleanup;
    }

    /* Bind to the SEND Netlink group */
    ipcom_memset(&sockaddr_nl, 0, sizeof(sockaddr_nl));
    sockaddr_nl.nl_len = sizeof(sockaddr_nl);
    sockaddr_nl.nl_family = IP_AF_NETLINK;
    sockaddr_nl.nl_groups = IP_NLMSG_GROUP_MASK(IP_RTNLGRP_IPV6_SEND);
    if (ipcom_bind(new_conf->netlink_sock,
                   (struct Ip_sockaddr*)&sockaddr_nl,
                   sizeof(sockaddr_nl)) != IPCOM_SUCCESS)
    {
        IPCOM_LOG2(ERR, "ipnet_send_reconfigure :: failed to bind to netlink SEND group,"
                   "errno=%d, %s", ipcom_errno, ipcom_strerror(ipcom_errno));
        goto cleanup;
    }

    (void)ipcom_sysvar_get_descriptive_int0("ipnet.inet6.send.cert_path_mode",
                                            IPNET_SEND_CERT_PATH_MODE_OFF,
                                            &new_conf->cert_path_mode,
                                            ipnet_send_cert_path_mode_map);
    if (new_conf->cert_path_mode == IPNET_SEND_CERT_PATH_MODE_HOST ||
        new_conf->cert_path_mode == IPNET_SEND_CERT_PATH_MODE_ROUTER)
    {
        struct Ip_icmp6_filter filter;

        new_conf->icmp6_sock = ipcom_socket(IP_AF_INET6, IP_SOCK_RAW, IP_IPPROTO_ICMPV6);
        if (new_conf->icmp6_sock == IP_INVALID_SOCKET)
        {
            IPCOM_LOG2(ERR, "ipnet_send_reconfigure :: failed to create icmp6 socket,"
                       "errno=%d, %s", ipcom_errno, ipcom_strerror(ipcom_errno));
            goto cleanup;
        }

        IP_ICMP6_FILTER_SETBLOCKALL(&filter);
        if (new_conf->cert_path_mode == IPNET_SEND_CERT_PATH_MODE_ROUTER)
            IP_ICMP6_FILTER_SETPASS(IPNET_ND_CERT_PATH_SOLICIT, &filter);
        else
            IP_ICMP6_FILTER_SETPASS(IPNET_ND_CERT_PATH_ADVERT, &filter);
        if (ipcom_setsockopt(new_conf->icmp6_sock, IP_IPPROTO_ICMPV6, IP_ICMP6_FILTER,
                             &filter, sizeof(filter)) == IP_SOCKERR)
        {
            IPCOM_LOG2(ERR, "ipnet_send_reconfigure :: failed to set ICMPv6 filter: %s(%d)",
                       ipcom_strerror(ipcom_errno), ipcom_errno);
            goto cleanup;
        }
    }

    /* Read RSA key */
    key_id = ipcom_sysvar_get(sysvar_key, IP_NULL, IP_NULL);
    if (!key_id)
    {
        IPCOM_LOG1(ERR, "ipnet_send_reconfigure :: no RSA key ID specified (sysvar %s", sysvar_key);
        goto cleanup;
    }
    new_conf->pkey = ipcom_key_db_pkey_get(key_id);
    if (!new_conf->pkey)
    {
        IPCOM_LOG1(ERR, "ipnet_send_reconfigure :: no RSA key with ID = %s exists", key_id);
        goto cleanup;
    }
    if (new_conf->pkey->type != EVP_PKEY_RSA)
    {
            IPCOM_LOG1(ERR, "ipnet_send_reconfigure :: key with ID = %s is not an RSA key", key_id);
            goto cleanup;
    }
    new_conf->rsa_key = new_conf->pkey->pkey.rsa;
    new_conf->pkey->pkey.rsa->references++;

    /* DER encode RSA public key */
    new_conf->der_len = ipnet_cga_der_encode_rsa_public_key(&new_conf->der_rsa_key, new_conf->rsa_key);
    if(new_conf->der_len == 0)
    {
        IPCOM_LOG0(ERR, "ipnet_send_reconfigure :: failed to DER encode RSA key");
        goto cleanup;
    }

    if (new_conf->cert_path_mode != IPNET_SEND_CERT_PATH_MODE_OFF)
    {
        path = ipcom_sysvar_get("ipnet.inet6.send.ca_cert_file", IP_NULL, IP_NULL);
        if (!path)
        {
            IPCOM_LOG0(ERR, "ipnet.inet6.send.ca_cert_file sysvar not set, cannot load CA certificates");
            goto cleanup;
        }
        new_conf->ca_cert = ipnet_send_load_x509_file(path);
        if (!new_conf->ca_cert)
        {
            IPCOM_LOG1(ERR, "Failed to load CA certificate from file %s", path);
            goto cleanup;
        }

        if (new_conf->cert_path_mode == IPNET_SEND_CERT_PATH_MODE_ROUTER)
        {
            /* Create our certificate chain */
            path2 = ipcom_sysvar_get("ipnet.inet6.send.cert_file", IP_NULL, IP_NULL);
            if (!path2)
            {
                IPCOM_LOG0(ERR, "ipnet.inet6.send.cert_file sysvar not set, cannot load certificate");
                goto cleanup;
            }
            new_conf->cert = ipnet_send_load_x509_file(path2);
            if (!new_conf->cert)
            {
                IPCOM_LOG1(ERR, "Failed to load certificate from file %s", path2);
                goto cleanup;
            }
            ipcom_free(path2);

            path2 = ipcom_sysvar_get("ipnet.inet6.send.intermediate_ca_cert_file", IP_NULL, IP_NULL);
            if (!path2)
            {
                IPCOM_LOG0(WARNING, "sysvar ipnet.inet6.send.intermediate_ca_cert_file not set, no intermediate CA certificates loaded");
            }
            else if (ipnet_send_load_cert_st_file(path2, new_conf->intermediate_certs) != IPCOM_SUCCESS)
            {
                IPCOM_LOG1(WARNING, "Failed to load intermediate CA certificates from file %s", path2);
            }
            else
            {
                IPCOM_LOG0(DEBUG, "Loaded intermediate CA certs:");
                ipnet_send_log_cert_stack(new_conf->intermediate_certs);
            }
            if (ipnet_send_cert_chain_verify(new_conf->ca_cert,
                                             new_conf->intermediate_certs,
                                             new_conf->cert,
                                             IP_NULL,
                                             &new_conf->cert_chain) != IPCOM_SUCCESS)
            {
                IPCOM_LOG0(ERR, "Failed to create certificate chain:");
                goto cleanup;
            }
            IPCOM_LOG0(INFO, "Loaded certificate chain:");
            ipnet_send_log_cert_stack(new_conf->cert_chain);
        }
        if (new_conf->cert_path_mode == IPNET_SEND_CERT_PATH_MODE_HOST)
        {
            path3 = ipcom_sysvar_get("ipnet.inet6.send.crl_file", IP_NULL, IP_NULL);
            if (path3)
            {
                ipnet_send_load_crl_file(path3, &new_conf->crl_st);
            }
            else
            {
                IPCOM_LOG0(INFO, "ipnet.inet6.send.clr_file sysvar not set, no CLRs loaded");
            }
        }
    }
    new_conf->timestamp_delta = ipcom_sysvar_get_as_int("ipnet.inet6.send.timestamp_delta", 300);
    new_conf->min_sec = ipcom_sysvar_get_as_int("ipnet.inet6.send.min_sec", 0);

    switch_config = IP_TRUE;
    rc = IPCOM_SUCCESS;

cleanup:
    if (path)
        ipcom_free(path);
    if (path2)
        ipcom_free(path2);
    if (path3)
        ipcom_free(path3);
    if (key_id)
        ipcom_free(key_id);
    if (bio)
        BIO_free(bio);

    if (switch_config)
    {
        *ppconf = new_conf;
        if (old_conf)
            ipnet_send_conf_free(old_conf);
    }
    else
    {
        if (old_conf)
            /* We have an old config, continue using it */
            rc = IPCOM_ERR_INVALID_CONFIG;
        if (new_conf)
            ipnet_send_conf_free(new_conf);
    }

    return rc;
}


/*
 *===========================================================================
 *                    ipnet_send_cert_chain_verify
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipnet_send_cert_chain_verify(X509 *ca_cert,
                             STACK_OF(X509) *cert_chain,
                             X509 *router_cert,
                             STACK_OF(X509_CRL) *crls,
                             STACK_OF(X509) **verified_cert_chain)
{
    X509_STORE_CTX* ctx;
    X509_STORE*     store;
    STACK_OF(509)  *ca_st;
    Ip_err          rc = IPCOM_ERR_FAILED;

    ca_st = sk_X509_new_null();
    store = X509_STORE_new();
    ctx = X509_STORE_CTX_new();

    if (!ca_st || !store || !ctx)
    {
        IPCOM_LOG0(ERR, "ipnet_send_cert_chain_verify :: out of memory");
        goto cleanup;
    }

    (void)sk_X509_push(ca_st, ca_cert);

    (void)X509_STORE_CTX_init(ctx, store, router_cert, cert_chain);
    (void)X509_STORE_CTX_trusted_stack(ctx, ca_st);
    if (sk_X509_CRL_num(crls) > 0)
    {
        X509_STORE_CTX_set0_crls(ctx, crls);
        X509_STORE_CTX_set_flags(ctx, X509_V_FLAG_CRL_CHECK);
    }
    if (X509_verify_cert(ctx) == 1)
    {
        rc = IPCOM_SUCCESS;
        if (verified_cert_chain != IP_NULL)
        {
            *verified_cert_chain = X509_STORE_CTX_get1_chain(ctx);
        }
    }

 cleanup:
    if (ca_st)
        sk_X509_free(ca_st);
    if (ctx != IP_NULL)
        X509_STORE_CTX_free(ctx);
    if (store != IP_NULL)
        X509_STORE_free(store);

    return rc;
}


/*
 *===========================================================================
 *                    ipnet_send_process_router_adv
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_send_process_router_adv(Ipnet_send_conf *conf,
                              struct Ip_in6_addr *router_addr,
                              Ipnet_pkt_nd_opt_hdr *first_opt,
                              int tot_opt_len,
                              int ifindex)
{
    Ipnet_pkt_nd_opt_prefix_info *prefix_opt;
    int                           i;
    Ipnet_cga_param               cga_param;
    Ipnet_send_prefix_map_entry  *map_entry;
    struct Ip_timeval             tv;

    for (i=0; IP_TRUE; i++)
    {
        prefix_opt = (Ipnet_pkt_nd_opt_prefix_info*)
            ipnet_icmp6_get_option(first_opt,
                                   tot_opt_len,
                                   IPNET_ND_OPT_PREFIX_INFORMATION,
                                   i);
        if (!prefix_opt)
            return;

        /* Sanity check of prefix option */
        if (prefix_opt->nd_opt_pi_prefix_len != 64)
        {
            IPCOM_LOG1(WARNING, "Invalid prefix len received in RA prefix option: %d",
                       prefix_opt->nd_opt_pi_prefix_len);
            continue;
        }

        map_entry = ipnet_send_prefix_map_get_entry(router_addr,
                                                    &prefix_opt->nd_opt_pi_prefix,
                                                    IP_TRUE);
        if (!map_entry)
        {
            IPCOM_LOG0(ERR, "ipnet_send_process_router_adv :: failed to create map entry");
            continue;
        }
        map_entry->ifindex = ifindex;
        if (conf->cert_path_mode == IPNET_SEND_CERT_PATH_MODE_HOST && !map_entry->cert_validated)
        {
            struct Ip_sockaddr_in6        to;
            Ipnet_pkt_nd_opt_cga   *opt_cga;


            if (IPCOM_TMO2_PENDING(&map_entry->tmo))
                ipcom_tmo2_cancel(&map_entry->tmo);

            IPCOM_LOG0(DEBUG2, "ipcom_tmo2_request 1, duration = 20");
            ipcom_tmo2_request(ipnet_send_tmo,
                               &map_entry->tmo,
                               ipnet_send_prefix_map_tmo_cb,
                               map_entry,
                               20000);

            /* Store routers RSA key for verification with certificate once the certificate
             * chain is complete. */
            opt_cga = (Ipnet_pkt_nd_opt_cga*)ipnet_icmp6_get_option(first_opt,
                                                                    tot_opt_len,
                                                                    IPNET_ND_OPT_CGA,
                                                                    0);
            if (!opt_cga)
            {
                IPCOM_LOG0(ERR, "ipnet_send_process_router_adv :: no CGA option");
                return;
            }
            map_entry->rsa = ipnet_cga_icmp6_opt_get_rsa_key(opt_cga);
            if (!map_entry->rsa)
            {
                IPCOM_LOG0(ERR, "ipnet_send_process_router_adv :: failed to get router's RSA key");
                return;
            }

            map_entry->id = ipnet_send_cert_path_id++;
            map_entry->prefered_time = ip_ntohl(prefix_opt->nd_opt_pi_preferred_time);
            map_entry->valid_time = ip_ntohl(prefix_opt->nd_opt_pi_valid_time);
            ipcom_memcpy(&map_entry->prefix, &prefix_opt->nd_opt_pi_prefix, 8);
            ipcom_memcpy(&map_entry->router_addr, router_addr, 16);

            ipcom_memset(&to, 0, sizeof(to));
            (void)ipcom_inet_pton(IP_AF_INET6, "ff02::2", &to.sin6_addr);
            IPCOM_SA_LEN_SET(&to, sizeof(to));
            to.sin6_family   = IP_AF_INET6;
            to.sin6_scope_id = ifindex;
            if (ipnet_send_cert_path_solicit_output(conf,
                                                    IPNET_SEND_CERT_PATH_ALL_COMPONENTS,
                                                    map_entry->id,
                                                    (struct Ip_sockaddr_in*)&to,
                                                    sizeof(to)) != IPCOM_SUCCESS)
            {
                IPCOM_LOG0(ERR, "ipnet_send_process_router_adv :: failed to send cert path solicitation");
            }
            continue;
        }

        /* Check if we have already added a CGA address for this prefix. If not, create one. */
        if (map_entry->cga_addr_generated == IP_FALSE)
        {
            ipcom_memset(&cga_param, 0, sizeof(cga_param));
            cga_param.rsa_key = conf->rsa_key;
            cga_param.der_encoded_key = conf->der_rsa_key;
            cga_param.der_key_len = conf->der_len;
            cga_param.sec = conf->min_sec;
            ipcom_memcpy(cga_param.subnet_prefix, &prefix_opt->nd_opt_pi_prefix, 8);
            if (ipnet_cga_generate(&cga_param, &map_entry->cga_addr) != IPCOM_SUCCESS)
            {
                IPCOM_LOG0(ERR, "ipnet_send_process_router_adv :: failed to generate CGA address");
                continue;
            }
            map_entry->cga_addr_generated = IP_TRUE;
            ipcom_memcpy(map_entry->cga_modifier, cga_param.modifier, sizeof(map_entry->cga_modifier));
            IPCOM_LOG1(DEBUG, "RA: created CGA addr: %s ",
                       ipcom_inet_ntop(IP_AF_INET6, &map_entry->cga_addr, ipnet_send_log_buf, sizeof(ipnet_send_log_buf)));
        }

        if (ipnet_send_addr_add(conf,
                                &map_entry->cga_addr,
                                64,
                                map_entry->cga_modifier,
                                ifindex,
                                ip_ntohl(prefix_opt->nd_opt_pi_preferred_time),
                                ip_ntohl(prefix_opt->nd_opt_pi_valid_time)) != IPCOM_SUCCESS)
        {
            char log_buf[IP_IFNAMSIZ];
            IPCOM_LOG2(ERR, "RA: failed to add CGA addr %s if i/f %s",
                       ipcom_inet_ntop(IP_AF_INET6, &map_entry->cga_addr, ipnet_send_log_buf, sizeof(ipnet_send_log_buf)),
                       ipcom_if_indextoname(ifindex, log_buf));

        }
        if (IPCOM_TMO2_PENDING(&map_entry->tmo))
            ipcom_tmo2_cancel(&map_entry->tmo);
        /* Delete map entry when prefix expires */
        tv.tv_sec = ip_ntohl(prefix_opt->nd_opt_pi_valid_time);
        tv.tv_usec = 0;
        IPCOM_LOG1(DEBUG2, "ipcom_tmo2_request 2, duration = %d", tv.tv_sec);
        ipcom_tmo2_request_timeval(ipnet_send_tmo,
                                   &map_entry->tmo,
                                   ipnet_send_prefix_map_tmo_cb,
                                   map_entry, &tv);

    }
}


/*
 *===========================================================================
 *                    ipnet_send_cert_path_advert_output2
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_send_cert_path_advert_output2(Ipnet_send_conf *conf,
                                    X509 *cert,
                                    Ip_u16 component,
                                    Ip_u16 all_components,
                                    Ip_u16 id,
                                    struct Ip_sockaddr_in6* addr,
                                    Ip_socklen_t addr_len)
{
    int buf_len;
    Ip_size_t tmp_len;
    Ip_u8 *buf;
    Ip_u8 *tmp_buf;
    Ipnet_pkt_nd_cert_path_advert *cert_path_adv;


    buf_len = i2d_X509(cert, IP_NULL);
    if (buf_len < 0)
    {
        IPCOM_LOG0(ERR, "ipnet_send_cert_path_advert_output2 :: i2d_X509() failed");
        return;
    }
    buf_len += 64;
    buf = ipcom_malloc(buf_len);
    if (!buf)
    {
        IPCOM_LOG0(ERR, "ipnet_send_cert_path_advert_output2 :: out of memory");
        goto cleanup;
    }
    cert_path_adv = (Ipnet_pkt_nd_cert_path_advert*) buf;
    cert_path_adv->icmp6_type = IPNET_ND_CERT_PATH_ADVERT;
    cert_path_adv->icmp6_code = 0;
    cert_path_adv->icmp6_cert_path_id = ip_htons(id);
    cert_path_adv->icmp6_cert_path_all_components = ip_htons(all_components);
    cert_path_adv->icmp6_cert_path_component = ip_htons(component);
    tmp_buf = buf + sizeof(Ipnet_pkt_nd_cert_path_advert);
    tmp_len = buf_len - sizeof(Ipnet_pkt_nd_cert_path_advert);
    if (ipnet_send_icmp6_opt_cert_add(tmp_buf, &tmp_len, cert))
    {
        IPCOM_LOG0(ERR, "ipnet_send_cert_path_advert_output2 :: ipnet_send_icmp6_opt_cert_add() failed");
        goto cleanup;
    }

    if (ipcom_sendto(conf->icmp6_sock,
                     buf,
                     sizeof(Ipnet_pkt_nd_cert_path_advert)+tmp_len,
                     0,
                     (struct Ip_sockaddr*)addr,
                     addr_len) != (Ip_ssize_t)(sizeof(Ipnet_pkt_nd_cert_path_advert)+tmp_len))
    {
        IPCOM_LOG2(ERR, "ipnet_send_cert_path_advert_output2 :: ipcom_sendto() failed, %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
        goto cleanup;
    }

 cleanup:
    ipcom_free(buf);
}

/*
 *===========================================================================
 *                    ipnet_send_cert_path_advert_output
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_send_cert_path_advert_output(Ipnet_send_conf *conf,
                                   struct Ip_sockaddr_in6* addr,
                                   Ip_socklen_t addr_len,
                                   Ip_u16 component,
                                   Ip_u16 id)
{
    if (component == IPNET_SEND_CERT_PATH_ALL_COMPONENTS)
    {
        int i;
        for (i = sk_X509_num(conf->cert_chain)-2; i>=0; i--)
            ipnet_send_cert_path_advert_output2(conf,
                                                sk_X509_value(conf->cert_chain, i),
                                                i,
                                                sk_X509_num(conf->cert_chain)-1,
                                                id,
                                                addr,
                                                addr_len);
    }
    else
    {
        if (component > sk_X509_num(conf->cert_chain)-1)
        {
            IPCOM_LOG2(ERR, "ipnet_send_cert_path_advert_output :: component value (%d) beyond cert chain end (%d) requested", component, sk_X509_num(conf->cert_chain)-1);
        }
        ipnet_send_cert_path_advert_output2(conf,
                                            sk_X509_value(conf->cert_chain, component),
                                            component,
                                            sk_X509_num(conf->cert_chain)-1,
                                            id,
                                            addr,
                                            addr_len);
    }
}


/*
 *===========================================================================
 *                    ipnet_send_cert_path_advert_input
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_send_cert_path_advert_input(Ipnet_send_conf *conf)
{
    int rc;
    Ip_u8 buf[1500];
    struct Ip_sockaddr_in6 from;
    Ip_socklen_t fromlen;
    Ipnet_pkt_nd_cert_path_advert  *cert_path_adv;
    Ipnet_pkt_nd_opt_cert          *cert_opt;
    Ipnet_pkt_nd_opt_hdr           *opt;
    Ipnet_send_prefix_map_entry    *map_entry;
    X509                           *x509;
    Ip_u8                          *ptr;
    EVP_PKEY                       *cert_key = IP_NULL;
    RSA                            *cert_rsa_key = IP_NULL;
    struct Ip_timeval               tv;

    fromlen = sizeof(from);
    rc = ipcom_recvfrom(conf->icmp6_sock, buf, sizeof(buf), 0, (struct Ip_sockaddr*)&from, &fromlen);
    if (rc < 0)
    {
        IPCOM_LOG2(ERR, "ipnet_send_cert_path_advert_input :: failed to read from icmp6 socket"
                   ", errno=%d, %s", ipcom_errno, ipcom_strerror(ipcom_errno));
        return;
    }
    IPCOM_LOG1(DEBUG, "ipnet_send_cert_path_advert_input :: src=%s",
               ipcom_inet_ntop(IP_AF_INET6,
                               &from.sin6_addr,
                               ipnet_send_log_buf,
                               sizeof(ipnet_send_log_buf)));
    if ((Ip_size_t)rc < sizeof(Ipnet_pkt_nd_cert_path_advert) + sizeof(Ipnet_pkt_nd_opt_cert))
    {
        IPCOM_LOG1(DEBUG, "ipnet_send_cert_path_advert_input :: received too small msg, %d bytes", rc);
        return;
    }
    cert_path_adv = (Ipnet_pkt_nd_cert_path_advert*)buf;
    if (cert_path_adv->icmp6_type != IPNET_ND_CERT_PATH_ADVERT)
    {
        IP_PANIC();
        return;
    }

    opt = (Ipnet_pkt_nd_opt_hdr*)(buf + sizeof(Ipnet_pkt_nd_cert_path_advert));
    if (!ipnet_icmp6_validate_options_len(opt, rc - sizeof(Ipnet_pkt_nd_cert_path_advert)))
    {
        IPCOM_LOG0(DEBUG, "ipnet_send_cert_path_advert_input :: invalid options");
        return;
    }
    cert_opt = (Ipnet_pkt_nd_opt_cert*)
        ipnet_icmp6_get_option(opt,
                               rc - sizeof(Ipnet_pkt_nd_cert_path_advert),
                               IPNET_ND_OPT_CERTIFICATE,
                               0);
    if (!cert_opt)
    {
        IPCOM_LOG0(DEBUG, "ipnet_send_cert_path_advert_input :: no certificate option");
        return;
    }

    map_entry = ipnet_send_prefix_map_get_entry_by_id(ip_ntohs(cert_path_adv->icmp6_cert_path_id));
    if (!map_entry)
    {
        IPCOM_LOG1(DEBUG, "ipnet_send_cert_path_advert_input :: no cert path adv expected for id=%d",
                   ip_ntohs(cert_path_adv->icmp6_cert_path_id));
        return;
    }
    if (map_entry->all_components == 0)
    {
        /* First cert adv received */
        map_entry->all_components = ip_ntohs(cert_path_adv->icmp6_cert_path_all_components);
        map_entry->next_component = map_entry->all_components - 1;
    }
    if (map_entry->next_component < ip_ntohs(cert_path_adv->icmp6_cert_path_component))
    {
        /* Either we have already received this component/certificate, or the router is sending
           its CA cert, which it SHOULD NOT according to rfc 3971. In either case, just drop this message */
        IPCOM_LOG2(DEBUG, "ipnet_send_cert_path_advert_input :: expected component %d, got %d",
                   map_entry->next_component, ip_htons(cert_path_adv->icmp6_cert_path_component));
        return;
    }
    ptr = cert_opt->nd_opt_cert_value;
    x509 = d2i_X509(IP_NULL, (const unsigned char**)&ptr, cert_opt->nd_opt_cert_len*8);
    if (!x509)
    {
        IPCOM_LOG0(DEBUG, "ipnet_send_cert_path_advert_input :: failed to decode X509 cert");
        return;
    }

    if (ip_ntohs(cert_path_adv->icmp6_cert_path_component) == 0)
    {
        /* Cert path is complete, do validation. First check that the CGA public key matches
         * the public key in the certificate*/
        cert_key = X509_PUBKEY_get(x509->cert_info->key);
        if (!cert_key || EVP_PKEY_type(cert_key->type) != EVP_PKEY_RSA)
        {
            IPCOM_LOG0(ERR, "ipnet_send_cert_path_advert_input :: failed to decode RSA key");
            goto cleanup;
        }
        cert_rsa_key = EVP_PKEY_get1_RSA(cert_key);
        if (BN_cmp(cert_rsa_key->n, map_entry->rsa->n) != 0 ||
            BN_cmp(cert_rsa_key->e, map_entry->rsa->e) != 0)
        {
            IPCOM_LOG0(ERR, "ipnet_send_cert_path_advert_input :: RSA key mismatch between router CGA option and certificate");
            goto cleanup;
        }
        /* Now verify the certificate */
        IPCOM_LOG0(DEBUG, "Cert path complete: ");
        ipnet_send_log_cert_stack(map_entry->cert_chain);
        if (ipnet_send_cert_chain_verify(conf->ca_cert,
                                         map_entry->cert_chain,
                                         x509,
                                         conf->crl_st,
                                         IP_NULL) != IPCOM_SUCCESS)
        {
            IPCOM_LOG0(ERR, "ipnet_send_cert_path_advert_input :: Failed to verify router's certificate chain:");
            ipnet_send_prefix_map_entry_free(map_entry);
            goto cleanup;
        }
        if (ipnet_send_cert_chain_verify_cb(conf->ca_cert,
                                            map_entry->cert_chain,
                                            x509,
                                            &map_entry->prefix) == IPCOM_SUCCESS)
        {
            /* Set prefix address */
            Ipnet_cga_param cga_param;

            map_entry->cert_validated = IP_TRUE;
            ipcom_memset(&cga_param, 0, sizeof(cga_param));
            cga_param.rsa_key = conf->rsa_key;
            cga_param.der_encoded_key = conf->der_rsa_key;
            cga_param.der_key_len = conf->der_len;
            cga_param.sec = conf->min_sec;
            ipcom_memcpy(cga_param.subnet_prefix, &map_entry->prefix, 8);
            if (ipnet_cga_generate(&cga_param, &map_entry->cga_addr) != IPCOM_SUCCESS)
            {
                IPCOM_LOG0(ERR, "ipnet_send_cert_path_advert_input :: failed to generate CGA address");
                goto cleanup;
            }
            ipcom_memcpy(map_entry->cga_modifier, cga_param.modifier, sizeof(map_entry->cga_modifier));
            map_entry->cga_addr_generated = IP_TRUE;
            if (ipnet_send_addr_add(conf,
                                    &map_entry->cga_addr,
                                    64,
                                    cga_param.modifier,
                                    map_entry->ifindex,
                                    map_entry->prefered_time,
                                    map_entry->valid_time) != IPCOM_SUCCESS)
            {
                IPCOM_LOG2(ERR, "ipnet_send_cert_path_advert_input :: failed to set CGA address %s on i/f %s",
                           ipcom_inet_ntop(IP_AF_INET6, &map_entry->cga_addr, ipnet_send_log_buf, IP_INET6_ADDRSTRLEN),
                           ipcom_if_indextoname(map_entry->ifindex, ipnet_send_log_buf+IP_INET6_ADDRSTRLEN));
                goto cleanup;

            }
            if (IPCOM_TMO2_PENDING(&map_entry->tmo))
                ipcom_tmo2_cancel(&map_entry->tmo);
            tv.tv_sec = map_entry->valid_time;
            tv.tv_usec = 0;
            IPCOM_LOG1(DEBUG2, "ipcom_tmo2_request 3, duration = %d", tv.tv_sec);
            ipcom_tmo2_request_timeval(ipnet_send_tmo,
                                       &map_entry->tmo,
                                       ipnet_send_prefix_map_tmo_cb,
                                       map_entry,
                                       &tv);
        }
        else
        {
            IPCOM_LOG0(ERR, "ipnet_send_cert_path_advert_input :: ipnet_send_cert_chain_verify_cb failed");
            ipnet_send_prefix_map_entry_free(map_entry);
            goto cleanup;
        }
    }
    else
    {
        /* This is an intermediate cert in the chain */
        sk_X509_push(map_entry->cert_chain, x509);
        x509->references++;
        map_entry->next_component--;
        ipcom_tmo2_reset(&map_entry->tmo);
    }

 cleanup:
    X509_free(x509);
    if (cert_key)
        EVP_PKEY_free(cert_key);
    if (cert_rsa_key)
        RSA_free(cert_rsa_key);

    return;
}


/*
 *===========================================================================
 *                    ipnet_send_cert_path_solicit_input
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_send_cert_path_solicit_input(Ipnet_send_conf *conf)
{
    int rc;
    Ip_u8 buf[1500];
    struct Ip_sockaddr_in6 from;
    Ip_socklen_t fromlen;
    Ipnet_pkt_nd_cert_path_solicit *cert_path_sol;
    Ipnet_pkt_nd_opt_trust_anchor  *trust_anchor_opt;
    Ipnet_pkt_nd_opt_hdr           *opt;
    X509_NAME                      *x509_name = IP_NULL;
    X509_NAME                      *issuer_name = IP_NULL;

    fromlen = sizeof(from);
    rc = ipcom_recvfrom(conf->icmp6_sock, buf, sizeof(buf), 0, (struct Ip_sockaddr*)&from, &fromlen);
    if (rc < 0)
    {
        IPCOM_LOG2(ERR, "ipnet_send_cert_path_solicit_input :: failed to read from icmp6 socket"
                   " , errno=%d, %s", ipcom_errno, ipcom_strerror(ipcom_errno));
    }
    IPCOM_LOG1(DEBUG, "ipnet_send_cert_path_solicit_input :: src=%s",
               ipcom_inet_ntop(IP_AF_INET6,
                               &from.sin6_addr,
                               ipnet_send_log_buf,
                               sizeof(ipnet_send_log_buf)));
    if ((Ip_size_t)rc < sizeof(Ipnet_pkt_nd_cert_path_solicit) + sizeof(Ipnet_pkt_nd_opt_trust_anchor))
    {
        IPCOM_LOG1(DEBUG, "ipnet_send_cert_path_solicit_input :: received too small msg, %d bytes", rc);
        return;
    }

    cert_path_sol = (Ipnet_pkt_nd_cert_path_solicit*)buf;
    opt = (Ipnet_pkt_nd_opt_hdr*)(buf + sizeof(Ipnet_pkt_nd_cert_path_solicit));
    if (!ipnet_icmp6_validate_options_len(opt, rc - sizeof(Ipnet_pkt_nd_cert_path_solicit)))
    {
        IPCOM_LOG0(DEBUG, "ipnet_send_cert_path_solicit_input :: invalid options");
        return;
    }
    trust_anchor_opt = (Ipnet_pkt_nd_opt_trust_anchor*)
        ipnet_icmp6_get_option(opt,
                               rc - sizeof(Ipnet_pkt_nd_cert_path_solicit),
                               IPNET_ND_OPT_TRUST_ANCHOR,
                               0);
    if (!trust_anchor_opt)
    {
        IPCOM_LOG0(DEBUG, "ipnet_send_cert_path_solicit_input :: no trust anchor option");
        return;
    }

    issuer_name = X509_get_issuer_name(conf->ca_cert);
    if (!issuer_name)
        goto cleanup;

    while (opt)
    {
        const unsigned char *ptr;
        if (opt->nd_opt_type == IPNET_ND_OPT_TRUST_ANCHOR)
            trust_anchor_opt = (Ipnet_pkt_nd_opt_trust_anchor*)opt;
        else
        {
            IPCOM_LOG1(DEBUG, "ipnet_send_cert_path_solicit_input :: unexpected ND option found in Cert Path Sol msg: %d", opt->nd_opt_type);
            goto cleanup;
        }
        ptr = trust_anchor_opt->nd_opt_ta_value;
        x509_name = d2i_X509_NAME(IP_NULL,
                                  &ptr,
                                  trust_anchor_opt->nd_opt_ta_len*8-trust_anchor_opt->nd_opt_ta_pad_len);
        if (!x509_name)
        {
            IPCOM_LOG0(DEBUG, "ipnet_send_cert_path_solicit_input :: failed to decode trust anchor option");
            goto cleanup;
        }

        if (X509_NAME_cmp(issuer_name, x509_name) == 0)
        {
            /* The host has listed 'our' CA as a trust anchor. Send advertisement response */
            ipnet_send_cert_path_advert_output(conf,
                                               &from,
                                               fromlen,
                                               ip_ntohs(cert_path_sol->icmp6_cert_path_components),
                                               ip_ntohs(cert_path_sol->icmp6_cert_path_id));
            goto cleanup;
        }
    }

 cleanup:
    if(x509_name)
        X509_NAME_free(x509_name);
}


/*
 *===========================================================================
 *                    ipnet_send_cert_path_solicit_output
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipnet_send_cert_path_solicit_output(Ipnet_send_conf *conf,
                                    Ip_u16 component,
                                    Ip_u16 id,
                                    struct Ip_sockaddr_in* addr,
                                    Ip_socklen_t addr_len)
{
    X509_NAME *x509_name;
    int name_len;
    Ip_u8 *msg_buf;
    Ip_u8 *ptr;
    Ipnet_pkt_nd_cert_path_solicit *cert_path_sol;
    int len;
    int tmp_len;
    Ip_err rc = IPCOM_ERR_FAILED;

    x509_name = X509_get_issuer_name(conf->ca_cert);
    if (!x509_name)
        return IPCOM_ERR_FAILED;
    name_len = i2d_X509_NAME(x509_name, IP_NULL);
    if (name_len < 0)
        return IPCOM_ERR_FAILED;

    /* Msg length:
     *   8 - ICMPv6 Cert Path Solicitation header
     *   4 - trust anchor option header
     *   8 - max pad lengh of trust anchor opt
     *  32 - margin */
    len =  8 + 4 + name_len + 8 + 320;
    msg_buf = ipcom_calloc(1, len);
    if (!msg_buf)
        return IPCOM_ERR_FAILED;

    cert_path_sol = (Ipnet_pkt_nd_cert_path_solicit*)msg_buf;
    cert_path_sol->icmp6_type = IPNET_ND_CERT_PATH_SOLICIT;
    cert_path_sol->icmp6_code = 0;
    cert_path_sol->icmp6_cert_path_id = ip_htons(id);
    cert_path_sol->icmp6_cert_path_components = ip_htons(component);

    ptr = msg_buf + sizeof(Ipnet_pkt_nd_cert_path_solicit);
    tmp_len = len - sizeof(Ipnet_pkt_nd_cert_path_solicit);
    if (ipnet_send_icmp6_opt_trust_anchor_add(ptr, &tmp_len, conf->ca_cert) != IPCOM_SUCCESS)
        goto cleanup;

    len = sizeof(Ipnet_pkt_nd_cert_path_solicit) + tmp_len;
    if (ipcom_sendto(conf->icmp6_sock, msg_buf, len, 0, (struct Ip_sockaddr *) addr, addr_len)
        == IP_SOCKERR)
    {
        IPCOM_LOG2(WARNING, "radvd: Failed to send Cert Path Solication: %s(%d)",
                   ipcom_strerror(ipcom_errno),
                   ipcom_errno);
    }

    rc = IPCOM_SUCCESS;

 cleanup:
    ipcom_free(msg_buf);

    return rc;
}


/*
 *===========================================================================
 *                    ipnet_send_process_netlink_msg
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_send_process_netlink_msg(Ipnet_send_conf *conf)
{
    Ip_ssize_t len;
    struct Ip_nlmsghdr *nlmsghdr;
    Ip_u8* ptr;

    len = ipcom_socketread(conf->netlink_sock, ipnet_send_nl_buf, sizeof(ipnet_send_nl_buf));
    if (len < 0)
    {
        IPCOM_LOG3(ERR, "Failed to read msg from netlink socket: len=%d, %s(%d)",
                   len, ipcom_strerror(ipcom_errno), ipcom_errno);
        return;
    }
    IPCOM_LOG1(DEBUG2, "Read %d bytes from netlink socket", len);

    ptr = ipnet_send_nl_buf;
    while (len > 0)
    {
        nlmsghdr = (struct Ip_nlmsghdr*)ptr;
        switch(nlmsghdr->nlmsg_type)
        {
        case IP_RTM_SEND_SIGN_REQ:
            ipnet_send_process_sign_create_req(conf, ptr, nlmsghdr->nlmsg_len);
            break;

        case IP_RTM_NEWSEND:
            ipnet_send_process_sign_verify_req(conf, ptr, nlmsghdr->nlmsg_len);
            break;

        default:
            IPCOM_LOG1(ERR, "ipnet_send_process_netlink_msg :: unknown netlink msg: %d",
                       nlmsghdr->nlmsg_type);
        }
        IPCOM_LOG1(DEBUG2, "Dispatched %d bytes netlink msg", nlmsghdr->nlmsg_len);
        ptr += nlmsghdr->nlmsg_len;
        len -= nlmsghdr->nlmsg_len;
    }
}


/*
 *===========================================================================
 *                    ipnet_send_process_sign_create_req
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_send_process_sign_create_req(Ipnet_send_conf *conf, Ip_u8 *nl_msg, int nl_len)
{
	EVP_MD_CTX                   ctx;
    int                          pkt_len;
    Ip_u8                       *new_nl_msg;
    int                          new_nl_len;
    Ipnet_send_nlmsg            *nlmsg_st;
    Ipnet_pkt_nd_opt_rsa_sign   *opt_rsa_sign;
    int                          icmp_offset;
    int                          tmp_len;
    Ip_u32                       chk_sum;
    struct Ip_sockaddr_nl        nladdr;
    struct Ip_in6_addr          *cga_addr;


    nlmsg_st = (Ipnet_send_nlmsg*)nl_msg;
    IPCOM_LOG4(DEBUG, "ipnet_send_process_sign_create_req :: src: %s, dst: %s, nc_key: %s, icmp type: %d",
               ipcom_inet_ntop(IP_AF_INET6,
                               &nlmsg_st->sec_nd_sign.src,
                               ipnet_send_log_buf,
                               sizeof(ipnet_send_log_buf)),
               ipcom_inet_ntop(IP_AF_INET6,
                               &nlmsg_st->sec_nd_sign.dst,
                               ipnet_send_log_buf+IP_INET6_ADDRSTRLEN,
                               sizeof(ipnet_send_log_buf)-IP_INET6_ADDRSTRLEN),
               ipcom_inet_ntop(IP_AF_INET6,
                               &nlmsg_st->sec_nd_sign.nc_key,
                               ipnet_send_log_buf+2*IP_INET6_ADDRSTRLEN,
                               sizeof(ipnet_send_log_buf)-2*IP_INET6_ADDRSTRLEN),
               nlmsg_st->icmp6.icmp6_type);

    /* Setup the netlink reply addr, so we can goto failure label */
    ipcom_memset(&nladdr, 0, sizeof(nladdr));
    nladdr.nl_family = IP_AF_NETLINK;

    /* Calculate the length of the reply netlink msg
     * - nonce opt: 8
     * - timestamp opt: 16
     * - cga opt: 36 + key len + some more (~32 bytes)
     * - rsa opt: 19 + key len
     * - extra: 32
     */
    pkt_len = nl_len + 8 + 16 + 2*BN_num_bytes(conf->rsa_key->n)+36+32+19+32;
    new_nl_msg = ipcom_calloc(1,pkt_len);
    if (!new_nl_msg)
    {
        IPCOM_LOG0(ERR, "Failed to sign NDP message, out of memory");
        goto failure;
    }
    ipcom_memcpy(new_nl_msg, nl_msg, nl_len);
    nlmsg_st = (Ipnet_send_nlmsg*)new_nl_msg;
    new_nl_len = nl_len;
    icmp_offset = ip_offsetof(Ipnet_send_nlmsg,icmp6);

    if (ipcom_memcmp(&nlmsg_st->sec_nd_sign.src,  &ip_in6addr_any, 16) == 0 &&
        nlmsg_st->icmp6.icmp6_type == IPNET_ND_NEIGHBOR_SOLICIT)
    {
        cga_addr = &nlmsg_st->sec_nd_sign.nc_key;
    }
    else
        cga_addr = &nlmsg_st->sec_nd_sign.src;

    /* Add nonce option */
    if (ipnet_send_add_nonce_opt(&nlmsg_st->sec_nd_sign.nc_key,
                                 nlmsg_st->icmp6.icmp6_type,
                                 &new_nl_msg[new_nl_len],
                                 &tmp_len) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "Failed to sign NDP message, could not add nonce");
        goto failure;
    }
    new_nl_len += tmp_len;

    /* Add timestamp option */
    ipnet_send_add_timestamp_opt(&new_nl_msg[new_nl_len]);
    new_nl_len += 16;

    /* Add cga option */
    if (ipnet_cga_icmp6_opt_add(conf->der_rsa_key,
                                conf->der_len,
                                cga_addr,
                                nlmsg_st->sec_nd_sign.ifindex,
                                &new_nl_msg[new_nl_len],
                                &tmp_len) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(DEBUG, "Failed to add CGA option");
        goto failure;
    }
    new_nl_len += tmp_len;

    /* Create temporary checksum. Needed in signature calc below. */
    nlmsg_st->icmp6.icmp6_cksum = 0;
    chk_sum = ipcom_in_checksum_update(&nlmsg_st->icmp6, new_nl_len-icmp_offset);
    chk_sum += ipnet_ip6_pseudo_header_checksum_update(&nlmsg_st->ip6_hdr.src,
                                                       &nlmsg_st->ip6_hdr.dst,
                                                       nlmsg_st->ip6_hdr.nxt,
                                                       new_nl_len-icmp_offset);
    nlmsg_st->icmp6.icmp6_cksum = ipcom_in_checksum_finish(chk_sum);

    /* Create signature */
    EVP_MD_CTX_init(&ctx);
	if (EVP_SignInit(&ctx, EVP_sha1()) != 1)
    {
         IPCOM_LOG0(DEBUG, "Failed to sign NDP message, EVP_SignInit failed");
        goto failure;
    }
    if (EVP_SignUpdate(&ctx,ipnet_send_cga_msg_type_tag, sizeof(ipnet_send_cga_msg_type_tag)) != 1)
    {
        IPCOM_LOG0(DEBUG, "Failed to sign NDP message, EVP_SignUpdate failed");
        goto failure;
    }
    IPCOM_LOG0(DEBUG2, "Sign update, CGA msg type tag:");
    ipnet_send_log_hex_buffer(ipnet_send_cga_msg_type_tag, sizeof(ipnet_send_cga_msg_type_tag));

    if (EVP_SignUpdate(&ctx, &nlmsg_st->ip6_hdr.src, sizeof(nlmsg_st->ip6_hdr.src)) != 1)
    {
        IPCOM_LOG0(DEBUG, "Failed to sign NDP message, EVP_SignUpdate failed");
        goto failure;
    }
    IPCOM_LOG0(DEBUG2, "Sign update, src addr:");
    ipnet_send_log_hex_buffer((Ip_u8*)&nlmsg_st->ip6_hdr.src, sizeof(nlmsg_st->ip6_hdr.src));

    if (EVP_SignUpdate(&ctx, &nlmsg_st->ip6_hdr.dst, sizeof(nlmsg_st->ip6_hdr.dst)) != 1)
    {
        IPCOM_LOG0(DEBUG, "Failed to sign NDP message, EVP_SignUpdate failed");
        goto failure;
    }
    IPCOM_LOG0(DEBUG2, "Sign update, dst addr:");
    ipnet_send_log_hex_buffer((Ip_u8*)&nlmsg_st->ip6_hdr.dst, sizeof(nlmsg_st->ip6_hdr.dst));

    if (EVP_SignUpdate(&ctx,&nlmsg_st->icmp6, new_nl_len-icmp_offset) != 1)
    {
        IPCOM_LOG0(DEBUG, "Failed to sign NDP message, EVP_SignUpdate failed");
        goto failure;
    }
    IPCOM_LOG0(DEBUG2, "Sign update, icmp header:");
    ipnet_send_log_hex_buffer((Ip_u8*)&nlmsg_st->icmp6, new_nl_len-icmp_offset);

    opt_rsa_sign = (Ipnet_pkt_nd_opt_rsa_sign*)&new_nl_msg[new_nl_len];
    opt_rsa_sign->nd_opt_rsa_sign_reserved = 0;
    ipnet_send_add_rsa_sign_key_hash(opt_rsa_sign, conf->der_rsa_key, conf->der_len);
    opt_rsa_sign->nd_opt_rsa_sign_type = IPNET_ND_OPT_RSA_SIGNATURE;
    if (EVP_SignFinal(&ctx, opt_rsa_sign->nd_opt_rsa_sign, (unsigned int*)&tmp_len, conf->pkey) != 1)
    {
        IPCOM_LOG0(DEBUG, "Failed to sign NDP message, EVP_SignFinal failed");
        goto failure;
    }
    (void)EVP_MD_CTX_cleanup(&ctx);

    /* Option length: 4 bytes + 16 bytes key hash + sign len */
    opt_rsa_sign->nd_opt_rsa_sign_len = (4+16+tmp_len +7)/8;
    /* Zero padding bytes */
    if ((4+16+tmp_len)%8 != 0)
        ipcom_memset(opt_rsa_sign->nd_opt_rsa_sign+tmp_len, 0, 8-(tmp_len+4+16)%8);

    new_nl_len += opt_rsa_sign->nd_opt_rsa_sign_len*8;

    ip_assert(new_nl_len <= pkt_len);

    /* Now the message is complete. Re-calc checksum */
    nlmsg_st->icmp6.icmp6_cksum = 0;
    chk_sum = ipcom_in_checksum_update(&nlmsg_st->icmp6, new_nl_len-icmp_offset);
    chk_sum += ipnet_ip6_pseudo_header_checksum_update(&nlmsg_st->ip6_hdr.src,
                                                       &nlmsg_st->ip6_hdr.dst,
                                                       nlmsg_st->ip6_hdr.nxt,
                                                       new_nl_len-icmp_offset);
    nlmsg_st->icmp6.icmp6_cksum = ipcom_in_checksum_finish(chk_sum);

    nlmsg_st->ip6_hdr.plen = ip_htons(new_nl_len-icmp_offset);

    nlmsg_st->nlmsghdr.nlmsg_len = new_nl_len;
    nlmsg_st->sec_nd_sign.status = IPCOM_SUCCESS;

    if (ipcom_sendto(conf->netlink_sock,
                     new_nl_msg,
                     new_nl_len,
                     0,
                     (struct Ip_sockaddr*)&nladdr,
                     sizeof(nladdr)) != new_nl_len)
    {
        IPCOM_LOG2(ERR,"ipcom_sendto() failed, %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
    }
    ipcom_free(new_nl_msg);

    return;

 failure:
    nlmsg_st = (Ipnet_send_nlmsg*)nl_msg;
    nlmsg_st->sec_nd_sign.status = IPCOM_ERR_FAILED;

    if (ipcom_sendto(conf->netlink_sock,
                     nl_msg,
                     nl_len,
                     0,
                     (struct Ip_sockaddr*)&nladdr,
                     sizeof(nladdr)) != nl_len)
    {
        IPCOM_LOG2(ERR,"ipcom_sendto() failed, %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
    }

    if (new_nl_msg)
        ipcom_free(new_nl_msg);
}


/*
 *===========================================================================
 *                    ipnet_send_process_sign_verify_req
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_send_process_sign_verify_req(Ipnet_send_conf *conf, Ip_u8 *nl_msg, int nl_len)
{

    struct Ip_sockaddr_nl        nladdr;
    Ipnet_pkt_nd_opt_hdr        *first_opt;
    Ipnet_pkt_nd_opt_hdr        *opt;
    Ipcom_pkt                    pkt;
    int                          tot_opt_len;
    Ipnet_send_nlmsg            *nlmsg_st;

    nlmsg_st = (Ipnet_send_nlmsg*)nl_msg;
    IPCOM_LOG4(DEBUG, "ipnet_send_process_sign_verify_req :: src: %s, dst: %s, nc_key: %s, icmp type: %d",
               ipcom_inet_ntop(IP_AF_INET6,
                               &nlmsg_st->sec_nd_sign.src,
                               ipnet_send_log_buf,
                               sizeof(ipnet_send_log_buf)),
               ipcom_inet_ntop(IP_AF_INET6,
                               &nlmsg_st->sec_nd_sign.dst,
                               ipnet_send_log_buf+IP_INET6_ADDRSTRLEN,
                               sizeof(ipnet_send_log_buf)),
               ipcom_inet_ntop(IP_AF_INET6,
                               &nlmsg_st->sec_nd_sign.nc_key,
                               ipnet_send_log_buf+2*IP_INET6_ADDRSTRLEN,
                               sizeof(ipnet_send_log_buf)),
               nlmsg_st->icmp6.icmp6_type);


    pkt.data = (Ip_u8*)&nlmsg_st->ip6_hdr;
    pkt.ipstart = 0;
    pkt.start = IPNET_IP6_HDR_SIZE;
    pkt.end = nl_len - ip_offsetof(Ipnet_send_nlmsg,ip6_hdr);

    first_opt = ipnet_icmp6_find_opts(&pkt, &tot_opt_len);
    if (!first_opt)
        return;
    opt = ipnet_icmp6_get_option(first_opt, tot_opt_len, IPNET_ND_OPT_TIMESTAMP, 0);
    if (!opt)
    {
        IPCOM_LOG0(DEBUG,"ipnet_send_process_sign_verify_req :: no timestamp option present");
        return;
    }
    if (opt && ipnet_send_verify_timestamp_opt(conf, opt) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(DEBUG,"ipnet_send_process_sign_verify_req :: timestamp verification failed");
        return;
    }

    opt = ipnet_icmp6_get_option(first_opt, tot_opt_len, IPNET_ND_OPT_NONCE, 0);
    if (!opt)
    {
        IPCOM_LOG0(DEBUG,"ipnet_send_process_sign_verify_req :: no nonce option present");
        return;
    }
    if (ipnet_send_verify_nonce_opt(&nlmsg_st->sec_nd_sign.nc_key,
                                    nlmsg_st->icmp6.icmp6_type,
                                    opt) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(DEBUG,"ipnet_send_process_sign_verify_req :: nonce verification failed");
        return;
    }

    if (ipnet_send_verify_rsa_sign_opt(&pkt, conf->min_sec) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(DEBUG,"ipnet_send_process_sign_verify_req :: RSA signature verification failed");
        return;
    }

    if (nlmsg_st->icmp6.icmp6_type == IPNET_ND_ROUTER_ADVERT)
    {
        ipnet_send_process_router_adv(conf,
                                      &nlmsg_st->sec_nd_sign.src,
                                      first_opt,
                                      tot_opt_len,
                                      nlmsg_st->sec_nd_sign.ifindex);
    }

    ipcom_memset(&nladdr, 0, sizeof(nladdr));
    nladdr.nl_family = IP_AF_NETLINK;
    if (ipcom_sendto(conf->netlink_sock,
                     nl_msg,
                     nl_len,
                     0,
                     (struct Ip_sockaddr*)&nladdr,
                     sizeof(nladdr)) != nl_len)
    {
        IPCOM_LOG2(ERR,"ipnet_send_process_sign_verify_req :: ipcom_sendto() failed, %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
    }
}


IP_STATIC Ip_bool
ipnet_icmp6_send_verify_opts(Ipcom_pkt *pkt)
{
    Ipnet_pkt_nd_opt_hdr* opt;
    int tot_opt_len;

    opt = ipnet_icmp6_find_opts(pkt, &tot_opt_len);
    if (!opt)
        return IP_TRUE;

    return ipnet_icmp6_validate_options_len(opt, tot_opt_len);
}

/*
 *===========================================================================
 *                    ipnet_send_get_mode
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_send_get_mode(Ipnet_netif *netif)
{
    int send_mode;


    send_mode = ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET6,
                                                 netif,
                                                 "send.mode",
                                                 IPNET_SEND_MODE_ON,
                                                 ipnet_send_mode_map);
    switch(send_mode)
    {
    case IPNET_SEND_MODE_ON:
    case IPNET_SEND_MODE_TRANSITION:
    case IPNET_SEND_MODE_OFF:
        break;
    default:
        send_mode = IPNET_SEND_MODE_ON;
    }

    return send_mode;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipnet_send_sign_create
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_send_sign_create(Ipnet_netif *netif, Ipnet_route_entry* rt_next_hop, Ipcom_pkt* icmp_pkt)
{
    Ipcom_pkt              *nl_pkt    = IP_NULL;
    Ipnet_netlink_mem_t     mem;
    struct Ip_nlmsghdr     *nlh;
    struct Ip_nlmsg_sec_nd_sign sec_nd_sign;
    Ipnet_ip6_nc_entry     *neighbor;
    Ipnet_pkt_icmp6        *icmp6_hdr;
    Ipnet_pkt_nd_neighbor_solicit *sol;
    struct Ip_in6_addr     *nc_key = IP_NULL;
    Ipnet_pkt_ip6          *ip6_hdr;
    int                     send_mode;
    int                     ret;

    ip6_hdr = (Ipnet_pkt_ip6*)&icmp_pkt->data[icmp_pkt->start];
    icmp6_hdr = (Ipnet_pkt_icmp6*)&icmp_pkt->data[icmp_pkt->start+IPNET_IP6_HDR_SIZE];

    IPCOM_LOG3(DEBUG, "ipnet_send_sign_create :: dst: %s, src: %s, type: %d",
                   ipcom_inet_ntop(IP_AF_INET6, &ip6_hdr->dst, ipnet->log_buf, sizeof(ipnet->log_buf)),
                   ipcom_inet_ntop(IP_AF_INET6, &ip6_hdr->src, ipnet->log_buf+IP_INET6_ADDRSTRLEN,
                                   sizeof(ipnet->log_buf)-IP_INET6_ADDRSTRLEN),
                   icmp6_hdr->icmp6_type);

    send_mode = ipnet_send_get_mode(netif);

    if (send_mode == IPNET_SEND_MODE_OFF)
        return IPNET_SEND_PKT_NOT_CONSUMED;


    /* Store icmp packet in route cache, pending signature creation */
    switch (icmp6_hdr->icmp6_type)
    {
    case IPNET_ND_NEIGHBOR_SOLICIT:
    case IPNET_ND_NEIGHBOR_ADVERT:
        sol = (Ipnet_pkt_nd_neighbor_solicit*)icmp6_hdr;
        nc_key = &sol->nd_ns_target;
        IPCOM_LOG1(DEBUG, "ipnet_send_sign_create :: ns_target: %s",
                   ipcom_inet_ntop(IP_AF_INET6, nc_key, ipnet->log_buf, sizeof(ipnet->log_buf)));
        break;
    case IPNET_ND_ROUTER_SOLICIT:
    case IPNET_ND_ROUTER_ADVERT:
        nc_key = &ip6_hdr->src;
        break;
    case IPNET_ND_REDIRECT:
        nc_key = &ip6_hdr->dst;
        break;

    default:
        IP_PANIC();
    }

    ipnet_route_lock(rt_next_hop);

    neighbor = ipnet_ip6_neighbor_cache_find(netif, nc_key, IP_TRUE);
    if (!neighbor)
    {
        IPCOM_LOG0(DEBUG, "Failed to add signature to ICMP packet. Dropping packet");
        goto err_out;
    }
    ipnet_route_set_lifetime(neighbor->ne.rt, 7);

    /* Send netlink message */
    if (!ipnet_netlink_pkt_alloc(&nl_pkt, IP_NULL, IP_TRUE, &mem, 2048))
        goto err_out;

    nlh = ipnet_netlink_nlmsg_sz_begin(&mem, 4712, IP_RTM_SEND_SIGN_REQ, 0);
    if (!nlh)
        goto err_out;

    sec_nd_sign.ifindex = netif->ipcom.ifindex;
    sec_nd_sign.vr = netif->vr_index;

    IPNET_IP6_SET_ADDR(&sec_nd_sign.dst, &ip6_hdr->dst);
    IPNET_IP6_SET_ADDR(&sec_nd_sign.src, &ip6_hdr->src);
    IPNET_IP6_SET_ADDR(&sec_nd_sign.nc_key, nc_key);

    if (ipnet_netlink_sz_put(&mem, &sec_nd_sign, sizeof(sec_nd_sign)) == IP_NULL)
        goto err_out;
    if (ipnet_netlink_sz_put(&mem,
                             &icmp_pkt->data[icmp_pkt->start],
                             icmp_pkt->end-icmp_pkt->start) == IP_NULL)
        goto err_out;

    ret = ipnet_netlink_nlmsg_sz_end(&mem, nlh);
    if (ret <= 0)
        goto err_out;
    nl_pkt->end += ret;

    IPCOM_LOG1(DEBUG2,"ipnet_send_sign_create :: sending %d bytes netlink msg", nl_pkt->end-nl_pkt->start);
    ipnet_rtnetlink_broadcast(nl_pkt, IP_RTNLGRP_IPV6_SEND);

    IPCOM_PKT_TRACE(icmp_pkt, IPCOM_PKT_ID_PRE_SEND_SIG);

    neighbor->pending_sign_create = icmp_pkt;
    neighbor->rt_next_hop = rt_next_hop;

    return IPCOM_SUCCESS;

 err_out:
    ipnet_route_unlock(rt_next_hop);
    if (nl_pkt)
        ipcom_pkt_free(nl_pkt);
    ipcom_pkt_free(icmp_pkt);
    return IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    ipnet_send_sign_verify
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_send_sign_verify(Ipnet_netif *netif,
                       IP_CONST struct Ip_in6_addr *src,
                       int hlim,
                       Ipcom_pkt* icmp6_pkt)
{
    Ipcom_pkt                     *nl_pkt    = IP_NULL;
    Ipnet_netlink_mem_t            mem;
    struct Ip_nlmsghdr            *nlh;
    struct Ip_nlmsg_sec_nd_sign    sec_nd_sign;
    Ipnet_ip6_nc_entry            *neighbor;
    Ipnet_pkt_ip6                 *ip6_hdr;
    Ipnet_pkt_icmp6               *icmp6_hdr;
    struct Ip_in6_addr            *nc_key;
    int                            ret     = -1;
    Ipnet_send_nlmsg              *send_nlmsg;
    Ipnet_pkt_nd_neighbor_solicit *sol;
    int                            send_mode;
    Ip_bool                        rsa_opt_present = IP_FALSE;

    ip6_hdr = (Ipnet_pkt_ip6*)&icmp6_pkt->data[icmp6_pkt->ipstart];
    icmp6_hdr = (Ipnet_pkt_icmp6*)&icmp6_pkt->data[icmp6_pkt->start];

    switch (icmp6_hdr->icmp6_type)
    {
    case IPNET_ND_NEIGHBOR_ADVERT:
    case IPNET_ND_NEIGHBOR_SOLICIT:
    case IPNET_ND_ROUTER_SOLICIT:
    case IPNET_ND_ROUTER_ADVERT:
    case IPNET_ND_REDIRECT:
        /* These are the packet types we need to process */
        break;

        /* We are not interested in other packets */
    default:
        return IPNET_SEND_PKT_NOT_CONSUMED;
    }

    send_mode = ipnet_send_get_mode(netif);
    if (ipnet_icmp6_send_verify_opts(icmp6_pkt) &&
        ipnet_icmp6_option_present(icmp6_pkt, IPNET_ND_OPT_RSA_SIGNATURE))
        rsa_opt_present = IP_TRUE;

    switch (send_mode)
    {
    case IPNET_SEND_MODE_OFF:
        return IPNET_SEND_PKT_NOT_CONSUMED;
    case IPNET_SEND_MODE_TRANSITION:
        if (rsa_opt_present)
            break;
        else
            /* Allow unsigned messages in this mode */
            return IPNET_SEND_PKT_NOT_CONSUMED;
    case IPNET_SEND_MODE_ON:
        if (rsa_opt_present)
            break;
        else
            goto err_out;
    }

    IPCOM_LOG3(DEBUG, "ipnet_send_sign_verify :: dst: %s, src: %s, type: %d",
               ipcom_inet_ntop(IP_AF_INET6, &ip6_hdr->dst, ipnet->log_buf, sizeof(ipnet->log_buf)),
               ipcom_inet_ntop(IP_AF_INET6, &ip6_hdr->src, ipnet->log_buf+IP_INET6_ADDRSTRLEN,
                               sizeof(ipnet->log_buf)-IP_INET6_ADDRSTRLEN),
               icmp6_hdr->icmp6_type);

    /* Check if a signature option is present */
    if (!ipnet_icmp6_send_verify_opts(icmp6_pkt) ||
        !ipnet_icmp6_option_present(icmp6_pkt, IPNET_ND_OPT_RSA_SIGNATURE))
    {
        if (send_mode == IPNET_SEND_MODE_ON)
            goto err_out;
        /* If we get here the send mode is transitional, allow
         * packets without signatures. */
        return IPNET_SEND_PKT_NOT_CONSUMED;
    }

    if (icmp6_hdr->icmp6_type == IPNET_ND_NEIGHBOR_ADVERT ||
        icmp6_hdr->icmp6_type == IPNET_ND_NEIGHBOR_SOLICIT)
    {
        /* DADs require special handling since src addr is set to addr_any*/
        sol = (Ipnet_pkt_nd_neighbor_solicit*)icmp6_hdr;
        nc_key = &sol->nd_ns_target;
        IPCOM_LOG1(DEBUG, "ipnet_send_sign_verify :: ns_target: %s",
                   ipcom_inet_ntop(IP_AF_INET6, nc_key, ipnet->log_buf, sizeof(ipnet->log_buf)));
    }
    else
        nc_key = &ip6_hdr->src;

    /* Store icmp packet in route cache, pending signature verification */
    neighbor = ipnet_ip6_neighbor_cache_find(netif, nc_key, IP_TRUE);
    if (!neighbor)
        goto err_out;

    if (neighbor->pending_sign_verify != IP_NULL)
    {
        IPCOM_LOG1(DEBUG, "ipnet_send_sign_verify: "
                   "packet already queued for sign ver for this peer (%s), dropping packet",
                   ipcom_inet_ntop(IP_AF_INET6, src, ipnet->log_buf, sizeof(ipnet->log_buf)));
        goto err_out;
    }
    ipnet_route_set_lifetime(neighbor->ne.rt, 13); /* TODO: verify implications of setting lifetime */
    /* Send signature request msg over netlink */
    if (!ipnet_netlink_pkt_alloc(&nl_pkt, IP_NULL, IP_TRUE, &mem, 2048))
        goto err_out;

    nlh = ipnet_netlink_nlmsg_sz_begin(&mem, 4711, IP_RTM_NEWSEND, 0);
    if (!nlh)
        goto err_out;

    ipcom_memset(&sec_nd_sign, 0, sizeof(sec_nd_sign));
    sec_nd_sign.ifindex = netif->ipcom.ifindex;
    sec_nd_sign.vr = netif->vr_index;
    sec_nd_sign.hlim = hlim;
    IPNET_IP6_SET_ADDR(&sec_nd_sign.src, src);
    IPNET_IP6_SET_ADDR(&sec_nd_sign.nc_key, nc_key);
    IPNET_IP6_SET_ADDR(&sec_nd_sign.dst, &ip6_hdr->dst);

    if (ipnet_netlink_sz_put(&mem, &sec_nd_sign, sizeof(sec_nd_sign)) == IP_NULL)
        goto err_out;
    if (ipnet_netlink_sz_put(&mem,
                             &icmp6_pkt->data[icmp6_pkt->ipstart],
                             icmp6_pkt->end-icmp6_pkt->ipstart) == IP_NULL)
        goto err_out;

    ret = ipnet_netlink_nlmsg_sz_end(&mem, nlh);
    if (ret <= 0)
        goto err_out;
    nl_pkt->end += ret;

    IPCOM_LOG1(DEBUG2,"ipnet_send_sign_verify :: sending %d bytes netlink msg", nl_pkt->end-nl_pkt->start);
    ipnet_rtnetlink_broadcast(nl_pkt, IP_RTNLGRP_IPV6_SEND);

    send_nlmsg = (Ipnet_send_nlmsg*)&nl_pkt->data[nl_pkt->start];

    IPCOM_PKT_TRACE(icmp6_pkt, IPCOM_PKT_ID_PRE_SEND_SIG);
    neighbor->pending_sign_verify = icmp6_pkt;

    return IPNET_SEND_PKT_CONSUMED;

 err_out:
    {
        Ipnet_pkt_icmp6* icmp6;
        Ipnet_pkt_ip6* ip6;

        icmp6 = (Ipnet_pkt_icmp6*) &icmp6_pkt->data[icmp6_pkt->start];
        ip6 = (Ipnet_pkt_ip6*) &icmp6_pkt->data[icmp6_pkt->ipstart];

        IPCOM_LOG3(DEBUG, "Failed to check signature on ICMP packet: "
                   "dst: %s, src: %s, type: %d. Dropping packet",
                   ipcom_inet_ntop(IP_AF_INET6, &ip6->dst, ipnet->log_buf, sizeof(ipnet->log_buf)),
                   ipcom_inet_ntop(IP_AF_INET6, &ip6->src, ipnet->log_buf+IP_INET6_ADDRSTRLEN,
                                   sizeof(ipnet->log_buf)-IP_INET6_ADDRSTRLEN),
                   icmp6->icmp6_type);
    }
    if(nl_pkt)
        ipcom_pkt_free(nl_pkt);
    ipcom_pkt_free(icmp6_pkt);
    return IPNET_SEND_PKT_CONSUMED;
}


/*
 *===========================================================================
 *                    ipnet_sendd
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IPCOM_PROCESS(ipnet_sendd)
{
    Ip_fd sock_ipd = IP_INVALID_SOCKET;
    Ip_fd sock_max;
    Ip_fd_set read_set;
    Ipnet_send_conf *conf = IP_NULL;
    ipcom_proc_init();

    if (ipnet_send_nonce_cache_init() != IPCOM_SUCCESS)
    {
        (void)ipcom_ipd_init("ipnet_sendd", IPCOM_ERR_FAILED, &sock_ipd);
        IPCOM_LOG0(CRIT, "ipnet_sendd :: failed to initialize nonce hash table, terminating");
        goto cleanup;
    }

    ipnet_send_prefix_map_init();

    ipnet_send_tmo = ipcom_tmo2_new();
    if (!ipnet_send_tmo)
    {
        (void)ipcom_ipd_init("ipnet_sendd", IPCOM_ERR_FAILED, &sock_ipd);
        IPCOM_LOG0(CRIT, "ipnet_sendd :: failed to create tmo2 pqueue, terminating");
        goto cleanup;
    }

    if (ipnet_send_reconfigure(&conf) != IPCOM_SUCCESS)
    {
        (void)ipcom_ipd_init("ipnet_sendd", IPCOM_ERR_INVALID_CONFIG, &sock_ipd);
        IPCOM_LOG0(CRIT, "ipnet_sendd :: failed to read configuration, terminating");
        goto cleanup;
    }

    if (sock_ipd == IP_INVALID_SOCKET) /* Only at start, not at reconfigure */
    {
        (void)ipcom_ipd_init("ipnet_sendd", IPCOM_SUCCESS, &sock_ipd);
    }

    sock_max = IP_MAX(conf->netlink_sock, sock_ipd) + 1;

    while(1)
    {
        IP_FD_ZERO(&read_set);
        IP_FD_SET(conf->netlink_sock, &read_set);
        if (conf->icmp6_sock != IP_INVALID_SOCKET)
            IP_FD_SET(conf->icmp6_sock, &read_set);
        IP_FD_SET(sock_ipd, &read_set);

        if (ipcom_tmo2_select(ipnet_send_tmo, sock_max, &read_set, IP_NULL, IP_NULL, IP_NULL) < 0)
        {
            IPCOM_LOG2(CRIT, "ipcom_tmo2_select() failed: %s(%d), terminating",
                       ipcom_strerror(ipcom_errno), ipcom_errno);
            goto cleanup;
        }
        if (IP_FD_ISSET(conf->netlink_sock, &read_set))
        {
            ipnet_send_process_netlink_msg(conf);
        }

        if (conf->icmp6_sock != IP_INVALID_SOCKET && IP_FD_ISSET(conf->icmp6_sock, &read_set))
        {
            if (conf->cert_path_mode == IPNET_SEND_CERT_PATH_MODE_HOST)
                ipnet_send_cert_path_advert_input(conf);
            else
                ipnet_send_cert_path_solicit_input(conf);
        }

        if (IP_FD_ISSET(sock_ipd, &read_set))
        {
            int event;

            event = ipcom_ipd_input(sock_ipd);
            switch(event)
            {
                case IPCOM_IPD_EVENT_RECONFIGURE:
                    IPCOM_LOG0(INFO, "received IPD reconfigure message");
                    switch(ipnet_send_reconfigure(&conf))
                    {
                    case IPCOM_SUCCESS:
                        IPCOM_LOG0(INFO, "Re-read configuration.");
                        break;
                    case IPCOM_ERR_INVALID_CONFIG:
                        IPCOM_LOG0(INFO, "Failed to re-read configuration. Continuing with old configuration.");
                        break;
                    default:
                        IPCOM_LOG0(CRIT, "Failed to re-read configuration, terminating.");
                        break;


                    }
                case IPCOM_IPD_EVENT_KILL:
                    IPCOM_LOG0(INFO, "received IPD kill message");
                    goto cleanup;
                case IPCOM_IPD_EVENT_NONE:
                    break;
                default:
                    IP_PANIC();
                    break;
            }
        }
    }

 cleanup:
    ipnet_send_prefix_map_free();
    ipnet_send_nonce_cache_free();
    if (conf)
        ipnet_send_conf_free(conf);
    if(sock_ipd != IP_INVALID_SOCKET)
        ipcom_socketclose(sock_ipd);
    if (ipnet_send_tmo)
        ipcom_tmo2_delete(ipnet_send_tmo);

    ipcom_proc_exit();
}


/*
 *===========================================================================
 *                    ipnet_rtnetlink_neigh_newsend_family
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_neigh_newsend_family(Ipnet_netlink_mem_t  *mem,
                                     struct Ip_nlmsghdr   *nlmsg,
                                     struct Ip_rtattr     **rta,
                                     int family)
{
    Ipnet_ip6_nc_entry *neighbor;
    struct Ip_nlmsg_sec_nd_sign* sec_nd_sign;
    Ipnet_netif* netif;
    int ret = 0;
    Ipcom_pkt *tmp_pkt;

    (void)mem;
    (void)rta;
    (void)family;

    sec_nd_sign = (struct Ip_nlmsg_sec_nd_sign*) &nlmsg[1];

    netif = ipnet_if_indextonetif((Ip_u16)sec_nd_sign->vr, sec_nd_sign->ifindex);
    if (!netif)
    {
        IPCOM_LOG0(INFO, "ipnet_rtnetlink_neigh_newsend_family :: no netif found");
        return 0;
    }
    neighbor = ipnet_ip6_neighbor_cache_find(netif, &sec_nd_sign->nc_key, IP_FALSE);
    if (!neighbor)
    {
        IPCOM_LOG0(INFO, "ipnet_rtnetlink_neigh_newsend_family :: no NC route entry found");
        return 0;
    }
    if (!neighbor->pending_sign_verify)
    {
        IPCOM_LOG0(INFO, "ipnet_rtnetlink_neigh_newsend_family :: no packet queued in NC route entry");
        return 0;
    }

    ipcom_pkt_set_info(neighbor->pending_sign_verify,
                         IPNET_PKT_INFO_SEND_SIG_VERIFIED,
                         0, IP_NULL);

    /* The packet is being passed to ipnet_icmp6_input for the second time, adjust counters */
    IPNET_STATS(icmp6_input--);
    IPCOM_MIB2_NETIF(ipv6IfIcmpInMsgs--);
    IPCOM_MIB2_SYSWI_U32_ADD(v6, icmpStatsInMsgs, -1);
    IPCOM_PKT_TRACE(neighbor->pending_sign_verify, IPCOM_PKT_ID_POST_SEND_SIG);
    tmp_pkt = neighbor->pending_sign_verify;
    neighbor->pending_sign_verify = IP_NULL;
	ret = ipnet_icmp6_input(netif,
                            &sec_nd_sign->src,
                            &sec_nd_sign->dst,
                            sec_nd_sign->hlim,
                            tmp_pkt);

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_rtnetlink_neigh_send_sign_req_family
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_neigh_send_sign_req_family(Ipnet_netlink_mem_t  *mem,
                                           struct Ip_nlmsghdr   *nlmsg,
                                           struct Ip_rtattr     **rta,
                                           int family)

{
    Ipnet_ip6_nc_entry *neighbor;
    struct Ip_nlmsg_sec_nd_sign* sec_nd_sign;
    Ipnet_netif* netif;
    Ipnet_pkt_ip6* ip6;
    int ret = 0;
    Ipcom_pkt* pkt;

    (void)mem;
    (void)rta;
    (void)family;

    sec_nd_sign = (struct Ip_nlmsg_sec_nd_sign*) &nlmsg[1];

    netif = ipnet_if_indextonetif((Ip_u16)sec_nd_sign->vr, sec_nd_sign->ifindex);
    if (!netif)
    {
        IPCOM_LOG0(DEBUG, "ipnet_rtnetlink_neigh_send_sign_req_family :: no netif found");
        return 0;
    }
    neighbor = ipnet_ip6_neighbor_cache_find(netif, &sec_nd_sign->nc_key, IP_FALSE);
    if (!neighbor)
    {
        IPCOM_LOG0(DEBUG, "ipnet_rtnetlink_neigh_send_sign_req_family :: no NC entry found");
        return 0;
    }

    if (sec_nd_sign->status == IPCOM_SUCCESS)
    {
        ip6 = (Ipnet_pkt_ip6*)&sec_nd_sign[1];

        if (!neighbor->pending_sign_create)
        {
            IPCOM_LOG0(INFO, "ipnet_rtnetlink_neigh_send_sign_req_family :: "
                       "no packet queued in NC route entry");
            ret = 0;
            goto cleanup;
        }
        pkt = ipcom_pkt_malloc(ip_ntohs(ip6->plen) + IPNET_IP6_HDR_SIZE, IP_FLAG_FC_STACKCONTEXT);
        if (!pkt)
        {
            IPCOM_LOG0(ERR, "ipnet_rtnetlink_neigh_send_sign_req_family :: "
                       "failed to allocate packet, dropping signed NDP message");
            ret = 0;
            goto cleanup;
        }
        pkt->start = ipcom_conf_max_link_hdr_size;
        pkt->end = pkt->start;
        pkt->ipstart = pkt->start;

        ipcom_memcpy(&pkt->data[pkt->ipstart], ip6, ip_ntohs(ip6->plen) + IPNET_IP6_HDR_SIZE);
        pkt->end += ip_ntohs(ip6->plen) + IPNET_IP6_HDR_SIZE;
        pkt->vr_index = neighbor->pending_sign_create->vr_index;
        pkt->flags = neighbor->pending_sign_create->flags;
        pkt->ifindex = neighbor->pending_sign_create->ifindex;

        IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_POST_SEND_SIG);
        ret = netif->link_ip6_output(netif,
                                     neighbor->rt_next_hop,
                                     pkt);
    }

 cleanup:
    if (neighbor->pending_sign_create)
    {
        ipcom_pkt_free(neighbor->pending_sign_create);
        neighbor->pending_sign_create = IP_NULL;
    }
    if (neighbor->rt_next_hop)
    {
        ipnet_route_unlock(neighbor->rt_next_hop);
        neighbor->rt_next_hop = IP_NULL;
    }

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_send_log_hex_buffer
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_send_log_hex_buffer(Ip_u8* buf, int len)
{
    int i;
    int j;
    char log_buf[70];


    for (i = 0, j=0; i < len; i++)
    {
        ipcom_sprintf(&log_buf[j], "%02x", buf[i]);
        if (i%32==31)
        {
            log_buf[j+2] = 0;
            IPCOM_LOG1(DEBUG2, "%s", log_buf);
            j = 0;
            continue;
        }
        if (i%8==7)
        {
            log_buf[j+2] = ' ';
            j += 3;
            continue;
        }

        j += 2;
    }

    if (j != 0)
    {
        log_buf[j] = 0;
        IPCOM_LOG1(DEBUG2, "%s", log_buf);
    }
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_send_create
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipnet_send_create(void)
{
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_send_start
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipnet_send_start(void)
{
    Ipcom_proc_attr proc_attr;

    RAND_bytes((unsigned char*)&ipnet_send_cert_path_id, sizeof(ipnet_send_cert_path_id));

    ipcom_proc_attr_init(&proc_attr);
    proc_attr.priority = IPCOM_PRIORITY_MIN;
    proc_attr.stacksize = IPCOM_PROC_STACK_MAX;
    if (ipcom_proc_acreate("ipnet_sendd", ipnet_sendd, &proc_attr, IP_NULL) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(CRIT, "Failed to start ipnet_sendd process");
        return IPCOM_ERR_FAILED;
    }

    return IPCOM_SUCCESS;
}

/*
 *===========================================================================
 *                    ipnet_send_version
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC const char *
ipnet_send_version(void)
{
    return "@(#) IPNET Secure Neighbor Discovery Daemon $Name: VXWORKS_ITER18A_FRZ10 $ - INTERPEAK_COPYRIGHT_STRING";
}


#else
int ipnet_send_empty_file;
#endif /* IPNET_USE_RFC3971 */
/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

