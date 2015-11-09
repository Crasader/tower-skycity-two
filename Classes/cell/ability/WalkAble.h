#ifndef __WALKABLE_H__
#define __WALKABLE_H__

#include "Ability.h"
#include "utils/AStar.h"

USING_NS_CC;

class WalkAble : public Ability
{
public:
	// 反向运动
	static Cell::Movement reverse(Cell::Movement dir){if(dir==Cell::Movement::UP){return Cell::Movement::DOWN;}
	else if(dir==Cell::Movement::DOWN){return Cell::Movement::UP;}
	else if(dir==Cell::Movement::LEFT){return Cell::Movement::RIGHT;}
	else if(dir==Cell::Movement::RIGHT){return Cell::Movement::LEFT;}
	else{return Cell::Movement::STOP;}
	}

	WalkAble();
	void update(float delta);

	// 向导控制接口
	void setGuide(Cell::Direction dir,int value);
	void resetGuides();
	void moveToSite(Vec2 site);
private:
	Vec2 _dis;
	PointArray *_paths;
	int _guides[5];
	Cell::Movement _trend;
private:
	void pathToSite(Vec2 dest);
	Point getNearSite(Point core);
	void guidesMakeTrend();
	void walk();
	bool nextTileCanMove(Cell::Movement);
	void switchMovement(Cell::Movement);
	bool checkCenterTile();
	Point pathMakeTrend();
private:
	AStar *_astar;
	// 位置
	Vec2 _nextSite;
	Vec2 _centerSite;
	Vec2 _faceSite;
};

#endif // __WALKABLE_H__!