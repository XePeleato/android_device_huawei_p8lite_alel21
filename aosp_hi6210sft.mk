#
# Copyright 2014 The Android Open-Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
$(call inherit-product, device/HUAWEI/hi6210sft/device.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base.mk)
# Sample: This is where we'd set a backup provider if we had one
# $(call inherit-product, device/sample/products/backup_overlay.mk)

TARGET_SCREEN_HEIGHT := 1280
TARGET_SCREEN_WIDTH := 720

# Copying some libs in order to get it working Quit: Libbinder, liblog

PRODUCT_COPY_FILES := \
		       vendor/bin/atcmdserver:system/bin/atcmdsercer\
		       vendor/bin/device_monitor:system/bin/device_monitor\
		       vendor/bin/gpsdaemon:system/bin/gpsdaemon\
		       vendor/bin/gpslogd:system/bin/gpslogd\
		       vendor/bin/hdb:system/bin/hdb\
		       vendor/bin/hwnff:system/bin/hwnff\
		       vendor/bin/hwnffserver:system/bin/hwnffserver\
		       vendor/bin/hwpged:system/bin/hwpged\
		       vendor/bin/hw_ueventd:system/bin/hw_ueventd\
		       vendor/bin/mac_addr_normalization:system/bin/mac_addr_normalization\
		       vendor/bin/mediaserver:system/bin/mediaserver\
		       vendor/bin/rild:system/bin/thermal-daemon\
		       vendor/mali/64bit/libGLES_mali.so:system/lib64/egl/libGLES_mali.so\
		       vendor/mali/32bit/libGLES_mali.so:system/lib/egl/libGLES_mali.so\
		       vendor/lib64/libutils.so:system/lib64/libutils.so\
		       vendor/lib64/hw/gralloc.hi6210sft.so:system/lib64/hw/gralloc.hi6210sft.so\
		       vendor/lib/hw/gralloc.hi6210sft.so:system/lib/hw/gralloc.hi6210sft.so\
		       vendor/lib/hw/hwcomposer.default.so:system/lib/hw/hwcomposer.default.so\
		       vendor/lib64/hw/hwcomposer.default.so:system/lib64/hw/hwcomposer.default.so\
		       vendor/lib64/hw/hwcomposer.hi6210sft.so:system/lib64/hw/hwcomposer.hi6210sft.so\
		       vendor/lib/hw/hwcomposer.hi6210sft.so:system/lib/hw/hwcomposer.hi6210sft.so\
                       vendor/lib64/hw/memtrack.hi6210sft.so:system/lib64/hw/memtrack.hi6210sft.so\
                       vendor/lib/hw/memtrack.hi6210sft.so:system/lib/hw/memtrack.hi6210sft.so\
                       vendor/lib64/libGLES_android.so:system/lib64/egl/libGLES_android.so\
                       vendor/lib/libGLES_android.so:system/lib/egl/libGLES_android.so\
                       vendor/lib64/libEGL.so:system/lib64/libEGL.so\
                       vendor/lib/libEGL.so:system/lib/libEGL.so\
                       vendor/lib64/libGLES_trace.so:system/lib64/libGLES_trace.so\
                       vendor/lib/libGLES_trace.so:system/lib/libGLES_trace.so\
                       vendor/lib64/libGLESv1_CM.so:system/lib64/libGLESv1_CM.so\
                       vendor/lib/libGLESv1_CM.so:system/lib/libGLESv1_CM.so\
                       vendor/lib64/libGLESv2.so:system/lib64/libGLESv2.so\
                       vendor/lib/libGLESv2.so:system/lib/libGLESv2.so\
                       vendor/lib64/libGLESv3.so:system/lib64/libGLESv3.so\
                       vendor/lib/libGLESv3.so:system/lib/libGLESv3.so\
                       vendor/lib64/libhardware.so:system/lib64/libhardware.so\
                       vendor/lib/libhardware.so:system/lib/libhardware.so\
                       vendor/lib64/libui.so:system/lib64/libui.so\
                       vendor/lib/libui.so:system/lib/libui.so\
                       vendor/lib64/lib_hwnsd_sf.so:system/lib64/lib_hwnsd_sf.so\
                       vendor/lib/lib_hwnsd_sf.so:system/lib/lib_hwnsd_sf.so\
                       vendor/lib64/libgui.so:system/lib64/libgui.so\
                       vendor/lib/libgui.so:system/lib/libgui.so\
                       vendor/lib64/libhwaps.so:system/lib64/libhwaps.so\
			vendor/lib/libhwaps.so:system/lib/libhwaps.so\
                       vendor/lib64/libion.so:system/lib64/libion.so\
                       vendor/lib/libion.so:system/lib/libion.so\
			vendor/lib64/libsurfaceflinger.so:system/lib64/libsurfaceflinger.so\
			vendor/lib/libsurfaceflinger.so:system/lib/libsurfaceflinger.so\
                      vendor/lib64/libhardware_legacy.so:system/lib64/libhardware_legacy.so\
			vendor/lib/libhardware_legacy.so:system/lib/libhardware_legacy.so\
			vendor/lib64/libCameraHwCallBack.so:system/lib64/libCameraHwCallBack.so\
			vendor/lib/libCameraHwCallBack.so:system/lib/libCameraHwCallBack.so\
			vendor/lib64/libaudioom.so:system/lib64/libaudioom.so\
			vendor/lib/libaudioom.so:system/lib/libaudioom.so\
			vendor/lib64/libbalong-ril.so:system/lib64/libbalong-ril.so\
			vendor/lib64/libbalong-ril-1.so:system/lib64/libbalong-ril-1.so\
			vendor/lib64/libbalong_audio_ril.so:system/lib64/libbalong_audio_ril.so\
			vendor/lib/libbalong_audio_ril.so:system/lib/libbalong_audio_ril.so\
			vendor/lib64/libcamera_client.so:system/lib64/libcamera_client.so\
			vendor/lib/libcamera_client.so:system/lib/libcamera_client.so\
			vendor/lib/libcamera_core.so:system/lib/libcamera_core.so\
			vendor/lib64/libCameraHwParam.so:system/lib64/libCameraHwParam.so\
			vendor/lib/libCameraHwParam.so:system/lib/libCameraHwParam.so\
			vendor/lib64/libCameraHwSendCmd.so:system/lib64/libCameraHwSendCmd.so\
			vendor/lib/libCameraHwSendCmd.so:system/lib/libCameraHwSendCmd.so\
			vendor/lib64/libcameraservice.so:system/lib64/libcameraservice.so\
			vendor/lib/libcameraservice.so:system/lib/libcameraservice.so\
			vendor/lib64/libgnuexif.so:system/lib64/libgnuexif.so\
			vendor/lib/libgnuexif.so:system/lib/libgnuexif.so\
			vendor/lib64/libservices.huawei.so:system/lib64/libservices.huawei.so\
			vendor/lib/libservices.huawei.so:system/lib/libservices.huawei.so\
			vendor/lib64/libteec.so:system/lib64/libteec.so\
			vendor/lib64/libbt_factory_test.so:system/lib64/libbt_factory_test.so\
			vendor/lib64/libbt_factory_test_hi110x.so:system/lib64/libbt_factory_test_hi110x.so\
			vendor/lib64/libconn_factory_test.so:system/lib64/libconn_factory_test.so\
			vendor/lib64/libfm_hal.so:system/lib64/libfm_hal.so\
			vendor/lib64/libfm_interface.so:system/lib64/libfm_interface.so\
			vendor/lib64/libfm_jni.so:system/lib64/libfm_jni.so\
			vendor/lib64/libfm_volume.so:system/lib64/libfm_volume.so\
			vendor/lib64/libgps_factory_test.so:system/lib64/libgps_factory_test.so\
			vendor/lib64/libgps_factory_test_hi110x.so:system/lib64/libgps_factory_test_hi110x.so\
			vendor/lib64/libhardware_legacy_bcm.so:system/lib64/libhardware_legacy_bcm.so\
			vendor/lib64/libhardware_legacy_hisi.so:system/lib64/libhardware_legacy_hisi.so\
			vendor/lib64/libhuawei_mmi_test.so:system/lib64/libhuawei_mmi_test.so\
			vendor/lib64/lib_k3_ffmpeg.so:system/lib64/lib_k3_ffmpeg.so\
			vendor/lib64/lib_k3_omx_avc.so:system/lib64/lib_k3_omx_avc.so\
			vendor/lib64/lib_k3_omx_avcenc.so:system/lib64/lib_k3_omx_avcenc.so\
			vendor/lib64/lib_k3_omxcore.so:system/lib64/lib_k3_omxcore.so\
			vendor/lib64/lib_k3_omx_mpeg2.so:system/lib64/lib_k3_omx_mpeg2.so\
			vendor/lib64/lib_k3_omx_mpeg4.so:system/lib64/lib_k3_omx_mpeg4.so\
			vendor/lib64/lib_k3_omx_rv.so:system/lib64/lib_k3_omx_rv.so\
			vendor/lib64/lib_k3_omx_vc1.so:system/lib64/lib_k3_omx_vc1.so\
			vendor/lib64/lib_k3_omx_vp8.so:system/lib64/lib_k3_omx_vp8.so\
			vendor/lib64/libmedia_jni.huawei.so:system/lib64/libmedia_jni.huawei.so\
			vendor/lib64/libmtp.huawei.so:system/lib64/libmtp.huawei.so\
			vendor/lib64/libnfc-nci.so:system/lib64/libnfc-nci.so\
			vendor/lib64/libnfc_nci_jni.so:system/lib64/libnfc_nci_jni.so\
			vendor/lib64/libnfc_ndef.so:system/lib64/libnfc_ndef.so\
			vendor/lib64/libOpenMAXAL.so:system/lib64/libOpenMAXAL.so\
			vendor/lib64/libOpenSLES.so:system/lib64/libOpenSLES.so\
			vendor/lib64/libwifi_factory_test.so:system/lib64/libwifi_factory_test.so\
			vendor/lib64/libwifi_factory_test_hi110x.so:system/lib64/libwifi_factory_test_hi110x.so\
			vendor/lib64/libwpa_client.so:system/lib64/libwpa_client.so\
			vendor/lib64/libwpa_client_hisi.so:system/lib64/libwpa_client_hisi.so\
			vendor/lib/libteec.so:system/lib/libteec.so\
			vendor/lib/libaudioflinger.so:system/lib/libaudioflinger.so\
			vendor/lib/libaudioflinger.huawei.so:system/lib/libaudioflinger.huawei.so\
			vendor/lib/libhuaweiaudioalgoservice.so:system/lib/libhuaweiaudioalgoservice.so\
			vendor/lib/libhuaweiprocessing.so:system/lib/libhuaweiprocessing.so\
			vendor/lib/libjpu.so:system/lib/libjpu.so\
			vendor/vendor/firmware/4343A0_001.001.034.0056.0193_ORC.hcd:system/vendor/firmware/4343A0_001.001.034.0056.0193_ORC.hcd\
			vendor/vendor/firmware/BCM4334B0_002.001.013.1554.1613_RC.hcd:system/vendor/firmware/BCM4334B0_002.001.013.1554.1613_RC.hcd\
			vendor/vendor/firmware/fw_bcm4343s_apsta_hw.bin:system/vendor/firmware/fw_bcm4343s_apsta_hw.bin\
			vendor/vendor/firmware/fw_bcm4343s_hw.bin:system/vendor/firmware/fw_bcm4343s_hw.bin\
			vendor/vendor/firmware/fw_bcm4343s_p2p_hw.bin:system/vendor/firmware/fw_bcm4343s_p2p_hw.bin\
			vendor/vendor/firmware/fw_bcm4343s_test_hw_apsta.bin:system/vendor/firmware/fw_bcm4343s_test_hw_apsta.bin\
			vendor/vendor/firmware/GNSS.bin:system/vendor/firmware/GNSS.bin\
			vendor/vendor/firmware/libpn547_fw.so:system/vendor/firmware/libpn547_fw.so\
			vendor/vendor/firmware/nvram4343s_hw.txt:system/vendor/firmware/nvram4343s_hw.txt\
			vendor/vendor/firmware/nvram_CHERRY_PLUS_TL00.txt:system/vendor/firmware/nvram_CHERRY_PLUS_TL00.txt\
			vendor/vendor/firmware/nvram_CHERRY_PLUS_TL20M.txt:system/vendor/firmware/nvram_CHERRY_PLUS_TL20M.txt\
			vendor/vendor/firmware/nvram_CHERRY_PLUS_UL00.txt:system/vendor/firmware/nvram_CHERRY_PLUS_UL00.txt\
			vendor/vendor/firmware/sdio_cfg:system/vendor/firmware/sdio_cfg\
			vendor/vendor/firmware/SDIO_CMP.bin:system/vendor/firmware/SDIO_CMP.bin\
			vendor/vendor/firmware/SDIO_COD.bin:system/vendor/firmware/SDIO_COD.bin\
			vendor/vendor/firmware/SDIO_RW.bin:system/vendor/firmware/SDIO_RW.bin\
			vendor/vendor/firmware/SDIO_RW_ALICE_TL00_FEM.bin:system/vendor/firmware/SDIO_RW_ALICE_TL00_FEM.bin\
			vendor/vendor/firmware/SDIO_RW_ALICE_TL02_FEM.bin:system/vendor/firmware/SDIO_RW_ALICE_TL02_FEM.bin\
			vendor/vendor/lib/libbt-vendor.so:system/vendor/lib/libbt-vendor.so\
			vendor/vendor/lib/libbt-vendor-hi110x.so:system/vendor/lib/libbt-vendor-hi110x.so\
			vendor/vendor/lib/libee_core.so:system/vendor/lib/libee_core.so\
			vendor/vendor/lib/libee_support.so:system/vendor/lib/libee_support.so\
			vendor/vendor/lib/libexticonv.so:system/vendor/lib/libexticonv.so\
			vendor/vendor/lib/libposclientcore.so:system/vendor/lib/libposclientcore.so\
			vendor/vendor/lib/libslpgw-single.so:system/vendor/lib/libslpgw-single.so\
			vendor/vendor/lib/libstlport_shared_rtti.so:system/vendor/lib/libstlport_shared_rtti.so\
			vendor/vendor/lib/libsupl.so:system/vendor/lib/libsupl.so\
			vendor/vendor/lib/libwvdrm_L3.so:system/vendor/lib/libwvdrm_L3.so\
			vendor/vendor/lib/libwvm.so:system/vendor/lib/libwvm.so\
			vendor/vendor/lib/libWVStreamControlAPI_L3.so:system/vendor/lib/libWVStreamControlAPI_L3.so\
			vendor/vendor/lib64/libbt-vendor-hi110x.so:system/vendor/lib64/libbt-vendor-hi110x.so\
			vendor/vendor/framework/com.huawei.audioalgo.jar:system/vendor/framework/com.huawei.audioalgo.jar\
			vendor/vendor/etc/audio_effects.conf:system/vendor/etc/audio_effects.conf\
			vendor/vendor/media/LMspeed_508.emd:system/vendor/media/LMspeed_508.emd\
			vendor/vendor/media/PFFprec_600.emd:system/vendor/media/PFFprec_600.emd\





                     
