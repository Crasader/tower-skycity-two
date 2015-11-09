#include "TrialsPanel.h"
#include "scene/StartScene.h"
#include "utils/PlotScript.h"
#include "utils/StringHandler.h"
#include "ui/UIScrollView.h"
#include "Global.h"
#include "cell/CellFactory.h"
#include "ui/PopPanel.h"
#include "Detect.h"
#include "cell/item/ItemBag.h"
#include "cell/actor/Actor.h"
#include "cell/Cell.h"
#include "map/Trial.h"
#include "UI/EndStatPanel.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "platform\android\jni\JniHelper.h"
#endif

#define NONE_IMAGE "none.png"

TrialsPanel* TrialsPanel::create(std::string name)
{
	TrialsPanel *pRet = new TrialsPanel(name);
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

void TrialsPanel::selectedItemEvent(Ref *pSender, ListView::EventType type)
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

			auto unit = listView->getItem(listView->getCurSelectedIndex());
			auto trial = static_cast<Trial*>(unit->getUserData());

			if(trial->getUnLock() == false)
			{
				auto num = (*(trial->getUnlockCondition()).begin()).second.asInt();
				auto confirmText = a2u("你需要") + cocos2d::Value(num).asString() + a2u("个") + 
				"###type:Image---filePath:trial/linghunshi.png###type:Text---text:" + a2u("才能解锁该关卡");
				PopPanel::getInstance()->note("cannt","type:Text---text:"+ confirmText, 1.0f, true, true);
			}
			else
			{
				auto confirmText = a2u("确定进入该关卡?");
				PopPanel::getInstance()->confirm("saveConfirm","type:Text---text:"+ confirmText,PromptPanel::TYPE_DONE_CANCEL,[=](){
					Detect::shareDetect()->setCurTrial(trial->getId());
					trial->ready();
					destroy();
				},nullptr,true,true);
			}
			break;
		}
	default:
		break;
	}
}

void TrialsPanel::selectedItemEventScrollView(Ref* pSender, ui::ScrollView::EventType type)
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

bool TrialsPanel::init()
{
	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(TrialsPanel::onButtonClicked, this));
	}
	
	//_container->setTouchEnabled(true);
	_container->setAnchorPoint(Vec2(0,0));
	_listView = static_cast<ListView*>(Helper::seekWidgetByName(_root, "ListView_content"));
	_endNumLab = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_end_num"));

	_trialUnit = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_trialUnit"));
	_trialUnit->retain();
	_trialUnit->removeFromParent();

	_listView->addEventListener((ui::ListView::ccListViewCallback)CC_CALLBACK_2(TrialsPanel::selectedItemEvent, this));
	_listView->addEventListener((ui::ListView::ccScrollViewCallback)CC_CALLBACK_2(TrialsPanel::selectedItemEventScrollView,this));
	
	_container->setScale(0.85f);
	_container->setOpacity(200);
	_container->runAction(FadeIn::create(0.2));
	_container->runAction(ScaleTo::create(0.2,1.0f,1.0f,1.0f));

	_root->setTouchEnabled(true);
	_container->setTouchEnabled(false);
	setKeyboardEnabled(true);
	return true;
}

void TrialsPanel::initWithTrials(Map<int, Trial*> trialss, int type)
{
	auto typeStr = "trialType" + cocos2d::Value(type).asString();
	auto name = PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", typeStr.c_str(),"name");
	auto intro = PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", typeStr.c_str(),"intro");

	auto nameLab = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_name"));
	auto introLab = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_intro"));
	nameLab->setString(name);
	introLab->setString(intro);

	Vector<Trial*> trials;
	for(auto pair:trialss)
	{
		auto trial = pair.second;
		if(trial->getModelByName("type").asInt() == type)
		{
			// 检测是否符合条件
			auto player = Detect::shareDetect()->getPlayer();
			auto condition = trial->getUnlockCondition();
			auto type = (*condition.begin()).first;
			auto num = (*condition.begin()).second.asInt();
			auto curNum = player->getItemBag()->getPropSizeByType(cocos2d::Value(type).asInt());
			if(curNum >= num)
				trial->setUnLock(true);

			trials.pushBack(trial);
		}
	}
	_trials = trials;
	for(auto trial:trials)
	{
		auto trialUnit = _trialUnit->clone();

		auto numLab = static_cast<Text*>(Helper::seekWidgetByName(trialUnit, "Label_num"));
		auto tagLab = static_cast<Text*>(Helper::seekWidgetByName(trialUnit, "Label_tag"));
		auto undoLab = static_cast<Text*>(Helper::seekWidgetByName(trialUnit, "Label_undo"));
		auto achievementLab = static_cast<Text*>(Helper::seekWidgetByName(trialUnit, "Label_achievement"));
		auto image = static_cast<ImageView*>(Helper::seekWidgetByName(trialUnit, "Image_15"));

		numLab->setVisible(false);
		tagLab->setVisible(false);
		undoLab->setVisible(false);
		achievementLab->setVisible(false);
		image->setVisible(false);

		if(trial->getUnLock() == false)
		{
			image->setVisible(true);
			numLab->setVisible(true);
			auto condition = trial->getUnlockCondition();
			numLab->setString("X" + cocos2d::Value((*condition.begin()).second).asString());
		}
		else
		{
			tagLab->setVisible(true);
			undoLab->setVisible(true);
			achievementLab->setVisible(true);
			tagLab->setString(trial->getModelByName("nickName").asString());
			achievementLab->setString(cocos2d::Value(trial->getUndoAchieNum()).asString());
		}

		trialUnit->setUserData(trial);
		_listView->pushBackCustomItem(trialUnit);
	}
}

void TrialsPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
		switch(tag)
		{
		case EXIT:
			destroy();
			break;
		}
	}
}

void TrialsPanel::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	if(keyCode == EventKeyboard::KeyCode::KEY_SPACE)
	{
		destroy();
	}
}