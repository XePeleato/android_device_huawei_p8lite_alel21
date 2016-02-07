obj-$(CONFIG_FB_3630) := hisifb.o
obj-$(CONFIG_FB_3635) := hisifb.o
#obj-m := hisifb.o

hisifb-objs := \
	hisi_mipi_dsi_host.o \
	hisi_mipi_dsi.o \
	hisi_rgb2mipi.o \
	hisi_fb_spi.o \
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
	hisi_fb_sysfs_utils.o \
	hisi_fb.o \
	hisi_overlay_utils.o \
	hisi_overlay_online.o \
	hisi_overlay_offline.o \
	hisi_block_algorithm.o \
	hisi_ov_cmd_list_utils.o \
	hisi_offlinecomposer_panel.o

hisifb-objs += ../panel/lcdc_fake_panel.o
hisifb-objs += ../panel/mipi_fake_panel.o
hisifb-objs += ../panel/hdmi_fake_panel.o
hisifb-objs += ../panel/lcdc_samsung_LMS350DF04.o
hisifb-objs += ../panel/mipi_tianma_OTM9608A.o
hisifb-objs += ../panel/mipi_toshiba_MDY90.o
hisifb-objs += ../panel/mipi_jdi_OTM1902B.o
hisifb-objs += ../panel/mipi_jdi_R69429.o
hisifb-objs += ../panel/mipi_jdi_NT35695.o
hisifb-objs += ../panel/mipi_jdi_NT36750.o
hisifb-objs += ../panel/mipi_auo_NT51021.o
hisifb-objs += ../panel/mipi_samsung_S6E3FA3X01.o
hisifb-objs += voltage/tps65132.o
hisifb-objs += voltage/ina231.o


ifeq ($(CONFIG_FB_3630), y)
EXTRA_CFLAGS += -Iarch/arm/mach-hisi
EXTRA_CFLAGS += -Idrivers/video/hisi/hi3630 \
		-Idrivers/video/hisi/panel
else ifeq ($(CONFIG_FB_3635), y)
EXTRA_CFLAGS += -Idrivers/video/hisi/hi3630 \
		-Idrivers/video/hisi/panel
else
EXTRA_CFLAGS += -Idrivers/video/hisi/hi3630 \
		-Idrivers/video/hisi/panel
endif


clean:
	rm *.o .*cmd
