#include "WkCocos/Loading/LoadingManager.h"

#include "WkCocos/Loading/Systems/Error.h"
#include "WkCocos/Loading/Systems/DataEval.h"
#include "WkCocos/Loading/Systems/DLClisting.h"
#include "WkCocos/Loading/Systems/DLCchecking.h"
#include "WkCocos/Loading/Systems/MD5checking.h"
#include "WkCocos/Loading/Systems/CurlDL.h"
#include "WkCocos/Loading/Systems/DLvalidating.h"
#include "WkCocos/Loading/Systems/Loading.h"
#include "WkCocos/Loading/Systems/ProgressUpdate.h"

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"

#include "WkCocos/Utils/ToolBox.h"

#include <climits>
#include <vector>

namespace WkCocos
{
	namespace Loading
	{
		void LoadingManager::addDataDownload(const std::string json_manifest_filename)
		{
			cocos2d::Data manifest_data = cocos2d::FileUtils::getInstance()->getDataFromFile(json_manifest_filename);
				
			rapidjson::Document json;

			std::string manifestStr = cocos2d::FileUtils::getInstance()->getStringFromFile(json_manifest_filename);

			json.Parse<0>(manifestStr.c_str());
			if (json.HasParseError()) {
				CCLOG("GetParseError %s\n", json.GetParseError());
			}
				//"packageUrl" : "http://example.com/assets_manager/TestScene/",
				//"remoteVersionUrl" : "http://example.com/assets_manager/TestScene/version.manifest",
				//"remoteManifestUrl" : "http://example.com/assets_manager/TestScene/project.manifest",
				//"version" : "1.0.0",
				//"engineVersion" : "Cocos2d-JS 3.0 beta",

			bool dlcEnable = cocostudio::DictionaryHelper::getInstance()->getBooleanValue_json(json, "dlcEnable", "error");
			std::string dlcUrl = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(json, "dlcUrl", "error");
			//std::string packageUrl = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(json,"packageUrl", "error") ;
			//std::string remoteVersionUrl = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(json, "remoteVersionUrl", "error");
			//std::string remoteManifestUrl = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(json, "remoteManifestUrl", "error");
			std::string minAppVersion = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(json, "minAppVersion", "error");
			std::string version = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(json, "version", 0);
			//std::string engineVersion = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(json, "engineVersion", "error");

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			// If we can find "localhost". Replace it with emulator access to localhost.
			dlcUrl.replace(dlcUrl.find("localhost"), 9, "10.0.2.2");
#endif
				


			unsigned long lver = 0;
			try {
				 lver = ToolBox::stoul(version);
				CCLOG("Manifest version %u for DLC at %s ", lver, dlcUrl.c_str());
			}
			catch (std::out_of_range oor)
			{
				lver = LONG_MAX;
			}

			if (dlcEnable)
			{
				entityx::Entity entity = entity_manager->create();
				entity.assign<Comp::DataListDownload>(dlcUrl, lver , minAppVersion);
				entity.assign<Comp::ProgressValue>(1);
			}
			else
			{
				CCLOG("Manifest IGNORED !");
			}

		}

		bool LoadingManager::addDataLoad(const std::vector<std::string> &  filepath)
		{
			for (auto path : filepath)
			{
				entityx::Entity entity = entity_manager->create();
				entity.assign<Comp::DataLoad>(path);
				entity.assign<Comp::ProgressValue>(1);
			}
			return true;
		}

		void LoadingManager::configure()
		{
			system_manager->add<Systems::Error>(m_error_callback);
			system_manager->add<Systems::DataEval>();
			system_manager->add<Systems::DLClisting>();
			system_manager->add<Systems::DLCchecking>();
			system_manager->add<Systems::MD5checking>();
			system_manager->add<Systems::CurlDL>();
			system_manager->add<Systems::DLvalidating>();
			system_manager->add<Systems::ASyncLoading>(m_concurrent_loads);
			system_manager->add<Systems::SyncLoading>();
			system_manager->add<Systems::ProgressUpdate>(m_progress_callback);
		};

		void LoadingManager::initialize()
		{
			//adding writable path ( where DLC downloads) as search path. First in list
			int i = 0;
			cocos2d::FileUtils *fileUtils = cocos2d::FileUtils::getInstance();
			std::vector<std::string> searchPaths = fileUtils->getSearchPaths();
			searchPaths.insert(searchPaths.begin() + i++, cocos2d::FileUtils::getInstance()->getWritablePath());
			//add more if needed
			fileUtils->setSearchPaths(searchPaths);
		}

		void LoadingManager::update(double dt) {

			//check for error and report them if needed
			system_manager->update<Systems::Error>(dt);
			//evaluate entities containing DataLoad components
			system_manager->update<Systems::DataEval>(dt);
			//listing versions avialable on DLC
			system_manager->update<Systems::DLClisting>(dt);
			//listing files in one version on DLC
			system_manager->update<Systems::DLCchecking>(dt);
			//check MD5 of files existing and downloaded
			system_manager->update<Systems::MD5checking>(dt);
			//dot he curl calls when needed
			system_manager->update<Systems::CurlDL>(dt);
			//validates if signature matches
			system_manager->update<Systems::DLvalidating>(dt);
			//asynchronously load data
			system_manager->update<Systems::ASyncLoading>(dt);
			//synchronously load data
			system_manager->update<Systems::SyncLoading>(dt);

			//display the progress
			system_manager->update<Systems::ProgressUpdate>(dt);

		}
			
	} // namespace Loading
}  // namespace dfgame
