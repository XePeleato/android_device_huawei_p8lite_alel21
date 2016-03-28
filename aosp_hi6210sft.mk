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
DEVICE_PACKAGE_OVERLAYS := device/HUAWEI/hi6210sft/overlay
Device_Root := device/HUAWEI/hi6210sft
#RIL
PRODUCT_PROPERTY_OVERRIDES += \
audioril.lib=libhuawei-audio-ril.so \
ro.telephony.ril_class=HuaweiRIL \
telephony.lteOnCdmaDevice=0 \
telephony.lteOnGsmDevice=1 \
ro.telephony.default_network=9

#Audio Config
PRODUCT_COPY_FILES += \
device/HUAWEI/hi6210sft/audio/audio_effects.conf:system/etc/audio_effects.conf \
device/HUAWEI/hi6210sft/audio/audio_policy.conf:system/etc/audio_policy.conf \


#Bluetooth
PRODUCT_COPY_FILES += \
device/HUAWEI/hi6210sft/bluetooth/auto_pair_devlist.conf:system/etc/bluetooth/auto_pair_devlist.conf \
device/HUAWEI/hi6210sft/bluetooth/bt_did.conf:system/etc/bluetooth/bt_did.conf \
device/HUAWEI/hi6210sft/bluetooth/bt_stack.conf:system/etc/bluetooth/bt_stack.conf


#Media patches
PRODUCT_COPY_FILES += \
device/HUAWEI/hi6210sft/media/media_profiles.xml:system/etc/media_profiles.xml \
device/HUAWEI/hi6210sft/media/media_codecs.xml:system/etc/media_codecs.xml \
device/HUAWEI/hi6210sft/media/media_codecs_ffmpeg.xml:system/etc/media_codecs_ffmpeg.xml

PRODUCT_PACKAGES += \
audio.primary.default \
audio_policy.stub \
audio.a2dp.default \
audio.usb.default \
audio.r_submix.default \
libaudioutils \
libtinyalsa \
tinyplay \
tinycap \
tinymix \
tinypcminfo \
sound_trigger.primary.hi6210sft \
camera.hi6210sft \
libion.huawei

#SeLinux
BOARD_SEPOLICY_DIRS += \
	device/HUAWEI/hi6210sft/selinux

BOARD_SEPOLICY_UNION += \
	file_contexts \
	installd.te
#etc perms (coming soon)



