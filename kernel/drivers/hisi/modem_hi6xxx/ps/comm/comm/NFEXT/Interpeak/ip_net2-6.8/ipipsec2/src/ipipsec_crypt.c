/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name:  $ $RCSfile: ipipsec_crypt.c,v $ $Revision: 1.29.4.2 $
 *   $Source: /home/interpeak/CVSRoot/ipipsec2/src/ipipsec_crypt.c,v $
 *   $Author: hxu4 $
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
#include <ipcom_syslog.h>
#include <ipcom_clib.h>

#include "ipipsec_h.h"
#include "ipipsec_pfkeyv2.h"
#ifdef OPENSSL_FIPS
#include <openssl/fips.h>
#endif

/*
 ****************************************************************************
 * 4                    EXTERN PROTOTYPES
 ****************************************************************************
 */

IP_EXTERN Ipipsec_data  ipipsec;

/*
 ****************************************************************************
 * 5                    DEFINES
 ****************************************************************************
 */

#define IPCOM_SYSLOG_PRIORITY    IPIPSEC_SYSTEM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPIPSEC


/*
 ****************************************************************************
 * 6                    TYPES
 ****************************************************************************
 */


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

#ifdef IPIPSEC_USE_AESMAC
IP_STATIC int AESMAC_Init(Ipipsec_auth_ctx *ctx);
IP_STATIC void AESMAC_Update(Ipipsec_auth_ctx *ctx, IP_CONST Ip_u8 *data, Ip_u32 length);
IP_STATIC void AESMAC_Final(Ip_u8 *digest, Ipipsec_auth_ctx *ctx);
#endif

#if defined(IPIPSEC_USE_AES) || defined(IPIPSEC_USE_AESMAC)
IP_STATIC Ip_s32  ipipsec_aes_setkey(Ip_u8 *secret, Ip_u32 length, Ipipsec_encrypt_key *ekey);
IP_STATIC void ipipsec_aes_encrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv);
#endif

#ifdef IPIPSEC_USE_ESP
#ifdef IPIPSEC_USE_AES
IP_STATIC void ipipsec_aes_decrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv);
#endif
#ifdef IPIPSEC_USE_AESCTR
IP_STATIC Ip_s32 ipipsec_aesctr_setkey(Ip_u8 *secret, Ip_u32 length, Ipipsec_encrypt_key *ekey);
IP_STATIC void ipipsec_aesctr_encrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv);
#endif
#endif


#ifdef IPIPSEC_USE_ESP

#ifdef IPIPSEC_USE_DES
IP_STATIC void ipipsec_descbc_encrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv);
IP_STATIC void ipipsec_descbc_decrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv);
IP_STATIC Ip_s32  ipipsec_descbc_setkey(Ip_u8 *secret, Ip_u32 length, Ipipsec_encrypt_key *ekey);
#endif

#ifdef IPIPSEC_USE_3DES
IP_STATIC void ipipsec_3descbc_encrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv);
IP_STATIC void ipipsec_3descbc_decrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv);
IP_STATIC Ip_s32  ipipsec_3descbc_setkey(Ip_u8 *secret, Ip_u32 length, Ipipsec_encrypt_key *ekey);
#endif

#ifdef IPIPSEC_USE_CAST
IP_STATIC void ipipsec_cast_encrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv);
IP_STATIC void ipipsec_cast_decrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv);
IP_STATIC Ip_s32  ipipsec_cast_setkey(Ip_u8 *secret, Ip_u32 length, Ipipsec_encrypt_key *ekey);
#endif

#ifdef IPIPSEC_USE_BLOWFISH
IP_STATIC void ipipsec_blowfish_encrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv);
IP_STATIC void ipipsec_blowfish_decrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv);
IP_STATIC Ip_s32  ipipsec_blowfish_setkey(Ip_u8 *secret, Ip_u32 length, Ipipsec_encrypt_key *ekey);
#endif

#endif /* IPIPSEC_USE_ESP */


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */


/*
 *===========================================================================
 *                         ipipsec_auth_method
 *===========================================================================
 */

