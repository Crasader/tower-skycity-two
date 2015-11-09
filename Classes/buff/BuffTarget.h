#ifndef __BUFF_TARGET_H__
#define __BUFF_TARGET_H__

#include "cocos2d.h"
USING_NS_CC;

class Buff;

class BuffTarget
{
public:
	virtual void addBuff(Buff* buff) = 0;
	virtual void removeBuff(Buff* buff) = 0;
	virtual void updateBuffsEffect() = 0;
};
#endif /*__BUFF_TARGET_H__*/