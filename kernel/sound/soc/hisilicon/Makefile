ifeq ($(CONFIG_HIFI_MAILBOX), y)
EXTRA_CFLAGS += -I$(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/
EXTRA_CFLAGS += -I$(BALONG_TOPDIR)/include/drv/
EXTRA_CFLAGS += -I$(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)/soc/
EXTRA_CFLAGS += -I$(srctree)/drivers/hisi/hifi_mailbox/mailbox
EXTRA_CFLAGS += -I$(srctree)/drivers/hisi/hifi_mailbox/ipcm
endif

# HI3630 Platform Support
snd-soc-hi3630-objs := hi3630_sio.o hi6210_pcm.o hi3630_srcup_dsp.o hi3630_asp_common.o hi3630_srcdown.o hi3630_fake_fm.o hi3630_fake_sio.o

ifeq ($(CONFIG_HDMI_K3), y)
snd-soc-hi3630-objs += hi3630_pcm_hdmi.o hi3630_hdmi_i2s.o
endif 
obj-$(CONFIG_SND_SOC_PCM_HI3630) += snd-soc-hi3630.o

# HI3630-HI6401 Machine Support
snd-soc-hi3630-hi6401-objs := hi3630_hi6401.o

# HI6210 Platform Support
snd-soc-hi6210-objs := hi6210_pcm.o
# HI6210-HISSC Machine Support
snd-soc-hi6210-hissc-objs := hi6210_hissc.o
obj-$(CONFIG_SND_HI3630_HI6401) += snd-soc-hi3630-hi6401.o
obj-$(CONFIG_SND_SOC_PCM_HI6210) += snd-soc-hi6210.o
obj-$(CONFIG_SND_HI6210_HISSC) += snd-soc-hi6210-hissc.o
# HI3630-HI6402 Machine Support
snd-soc-hi3630-hi6402-objs := hi3630_hi6402.o
snd-soc-hi3630-hi6402-objs += hi3630_hi6402es.o

obj-$(CONFIG_SND_HI3630_HI6402) += snd-soc-hi3630-hi6402.o
