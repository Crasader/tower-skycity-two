#ifndef __ABOUT_PANEL_H__
#define __ABOUT_PANEL_H__

#include "UI/SimplePanel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;

class LogoPanel : public SimplePanel
{	
public:
	enum buttons
	{
	};
	LogoPanel(std::string name):SimplePanel(name){}
	static LogoPanel* create(std::string name);
	bool init();
private:
	virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void end();
	ImageView* _logo;
};


#endif /*__ABOUT_PANEL_H__*/