IP_STATIC IP_CONST Ipipsec_auth_method   ipipsec_auth_method[SADB_AALG_MAX+1] =
{
  /*********** [0] NONE ***********/
  {
    0, 0, 0, 0, IP_NULL, IP_NULL, IP_NULL
  },


  /*********** [1] MD5HMAC ***********/
  {
    0, 0, 0, 0, IP_NULL, IP_NULL, IP_NULL
  },


  /*********** [2] SHA1HMAC ***********/
  {
    0, 0, 0, 0, IP_NULL, IP_NULL, IP_NULL
  },


  /*********** [3] MD5HMAC96 ***********/
  {
#ifdef IPIPSEC_USE_MD5
    MD5_DIGEST_LENGTH /* 16 */ , MD5_DIGEST_LENGTH /* 16 */ ,
    MD5_DIGEST_LENGTH /* 16 */ , IPIPSEC_HMAC_AUTHLEN /* 12 */,
    (Hash_init)MD5_Init,
    (Hash_update)MD5_Update,
    (Hash_final)MD5_Final
#else
    0, 0, 0, 0, IP_NULL, IP_NULL, IP_NULL
#endif
  },


  /*********** [4] SHA1HMAC96 ***********/
  {
#ifdef IPIPSEC_USE_SHA
    SHA_DIGEST_LENGTH /* 20 */, SHA_DIGEST_LENGTH /* 20 */,
    SHA_DIGEST_LENGTH /* 20 */, IPIPSEC_HMAC_AUTHLEN /* 12 */,
    (Hash_init)SHA1_Init,
    (Hash_update)SHA1_Update,
    (Hash_final)SHA1_Final
#else
    0, 0, 0, 0, IP_NULL, IP_NULL, IP_NULL
#endif
  },


  /*********** [5] RIPEMD160HMAC96 ***********/
  {
#ifdef IPIPSEC_USE_RIPEMD
    RIPEMD160_DIGEST_LENGTH /* 20 */, RIPEMD160_DIGEST_LENGTH /* 20 */,
    RIPEMD160_DIGEST_LENGTH /* 20 */, IPIPSEC_HMAC_AUTHLEN /* 12 */,
    (Hash_init)RIPEMD160_Init,
    (Hash_update)RIPEMD160_Update,
    (Hash_final)RIPEMD160_Final
#else
    0, 0, 0, 0, IP_NULL, IP_NULL, IP_NULL
#endif
  },


  /*********** [6] SADB_X_AALG_MD5 (OLD AH - Keyed MD5) ***********/
  {
#ifdef IPIPSEC_USE_MD5
    4, 16,
    MD5_DIGEST_LENGTH /* 16 */, MD5_DIGEST_LENGTH /* 16 */,
    (Hash_init)MD5_Init,
    (Hash_update)MD5_Update,
    (Hash_final)MD5_Final
#else
    0, 0, 0, 0, IP_NULL, IP_NULL, IP_NULL
#endif
  },


  /*********** [7] SADB_X_AALG_SHA1 (OLD AH - Keyed SHA1) ***********/
  {
#ifdef IPIPSEC_USE_SHA
    4, 20,
    SHA_DIGEST_LENGTH /* 20 */, SHA_DIGEST_LENGTH /* 20 */,
    (Hash_init)SHA1_Init,
    (Hash_update)SHA1_Update,
    (Hash_final)SHA1_Final
#else
    0, 0, 0, 0, IP_NULL, IP_NULL, IP_NULL
#endif
  },


  /*********** [8] SADB_X_AALG_AESXCBCMAC96 ***********/
  {
#ifdef IPIPSEC_USE_AESMAC
    AES_BLOCK_SIZE /* 16 */, AES_BLOCK_SIZE /* 16 */,
    AES_BLOCK_SIZE /* 16 */, IPIPSEC_HMAC_AUTHLEN /* 12 */,
    (Hash_init)AESMAC_Init,
    (Hash_update)AESMAC_Update,
    (Hash_final)AESMAC_Final
#else
    0, 0, 0, 0, IP_NULL, IP_NULL, IP_NULL
#endif
  }

};


/*
 *===========================================================================
 *                    ipipsec_encrypt_method
 *===========================================================================
 * Crypt entry:
 * {
 *    key_size,
 *    minkey_bytes, maxkey_bytes, block_bytes, iv_bytes,
 *    encrypt function,
 *    decrypt function,
 *    setkey function
 * }
 *
 */
#ifdef IPIPSEC_USE_ESP

#define MAX_ENC_ALG    (SADB_EALG_MAX + 1)

