LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := camera.hi6210sft
LOCAL_SRC_FILES := CameraWrapper.cpp

LOCAL_C_INCLUDES := \
    system/media/camera/include

LOCAL_SHARED_LIBRARIES := \
    libhardware liblog libcamera_client libutils libcutils libdl

LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_CFLAGS := -Werror
include $(BUILD_SHARED_LIBRARY)