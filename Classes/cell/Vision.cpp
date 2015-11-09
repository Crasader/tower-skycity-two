#include "Vision.h"
#include "Detect.h"

Vision* Vision::create(int id)
{
	Vision *pRet = new Vision(id);
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

Vision* Vision::create(ValueMap& model)
{
	Vision *pRet = new Vision(model);
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

void Vision::crashOn(Cell* cell)
{
	Cell::crashOn(cell);
	if(cell != _owner)
		_owner->inVision(cell);
}

void Vision::crashOut(Cell* cell)
{
	Cell::crashOut(cell);
	if(cell != _owner)
		_owner->outVision(cell);
}

int Vision::isBlock()
{
	return 0;
}

void Vision::initView()
{
	if(Detect::shareDetect()->viewHide != 1)
	{
		_label1 = Label::createWithTTF("", "fonts/58983.ttf", 13);
		_label1->setColor(Color3B::WHITE);
		addChild(_label1);
		_label1->setPosition(Vec2(0,0));
		_label1->setAnchorPoint(Vec2(0.5, 0.5));
		_label1->setVisible(true);
		_label1->setString("sb");
	}
}