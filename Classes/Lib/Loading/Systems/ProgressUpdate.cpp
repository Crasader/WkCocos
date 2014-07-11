#include "WkCocos/Loading/Systems/ProgressUpdate.h"

#include "WkCocos/Loading/Events/Error.h"

namespace WkCocos
{
	namespace Loading
	{
		namespace Systems
		{
			ProgressUpdate::ProgressUpdate(std::function<void(float)> a_setProgressPercent)
			: setProgressPercent(a_setProgressPercent)
			, totalProgValMax(0)
			, curProgVal(0)
			{
			}

			void ProgressUpdate::update(entityx::ptr<entityx::EntityManager> es, entityx::ptr<entityx::EventManager> events, double dt) 
			{
				unsigned int new_curProgVal = 0;
				for (auto entity : es->entities_with_components<Comp::ProgressValue>()) 
				{
					entityx::ptr<Comp::ProgressValue> pv = entity.component<Comp::ProgressValue>();
					new_curProgVal += pv->progval;
				}

				//to keep max the max, even when we add data to load while already loading.
				if (new_curProgVal > curProgVal)
				{
					totalProgValMax = new_curProgVal + totalProgValMax - curProgVal;
				}

				curProgVal = new_curProgVal;
				if (0 == totalProgValMax) //nothing to do
				{
					setProgressPercent(1.0f);
				}
				else
				{
					setProgressPercent(1.0f - (float)curProgVal / (float)totalProgValMax);
				}
			};

					
		}//namespace Systems
	}//namespace Loading
}//namespace WkCocos

