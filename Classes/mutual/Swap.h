#ifndef __SHOP_H__
#define __SHOP_H__

#include "cocos2d.h"

class SwapPanel;
class Actor;
class Effect;

USING_NS_CC;

class Swap:public Node
{
public:
	Swap(std::string buyer, std::string seller);
	static Swap* create(std::string buyer, std::string seller);
	ValueVector options;
	std::string title;
	void deal(int option);
private:
	Actor* _buyer;
	Actor* _seller;
private:
	void realize(Actor* actor, ValueMap map);
	bool check(Actor* actor, ValueMap map);
private:
	SwapPanel* _swapPanel;
};

#endif /*__SHOP_H__*/