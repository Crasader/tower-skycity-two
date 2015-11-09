#include "TrialCheckPanel.h"
#include "scene/StartScene.h"
#include "utils/PlotScript.h"
#include "utils/StringHandler.h"
#include "ui/UIScrollView.h"
#include "Global.h"
#include "cell/CellFactory.h"
#include "ui/PopPanel.h"
#include "Detect.h"
#include "utils/Tools.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "platform\android\jni\JniHelper.h"
#endif

#define NONE_IMAGE "none.png"

TrialCheckPanel* TrialCheckPanel::create(std::string name)
{
	TrialCheckPanel *pRet = new TrialCheckPanel(name);
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

void TrialCheckPanel::selectedItemEvent(Ref *pSender, ListView::EventType type)
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
			break;
		}
	default:
		break;
	}
}

void TrialCheckPanel::selectedItemEventScrollView(Ref* pSender, ui::ScrollView::EventType type)
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

bool TrialCheckPanel::init()
{
	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(TrialCheckPanel::onButtonClicked, this));
	}
	
	//_container->setTouchEnabled(true);
	_container->setAnchorPoint(Vec2(0,0));
	_listView = static_cast<ListView*>(Helper::seekWidgetByName(_root, "ListView_content"));

	_trialUnit = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_trialUnit"));
	_trialUnit->retain();
	_trialUnit->removeFromParent();

	_listView->addEventListener((ui::ListView::ccListViewCallback)CC_CALLBACK_2(TrialCheckPanel::selectedItemEvent, this));
	_listView->addEventListener((ui::ListView::ccScrollViewCallback)CC_CALLBACK_2(TrialCheckPanel::selectedItemEventScrollView,this));
	
	_container->setScale(0.85f);
	_container->setOpacity(200);
	_container->runAction(FadeIn::create(0.2));
	_container->runAction(ScaleTo::create(0.2,1.0f,1.0f,1.0f));

	_root->setTouchEnabled(true);
	_container->setTouchEnabled(false);
	_root->addTouchEventListener(CC_CALLBACK_2(TrialCheckPanel::onButtonClicked, this));
	setKeyboardEnabled(true);
	return true;
}

TrialCheckPanel::~TrialCheckPanel()
{
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(TrialCheckPanel::timeUpdate),this);
}

void TrialCheckPanel::timeUpdate(float delay)
{
	auto initTime = _trial->getInitTime();
	auto curTime = getTime();
	int durTime = difftime(curTime, initTime);
	int shi = durTime/3600;
	int fen = durTime/60%60;
	int miao = durTime%60;
	char date[32] = {0};
	sprintf(date, "%02d:%02d:%02d",shi,fen,miao);
	_timeLab->setString(StringUtils::format("%s",date));
}

void TrialCheckPanel::initWithTrial(Trial* trial, bool last)
{
	if(trial == nullptr) return;
	_trial = trial;
	
	_reset = static_cast<Button*>(Helper::seekWidgetByName(_root, "Button_reset"));
	_back = static_cast<Button*>(Helper::seekWidgetByName(_root, "Button_back"));
	_reset->setVisible(false);
	_back->setVisible(false);
	if(last == true)
	{
		_reset->setVisible(true);
		_back->setVisible(true);
	}
	// 统计 时间 次数
	_timeLab = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_time"));

	timeUpdate(0);
	if(last == false)
		Director::getInstance()->getScheduler()->schedule(schedule_selector(TrialCheckPanel::timeUpdate),this,1.0f,-1,0.0,false);

	// 成就完成
	Vector<Achievement*> newAchieves;
	auto achievements = trial->getAchievements();
	for(auto achievement:achievements)
	{
		if(achievement->getReached() == false)
		{
			newAchieves.pushBack(achievement);
		}
	}

	for(auto achieve:achievements)
	{
		auto trialUnit = _trialUnit->clone();
		
		auto tagLab = static_cast<Text*>(Helper::seekWidgetByName(trialUnit, "Label_tag"));
		auto numLab = static_cast<Text*>(Helper::seekWidgetByName(trialUnit, "Label_num"));
		auto image = static_cast<ImageView*>(Helper::seekWidgetByName(trialUnit, "Image_image"));
		auto yesLab = static_cast<Text*>(Helper::seekWidgetByName(trialUnit, "Label_yes"));
		auto noLab = static_cast<Text*>(Helper::seekWidgetByName(trialUnit, "Label_no"));

		yesLab->setVisible(false);
		noLab->setVisible(false);
		if(achieve->checkConditions() == true)
			yesLab->setVisible(true);
		else
			noLab->setVisible(true);

		numLab->setString("X" + achieve->getModelByName("reward").asString());
		tagLab->setString(achieve->getModelByName("introduce").asString());

		trialUnit->setUserData(trial);
		_listView->pushBackCustomItem(trialUnit);
	}
}

void TrialCheckPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
		switch(tag)
		{
		case RESET:
			
			break;
		case BACK:

			break;
		}
		destroy();
	}
}

void TrialCheckPanel::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	if(keyCode == EventKeyboard::KeyCode::KEY_SPACE)
	{
		destroy();
	}
}