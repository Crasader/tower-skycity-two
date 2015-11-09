#include "Wall.h"
#include "Detect.h"

Wall* Wall::create(int id)
{
	Wall *pRet = new Wall(id);
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

Wall* Wall::create(ValueMap& model)
{
	Wall *pRet = new Wall(model);
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

void Wall::crashOn(Cell* cell)
{
	Cell::crashOn(cell);
}

void Wall::crashOut(Cell* cell)
{
	Cell::crashOut(cell);
}

int Wall::isBlock()
{
	return 1;
}

void Wall::onEnter()
{
	Cell::onEnter();
	this->unscheduleUpdate();
}

void Wall::initView()
{
	if(Detect::shareDetect()->viewHide != 1)
	{
		_label1 = Label::createWithTTF("", "fonts/58983.ttf", 13);
		_label1->setColor(Color3B::BLUE);
		addChild(_label1);
		_label1->setPosition(Vec2(0,0));
		_label1->setAnchorPoint(Vec2(0.5, 0.5));
		_label1->setVisible(true);
		_label1->setString("qb");
	}
}

std::string Wall::getType()
{
	return "Wall";
}