IP_STATIC IP_CONST Ipipsec_encrypt_method   ipipsec_encrypt_method[MAX_ENC_ALG] =
{
  /*********** [0] NONE ***********/
  {
      0, 0, 0, 0, 0, IP_NULL, IP_NULL, IP_NULL   /* Valid but not used for null cipher. */
  },


  /*********** [1] DESCBC (64 bits -acctually 56 bits key) ***********/
  {
#ifdef IPIPSEC_USE_DES
      sizeof(des_key_schedule) + sizeof(Ipipsec_sa_encrypt) - sizeof(Ipipsec_encrypt_key),
      8, 8, 8, 8,
      ipipsec_descbc_encrypt,
      ipipsec_descbc_decrypt,
      ipipsec_descbc_setkey
#else
      sizeof(Ipipsec_sa_encrypt) - sizeof(Ipipsec_encrypt_key),
      8, 8, 8, 8,
      IP_NULL, IP_NULL, IP_NULL
#endif
  },


  /*********** [2] 3DESCBC ***********/
  {
#ifdef IPIPSEC_USE_3DES
      3 * sizeof(des_key_schedule) + sizeof(Ipipsec_sa_encrypt) - sizeof(Ipipsec_encrypt_key),
      24, 24, 8, 8,
      ipipsec_3descbc_encrypt,
      ipipsec_3descbc_decrypt,
      ipipsec_3descbc_setkey
#else
      sizeof(Ipipsec_sa_encrypt) - sizeof(Ipipsec_encrypt_key),
      24, 24, 8, 8,
      IP_NULL, IP_NULL, IP_NULL
#endif
  },


  /*********** [3] BLOWFISH ***********/
  {
#ifdef IPIPSEC_USE_BLOWFISH
      sizeof(BF_KEY) + sizeof(Ipipsec_sa_encrypt) - sizeof(Ipipsec_encrypt_key),
      5, IPIPSEC_BLF_MAXKEYLEN, 8, 8,
      ipipsec_blowfish_encrypt,
      ipipsec_blowfish_decrypt,
      ipipsec_blowfish_setkey
#else
      sizeof(Ipipsec_sa_encrypt) - sizeof(Ipipsec_encrypt_key),
      5, IPIPSEC_BLF_MAXKEYLEN, 8, 8,
      IP_NULL, IP_NULL, IP_NULL
#endif
  },


  /*********** [4] CAST ***********/
  {
#ifdef IPIPSEC_USE_CAST
      sizeof(CAST_KEY) + sizeof(Ipipsec_sa_encrypt) - sizeof(Ipipsec_encrypt_key),
      5, 16, 8, 8,
      ipipsec_cast_encrypt,
      ipipsec_cast_decrypt,
      ipipsec_cast_setkey
#else
      sizeof(Ipipsec_sa_encrypt) - sizeof(Ipipsec_encrypt_key),
      5, 16, 8, 8,
      IP_NULL, IP_NULL, IP_NULL
#endif
  },


  /*********** [5] Skipjack ***********/
  {
      0, 0, 0, 0, 0, IP_NULL, IP_NULL, IP_NULL
  },


  /*********** [6] AESCBC (128,192,256 bits only) ***********/
  {
#ifdef IPIPSEC_USE_AES
      2 * sizeof(AES_KEY) + sizeof(Ipipsec_sa_encrypt) - sizeof(Ipipsec_encrypt_key),
      16, 32, 16, 16,
      ipipsec_aes_encrypt,
      ipipsec_aes_decrypt,
      ipipsec_aes_setkey
#else
      sizeof(Ipipsec_sa_encrypt) - sizeof(Ipipsec_encrypt_key),
      16, 32, 16, 16,
      IP_NULL, IP_NULL, IP_NULL
#endif
  },

  /*********** [7] AESCTR (128,192,256 bits + 32 bits nonce) ***********/
  {
#ifdef IPIPSEC_USE_AESCTR
      sizeof(struct aesctr_struct) + sizeof(Ipipsec_sa_encrypt) - sizeof(Ipipsec_encrypt_key),
      20, 36, 16, 8,
      ipipsec_aesctr_encrypt,
      ipipsec_aesctr_encrypt,
      ipipsec_aesctr_setkey
#else
      sizeof(Ipipsec_sa_encrypt) - sizeof(Ipipsec_encrypt_key),
      16, 16, 16, 8,
      IP_NULL, IP_NULL, IP_NULL
#endif
  }
};
#endif /* #ifdef IPIPSEC_USE_ESP */


/*
 ****************************************************************************
 * 9                    ENCRYPTION FUNCTIONS
 ****************************************************************************
 */

#ifdef IPIPSEC_USE_ESP

/*
 *===========================================================================
 *                    ipipsec_descbc_encrypt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPIPSEC_USE_DES
IP_STATIC void
ipipsec_descbc_encrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv)
{
    des_cbc_encrypt(data, data, length, key->des, (des_cblock *)iv, DES_ENCRYPT);
}


/*
 *===========================================================================
 *                    ipipsec_descbc_decrypt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipipsec_descbc_decrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv)
{
    des_cbc_encrypt(data, data, length, key->des, (des_cblock *)iv, DES_DECRYPT);
}


/*
 *===========================================================================
 *                    ipipsec_descbc_setkey
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_s32
ipipsec_descbc_setkey(Ip_u8 *secret, Ip_u32 length, Ipipsec_encrypt_key *key)
{
    if (length != DES_KEY_SZ)
        return -1;

    if (des_set_key((const_des_cblock *)secret, key->des) != 0)
        return -1;

    return 0;
}
#endif /* #ifdef IPIPSEC_USE_DES */


