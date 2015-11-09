#ifndef __ABILITY_H__
#define __ABILITY_H__

#include "Ability.h"
#include "WalkAble.h"

Ability* Ability::create(std::string type)
{
	Ability* ability = nullptr;
	if(type == "walk")
	{
		ability = new WalkAble();
	}
	else if(type == "skill")
	{

	}
	else if(type == "fight")
	{

	}
	else if(type == "chat")
	{

	}
	else if(type == "swap")
	{

	}
	else
	{
		ability = new Ability();
	}
	ability->autorelease();
	return ability;
}

void Ability::setTarget(Cell* cell)
{
	_target = cell;
}

#endif //__ABILITY_H__