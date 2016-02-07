/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcrypto_config.c,v $ $Revision: 1.11 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipcrypto_config.c,v $
 *     $Author: rboden $
 *     $State: Exp $ $Locker:  $
 *
 *     Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
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

#include <ipcom_config.h>

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_sysvar.h>
#include <ipcom_shell.h>
#include <ipcom_os.h>


/*
 ****************************************************************************
 * 3                    DEFINES
 ****************************************************************************
 */

#define SHELL_ADD_CMD(cmd, usage, desc, hook)    \
 ipcom_shell_add_cmd(cmd, usage, desc, hook, IPCOM_PRIORITY_MIN, IPCOM_PROC_STACK_MAX)

#ifdef _WRS_CONFIG_COMPONENT_IPCRYPTO_USE_FIPS_140_2
#define OPENSSL_FIPS
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

/*
 *===========================================================================
 *                         apps
 *===========================================================================
 */
extern void ipcrypto_cmds_startup(void);

extern int verify_main(int argc, char ** argv);
extern int asn1parse_main(int argc, char ** argv);
extern int req_main(int argc,char *argv[]);
extern int dgst_main(int argc,char *argv[]);
extern int dh_main(int argc,char *argv[]);
extern int dhparam_main(int argc,char *argv[]);
extern int enc_main(int argc,char *argv[]);
extern int passwd_main(int argc,char *argv[]);
extern int gendh_main(int argc,char *argv[]);
extern int ca_main(int argc,char *argv[]);
extern int crl_main(int argc,char *argv[]);
extern int rsa_main(int argc,char *argv[]);
extern int rsautl_main(int argc,char *argv[]);
extern int dsa_main(int argc,char *argv[]);
extern int dsaparam_main(int argc,char *argv[]);
extern int x509_main(int argc,char *argv[]);
extern int genrsa_main(int argc,char *argv[]);
extern int gendsa_main(int argc,char *argv[]);
extern int speed_main(int argc,char *argv[]);
extern int version_main(int argc,char *argv[]);
extern int pkcs7_main(int argc,char *argv[]);
extern int crl2pkcs7_main(int argc,char *argv[]);
extern int nseq_main(int argc,char *argv[]);
extern int pkcs12_main(int argc,char *argv[]);
extern int pkcs8_main(int argc,char *argv[]);
extern int spkac_main(int argc,char *argv[]);
extern int smime_main(int argc,char *argv[]);
extern int rand_main(int argc,char *argv[]);
extern int engine_main(int argc,char *argv[]);


extern int ipcrypto_cmd_cmp(int argc, char **argv);
extern int ipcrypto_cmd_memory(int argc, char **argv);
#ifdef OPENSSL_FIPS
extern int ipcrypto_cmd_fips_ctrl(int argc, char **argv);
#endif

IP_GLOBAL void ipcrypto_key_db_install_example_keys(void);

/*
 *===========================================================================
 *                         test
 *===========================================================================
 */
extern int bftest(int argc, char **argv);
extern int bntest(int argc, char **argv);
extern int divtest(int argc, char **argv);
extern int exptest(int argc, char **argv);
extern int casttest(int argc, char **argv);
extern int destest(int argc, char **argv);
extern int dhtest(int argc, char **argv);
extern int dsatest(int argc, char **argv);
/* extern int ectest(int argc, char **argv); */
/* extern int ecdhtest(int argc, char **argv); */
/* extern int ecdsatest(int argc, char **argv); */
extern int enginetest(int argc, char **argv );
extern int hmactest(int argc, char **argv);
extern int evp_test(int argc, char **argv);
extern int hmactest(int argc, char **argv);
extern int md2test(int argc, char **argv);
extern int md4test(int argc, char **argv);
extern int md5test(int argc, char **argv);
/* extern int mdc2test(int argc, char **argv); */
extern int randtest(int argc, char **argv);
extern int rc2test(int argc, char **argv);
extern int rc4test(int argc, char **argv);
extern int rc5test(int argc, char **argv);
extern int rmdtest(int argc, char **argv);
extern int rsa_test(int argc, char **argv);
extern int sha1test(int argc, char **argv);
extern int shatest(int argc, char **argv);
extern int sha512test(int argc, char **argv);
extern int tabtest(int argc, char **argv);
extern int ipcrypto_cmd_aescmac_test(int argc, char **argv);
extern int ipcrypto_cmd_aeskeywrap_test(int argc, char **argv);