# Copying some libs in order to get it working
PRODUCT_COPY_FILES += \
		       vendor/bin/atcmdserver:system/bin/atcmdserver \
		       vendor/ons.bin:system/ons.bin \
		       vendor/isp.bin:system/isp.bin \
		       vendor/phone.prop:system/phone.prop \
		       vendor/bin/mediaserver:system/bin/mediaserver \
		       vendor/lib/libaudioflinger.so:system/lib/libaudioflinger.so \
		       vendor/lib/libaudiopolicyservice.so:system/lib/libaudiopolicyservice.so \
		       vendor/lib64/libaudiopolicyservice.so:system/lib64/libaudiopolicyservice.so \
		       vendor/lib/libsrsprocessing.so:system/lib/libsrsprocessing.so \
		       vendor/lib/libcameraservice.so:system/lib/libcameraservice.so \
		       vendor/lib64/libcameraservice.so:system/lib64/libcameraservice.so \
		       vendor/lib/drm/libdrmhwomavoneplugin.so:system/lib/drm/libdrmhwomavoneplugin.so\
		       vendor/vendor/lib/drm/libdrmwvmplugin.so:system/vendor/lib/drm/libdrmwvmplugin.so.so\
		       vendor/bin/glgps:system/bin/glgps\
		       vendor/bin/device_monitor:system/bin/device_monitor\
		       vendor/bin/gpsdaemon:system/bin/gpsdaemon\
		       vendor/bin/gpslogd:system/bin/gpslogd\
		       vendor/bin/hdb:system/bin/hdb\
		       vendor/bin/hwnff:system/bin/hwnff\
		       vendor/bin/hwnffserver:system/bin/hwnffserver\
		       vendor/bin/hwpged:system/bin/hwpged\
		       vendor/bin/hw_ueventd:system/bin/hw_ueventd\
		       vendor/bin/mac_addr_normalization:system/bin/mac_addr_normalization\
		       vendor/bin/rild:system/bin/rild\
		       vendor/mali/64bit/libGLES_mali.so:system/lib64/egl/libGLES_mali.so\
		       vendor/mali/32bit/libGLES_mali.so:system/lib/egl/libGLES_mali.so\
		       vendor/lib64/hw/audio.a2dp.default.so:system/lib64/hw/audio.a2dp.default.so\
		       vendor/lib/hw/audio.a2dp.default.so:system/lib/hw/audio.a2dp.default.so\
		       vendor/lib64/hw/audio.primary.default.so:system/lib64/hw/audio.primary.default.so\
		       vendor/lib/hw/audio.primary.hi6210sft.so:system/lib/hw/audio.primary.hi6210sft.so \
		       vendor/lib64/hw/audio.primary.hi6210sft.so:system/lib64/hw/audio.primary.hi6210sft.so \
		       vendor/lib/hw/audio.primary.default.so:system/lib/hw/audio.primary.default.so\
		       vendor/lib64/hw/audio.r_submix.default.so:system/lib64/hw/audio.r_submix.default.so\
		       vendor/lib/hw/audio.r_submix.default.so:system/lib/hw/audio.r_submix.default.so\
		       vendor/lib/hw/bluetooth.default.so:system/lib/hw/bluetooth.default.so \
		       vendor/lib64/hw/camera.hi6210sft.so:system/lib64/hw/camera.hi6210sft.so\
		       vendor/lib/hw/camera.hi6210sft.so:system/lib/hw/camera.hi6210sft.so\
		       vendor/lib64/hw/bastet.hi6210sft.so:system/lib64/hw/bastet.hi6210sft.so\
		       vendor/lib64/hw/keystore.hi6210sft.so:system/lib64/hw/keystore.hi6210sft.so\
		       vendor/lib/hw/keystore.hi6210sft.so:system/lib/hw/keystore.hi6210sft.so\
		       vendor/lib64/hw/gps.hi6210sft.so:system/lib64/hw/gps.hi6210sft.so\
		       vendor/lib64/hw/gps.hi110x.default.so:system/lib64/hw/gps.hi110x.default.so\
		       vendor/lib/hw/gps.hi110x.default.so:system/lib/hw/gps.hi110x.default.so\
		       vendor/lib64/hw/libbcmfm_if.so:system/lib64/hw/libbcmfm_if.so\
		       vendor/lib/hw/libbcmfm_if.so:system/lib/hw/libbcmfm_if.so\
		       vendor/lib64/hw/libhisifm_if.so:system/lib64/hw/libhisifm_if.so\
		       vendor/lib/hw/libhisifm_if.so:system/lib/hw/libhisifm_if.so\
		       vendor/lib64/hw/lights.default.so:system/lib64/hw/lights.default.so\
		       vendor/lib/hw/lights.default.so:system/lib/hw/lights.default.so\
		       vendor/lib64/hw/nfc_nci.pn54x.default.so:system/lib64/hw/nfc_nci.pn54x.default.so\
		       vendor/lib64/hw/power.default.so:system/lib64/hw/power.default.so\
		       vendor/lib/hw/power.default.so:system/lib/hw/power.default.so\
		       vendor/lib64/hw/sensors.hi6210sft.so:system/lib64/hw/sensors.hi6210sft.so\
		       vendor/lib/hw/sensors.hi6210sft.so:system/lib/hw/sensors.hi6210sft.so\
		       vendor/lib64/hw/gralloc.hi6210sft.so:system/lib64/hw/gralloc.hi6210sft.so\
		       vendor/lib/hw/gralloc.hi6210sft.so:system/lib/hw/gralloc.hi6210sft.so\
		       vendor/lib/hw/hwcomposer.default.so:system/lib/hw/hwcomposer.default.so\
		       vendor/lib64/hw/hwcomposer.default.so:system/lib64/hw/hwcomposer.default.so\
		       vendor/lib64/hw/hwcomposer.hi6210sft.so:system/lib64/hw/hwcomposer.hi6210sft.so\
		       vendor/lib/hw/hwcomposer.hi6210sft.so:system/lib/hw/hwcomposer.hi6210sft.so\
                       vendor/lib64/hw/memtrack.hi6210sft.so:system/lib64/hw/memtrack.hi6210sft.so\
                       vendor/lib/hw/memtrack.hi6210sft.so:system/lib/hw/memtrack.hi6210sft.so\
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
                       vendor/lib/libion.so:system/lib/libion.so\
                       vendor/lib64/libion.so:system/lib64/libion.so\
			vendor/lib64/libsurfaceflinger.so:system/lib64/libsurfaceflinger.so\
			vendor/lib/libsurfaceflinger.so:system/lib/libsurfaceflinger.so\
                      vendor/lib64/libhardware_legacy.so:system/lib64/libhardware_legacy.so\
			vendor/lib/libhardware_legacy.so:system/lib/libhardware_legacy.so\
			vendor/lib64/libCameraHwCallBack.so:system/lib64/libCameraHwCallBack.so\
			vendor/lib/libCameraHwCallBack.so:system/lib/libCameraHwCallBack.so\
			vendor/lib64/libaudioom.so:system/lib64/libaudioom.so\
			vendor/lib/libaudioom.so:system/lib/libaudioom.so\
			vendor/lib64/libbalong-ril.so:system/lib64/libbalong-ril.so\
		        vendor/lib64/libril.so:system/lib64/libril.so\
		        vendor/lib/libril.so:system/lib/libril.so\
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
			vendor/lib/libaudioflinger.huawei.so:system/lib/libaudioflinger.huawei.so\
			vendor/lib/libhuaweiaudioalgoservice.so:system/lib/libhuaweiaudioalgoservice.so\
			vendor/lib/libhuaweiprocessing.so:system/lib/libhuaweiprocessing.so\
			vendor/lib/libjpu.so:system/lib/libjpu.so\
			vendor/lib64/libjpu.so:system/lib64/libjpu.so\
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
			vendor/vendor/lib/libbt-vendor-hi110x.so:system/vendor/lib/libbt-vendor-hi110x.so\
			vendor/vendor/lib/libee_core.so:system/vendor/lib/libee_core.so\
			vendor/vendor/lib/libee_support.so:system/vendor/lib/libee_support.so\
			vendor/vendor/lib/libexticonv.so:system/vendor/lib/libexticonv.so\
			vendor/vendor/lib/libposclientcore.so:system/vendor/lib/libposclientcore.so\
			vendor/vendor/lib/libslpgw-single.so:system/vendor/lib/libslpgw-single.so\
			vendor/vendor/lib/libstlport_shared_rtti.so:system/vendor/lib/libstlport_shared_rtti.so\
			vendor/vendor/lib/libsupl.so:system/vendor/lib/libsupl.so\
			vendor/vendor/lib/libwvdrm_L3.so:system/vendor/lib/libwvdrm_L3.so\
			vendor/vendor/lib/libWVStreamControlAPI_L3.so:system/vendor/lib/libWVStreamControlAPI_L3.so\
			vendor/vendor/lib64/libbt-vendor-hi110x.so:system/vendor/lib64/libbt-vendor-hi110x.so\
			vendor/vendor/framework/com.huawei.audioalgo.jar:system/vendor/framework/com.huawei.audioalgo.jar\
			vendor/vendor/media/LMspeed_508.emd:system/vendor/media/LMspeed_508.emd\
			vendor/vendor/media/PFFprec_600.emd:system/vendor/media/PFFprec_600.emd





                     