/*
 *===========================================================================
 *                    ipipsec_3descbc_encrypt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPIPSEC_USE_3DES
IP_STATIC void
ipipsec_3descbc_encrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv)
{
    des_ede3_cbc_encrypt(data, data, length,
			 key->des3.key1, key->des3.key2, key->des3.key3,
			 (des_cblock *)iv, DES_ENCRYPT);
}


/*
 *===========================================================================
 *                    ipipsec_3descbc_decrypt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipipsec_3descbc_decrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv)
{
    des_ede3_cbc_encrypt(data, data, length,
                         key->des3.key1, key->des3.key2, key->des3.key3,
                         (des_cblock *)iv, DES_DECRYPT);
}


/*
 *===========================================================================
 *                    ipipsec_3descbc_setkey
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_s32
ipipsec_3descbc_setkey(Ip_u8 *secret, Ip_u32 length, Ipipsec_encrypt_key *key)
{
    /* 3 keys a 8 bytes. */
    if (length != 24)
        return -1;

    /* Check that the 3 des keys differ. */
    if (ipcom_memcmp(secret, secret + DES_KEY_SZ, DES_KEY_SZ) == 0 ||
        ipcom_memcmp(secret + DES_KEY_SZ, secret + 2*DES_KEY_SZ, DES_KEY_SZ) == 0)
        return -1;

    /* Set the 3 keys. */
    if (des_set_key((const_des_cblock *)secret, key->des3.key1) != 0)
        return -1;
    if (des_set_key((const_des_cblock *)(secret + 8), key->des3.key2) != 0)
        return -1;
    if (des_set_key((const_des_cblock *)(secret + 16), key->des3.key3) != 0)
        return -1;

    return 0;
}
#endif /* #ifdef IPIPSEC_USE_3DES */


/*
 *===========================================================================
 *                    ipipsec_cast_encrypt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPIPSEC_USE_CAST
IP_STATIC void
ipipsec_cast_encrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv)
{
    CAST_cbc_encrypt(data, data, length, &key->cast, iv, CAST_ENCRYPT);
}


/*
 *===========================================================================
 *                    ipipsec_cast_decrypt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipipsec_cast_decrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv)
{
    CAST_cbc_encrypt(data, data, length, &key->cast, iv, CAST_DECRYPT);
}


/*
 *===========================================================================
 *                    ipipsec_cast_setkey
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_s32
ipipsec_cast_setkey(Ip_u8 *secret, Ip_u32 length, Ipipsec_encrypt_key *key)
{
    CAST_set_key(&key->cast, length, secret);
    return 0;
}
#endif /* #ifdef IPIPSEC_USE_CAST */


/*
 *===========================================================================
 *                    ipipsec_blowfish_encrypt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPIPSEC_USE_BLOWFISH
IP_STATIC void
ipipsec_blowfish_encrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv)
{
    BF_cbc_encrypt(data, data, length, &key->bf, iv, BF_ENCRYPT);
}


/*
 *===========================================================================
 *                    ipipsec_blowfish_decrypt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipipsec_blowfish_decrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv)
{
    BF_cbc_encrypt(data, data, length, &key->bf, iv, BF_DECRYPT);
}


/*
 *===========================================================================
 *                    ipipsec_blowfish_setkey
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_s32
ipipsec_blowfish_setkey(Ip_u8 *secret, Ip_u32 length, Ipipsec_encrypt_key *key)
{
    BF_set_key(&key->bf, length, secret);
    return 0;
}
#endif /* #ifdef IPIPSEC_USE_BLOWFISH */

#endif /* #ifdef IPIPSEC_USE_ESP */


/*
 *===========================================================================
 *                    ipipsec_crypt_random_fill
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipipsec_crypt_random_fill(Ip_u8 *buf, int length)
{
    int i;

    /*!!FIXME: too expensive! */
    for (i = 0; i < length; i++)
        buf[i] = (Ip_u8)ipcom_random();
}


/*
 *===========================================================================
 *                    ipipsec_aes_decrypt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPIPSEC_USE_AES
#ifdef IPIPSEC_USE_ESP
IP_STATIC void
ipipsec_aes_decrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv)
{
    AES_cbc_encrypt(data, data, (unsigned long)length, (AES_KEY *)&key->aes[1],
                    (unsigned char *)iv, AES_DECRYPT);
}
#endif
#endif /* IPIPSEC_USE_AES */


