#include "Actor.h"
#include "Global.h"
#include "utils/AStar.h"
#include "TObject.h"
#include "utils/StringHandler.h"
#include "scene/GameScene.h"
#include "base/CCNS.h"
#include "model/Model.h"
#include "editor-support/cocostudio/DictionaryHelper.h"
#include "mutual/Fight.h"
#include "Detect.h"
#include "mutual/ChatManager.h"
#include "UI/PopPanel.h"
#include "mutual/Swap.h"
#include "ui/DiaryPanel.h"
#include "utils/PlotScript.h"
#include "cell/CellFactory.h"
#include "ui/PaperPanel.h"
#include "ui/LevelUpPanel.h"
#include "Utils/Tools.h"
#include "cell/CellFactory.h"
#include "cell/Vision.h"

#include "ui/CocosGUI.h"
#include "cocostudio/CCSGUIReader.h"

#include "cell/item/ItemBag.h"
#include "cell/item/SparDisk.h"
#include "ui/DiaryPanel.h"
#include "utils/EventHelper.h"
#include "buff/Buff.h"
#include "User.h"
#include "map/Floor.h"
#include "map/Tower.h"
#include "map/World.h"

USING_NS_CC;

Actor* Actor::create(int id)
{
	Actor *pRet = new Actor(id);
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

Actor* Actor::create(ValueMap& model)
{
	Actor *pRet = new Actor(model);
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

void Actor::initModel()
{
	Cell::initModel();
	// 初始化数据
	int typeId = _typeId;
	if(DATA_MODE == 1)
	{
		rapidjson::Document &datas = Model::shareModel()->actorDatas;
		auto &data = Model::GetDataFromCol1(datas,typeId);
		Model::JsonAddModel(data,_model);
	}
	else if(DATA_MODE == 2)
	{
		auto model = Model::shareModel()->actorCsv->getModelById(typeId);
		Model::modelAddModel(model,_model);
	}
}

void Actor::refreshModel()
{
	Cell::refreshModel();
	// 初始化数据
	int typeId = _typeId;
	if(DATA_MODE == 1)
	{
		rapidjson::Document &datas = Model::shareModel()->actorDatas;
		auto &data = Model::GetDataFromCol1(datas,typeId);
		Model::JsonAddModel(data,_model);
	}
	else if(DATA_MODE == 2)
	{
		auto model = Model::shareModel()->actorCsv->getModelById(typeId);
		Model::modelCoverModel(model,_model);
	}
}

ValueMap Actor::saveModel()
{
	ValueMap model = Cell::saveModel();
	model["nickName"] = _nickName;
	model["hp"] = _hp;
	model["str"] = _str;
	model["def"] = _def;
	model["xp"] = _xp;
	model["level"] = _level;
	model["gold"] = _gold;
	model["chatId"] = _chatId;
	model["swapCount"] = _swapCount;
	model["relation"] = _model["relation"];
	model["dealAppear"] = _model["dealAppear"];
	model["swapId"] = _model["swapId"];
	model["noDead"] = _model["noDead"];
	model["isDead"] = _isDead;
	model["itemBag"] = _itemBag->saveModel();
	model["diary"] = _diary->saveModel();
	model["sparDisk"] = _sparDisk->saveModel();
	model["useBombNum"] = _useBombNum;
	model["useHoeNum"] = _useHoeNum;
	return model;
}

void Actor::initOthers()
{
	Cell::initOthers();
	

	// 属性赋值
	if(_model["level"].isNull())
	{
		_model["level"] = _model["level_init"];
	}
	_level = _model["level"].asInt();
	if(DATA_MODE == 1)
	{
		rapidjson::Document &datas1 = Model::shareModel()->levelDatas;
		auto &data = Model::GetDataFromCol1(datas1,_level);
		_maxHp = DICHELPER->getIntValue_json(data,"maxHp");

		rapidjson::Document &datas2 = Model::shareModel()->levelDatas;
		auto &lastData = Model::GetDataFromCol1(datas2,_level);
		_lastXp = DICHELPER->getIntValue_json(lastData,"xp");

		rapidjson::Document &datas3 = Model::shareModel()->levelDatas;
		auto &nextData = Model::GetDataFromCol1(datas3,_level + 1);
		_nextXp = DICHELPER->getIntValue_json(nextData,"xp");
	}
	else if(DATA_MODE == 2)
	{
		auto models = Model::shareModel()->levelCsv;
		_maxHp = models->getValueByIdAndName(_level,"maxHp").asInt();
		_lastXp = models->getValueByIdAndName(_level,"xp").asInt();
		_nextXp = models->getValueByIdAndName(_level + 1,"xp").asInt();
	}

	// 初始化数据
	if(_model["hp"].isNull())
	{
		_model["hp"] = _model["hp_init"];
	}
	_hp = _model["hp"].asInt();

	if(_model["str"].isNull())
	{
		_model["str"] = _model["str_init"];
	}
	_str = _model["str"].asInt();

	if(_model["def"].isNull())
	{
		_model["def"] = _model["def_init"];
	}
	_def = _model["def"].asInt();

	if(_model["mdef"].isNull())
	{
		_model["mdef"] = _model["mdef_init"];
	}
	_mdef = _model["mdef"].asInt();

	if(_model["xp"].isNull())
	{
		_model["xp"] = _model["xp_init"];
	}
	_xp = _model["xp"].asInt();

	if(_model["level"].isNull())
	{
		_model["level"] = _model["level_init"];
	}
	_level = _model["level"].asInt();

	if(_model["gold"].isNull())
	{
		_model["gold"] = _model["gold_init"];
	}
	_gold = _model["gold"].asInt();

	if(_model["speed"].isNull())
	{
		_model["speed"] = _model["speed_init"];
	}
	_speed = _model["speed"].asFloat();
	

	if(_model["chatId"].isNull())
	{
		_model["chatId"] = _model["chatId_init"];
	}
	_chatId = _model["chatId"].asInt();


	_nickName = _model["nickName"].asString();
	setName(_model["name"].asString());

	
	_swapCount = 0;
	if(!_model["swapCount"].isNull())
	{
		_swapCount = _model["swapCount"].asInt();
	}

	if(!_model["isDead"].isNull())
	{
		_isDead = _model["isDead"].asBool();
	}

	if(Detect::shareDetect()->viewHide == 1)
	{
		if(!_model["viewHide"].isNull())
		{

			if(_model["viewHide"].asInt() == 1)
			{
				this->setVisible(false);
			}
		}
	}

	_initTime = getTime();
	_durTime = 0;
	if(!_model["durTime"].isNull())
	{
		_durTime = _model["durTime"].asDouble();
	}

	_useBombNum = 0;
	if(!_model["useBombNum"].isNull())
	{
		_useBombNum = _model["useBombNum"].asInt();
	}
	_useHoeNum = 0;
	if(!_model["useHoeNum"].isNull())
	{
		_useHoeNum = _model["useHoeNum"].asInt();
	}

	// 视野
	if(!_model["vision"].isNull() && _model["vision"].asString() != "")
	{
		PointArray* visionArr = PointArray::create(100);
		auto visionsStr = _model["vision"].asString();
		ValueVector visionStrVec;
		stringToValueVector(visionsStr,"|",visionStrVec);
		for(auto visionStr:visionStrVec)
		{
			visionArr->addControlPoint(PointFromString(visionStr.asString()));
		}
		for(int i = 0; i < visionArr->count(); i++)
		{
			auto disSite = visionArr->getControlPointAtIndex(i);
			ValueMap model;
			model["type"] = "Vision";
			auto vision = dynamic_cast<Vision*>(CellFactory::getInstance()->createCell(model));
			vision->setDisSite(disSite);
			vision->setOwner(this);
			_visions.pushBack(vision);
		}
	}

	if(_refresh == false)
	{
		if(!_model["itemBag"].isNull())
		{
			_itemBag = ItemBag::create(_model["itemBag"].asValueMap());
			_itemBag->setOwner(this);
		}
		else
		{
			_itemBag = ItemBag::create();
			_itemBag->setOwner(this);
		}
		CC_SAFE_RETAIN(_itemBag);


		if(!_model["sparDisk"].isNull())
		{
			_sparDisk = SparDisk::create(_model["sparDisk"].asValueMap());
			_sparDisk->setOwner(this);
		}
		else
		{
			_sparDisk = SparDisk::create();
			_sparDisk->setOwner(this);
		}
		CC_SAFE_RETAIN(_sparDisk);

		if(!_model["diary"].isNull())
		{
			_diary = DiaryPanel::create("diary", _model["diary"].asValueMap());
			_diary->setOwner(this);
		}
		else
		{
			_diary = DiaryPanel::create("diary");
			_diary->setOwner(this);
		}
		CC_SAFE_RETAIN(_diary);
	}

	
}


void Actor::initNull()
{
	Cell::initNull();
	// 其他属性初始化
	_itemBag = nullptr;
	_sparDisk = nullptr;
	_diary = nullptr;
	
	_isDead = false;

	_grabCell = nullptr;
	_dargAction1 = nullptr;
	_dargAction2 = nullptr;
}

void Actor::setNickName(std::string nickName)
{
	_nickName = nickName;
}

void Actor::setHp(int hp)
{
	//_hp = (hp <= _maxHp)?hp:_maxHp;
	_hp = hp;
	_hp = (_hp <= 0)?0:_hp;
	if(_hp == 0)
	{
		dead();
	}
}

void Actor::setStr(int str)
{
	_str = str;	
}

void Actor::setDef(int def)
{
	_def = def;
}

void Actor::setMdef(int mdef)
{
	_mdef = mdef;
}

void Actor::setXp(int xp)
{
	_xp = xp;
}

void Actor::setGold(int gold)
{
	_gold = gold;
}

void Actor::setSpeed(float speed)
{
	_speed = speed;
}

void Actor::setLevel(int level)
{
	_level = level;

	if(DATA_MODE == 1)
	{
		rapidjson::Document &datas1 = Model::shareModel()->levelDatas;
		auto &data = Model::GetDataFromCol1(datas1,_level);
		_maxHp = DICHELPER->getIntValue_json(data,"maxHp");
		setHp(_hp);

		rapidjson::Document &datas2 = Model::shareModel()->levelDatas;
		auto &lastData = Model::GetDataFromCol1(datas2,_level);
		_lastXp = DICHELPER->getIntValue_json(lastData,"xp");

		rapidjson::Document &datas3 = Model::shareModel()->levelDatas;
		auto &nextData = Model::GetDataFromCol1(datas3,_level + 1);
		_nextXp = DICHELPER->getIntValue_json(nextData,"xp");
	}
	else if(DATA_MODE == 2)
	{
		auto models = Model::shareModel()->levelCsv;
		_maxHp = models->getValueByIdAndName(_level,"maxHp").asInt();
		setHp(_hp);
		_lastXp = models->getValueByIdAndName(_level,"xp").asInt();
		_nextXp = models->getValueByIdAndName(_level + 1,"xp").asInt();
	}
}

int Actor::getHp()
{
	return _hp + ((_itemBag == nullptr) ? 0 : _itemBag->getAddHp());
}
int Actor::getStr()
{
	return _str + ((_itemBag == nullptr) ? 0 : _itemBag->getAddStr());
}
int Actor::getDef()
{
	return _def + ((_itemBag == nullptr) ? 0 : _itemBag->getAddDef());
}
int Actor::getMdef()
{
	return _mdef;
}

int Actor::getXp()
{
	return _xp;
}

double Actor::getDurTime()
{
	auto time = getTime();
	return _durTime + (time - _initTime);
}

void Actor::addHp(int hp)
{
	auto value = _hp + hp;
	setHp(value);
}

void Actor::addStr(int str)
{
	auto value = _str + str;
	setStr(value);
}

void Actor::addDef(int def)
{
	auto value = _def + def;
	setDef(value);
}

void Actor::addMdef(int mdef)
{
	auto value = _mdef + mdef;
	setMdef(value);
}

void Actor::addXp(int xp)
{
	auto value = _xp + xp;

	int level;

	if(DATA_MODE == 1)
	{
		rapidjson::Document &datas = Model::shareModel()->levelDatas;
		auto &data = Model::GetDataFromCol2(datas,value,1);
		level = DICHELPER->getIntValue_json(data,"level");
	}
	else if(DATA_MODE == 2)
	{
		auto models = Model::shareModel()->levelCsv;
		level = models->getModelByAlikeTypeAndValue("xp",cocos2d::Value(value))["level"].asInt();
	}

	setLevel(level);
	setXp(value);
}

void Actor::addLevel(int level)
{
	auto value = _level + level;
	setLevel(value);
}

void Actor::addGold(int gold)
{
	auto value = _gold + gold;
	setGold(value);
}

void Actor::addSpeed(float speed)
{
	auto value = _speed + speed;
	setSpeed(value);
}

void Actor::setSite(const Vec2 &vec)
{
	Cell::setSite(vec);
}

void Actor::initView()
{
	_shadow = Sprite::createWithSpriteFrameName("shadow.png");
	_shadow->setAnchorPoint(Point(0.5,0.25));
	addChild(_shadow);

	Cell::initView();
	//animate(IDLE,_face);
#if DETECT_ACTOR == 1
	_damageLabel = Label::createWithTTF("", "fonts/58983.ttf", 13);
	_damageLabel->setColor(Color3B::WHITE);
	addChild(_damageLabel);
	_damageLabel->setPosition(Vec2(0,-10));
	_damageLabel->setAnchorPoint(Vec2(0.5, 0.5));
	_damageLabel->setVisible(false);
	_damageLabel->setString("");

	_numLabel = Label::createWithTTF("", "fonts/58983.ttf", 12);
	_numLabel->setColor(Color3B::WHITE);
	addChild(_numLabel);
	_numLabel->setPosition(Vec2(0,0));
	_numLabel->setAnchorPoint(Vec2(0.5, 0.5));
	_numLabel->setVisible(false);
	_numLabel->setString("");

	_num2Label = Label::createWithTTF("", "fonts/58983.ttf", 12);
	_num2Label->setColor(Color3B::WHITE);
	addChild(_num2Label);
	_num2Label->setPosition(Vec2(15,0));
	_num2Label->setAnchorPoint(Vec2(0.5, 0.5));
	_num2Label->setVisible(false);
	_num2Label->setString("");

	_num3Label = Label::createWithTTF("", "fonts/58983.ttf", 12);
	_num3Label->setColor(Color3B::WHITE);
	addChild(_num3Label);
	_num3Label->setPosition(Vec2(0,10));
	_num3Label->setAnchorPoint(Vec2(0.5, 0.5));
	_num3Label->setVisible(false);
	_num3Label->setString("");
	//_damageLabel->enableOutline(Color4B::GREEN,10);
	/*_winLabel = Text::create("","Arial",15);
	_winLabel->setColor(Color3B::WHITE);
	addChild(_winLabel);
	_winLabel->setPosition(Vec2(30,10));
	_winLabel->setAnchorPoint(Vec2(0.5, 0.5));
	_winLabel->setVisible(false);
	_winLabel->setString("");
	_winLabel->enableShadow(Color4B::RED,Size(1,-1),100);*/
#endif
}

#if DETECT_ACTOR == 1
void Actor::setDamageLabel(std::string damage, std::string win)
{
	_damageLabel->setVisible(true);
	_damageLabel->setString(damage);
	//_winLabel->setString(win);
	if(win == "win" && damage != "over")
	{
		//_damageLabel->enableShadow(Color4B::GREEN,Size(1,-1),100);
		_damageLabel->enableOutline(Color4B(120,196,0,255),1);
	}
	else
	{
		//_damageLabel->enableShadow(Color4B::RED,Size(1,-1),100);
		_damageLabel->enableOutline(Color4B(196,0,0,255),1);
	}
}

void Actor::setNumLabel(std::string num)
{
	_numLabel->setVisible(true);
	_numLabel->setString(num);
	_numLabel->enableOutline(Color4B(196,0,0,255),1);
}

void Actor::setNum2Label(std::string num)
{
	_num2Label->setVisible(true);
	_num2Label->setString(num);
	_num2Label->enableOutline(Color4B(196,0,0,255),1);
}

void Actor::setNum3Label(std::string num)
{
	_num3Label->setVisible(true);
	_num3Label->setString(num);
	_num3Label->enableOutline(Color4B(196,0,0,255),1);
}
#endif

void Actor::onEnter()
{
	Cell::onEnter();
	
}

void Actor::jumpTo(Vec2 site)
{
	int block = _blockFunc(site);
	// 特殊门或怪物, 还有无法跳跃
	if(block > 0)
		return;
	auto floor = _floor;
	
	setCanWalk(false);
	setCanTouch(false);
	auto pos = getPosition();
	/*
	if(Detect::shareDetect()->getCellByName(getName()) != nullptr)
	{
	this->retain();
	this->removeFromFloor();
	this->setFloor(floor);
	this->setBlockFunc(CC_CALLBACK_1(Floor::isBlockBySite,floor));
	floor->getAir()->addChild(this);
	}
	this->setPosition(pos);*/
	auto toPos = Vec2(site.x * TILE_WIDTH + TILE_HALF_WIDTH, site.y * TILE_HEIGHT + TILE_HALF_HEIGHT);
	// 跟相对距离有关
	float dur = 0.5;
	float height = sqrt(pow(toPos.x - pos.x,2) + pow(toPos.y - pos.y,2)) / 5 + 20;

	stopAction(_jumpAction);
	_jumpAction = Sequence::create(MoveTo::create(dur, toPos),CallFunc::create([=](){
		setCanWalk(true);
		setCanTouch(true);
	}),nullptr);
	runAction(_jumpAction);

	_view->stopAction(_viewJumpAction);
	_view->setPosition(0,0);
	_viewJumpAction = JumpTo::create(dur, _view->getPosition(), height, 1);
	_view->runAction(_viewJumpAction);

	_shadow->stopAction(_shadowJumpSmallAction);
	_shadow->setScale(1.0);
	auto smallAction = ScaleBy::create(dur / 2, 0.7, 0.7);
	_shadowJumpSmallAction = Sequence::create(smallAction, smallAction->reverse(), nullptr);
	_shadow->runAction(_shadowJumpSmallAction);
}

void Actor::grabCell(Cell* cell, Direction dir)
{
	if(cell->getCanWalk() == false) return;
	if(this->getCanWalk() == false) return;
	auto dirVec = cell->getSite() - this->getSite();
	Direction dis;
	if(dirVec == CrossVecs[1])
	{
		dis = LEFT;
	}else if(dirVec == CrossVecs[2])
	{
		dis = UP;
	}else if(dirVec == CrossVecs[3])
	{
		dis = RIGHT;
	}else if(dirVec == CrossVecs[4])
	{
		dis = DOWN;
	}
	else
		return;
	if(dis != dir && dis != reverse(dir))
		return;

	stopAction(_dargAction1);
	cell->stopAction(_dargAction2);

	Vec2 toSite;
	if(dis == dir)
		toSite = cell->getSite() + dirVec;
	else if(dis != dir)
		toSite = cell->getSite() - dirVec;
	auto toPos = Vec2(toSite.x * TILE_WIDTH + TILE_HALF_WIDTH, toSite.y * TILE_HEIGHT + TILE_HALF_HEIGHT);
	int isblock = this->getFloor()->isBlockBySite(toSite);
	if(isblock > 0) return;

	// 跟相对距离有关
	auto dur = 0.5;
	_dargAction2 = Sequence::create(EaseCubicActionInOut::create(MoveTo::create(dur, toPos)),CallFunc::create([=](){
		cell->setSite(toSite);
	}),nullptr);

	
	if(dis == dir)
		toSite = this->getSite() + dirVec;
	else if(dis != dir)
		toSite = this->getSite() - dirVec;
	toPos = Vec2(toSite.x * TILE_WIDTH + TILE_HALF_WIDTH, toSite.y * TILE_HEIGHT + TILE_HALF_HEIGHT);
	isblock = this->getFloor()->isBlockBySite(toSite);
	if(toSite != cell->getSite() && isblock > 0) return;
	// 跟相对距离有关
	dur = 0.4;

	_actionMove = true;
	cell->setActionMove(true);
	animate(Cell::WALK, _face);
	cell->animate(Cell::WALK, _face);
	_dargAction1 = Sequence::create(EaseCubicActionInOut::create(MoveTo::create(dur, toPos)),CallFunc::create([=](){
		_actionMove = false;
		cell->setActionMove(false);
		this->setSite(toSite);
	}),nullptr);

	runAction(_dargAction1);
	cell->runAction(_dargAction2);
}

// ################################################  碰撞函数  ##################################################
// 单元格区域
void Actor::crashOn(Cell* cell)
{
	Cell::crashOn(cell);
	auto name = cell->getName();
	auto type = cell->getType();
	// 捡到消费道具
	if(type == "Prop")
	{
		log("I pick a Prop[%s]",name.c_str());
		auto prop = dynamic_cast<Prop*>(cell);
		if(prop != nullptr) addProp(prop);
	}
	// 捡到装备
	else if(type == "Equip")
	{
		log("I pick a Equip[%s]",name.c_str());
		auto equip = dynamic_cast<Equip*>(cell);
		if(equip == nullptr) return;
		auto ownEquip = _itemBag->getEquipByType(equip->getTypeId1());
		if(ownEquip != nullptr && ownEquip->getTypeId() > equip->getTypeId())
		{
			PopPanel::getInstance()->note("cantEquip","type:Text---text:"+ a2u("你已经拥有了更高级的装备"));
		}
		else
		{
			addEquip(equip);
		}
	}
	// 捡到特殊道具
	else if(type == "Special")
	{
		log("I pick a Special[%s]",name.c_str());
		auto special = dynamic_cast<Special*>(cell);
		if(special != nullptr) addSpecial(special);
	}
	// 捡到一次性道具
	else if(type == "Gem")
	{
		log("I use a Gem[%s]",name.c_str());
		auto gem = dynamic_cast<Gem*>(cell);
		getGemByType(gem->getTypeId());
	}
	// 捡到晶石
	else if(type == "Spar")
	{
		log("I use a Spar[%s]",name.c_str());
		if(_itemBag->getSpecialByType(2007001) == nullptr)
		{
			EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:otherNote", StringUtils::format("text:%s|site:{%f,%f}",a2u("无法捡起").c_str(),getPosition().x,getPosition().y));
			return;
		}
		else
		{
			auto spar = dynamic_cast<Spar*>(Detect::shareDetect()->getCellByName(name));
			if(spar == nullptr) return;
			addSpar(spar);
			spar->removeFromFloor();
		}
	}
	// 踩到机关
	else if(type == "Switch")
	{
		log("I crashOn a Switch[%s]",name.c_str());
	}
}
void Actor::crashOut(Cell* cell)
{
	Cell::crashOut(cell);
	//auto type = cell->getType();
	//// 踩离机关
	//if(type == "Switch" && !effect["crashOut"].isNull())
	//{
	//	log("I crashOut a Switch[%s]",cell->getName().c_str());
	//	auto target = cell->getName();
	//	EventHelper::getInstance()->dispatchByStr("target:"+target+"|type:"+effect["crashOut"].asString()+"|senderType:Monster|senderName:"+getName()+"|senderTypeId:"+cocos2d::Value(getTypeId()).asString(),"");
	//}
}
// 正好踩到单元格中点
void Actor::stepOn(Cell* cell)
{
	Cell::stepOn(cell);
}
// 触碰单元格
void Actor::touchOut(Cell* cell)
{
	Cell::touchOut(cell);
}
// 触碰单元格
void Actor::touchOn(Cell* cell)
{
	Cell::touchOn(cell);
	if(cell->getModelByName("canDrag").asInt() == 1)
	{
		_grabCell = cell;
	}
	// 接触门
	if(cell->getType() == "Door")
	{
		auto door = dynamic_cast<Door*>(cell);
		auto keyIdValue = door->getEffectMap()["needKeyId"];
		if(!keyIdValue.isNull())
		{
			auto keyId = keyIdValue.asInt();
			if(_itemBag->getPropSizeByType(keyId) > 0)
			{
				// 钥匙开门
				EventHelper::getInstance()->dispatchByStr("target:"+door->getName()+"|type:keyOpen|senderName:"+getName(),"keyId:"+cocos2d::Value(keyId).asString());
			}
			else if(!door->getModelByName("forceOpen").isNull())
			{
				// 强制开门
				EventHelper::getInstance()->dispatchByStr("target:"+door->getName()+"|type:forceOpen","");
			}
			else
			{
				log("I don't have a key[%d].",keyId);
				std::string effect;
				if(keyId == 0)
				{
					effect = "text:" + a2u("无法打开") + "|site:" + pointToString(getPosition());
				}
				else
				{
					auto key = Item::create(keyId);
					effect = "text:" + GETSTRING("none") + key->getNickName() + "|site:" + pointToString(getPosition());
				}

				EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:otherNote", effect);
			}
		}
		else
		{
			EventHelper::getInstance()->dispatchByStr("target:"+door->getName()+"|type:open|senderName:"+getName(),"");	
		}
	}
	// 摸到机关
	else if(cell->getType() == "Switch")
	{
		log("I touch a Switch[%s]",cell->getName().c_str());
	}
	// 碰到怪物
	else if(cell->getType() == "Monster")
	{
		auto monster = dynamic_cast<Monster*>(cell);
		int relation = monster->getModelByName("relation").asInt();
		if(relation == 1)
		{
			if(cell->getBusy() == false)
			{
				ChatManager::getInstance()->chatWith(monster->getName());
			}
		}
		else if(relation == 2)
		{
			if(cell->getBusy() == false)
			{
				auto swap = Swap::create(this->getName(), monster->getName());
				addChild(swap);
			}
		}
		else if(relation == 3)
		{
			if(cell->getBusy() == false && monster->getIsDead() == false)
			{
				_rival = monster->getName();
				auto mhit = monster->getStr() - this->getDef();
				auto phit = this->getStr() - monster->getDef();
				mhit = mhit > 0 ? mhit:0;
				phit = phit > 0 ? phit:0;
				int mround = phit == 0 ? 10000000:ceil(monster->getHp() * 1.0 / phit);
				bool win = false;
				auto damage = mhit * (mround - 1);
				win = damage >= this->getHp() ? false : true;

				if(win == false)
				{
					PopPanel::getInstance()->confirm("canFight","type:Text---text:"+ a2u("实力不济，确定要去###type:Text---text:送死|color:{255,0,0}###type:Text---text:？"),PromptPanel::TYPE_DONE_CANCEL,[=](){
						auto fight = Fight::create(this->getName(), _rival);
						addChild(fight);
					},nullptr,true,true);
				}
				else
				{
					auto fight = Fight::create(this->getName(), _rival);
					addChild(fight);
				}
			}
		}
	}
}

void Actor::inVision(Cell* cell)
{
	Cell::inVision(cell);
}

void Actor::outVision(Cell* cell)
{
	Cell::outVision(cell);
}
// ################################################  碰撞函数  ##################################################

int Actor::isBlock()
{
	return _model["isBlock"].asInt();
}

void Actor::dead()
{
	conditions[getName()+"_dead"] = LuaValue::intValue(1);
	conditions[getName()+"_isDead"] = LuaValue::intValue(1);
	setIsDead(true);
	if(getModelByName("noDead").isNull())
	{
		runAction(Sequence::create(DelayTime::create(0.2), CallFunc::create([=](){Detect::shareDetect()->removeCell(this);}),nullptr));
	}
	EventHelper::getInstance()->dispatchByStr("target:"+getName()+"|type:isDead","");
}

void Actor::setPosition(const Vec2& pos)
{
	Cell::setPosition(pos);
	if(_shader != nullptr)
		_shader->update(pos);
}

Actor::~Actor()
{
	CC_SAFE_RELEASE(_diary);
	CC_SAFE_RELEASE(_sparDisk);
	CC_SAFE_RELEASE(_itemBag);
}

void Actor::addPropByType(int typeId, int num)
{
	while(num--)
	{
		ValueMap model;
		model["typeId"] = typeId;
		auto prop = Prop::create(model);
		addProp(prop);
	}
}

void Actor::addEquipByType(int typeId)
{
	ValueMap model;
	model["typeId"] = typeId;
	auto equip = Equip::create(model);
	addEquip(equip);
}

void Actor::addSpecialByType(int typeId, int num)
{
	while(num--)
	{
		ValueMap model;
		model["typeId"] = typeId;
		auto special = Special::create(model);
		addSpecial(special);
	}	
}

void Actor::addSparByType(int typeId)
{
	ValueMap model;
	model["typeId"] = typeId;
	auto spar = Spar::create(model);
	addSpar(spar);
}

void Actor::removePropByType(int typeId, int num)
{
	_itemBag->removePropByType(typeId, num);
	conditions[getName() + "_has_" + cocos2d::Value(typeId).asString() + "_num"] = LuaValue::intValue(_itemBag->getPropSizeByType(typeId));
}

void Actor::removeSpecialByType(int typeId, int num)
{
	_itemBag->removeSpecialByType(typeId, num);
	conditions[getName() + "_has_" + cocos2d::Value(typeId).asString() + "_num"] = LuaValue::intValue(_itemBag->getSpecialSizeByType(typeId));
}

void Actor::addEquip(Equip *equip)
{
	int typeId = equip->getTypeId();
	std::string name = equip->getName();
	_itemBag->addEquip(equip);
	conditions[getName() + "_has_" + name] = LuaValue::intValue(1);
	conditions[getName() + "_has_" + cocos2d::Value(typeId).asString() + "_num"] = LuaValue::intValue(1);
}

void Actor::addSpar(Spar *spar)
{
	int typeId = spar->getTypeId();
	std::string name = spar->getName();
	_sparDisk->addSpar(spar);
	conditions[getName() + "_has_" + name] = LuaValue::intValue(1);
	conditions[getName() + "_has_" + cocos2d::Value(typeId).asString() + "_num"] = LuaValue::intValue(1);
}

void Actor::addProp(Prop *prop)
{
	int typeId = prop->getTypeId();
	std::string name = prop->getName();
	_itemBag->addProp(prop);
	conditions[getName() + "_add_" + name] = LuaValue::intValue(1);
	conditions[getName() + "_has_" + name] = LuaValue::intValue(1);
	conditions[getName() + "_has_" + cocos2d::Value(typeId).asString() + "_num"] = LuaValue::intValue(_itemBag->getPropSizeByType(typeId));
}

void Actor::addSpecial(Special *special)
{
	int typeId = special->getTypeId();
	std::string name = special->getName();
	_itemBag->addSpecial(special);
	conditions[getName() + "_has_" + name] = LuaValue::intValue(1);
	conditions[getName() + "_has_" + cocos2d::Value(typeId).asString() + "_num"] = LuaValue::intValue(_itemBag->getSpecialSizeByType(typeId));
}

void Actor::removeProp(Prop *prop)
{
	int typeId = prop->getTypeId();
	std::string name = prop->getName();
	_itemBag->removeProp(prop);
	conditions[getName() + "_has_" + name] = LuaValue::intValue(0);
	conditions[getName() + "_has_" + cocos2d::Value(typeId).asString() + "_num"] = LuaValue::intValue(_itemBag->getPropSizeByType(typeId));
}

bool Actor::onTrigger(EventCustom *event)
{
	Cell::onTrigger(event);
	auto data = static_cast<ValueMap*>(event->getUserData());
	auto effect = (*data)["effect"].asValueMap();
	if((*data)["type"].asString() == "face")
	{
		int dir = DOWN;
		if(!effect["dir"].isNull())
		{
			dir = effect["dir"].asInt();
		}
		else if(!effect["toCell"].isNull())
		{
			Vec2 site;
			auto cell = Detect::shareDetect()->getCellByName(effect["toCell"].asString());
			if(cell != nullptr) site = cell->getSite();
			int disX = site.x - _site.x;
			int dixY = site.y - _site.y;
			if(disX != 0)
				dir = disX > 0 ? RIGHT:LEFT;
			if(dixY != 0)
				dir = dixY > 0 ? UP:DOWN;
		}
		animate(_ani,Direction(dir));
	}
	else if((*data)["type"].asString() == "flyOff")
	{
		Vec2 position(0,0);
		float duration = 1;
		if(!effect["position"].isNull())
		{
			position = PointFromString(effect["position"].asString());
		}
		if(!effect["duration"].isNull())
		{
			duration = effect["duration"].asFloat();
		}
		this->runAction(MoveTo::create(duration,position));
	}
	else if((*data)["type"].asString() == "dead")
	{
		dead();
	}
	else if((*data)["type"].asString() == "isDead")
	{
		log("isDead");
	}
	
	else if((*data)["type"].asString() == "useProp")
	{
		int typeId; 
		if(!(*data)["senderTypeId"].isNull())
			typeId = (*data)["senderTypeId"].asInt();
		else
			typeId = effect["typeId"].asInt();
		auto num = effect["num"].asInt();
		log("I use a prop[%d].",typeId);
		usePropByType(typeId,num);
	}
	else if((*data)["type"].asString() == "guideTo")
	{
		Vec2 site(0,0);
		if(!effect["site"].isNull())
		{
			site = PointFromString(effect["site"].asString());
		}
		else if(!effect["toCell"].isNull())
		{
			auto cell = Detect::shareDetect()->getCellByName(effect["toCell"].asString());
			if(cell != nullptr) site = cell->getSite();
		}
		else if(!effect["toCellTypeId"].isNull())
		{
			auto cell = getFloor()->getCellByType(effect["toCellTypeId"].asInt());
			if(cell != nullptr) site = cell->getSite();
		}
		moveToSite(site);
	}
	else if((*data)["type"].asString() == "setGuide")
	{
		auto guide = effect["guide"].asInt();
		auto value = effect["value"].asInt();
		setGuide((Direction)guide,value);
	}
	else if((*data)["type"].asString() == "chat")
	{
		log("I chat a role[%s]",(*data)["senderName"].asString().c_str());
		if(!effect["with"].isNull())
		{
			ChatManager::getInstance()->chatWith(effect["with"].asString());
		}
	}
	else if((*data)["type"].asString() == "setChatId")
	{
		auto id = effect["chatId"].asInt();
		_chatId = id;
	}
	// 战斗
	else if((*data)["type"].asString() == "fight")
	{
		auto rival = effect["with"].asString();
		auto fight = Fight::create(this->getName(), rival);
		addChild(fight);
	}
	else if((*data)["type"].asString() == "stop")
	{
		_paths = nullptr;
		//_dis = Vec2(0,0);
		resetGuides();
	}
	else if((*data)["type"].asString() == "dead")
	{
		//Detect::getInstance()->removeCellByName(getName());
	}
	else if((*data)["type"].asString() == "addSpeed")
	{
		auto num = effect["num"].asInt();
		_saveSpeed += num;
		_speed += num;
	}
	else if((*data)["type"].asString() == "setSpeed")
	{
		auto num = effect["num"].asInt();
		_speed = num;
	}
	else if((*data)["type"].asString() == "resumeSpeed")
	{
		_speed -= _saveSpeed;
	}
	else if((*data)["type"].asString() == "setSite")
	{
		auto site = PointFromString(effect["site"].asString());
		setSite(site);
	}
	else if((*data)["type"].asString() == "addPropByType")
	{
		auto typeId = effect["typeId"].asInt();
		auto num = effect["num"].asInt();
		addPropByType(typeId,num);
	}
	else if((*data)["type"].asString() == "removePropByType")
	{
		auto typeId = effect["typeId"].asInt();
		auto num = effect["num"].asInt();
		removePropByType(typeId,num);
	}
	else if((*data)["type"].asString() == "addEquipByType")
	{
		auto typeId = effect["typeId"].asInt();
		addEquipByType(typeId);
	}
	else if((*data)["type"].asString() == "addSpecialByType")
	{
		auto typeId = effect["typeId"].asInt();
		auto num = effect["num"].asInt();
		addSpecialByType(typeId,num);
	}
	else if((*data)["type"].asString() == "getGemByType")
	{
		auto typeId = effect["typeId"].asInt();
		auto num = effect["num"].asInt();
		getGemByType(typeId,num);
	}
	else if((*data)["type"].asString() == "setHp")
	{
		auto num = effect["num"].asInt();
		setHp(num);
	}
	else if((*data)["type"].asString() == "addHp")
	{
		auto num = effect["num"].asInt();
		addHp(num);
	}
	else if((*data)["type"].asString() == "addStr")
	{
		auto num = effect["num"].asInt();
		addStr(num);
	}
	else if((*data)["type"].asString() == "addDef")
	{
		auto num = effect["num"].asInt();
		addDef(num);
	}
	else if((*data)["type"].asString() == "setStr")
	{
		auto num = effect["num"].asInt();
		setStr(num);
	}
	else if((*data)["type"].asString() == "setDef")
	{
		auto num = effect["num"].asInt();
		setDef(num);
	}
	else if((*data)["type"].asString() == "setGold")
	{
		auto num = effect["num"].asInt();
		setGold(num);
	}
	else if((*data)["type"].asString() == "setXp")
	{
		auto num = effect["num"].asInt();
		setXp(num);
	}
	else if((*data)["type"].asString() == "addGold")
	{
		auto num = effect["num"].asInt();
		addGold(num);
	}
	else if((*data)["type"].asString() == "writeDiary")
	{
 		auto type = effect["type"].asString();
		auto content = effect["content"].asString();
		if(type == "tip")
		{
			_diary->addTip(content);
		}
		else if(type == "task")
		{
			_diary->addTask(content);
		}
		else if(type == "map")
		{
			_diary->addMap(content);
		}
	}
	else if((*data)["type"].asString() == "showDiary")
	{
		auto type = effect["type"].asString();
		auto content = effect["content"].asString();
		_diary->show(type,content);
		PopPanel::getInstance()->addPanel(_diary,1);
	}
	else if((*data)["type"].asString() == "disappear")
	{
		disappear();
	}
	return false;
}

void Actor::getGemByType(int typeId, int num)
{
	Gem* gem;
	while(num--)
	{
		gem = Gem::create(typeId);

		switch (gem->getTypeId())
		{
		case 2003001:case 2003002:case 2003003:
			EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:use_drug");
			break;
		case 2003004:
			EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:pickup_gem_1");
			break;
		case 2003005:
			EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:pickup_gem_2");
			break;
			default:
				break;
		}

		auto effect = gem->getEffectMap();
		for(auto pair:effect)
		{
			if(pair.first == "str")
			{
				addStr(pair.second.asInt());
				log("str:%s=%d",pair.second.asString().c_str(),_str);
			}
			else if(pair.first == "def")
			{
				addDef(pair.second.asInt());
				log("def:%s=%d",pair.second.asString().c_str(),_def);
			}
			else if(pair.first == "xp")
			{
				addXp(pair.second.asInt());
				log("xp:%s=%d",pair.second.asString().c_str(),_xp);
			}
			else if(pair.first == "hp")
			{
				addHp(pair.second.asInt());
				log("hp:%s=%d",pair.second.asString().c_str(),_hp);
			}
			else if(pair.first == "gold")
			{
				addGold(pair.second.asInt());
				log("gold:%s=%d",pair.second.asString().c_str(),_hp);
			}
			else if(pair.first == "buff")
			{
				Detect::shareDetect()->getUser()->addBuff(Buff::create(pair.second.asInt()));
			}
		}
	}

	EventHelper::getInstance()->dispatchByStr("target:"+gem->getName()+"|type:used","");
}

void Actor::usePropByType(int typeId, int num)
{
	Prop* prop;
	bool use = true;
	while(num--)
	{
		if(_itemBag->getPropSizeByType(typeId) == 0)
		{
			return;
		}
		prop = _itemBag->getPropByType(typeId);
		// 笔记撕页或藏宝图
		if(prop->getTypeId1() == 4 && prop->getType() == "Prop")
		{
			if(_itemBag->getSpecialByType(2007002) == nullptr)
			{
				PopPanel::getInstance()->note("treaMap","type:Text---text:"+ a2u("找到笔记本才可以查看"),1.0f,false,true);
				return;
			}
			auto type = prop->getModelByName("treaType").asString();
			auto name = "type:Text---text:" + prop->getModelByName("treaName").asString();
			std::string text = "type:Text---text:"+ a2u("恭喜获得");
			if(type == "tip")
			{
				_diary->addTip(name);
				text += a2u("重要情报");
			}
			else if(type == "task")
			{
				_diary->addTask(name);
				text += a2u("任务提示");
			}
			else if(type == "map")
			{
				_diary->addMap(name);
				text += a2u("藏宝图");
			}
			text += a2u(",请查看笔记本");
			if(!prop->getModelByName("treaText").isNull())
			{
				text = prop->getModelByName("treaText").asString();
			}
			PopPanel::getInstance()->confirm("treaMap",text,PromptPanel::TYPE_DONE,nullptr,nullptr,true,true);
		}
		// 锤头
		if(prop->getTypeId1() == 14 && prop->getType() == "Prop")
		{
			const Vec2 vecs[1] = {_faceSite - _site};
			const int typeIds[1] = {2005009};
			auto cell = _floor->getNearByTypeId(_site,vecs,1,typeIds,1);
			if(cell != nullptr && cell->getModelByName("notSmash").isNull())
			{
				auto sc = dynamic_cast<Switch*>(cell);
				sc->on();

				ValueMap value2;
				value2["typeId"] = 3001001;
				auto effect2 = static_cast<Effect*>(CellFactory::getInstance()->createCell(value2));
				Detect::shareDetect()->addCell(effect2,cell->getSite(),4,_floor->getId(),_floor->getTower()->getId());
				effect2->erupt();
			}
			else
			{
				use = false;
				PopPanel::getInstance()->note("cantDig","type:Text---text:"+ a2u("无法使用锤头"));
			}
		}
		// 炸药包
		if(prop->getTypeId1() == 15 && prop->getType() == "Prop")
		{
			const Vec2 vecs[1] = {_faceSite - _site};
			const std::string types[2] = {"Monster","Monster"};
			auto cell = _floor->getNearByType(_site,vecs,1,types,2);
			if(cell != nullptr)
			{
				if(cell->getModelByName("canBomb").asInt() == 1)
				{
					ValueMap value2;
					value2["typeId"] = 3001009;
					auto effect2 = static_cast<Effect*>(CellFactory::getInstance()->createCell(value2));
					effect2->setClearTime(1);
					BlendFunc func = {GL_ONE_MINUS_DST_COLOR, GL_ONE};
					effect2->getView()->setBlendFunc(func);
					Detect::shareDetect()->addCell(effect2,cell->getSite(),4,_floor->getId(),_floor->getTower()->getId());
					effect2->erupt();

					cell->disappear();

					_useBombNum++;
				}
				else
				{
					use = false;
					PopPanel::getInstance()->note("cantDig","type:Text---text:"+ a2u("炸药对他无效"));
				}
			}
			else
			{
				use = false;
				PopPanel::getInstance()->note("cantDig","type:Text---text:"+ a2u("无法炸到任何东西"));
			}
		}
		// 锄头
		else if(prop->getTypeId1() == 10)
		{
			auto cells = _floor->getUnit(_site)->getCells();
			bool canDig = false;
			Cell* locate = nullptr;
			cocos2d::Value fixTrea;
			for(auto cell:cells)
			{
				// 藏宝的地方
				if(cell->getTypeId() == 2005002)
				{
					canDig = true;
					locate = cell;
					if(!cell->getModelByName("fixTrea").isNull())
					{
						fixTrea = cell->getModelByName("fixTrea");
					}
					break;
				}
			}
			Point appearSite;
			// 地点
			std::vector<Point> points;	
			for(auto unit : CrossNoCenterVecs)
			{
				Point p(_site);
				p += unit;
				if(_blockFunc(p) == 0)
				{
					points.push_back(p);
				}
			}

			if(points.size() == 0)
			{
				canDig = false;
			}
			else
			{
				appearSite = _site;//points.at(CCRANDOM_0_1()*(points.size()-1));
			}

			if(canDig == true)
			{
				Cell* treasure = nullptr;
				if(!fixTrea.isNull())
				{
					treasure = Detect::shareDetect()->getCellByName(fixTrea.asString());
				}
				else
				{
					auto treaRates = prop->getEffectMap();
					ValueMap locateRates;
					if(!locate->getModelByName("rate").isNull())
					{
						stringToValueMap(locate->getModelByName("rate").asString(),locateRates);
					}

					for(auto pair1:treaRates)
					{
						for(auto pair2:locateRates)
						{
							if(pair1.first == pair2.first)
							{
								treaRates[pair1.first] = pair1.second.asInt() + pair2.second.asInt();
							}
						}
					}

					ValueVector treaTypes;
					ValueVector treaRateAreas;
					for(auto pair1:treaRates)
					{
						float rate = 0;
						if(treaRateAreas.size() > 0)
							rate = treaRateAreas.at(treaRateAreas.size()-1).asFloat();
						rate += pair1.second.asFloat();

						treaTypes.push_back(cocos2d::Value(pair1.first));
						treaRateAreas.push_back(cocos2d::Value(rate));
					}

					float random = CCRANDOM_0_1()*treaRateAreas.at(treaRateAreas.size()-1).asFloat();

					int i = 0;
					for(auto pair:treaRateAreas)
					{
						if(random <= pair.asFloat())
							break;
						i++;
					}
					int treasureType = treaTypes.at(i).asInt();
					ValueMap model;
					model["typeId"] = treasureType;
					model["name"] = "treasure_" + cocos2d::Value(cellCount++).asString();
					treasure = CellFactory::getInstance()->createCell(model);
					if(treasure != nullptr)
						treasure->retain();
				}

				//使用了一次锄头
				_useHoeNum++;
				auto findTreasure = true;
				if(treasure != nullptr)
				{
					if(Detect::shareDetect()->getCellByName(treasure->getName()) != nullptr)
					{
						runAction(Sequence::create(DelayTime::create(1.0),CallFunc::create([=](){Detect::shareDetect()->convey(treasure,appearSite,1,_floor->getId(),_floor->getTower()->getId());}),nullptr));
					}
					else
					{
						runAction(Sequence::create(DelayTime::create(1.0),CallFunc::create([=](){Detect::shareDetect()->addCell(treasure,appearSite,1,_floor->getId(),_floor->getTower()->getId());treasure->release();}),nullptr));
					}
					//Detect::shareDetect()->removeCell(locate);
				}
				else
				{
					PopPanel::getInstance()->note("cantDig","type:Text---text:"+ a2u("什么也没挖到"));
				}
			}
			else
			{
				use = false;
				PopPanel::getInstance()->note("cantDig","type:Text---text:"+ a2u("这个地方看来不能挖下去"));
			}
		}
		else
		{
			auto effect = prop->getEffectMap();
			for(auto pair:effect)
			{
				if(pair.first == "str")
				{
					addStr(pair.second.asInt());
					log("str:%s=%d",pair.second.asString().c_str(),_str);
				}
				else if(pair.first == "def")
				{
					addDef(pair.second.asInt());
					log("def:%s=%d",pair.second.asString().c_str(),_def);
				}
				else if(pair.first == "xp")
				{
					addXp(pair.second.asInt());
					log("xp:%s=%d",pair.second.asString().c_str(),_xp);
				}
				else if(pair.first == "hp")
				{
					addHp(pair.second.asInt());
					log("hp:%s=%d",pair.second.asString().c_str(),_hp);
				}
				else if(pair.first == "gold")
				{
					addGold(pair.second.asInt());
					log("gold:%s=%d",pair.second.asString().c_str(),_hp);
				}
				else if(pair.first == "doubleHp")
				{
					addHp(getHp());
					log("gold:%s=%d",pair.second.asString().c_str(),_hp);
				}
				else if(pair.first == "doubleSpeed")
				{
					_speed *= 2;
				}
				else if(pair.first == "halfFightDelay")
				{
					_fightDelay *= 0.5;
				}
			}
		}
		// 书信
		if(prop->getTypeId1() == 16 && prop->getType() == "Prop")
		{
			auto effect = prop->getEffectMap();
			std::string text;
			if(!effect["textNum"].isNull())
			{
				auto paper = PaperPanel::create("paper");
				paper->initWithTextNum(effect["textNum"].asInt());
				PopPanel::getInstance()->addPanel(paper,1);
			}
			else
			{
				text = effect["text"].asString();
				auto paper = PaperPanel::create("paper");
				paper->initWithText(text);
				PopPanel::getInstance()->addPanel(paper,1);
			}
			use = false;
		}

		EventHelper::getInstance()->dispatchByStr("target:"+prop->getName()+"|type:used","");

		if(use == true)
		{
			prop->addDurable(-1);
			if(prop->getDurable() <= 0)
			{
				removeProp(prop);
			}
		}
	}	
}

float Actor::getFightDelay()
{
	return _fightDelay * Detect::shareDetect()->getUser()->getMultiFightDelay();
}

ValueMap Monster::saveModel()
{
	auto model = Actor::saveModel();

	return model;
}

Monster* Monster::create(int typeId)
{
	Monster *pRet = new Monster(typeId);
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

Monster* Monster::create(ValueMap& model)
{
	Monster *pRet = new Monster(model);
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


void Monster::initNull()
{
	Actor::initNull();
	_jiaji = true;
}

void Monster::initModel()
{
	Actor::initModel();
}

void Monster::initOthers()
{
	Actor::initOthers();
	if(!_model["attrs"].isNull())
	{
		ValueVector attrMap;
		stringToValueVector(_model["attrs"].asString(), "|", attrMap);
		for(auto pair:attrMap)
		{
			_attrs.push_back(pair.asString());
		}
		removeAttrs("");
	}

	if(!_model["attrsData"].isNull())
	{
		stringToValueMap(_model["attrsData"].asString(), _attrsData);

		if(hasAttrs("xixue"))
		{
			if(_attrsData["xixueRate"].isNull())
			{
				auto xixueRate = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "xixue","rate")).asFloat();
				_attrsData["xixueRate"] = xixueRate;
			}
			if(_attrsData["xixueAbsorb"].isNull())
			{
				auto xixueAbsorb = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "xixue","absorb")).asInt();
				_attrsData["xixueAbsorb"] = xixueAbsorb;
			}
		}
		if(hasAttrs("jinghua"))
		{
			if(_attrsData["jinghuaRate"].isNull())
			{
				auto jinghuaRate = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "jinghua","rate")).asFloat();
				_attrsData["jinghuaRate"] = jinghuaRate;
			}
		}
		if(hasAttrs("zuji"))
		{
			if(_attrsData["zujiRate"].isNull())
			{
				auto jinghuaRate = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "zuji","rate")).asFloat();
				_attrsData["zujiRate"] = jinghuaRate;
			}
		}
	}
}

