# EXTRA_CFLAGS += -wformat
EXTRA_CFLAGS   			+=-Idrivers/hisi/ipc_hi6xxx/

obj-$(CONFIG_HISILICON_PLATFORM_PM_DFS) += hi6xxx-power-common.o hi6xxx-power-mca.o hi6xxx-power-dfs.o hi6xxx-power-debug.o pwrctrl_power_state_manger.o
obj-$(CONFIG_HISILICON_PLATFORM_PM_SLEEP) += hi6xxx-power-contorl.o 
hi6xxx-power-contorl-y += hi6xxx-core-pm.o hi6xxx-hotplug.o hi6xxx-pm.o hi6xxx-pm-asm.o 

ifneq ($(strip $(CONFIG_HISILICON_PLATFORM_PM_SLEEP)), y)
obj-$(CONFIG_HISILICON_PLATFORM_MAINTAIN) += hi6xxx-hotplug.o
endif

# EXTRA_CFLAGS -= -Wformat