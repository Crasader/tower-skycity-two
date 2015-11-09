#ifndef __PPJOY_PANEL_H__
#define __PPJOY_PANEL_H__

#include "cocos2d.h"
#include "UI/SimplePanel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;

class DragNode;
class JoyStickPanel;

class PpJoyPanel : public SimplePanel
{
public:
	enum buttons
	{
		HIT = 1,
		JUMP,
		OK
	};
	~PpJoyPanel();
	PpJoyPanel(std::string name):SimplePanel(name){}
	static PpJoyPanel* create(std::string name);
	bool init();
	/** 回调函数指针 */
	std::function<void(cocos2d::Ref *ref, int touchType)> onHit;
	std::function<void(cocos2d::Ref *ref, int touchType)> onJump;
	/** 回调函数指针 */
	std::function<void(int)> onDirection;

	void adjustPosition(bool bol);

	void setEnabled(bool bol);
	void setJoyStickEnabled(bool bol);
	void setHitEnabled(bool bol);
	void setJumpEnabled(bool bol);
	bool getEnabled(){return _enabled;}
	bool getJoyStickEnabled(){return _joyStickEnabled;}
	bool getHitEnabled(){return _hitEnabled;}
	bool getJumpEnabled(){return _jumpEnabled;}
protected:
	
private:
	bool _joyStickEnabled;
	bool _hitEnabled;
	bool _jumpEnabled;
	bool _enabled;
	Button* _hitButton;
	Button* _jumpButton;
	Button* _okButton;
	DragNode *_hit;
	DragNode *_jump;
	DragNode *_joyStick;
	JoyStickPanel* _joyStickPanel;
	Layout* _confirmLayOut;
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
};


#endif /*__PPJOY_PANEL_H__*/