## Setting up ramdisk

PRODUCT_COPY_FILES += $(call add-to-product-copy-files-if-exists,\
			$(LOCAL_PATH)/ramdisk/fstab.hi6210sft:root/fstab.hi6210sft \
			$(LOCAL_PATH)/ramdisk/init.hi6210sft.rc:root/init.hi6210sft.rc \
			$(LOCAL_PATH)/ramdisk/ueventd.hi6210sft.rc:root/ueventd.hi6210sft.rc \
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
			$(LOCAL_PATH)/ramdisk/sbin/healthd:root/sbin/healthd\
			$(LOCAL_PATH)/ramdisk/sbin/teecd:root/sbin/teecd)

                       
                       

#Bin                      
                       
PRODUCT_COPY_FILES += \
		vendor/bin/afar:system/bin/afar \
		vendor/bin/agnsscontrol:system/bin/agnsscontrol \
		vendor/bin/agnsslog:system/bin/agnsslog \
		vendor/bin/akmd09911:system/bin/akmd09911 \
		vendor/bin/bastetd:system/bin/bastetd \
		vendor/bin/chargelogcat:system/bin/chargelogcat \
		vendor/bin/checkntfs:system/bin/checkntfs \
		vendor/bin/chr_logd:system/bin/chr_logd \
		vendor/bin/crashnotice:system/bin/crashnotice \
		vendor/bin/data_cleaner:system/bin/data_cleaner \
		vendor/bin/dexopt-wrapper:system/bin/dexopt-wrapper \
		vendor/bin/diagserver:system/bin/diagserver \
		vendor/bin/dmesgcat:system/bin/dmesgcat \
		vendor/bin/dumptool:system/bin/dumptool \
		vendor/bin/eventcat:system/bin/eventcat \
		vendor/bin/exfatfsck:system/bin/exfatfsck \
		vendor/bin/filebackup:system/bin/filebackup \
		vendor/bin/firmware_bfg_loglevel:system/bin/firmware_bfg_loglevel \
		vendor/bin/get_board_ver:system/bin/get_board_ver \
		vendor/bin/get_chip_ver:system/bin/get_chip_ver \
		vendor/bin/get_param_ver:system/bin/get_param_ver \
		vendor/bin/gnss_engine:system/bin/gnss_engine \
		vendor/bin/hi110x_dump:system/bin/hi110x_dump \
		vendor/bin/hi110x_except_logd:system/bin/hi110x_except_logd \
		vendor/bin/hi110x_logd:system/bin/hi110x_logd \
		vendor/bin/hisi_connectivity.sh:system/bin/hisi_connectivity.sh \
		vendor/bin/hostapd:system/bin/hostapd \
		vendor/bin/hostapd_cli_hisi:system/bin/hostapd_cli_hisi \
		vendor/bin/hostapd_hisi:system/bin/hostapd_hisi \
		vendor/bin/huawei_tp_test:system/bin/huawei_tp_test \
		vendor/bin/libcgroup.so:system/bin/libcgroup.so \
		vendor/bin/mkexfatfs:system/bin/mkexfatfs \
		vendor/bin/mkntfs:system/bin/mkntfs \
		vendor/bin/modem_resetinfo:system/bin/modem_resetinfo \
		vendor/bin/modemlogcat_lte:system/bin/modemlogcat_lte \
		vendor/bin/ntfs-3g:system/bin/ntfs-3g \
		vendor/bin/oam_app:system/bin/oam_app \
		vendor/bin/octty:system/bin/octty \
		vendor/bin/pcscd:system/bin/pcscd \
		vendor/bin/powerlogd:system/bin/powerlogd \
		vendor/bin/preparesd.sh:system/bin/preparesd.sh \
		vendor/bin/server_agent:system/bin/server_agent \
		vendor/bin/set_log:system/bin/set_log \
		vendor/bin/shutdownanimation:system/bin/shutdownanimation \
		vendor/bin/sleeplogcat:system/bin/sleeplogcat \
		vendor/bin/start_110x_service.sh:system/bin/start_110x_service.sh \
		vendor/bin/supl20clientd:system/bin/supl20clientd \
		vendor/bin/thermal-daemon:system/bin/thermal-daemon \
		vendor/bin/wl:system/bin/wl \
		vendor/bin/wl_config:system/bin/wl_config \
		vendor/bin/wpa_cli_hisi:system/bin/wpa_cli_hisi \
		vendor/bin/wpa_supplicant_hisi:system/bin/wpa_supplicant_hisi 

