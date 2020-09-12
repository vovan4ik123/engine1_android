LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := sqlite

LOCAL_C_INCLUDES := $(LOCAL_PATH)/

LOCAL_SRC_FILES := \
 $(LOCAL_PATH)/sqlite3.c

include $(BUILD_SHARED_LIBRARY)