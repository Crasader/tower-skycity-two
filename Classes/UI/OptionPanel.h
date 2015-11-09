#ifndef __OPTION_PANEL_H__
#define __OPTION_PANEL_H__

#include "UI/SimplePanel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class OptionPanel : public SimplePanel
{	
public:
	enum buttons
	{
		EXIT = 1,
		ADJUSTPOS
	};
	OptionPanel(std::string name):SimplePanel(name){}
	static OptionPanel* create(std::string name);
	bool init();
private:
	CheckBox* _virtual;
	CheckBox* _touch;
	void initControl(bool virtualed, bool touched);
	void selectedTouchEvent(Ref* sender,CheckBox::EventType type);
	void selectedVirtualEvent(Ref* sender,CheckBox::EventType type);
	virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
};


#endif /*__OPTION_PANEL_H__*/