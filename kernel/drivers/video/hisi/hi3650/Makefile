obj-$(CONFIG_FB_hi3650) := hisifb.o
#obj-m := hisifb.o

hisifb-objs := \
	hisi_mipi_dsi_host.o \
	hisi_mipi_dsi.o \
	hisi_pwm.o \
	hisi_blpwm.o \
	hisi_dpe.o \
	hisi_dpe_utils.o \
	hisi_fb_panel.o \
	hisi_fb_isr.o \
	hisi_fb_vsync.o \
	hisi_fb_buf_sync.o \
	hisi_fb_bl.o \
	hisi_fb_utils.o \
	hisi_fb.o \
	hisi_overlay_utils.o \
	hisi_overlay_online.o \

hisifb-objs += ../panel/mipi_fake_panel.o
hisifb-objs += ../panel/hdmi_fake_panel.o
hisifb-objs += ../panel/mipi_toshiba_MDY90.o
hisifb-objs += ../panel/mipi_jdi_OTM1902B.o
hisifb-objs += ../panel/mipi_jdi_NT35695.o

ifeq ($(CONFIG_ARCH_HI3630FPGA), y)
hisifb-objs += \
	hisi_rgb2mipi.o \
	hisi_fb_spi.o \
	../panel/lcdc_samsung_LMS350DF04.o \
	../panel/mipi_tianma_OTM9608A.o
else ifeq ($(CONFIG_HISI_3635_FPGA), y)
hisifb-objs += \
	hisi_rgb2mipi.o \
	hisi_fb_spi.o \
	../panel/lcdc_samsung_LMS350DF04.o \
	../panel/mipi_tianma_OTM9608A.o
else ifeq ($(CONFIG_HISI_3650_FPGA), y)
hisifb-objs += \
	hisi_rgb2mipi.o \
	hisi_fb_spi.o \
	../panel/lcdc_samsung_LMS350DF04.o \
	../panel/mipi_tianma_OTM9608A.o
else ifeq ($(CONFIG_HISI_3635_EMULATOR), y)
hisifb-objs += \
	../panel/lcdc_fake_panel.o
else ifeq ($(CONFIG_HISI_3650_EMULATOR), y)
hisifb-objs += \
	../panel/lcdc_fake_panel.o
else
hisifb-objs += voltage/tps65132.o
endif

ifeq ($(CONFIG_ARCH_HI3630FPGA), y)
EXTRA_CFLAGS += -Iarch/arm/mach-hisi
EXTRA_CFLAGS += -Idrivers/video/hisi/hi3630 \
		-Idrivers/video/hisi/panel
else ifeq ($(CONFIG_HISI_3635_FPGA), y)
EXTRA_CFLAGS += -Idrivers/video/hisi/hi3630 \
		-Idrivers/video/hisi/panel
else ifeq ($(CONFIG_HISI_3650_FPGA), y)
EXTRA_CFLAGS += -Idrivers/video/hisi/hi3650 \
		-Idrivers/video/hisi/panel

else ifeq ($(CONFIG_HISI_3635_EMULATOR), y)
EXTRA_CFLAGS += -Idrivers/video/hisi/hi3630 \
		-Idrivers/video/hisi/panel
else ifeq ($(CONFIG_HISI_3650_EMULATOR), y)
EXTRA_CFLAGS += -Idrivers/video/hisi/hi3650 \
		-Idrivers/video/hisi/panel

else ifeq ($(CONFIG_ARCH_HI3630), y)
EXTRA_CFLAGS += -Iarch/arm/mach-hisi
EXTRA_CFLAGS += -Idrivers/video/hisi/hi3630 \
		-Idrivers/video/hisi/panel
else ifeq ($(CONFIG_HISI_3635), y)
EXTRA_CFLAGS += -Idrivers/video/hisi/hi3630 \
		-Idrivers/video/hisi/panel
else ifeq ($(CONFIG_HISI_3650), y)
EXTRA_CFLAGS += -Idrivers/video/hisi/hi3650 \
		-Idrivers/video/hisi/panel
endif

clean:
	rm *.o .*cmd