#if defined(IPIPSEC_USE_AES) || defined(IPIPSEC_USE_AESMAC)
/*
 *===========================================================================
 *                    ipipsec_aes_encrypt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipipsec_aes_encrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv)
{
    AES_cbc_encrypt(data, data, (unsigned long)length, (AES_KEY *)&key->aes[0],
                    (unsigned char *)iv, AES_ENCRYPT);
}


/*
 *===========================================================================
 *                    ipipsec_aes_setkey
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_s32
ipipsec_aes_setkey(Ip_u8 *secret, Ip_u32 length, Ipipsec_encrypt_key *key)
{
    if (length != 16 && length != 24 && length != 32)
        return -1;

    (void)AES_set_encrypt_key(secret, length * 8, &key->aes[0]);
    (void)AES_set_decrypt_key(secret, length * 8, &key->aes[1]);

    return 0;
}
#endif /* defined(IPIPSEC_USE_AES) || defined(IPIPSEC_USE_AESMAC) */


#ifdef IPIPSEC_USE_AESCTR
/*
 *===========================================================================
 *                    ipipsec_aesctr_encrypt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipipsec_aesctr_encrypt(Ip_u8 *data, Ip_u32 length, Ipipsec_encrypt_key *key, Ip_u8 *iv)
{
    unsigned char ivec[AES_BLOCK_SIZE];

    ivec[0] = key->aesctr.nonce[0];
    ivec[1] = key->aesctr.nonce[1];
    ivec[2] = key->aesctr.nonce[2];
    ivec[3] = key->aesctr.nonce[3];
    ipcom_memcpy(ivec + 4, iv, 8);
    ivec[12] = 0; ivec[13] = 0; ivec[14] = 0; ivec[15] = 1;

    AES_ctr128_encrypt(data, data, (const unsigned long)length, (AES_KEY *)&key->aesctr.key,
                       ivec,
                       key->aesctr.ecount_buf,
                       &key->aesctr.num);
}


/*
 *===========================================================================
 *                    ipipsec_aesctr_setkey
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_s32
ipipsec_aesctr_setkey(Ip_u8 *secret, Ip_u32 length, Ipipsec_encrypt_key *key)
{
    length -= 4;   /* remove nonce 32-bits */

    if (length != 16 && length != 24 && length != 32)
        return -1;

    /* Clear aesctr structure (important for ecount_buf and num) */
    ipcom_memset(&key->aesctr, 0, sizeof(key->aesctr));

    /* Save nonce */
    ipcom_memcpy(key->aesctr.nonce, secret + length, 4);

    /* Store key */
    if (AES_set_encrypt_key(secret, length * 8, &key->aesctr.key) != 0)
        return -1;

    return 0;
}
#endif /* IPIPSEC_USE_AESCTR */


/*
 ****************************************************************************
 * 10                    PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipipsec_auth_setkey
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_s32
ipipsec_auth_setkey(Ipipsec_sa *sa, Ip_u8 aalg, Ip_u8 *key, Ip_u32 bytes)
{
    Ip_u32   i;

    /* Check if the authentication algorithm is supported/implemented. */
    if (aalg > SADB_AALG_MAX)
        return -IP_ERRNO_EINVAL;
    if (ipipsec_auth_method[aalg].init == IP_NULL)
        return -IP_ERRNO_EINVAL;

    /* Check the authentication key length. */
    if (bytes < ipipsec_auth_method[aalg].minkey_bytes ||
        bytes > ipipsec_auth_method[aalg].maxkey_bytes ||
        (bytes & 0x3L))   /* key-length must be multiple of 32 bits. */
        return -IP_ERRNO_EINVAL;

    ip_assert(bytes <= IPIPSEC_AUTH_MAXKEYLEN);

    /* Get memory to store the key. */
    sa->auth = (Ipipsec_sa_auth *)ipcom_malloc(sizeof(Ipipsec_sa_auth));
    if (sa->auth == IP_NULL)
        return -IP_ERRNO_ENOMEM;
    ipcom_memset(sa->auth, 0, sizeof(Ipipsec_sa_auth));

    /* Save key length but not the raw key. */
    sa->auth->keyLen = bytes;
    sa->auth->alg = &ipipsec_auth_method[aalg];

    /*** OLD AH ***/
    if (aalg == SADB_X_AALG_MD5 || aalg == SADB_X_AALG_SHA1)
    {
        Ip_u8   bits[8];
        Ip_u8   first[1] = { 0x80 };
        Ip_u32  indx, padLen;

        IPCOM_LOG0(DEBUG, "OLD AH - Keyed MD5/SHA");

        sa->auth->key = ipcom_malloc(sa->auth->keyLen);
        if (sa->auth->key == IP_NULL)
            return -IP_ERRNO_ENOMEM;
        ipcom_memcpy(sa->auth->key, key, bytes);  /* need to save key. */

        /*** keyed SHA/MD5: key,keyfill [datagram,key,MD5fill] ***/

        /* Key */
        if (!ipipsec_auth_method[aalg].init(&sa->auth->i_ctx))
            return -IP_ERRNO_EINVAL;
        ipipsec_auth_method[aalg].update(&sa->auth->i_ctx, sa->auth->key, bytes);

        /* Keyfill padding - pad out to 56 mod 64. */
        indx = (Ip_u32)(bytes & 0x3f);
        padLen = (indx < 56) ? (56 - indx) : (120 - indx);
        ip_assert(padLen > 0);
        ipipsec_auth_method[aalg].update(&sa->auth->i_ctx, first, 1);
        ipipsec_auth_method[aalg].update(&sa->auth->i_ctx, ipipsec.zeroes, padLen - 1);

        /* Keyfill length (the last 64-bit block). */
        ipcom_memset(bits, 0, sizeof(bits));
        bytes <<= 3;  /* now bits. */
        ip_assert(bytes < 256);
        if (aalg == SADB_X_AALG_SHA1)
            bits[7] = (Ip_u8)(bytes & 0xff);
        else
            bits[0] = (Ip_u8)(bytes & 0xff);
        ipipsec_auth_method[aalg].update(&sa->auth->i_ctx, bits, 8);
    }

