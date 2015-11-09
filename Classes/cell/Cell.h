#ifndef __CELL_H__
#define __CELL_H__

#include "cocos2d.h"
#include "TObject.h"
#include "Global.h"

USING_NS_CC;
class Floor;
class Unit;
class AStar;
class Battery;

class Shader:public Ref
{
public:
	static Shader* create(std::string pshade, std::string pmask);
	Shader(std::string pshade, std::string pmask);
	~Shader();
	bool init();
	Sprite* shade;
	Sprite* mask;
	void update(Vec2 pos);
};

class Vision;

class Cell : public Layer, public TObject
{
public:
	// �˶�״̬
	enum Movement
	{
		STOP,LEFT,UP,RIGHT,DOWN
	};
	// �����˶�
	static Movement reverse(Movement dir){if(dir==Movement::UP){return Movement::DOWN;}
	else if(dir==Movement::DOWN){return Movement::UP;}
	else if(dir==Movement::LEFT){return Movement::RIGHT;}
	else if(dir==Movement::RIGHT){return Movement::LEFT;}
	else{return Movement::STOP;}
	}
	// ����
	typedef Movement Direction;
	static std::string MovementStr[5];

	enum AnimationType
	{
		IDLE,WALK,HIT
	};
	static std::string AnimationTypeStr[3];

	

//----------------------------------------------  ���  ---------------------------------------------
public:
	Sprite* getView(){return _view;}
protected:
	Sprite* _view;

//----------------------------------------------  �����  ---------------------------------------------
public:
	virtual void loadModel(ValueMap &map);
	virtual void refresh();
	virtual void refreshModel();
	virtual ValueMap saveModel();
protected:
	bool _refresh;

//----------------------------------------------  λ��  ---------------------------------------------
	CC_SYNTHESIZE(Unit*, _unit, Unit);
	CC_SYNTHESIZE(Floor*, _floor, Floor);
public:
	virtual void detachFromUnit();
	virtual void removeFromFloor();

	virtual Vec2 getSite() const;
	virtual void setSite(const Vec2 &vec);
	Vec2 getFaceSite(){return _faceSite;}
	virtual int getLayer();
	virtual void setLayer(int id);
	virtual void moveToNewSite();
	virtual void stepToNewSite();
	virtual void stepOutTheSite();
protected:
	Vec2 _site;
	Vec2 _lastSite;
	Vec2 _faceSite;
	int _layer;
private:
	Vec2 _nextSite;
	Vec2 _centerSite;



//----------------------------------------------  ����  ---------------------------------------------
public:
	virtual void animate(AnimationType type, Direction dir);
	virtual bool setAnimation(const std::string &str, bool loop = false, CallFuncN* callBack = nullptr, bool again = false, float duration = -1.0);
protected:
	AnimationType _ani;
	ValueMap _animations;
	Action *_curAnimate;
	Animation *_curAnimation;
	
//----------------------------------------------  ����  ---------------------------------------------
public:
	void setConveyCount(int n){_conveyCount = n;}
protected:
	int _conveyCount;

//----------------------------------------------  ����  ---------------------------------------------
protected:
	Battery* _battery;
public:
	void fire();
	void stopFire();
	void setBattery(Battery* battery);
	Battery* getBattery(){return _battery;}
//----------------------------------------------  �˶�  ---------------------------------------------
public:
	void setGuide(Direction dir,int value);
	void resetGuides();
	void moveToSite(Vec2 site);
	void setCanGuide(bool b, float delay = 0);

	virtual void convey(Vec2 site, int layerId, int floorId, int towerId);

	// ��·����
	virtual void runPath(std::string name);
	// ����
	virtual void jumpTo(Vec2 site);

	// ����
	virtual void fall();

	bool getCanGuide(){return _canGuide;}
	void setCanWalk(bool b){_canWalk = b;}
	bool getCanWalk(){return _canWalk;}
	CC_SYNTHESIZE(bool, _canTouch, CanTouch);
	CC_SYNTHESIZE(bool, _actionMove, ActionMove);
	float getSpeed();
	Direction getFace();
protected:
	virtual bool nextTileCanMove(Direction dir);
private:
	Point getNearSite(Point core);
	void guidesMakeTrend();
	void walk();
	void switchMovement(Movement);
	bool checkCenterTile();
	Point pathMakeTrend();
	void pathToSite(Vec2 site);	
protected:
	Movement _move;
	Direction _face;
	float _speed;
	float _saveSpeed;
	Vec2 _dis;
	PointArray *_paths;
	int _guides[5];
	float _fightDelay;
	Movement _trend;

	bool _canWalk;
	bool _canGuide;
	AStar *_astar;

	Action* _jumpAction;
	Action* _viewJumpAction;
	Action* _shadowJumpSmallAction;
//----------------------------------------------  ��ײ����  ---------------------------------------------
public:
	// ��Ԫ������
	virtual void crashOn(Cell* cell);
	virtual void crashOut(Cell* cell);
	// ���òȵ���Ԫ���е�
	virtual void stepOn(Cell* cell);
	// һ����ɾ��
	virtual void stepOut(Cell* cell);
	// ������Ԫ��
	virtual void touchOn(Cell* cell);
	virtual void touchOut(Cell* cell);

	virtual int isBlock();
	virtual void setBlockFunc(std::function<int(Vec2&)> func);
	std::function<int(Vec2&)> getBlockFunc();
protected:
	std::function<int(Vec2&)> _blockFunc;
	

//----------------------------------------------  ��ʼ��  ---------------------------------------------
protected:
	bool init();
	virtual void initNull();
	virtual void initView();
	virtual void initModel();
	virtual void initOthers();
//----------------------------------------------  ����  ---------------------------------------------
public:
	~Cell();
	Cell(int typeId);
	Cell(ValueMap& model);
	std::string getType();
	virtual int getTypeId();
	virtual std::string getImageUrl();
	virtual std::string getNickName();
	virtual std::string getEffect();

	virtual Shader* getShader(){return _shader;}
	const cocos2d::Value getModelByName(std::string name);
	void setModelByName(std::string name, cocos2d::Value value);

	CC_SYNTHESIZE(bool, _busy, Busy);
	CC_SYNTHESIZE(int, _mutual, Mutual);
	void disappear();

public:
	virtual void inVision(Cell* cell);
	virtual void outVision(Cell* cell);

	void addVisions();
	void removeVisions();
protected:
	Vector<Vision*> _visions;
	virtual void setPosition(const Vec2 &vec);
protected:
	virtual bool onTrigger(EventCustom *event);
	void onEnter();
	void onExit();
	virtual void destroy();
	void update(float delta);
	const std::string getNameId();
	
	ValueMap _effect;
	ValueMap _triggerOn;
	Shader* _shader;

	std::string _nameId;
	int _typeId;
#if DETECT_ACTOR == 1
public:
	void setLabel(std::string str1, std::string str2);
protected:
	Label *_label1;
	Label *_label2;
#endif
	
};

#endif /*__CELL_H__*/