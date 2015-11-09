#include "TrialPanel.h"
#include "scene/StartScene.h"
#include "utils/PlotScript.h"
#include "utils/StringHandler.h"
#include "ui/UIScrollView.h"
#include "Global.h"
#include "cell/CellFactory.h"
#include "ui/PopPanel.h"
#include "Detect.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "platform\android\jni\JniHelper.h"
#endif

#define NONE_IMAGE "none.png"

TrialPanel* TrialPanel::create(std::string name)
{
	TrialPanel *pRet = new TrialPanel(name);
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

void TrialPanel::selectedItemEvent(Ref *pSender, ListView::EventType type)
{
	switch (type)
	{
	case cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_START:
		{
			ListView* listView = static_cast<ListView*>(pSender);
			CC_UNUSED_PARAM(listView);
			CCLOG("select child start index = %ld", listView->getCurSelectedIndex());
			//auto tag = listView->getItem(listView->getCurSelectedIndex())->getTag();
			//auto endStatUnit = _endStatsUnit.at(listView->getCurSelectedIndex());
			break;
		}
	case cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_END:
		{
			ListView* listView = static_cast<ListView*>(pSender);
			CC_UNUSED_PARAM(listView);
			CCLOG("select child end index = %ld", listView->getCurSelectedIndex());

			/*auto unit = listView->getItem(listView->getCurSelectedIndex());
			auto achieve = static_cast<Achievement*>(unit->getUserData());*/
			break;
		}
	default:
		break;
	}
}

void TrialPanel::selectedItemEventScrollView(Ref* pSender, ui::ScrollView::EventType type)
{
	switch (type) {
	case ui::ScrollView::EventType::SCROLL_TO_BOTTOM:
		CCLOG("SCROLL_TO_BOTTOM");
		break;
	case ui::ScrollView::EventType::SCROLL_TO_TOP:
		CCLOG("SCROLL_TO_TOP");
		break;
	default:
		break;
	}
}

bool TrialPanel::init()
{
	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(TrialPanel::onButtonClicked, this));
	}
	_container->setTouchEnabled(true);
	_container->setAnchorPoint(Vec2(0,0));
	_listView = static_cast<ListView*>(Helper::seekWidgetByName(_root, "ListView_content"));

	_achieveUnit = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_achieveUnit"));
	_achieveUnit->retain();
	_achieveUnit->removeFromParent();

	_listView->addEventListener((ui::ListView::ccListViewCallback)CC_CALLBACK_2(TrialPanel::selectedItemEvent, this));
	_listView->addEventListener((ui::ListView::ccScrollViewCallback)CC_CALLBACK_2(TrialPanel::selectedItemEventScrollView,this));
	
	_container->setScale(0.85f);
	_container->setOpacity(200);
	_container->runAction(FadeIn::create(0.2));
	_container->runAction(ScaleTo::create(0.2,1.0f,1.0f,1.0f));

	_root->setTouchEnabled(true);
	_container->setTouchEnabled(false);
	_root->addTouchEventListener(CC_CALLBACK_2(TrialPanel::onButtonClicked, this));
	setKeyboardEnabled(true);
	return true;
}

void TrialPanel::initWithTrial(Trial* trial)
{
	_trial = trial;

	auto nameLab = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_name"));
	nameLab->setString(trial->getModelByName("nickName").asString());

	auto introLab = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_intro"));
	introLab->setString(trial->getModelByName("introduce").asString());

	Vector<Achievement*> newAchieves;
	auto achievements = trial->getAchievements();
	for(auto achievement:achievements)
	{
		newAchieves.pushBack(achievement);
	}
	// µ÷ÕûË³Ðò
	auto begin = newAchieves.begin();
	auto end = newAchieves.end();
	for(auto iter = begin;iter != end;)
	{
		if((*iter)->getReached() == true)
		{
			auto remove = iter;
			iter = newAchieves.erase(iter);
			newAchieves.pushBack((*remove));
		}
		else
			iter++;
	}

	int i = 0;
	for(auto achieve:newAchieves)
	{
		auto achieveUnit = _achieveUnit->clone();
		
		auto numLab = static_cast<Text*>(Helper::seekWidgetByName(achieveUnit, "Label_num"));
		auto image = static_cast<ImageView*>(Helper::seekWidgetByName(achieveUnit, "Image_image"));
		auto tagLab = static_cast<Text*>(Helper::seekWidgetByName(achieveUnit, "Label_tag"));
		auto starLab = static_cast<Text*>(Helper::seekWidgetByName(achieveUnit, "Label_star"));
		
		tagLab->setString(achieve->getModelByName("introduce").asString());
		if(achieve->getReached() == true)
		{
			image->setVisible(false);
			starLab->setVisible(false);
			numLab->setVisible(false);
			achieveUnit->setOpacity(100);
		}
		achieveUnit->setUserData(achieve);
		achieveUnit->setTag(achieve->getId());
		_listView->pushBackCustomItem(achieveUnit);
	}
}

void TrialPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
		switch(tag)
		{
		case BEGIN:
			_trial->start();
			break;
		}
		destroy();
	}
}

void TrialPanel::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	if(keyCode == EventKeyboard::KeyCode::KEY_SPACE)
	{
		destroy();
	}
}