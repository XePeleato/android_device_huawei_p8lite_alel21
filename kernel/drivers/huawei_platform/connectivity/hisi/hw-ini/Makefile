#Makefile for Hisilicon Wi-Fi Chip Hi1101

#PLATFORM_CFG=-DPLATFORM_TYPE=PLATFORM_K3
#ifeq ($(findstring ${PLATFORM_CFG},${KBUILD_CFLAGS}),${PLATFORM_CFG})
#EXTRA_CFLAGS += -DHISI_INI_EVB
#endif

EXTRA_CFLAGS += $(CFLAGS)  \
		   -DHi110X_VERSION_ROM \

EXTRA_CFLAGS  += -DHISI_GET_BOARD_ID

obj-y 				+= hw_ini.o
hw_ini-objs			:= hisi_ini.o  
