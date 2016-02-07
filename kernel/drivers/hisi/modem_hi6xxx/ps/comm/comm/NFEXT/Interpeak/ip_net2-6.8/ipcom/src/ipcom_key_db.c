/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_key_db.c,v $ $Revision: 1.2.12.2.4.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_key_db.c,v $
 *   $Author: ulf $
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

#include "ipcom_config.h"

#ifdef IPCOM_USE_KEY_DB

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */
#define IPCOM_USE_CLIB_PROTO
#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_clib.h>
#include <ipcom_sysvar.h>
#include <ipcom_syslog.h>
#include <ipcom_err.h>
#include <ipcom_file.h>
#include <ipcom_key_db.h>

#ifdef IPCRYPTO
#include <ipcrypto.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#endif /* IPCRYPTO */

#if defined(IP_PORT_OSE5)
#include <openssl/aes.h>
#include <openssl/sha.h>
#elif defined(IPMCRYPTO)
#include <ip/openssl/aes.h>
#include <ip/openssl/sha.h>
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */
#ifndef IPCOM_KEY_DB_MAX_PATH
#define IPCOM_KEY_DB_MAX_PATH 128
#endif

#ifndef IPCOM_KEY_DB_FILE_ROOT
#define IPCOM_KEY_DB_FILE_ROOT IPCOM_FILE_ROOT
#endif

#ifndef IPCOM_KEY_DB_MAX_SECRET
#define IPCOM_KEY_DB_MAX_SECRET 32
#endif

#define IPCOM_KEY_DB_TYPE_PKEY 1
#define IPCOM_KEY_DB_TYPE_BUF 2

#define IPCOM_KEY_DB_MODE_READ  1
#define IPCOM_KEY_DB_MODE_WRITE 2

#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM_KEY_DB

#define MAX_BLOCK_SIZE 16

#ifdef EVP_MAX_KEY_LENGTH
#define IPCOM_KEY_DB_MAX_KEY_LENGTH EVP_MAX_KEY_LENGTH
#else
#define IPCOM_KEY_DB_MAX_KEY_LENGTH 32
#endif

#ifdef  EVP_MAX_IV_LENGTH
#define IPCOM_KEY_DB_MAX_IV_LENGTH EVP_MAX_IV_LENGTH
#else
#define IPCOM_KEY_DB_MAX_IV_LENGTH 16
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

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC Ip_err ipcom_key_db_init(void *unused);
IP_STATIC int ipcom_key_db_get_kek_iv(unsigned char* key,
                                       int key_len,
                                       unsigned char* iv,
                                       int iv_len);

IP_STATIC Ip_err ipcom_key_db_get_dir_name(char* dir, Ip_size_t len, int mode);

