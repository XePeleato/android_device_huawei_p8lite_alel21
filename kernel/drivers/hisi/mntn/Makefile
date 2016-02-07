ccflags-y += -I$(srctree)/arch/arm/mach-hi6210-legacy/include
ccflags-y += -I$(srctree)/arch/arm/include/
ccflags-y += -I$(srctree)/drivers/hisi/secure_boot/inc
ccflags-y += -I$(srctree)/drivers/hisi/ipc_hi6xxx
ccflags-y += -I$(srctree)/drivers/hisi/mailbox/hi6xxx_mailbox
ccflags-y += -I$(srctree)/drivers/hisi/ap/config/product/$(OBB_PRODUCT_NAME)/config

obj-$(CONFIG_HISILICON_PLATFORM_MAINTAIN)	+= hisilicon_platform_mntn.o
obj-$(CONFIG_HISILICON_PLATFORM_HISI_EASYSHELL)	+= hisi-easy-shell.o
ifdef CONFIG_ARCH_HI6XXX
ifndef CONFIG_ARM64
hisilicon_platform_mntn-y := \
	excDrv.o dump_info.o queue.o util.o dump.o debug_func.o \
	mntn.o reset.o reset_sub_hifi.o reset_hifiloader.o \
	hisi_dmesg.o fastbootlog.o reset_secure.o 

ifeq ($(strip $(CONFIG_ARCH_HI6XXX)), y)
ifeq ($(strip $(CONFIG_DEBUG_FS)), y)
hisilicon_platform_mntn-y += vstool_if.o
endif
endif
else
hisilicon_platform_mntn-y := \
	excDrv.o dump_info.o queue.o util.o dump.o debug_func.o \
	mntn.o reset.o reset_sub_hifi.o reset_hifiloader.o reset_secure.o fastbootlog.o hisi_dmesg.o \
	mntn_filesys.o mntn_save_logdata.o
endif
else
hisilicon_platform_mntn-y := debug_func.o util.o fastbootlog.o dump.o
endif
