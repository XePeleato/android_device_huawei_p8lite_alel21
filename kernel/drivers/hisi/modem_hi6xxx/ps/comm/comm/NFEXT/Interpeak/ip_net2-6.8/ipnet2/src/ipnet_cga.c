/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_cga.c,v $ $Revision: 1.6 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_cga.c,v $
 *   $Author: rboden $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Roger Boden <roger.boden@windriver.com>
 ******************************************************************************
 */


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
#include <ipcom_syslog.h>
#include <ipcom_err.h>

#include <ipcom_file.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_cga.h"
#include "ipnet_cga_h.h"
#include "ipnet_send.h"
#include "ipnet_icmp6.h"

#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/bio.h>
#include <openssl/rand.h>

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_CGA_MODIFIER_LEN 16


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

IP_STATIC void ipnet_cga_calc_hash1(Ipnet_cga_param* cga_param, Ip_u8 *hash);
IP_STATIC void ipnet_cga_calc_hash2(Ipnet_cga_param* cga_param, Ip_u8 *hash);
IP_STATIC Ip_err ipnet_cga_verify_modifier(IP_CONST char *modifier);
IP_STATIC void ipnet_cga_add_modifier(IP_CONST char *modifier, Ip_u8 *buf);
IP_STATIC Ip_u8 ipnet_cga_hex2bin(IP_CONST char hex);

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC char   ipnet_cga_log_buf[128];

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipnet_cga_hex2bin
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_u8
ipnet_cga_hex2bin(IP_CONST char hex)
{
    if (hex >= '0' && hex <= '9')
        return hex - '0';
    if (hex >= 'a' && hex <= 'f')
        return hex - 'a' + 10;
    if (hex >= 'A' && hex <= 'F')
        return hex - 'A' + 10;

    IP_PANIC();

    return 0xff;
}


/*
 *===========================================================================
 *                    ipnet_cga_bin2hex
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char
ipnet_cga_bin2hex(Ip_u8 bin)
{
    if (bin <= 9)
        return bin + '0';
    if (bin >= 0xA && bin <= 0xF)
        return bin + 'A' - 10;

    IP_PANIC();

    return 'X';
}


/*
 *===========================================================================
 *                    ipnet_cga_modifier_bin2hex
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_cga_modifier_bin2hex(Ip_u8 *bin, int buf_len, char *hex)
{
    int i;

    for (i=0; i<buf_len; i++)
    {
        hex[2*i] = ipnet_cga_bin2hex(bin[i]>>4);
        hex[2*i+1] = ipnet_cga_bin2hex(bin[i]&0xF);
    }
    hex[2*i] = 0;
}


/*
 *===========================================================================
 *                    ipnet_cga_add_modifier
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cga_add_modifier(IP_CONST char *modifier, Ip_u8 *buf)
{
    int i;

    for (i=0; i<IPNET_CGA_MODIFIER_LEN; i++)
    {
        buf[i] = ipnet_cga_hex2bin(modifier[2*i])*16+ipnet_cga_hex2bin(modifier[2*i+1]);
    }
}


/*
 *===========================================================================
 *                    ipnet_cga_verify_modifier
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipnet_cga_verify_modifier(IP_CONST char *modifier)
{
    int i;

    if (ipcom_strlen(modifier) != IPNET_CGA_MODIFIER_LEN*2)
        return IPCOM_ERR_INVALID_ARG;

    for (i=0; i<IPNET_CGA_MODIFIER_LEN*2; i++)
    {
        if ((modifier[i] >= '0' && modifier[i] <= '9') ||
            (modifier[i] >= 'a' && modifier[i] <= 'f') ||
            (modifier[i] >= 'A' && modifier[i] <= 'F'))
        {
            continue;
        }

        /* Illegal character found in modifier */
        return IPCOM_ERR_INVALID_ARG;
    }

    return IPCOM_SUCCESS;
}