IP_GLOBAL int ipcom_key_db_cb(char *buf, int size, int rwflag, void *userdata);
IP_GLOBAL char ipcom_key_db_get_char(int in_chr);

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */
IP_STATIC Ipcom_once_t   ipcom_key_db_once = IPCOM_ONCE_INIT;
IP_STATIC Ipcom_mutex    ipcom_key_db_mutex;

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_key_db_strcatn
 *===========================================================================
 * Description: A variant of strncat. This function takes a length argument
 *              of the dest buffer and ensures that the destination buffer is
 *              not overrun.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_key_db_strcatn(char* dest, Ip_size_t n, const char* src)
{
    if (ipcom_strlen(dest)+ipcom_strlen(src) >= n)
        return IPCOM_ERR_FAILED;

    ipcom_strcat(dest, src);

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_key_db_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_key_db_init(void *unused)
{
    Ip_err retval;

    (void)unused;

    retval = ipcom_mutex_create(&ipcom_key_db_mutex);
    ip_assert(retval == IPCOM_SUCCESS);

    if (retval != IPCOM_SUCCESS)
        return retval;

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_key_db_opendb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void*
ipcom_key_db_opendb(int mode)
{
    IP_DIR* dir;
    char dir_path[IPCOM_KEY_DB_MAX_PATH] = {0};

    if (ipcom_once(&ipcom_key_db_once, ipcom_key_db_init, IP_NULL) != IPCOM_SUCCESS)
        return IP_NULL;

    ipcom_mutex_lock(ipcom_key_db_mutex);

    if (ipcom_key_db_get_dir_name(dir_path,
                                  sizeof(dir_path),
                                  mode) != IPCOM_SUCCESS)
    {
        ipcom_mutex_unlock(ipcom_key_db_mutex);
        return IP_NULL;
    }

    dir = ipcom_opendir(dir_path);

    ipcom_mutex_unlock(ipcom_key_db_mutex);
    return dir;
}


/*
 *===========================================================================
 *                    ipcom_key_db_readdb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_key_db_readdb(void* handle, char* name, int name_len)
{
    IP_DIR* dir;
    struct Ip_dirent* dirent;

    if (ipcom_once(&ipcom_key_db_once, ipcom_key_db_init, IP_NULL) != IPCOM_SUCCESS)
        return IPCOM_ERR_FAILED;

    ipcom_mutex_lock(ipcom_key_db_mutex);

    dir = handle;
    while (1)
    {
        dirent = ipcom_readdir(dir);
        if (!dirent)
        {
            ipcom_mutex_unlock(ipcom_key_db_mutex);
            return IPCOM_ERR_END;
        }

        /* ipcom Ip_dirent struct gives no way to tell if its a real file or
           directory (due to restrictions in VxWorks file API). */
        if (ipcom_strcmp(dirent->d_name, ".") == 0 ||
            ipcom_strcmp(dirent->d_name, "..") == 0)
        {
            continue;
        }

        if ((int)ipcom_strlen(dirent->d_name) > name_len + 1)
        {
            ipcom_mutex_unlock(ipcom_key_db_mutex);
            return IPCOM_ERR_INVALID_ARG;
        }
        break;
    }
    ipcom_strcpy(name, dirent->d_name);

    ipcom_mutex_unlock(ipcom_key_db_mutex);

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_key_db_closedb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_key_db_closedb(void* handle)
{
    IP_DIR* dir;

    dir = handle;

    return ipcom_closedir(dir);
}


/*
 *===========================================================================
 *                    ipcom_key_db_create_dirs
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_key_db_create_dirs(const char* path)
{
    char* tmp_path;
    char* slash_ptr;
    int i;
    int j;

    if (!path)
    {
        IPCOM_LOG0(ERR, "ipcom_key_db_create_dirs :: invalid path");
        return;
    }

    if (!ipcom_strchr(path, '/'))
    {
        /* No slashes in path => path is a relative 'leaf' path */
        ipcom_mkdir(path, 0744);
        return;
    }

    /* Create intermediate directories */
    for (i=1;; i++)
    {
        tmp_path = ipcom_strdup(path);
        if (!tmp_path)
        {
            IPCOM_LOG0(ERR, "ipcom_key_db_create_dirs :: out of memory");
            return;
        }

        for (j=0, slash_ptr = tmp_path; j<i; j++)
        {
            if (*slash_ptr == '/')
                slash_ptr++;
            slash_ptr = ipcom_strchr(slash_ptr, '/');
        }
        if (!slash_ptr)
        {
            ipcom_free(tmp_path);
            break;
        }

        *slash_ptr = 0;
        /* If we have an absolute path it will start with a slash */
        if (ipcom_strlen(tmp_path) > 0 && ipcom_strcmp(tmp_path, ".") != 0)
            ipcom_mkdir(tmp_path, 0744);
        ipcom_free(tmp_path);
    }

    /* Create the final 'leaf' directory */
    if (path[ipcom_strlen(path)-1] != '/')
        ipcom_mkdir(path, 0744);
}


