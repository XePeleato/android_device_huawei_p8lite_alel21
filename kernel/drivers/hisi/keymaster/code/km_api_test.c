#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

#include "keymaster_hisi.h"
#include "crys_rsa_build.h"
#include "crys_rsa_schemes.h"
#include "crys_rsa_types.h"
#include "crys.h"
#include "crys_rsa_kg.h"
#include "crys_rsa_prim.h"
#include "dx_cclib.h"
#include "log.h"



#define WRITE_REGISTER_ULONG(addr,val) (*(volatile unsigned int *)(addr) = (val))
#define REG_BASE_SCTRL                  (0xF711A000)//SCTRL
#define SC_PERRSTDIS3                   (REG_BASE_SCTRL+0x0A8)
#define BIT_SYS_CryptCell_RST                       ( 0x1<<15 )
#define TST_NO_CRT       0
#define TST_CRT_MODE 1
#define TST_INTERNAL_KG         0
#define TST_NO_INTERNAL_KEYGEN  1


#define RSA_KEY_SIZE			4096
#define SC_PERCLKEN2                   (0xF711A000+0x618)
#define BIT_seceng_core_CLK                ( 0x1<<15 )
#define BIT_seceng_axi_CLK                 ( 0x1<<16 )

#define SC_PERRSTDIS1                   (0xF711A000+0x6D0)
#define SC_RSTDIS1			(0xF711A000+0x6cc)
#define BIT_seceng_n_RST                ( 0x1<<25 )
#define MOD_SIZE_IN_BYTES 				64
#define MAX_pri_table_NUM		50


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
static unsigned char input[2048] = {'h','e','l','l','o','w','o','r','l','d','!','!',0};
static unsigned char out[2048];
static unsigned char dec[2048];
static CRYS_RSAPrimeData_t tmp_prim_data;
static CRYS_RSAUserPrivKey_t tmp_private_data;
static CRYS_RSAUserPubKey_t tmp_public_data;
static CRYS_RSAKGData_t  tmp_kg_data;
static uint8_t E[] = {0x01,0x00,0x01};
static int init_status = 0;

int dx_hw_reset(void)
{
	int ret;
        unsigned int dwLoopCount = 0;
	if(init_status != 0)
		return 0;
        WRITE_REGISTER_ULONG(SC_PERCLKEN2,
                             BIT_seceng_core_CLK |
                             BIT_seceng_axi_CLK );
	mdelay(10);

        /* reset */
        WRITE_REGISTER_ULONG(SC_RSTDIS1,
                             BIT_seceng_n_RST );

	mdelay(10);
        WRITE_REGISTER_ULONG(SC_PERRSTDIS1,
                             BIT_seceng_n_RST );
        mdelay(10);
	ret = DX_CclibInit();
	if(ret == 0)
	{
		init_status = 1;	
	}
        LOGE("dx cclib init %x",ret);
        return ret;
}

int km_api_test(void)
{
        int ret;
	int status;
        LOGD("here into thread km_api_test function");
        ret = dx_hw_reset();

        LOGD("init status %08x",ret);

        memset(&tmp_private_data,0,sizeof(tmp_private_data));
        memset(&tmp_public_data,0,sizeof(tmp_public_data));
        memset(&tmp_kg_data,0,sizeof(tmp_kg_data));
        status = CRYS_RSA_KG_GenerateKeyPairCRT(E,sizeof(E),2048,&tmp_private_data,
                        &tmp_public_data,&tmp_kg_data);
        LOGD("generate key pair status %x",status);


        LOGD("here goto sign data is %s",input);
        status = CRYS_RSA_PRIM_Sign(&tmp_private_data,&tmp_prim_data,input,256,out);
        //modulus size
        LOGD("sign status %d",status);

        memset(dec,0,512);
        status = CRYS_RSA_PRIM_Verify(&tmp_public_data,&tmp_prim_data,out,256,dec);
        LOGD("verify status %d",status);

        LOGD("verify data [%s]",dec);

        return 0;
}

int __init km_init(void)
{
	km_api_test();
        return 0;
}

void __exit km_exit(void)
{
        return;
}

module_init(km_init);
module_exit(km_exit);

