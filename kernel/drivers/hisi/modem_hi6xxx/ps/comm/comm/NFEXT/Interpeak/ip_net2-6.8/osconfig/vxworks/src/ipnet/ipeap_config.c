/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipeap_config.c,v $ $Revision: 1.7 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipeap_config.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *   Design and implementation by Roger Boden <roger@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01g,24dec08,mry  Adding EAP-WSC
01f,02apr08,dwb  Adding EAP-AKA
01e,28feb08,mry  Adding LEAP support
01d,21feb08,mry  Adding PEAPv1, GTC support
01c,03jan07,kch  Updated CVS revision log entry for Jan 03 code drop.
01b,12dec06,kch  Added EAP_INSTALL_CALLBACK_HOOK parameter.
01a,19oct06,kch  Integrated to VxWorks build infrastructure.
*/


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 2                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef IPCOM_USE_CLIB_PROTO
#undef IPCOM_USE_CLIB_PROTO
#endif

#if 0
#include "ipeap_config.h"
#endif

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_clib.h>

#ifdef IPEAP_USE_DEFAULT_KEY_AND_CERT
#include <ipcom_err.h>
#include <ipcom_sysvar.h>
#include <ipcom_syslog.h>
#endif /* IPEAP_USE_DEFAULT_KEY_AND_CERT */

#ifdef INCLUDE_IPEAP_TLS_COMMON
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include <openssl/pem.h>
#endif /* INCLUDE_IPEAP_TLS_COMMON */

/* some internal macro mappings - ugly but necessary! */
#ifdef INCLUDE_IPEAP_USE_EAP_MD5
#define IPEAP_USE_EAP_MD5
#endif /* INCLUDE_IPEAP_USE_EAP_MD5 */

#ifdef INCLUDE_IPEAP_USE_EAP_GTC
#define IPEAP_USE_EAP_GTC
#endif /* INCLUDE_IPEAP_USE_EAP_GTC */

#ifdef INCLUDE_IPEAP_USE_EAP_LEAP
#define IPEAP_USE_EAP_LEAP
#endif /* INCLUDE_IPEAP_USE_EAP_LEAP */

#ifdef INCLUDE_IPEAP_USE_EAP_MSCHAPV2
#define IPEAP_USE_EAP_MSCHAPV2
#endif /* INCLUDE_IPEAP_USE_EAP_MSCHAPV2 */

#ifdef INCLUDE_IPEAP_USE_EAP_TLS
#define IPEAP_USE_EAP_TLS
#endif /* INCLUDE_IPEAP_USE_EAP_TLS */

#ifdef INCLUDE_IPEAP_USE_EAP_TTLS
#define IPEAP_USE_EAP_TTLS
#endif /* INCLUDE_IPEAP_USE_EAP_TTLS */

#ifdef INCLUDE_IPEAP_USE_EAP_PEAP
#define IPEAP_USE_EAP_PEAP
#endif /* INCLUDE_IPEAP_USE_EAP_PEAP */

#ifdef INCLUDE_IPEAP_USE_EAP_SIM
#define IPEAP_USE_EAP_SIM
#endif /* INCLUDE_IPEAP_USE_EAP_SIM */

#ifdef INCLUDE_IPEAP_USE_EAP_AKA
#define IPEAP_USE_EAP_AKA
#endif /* INCLUDE_IPEAP_USE_EAP_AKA */

#ifdef INCLUDE_IPEAP_USE_EAP_WSC
#ifndef INCLUDE_IPWPS
#error INCLUDE_IPEAP_USE_EAP_WSC requires INCLUDE_IPWPS
#endif
#define IPEAP_USE_EAP_WSC
#endif /* INCLUDE_IPEAP_USE_EAP_WSC */

#include "ipeap.h"

/*
 ****************************************************************************
 * 3                    DEFINES
 ****************************************************************************
 */