#Lib
PRODUCT_COPY_FILES += \
		vendor/lib/cp_if.so:system/lib/cp_if.so \
		vendor/lib/imedia_filters.so:system/lib/imedia_filters.so \
		vendor/lib/lib_atprotocolsw.so:system/lib/lib_atprotocolsw.so \
		vendor/lib/lib_hwnsd_input.so:system/lib/lib_hwnsd_input.so \
		vendor/lib/lib_k3_ffmpeg.so:system/lib/lib_k3_ffmpeg.so \
		vendor/lib/lib_k3_omx_avc.so:system/lib/lib_k3_omx_avc.so \
		vendor/lib/lib_k3_omx_avcenc.so:system/lib/lib_k3_omx_avcenc.so \
		vendor/lib/lib_k3_omx_mpeg2.so:system/lib/lib_k3_omx_mpeg2.so \
		vendor/lib/lib_k3_omx_mpeg4.so:system/lib/lib_k3_omx_mpeg4.so \
		vendor/lib/lib_k3_omx_rv.so:system/lib/lib_k3_omx_rv.so \
		vendor/lib/lib_k3_omx_vc1.so:system/lib/lib_k3_omx_vc1.so \
		vendor/lib/lib_k3_omx_vp8.so:system/lib/lib_k3_omx_vp8.so \
		vendor/lib/lib_k3_omxcore.so:system/lib/lib_k3_omxcore.so \
		vendor/lib/libadc_old_sdk.so:system/lib/libadc_old_sdk.so \
		vendor/lib/libaes.so:system/lib/libaes.so \
		vendor/lib/libagnssal.so:system/lib/libagnssal.so \
		vendor/lib/libairsharing_platform.so:system/lib/libairsharing_platform.so \
		vendor/lib/libatpswdck.so:system/lib/libatpswdck.so \
		vendor/lib/libaudioalgoservice_jni.so:system/lib/libaudioalgoservice_jni.so \
		vendor/lib/libaudio-resampler.so:system/lib/libaudio-resampler.so \
		vendor/lib/libaudioresampler.so:system/lib/libaudioresampler.so \
		vendor/lib/libaudio_custom.so:system/lib/libaudio_custom.so \
		vendor/lib/libaudioeffect_jni.so:system/lib/libaudioeffect_jni.so \
		vendor/lib/libaudio_custom.so:system/lib/libaudio_custom.so \
		vendor/lib/libaudiopolicymanager.so:system/lib/libaudiopolicymanager.so \
		vendor/lib/libaudiopolicymanagerdefault.so:system/lib/libaudiopolicymanagerdefault.so \
		vendor/lib/libaudioutils.so:system/lib/libaudioutils.so \
		vendor/lib/libavcodec.so:system/lib/libavcodec.so \
		vendor/lib/libavformat.so:system/lib/libavformat.so \
		vendor/lib/libavutil.so:system/lib/libavutil.so \
		vendor/lib/libbalong-ril.so:system/lib/libbalong-ril.so \
		vendor/lib/libbalong-ril-1.so:system/lib/libbalong-ril-1.so \
		vendor/lib/libBestShot.so:system/lib/libBestShot.so \
		vendor/lib/libbluetoothex_jni.so:system/lib/libbluetoothex_jni.so \
		vendor/lib/libbluetooth_jni.so:system/lib/libbluetooth_jni.so \
		vendor/lib/libbreakpad.so:system/lib/libbreakpad.so \
		vendor/lib/libbspatchhwouc.so:system/lib/libbspatchhwouc.so \
		vendor/lib/libbt_factory_test.so:system/lib/libbt_factory_test.so \
		vendor/lib/libbt_factory_test_hi110x.so:system/lib/libbt_factory_test_hi110x.so \
		vendor/lib/libcamera_omron.so:system/lib/libcamera_omron.so \
		vendor/lib/libcamera_post_mediaserver.so:system/lib/libcamera_post_mediaserver.so \
		vendor/lib/libcares.so:system/lib/libcares.so \
		vendor/lib/libcgroup.so:system/lib/libcgroup.so \
		vendor/lib/libchrlog.so:system/lib/libchrlog.so \
		vendor/lib/libcontactsmatchname.so:system/lib/libcontactsmatchname.so \
		vendor/lib/libevent.so:system/lib/libevent.so \
		vendor/lib/libfb.so:system/lib/libfb.so \
		vendor/lib/libfb_cpucapabilities.so:system/lib/libfb_cpucapabilities.so \
		vendor/lib/libfb_creativeediting.so:system/lib/libfb_creativeediting.so \
		vendor/lib/libfb_crypto.so:system/lib/libfb_crypto.so \
		vendor/lib/libfb_dalvik-internals.so:system/lib/libfb_dalvik-internals.so \
		vendor/lib/libfb_ffmpeg.so:system/lib/libfb_ffmpeg.so \
		vendor/lib/libfb_ffmpeg_jni.so:system/lib/libfb_ffmpeg_jni.so \
		vendor/lib/libfm_interface.so:system/lib/libfm_interface.so \
		vendor/lib/libfm_jni.so:system/lib/libfm_jni.so \
		vendor/lib/libfm_volume.so:system/lib/libfm_volume.so \
		vendor/lib/libgnssadapter.so:system/lib/libgnssadapter.so \
		vendor/lib/libhardware_legacy_bcm.so:system/lib/libhardware_legacy_bcm.so \
		vendor/lib/libhardware_legacy_hisi.so:system/lib/libhardware_legacy_hisi.so \
		vendor/lib/libhelixplayer.so:system/lib/libhelixplayer.so \
		vendor/lib/libhisi_efuse.so:system/lib/libhisi_efuse.so \
		vendor/lib/libhisi_ini.so:system/lib/libhisi_ini.so \
		vendor/lib/libhuaweiaudioeffectutil.so:system/lib/libhuaweiaudioeffectutil.so \
		vendor/lib/libhwdeviceinfo.so:system/lib/libhwdeviceinfo.so \
		vendor/lib/libhwextdevice.so:system/lib/libhwextdevice.so \
		vendor/lib/libjpegenchw.so:system/lib/libjpegenchw.so \
		vendor/lib/libmax98925.so:system/lib/libmax98925.so \
		vendor/lib/libmedia.so:system/lib/libmedia.so \
		vendor/lib/libmedia_jni.huawei.so:system/lib/libmedia_jni.huawei.so \
		vendor/lib/libmediarecorder.huawei.so:system/lib/libmediarecorder.huawei.so \
		vendor/lib/libmediaplayerservice.so:system/lib/libmediaplayerservice.so \
		vendor/lib/libnfc_nci_jni.so:system/lib/libnfc_nci_jni.so \
		vendor/lib/libnfc-nci.so:system/lib/libnfc-nci.so \
		vendor/lib/liboeminfo.so:system/lib/liboeminfo.so \
		vendor/lib/libpowergenie_native3.so:system/lib/libpowergenie_native3.so \
		vendor/lib/libsensorsgps.so:system/lib/libsensorsgps.so \
		vendor/lib/libsqlite3.so:system/lib/libsqlite3.so \
		vendor/lib/libstagefright.so:system/lib/libstagefright.so \
		vendor/lib/libstagefright.huawei.so:system/lib/libstagefright.huawei.so \
		vendor/lib/libstagefright_foundation.so:system/lib/libstagefright_foundation.so \
		vendor/lib/libstagefright_soft_ffmpegaudiodec.so:system/lib/libstagefright_soft_ffmpegaudiodec.so \
		vendor/lib/libstagefright_soft_ffmpegvideodec.so:system/lib/libstagefright_soft_ffmpegvideodec.so \
		vendor/lib/libstagefrighthw.so:system/lib/libstagefrighthw.so \
		vendor/lib/libsupl10client.so:system/lib/libsupl10client.so \
		vendor/lib/libsupl20client.so:system/lib/libsupl20client.so \
		vendor/lib/libsupl20comon.so:system/lib/libsupl20comon.so \
		vendor/lib/libsupl20if.so:system/lib/libsupl20if.so \
		vendor/lib/libsupl20oasn1comn.so:system/lib/libsupl20oasn1comn.so \
		vendor/lib/libsupl20oasn1lpp.so:system/lib/libsupl20oasn1lpp.so \
		vendor/lib/libsupl20oasn1rrc.so:system/lib/libsupl20oasn1rrc.so \
		vendor/lib/libsupl20oasn1rrlp.so:system/lib/libsupl20oasn1rrlp.so \
		vendor/lib/libsupl20oasn1supl1.so:system/lib/libsupl20oasn1supl1.so \
		vendor/lib/libsupl20oasn1supl2.so:system/lib/libsupl20oasn1supl2.so \
		vendor/lib/libsupl20oasn1tia.so:system/lib/libsupl20oasn1tia.so \
		vendor/lib/libtfa9895.so:system/lib/libtfa9895.so \
		vendor/lib/libwifipro.so:system/lib/libwifipro.so \
		vendor/lib/libwpa_client_hisi.so:system/lib/libwpa_client_hisi.so


