#ifndef __DFGAME_PRELOAD_ENTITY_EVENTS_ERROR_H__
#define __DFGAME_PRELOAD_ENTITY_EVENTS_ERROR_H__

#include "entityx/entityx.h"
#include "WkCocos/Preload/Comp/DataLoad.h"

namespace WkCocos
{
	namespace Preload
	{
		namespace Events
		{
			struct Error : public entityx::Event<Error>
			{
				Error(entityx::Entity e, std::string system_msg) : errored_entity(e)
				{
					auto tempDL = e.component<Comp::DataLoad>();
					if (tempDL)
					{
						msg = system_msg + " " + tempDL->m_filepath;
					}
				}

				entityx::Entity errored_entity;

				std::string msg;

			};

		}//namespace Event
	}//namespace Preload
}//namespace WkCocos


#endif // __DFGAME_PRELOAD_ENTITY_EVENTS_ERROR_H__
