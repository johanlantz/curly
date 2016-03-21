#####RELEASE######
#APP_OPTIM := release
#################


######DEBUG######
APP_OPTIM := debug
LOCAL_CFLAGS += -g
NDK_DEBUG=1
#################

# The architectures you would like to build for
APP_ABI := armeabi armeabi-v7a x86
#APP_ABI := armeabi
APP_PLATFORM := android-18

