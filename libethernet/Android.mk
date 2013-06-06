
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../include \

LOCAL_SHARED_LIBRARIES := \
						liblog \
						libcutils \
						libnetutils \

LOCAL_SRC_FILES := \
				libethernet.c \

LOCAL_MODULE := libethernet.$(TARGET_PRODUCT)
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
