obj-y += hwcam_usrdev.o hwcam_cfgdev.o hwcam_cfgpipeline.o hwcam_cfgstream.o hwcam_compat32.o

obj-y += sensor/

ifeq ($(CONFIG_HUAWEI_CAMERA_USE_EXTISP),y)
	obj-y += extisp/ 
endif

obj-y += isp/ 
obj-y += cci/
obj-y += csi/
obj-y += io/
obj-y += pmic/
obj-y += platform/