#ifdef IPEAP_USE_DEFAULT_KEY_AND_CERT
#define IPEAP_DEFAULT_CERT \
    "-----BEGIN CERTIFICATE-----\n" \
    "MIIDBzCCAnCgAwIBAgIBCzANBgkqhkiG9w0BAQQFADBxMQswCQYDVQQGEwJTRTES\n" \
    "MBAGA1UECBMJU3RvY2tob2xtMRIwEAYDVQQHEwlTdG9ja2hvbG0xEjAQBgNVBAoT\n" \
    "CUludGVycGVhazEUMBIGA1UECxMLRGV2ZWxvcG1lbnQxEDAOBgNVBAMTB1Rlc3Qg\n" \
    "Q0EwHhcNMDMwOTIyMTEzMzQzWhcNMDgwOTIwMTEzMzQzWjBUMQswCQYDVQQGEwJT\n" \
    "RTESMBAGA1UECBMJU3RvY2tob2xtMRIwEAYDVQQKEwlJbnRlcnBlYWsxHTAbBgNV\n" \
    "BAMTFGlwd2Vicy5pbnRlcnBlYWsuY29tMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCB\n" \
    "iQKBgQDcRErz2s+oJD1/FqeYXQc/FODeZk/LtrR6RkpwsYnTP+Wgi+TuqlWsIH93\n" \
    "bodh6g4bowTEz+Mi7rN5GSXVCMD21ty/edYg8QB6hyfyLqwAhdofippryrPUidnX\n" \
    "aDKZwzRCsojKYPxq1OgHNeAKUVprihDxXfRmwaTZMad15xC5LQIDAQABo4HLMIHI\n" \
    "MAkGA1UdEwQCMAAwHQYDVR0OBBYEFOghcvw/2RAQaWyf+koKyRT00Ph0MIGbBgNV\n" \
    "HSMEgZMwgZCAFKz+iHi8ujH8Efr9gk121K5RXgB5oXWkczBxMQswCQYDVQQGEwJT\n" \
    "RTESMBAGA1UECBMJU3RvY2tob2xtMRIwEAYDVQQHEwlTdG9ja2hvbG0xEjAQBgNV\n" \
    "BAoTCUludGVycGVhazEUMBIGA1UECxMLRGV2ZWxvcG1lbnQxEDAOBgNVBAMTB1Rl\n" \
    "c3QgQ0GCAQAwDQYJKoZIhvcNAQEEBQADgYEA0UCtTFM5ZBMyB9BRtyYtxalEL/Bw\n" \
    "ATPRaqAyCou1w2BZaz0HixE6mpyOsDMgK376N7X0O2FEr5MYbZcWgrlKJGeXPghT\n" \
    "QIrPM7ImesJyyu+9Qv41lVYOVAbpTBEjnIUm2QvSVjb5r++F3dWwH9S/bhM5Mn/s\n" \
    "3iGgfUthSvury/k=\n" \
    "-----END CERTIFICATE-----\n"

#define IPEAP_DEFAULT_KEY \
    "-----BEGIN RSA PRIVATE KEY-----\n" \
    "MIICXAIBAAKBgQDcRErz2s+oJD1/FqeYXQc/FODeZk/LtrR6RkpwsYnTP+Wgi+Tu\n" \
    "qlWsIH93bodh6g4bowTEz+Mi7rN5GSXVCMD21ty/edYg8QB6hyfyLqwAhdofippr\n" \
    "yrPUidnXaDKZwzRCsojKYPxq1OgHNeAKUVprihDxXfRmwaTZMad15xC5LQIDAQAB\n" \
    "AoGBAKfRsHmSoNwuoFwP2qkPV9IMCjfoI6vGDEONAnJSEfxNlpp3e3kLdjKqn/NS\n" \
    "1vsW5KDR13RMH3Yh/8OoEVb6UGlWd4QSCfyrGwvtV3nNX49YrEOl0mhs8ZL3gVuv\n" \
    "pbxLfCwMHvSTxZdQJBSW/V+rOtKMWxmIrX7la8sgtcYyysCBAkEA+mxyo2o397HG\n" \
    "temGtL92jAx+uqHF6J4wlZEqyzW5lFhsr48EYnP0O5FgSTHv/e3hD2NiRuux2tfS\n" \
    "NBCpOWJ+1QJBAOEr7zDKDEMoZgyogvQVHtIkmbn57a9Bd11S2a82RxCbcY/+OFUQ\n" \
    "s67f+Bl60clPot4HvjKfQducPHj6/HV77PkCQFC9anC5rHXWAaVhN6dLG+Onm6x8\n" \
    "wBxx9GQku9Fs7vqfT9uClv1POW2DP7IZwbDzY17cwjDviRZwAzv6KzVvRkkCQDoG\n" \
    "8XAYUYqpCYtvjbfLDZNIHVmg/9CvRUyGglS13yB3fo+GCwxY7oU9cJlYXmHLpQQk\n" \
    "6K3w/q7Sl37x50d7dTkCQCg7yi3+zGDwQClISKus8Sa6phul8TwiJNULQAPrPiBR\n" \
    "vsLcvQ/X+J8liQbsKnRvkxjWLZezwRYZXimxtKQPZ2I=\n" \
    "-----END RSA PRIVATE KEY-----\n"

