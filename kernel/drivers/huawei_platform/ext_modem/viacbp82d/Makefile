ifeq ($(EVDO_VIA_HAVE_VOLT_PROTECTION), yes)#if we have voltage protection in design
ccflags-y += -DOEM_HAVE_VOLT_PROTECTION
endif

ccflags-y += -DWAKE_HOST_BY_SYNC

subdir-ccflags-y 			+= -I$(BALONG_TOPDIR)/include/taf/

obj-$(CONFIG_VIACBP82D_3RD_MODEM) += core.o power.o oem.o sync.o
obj-$(CONFIG_VIACBP82D_3RD_MODEM_SPI) += spi/
