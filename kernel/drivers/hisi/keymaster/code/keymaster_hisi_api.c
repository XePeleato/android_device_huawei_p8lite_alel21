#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <asm/io.h>
#include <linux/hisi/hi6xxx-iomap.h>
#include "keymaster_hisi.h"
#include "crys_rsa_build.h"
#include "crys_rsa_schemes.h"
#include "crys_rsa_types.h"
#include "crys.h"
#include "crys_rsa_kg.h"
#include "crys_rsa_prim.h"
#include "dx_cclib.h"
#include "log.h"
#include "soc_baseaddr_interface.h"
#include "soc_peri_sctrl_interface.h"

#define MOD_SIZE_IN_BYTES 				64
#define MAX_pri_table_NUM		50
#define TST_NO_CRT       0
#define TST_CRT_MODE 1
#define TST_INTERNAL_KG         0
#define TST_NO_INTERNAL_KEYGEN  1


#define RSA_KEY_SIZE			4096
#define BIT(x) (0x1 << (x))

struct key_blob_t {
        CRYS_RSAUserPubKey_t  public_key;
        int public_size;
        int hash_private;
};

static int KM_MAJOR;
static struct class *dev_class = NULL;
static struct device *dev_device = NULL;
static int hash_index = 0;

static CRYS_RSAUserPrivKey_t pri_table[MAX_pri_table_NUM];

static CRYS_RSAPrimeData_t tmp_prim_data;
static CRYS_RSAUserPrivKey_t tmp_private_data;
static CRYS_RSAKGData_t  tmp_kg_data;
static int init_status = 0;

/*
 * reset dx ip and do cclib init
 */
int dx_hw_reset(void)
{
        int ret;
        if(init_status != 0)
                return 0;

       void __iomem *va_km = ioremap(SOC_PERI_SCTRL_BASE_ADDR, 0x1000);

	/*SECENG Enable CLK*/
	writel(BIT(SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_seceng_acpu_START), SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_ADDR(va_km));
        mdelay(10);

	/*SECENG RSTEN*/
	writel(BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_periph_rsten2_seceng_START), SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_ADDR(va_km));
        mdelay(10);

	/*SECENG RSTDIS*/
	writel(BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_periph_rstdis2_seceng_START), SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_ADDR(va_km));
        mdelay(10);

	iounmap(va_km);

        ret = DX_CclibInit();
        if(ret == 0) {
                init_status = 1;
        }
        LOGE("dx cclib init %x",ret);
        return ret;
}

/*
 * Use private key to sign data
 */
int km_sign_data(void *arg)
{
        int status;
        int data_out_len;
        struct key_blob_t * key_blob;
        uint8_t * signed_data_out;
        struct sign_data_t * sign_data = (struct sign_data_t *)arg;
        if(NULL == sign_data) {
                LOGE("argument error");
                return -1;
        }
        key_blob = (struct key_blob_t *)sign_data->blob;
        if((NULL == key_blob) || (NULL == sign_data->data_in)) {
                LOGE("input error");
                return -1;
        }
        signed_data_out = (uint8_t *)kmalloc(RSA_KEY_SIZE,GFP_KERNEL);
        if(NULL == signed_data_out) {
                LOGE("kmalloc error");
                return -1;
        }

        LOGD("hash_private %d",key_blob->hash_private);
        status = CRYS_RSA_PRIM_Sign(&(pri_table[key_blob->hash_private]),&tmp_prim_data,(DxUint8_t *)sign_data->data_in,
                                    sign_data->data_in_len,signed_data_out);

        LOGD("sign status %d in len %d \n",status,sign_data->data_in_len);
        if(status != 0) {
                LOGE("km sign data error %x",status);
                goto out;
        }
        data_out_len = key_blob->public_size / 8;
        LOGD("current public size %d",key_blob->public_size);
        status = copy_to_user(sign_data->data_out,signed_data_out,key_blob->public_size);
        if(status != 0) {
                LOGE("copy to user  error %x",status);
                goto out;
        }
        status = copy_to_user(&(sign_data->data_out_len),&data_out_len,sizeof(int));
        if(status != 0) {
                LOGE("copy to user  error %x",status);
                goto out;
        }
        if(status != 0) {
                LOGE("copy to user  error %x",status);
                goto out;
        }
        kfree(signed_data_out);
        signed_data_out = NULL;
        return 0;
out:
        kfree(signed_data_out);
        signed_data_out = NULL;
        return status;
}

