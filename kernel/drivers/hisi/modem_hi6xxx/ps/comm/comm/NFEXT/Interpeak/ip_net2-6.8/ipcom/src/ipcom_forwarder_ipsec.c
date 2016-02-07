/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_forwarder_ipsec.c,v $ $Revision: 1.26.22.1 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_forwarder_ipsec.c,v $
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
 * This module implements hardware assisted IP forwarding with IPSec. This
 * version supports Cavium Network's CN38XX and CN58XX only. The CPU must
 * support unaligned memory access.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"

#ifdef IPCOM_USE_FORWARDER_IPSEC

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

/* Ipcom include files */
#define IPCOM_USE_CLIB_PROTO
#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_inet.h"
#include "ipcom_forwarder.h"

#include <ipipsec_h.h>
#include <ipipsec_pfkeyv2.h>


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_FORWARDER_IPSEC_HMAC_LEN    12
#define IPCOM_FORWARDER_IPSEC_ESP_HLEN     8

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

/* Cavium crypto library API */
IP_EXTERN int DES_ede3_cbc_sha1_encrypt(Ip_u8 *des_key,
                                        Ip_u16 auth_keylen, Ip_u8 *auth_key,
                                        Ip_u8 *espheader, Ip_u8 *des_iv,
                                        Ip_u8 *pktptr, Ip_u16 pktlen,
                                        Ip_u8 *outptr, Ip_u16 *outlen);

IP_EXTERN int DES_ede3_cbc_md5_encrypt(Ip_u8 *des_key,
                                       Ip_u16 auth_keylen, Ip_u8 *auth_key,
                                       Ip_u8 *espheader, Ip_u8 *des_iv,
                                       Ip_u8 *pktptr, Ip_u16 pktlen,
                                       Ip_u8 *outptr, Ip_u16 *outlen);

IP_EXTERN int AES_cbc_md5_encrypt(Ip_u16 aes_key_len, Ip_u8 *aes_key,
                                  Ip_u16 auth_keylen, Ip_u8 *auth_key,
                                  Ip_u8 *espheader, Ip_u8 *aes_iv,
                                  Ip_u8 *pktptr, Ip_u16 pktlen,
                                  Ip_u8 *outptr, Ip_u16 *outlen);

IP_EXTERN int AES_cbc_sha1_encrypt(Ip_u16 aes_key_len, Ip_u8 *aes_key,
                                   Ip_u16 sha1_keylen, Ip_u8 *sha1_key,
                                   Ip_u8 *espheader, Ip_u8 *aes_iv,
                                   Ip_u8 *pktptr, Ip_u16 pktlen,
                                   Ip_u8 *outptr, Ip_u16 *outlen);

IP_EXTERN int DES_ede3_cbc_sha1_decrypt(Ip_u8 *des_key,
                                        Ip_u16 auth_keylen, Ip_u8 *auth_key,
                                        Ip_u8 *des_iv, Ip_u8 *pktptr,
                                        Ip_u16 pktlen, Ip_u8 *outptr,
                                        Ip_u16 *outlen, Ip_u8 compdigest);

IP_EXTERN int DES_ede3_cbc_md5_decrypt(Ip_u8 *des_key,
                                       Ip_u16 auth_keylen, Ip_u8 *auth_key,
                                       Ip_u8 *des_iv, Ip_u8 *pktptr,
                                       Ip_u16 pktlen, Ip_u8 *outptr,
                                       Ip_u16 *outlen, Ip_u8 compdigest);

IP_EXTERN int AES_cbc_md5_decrypt(Ip_u16 aes_key_len, Ip_u8 *aes_key,
                                  Ip_u16 auth_keylen, Ip_u8 *auth_key,
                                  Ip_u8 *aes_iv, Ip_u8 *pktptr,
                                  Ip_u16 pktlen, Ip_u8 *outptr,
                                  Ip_u16 *outlen, Ip_u8 compdigest);

IP_EXTERN int AES_cbc_sha1_decrypt(Ip_u16 aes_key_len, Ip_u8 *aes_key,
                                   Ip_u16 sha1_keylen, Ip_u8 *sha1_key,
                                   Ip_u8 *aes_iv, Ip_u8 *pktptr,
                                   Ip_u16 pktlen, Ip_u8 *outptr,
                                   Ip_u16 *outlen, Ip_u8 compdigest);

IP_EXTERN int AES_cbc_sha1_encrypt_fast(Ip_u16 aes_key_len, Ip_u8 *aes_key,
                                        Ip_u8 *espheader, Ip_u8 *aes_iv,
                                        Ip_u8 *pktptr, Ip_u16 pktlen,
                                        Ip_u8 *outptr,
                                        Ip_u8 *ictx,  Ip_u8 *octx);

IP_EXTERN int AES_cbc_sha1_decrypt_fast(Ip_u16 aes_key_len, Ip_u8 *aes_key,
                                        Ip_u8 *aes_iv, Ip_u8 *pktptr,
                                        Ip_u16 pktlen, Ip_u8 *outptr,
                                        Ip_u8 *ictx,   Ip_u8 *octx);

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC void ipcom_forwarder_ipsec_remove_sa(Ipipsec_sa *sa);
IP_PUBLIC Ip_u32 ipcom_forwarder_ipsec_get_seq(Ipipsec_sa *sa);
IP_PUBLIC void ipcom_forwarder_ipsec_get_replay(Ipipsec_sa *sa);
IP_PUBLIC int ipcom_forwarder_ipsec_check_replay(Ipipsec_sa *sa, Ip_u32 seq);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC Ip_u32 ipsec_sa_cache_id = 1;
IP_EXTERN int ipcom_forwarder_ipsec_sa_cache_size;


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_esp_3des_sha1_encrypt
 *===========================================================================
 * Description: Construct an IPSec ESP packet from a clear text packet
 *              using 3DES for encryption and SHA1 for authentication.
 * Parameters:  ekeylen - encryption key length
 *              ekey    - encryption key
 *              akeylen - authentication key length
 *              akey    - authentication key
 *              esp     - ESP header
 *              iv      - initialization vector
 *              pkt     - incoming packet
 *              pktlen  - length of packet
 *              out     - location where the ESP packet will be written
 * Returns:      0 = success
 *              -1 = failure
 */
IP_STATIC int
ipcom_forwarder_ipsec_esp_3des_sha1_encrypt(Ip_u16 ekeylen, Ip_u8  *ekey,
                                            Ip_u16 akeylen, Ip_u8  *akey,
                                            Ip_u8  *esp,    Ip_u8  *iv,
                                            Ip_u8  *pkt,    Ip_u16 pktlen,
                                            Ip_u8  *out,
                                            Ip_u8  *ictx,   Ip_u8  *octx)

