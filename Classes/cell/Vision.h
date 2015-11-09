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
	//// ��ײ����
	//// ��Ԫ������
	virtual void crashOn(Cell* cell);
	virtual void crashOut(Cell* cell);
	//// ���òȵ���Ԫ���е�
	//virtual void stepOn(Cell* cell);
	//// ������Ԫ��
	//virtual void touchOn(Cell* cell);

	virtual int isBlock();
private:
	virtual void initView();
	
};

#endif /*__VISION_H__*/