#define IPEAP_DEFAULT_CA_CERT \
    "-----BEGIN CERTIFICATE-----\n" \
    "MIIDJzCCApCgAwIBAgIBADANBgkqhkiG9w0BAQQFADBxMQswCQYDVQQGEwJTRTES\n" \
    "MBAGA1UECBMJU3RvY2tob2xtMRIwEAYDVQQHEwlTdG9ja2hvbG0xEjAQBgNVBAoT\n" \
    "CUludGVycGVhazEUMBIGA1UECxMLRGV2ZWxvcG1lbnQxEDAOBgNVBAMTB1Rlc3Qg\n" \
    "Q0EwHhcNMDAxMjEyMTIxODA3WhcNMTAxMjEwMTIxODA3WjBxMQswCQYDVQQGEwJT\n" \
    "RTESMBAGA1UECBMJU3RvY2tob2xtMRIwEAYDVQQHEwlTdG9ja2hvbG0xEjAQBgNV\n" \
    "BAoTCUludGVycGVhazEUMBIGA1UECxMLRGV2ZWxvcG1lbnQxEDAOBgNVBAMTB1Rl\n" \
    "c3QgQ0EwgZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJAoGBANyiExOUr4XXnUbHu8hs\n" \
    "xMRmjQCXcQHUMihjt5fQyec5xGFSemf6EOMGpo9lVe2xU4N6FRljS10TEnKLfe7X\n" \
    "ThODCNCN1jBZv9oIWoJF4sfNNmjeNjkCs1IphKjipre5I33gtyJ8ZOCSNo6J+HKC\n" \
    "VrwwqDbgVK8j5of+cr1zd+CLAgMBAAGjgc4wgcswHQYDVR0OBBYEFKz+iHi8ujH8\n" \
    "Efr9gk121K5RXgB5MIGbBgNVHSMEgZMwgZCAFKz+iHi8ujH8Efr9gk121K5RXgB5\n" \
    "oXWkczBxMQswCQYDVQQGEwJTRTESMBAGA1UECBMJU3RvY2tob2xtMRIwEAYDVQQH\n" \
    "EwlTdG9ja2hvbG0xEjAQBgNVBAoTCUludGVycGVhazEUMBIGA1UECxMLRGV2ZWxv\n" \
    "cG1lbnQxEDAOBgNVBAMTB1Rlc3QgQ0GCAQAwDAYDVR0TBAUwAwEB/zANBgkqhkiG\n" \
    "9w0BAQQFAAOBgQAMwrGnbzXJl2hTKmBAOxAEIXWAVIW0MadPZLak9VrPVPufwAI1\n" \
    "2mJcFlJPhOE1s9DO88bxJZighv7o0V9QdIH4aVr+qanPOmlzgRGXkbbqK8EFHhgQ\n" \
    "FqCORRhrlsFdeRnswiZyPeEu3vpvFgN7Oy9Hk8gobIZ1k7Ly4b1BqrpovQ==\n" \
    "-----END CERTIFICATE-----\n"