bool Monster::hasAttrs(std::string type)
{
	vector<std::string>::iterator itr = _attrs.begin();
	while (itr != _attrs.end())
	{
		if (*itr == type)
			return true;
		++itr;
	}
	return false;
}

void Monster::removeAttrs(std::string type)
{
	vector<std::string>::iterator itr = _attrs.begin();
	while (itr!=_attrs.end())
	{
		if (*itr == type)
			itr = _attrs.erase(itr);
		else
			itr++;
	}
}

void Monster::addAttrs(std::string type)
{
	_attrs.push_back(type);
}

std::vector<std::string> Monster::getAttrs()
{
	return _attrs;
}

cocos2d::Value Monster::getAttrData(std::string name)
{
	return _attrsData[name];
}

void Monster::inVision(Cell* cell)
{
	Actor::inVision(cell);
	if(cell->getType() == "Player")
	{
		auto player = dynamic_cast<Player*>(cell);
		if(this->hasAttrs("zuji"))
		{
			player->addHp(_attrsData["zujiDam"].asInt());
			auto backSite = this->getSite() - player->getSite() + this->getSite();
			this->jumpTo(backSite);
		}
		else if(this->hasAttrs("jiaji"))
		{
			auto oppSite = player->getSite() - (this->getSite() - player->getSite());
			auto cells = _floor->getUnit(oppSite)->getCells();
			for(auto ce:cells)
			{
				if(ce->getTypeId() == this->getTypeId())
				{
					auto otherMonster = dynamic_cast<Monster*>(ce);
					if(otherMonster->getJiaji() == true)
					{
						player->addHp(-player->getHp() / 2);
						this->setJiaji(false);
					}
				}
			}
		}
	}
}

