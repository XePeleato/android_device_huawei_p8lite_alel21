#ifndef ANDROID_HARDWARE_HISI_KEYMASTER_H
#define ANDROID_HARDWARE_HISI_KEYMASTER_H
#include "log.h"

#define HISI_KEYSTORE_KEYMASTER "HISI_KEYMASTER"
#define KEYMATER_SUCCESS  0
#define KEYMASTER_FAILURE  -1

#define HISI_KEYMASTER_API_VERSION 2

#define KM_MAGIC_NUM     (0x4B4D4B42)
#define KM_KEY_SIZE_MAX  (512)
#define KM_IV_LENGTH     (16)
#define KM_HMAC_LENGTH   (32)


#define KM_IOC_MAGIC  'k'

#define KM_IOC_GET_DATA _IOWR(KM_IOC_MAGIC, 1, int)
#define KM_IOC_GEN_DATA _IOWR(KM_IOC_MAGIC, 2, int)
#define KM_IOC_IMP_DATA _IOWR(KM_IOC_MAGIC, 3, int)
#define KM_IOC_SIG_DATA _IOWR(KM_IOC_MAGIC, 4, int)
#define KM_IOC_VRY_DATA _IOWR(KM_IOC_MAGIC, 5, int)
#define KM_IOC_TST_DATA _IOWR(KM_IOC_MAGIC, 6, int)

#define KM_IOC_MAXNR 6

struct gen_key_pair_t {
        int crt;                         /*in*/
        void *exponent;
        uint16_t exponent_len;                /*in*/
        int key_size;                    /*in*/
        void *blob;                      /*out*/
        int blob_len;                  /*out*/
};

struct get_keypair_t {
        const void * blob;             /*in*/
        int blob_len;            /*in*/
        uint8_t *exponent;       /*out*/
        int exponent_len;        /*out*/
        uint8_t *modulus;        /*out*/
        int modulus_len;         /*out*/
};

struct sign_data_t {
        const void * blob;
        int blob_len;
        const void * data_in;
        size_t data_in_len;
        void * data_out;
        size_t data_out_len;
};

struct verify_data_t {
        const void * blob;
        int blob_len;
        const void * data_in;
        int data_in_len;
        void * data_out;
        int data_out_len;
};

struct import_key_t {
        int crt_mode;                /*crt mode or not*/
        void * pri_exponent;
        uint16_t pri_exponent_len;
        void * pub_exponent;
        uint16_t pub_exponent_len;
        void * modulus_ptr;
        uint16_t  modulus_len;

        void *rsa_p;
        uint16_t  rsa_p_len;
        void *rsa_q;
        uint16_t  rsa_q_len;
        void *rsa_dp;
        uint16_t  rsa_dp_len;
        void *rsa_dq;
        uint16_t  rsa_dq_len;
        void *rsa_qinv;
        uint16_t  rsa_qinv_len;
        void *blob;
        int   blob_len;
};

struct km_test_data {
        void *data;
        int len;
};

#endif  // ANDROID_HARDWARE_HISI_KEYMASTER_H

