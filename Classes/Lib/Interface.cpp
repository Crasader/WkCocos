#include "WkCocos/Interface.h"
#include "WkCocos/Utils/ToolBox.h"

namespace WkCocos
{
	std::unordered_map<std::string, cocos2d::ui::Widget *> Interface::widget_cache;

	cocos2d::ui::Widget * Interface::load(std::string filepath)
	{
		auto uicached = widget_cache.find(filepath);
		if (uicached == widget_cache.end()) //ui not found in cache
		{
			cocostudio::GUIReader* reader = cocostudio::GUIReader::getInstance();

			cocos2d::ui::Widget* widget = reader->widgetFromJsonFile(filepath.c_str());
			if (widget)
			{
				widget->retain(); //we need to retain it in memory ( or cocos will drop it )
				widget_cache.insert(std::pair<std::string, cocos2d::ui::Widget*>(filepath, widget));
				return widget;
			}
			//error cannot read Json UI
			return nullptr;
		}
		else
		{
			//return already loaded UI
			return uicached->second;
		}
	}
	
	void Interface::forget(std::string filepath)
	{
		auto uicached = widget_cache.find(filepath);
		if (uicached != widget_cache.end()) //ui found in cache
		{
			uicached->second->release();
			widget_cache.erase(filepath);
		}
	}

	Interface::Interface()
		: m_widget(nullptr)
		, m_filepath("")
	{
	}

	Interface::Interface(std::string filepath)
		try : m_filepath(filepath)
		, m_widget(nullptr)
	{
		CCLOG("Interface construction");

		//if in cache it will already find it. otherwise it will populate cache
		m_widget = load(m_filepath);

		if (!m_widget)
		{
			CCLOGERROR("Interface construction ERROR !");
			throw std::logic_error("Error while constructing widget. aborting");
		}
	}
	catch (std::exception)
	{
		CCLOGERROR("ERROR BUILDING INTERFACE");
	}

	Interface::~Interface()
	{
		forget(m_filepath);
	}
	
	void Interface::setVisible(bool visible)
	{
		m_widget->setVisible(visible);
		for (cocos2d::experimental::ui::WebView* wv : m_webviews)
		{
			wv->setVisible(visible);
		}
	}
	
	void Interface::addChild(cocos2d::Node * child)
	{
		addWebView(child);
		m_widget->addChild(child);
	}

	void Interface::addWebView(cocos2d::Node * child)
	{
		if ("WebView" == child->getDescription())
		{
			auto childcast = dynamic_cast<cocos2d::experimental::ui::WebView*>(child);
			m_webviews.insert(childcast);
		}
		//recurse
		for( cocos2d::Node* node : child->getChildren())
		{
			addWebView(node);
		}
	}

	void Interface::removeChild(cocos2d::Node * child)
	{
		removeWebView(child);
		m_widget->removeChild(child);
	}
	
	void Interface::removeWebView(cocos2d::Node * child)
	{
		if ("WebView" == child->getDescription())
		{
			auto childcast = dynamic_cast<cocos2d::experimental::ui::WebView*>(child);
			m_webviews.erase(childcast);
		}
		//recurse
		for (cocos2d::Node* node : child->getChildren())
		{
			removeWebView(node);
		}
	}

	void Interface::update(float delta)
	{
		//NOTHING to do here by default.
	}

	cocos2d::ui::Widget* Interface::getWidgetInHierarchy(const std::string& name, cocos2d::ui::Widget* root/* = nullptr*/)
	{
		cocos2d::ui::Widget* found = nullptr;
		if (!root)
		{
			root = m_widget;
		}
		if (root->getName() == name)
		{
			found = root;
		}
		else
		{
			auto currentChildren = root->getChildren().begin();
			while (currentChildren != root->getChildren().end() && found == nullptr)
			{
				found = getWidgetInHierarchy(name, static_cast<cocos2d::ui::Widget*>(*currentChildren));
				++currentChildren;
			}
		}

		return found;
	}

} //namespace dfgame