/*
 *===========================================================================
 *                    ipnet_cga_verify_hash2
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipnet_cga_verify_hash2(Ip_u8* hash2, int sec)
{
    int bytes;
    int i;

    bytes = 2*sec;
    for (i=0; i<bytes; i++)
        if(hash2[i] != 0)
            return IPCOM_ERR_FAILED;

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_cga_calc_hash1
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cga_calc_hash1(Ipnet_cga_param* cga_param, Ip_u8 *hash)
{
    SHA_CTX sha_ctx;


    SHA1_Init(&sha_ctx);
    SHA1_Update(&sha_ctx, cga_param->modifier, sizeof(cga_param->modifier));
    SHA1_Update(&sha_ctx, cga_param->subnet_prefix, sizeof(cga_param->subnet_prefix));
    SHA1_Update(&sha_ctx, &cga_param->collision_count, sizeof(cga_param->collision_count));
    SHA1_Update(&sha_ctx, cga_param->der_encoded_key, cga_param->der_key_len);
    SHA1_Final(hash, &sha_ctx);
}


/*
 *===========================================================================
 *                    ipnet_cga_calc_hash2
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cga_calc_hash2(Ipnet_cga_param* cga_param, Ip_u8 *hash)
{
    SHA_CTX sha_ctx;
    Ip_u8 zero_bytes[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};


    SHA1_Init(&sha_ctx);

    SHA1_Update(&sha_ctx, cga_param->modifier, sizeof(cga_param->modifier));
    SHA1_Update(&sha_ctx, zero_bytes, sizeof(zero_bytes));
    SHA1_Update(&sha_ctx, cga_param->der_encoded_key, cga_param->der_key_len);
    SHA1_Final(hash, &sha_ctx);
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_cga_generate
 *===========================================================================
 * Description:
 * From RFC 3972:

    A CGA and associated parameters SHOULD be generated as follows:

   1. Set the modifier to a random or pseudo-random 128-bit value.

   2. Concatenate from left to right the modifier, 9 zero octets, the
      encoded public key, and any optional extension fields.  Execute
      the SHA-1 algorithm on the concatenation.  Take the 112 leftmost
      bits of the SHA-1 hash value.  The result is Hash2.

   3. Compare the 16*Sec leftmost bits of Hash2 with zero.  If they are
      all zero (or if Sec=0), continue with step 4.  Otherwise,
      increment the modifier by one and go back to step 2.

   4. Set the 8-bit collision count to zero.

   5. Concatenate from left to right the final modifier value, the
      subnet prefix, the collision count, the encoded public key, and
      any optional extension fields.  Execute the SHA-1 algorithm on the
      concatenation.  Take the 64 leftmost bits of the SHA-1 hash value.
      The result is Hash1.

   6. Form an interface identifier from Hash1 by writing the value of
      Sec into the three leftmost bits and by setting bits 6 and 7
      (i.e., the "u" and "g" bits) to zero.

   7. Concatenate the 64-bit subnet prefix and the 64-bit interface
      identifier to form a 128-bit IPv6 address with the subnet prefix
      to the left and interface identifier to the right, as in a
      standard IPv6 address [RFC3513].

   8. Perform duplicate address detection if required, as per [RFC3971].
      If an address collision is detected, increment the collision count
      by one and go back to step 5.  However, after three collisions,
      stop and report the error.

   9. Form the CGA Parameters data structure by concatenating from left
      to right the final modifier value, the subnet prefix, the final
      collision count value, the encoded public key, and any optional
      extension fields.

 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipnet_cga_generate(Ipnet_cga_param *cga_param,
                   struct Ip_in6_addr *cga_addr)
{
    Ip_err rc = IPCOM_ERR_FAILED;
    Ip_u8  hash1[SHA_DIGEST_LENGTH];
    Ip_u8  hash2[SHA_DIGEST_LENGTH];

    /* Check input arguments */
    if (cga_param->sec > 7 || cga_param->sec < 0)
        return IPCOM_ERR_INVALID_ARG;

    /* 1. */
    if (!RAND_bytes(cga_param->modifier, sizeof(cga_param->modifier)))
    {
        IPCOM_LOG0(ERR, "ipnet_cga_generate :: RAND_bytes() failed");
        rc = IPCOM_ERR_FAILED;
        goto cleanup;
    }

    while (1)
    {
        /* 2. */
        ipnet_cga_calc_hash2(cga_param, hash2);

        /* 3. */
        if (ipnet_cga_verify_hash2(hash2, cga_param->sec) != IPCOM_SUCCESS)
        {
            Ip_u64 tmp;

            /* Increment modifier */
            ipcom_memcpy(&tmp, &cga_param->modifier[8], 8);
            IP_U64_ADD32(tmp, 1);
            ipcom_memcpy(&cga_param->modifier[8], &tmp, 8);
            continue;
        }

        /* If we get here, we have generated a valid hash2 */
        break;
    }

    /* 5. */
    ipnet_cga_calc_hash1(cga_param, hash1);

    /* 6. */
    hash1[0] = (cga_param->sec << 5) + (hash1[0] & (034));

    /* 7 */
    ipcom_memcpy(cga_addr, cga_param->subnet_prefix, 8);
    ipcom_memcpy(&cga_addr->in6.addr8[8], hash1, 8);

    rc = IPCOM_SUCCESS;

 cleanup:

    return rc;
}

