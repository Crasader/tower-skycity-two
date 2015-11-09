#include "Fight.h"
#include "utils/StringHandler.h"
#include "UI/ItemBagPanel.h"
#include "Detect.h"
#include "cell/CellFactory.h"
#include "utils/PlotScript.h"
#include "utils/EventHelper.h"
#include "map/Floor.h"
#include "map/Tower.h"
#include "cell/item/ItemBag.h"


USING_NS_CC;
using namespace cocostudio;
using namespace ui;

Fight::Fight(std::string player, std::string monster)
{
	_player = dynamic_cast<Player*>(Detect::shareDetect()->getCellByName(player));
	_monster = dynamic_cast<Monster*>(Detect::shareDetect()->getCellByName(monster));
	_end = false;
	// 先攻属性
	_turn = 1;
	if(_monster->hasAttrs("xiangong") == true)
		_turn = 2;
	_mdef = _player->getMdef();

	// 坚固属性
	if(_monster->hasAttrs("jiangu"))
	{
		_monster->setDef(_player->getStr() - 1);
	}

	_player->setCanWalk(false);
	_player->setCanGuide(false);
	_monster->setCanWalk(false);
	_monster->setCanGuide(false);

	/*ValueMap value1;
	value1["typeId"] = 3001008;
	value1["retain"] = 1;
	ValueMap value2;
	value2["typeId"] = 3001008;
	value2["retain"] = 1;
	ValueMap value3;
	value3["typeId"] = 3001009;
	value3["retain"] = 1;
	ValueMap value4;
	value4["typeId"] = 3001009;
	value4["retain"] = 1;
	_effect1 = static_cast<Effect*>(CellFactory::getInstance()->createCell(value1));
	_effect1->setClearTime(1000);
	_effect2 = static_cast<Effect*>(CellFactory::getInstance()->createCell(value2));
	_effect2->setClearTime(1000);
	_effect3 = static_cast<Effect*>(CellFactory::getInstance()->createCell(value3));
	_effect3->setClearTime(1000);
	_effect4 = static_cast<Effect*>(CellFactory::getInstance()->createCell(value4));
	_effect4->setClearTime(1000);
	BlendFunc func = {GL_ONE_MINUS_DST_COLOR, GL_ONE};
	_effect3->getView()->setBlendFunc(func);
	_effect4->getView()->setBlendFunc(func);
	Detect::shareDetect()->addCell(_effect1,_player->getSite(),4,Detect::shareDetect()->getCurFloor()->getId());
	Detect::shareDetect()->addCell(_effect2,_monster->getSite(),4,Detect::shareDetect()->getCurFloor()->getId());
	Detect::shareDetect()->addCell(_effect3,_player->getSite(),4,Detect::shareDetect()->getCurFloor()->getId());
	Detect::shareDetect()->addCell(_effect4,_monster->getSite(),4,Detect::shareDetect()->getCurFloor()->getId());*/

	/*_shake1 = Shake::create(0.1f,3,0);
	_shake1->retain();
	_shake2 = Shake::create(0.1f,3,0);
	_shake2->retain();*/

	auto tintred1 = TintBy::create(0.2f, 0, -255, -255);
	_tintred1 = Sequence::create(tintred1,tintred1->reverse(),nullptr);
	_tintred1->retain();
	auto tintred2 = TintBy::create(0.2f, 0, -255, -255);
	_tintred2 = Sequence::create(tintred2,tintred2->reverse(),nullptr);
	_tintred2->retain();

	_fightCount = 0;
	_fightDelay = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "fight","fightDelay")).asFloat();
	_fightTotal = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "fight","fightTotal")).asInt();
	
	start();
}

Fight* Fight::create(std::string oneName, std::string twoName)
{
	Fight *pRet = new Fight(oneName, twoName);
	if (pRet)
	{
		pRet->autorelease();
		//pRet->retain();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}	
}

void Fight::start()
{
	float wait = 0;
	// 吸血、净化
	if(_monster->hasAttrs("xixue"))
	{
		auto xixueRate = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "xixue","rate")).asFloat();
		auto xixueAbsorb = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "xixue","absorb")).asInt();
		auto dropHp = _player->getHp() * xixueRate;
		_player->addHp(-dropHp);
		if(xixueAbsorb == 1)
			_monster->addHp(dropHp);
		wait = 0.5;
	}
	if(_monster->hasAttrs("jinghua"))
	{
		auto jinghuaRate = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "jinghua","rate")).asInt();
		auto dropHp = _player->getMdef() * jinghuaRate;
		_player->addHp(-dropHp);
		wait = 0.5;
	}
	_player->getMdef();
	this->scheduleOnce(schedule_selector(Fight::round),wait);
}

