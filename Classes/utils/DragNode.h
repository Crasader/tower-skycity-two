#ifndef __DRAG_NODE__
#define __DRAG_NODE__

#include "cocos2d.h"
#include "ui/SimplePanel.h"
USING_NS_CC;

/************************************************************************/
/* 自动添加进内存管理，需要手动release                                     */
/************************************************************************/
class DragNode : public Ref
{
public:
	~DragNode();
	DragNode(Node* touchNode, Node* moveNode);
	static DragNode* create(Node* node, Node* moveNode);
	bool init();
	CC_SYNTHESIZE(Node*, _touchNode, TouchNode);
	CC_SYNTHESIZE(Node*, _moveNode, MoveNode);
	CC_SYNTHESIZE(float, _dragDelay, DragDelay);
	std::function<void(cocos2d::Ref *ref, ui::Widget::TouchEventType touchType)> onTouchedBegan;
	std::function<void(cocos2d::Ref *ref, ui::Widget::TouchEventType touchType)> onTouchedMoved;
	std::function<void(cocos2d::Ref *ref, ui::Widget::TouchEventType touchType)> onTouchedEnded;
private:
	bool _downLast;
	bool _canDrug;
	EventListenerTouchOneByOne* _listener;
	bool onTouchBegan(Touch* touch,Event* event);
	void onTouchMoved(Touch* touch,Event* event);
	void onTouchEnded(Touch* touch,Event* event);
};

#endif //__DRAG_NODE__