/*
 * use public key to verify data
 */
int km_verify_data(void *arg)
{
        int status;
        int out_len;
        struct key_blob_t * key_blob;
        struct verify_data_t * verify_data = (struct verify_data_t *)arg;
        uint8_t * verified_data;
        if(NULL == verify_data) {
                LOGE("argument error");
                return -1;
        }
        key_blob = (struct key_blob_t *)verify_data->blob;
        if((NULL == key_blob) || (NULL == verify_data->data_in)) {
                LOGE("input data error");
                return -1;
        }

        verified_data = (uint8_t *)kmalloc(RSA_KEY_SIZE,GFP_KERNEL);
        if(NULL == verified_data) {
                LOGE("data null error");
                return -1;
        }
        LOGD("verify data input len %d",verify_data->data_in_len);

        status = CRYS_RSA_PRIM_Verify(&(key_blob->public_key),&tmp_prim_data,(DxUint8_t *)verify_data->data_in,
                                      verify_data->data_in_len,verified_data);
        LOGD("sign status %d in len %d \n",status,verify_data->data_in_len);

        if(status != 0) {
                LOGE("km verify data error %x",status);
                goto out;
        }
        out_len = key_blob->public_size / 8;
        status = copy_to_user(verify_data->data_out,verified_data,2048);
        if(status != 0) {
                LOGE("copy to user  error %x",status);
                goto out;
        }
        status = copy_to_user(&(verify_data->data_out_len),&(out_len),sizeof(int));
        if(status != 0) {
                LOGE("copy to user  error %x",status);
                goto out;
        }
        kfree(verified_data);
        verified_data = NULL;
        return 0;
out:
        kfree(verified_data);
        verified_data = NULL;
        return status;
}

/*
 * import key from x.509 format
 */
int km_import_key(void *arg)
{
        int status;
        int key_blob_len = sizeof(struct key_blob_t);
        struct key_blob_t * key_blob;
        struct import_key_t * import_key = (struct import_key_t *)arg;
        if((NULL == import_key) || (NULL == import_key->blob)) {
                LOGE("input data error");
                return -1;
        }
        
        key_blob = kmalloc(sizeof(struct key_blob_t),GFP_KERNEL);
        if(NULL == key_blob) {
                LOGE("kmalloc error");
                return -1;
        }

        LOGD("current e_len %d m_len %d",import_key->pub_exponent_len,import_key->modulus_len);
        status = CRYS_RSA_Build_PubKey(&(key_blob->public_key),import_key->pub_exponent,
                                       import_key->pub_exponent_len,
                                       import_key->modulus_ptr,import_key->modulus_len);
        if(!import_key->crt_mode) {
                status = CRYS_RSA_Build_PrivKey(&tmp_private_data,import_key->pri_exponent,import_key->pri_exponent_len,
                                                import_key->pub_exponent,import_key->pub_exponent_len,import_key->modulus_ptr,
                                                import_key->modulus_len);
                if(status != 0) {
                        LOGE("km sign data error %x",status);
                        goto out;
                }
        } else {
                status = CRYS_RSA_Build_PrivKeyCRT(&tmp_private_data,import_key->rsa_p,import_key->rsa_p_len,
                                                   import_key->rsa_q,import_key->rsa_q_len,import_key->rsa_dp,
                                                   import_key->rsa_dp_len,import_key->rsa_dq,import_key->rsa_dq_len,
                                                   import_key->rsa_qinv,import_key->rsa_qinv_len);
                if(status != 0) {
                        LOGE("km import private key crt mode %x",status);
                        goto out;
                }
        }
        key_blob->hash_private = hash_index;
        key_blob->public_size = import_key->modulus_len * 8;
        memcpy(pri_table + hash_index, &tmp_private_data,sizeof(CRYS_RSAUserPrivKey_t));
        hash_index ++;
        if(hash_index >= MAX_pri_table_NUM - 1) {
                hash_index = 0;
        }

        status = copy_to_user(import_key->blob,key_blob,sizeof(struct key_blob_t));
        if(status != 0) {
                LOGE("copy to user  error %x",status);
                goto out;
        }
        status = copy_to_user(&(import_key->blob_len),&key_blob_len,sizeof(int));
        if(status != 0) {
                LOGE("copy to user  error %x",status);
                goto out;
        }
        kfree(key_blob);
        key_blob = NULL;
        return 0;
out:
        kfree(key_blob);
        key_blob = NULL;
        return status;
}