#ifdef IPIPSEC_USE_AESMAC
    /*** AES MAC ***/
    else  if (aalg == SADB_X_AALG_AESXCBCMAC96)
    {
        ipcom_memcpy(sa->auth->i_ctx.aes.key, key, sa->auth->keyLen);
        if (!ipipsec_auth_method[aalg].init(&sa->auth->i_ctx))
            return -IP_ERRNO_EINVAL;
    }
#endif /* IPIPSEC_USE_AESMAC */

    /*** HMAC ***/
    else
    {
        Ip_u8    keybuf[IPIPSEC_AUTH_MAXKEYLEN];

        ipcom_memcpy(keybuf, key, bytes);   /* No need to save key for HMAC */

        /* HMAC - precompute the I pad. */
        for (i = 0; i < bytes; i++)
            keybuf[i] ^= HMAC_IPAD;
        if (!ipipsec_auth_method[aalg].init(&sa->auth->i_ctx))
            return -IP_ERRNO_EINVAL;
        ipipsec_auth_method[aalg].update(&sa->auth->i_ctx, keybuf, bytes);
        ipipsec_auth_method[aalg].update(&sa->auth->i_ctx, ipipsec.i_buffer, 64 - bytes);

        /* HMAC - precompute the O pad. */
        for (i = 0; i < bytes; i++)
            keybuf[i] ^= (HMAC_IPAD ^ HMAC_OPAD);
        if (!ipipsec_auth_method[aalg].init(&sa->auth->o_ctx))
            return -IP_ERRNO_EINVAL;
        ipipsec_auth_method[aalg].update(&sa->auth->o_ctx, keybuf, bytes);
        ipipsec_auth_method[aalg].update(&sa->auth->o_ctx, ipipsec.o_buffer, 64 - bytes);

        ipcom_memset(keybuf, 0, sizeof(keybuf));   /* paranoia */
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipipsec_encrypt_setkey
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPIPSEC_USE_ESP
IP_GLOBAL Ip_s32
ipipsec_encrypt_setkey(Ipipsec_sa *sa, Ip_u8 ealg, Ip_u8 *key, Ip_u32 bytes)
{
    /* Check if the encryption algorithm is supported/implemented. */
    if (ealg >= MAX_ENC_ALG)
        return -IP_ERRNO_EINVAL;
    if (!ipipsec_encrypt_method[ealg].encrypt ||
        !ipipsec_encrypt_method[ealg].decrypt ||
        !ipipsec_encrypt_method[ealg].setkey)
    {
        return -IP_ERRNO_EINVAL;
    }

    /* Check the encryption key length. */
    if (bytes < ipipsec_encrypt_method[ealg].minkey_bytes ||
        bytes > ipipsec_encrypt_method[ealg].maxkey_bytes)
        return -IP_ERRNO_EINVAL;

    /* Get memory to store the key. */
    sa->encrypt = (Ipipsec_sa_encrypt *)ipcom_malloc(ipipsec_encrypt_method[ealg].key_size);
    if (sa->encrypt == IP_NULL)
        return -IP_ERRNO_ENOMEM;
    ipcom_memset(sa->encrypt, 0, ipipsec_encrypt_method[ealg].key_size);

    /* IV */
    ip_assert(ipipsec_encrypt_method[ealg].iv_bytes <= IPIPSEC_MAX_IV_SIZE);
    sa->encrypt->iv_length = ipipsec_encrypt_method[ealg].iv_bytes;
    if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_HALFIV))
    {
        sa->encrypt->iv_length = (Ip_u16)(sa->encrypt->iv_length >> 1);
        sa->header_space += (ipipsec_encrypt_method[ealg].iv_bytes / 2);
    }
    else
        sa->header_space += ipipsec_encrypt_method[ealg].iv_bytes;

    /* Initialize IV with random bytes. */
    ipipsec_crypt_random_fill(sa->encrypt->iv, sa->encrypt->iv_length);

    /* Save encryption method pointer. */
    sa->encrypt->alg = &ipipsec_encrypt_method[ealg];

    /* ESP trailer space. */
    sa->trailer_space += sa->encrypt->alg->block_bytes;
    if (sa->encrypt->alg->block_bytes >= 2)
    {
        /*  Reduce trailer space since length+protocol fits within the block size. */
        sa->trailer_space -= 2;
    }

    /* Store encryption key SA. */
    sa->encrypt->bytes = bytes;
    if (ipipsec_encrypt_method[ealg].setkey != IP_NULL)
    {
        if (ipipsec_encrypt_method[ealg].setkey(key, bytes, &sa->encrypt->key) != 0)
        {
            IPCOM_LOG1(ERR, "ipipsec_encrypt_setkey() :: failed to set encryption key for EALG=%d", ealg);
            ipcom_memset(sa->encrypt, 0, ipipsec_encrypt_method[ealg].key_size);   /* clear enc key. */
            ipcom_free(sa->encrypt);
            sa->encrypt = IP_NULL;
            return -IP_ERRNO_EINVAL;
        }
    }

    return 0;
}
#endif /* IPIPSEC_USE_ESP */


