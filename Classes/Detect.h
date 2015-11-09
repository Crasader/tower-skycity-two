#ifndef __DETECT_H__
#define __DETECT_H__

#include "cocos2d.h"
#include "TObject.h"

class Cell;
class Player;
class Tower;
class Floor;
class World;
class Trial;
class PpJoyPanel;
class Shake;
class User;

USING_NS_CC;

class Detect :public Layer, public TObject
{
public:
	Detect();
	~Detect();
	static Detect* shareDetect();
	void reset();
	void onEnter();

	// 1 对话条件 2 弹出窗口 3 剧情条件 4 上下楼梯
	void setControlled(int tag,bool b);

	bool attachCell(Cell* cell);
	bool detachCell(Cell* cell);

	// -1当前楼层
	void addCell(Cell* cell, Point site, int layerId = 0, int foorId = 1, int towerId = 1);
	void removeCell(Cell* cell);
	void removeAllCell();
	void addCellByType(int typeId, Point site, int layerId = 0, int foorId = -1, int towerId = 1);
	Cell* getCellByName(std::string name);


	Floor* getFloor(int id, int towerId = -1);
	Tower* getTower(int id);
	CC_SYNTHESIZE(World*, _world,World);

	void resetCurFloor();
	Floor* getCurFloor();
	void setCurFloor(int floorId, int towerId = -1, float duration = 0.3);
	void resetCurTower();
	Tower* getCurTower();
	void setCurTower(int towerId);
	// 当前关卡
	void resetCurTrial();
	Trial* getCurTrial();
	void setCurTrial(int trial);

	Player* getPlayer();
	void setPlayer(Player* player);

	void initModel();
	void loadModel(ValueMap &map);
	ValueMap saveModel();
	bool onTrigger(EventCustom *event);
	const std::string getNameId();
	std::string getType();

// ###################################################   虚拟键   ###################################################
public:
	void setPpJoy(PpJoyPanel* ppJoy);
	PpJoyPanel* getPpJoy();
private:
	PpJoyPanel *_ppJoy;
	// 虚拟回调
	void onDirection(int enums);
	void onHit(cocos2d::Ref *ref, int touchType);
	void onJump(cocos2d::Ref *ref, int/*ui::Widget::TouchEventType*/ touchType);
// ###################################################   虚拟键   ###################################################
public:
	void addCellToWaitingRoom(Cell* cell);
	void removeCellFromWaitingRoom(Cell* cell);
	Cell* getCellFromWaitingRoomByName(std::string name);
	
	void convey(Cell* cell, Vec2 site, int layerId, int floorId, int towerId);

	bool load;
	std::string loadPath;
	bool lookActorOpen;

	int viewHide;

	void refreshCells();
	void refreshPlayer();

	/*void playBgMsc(std::string name, bool loop = false);
	void stopBgMsc(bool bRelease = false);
	void playEffectMsc(std::string name);
	void stopEffectMsc(std::string name);*/

	std::string getMscPathByName(std::string name);

	bool getSoundOn(){return _soundOn;}
	bool getEffectOn(){return _effectOn;}

	void setSoundOn(bool value);
	void setEffectOn(bool value);

	CC_SYNTHESIZE(User*, _user, User);

	CC_SYNTHESIZE(bool, _touched, Touched);
private:
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
	bool onTouchBegan(Touch *touch, Event *unused_event);
	void onTouchEnded(Touch *touch, Event *unused_event);
private:
	bool _soundOn;
	bool _effectOn;

	std::string _curBGmusic;

	
	Player* _player;
	Floor* _curFloor;
	Tower* _curTower;

	Trial* _curTrial;

	bool _controlled;
	
	ValueMap _controlCondition;

	Map<std::string, Cell*> _cells;
	Map<std::string, Cell*> _waitingRoom;

	LayerColor* _worldDark;

private:
	static Detect* Detect::_detect;

	// 单件模式的清除工作
	class Garbo  
	{  
	private:  
		~Garbo()  
		{  
			if (_detect)  
			{
				_detect->reset();
				_detect->release();  
				_detect = NULL;
				log("destroy detect!");  
			}  
		}  
	private:  
		static Garbo m_Garbo;
	};
};

#endif /*__DETECT_H__*/
