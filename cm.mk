# Copyright (C) 2016 The CyanogenMod Project
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

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

# Inherit from hi6210sft device
$(call inherit-product, device/linaro/hi6210sft/device.mk)

# Inherit some common CM stuff.
$(call inherit-product, vendor/cm/config/common_full_phone.mk)

PRODUCT_NAME := cm_hi6210sft
PRODUCT_DEVICE := hi6210sft
PRODUCT_MANUFACTURER := Huawei
PRODUCT_BRAND := Huawei
PRODUCT_GMS_CLIENTID_BASE := android-huawei
#PRODUCT_PROPERTY_OVERRIDES += \
    ro.product.device=hwALE-H \
    ro.product.model=ALE-L21 \
    ro.product.name=ALE-L21 \
    ro.build.id=HuaweiALE-L21

TARGET_VENDOR_PRODUCT_NAME := ALE-L21
TARGET_VENDOR_DEVICE_NAME := ALE-L21

PRODUCT_BUILD_PROP_OVERRIDES += \
    BUILD_FINGERPRINT="hi6210sft/hi6210sft/hi6210sft:6.0/MRA58K/jenkins06271131:user/test-keys" \
    PRIVATE_BUILD_DESC="ALE-L21-user 6.0 HuaweiALE-L21 C432B575 release-keys" \
    PRODUCT_MODEL="ALE-L21"

PRODUCT_SYSTEM_PROPERTY_BLACKLIST += ro.product.model