#endif

#if 0
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPEAP
#define IPCOM_SYSLOG_PRIORITY    IPEAP_SYSLOG_PRIORITY
#endif

/*
 ****************************************************************************
 * 4                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    EXTERN PROTOTYPES
 ****************************************************************************
 */
extern IP_PUBLIC Ipeap_vendor_id ipeap_wsc_vendor_id;

/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipeap_configure(void);

/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */

IP_CONST Ipcom_sysvar ipeap_conf_sysvar[] =
{
    { "ipeap.identity", EAP_IDENTITY},
#ifdef INCLUDE_IPEAP_USE_EAP_MD5
    { "ipeap.md5", EAP_MD5},
    { "ipeap.md5.secret", EAP_MD5_SECRET},
#endif

#ifdef INCLUDE_IPEAP_USE_EAP_GTC
    { "ipeap.gtc", EAP_GTC},
#endif

#ifdef INCLUDE_IPEAP_USE_EAP_LEAP
    { "ipeap.leap", EAP_LEAP},
    { "ipeap.leap.user", EAP_LEAP_USER_ID },
    { "ipeap.leap.pw", EAP_LEAP_PASSWD},
#endif

#ifdef INCLUDE_IPEAP_USE_EAP_MSCHAPV2
    { "ipeap.mschapv2", EAP_MSCHAPV2},
    { "ipeap.mschapv2.user", EAP_MSCHAPV2_USER_ID },
    { "ipeap.mschapv2.pw", EAP_MSCHAPV2_PASSWD},
#endif

#ifdef INCLUDE_IPEAP_TLS_COMMON
    { "ipeap.tls.key_file", EAP_TLS_KEY_FILE},
    { "ipeap.tls.cert_file", EAP_TLS_CERT_FILE},
    { "ipeap.tls.ca_cert_file", EAP_TLS_CA_CERT_FILE},
#endif

#ifdef INCLUDE_IPEAP_USE_EAP_TLS
    { "ipeap.tls", EAP_TLS},
#endif

#ifdef INCLUDE_IPEAP_USE_EAP_TTLS
    { "ipeap.ttls", EAP_TTLS},
    { "ipeap.ttls.phase2", EAP_TTLS_PHASE2},  /* mschapv2 | pap */
    { "ipeap.ttls.user", EAP_TTLS_USER_ID},
    { "ipeap.ttls.pw", EAP_TTLS_PASSWD},
#endif

#ifdef INCLUDE_IPEAP_USE_EAP_PEAP
    { "ipeap.peap", EAP_PEAP},
    { "ipeap.peap.min_version", EAP_PEAP_MINVER},
    { "ipeap.peap.max_version", EAP_PEAP_MAXVER},
    { "ipeap.peap.v1.prf_label", EAP_PEAP_V1_PRF_LABEL},
#endif

#ifdef INCLUDE_IPEAP_USE_EAP_SIM
    { "ipeap.sim", EAP_SIM},
    { "ipeap.sim.test", EAP_SIM_TEST},
#endif

#ifdef INCLUDE_IPEAP_USE_EAP_AKA
    { "ipeap.aka", EAP_AKA},
    { "ipeap.aka.test", EAP_AKA_TEST},
#endif

#ifdef INCLUDE_IPEAP_USE_EAP_WSC
    { "ipeap.wsc", EAP_WSC},
#endif

    { IP_NULL, IP_NULL }
};


#ifdef IPEAP_USE_DEFAULT_KEY_AND_CERT
/*
 *===========================================================================
 *                    ipeap_tmpfile
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipeap_tmpfile(const char *file, const char *data)
{
    int len = strlen(data);
    int fd;

    if ((fd = ipcom_fileopen(file, IP_O_RDWR | IP_O_CREAT,
                             IP_S_IRUSR | IP_S_IWUSR)) < 0)
        return IPCOM_ERR_FAILED;

    if(ipcom_filewrite(fd, data, len) != len)
    {
        (void)ipcom_fileclose(fd);
        (void)ipcom_unlink(file);
        return IPCOM_ERR_FAILED;
    }
    (void)ipcom_fileclose(fd);
    return IPCOM_SUCCESS;
}
#endif /* IPEAP_USE_DEFAULT_KEY_AND_CERT */


