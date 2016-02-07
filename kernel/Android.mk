#Android makefile to build kernel as a part of Android Build
ifeq ($(BALONG_TOPDIR),)
export BALONG_TOPDIR := $(shell pwd)/vendor/hisi
endif

ifeq ($(OBB_PRODUCT_NAME),)
export OBB_PRODUCT_NAME := $(HISI_TARGET_PRODUCT)
endif

HISI_3635_MODEM_DEFCONFIG := vendor/hisi/config/product/$(OBB_PRODUCT_NAME)/os/acore/hi6930_modem_defconfig
ifeq ($(strip $(TARGET_BOARD_PLATFORM)), hi3635)
HISI_3635_MODEM_DEFCONFIG := vendor/hisi/config/product/$(OBB_PRODUCT_NAME)/os/acore/hi3635_modem_defconfig
endif

ifeq ($(OBB_PRINT_CMD), true)
KERNEL_OUT := vendor/hisi/build/delivery/$(OBB_PRODUCT_NAME)/obj/android
else
KERNEL_OUT := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ
endif
KERNEL_CONFIG := $(KERNEL_OUT)/.config

ifeq ($(TARGET_ARM_TYPE), arm64)
KERNEL_ARCH_PREFIX := arm64
CROSS_COMPILE_PREFIX=aarch64-linux-android-
TARGET_PREBUILT_KERNEL := $(KERNEL_OUT)/arch/arm64/boot/Image
else
KERNEL_ARCH_PREFIX := arm
CROSS_COMPILE_PREFIX=arm-linux-gnueabihf-
TARGET_PREBUILT_KERNEL := $(KERNEL_OUT)/arch/arm/boot/zImage
endif

COMMON_HEAD := $(shell pwd)/kernel/drivers/
COMMON_HEAD += $(shell pwd)/kernel/mm/
COMMON_HEAD += $(shell pwd)/kernel/include/hisi/
COMMON_HEAD += $(shell pwd)/vendor/hisi/ap/config/product/$(OBB_PRODUCT_NAME)/include/
COMMON_HEAD += $(shell pwd)/external/efipartition	

ifeq ($(HISI_TARGET_PRODUCT), hi6210sft)
COMMON_HEAD += $(shell pwd)/vendor/hisi/ap/platform/hi6210sft/
endif

ifneq ($(COMMON_HEAD),)
BALONG_INC := $(patsubst %,-I%,$(COMMON_HEAD))
else
BALONG_INC := 
endif
 
export BALONG_INC

KERNEL_N_TARGET ?= vmlinux
UT_EXTRA_CONFIG ?= 

KERNEL_ARCH_ARM_CONFIGS := $(shell pwd)/kernel/arch/$(KERNEL_ARCH_PREFIX)/configs
KERNEL_GEN_CONFIG_FILE := hw_$(TARGET_PRODUCT)_defconfig
KERNEL_GEN_CONFIG_PATH := $(KERNEL_ARCH_ARM_CONFIGS)/$(KERNEL_GEN_CONFIG_FILE)

KERNEL_COMMON_DEFCONFIG := $(KERNEL_ARCH_ARM_CONFIGS)/$(KERNEL_DEFCONFIG)
KERNEL_PRODUCT_CONFIGS := $(shell pwd)/device/hisi/customize/config/${TARGET_ARM_TYPE}/$(TARGET_DEVICE)/${TARGET_PRODUCT}/product_config/kernel_config

ifeq ($(strip $(TARGET_PRODUCT)), hisi_pilot)
KERNEL_PRODUCT_CONFIGS := $(shell pwd)/device/hisi/customize/config/${TARGET_ARM_TYPE}/hi6220/${TARGET_PRODUCT}/product_config/kernel_config
endif

.PHONY: $(GPIO_CONFIG_TARGET)
$(GPIO_CONFIG_TARGET) : FORCE

DTS_PARSE_CONFIG:
	@-rm -f $(PRODUCT_OUT)/dt.img
	@-rm -rf $(KERNEL_OUT)/arch/$(KERNEL_ARCH_PREFIX)/boot/dts
	@cd device/hisi/customize/build_script; ./auto_dts_gen.sh

HI3XXX_MODEM_DIR := $(shell pwd)/vendor/hisi/modem/drv/acore/kernel/drivers/modem
ifeq ($(wildcard $(HI3XXX_MODEM_DIR)),)
$(HI3XXX_MODEM_DIR):
	@mkdir -p $(HI3XXX_MODEM_DIR)
	@touch $(HI3XXX_MODEM_DIR)/Makefile
	@touch $(HI3XXX_MODEM_DIR)/Kconfig
endif

HI6XXX_MODEM_DRV_DIR := $(shell pwd)/vendor/hisi/modem/drv820/common/src/acore
ifeq ($(wildcard $(HI6XXX_MODEM_DRV_DIR)),)
$(HI6XXX_MODEM_DRV_DIR):
	@mkdir -p $(HI6XXX_MODEM_DRV_DIR)
	@touch $(HI6XXX_MODEM_DRV_DIR)/Makefile
	@touch $(HI6XXX_MODEM_DRV_DIR)/Kconfig
endif

HISI_PILOT_KERNEL_DIR := $(shell pwd)/kernel/drivers/hisi_pilot/

$(KERNEL_GEN_CONFIG_PATH): FORCE
	+$(shell $(shell pwd)/device/hisi/customize/build_script/kernel-config.sh -f $(KERNEL_COMMON_DEFCONFIG) -d $(KERNEL_PRODUCT_CONFIGS) -o $(KERNEL_GEN_CONFIG_PATH))
