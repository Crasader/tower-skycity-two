#include "JoyStickPanel.h"

#define NONE_IMAGE "none.png"

JoyStickPanel* JoyStickPanel::create(std::string name)
{
	JoyStickPanel *pRet = new JoyStickPanel(name);
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool JoyStickPanel::init()
{
	_up = static_cast<Button*>(Helper::seekWidgetByName(_root, "Button_up"));
	_right = static_cast<Button*>(Helper::seekWidgetByName(_root, "Button_right"));
	_left = static_cast<Button*>(Helper::seekWidgetByName(_root, "Button_left"));
	_down = static_cast<Button*>(Helper::seekWidgetByName(_root, "Button_down"));
	_hit = static_cast<Button*>(Helper::seekWidgetByName(_root, "Button_hit"));
	_bg = static_cast<Button*>(Helper::seekWidgetByName(_root, "Button_bg"));

	_root->setTouchEnabled(false);
	_up->setTouchEnabled(false);
	_right->setTouchEnabled(false);
	_left->setTouchEnabled(false);
	_down->setTouchEnabled(false);
	_buttons->setTouchEnabled(false);
	_bg->setTouchEnabled(false);
	_hit->setTouchEnabled(false);

	/*for(auto node:_buttons->getChildren())
	{
	auto button = static_cast<Button*>(node);
	button->addTouchEventListener(_CALLBACK_2(JoyStickPanel::onButtonClicked, this));
	}*/

	_chassis = static_cast<ImageView*>(Helper::seekWidgetByName(_root, "Image_bg"));
	//_chassis->setOpacity(100);
	_touchDot = Sprite::createWithSpriteFrameName(NONE_IMAGE);
	this->addChild(_touchDot,2);

	isDieRadius = false;
	isAutoPosition = false;
	isMoveOut = false;
	_downLast = false;
	_canDrug = false;
	_direction = DEFAULT;

	onHit = nullptr;
	onDirection = nullptr;

	return true;
}

void JoyStickPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
	}
}

void JoyStickPanel::onRun()
{
	listener = EventListenerTouchOneByOne::create();
	CC_SAFE_RETAIN(listener);
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(JoyStickPanel::onTouchBegan,this);
	listener->onTouchMoved = CC_CALLBACK_2(JoyStickPanel::onTouchMoved,this);
	listener->onTouchEnded = CC_CALLBACK_2(JoyStickPanel::onTouchEnded,this);
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener,_buttons);

	listener1 = EventListenerTouchOneByOne::create();
	CC_SAFE_RETAIN(listener1);
	listener1->setSwallowTouches(true);
	listener1->onTouchBegan = CC_CALLBACK_2(JoyStickPanel::onHitTouchBegan,this);
	listener1->onTouchMoved = CC_CALLBACK_2(JoyStickPanel::onHitTouchMoved,this);
	listener1->onTouchEnded = CC_CALLBACK_2(JoyStickPanel::onHitTouchEnded,this);
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener1,_hit);
}

void JoyStickPanel::onDisable()
{
	this->_eventDispatcher->removeEventListener(listener);
	CC_SAFE_RELEASE_NULL(listener);
	this->_eventDispatcher->removeEventListener(listener1);
	CC_SAFE_RELEASE_NULL(listener1);
	//isDieRadius = false;
	isAutoPosition = false;
	isMoveOut = false;
}

bool JoyStickPanel::onHitTouchBegan(Touch* touch,Event* event)
{
	Vec2 locationInNode = _hit->convertToNodeSpace(touch->getLocation());
	Size s = _hit->getContentSize();
	Rect rect = Rect(0, 0, s.width, s.height);

	if (rect.containsPoint(locationInNode))
	{
		_hit->setOpacity(255);

		_downLast = true;
		this->stopAllActions();
		this->runAction(Sequence::create(DelayTime::create(1.0f), CallFunc::create([=]()
		{
			if(_downLast == true)
			{
				_canDrug = true;
				// ÍÏ¶¯¶¯»­
				auto scaleBy = ScaleBy::create(0.08,1.1,1.1);
				this->runAction(Sequence::create(scaleBy,scaleBy->reverse(),nullptr));
			}
		}),nullptr));
		return true;
	}
	return false;
}

void JoyStickPanel::onHitTouchMoved(Touch* touch,Event* event)
{
	if(_canDrug == true)
	{
		this->setPosition(getPosition() + touch->getDelta());
	}

	Vec2 locationInNode = _hit->convertToNodeSpace(touch->getLocation());
	Size s = _hit->getContentSize();
	Rect rect = Rect(0, 0, s.width, s.height);

	if (!rect.containsPoint(locationInNode))
	{
		_downLast = false;
	}
}

void JoyStickPanel::onHitTouchEnded(Touch* touch,Event* event)
{
	_hit->setOpacity(100);

	Vec2 locationInNode = _hit->convertToNodeSpace(touch->getLocation());
	Size s = _hit->getContentSize();
	Rect rect = Rect(0, 0, s.width, s.height);

	if (rect.containsPoint(locationInNode))
	{
		if(onHit && _canDrug != true)
		{
			onHit(_hit,Widget::TouchEventType::ENDED);
		}
	}
	_downLast = false;
	_canDrug = false;
}

