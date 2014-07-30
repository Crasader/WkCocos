#ifndef __TIMERUI_H__
#define __TIMERUI_H__

#include "cocos2d.h"
#include "cocos/ui/CocosGUI.h"

#include "WkCocos/Interface.h"

/**
* This UI is the main menu UI
*/
class TimerUI : public WkCocos::Interface
{
public:
		
	static const std::string id;

	/**
	* Default constructor
	*/
	TimerUI();

	/**
	* Destructor
	*/
	virtual ~TimerUI();
	
	void startCallback(cocos2d::Ref* widgetRef, cocos2d::ui::Widget::TouchEventType input);
	void stopCallback(cocos2d::Ref* widgetRef, cocos2d::ui::Widget::TouchEventType input);

protected:

	cocos2d::ui::Button* m_startButton;
	cocos2d::ui::Button* m_stopButton;
	cocos2d::ui::Text* m_startLabel;
	cocos2d::ui::Text* m_stopLabel;
	cocos2d::ui::Text* m_countLabel;
};


#endif // __TIMERUI_H__