#ifdef INCLUDE_IPEAP_TLS_COMMON

/****************************************************************************
 *
 * ipeap_tls_cert_verify_example - TLS cert verification example routine
 *
 * This is an  example implementation of the callback routine to verify
 * the peer TLS certification.  To use this example routine, assign it to
 * the macro EAP_TLS_CERT_VERIFY_CALLBACK_HOOK.
 *
 * NOMANUAL
 */
_WRS_INLINE
int ipeap_tls_cert_verify_example(int ok, X509_STORE_CTX *ctx)
{
    (void) ctx;
    return ok;
}

/****************************************************************************
 *
 * ipeap_load_tls_key_example - TLS key load example routine
 *
 * This is an example implementation of the callback routine loads the key
 * pair to use in SSL/TLS handshakes. To use this example routine, assign
 * it to the macro EAP_LOAD_TLS_KEY_CALLBACK_HOOK.
 *
 * NOMANUAL
 */
_WRS_INLINE
EVP_PKEY * ipeap_load_tls_key_example (void)
{
    BIO* bio;
    EVP_PKEY* ret = IP_NULL;
    char* file;
    char tmp_file[32];
    char* pfile;

    file = ipcom_sysvar_get("ipeap.tls.key_file", IP_NULL, 0);
    if(!file)
    {
        printf("ipeap_load_tls_key : failed to get "
               "ipeap.tls.key_file sysvar");
        return IP_NULL;
    }

#ifdef IPEAP_USE_DEFAULT_KEY_AND_CERT
    if(strcmp(file, "default") == 0)
    {
        /* Make the file path absolute */
        snprintf(tmp_file, sizeof(tmp_file), "%stmp_%s",
                 IPCOM_FILE_ROOT, file);
        (void)ipeap_tmpfile(tmp_file, IPEAP_DEFAULT_KEY);
        pfile = tmp_file;
    }
    else
#endif
        pfile = file;

    bio = BIO_new(BIO_s_file());
    if(!bio)
        goto cleanup;

    if(!BIO_read_filename(bio, pfile) )
        goto cleanup;

    ret = PEM_read_bio_PrivateKey(bio, IP_NULL, IP_NULL, IP_NULL);

cleanup:
    if(file)
    {
#ifdef IPEAP_USE_DEFAULT_KEY_AND_CERT
        if(strcmp(file, "default") == 0)
            (void)ipcom_unlink(tmp_file); /*lint !e645*/
#endif
        ipcom_free(file);
    }
    if(bio)
        (void)BIO_free(bio);

    return ret;

}

/****************************************************************************
 *
 * ipeap_load_tls_cert_example - X509 certificate load example routine
 *
 * This is an example implementation of the callback routine to loads the
 * X509 certificate to use in SSL/TLS handshakes. To use this example
 * routine, assign it to the macro EAP_LOAD_TLS_CERT_CALLBACK_HOOK.
 *
 * NOMANUAL
 */