void Monster::outVision(Cell* cell)
{
	Actor::outVision(cell);
	if(this->hasAttrs("jiaji"))
		this->setJiaji(true);
}

Player* Player::create(int typeId)
{
	Player *pRet = new Player(typeId);
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

Player* Player::create(ValueMap& model)
{
	Player *pRet = new Player(model);
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

void Player::initModel()
{
	Actor::initModel();
}

ValueMap Player::saveModel()
{
	auto model = Actor::saveModel();

	auto curTime = getTime();
	_durTime += difftime(curTime, _initTime);

	model["durTime"] = _durTime;
	return model;
}

void Player::initOthers()
{	
	Actor::initOthers();
}

void Player::initNull()
{
	Actor::initNull();
	_shader = Shader::create("shade.png", "sten.png");
	_fightDelay = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "fightNew","roundDelay")).asFloat();
}

void Player::refresh()
{
	Actor::refresh();
	updateToHud();
}

void Player::onEnter()
{
	Actor::onEnter();
}

ItemBag* Player::getItemBag()
{
	return _itemBag;
}

SparDisk* Player::getSparDisk()
{
	return _sparDisk;
}

DiaryPanel* Player::getDiary()
{
	return _diary;
}

void Player::update(float delta)
{
	Actor::update(delta);	
}

