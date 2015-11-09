#ifndef __JOYSTICK_PANEL_H__
#define __JOYSTICK_PANEL_H__

#include "cocos2d.h"
#include "UI/SimplePanel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class JoyStickPanel : public SimplePanel
{
public:
	enum JoystickEnum
	{
		DEFAULT,
		D_UP,
		D_DOWN,
		D_LEFT,
		D_RIGHT,
		D_LEFT_UP,
		D_LEFT_DOWN,
		D_RIGHT_UP,
		D_RIGHT_DOWN
	};

	enum buttons
	{
		LEFT = 1,
		RIGHT,
		UP,
		DOWN,
		HIT,
		BG
	};
	JoyStickPanel(std::string name):SimplePanel(name){}
	static JoyStickPanel* create(std::string name);
	bool init();

	/** �����u���� */
	void onRun();
	/** ������� */
	void onDisable();
	/** ���������뾶���������뾶��ҡ����ʧЧ */
	void setDieRadius(float radius);
	/** ������Ч����뾶���������Ч����ȣ������ã� */
	void setFailRadius(float radius);
	/** �Ƿ���ʾ���̺ʹ��� */
	void setVisibleJoystick(bool visible);
	/** �Ƿ���ʾ���� */
	void setVisibleChassis(bool visible);
	/** �Ƿ����ɱ任ҡ������λ�ã�������Ļ��ÿһ�ΰ������ʱ�����꽫��ҡ���������꣬�ƶ�ʱ�����ı�ҡ�������ֱ꣬���´ΰ������ */
	void setAutoPosition(bool value);
	/** �ص�����ָ�� */
	std::function<void(JoystickEnum)> onDirection;

	std::function<void(cocos2d::Ref *ref, ui::Widget::TouchEventType touchType)> onHit;

	Button* getHitButton(){return _hit;}
protected:
	/** ��Ч����뾶 */
	float _radius;
	/** ʧЧ����뾶 */
	float _failradius;
	/** �Ƿ��Ƴ���Ч���� */
	bool isMoveOut;
	/** �Ƿ������Ч����뾶 */
	bool isDieRadius;
	/** �Ƿ����ɱ任ҡ�������� */
	bool isAutoPosition;
	/** ���� */
	JoystickEnum _direction;
	/** ���� */
	ImageView* _chassis;
	/** ���� */
	Sprite* _touchDot;
	EventListenerTouchOneByOne* listener;
	EventListenerTouchOneByOne* listener1;
	bool onTouchBegan(Touch* touch,Event* event);
	void onTouchMoved(Touch* touch,Event* event);
	void onTouchEnded(Touch* touch,Event* event);

	bool onHitTouchBegan(Touch* touch,Event* event);
	void onHitTouchMoved(Touch* touch,Event* event);
	void onHitTouchEnded(Touch* touch,Event* event);

	void onButtonDown(cocos2d::Ref *ref, ui::Widget::TouchEventType touchType);
	/** 
	1�����ô��㣬���ж��Ƿ�����Ч�����ڣ��������Ч�����ڣ������ã�
	2�����ͽǶȱ仯�����������Ч�����ڣ� */
	void setTouchDotPosition(Vec2 vec1,Vec2 vec2);
	/** 
	1������ҡ�����˷���
	2�����ͽǶȱ仯���ص����ȱ仯���� */
	void changeAngle( Vec2 position );
	/** �ص�ע��ļ������� */
	void callDirectionFun();
	/** ���ã������Ȳ��� DEFAULTʱ�����ã� */
	void resetState();

	
private:
	Button *_up;
	Button *_right;
	Button *_left;
	Button *_down;
	Button *_hit;
	Button *_bg;
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);

	bool _downLast;
	bool _canDrug;
};


#endif /*__JOYSTICK_PANEL_H__*/