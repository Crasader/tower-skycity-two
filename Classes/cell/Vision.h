#ifndef __VISION_H__
#define __VISION_H__

#include "cocos2d.h"
#include "cell/Cell.h"
USING_NS_CC;

class Vision : public Cell
{
public:
	Vision(int typeId):Cell(typeId){}
	Vision(ValueMap& model):Cell(model){}
	static Vision* create(int typeId);
	static Vision* create(ValueMap& model);

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

	virtual int isBlock();
private:
	virtual void initView();
	
};

#endif /*__VISION_H__*/