/*
 *===========================================================================
 *                    ipnet_cga_verify
 *===========================================================================
 * Description:

   The CGA MUST be verified with the following steps:

   1. Check that the collision count in the CGA Parameters data
      structure is 0, 1, or 2.  The CGA verification fails if the
      collision count is out of the valid range.

   2. Check that the subnet prefix in the CGA Parameters data structure
      is equal to the subnet prefix (i.e., the leftmost 64 bits) of the
      address.  The CGA verification fails if the prefix values differ.

   3. Execute the SHA-1 algorithm on the CGA Parameters data structure.
      Take the 64 leftmost bits of the SHA-1 hash value.  The result is
      Hash1.

   4. Compare Hash1 with the interface identifier (i.e., the rightmost
      64 bits) of the address.  Differences in the three leftmost bits
      and in bits 6 and 7 (i.e., the "u" and "g" bits) are ignored.  If
      the 64-bit values differ (other than in the five ignored bits),
      the CGA verification fails.

   5. Read the security parameter Sec from the three leftmost bits of
      the 64-bit interface identifier of the address.  (Sec is an
      unsigned 3-bit integer.)

   6. Concatenate from left to right the modifier, 9 zero octets, the
      public key, and any extension fields that follow the public key in
      the CGA Parameters data structure.  Execute the SHA-1 algorithm on
      the concatenation.  Take the 112 leftmost bits of the SHA-1 hash
      value.  The result is Hash2.

   7. Compare the 16*Sec leftmost bits of Hash2 with zero.  If any one
      of them is not zero, the CGA verification fails.  Otherwise, the
      verification succeeds.  (If Sec=0, the CGA verification never
      fails at this step.)

 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipnet_cga_verify(Ipnet_cga_param *cga_param,
                 struct Ip_in6_addr *cga_addr)
{
    Ip_u8  hash1[SHA_DIGEST_LENGTH];
    Ip_u8  hash2[SHA_DIGEST_LENGTH];
    int  sec;

    /* 1. */
    if (cga_param->collision_count > 2)
        return IPCOM_ERR_INVALID_ARG;

    /* 2. */
    if (ipcom_memcmp(cga_param->subnet_prefix, cga_addr, sizeof(cga_param->subnet_prefix)) != 0)
        return IPCOM_ERR_INVALID_ARG;

    /* 3. */
    ipnet_cga_calc_hash1(cga_param, hash1);
    IPCOM_LOG0(DEBUG2, "ipnet_cga_verify :: hash1");
    ipnet_send_log_hex_buffer(hash1, 8);

    /* 4. */
    /* Check first byte with bits 0,1,2 (sec) and bits 6,7 set to zero */
    if ((hash1[0] & 034) != (cga_addr->in6.addr8[8] & 034))
        return IPCOM_ERR_FAILED;
    /* Check remaining 7 bytes */
    if (ipcom_memcmp(&hash1[1], &cga_addr->in6.addr8[9], 7))
        return IPCOM_ERR_FAILED;

    /* 5. */
    sec = (cga_addr->in6.addr8[8] & ((1<<7)+(1<<6)+(1<<5))) >> 5;
    IPCOM_LOG1(DEBUG2, "ipnet_cga_verify :: sec: %d", sec);
    if (sec != cga_param->sec)
        return IPCOM_ERR_FAILED;

    /* 6. */
    ipnet_cga_calc_hash2(cga_param, hash2);
    IPCOM_LOG0(DEBUG2, "ipnet_cga_verify :: hash2");
    ipnet_send_log_hex_buffer(hash2, 14);

    /* 7. */
    return ipnet_cga_verify_hash2(hash2, sec);
}


