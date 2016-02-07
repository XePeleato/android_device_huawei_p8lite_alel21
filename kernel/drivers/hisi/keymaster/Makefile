#
# Makefile for kernel keymaster
#
KM_PATH := $(srctree)/drivers/hisi/keymaster
EXTRA_CFLAGS += \
	-DFW_VER_MAJOR=1 \
	-DFW_VER_MINOR=0 \
	-DFW_VER_PATCH=1 \
	-DDX_CC_TEE \
	-DMAX_NUM_HW_QUEUES=1 \
	-DDLLI_MAX_BUFF_SIZE=128 \
	-DFW_MLLI_TABLE_LEN=128 \
	-DSEP_SUPPORT_SHA=512 \
	-DMIN_CRYPTO_TAIL_SIZE=32 \
	-DENABLE_AES_DRIVER=1 \
	-DENABLE_DES_DRIVER=1 \
	-DENABLE_HASH_DRIVER=1 \
	-DENABLE_HMAC_DRIVER=1 \
	-DENABLE_RC4_DRIVER=1 \
	-DENABLE_AEAD_DRIVER=1 \
	-DENABLE_ECC_DRIVER=1 \
	-DENABLE_RSA_DRIVER=1 \
	-DENABLE_BYPASS_DRIVER=1 \
	-DENABLE_COMBINED_DRIVER=1 \
	-DENABLE_KDF_DH_DRIVER=1 \
	-DCRYS_NO_FIPS_SUPPORT \
	-DCRYS_NO_RSA_SELF_TEST_SUPPORT \
	-DCRYS_RND_SEM_DISABLE \
	-DCRYS_PKA_INDIRECT_SRAM_ACCESS_MODE \
	-DDX_CONFIG_HASH_SHA_512_SUPPORTED \
	-DSEP_CTX_SIZE_LOG2=8 \
	-DLLF_RND_HW_DMA_SRAM_ADDRESS=0x1800 \
	-DDX_MLLI_SRAM_BASE_ADDR=0x2000 \
	-DDX_DRIVER_ADAPTOR_CONTEXT_CACHE_ADDR=0x3c00 \
	-DDX_DRIVER_ADAPTOR_CONTEXT_CACHE_LAST_WORD_ADDR=0x3FFC\
	-DLITTLE__ENDIAN \
	-DHASLONGLONG \
	-DDX_CC_SRAM_INDIRECT_ACCESS \
	-I$(KM_PATH)/include


		#-DLLF_RND_HW_DMA_SRAM_ADDRESS=0xFFF22000 \
		-DDX_MLLI_SRAM_BASE_ADDR=0xFFF23000 \
		-DDX_DRIVER_ADAPTOR_CONTEXT_CACHE_ADDR=0xFFF24000 \
		-DDX_DRIVER_ADAPTOR_CONTEXT_CACHE_LAST_WORD_ADDR=0xFFF25000\


obj-$(CONFIG_HISILICON_PLATFORM_KEYMASTER) += hisi_keymaster.o

hisi_keymaster-y :=  \
	code/keymaster_hisi_api.o \
	code/crys_ccm.o \
	code/dx_rng_plat.o\
	code/crys_rsa_sign.o\
	code/crys_ecpki_kg.o\
	code/crys_ecpki_build.o\
	code/crys_rsa_kg.o\
	code/llf_ecpki_modular_arithmetic.o\
	code/crys_aes.o\
	code/llf_pki_genkey.o\
	code/dx_cclib.o\
	code/crys_rsa_pss21_util.o\
	code/cc_plat.o\
	code/dx_asym_init.o\
	code/hash.o\
	code/mlli_plat.o\
	code/llf_pki_rsa.o\
	code/dx_pal_dma.o\
	code/bypass.o\
	code/crys_combined.o\
	code/validate_crys_bypass_plat.o\
	code/crys_context_relocation.o\
	code/crys_hmac.o\
	code/dma_object.o\
	code/crys_aes_wrap.o\
	code/dx_pal.o\
	code/rc4.o\
	code/crys_common_math.o\
	code/crys_aesccm.o\
	code/crys_bypass.o\
	code/sym_adaptor_driver.o\
	code/crys_rsa_build.o\
	code/crys_common_conv_endian.o\
	code/cc_acl_plat.o\
	code/hmac.o\
	code/dma_buffer.o\
	code/crys_ecdsa_sign.o\
	code/crys_dh_kg.o\
	code/sym_crypto_driver.o\
	code/crys_rc4.o\
	code/llf_rnd.o\
	code/crys_rsa_pkcs_ver15_util.o\
	code/llf_pki_exp.o\
	code/crys_dh.o\
	code/crys_ecdsa_verify.o\
	code/combined.o\
	code/key_object.o\
	code/dx_hal.o\
	code/completion_plat.o\
	code/hw_queue.o\
	code/crys_des.o\
	code/crys_rsa_oaep.o\
	code/crys_kdf.o\
	code/crys_rnd.o\
	code/dx_vos_sem.o\
	code/hw_queue_plat.o\
	code/llf_ecpki_ec_arithmetic.o\
	code/aead.o\
	code/llf_pki_genkey_x931_find_prime.o\
	code/crys_rsa_prim.o\
	code/llf_ecpki_ecdsa.o\
	code/crys_ecdh.o\
	code/llf_ecpki_export.o\
	code/llf_ecpki_version.o\
	code/mlli.o\
	code/cipher.o\
	code/llf_pki_pka.o\
	code/dx_util.o\
	code/key_buffer.o\
	code/llf_ecpki_domains.o\
	code/crys_rsa_verify.o\
	code/llf_ecpki_genkey.o\
	code/crys_hash.o 
	
