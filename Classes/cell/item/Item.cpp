#include "Item.h"
#include "model/Model.h"
#include "editor-support/cocostudio/DictionaryHelper.h"
#include "utils/StringHandler.h"
#include "scene/GameScene.h"
#include "Detect.h"
#include "cell/CellFactory.h"
#include "utils/EventHelper.h"
#include "cell/effect/Effect.h"
#include "map/Floor.h"
#include "map/Tower.h"
#include "map/Tower.h"

Item* Item::create(int id)
{
	Item *pRet = new Item(id);
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

Item* Item::create(ValueMap& model)
{
	Item *pRet = new Item(model);
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

int Item::getTypeId1()
{
	return _model["typeId1"].asInt();
}

std::string Item::getType()
{
	return "Item";
}

void Item::initView()
{
	Cell::initView();
}

void Item::refreshModel()
{
	Cell::refreshModel();

	int typeId = _typeId;
	if(DATA_MODE == 1)
	{
		rapidjson::Document &datas = Model::shareModel()->itemDatas;
		auto &data = Model::GetDataFromCol1(datas,typeId);
		Model::JsonAddModel(data,_model);
	}
	else if(DATA_MODE == 2)
	{
		auto model = Model::shareModel()->itemCsv->getModelById(typeId);
		Model::modelCoverModel(model,_model);
	}
}

void Item::initModel()
{
	Cell::initModel();

	int typeId = _typeId;
	if(DATA_MODE == 1)
	{
		rapidjson::Document &datas = Model::shareModel()->itemDatas;
		auto &data = Model::GetDataFromCol1(datas,typeId);
		Model::JsonAddModel(data,_model);
	}
	else if(DATA_MODE == 2)
	{
		auto model = Model::shareModel()->itemCsv->getModelById(typeId);
		Model::modelAddModel(model,_model);
	}
}

void Item::loadModel(ValueMap &map)
{
	_durable = map["durable"].asInt();
}

ValueMap Item::saveModel()
{
	ValueMap model = Cell::saveModel();
	model["durable"] = _durable;
	return model;
}

void Item::initOthers()
{
	Cell::initOthers();
	// 组合触发事件
	strArray triggers;
	if(!_model["triggerD"].isNull() && _model["triggerD"].asString() != "null")
		triggers.push_back(_model["triggerD"].asString());

	if(!_model["triggerL"].isNull() && _model["triggerL"].asString() != "null")
		triggers.push_back(_model["triggerL"].asString());

	auto triggerS = _model["triggerS"].asString();
	if(triggerS != "null")
		triggers.push_back(triggerS);

	std::string trigger;
	for(int i = 0; i < triggers.size();i++)
	{
		auto str = triggers.at(i);
		if(i < triggers.size() - 1)
		{
			trigger += (str + "###");
		}
		else
		{
			trigger += str;
		}

	}
	if(_model["trigger"].isNull() || _model["trigger"].asString() == "null")
	{
		_model["trigger"] = trigger;
	}


	if(_model["speed"].isNull())
	{
		_model["speed"] = _model["speed_init"];
	}
	_speed = _model["speed"].asFloat();

	_effect = ValueMap();
	stringToValueMap(_model["effect"].asString(), _effect);
	this->setName(_model["name"].asString());
	_isBlock = _model["isBlock"].asInt();

	_durable = 0;
	if(_model["durable"].isNull())
	{
		_model["durable"] = _model["durable_init"];
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

	_durable = _model["durable"].asInt();
}

void Item::addDurable(int num)
{
	auto value = num + _durable;
	setDurable(value);
}

void Item::setDurable(int value)
{
	_durable = value;
}

int Item::getDurable()
{
	return _durable;
}

void Item::onEnter()
{
	Cell::onEnter();
}
//
//Vec2 Item::getSite() const
//{
//	//auto vec = this->getPosition();
//	return _site;//Vec2((vec.x - TILE_HALF_WIDTH) / TILE_WIDTH, (vec.y - TILE_HALF_HEIGHT) / TILE_HEIGHT);
//}
//
//void Item::setSite(const Vec2 &vec)
//{
//	Cell::setSite(vec);
//}

void Item::touchOn(Cell* cell)
{
	Cell::touchOn(cell);
	if(getModelByName("canDrag").asInt() == 1)
	{

	}
}

void Item::touchOut(Cell* cell)
{
	Cell::touchOut(cell);
	if(getModelByName("canDrag").asInt() == 1)
	{

	}
}

bool Item::onTrigger(EventCustom *event)
{
	return false;
}

int Item::isBlock()
{
	return _isBlock;
}

ValueMap Item::getEffectMap()
{
	ValueMap effect;
	return stringToValueMap(getEffect(),effect);
}

Equip* Equip::create(int id)
{
	Equip *pRet = new Equip(id);
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

Equip* Equip::create(ValueMap& model)
{
	Equip *pRet = new Equip(model);
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

bool Equip::onTrigger(EventCustom *event)
{
	Cell::onTrigger(event);
	auto data = static_cast<ValueMap*>(event->getUserData());
	return false;
}

void Equip::loadModel(ValueMap &map)
{
	
}

ValueMap Equip::saveModel()
{
	ValueMap model = Item::saveModel();
	model["shopId"] = _model["shopId"];
	return model;
}

std::string Equip::getType()
{
	return "Equip";
}

void Equip::crashOn(Cell* cell)
{
	Item::crashOn(cell);
	if(cell->getType() == "Player")
	{
		log("Equip[%s] is pick",getNickName().c_str());
		removeFromFloor();
	}
}

Prop* Prop::create(int id)
{
	Prop *pRet = new Prop(id);
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

Prop* Prop::create(ValueMap& model)
{
	Prop *pRet = new Prop(model);
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

void Prop::crashOn(Cell* cell)
{
	Item::crashOn(cell);
	if(cell->getType() == "Player")
	{
		EventHelper::getInstance()->dispatchByStr("target:"+getName()+"|type:crashOnByPlayer","");
		log("Prop[%s] is pick",getNickName().c_str());
		removeFromFloor();
	}
}

bool Prop::onTrigger(EventCustom *event)
{
	Cell::onTrigger(event);
	return false;
}

std::string Prop::getType()
{
	return "Prop";
}

Special* Special::create(int id)
{
	Special *pRet = new Special(id);
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

Special* Special::create(ValueMap& model)
{
	Special *pRet = new Special(model);
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

bool Special::onTrigger(EventCustom *event)
{
	Cell::onTrigger(event);
	return false;
}

void Special::crashOn(Cell* cell)
{
	if(cell->getType() == "Player")
	{
		log("Special[%s] is pick",getNickName().c_str());
		removeFromFloor();
	}
}

std::string Special::getType()
{
	return "Special";
}


Door* Door::create(int id)
{
	Door *pRet = new Door(id);
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

Door* Door::create(ValueMap& model)
{
	Door *pRet = new Door(model);
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

bool Door::onTrigger(EventCustom *event)
{
	Cell::onTrigger(event);
	auto data = static_cast<ValueMap*>(event->getUserData());
	if((*data)["type"].asString() == "open")
	{
		log("Door[%s] is open",getNickName().c_str());
		open();
	}
	else if((*data)["type"].asString() == "close")
	{
		log("Door[%s] is close",getNickName().c_str());
		close();
	}
	else if((*data)["type"].asString() == "reverse")
	{
		_isOpen == 1?close():open();
	}
	else if((*data)["type"].asString() == "keyOpen" && _isOpen == 0)
	{
		log("Door[%s] is open",getNickName().c_str());
		auto target = (*data)["senderName"].asString();
		auto keyId = (*data)["effect"].asValueMap()["keyId"].asInt();
		EventHelper::getInstance()->dispatchByStr("target:"+getName()+"|type:open","");
		EventHelper::getInstance()->dispatchByStr("target:"+target+"|type:useProp|senderType:Prop|senderName:"+getName()+"|senderTypeId:"+cocos2d::Value(keyId).asString(),"num:1|inBag:1");
	}
	else if((*data)["type"].asString() == "forceOpen" && _isOpen == 0)
	{
		auto keyId = (*data)["effect"].asValueMap()["keyId"].asInt();
		EventHelper::getInstance()->dispatchByStr("target:"+getName()+"|type:open","");
	}
	conditions[getName()+"_"+(*data)["type"].asString()] = LuaValue::intValue(1);
	return false;
}

void Door::initModel()
{
	Item::initModel();
}

void Door::initOthers()
{
	Item::initOthers();
	_delayCall = Sequence::create(DelayTime::create(0.5), CallFunc::create([=](){_isBlock = 0;}), NULL);
	_delayCall->retain();
	_isOpen = 0;
	if(!_model["isOpen"].isNull())
	{
		_isOpen = _model["isOpen"].asInt();
	}
	auto animation = AnimationCache::getInstance()->getAnimation(_animations["open"].asString());
	auto frames = animation->getFrames();
	if(_isOpen == 1)
	{
		auto frame = frames.at(frames.size()-1);
		_view->setSpriteFrame(frame->getSpriteFrame());
		//setAnimation("open",false,CallFuncN::create([this](Ref* sender){}),false,0);
		_isBlock = 0;
		//stopAction(_delayCall);
		//runAction(_delayCall);
		conditions[getName()+"_isOpen"] = LuaValue::intValue(1);
	}
	else
	{
		auto frame = frames.at(0);
		_view->setSpriteFrame(frame->getSpriteFrame());
		_isBlock = 2;
		//setAnimation("close",false,CallFuncN::create([this](Ref* sender){_isBlock = 2;}),false,0);
		conditions[getName()+"_isOpen"] = LuaValue::intValue(0);
	}
}

std::string Door::getType()
{
	return "Door";
}

ValueMap Door::saveModel()
{
	auto model = Item::saveModel();
	model["isOpen"] = _isOpen;
	model["forceOpen"] = _model["forceOpen"];
	return model;
}

void Door::open()
{
	setAnimation("open",false,CallFuncN::create([this](Ref* sender){}));
	stopAction(_delayCall);
	runAction(_delayCall);
	_isOpen = 1;
	conditions[getName()+"_isOpen"] = LuaValue::intValue(1);

	if(getTypeId() != 2001001)
	{
		EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:doorOpen");
	}
}

void Door::close()
{
	setAnimation("close",false,CallFuncN::create([this](Ref* sender){}));
	_isBlock = 2;
	_isOpen = 0;
	conditions[getName()+"_isOpen"] = LuaValue::intValue(0);
}

Door::~Door()
{
	stopAction(_delayCall);
	_delayCall->release();
};

Spar* Spar::create(int id)
{
	Spar *pRet = new Spar(id);
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

Spar* Spar::create(ValueMap& model)
{
	Spar *pRet = new Spar(model);
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

bool Spar::onTrigger(EventCustom *event)
{
	Cell::onTrigger(event);
	return false;
}

std::string Spar::getType()
{
	return "Spar";
}

void Spar::initOthers()
{
	Item::initOthers();
	auto effectMap = getEffectMap();
	auto init = PointFromString(effectMap["init"].asString());
	auto max = PointFromString(effectMap["max"].asString());
	auto upLevel = PointFromString(effectMap["upLevel"].asString());
	_costPatchNum = effectMap["costPatchNum"].asInt();
	_upConfirm = effectMap["upConfirm"].asString();
	_rate = init.x;
	_drate = init.y;
	_maxRate = max.x;
	_maxDrate = max.y;
	_upRate = upLevel.x;
	_upDrate = upLevel.y;
	if(!_model["rate"].isNull())
	{
		_rate = _model["rate"].asFloat();
	}
	if(!_model["drate"].isNull())
	{
		_drate = _model["drate"].asFloat();
	}

	stringReplace(_upConfirm,"%cost",cocos2d::Value(_costPatchNum).asString());
	stringReplace(_upConfirm,"%rate",cocos2d::Value(int(_upRate*100)).asString()+"%");
	stringReplace(_upConfirm,"%drate",cocos2d::Value(int(_upDrate*100)).asString()+"%");
}

void Spar::upLevel()
{
	if(_rate < _maxRate)
	{
		_rate += _upRate;
	}
	if(_drate < _maxDrate)
	{
		_drate += _upDrate;
	}
}

void Spar::loadModel(ValueMap &map)
{
	Item::loadModel(map);
}

ValueMap Spar::saveModel()
{
	ValueMap model = Item::saveModel();
	model["rate"] = _rate;
	model["drate"] = _drate;
	return model;
}

Gem* Gem::create(int id)
{
	Gem *pRet = new Gem(id);
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

Gem* Gem::create(ValueMap& model)
{
	Gem *pRet = new Gem(model);
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

bool Gem::onTrigger(EventCustom *event)
{
	Item::onTrigger(event);
	return false;
}

void Gem::crashOn(Cell* cell)
{
	if(cell->getType() == "Player")
	{
		log("Gem[%s] is pick",getNickName().c_str());
		removeFromFloor();
	}
}

std::string Gem::getType()
{
	return "Gem";
}

Missile* Missile::create(int id)
{
	Missile *pRet = new Missile(id);
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

Missile* Missile::create(ValueMap& model)
{
	Missile *pRet = new Missile(model);
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

bool Missile::onTrigger(EventCustom *event)
{
	Item::onTrigger(event);
	return false;
}

void Missile::crashOn(Cell* cell)
{
	auto type = cell->getType();
	if(_owner != nullptr && cell == _owner)
	{
		log("Missile[%s] is hiting owner",getNickName().c_str());
		//removeFromFloor();
		return;
	}
	if(type == "Wall")
	{
		//// 爆炸效果
		ValueMap value;
		value["typeId"] = 3002001;
		auto effect = CellFactory::getInstance()->createCell(value);
		Detect::shareDetect()->addCell(effect,getSite(),4,_floor->getId(),_floor->getTower()->getId());
		destroy();
	}
}

// 碰撞到边界就销毁了
void Missile::moveToNewSite()
{
	Item::moveToNewSite();
	if(_floor == nullptr) return;
	if(_site.x >= _floor->getHorizonTileNum() + TILE_BORDER || _site.x < -TILE_BORDER || _site.y >= _floor->getVerticalTileNum() + TILE_BORDER || _site.y < -TILE_BORDER)
	{
		destroy();
	}
}

void Missile::destroy()
{
	Item::destroy();
	_effect->stopAllActions();
	this->stopAllActions();
	_effect->removeFromFloor();
	removeFromFloor();
}

std::string Missile::getType()
{
	return "Missile";
}

void Missile::initModel()
{
	Item::initModel();
}

void Missile::initOthers()
{
	Item::initOthers();
	_owner = nullptr;
}

void Missile::fire()
{
	/*ValueMap value;
	value["typeId"] = 3001008;
	_effect = static_cast<Effect*>(CellFactory::getInstance()->createCell(value));
	Detect::shareDetect()->addCell(_effect,getSite(),4,_floor->getId(),_floor->getTower()->getId());
	_effect->runAction(Follow::create(this));*/
	//this->runAction(Follow::create(_effect));
}

// 导弹
void Missile::fireToSite(Vec2 site)
{
	fire();
	moveToSite(site);
}

// 方向
void Missile::fireToAngle(float angle)
{
	auto radian = CC_DEGREES_TO_RADIANS(angle);
	auto dis = 3000;
	auto duration = dis / _speed * Director::getInstance()->getDeltaTime();
	auto pos = this->getPosition();
	Vec2 dest(pos.x + cos(radian) * dis, pos.y + sin(radian) * dis);
	auto action = MoveTo::create(duration,dest);
	this->runAction(action);

	// 火焰动画
	ValueMap value;
	value["typeId"] = 3001015;
	value["repeatTime"] = 1000;
	value["clearTime"] = -1;
	_effect = static_cast<Effect*>(CellFactory::getInstance()->createCell(value));
	BlendFunc func = {GL_ONE_MINUS_DST_COLOR, GL_ONE};
	_effect->getView()->setBlendFunc(func);
	Detect::shareDetect()->addCell(_effect,getSite(),4,_floor->getId(),_floor->getTower()->getId());
	_effect->runAction(action->clone());
	//fire();
}

// 方向
void Missile::fireToDirection(Direction dir)
{
	float angle;
	switch (dir)
	{
	case LEFT:
		angle = 180;
		break;
	case UP:
		angle = 90;
		break;
	case RIGHT:
		angle = 0;
		break;
	case DOWN:
		angle = 270;
		break;
	default:
		break;
	}
	auto radian = CC_DEGREES_TO_RADIANS(angle);
	auto dis = 3000;
	auto duration = dis / _speed * Director::getInstance()->getDeltaTime();
	auto pos = this->getPosition();
	Vec2 dest(pos.x + cos(radian) * dis, pos.y + sin(radian) * dis);
	auto action = MoveTo::create(duration,dest);
	this->runAction(action);

	// 火焰动画
	ValueMap value;
	value["typeId"] = 3001015;
	value["repeatTime"] = 1000;
	value["clearTime"] = -1;
	_effect = static_cast<Effect*>(CellFactory::getInstance()->createCell(value));
	BlendFunc func = {GL_ONE_MINUS_DST_COLOR, GL_ONE};
	_effect->getView()->setBlendFunc(func);
	Detect::shareDetect()->addCell(_effect,getSite(),4,_floor->getId(),_floor->getTower()->getId());
	_effect->runAction(action->clone());
	//fire();
}

int Switch::isOn()
{
	return _on;
}

Switch* Switch::create(int id)
{
	Switch *pRet = new Switch(id);
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

Switch* Switch::create(ValueMap& model)
{
	Switch *pRet = new Switch(model);
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

void Switch::crashOn(Cell* cell)
{
	Item::crashOn(cell);
	if(!_model["fixCell"].isNull())
	{
		cell->setCanWalk(false);
	}
	conditions[getName()+"_crashing"] = LuaValue::intValue(1);
	conditions[getName()+"_crashOn"] = LuaValue::intValue(1);
}

void Switch::crashOut(Cell* cell)
{
	Item::crashOut(cell);
	if(!_model["fixCell"].isNull())
	{
		cell->setCanWalk(true);
	}
	conditions[getName()+"_crashing"] = LuaValue::intValue(0);
	conditions[getName()+"_crashOut"] = LuaValue::intValue(1);
}

void Switch::touchOn(Cell* cell)
{
	Item::touchOn(cell);
	conditions[getName()+"_touchOn"] = LuaValue::intValue(1);
}

void Switch::stepOn(Cell* cell)
{
	Item::stepOn(cell);
	if(!_model["fixCell"].isNull())
	{
		cell->setCanWalk(false);
	}
}

void Switch::stepOut(Cell* cell)
{
	Item::stepOut(cell);
	if(!_model["fixCell"].isNull())
	{
		cell->setCanWalk(true);
	}
}

bool Switch::onTrigger(EventCustom *event)
{
	Cell::onTrigger(event);
	auto data = static_cast<ValueMap*>(event->getUserData());
	conditions[getName()+"_"+(*data)["type"].asString()] = LuaValue::intValue(1);
	if((*data)["type"].asString() == "on")
	{
		on();
	}
	else if((*data)["type"].asString() == "off")
	{
		off();
	}
	else if((*data)["type"].asString() == "delayRemove")
	{
		auto delay = (*data)["effect"].asValueMap()["delay"].asFloat();
		runAction(Sequence::create(DelayTime::create(delay),CallFunc::create([=](){this->removeFromFloor();}),nullptr));
	}
	else if((*data)["type"].asString() == "reverse")
	{
		if(_on == 1)
		{
			off();
		}
		else
		{
			on();
		}
	}
	return false;
}

std::string Switch::getType()
{
	return "Switch";
}

void Switch::initOthers()
{
	Item::initOthers();
	_on = 0;
	auto onV = _model["isOn"];
	if(!onV.isNull())
	{
		_on = onV.asInt();
	}
	if(!_animations["on"].isNull())
	{
		auto animation = AnimationCache::getInstance()->getAnimation(_animations["on"].asString());
		auto frames = animation->getFrames();
		if(_on == 1)
		{
			auto frame = frames.at(frames.size()-1);
			_view->setSpriteFrame(frame->getSpriteFrame());
			if(getTypeId() == 2005009)
			{
				_isBlock = 100;
			}
		}
		else
		{
			auto frame = frames.at(0);
			_view->setSpriteFrame(frame->getSpriteFrame());
		}
	}
}

ValueMap Switch::saveModel()
{
	auto model = Item::saveModel();
	model["trigger"] = _model["trigger"];
	model["isOn"] = _on;
	return model;
}

void Switch::on()
{
	log("Switch[%s] is on",getNickName().c_str());
	_on = 1;
	setAnimation("on",false,CallFuncN::create([this](Ref* sender){}));
	if(getTypeId() == 2005009)
	{
		_isBlock = 100;
	}
}

void Switch::off()
{
	log("Switch[%s] is off",getNickName().c_str());
	_on = 0;
	setAnimation("off",false,CallFuncN::create([this](Ref* sender){}));
}