## Setting up ramdisk

PRODUCT_COPY_FILES += $(call add-to-product-copy-files-if-exists,\
			$(LOCAL_PATH)/ramdisk/fstab.hi6210sft:root/fstab.hi6210sft \
			$(LOCAL_PATH)/ramdisk/init.hi6210sft.rc:root/init.hi6210sft.rc \
			$(LOCAL_PATH)/ramdisk/ueventd.hi6210sft.rc:root/ueventd.hi6210sft.rc \
			$(LOCAL_PATH)/ramdisk/file_contexts:root/file_contexts\
			$(LOCAL_PATH)/ramdisk/init:root/init\
			$(LOCAL_PATH)/ramdisk/init.5801.rc:root/init.5801.rc\
			$(LOCAL_PATH)/ramdisk/init.6165.rc:root/init.6165.rc\
			$(LOCAL_PATH)/ramdisk/init.10106.rc:root/init.10106.rc\
			$(LOCAL_PATH)/ramdisk/init.51054.rc:root/init.51054.rc\
			$(LOCAL_PATH)/ramdisk/init.102173.rc:root/init.102173.rc\
			$(LOCAL_PATH)/ramdisk/init.142782.rc:root/init.142782.rc\
			$(LOCAL_PATH)/ramdisk/init.audio.rc:root/init.audio.rc\
			$(LOCAL_PATH)/ramdisk/init.chip.usb.rc:root/init.chip.usb.rc\
			$(LOCAL_PATH)/ramdisk/init.connectivity.bcm43xx.rc:root/init.connectivity.bcm43xx.rc\
			$(LOCAL_PATH)/ramdisk/init.connectivity.hi110x.rc:root/init.connectivity.hi110x.rc\
			$(LOCAL_PATH)/ramdisk/init.connectivity.rc:root/init.connectivity.rc\
			$(LOCAL_PATH)/ramdisk/init.device.rc:root/init.device.rc\
			$(LOCAL_PATH)/ramdisk/init.extmodem.rc:root/init.extmodem.rc\
			$(LOCAL_PATH)/ramdisk/init.hisi.rc:root/init.hisi.rc\
			$(LOCAL_PATH)/ramdisk/init.manufacture.rc:root/init.manufacture.rc\
			$(LOCAL_PATH)/ramdisk/init.performance.rc:root/init.performance.rc\
			$(LOCAL_PATH)/ramdisk/init.platform.rc:root/init.platform.rc\
			$(LOCAL_PATH)/ramdisk/init.protocol.rc:root/init.protocol.rc\
			$(LOCAL_PATH)/ramdisk/init.rc:root/init.rc\
			$(LOCAL_PATH)/ramdisk/init.recovery.hi110x.rc:root/init.recovery.hi110x.rc\
			$(LOCAL_PATH)/ramdisk/init.recovery.hi6210sft.rc:root/init.recovery.hi6210sft.rc\
			$(LOCAL_PATH)/ramdisk/init.tee.rc:root/init.tee.rc\
			$(LOCAL_PATH)/ramdisk/init.usb.rc:root/init.usb.rc\
			$(LOCAL_PATH)/ramdisk/init.zygote64_32.rc:root/init.zygote64_32.rc\
			$(LOCAL_PATH)/ramdisk/property_contexts:root/property_contexts\
			$(LOCAL_PATH)/ramdisk/seapp_contexts:root/seapp_contexts\
			$(LOCAL_PATH)/ramdisk/sepolicy:root/sepolicy\
			$(LOCAL_PATH)/ramdisk/service_contexts:root/service_contexts\
			$(LOCAL_PATH)/ramdisk/ueventd.5801.rc:root/ueventd.5801.rc\
			$(LOCAL_PATH)/ramdisk/ueventd.6165.rc:root/ueventd.6165.rc\
			$(LOCAL_PATH)/ramdisk/ueventd.10106.rc:root/ueventd.10106.rc\
			$(LOCAL_PATH)/ramdisk/ueventd.51054.rc:root/ueventd.51054.rc\
			$(LOCAL_PATH)/ramdisk/ueventd.102173.rc:root/ueventd.102173.rc\
			$(LOCAL_PATH)/ramdisk/ueventd.142782.rc:root/ueventd.142782.rc\
			$(LOCAL_PATH)/ramdisk/ueventd.rc:root/ueventd.rc\
			$(LOCAL_PATH)/ramdisk/sbin/adbd:root/sbin/adbd\
			$(LOCAL_PATH)/ramdisk/sbin/check_root:root/sbin/check_root\
			$(LOCAL_PATH)/ramdisk/sbin/e2fsck_s:root/sbin/e2fsck_s\
			$(LOCAL_PATH)/ramdisk/sbin/emmc_partation:root/sbin/emmc_partation\
			$(LOCAL_PATH)/ramdisk/sbin/hdbd:root/sbin/hdbd\
			$(LOCAL_PATH)/ramdisk/sbin/adbd:healthd/sbin/healthd\
			$(LOCAL_PATH)/ramdisk/sbin/adbd:teecd/sbin/teecd)

                       
                       
                       
                       

#PRODUCT_COPY_FILES := vendor/mali/64bit/libGLES_mali.so:system/lib64/egl/libGLES_mali.so\
#                      vendor/mali/64bit/gralloc.hi6210sft.so:system/lib64/hw/gralloc.hi6210sft.so\
#                       vendor/mali/32bit/libGLES_mali.so:system/lib/egl/libGLES_mali.so \
#                       vendor/mali/32bit/gralloc.hi6210sft.so:system/lib/hw/gralloc.hi6210sft.so

# Inherit from the common Open Source product configuration
$(call inherit-product, $(SRC_TARGET_DIR)/product/aosp_base_telephony.mk)

PRODUCT_NAME := aosp_hi6210sft
PRODUCT_DEVICE := hi6210sft
PRODUCT_BRAND := Android
PRODUCT_MODEL := AOSP on P8 Lite K
PRODUCT_MANUFACTURER := Huawei
PRODUCT_RESTRICT_VENDOR_FILES := false

#$(call inherit-product, device/HUAWEI/hi6210sft/device.mk)
#$(call inherit-product-if-exists, vendor/HUAWEI/hi6210sft/device-vendor.mk)

PRODUCT_NAME := aosp_hi6210sft

PRODUCT_PACKAGES += \
    Launcher3




