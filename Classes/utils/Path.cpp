#include "Path.h"
#include "utils/StringHandler.h"
#include "map/Floor.h"

USING_NS_CC;

Path::Line* Path::Line::create(ValueMap model)
{
	auto line = new Line();
	line->init(model);
	line->autorelease();
	return line;
}

bool Path::Line::init(ValueMap model)
{
	auto startVec = model["start"].asValueMap();
	auto finishVec = model["finish"].asValueMap();
	auto pathPosVec = model["pathPos"].asValueMap();

	auto startOri = Vec2(startVec["x"].asFloat() + pathPosVec["x"].asFloat(),  pathPosVec["y"].asFloat() - startVec["y"].asFloat());
	auto finishOri = Vec2(finishVec["x"].asFloat() + pathPosVec["x"].asFloat(),  pathPosVec["y"].asFloat() - finishVec["y"].asFloat());
	startOri = Vec2((int)(startOri.x * 0.025), (int)(startOri.y * 0.025));
	finishOri = Vec2((int)(finishOri.x * 0.025), (int)(finishOri.y * 0.025));
	start = Vec2(startOri.x * TILE_WIDTH + TILE_HALF_WIDTH, startOri.y * TILE_HEIGHT + TILE_HALF_HEIGHT);
	finish = Vec2(finishOri.x * TILE_WIDTH + TILE_HALF_WIDTH, finishOri.y * TILE_HEIGHT + TILE_HALF_HEIGHT);

	if(!model["data"].isNull())
		data = model["data"].asValueMap();

	return true;
}

FiniteTimeAction* Path::Line::getActionWithCell(Cell* cell, bool reverse)
{
	Vec2 p1;
	/*if(checkInLine(cell->getPosition()))
		p1 = cell->getPosition();
	else*/
		p1 = this->start;
	auto p2 = this->finish;
	if(reverse == true)
	{
		p1 = this->finish;
		p2 = this->start;
	}
	auto dis = sqrt((p2.x-p1.x)*(p2.x-p1.x)+(p2.y-p1.y)*(p2.y-p1.y));
	
	Vector<FiniteTimeAction*> actions;
	auto move = MoveTo::create(GET_DUR_BY_SPEED_AND_DIS(cell->getSpeed(),dis), p2);
	FiniteTimeAction* moveAction = move;
	if(!data["ease"].isNull())
	{
		auto easeStr = data["ease"].asString();
		if(easeStr == "EaseCubicActionOut")
		{
			moveAction = EaseCubicActionOut::create(move);
		}
		else if(easeStr == "EaseBackInOut")
		{
			moveAction = EaseBackInOut::create(move);
		}
		else if(easeStr == "EaseExponentialInOut")
		{
			moveAction = EaseExponentialInOut::create(move);
		}
	}
	actions.pushBack(moveAction);
	if(!data["startDelay"].isNull())
		actions.insert(0, DelayTime::create(data["startDelay"].asFloat()));
	if(!data["finishDelay"].isNull())
		actions.pushBack(DelayTime::create(data["finishDelay"].asFloat()));
	auto action = Sequence::create(actions);
	return action;
}

bool Path::Line::checkInLine(Point point)
{
	if((finish.y - start.y)/(finish.x - start.x) * (point.x - start.x) + start.y == point.y)
	{
		return true;
	}
	return false;
}

Path* Path::create(ValueMap model)
{
	Path* path = new Path();
	if(path != nullptr && path->initWithModel(model))
	{
		path->autorelease();
		return path;
	}
	else
	{
		return nullptr;
	}
}

void Path::setFloor(Floor* floor)
{
	_floor = floor;
}

bool Path::initWithModel(ValueMap model)
{
	_model = model;

	ValueVector points = model["polylinePoints"].asValueVector();
	_points = PointArray::create(points.size());
	_points->retain();
	for(auto point:points)
	{
		Vec2 vec(point.asValueMap()["x"].asFloat(), point.asValueMap()["y"].asFloat());
		_points->addControlPoint(vec);
	}

	ValueVector datas;
	ValueVector datasVec;
	stringToValueVector(model["datas"].asString(), "###", datasVec);
	for(auto data:datasVec)
	{
		ValueMap model;
		stringToValueMap(data.asString(),model);
		datas.push_back(cocos2d::Value(model));
	}

	int i = 0;
	for(auto iter = points.begin(); iter != points.end() - 1; iter ++)
	{
		auto start = (*iter).asValueMap();
		auto finish = (*(iter+1)).asValueMap();
		ValueMap lineModel;
		lineModel["start"] = start;
		lineModel["finish"] = finish;
		ValueMap pathPos;
		pathPos["x"] = model["x"];
		pathPos["y"] = model["y"];
		lineModel["pathPos"] = pathPos;
		if(i < datas.size())
			lineModel["data"] = datas.at(i);
		Line* line = Line::create(lineModel);
		_lines.pushBack(line);
		i++;
	}
	return true;
}

void Path::runCell(Cell* cell)
{
	int i = 0;
	for(; i < _lines.size(); i++)
	{
		auto line = _lines.at(i);
		if(line->checkInLine(cell->getPosition()))
		{
			break;
		}
	}
	Action* action;

	Vector<FiniteTimeAction*> actions;
	if(i < _lines.size())
	{
		for(; i < _lines.size(); i++)
		{
			auto line = _lines.at(i);
			actions.pushBack(line->getActionWithCell(cell));
		}
		if(!_model["type"].isNull())
		{
			auto type = _model["type"].asString();
			if(type == "backForever")
			{
				int j = _lines.size() - 1;
				for(; j >=0 ; j--)
				{
					auto line = _lines.at(j);
					actions.pushBack(line->getActionWithCell(cell, true));
				}
				action = RepeatForever::create(Sequence::create(actions));
			}
			else if(type == "forever")
			{
				action = RepeatForever::create(Sequence::create(actions));
			}
		}
		else
			action = Sequence::create(actions);

		cell->runAction(action);
	}
}