/*
 *===========================================================================
 *                     ipipsec_crypt_get_algs
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipipsec_crypt_get_algs(struct sadb_supported *supported)
{
    struct sadb_alg  *alg;
    Ip_s32  i, tot;

    tot = 0;
    alg = (struct sadb_alg *)((Ip_u8 *)supported + sizeof(struct sadb_supported));

    /* Fill in encryption algorithms */
#ifdef IPIPSEC_USE_ESP
    for (i = 1; i < MAX_ENC_ALG; i++)
        if (ipipsec_encrypt_method[i].encrypt)
        {
            alg->sadb_alg_id = (uint8_t)i;
            alg->sadb_alg_ivlen = (uint8_t)(ipipsec_encrypt_method[i].iv_bytes << 3);
            alg->sadb_alg_minbits = (uint16_t)(ipipsec_encrypt_method[i].minkey_bytes << 3);
            alg->sadb_alg_maxbits  = (uint16_t)(ipipsec_encrypt_method[i].maxkey_bytes << 3);
            alg->sadb_alg_reserved = 0;
            tot++;
            alg++;
        }
    supported->sadb_supported_nencrypt = (uint8_t)tot;
#endif

    /* Fill in authentication algorithms */
    for (i = 1; i < (SADB_AALG_MAX + 1); i++)
        if (ipipsec_auth_method[i].init)
        {
#if defined(OPENSSL_FIPS) && defined(IPIPSEC_USE_MD5)
            /* MD5 not possible/allowed to use when the system is in FIPS 140-2 mode */
            if (ipipsec_auth_method[i].init == (Hash_init)MD5_Init && FIPS_mode())
                continue;
#endif
            alg->sadb_alg_id = (uint8_t)i;
            alg->sadb_alg_ivlen = 0;
            alg->sadb_alg_minbits = (uint16_t)(ipipsec_auth_method[i].minkey_bytes << 3);
            alg->sadb_alg_maxbits  = (uint16_t)(ipipsec_auth_method[i].maxkey_bytes << 3);
            alg->sadb_alg_reserved = 0;
            tot++;
            alg++;
        }
    supported->sadb_supported_nauth = (uint8_t)tot;

    supported->sadb_supported_len = (uint16_t)(1 + tot);
    supported->sadb_supported_exttype = SADB_EXT_SUPPORTED;
    supported->sadb_supported_reserved = 0;
}


