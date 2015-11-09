#include "cocos2d.h"
#include "cell/Cell.h"

USING_NS_CC;

class Ability : public Ref
{
public:
	static Ability* create(std::string type);
	void setTarget(Cell* cell);
	Cell* getTarget(){return _target;}

	virtual void update(float delta);
protected:
	Cell* _target;
};
