# curly
Curly is a high level API for libcurl intended to simplify the common usecase of performing memory based http operations. 

## Usage
#### Simple put or get
If you have a basic use-case even the curly_init() function is optional and you perform PUT or GET as easy as:
```
char* data = "test";
curly_http_get("https://httpbin.org", NULL, NULL);
curly_http_put("https://httpbin.org/put/", data, strlen(data), NULL, NULL);
```
#### Using http headers
Header manipulation is done with a json array, for instance like this:
```
char* headers = "{\"Accept\":\"text/plain\"}";
curly_http_get("https://httpbin.org", headers, NULL);
```
Removing default headers follows the normal libcurl way of working so the example below will remove the Accept header from the request:
```
char* headers = "{\"Accept\":}";
curly_http_get("https://httpbin.org", headers, NULL);
```
#### Using ssl
By default curly enables peer verification (see curl docs for VerifyPeer). You can disable this check by setting do_not_verify_peer to 1. For iOS/OS-X the DarwinSSL backend is used so there is no real reason to do that. For other platforms using OpenSSL you must provide a list of trusted Certificate Authorities (CA). This list might be available in the OS already or you have to construct it. For Android I have a gist here: https://gist.github.com/johanlantz/6c53af21723ac9adfd00

## Installation
There are several ways to use curly depending on your platform and requirements.

### iOS
For iOS there is a .podspec available right here in the root so installation is simple. Public cocoapod releases are also published (https://cocoapods.org/pods/curly). The iOS version has no OpenSSL dependendy but instead uses DarwinSSL.

### Android
As usual with native code in Android it's a bit more complex than for iOS. Basically you have two options:

#### Use Curly as a static library
Use curly as a static lib together with libcrypto.a and libssl.a inside yourexisting dynamic library by adding the instructions below to your Android.mk (the order of the libs is important):
```
include $(CLEAR_VARS)
LOCAL_MODULE := curl-prebuilt
LOCAL_SRC_FILES := PATH_TO_CURLY/third-party/curl/android/$(TARGET_ARCH_ABI)/libcurl.a
include $(PREBUILT_STATIC_LIBRARY)
LOCAL_C_INCLUDES += PATH_TO_CURLY/third-party/ PATH_TO_CURLY/third-party/curl/android/include
LOCAL_STATIC_LIBRARIES += curl-prebuilt ssl-prebuilt crypto-prebuilt
```
Also keep in mind that if you intend to share the openssl lib with one or more projects in the same .so you can not initialize openssl more than once and you might have threading issues between the openssl users (there is plenty of information about this if you google for it). If you do not have another lib relying on openssl in the same .so you will have no issues, just link with the existing crypto libs on the android device or preferably with your own or the precompiled ones you find in the third-party folder since old Android versions have very old OpenSSL libs installed.

#### Use Curly as a dynamic library
This is a good option if you have other native libraries and you want to add http support. The prebuilt libcurly.so can be found in the android folder and it is already encapsulating libcrypto, libssl and libcurl. To add it to your existing .so project you would have to add the following in your Android.mk:
```
include $(CLEAR_VARS)
LOCAL_MODULE := curly-prebuilt
LOCAL_SRC_FILES := PATH_TO_CURLY/third-party/curly/android/libs/$(TARGET_ARCH_ABI)/libcurly.so
include $(PREBUILT_SHARED_LIBRARY)
LOCAL_C_INCLUDES + PATH_TO_CURLY/
LOCAL_SHARED_LIBRARIES += curly-prebuilt 
```

### Windows
The simplest approach is to just add curly.h and curly.c to your existing project and then poijt Visual Studio to your prebuilt curl and openssl libs (they are also provided in the third-party folder here). 

#### Unit tests
Unit tests using Google Test are provided as well as a pre-made solution for Visual Studio. 

Do not forget to run:
```
git submodule init
git submodule update
```

in order to fetch the GTest submodule needed for the unittests.

##### Visual Studio conversion of gtest solution
If you want to use the gtest unit-tests and Visual Studio says the gtest-md.sln file can not be converted to your version of Visual Studio you must first open the gtest-md.sln solution manually so the conversion can be performed. Then just open the CurlyTest.sln file and it should be ok.