/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

#if IPCOM_USE_SHELL != IPCOM_SHELL_NONE




#ifdef INCLUDE_IPCRYPTO_USE_CMDS
IP_STATIC int ipcrypto_verify_main(int argc, char ** argv);
IP_STATIC int ipcrypto_asn1parse_main(int argc, char ** argv);

IP_STATIC int ipcrypto_req_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_dgst_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_dh_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_dhparam_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_enc_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_passwd_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_gendh_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_ca_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_crl_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_rsa_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_rsautl_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_dsa_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_dsaparam_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_x509_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_genrsa_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_gendsa_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_speed_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_version_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_pkcs7_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_crl2pkcs7_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_nseq_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_pkcs12_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_pkcs8_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_spkac_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_smime_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_rand_main(int argc,char *argv[]);

IP_STATIC int ipcrypto_engine_main(int argc,char *argv[]);
#endif
#endif /* IPCOM_USE_SHELL != IPCOM_SHELL_NONE */

IP_PUBLIC Ip_err ipcrypto_configure(void);

/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         SYSVAR CONFIGURATION
 *===========================================================================
 */

IP_STATIC IP_CONST Ipcom_sysvar ipcrypto_conf_sysvar[] =
{
    { "OPENSSL_CONF", OPENSSL_CONF },
    { "OPENSSL_HOME", OPENSSL_HOME },
    { IP_NULL, IP_NULL }
};

#ifdef OPENSSL_FIPS
/* When running ipcrypto in FIPS 140-2 mode, a fingerprint (hash) is calculated
 * of the FIPS module implementation. This fingerprint is compared to an expected
 * value (ipcrypto_fips_signature). If the value matches, the system is allowed
 * to enter FIPS 140-2 mode. Initializing ipcrypto_fips_signature to all 0xEE
 * will not enable FIPS 140-2 mode. After you have booted your image you can
 * use the shell command 'fips_ctrl hash' to calculate the fips_signature.
 * Thereafter update ipcrypto_fips_signature below, rebuild and reboot and
 * ipcrypto will start in FIPS 140-2 mode. */
unsigned char ipcrypto_fips_signature[20] =
    { 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
      0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE };
#endif

/*
 ****************************************************************************
 * 8                    STATIC FUNCTIONS
 ****************************************************************************
 */


#if IPCOM_USE_SHELL != IPCOM_SHELL_NONE
/*
 *===========================================================================
 *                    SHELL COMMANDS
 *===========================================================================
 */

#ifdef INCLUDE_IPCRYPTO_USE_CMDS
IP_STATIC int
ipcrypto_verify_main(int argc, char ** argv)
{
    ipcrypto_cmds_startup();
    return verify_main(argc, argv);
}



IP_STATIC int
ipcrypto_asn1parse_main(int argc, char ** argv)
{
    ipcrypto_cmds_startup();
    return asn1parse_main(argc, argv);
}



IP_STATIC int
ipcrypto_req_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return req_main(argc, argv);
}

IP_STATIC int
ipcrypto_dgst_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return dgst_main(argc, argv);
}



IP_STATIC int
ipcrypto_dh_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return dh_main(argc, argv);
}

IP_STATIC int
ipcrypto_dhparam_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return dhparam_main(argc, argv);
}

IP_STATIC int
ipcrypto_enc_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return enc_main(argc, argv);
}

