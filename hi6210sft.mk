#
# Inherit the full_base and device configurations
$(call inherit-product, device/linaro/hi6210sft/device.mk)
$(call inherit-product, device/linaro/hi6210sft/vendor.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/aosp_base_telephony.mk)
$(call inherit-product, vendor/cm/config/common_full_phone.mk)

#
# Overrides
PRODUCT_NAME := hi6210sft
PRODUCT_DEVICE := hi6210sft
PRODUCT_MODEL := CM14.1 by XePeleato