void Player::setHp(int hp)
{
	Actor::setHp(hp);
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:setHp", StringUtils::format("num:%d|maxHp:%d",getHp(),_maxHp));
#if DETECT_ACTOR == 1
	Detect::shareDetect()->getCurFloor()->updateFight();
#endif
}
void Player::setStr(int str)
{
	Actor::setStr(str);
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:setStr", StringUtils::format("num:%d",getStr()));
#if DETECT_ACTOR == 1
	Detect::shareDetect()->getCurFloor()->updateFight();
#endif
}
void Player::setDef(int def)
{
	Actor::setDef(def);
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:setDef", StringUtils::format("num:%d",getDef()));
#if DETECT_ACTOR == 1
	Detect::shareDetect()->getCurFloor()->updateFight();
#endif
}
void Player::setMdef(int mdef)
{
	Actor::setMdef(mdef);
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:setDef", StringUtils::format("num:%d",getMdef()));
#if DETECT_ACTOR == 1
	Detect::shareDetect()->getCurFloor()->updateFight();
#endif
}
void Player::setXp(int xp)
{
	Actor::setXp(xp);
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:setXp", StringUtils::format("num:%d|lastXp:%d|nextXp:%d",getXp(),_lastXp,_nextXp));
}

void Player::setGold(int gold)
{
	Actor::setGold(gold);
	_itemBag->setGold(_gold);
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:setGold", StringUtils::format("num:%d",getGold()));
}