bool JoyStickPanel::onTouchBegan(Touch* touch,Event* event)
{
	Vec2 locationInNode = this->convertToNodeSpace(touch->getLocation());
	if( isAutoPosition )
	{
		this->setPosition(touch->getLocation());
		return true;
	}
	if( isAutoPosition==false && isDieRadius )
	{
		if( locationInNode.getLength() > _radius )
		{
			return false;
		}
	}
	_touchDot->setPosition(locationInNode);
	if( locationInNode.getLength() > _failradius )
	{
		changeAngle(locationInNode);
	}
	return true;
}
void JoyStickPanel::onTouchMoved(Touch* touch,Event* event)
{
	Vec2 locationInNode = this->convertToNodeSpace(touch->getLocation());
	if( isDieRadius )
	{
		if( locationInNode.getLength() < _radius )
		{
			if( isMoveOut )
			{
				_touchDot->setPosition(locationInNode);
				isMoveOut = false;
			}
			setTouchDotPosition(locationInNode,_touchDot->getPosition() + touch->getDelta());
			return;
		}
	}
	else
	{
		setTouchDotPosition(locationInNode,_touchDot->getPosition() + touch->getDelta());
		return;
	}

	isMoveOut = true;
	_touchDot->setPosition(0,0);
	resetState();
}
void JoyStickPanel::onTouchEnded(Touch* touch,Event* event)
{
	_touchDot->setPosition(0,0);
	isMoveOut = false;
	resetState();
}

void JoyStickPanel::setTouchDotPosition(Vec2 vec1,Vec2 vec2)
{
	_touchDot->setPosition(vec2);
	if( _failradius>0 )
	{
		if( vec1.getLength() < _failradius )
		{
			resetState();
			return;
		}
	}
	changeAngle(vec1);
}

void JoyStickPanel::setDieRadius(float radius)
{
	_radius = radius;
	isDieRadius = true;
}

void JoyStickPanel::setAutoPosition(bool value)
{
	isAutoPosition = value;
}

void JoyStickPanel::setFailRadius(float radius)
{
	_failradius = radius;
}


void JoyStickPanel::changeAngle( Vec2 position )
{
	_right->setBrightStyle(Widget::BrightStyle::NORMAL);
	_up->setBrightStyle(Widget::BrightStyle::NORMAL);
	_down->setBrightStyle(Widget::BrightStyle::NORMAL);
	_left->setBrightStyle(Widget::BrightStyle::NORMAL);

	_bg->setBrightStyle(Widget::BrightStyle::HIGHLIGHT);

	auto angle = CC_RADIANS_TO_DEGREES(position.getAngle());
	if(angle > -45 && angle < 45)
	{
		_direction=D_RIGHT;
		_right->setBrightStyle(Widget::BrightStyle::HIGHLIGHT);
	}
	/*else if(angle > 22.5 && angle < 67.5)
	{
	_direction=D_RIGHT_UP;
	}*/
	else if(angle > 45 && angle < 135)
	{
		_direction=D_UP;
		_up->setBrightStyle(Widget::BrightStyle::HIGHLIGHT);
	}
	/*else if(angle > 112.5 && angle < 157.5)
	{
	_direction=D_LEFT_UP;
	}*/
	else if((angle > 135 && angle < 180)||(angle < -135 && angle > -180))
	{
		_direction=D_LEFT;
		_left->setBrightStyle(Widget::BrightStyle::HIGHLIGHT);
	}
	/*else if(angle < -112.5 && angle > -157.5)
	{
	_direction=D_LEFT_DOWN;
	}*/
	else if(angle < -45 && angle > -135)
	{
		_direction=D_DOWN;
		_down->setBrightStyle(Widget::BrightStyle::HIGHLIGHT);
	}
	/*else if(angle < -22.5 && angle > -67.5)
	{
	_direction=D_RIGHT_DOWN;
	}*/
	callDirectionFun();
}

void JoyStickPanel::callDirectionFun()
{
	if( onDirection )
	{
		onDirection(_direction);
	}
}

void JoyStickPanel::resetState()
{
	if(_direction != DEFAULT)
	{
		_right->setBrightStyle(Widget::BrightStyle::NORMAL);
		_up->setBrightStyle(Widget::BrightStyle::NORMAL);
		_down->setBrightStyle(Widget::BrightStyle::NORMAL);
		_left->setBrightStyle(Widget::BrightStyle::NORMAL);
		_bg->setBrightStyle(Widget::BrightStyle::NORMAL);
		_direction = DEFAULT;
		callDirectionFun();
	}
}

void JoyStickPanel::setVisibleJoystick(bool visible)
{
	_chassis->setVisible(visible);
	_touchDot->setVisible(visible);
}

void JoyStickPanel::setVisibleChassis(bool visible)
{
	_chassis->setVisible(visible);
}