void Fight::effect(Actor* actor, int hit)
{
	ValueMap value1;
	value1["typeId"] = 3001008;
	value1["retain"] = 1;
	auto effect1 = static_cast<Effect*>(CellFactory::getInstance()->createCell(value1));
	effect1->setClearTime(1);
	Detect::shareDetect()->addCell(effect1,actor->getSite(),4,Detect::shareDetect()->getCurFloor()->getId(),Detect::shareDetect()->getCurTower()->getId());

	effect1->erupt();
	// 战斗音效
	float random = CCRANDOM_0_1();
	if(random > 0.2)
	{
		EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:hit1");
	}
	else
	{
		EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:hit2");
	}

	Label* hitHp;
	hitHp = Label::createWithTTF("", "fonts/58983.ttf", 12);
	hitHp->enableOutline(Color4B(196,0,0,255),1);
	hitHp->setTextColor(Color4B::WHITE);
	hitHp->setAnchorPoint(Vec2(0.5, 0.5));
	actor->addChild(hitHp);

	//hitHp->setVisible(false);
	hitHp->setString(cocos2d::Value(hit).asString());
	int r = -(TILE_HALF_WIDTH + 30) / 2 + CCRANDOM_0_1() * (TILE_HALF_WIDTH + 30);
	hitHp->runAction(Sequence::create(JumpTo::create(0.6, Vec2(r,20), 10, 1), CallFuncN::create([=](Node* node){
		node->setVisible(false);
		node->removeFromParentAndCleanup(true);
	}),NULL));
	auto scaleBy = ScaleBy::create(0.08f, 1.5f, 1.5f);
	hitHp->runAction(Sequence::create(scaleBy,scaleBy->reverse(),NULL));


	/*Label* hitHp1;
	hitHp1 = Label::createWithTTF("", "fonts/58983.ttf", 15);
	hitHp1->enableOutline(Color4B(196,0,0,255),1);
	hitHp1->setTextColor(Color4B::YELLOW);
	_effect2->erupt();
	actor->stopAction(_shake1);
	actor->runAction(_shake1);
	actor->stopAction(_tintred1);
	actor->runAction(_tintred1);
	hitHp1->setColor(Color3B::WHITE);
	actor->addChild(hitHp1);
	hitHp1->setPosition(Vec2(30,40));
	hitHp1->setAnchorPoint(Vec2(0.5, 0.5));
	hitHp1->setVisible(true);
	hitHp1->setString(cocos2d::Value(-hit).asString());
	int r = -TILE_HALF_HEIGHT + rand() % (TILE_HALF_HEIGHT+TILE_HALF_HEIGHT+1) + 30;
	hitHp1->runAction(Sequence::create(JumpTo::create(0.6, Vec2(r,20), 10, 1), CallFuncN::create([](Node* node){node->setVisible(false);node->removeFromParentAndCleanup(true);}),NULL));*/
}

void Fight::round(float delay)
{
	_fightCount++;
	if(_fightCount > _fightTotal)
	{
		if(_fightDelay != 0)
		{
			ValueMap value1;
			value1["typeId"] = 3001009;
			value1["retain"] = 1;
			auto effect = static_cast<Effect*>(CellFactory::getInstance()->createCell(value1));
			effect->setClearTime(1);
			BlendFunc func = {GL_ONE_MINUS_DST_COLOR, GL_ONE};
			effect->getView()->setBlendFunc(func);
			Detect::shareDetect()->addCell(effect,_monster->getSite(),4,Detect::shareDetect()->getCurFloor()->getId(),Detect::shareDetect()->getCurTower()->getId());

			effect->erupt();
		}
		_fightDelay = 0;
	}
	if(_turn == 1)
	{
		// 计算伤害
		int hit_player = _player->getStr() - _monster->getDef();
		hit_player = hit_player > 0? hit_player:0;
		
		// 伤害结果
		int hp = _monster->getHp();
		hp -= hit_player;

		if(_fightDelay != 0)
			effect(_monster, hit_player);

		// 打败了怪物
		if(hp <= 0)
		{
			// 胜利结算
			_player->addXp(_monster->getXp());
			int addGold;
			if(_player->getItemBag()->getPropByType(2002015) == nullptr)
			{
				addGold = _monster->getGold();
			}
			else
			{
				addGold = _monster->getGold() * 2;
			}
			_player->addGold(addGold);
			_player->setCanWalk(true);
			_player->setCanGuide(true, 0.5);

			_monster->setHp(0);
			end();
		}
		// 还未打败
		else
		{
			_monster->addHp(-hit_player);
			_turn = 2;
			runAction(Sequence::create(DelayTime::create(_fightDelay), CallFunc::create([=](){round(0);}), nullptr));
		}
	}
	else if(_turn == 2)
	{
		// 计算伤害
		int hit_monster = _monster->getStr() - (_monster->hasAttrs("mogong") ? 0 : _player->getDef());
		hit_monster = hit_monster > 0? hit_monster:0;
		if(_monster->hasAttrs("lianji") == true)
		{
			if(_fightDelay != 0)
			{
				effect(_player, hit_monster);
				runAction(Sequence::create(DelayTime::create(_fightDelay * 0.8), CallFunc::create([=](){effect(_player, hit_monster);}),nullptr));
			}
			hit_monster *= 2;
		}
		else
		{
			if(_fightDelay != 0)
				effect(_player, hit_monster);
		}
		int hp = _player->getHp();
		int loseHp = 0;
		// 魔防补偿
		if(_mdef > 0)
		{
			_mdef -= hit_monster;
			if(_mdef <= 0)
			{
				hp += _mdef;
				_mdef = 0;
				loseHp = _mdef;
			}
		}
		else
		{
			hp -= hit_monster;
			loseHp = hit_monster;
		}
		// 被打败了
		if(hp <= 0)
		{
			_player->setHp(0);
			end();
		}
		// 勇士还没死
		else
		{
			_player->addHp(-loseHp);
			_turn = 1;
			runAction(Sequence::create(DelayTime::create(_fightDelay), CallFunc::create([=](){round(0);}),nullptr));
		}
	}
}

void Fight::end()
{
	/*_effect1->removeFromFloor();
	_effect2->removeFromFloor();
	_effect3->removeFromFloor();
	_effect4->removeFromFloor();
	_shake1->release();
	_tintred1->release();
	_shake2->release();
	_tintred2->release();*/

	this->stopAllActions();

	_end = true;
	this->removeFromParentAndCleanup(true);
	//runAction(Sequence::create(DelayTime::create(1), CallFunc::create([=](){}),nullptr));
}