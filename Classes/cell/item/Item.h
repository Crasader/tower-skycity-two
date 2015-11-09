#ifndef __ITEM_H__
#define __ITEM_H__

#include "cocos2d.h"
#include "cell/Cell.h"

USING_NS_CC;

class Effect;

class Item : public Cell
{
public:
	Item(int typeId):Cell(typeId){}
	Item(ValueMap& model):Cell(model){}
	//~Item();
	static Item* create(int typeId);
	static Item* create(ValueMap& model);
	int getTypeId1();
	std::string getType();
	virtual ValueMap getEffectMap();
	virtual void addDurable(int num);
	int getDurable();
// override	
	virtual void onEnter();
	virtual int isBlock();
	virtual void loadModel(ValueMap &map);
	virtual ValueMap saveModel();
	//void setAnimation(AnimationType type);
protected:
	virtual void setDurable(int value);
	virtual void initView();
	virtual void refreshModel();
	virtual void initModel();
	virtual void initOthers();
	virtual bool onTrigger(EventCustom *event);

	//// 单元格区域
	//virtual void crashOn(Cell* cell);
	//virtual void crashOut(Cell* cell);
	//// 正好踩到单元格中点
	//virtual void stepOn(Cell* cell);
	// 触碰单元格
	virtual void touchOn(Cell* cell);
	virtual void touchOut(Cell* cell);
protected:
	int _durable;
	int _isBlock;
};
// 装备
class Equip : public Item
{
public:
	Equip(int typeId):Item(typeId){}
	Equip(ValueMap& model):Item(model){}
	static Equip* create(int typeId);
	static Equip* create(ValueMap& model);
	bool onTrigger(EventCustom *event);
	std::string getType();
	virtual void loadModel(ValueMap &map);
	virtual ValueMap saveModel();
protected:
	virtual void crashOn(Cell* cell);
};
// 消耗品
class Prop : public Item
{
public:
	Prop(int typeId):Item(typeId){}
	Prop(ValueMap& model):Item(model){}
	static Prop* create(int typeId);
	static Prop* create(ValueMap& model);
	bool onTrigger(EventCustom *event);
	std::string getType();
protected:
	virtual void crashOn(Cell* cell);
};
// 特殊道具
class Special : public Item
{
public:
	Special(int typeId):Item(typeId){}
	Special(ValueMap& model):Item(model){}
	static Special* create(int typeId);
	static Special* create(ValueMap& model);
	bool onTrigger(EventCustom *event);
	std::string getType();
protected:
	virtual void crashOn(Cell* cell);
};
// 晶石
class Spar : public Item
{
public:
	Spar(int typeId):Item(typeId){}
	Spar(ValueMap& model):Item(model){}
	static Spar* create(int typeId);
	static Spar* create(ValueMap& model);
	bool onTrigger(EventCustom *event);
	std::string getType();
	CC_SYNTHESIZE(float, _rate, Rate);
	CC_SYNTHESIZE(float, _drate, Drate);

	CC_SYNTHESIZE(float, _maxRate, MaxRate);
	CC_SYNTHESIZE(float, _maxDrate, MaxDrate);

	CC_SYNTHESIZE(float, _upRate, UpRate);
	CC_SYNTHESIZE(float, _upDrate, UpDrate);

	CC_SYNTHESIZE(int, _costPatchNum, CostPatchNum);

	CC_SYNTHESIZE(std::string, _upConfirm, UpConfirm);

	virtual void loadModel(ValueMap &map);
	virtual ValueMap saveModel();

	void upLevel();
protected:
	void initOthers();
};
// 一次性使用道具
class Gem : public Item
{
public:
	Gem(int typeId):Item(typeId){}
	Gem(ValueMap& model):Item(model){}
	static Gem* create(int typeId);
	static Gem* create(ValueMap& model);
	bool onTrigger(EventCustom *event);
	std::string getType();
protected:
	virtual void crashOn(Cell* cell);
};
// 炸弹
class Missile : public Item
{
public:
	Missile(int typeId):Item(typeId){}
	Missile(ValueMap& model):Item(model){}
	static Missile* create(int typeId);
	static Missile* create(ValueMap& model);
	bool onTrigger(EventCustom *event);
	void initModel();
	void initOthers();
	std::string getType();
	CC_SYNTHESIZE(int, _damage, Damage);
	CC_SYNTHESIZE(int, _effectId, EffectId);
	CC_SYNTHESIZE(float, _duration, Duration);

	CC_SYNTHESIZE(Cell*, _owner, Owner);

	// 导弹
	void fireToSite(Vec2 site);
	// 方向
	void fireToAngle(float angle);
	void fireToDirection(Direction dir);
protected:
	//检查游戏对象的下一个方块是否可行
	virtual void moveToNewSite();
	virtual void crashOn(Cell* cell);
	virtual void destroy();
	void fire();
	Effect* _effect;
};
// 门
class Door : public Item
{
public:
	Door(int typeId):Item(typeId){}
	Door(ValueMap& model):Item(model){}
	static Door* create(int typeId);
	static Door* create(ValueMap& model);
	bool onTrigger(EventCustom *event);
	void initModel();
	void initOthers();
	void open();
	void close();
	std::string getType();
	~Door();
protected:
	virtual ValueMap saveModel();
private:
	int _isOpen;
	Action* _delayCall;
//protected:
//	virtual void crashOn(Cell* cell);
};
// 机关
class Switch : public Item
{
public:
	Switch(int typeId):Item(typeId){}
	Switch(ValueMap& model):Item(model){}
	static Switch* create(int typeId);
	static Switch* create(ValueMap& model);
	bool onTrigger(EventCustom *event);
	void initOthers();
	ValueMap saveModel();
	void on();
	void off();
	std::string getType();
	int isOn();
protected:
	// 碰撞函数
	// 单元格区域
	virtual void crashOn(Cell* cell);
	virtual void crashOut(Cell* cell);
	// 正好踩到单元格中点
	virtual void stepOn(Cell* cell);
	virtual void stepOut(Cell* cell);
	// 触碰单元格
	virtual void touchOn(Cell* cell);
private:
	int _on;
};

#endif /*__ITEM_H__*/