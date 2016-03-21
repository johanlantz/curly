
LOCAL_PATH	:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := ssl-prebuilt
LOCAL_SRC_FILES := ../../third-party/openssl/android/$(TARGET_ARCH_ABI)/libssl.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := crypto-prebuilt
LOCAL_SRC_FILES := ../../third-party//openssl/android/$(TARGET_ARCH_ABI)/libcrypto.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := curl-prebuilt
LOCAL_SRC_FILES := ../../third-party/curl/android/$(TARGET_ARCH_ABI)/libcurl.a
include $(PREBUILT_STATIC_LIBRARY)

#Build our own curly shared lib
 
include $(CLEAR_VARS)

LOCAL_MODULE := curly

$(info Will build for architecture:$(TARGET_ARCH_ABI) in mode $(APP_OPTIM))

# Android build settings
LOCAL_CFLAGS     := -Werror $(APP_CFLAGS) -frtti 

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../ \
					$(LOCAL_PATH)/../../third-party/curl/android/include \

LOCAL_SRC_FILES  := ../../curly.c

#Here we load the system libs, libz can be removed if we do not need compressed http responses
LOCAL_LDLIBS     := -ldl -lz

LOCAL_STATIC_LIBRARIES += gcc
LOCAL_STATIC_LIBRARIES += opus-prebuilt ssl-prebuilt crypto-prebuilt curl-prebuilt

include $(BUILD_SHARED_LIBRARY)
