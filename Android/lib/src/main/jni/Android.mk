LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := wkcocos_static

LOCAL_MODULE_FILENAME := libwkcocos

LOCAL_SRC_FILES := jni.cpp \
                   ../../../../../Classes/Lib/Utils/ToolBox.cpp \
                   ../../../../../Classes/Lib/Utils/libb64.c \
                   ../../../../../Classes/Lib/Utils/sha1.cpp \
                   ../../../../../Classes/Lib/Utils/Crypto.cpp \
                   ../../../../../Classes/Lib/Utils/UUID.cpp \
                   ../../../../../Classes/Lib/Utils/WkJniHelper.cpp \
				   ../../../../../Classes/Lib/Utils/GPGSManager.cpp \
                   ../../../../../Classes/Lib/Options.cpp \
                   ../../../../../Classes/Lib/Player.cpp \
                   ../../../../../Classes/Lib/Scene.cpp \
                   ../../../../../Classes/Lib/Interface.cpp \
                   ../../../../../Classes/Lib/Download/Download.cpp \
                   ../../../../../Classes/Lib/Download/Version.cpp \
                   ../../../../../Classes/Lib/Preload/Preload.cpp \
                   ../../../../../Classes/Lib/Utils/CocosAppender.cpp \
                   ../../../../../Classes/Lib/Utils/log/logappender.cpp \
                   ../../../../../Classes/Lib/Utils/log/loglevel.cpp \
                   ../../../../../Classes/Lib/Utils/log/logstream.cpp \
                   ../../../../../Classes/Lib/Utils/log/logstreambuf.cpp \
                   ../../../../../Classes/Lib/Utils/log/nullstream.cpp \
                   ../../../../../Classes/Lib/Utils/log/nullstreambuf.cpp \
                   ../../../../../Classes/Lib/Download/Systems/DLCchecking.cpp \
                   ../../../../../Classes/Lib/Download/Systems/DLClisting.cpp \
                   ../../../../../Classes/Lib/Download/Systems/DLvalidating.cpp \
                   ../../../../../Classes/Lib/Download/Systems/CurlDL.cpp \
                   ../../../../../Classes/Lib/Download/Systems/MD5checking.cpp \
                   ../../../../../Classes/Lib/Download/Systems/ProgressUpdate.cpp \
                   ../../../../../Classes/Lib/Preload/Systems/DataEval.cpp \
                   ../../../../../Classes/Lib/Preload/Systems/SyncLoading.cpp \
                   ../../../../../Classes/Lib/Preload/Systems/ASyncLoading.cpp \
                   ../../../../../Classes/Lib/Preload/Systems/ProgressUpdate.cpp \
                   ../../../../../Classes/Lib/LocalData/LocalDataManager.cpp \
                   ../../../../../Classes/Lib/LocalData/Systems/JSONLoginID.cpp \
                   ../../../../../Classes/Lib/LocalData/Systems/JSONReader.cpp \
                   ../../../../../Classes/Lib/LocalData/Systems/JSONPlayerData.cpp \
                   ../../../../../Classes/Lib/LocalData/Systems/JSONWriter.cpp \
                   ../../../../../Classes/Lib/OnlineData/OnlineDataManager.cpp \
                   ../../../../../Classes/Lib/OnlineData/Comp/OnlineData.cpp \
                   ../../../../../Classes/Lib/OnlineData/Systems/Storage.cpp \
                   ../../../../../Classes/Lib/OnlineData/Systems/User.cpp \
				   ../../../../../Classes/Lib/OnlineData/Systems/Timer.cpp \
                   ../../../../../Classes/Lib/Shop/Shop.cpp \
                   ../../../../../Classes/Lib/Shop/Inventory.cpp \
                   ../../../../../Classes/Lib/Shop/Assets.cpp \
                   ../../../../../Classes/Lib/Shop/SOOMLA/ShopEventHandler.cpp \
                   ../../../../../Classes/Lib/Shop/SOOMLA/ShopAssets.cpp \
                   ../../../../../Classes/Lib/Timer/Timer.cpp \
                   ../../../../../Classes/Lib/Timer/Systems/AlarmDestructor.cpp \
                   ../../../../../Classes/Lib/Action/NodeAction.cpp \
                   ../../../../../Classes/Lib/Action/UIAction.cpp \
                   ../../../../../Classes/Lib/Timer/Systems/AlarmRinger.cpp \
                   ../../../../../Classes/Lib/Timer/Systems/TimeUpdater.cpp \
                   ../../../../../Classes/Lib/StrongBox/StrongBox.cpp \
                   ../../../../../Classes/Lib/Save.cpp \
				   ../../../../../Classes/Lib/Actor.cpp \
				   ../../../../../Classes/Lib/World.cpp \
                   ../../../../../Classes/Lib/WebView/UIWebView.cpp \
                   ../../../../../Classes/Lib/WebView/UIWebViewImpl-android.cpp \
                   ../../../../../Classes/Lib/Helper/GameLogic.cpp \
                   ../../../../../Classes/Lib/PushNotifications/PushNotifications.cpp

#do not include this file, it's already in libcurl
#                   ../../../../../Classes/Lib/Utils/md5.c \

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../../../Include

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../../../Include

LOCAL_WHOLE_STATIC_LIBRARIES += entityx_static
#This is for cocos2d-x
LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static
#LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
#LOCAL_WHOLE_STATIC_LIBRARIES += cocosbuilder_static
#LOCAL_WHOLE_STATIC_LIBRARIES += spine_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocostudio_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_network_static
#cocos SDKs
LOCAL_WHOLE_STATIC_LIBRARIES += app42SDK_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_store_static

include $(BUILD_STATIC_LIBRARY)

$(call import-module,entityx)
#This is for cocos2d-x
$(call import-module,.)
$(call import-module,audio/android)
#$(call import-module,Box2D)
#$(call import-module,editor-support/cocosbuilder)
#$(call import-module,editor-support/spine)
$(call import-module,editor-support/cocostudio)
$(call import-module,network)
#cocos SDKs
$(call import-module,App42_Cocos2DX_SDK)
$(call import-module,cocos2dx-store/android/jni)

