#
# Makefile for hisilicon K3 soc camera v4l2 driver
# Author: j00140427
#
EXTRA_CFLAGS += -Idrivers/media/video/hik3/vpp

#KBUILD_CFLAGS := $(filter-out -Werror,$(KBUILD_CFLAGS))

obj-$(CONFIG_HIK3_VPP) += vpp.o
vpp-objs=vpp_drv.o vpp_inter.o vpp_hal.o

