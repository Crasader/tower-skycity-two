#include "Trial.h"
#include "model/Model.h"
#include "editor-support/cocostudio/DictionaryHelper.h"
#include "utils/StringHandler.h"
#include "utils/Tools.h"
#include "Detect.h"
#include "UI/TrialPanel.h"
#include "UI/PopPanel.h"
#include "cell/actor/Actor.h"

Trial::Trial(int id)
{
	_id = id;
}

Trial::Trial(ValueMap model)
{
	_model =  model;
	_id = _model["id"].asInt();
}


Trial* Trial::create(int id)
{
	Trial *ret = new Trial(id);	
	if (ret && ret->init())
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}

Trial* Trial::create(ValueMap model)
{
	Trial *ret = new Trial(model);
	if (ret && ret->init())
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}

bool Trial::init()
{
	initModel();
	initOthers();
	return true;
}

void Trial::loadModel(ValueMap &model)
{
	ValueVector achievementsModel = model["achievements"].asValueVector();
	for(auto achievementModel:achievementsModel)
	{
		auto id = achievementModel.asValueMap()["id"].asInt();
		for(auto achievement:_achievements)
		{
			if(achievement->getId() == id)
			{
				achievement->loadModel(achievementModel.asValueMap());
			}
		}
	}
	_unLock = (bool)model["unLock"].asInt();
}

void Trial::initModel()
{
	int id = _id;
	if(DATA_MODE == 1)
	{
		rapidjson::Document &datas = Model::shareModel()->trialDatas;
		auto &data = Model::GetDataFromCol1(datas,id);
		Model::JsonAddModel(data,_model);
	}
	else if(DATA_MODE == 2)
	{
		auto model = Model::shareModel()->trialCsv->getModelById(id);
		Model::modelAddModel(model,_model);
	}
}

void Trial::initOthers()
{
	_unLock = false;

	auto achievementsStr = _model["achievements"].asString();
	ValueVector achievements;
	if(achievementsStr != "")
		stringToValueVector(achievementsStr,",",achievements);
	for(auto value:achievements)
	{
		auto id = value.asInt();
		auto achievement = Achievement::create(id);
		achievement->setTrial(this);
		_achievements.pushBack(achievement);
	}
}

ValueMap Trial::saveModel()
{
	ValueMap model;
	ValueVector achievements;
	for(auto achievement:_achievements)
	{
		achievements.push_back(cocos2d::Value(achievement->saveModel()));
	}
	model["achievements"] = achievements;
	model["unLock"] = (int)_unLock;
	return model;
}

bool Trial::onTrigger(EventCustom *event)
{
	return false;
}

const std::string Trial::getNameId()
{
	return "0";
}

std::string Trial::getType()
{
	return "Trial";
}

void Trial::ready()
{
	// 切换地图 切换角色
	auto floor = _model["floor"].asString();
	if(floor == "") return;
	ValueVector vector;
	stringToValueVector(floor, ",", vector);
	auto detect = Detect::shareDetect();
	detect->getFloor(vector.at(0).asInt(), vector.at(1).asInt());
	detect->setPlayer(dynamic_cast<Player*>(detect->getCellByName("player_" + vector.at(0).asString() + "_" + vector.at(1).asString())));
	detect->setCurFloor(vector.at(1).asInt(),vector.at(0).asInt(),2.0f);

	detect->runAction(Sequence::create(DelayTime::create(3.0), CallFunc::create([=](){
		auto trialPanel = TrialPanel::create("trial");
		trialPanel->initWithTrial(this);
		PopPanel::getInstance()->addPanel(trialPanel, 1);
	}),nullptr));
}

void Trial::start()
{
	// 成就关卡介绍


	// 点击确定按钮开始计时
	_initTime = getTime();
}

void Trial::end()
{
	// 结算 界面，点击确定按钮回到主传送点，结算获得

}

time_t Trial::getLastTime()
{
	auto curTime = getTime();
	auto durTime = difftime(curTime, _initTime);
	return durTime;
}

ValueMap Trial::getUnlockCondition()
{
	ValueMap condition;
	stringToValueMap(_model["unlockCondition"].asString(), condition);
	return condition;
}

cocos2d::Value Trial::getModelByName(std::string name)
{
	return _model[name];
}

int Trial::getUndoAchieNum()
{
	int i = 0;
	for(auto achieve:_achievements)
	{
		if(achieve->getReached() == false)
			i++;
	}
	return i;
}

Achievement::Achievement(int id)
{
	_id = id;
}

Achievement::Achievement(ValueMap model)
{
	_model =  model;
	_id = _model["id"].asInt();
}

Achievement* Achievement::create(int id)
{
	Achievement *ret = new Achievement(id);	
	if (ret && ret->init())
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}

Achievement* Achievement::create(ValueMap model)
{
	Achievement *ret = new Achievement(model);
	if (ret && ret->init())
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}

bool Achievement::init()
{
	initModel();
	initOthers();
	return true;
}

cocos2d::Value Achievement::getModelByName(std::string name)
{
	return _model[name];
}

void Achievement::loadModel(ValueMap &model)
{
	if(!model["reached"].isNull())
	{
		_reached = (bool)model["reached"].asInt();
	}
}

void Achievement::initModel()
{
	int id = _id;
	if(DATA_MODE == 1)
	{
		rapidjson::Document &datas = Model::shareModel()->achievementDatas;
		auto &data = Model::GetDataFromCol1(datas,id);
		Model::JsonAddModel(data,_model);
	}
	else if(DATA_MODE == 2)
	{
		auto model = Model::shareModel()->achievementCsv->getModelById(id);
		Model::modelAddModel(model,_model);
	}
}

void Achievement::initOthers()
{
	auto conditionsStr = _model["condition"].asString();
	stringToValueMap(conditionsStr, _conditions);

	auto rewardsStr = _model["reward"].asString();
	stringToValueMap(rewardsStr, _rewards);

	_reached = false;
	if(!_model["reached"].isNull())
	{
		_reached = (bool)_model["reached"].asInt();
	}
}

bool Achievement::checkNumByStr(float num1, std::string data)
{
	const char* signs[5] = {"<",">","<=",">=","="};
	std::string sign;
	float num;
	for(int i = 0; i < 5; i++)
	{
		auto iter = data.find(signs[i]);
		auto iterEnd = data.rfind(sign[i]);
		if(iter != data.npos)
		{
			sign = data.substr(iter, std::string(signs[i]).size());
			num = cocos2d::Value(data.substr(iterEnd, data.size())).asFloat(); 
		}
	}
	bool fit = false;
	if((sign == "<" && num1 < num)
		||(sign == ">" && num1 > num)
		||(sign == "<=" && num1 <= num)
		||(sign == ">=" && num1 >= num)
		||(sign == "=" && num1 == num))
		fit = true;
	return fit;
}

bool Achievement::checkConditions()
{
	for(auto pair:_conditions)
	{
		auto player = Detect::shareDetect()->getPlayer();
		auto type = pair.first;
		auto data = pair.second.asString();
		bool fit = true;
		if(type == "time")
			fit = checkNumByStr(_trial->getLastTime(),data);
		//else if(type == "")
			//checkNumByStr(,data);
		if(fit == false)
			return false;
	}
	return true;
}

ValueMap Achievement::saveModel()
{
	ValueMap model;
	model["id"] = _id;
	model["reached"] = (int)_reached;
	return model;
}

bool Achievement::onTrigger(EventCustom *event)
{
	return false;
}

const std::string Achievement::getNameId()
{
	return "0";
}

std::string Achievement::getType()
{
	return "Achievement";
}