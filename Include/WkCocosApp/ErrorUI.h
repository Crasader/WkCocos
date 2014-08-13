#ifndef __ERRORUI_H__
#define __ERRORUI_H__

#include "ui/CocosGUI.h"

#include "WkCocos/Interface.h"

//#include "WkCocosApp/LoadingScene.h"

/**
* This UI is the main menu UI
*/
class ErrorUI : public WkCocos::Interface
{
public:
		
	static const std::string id;

	/**
	* Default constructor
	*/
	ErrorUI();

	/**
	* Destructor
	*/
	virtual ~ErrorUI();

	void setRefreshCallback(std::function<void()> cb);
	void setSkipCallback(std::function<void()> cb);

	void refreshCallback(cocos2d::Ref* widgetRef, cocos2d::ui::Widget::TouchEventType input);
	void skipCallback(cocos2d::Ref* widgetRef, cocos2d::ui::Widget::TouchEventType input);

	void activate(std::string msg);
	void deactivate();

	std::function<void()> m_refreshCB;
	std::function<void()> m_skipCB;

protected:
	cocos2d::ui::Button* m_refreshButton;
	cocos2d::ui::Button* m_skipButton;
	cocos2d::ui::Text* m_label;
};

#endif // __ERRORUI_H__