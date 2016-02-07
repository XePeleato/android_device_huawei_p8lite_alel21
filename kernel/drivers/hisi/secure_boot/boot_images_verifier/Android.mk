LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
CROSS_COMPLIE := prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.7/bin/arm-linux-androideabi-
LOCAL_CC :=  $(CROSS_COMPLIE)gcc

LOCAL_SRC_FILES := \
	bootimagesverifier.c \
	bootimagesverifier_base.c \
	bootimagesverifier_internal.c \
	bootimagesverifier_parser.c \
	bootimagesverifier_swvalidation_anddecryption.c
	
	
	
LOCAL_C_INCLUDES := $(call include-path-for, bootloader)

LOCAL_CFLAGS := -O2 -g -W -Wall 
LOCAL_CFLAGS += -march=armv7-a
LOCAL_CFLAGS += -DDX_SB_HASH_LENGTH_IN_WORDS=4 -DDX_SB_MAX_NUM_OF_OTP_KEYS=128 -DDX_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES=32768
LOCAL_CFLAGS += -I$(SOC_INC_PATH)

SECURE_BOOT_PATH := bootable/bootloader/legacy/dx_fw/codesafe/src/secure_boot
SECURE_SHARED_PATH := bootable/bootloader/legacy/dx_fw/shared
SECURE_HOST_PATH := bootable/bootloader/legacy/dx_fw/host
SECURE_CRYS_PATH := bootable/bootloader/legacy/dx_fw/codesafe/src/crys
LOCAL_CFLAGS += -I$(SECURE_BOOT_PATH)/boot_images_verifier
LOCAL_CFLAGS += -I$(SECURE_BOOT_PATH)/crypto_driver
LOCAL_CFLAGS += -I$(SECURE_BOOT_PATH)/gen
LOCAL_CFLAGS += -I$(SECURE_BOOT_PATH)/platform/nvm
LOCAL_CFLAGS += -I$(SECURE_BOOT_PATH)/platform/nvm/nvm_no_sep
LOCAL_CFLAGS += -I$(SECURE_BOOT_PATH)/secure_boot_gen
LOCAL_CFLAGS += -I$(SECURE_BOOT_PATH)/util
LOCAL_CFLAGS += -I$(SECURE_SHARED_PATH)/hw/include
LOCAL_CFLAGS += -I$(SECURE_SHARED_PATH)/include
LOCAL_CFLAGS += -I$(SECURE_SHARED_PATH)/include/pal
LOCAL_CFLAGS += -I$(SECURE_SHARED_PATH)/include/pal/linux
LOCAL_CFLAGS += -I$(SECURE_SHARED_PATH)/include/pal/no_os
LOCAL_CFLAGS += -I$(SECURE_HOST_PATH)/src/sbromlib
LOCAL_CFLAGS += -I$(SECURE_HOST_PATH)/include
LOCAL_CFLAGS += -I$(SECURE_HOST_PATH)/src/cclib
LOCAL_CFLAGS += -I$(SECURE_HOST_PATH)/src/cclib/tee
LOCAL_CFLAGS += -I$(SECURE_CRYS_PATH)/ccm/inc
LOCAL_CFLAGS += -I$(SECURE_CRYS_PATH)/common/inc
LOCAL_CFLAGS += -I$(SECURE_CRYS_PATH)/driver
LOCAL_CFLAGS += -I$(SECURE_CRYS_PATH)/ecc/crys_ecc/ecc_common/inc
LOCAL_CFLAGS += -I$(SECURE_CRYS_PATH)/ecc/llf_pki_ec/inc
LOCAL_CFLAGS += -I$(SECURE_CRYS_PATH)/gen/inc
LOCAL_CFLAGS += -I$(SECURE_CRYS_PATH)/rnd/crys_rnd/inc
LOCAL_CFLAGS += -I$(SECURE_CRYS_PATH)/rnd/llf_rnd/inc
LOCAL_CFLAGS += -I$(SECURE_CRYS_PATH)/rsa/crys_rsa/inc
LOCAL_CFLAGS += -I$(SECURE_CRYS_PATH)/rsa/llf_pki/inc
LOCAL_CFLAGS += -I$(SECURE_CRYS_PATH)/sym/api
LOCAL_CFLAGS += -I$(SECURE_CRYS_PATH)/sym/driver
LOCAL_CFLAGS += -I$(SECURE_CRYS_PATH)/

LOCAL_MODULE := libboot_boot_images_verifier
LOCAL_MODULE_TAGS := optional
include $(BUILD_RAW_STATIC_LIBRARY)
