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
	//// ��ײ����
	//// ��Ԫ������
	virtual void crashOn(Cell* cell);
	virtual void crashOut(Cell* cell);
	//// ���òȵ���Ԫ���е�
	//virtual void stepOn(Cell* cell);
	//// ������Ԫ��
	//virtual void touchOn(Cell* cell);
	virtual std::string getType();
	virtual int isBlock();
private:
	virtual void initView();
	virtual void onEnter();
	
};

#endif /*__WALL_H__*/