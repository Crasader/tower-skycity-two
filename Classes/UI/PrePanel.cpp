#include "PrePanel.h"
#include "scene/GameScene.h"
#include "UI/FilePanel.h"
#include "UI/ConfigPanel.h"
#include "utils/StringHandler.h"


#define NONE_IMAGE "none.png"

PrePanel* PrePanel::create(std::string name)
{
	PrePanel *pRet = new PrePanel(name);
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

bool PrePanel::init()
{
	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(PrePanel::onButtonClicked, this));
	}


	_image = Sprite::createWithSpriteFrameName("start/800-480(1).png");
	_image->setAnchorPoint(Vec2(0,0));
	addChild(_image);
	_image->retain();
	/*_text = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_text"));
	_text->setVisible(false);
	_panelContent->setVisible(false);*/

	auto textClone = Text::create("sdfisojoeifjesof","Arial",30);

	CCSize size = CCDirector::sharedDirector()->getVisibleSize();

	//����Ҫ��ʾ������
	_text = Label::create(a2u("text\n���Ƿ�ɽ������\n�װ�\n�ֶ�����������\n���·�Ϊ����\n���������������\n�����Ƿ�Υ��\n"),"Arial",20);
	_text->setPosition(ccp(100, -120));
	_text->setColor(Color3B::BLACK);

	//���Ʋü�����
	DrawNode* shap = DrawNode::create();
	Point point[4] = {ccp(0,0), ccp(200, 0), ccp(200, 200), ccp(0, 200)};
	shap->drawPolygon(point, 4, ccc4f(355, 255, 255, 255), 2, ccc4f(255, 255, 255, 255));
	_cliper = ClippingNode::create();
	_cliper->setStencil(shap);
	//cliper->setAnchorPoint(ccp(.5, .5));
	_cliper->setPosition(ccp(size.width / 2 - 100, size.height / 2 - 270));
	addChild(_cliper);
	//��Ҫ���������ּ��뵽�ü�����
	_cliper->addChild(_text);

	Director::getInstance()->setDepthTest(false);

	_text->runAction(Sequence::create(MoveTo::create(1, Vec2(_text->getPositionX(),300)),DelayTime::create(1.0),CallFunc::create([=](){
		removeChild(_cliper);
		//Director::getInstance()->setDepthTest(false);
		auto s = GameScene::create();
		s->retain();
		//Director::getInstance()->setDepthTest(true);
		s->release();
		_image->release();
		auto scene = TransitionFade::create(1.0f, s, Color3B::BLACK);
		if (scene)
		{
			Director::getInstance()->replaceScene(scene);
		}
	}),nullptr));
	return true;
}

void PrePanel::rollText(float)
{
	_text->getPositionY()>300 ? _text->setPositionY(-150) : _text->setPositionY(_text->getPositionY()+2);
}

void PrePanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
		switch(tag)
		{
		case BEGIN:
			{

			}			
			break;
		case OPTION:
			{

			}
			break;
		case ABOUT:

			break;
		}

		//_SAFE_RETAIN(this); 
		//this->removeFromParentAndCleanup(true);
	}
}

void PrePanel::onEnter()
{
	SimplePanel::onEnter();
	/*for(auto node:_buttons->getChildren())
	{
	auto button = static_cast<Button*>(node);
	button->setOpacity(0);
	}*/
	/*auto image = static_cast<ImageView*>(Helper::seekWidgetByName(_root, "Image_mota"));
	image->setOpacity(0);
	image->runAction(FadeIn::create(1.0));
	image->runAction(Sequence::create(MoveTo::create(1.0,Vec2(image->getPosition().x,image->getPosition().y + 10)),CallFunc::create([this](){
	for(auto node:_buttons->getChildren())
	{
	auto button = static_cast<Button*>(node);
	button->runAction(FadeIn::create(1.0));
	}
	}),NULL));*/

}

void PrePanel::onExit()
{
	SimplePanel::onExit();
}