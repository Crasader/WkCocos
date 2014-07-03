#ifndef __DFGAME_LOADING_ENTITY_COMP_PROGRESSVALUE_H__
#define __DFGAME_LOADING_ENTITY_COMP_PROGRESSVALUE_H__

#include "entityx/entityx.h"

namespace dfgame
{
	namespace Loading
	{
		namespace Comp
		{

			struct ProgressValue : entityx::Component<ProgressValue> {
				ProgressValue(unsigned short a_progval) : progval(a_progval) {}

				unsigned short progval;
			};

		}//namespace Comp
	}//namespace Loading
}//namespace dfgame

#endif // __DFGAME_LOADING_ENTITY_COMP_PROGRESSVALUE_H__