ifneq ($(filter hi3635fpga hi3635 hi3635emulator, $(TARGET_BOARD_PLATFORM)),)
ifeq ($(strip $(CFG_HISI_MINI_AP)), false)
	cat $(HISI_3635_MODEM_DEFCONFIG) >> $(KERNEL_GEN_CONFIG_PATH)
	echo     >> $(KERNEL_GEN_CONFIG_PATH)
	echo "CONFIG_HISI_BALONG_MODEM_HI3XXX=y" >> $(KERNEL_GEN_CONFIG_PATH)
endif
endif

ifeq ($(OBB_PRINT_CMD), true)
$(KERNEL_CONFIG): MAKEFLAGS :=
$(KERNEL_CONFIG):
	+mkdir -p $(KERNEL_OUT)
	$(MAKE) -C kernel O=../$(KERNEL_OUT) ARCH=$(KERNEL_ARCH_PREFIX) CROSS_COMPILE=$(CROSS_COMPILE_PREFIX) hisi_$(TARGET_PRODUCT)_defconfig
	+rm -frv $(KERNEL_GEN_CONFIG_PATH)
else
ifeq ($(HISI_PILOT_LIBS), true)
$(KERNEL_CONFIG): $(KERNEL_GEN_CONFIG_PATH) $(HI3XXX_MODEM_DIR) $(HI6XXX_MODEM_DRV_DIR) HISI_PILOT_PREBUILD
else
$(KERNEL_CONFIG): $(KERNEL_GEN_CONFIG_PATH) $(HI3XXX_MODEM_DIR) $(HI6XXX_MODEM_DRV_DIR)
endif
	mkdir -p $(KERNEL_OUT)
	$(MAKE) -C kernel O=../$(KERNEL_OUT) ARCH=$(KERNEL_ARCH_PREFIX) CROSS_COMPILE=$(CROSS_COMPILE_PREFIX) $(KERNEL_GEN_CONFIG_FILE)
	@rm -frv $(KERNEL_GEN_CONFIG_PATH)
endif

ifneq ($(filter hi3635fpga hi3635 hi3635emulator, $(TARGET_BOARD_PLATFORM)),)
$(TARGET_PREBUILT_KERNEL): $(KERNEL_CONFIG) DTS_PARSE_CONFIG
else
$(TARGET_PREBUILT_KERNEL): $(GPIO_CONFIG_TARGET) $(KERNEL_CONFIG) DTS_PARSE_CONFIG
endif
ifeq ($(OBB_PRINT_CMD), true)
	$(hide) $(MAKE) -C kernel O=../$(KERNEL_OUT) ARCH=$(KERNEL_ARCH_PREFIX) CROSS_COMPILE=$(CROSS_COMPILE_PREFIX) $(KERNEL_N_TARGET) $(UT_EXTRA_CONFIG)
	touch $@
else
	$(hide) $(MAKE) -C kernel O=../$(KERNEL_OUT) ARCH=$(KERNEL_ARCH_PREFIX) CROSS_COMPILE=$(CROSS_COMPILE_PREFIX)
endif

HISI_PILOT_PREBUILD:
	$(hide) rm -rf kernel/include/huawei_platform
	$(hide) rm -rf kernel/include/modem
	$(hide) rm -rf kernel/drivers/huawei_platform
	$(hide) rm -rf kernel/drivers/huawei_platform_legacy
	$(hide) rm -rf kernel/drivers/hisi/modem_hi6xxx
	$(hide) rm -rf kernel/drivers/device-depend-arm64
	$(hide) cp -rf $(HISI_PILOT_TOPDIR)kernel/include/huawei_platform kernel/include/.
	$(hide) cp -rf $(HISI_PILOT_TOPDIR)kernel/drivers/huawei_platform kernel/drivers/
	$(hide) cp -rf $(HISI_PILOT_TOPDIR)kernel/drivers/huawei_platform_legacy kernel/drivers/
	$(hide) cp -rf $(HISI_PILOT_TOPDIR)kernel/drivers/hisi/modem_hi6xxx kernel/drivers/hisi/.
	$(hide) cp $(HISI_PILOT_KERNEL_DIR)/Makefile.pilot $(HISI_PILOT_KERNEL_DIR)/Makefile
	$(hide) cp $(HISI_PILOT_KERNEL_DIR)/Kconfig.pilot $(HISI_PILOT_KERNEL_DIR)/Kconfig
ifeq ($(TARGET_VERSION_MODE),factory)
	$(hide) cd kernel/drivers/; ln -s ../../vendor/hisi_pilot/libs_factory/kernel/drivers/device-depend-arm64 device-depend-arm64
else
	$(hide) cd kernel/drivers/; ln -s ../../vendor/hisi_pilot/libs/kernel/drivers/device-depend-arm64 device-depend-arm64
endif

kernelconfig: $(KERNEL_GEN_CONFIG_PATH)
	mkdir -p $(KERNEL_OUT)
	$(MAKE) -C kernel O=../$(KERNEL_OUT) ARCH=$(KERNEL_ARCH_PREFIX) CROSS_COMPILE=$(CROSS_COMPILE_PREFIX) $(KERNEL_GEN_CONFIG_FILE) menuconfig
	@rm -frv $(KERNEL_GEN_CONFIG_PATH)



zImage Image:$(TARGET_PREBUILT_KERNEL)
	@mkdir -p $(dir $(INSTALLED_KERNEL_TARGET))
	@cp -fp $(TARGET_PREBUILT_KERNEL) $(INSTALLED_KERNEL_TARGET)
pclint_kernel: $(KERNEL_CONFIG)
	$(hide) $(MAKE) -C kernel O=../$(KERNEL_OUT) ARCH=$(KERNEL_ARCH_PREFIX) CROSS_COMPILE=$(CROSS_COMPILE_PREFIX) pc_lint_all