IP_STATIC int
ipcrypto_passwd_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return passwd_main(argc, argv);
}

IP_STATIC int
ipcrypto_gendh_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return gendh_main(argc, argv);
}

IP_STATIC int
ipcrypto_ca_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return ca_main(argc, argv);
}

IP_STATIC int
ipcrypto_crl_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return crl_main(argc, argv);
}

IP_STATIC int
ipcrypto_rsa_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return rsa_main(argc, argv);
}

IP_STATIC int
ipcrypto_rsautl_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return rsautl_main(argc, argv);
}

IP_STATIC int
ipcrypto_dsa_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return dsa_main(argc, argv);
}

IP_STATIC int
ipcrypto_gendsa_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return gendsa_main(argc, argv);
}

IP_STATIC int
ipcrypto_dsaparam_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return dsaparam_main(argc, argv);
}

IP_STATIC int
ipcrypto_x509_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return x509_main(argc, argv);
}

IP_STATIC int
ipcrypto_genrsa_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return genrsa_main(argc, argv);
}

IP_STATIC int
ipcrypto_speed_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return speed_main(argc, argv);
}

IP_STATIC int
ipcrypto_version_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return version_main(argc, argv);
}

IP_STATIC int
ipcrypto_pkcs7_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return pkcs7_main(argc, argv);
}

IP_STATIC int
ipcrypto_crl2pkcs7_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return crl2pkcs7_main(argc, argv);
}

IP_STATIC int
ipcrypto_nseq_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return nseq_main(argc, argv);
}

IP_STATIC int
ipcrypto_pkcs12_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return pkcs12_main(argc, argv);
}

IP_STATIC int
ipcrypto_pkcs8_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return pkcs8_main(argc, argv);
}

IP_STATIC int
ipcrypto_spkac_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return spkac_main(argc, argv);
}

IP_STATIC int
ipcrypto_smime_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return smime_main(argc, argv);
}

IP_STATIC int
ipcrypto_rand_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return rand_main(argc, argv);
}

IP_STATIC int
ipcrypto_engine_main(int argc,char *argv[])
{
    ipcrypto_cmds_startup();
    return engine_main(argc, argv);
}

#endif
#endif /* IPCOM_USE_SHELL != IPCOM_SHELL_NONE */

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
 *                    ipcrypto_configure
 *===========================================================================
 */
