# hifi dsp device driver

ifeq ($(CONFIG_SEATTLE),y)
EXTRA_CFLAGS += -DPLATFORM_HI3XXX
endif
ifeq ($(CONFIG_HISILICON_PLATFORM_HI6XXX),y)
EXTRA_CFLAGS += -DPLATFORM_HI6XXX
endif

obj-$(CONFIG_HISILICON_PLATFORM_HIFI_DSP)	+= hifi_lpp.o
obj-$(CONFIG_HISILICON_PLATFORM_HIFI_DSP)	+= hifi_om.o

ifeq ($(CONFIG_SEATTLE),y)
EXTRA_CFLAGS += -I$(BALONG_TOPDIR)/include/drv/
EXTRA_CFLAGS += -I$(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/
EXTRA_CFLAGS += -I$(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)/soc/
EXTRA_CFLAGS += -Idrivers/hisi/hifi_mailbox/mailbox
EXTRA_CFLAGS += -Idrivers/hisi/hifi_mailbox/ipcm
endif

