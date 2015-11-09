#include "MonsterInfoPanel.h"
#include "scene/StartScene.h"
#include "utils/PlotScript.h"
#include "utils/StringHandler.h"
#include "Detect.h"
#include "model/Model.h"
#include "cell/actor/Actor.h"

#define NONE_IMAGE "none.png"

MonsterInfoPanel* MonsterInfoPanel::create(std::string name)
{
	MonsterInfoPanel *pRet = new MonsterInfoPanel(name);
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

bool MonsterInfoPanel::init()
{
	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(MonsterInfoPanel::onButtonClicked, this));
	}
	_container->setScale(0.85f);
	_container->setOpacity(200);
	_container->runAction(FadeIn::create(0.2));
	_container->runAction(ScaleTo::create(0.2,1.0f,1.0f,1.0f));

	_root->setTouchEnabled(true);
	_container->setTouchEnabled(false);
	_root->addTouchEventListener(CC_CALLBACK_2(MonsterInfoPanel::onButtonClicked, this));
	setKeyboardEnabled(true);
	return true;
}

void MonsterInfoPanel::initWithMonster(Monster* monster)
{
	auto image = static_cast<ImageView*>(Helper::seekWidgetByName(_root, "Image_image"));
	auto labelName = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_name"));
	auto labelHp = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_hp"));
	auto labelStr = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_str"));
	auto labelDef = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_def"));
	auto labelXp = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_xp"));
	auto labelGold = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_gold"));
	auto labelDamage = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_damage"));

	auto labelAttrInfo = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_attr_info"));
	auto labelMonsterInfo = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_monster_info"));

	image->loadTexture(monster->getImageUrl(),Widget::TextureResType::PLIST);
	labelName->setString(monster->getNickName());

	labelHp->setString(cocos2d::Value(monster->getHp()).asString());
	labelStr->setString(cocos2d::Value(monster->getStr()).asString());
	labelDef->setString(cocos2d::Value(monster->getDef()).asString());
	labelGold->setString(cocos2d::Value(monster->getGold()).asString());
	labelXp->setString(cocos2d::Value(monster->getXp()).asString());
	labelMonsterInfo->setString(a2u("¹ÖÎï¼ò½é: ") + cocos2d::Value(monster->getModelByName("introduce")).asString());

	std::string attrStr;
	auto attrCsv = Model::shareModel()->attrCsv;
	for(auto attr:monster->getAttrs())
	{
		auto attrModel = attrCsv->getModelByAssignTypeAndValue("type", cocos2d::Value(attr));
		auto nickName = attrModel["nickName"].asString();
		auto color = attrModel["color"].asString();
		auto info = attrModel["introduce"].asString();
		attrStr += nickName + ": " + info + "\n";
	}
	// É¾µôÄ©Î²µÄ\n
	if(attrStr.size() != 0)
	{
		attrStr.pop_back();
		attrStr.pop_back();
	}

	labelAttrInfo->setString(attrStr);


	// ¼ÆËãÉËº¦
	// ÉËº¦¼ÆËã
	auto player = Detect::shareDetect()->getPlayer();
	auto mhit = monster->getStr() - (monster->hasAttrs("mogong") ? 0 : player->getDef());
	auto phit = player->getStr() - monster->getDef();
	mhit = mhit > 0 ? mhit:0;
	phit = phit > 0 ? phit:0;
	int mround = ((phit == 0) ? 10000000:ceil(monster->getHp() * 1.0 / phit));
	int damage = mhit * (monster->hasAttrs("xiangong")?mround:(mround - 1)) * (monster->hasAttrs("lianji")? 2 : 1)
		- player->getMdef()
		+ (monster->hasAttrs("jinghua") ? player->getMdef() * monster->getAttrData("jinghuaRate").asFloat() : 0) 
		+ (monster->hasAttrs("xixue") ? player->getHp() * monster->getAttrData("xixueRate").asFloat() : 0);
	std::string damageStr = phit == 0 ? a2u("XXX") : cocos2d::Value(damage).asString();
	labelDamage->setString(cocos2d::Value(damageStr).asString());
}

void MonsterInfoPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
		switch(tag)
		{
		}
		destroy();
	}
}

void MonsterInfoPanel::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	if(keyCode == EventKeyboard::KeyCode::KEY_SPACE)
	{
		destroy();
	}
}