void Player::convey(Vec2 site, int layerId, int floorId, int towerId)
{
	Actor::convey(site, layerId, floorId, towerId);
	if(getName() == Detect::shareDetect()->getPlayer()->getName())
	{
		Detect::shareDetect()->setCurFloor(floorId, towerId);
	}
}

void Player::setLevel(int level)
{
	auto oldLevel = _level;
	auto newLevel = level;

	if(oldLevel < 1)
	{
		oldLevel = 1;
	}
	
	int addHpNum = 0;
	int addStrNum = 0;
	int addDefNum = 0;
	auto i = oldLevel;
	for(i++; i <= newLevel; i++)
	{
		std::string rewardStr;
		if(DATA_MODE == 1)
		{
			rapidjson::Document &datas = Model::shareModel()->levelDatas;
			auto &data = Model::GetDataFromCol1(datas,i);
			rewardStr = DICHELPER->getStringValue_json(data,"reward");
		}
		else if(DATA_MODE == 2)
		{
			rewardStr = Model::shareModel()->levelCsv->getValueByIdAndName(i, "reward").asString();
		}

		ValueMap reward;
		stringToValueMap(rewardStr, reward);
		for(auto pair:reward)
		{
			if(pair.first == "str")
			{
				addStrNum += pair.second.asInt();
				addStr(pair.second.asInt());
				log("str:%s=%d",pair.second.asString().c_str(),_str);
			}
			else if(pair.first == "def")
			{
				addDefNum += pair.second.asInt();
				addDef(pair.second.asInt());
				log("def:%s=%d",pair.second.asString().c_str(),_def);
			}
			else if(pair.first == "xp")
			{
				addXp(pair.second.asInt());
				log("def:%s=%d",pair.second.asString().c_str(),_xp);
			}
			else if(pair.first == "hp")
			{
				addHpNum += pair.second.asInt();
				addHp(pair.second.asInt());
				log("hp:%s=%d",pair.second.asString().c_str(),_hp);
			}
		}
	}

	if(newLevel > oldLevel)
	{
		EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:uplevel");

		int nums[] = {addHpNum, addStrNum, addDefNum};
		auto panel = LevelUpPanel::create("levelUp");
		panel->initWithNums(nums);
		PopPanel::getInstance()->removeAllPanelsByOrder(1);
		PopPanel::getInstance()->addPanel(panel,1);
	}

	Actor::setLevel(level);
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:setLevel", StringUtils::format("num:%d",_level));
}

