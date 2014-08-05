#ifndef __WKCOCOS_ONLINEDATA_ONLINEDATAMANAGER_H__
#define __WKCOCOS_ONLINEDATA_ONLINEDATAMANAGER_H__

#include "entityx/entityx.h"

#include "Common/App42API.h"

#define DEFAULT_SAVE_VERSION 1

namespace WkCocos
{
	namespace OnlineData
	{
		class OnlineDataManager
		{
		public:
			/**
			* Constructor 
			*/
			OnlineDataManager(std::string app_access_key, std::string app_secret_key);

			/**
			* Destructor
			*/
			~OnlineDataManager();
			
			/**
			* Create User
			*/
			void loginNew(std::string userid, std::string password, std::string email, std::function<void(::App42::App42UserResponse*)> callback);
			
			/**
			* Login User
			*/
			void login(std::string userid, std::string password, std::function<void(::App42::App42UserResponse*)> callback);

			/**
			* Save User Data
			*/
			void save(std::string userid, std::string user_data, std::function<void(std::string)> callback);

			/**
			* Load User Data
			*/
			void load(std::string userid, std::function<void(std::string)> callback);

			void update(double dt);

		protected :

			entityx::ptr<entityx::EventManager> event_manager;
			entityx::ptr<entityx::EntityManager> entity_manager;
			entityx::ptr<entityx::SystemManager> system_manager;

			std::function<void()> m_error_callback;
		};


	} // namespace OnlineData
} // namespace WkCocos



#endif // __WKCOCOS_ONLINEDATA_ONLINEDATAMANAGER_H__