_WRS_INLINE
X509 * ipeap_load_tls_cert_example (void)
{
    BIO* bio;
    X509* ret = IP_NULL;
    char* file;
    char tmp_file[32];
    char* pfile;

    file = ipcom_sysvar_get("ipeap.tls.cert_file", IP_NULL, 0);
    if(!file)
    {
        printf("ipeap_load_tls_cert: "
                "failed to get ipeap.tls.cert_file sysvar\n");
        return IP_NULL;
    }

#ifdef IPEAP_USE_DEFAULT_KEY_AND_CERT
    if(strcmp(file, "default") == 0)
    {
        /* Make the file path absolute */
        snprintf(tmp_file, sizeof(tmp_file), "%stmp_%s", IPCOM_FILE_ROOT, file);
        (void)ipeap_tmpfile(tmp_file, IPEAP_DEFAULT_CERT);
        pfile = tmp_file;
    }
    else
#endif
        pfile = file;

    bio = BIO_new(BIO_s_file());
    if(!bio)
        goto cleanup;

    if(!BIO_read_filename(bio, pfile) )
        goto cleanup;

    ret = PEM_read_bio_X509(bio, IP_NULL, IP_NULL, IP_NULL);

 cleanup:
    if(file)
    {
#ifdef IPEAP_USE_DEFAULT_KEY_AND_CERT
        if(strcmp(file, "default") == 0)
            (void)ipcom_unlink(tmp_file);  /*lint !e645*/
#endif
        ipcom_free(file);
    }
    if(bio)
        (void)BIO_free(bio);

    return ret;

}

/****************************************************************************
 *
 * ipeap_load_ca_certs_example - X509 CA certificates example routine
 *
 * This is an example implementation of the callback routine to loads the
 * X509 CA certificates to use in SSL/TLS handshakes. To use this example
 * routine, assign it to the macro EAP_LOAD_CA_CERT_CALLBACK_HOOK.
 *
 * NOMANUAL
 */
_WRS_INLINE
Ip_err ipeap_load_ca_certs_example(X509_STORE *cert_store)
{
    BIO* bio = IP_NULL;
    X509* ca_cert;
    Ip_err rc = IPCOM_ERR_FAILED;
    char* file;
    char* pfile;
    char tmp_file[32];

    file = ipcom_sysvar_get("ipeap.tls.ca_cert_file", IP_NULL, 0);
    if(!file)
    {
        printf("ipeap_load_tls_ca_certs: "
               "failed to get ipeap.tls.ca_cert_file sysvar\n");
        return IPCOM_ERR_FAILED;
    }

#ifdef IPEAP_USE_DEFAULT_KEY_AND_CERT
    if(strcmp(file, "default") == 0)
    {
        /* Make the file path absolute */
        snprintf(tmp_file, sizeof(tmp_file), "%stmp_%s", IPCOM_FILE_ROOT, file);
        (void)ipeap_tmpfile(tmp_file, IPEAP_DEFAULT_CA_CERT);
        pfile = tmp_file;
    }
    else
#endif

    pfile = file;

    bio = BIO_new(BIO_s_file());
    if(!bio)
        goto cleanup;

    if(!BIO_read_filename(bio, pfile))
        goto cleanup;

    while(IP_NULL != (ca_cert = PEM_read_bio_X509(bio, IP_NULL, IP_NULL,
                    IP_NULL)))
    {
        int ret;

        ret = X509_STORE_add_cert(cert_store, ca_cert);
        X509_free(ca_cert);
        if(!ret)
            goto cleanup;
    }

    rc = IPCOM_SUCCESS;

 cleanup:
    if(file)
    {
#ifdef IPEAP_USE_DEFAULT_KEY_AND_CERT
        if(strcmp(file, "default") == 0)
            (void)ipcom_unlink(tmp_file);  /*lint !e645*/
#endif
        ipcom_free(file);
    }
    if(bio)
        (void)BIO_free(bio);

    return rc;
}

#endif /* INCLUDE_IPEAP_TLS_COMMON */

