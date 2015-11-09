#include "OptionPanel.h"
#include "scene/StartScene.h"
#include "utils/PlotScript.h"
#include "utils/StringHandler.h"
#include "Detect.h"
#include "ui/PpJoyPanel.h"

#define NONE_IMAGE "none.png"

OptionPanel* OptionPanel::create(std::string name)
{
	OptionPanel *pRet = new OptionPanel(name);
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool OptionPanel::init()
{
	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(OptionPanel::onButtonClicked, this));
	}
	
	_container->setAnchorPoint(Vec2(0,0));
	_virtual = static_cast<CheckBox*>(Helper::seekWidgetByName(_root, "CheckBox_virtual"));
	_virtual->addEventListener(CC_CALLBACK_2(OptionPanel::selectedVirtualEvent, this));
	_touch = static_cast<CheckBox*>(Helper::seekWidgetByName(_root, "CheckBox_touch"));
	_touch->addEventListener(CC_CALLBACK_2(OptionPanel::selectedTouchEvent, this));
	
	_container->setScale(0.85f);
	_container->setOpacity(200);
	_container->runAction(FadeIn::create(0.2));
	_container->runAction(ScaleTo::create(0.2,1.0f,1.0f,1.0f));

	_root->setTouchEnabled(true);
	_container->setTouchEnabled(false);
	setKeyboardEnabled(true);

	initControl(Detect::shareDetect()->getPpJoy()->getEnabled(), Detect::shareDetect()->getTouched());
	return true;
}

void OptionPanel::initControl(bool virtualed, bool touched)
{
	_virtual->setSelectedState(virtualed);
	_touch->setSelectedState(touched);
}

void OptionPanel::selectedTouchEvent(Ref* sender,CheckBox::EventType type)
{
	switch (type)
	{
	case CheckBox::EventType::SELECTED:
		CCLOG("selected");//选中时响应
		Detect::shareDetect()->setTouched(true);
		break;
	case CheckBox::EventType::UNSELECTED:
		CCLOG("unselected");//未选中时响应
		Detect::shareDetect()->setTouched(false);
		break;
	default:
		break;
	}
}

void OptionPanel::selectedVirtualEvent(Ref* sender,CheckBox::EventType type)
{
	switch (type)
	{
	case CheckBox::EventType::SELECTED:
		CCLOG("selected");//选中时响应
		Detect::shareDetect()->getPpJoy()->setEnabled(true);
		break;
	case CheckBox::EventType::UNSELECTED:
		CCLOG("unselected");//未选中时响应
		Detect::shareDetect()->getPpJoy()->setEnabled(false);
		break;
	default:
		break;
	}
}

void OptionPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
		switch(tag)
		{
		case EXIT:
			
			break;
		case ADJUSTPOS:
			Detect::shareDetect()->getPpJoy()->adjustPosition(true);
			break;
		}
		destroy();
	}
}

void OptionPanel::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	if(keyCode == EventKeyboard::KeyCode::KEY_SPACE)
	{
		destroy();
	}
}
