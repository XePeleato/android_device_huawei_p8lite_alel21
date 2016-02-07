#
# Makefile for hisilicon camera flash driver
#
EXTRA_CFLAGS += -Idrivers/media/huawei/camera/sensor/flash
EXTRA_CFLAGS += -Idrivers/media/huawei/camera/
EXTRA_CFLAGS += -fno-pic

obj-$(CONFIG_HISI_CAMERA_FLASH) += hw_flash_i2c.o
obj-$(CONFIG_HISI_CAMERA_FLASH) += hw_flash.o

obj-$(CONFIG_HISI_CAMERA_TPS61310) += tps61310.o

obj-$(CONFIG_HISI_CAMERA_LM3642) += lm3642.o
obj-$(CONFIG_HISI_CAMERA_PMU_LED) += pmu_led.o
obj-$(CONFIG_HISI_CAMERA_LM3646) += lm3646.o
obj-$(CONFIG_HISI_CAMERA_LM3646_FRONT) += lm3646_front.o