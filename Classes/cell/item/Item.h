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

	//// ��Ԫ������
	//virtual void crashOn(Cell* cell);
	//virtual void crashOut(Cell* cell);
	//// ���òȵ���Ԫ���е�
	//virtual void stepOn(Cell* cell);
	// ������Ԫ��
	virtual void touchOn(Cell* cell);
	virtual void touchOut(Cell* cell);
protected:
	int _durable;
	int _isBlock;
};
// װ��
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
// ����Ʒ
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
// �������
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
// ��ʯ
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
// һ����ʹ�õ���
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
// ը��
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

	// ����
	void fireToSite(Vec2 site);
	// ����
	void fireToAngle(float angle);
	void fireToDirection(Direction dir);
protected:
	//�����Ϸ�������һ�������Ƿ����
	virtual void moveToNewSite();
	virtual void crashOn(Cell* cell);
	virtual void destroy();
	void fire();
	Effect* _effect;
};
// ��
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
// ����
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
	// ��ײ����
	// ��Ԫ������
	virtual void crashOn(Cell* cell);
	virtual void crashOut(Cell* cell);
	// ���òȵ���Ԫ���е�
	virtual void stepOn(Cell* cell);
	virtual void stepOut(Cell* cell);
	// ������Ԫ��
	virtual void touchOn(Cell* cell);
private:
	int _on;
};

#endif /*__ITEM_H__*/