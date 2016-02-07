pasr-objs := helper.o init.o core.o

obj-$(CONFIG_PASR) += pasr.o
obj-$(CONFIG_HI6210_PASR) += hi6210.o
obj-$(CONFIG_HI3635_PASR) += hi3635.o

ccflags-$(CONFIG_PASR_DEBUG) := -DDEBUG