void Player::addSpar(Spar *spar)
{
	Actor::addSpar(spar);
	PopPanel::getInstance()->note("getSpar","type:Text---text:"+ a2u("获得了 ") + spar->getNickName());
}

void Player::addEquip(Equip *equip)
{
	Actor::addEquip(equip);
	PopPanel::getInstance()->note("getSpar","type:Text---text:"+ a2u("获得了 ") + equip->getNickName());
	if(equip->getTypeId1() == 1)
		EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:sword");
	else if(equip->getTypeId1() == 2)
		EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:shield");
}

void Player::addSpecial(Special *special)
{
	Actor::addSpecial(special);
	std::string note;
	if(special->getTypeId1() == 1)
	{
		note = "type:Text---text:"+ a2u("获得了 ") + special->getNickName() + a2u(",开启晶石系统");
	}
	else if(special->getTypeId1() == 2)
	{
		note = "type:Text---text:"+ a2u("获得了 ") + special->getNickName();
	}
	else if(special->getTypeId1() == 3)
	{
		note = "type:Text---text:"+ a2u("获得了 ") + special->getNickName() + a2u(",开启传送功能");
	}
	else if(special->getTypeId1() == 4)
	{
		Detect::shareDetect()->lookActorOpen = true;
#if DETECT_ACTOR == 1
		Detect::shareDetect()->getCurFloor()->updateFight();
#endif
		note = "type:Text---text:"+ a2u("获得了 ") + special->getNickName() + a2u(",可以查看怪物数据");
	}
	PopPanel::getInstance()->note("getSpecial",note,1.5f);

	EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:special");
}