#Lib64
PRODUCT_COPY_FILES += \
		vendor/lib64/lib_hwnsd_input.so:system/lib64/lib_hwnsd_input.so \
		vendor/lib64/libaes.so:system/lib64/libaes.so \
		vendor/lib64/libagnssal.so:system/lib64/libagnssal.so \
		vendor/lib64/libaudio_custom.so:system/lib64/libaudio_custom.so \
		vendor/lib64/libaudioalgoservice_jni.so:system/lib64/libaudioalgoservice_jni.so \
		vendor/lib64/libaudio-resampler.so:system/lib64/libaudio-resampler.so \
		vendor/lib64/libaudioroute.so:system/lib64/libaudioroute.so \
		vendor/lib64/libbastet.so:system/lib64/libbastet.so \
		vendor/lib64/libbluetoothex_jni.so:system/lib64/libbluetoothex_jni.so \
		vendor/lib64/libchrlog.so:system/lib64/libchrlog.so \
		vendor/lib64/libgnssadapter.so:system/lib64/libgnssadapter.so \
		vendor/lib64/libhisi_efuse.so:system/lib64/libhisi_efuse.so \
		vendor/lib64/libhisi_ini.so:system/lib64/libhisi_ini.so \
		vendor/lib64/libmax98925.so:system/lib64/libmax98925.so \
		vendor/lib64/libmediarecorder.huawei.so:system/lib64/libmediarecorder.huawei.so \
		vendor/lib64/liboeminfo.so:system/lib64/liboeminfo.so \
		vendor/lib64/libpowergenie_native3.so:system/lib64/libpowergenie_native3.so \
		vendor/lib64/libstagefright.huawei.so:system/lib64/libstagefright.huawei.so \
		vendor/lib64/libstagefright_soft_ffmpegaudiodec.so:system/lib64/libstagefright_soft_ffmpegaudiodec.so \
		vendor/lib64/libstagefright_soft_ffmpegvideodec.so:system/lib64/libstagefright_soft_ffmpegvideodec.so \
		vendor/lib64/libstagefrighthw.so:system/lib64/libstagefrighthw.so \
		vendor/lib64/libtfa9895.so:system/lib64/libtfa9895.so \
		vendor/lib64/libwifipro.so:system/lib64/libwifipro.so 


		




	NFCEE_ACCESS_PATH := vendor/etc/nfcee_access.xml
PRODUCT_COPY_FILES += \
	$(NFCEE_ACCESS_PATH):system/etc/nfcee_access.xml








# Inherit from the common Open Source product configuration
$(call inherit-product, $(SRC_TARGET_DIR)/product/aosp_base_telephony.mk)

PRODUCT_NAME := aosp_hi6210sft
PRODUCT_DEVICE := hi6210sft
PRODUCT_BRAND := Android
PRODUCT_MODEL := AOSP by XePeleato
PRODUCT_MANUFACTURER := Huawei
PRODUCT_RESTRICT_VENDOR_FILES := false

#$(call inherit-product, device/HUAWEI/hi6210sft/device.mk)
#$(call inherit-product-if-exists, vendor/HUAWEI/hi6210sft/device-vendor.mk)

PRODUCT_NAME := aosp_hi6210sft

PRODUCT_PACKAGES += \
    Launcher3




