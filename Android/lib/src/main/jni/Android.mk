LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := wkcocos_static

LOCAL_MODULE_FILENAME := libwkcocos

LOCAL_SRC_FILES := jni.cpp \
                   ../../../../../Classes/Lib/Utils/ToolBox.cpp \
                   ../../../../../Classes/Lib/Utils/libb64.c \
                   ../../../../../Classes/Lib/Utils/md5.c \
                   ../../../../../Classes/Lib/Utils/sha1.cpp \
                   ../../../../../Classes/Lib/Utils/Crypto.cpp \
                   ../../../../../Classes/Lib/Options.cpp \
                   ../../../../../Classes/Lib/Player.cpp \
                   ../../../../../Classes/Lib/Scene.cpp \
                   ../../../../../Classes/Lib/Interface.cpp \
                   ../../../../../Classes/Lib/Save/Local.cpp \
                   ../../../../../Classes/Lib/Save/Online.cpp \
                   ../../../../../Classes/Lib/App42/Setup.cpp \
                   ../../../../../Classes/Lib/App42/Login.cpp \
                   ../../../../../Classes/Lib/Loading/LoadingManager.cpp \
                   ../../../../../Classes/Lib/Loading/Systems/DataEval.cpp \
                   ../../../../../Classes/Lib/Loading/Systems/DLCchecking.cpp \
                   ../../../../../Classes/Lib/Loading/Systems/DLClisting.cpp \
                   ../../../../../Classes/Lib/Loading/Systems/DLvalidating.cpp \
                   ../../../../../Classes/Lib/Loading/Systems/CurlDL.cpp \
                   ../../../../../Classes/Lib/Loading/Systems/CurlMultiDL.cpp \
                   ../../../../../Classes/Lib/Loading/Systems/Error.cpp \
                   ../../../../../Classes/Lib/Loading/Systems/Loading.cpp \
                   ../../../../../Classes/Lib/Loading/Systems/MD5checking.cpp \
                   ../../../../../Classes/Lib/Loading/Systems/ProgressUpdate.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../../../Include

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../../../Include

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static
#LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
#LOCAL_WHOLE_STATIC_LIBRARIES += cocosbuilder_static
#LOCAL_WHOLE_STATIC_LIBRARIES += spine_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocostudio_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_network_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static
LOCAL_WHOLE_STATIC_LIBRARIES += libevent2_static
LOCAL_WHOLE_STATIC_LIBRARIES += entityx_static
LOCAL_WHOLE_STATIC_LIBRARIES += app42SDK_static

include $(BUILD_STATIC_LIBRARY)

$(call import-module,.)
$(call import-module,audio/android)
#$(call import-module,Box2D)
#$(call import-module,editor-support/cocosbuilder)
#$(call import-module,editor-support/spine)
$(call import-module,editor-support/cocostudio)
$(call import-module,network)
$(call import-module,extensions)
$(call import-module,libevent)
$(call import-module,entityx)
$(call import-module,App42_Cocos2DX_SDK)