{
    Ip_u16 outlen;

    (void)ekeylen;
    (void)ictx;
    (void)octx;
    return DES_ede3_cbc_sha1_encrypt(ekey,
                                     akeylen, akey,
                                     esp, iv,
                                     pkt, pktlen,
                                     out, &outlen);
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_esp_3des_sha1_decrypt
 *===========================================================================
 * Description: Construct a clear text packet from an IPSec ESP packet
 *              using 3DES for decryption and SHA1 for authentication.
 * Parameters:  ekeylen - decryption key length
 *              ekey    - decryption key
 *              akeylen - authentication key length
 *              akey    - authentication key
 *              iv      - initialization vector
 *              pkt     - incoming packet
 *              pktlen  - length of packet
 *              out     - location where the clear text packet will be written
 * Returns:      0 = success
 *              -1 = failure
 */
IP_STATIC int
ipcom_forwarder_ipsec_esp_3des_sha1_decrypt(Ip_u16 ekeylen, Ip_u8 *ekey,
                                            Ip_u16 akeylen, Ip_u8 *akey,
                                            Ip_u8  *iv,     Ip_u8 *pkt,
                                            Ip_u16 pktlen,  Ip_u8 *out,
                                            Ip_u8  *ictx,   Ip_u8 *octx)
{
    Ip_u16 outlen;

    (void)ekeylen;
    (void)ictx;
    (void)octx;
    return DES_ede3_cbc_sha1_decrypt(ekey,
                                     akeylen, akey,
                                     iv, pkt,
                                     pktlen, out,
                                     &outlen, 0);
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_esp_3des_md5_encrypt
 *===========================================================================
 * Description: Construct an IPSec ESP packet from a clear text packet
 *              using 3DES for encryption and MD5 for authentication.
 * Parameters:  ekeylen - encryption key length
 *              ekey    - encryption key
 *              akeylen - authentication key length
 *              akey    - authentication key
 *              esp     - ESP header
 *              iv      - initialization vector
 *              pkt     - incoming packet
 *              pktlen  - length of packet
 *              out     - location where the ESP packet will be written
 * Returns:      0 = success
 *              -1 = failure
 */
IP_STATIC int
ipcom_forwarder_ipsec_esp_3des_md5_encrypt(Ip_u16 ekeylen, Ip_u8  *ekey,
                                           Ip_u16 akeylen, Ip_u8  *akey,
                                           Ip_u8  *esp,    Ip_u8  *iv,
                                           Ip_u8  *pkt,    Ip_u16 pktlen,
                                           Ip_u8  *out,
                                           Ip_u8  *ictx,   Ip_u8  *octx)

{
    Ip_u16 outlen;

    (void)ekeylen;
    (void)ictx;
    (void)octx;
    return DES_ede3_cbc_md5_encrypt(ekey,
                                    akeylen, akey,
                                    esp, iv,
                                    pkt, pktlen,
                                    out, &outlen);
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_esp_3des_md5_decrypt
 *===========================================================================
 * Description: Construct a clear text packet from an IPSec ESP packet
 *              using 3DES for decryption and MD5 for authentication.
 * Parameters:  ekeylen - decryption key length
 *              ekey    - decryption key
 *              akeylen - authentication key length
 *              akey    - authentication key
 *              iv      - initialization vector
 *              pkt     - incoming packet
 *              pktlen  - length of packet
 *              out     - location where the clear text packet will be written
 * Returns:      0 = success
 *              -1 = failure
 */
IP_STATIC int
ipcom_forwarder_ipsec_esp_3des_md5_decrypt(Ip_u16 ekeylen, Ip_u8 *ekey,
                                           Ip_u16 akeylen, Ip_u8 *akey,
                                           Ip_u8  *iv,     Ip_u8 *pkt,
                                           Ip_u16 pktlen,  Ip_u8 *out,
                                           Ip_u8  *ictx,   Ip_u8 *octx)
{
    Ip_u16 outlen;

    (void)ekeylen;
    (void)ictx;
    (void)octx;
    return DES_ede3_cbc_md5_decrypt(ekey,
                                    akeylen, akey,
                                    iv, pkt,
                                    pktlen, out,
                                    &outlen, 0);
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_esp_aes_sha1_encrypt
 *===========================================================================
 * Description: Construct an IPSec ESP packet from a clear text packet
 *              using AES for encryption and SHA1 for authentication.
 * Parameters:  ekeylen - encryption key length
 *              ekey    - encryption key
 *              akeylen - authentication key length
 *              akey    - authentication key
 *              esp     - ESP header
 *              iv      - initialization vector
 *              pkt     - incoming packet
 *              pktlen  - length of packet
 *              out     - location where the ESP packet will be written
 * Returns:      0 = success
 *              -1 = failure
 */
IP_STATIC int
ipcom_forwarder_ipsec_esp_aes_sha1_encrypt(Ip_u16 ekeylen, Ip_u8  *ekey,
                                           Ip_u16 akeylen, Ip_u8  *akey,
                                           Ip_u8  *esp,    Ip_u8  *iv,
                                           Ip_u8  *pkt,    Ip_u16 pktlen,
                                           Ip_u8  *out,
                                           Ip_u8  *ictx,   Ip_u8  *octx)
{
    (void)akey;
    (void)akeylen;
    return AES_cbc_sha1_encrypt_fast(ekeylen, ekey,
                                     esp, iv,
                                     pkt, pktlen,
                                     out,
                                     ictx, octx);
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_esp_aes_sha1_decrypt
 *===========================================================================
 * Description: Construct a clear text packet from an IPSec ESP packet
 *              using AES for decryption and SHA1 for authentication.
 * Parameters:  ekeylen - decryption key length
 *              ekey    - decryption key
 *              akeylen - authentication key length
 *              akey    - authentication key
 *              iv      - initialization vector
 *              pkt     - incoming packet
 *              pktlen  - length of packet
 *              out     - location where the clear text packet will be written
 * Returns:      0 = success
 *              -1 = failure
 */
IP_STATIC int
ipcom_forwarder_ipsec_esp_aes_sha1_decrypt(Ip_u16 ekeylen, Ip_u8 *ekey,
                                           Ip_u16 akeylen, Ip_u8 *akey,
                                           Ip_u8  *iv,     Ip_u8 *pkt,
                                           Ip_u16 pktlen,  Ip_u8 *out,
                                           Ip_u8  *ictx,   Ip_u8  *octx)
{
    (void)akey;
    (void)akeylen;
    return AES_cbc_sha1_decrypt_fast(ekeylen, ekey,
                                     iv, pkt,
                                     pktlen, out,
                                     ictx, octx);
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_esp_aes_md5_encrypt
 *===========================================================================
 * Description: Construct an IPSec ESP packet from a clear text packet
 *              using AES for encryption and MD5 for authentication.
 * Parameters:  ekeylen - encryption key length
 *              ekey    - encryption key
 *              akeylen - authentication key length
 *              akey    - authentication key
 *              esp     - ESP header
 *              iv      - initialization vector
 *              pkt     - incoming packet
 *              pktlen  - length of packet
 *              out     - location where the ESP packet will be written
 * Returns:      0 = success
 *              -1 = failure
 */
IP_STATIC int
ipcom_forwarder_ipsec_esp_aes_md5_encrypt(Ip_u16 ekeylen, Ip_u8  *ekey,
                                          Ip_u16 akeylen, Ip_u8  *akey,
                                          Ip_u8  *esp,    Ip_u8  *iv,
                                          Ip_u8  *pkt,    Ip_u16 pktlen,
                                          Ip_u8  *out,
                                          Ip_u8  *ictx,   Ip_u8  *octx)
{
    Ip_u16 outlen;

    (void)ictx;
    (void)octx;
    return AES_cbc_md5_encrypt(ekeylen, ekey,
                               akeylen, akey,
                               esp, iv,
                               pkt, pktlen,
                               out, &outlen);
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_esp_aes_md5_decrypt
 *===========================================================================
 * Description: Construct a clear text packet from an IPSec ESP packet
 *              using AES for decryption and MD5 for authentication.
 * Parameters:  ekeylen - decryption key length
 *              ekey    - decryption key
 *              akeylen - authentication key length
 *              akey    - authentication key
 *              iv      - initialization vector
 *              pkt     - incoming packet
 *              pktlen  - length of packet
 *              out     - location where the clear text packet will be written
 * Returns:      0 = success
 *              -1 = failure
 */
IP_STATIC int
ipcom_forwarder_ipsec_esp_aes_md5_decrypt(Ip_u16 ekeylen, Ip_u8 *ekey,
                                          Ip_u16 akeylen, Ip_u8 *akey,
                                          Ip_u8  *iv,     Ip_u8 *pkt,
                                          Ip_u16 pktlen,  Ip_u8 *out,
                                          Ip_u8  *ictx,   Ip_u8 *octx)
{
    Ip_u16 outlen;

    (void)ictx;
    (void)octx;
    return AES_cbc_md5_decrypt(ekeylen, ekey,
                               akeylen, akey,
                               iv, pkt,
                               pktlen, out,
                               &outlen, 0);
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_sa_header_space
 *===========================================================================
 * Description: Calculate the additional header space required to construct
 *              the ESP header.
 * Parameters:  sa - security association
 * Returns:     The required number of bytes
 *
 */
IP_STATIC Ip_u16
ipcom_forwarder_ipsec_sa_header_space(Ipipsec_sa *sa)
{
    int tot_header_space = 0;

    tot_header_space += sa->header_space;
    tot_header_space += sa->trailer_space;
    tot_header_space += (sa->key.domain == IP_AF_INET
                        ? IPCOM_IP_HEADER_LENGTH : IPCOM_IP6_HEADER_LENGTH);

    return (Ip_u16)tot_header_space;
}

/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_tag_sa
 *===========================================================================
 * Description: Create a tag for an SA.
 * Parameters:  spi - security parameter index of the SA.
 * Returns:     Tag value.
 *
 */
IP_INLINE Ip_u16
ipcom_forwarder_ipsec_tag_sa(Ip_u32 spi)
{
    return ipcom_forwarder_hw_crc16(&spi, 1) & (ipcom_fw.fw_ipsec_entries - 1);
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_flow_lookup
 *===========================================================================
 * Description: Find an IPSec flow in the main IPSec module.
 * Parameters:  domain  - domain to search
 *              dstaddr - destination address to use in the search
 *              input   - boolean used to search for input or output flows.
 * Returns:     Matching flow or IP_NULL if not found.
 *
 */
IP_STATIC Ipipsec_flow *
ipcom_forwarder_ipsec_flow_lookup(Ip_u8 domain, void *dstaddr, Ip_bool input)
{
    Ipipsec_flow  *flow;
    Ipipsec_param  param;

    ipcom_memset(&param, 0, sizeof(param));
    param.key.direction = input ? IPIPSEC_DIR_IN : IPIPSEC_DIR_OUT;
    param.key.domain    = domain;
#ifdef IPCOM_USE_INET
    if (domain == IP_AF_INET)
        ipcom_memcpy(&param.key.dst, dstaddr, sizeof(struct Ip_in_addr));
#endif
#ifdef IPCOM_USE_INET6
    if (domain == IP_AF_INET6)
        ipcom_memcpy(&param.key.dst, dstaddr, sizeof(struct Ip_in6_addr));
#endif

    flow = ipipsec_flow_lookup(&param.key, IP_NULL, IPIPSEC_LFLAGS_NOCHECK_PORTS
                                                    | IPIPSEC_LFLAGS_NOCHECK_SRC
                                                    | IPIPSEC_LFLAGS_NOCHECK_PROTO);

    if (flow && flow->flowtype == SADB_X_FLOW_TYPE_USE)
    {
#ifdef FW_IPSEC_DEBUG
        ipcom_printf("FW_IPSEC :: flow lookup: %s\n", ipipsec_flow_name(flow, flow->sa));
#endif
        return flow;
    }

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_populate_sa
 *===========================================================================
 * Description: Populate a SA cache entry using a SA.
 * Parameters:  fw_ipsec_sa - SA cache entry
 *              sa          - security association
 * Returns:     0 = success
 *             -1 = fail
 */
IP_STATIC int
ipcom_forwarder_ipsec_populate_sa(Ipcom_fw_ipsec_sa_t *fw_ipsec_sa, Ipipsec_sa *sa)
{
    /* Setup encrypt functions */
    ipcom_memset(fw_ipsec_sa, 0, sizeof(*fw_ipsec_sa));

    fw_ipsec_sa->cache_id = ipsec_sa_cache_id;

    fw_ipsec_sa->spi      = sa->key.ports.espah_spi_n;
    fw_ipsec_sa->seq      = sa->seql;
    fw_ipsec_sa->domain   = sa->key.domain;
    fw_ipsec_sa->proto    = sa->key.proto;
    if (sa->sadb_sa_auth == SADB_AALG_MD5HMAC96)
        fw_ipsec_sa->algid = sa->sadb_sa_encrypt == SADB_EALG_3DESCBC ? IPCOM_FORWARDER_IPSEC_3DES_MD5 : IPCOM_FORWARDER_IPSEC_AES_MD5;
    else
        fw_ipsec_sa->algid = sa->sadb_sa_encrypt == SADB_EALG_3DESCBC ? IPCOM_FORWARDER_IPSEC_3DES_SHA1 : IPCOM_FORWARDER_IPSEC_AES_SHA1;
    fw_ipsec_sa->ivlen    = sa->encrypt->iv_length;
    fw_ipsec_sa->akeylen  = sa->auth->keyLen;
    fw_ipsec_sa->ekeylen  = sa->encrypt->bytes;
    fw_ipsec_sa->blocklen = sa->encrypt->alg->block_bytes;

    /* Setup keys */
    ipcom_memcpy(fw_ipsec_sa->akey, sa->esp_authkey, sa->auth->keyLen);
    ipcom_memcpy(fw_ipsec_sa->ekey, sa->esp_enckey, sa->encrypt->bytes);
    ipcom_memcpy(fw_ipsec_sa->iv, sa->encrypt->iv, sa->encrypt->iv_length);
    ipcom_memcpy(fw_ipsec_sa->ictx, &sa->auth->i_ctx, sizeof(fw_ipsec_sa->ictx));
    ipcom_memcpy(fw_ipsec_sa->octx, &sa->auth->o_ctx, sizeof(fw_ipsec_sa->octx));

    /* Setup addresses */
#ifdef IPCOM_USE_INET
    if (sa->key.domain == IP_AF_INET)
    {
        fw_ipsec_sa->src[0] = sa->key.src.in.s_addr;
        fw_ipsec_sa->dst[0] = sa->key.dst.in.s_addr;
    }
#endif

#ifdef IPCOM_USE_INET6
    if (sa->key.domain == IP_AF_INET6)
    {
        fw_ipsec_sa->src[0] = sa->key.src.in6.in6.addr32[0];
        fw_ipsec_sa->src[1] = sa->key.src.in6.in6.addr32[1];
        fw_ipsec_sa->src[2] = sa->key.src.in6.in6.addr32[2];
        fw_ipsec_sa->src[3] = sa->key.src.in6.in6.addr32[3];
        fw_ipsec_sa->dst[0] = sa->key.dst.in6.in6.addr32[0];
        fw_ipsec_sa->dst[1] = sa->key.dst.in6.in6.addr32[1];
        fw_ipsec_sa->dst[2] = sa->key.dst.in6.in6.addr32[2];
        fw_ipsec_sa->dst[3] = sa->key.dst.in6.in6.addr32[3];
    }
#endif

    /* Setup anti-replay */
    fw_ipsec_sa->replay         = sa->sadb_sa_replay;
    fw_ipsec_sa->replay_counter = sa->replay_counter;
    fw_ipsec_sa->replay_bitmap  = sa->replay_bitmap;

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_match_sa
 *===========================================================================
 * Description: Check if a SA matches a SA cache entry.
 * Parameters:  fw_ipsec_sa - SA cache entry
 *              sa          - security association
 * Returns:     IP_TRUE  = match
 *              IP_FALSE = no match
 */
IP_STATIC Ip_bool
ipcom_forwarder_ipsec_match_sa(Ipcom_fw_ipsec_sa_t *fw_ipsec_sa, Ipipsec_sa *sa)
{
    if (fw_ipsec_sa->spi != sa->key.ports.espah_spi_n
        || fw_ipsec_sa->domain != sa->key.domain
        || fw_ipsec_sa->proto != sa->key.proto)
    {
        return IP_FALSE;
    }

#ifdef IPCOM_USE_INET
    if (sa->key.domain == IP_AF_INET)
    {
        if (fw_ipsec_sa->dst[0] != sa->key.dst.in.s_addr)
            return IP_FALSE;
    }
#endif

#ifdef IPCOM_USE_INET6
    if (sa->key.domain == IP_AF_INET6)
    {
        if (fw_ipsec_sa->dst[0] != sa->key.dst.in6.in6.addr32[0]
            || fw_ipsec_sa->dst[1] != sa->key.dst.in6.in6.addr32[1]
            || fw_ipsec_sa->dst[2] != sa->key.dst.in6.in6.addr32[2]
            || fw_ipsec_sa->dst[3] != sa->key.dst.in6.in6.addr32[3])
        {
            return IP_FALSE;
        }
    }
#endif

    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_add_sa
 *===========================================================================
 * Description: Add a SA to the SA cache.
 * Parameters:  fw_ipsec_sa - SA cache entry
 *              sa          - security association
 *              if_mtu      - interface mtu
 * Returns:     0 = success
 *             -1 = fail
 */
IP_STATIC int
ipcom_forwarder_ipsec_add_sa(Ip_u8 domain, Ipipsec_sa *sa, fw_ipsec_t *fw_ipsec, Ip_u32 if_mtu)
{
    Ipcom_fw_ipsec_sa_t *fw_ipsec_sa;
    Ipipsec_sa *sa_master;
    Ipipsec_key sa_key;
    Ip_u16 key;

    /* Check IP forwarder IPsec limitations against the SA */
    if (sa->key.domain != domain)
    {
        /* IPv4 in IPv6 tunnel or vice versa is not supported */
        return -1;
    }

    if (sa->group_next != IP_NULL)
    {
        /* Grouped SAs not supported */
        return -1;
    }

    if (!IP_U64_ISZERO(sa->lthard.bytes) || !IP_U64_ISZERO(sa->ltsoft.bytes))
    {
        /* SA lifetime (bytes) not supported */
        return -1;
    }

    if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_ESN)
        || IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_TFC))
    {
        /* ESN / TFC not supported */
        return -1;
    }

    if (sa->key.direction == IPIPSEC_DIR_OUT &&
        (IP_BIT_ISFALSE(sa->sadb_sa_flags, SADB_X_SAFLAGS_TUNNEL)
         || IP_BIT_ISTRUE(sa->sadb_sa_flags, SADB_X_SAFLAGS_UDPENCAP)))
    {
        /* Must be tunnel mode and not UDP encapsulation */
        return -1;
    }

    if (sa->key.proto != IP_IPPROTO_ESP
        || (sa->sadb_sa_auth != SADB_AALG_MD5HMAC96 && sa->sadb_sa_auth != SADB_AALG_SHA1HMAC96)
        || (sa->sadb_sa_encrypt != SADB_EALG_3DESCBC && sa->sadb_sa_encrypt != SADB_X_EALG_AES))
    {
        /* Supported algorithms: 3DES/AES/MD5/SHA1 */
        return -1;
    }

    key = ipcom_forwarder_ipsec_tag_sa(sa->key.ports.espah_spi_n);
    fw_ipsec_sa = sa->key.direction == IPIPSEC_DIR_OUT
        ? &ipcom_fw.fw_cache_tbl_enc[key]
        : &ipcom_fw.fw_cache_tbl_dec[key];

    if (fw_ipsec_sa->cache_id != 0)
    {
        /* Find master SA */
        ipcom_memset(&sa_key, 0, sizeof(sa_key));
        sa_key.direction = sa->key.direction;
        sa_key.domain    = fw_ipsec_sa->domain;
        sa_key.proto     = fw_ipsec_sa->proto;
#ifdef IPCOM_USE_INET
        if (fw_ipsec_sa->domain == IP_AF_INET)
        {
            ipcom_memcpy(&sa_key.src, fw_ipsec_sa->src, sizeof(struct Ip_in_addr));
            ipcom_memcpy(&sa_key.dst, fw_ipsec_sa->dst, sizeof(struct Ip_in_addr));
        }
#endif
#ifdef IPCOM_USE_INET6
        if (fw_ipsec_sa->domain == IP_AF_INET6)
        {
            ipcom_memcpy(&sa_key.src, fw_ipsec_sa->src, sizeof(struct Ip_in6_addr));
            ipcom_memcpy(&sa_key.dst, fw_ipsec_sa->dst, sizeof(struct Ip_in6_addr));
        }
#endif
        sa_key.ports.espah_spi_n = fw_ipsec_sa->spi;

        /* Update master SA */
        sa_master = ipipsec_sa_lookup(&sa_key, 0);
        if (sa_master)
        {
#ifdef FW_IPSEC_DEBUG
            ipcom_printf("FW_IPSEC :: Update master SA! spi: %x dir: %s ix: %d domain: %d\n",
                         sa_master->key.ports.espah_spi_n, sa_master->key.direction == IPIPSEC_DIR_OUT ? "output" : "input",
                         key, sa_master->key.domain);
#endif
            if (sa_master->key.direction == IPIPSEC_DIR_OUT)
            {
                /* Copy sequence number */
                sa_master->seql = fw_ipsec_sa->seq;
            }

            if (sa_master->key.direction == IPIPSEC_DIR_IN)
            {
                /* Copy counter and bitmap */
                sa_master->replay_counter = fw_ipsec_sa->replay_counter;
                sa_master->replay_bitmap  = fw_ipsec_sa->replay_bitmap;
            }
        }

        /* Check for collision */
        if (ipcom_forwarder_ipsec_match_sa(fw_ipsec_sa, sa) == IP_FALSE)
        {
#ifdef FW_IPSEC_DEBUG
            ipcom_printf("FW_IPSEC :: SA collision! spi: %x, dir: %s ix: %d domain: %d cache id: %d\n",
                         sa->key.ports.espah_spi_n, sa->key.direction == IPIPSEC_DIR_OUT ? "output" : "input",
                         key, sa->key.domain, ipsec_sa_cache_id);
#endif
            /* Increment cache id after collision in order to invalidate forwarding cache entries */
            if (sa->key.direction == IPIPSEC_DIR_OUT)
                ipsec_sa_cache_id += 2;
        }
    }

    if (sa->key.direction == IPIPSEC_DIR_OUT)
    {
        fw_ipsec->key = key;
        fw_ipsec->mtu = (Ip_u16)if_mtu - ipcom_forwarder_ipsec_sa_header_space(sa);
        fw_ipsec->cache_id = ipsec_sa_cache_id;
    }

    /* Populate SA cache */
    ipcom_forwarder_ipsec_populate_sa(fw_ipsec_sa, sa);

#ifdef FW_IPSEC_DEBUG
    ipcom_printf("FW_IPSEC :: adding SA! spi: %x, dir: %s ix: %d domain: %d cache id: %d\n",
                 sa->key.ports.espah_spi_n, sa->key.direction == IPIPSEC_DIR_OUT ? "output" : "input",
                 key, sa->key.domain, ipsec_sa_cache_id);
#endif

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_lookup_sa
 *===========================================================================
 * Description: Lookup an IPv4 ESP packet in the SA cache.
 * Parameters:  ip4         - IPv4 header
 *              spi         - security parameter index
 *              fw_ipsec_sa - SA cache entry
 * Returns:     SA cache entry if packet matched or IP_NULL if not matched.
 */
#ifdef IPCOM_USE_INET
IP_INLINE Ipcom_fw_ipsec_sa_t *
ipcom_forwarder_ipsec_lookup_sa(Ipcom_ip_header *ip4, Ip_u32 spi, Ipcom_fw_ipsec_sa_t *fw_ipsec_sa)
{
    if (fw_ipsec_sa->src[0] == ip4->src.addr32    /* not alignment safe! */
        && fw_ipsec_sa->dst[0] == ip4->dst.addr32
        && fw_ipsec_sa->spi == spi
        && fw_ipsec_sa->domain == IP_AF_INET)
    {
        return fw_ipsec_sa;
    }

    return IP_NULL;
}
#endif /* IPCOM_USE_INET */


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_lookup_sa6
 *===========================================================================
 * Description: Lookup an IPv6 ESP packet in the SA cache.
 * Parameters:  ip6         - IPv4 header
 *              spi         - security parameter index
 *              fw_ipsec_sa - SA cache entry
 * Returns:     SA cache entry if packet matched or IP_NULL if not matched.
 */
#ifdef IPCOM_USE_INET6
IP_INLINE Ipcom_fw_ipsec_sa_t *
ipcom_forwarder_ipsec_lookup_sa6(Ipcom_ip6_header *ip6, Ip_u32 spi, Ipcom_fw_ipsec_sa_t *fw_ipsec_sa)
{
    if (fw_ipsec_sa->src[0] == ip6->src.in6.addr32[0]  /* not alignment safe! */
        && fw_ipsec_sa->src[1] == ip6->src.in6.addr32[1]
        && fw_ipsec_sa->src[2] == ip6->src.in6.addr32[2]
        && fw_ipsec_sa->src[3] == ip6->src.in6.addr32[3]
        && fw_ipsec_sa->dst[0] == ip6->dst.in6.addr32[0]
        && fw_ipsec_sa->dst[1] == ip6->dst.in6.addr32[1]
        && fw_ipsec_sa->dst[2] == ip6->dst.in6.addr32[2]
        && fw_ipsec_sa->dst[3] == ip6->dst.in6.addr32[3]
        && fw_ipsec_sa->spi == spi
        && fw_ipsec_sa->domain == IP_AF_INET6)
    {
        return fw_ipsec_sa;
    }

    return IP_NULL;
}
#endif /* IPCOM_USE_INET6 */


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_sprint_sa
 *===========================================================================
 * Description: Print a SA cache entry.
 * Parameters:  in  - boolean used to print input or output SAs
 *              buf - location where the SA is printed
 *              fw_ipsec_sa - SA cache entry
 * Returns:     nothing
 */
IP_PUBLIC void
ipcom_forwarder_ipsec_sprint_sa(Ip_bool in, char *buf, Ipcom_fw_ipsec_sa_t *fw_ipsec_sa)
{
    int  num;
    char addrstr1[64];
    char addrstr2[64];

    addrstr1[0] = '\0';
    addrstr2[0] = '\0';
    num = ipcom_sprintf(buf, "domain=%s proto=%s spi=0x%08x src=%s dst=%s",
                        fw_ipsec_sa->domain == IP_AF_INET ? "IPv4" : "IPv6",
                        ipcom_fw.fw_ipsec_func[fw_ipsec_sa->algid].id,
                        ip_ntohl(fw_ipsec_sa->spi),
                        ipcom_inet_ntop(fw_ipsec_sa->domain, fw_ipsec_sa->src, addrstr1, sizeof(addrstr1)),
                        ipcom_inet_ntop(fw_ipsec_sa->domain, fw_ipsec_sa->dst, addrstr2, sizeof(addrstr2)));
    if (num > 0)
    {
        if (in)
        {
            ipcom_sprintf(&buf[num], " replay(win:cnt:map)=%d:%d:0x%08x\n",
                          fw_ipsec_sa->replay,
                          fw_ipsec_sa->replay_counter,
                          fw_ipsec_sa->replay_bitmap);
        }
        else
        {
            ipcom_sprintf(&buf[num], " next_seq=%u\n", fw_ipsec_sa->seq);
        }
    }
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_populate_func_table
 *===========================================================================
 * Description: Configure the IPSec function table. For each combination
 *              of encryption and authentication algorithm functions
 *              for encryption and decryption are registered.
 * Parameters:  none.
 * Returns:     nothing.
 *
 */
IP_PUBLIC void
ipcom_forwarder_ipsec_populate_func_table(void)
{
    /* 3DES-MD5 */
    ipcom_fw.fw_ipsec_func[IPCOM_FORWARDER_IPSEC_3DES_MD5].encrypt =
        ipcom_forwarder_ipsec_esp_3des_md5_encrypt;
    ipcom_fw.fw_ipsec_func[IPCOM_FORWARDER_IPSEC_3DES_MD5].decrypt =
        ipcom_forwarder_ipsec_esp_3des_md5_decrypt;
    ipcom_fw.fw_ipsec_func[IPCOM_FORWARDER_IPSEC_3DES_MD5].id = "ESP-3DES-MD5";

    /* AES-MD5 */
    ipcom_fw.fw_ipsec_func[IPCOM_FORWARDER_IPSEC_AES_MD5].encrypt =
        ipcom_forwarder_ipsec_esp_aes_md5_encrypt;
    ipcom_fw.fw_ipsec_func[IPCOM_FORWARDER_IPSEC_AES_MD5].decrypt =
        ipcom_forwarder_ipsec_esp_aes_md5_decrypt;
    ipcom_fw.fw_ipsec_func[IPCOM_FORWARDER_IPSEC_AES_MD5].id = "ESP-AES-MD5";

    /* 3DES-SHA1 */
    ipcom_fw.fw_ipsec_func[IPCOM_FORWARDER_IPSEC_3DES_SHA1].encrypt =
        ipcom_forwarder_ipsec_esp_3des_sha1_encrypt;
    ipcom_fw.fw_ipsec_func[IPCOM_FORWARDER_IPSEC_3DES_SHA1].decrypt =
        ipcom_forwarder_ipsec_esp_3des_sha1_decrypt;
    ipcom_fw.fw_ipsec_func[IPCOM_FORWARDER_IPSEC_3DES_SHA1].id = "ESP-3DES-SHA1";

    /* AES-SHA1 */
    ipcom_fw.fw_ipsec_func[IPCOM_FORWARDER_IPSEC_AES_SHA1].encrypt =
        ipcom_forwarder_ipsec_esp_aes_sha1_encrypt;
    ipcom_fw.fw_ipsec_func[IPCOM_FORWARDER_IPSEC_AES_SHA1].decrypt =
        ipcom_forwarder_ipsec_esp_aes_sha1_decrypt;
    ipcom_fw.fw_ipsec_func[IPCOM_FORWARDER_IPSEC_AES_SHA1].id = "ESP-AES-SHA1";
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_check_replay_window
 *===========================================================================
 * Description: Check that the sequence number of a IPSec packet is within
 *              the specified anti-replay window size. Based on public code
 *              from RFC2401.
 * Parameters:  fw_ipsec_sa - SA cache entry
 *              seq         - sequence number
 * Returns:     0  = anti-replay check ok
 *              -1 = anti-replay check failed
 */
IP_PUBLIC int
ipcom_forwarder_ipsec_check_replay_window(Ipcom_fw_ipsec_sa_t *fw_ipsec_sa, Ip_u32 seq)
{
    Ip_u32 diff;

    if (IP_LIKELY(seq))
    {
        if (IP_LIKELY(seq > fw_ipsec_sa->replay_counter))
        {
            /* New larger sequence number */
            diff = seq - fw_ipsec_sa->replay_counter;
            if (IP_LIKELY(diff < fw_ipsec_sa->replay))
            {
                /* In window */
                fw_ipsec_sa->replay_bitmap <<= diff;
                fw_ipsec_sa->replay_bitmap |= 1;    /* set bit for this packet */
            }
            else
                fw_ipsec_sa->replay_bitmap = 1;     /* This packet has a "way larger" */

            fw_ipsec_sa->replay_counter = seq;
            return 0;
        }
        else
        {
            /* lower sequence number */
            diff = fw_ipsec_sa->replay_counter - seq;

            if (diff >= fw_ipsec_sa->replay)
                return -1;    /* too old or wrapped */

            if (fw_ipsec_sa->replay_bitmap & (1UL << diff))
                return -1;    /* already seen */

            fw_ipsec_sa->replay_bitmap |= (1UL << diff);   /* mark as seen */

            /* out of order but good */
            return 0;
        }
    }
    else
    {
        /* first == 0 or wrapped */
        return -1;
    }
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_check_replay
 *===========================================================================
 * Description:  Check anti-replay window for a slow path packet
 * Parameters:   sa  - security association
 *               seq - sequence number
 * Returns:      1 = SA not found
 *               0 = replay check ok
 *              -1 = replay check failed
 */
IP_PUBLIC int
ipcom_forwarder_ipsec_check_replay(Ipipsec_sa *sa, Ip_u32 seq)
{
    Ipcom_fw_ipsec_sa_t *fw_ipsec_sa;
    Ip_u16 key;
    int ret = 1;

    ip_assert(sa->key.direction == IPIPSEC_DIR_IN);
    ip_assert(seq != 0);

    if (ipcom_forwarder_ipsec_sa_cache_size)
    {
        IPCOM_FWD_LOCK();

        key = ipcom_forwarder_ipsec_tag_sa(sa->key.ports.espah_spi_n);
        fw_ipsec_sa = &ipcom_fw.fw_cache_tbl_dec[key];
        if (ipcom_forwarder_ipsec_match_sa(fw_ipsec_sa, sa) == IP_TRUE)
        {
            ret = ipcom_forwarder_ipsec_check_replay_window(fw_ipsec_sa, seq);
            if (ret == 0)
            {
                /* Copy counter and bitmap */
                sa->replay_counter = fw_ipsec_sa->replay_counter;
                sa->replay_bitmap  = fw_ipsec_sa->replay_bitmap;
            }

#ifdef FW_IPSEC_DEBUG
            ipcom_printf("FW_IPSEC :: Check replay=%x,%x - spi: %x, dir: %s ix: %d domain: %d\n",
                         fw_ipsec_sa->replay_counter, fw_ipsec_sa->replay_bitmap,
                         sa->key.ports.espah_spi_n, sa->key.direction == IPIPSEC_DIR_OUT ? "output" : "input",
                         key, sa->key.domain);
            if (ret < 0)
                ipcom_printf("FW_IPSEC :: Check replay failed!\n");
#endif
        }

        IPCOM_FWD_RELEASE();
    }

    return ret;
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_get_replay
 *===========================================================================
 * Description: Get anti-replay window size for a SA cache entry.
 * Parameters:  sa  - security association
 * Returns:     nothing.
 */
IP_PUBLIC void
ipcom_forwarder_ipsec_get_replay(Ipipsec_sa *sa)
{
    Ipcom_fw_ipsec_sa_t *fw_ipsec_sa;
    Ip_u16 key;

    ip_assert(sa->key.direction == IPIPSEC_DIR_IN);

    if (ipcom_forwarder_ipsec_sa_cache_size)
    {
        IPCOM_FWD_LOCK();

        key = ipcom_forwarder_ipsec_tag_sa(sa->key.ports.espah_spi_n);
        fw_ipsec_sa = &ipcom_fw.fw_cache_tbl_dec[key];
        if (ipcom_forwarder_ipsec_match_sa(fw_ipsec_sa, sa) == IP_TRUE)
        {
#ifdef FW_IPSEC_DEBUG
            ipcom_printf("FW_IPSEC :: Get replay=%x,%x - spi: %x, dir: %s ix: %d domain: %d\n",
                         fw_ipsec_sa->replay_counter, fw_ipsec_sa->replay_bitmap,
                         sa->key.ports.espah_spi_n, sa->key.direction == IPIPSEC_DIR_OUT ? "output" : "input",
                         key, sa->key.domain);
#endif
            /* Copy counter and bitmap */
            sa->replay_counter = fw_ipsec_sa->replay_counter;
            sa->replay_bitmap  = fw_ipsec_sa->replay_bitmap;
        }

        IPCOM_FWD_RELEASE();
    }
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_get_seq
 *===========================================================================
 * Description: Get sequence number for a SA cache entry
 * Parameters:  sa -  security association
 * Returns:     Sequence number.
 */
IP_PUBLIC Ip_u32
ipcom_forwarder_ipsec_get_seq(Ipipsec_sa *sa)
{
    Ipcom_fw_ipsec_sa_t *fw_ipsec_sa;
    Ip_u16 key;
    Ip_u32 seq = sa->seql;

    ip_assert(sa->key.direction == IPIPSEC_DIR_OUT);

    if (ipcom_forwarder_ipsec_sa_cache_size)
    {
        IPCOM_FWD_LOCK();

        key = ipcom_forwarder_ipsec_tag_sa(sa->key.ports.espah_spi_n);
        fw_ipsec_sa = &ipcom_fw.fw_cache_tbl_enc[key];
        if (ipcom_forwarder_ipsec_match_sa(fw_ipsec_sa, sa) == IP_TRUE)
        {
            seq = fw_ipsec_sa->seq;
            if (fw_ipsec_sa->replay == 0 || fw_ipsec_sa->seq != 0)
            {
#ifdef FW_IPSEC_DEBUG
                ipcom_printf("FW_IPSEC :: Get sequence no=%u - spi: %x, dir: %s ix: %d domain: %d\n",
                             seq, sa->key.ports.espah_spi_n, sa->key.direction == IPIPSEC_DIR_OUT ? "output" : "input",
                             key, sa->key.domain);
#endif
                /* Increment if anti-replay is deactivated or if not wrapped */
                fw_ipsec_sa->seq++;
            }
        }

        IPCOM_FWD_RELEASE();
    }

    return seq;
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_remove_sa
 *===========================================================================
 * Description: Remove an SA cache entry
 * Parameters:  sa -  security association
 * Returns:     nothing.
 */
IP_PUBLIC void
ipcom_forwarder_ipsec_remove_sa(Ipipsec_sa *sa)
{
    Ipcom_fw_ipsec_sa_t *fw_ipsec_sa;
    Ip_u16 key;

    if (ipcom_forwarder_ipsec_sa_cache_size)
    {
        IPCOM_FWD_LOCK();

        key = ipcom_forwarder_ipsec_tag_sa(sa->key.ports.espah_spi_n);
        fw_ipsec_sa = sa->key.direction == IPIPSEC_DIR_OUT
            ? &ipcom_fw.fw_cache_tbl_enc[key]
            : &ipcom_fw.fw_cache_tbl_dec[key];

        if (ipcom_forwarder_ipsec_match_sa(fw_ipsec_sa, sa) == IP_TRUE)
        {
            ipcom_memset(fw_ipsec_sa, 0, sizeof(*fw_ipsec_sa));

#ifdef FW_IPSEC_DEBUG
            ipcom_printf("FW_IPSEC :: Remove SA - spi: %x, dir: %s ix: %d domain: %d cache id: %d\n",
                         sa->key.ports.espah_spi_n, sa->key.direction == IPIPSEC_DIR_OUT ? "output" : "input",
                         key, sa->key.domain, ipsec_sa_cache_id);
#endif
        }

        IPCOM_FWD_RELEASE();
    }
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_set_fw_if
 *===========================================================================
 * Description: Find the IPsec flow for a packet and add a SA cache entry.
 * Parameters:  p        - packet
 *              pkt      - the Ipcom_pkt
 *              mtu      - egress interface mtu
 *              fw_ipsec - IPSec forwarding cache entry
 * Returns:     IPCOM_FWD_IPSEC_NOCACHE  - do not create fast forwarding cache entry
 *              IPCOM_FWD_IPSEC_CLEARTXT - no encryption or decryption
 *              IPCOM_FWD_IPSEC_ENCRYPT  - mark cache entry for encryption
 *              IPCOM_FWD_IPSEC_DECRYPT  - mark cache entry for decryption
 */
IP_PUBLIC Ip_u8
ipcom_forwarder_ipsec_set_fw_if(Ip_u8 *p, Ipcom_pkt *pkt,
                                Ip_u32 mtu, fw_ipsec_t *fw_ipsec)
{
    Ip_u32 *dstaddr;
    Ip_u32 dstaddr_n[5]; /* Five elements needed to fit IPv6 address and the seed */
    Ip_u16 type;
    Ip_u8 domain = 0;
    Ipipsec_flow *flow;
    Ip_u32 fw_key;

    /* Get domain */
    type = IP_GET_NTOHS(&p[-2]);
    if (type == IP_IPv4)
    {
        domain = IP_AF_INET;
        dstaddr = (Ip_u32 *)p + 4;
    }
    else if (type == IP_IPv6)
    {
        domain = IP_AF_INET6;
        dstaddr = (Ip_u32 *)p + 6;
    }
#ifdef IPCOM_USE_FORWARDER_VLAN
    else if (type == IP_VLAN)
    {
        type = IP_GET_NTOHS(&p[2]);
        if (type == IP_IPv4)
        {
            domain = IP_AF_INET;
            dstaddr = (Ip_u32 *)p + 5;
        }
        else if (type == IP_IPv6)
        {
            domain = IP_AF_INET6;
            dstaddr = (Ip_u32 *)p + 7;
        }
        else
            return IPCOM_FWD_IPSEC_CLEARTXT; /* unknown ethernet type */
    }
#endif /* IPCOM_USE_FORWARDER_VLAN */
    else
        return IPCOM_FWD_IPSEC_CLEARTXT; /* unknown ethernet type */

    /* Check output flow */
    flow = ipcom_forwarder_ipsec_flow_lookup(domain, dstaddr, IP_FALSE);
    if (flow)
    {
        if (pkt->ipsec_hlen)
        {
            if (ipcom_forwarder_ipsec_add_sa(domain, flow->sa, fw_ipsec, mtu) < 0)
            {
                return IPCOM_FWD_IPSEC_NOCACHE;
            }

            return IPCOM_FWD_IPSEC_ENCRYPT;
        }
        else
        {
            /* If a flow matches but the packet was not encrypted in slow path we must not
             * add a cache entry. This could happen if the user configures a flow
             * with for example a TCP/UDP port interval and the packet is outside of the
             * port interval. If the cache is populated and a future packet is inside of the
             * interval it would be forwarded without encryption.
             */
#ifdef FW_IPSEC_DEBUG
            fw_key = ipcom_forwarder_pkt_get_key(pkt);
            ipcom_printf("FW_IPSEC :: IPsec flow (output) matched clear text packet, ix: %d domain: %d cache id: %d\n",
                         fw_key, domain, ipsec_sa_cache_id);
#endif
            return IPCOM_FWD_IPSEC_NOCACHE;
        }
    }

    /* Check input flow */
    flow = ipcom_forwarder_ipsec_flow_lookup(domain, dstaddr, IP_TRUE);
    if (flow)
    {
        if (IP_BIT_ISTRUE(pkt->flags, IPCOM_PKT_FLAG_IPSEC_DONE))
        {
            if (ipcom_forwarder_ipsec_add_sa(domain, flow->sa, fw_ipsec, mtu) < 0)
            {
                return IPCOM_FWD_IPSEC_NOCACHE;
            }

            /* Create a new tag based on the destination address of the decrypted packet and the seed */
            if (domain == IP_AF_INET)
            {
                ipcom_memcpy(dstaddr_n, dstaddr, 1 * 4);
                dstaddr_n[1] = ipcom_forwarder_get_seed();
                fw_key =
                    (ipcom_forwarder_hw_crc16(dstaddr_n, 2) & (ipcom_fw.fw_v4_entries - 1)) <<
                    IPCOM_FORWARDER_TRUE_CACHE_WAYS_SHIFT;

            }
            else
            {
                ipcom_memcpy(dstaddr_n, dstaddr, 4 * 4);
                dstaddr_n[4] = ipcom_forwarder_get_seed();
                fw_key =
                    (ipcom_forwarder_hw_crc16(dstaddr_n, 5) & (ipcom_fw.fw_v6_entries - 1)) <<
                    IPCOM_FORWARDER_TRUE_CACHE_WAYS_SHIFT;
            }
            ipcom_forwarder_pkt_set_key(pkt, fw_key);

            return IPCOM_FWD_IPSEC_DECRYPT;
        }
        else
        {
            /* Do not add a cache entry for the same reason as output flows */
#ifdef FW_IPSEC_DEBUG
            fw_key = ipcom_forwarder_pkt_get_key(pkt);
            ipcom_printf("FW_IPSEC :: IPsec flow (input) matched clear text packet, ix: %d domain: %d cache id: %d\n",
                         fw_key, domain, ipsec_sa_cache_id);
#endif
            return IPCOM_FWD_IPSEC_NOCACHE;
        }
    }

    /* No IPsec flow matched */
    return IPCOM_FWD_IPSEC_CLEARTXT;
}


/*
 *===========================================================================
 *                    ipipsec_forwarder_ipsec_input
 *===========================================================================
 * Description: Decrypt incoming ESP packet.
 * Parameters:  domain          - domain the packet belongs to
 *              fw_ipsec_replay - anti-replay window
 *              p               - packet
 *              length          - length of packet
 *              headers         - number of bytes to be removed from the start
 *              trailers        - number of packets to be removed from the end
 * Returns:     1 = skipped
 *              0 = ok
 *             -1 = failed
 *             -2 = HMAC failed
 */
IP_PUBLIC int
ipcom_forwarder_ipsec_input(Ip_u8 *domain, fw_ipsec_replay_t *fw_ipsec_replay,
                            Ip_u8 *p, unsigned int length,
                            unsigned int *headers, unsigned int *trailers)
{
    unsigned int clen, hlen;
    int ret = -1;
    Ip_u32 *hmac_p, spi_n;
    Ip_u32 hmac[3];
    Ipcom_fw_ipsec_sa_t *fw_ipsec_sa;
    Ip_u16 key;

#ifdef IPCOM_USE_INET
    if (*domain == IP_AF_INET)
    {
        Ipcom_ip_header *ip4;

        ip4 = (Ipcom_ip_header *)p;
        if (ip4->protocol != IP_IPPROTO_ESP)
            return 1;
        hlen  = (ip4->vers_len & 0x0f) << 2;
        spi_n = *(Ip_u32 *)&p[hlen];  /* not alignment safe! */

        key = ipcom_forwarder_ipsec_tag_sa(spi_n);
        fw_ipsec_sa = &ipcom_fw.fw_cache_tbl_dec[key];
        if (ipcom_forwarder_ipsec_lookup_sa(ip4, spi_n, fw_ipsec_sa) == IP_NULL)
            return 1;
    }
    else
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    if (*domain == IP_AF_INET6)
    {
        Ipcom_ip6_header *ip6;

        ip6 = (Ipcom_ip6_header *)p;
        if (ip6->nxt != IP_IPPROTO_ESP)
            return 1;
        hlen  = IPCOM_IP6_HEADER_LENGTH;
        spi_n = *(Ip_u32 *)&p[hlen];  /* not alignment safe! */

        key = ipcom_forwarder_ipsec_tag_sa(spi_n);
        fw_ipsec_sa = &ipcom_fw.fw_cache_tbl_dec[key];
        if (ipcom_forwarder_ipsec_lookup_sa6(ip6, spi_n, fw_ipsec_sa) == IP_NULL)
            return 1;
    }
    else
#endif /* IPCOM_USE_INET6 */
        return 1;

#ifdef FW_IPSEC_DEBUG
    ipcom_printf("FW_IPSEC :: ipsec input found v%d SA at index: %d\n",
                 fw_ipsec_sa->domain == IP_AF_INET ? 4 : 6, key);
#endif

    /* Encrypted payload length should be at least 1 block and of even block-size. */
    *headers = hlen + IPCOM_FORWARDER_IPSEC_ESP_HLEN + fw_ipsec_sa->ivlen;
    clen = length - *headers - IPCOM_FORWARDER_IPSEC_HMAC_LEN;
    if ((int)clen <= 0 || (clen & (fw_ipsec_sa->blocklen-1)))
        return 1; /* skip */

    /* Save incoming HMAC */
    hmac_p    = (Ip_u32 *)&p[length - IPCOM_FORWARDER_IPSEC_HMAC_LEN];
    hmac[0]   = hmac_p[0];
    hmac[1]   = hmac_p[1];
    hmac[2]   = hmac_p[2];

    /* Single pass ESP decrypt */
    ret = ipcom_fw.fw_ipsec_func[fw_ipsec_sa->algid].decrypt(fw_ipsec_sa->ekeylen, fw_ipsec_sa->ekey,
                                                             fw_ipsec_sa->akeylen, fw_ipsec_sa->akey,
                                                             &p[hlen + IPCOM_FORWARDER_IPSEC_ESP_HLEN],
                                                             &p[hlen], length - hlen,
                                                             &p[hlen],
                                                             fw_ipsec_sa->ictx, fw_ipsec_sa->octx);
    if (ret == 0)
    {
        /* Compare OCTEON created HMAC versus incoming HMAC */
        if (hmac[0] != hmac_p[0] || hmac[1] != hmac_p[1] || hmac[2] != hmac_p[2])
            return -2;

        /* Check length of clear text packet */
        *trailers = p[length - 2 - IPCOM_FORWARDER_IPSEC_HMAC_LEN] + 2 + IPCOM_FORWARDER_IPSEC_HMAC_LEN;
        if (length < *headers + *trailers)
            return -1;

        /* Assume tunnel mode and check inner domain */
        if ((p[0] & 0xf0) == 0x40)
            *domain = IP_AF_INET;
        else if ((p[0] & 0xf0) == 0x60)
            *domain = IP_AF_INET6;
        else
            return -1;

        /* Prepare for anti-replay check */
        fw_ipsec_replay->replay = fw_ipsec_sa->replay;
        fw_ipsec_replay->key    = key;
        fw_ipsec_replay->seq    = ip_ntohl(*(Ip_u32 *)&p[hlen+4]); /* not alignment safe! */
    }

    return ret;
}


/*
 *===========================================================================
 *                    ipipsec_forwarder_ipsec_output
 *===========================================================================
 * Description: Encrypt outgoing packet and create the ESP header.
 * Parameters:  ipsec    - IPSec forwarding cache entry
 *              p        - packet
 *              length   - length of packet
 *              headers  - number of ESP header bytes
 *              trailers - number of ESP trailer bytes
 * Returns:     1 = skipped
 *              0 = ok
 *             -1 = failed
 */
IP_PUBLIC int
ipcom_forwarder_ipsec_output(fw_ipsec_t *ipsec,
                             Ip_u8 *p, unsigned int length,
                             unsigned int *headers, unsigned int *trailers)
{
    int i, remainder, ret;
    unsigned int align;
    Ip_u32 seq, esp[2];
    Ipcom_fw_ipsec_sa_t *fw_ipsec_sa;
    Ip_u64 iv[2];

    if (IP_LIKELY(length <= ipsec->mtu))
    {
        fw_ipsec_sa = &ipcom_fw.fw_cache_tbl_enc[ipsec->key];
        if (IP_LIKELY(ipsec->cache_id == fw_ipsec_sa->cache_id))
        {
#ifdef FW_IPSEC_DEBUG
            ipcom_printf("FW_IPSEC :: ipsec output found v%d SA at index: %d\n",
                         fw_ipsec_sa->domain == IP_AF_INET ? 4 : 6, ipsec->key);
#endif

            if (IP_LIKELY(fw_ipsec_sa->replay == 0))
            {
                seq = fw_ipsec_sa->seq++;
            }
            else
            {
                fw_lock_key_t lkey = ipcom_forwarder_atomic_lock(ipsec->key);
                if (IP_UNLIKELY(fw_ipsec_sa->seq == 0))
                {
                    ipcom_forwarder_atomic_unlock(ipsec->key, lkey);
#ifdef FW_IPSEC_DEBUG
                    ipcom_printf("FW_IPSEC :: SA replay wrap for index: %d\n", ipsec->key);
#endif
                    return 1;
                }
                seq = fw_ipsec_sa->seq++;
                ipcom_forwarder_atomic_unlock(ipsec->key, lkey);
            }
        }
        else
        {

#ifdef FW_IPSEC_DEBUG
            ipcom_printf("FW_IPSEC :: previous output SA collision! (fw cache id=%d SA cache id=%d)\n",
                         ipsec->cache_id, fw_ipsec_sa->cache_id);
#endif
            return 1;
        }

        /* Add padding */
        remainder = (length + 2) & (fw_ipsec_sa->blocklen - 1);
        for (i = 0; remainder && i < (fw_ipsec_sa->blocklen - remainder); i++)
            p[length + i] = i + 1;
        p[length + i] = i;
        *trailers = i + 2 + IPCOM_FORWARDER_IPSEC_HMAC_LEN;
        *headers  = sizeof(esp) + fw_ipsec_sa->ivlen;

#ifdef IPCOM_USE_INET
        if (fw_ipsec_sa->domain == IP_AF_INET)
        {
            Ipcom_ip_header *inner_ip, *outer_ip;

            align = 4;
            p[length + i + 1] = IP_IPPROTO_IPIP;
            *headers += IPCOM_IP_HEADER_LENGTH;

            /* Add outer IPv4 header */
            inner_ip = (Ipcom_ip_header *)p;
            outer_ip = (Ipcom_ip_header *)(p - *headers - align);
            outer_ip->vers_len = 0x45;
            outer_ip->tos      = inner_ip->tos;
            outer_ip->length   = ip_htons(*headers + length + *trailers);
            outer_ip->id       = inner_ip->id;
            outer_ip->frag     = inner_ip->frag & IPCOM_IPFRAG_DF;
            outer_ip->ttl      = inner_ip->ttl;
            outer_ip->protocol = IP_IPPROTO_ESP;
            outer_ip->src.addr32 = fw_ipsec_sa->src[0]; /* not alignment safe! */
            outer_ip->dst.addr32 = fw_ipsec_sa->dst[0]; /* not alignment safe! */
            outer_ip->checksum = 0;
            outer_ip->checksum = ipcom_in_checksum(outer_ip, IPCOM_IP_HEADER_LENGTH); /* calculate header checksum */

            /* Adjust for alignment */
            *headers  += align;
            *trailers -= align;
        }
        else
#endif
#ifdef IPCOM_USE_INET6
        if (fw_ipsec_sa->domain == IP_AF_INET6)
        {
            Ipcom_ip6_header *inner_ip, *outer_ip;

            align = 0;
            p[length + i + 1] = IP_IPPROTO_IPV6;
            *headers += IPCOM_IP6_HEADER_LENGTH;

            /* Add outer IPv6 header */
            inner_ip = (Ipcom_ip6_header *)p;
            outer_ip = (Ipcom_ip6_header *)(p - *headers);
            outer_ip->ver_class_flow = ip_htonl(ip_ntohl(inner_ip->ver_class_flow) & 0xfff00000); /* not alignment safe! */
            outer_ip->plen = ip_htons(*headers - IPCOM_IP6_HEADER_LENGTH + length + *trailers);
            outer_ip->nxt  = IP_IPPROTO_ESP;
            outer_ip->hlim = inner_ip->hlim;
            outer_ip->src.in6.addr32[0] = fw_ipsec_sa->src[0]; /* not alignment safe! */
            outer_ip->src.in6.addr32[1] = fw_ipsec_sa->src[1];
            outer_ip->src.in6.addr32[2] = fw_ipsec_sa->src[2];
            outer_ip->src.in6.addr32[3] = fw_ipsec_sa->src[3];
            outer_ip->dst.in6.addr32[0] = fw_ipsec_sa->dst[0];
            outer_ip->dst.in6.addr32[1] = fw_ipsec_sa->dst[1];
            outer_ip->dst.in6.addr32[2] = fw_ipsec_sa->dst[2];
            outer_ip->dst.in6.addr32[3] = fw_ipsec_sa->dst[3];
        }
        else
#endif /* IPCOM_USE_INET6 */
            return -1;

        /* Prepare ESP header */
        esp[0] = fw_ipsec_sa->spi;
        esp[1] = ip_htonl(seq);

        /* Single pass ESP encrypt */
        iv[0] = fw_ipsec_sa->iv[0];
        iv[1] = fw_ipsec_sa->iv[1];
        ret = ipcom_fw.fw_ipsec_func[fw_ipsec_sa->algid].encrypt(fw_ipsec_sa->ekeylen, fw_ipsec_sa->ekey,
                                                                 fw_ipsec_sa->akeylen, fw_ipsec_sa->akey,
                                                                 (Ip_u8 *)&esp, (Ip_u8 *)iv,
                                                                 p, length + i + 2,
                                                                 p - sizeof(esp) - fw_ipsec_sa->ivlen - align,
                                                                 fw_ipsec_sa->ictx, fw_ipsec_sa->octx);
        /* Return new context */
        fw_ipsec_sa->iv[0] = iv[0];
        fw_ipsec_sa->iv[1] = iv[1];
        return ret;
    }
    else
    {
        /* Let the slow path deal with fragmentation */
#ifdef FW_IPSEC_DEBUG
        ipcom_printf("FW_IPSEC :: must fragment -- length(%d) > mtu(%d)\n",
                     length, ipsec->mtu);
#endif
        return 1;
    }
}


/*
 *===========================================================================
 *                    ipipsec_forwarder_ipsec_recrypt
 *===========================================================================
 * Description: Re-encrypt a previously decrypted packet.
 * Parameters:  p      - packet
 *              length - length of packet.
 * Returns:     0 = ok
 *             -1 = failed
 */
IP_PUBLIC int
ipcom_forwarder_ipsec_recrypt(Ip_u8 *p, unsigned int length)
{
    int ret, hlen;
    Ipcom_fw_ipsec_sa_t *fw_ipsec_sa;
    Ip_u8 iv[16];
    Ip_u32 spi_n;
    Ip_u16 key;

#ifdef IPCOM_USE_INET
    if ((p[0] & 0xf0) == 0x40)
    {
        Ipcom_ip_header *ip4;

        ip4 = (Ipcom_ip_header *)p;
        if (ip4->protocol != IP_IPPROTO_ESP)
            return -1;
        hlen  = (ip4->vers_len & 0x0f) << 2;
        spi_n = *(Ip_u32 *)&p[hlen];  /* not alignment safe! */

        key = ipcom_forwarder_ipsec_tag_sa(spi_n);
        fw_ipsec_sa = &ipcom_fw.fw_cache_tbl_dec[key];
        if (ipcom_forwarder_ipsec_lookup_sa(ip4, spi_n, fw_ipsec_sa) == IP_NULL)
            return -1;
    }
    else
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    if ((p[0] & 0xf0) == 0x60)
    {
        Ipcom_ip6_header *ip6;

        ip6 = (Ipcom_ip6_header *)p;
        if (ip6->nxt != IP_IPPROTO_ESP)
            return -1;
        hlen  = IPCOM_IP6_HEADER_LENGTH;
        spi_n = *(Ip_u32 *)&p[hlen];  /* not alignment safe! */

        key = ipcom_forwarder_ipsec_tag_sa(spi_n);
        fw_ipsec_sa = &ipcom_fw.fw_cache_tbl_dec[key];
        if (ipcom_forwarder_ipsec_lookup_sa6(ip6, spi_n, fw_ipsec_sa) == IP_NULL)
            return -1;
    }
    else
#endif /* IPCOM_USE_INET6 */
        return -1;

#ifdef FW_IPSEC_DEBUG
        ipcom_printf("FW_IPSEC :: ipsec recrypt found v%d SA at index: %d\n",
                     fw_ipsec_sa->domain == IP_AF_INET ? 4 : 6, key);
#endif

    length -= hlen + IPCOM_FORWARDER_IPSEC_ESP_HLEN + fw_ipsec_sa->ivlen + IPCOM_FORWARDER_IPSEC_HMAC_LEN;
    if (length & (fw_ipsec_sa->blocklen-1))
        return -1;
    p += hlen;
    ipcom_memcpy(iv, p + IPCOM_FORWARDER_IPSEC_ESP_HLEN, fw_ipsec_sa->ivlen);

    /* Single pass ESP encrypt */
    ret = ipcom_fw.fw_ipsec_func[fw_ipsec_sa->algid].encrypt(fw_ipsec_sa->ekeylen, fw_ipsec_sa->ekey,
                                                             fw_ipsec_sa->akeylen, fw_ipsec_sa->akey,
                                                             p, iv,
                                                             p + IPCOM_FORWARDER_IPSEC_ESP_HLEN + fw_ipsec_sa->ivlen,
                                                             length,
                                                             p,
                                                             fw_ipsec_sa->ictx, fw_ipsec_sa->octx);
    return ret;
}

#else
int ipcom_forwarder_ipsec_empty_file;
#endif /* IPCOM_USE_FORWARDER_IPSEC */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