IP_PUBLIC Ip_err
ipcrypto_configure(void)
{
	/* Initialize IPCRYPTO system variables. */
    (void)ipcom_sysvar_setv(ipcrypto_conf_sysvar, IPCOM_SYSVAR_FLAG_OVERWRITE);

#if IPCOM_USE_SHELL != IPCOM_SHELL_NONE

#ifdef INCLUDE_IPCRYPTO_USE_CMDS
    SHELL_ADD_CMD("verify",
                  "verify [-verbose] [-CApath path] [-CAfile file] [-purpose purpose] cert1 cert2 ...",
                  "X.509 Certificate Verification", ipcrypto_verify_main);
    SHELL_ADD_CMD("asn1parse", "asn1parse [options]",
                  "Parse an ASN.1 sequence", ipcrypto_asn1parse_main);

    SHELL_ADD_CMD("req", "req [options]",
                  "X.509 Certificate Signing Request (CSR) Management", ipcrypto_req_main);

    SHELL_ADD_CMD("dgst", "dgst [options] [file ...]", "Message Digest Calculation", ipcrypto_dgst_main);

    SHELL_ADD_CMD("dh", "dh [options]",
                  "Diffie-Hellman Parameter Management. Obsoleted by dhparam", ipcrypto_dh_main);

    SHELL_ADD_CMD("dhparam", "dhparam [options] [numbits]",
                  "Generation and Management of Diffie-Hellman Parameters", ipcrypto_dhparam_main);

    SHELL_ADD_CMD("gendh", "x", "Generation of Diffie-Hellman Parameters. Obsoleted by dhparam", ipcrypto_gendh_main);

    SHELL_ADD_CMD("enc", "enc [options]", "Encoding with Ciphers", ipcrypto_enc_main);

    SHELL_ADD_CMD("passwd", "passwd [options] [passwords]", "Generation of hashed passwords", ipcrypto_passwd_main);

    SHELL_ADD_CMD("ca", "ca args", "Certificate Authority (CA) Management", ipcrypto_ca_main);

    SHELL_ADD_CMD("crl", "crl args", "Certificate Revocation List (CRL) Management", ipcrypto_crl_main);

    SHELL_ADD_CMD("rsa", "rsa [options]", "RSA Data Management", ipcrypto_rsa_main);

    SHELL_ADD_CMD("rsautl", "rsautl [options]",
                  "RSA utility for signing, verification, encryption, and decryption", ipcrypto_rsautl_main);

    SHELL_ADD_CMD("genrsa", "genrsa [args] [numbits]", "Generation of RSA keys", ipcrypto_genrsa_main);

    SHELL_ADD_CMD("dsa", "dsa [options]", "DSA key processing", ipcrypto_dsa_main);

    SHELL_ADD_CMD("dsaparam", "dsaparam [options]", "DSA parameter manipulation and generation", ipcrypto_dsaparam_main);

    SHELL_ADD_CMD("gendsa", "gendsa [args] dsaparam-file",
                  "Generation of DSA Parameters", ipcrypto_gendsa_main);

    SHELL_ADD_CMD("x509", "x509 [options]", "Certificate display and signing utility", ipcrypto_x509_main);

    SHELL_ADD_CMD("speed", "speed [options]", "Test encryption performance", ipcrypto_speed_main);

    SHELL_ADD_CMD("pkcs7", "pkcs7 [options]", "PKCS#7 Data Management", ipcrypto_pkcs7_main);

    SHELL_ADD_CMD("crl2p7", "crl2p7 [options]", "CRL to PKCS#7 Conversion", ipcrypto_crl2pkcs7_main);

    SHELL_ADD_CMD("nseq", "nseq [options]", "Create or examine a netscape certificate sequence", ipcrypto_nseq_main);

    SHELL_ADD_CMD("pkcs12", "pkcs12 [options]", "PKCS#12 file utility", ipcrypto_pkcs12_main);

    SHELL_ADD_CMD("pkcs8", "pkcs8 [options]", "PKCS#8 format private key conversion tool", ipcrypto_pkcs8_main);

    SHELL_ADD_CMD("spkac", "spkac [options]", "SPKAC printing and generating utility", ipcrypto_spkac_main);

    SHELL_ADD_CMD("smime", "smime [options]", "S/MIME mail processing", ipcrypto_smime_main);

    SHELL_ADD_CMD("rand", "rand [options]", "Generate pseudo-random bytes", ipcrypto_rand_main);

    SHELL_ADD_CMD("ipcrypto_ver", "ipcrypto_ver", "Show IPCRYPTO version", ipcrypto_version_main);

    SHELL_ADD_CMD("engine", "engine", "Hardware Crypto Support", ipcrypto_engine_main);

#ifdef OPENSSL_FIPS
    SHELL_ADD_CMD("fips_ctrl", "fips_ctrl", "FIPS control utility", ipcrypto_cmd_fips_ctrl);
#endif

#endif


#ifdef INCLUDE_IPCRYPTO_USE_TEST_CMDS
    SHELL_ADD_CMD("cmp",     "cmp",    "cmp",    ipcrypto_cmd_cmp);
    SHELL_ADD_CMD("sslmem",  "sslmem", "sslmem", ipcrypto_cmd_memory);

    /* general test/ progs. */
    SHELL_ADD_CMD("bntest", "bntest", "Big number test utility.", bntest);
    SHELL_ADD_CMD("exptest", "exptest", "Big number test utility.", exptest);

#if 0
    SHELL_ADD_CMD("ectest", "ectest", "ectest test utility.", ectest);
    SHELL_ADD_CMD("ecdhtest", "ecdhtest", "ecdhtest test utility.", ecdhtest);
    SHELL_ADD_CMD("ecdsatest", "ecdsatest", "ecdsatest test utility.", ecdsatest);
#endif
    SHELL_ADD_CMD("enginetest", "enginetest", "engine test utility.", enginetest);

#ifndef OPENSSL_FIPS
    SHELL_ADD_CMD("md2test", "md2test", "md2test test utility.", md2test);
#endif

#ifndef OPENSSL_FIPS
    SHELL_ADD_CMD("md4test", "md4test", "md4test test utility.", md4test);
#endif

    SHELL_ADD_CMD("md5test", "md5test", "md5test test utility.", md5test);

    SHELL_ADD_CMD("hmactest", "hmactest", "hmactest test utility.", hmactest);

#ifndef OPENSSL_FIPS
    SHELL_ADD_CMD("rc2test", "rc2test", "RC2 test utility.", rc2test);
#endif

#ifndef OPENSSL_FIPS
    SHELL_ADD_CMD("rc4test", "rc4test", "RC4 test utility.", rc4test);
#endif

    SHELL_ADD_CMD("destest", "destest", "destest test utility.", destest);

    SHELL_ADD_CMD("sha1test", "sha1test", "sha1 test utility.", sha1test);
#ifndef OPENSSL_FIPS
    SHELL_ADD_CMD("shatest", "shatest", "sha test utility.", shatest);
    SHELL_ADD_CMD("sha512test", "sha512test", "sha512 test utility.", sha512test);
#endif

#if 0
    SHELL_ADD_CMD("mdc2test", "mdc2test", "mdc2test test utility.", mdc2test);
#endif

#ifndef OPENSSL_FIPS
    SHELL_ADD_CMD("rmdtest", "rmdtest", "rmdtest test utility.", rmdtest);
#endif

    SHELL_ADD_CMD("randtest", "randtest", "randtest test utility.", randtest);

    SHELL_ADD_CMD("dhtest", "dhtest", "dhtest test utility.", dhtest);


#ifndef OPENSSL_FIPS
    SHELL_ADD_CMD("casttest", "casttest", "casttest test utility.", casttest);
#endif

#ifndef OPENSSL_FIPS
    SHELL_ADD_CMD("bftest", "bftest", "bftest test utility.", bftest);
#endif

    SHELL_ADD_CMD("dsatest", "dsatest", "dsatest test utility.", dsatest);

    SHELL_ADD_CMD("exptest", "exptest", "exptest test utility.", exptest);

    SHELL_ADD_CMD("rsa_test", "rsa_test", "rsa_test test utility.", rsa_test);
    SHELL_ADD_CMD("rsa_oaep_test", "rsa_oaep_test", "rsa_oaep_test test utility.", rsa_test);


    SHELL_ADD_CMD("evp_test", "evp_test", "evp_test test utility.", evp_test);

    SHELL_ADD_CMD("tabtest", "tabtest", "X509 test utility.", tabtest);
    SHELL_ADD_CMD("cmactest", "cmactest", "AES-CMAC test utility.",ipcrypto_cmd_aescmac_test);
    SHELL_ADD_CMD("aeskeywraptest", "aeskeywraptest", "AES-Keywrap test utility.",ipcrypto_cmd_aeskeywrap_test);


#endif /* INCLUDE_IPCRYPTO_USE_TEST_CMDS */

#endif /* IPCOM_USE_SHELL != IPCOM_SHELL_NONE */

#ifdef INCLUDE_IPCRYPTO_USE_KEY_DB_EXAMPLE_KEYS
    ipcrypto_key_db_install_example_keys();
#endif

    /* No configuration to enter */
    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