/*
 *===========================================================================
 *                    ipcom_key_db_get_dir_name
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_key_db_get_dir_name(char* dir, Ip_size_t len, int mode)
{

    ip_assert(mode == IPCOM_KEY_DB_TYPE_PKEY || mode == IPCOM_KEY_DB_TYPE_BUF);

    if (ipcom_key_db_strcatn(dir, len, IPCOM_KEY_DB_FILE_ROOT) != IPCOM_SUCCESS)
        return IPCOM_ERR_FAILED;

    if (dir[ipcom_strlen(dir)-1] != '/')
        if (ipcom_key_db_strcatn(dir, len, "/") != IPCOM_SUCCESS)
            return IPCOM_ERR_FAILED;

    switch (mode)
    {
    case IPCOM_KEY_DB_TYPE_PKEY:
        if (ipcom_key_db_strcatn(dir, len, "pkey_db/") != IPCOM_SUCCESS)
            return IPCOM_ERR_FAILED;
        break;
    case IPCOM_KEY_DB_TYPE_BUF:
        if (ipcom_key_db_strcatn(dir, len, "buf_db/") != IPCOM_SUCCESS)
            return IPCOM_ERR_FAILED;
        break;

    default:
        IP_PANIC();
    }

    if (ipcom_strlen(dir) == len-1)
        return IPCOM_ERR_FAILED;

    ipcom_key_db_create_dirs(dir);

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_key_db_get_file_name
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_key_db_get_file_name(const char* key_id, char* file_path, Ip_size_t size, int type)
{
    if (ipcom_key_db_get_dir_name(file_path, size, type) != IPCOM_SUCCESS)
    {
        IPCOM_LOG2(ERR, "ipcom_key_db_get_dir_name failed for path=%s, type=%d",
                   file_path, type);
        return IPCOM_ERR_FAILED;
    }

    ipcom_key_db_strcatn(file_path, size, key_id);

    if (ipcom_strlen(file_path) == (size-1))
    {
        IPCOM_LOG2(ERR, "Failed to create sec key db file name for key_id=%s, type=%d",
                   key_id, type);
        return IPCOM_ERR_FAILED;
    }

    return IPCOM_SUCCESS;
}


#ifdef IPCRYPTO
/*
 *===========================================================================
 *                    ipcom_key_db_get_bio
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC BIO*
ipcom_key_db_get_bio(const char* key_id, int key_type, int mode)
{
    char file_path[IPCOM_KEY_DB_MAX_PATH] = {0};
    Ip_size_t size = sizeof(file_path);
    BIO  *bio = IP_NULL;

    if (ipcom_key_db_get_file_name(key_id, file_path, size, key_type) != IPCOM_SUCCESS)
        return IP_NULL;

    bio = BIO_new(BIO_s_file());
    if (!bio)
    {
        IPCOM_LOG0(ERR, "ipcom_key_db_get_bio :: out of memory");
        goto cleanup;
    }

    switch (mode)
    {
    case IPCOM_KEY_DB_MODE_READ:
        if (!BIO_read_filename(bio, file_path))
        {
            IPCOM_LOG1(DEBUG, "ipcom_key_db_get_bio :: failed to open %s", file_path);
            goto cleanup;
        }
        break;

    case IPCOM_KEY_DB_MODE_WRITE:
        if (!BIO_write_filename(bio, file_path))
        {
            IPCOM_LOG1(ERR, "ipcom_key_db_get_bio :: failed to open %s", file_path);
            goto cleanup;
        }
        break;

    default:
        IP_PANIC();
        goto cleanup;
    }

    ERR_clear_error();
    ERR_remove_state(0);
    return bio;

 cleanup:
    if (bio)
        BIO_free(bio);
    ERR_clear_error();
    ERR_remove_state(0);

    return IP_NULL;
}
#endif


/*
 *===========================================================================
 *                    ipcom_key_db_get_fp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC IP_FILE*
ipcom_key_db_get_fp(const char* key_id, int key_type, int mode)
{
    char file_path[IPCOM_KEY_DB_MAX_PATH] = {0};
    Ip_size_t size = sizeof(file_path);
    char* mode_str;
    if (ipcom_key_db_get_file_name(key_id, file_path, size, key_type) != IPCOM_SUCCESS)
        return IP_NULL;

    switch (mode)
    {
    case IPCOM_KEY_DB_MODE_READ:
        mode_str = "r";
        break;

    case IPCOM_KEY_DB_MODE_WRITE:
        mode_str = "w";
        break;

    default:
        IP_PANIC();
        break;
    }

    return ipcom_fopen(file_path, mode_str);
}


/*
 *===========================================================================
 *                    ipcom_key_db_get_kek_iv
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_key_db_get_kek_iv(unsigned char* key, int key_len, unsigned char* iv, int iv_len)
{
    char pw[IPCOM_KEY_DB_MAX_SECRET];
    unsigned char md_buf[20];
    int addmd=0;
    unsigned int mds=20;
    unsigned int i;
    SHA_CTX ctx;

    if (ipcom_key_db_cb(pw, sizeof(pw), 0, IP_NULL) <= 0)
        return IPCOM_ERR_FAILED;

    for (;;)
    {
        (void)SHA1_Init(&ctx);
        if (addmd++)
            SHA1_Update(&ctx,&(md_buf[0]),mds);
        SHA1_Update(&ctx,pw,ipcom_strlen(pw));
        SHA1_Final(md_buf,&ctx);

        i=0;
        mds=20;
        if (key_len)
        {
            for (;;)
            {
                if (key_len == 0)
                    break;
                if (i == mds)
                    break;
                if (key != IP_NULL)
                    *(key++) = md_buf[i];
                key_len--;
                i++;
            }
        }
        if (iv_len && (i != mds))
        {
            for (;;)
            {
                if (iv_len == 0)
                    break;
                if (i == mds)
                    break;
                if (iv != IP_NULL)
                    *(iv++)=md_buf[i];
                iv_len--;
                i++;
            }
        }
        if ((key_len == 0) && (iv_len == 0))
            break;
    }
    ipcom_memset(md_buf, 0, sizeof(md_buf));

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_key_db_buf_encrypt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_key_db_buf_encrypt(const Ip_u8* in, const int inl, Ip_u8* out, int* outl)
{
    int pad_len;
    AES_KEY aes_key;
    unsigned char key[IPCOM_KEY_DB_MAX_KEY_LENGTH];
    unsigned char iv[IPCOM_KEY_DB_MAX_IV_LENGTH];
    int i;
    unsigned long encr_len;
    Ip_u8* in_ptr;

    ipcom_key_db_get_kek_iv(key, sizeof(key), iv, sizeof(iv));

    in_ptr = ipcom_malloc(inl + AES_BLOCK_SIZE);
    ipcom_memcpy(in_ptr, in, inl);

    pad_len = AES_BLOCK_SIZE - (inl % AES_BLOCK_SIZE);
    if (*outl < inl + pad_len)
    {
        *outl = inl + pad_len;
        return IPCOM_ERR_INVALID_ARG;
    }
    *outl = inl + pad_len;

    for (i=inl; i<inl+pad_len; i++)
        in_ptr[i] = pad_len;
    (void)AES_set_encrypt_key(key, 256, &aes_key);
    encr_len = *outl;

    (void)AES_cbc_encrypt(in_ptr, out, encr_len, &aes_key, iv, 1);

    ipcom_free(in_ptr);

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_key_db_buf_decrypt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_key_db_buf_decrypt(const Ip_u8* in, const int inl, Ip_u8* out, int* outl)
{
    int pad_len;
    AES_KEY aes_key;
    unsigned char key[IPCOM_KEY_DB_MAX_KEY_LENGTH];
    unsigned char iv[IPCOM_KEY_DB_MAX_IV_LENGTH];

    ip_assert((inl % AES_BLOCK_SIZE) == 0);
    ip_assert(*outl >= inl);

    ipcom_key_db_get_kek_iv(key, sizeof(key), iv, sizeof(iv));

    (void)AES_set_decrypt_key(key, 256, &aes_key);

    (void)AES_cbc_encrypt(in, out, inl, &aes_key, iv, 0);

    if (out[inl-1] > MAX_BLOCK_SIZE)
    {
        IPCOM_LOG1(ERR, "ipcom_key_db_buf_decrypt :: invalid padding, stated pad len=%d",
                   out[inl-1]);
        return IPCOM_ERR_FAILED;
    }

    pad_len = out[inl-1];
    *outl = inl - pad_len;

    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_key_db_get_char
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL char
ipcom_key_db_get_char(int in_chr)
{
    char ret;

    ret = (in_chr % 126);
    if (ret < 32)
        ret += 32;

    return ret;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#ifdef IPCRYPTO
/*
 *===========================================================================
 *                    ipcom_key_db_pkey_get
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC EVP_PKEY*
ipcom_key_db_pkey_get(const char* key_id)
{
    BIO  *bio = IP_NULL;
    EVP_PKEY* evp_pkey = IP_NULL;

    if (!key_id || ipcom_strlen(key_id) > IPCOM_KEY_DB_MAX_ID)
        return IP_NULL;

    if (ipcom_once(&ipcom_key_db_once, ipcom_key_db_init, IP_NULL) != IPCOM_SUCCESS)
        return IP_NULL;

    ipcom_mutex_lock(ipcom_key_db_mutex);

    bio = ipcom_key_db_get_bio(key_id, IPCOM_KEY_DB_TYPE_PKEY, IPCOM_KEY_DB_MODE_READ);
    if (!bio)
    {
        IPCOM_LOG1(ERR, "ipcom_key_db_get :: key_id %s does not exist", key_id);
        goto cleanup;
    }
    evp_pkey = PEM_read_bio_PrivateKey(bio, IP_NULL, ipcom_key_db_cb, IP_NULL);
    if (!evp_pkey)
    {
        IPCOM_LOG1(ERR, "ipcom_key_db_get :: failed to load key (%s) from file", key_id);
    }

 cleanup:
    if (bio)
        BIO_free(bio);
    ipcom_mutex_unlock(ipcom_key_db_mutex);

    return evp_pkey;
}


/*
 *===========================================================================
 *                    ipcom_key_db_pkey_import
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_key_db_pkey_import(const char* key_id, const EVP_PKEY* key)
{
    BIO  *bio = IP_NULL;
    Ip_err rc = IPCOM_ERR_FAILED;

    if (!key || !key_id || ipcom_strlen(key_id) > IPCOM_KEY_DB_MAX_ID)
        return IPCOM_ERR_INVALID_ARG;

    rc =  ipcom_once(&ipcom_key_db_once, ipcom_key_db_init, IP_NULL);
    if (rc != IPCOM_SUCCESS)
        return rc;
    rc = IPCOM_ERR_FAILED;

    ipcom_mutex_lock(ipcom_key_db_mutex);

    /* Check if key_id already exists */
    bio = ipcom_key_db_get_bio(key_id, IPCOM_KEY_DB_TYPE_PKEY, IPCOM_KEY_DB_MODE_READ);
    if (bio)
    {
        BIO_free(bio);
        ipcom_mutex_unlock(ipcom_key_db_mutex);
        return IPCOM_ERR_ALREADY_INSTALLED;
    }

    bio = ipcom_key_db_get_bio(key_id, IPCOM_KEY_DB_TYPE_PKEY, IPCOM_KEY_DB_MODE_WRITE);
    if (!bio)
    {
        BIO_free(bio);
        ipcom_mutex_unlock(ipcom_key_db_mutex);
        return IPCOM_ERR_FAILED;
    }

    /* Write key */
    if (!PEM_write_bio_PrivateKey(bio, (EVP_PKEY*)key, EVP_aes_256_cbc(),
                                  IP_NULL, IP_NULL, ipcom_key_db_cb, IP_NULL))
    {
        IPCOM_LOG1(ERR, "ipcom_key_db_pkey_import :: PEM_write_bio_PrivateKey failed for key_id: %s", key_id);
        goto cleanup;
    }

    rc = IPCOM_SUCCESS;

 cleanup:
    if (bio)
        BIO_free(bio);
    ipcom_mutex_unlock(ipcom_key_db_mutex);

    return rc;
}


