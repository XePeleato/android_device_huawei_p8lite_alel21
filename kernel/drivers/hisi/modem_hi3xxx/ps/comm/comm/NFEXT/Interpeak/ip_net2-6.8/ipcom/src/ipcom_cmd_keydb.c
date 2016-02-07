/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_cmd_keydb.c,v $ $Revision: 1.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_cmd_keydb.c,v $
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
#include <ipcom_err.h>
#include <ipcom_sock.h>
#include <ipcom_shell.h>


#include "ipcom_key_db.h"

#ifdef IPCRYPTO
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */
#define IPCOM_CMD_KEYDB_LIST   1
#define IPCOM_CMD_KEYDB_IMPORT 2
#define IPCOM_CMD_KEYDB_DELETE 3
#define IPCOM_CMD_KEYDB_PRINT  4
#define IPCOM_CMD_KEYDB_FIND   5
#define IPCOM_CMD_KEYDB_VERIFY 6
#define IPCOM_CMD_KEYDB_HELP   7

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

struct Ipcom_cmd_keydb_opt_st
{
    const char* opt_name;
    int id;
};
typedef struct Ipcom_cmd_keydb_opt_st Ipcom_cmd_keydb_opt;

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

IP_GLOBAL int ipcom_cmd_keydb(int argc, char** argv);
IP_STATIC int ipcom_cmd_keydb_delete(const char* type, const char* key_id);

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */
IP_STATIC Ipcom_cmd_keydb_opt keydb_opt_map[] =
    {
        { "list", IPCOM_CMD_KEYDB_LIST },
        { "import", IPCOM_CMD_KEYDB_IMPORT },
        { "delete", IPCOM_CMD_KEYDB_DELETE },
        { "print", IPCOM_CMD_KEYDB_PRINT },
#ifdef IPCRYPTO
        { "find", IPCOM_CMD_KEYDB_FIND },
#endif
#ifdef IPCOM_USE_KEY_DB_CMD_VERIFY
        { "verify", IPCOM_CMD_KEYDB_VERIFY },
#endif
        { "help", IPCOM_CMD_KEYDB_HELP },
        { "-h", IPCOM_CMD_KEYDB_HELP },
        { IP_NULL, 0 }
    };

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_cmd_keydb_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_cmd_keydb_usage(void)
{
    ipcom_printf("keydb [options]"IP_LF
                 "   list <key|buf>"IP_LF
                 "   import key key_id <key_file>"IP_LF
                 "   import buf key_id buf"IP_LF
                 "   delete <key|buf> <id>"IP_LF
                 "   print <key_id>"IP_LF
                 "   find <cert_file>"IP_LF
#ifdef IPCOM_USE_KEY_DB_CMD_VERIFY
                 "   verify <buf_id>"IP_LF
#endif
                 "   help|-h"IP_LF);
}


/*
 *===========================================================================
 *                    ipcom_cmd_keydb_import
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_cmd_keydb_import(const char* type, const char* key_id, const char* extra)
{
    int rc;

    if (!type || !key_id)
        return IPCOM_ERR_INVALID_ARG;

    if (ipcom_strcmp(type, "key") == 0)
    {
#ifdef IPCRYPTO
        EVP_PKEY* pkey;
        IP_FILE* fp;
        const char* file = extra;

        if (!file)
        {
            ipcom_printf("File argument missing"IP_LF);
            return IPCOM_ERR_INVALID_ARG;
        }

        fp = ipcom_fopen(file, "r");
        if (!fp)
        {
            ipcom_printf("Failed to open file: %s", file);
            return IPCOM_ERR_INVALID_ARG;
        }
        pkey = PEM_read_PrivateKey(fp, IP_NULL, IP_NULL, IP_NULL);
        if (!pkey)
        {
            ipcom_printf("Failed to read key from file: %s", file);
            return IPCOM_ERR_FAILED;
        }
        rc = ipcom_key_db_pkey_import(key_id, pkey);
        if (rc != IPCOM_SUCCESS)
            ipcom_printf("Failed: %s"IP_LF, ipcom_err_string(rc));
        EVP_PKEY_free(pkey);
        return rc;
#else
        ipcom_printf("Compiled out. Requires IPCRYPTO"IP_LF);
        return IPCOM_ERR_NOT_SUPPORTED;
#endif /* IPCRYPTO */
    }

    if (ipcom_strcmp(type, "buf") == 0)
    {
        Ipcom_getline* gl = IP_NULL;
        char prompted_key[2][IPCOM_KEY_DB_MAX_BUF+4];
        char* prompt[2] = {"Enter key:", "Re-type key:"};
        int i;

        gl = ipcom_getline_init(2);
        if(!gl)
        {
            ipcom_printf("Failed."IP_LF);
            return IPCOM_ERR_FAILED;
        }
        for (i=0; i<2; i++)
        {
            ipcom_printf(prompt[i]);
            if(ipcom_getline(IP_INVALID_SOCKET, prompted_key[i], sizeof(prompted_key[i]), IP_FALSE,
                             gl, ipcom_strlen(prompt[i])))
            {
                ipcom_printf("Failed"IP_LF);
                ipcom_getline_exit(gl);
                return IPCOM_ERR_FAILED;
            }
            ipcom_printf(IP_LF);
        }
        ipcom_getline_exit(gl);

        if (ipcom_strcmp(prompted_key[0], prompted_key[1]) != 0)
        {
            ipcom_printf("Failed. Entered keys do not match"IP_LF);
            return IPCOM_ERR_FAILED;
        }
        rc = ipcom_key_db_buf_import(key_id, (Ip_u8*)prompted_key[0], ipcom_strlen(prompted_key[0]));
        if (rc != IPCOM_SUCCESS)
            ipcom_printf("Failed: %s"IP_LF, ipcom_err_string(rc));
        return rc;
    }

    ipcom_printf("Unknown import type: %s", type);
    return IPCOM_ERR_INVALID_ARG;
}


