#ifndef __BATTERY_H__
#define __BATTERY_H__

#include "cocos2d.h"
USING_NS_CC;

class Cell;

class Battery : public Ref
{
public:
	~Battery();
	static Battery* create(ValueMap model);
	bool initWithModel(ValueMap model);
	void setTarget(Cell* cell);
	Cell* getTarget(){return _target;}

	void fire();
	void stop();
private:
	ValueMap _model;
	Cell* _target;
	
	Action* _fireAction;
	ValueVector _fireData; 
};

#endif //__BATTERY_H__