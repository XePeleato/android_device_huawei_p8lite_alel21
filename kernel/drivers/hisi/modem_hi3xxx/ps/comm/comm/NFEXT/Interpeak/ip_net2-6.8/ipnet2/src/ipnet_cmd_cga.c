/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_cmd_cga.c,v $ $Revision: 1.2 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_cmd_cga.c,v $
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
#include <ipcom_getopt.h>
#include <ipcom_err.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_cga.h"

#include <openssl/pem.h>

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

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

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipnet_cmd_cga_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_cga_usage(char* cmd)
{
    ipcom_printf("%s usage:"IP_LF
                 "-k <key file>   File holding the RSA key"IP_LF
                 "-p <subnet>     Subnet prefix for CGA address"IP_LF
                 "-s <sec value>  "IP_LF
                 "-t              Call verify with test vectors"IP_LF
                 "-v              Verify the generated address"IP_LF,
                 cmd);
}


/*
 *===========================================================================
 *                    ipnet_cmd_cga_verify_test_vectors
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_cga_verify_test_vectors(void)
{
    Ip_u8 der_key[] =
        { 0x30,0x5c ,0x30,0x0d ,0x06,0x09 ,0x2a,0x86 ,0x48,0x86 ,0xf7,0x0d,
          0x01,0x01 ,0x01,0x05 ,0x00,0x03 ,0x4b,0x00 ,0x30,0x48 ,0x02,0x41,
          0x00,0xc2 ,0xc2,0xf1 ,0x37,0x30 ,0x54,0x54 ,0xf1,0x0b ,0xd9,0xce,
          0xa3,0x68 ,0x44,0xb5 ,0x30,0xe9 ,0x21,0x1a ,0x4b,0x26 ,0x2b,0x16,
          0x46,0x7c ,0xb7,0xdf ,0xba,0x1f ,0x59,0x5c ,0x01,0x94 ,0xf2,0x75,
          0xbe,0x5a ,0x4d,0x38 ,0x6f,0x2c ,0x3c,0x23 ,0x82,0x50 ,0x87,0x73,
          0xc7,0x86 ,0x7f,0x9b ,0x3b,0x9e ,0x63,0xa0 ,0x9c,0x7b ,0xc4,0x8f,
          0x7a,0x54 ,0xeb,0xef ,0xaf,0x02 ,0x03,0x01 ,0x00,0x01
        };
    Ip_u8 modifier[] = { 0x89,0xa8,0xa8,0xb2,0xe8,0x58,0xd8,0xb8,0xf2,0x63,0x3f,0x44,0xd2,0xd4,0xce,0x9b};
    Ipnet_cga_param cga_param;
    struct Ip_in6_addr cga_addr;
    Ip_u8 addr_data[] = {0xfe,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x4a,0x5b,0xf6,0xff,0xb4,0xca,0x6c};
    int rc;


    ipcom_memset(&cga_param, 0, sizeof(cga_param));
    ipcom_memcpy(cga_param.modifier, modifier, sizeof(modifier));
    ipcom_memcpy(cga_param.subnet_prefix, addr_data, 8);
    ipcom_memcpy(&cga_addr, addr_data, sizeof(addr_data));
    cga_param.der_encoded_key = der_key;
    cga_param.der_key_len = sizeof(der_key);
    cga_param.sec = 1;

    rc = ipnet_cga_verify(&cga_param, &cga_addr);
    if (rc == IPCOM_SUCCESS)
        ipcom_printf("Test vector successfully verified"IP_LF);
    else
        ipcom_printf("Failed to verify test vectors"IP_LF);

    return rc;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

IP_GLOBAL int
ipnet_cmd_cga(int argc, char **argv)
{
    Ipnet_cga_param   *cga_param;
    Ipcom_getopt       opt;
    BIO               *bio;
    char              *key_file = IP_NULL;
    int                ret = IPCOM_ERR_FAILED;
    int                c;
    unsigned int       i;
    struct Ip_in6_addr cga_addr;
    struct Ip_in6_addr subnet_prefix;
    char              *subnet_prefix_str = IP_NULL;
    char               buf[IP_INET6_ADDRSTRLEN];
    Ip_bool            verify = IP_FALSE;
    RSA               *rsa;

    ipcom_getopt_clear_r(&opt);

    cga_param = ipnet_cga_param_new();
    if (!cga_param)
    {
        ipcom_printf("Failed, out of memory"IP_LF);
        return IPCOM_ERR_NO_MEMORY;
    }

    while ((c = ipcom_getopt_r(argc, argv, "k:p:s:tvh", &opt)) != -1)
    {
        switch (c)
        {
        case 'k':
            key_file = opt.optarg;
            break;
        case 's':
            cga_param->sec = ipcom_atoi(opt.optarg);
            if (cga_param->sec < 0 || cga_param->sec > 7)
            {
                ipcom_printf("Illegal sec value, must be between 0-7");
                return IPCOM_ERR_INVALID_ARG;
            }
            break;

        case 'p':
            subnet_prefix_str = opt.optarg;
            break;
        case 'v':
            verify = IP_TRUE;
            break;
        case 't':
            return ipnet_cmd_cga_verify_test_vectors();
            break;

        case 'h':
            ipnet_cmd_cga_usage(argv[0]);
            return IPCOM_SUCCESS;

        default:
            ipnet_cmd_cga_usage(argv[0]);
        }

    }

    /* Sanity checks */
    if (!key_file)
    {
        ipcom_printf("Key file must be specified (-k)"IP_LF);
        ipnet_cmd_cga_usage(argv[0]);
        return IPCOM_ERR_FAILED;
    }
    if (!subnet_prefix_str)
    {
        ipcom_printf("Subnet prefix must be specified (-p)"IP_LF);
        ipnet_cmd_cga_usage(argv[0]);
        return IPCOM_ERR_FAILED;
    }
    if (ipcom_inet_pton(IP_AF_INET6, subnet_prefix_str, &subnet_prefix) != 1)
    {
        ipcom_printf("Invalid subnet"IP_LF);
        return IPCOM_ERR_INVALID_ARG;
    }
    ipcom_memcpy(cga_param->subnet_prefix, &subnet_prefix, sizeof(cga_param->subnet_prefix));

    bio = BIO_new(BIO_s_file());
    if (!bio)
    {
        ipcom_printf("Out of memory");
        return IPCOM_ERR_FAILED;
    }
    if (!BIO_read_filename(bio, key_file))
    {
        ipcom_printf("Non-existent file: %s", key_file);
        goto cleanup;
    }
    rsa = PEM_read_bio_RSAPrivateKey(bio, IP_NULL, IP_NULL, IP_NULL);
    if (!rsa)
    {
        ipcom_printf("Failed to load RSA key from file %s", key_file);
        goto cleanup;
    }
    if (ipnet_cga_param_set_rsa(cga_param, rsa) != IPCOM_SUCCESS)
    {
        ipcom_printf("Failed to set RSA key for CGA param");
        goto cleanup;
    }
    ipnet_cga_generate(cga_param, &cga_addr);

    ipcom_printf("Modifier: ");
    for(i=0; i<sizeof(cga_param->modifier); i++)
        ipcom_printf("%02x", cga_param->modifier[i]);
    ipcom_printf(IP_LF);
    ipcom_printf("CGA: %s"IP_LF, ipcom_inet_ntop(IP_AF_INET6, &cga_addr, buf, sizeof(buf)));
    ret = IPCOM_SUCCESS;

    if (verify)
    {
        if (ipnet_cga_verify(cga_param, &cga_addr) == IPCOM_SUCCESS)
            ipcom_printf("CGA parameters successfully verified"IP_LF);
        else
            ipcom_printf("CGA parameters verification failed"IP_LF);
    }

 cleanup:
    if (bio)
        BIO_free(bio);
    ipnet_cga_param_free(cga_param);

    return ret;
}

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#else
int ipnet_cmd_cga_empty_file;
#endif /* IPNET_USE_RFC3971 */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

