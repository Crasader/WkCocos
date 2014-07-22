#include "WkCocos/Loading/Systems/DataEval.h"

#include "WkCocos/Loading/Events/Error.h"
#include "WkCocos/Loading/Events/Downloaded.h"
#include "WkCocos/Interface.h"

namespace WkCocos
{
	namespace Loading
	{
		namespace Systems
		{
			/**
			* This function will choose a loader depending on the extension of the filename.
			*/
			void DataEval::chooseLoader(entityx::Entity entity, std::string datafile, entityx::ptr<entityx::EventManager> events)
			{
				//if png we can load asynchronously
				if (datafile.compare(datafile.size() - 4, 4, ".png") == 0)
				{
					//assigning the proper loadfunc component
					entity.assign<Comp::LoadASyncFunc>(datafile, 0, [datafile](std::function<void(cocos2d::Texture2D*)> endcb){
						cocos2d::Director::getInstance()->getTextureCache()->addImageAsync(datafile, endcb);
					});
				}
				//but plist has to be loaded synchronously
				else if (datafile.compare(datafile.size() - 6, 6, ".plist") == 0)
				{
					//assigning the proper loadfunc component
					entity.assign<Comp::LoadSyncFunc>(datafile, 0, [datafile](){
						cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(datafile);
					});
				}
				//and json needs to be loaded with specific loader.
				else if (datafile.compare(datafile.size() - 8, 8, ".ui.json") == 0)
				{
					entityx::ptr<Comp::DataLoad> dataload = entity.component<Comp::DataLoad>();
					std::string file = dataload->getFilepath();
					//assigning the proper loadfunc component
					entity.assign<Comp::LoadSyncFunc>(file, 0, [file](){
						Interface::load(file);
					});
				}
				else if (datafile.compare(datafile.size() - 10, 10, ".anim.json") == 0)
				{
					//TODO
					//assigning the proper loadfunc component
					//entity.assign<Comp::LoadSyncFunc>(0, [respath](){

					//});
				}
				else if (datafile.compare(datafile.size() - 11, 11, ".scene.json") == 0)
				{
					//TODO
					//assigning the proper loadfunc component
					//entity.assign<Comp::LoadSyncFunc>(0, [respath](){

					//});
				}
				else
				{
					//Signal Error
					events->emit<Events::Error>(entity);
					//we cant do more with this entity. kill it.
					entity.destroy();
				}
			}

			void DataEval::update(entityx::ptr<entityx::EntityManager> es, entityx::ptr<entityx::EventManager> events, double dt)
			{
				for (auto entity : es->entities_with_components<Comp::DataLoad>())
				{
					entityx::ptr<Comp::DataLoad> dataload = entity.component<Comp::DataLoad>();

					//we need to wait for the file to be there ( might come with DLC), otherwise cocos loading will fail.
					if (cocos2d::FileUtils::getInstance()->isFileExist(dataload->getFilepath()))
					{
						//generate the appropriate loader component
						chooseLoader(entity, dataload->getFilepath(), events);

						//remove the component we just processed
						entity.remove<Comp::DataLoad>();
					}

				}

			}

			void DataEval::receive(const Events::Downloaded &dl) {}
					
		}//namespace Systems
	}//namespace Loading
}//namespace WkCocos

