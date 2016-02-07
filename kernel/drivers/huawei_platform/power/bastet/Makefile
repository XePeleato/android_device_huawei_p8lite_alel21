#
# Makefile for the Bastet.
#
subdir-ccflags-y        += -I$(BALONG_TOPDIR)/include/drv/
subdir-ccflags-y        += -I$(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/
subdir-ccflags-y        += -I$(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/include_gu/
subdir-ccflags-y        += -I$(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)/
subdir-ccflags-y        += -I$(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)/soc/
subdir-ccflags-y        += -I$(BALONG_TOPDIR)/modem/drv/common/include/
EXTRA_CFLAGS += -Inet/netfilter

obj-$(CONFIG_HUAWEI_BASTET) += bastet_sync_seq.o bastet_traffic_flow.o bastet_dev.o bastet_utils.o

ifeq ($(OBB_PRODUCT_NAME), hi3635_udp)
BALONG_BASE_DIR := $(BALONG_TOPDIR)
BALONG_TAF_DIR := $(srctree)/drivers/hisi/modem_hi6xxx/modem/taf
else ifeq ($(OBB_PRODUCT_NAME), hi6210sft)
BALONG_BASE_DIR := $(BALONG_TOPDIR)/modem
BALONG_TAF_DIR := $(srctree)/drivers/hisi/modem_hi6xxx/taf
endif

-include $(srctree)/drivers/hisi/modem_hi6xxx/config/product/$(OBB_PRODUCT_NAME)/config/balong_product_config.mk

ifeq ($(CFG_FEATURE_BASTET), FEATURE_ON)
ifeq ($(CONFIG_BASTET_BALONG_COMM), y)

ifeq ($(TARGET_ARM_TYPE), arm64)
EXTRA_CFLAGS += -Iarch/arm64/include/asm
else
EXTRA_CFLAGS += -Iarch/arm/include/asm
endif

EXTRA_CFLAGS += -Iinclude/linux
EXTRA_CFLAGS += -Iinclude/generated
EXTRA_CFLAGS += -I$(srctree)/drivers/hisi/modem_hi6xxx/include/med/
EXTRA_CFLAGS += -I$(srctree)/drivers/hisi/modem_hi6xxx/include/nv/gu/codec/
EXTRA_CFLAGS += -I$(srctree)/drivers/hisi/modem_hi6xxx/include/nv/gu/nas/
EXTRA_CFLAGS += -I$(srctree)/drivers/hisi/modem_hi6xxx/include/oam/gu/log/
EXTRA_CFLAGS += -I$(srctree)/drivers/hisi/modem_hi6xxx/include/oam/gu/osa/
EXTRA_CFLAGS += -I$(srctree)/drivers/hisi/modem_hi6xxx/include/oam/comm/om/
EXTRA_CFLAGS += -I$(srctree)/drivers/hisi/modem_hi6xxx/include/oam/comm/osa/
EXTRA_CFLAGS += -I$(srctree)/drivers/hisi/modem_hi6xxx/include/ps/tlps/
EXTRA_CFLAGS += -I$(srctree)/drivers/hisi/modem_hi6xxx/include/ps/gups/
EXTRA_CFLAGS += -I$(srctree)/drivers/hisi/modem_hi6xxx/include/taf/
EXTRA_CFLAGS += -I$(srctree)/drivers/hisi/modem_hi6xxx/config/osa/
EXTRA_CFLAGS += -I$(srctree)/drivers/hisi/modem_hi6xxx/config/product/$(OBB_PRODUCT_NAME)/config/
EXTRA_CFLAGS += -I$(BALONG_TAF_DIR)/comm/src/acore/bastet/inc
EXTRA_CFLAGS += -I$(BALONG_TAF_DIR)/comm/src/acore/RNIC/Inc/

ifneq ($(TARGET_ARM_TYPE), arm64)
EXTRA_CFLAGS += -D__LINUX_ARM_ARCH__=7 -march=armv7-a -Uarm
endif

EXTRA_CFLAGS +=  -DVOS_LINUX=4 -DOSA_CPU_ACPU=1 -DVOS_OS_VER=VOS_LINUX -DVOS_OSA_CPU=OSA_CPU_ACPU -D__LOG_RELEASE__ \
				-DBSP_CORE_APP -DGU_MODE -DMSP_SUPPORT_USB_VCOM1 -DMSP_SUPPORT_USB_VCOM2 -DVOS_DOPRA_VER=2 -DNDIS_RELEASE \
				-DBALONG_PHY_V200=2 -DBALONG_PHY_V300=3 -DBALONG_PHY_VER=BALONG_PHY_V300 -DBALONG_CHIP_V200=2 \
				-DBALONG_CHIP_V300=3 -DBALONG_CHIP_V500=4 -DBALONG_CHIP_VER=BALONG_CHIP_V500 -DMSP_GUNAS_AT_UNITE -DVERSION_V3R2 

EXTRA_CFLAGS += -DCONFIG_HUAWEI_BASTET_COMM

EXTRA_CFLAGS += -fsigned-char

obj-y += bastet_comm.o

endif
endif