/*
 *===========================================================================
 *                    ipcom_cmd_keydb_list
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_cmd_keydb_list(const char* type)
{
    void* handle;
    char name[64];

    if (ipcom_strcmp(type, "key") == 0)
    {
#ifdef IPCRYPTO
        handle = ipcom_key_db_pkey_opendb();
        if (!handle)
        {
            ipcom_printf("Failed to list key ids"IP_LF);
            return IPCOM_ERR_FAILED;
        }
        while (1)
        {
            if (ipcom_key_db_pkey_readdb(handle, name, (int)sizeof(name)) == IPCOM_SUCCESS)
                ipcom_printf("%s"IP_LF, name);
            else
            {
                ipcom_key_db_pkey_closedb(handle);
                return IPCOM_SUCCESS;
            }
        }
#else
        ipcom_printf("Compiled out. Requires IPCRYPTO"IP_LF);
        return IPCOM_ERR_NOT_SUPPORTED;
#endif
    }

    if (ipcom_strcmp(type, "buf") == 0)
    {
        handle = ipcom_key_db_buf_opendb();
        if (!handle)
        {
            ipcom_printf("Failed to list buf ids"IP_LF);
            return IPCOM_ERR_FAILED;
        }
        while (1)
        {
            if (ipcom_key_db_buf_readdb(handle, name, (int)sizeof(name)) == IPCOM_SUCCESS)
                ipcom_printf("%s"IP_LF, name);
            else
            {
                ipcom_key_db_buf_closedb(handle);
                return IPCOM_SUCCESS;
            }
        }
    }

    ipcom_printf("Unknown list type: %s", type);
    return IPCOM_ERR_INVALID_ARG;

}


/*
 *===========================================================================
 *                    ipcom_cmd_keydb_delete
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_cmd_keydb_delete(const char* type, const char* key_id)
{
    if (ipcom_strcmp(type, "key") == 0)
    {
#ifdef IPCRYPTO
        if (ipcom_key_db_pkey_del(key_id))
        {
            ipcom_printf("Failed"IP_LF);
            return IPCOM_ERR_FAILED;
        }
        return IPCOM_SUCCESS;
#else
        ipcom_printf("Compiled out. Requires IPCRYPTO"IP_LF);
        return IPCOM_ERR_NOT_SUPPORTED;
#endif
    }

    if (ipcom_strcmp(type, "buf") == 0)
    {
        if (ipcom_key_db_buf_del(key_id) != IPCOM_SUCCESS)
        {
            ipcom_printf("Failed"IP_LF);
            return IPCOM_ERR_FAILED;
        }
        return IPCOM_SUCCESS;
    }

    ipcom_printf("Unknown type: %s", type);
    return IPCOM_ERR_INVALID_ARG;
}


#ifdef IPCRYPTO
/*
 *===========================================================================
 *                    ipcom_cmd_keydb_find
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_cmd_keydb_find(const char* cert_file)
{
    X509* x509 = IP_NULL;
    EVP_PKEY* pkey = IP_NULL;
    IP_FILE* fp;
    int rc = IPCOM_ERR_FAILED;
    char key_id[64];
    int key_id_len = (int)sizeof(key_id);

    fp = ipcom_fopen(cert_file, "r");
    if (!fp)
    {
        ipcom_printf("Failed to open certificate file: %s"IP_LF, cert_file);
        return IPCOM_ERR_INVALID_ARG;
    }

    x509 = PEM_read_X509(fp, IP_NULL, IP_NULL, IP_NULL);
    if (!x509)
    {
        ipcom_printf("Failed to open certificate file: %s"IP_LF, cert_file);
        goto cleanup;
    }

    pkey = ipcom_key_db_pkey_get2(x509, key_id, &key_id_len);
    if (pkey)
    {
        ipcom_printf("Key matching certificate found. key_id: %s"IP_LF, key_id);
        rc = IPCOM_SUCCESS;
    }
    else
    {
        ipcom_printf("No matching key found."IP_LF);
        rc = IPCOM_ERR_NOT_FOUND;
    }

 cleanup:
    if (fp)
        ipcom_fclose(fp);
    if (x509)
        X509_free(x509);
    if (pkey)
        EVP_PKEY_free(pkey);

    return rc;
}


/*
 *===========================================================================
 *                    ipcom_cmd_keydb_print
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_cmd_keydb_print(const char* key_id)
{
    EVP_PKEY *pkey;

    pkey = ipcom_key_db_pkey_get(key_id);
    if (!pkey)
    {
        ipcom_printf("Unknown key_id: %s"IP_LF, key_id);
        return IPCOM_ERR_INVALID_ARG;
    }

#ifndef OPENSSL_NO_RSA
    if (pkey->type == EVP_PKEY_RSA)
    {
        ipcom_printf("RSA Public Key: (%d bit)\n",
                     BN_num_bits(pkey->pkey.rsa->n));
        /* We do not want to enable output of sensitive fields, setting them
           to zero will prevent RSA_print_fd from printing them */
        BN_clear_free(pkey->pkey.rsa->d);
        pkey->pkey.rsa->d = IP_NULL;
        BN_clear_free(pkey->pkey.rsa->p);
        pkey->pkey.rsa->p = IP_NULL;
        BN_clear_free(pkey->pkey.rsa->q);
        pkey->pkey.rsa->q = IP_NULL;
        BN_clear_free(pkey->pkey.rsa->dmp1);
        pkey->pkey.rsa->dmp1 = IP_NULL;
        BN_clear_free(pkey->pkey.rsa->dmq1);
        pkey->pkey.rsa->dmq1 = IP_NULL;
        BN_clear_free(pkey->pkey.rsa->iqmp);
        pkey->pkey.rsa->iqmp = IP_NULL;
        RSA_print_fp(ip_stdout,pkey->pkey.rsa,0);
    }
