#
# Copyright 2014 The Android Open Source Project
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
$(call inherit-product, device/HUAWEI/hi6210sft/cm_hi6210sft.mk)
$(call inherit-product, vendor/cm/config/common_full_phone.mk)
$(call inherit-product, vendor/cm/config/nfc_enhanced.mk)
include $(call all-makefiles-under,$(LOCAL_PATH))


PRODUCT_MAKEFILES := \
    $(LOCAL_DIR)/cm_hi6210sft.mk \
    $(LOCAL_DIR)/kernel/HUAWEI/hi6210sft/Android.mk

PRODUCT_NAME := cm_hi6210sft
PRODUCT_DEVICE := hi6210sft
PRODUCT_BRAND := Android
PRODUCT_MODEL := CM by XePeleato
TARGET_VENDOR := HUAWEI
PRODUCT_MANUFACTURER := Huawei
PRODUCT_RESTRICT_VENDOR_FILES := false