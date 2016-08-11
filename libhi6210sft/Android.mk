# * libhi6210sft
# *
# * 2016 XePeleato
# *
# * I have disassembled pieces of some libraries in order to get a functional ROM.
# *Some of the work shown here, is not mine
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
	exif_patch.c \
	main.c \
	gui_patch.cpp \
	log_patch.c \
	camera_patch.c

LOCAL_SHARED_LIBRARIES := liblog libcutils libgui libbinder libutils
LOCAL_MULTILIB := both
LOCAL_MODULE := libhi6210sft
LOCAL_LDLIBS := -llog
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)