/*
 *===========================================================================
 *                    ipcom_key_db_pkey_get2
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC EVP_PKEY*
ipcom_key_db_pkey_get2(struct x509_st* x509, char* key_id, int* key_id_len)
{
    EVP_PKEY* pkey = IP_NULL;
    char int_key_id[IPCOM_KEY_DB_MAX_PATH] = {0};
    EVP_PKEY* cert_pub_key = IP_NULL;
    void* handle;

    if (!x509)
        return IP_NULL;

    if (ipcom_once(&ipcom_key_db_once, ipcom_key_db_init, IP_NULL) != IPCOM_SUCCESS)
        return IP_NULL;

    ipcom_mutex_lock(ipcom_key_db_mutex);

    handle = ipcom_key_db_pkey_opendb();
    if (!handle)
        goto cleanup;

    cert_pub_key=X509_get_pubkey(x509);
    if (!cert_pub_key)
    {
        ipcom_mutex_unlock(ipcom_key_db_mutex);
        return IP_NULL;
    }

    /* Iterate over all keys in the DB and see if any of them match the X509 certificate */
    while (1)
    {
        if (ipcom_key_db_pkey_readdb(handle, int_key_id, (int)sizeof(int_key_id)) != IPCOM_SUCCESS)
            goto cleanup;

        pkey = ipcom_key_db_pkey_get(int_key_id);
        if (!pkey)
            goto cleanup;

        if (EVP_PKEY_cmp(pkey, cert_pub_key) == 1)
        {
            if (key_id && key_id_len)
            {
                if ((int)ipcom_strlen(int_key_id) < *key_id_len)
                    ipcom_strcpy(key_id, int_key_id);
                else
                    *key_id = 0;
                *key_id_len = ipcom_strlen(int_key_id);
            }
            break;
        }
        EVP_PKEY_free(pkey);
        pkey = IP_NULL;
    }

 cleanup:
    if (handle)
        ipcom_key_db_pkey_closedb(handle);
    if (cert_pub_key)
        EVP_PKEY_free(cert_pub_key);

    ipcom_mutex_unlock(ipcom_key_db_mutex);

    return pkey;
}