#ifdef IPIPSEC_USE_AESMAC
/*
 *===========================================================================
 *                     AESMAC_Init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
AESMAC_Init(Ipipsec_auth_ctx *ctx)
{
    AES_KEY aes[2];
    Ip_u8 iv[AES_BLOCK_SIZE];

    ipcom_memset(ctx->aes.e, 0, sizeof(ctx->aes.e));
    ipcom_memset(ctx->aes.msg, 0, sizeof(ctx->aes.msg));
    ctx->aes.msgLen = 0;
    ipcom_memset(ctx->aes.k1, 0x01, sizeof(ctx->aes.k1));
    ipcom_memset(ctx->aes.k2, 0x02, sizeof(ctx->aes.k2));
    ipcom_memset(ctx->aes.k3, 0x03, sizeof(ctx->aes.k3));
    (void)ipipsec_aes_setkey(ctx->aes.key, sizeof(ctx->aes.key), (Ipipsec_encrypt_key *)&aes[0]);
    ipcom_memset(iv, 0, sizeof(iv));
    (void)ipipsec_aes_encrypt(ctx->aes.k1, sizeof(ctx->aes.k1), (Ipipsec_encrypt_key *)&aes[0], iv);
    ipcom_memset(iv, 0, sizeof(iv));
    (void)ipipsec_aes_encrypt(ctx->aes.k2, sizeof(ctx->aes.k2), (Ipipsec_encrypt_key *)&aes[0], iv);
    ipcom_memset(iv, 0, sizeof(iv));
    (void)ipipsec_aes_encrypt(ctx->aes.k3, sizeof(ctx->aes.k3), (Ipipsec_encrypt_key *)&aes[0], iv);
    return 1;
}


/*
 *===========================================================================
 *                     AESMAC_Update
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
AESMAC_Update(Ipipsec_auth_ctx *ctx, IP_CONST Ip_u8 *data, Ip_u32 length)
{
    Ip_u8 *e;
    Ip_u8 iv[AES_BLOCK_SIZE];
    int i;
    AES_KEY aes[2];

    /* This algoritm is specified in RFC3566 */
    e = ctx->aes.e;
    (void)ipipsec_aes_setkey(ctx->aes.k1, sizeof(ctx->aes.k1), (Ipipsec_encrypt_key *)&aes[0]);

    /* Append data to context data buffer, digest data when buffer is full */
    while ((ctx->aes.msgLen + length) > sizeof(ctx->aes.msg))
    {
        /* append so we have full buffer */
        i = sizeof(ctx->aes.msg) - ctx->aes.msgLen;
        ipcom_memcpy(&ctx->aes.msg[ctx->aes.msgLen], data, i);
        /* jump handled data */
        data += i;
        length -= i;
        ctx->aes.msgLen = 0;
        /* Encrypt e buffer with k1 */
        for (i = 0; i<(int)sizeof(ctx->aes.msg); i++)
            e[i] = e[i] ^ (ctx->aes.msg[i]);
        ipcom_memset(iv, 0, sizeof(iv));
        ipipsec_aes_encrypt(e, sizeof(ctx->aes.msg), (Ipipsec_encrypt_key *)&aes[0], iv);
    }

    /* Append unhandled data to context data */
    ipcom_memcpy(&ctx->aes.msg[ctx->aes.msgLen], data, length);
    ctx->aes.msgLen += length;
}


/*
 *===========================================================================
 *                     AESMAC_Final
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
AESMAC_Final(Ip_u8 *digest, Ipipsec_auth_ctx *ctx)
{
    int     i;
    Ip_u8   iv[AES_BLOCK_SIZE];
    AES_KEY aes[2];

    ipcom_memset(iv, 0, sizeof(iv));
    (void)ipipsec_aes_setkey(ctx->aes.k1, sizeof(ctx->aes.k1), (Ipipsec_encrypt_key *)&aes[0]);

    if (sizeof(ctx->aes.msg) > ctx->aes.msgLen)
    {
        ctx->aes.msg[ctx->aes.msgLen] = 0x80;
        i = sizeof(ctx->aes.msg) - ctx->aes.msgLen - 1;
        if (i > 0)
            ipcom_memset(&ctx->aes.msg[ctx->aes.msgLen+1], 0, i);
        for (i = 0; i < (int)sizeof(ctx->aes.e); i++)
            ctx->aes.e[i] = ctx->aes.e[i] ^ ctx->aes.msg[i] ^ ctx->aes.k3[i];
        ipipsec_aes_encrypt(ctx->aes.e, sizeof(ctx->aes.msg), (Ipipsec_encrypt_key *)&aes[0], iv);
    }
    else
    {
        for (i = 0; i < (int)sizeof(ctx->aes.e); i++)
            ctx->aes.e[i] = ctx->aes.e[i] ^ ctx->aes.msg[i] ^ ctx->aes.k2[i];
        ipipsec_aes_encrypt(ctx->aes.e, sizeof(ctx->aes.msg), (Ipipsec_encrypt_key *)&aes[0], iv);
    }
    ipcom_memcpy(digest, ctx->aes.e, sizeof(ctx->aes.e));
}
#endif /* IPIPSEC_USE_AESMAC */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
