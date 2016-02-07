#
# Makefile for kernel secure_boot
#

SB_PATH := $(srctree)/drivers/hisi/secure_boot
EXTRA_CFLAGS += \
	     -DDX_SB_HASH_LENGTH_IN_WORDS=4 \
	     -DDX_SB_MAX_NUM_OF_OTP_KEYS=128 \
	     -DDX_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES=32768 \
	     -DDX_TST_HARDWARE_REG_PRINT_MODE=0 \
	     -DSEP_SUPPORT_SHA=0 \
	     -I$(SB_PATH)/boot_images_verifier \
         -I$(srctree)/include/linux/hisi \
             -I$(SB_PATH)/inc
EXTRA_CFLAGS += -DDX_SB_HASH_LENGTH_IN_WORDS=4 -DDX_SB_MAX_NUM_OF_OTP_KEYS=2 -DDX_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES=32768
EXTRA_CFLAGS += -DDX_SB_ADDITIONAL_DATA_SUPPORTED -DDX_SB_RAM_USAGE_ONLY -DDX_SB_INDIRECT_SRAM_ACCESS

obj-y += hisi_secureboot.o
hisi_secureboot-y :=  \
		  boot_images_verifier/bootimagesverifier.o \
		  boot_images_verifier/bootimagesverifier_base.o \
		  boot_images_verifier/bootimagesverifier_internal.o \
		  boot_images_verifier/bootimagesverifier_parser.o \
		  boot_images_verifier/bootimagesverifier_swvalidation_anddecryption.o \
		  crypto_driver/crypto_driver.o\
		  crypto_driver/crypto_driver_gen.o\
		  crypto_driver/dx_hal_sb.o\
		  crypto_driver/hash_sha2.o\
		  crypto_driver/llf_hash_sha2.o\
		  crypto_driver/rsa_exp.o\
		  crypto_driver/rsa_pki_pka.o\
		  crypto_driver/rsa_verify.o\
		  platform/nvm/nvm_no_sep/nvm_no_sep.o\
		  secure_boot_gen/secureboot_gen_no_sep.o\
		  util/util.o\
		  sw_revocation_manager/swrevocationmanager.o \
		  host/src/sbromlib/sbrom_management_api.o \
		  host/src/sbromlib/sbrom_crypto_driver.o

