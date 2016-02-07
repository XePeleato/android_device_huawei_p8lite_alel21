ifeq ($(EVDO_VIA_HAVE_VOLT_PROTECTION), yes)#if we have voltage protection in design
ccflags-y += -DOEM_HAVE_VOLT_PROTECTION
endif

ccflags-y += -DWAKE_HOST_BY_SYNC
obj-$(CONFIG_VIACBP82D_3RD_MODEM_SPI) += cbp_spi.o modem_spi.o
obj-$(CONFIG_HISI_BALONG_MODEM) += rnic_adapter.o
