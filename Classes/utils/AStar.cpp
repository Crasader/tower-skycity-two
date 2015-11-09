#include "AStar.h"

USING_NS_CC;

Vec2 AStar::CrossVecs[4] = {Vec2(-1,0),Vec2(0,1),Vec2(1, 0),Vec2(0,-1)};

AStar* AStar::create()
{
	AStar *pRet = new AStar();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		pRet->retain();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}	
}

AStar::~AStar()
{
	release();
}

bool AStar::init()
{
	return true;
}

AStar::AStar():_function(nullptr){}

void AStar::initCheckPointFunc(const std::function<bool(Point&)> &func)
{
	_function = func;
}

PointArray* AStar::getShortPath(Point start, Point end)
{
	_shortPath = PointArray::create(20);

	/*if (start == end)
	{
	LOG("You're already there! :P");
	return;
	}*/
	/*if (!_layer->isValidTileCoord(end) || _layer->isWallAtTileCoord(end)){
	SimpleAudioEngine::getInstance()->playEffect("hitWall.wav");
	return;
	}*/
	//LOG("Start: %f, %f", start.x, start.y);
	//LOG("End: %f, %f", end.x, end.y);
	//bool pathFound = false;
	_openCells.clear();
	_closeCells.clear();
	// ���ȣ������ʼ���굽open�б�
	this->insertInOpenCells(Cell::createWithPosition(start));
	do{
		// �õ���С��Fֵ����
		// ��Ϊ�������б���һ������������С��Fֵ
		Cell *curCell = _openCells.at(0);
		// ��ӵ�ǰ���赽closed�б�
		_closeCells.pushBack(curCell);
		// ������open�б������Ƴ�
		// ��Ҫע����ǣ������Ҫ�ȴ�open�б������Ƴ���ӦС�Ķ�����ڴ�
		_openCells.erase(0);
		// �����ǰ������Ŀ�귽�����꣬��ô�������
		if (curCell->getPosition() == end)
		{
			buildPath(curCell);
			_openCells.clear();
			_closeCells.clear();
			break;
		}
		// �õ���ǰ��������ڷ�������
		getBordorPoints(curCell->getPosition());
		for (ssize_t i = 0; i < _points->count(); ++i)
		{
			Cell *cell = Cell::createWithPosition(_points->getControlPointAtIndex(i));
			// ��鲽���ǲ����Ѿ���closed�б�			
			if(getCellIndex(_closeCells, cell) != -1)
			{
				continue;
			}
			// ����ӵ�ǰ���赽�˲���ĳɱ�
			int cost = this->cellCost(curCell, cell);
			// ���˲����Ƿ��Ѿ���open�б�
			ssize_t index = getCellIndex(_openCells, cell);
			// ����open�б������
			if (index == -1)
			{
				// ���õ�ǰ������Ϊ��һ������
				cell->setParent(curCell);
				// Gֵ��ͬ����һ����Gֵ + ����һ��������ĳɱ�
				cell->setGScore(curCell->getGScore() + cost);
				// Hֵ���ǴӴ˲��赽Ŀ�귽��������ƶ�������ֵ
				cell->setHScore(this->computeHScore(cell->getPosition(), end));
				// ������ӵ�open�б�
				this->insertInOpenCells(cell);
			}
			else
			{
				// ��ȡ�ɵĲ��裬��ֵ�Ѿ������
				cell = _openCells.at(index);
				// ���Gֵ�Ƿ���ڵ�ǰ���赽�˲����ֵ
				if ((curCell->getGScore() + cost) < cell->getGScore())
				{
					// ���õ�ǰ������Ϊ��һ������
					cell->setParent(curCell);
					// Gֵ��ͬ����һ����Gֵ + ����һ��������ĳɱ�
					cell->setGScore(curCell->getGScore() + cost);
					// ��ΪGֵ�ı��ˣ�FֵҲ����Ÿı�
					// ����Ϊ�˱���open�б�������Ҫ���˲����Ƴ��������°������
					// ���Ƴ�֮ǰ����Ҫ�ȱ�������
					cell->retain();
					// ���ڿ��Է����Ƴ������õ��ı��ͷ�
					_openCells.erase(index);
					// ���°������
					this->insertInOpenCells(cell);
					// ���ڿ����ͷ����ˣ���Ϊopen�б�Ӧ�ó�����
					cell->release();
				}
			}
		}
	} while (_openCells.size() > 0);

	_shortPath->insertControlPoint(start,0);


	/*for(int l = 0; l < _shortPath->count(); l++)
	{
	auto v = _shortPath->getControlPointAtIndex(l);
	log("%f       %f", v.x, v.y);
	}*/

	return _shortPath;
}