/*
 ****************************************************************************
 * 9                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipeap_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipeap_configure(void)
{
    (void)ipcom_sysvar_setv(ipeap_conf_sysvar, 0);

    ipcom_openlog(IP_NULL, 0, IPCOM_LOG_IPEAP);
    ipcom_setlogmask_on(IPEAP_LOG_LEVEL, IPCOM_LOG_IPEAP);
    
#ifdef INCLUDE_IPEAP_USE_EAP_TLS
    (void)ipeap_mech_type_register(IPEAP_TYPE_TLS,
                                   ipeap_tls_suppl_methods_get());
#endif
#ifdef INCLUDE_IPEAP_USE_EAP_TTLS
    (void)ipeap_mech_type_register(IPEAP_TYPE_TTLS,
                                   ipeap_ttls_suppl_methods_get());
#endif
#ifdef INCLUDE_IPEAP_USE_EAP_PEAP
    (void)ipeap_mech_type_register(IPEAP_TYPE_PEAP,
                                   ipeap_peap_suppl_methods_get());
#endif
#ifdef INCLUDE_IPEAP_USE_EAP_SIM
    (void)ipeap_mech_type_register(IPEAP_TYPE_SIM, ipeap_sim_methods_get());
#endif
#ifdef INCLUDE_IPEAP_USE_EAP_AKA
    (void)ipeap_mech_type_register(IPEAP_TYPE_AKA, ipeap_aka_methods_get());
#endif
#ifdef INCLUDE_IPEAP_USE_EAP_MD5
    (void)ipeap_mech_type_register(IPEAP_TYPE_MD5_CHALLENGE,
                                   ipeap_md5_methods_get());
#endif
#ifdef INCLUDE_IPEAP_USE_EAP_MSCHAPV2
    (void)ipeap_mech_type_register(IPEAP_TYPE_MSCHAPV2,
                                   ipeap_mschapv2_suppl_methods_get());
#endif
#ifdef INCLUDE_IPEAP_USE_EAP_GTC
    (void)ipeap_mech_type_register(IPEAP_TYPE_GTC,
                                   ipeap_gtc_methods_get());
#endif
#ifdef INCLUDE_IPEAP_USE_EAP_LEAP
    (void)ipeap_mech_type_register(IPEAP_TYPE_LEAP,
                                   ipeap_leap_methods_get());
#endif
#ifdef INCLUDE_IPEAP_USE_EAP_WSC
    (void)ipeap_mech_type_register_expanded(ipeap_wsc_vendor_id,
				                            IPEAP_VT_WSC_SIMPLE_CONFIG,
											ipeap_wsc_methods_get());
#endif

    return IPCOM_SUCCESS;
}


#ifdef INCLUDE_IPEAP_TLS_COMMON

#if EAP_INSTALL_CALLBACK_HOOK == TRUE

/*
 * forward declarations for the user-defined eap callback hooks to satisfy
 * the compiler.
 */

int EAP_TLS_CERT_VERIFY_CALLBACK_HOOK (int,  X509_STORE_CTX *);
EVP_PKEY * EAP_LOAD_TLS_KEY_CALLBACK_HOOK (void);
X509 * EAP_LOAD_TLS_CERT_CALLBACK_HOOK (void);
Ip_err EAP_LOAD_CA_CERT_CALLBACK_HOOK (X509_STORE *);

#else

/*
 * EAP_TLS_CERT_VERIFY_CALLBACK_HOOK, EAP_LOAD_TLS_KEY_CALLBACK_HOOK,
 * EAP_LOAD_TLS_CERT_CALLBACK_HOOK and EAP_LOAD_CA_CERT_CALLBACK_HOOK
 * are applicable only if EAP_INSTALL_CALLBACK_HOOK is set to TRUE.
 * Explicitly re-define these hooks to NULL so that we don't get compiler
 * errors
 */

#undef EAP_TLS_CERT_VERIFY_CALLBACK_HOOK
#define EAP_TLS_CERT_VERIFY_CALLBACK_HOOK   NULL

#undef EAP_LOAD_TLS_KEY_CALLBACK_HOOK
#define EAP_LOAD_TLS_KEY_CALLBACK_HOOK      NULL

#undef EAP_LOAD_TLS_CERT_CALLBACK_HOOK
#define EAP_LOAD_TLS_CERT_CALLBACK_HOOK     NULL

#undef EAP_LOAD_CA_CERT_CALLBACK_HOOK
#define EAP_LOAD_CA_CERT_CALLBACK_HOOK      NULL

#endif /* EAP_INSTALL_CALLBACK_HOOK */

