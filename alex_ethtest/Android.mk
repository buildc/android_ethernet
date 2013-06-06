LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	alex_ethtest.cpp \

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../include

LOCAL_SHARED_LIBRARIES := \
					libcutils \
					libhardware \

LOCAL_MODULE:= alex_ethtest

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT_SBIN)
LOCAL_UNSTRIPPED_PATH := $(TARGET_ROOT_OUT_SBIN_UNSTRIPPED)

include $(BUILD_EXECUTABLE)
