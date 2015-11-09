#include "DragNode.h"

DragNode::DragNode(Node* touchNode, Node* moveNode)
{
	_touchNode = touchNode;
	_moveNode = moveNode;
}

DragNode* DragNode::create(Node* touchNode, Node* moveNode)
{
	auto ret = new DragNode(touchNode, moveNode);
	if(ret && ret->init())
	{
		ret->autorelease();
		ret->retain();
		return ret;
	}
	else
	{
		delete ret;
		ret = NULL;
		return NULL;
	}
}

bool DragNode::init()
{
	_dragDelay = 0.0f;

	onTouchedBegan = nullptr;
	onTouchedMoved = nullptr;
	onTouchedEnded = nullptr;

	_listener = EventListenerTouchOneByOne::create();
	_listener->setSwallowTouches(true);
	_listener->onTouchBegan = CC_CALLBACK_2(DragNode::onTouchBegan,this);
	_listener->onTouchMoved = CC_CALLBACK_2(DragNode::onTouchMoved,this);
	_listener->onTouchEnded = CC_CALLBACK_2(DragNode::onTouchEnded,this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_listener,_touchNode);
	return true;
}

DragNode::~DragNode()
{
	Director::getInstance()->getEventDispatcher()->removeEventListener(_listener);
	_listener = nullptr;
}

bool DragNode::onTouchBegan(Touch* touch,Event* event)
{
	auto node = event->getCurrentTarget();
	Vec2 locationInNode = _touchNode->convertToNodeSpace(touch->getLocation());
	Size s = node->getContentSize();
	Rect rect = Rect(0, 0, s.width, s.height);

	if (rect.containsPoint(locationInNode))
	{
		_moveNode->setOpacity(255);

		if(onTouchedBegan && _canDrug != true)
		{
			onTouchedBegan(_moveNode,Widget::TouchEventType::ENDED);
		}
		_downLast = true;
		_moveNode->stopAllActions();
		_moveNode->runAction(Sequence::create(DelayTime::create(_dragDelay), CallFunc::create([=]()
		{
			if(_downLast == true)
			{
				_canDrug = true;
				// ÍÏ¶¯¶¯»­
				//auto scaleBy = ScaleBy::create(0.08,1.1,1.1);
				//node->runAction(Sequence::create(scaleBy,scaleBy->reverse(),nullptr));
			}
		}),nullptr));
		return true;
	}
	return false;
}

void DragNode::onTouchMoved(Touch* touch,Event* event)
{
	if(_canDrug == true)
	{
		_moveNode->setPosition(_moveNode->getPosition() + touch->getDelta());
	}

	Vec2 locationInNode = _touchNode->convertToNodeSpace(touch->getLocation());
	Size s = _touchNode->getContentSize();
	Rect rect = Rect(0, 0, s.width, s.height);

	if (!rect.containsPoint(locationInNode))
	{
		if(onTouchedMoved && _canDrug != true)
		{
			onTouchedMoved(_moveNode,Widget::TouchEventType::ENDED);
		}
		_downLast = false;
	}
}

void DragNode::onTouchEnded(Touch* touch,Event* event)
{
	_moveNode->setOpacity(100);

	Vec2 locationInNode = _touchNode->convertToNodeSpace(touch->getLocation());
	Size s = _touchNode->getContentSize();
	Rect rect = Rect(0, 0, s.width, s.height);

	if (rect.containsPoint(locationInNode))
	{
		if(onTouchedEnded && _canDrug != true)
		{
			onTouchedEnded(_moveNode,Widget::TouchEventType::ENDED);
		}
	}
	_downLast = false;
	_canDrug = false;
}