/* typedef to make compiler happy... */
typedef int (*ipeap_tls_cert_cb)(int ok, X509_STORE_CTX *ctx);
typedef EVP_PKEY * (*ipeap_load_tls_cb)(void);
typedef X509  * (*ipeap_load_tls_cert_cb)(void);
typedef Ip_err (*ipeap_load_ca_cert_cb)(X509_STORE *cert_store);

/****************************************************************************
 * ipeap_tls_cert_verify - Verify peer TLS certification
 *
 * This function is called once for each certificate in the certificate
 * chain of the TLS peer. Additional certificate and policy checks can
 * be implemented in this function.
 *
 * .IP <ok>
 * This parameter shows the result of the internal certificate verification
 * of the SSL library.
 * .IP <ctx>
 * Pointer to the certificate store
 * .LP
 *
 * RETURNS:
 * \is
 * \i 1 if the certicate is accepted
 * \i 0 if the certicate is rejected
 * \ie
 *
 * NOMANUAL
 */
IP_PUBLIC int ipeap_tls_cert_verify(int ok, X509_STORE_CTX *ctx)
{
    ipeap_tls_cert_cb cert_cb;

    cert_cb = (EAP_TLS_CERT_VERIFY_CALLBACK_HOOK == NULL) ?
              ipeap_tls_cert_verify_example : 
              EAP_TLS_CERT_VERIFY_CALLBACK_HOOK;

    return cert_cb(ok, ctx);
}

/****************************************************************************
 *
 * ipeap_load_tls_key - Loads the key pair to use in SSL/TLS handshakes
 *
 * This function loads the key pair to use in SSL/TLS handshakes. It is
 * called every time a new EAP-TLS session is initiated.
 *
 * RETURNS: Pointer to the key pair, or IP_NULL in case of failure.
 *
 * NOMANUAL
 */
IP_PUBLIC EVP_PKEY *ipeap_load_tls_key (void)
{
    ipeap_load_tls_cb  tls_cb;

    tls_cb = (EAP_LOAD_TLS_KEY_CALLBACK_HOOK == NULL) ? 
             ipeap_load_tls_key_example : EAP_LOAD_TLS_KEY_CALLBACK_HOOK;

    return tls_cb();

}

/****************************************************************************
 *
 * ipeap_load_tls_cert - Loads the X509 certificate
 *
 * This function loads the X509 certificate to use in SSL/TLS handshakes.
 * It is called every time a new EAP-TLS session is initiated.
 *
 * RETURNS: Pointer to the certificate, or IP_NULL in case of failure.
 *
 * NOMANUAL
 */
IP_PUBLIC X509 * ipeap_load_tls_cert (void)
{
    ipeap_load_tls_cert_cb tls_cb;

    tls_cb = (EAP_LOAD_TLS_CERT_CALLBACK_HOOK == NULL) ?
             ipeap_load_tls_cert_example : EAP_LOAD_TLS_CERT_CALLBACK_HOOK;

    return tls_cb();
}

/****************************************************************************
 * ipeap_load_ca_certs - Loads the X509 CA certificates
 *
 * This function loads the X509 CA certificates to use in SSL/TLS handshakes.
 * It is called every time a new EAP-TLS session is initiated.
 *
 * .IP <cert_store>
 * Expects a pointer to the certificate store where the CA certificates
 * shall be stored.
 *
 * RETURNS:
 * \is
 * \i IPCOM_SUCCESS if the CA certificates where successfully loaded.
 * \i IPCOM_ERR_FAILED if failed to load the CA certificates
 * \ie
 *
 * NOMANUAL
 */
IP_PUBLIC Ip_err ipeap_load_ca_certs(X509_STORE *cert_store)
{
    ipeap_load_ca_cert_cb cert_cb;

    cert_cb = (EAP_LOAD_CA_CERT_CALLBACK_HOOK == NULL) ?
              ipeap_load_ca_certs_example : EAP_LOAD_CA_CERT_CALLBACK_HOOK;

    return cert_cb(cert_store);
}

#endif /* INCLUDE_IPEAP_TLS_COMMON  */
