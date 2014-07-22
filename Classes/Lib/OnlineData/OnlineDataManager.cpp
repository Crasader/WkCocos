#include "WkCocos/OnlineData/OnlineDataManager.h"

#include "WkCocos/OnlineData/Systems/User.h"

#include "WkCocos/OnlineData/Comp/OnlineData.h"

#include "cocos/cocos2d.h"
#include "Common/App42API.h"

namespace WkCocos
{
	namespace OnlineData
	{
		OnlineDataManager::OnlineDataManager(std::string app_access_key, std::string app_secret_key)
			: event_manager(entityx::EventManager::make())
			, entity_manager(entityx::EntityManager::make(event_manager))
			, system_manager(entityx::SystemManager::make(entity_manager, event_manager))
		{
			App42API::Initialize(app_access_key, app_secret_key);
			system_manager->add<Systems::User>();
			system_manager->configure();
		}
		
		void OnlineDataManager::loginNew(std::string userid, std::string password, std::string email, std::function<void(App42UserResponse*)> callback)
		{

			auto newentity = entity_manager->create();
			//new File component for each request. The aggregator system will detect duplicates and group them
			newentity.assign<Comp::Create>(userid, password, email, [=](App42UserResponse* r){

				if (r->isSuccess)
				{
					login(userid, password, callback);
				}
				else // if creation failed, 
				{
					//call the callback with error already
					callback(r);
				}
				
			});
			
		}

		void OnlineDataManager::login(std::string userid, std::string password, std::function<void(App42UserResponse*)> callback)
		{
			auto newentity = entity_manager->create();
			//new File component for each request. The aggregator system will detect duplicates and group them
			newentity.assign<Comp::Login>(userid, password, [=](App42UserResponse* r){
				callback(r);
			});
		}

		/*
		void OnlineDataManager::save(std::string userid, std::string profile_data, std::function<void(App42UserCustomResponse*)> callback)
		{
			auto newentity = entity_manager->create();
			//new File component for each request. The aggregator system will detect duplicates and group them
			newentity.assign<Comp::SaveProfile>(userid, profile_data, [=](App42UserCustomResponse* r){
				callback(r);
			});
		}

		void OnlineDataManager::load(std::string userid, std::function<void(App42UserCustomResponse*)> callback)
		{
			auto newentity = entity_manager->create();
			//new File component for each request. The aggregator system will detect duplicates and group them
			newentity.assign<Comp::LoadProfile>(userid, [=](App42UserCustomResponse* r){
				callback(r);
			});
		}
		*/

		void OnlineDataManager::update(double dt) {
			//check for error and report them if needed
			system_manager->update<Systems::User>(dt);
		}

	} // namespace LocalData
}  // namespace WkCocos
