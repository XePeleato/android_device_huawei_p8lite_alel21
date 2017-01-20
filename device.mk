#
# Copyright (C) 2011 The Android Open-Source Project
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

LOCAL_PATH := device/linaro/hi6210sft
LOCAL_FSTAB := $(LOCAL_PATH)/rootdir/fstab.hi6210sft
TARGET_RECOVERY_FSTAB = $(LOCAL_FSTAB)
TARGET_SCREEN_HEIGHT := 1280
TARGET_SCREEN_WIDTH := 720
#DEVICE_PACKAGE_OVERLAYS := $(LOCAL_PATH)/overlay

# Need AppWidget permission to prevent from Launcher's crash.
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.software.app_widgets.xml:system/etc/permissions/android.software.app_widgets.xml

# Build libion for new double-buffering HDLCD driver
PRODUCT_PACKAGES += libion

# Build gralloc
#PRODUCT_PACKAGES += gralloc.hi6210sft

#Build hwcomposer
#PRODUCT_PACKAGES += hwcomposer.hi6210sft

PRODUCT_PACKAGES += lights.hi6210sft libhi6210sft

# Build Snap
PRODUCT_PACKAGES += Snap

# Include ION tests
PRODUCT_PACKAGES += iontest \
                    ion-unit-tests

# Set zygote config
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.zygote=zygote64_32
PRODUCT_PROPERTY_OVERRIDES += \
         debug.sf.no_hw_vsync=1 \
         ro.secure=0 \
         ro.adb.secure=0 \
	 persist.sys.usb.config=mtp \
	 sys.usb.configfs=0


PRODUCT_COPY_FILES += \
        frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
        frameworks/native/data/etc/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml \
        frameworks/native/data/etc/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml \
	frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml \
	frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \

#RIL
PRODUCT_PROPERTY_OVERRIDES += \
     audioril.lib=libhuawei-audio-ril.so \
     ro.telephony.ril_class=HwHisiRIL


#Audio Config
PRODUCT_COPY_FILES += \
     $(LOCAL_PATH)/audio/audio_effects.conf:system/etc/audio_effects.conf \
     $(LOCAL_PATH)/audio/audio_policy.conf:system/etc/audio_policy.conf

#Media patches
PRODUCT_COPY_FILES += \
     $(LOCAL_PATH)/media/media_profiles.xml:system/etc/media_profiles.xml \
     $(LOCAL_PATH)/media/media_codecs.xml:system/etc/media_codecs.xml \
     $(LOCAL_PATH)/media/media_codecs_ffmpeg.xml:system/etc/media_codecs_ffmpeg.xml

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
     libion.huawei \
     libwpa_client \
     wpa_supplicant \
     wpa_supplicant-conf \
     libnfc-nci \
     libnfc_nci_jni \
     Nfc \
     Tag \
     com.android.nfc_extras \
     dlopener \
     hwchelper

#SeLinux
BOARD_SEPOLICY_DIRS += \
     $(LOCAL_PATH)/selinux

# Call vendor files (by now just put all files in /vendor dir and adjust vendor.mk file)
$(call inherit-product, $(LOCAL_PATH)/vendor.mk)

