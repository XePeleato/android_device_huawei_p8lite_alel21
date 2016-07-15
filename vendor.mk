#################################################################################
#
#				P8 Lite
#			    Propietary Blobs
#
#################################################################################





#Overlay
DEVICE_PACKAGE_OVERLAYS := device/HUAWEI/hi6210sft/overlay

#Audio Config
PRODUCT_COPY_FILES += \
device/HUAWEI/hi6210sft/audio/audio_effects.conf:system/etc/audio_effects.conf \
device/HUAWEI/hi6210sft/audio/audio_policy.conf:system/etc/audio_policy.conf

#Bluetooth
PRODUCT_COPY_FILES += \
device/HUAWEI/hi6210sft/bluetooth/auto_pair_devlist.conf:system/etc/bluetooth/auto_pair_devlist.conf \
device/HUAWEI/hi6210sft/bluetooth/bt_did.conf:system/etc/bluetooth/bt_did.conf \
device/HUAWEI/hi6210sft/bluetooth/bt_stack.conf:system/etc/bluetooth/bt_stack.conf


# Graphics
PRODUCT_COPY_FILES += \
		vendor/lib/egl/libGLES_mali.so:system/lib/egl/libGLES_mali.so \
		vendor/lib/egl/libGLES_android.so:system/lib/egl/libGLES_android.so \
		vendor/lib64/egl/libGLES_mali.so:system/lib64/egl/libGLES_mali.so \
		vendor/lib64/egl/libGLES_android.so:system/lib64/egl/libGLES_android.so \
		vendor/lib/hw/hwcomposer.hi6210sft.so:system/lib/hw/hwcomposer.hi6210sft.so \
		vendor/lib64/hw/hwcomposer.hi6210sft.so:system/lib64/hw/hwcomposer.hi6210sft.so





#Hardware Packages
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
libion.huawei