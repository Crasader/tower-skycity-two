#ifndef __PATH_H__
#define __PATH_H__

#include "cocos2d.h"
USING_NS_CC;

class Floor;
class Cell;

class Path : public Ref
{
public:
	class Line : public Ref
	{
	public:
		static Line* create(ValueMap model);
		Point start;
		Point finish;
		bool checkInLine(Point point);
		ValueMap model;
		ValueMap data;
		bool init(ValueMap model);

		FiniteTimeAction* getActionWithCell(Cell* cell, bool reverse = false);
	};

	static Path* create(ValueMap model);
	bool initWithModel(ValueMap model);

	Floor* getFloor(){return _floor;}
	void setFloor(Floor* floor);

	PointArray* getPointArray() const {return _points;}

	void runCell(Cell* cell);
private:
	PointArray* _points;
	Vector<Line*> _lines;
	ValueMap _model;
	Floor* _floor;
};

#endif //__PATH_H__