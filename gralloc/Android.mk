# 
# Copyright (C) 2010 ARM Limited. All rights reserved.
# 
# Copyright (C) 2008 The Android Open Source Project
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


LOCAL_PATH := $(call my-dir)

# HAL module implemenation, not prelinked and stored in
# hw/<OVERLAY_HARDWARE_MODULE_ID>.<ro.product.board>.so
include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false

ifeq ($(shell test $(PLATFORM_SDK_VERSION) -ge 21 && echo OK),OK)
	LOCAL_MODULE_RELATIVE_PATH := hw
else
	LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
endif

MALI_DDK_TEST_PATH := hardware/arm/

LOCAL_MODULE := gralloc.hi6210sft
#LOCAL_MODULE_TAGS := optional

# Which DDK are we building for?
ifeq (,$(wildcard $(MALI_DDK_TEST_PATH)))
# Mali-T6xx DDK
MALI_DDK_PATH := vendor/arm/mali6xx
LOCAL_SHARED_LIBRARIES := liblog libcutils libGLESv1_CM libion

# All include files are accessed from the DDK root
DDK_PATH := $(LOCAL_PATH)/../../..
UMP_HEADERS_PATH := bionic/libc/kernel/uapi
LOCAL_C_INCLUDES := $(DDK_PATH) $(UMP_HEADERS_PATH)

LOCAL_CFLAGS := -DLOG_TAG=\"gralloc\" -DSTANDARD_LINUX_SCREEN -DMALI_600
else
# Mali-200/300/400MP DDK
MALI_DDK_PATH := hardware/arm/mali
#SHARED_MEM_LIBS := libUMP
SHARED_MEM_LIBS := libion libhardware
LOCAL_SHARED_LIBRARIES := liblog libcutils libGLESv1_CM $(SHARED_MEM_LIBS)

LOCAL_C_INCLUDES := system/core/include/ $(MALI_DDK_PATH)/include

#ION Includes
LOCAL_C_INCLUDES := \
		bionic/libc/kernel/uapi

# Include the UMP header files
LOCAL_C_INCLUDES += $(MALI_DDK_PATH)/src/ump/include

LOCAL_CFLAGS := -DLOG_TAG=\"gralloc\" -DGRALLOC_32_BITS -DSTANDARD_LINUX_SCREEN -DPLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)
endif


LOCAL_SRC_FILES := \
	gralloc_module.cpp \
	alloc_device.cpp \
	framebuffer_device.cpp

#LOCAL_CFLAGS+= -DMALI_VSYNC_EVENT_REPORT_ENABLE
include $(BUILD_SHARED_LIBRARY)
