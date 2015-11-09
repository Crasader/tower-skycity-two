#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "cocos2d.h"
#include "TObject.h"
USING_NS_CC;

class Trial;

class Achievement : public Ref, public TObject
{
public:
	Achievement(int id);
	Achievement(ValueMap model);
	static Achievement* create(int id);
	static Achievement* create(ValueMap model);

	bool checkConditions();
	virtual bool init();
	virtual void loadModel(ValueMap &map);
	virtual void initModel();
	virtual void initOthers();
	virtual ValueMap saveModel();

	cocos2d::Value getModelByName(std::string name);
	virtual bool onTrigger(EventCustom *event);
	const std::string getNameId();
	std::string getType();
	int getId(){return _id;}
	CC_SYNTHESIZE(bool, _reached, Reached);
	CC_SYNTHESIZE(Trial*, _trial, Trial);
private:
	bool checkNumByStr(float num1, std::string data);

	int _id;
	ValueMap _model;
	ValueMap _conditions;
	ValueMap _rewards;
};

class Trial : public Ref, public TObject
{
public:
	Trial(int id);
	Trial(ValueMap model);
	static Trial* create(int id);
	static Trial* create(ValueMap model);
	virtual bool init();
	virtual void loadModel(ValueMap &map);
	virtual void initModel();
	virtual void initOthers();
	virtual ValueMap saveModel();

	virtual bool onTrigger(EventCustom *event);
	const std::string getNameId();
	std::string getType();

	int getId(){return _id;}
	CC_SYNTHESIZE(bool, _unLock, UnLock);
	ValueMap getUnlockCondition();

	cocos2d::Value getModelByName(std::string name);
	int getUndoAchieNum();
	Vector<Achievement*> getAchievements(){return _achievements;}

	time_t getInitTime(){return _initTime;}
	time_t getLastTime();

	void ready();
	void start();
	void end();
private:
	

	int _id;
	time_t _initTime;
	ValueMap _model;
	Vector<Achievement*> _achievements;
};


#endif //__LEVEL_H__