void Player::addProp(Prop *prop)
{
	Actor::addProp(prop);
	auto typeId = prop->getTypeId();
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:addProp", StringUtils::format("propType:%d|addPropNum:%d|propNum:%d|propName:%s|site:{%f,%f}",typeId,1,_itemBag->getPropSizeByType(typeId),prop->getNickName().c_str(),getPosition().x,getPosition().y));
	switch (typeId)
	{
	case 2002001:case 2002002:case 2002003:
		EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:pickup_key");
		break;
	case 2002007:case 2002008:case 2002009:
		EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:pickup_gem_3");
		break;
	default:
		break;
	}

	// 笔记撕页、晶石碎片、锄头、藏宝图、圣水、幸运金币、锤头、炸药包、书信
	auto typeId1 = prop->getTypeId1();
	switch (typeId1)
	{
	case 4:case 5:case 10:case 11:case 12:case 13:case 14:case 15:case 16:
		PopPanel::getInstance()->note("getSpar","type:Text---text:"+ a2u("获得了 ") + prop->getNickName());
		break;
	default:
		break;
	}
}

void Player::removeProp(Prop *prop)
{
	auto typeId = prop->getTypeId();
	auto nickName = prop->getNickName().c_str();
	Actor::removeProp(prop);
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:removeProp", StringUtils::format("propType:%d|addPropNum:%d|propNum:%d|propName:%s|site:{%f,%f}",typeId,1,_itemBag->getPropSizeByType(typeId),nickName,getPosition().x,getPosition().y));
}