/*
 *===========================================================================
 *                    ipnet_cga_icmp6_opt_add
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipnet_cga_icmp6_opt_add(Ip_u8 *der_rsa_key,
                        int der_len,
                        struct Ip_in6_addr *src_addr,
                        int ifindex,
                        Ip_u8 *buf,
                        int *len)
{
    char ifname[IP_IFNAMSIZ];
    char sysvar_name[80+IP_IFNAMSIZ];
    char *modifier;
    Ip_err ret = IPCOM_ERR_FAILED;
    Ipnet_pkt_nd_opt_cga *cga_opt;


    cga_opt = (Ipnet_pkt_nd_opt_cga*)buf;
    cga_opt->nd_opt_cga_type = IPNET_ND_OPT_CGA;


    *len = 4; /* Option header */

    if (ipcom_if_indextoname(ifindex, ifname) == IP_NULL)
    {
        IPCOM_LOG1(ERR, "ipnet_cga_add_option :: invalid ifindex: %d", ifindex);
        return IPCOM_ERR_FAILED;
    }

    ipcom_sprintf(sysvar_name,"ipnet.inet6.cga.%s.modifier.%s", ifname,
                  ipcom_inet_ntop(IP_AF_INET6, src_addr, ipnet_cga_log_buf, sizeof(ipnet_cga_log_buf)));
    modifier = ipcom_sysvar_get(sysvar_name, IP_NULL, IP_NULL);
    if (!modifier)
    {
        IPCOM_LOG2(ERR, "No CGA modifier set for i/f %s (sysvar %s)",
                   ifname, sysvar_name);
        return IPCOM_ERR_FAILED;
    }

    if (ipnet_cga_verify_modifier(modifier) != IPCOM_SUCCESS)
    {
        IPCOM_LOG2(ERR, "Invalid modifier for i/f %s: %s",
                   ifname, modifier);
        goto cleanup;
    }

    ipnet_cga_add_modifier(modifier, &buf[*len]);
    *len += IPNET_CGA_MODIFIER_LEN;

    ipcom_memcpy(&buf[*len], src_addr, 8);
    *len += 8;

    /* Collision count */
    buf[*len] = 0;
    *len += 1;

    ipcom_memcpy(&buf[*len], der_rsa_key, der_len);
    *len += der_len;

    /* Fix padding */
    cga_opt->nd_opt_cga_pad_len = 0;
    if ((*len)%8 != 0)
    {
        ipcom_memset(&buf[*len], 0, 8-(*len)%8);
        cga_opt->nd_opt_cga_pad_len = 8-(*len)%8;
        *len += 8-(*len)%8;
    }

    cga_opt->nd_opt_cga_len = *len/8;

    ret = IPCOM_SUCCESS;

 cleanup:
    if (modifier)
        ipcom_free(modifier);

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_cga_icmp6_opt_verify
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipnet_cga_icmp6_opt_verify(Ipnet_pkt_nd_opt_cga *opt_cga, struct Ip_in6_addr *cga_addr)
{
    Ipnet_cga_param cga_param;
    RSA *rsa;
    Ip_err rc = IPCOM_ERR_FAILED;

    ipcom_memset(&cga_param, 0, sizeof(cga_param));
    ipcom_memcpy(cga_param.modifier, opt_cga->modifier, sizeof(cga_param.modifier));
    ipcom_memcpy(cga_param.subnet_prefix, opt_cga->subnet_prefix, sizeof(cga_param.subnet_prefix));
    cga_param.collision_count = opt_cga->collision_count;
    rsa = ipnet_cga_icmp6_opt_get_rsa_key(opt_cga);
    if (!rsa)
    {
        return IPCOM_ERR_FAILED;
    }
    if (ipnet_cga_param_set_rsa(&cga_param, rsa) != IPCOM_SUCCESS)
        goto cleanup;

    cga_param.sec = (cga_addr->in6.addr8[8]) >> 5;

    rc = ipnet_cga_verify(&cga_param, cga_addr);

 cleanup:
    RSA_free(rsa);
    if (cga_param.rsa_key)
        RSA_free(cga_param.rsa_key);

    if (cga_param.der_encoded_key)
        ipcom_free(cga_param.der_encoded_key);

    return rc;
}