/*
 * generate rsa public & private key pair
 */
int km_generate_key(void *arg)
{
        int status;
        uint16_t e_len;
        struct gen_key_pair_t * gen_keypair;
        struct key_blob_t * key_blob;
        uint32_t e_value;
        int key_blob_len = sizeof(struct key_blob_t);
        key_blob = kmalloc(sizeof(struct key_blob_t),GFP_KERNEL);
        if(key_blob == NULL) {
                LOGE("allocate key_blob failed !");
                return -1;
        }

        gen_keypair = (struct gen_key_pair_t *)arg;
        if(gen_keypair == NULL) {
                LOGE("arg is null");
                return -1;
        }
        if(gen_keypair->blob == NULL) {
                LOGE("generate key pair blob null");
                return -1;
        }
        e_value = *(uint32_t *)(gen_keypair->exponent);
        if(e_value <= 0xff)
                e_len = 1;
        else if(e_value > 0xff && e_value <= 0xffff)
                e_len = 2;
        else if(e_value > 0xffff && e_value <= 0xffffff)
                e_len = 3;
        else if(e_value > 0xffffff)
                e_len = 4;

        LOGE("current evalue elen %x \t%d",e_value,e_len);

        status = CRYS_RSA_KG_GenerateKeyPairCRT((uint8_t *)gen_keypair->exponent,
                                                e_len,
                                                gen_keypair->key_size,
                                                &tmp_private_data,
                                                &(key_blob->public_key),
                                                &tmp_kg_data);
        if(status != 0) {
                LOGE("generate key pair error %d",status);
                goto out;
        }
        key_blob->hash_private = hash_index;
        key_blob->public_size = gen_keypair->key_size;
        memcpy(pri_table + hash_index, &tmp_private_data,sizeof(CRYS_RSAUserPrivKey_t));
        status = copy_to_user(gen_keypair->blob,key_blob,sizeof(struct key_blob_t));
        if(status != 0) {
                LOGE("copy to user  error %x",status);
                goto out;
        }
        status = copy_to_user(&(gen_keypair->blob_len),&key_blob_len,sizeof(int));
        if(status != 0) {
                LOGE("copy to user  error %x",status);
                goto out;
        }
        hash_index ++;
        if(hash_index >= MAX_pri_table_NUM - 1) {
                LOGE("mntn keypair over max");
                hash_index = 0;
        }
        kfree(key_blob);
        key_blob = NULL;
        return 0;
out:
        kfree(key_blob);
        key_blob = NULL;
        return status;
}

/*
 * get the public key from key blob
 */
