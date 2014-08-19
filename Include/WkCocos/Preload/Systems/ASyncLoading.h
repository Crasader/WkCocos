#ifndef __DFGAME_PRELOAD_ENTITY_SYSTEMS_ASYNCLOADING_H__
#define __DFGAME_PRELOAD_ENTITY_SYSTEMS_ASYNCLOADING_H__

#include "entityx/entityx.h"
#include "WkCocos/Preload/Comp/DataLoad.h"

namespace WkCocos
{
	namespace Preload
	{
		namespace Systems
		{
			//system managing async loading. max_concurrent at a time.
			struct ASyncLoading : public entityx::System<ASyncLoading>
			{
				ASyncLoading(unsigned short max_concurrent);

				void update(entityx::ptr<entityx::EntityManager> es, entityx::ptr<entityx::EventManager> events, double dt) override;

				unsigned short m_max_concurrent;
				unsigned short m_cur_concurrent;
			};

		}//namespace Systems
	}//namespace Preload
}//namespace WkCocos

#endif // __DFGAME_PRELOAD_ENTITY_SYSTEMS_ASYNCLOADING_H__