/*
 *===========================================================================
 *                    ipcom_key_db_pkey_opendb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void*
ipcom_key_db_pkey_opendb(void)
{
    return ipcom_key_db_opendb(IPCOM_KEY_DB_TYPE_PKEY);

}


/*
 *===========================================================================
 *                    ipcom_key_db_pkey_readdb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_key_db_pkey_readdb(void* handle, char* name, int name_len)
{
    if (!handle || !name)
        return IPCOM_ERR_INVALID_ARG;

    return ipcom_key_db_readdb(handle, name, name_len);
}


/*
 *===========================================================================
 *                    ipcom_key_db_pkey_closedb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_key_db_pkey_closedb(void* handle)
{
    return ipcom_key_db_closedb(handle);
}
#endif /* IPCRYPTO */


/*
 *===========================================================================
 *                    ipcom_key_db_buf_import
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_key_db_buf_import(const char* key_id, const Ip_u8* key, int key_len)
{
    IP_FILE* fp;
    Ip_err rc;
    unsigned char* encr_key;
    int encr_len;

    if (!key_id || ipcom_strlen(key_id) > IPCOM_KEY_DB_MAX_ID ||
        !key ||
        key_len > IPCOM_KEY_DB_MAX_BUF)
        return IPCOM_ERR_INVALID_ARG;

    rc = ipcom_once(&ipcom_key_db_once, ipcom_key_db_init, IP_NULL);
    if (rc != IPCOM_SUCCESS)
        return rc;
    rc = IPCOM_ERR_FAILED;

    ipcom_mutex_lock(ipcom_key_db_mutex);

    fp = ipcom_key_db_get_fp(key_id, IPCOM_KEY_DB_TYPE_BUF, IPCOM_KEY_DB_MODE_READ);
    if (fp)
    {
        ipcom_fclose(fp);
        ipcom_mutex_unlock(ipcom_key_db_mutex);
        return IPCOM_ERR_ALREADY_INSTALLED;
    }
    fp = ipcom_key_db_get_fp(key_id, IPCOM_KEY_DB_TYPE_BUF, IPCOM_KEY_DB_MODE_WRITE);
    if (!fp)
    {
        ipcom_mutex_unlock(ipcom_key_db_mutex);
        return IPCOM_ERR_FAILED;
    }

    encr_len = key_len+MAX_BLOCK_SIZE;
    encr_key = ipcom_malloc(encr_len);
    if (!encr_key)
    {
        IPCOM_LOG0(ERR, "ipcom_key_db_import_buf :: out of memory");
        goto cleanup;
    }

    if (ipcom_key_db_buf_encrypt(key, key_len, encr_key, &encr_len) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "ipcom_key_db_import_buf :: ipcom_key_db_buf_encrypt failed");
        goto cleanup;
    }

    if (ipcom_fwrite(encr_key, encr_len, 1, fp) != 1)
    {
        IPCOM_LOG1(ERR, "ipcom_key_db_import_buf :: ipcom_fwrite() failed, errno: %d", ipcom_errno);
        goto cleanup;
    }
    rc = IPCOM_SUCCESS;

 cleanup:
    if (fp)
        ipcom_fclose(fp);
    if (encr_key)
        ipcom_free(encr_key);
    ipcom_mutex_unlock(ipcom_key_db_mutex);

    return rc;
}


/*
 *===========================================================================
 *                    ipcom_key_db_pkey_del
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_key_db_pkey_del(const char* key_id)
{
    Ip_err err;
    char path[IPCOM_KEY_DB_MAX_PATH] = {0};
    IP_FILE* fp;

    if (!key_id || ipcom_strlen(key_id) > IPCOM_KEY_DB_MAX_ID)
        return IPCOM_ERR_INVALID_ARG;

    err = ipcom_once(&ipcom_key_db_once, ipcom_key_db_init, IP_NULL);
    if (err != IPCOM_SUCCESS)
        return err;

    ipcom_mutex_lock(ipcom_key_db_mutex);

    fp = ipcom_key_db_get_fp(key_id, IPCOM_KEY_DB_TYPE_PKEY, IPCOM_KEY_DB_MODE_READ);
    if (!fp)
    {
        ipcom_mutex_unlock(ipcom_key_db_mutex);
        return IPCOM_ERR_NOT_FOUND;
    }
    if (ipcom_key_db_get_dir_name(path,
                                  sizeof(path),
                                  IPCOM_KEY_DB_TYPE_PKEY) != IPCOM_SUCCESS)
    {
        ipcom_mutex_unlock(ipcom_key_db_mutex);
        return IPCOM_ERR_FAILED;
    }

    ipcom_key_db_strcatn(path, sizeof(path), key_id);
    if (ipcom_unlink(path))
        err = IPCOM_ERR_FAILED;
    else
        err= IPCOM_SUCCESS;

    ipcom_mutex_unlock(ipcom_key_db_mutex);

    return err;
}


/*
 *===========================================================================
 *                    ipcom_key_db_buf_get
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_key_db_buf_get(const char* key_id, Ip_u8* buf, int* buf_len)
{
    IP_FILE* fp;
    unsigned char* encr_key = IP_NULL;
    unsigned char* clear_text_key = IP_NULL;
    int encr_len;
    int clear_text_len;
    Ip_err rc;

    if (!key_id || ipcom_strlen(key_id) > IPCOM_KEY_DB_MAX_ID ||
        !buf ||
        !buf_len)
    {
        return IPCOM_ERR_INVALID_ARG;
    }

    rc = ipcom_once(&ipcom_key_db_once, ipcom_key_db_init, IP_NULL);
    if (rc != IPCOM_SUCCESS)
        return rc;
    rc = IPCOM_ERR_FAILED;

    ipcom_mutex_lock(ipcom_key_db_mutex);

    fp = ipcom_key_db_get_fp(key_id, IPCOM_KEY_DB_TYPE_BUF, IPCOM_KEY_DB_MODE_READ);
    if (!fp)
    {
        rc = IPCOM_ERR_NOT_FOUND;
        goto cleanup;
    }
    encr_key = ipcom_malloc(IPCOM_KEY_DB_MAX_BUF+MAX_BLOCK_SIZE);
    clear_text_key = ipcom_malloc(IPCOM_KEY_DB_MAX_BUF+MAX_BLOCK_SIZE);
    clear_text_len = IPCOM_KEY_DB_MAX_BUF+MAX_BLOCK_SIZE;
    if (!encr_key || !clear_text_key)
    {
        IPCOM_LOG0(ERR, "ipcom_key_db_get_buf :: out of memory");
        rc = IPCOM_ERR_NO_MEMORY;
        goto cleanup;
    }
    encr_len = ipcom_fread(encr_key, 1, IPCOM_KEY_DB_MAX_BUF+MAX_BLOCK_SIZE, fp);
    if (encr_len <= 0)
    {
        IPCOM_LOG1(ERR, "ipcom_key_db_get_buf :: failed to read key_id %s", key_id);
        goto cleanup;
    }
    if (ipcom_key_db_buf_decrypt(encr_key, encr_len, clear_text_key, &clear_text_len) == IPCOM_SUCCESS)
    {
        *buf_len = clear_text_len;
        if (*buf_len >= clear_text_len)
        {
            ipcom_memcpy(buf, clear_text_key, clear_text_len);
            rc = IPCOM_SUCCESS;
        }
        else
            rc = IPCOM_ERR_INVALID_ARG;
    }
    else
    {
        IPCOM_LOG0(ERR, "ipcom_key_db_get_buf :: ipcom_key_db_buf_decrypt failed");
        goto cleanup;
    }

 cleanup:
    if (fp)
        ipcom_fclose(fp);
    if (encr_key)
        ipcom_free(encr_key);
    if (clear_text_key)
        ipcom_free(clear_text_key);
    ipcom_mutex_unlock(ipcom_key_db_mutex);

    return rc;
}


/*
 *===========================================================================
 *                    ipcom_key_db_buf_del
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_key_db_buf_del(const char* key_id)
{
    Ip_err err;
    char path[IPCOM_KEY_DB_MAX_PATH] = {0};

    if (!key_id || ipcom_strlen(key_id) > IPCOM_KEY_DB_MAX_ID)
    {
        return IPCOM_ERR_INVALID_ARG;
    }

    err = ipcom_once(&ipcom_key_db_once, ipcom_key_db_init, IP_NULL);
    if (err != IPCOM_SUCCESS)
        return err;

    ipcom_mutex_lock(ipcom_key_db_mutex);

    if (ipcom_key_db_get_dir_name(path,
                                  sizeof(path),
                                  IPCOM_KEY_DB_TYPE_BUF) != IPCOM_SUCCESS)
    {
        ipcom_mutex_unlock(ipcom_key_db_mutex);
        return IPCOM_ERR_FAILED;
    }

    if (ipcom_key_db_strcatn(path, sizeof(path), key_id) == IPCOM_SUCCESS)
    {
        if (ipcom_unlink(path))
            err = IPCOM_ERR_FAILED;
        else
            err= IPCOM_SUCCESS;
    }
    else
        err= IPCOM_SUCCESS;

    ipcom_mutex_unlock(ipcom_key_db_mutex);

    return err;
}


/*
 *===========================================================================
 *                    ipcom_key_db_buf_opendb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void*
ipcom_key_db_buf_opendb(void)
{
    return ipcom_key_db_opendb(IPCOM_KEY_DB_TYPE_BUF);
}


/*
 *===========================================================================
 *                    ipcom_key_db_buf_readdb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_key_db_buf_readdb(void* handle, char* name, int name_len)
{
    return ipcom_key_db_readdb(handle, name, name_len);
}


/*
 *===========================================================================
 *                    ipcom_key_db_buf_closedb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_key_db_buf_closedb(void* handle)
{
    return ipcom_key_db_closedb(handle);
}


#else
int ipcom_key_db_empty_file;
#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