void Player::addPropByType(int typeId, int num)
{
	Actor::addPropByType(typeId,num);
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:addProp", StringUtils::format("propType:%d|addPropNum:%d|propNum:%d|site:{%f,%f}",typeId,num,_itemBag->getPropSizeByType(typeId),getPosition().x,getPosition().y));
}

void Player::addSpecialByType(int typeId, int num)
{
	Actor::addSpecialByType(typeId,num);
}

void Player::addEquipByType(int typeId)
{
	Actor::addEquipByType(typeId);
}

void Player::addSparByType(int typeId)
{
	Actor::addSparByType(typeId);
}

void Player::removePropByType(int typeId, int num)
{
	Actor::removePropByType(typeId, num);
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:removeProp", StringUtils::format("propType:%d|addPropNum:%d|propNum:%d|site:{%f,%f}",typeId,num,_itemBag->getPropSizeByType(typeId),getPosition().x,getPosition().y));
}

bool Player::onTrigger(EventCustom *event)
{
	Actor::onTrigger(event);
	auto data = static_cast<ValueMap*>(event->getUserData());
	auto effect = (*data)["effect"].asValueMap();
	return false;
}

void Player::dead()
{
	conditions[getName()+"_dead"] = LuaValue::intValue(1);
	conditions[getName()+"_isDead"] = LuaValue::intValue(1);
	setIsDead(true);
}

void Player::inVision(Cell* cell)
{
	Actor::inVision(cell);
}

void Player::outVision(Cell* cell)
{
	Actor::outVision(cell);
}

void Player::addHp(int hp)
{
	Actor::addHp(hp);
	if(hp <= 0) return;
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:addHp", StringUtils::format("num:%d|site:{%f,%f}",hp,getPosition().x,getPosition().y));
}

void Player::addStr(int str)
{
	Actor::addStr(str);
	if(str <= 0) return;
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:addStr", StringUtils::format("num:%d|site:{%f,%f}",str,getPosition().x,getPosition().y));
}

void Player::addDef(int def)
{
	Actor::addDef(def);
	if(def <= 0) return;
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:addDef", StringUtils::format("num:%d|site:{%f,%f}",def,getPosition().x,getPosition().y));
}

void Player::addMdef(int mdef)
{
	Actor::addMdef(mdef);
	if(mdef <= 0) return;
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:addDef", StringUtils::format("num:%d|site:{%f,%f}",mdef,getPosition().x,getPosition().y));
}

void Player::addXp(int xp)
{
	Actor::addXp(xp);
	if(xp <= 0) return;
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:addXp", StringUtils::format("num:%d|site:{%f,%f}",xp,getPosition().x,getPosition().y));
}

void Player::addGold(int gold)
{
	Actor::addGold(gold);
	if(gold <= 0) return;
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:addGold", StringUtils::format("num:%d|site:{%f,%f}",gold,getPosition().x,getPosition().y));
}

void Player::addLevel(int level)
{
	Actor::addLevel(level);
	if(level <= 0) return;
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:addLevel", StringUtils::format("num:%d|site:{%f,%f}",level,getPosition().x,getPosition().y));
}

void Player::updateToHud()
{
	setHp(_hp);
	setStr(_str);
	setDef(_def);
	setMdef(_mdef);
	setXp(_xp);
	setLevel(_level);
	setGold(_gold);

	addPropByType(2002001,0);
	addPropByType(2002002,0);
	addPropByType(2002003,0);
}
