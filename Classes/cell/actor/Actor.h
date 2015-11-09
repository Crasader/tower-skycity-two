#ifndef __ACTOR_H__
#define __ACTOR_H__

#include "cell/Cell.h"

class ItemBag;
class SparDisk;
class DiaryPanel;
class Equip;
class Prop;
class Special;
class Spar;
USING_NS_CC;

// 角色
class Actor : public Cell
{
public:
	Actor(int typeId):Cell(typeId){}
	Actor(ValueMap& model):Cell(model){}
	~Actor();
	static Actor* create(int typeId);
	static Actor* create(ValueMap& model);
	virtual std::string getType(){return "Actor";}
	// override Sprite
	void onEnter();
	// override TObject
	virtual void setSite(const Vec2 &vec);
	virtual void initNull();
	virtual void initModel();
	virtual void initOthers();
	virtual ValueMap saveModel();
	virtual bool onTrigger(EventCustom *event);
	virtual int isBlock();
	virtual void jumpTo(Vec2 site);
	CC_SYNTHESIZE(Cell*, _grabCell, GrabCell);
	virtual void grabCell(Cell* cell, Direction dir);
// #########################################   背包   ###########################################
public:
	virtual void addEquip(Equip *equip);
	virtual void addEquipByType(int typeId);

	virtual void addProp(Prop *prop);
	virtual void addPropByType(int typeId, int num = 1);

	virtual void addSpecial(Special *special);
	virtual void addSpecialByType(int typeId, int num = 1);

	virtual void removeProp(Prop *prop);
	virtual void removePropByType(int typeId, int num = 1);
	virtual void removeSpecialByType(int typeId, int num = 1);

	virtual void getGemByType(int typeId, int num = 1);
	virtual void usePropByType(int typeId, int num = 1);

	virtual void addSpar(Spar *spar);
	virtual void addSparByType(int typeId);
protected:
	ItemBag *_itemBag;
	SparDisk *_sparDisk;

	Sprite* _shadow;
// #########################################   背包   ###########################################


protected:
	// 属性
	std::string _nickName;
	std::string _type;
	
	int _level;
	int _hp;
	int _str;
	int _def;
	int _xp;
	int _gold;
	int _mdef;
	int _fw;
	int _maxHp;
	int _nextXp;
	int _lastXp;
public:
	int getHp();
	int getStr();
	int getDef();
	int getMdef();
	int getXp();
	int getLevel(){return _level;}
	int getGold(){return _gold;}

	void setNickName(std::string nickName);
	virtual void setHp(int hp);
	virtual void setStr(int str);
	virtual void setDef(int def);
	virtual void setMdef(int mdef);
	virtual void setXp(int xp);
	virtual void setLevel(int level);
	virtual void setGold(int gold);
	virtual void setSpeed(float speed);

	virtual void addHp(int hp);
	virtual void addStr(int str);
	virtual void addDef(int def);
	virtual void addMdef(int mdef);
	virtual void addXp(int xp);
	virtual void addLevel(int level);
	virtual void addGold(int gold);
	virtual void addSpeed(float speed);

	double getDurTime();
	void setDurTime(double time){_durTime = time;}

	CC_SYNTHESIZE(bool, _isDead, IsDead);
	CC_SYNTHESIZE(int, _chatId, ChatId);
	void setSwapCount(int n){_swapCount = n;}
	int getSwapCount(){return _swapCount;}

	virtual void refreshModel();
protected:
	// 碰撞函数
	// 单元格区域
	virtual void crashOn(Cell* cell);
	virtual void crashOut(Cell* cell);
	// 正好踩到单元格中点
	virtual void stepOn(Cell* cell);
	// 触碰单元格
	virtual void touchOn(Cell* cell);
	virtual void touchOut(Cell* cell);

	// 进入视野
	virtual void inVision(Cell* cell);
	virtual void outVision(Cell* cell);

	virtual void initView();
	
	virtual void dead();
	virtual void setPosition(const Vec2& pos) override;
protected:
	DiaryPanel *_diary;
	int _swapCount;
	std::string _rival;
	long _initTime;
	double _durTime;