ssize_t AStar::getCellIndex(cocos2d::Vector<Cell*> &cells, Cell *cell)
{	
	for (ssize_t i = 0; i < cells.size(); ++i){
		if (cells.at(i)->isEqual(cell)){
			return i;
		}
	}
	return -1;
}

//ֱ�Ƿ�������
void AStar::getBordorPoints(Point core)
{
	_points = PointArray::create(4);	
	for(auto unit : CrossVecs)
	{
		Point p(core);
		p += unit;
		if(_function)
		{
			if(_function(p) == 0)
			{
				_points->addControlPoint(p);
			}			
		}else
		{
			_points->addControlPoint(p);
		}
	}
}

void AStar::buildPath(const Cell *cell)
{
	do{
		// ��ʼλ�ò�Ҫ�������
		if (cell->getParent())
		{
			// ���ǲ��뵽����0��λ�ã��Ա㷴ת·��
			Vec2 p = cell->getPosition();
			_shortPath->insertControlPoint(p, 0);
		}
		cell = cell->getParent();   // ����
	} while (cell);                 // ֱ��û����һ��
}


void AStar::insertInOpenCells(Cell *cell)
{
	int cellFScore = cell->getFScore();
	ssize_t count = _openCells.size();
	ssize_t i = 0;
	for (; i < count; ++i)
	{
		if (cellFScore <= _openCells.at(i)->getFScore())
		{
			break;
		}
	}
	_openCells.insert(i, cell);
}

int AStar::computeHScore(Point start, Point end)
{
	// ����ʹ�������ٷ���������ӵ�ǰ���赽��Ŀ�경�裬��ˮƽ�ʹ�ֱ�����ܵĲ���
	// �����˿�����·�ϵĸ����ϰ�
	return abs(end.x - start.x) + abs(end.y - start.y);
}

int AStar::cellCost(const Cell *start, const Cell *end){
	//return ((fromCell->getPosition().x != toCell->getPosition().x)
	//	&& (fromCell->getPosition().y != toCell->getPosition().y)) ? 14 : 10;
	return 1;
}


AStar::Cell::Cell() :
	_position(Point::ZERO),
	_gScore(0),
	_hScore(0),
	_parent(nullptr)
{
}

AStar::Cell::~Cell()
{
}

AStar::Cell *AStar::Cell::createWithPosition(Point pos)
{
	AStar::Cell *pRet = new AStar::Cell();
	if (pRet && pRet->initWithPosition(pos))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return nullptr;
	}
}

bool AStar::Cell::initWithPosition(Point pos)
{
	bool bRet = false;
	do
	{
		this->setPosition(pos);
		bRet = true;
	} while (0);

	return bRet;
}



bool AStar::Cell::isEqual(const AStar::Cell *other) const
{
	return this->getPosition() == other->getPosition();
}

std::string AStar::Cell::getDescription() const
{
	return StringUtils::format("pos=[%.0f;%.0f]  g=%d  h=%d  f=%d",
		this->getPosition().x, this->getPosition().y,
		this->getGScore(), this->getHScore(), this->getFScore());
}