#ifndef __WALL_H__
#define __WALL_H__

#include "cocos2d.h"
#include "cell/Cell.h"
USING_NS_CC;

class Wall : public Cell
{
public:
	Wall(int typeId):Cell(typeId){}
	Wall(ValueMap& model):Cell(model){}
	static Wall* create(int typeId);
	static Wall* create(ValueMap& model);

	CC_SYNTHESIZE(Vec2, _disSite, DisSite);
	CC_SYNTHESIZE(Cell*, _owner, Owner);
	//// 碰撞函数
	//// 单元格区域
	virtual void crashOn(Cell* cell);
	virtual void crashOut(Cell* cell);
	//// 正好踩到单元格中点
	//virtual void stepOn(Cell* cell);
	//// 触碰单元格
	//virtual void touchOn(Cell* cell);
	virtual std::string getType();
	virtual int isBlock();
private:
	virtual void initView();
	virtual void onEnter();
	
};

#endif /*__WALL_H__*/