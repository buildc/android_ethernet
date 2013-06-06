
LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_PRODUCT),duv060)
   include $(call all-makefiles-under,$(LOCAL_PATH))
endif