/*
 *===========================================================================
 *                    ipnet_cga_opt_get_rsa_key
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL RSA*
ipnet_cga_icmp6_opt_get_rsa_key(Ipnet_pkt_nd_opt_cga *opt_cga)
{
    RSA         *rsa = IP_NULL;
    EVP_PKEY    *pkey;
    X509_PUBKEY *x509_pkey;
    Ip_u8       *ptr;

    if (!opt_cga)
    {
        return IP_NULL;
    }

    ptr = opt_cga->pub_key;
    x509_pkey = d2i_X509_PUBKEY(IP_NULL,
                                (const unsigned char **)&ptr,
                                IPNET_CGA_ICMP6_OPT_RSA_LEN(opt_cga));
    if (!x509_pkey)
    {
        IPCOM_LOG0(DEBUG, "ipnet_cga_icmp6_opt_get_rsa_key :: Failed to unpack RSAkey from CGA option");
        return IP_NULL;
    }

    pkey = X509_PUBKEY_get(x509_pkey);
    if (!pkey)
    {
        IPCOM_LOG0(DEBUG, "ipnet_cga_icmp6_opt_get_rsa_key :: X509_PUBKEY_get");
        goto cleanup;
    }
    rsa = EVP_PKEY_get1_RSA(pkey);

 cleanup:
    if (pkey)
        EVP_PKEY_free(pkey);
    if (x509_pkey)
        X509_PUBKEY_free(x509_pkey);

    return rsa;
}


/*
 *===========================================================================
 *                    ipnet_cga_der_encode_rsa_public_key
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_cga_der_encode_rsa_public_key(Ip_u8 **ppbuf, RSA* rsa)
{
    X509_PUBKEY *x509_pkey = IP_NULL;
    EVP_PKEY *pkey = IP_NULL;
    int der_key_len = 0;
    Ip_u8 *tmp;

    pkey = EVP_PKEY_new();
    if (!pkey)
    {
        IPCOM_LOG0(DEBUG, "ipnet_cga_der_encode_rsa_public_key :: EVP_PKEY_new() failed");
        return IPCOM_ERR_FAILED;
    }
    if (EVP_PKEY_set1_RSA(pkey, rsa) == 0)
    {
        IPCOM_LOG0(DEBUG, "ipnet_cga_der_encode_rsa_public_key :: EVP_PKEY_set1_RSA() failed");
        goto cleanup;
    }
    if (X509_PUBKEY_set(&x509_pkey, pkey) == 0)
    {
        IPCOM_LOG0(DEBUG, "ipnet_cga_der_encode_rsa_public_key :: X509_PUBKEY_set failed");
        goto cleanup;
    }
    der_key_len = i2d_X509_PUBKEY(x509_pkey, IP_NULL);
    if (der_key_len <= 0)
    {
        IPCOM_LOG0(DEBUG, "ipnet_cga_der_encode_rsa_public_key :: i2d_X509_PUBKEY failed");
        goto cleanup;
    }
    *ppbuf = ipcom_malloc(der_key_len);
    if (!*ppbuf)
    {
        IPCOM_LOG0(DEBUG, "ipnet_cga_der_encode_rsa_public_key :: out of memory");
        goto cleanup;
    }
    tmp = *ppbuf;
    if (i2d_X509_PUBKEY(x509_pkey, &tmp) < 0)
    {
        IPCOM_LOG0(DEBUG, "ipnet_cga_der_encode_rsa_public_key :: i2d_X509_PUBKEY failed 2");
        goto cleanup;
    }

 cleanup:
    EVP_PKEY_free(pkey);
    if (x509_pkey)
        X509_PUBKEY_free(x509_pkey);

    return der_key_len;
}


/*
 *===========================================================================
 *                    ipnet_cga_param_set_rsa
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipnet_cga_param_set_rsa(Ipnet_cga_param *cga_param, RSA *rsa)
{
    int der_key_len;

    der_key_len = ipnet_cga_der_encode_rsa_public_key(&cga_param->der_encoded_key, rsa);
    if (der_key_len <= 0)
        return IPCOM_ERR_FAILED;

    cga_param->der_key_len = der_key_len;
    cga_param->rsa_key = rsa;
    RSA_up_ref(cga_param->rsa_key);

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_cga_param_new
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ipnet_cga_param*
ipnet_cga_param_new(void)
{
    return ipcom_calloc(1, sizeof(Ipnet_cga_param));
}


/*
 *===========================================================================
 *                    ipnet_cga_param_free
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_cga_param_free(Ipnet_cga_param* cga_param)
{
    if (cga_param->der_encoded_key)
        ipcom_free(cga_param->der_encoded_key);
    if (cga_param->rsa_key)
        RSA_free(cga_param->rsa_key);

    ipcom_free(cga_param);
}

/*
 *===========================================================================
 *                    ipnet_cga_is_valid_addr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipnet_cga_is_valid_addr(struct Ip_in6_addr *addr, int ifindex)
{
    char ifname[IP_IFNAMSIZ];
    char sysvar_name[80+IP_IFNAMSIZ];
    char* modifier;

    if (ipcom_if_indextoname(ifindex, ifname) == IP_NULL)
        return IPCOM_ERR_FAILED;

    ipcom_sprintf(sysvar_name,"ipnet.inet6.cga.%s.modifier.%s", ifname,
                  ipcom_inet_ntop(IP_AF_INET6, addr, ipnet_cga_log_buf, sizeof(ipnet_cga_log_buf)));
    modifier = ipcom_sysvar_get(sysvar_name, IP_NULL, IP_NULL);
    if (!modifier)
         return IPCOM_ERR_FAILED;

    ipcom_free(modifier);

    return IPCOM_SUCCESS;
}

/*
 *===========================================================================
 *                    ipnet_cga_get_sec
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_cga_get_sec(struct Ip_in6_addr *cga_addr)
{
    return (cga_addr->in6.addr8[8] & ((1<<7)+(1<<6)+(1<<5))) >> 5;
}

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#else
int ipnet_cga_empty_file;
#endif /* IPNET_USE_RFC3971 */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

