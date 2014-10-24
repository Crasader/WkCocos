#ifndef __WKCOCOS_LOCALDATA_LOCALDATAMANAGER_H__
#define __WKCOCOS_LOCALDATA_LOCALDATAMANAGER_H__

#include "entityx/entityx.h"
#include "WkCocos/Actor.h"
#include "WkCocos/LocalData/Events/Error.h"
//#include "json/document.h"

#define DEFAULT_SAVE_VERSION 1

namespace WkCocos
{
	namespace LocalData
	{
		class LocalDataManager : public WkCocos::Actor
		{
		public:
			/**
			* Constructor 
			*/
			LocalDataManager();

			/**
			* Destructor
			*/
			~LocalDataManager();
			
			//TODO : Actor behavior
			struct LoginIdSaved : public Event < LoginIdSaved >
			{

			};

			struct LoginIdLoaded : public Event < LoginIdLoaded >
			{

			};

			struct DataSaved : public Event < DataSaved >
			{

			};

			struct DataLoaded : public Event < DataLoaded >
			{

			};


			/**
			* LoginID
			*/
			bool saveLoginID(std::string user, std::string passwd, std::string key = "", short version = DEFAULT_SAVE_VERSION);
			bool loadLoginID(std::function<void(std::string user, std::string passwd)> load_cb, std::string key = "", short version = DEFAULT_SAVE_VERSION);
			
			/**
			* PlayerData ( similar interface to OnlineDataManager )
			*/
			bool saveData(const std::string& saveName, std::string data, std::string key = "", short version = DEFAULT_SAVE_VERSION);
			bool loadData(const std::string& saveName, std::function<void(std::vector<std::string> data)> load_cb, std::string key = "", short version = DEFAULT_SAVE_VERSION);
			bool deleteData(const std::string& saveName, std::function<void(std::string data)> delete_cb, short version = DEFAULT_SAVE_VERSION);

			void update(double dt);

			inline entityx::ptr<entityx::EventManager> getEventManager() { return event_manager; }

		protected :

			entityx::ptr<entityx::EventManager> event_manager;
			entityx::ptr<entityx::EntityManager> entity_manager;
			entityx::ptr<entityx::SystemManager> system_manager;

			//std::function<void(std::string)> m_error_callback;

		};


	} // namespace LocalData
} // namespace WkCocos



#endif // __WKCOCOS_LOCALDATA_LOCALDATAMANAGER_H__