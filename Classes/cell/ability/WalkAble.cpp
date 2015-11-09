#include "WalkAble.h"
#include "Detect.h"


Vec2 WalkAble::pathMakeTrend()
{	
	if(_paths->count() > 1)
	{
		Point curSite;
		Point nextSite;
		curSite = _paths->getControlPointAtIndex(0);
		_paths->removeControlPointAtIndex(0);
		nextSite = _paths->getControlPointAtIndex(0);

		Point p = nextSite - curSite;
		if(p == CrossVecs[1])
		{
			_trend = Cell::Movement::LEFT;
		}else if(p == CrossVecs[2])
		{
			_trend = Cell::Movement::UP;
		}else if(p == CrossVecs[3])
		{
			_trend = Cell::Movement::RIGHT;
		}else if(p == CrossVecs[4])
		{
			_trend = Cell::Movement::DOWN;
		}
		return curSite;
	}
	return Vec2();
}

void WalkAble::update(float delta)
{
	guidesMakeTrend();
	walk();
}

void WalkAble::walk()
{
	auto move = _target->getMove();
	if(_trend != Cell::STOP)
	{
		if (nextTileCanMove(_trend) && (move == _trend||move == reverse(_trend)||move == Cell::STOP||checkCenterTile()))
		{
			switchMovement(_trend);
			//playSound = true;
		}
		else if(move == Cell::STOP)
		{
			_target->animate(Cell::IDLE, (Cell::Direction)_trend);
		}
	}

	if(_dis.isZero() == true)
	{
		_target->animate(Cell::IDLE, _target->getFace());
	}
	else
	{
		_target->animate(Cell::WALK, _target->getFace());
	}

	Vec2 vec = _target->getPosition();

	vec.x += _dis.x * _target->getSpeed();
	vec.y += _dis.y * _target->getSpeed();// * / 40

	_target->setPosition(vec);

	Vec2 newSite;
	vec *= 0.025;
	newSite.setPoint(floor(vec.x), floor(vec.y));
	if(newSite != _target->getSite())
	{
		_target->setLastSite(_target->getSite());
		_target->setSite(newSite);
		//_conveyCount++;
		_target->moveToNewSite();
	}
	// 判断中点，然后根据路径修改运动趋势
	if (checkCenterTile()) 
	{
		_trend = Cell::STOP;
		if(_paths != nullptr && _paths->getControlPointAtIndex(0) == _target->getSite())
		{
			pathMakeTrend();
		}
		switchMovement(Cell::STOP);
	}
}

//检查坐标在不在方块的中心
bool WalkAble::checkCenterTile()
{
	auto site = _target->getSite();
	Vec2 center(((site.x * TILE_WIDTH) + TILE_HALF_WIDTH), ((site.y * TILE_HEIGHT) + TILE_HALF_HEIGHT));
	auto vec = _target->getPosition();
	if(center == vec)
	{
		if(_centerSite != site)
		{
			_centerSite = site;
			_target->stepToNewSite();
		}
		return true;
	}
	return false;
}

//运动吧
void WalkAble::switchMovement(Cell::Movement movement)
{
	_dis.set(CrossVecs[int(movement)]);
	_target->setMove(movement);
	if(movement != Cell::STOP)
	{
		_target->animate(_target->getAni(), Cell::Direction(movement));
	}
	_nextSite = _target->getSite() + _dis;
}


//void WalkAble::setCanGuide(bool b, float delay)
//{
//	resetGuides();
//	if(delay != 0)
//	{
//		auto antion = Sequence::create(DelayTime::create(delay), CallFunc::create([=](){setCanGuide(b);}), NULL);
//		this->runAction(antion);
//		return;
//	}
//	if(b == false)
//	{
//		_trend = STOP;
//		_canGuide = false;
//	}
//	else if(b == true)
//	{
//		_canGuide = true;
//	}
//}

void WalkAble::moveToSite(Vec2 site)
{
	auto site = _target->getSite();
	int block = _target->getBlockFunc()(site);
	if(block > 0)
	{
		// 特殊门或怪物走到附近
		if(block == 2)
		{
			auto nearSite = getNearSite(site);
			if(nearSite != site)
				pathToSite(nearSite);
		}
		// 正好面对
		if(_faceSite == site)
		{
			// 先趋势转向
			Point p = site - site;
			if(p == CrossVecs[1])
			{
				_trend = Cell::LEFT;
			}else if(p == CrossVecs[2])
			{
				_trend = Cell::UP;
			}else if(p == CrossVecs[3])
			{
				_trend = Cell::RIGHT;
			}else if(p == CrossVecs[4])
			{
				_trend = Cell::DOWN;
			}
			//animate(_ani,Direction(dir));
			// 后触发
			/*if(_floor != nullptr)
			_floor->touchUnit(this, _faceSite);*/
		}
	}
	else
	{
		Detect::shareDetect()->addCellByType(3001010,site,2,_target->getFloor()->getId(),_target->getFloor()->getTower()->getId());
		pathToSite(site);
	}
}

void WalkAble::setGuide(Cell::Direction dir,int value)
{
	_guides[(int)dir] = value;
}

void WalkAble::resetGuides()
{
	for(int i = 1; i < 5;i++)
	{
		_guides[i] = 0;
	}
}

void WalkAble::pathToSite(Vec2 dest)
{
	CC_SAFE_RELEASE(_paths);
	Vec2 vec = _target->getPosition();
	Point site;
	vec = (vec - Vec2(TILE_HALF_WIDTH, TILE_HALF_HEIGHT)) * 0.025;
	site.setPoint(int(vec.x), int(vec.y));
	Vec2 nextSite = site + _dis;

	auto move = _target->getMove();
	_astar->initCheckPointFunc(_target->getBlockFunc());
	_paths = _astar->getShortPath((move == Cell::UP || move ==Cell::RIGHT)?nextSite:site,dest);
	auto removeSite = pathMakeTrend();
	if (_trend != Cell::STOP && move != reverse(_trend) && move != Cell::STOP && !checkCenterTile()/* && removeSite != Vec2()*/)
	{
		_paths->insertControlPoint(removeSite, 0);
	}
	CC_SAFE_RETAIN(_paths);
}