	Action* _dargAction1;
	Action* _dargAction2;
private:
	void setFightDelay(float delay){_fightDelay = delay;}
	float getFightDelay();

	CC_SYNTHESIZE(int, _useHoeNum, UseHoeNum);
	CC_SYNTHESIZE(int, _useBombNum, UseBombNum);
#if DETECT_ACTOR == 1
public:
	void setDamageLabel(std::string damage, std::string win);
	void setNumLabel(std::string num);
	void setNum2Label(std::string num);
	void setNum3Label(std::string num);
protected:
	Label *_damageLabel;
	Label *_numLabel;
	Label *_num2Label;
	Label *_num3Label;
#endif
};

// 怪物
class Monster : public Actor
{
public:
	Monster(int typeId):Actor(typeId){}
	Monster(ValueMap& model):Actor(model){}
	static Monster* create(int id);
	static Monster* create(ValueMap& model);
	virtual ValueMap saveModel();
	virtual void initNull();
	virtual void initModel();
	virtual void initOthers();
	virtual std::string getType(){return "Monster";}
public:
	bool hasAttrs(std::string type);
	void removeAttrs(std::string type);
	void addAttrs(std::string type);
	std::vector<std::string> getAttrs();
	CC_SYNTHESIZE(bool, _jiaji, Jiaji);

	cocos2d::Value getAttrData(std::string name);
protected:
	// 怪物属性 xixue jiangu xiangong mogong lianji jinghua jiaji zuji 
	// 吸血、坚固、先攻、魔攻、连击、净化、夹击、阻击
	std::vector<std::string> _attrs;
	ValueMap _attrsData;
	// 进入视野
	virtual void inVision(Cell* cell);
	virtual void outVision(Cell* cell);
protected:
	//void onEnter();
	//virtual bool onTrigger(EventCustom *event);
	//virtual void dead();
	//// 碰撞函数
	//// 单元格区域
	//virtual void crashOn(Cell* cell);
	//virtual void crashOut(Cell* cell);
	//// 正好踩到单元格中点
	//virtual void stepOn(Cell* cell);
	//// 触碰单元格
	//virtual void touchOn(Cell* cell);
};

// 玩家
class Player : public Actor
{
public:
	Player(int typeId):Actor(typeId){}
	Player(ValueMap& model):Actor(model){}
	static Player* create(int id);
	static Player* create(ValueMap& model);
	std::string getType(){return "Player";}
	void onEnter();
	void update(float delta);
	virtual void initNull();
	virtual void initModel();
	virtual ValueMap saveModel();
	virtual void initOthers();
	virtual bool onTrigger(EventCustom *event);
	ItemBag* getItemBag();
	SparDisk* getSparDisk();
	DiaryPanel* getDiary();
	virtual void addEquipByType(int typeId);
	virtual void addPropByType(int typeId, int num = 1);
	virtual void addSpecialByType(int typeId, int num);
	virtual void addSpecial(Special *special);
	virtual void addEquip(Equip *equip);
	virtual void addSpar(Spar *spar);
	virtual void addProp(Prop *prop);
	virtual void removeProp(Prop *prop);
	virtual void removePropByType(int typeId, int num = 1);

	virtual void addSparByType(int typeId);

	virtual void refresh();
	
	virtual void updateToHud();
	//~Player();

	virtual void addHp(int hp);
	virtual void addStr(int str);
	virtual void addDef(int def);
	virtual void addMdef(int mdef);
	virtual void addXp(int xp);
	virtual void addGold(int gold);
	virtual void addLevel(int level);

	virtual void setHp(int hp);
	virtual void setStr(int str);
	virtual void setDef(int def);
	virtual void setMdef(int mdef);
	virtual void setXp(int xp);
	virtual void setGold(int gold);
	virtual void setLevel(int level);

	
protected:
	virtual void convey(Vec2 site, int layerId, int floorId, int towerId);
	virtual void dead();

	// 进入视野
	virtual void inVision(Cell* cell);
	virtual void outVision(Cell* cell);
private:
};


#endif /*__ACTOR_H__*/