#endif

#ifndef OPENSSL_NO_DSA
    if (pkey->type == EVP_PKEY_DSA)
    {
        ipcom_printf("DSA Public Key: (%d bit)\n",
                     BN_num_bits(pkey->pkey.dsa->p));
        /* We do not want to enable output of sensitive fields, setting them
           to zero will prevent DSA_print_fd from printing them */
        BN_clear_free(pkey->pkey.dsa->priv_key);
        pkey->pkey.dsa->priv_key = IP_NULL;
        DSA_print_fp(ip_stdout,pkey->pkey.dsa,0);
    }
#endif

    EVP_PKEY_free(pkey);

    return 0;
}
#endif /* IPCRYPTO */


#ifdef IPCOM_USE_KEY_DB_CMD_VERIFY
/*
 *===========================================================================
 *                    ipcom_cmd_keydb_verify
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_cmd_keydb_verify(const char* key_id)
{
    char prompted_key[IPCOM_KEY_DB_MAX_BUF+1];
    char stored_key[IPCOM_KEY_DB_MAX_BUF+1];
    int key_len = sizeof(stored_key);
    Ipcom_getline* gl = IP_NULL;
    Ip_err rc = IPCOM_ERR_FAILED;

    gl = ipcom_getline_init(2);
    if(!gl)
        return IPCOM_ERR_FAILED;

    ipcom_printf("Enter key:");
    if(ipcom_getline(IP_INVALID_SOCKET, prompted_key, sizeof(prompted_key), IP_FALSE,
                     gl, ipcom_strlen("Enter key:")))
    {
        ipcom_printf("Failed"IP_LF);
        goto cleanup;
    }
    ipcom_printf(IP_LF);
    if (ipcom_key_db_buf_get(key_id, (Ip_u8*)stored_key, &key_len) != IPCOM_SUCCESS)
    {
        ipcom_printf("Failed to read key from db"IP_LF);
        goto cleanup;
    }

    if ((int)ipcom_strlen(prompted_key) == key_len &&
        ipcom_memcmp(stored_key, prompted_key, key_len) == 0)
    {
        ipcom_printf("Keys match"IP_LF);
    }
    else
        ipcom_printf("Keys do not match"IP_LF);

    rc = IPCOM_SUCCESS;

 cleanup:
    if(gl)
        ipcom_getline_exit(gl);

    return rc;
}
#endif /* IPCOM_USE_KEY_DB_CMD_VERIFY */

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_cmd_keydb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipcom_cmd_keydb(int argc, char** argv)
{
    int cmd_id;
    int matches = 0;
    Ipcom_cmd_keydb_opt* cmd_opt = keydb_opt_map;

    if (argc < 2)
    {
        ipcom_cmd_keydb_usage();
        return IPCOM_ERR_INVALID_ARG;
    }
    for (;cmd_opt->opt_name; cmd_opt++)
    {
        if (ipcom_strncmp(argv[1], cmd_opt->opt_name, ipcom_strlen(argv[1])) == 0)
        {
            cmd_id = cmd_opt->id;
            matches++;
        }
    }
    if (matches == 0)
    {
        ipcom_printf("Unknown option: %s"IP_LF, argv[1]);
        ipcom_printf("%s -h gives usage info"IP_LF, argv[0]);
        return IPCOM_ERR_INVALID_ARG;
    }
    if (matches > 1)
    {
        ipcom_printf("Ambiguous option : %s"IP_LF, argv[1]);
        ipcom_printf("%s -h gives usage info"IP_LF, argv[0]);
        return IPCOM_ERR_INVALID_ARG;
    }

    switch (cmd_id)
    {
    case IPCOM_CMD_KEYDB_LIST:
        if (argc != 3)
        {
            ipcom_cmd_keydb_usage();
            return IPCOM_ERR_INVALID_ARG;
        }
        return ipcom_cmd_keydb_list(argv[2]);
        break;

    case IPCOM_CMD_KEYDB_IMPORT:
        if (argc < 4 || argc > 5)
        {
            ipcom_cmd_keydb_usage();
            return IPCOM_ERR_INVALID_ARG;
        }
        return ipcom_cmd_keydb_import(argv[2], argv[3], argv[4]);
        break;

    case IPCOM_CMD_KEYDB_DELETE:
        if (argc != 4)
        {
            ipcom_cmd_keydb_usage();
            return IPCOM_ERR_INVALID_ARG;
        }
        return ipcom_cmd_keydb_delete(argv[2], argv[3]);
        break;

    case IPCOM_CMD_KEYDB_PRINT:
        if (argc != 3)
        {
            ipcom_cmd_keydb_usage();
            return IPCOM_ERR_INVALID_ARG;
        }
#ifdef IPCRYPTO
        return ipcom_cmd_keydb_print(argv[2]);
#else
        ipcom_printf("Compiled out. Requires IPCRYPTO"IP_LF);
        return IPCOM_ERR_NOT_SUPPORTED;
#endif
        break;

    case IPCOM_CMD_KEYDB_FIND:
        if (argc != 3)
        {
            ipcom_cmd_keydb_usage();
            return IPCOM_ERR_INVALID_ARG;
        }
#ifdef IPCRYPTO
        return ipcom_cmd_keydb_find(argv[2]);
#else
        ipcom_printf("Compiled out. Requires IPCRYPTO"IP_LF);
        return IPCOM_ERR_NOT_SUPPORTED;
#endif
        break;

    case IPCOM_CMD_KEYDB_VERIFY:
        if (argc != 3)
        {
            ipcom_cmd_keydb_usage();
            return IPCOM_ERR_INVALID_ARG;
        }
        return ipcom_cmd_keydb_verify(argv[2]);
        break;

    case IPCOM_CMD_KEYDB_HELP:
        ipcom_cmd_keydb_usage();
        return IPCOM_SUCCESS;

        break;

    default:
        return IPCOM_ERR_FAILED;
    }

    return 0;
}


#else
int ipcom_cmd_keydb_empty_file;
#endif


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

