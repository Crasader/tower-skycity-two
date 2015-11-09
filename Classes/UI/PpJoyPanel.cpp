#include "PpJoyPanel.h"
#include "Detect.h"
#include "utils/DragNode.h"
#include "ui/JoyStickPanel.h"

#define NONE_IMAGE "none.png"

PpJoyPanel* PpJoyPanel::create(std::string name)
{
	PpJoyPanel *pRet = new PpJoyPanel(name);
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

PpJoyPanel::~PpJoyPanel()
{
}

bool PpJoyPanel::init()
{
	onHit = nullptr;
	onJump = nullptr;
	_hit = nullptr;
	_jump = nullptr;
	_joyStick = nullptr;
	_enabled = true;

	_hitButton = static_cast<Button*>(Helper::seekWidgetByName(_root, "Button_hit"));
	_jumpButton = static_cast<Button*>(Helper::seekWidgetByName(_root, "Button_jump"));
	
	_hitButton->addTouchEventListener(CC_CALLBACK_2(PpJoyPanel::onButtonClicked, this));
	_jumpButton->addTouchEventListener(CC_CALLBACK_2(PpJoyPanel::onButtonClicked, this));
	// ÐéÄâ°´Å¥
	auto joyStickLayOut = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_joyStickLayOut"));
	_joyStickPanel = JoyStickPanel::create("joyStick");
	joyStickLayOut->addChild(_joyStickPanel);

	_joyStickPanel->setDieRadius(100);//ÉèÖÃËÀÍö°ë¾¶£¨ÍâÈ¦£©
	_joyStickPanel->setFailRadius(10);//ÉèÖÃÊ§Ð§°ë¾¶£¨ƒÈÈ¦£©
	//_joyStickPanel->setAutoPosition(true);

	_confirmLayOut = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_confirm"));
	_okButton = static_cast<Button*>(Helper::seekWidgetByName(_confirmLayOut, "Button_ok"));
	_okButton->addTouchEventListener(CC_CALLBACK_2(PpJoyPanel::onButtonClicked, this));

	// Ä¬ÈÏµ÷ÕûºÃÎ»ÖÃ
	adjustPosition(false);
	return true;
}

void PpJoyPanel::adjustPosition(bool bol)
{
	if(bol == true)
	{
		_root->setTouchEnabled(true);

		_confirmLayOut->setVisible(true);
		_confirmLayOut->setEnabled(true);

		_hitButton->setTouchEnabled(false);
		_jumpButton->setTouchEnabled(false);
		_joyStickPanel->onDisable();
		_hit = DragNode::create(_hitButton, _hitButton);
		_jump = DragNode::create(_jumpButton, _jumpButton);
		auto touchNode = static_cast<Layout*>(Helper::seekWidgetByName(_joyStickPanel->getRoot(), "Image_bg"));
		_joyStick = DragNode::create(touchNode ,_joyStickPanel);
	}
	else
	{
		_root->setTouchEnabled(false);

		_confirmLayOut->setVisible(false);
		_confirmLayOut->setEnabled(false);

		CC_SAFE_RELEASE(_hit);
		CC_SAFE_RELEASE(_jump);
		CC_SAFE_RELEASE(_joyStick);
		_joyStickPanel->onRun();
		_hitButton->setTouchEnabled(true);
		_jumpButton->setTouchEnabled(true);
	}
}

void PpJoyPanel::setEnabled(bool bol)
{
	_enabled = bol;
	setJoyStickEnabled(bol);
	setHitEnabled(bol);
	setJumpEnabled(bol);
}

void PpJoyPanel::setHitEnabled(bool bol)
{
	_hitEnabled = bol;
	_hitButton->setVisible(bol);
	_hitButton->setEnabled(bol);
}

void PpJoyPanel::setJumpEnabled(bool bol)
{
	_jumpEnabled = bol;
	_jumpButton->setVisible(bol);
	_jumpButton->setEnabled(bol);
}

void PpJoyPanel::setJoyStickEnabled(bool val)
{
	_joyStickEnabled = val;
	_joyStickPanel->setVisible(val);
	if(val)
		_joyStickPanel->onDirection = onDirection;
	else
		_joyStickPanel->onDirection = nullptr;
}

void PpJoyPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	auto button = static_cast<Button*>(ref);
	auto tag = button->getTag();
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		switch (tag)
		{
		case PpJoyPanel::HIT:
			if(onHit != nullptr)
				onHit(ref,(int)touchType);
			break;
		case PpJoyPanel::JUMP:
			if(onJump != nullptr)
				onJump(ref,(int)touchType);
			break;
		case PpJoyPanel::OK:
			adjustPosition(false);
			break;
		default:
			break;
		}
	}
}