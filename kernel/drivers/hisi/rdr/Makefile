
subdir-ccflags-y   			+= -I$(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/
subdir-ccflags-y   			+= -I$(BALONG_TOPDIR)/include/drv/
subdir-ccflags-y   			+= -I$(BALONG_TOPDIR)/modem/drv/common/include/
subdir-ccflags-y   			+= -I$(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)/soc/

obj-$(CONFIG_HISI_RDR) += rdr_control.o rdr_dump.o rdr_recorder.o rdr_field.o rdr_ringbuffer.o rdr_compress.o
obj-$(CONFIG_HISI_RDR_TEST) += rdr_test_unit.o rdr_ut.o
obj-$(CONFIG_HIFI_RESET) += rdr_hifi_reset.o
ifdef CONFIG_HIFI_RESET
EXTRA_CFLAGS += -D_HIFI_WD_DEBUG
endif

