#include "Battery.h"
#include "utils/StringHandler.h"
#include "cell/item/Item.h"
#include "cell/CellFactory.h"
#include "Detect.h"
#include "map/Floor.h"
#include "map/Tower.h"

USING_NS_CC;

Battery* Battery::create(ValueMap model)
{
	Battery* battery = new Battery();
	if(battery != nullptr && battery->initWithModel(model))
	{
		battery->autorelease();
		return battery;
	}
	else
	{
		return nullptr;
	}
}

bool Battery::initWithModel(ValueMap model)
{
	_model = model;

	auto fireDataStr = _model["fireData"].asString();
	ValueVector vec1;
	stringToValueVector(fireDataStr, "###", vec1);
	for(auto oneStr:vec1)
	{
		ValueMap one;
		stringToValueMap(oneStr.asString(), one);

		ValueVector idValues;
		if(!one["missileIds"].isNull())
		{
			auto missileIdsStr = one["missileIds"].asString();
			strArray ids;
			split(missileIdsStr, ",", ids);
			for(auto id:ids)
			{
				idValues.push_back(cocos2d::Value(id));
			}
			
		}
		one["missileIds"] = idValues;


		ValueVector angleValues;
		if(!one["missileAngles"].isNull())
		{
			auto missileAnglesStr = one["missileAngles"].asString();
			strArray angles;
			
			split(missileAnglesStr, ",", angles);
			for(auto angle:angles)
			{
				angleValues.push_back(cocos2d::Value(angle));
			}
		}
		one["missileAngles"] = angleValues;

		_fireData.push_back(cocos2d::Value(one));
	}

	Sequence* _seq;
	Vector<FiniteTimeAction*> actions;
	for(auto oneV:_fireData)
	{
		FiniteTimeAction* oneAction;

		auto one = oneV.asValueMap();
		auto type = one["type"].asString();

		if(type == "fire")
		{
			auto missileIds = one["missileIds"].asValueVector();

			auto missileAngles = one["missileAngles"].asValueVector();

			oneAction = CallFunc::create([=](){
				int i = 0;
				for(auto idV:missileIds)
				{
					auto id = idV.asInt();
					auto angle = missileAngles.at(i);
					ValueMap model;
					model["typeId"] = id;
					auto missile = dynamic_cast<Missile*>(CellFactory::getInstance()->createCell(model));
					missile->setOwner(_target);
					Detect::shareDetect()->addCell(missile, _target->getSite(),4, _target->getFloor()->getId(), _target->getFloor()->getTower()->getId());
					if(angle.asString() == "face")
						missile->fireToDirection(_target->getFace());
					else
						missile->fireToAngle(angle.asFloat());
					i++;
				}
				;});
		}
		else if(type == "wait")
		{
			auto duration = one["duration"].asFloat();
			oneAction = DelayTime::create(duration);
		}
		actions.pushBack(oneAction);
	}
	_seq = Sequence::create(actions);
	if(!_model["loop"].isNull())
	{
		_fireAction = RepeatForever::create(_seq);
	}
	else
		_fireAction = _seq;
	_fireAction->retain();

	return true;
}

void Battery::setTarget(Cell* cell)
{
	_target = cell;
}

void Battery::fire()
{
	if(_fireAction != nullptr)
		_target->runAction(_fireAction);
}

void Battery::stop()
{
	if(_fireAction != nullptr)
		_target->stopAction(_fireAction);
}

Battery::~Battery()
{
	CC_SAFE_RELEASE_NULL(_fireAction);
}
