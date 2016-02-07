obj-$(CONFIG_HDMI_K3):= hdmi.o
hdmi-objs := k3_hdmi_hw_tpi.o k3_edid.o k3_hdcp.o k3_hdmi.o k3_cec.o k3_mhl_tx.o k3_drv_mhl_tx.o

EXTRA_CFLAGS += -Idrivers/video/k3 -Idrivers/input/touchscreen/k3_ts_cts -Idrivers/tzdriver
EXTRA_CFLAGS += -Iarch/arm/mach-hisi

ifeq ($(CONFIG_MHL_K3),y)
EXTRA_CFLAGS += -DMHL_SUPPORT=1
endif

ifeq ($(CONFIG_HDCP_ENABLE_K3),y)
EXTRA_CFLAGS += -DUSE_HDCP=1
endif

ifeq ($(TARGET_PRODUCT),MediaPad)
EXTRA_CFLAGS += -DHDMI_MAX_VESA_TIMING_CODE=127
endif

ifeq ($(TARGET_BUILD_VARIANT),eng)
EXTRA_CFLAGS += -DENABLE_REG_DUMP=1
endif