int km_get_key(void *arg)
{
        int status;
        struct key_blob_t * key_blob;
        uint8_t * Exponent_ptr;
        uint8_t * Modulus_ptr;
        int ModulusSize_ptr = RSA_KEY_SIZE;
        int ExponentSize_ptr = RSA_KEY_SIZE;
        struct get_keypair_t * get_keypair = (struct get_keypair_t *)arg;
        if(NULL == get_keypair) {
                LOGD("argument error");
                return -1;
        }
        if((NULL == get_keypair->modulus) || (NULL == get_keypair->exponent)) {
                LOGD("modulus or exponent null");
                return -1;
        }
        key_blob = (struct key_blob_t *)get_keypair->blob;

        Exponent_ptr = (uint8_t *)kmalloc(RSA_KEY_SIZE,GFP_KERNEL);
        if(NULL == Exponent_ptr) {
                LOGE("kmalloc error");
                return -1;
        }
        Modulus_ptr = (uint8_t *)kmalloc(RSA_KEY_SIZE,GFP_KERNEL);
        if(NULL == Modulus_ptr) {
                kfree(Exponent_ptr);
                Exponent_ptr = NULL;
                LOGE("kmalloc error");
                return -1;
        }
        LOGD("get public key %d %d ",ExponentSize_ptr,ModulusSize_ptr);

        status = CRYS_RSA_Get_PubKey(&(key_blob->public_key),Exponent_ptr, (DxUint16_t *)&ExponentSize_ptr,
                                     Modulus_ptr,(DxUint16_t *)&ModulusSize_ptr);

        LOGD("current status %x",status);
        LOGD("get public key %d %d ",ExponentSize_ptr,ModulusSize_ptr);
        if(status != 0) {
                LOGE("get public key error");
                goto out;
        }
        status = copy_to_user(get_keypair->exponent,Exponent_ptr,ExponentSize_ptr);
        if(status != 0) {
                LOGE("copy to user  error %x",status);
                goto out;
        }
        status = copy_to_user(&(get_keypair->exponent_len),&ExponentSize_ptr,sizeof(int));
        if(status != 0) {
                LOGE("copy to user  error %x",status);
                goto out;
        }
        status = copy_to_user(get_keypair->modulus,Modulus_ptr,ModulusSize_ptr);
        if(status != 0) {
                LOGE("copy to user  error %x",status);
                goto out;
        }
        status = copy_to_user(&(get_keypair->modulus_len),&ModulusSize_ptr,sizeof(int));
        if(status != 0) {
                LOGE("copy to user  error %x",status);
                goto out;
        }

        LOGD("after %d %d",get_keypair->exponent_len,get_keypair->modulus_len);
        kfree(Exponent_ptr);
        kfree(Modulus_ptr);
        Exponent_ptr = NULL;
        Modulus_ptr = NULL;
        return 0;
out:
        kfree(Exponent_ptr);
        kfree(Modulus_ptr);
        Exponent_ptr = NULL;
        Modulus_ptr = NULL;
        return status;
}

/*
 * The api for user space
 */
long km_ioctl(struct file *filp,unsigned int cmd, unsigned long arg)
{
        int status;
        int ret;
        if(NULL == (void *)arg) {
                LOGE("ioctl arg is null");
                return -1;
        }
        ret = dx_hw_reset();
        if(ret != 0) {
                LOGE("dx hw reset error %d",ret);
                return ret;
        }
        switch(cmd) {
        case  KM_IOC_GET_DATA:
                status = km_get_key((void *)arg);
                LOGD("KM_IOC_GET_DATA");
                break;
        case  KM_IOC_GEN_DATA:
                LOGD("KM_IOC_GEN_DATA");
                status = km_generate_key((void *)arg);
                break;
        case  KM_IOC_IMP_DATA:
                LOGD("KM_IOC_SIG_DATA");
                status = km_import_key((void *)arg);
                break;
        case  KM_IOC_SIG_DATA:
                LOGD("KM_IOC_SIG_DATA");
                status = km_sign_data((void *)arg);
                break;
        case  KM_IOC_VRY_DATA:
                LOGD("KM_IOC_VRY_DATA");
                status = km_verify_data((void *)arg);
                break;
        default:
                return -EINVAL;

        }
        return (long)status;
}

int km_open(struct inode *inode, struct file *filp)
{
        return 0;
}

static int km_release(struct inode *inode, struct file *filp)
{
        return 0;
}

static const struct file_operations km_fops = {
        .owner = THIS_MODULE,
        .open  = km_open,
        .unlocked_ioctl = km_ioctl,
        .release = km_release,
};

int km_dev_init(void)
{
        KM_MAJOR = register_chrdev(0,"kmdev",&km_fops);
        if(0 == KM_MAJOR) {
                LOGE("register chrdev error");
                return -1;
        }
        dev_class = class_create(THIS_MODULE,"kmdev");
        if(NULL == dev_class) {
                LOGE("class create failed");
                return -1;
        }
        dev_device = device_create(dev_class, NULL, MKDEV(KM_MAJOR,0), NULL, "%s", "kmdev");
        if(NULL == dev_device){
                LOGE("device create Error");
                return -1;
        }
        return 0;
}

int __init km_init(void)
{
        km_dev_init();
        return 0;
}

void __exit km_exit(void)
{
        device_destroy(dev_class, MKDEV(KM_MAJOR, 0));
        class_destroy(dev_class);
        unregister_chrdev(KM_MAJOR, "kmdev");
        DX_CclibFini();
        return;
}

module_init(km_init);
module_exit(km_exit);

