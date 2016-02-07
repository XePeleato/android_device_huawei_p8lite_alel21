# Makefile for Discretix CC44P Linux device driver

MODULE_NAME = dx_cc44p

obj-$(CONFIG_CRYPTO_DEV_DX_CC44P) := $(MODULE_NAME).o
$(MODULE_NAME)-objs := dx_driver.o dx_sysfs.o dx_buffer_mgr.o dx_request_mgr.o dx_cipher.o dx_hash.o dx_aead.o dx_ivgen.o dx_sram_mgr.o


EXTRA_CFLAGS += -c -DLITTLE__ENDIAN -DHASLONGLONG
EXTRA_CFLAGS += -Wall

EXTRA_CFLAGS += -DDX_CC_REE #-DCOMP_IN_WQ
EXTRA_CFLAGS += -DDX_ROM_VERSION=$(CONFIG_DX_ROM_VERSION)
EXTRA_CFLAGS += -DDX_SEP_SRAM_SIZE=$(CONFIG_DX_SEP_SRAM_SIZE)
ifeq ($(CONFIG_DX_DISABLE_COHERENT_DMA_OPS),y)
EXTRA_CFLAGS += -DDISABLE_COHERENT_DMA_OPS
endif
ifeq ($(CONFIG_DX_FPGA_ENV),y)
EXTRA_CFLAGS += -DDX_FPGA_ENV
endif

ifeq ($(CONFIG_DX_DEBUG),y) # Remove optimization flags
EXTRA_CFLAGS += -DDX_DEBUG
define remove_o2
	CFLAGS_REMOVE_$(1) = -O2
endef
$(foreach obj,$(MODULE_NAME)-objs,$(eval $(call remove_o2,$(obj))))
EXTRA_CFLAGS += -DDEBUG -O1
endif
