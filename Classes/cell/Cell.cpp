#include "Vision.h"
#include "Cell.h"
#include "Detect.h"
#include "User.h"
#include "utils/EventHelper.h"
#include "utils/AStar.h"
#include "Global.h"
#include "model/Model.h"
#include "map/Tower.h"
#include "map/Floor.h"
#include "map/World.h"
#include "actor/Actor.h"
#include "utils/Path.h"
#include "cell/accessory/Battery.h"

std::string Cell::MovementStr[5] = {"stop","left","up","right","down"};
std::string Cell::AnimationTypeStr[3] = {"idle","walk","hit"};

bool Cell::onTrigger(EventCustom *event)
{
	auto data = static_cast<ValueMap*>(event->getUserData());
	auto effect = (*data)["effect"].asValueMap();
	for(auto type:_triggerOn)
	{
		if((*data)["type"].asString() == type.first)
		{
			for(auto tri:type.second.asValueVector())
			{
				auto vec = tri.asValueVector();
				ValueMap map;
				stringToValueMap(vec.at(0).asString(),map);
				if(map["target"].isNull())
				{
					map["target"] = (*data)["senderName"].asString();
				}
				else if(map["target"].asString() == "this")
				{
					map["target"] = this->getName();
				}
				else if(map["target"].asString() == "player")
				{
					map["target"] = Detect::shareDetect()->getPlayer()->getName();
				}
				if(!map["senderTarget"].isNull())
				{
					if(map["senderTarget"].asString() != (*data)["senderName"].asString())
						continue;
				}
				auto mapStr = valueMapToString(map);
				if(!map["delay"].isNull())
				{
					auto delay = map["delay"].asFloat();
					Director::getInstance()->getRunningScene()->runAction(Sequence::create(DelayTime::create(delay),CallFunc::create([=](){
						EventHelper::getInstance()->dispatchByStr(mapStr,vec.at(1).asString());
					}),nullptr));
				}
				else
				{
					EventHelper::getInstance()->dispatchByStr(mapStr,vec.at(1).asString());
				}
			}
		}
	}

	if((*data)["type"].asString() == "convey" && _conveyCount > 1)
	{
		int towerId = getFloor()->getTower()->getId();
		int floorId;
		if(!effect["towerId"].isNull())
		{
			towerId = effect["towerId"].asInt();
		}
		if(!effect["floorId"].isNull())
		{
			floorId = effect["floorId"].asInt();
		}
		else if(!effect["addFloorId"].isNull())
		{
			floorId = _floor->getId() + effect["addFloorId"].asInt();
		}
		else
		{
			floorId = _floor->getId();
		}
		if(floorId < 1 || floorId > Detect::shareDetect()->getTower(towerId)->getFloorsNum())
		{
			log("not valid floorId");
			return false;
		}
		_conveyCount = 0;

		Vec2 site(0,0);
		if(!effect["site"].isNull())
		{
			site = PointFromString(effect["site"].asString());
		}
		else if(!effect["toCell"].isNull())
		{
			auto cell = Detect::shareDetect()->getCellByName(effect["toCell"].asString());
			if(cell != nullptr) site = cell->getSite();
		}
		else if(!effect["toCellTypeId"].isNull())
		{
			auto cell = Detect::shareDetect()->getWorld()->getTower(towerId)->getFloor(floorId)->getCellByType(effect["toCellTypeId"].asInt());
			if(cell != nullptr) site = cell->getSite();
		}

		float delayTime = 0;
		if(!effect["delay"].isNull())
		{
			delayTime = effect["delay"].asFloat();
		}
		if(this->getType() == "Player")
			Detect::shareDetect()->setControlled(4,0);
		runAction(Sequence::create(DelayTime::create(delayTime),CallFunc::create([=](){
			convey(site,3,floorId,towerId);
			if(this->getType() == "Player")
				Detect::shareDetect()->setControlled(4,1);
		}),nullptr));
	}
	else if((*data)["type"].asString() == "fall")
	{
		fall();
	}
	else if((*data)["type"].asString() == "disappear")
	{
		disappear();
	}
	else if((*data)["type"].asString() == "fire")
	{
		if(!effect["on"].isNull())
		{
			fire();
		}
		else if(!effect["stop"].isNull())
		{
			stopFire();
		}
	}
	return false;
}

//################################################  碰撞函数  ##################################################
// 单元格区域
void Cell::crashOn(Cell* cell)
{

}

void Cell::crashOut(Cell* cell)
{

}

// 正好踩到单元格中点
void Cell::stepOn(Cell* cell)
{

}

// 一般是删除
void Cell::stepOut(Cell* cell)
{

}

// 触碰单元格
void Cell::touchOn(Cell* cell)
{

}

// 触碰单元格
void Cell::touchOut(Cell* cell)
{

}
// ################################################  碰撞函数  ##################################################


#if DETECT_ACTOR == 1
void Cell::setLabel(std::string str1, std::string str2)
{
	_label1->setString(str1);
	//_label2->setString(win);
	if(str2 == "win")
	{
		//_label1->enableShadow(Color4B::GREEN,Size(1,-1),100);
		_label1->enableOutline(Color4B(120,196,0,255),1);

	}
	else
	{
		//_label1->enableShadow(Color4B::RED,Size(1,-1),100);
		_label1->enableOutline(Color4B(196,0,0,255),1);
	}
}
#endif

Vec2 Cell::getSite() const
{
	return _site;
}

void Cell::setSite(const Vec2 &vec)
{
	setPosition(Vec2(vec.x * TILE_WIDTH + TILE_HALF_WIDTH, vec.y * TILE_HEIGHT + TILE_HALF_HEIGHT));
	if(vec != _site)
	{
		_lastSite = _site;
		_site = vec;
		moveToNewSite();
		//stepToNewSite();
	}
}

void Cell::setPosition(const Vec2 &vec)
{
	Layer::setPosition(vec);
	for(auto vision:_visions)
	{
		auto disVec = Vec2(vision->getDisSite().x * TILE_WIDTH, vision->getDisSite().y * TILE_HEIGHT);
		vision->setPosition(disVec + vec);
	}
}

void Cell::detachFromUnit()
{
	if(_unit)
	{
		_unit->detach(this);
	}
}

void Cell::removeFromFloor()
{
	if(_floor)
	{
		_floor->removeCell(this);
	}
}

void Cell::moveToNewSite()
{
	if(_floor == nullptr) return;
	_floor->detachFromUnit(this,_lastSite);
	_floor->attachToUnit(this, _site);
}

void Cell::stepToNewSite()
{
	if(_floor == nullptr) return;
	_floor->centerToUnit(this,_site);
}

void Cell::stepOutTheSite()
{
	if(_floor == nullptr) return;
	_floor->centerOutUnit(this,_site);
}

float Cell::getSpeed()
{
	return _speed * Detect::shareDetect()->getUser()->getMultiSpeed();
}

Cell::Direction Cell::getFace()
{
	return _face;
}

int Cell::getLayer()
{
	return _model["layer"].asInt();
}

void Cell::setLayer(int id)
{
	_layer = id;
}

int Cell::isBlock()
{
	return 1;
}

bool Cell::init()
{
	Layer::init();
	bool bRet = false;
	do{
		initNull();
		initModel();
		initView();
		initOthers();

		// 脚本初始化
		if(!_model["trigger"].isNull() && _model["trigger"].asString() != "")
		{
			ValueVector vec1;
			stringToValueVector(_model["trigger"].asString(),"###",vec1);
			for (auto i:vec1)
			{
				strArray a;
				split(i.asString(),"$",a);

				if(a.size() > 1)
				{
					ValueVector vec3;
					stringToValueVector(a.at(1),"~~~",vec3);
					if(vec3.size() < 2)
					{
						vec3.push_back(cocos2d::Value(""));
					}
					if(_triggerOn[a.at(0)].isNull())
					{
						_triggerOn[a.at(0)] = ValueVector();
					}
					_triggerOn[a.at(0)].asValueVector().push_back(cocos2d::Value(vec3));
				}			
			}
		}
		// 事件监听
		_listener = EventListenerCustom::create(getName(), [=](EventCustom *event){onTrigger(event);});
		_eventDispatcher->addEventListenerWithFixedPriority(_listener, 1);

		bRet = true;
	} while (0);

	log("%s------build", getName().c_str());
	return bRet;
}

void Cell::initNull()
{
	setName(_model["name"].asString());
	_nameId = getName();
	_curAnimate = nullptr;
	_curAnimation = nullptr;
	_lastSite = _site = Vec2(-1,-1);
	_shader = nullptr;
	_floor = nullptr;
	_refresh = false;
	_busy = false;
	_mutual = 0;

	_saveSpeed = 0;
	_ani = IDLE;
	_face = DOWN;
	_move = STOP;
	_paths = nullptr;
	_trend = STOP;
	_canWalk = true;
	_canGuide = true;
	_canTouch = true;
	_astar = AStar::create();
	_jumpAction = nullptr;
	_viewJumpAction = nullptr;
	_shadowJumpSmallAction = nullptr;

	_battery = nullptr;
	_actionMove = false;
	resetGuides();
}

void Cell::destroy()
{
	//this->stopAllActions();
	/*_eventDispatcher->removeEventListener(_listener);
	CC_SAFE_RELEASE(_listener);
	_listener = nullptr;*/
	//_curAnimate = nullptr;
	//_curAnimation = nullptr;
	//_blockFunc = nullptr;
	//removeFromParent();
}

void Cell::initModel()
{
	
}

void Cell::loadModel(ValueMap &map)
{
	Model::modelCoverModel(map,_model);
}

void Cell::refreshModel()
{

}

void Cell::refresh()
{
	_refresh = true;
	refreshModel();
	initOthers();
	_refresh = false;
}

ValueMap Cell::saveModel()
{
	ValueMap model;
	model["name"] = getName();
	model["site"] = pointToString(getSite());
	model["typeId"] = getTypeId();
	model["trigger"] = _model["trigger"];
	model["busy"] = _busy;
	model["mutual"] = _mutual;
	model["move"] = _move;
	model["speed"] = _speed;
	model["conveyCount"] = _conveyCount;
	return model;
}

void Cell::initView()
{
	// 动画
	_animations = ValueMap();
	stringToValueMap(_model["animation"].asString(), _animations);

	auto imageUrl = _model["image"].asString();
	if(imageUrl == "")
	{
		if(_animations.size() > 0)
		{
			auto animation = AnimationCache::getInstance()->getAnimation((*_animations.begin()).second.asString());
			auto frames = animation->getFrames();

			auto frame = frames.at(0);
			_view = Sprite::createWithSpriteFrame(frame->getSpriteFrame());
		}
		else
		{
			imageUrl = NONE_IMAGE;
			_view = Sprite::createWithSpriteFrameName(imageUrl);
		}
	}
	else 
		_view = Sprite::createWithSpriteFrameName(imageUrl);

	_view->setAnchorPoint(Point(0.5,0.25));
	addChild(_view);

#if DETECT_ACTOR == 1
	_label1 = Label::createWithTTF("", "fonts/58983.ttf", 13);
	_label1->setColor(Color3B::WHITE);
	addChild(_label1);
	_label1->setPosition(Vec2(30,10));
	_label1->setAnchorPoint(Vec2(0.5, 0.5));
	_label1->setVisible(true);
	_label1->setString("");
	//_label1->enableOutline(Color4B::GREEN,10);
	/*_label2 = Text::create("","Arial",15);
	_label2->setColor(Color3B::WHITE);
	addChild(_label2);
	_label2->setPosition(Vec2(30,10));
	_label2->setAnchorPoint(Vec2(0.5, 0.5));
	_label2->setVisible(false);
	_label2->setString("");
	_label2->enableShadow(Color4B::RED,Size(1,-1),100);*/
#endif
	//setLabel(getName(),"");
}

void Cell::initOthers()
{
	if(!_model["busy"].isNull())
	{
		_busy = _model["busy"].asBool();
	}
	if(!_model["mutual"].isNull())
	{
		_mutual = _model["mutual"].asInt();
	}

	_conveyCount = 2;
	if(!_model["conveyCount"].isNull())
	{
		_conveyCount = _model["conveyCount"].asInt();
	}
	if(!_model["fireData"].isNull())
	{
		ValueMap fireModel;
		fireModel["fireData"] = _model["fireData"];
		fireModel["loop"] = _model["fireLoop"];
		_battery = Battery::create(fireModel);
		_battery->retain();
		_battery->setTarget(this);
	}
}

Cell::~Cell()
{
	//CC_SAFE_RELEASE_NULL(_view);
	//CC_SAFE_RELEASE_NULL(_label1);
	CC_SAFE_RELEASE_NULL(_battery);
	_visions.clear();
	CC_SAFE_RELEASE(_paths);
	if(_listener != nullptr)
	{
		_eventDispatcher->removeEventListener(_listener);
		_listener = nullptr;
	}
	log("%s------destroy", getName().c_str());
}

void Cell::setBlockFunc(std::function<int(Vec2&)> func)
{
	_blockFunc = func;
	_astar->initCheckPointFunc(func);
}

std::function<int(Vec2&)> Cell::getBlockFunc()
{
	return _blockFunc;
}

const std::string Cell::getNameId()
{
	return _model["name"].asString();
}

std::string Cell::getEffect()
{
	return _model["effect"].asString();
}

std::string Cell::getImageUrl()
{
	return _model["image"].asString();
}

const cocos2d::Value Cell::getModelByName(std::string name)
{
	return _model[name];
}

void Cell::setModelByName(std::string name, cocos2d::Value value)
{
	_model[name] = value;
}

void Cell::onEnter()
{
	Layer::onEnter();
	if(_shader != nullptr)
	{
		_shader->shade->runAction(RepeatForever::create(Sequence::create(FadeTo::create(0.1,80),FadeTo::create(0.1,200),DelayTime::create(0.8),nullptr)));
		//_shader->shade->runAction(RepeatForever::create(Shaky3D::create(5, Size(15,10), 4, false)));
	}
	_curAnimation = nullptr;
	animate(_ani,_face);
	scheduleUpdate();
	//animate(IDLE,_face);
}

void Cell::onExit()
{
	Layer::onExit();
	this->unscheduleUpdate();
	//_eventDispatcher->removeEventListener(_listener);
}

void Cell::disappear()
{
	this->setBusy(true);
	_view->runAction(Sequence::create(FadeOut::create(1.0f),CallFunc::create([=](){this->setBusy(false);this->removeFromFloor();}),nullptr));
}

std::string Cell::getType()
{
	return "Cell";
}

std::string Cell::getNickName()
{
	return _model["nickName"].asString();
}

// #########################################   动画   ##############################################

void Cell::animate(AnimationType type, Direction dir)
{
	auto str = StringUtils::format("%s_%s", AnimationTypeStr[type].c_str(), MovementStr[dir].c_str());
	if(setAnimation(str, true) == true)
	{
		_ani = type;
		_face = dir;
		_faceSite = _site + CrossVecs[_face];
	}
}

bool Cell::setAnimation(const std::string &str, bool loop, CallFuncN* callBack, bool again, float duration)
{
	if(_animations.find(str) == _animations.end())
		return false;
	auto animation = AnimationCache::getInstance()->getAnimation(_animations[str].asString());
	if (again == false && _curAnimation == animation)
		return false;
	_curAnimation = animation;
	if (_curAnimate != nullptr)
	{
		_view->stopAction(_curAnimate);
	}
	auto animate = Animate::create(animation);
	if(duration != -1) animate->setDuration(duration);
	if(loop == true)
	{
		_curAnimate = RepeatForever::create(Sequence::create(animate,callBack,NULL));
	}
	else
	{
		_curAnimate = Sequence::create(animate,callBack,NULL);
	}
	_view->runAction(_curAnimate);
	return true;
}
// #########################################   动画   ##############################################


// #########################################   运动   ##############################################
void Cell::update(float delta)
{
	Layer::update(delta);
	if(_canGuide == true)
		guidesMakeTrend();
	if(_canWalk == true)
		walk();
}

void Cell::pathToSite(Vec2 dest)
{
	CC_SAFE_RELEASE(_paths);
	Vec2 vec = this->getPosition();
	Point site;
	vec = (vec - Vec2(TILE_HALF_WIDTH, TILE_HALF_HEIGHT)) * 0.025;
	site.setPoint(int(vec.x), int(vec.y));
	Vec2 nextSite = site + _dis;

	_paths = _astar->getShortPath((_move == UP || _move ==RIGHT)?nextSite:site,dest);
	auto removeSite = pathMakeTrend();
	if (_trend != STOP && _move != reverse(_trend) && _move != STOP && !checkCenterTile()/* && removeSite != Vec2()*/)
	{
		_paths->insertControlPoint(removeSite, 0);
	}
	CC_SAFE_RETAIN(_paths);
}

Vec2 Cell::pathMakeTrend()
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
			_trend = Movement::LEFT;
		}else if(p == CrossVecs[2])
		{
			_trend = Movement::UP;
		}else if(p == CrossVecs[3])
		{
			_trend = Movement::RIGHT;
		}else if(p == CrossVecs[4])
		{
			_trend = Movement::DOWN;
		}
		return curSite;
	}
	return Vec2();
}

void Cell::walk()
{
	if(_trend != STOP)
	{
		if (nextTileCanMove(_trend) && (_move == _trend||_move == reverse(_trend)||_move == STOP||checkCenterTile()))
		{
			switchMovement(_trend);
			//playSound = true;
		}
		// 不能移动转向
		else if(_move == STOP)
		{
			animate(IDLE, (Direction)_trend);
		}
	}

	if(_dis.isZero() == true && _actionMove == false)
		animate(IDLE, _face);
	else
		animate(WALK, _face);

	Vec2 vec = this->getPosition();

	vec.x += _dis.x * getSpeed();
	vec.y += _dis.y * getSpeed();// * / 40

	this->setPosition(vec);

	Vec2 newSite;
	vec *= 0.025;
	newSite.setPoint(floor(vec.x), floor(vec.y));
	if(newSite != _site)
	{
		_lastSite = _site;
		_site = newSite;
		_conveyCount++;
		moveToNewSite();
	}
	// 判断中点，然后根据路径修改运动趋势
	if (checkCenterTile()) 
	{
		_trend = STOP;
		if(_paths != nullptr && _paths->getControlPointAtIndex(0) == _site)
			pathMakeTrend();
		switchMovement(STOP);
	}
}

//检查坐标在不在方块的中心
bool Cell::checkCenterTile()
{
	Vec2 center(((_site.x * TILE_WIDTH) + TILE_HALF_WIDTH), ((_site.y * TILE_HEIGHT) + TILE_HALF_HEIGHT));
	auto vec = this->getPosition();
	if(center == vec)
	{
		if(_centerSite != _site)
		{
			_centerSite = _site;
			stepToNewSite();
		}
		return true;
	}
	return false;
}

//运动吧
void Cell::switchMovement(Movement movement)
{
	_dis.set(CrossVecs[int(movement)]);
	_move = movement;
	if(movement != STOP)
	{
		animate(_ani, Direction(movement));
	}
	_nextSite = _site + _dis;
}

void Cell::setCanGuide(bool b, float delay)
{
	resetGuides();
	if(delay != 0)
	{
		auto antion = Sequence::create(DelayTime::create(delay), CallFunc::create([=](){setCanGuide(b);}), NULL);
		this->runAction(antion);
		return;
	}
	if(b == false)
	{
		_trend = STOP;
		_canGuide = false;
	}
	else if(b == true)
	{
		_canGuide = true;
	}
}

//检查游戏对象的下一个方块是否可行
bool Cell::nextTileCanMove(Direction dir)
{	
	//设置下一个方块行和列
	Vec2 vec = _site + CrossVecs[int(dir)];
	return !_blockFunc(vec);
}

// 走到。。附近
Point Cell::getNearSite(Point core)
{
	std::vector<Point> points;	
	for(auto unit : CrossVecs)
	{
		Point p(core);
		p += unit;
		if(p == _site) return _site;
		if(_blockFunc(p) == 0)
		{
			points.push_back(p);
		}
	}
	int min = 10000000;
	int i = 0;
	int num;
	for(auto point:points)
	{
		int dis = abs(_site.x - point.x) + abs(_site.y - point.y);
		if(min > dis)
		{
			min = dis;
			num = i;
		}
		i++;
	}
	if(points.size() == 0)
		return _site;
	else
		return points.at(num);
}

void Cell::moveToSite(Vec2 site)
{
	if(_canGuide == false)
		return;
	int block = _blockFunc(site);
	if(block > 0)
	{
		// 特殊门或怪物走到附近
		if(block == 2)
		{
			auto nearSite = getNearSite(site);
			if(nearSite != _site)
				pathToSite(nearSite);
		}
		// 在附近趋势转向，触发
		auto dis = site - _site;
		if(pow(dis.x,2) + pow(dis.y,2) == 1)
		{
			// 先趋势转向
			Point dis = site - _site;
			Direction dir;
			if(dis == CrossVecs[1])
			{
				dir = LEFT;
			}else if(dis == CrossVecs[2])
			{
				dir = UP;
			}else if(dis == CrossVecs[3])
			{
				dir = RIGHT;
			}else if(dis == CrossVecs[4])
			{
				dir = DOWN;
			}
			animate(_ani,Direction(dir));
			// 后触发
			if(_floor != nullptr)
				_floor->touchBeganUnit(this, _faceSite);
		}
	}
	else
	{
		Detect::shareDetect()->addCellByType(3001010,site,2,_floor->getId(),_floor->getTower()->getId());
		pathToSite(site);
	}
}

void Cell::setGuide(Cell::Direction dir,int value)
{
	_guides[(int)dir] = value;
}

void Cell::resetGuides()
{
	for(int i = 1; i < 5;i++)
	{
		_guides[i] = 0;
	}
}

void Cell::guidesMakeTrend()
{
	// 方向从1开始
	for(int guide = 1; guide < 5;guide++)
	{
		if(_guides[guide] == 1)
		{
			_trend = (Movement)guide;
			break;
		}
	}
}

void Cell::convey(Vec2 site, int layerId, int floorId, int towerId)
{
	Detect::shareDetect()->convey(this, site, layerId, floorId, towerId);
}

void Cell::runPath(std::string name)
{
	auto path = this->getFloor()->getPathByName(name);
	if(path != nullptr)
	{
		path->runCell(this);
	}
}

void Cell::jumpTo(Vec2 site)
{
	int block = _blockFunc(site);
	// 特殊门或怪物, 还有无法跳跃
	if(block > 0)
		return;
	auto floor = _floor;
	auto pos = getPosition();
	setCanWalk(false);
	setCanTouch(false);
	/*if(Detect::shareDetect()->getCellByName(getName()) != nullptr)
	{
	this->retain();
	this->removeFromFloor();
	this->setFloor(floor);
	this->setBlockFunc(CC_CALLBACK_1(Floor::isBlockBySite,floor));
	floor->getAir()->addChild(this);
	}
	this->setPosition(pos);*/
	auto toPos = Vec2(site.x * TILE_WIDTH + TILE_HALF_WIDTH, site.y * TILE_HEIGHT + TILE_HALF_HEIGHT);
	// 跟相对距离有关
	float dur = 0.5;
	float height = sqrt(pow(toPos.x - pos.x,2) + pow(toPos.y - pos.y,2)) / 5 + 20;
	this->stopAction(_jumpAction);
	_jumpAction = Sequence::create(JumpTo::create(dur, toPos, height, 1),CallFunc::create([=](){
		//this->removeFromParent();
		setCanWalk(true);
		setCanTouch(true);
		//floor->addCell(this,site,getLayer());
	}),nullptr);
	runAction(_jumpAction);
}

// 掉进
void Cell::fall()
{
	this->setCanWalk(false);
	this->setCanTouch(false);

	auto shape = DrawNode::create();
	static Vec2 triangle[4];
	triangle[0] = Vec2(-_view->getContentSize().width / 2, -TILE_HALF_HEIGHT+6);
	triangle[1] = Vec2(_view->getContentSize().width / 2, -TILE_HALF_HEIGHT+6);
	triangle[2] = Vec2(_view->getContentSize().width / 2, _view->getContentSize().height + TILE_HALF_HEIGHT - 6);
	triangle[3] = Vec2(-_view->getContentSize().width / 2, _view->getContentSize().height + TILE_HALF_HEIGHT - 6);

	static Color4F green(0, 1, 0, 1);
	shape->drawPolygon(triangle, 4, green, 0, green);

	//shape->setAnchorPoint(Vec2(0.5,0.5));
	auto stencil = shape;

	auto clipper = ClippingNode::create();
	clipper->setStencil(stencil);
	this->addChild(clipper);

	_view->retain();
	_view->removeFromParent();

	auto content = _view;
	clipper->addChild(content);
	_view->release();

	_view->runAction(Sequence::create(EaseIn::create(MoveTo::create(1.0f, Vec2(0, -_view->getContentSize().height)),8.5f), CallFunc::create([=](){
		_view->retain();
		_view->removeFromParent();
		this->removeChild(clipper);
		_view->setPosition(0,0);
		addChild(_view);
		this->convey(_site, getLayer(), getFloor()->getId()-1,getFloor()->getTower()->getId());
		this->setCanWalk(true);
		this->setCanTouch(true);
	}),nullptr));
}

// ###################################  运动  ######################################

void Cell::fire()
{
	if(_battery != nullptr) 
		_battery->fire();
}
void Cell::stopFire()
{
	if(_battery != nullptr) 
		_battery->stop();
}
void Cell::setBattery(Battery* battery)
{
	_battery = battery; 
	_battery->setTarget(this);
}

Cell::Cell(ValueMap& model)
{
	_model = model;
	auto value = _model["typeId"];
	if(!value.isNull())
	{
		_typeId = value.asInt();
	}
	else
	{
		_typeId = 1;
	}
}

Cell::Cell(int typeId)
{
	_typeId = typeId;
}

int Cell::getTypeId()
{
	return _typeId;
}

void Cell::addVisions()
{
	for(auto vision:_visions)
	{
		_floor->addCell(vision, vision->getDisSite() + _site, 1);
	}
}

void Cell::removeVisions()
{
	for(auto vision:_visions)
	{
		vision->removeFromFloor();
	}
}

void Cell::inVision(Cell* cell)
{
	//_visionCells.insert(cell->getName(), cell);
}

void Cell::outVision(Cell* cell)
{
	//_visionCells.erase(cell->getName());
}

//void setPosition(const Vec2 &vec)
//{
//	Layer::setPosition(x, y);
//}

Shader* Shader::create(std::string pshade, std::string pmask)
{
	Shader *ret = new Shader(pshade, pmask);
	if (ret && ret->init())
	{
		//ret->autorelease();
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}

Shader::Shader(std::string pshade, std::string pmask)
{
	shade = Sprite::createWithSpriteFrameName(pshade);
	mask = Sprite::createWithSpriteFrameName(pmask);
	//BlendFunc func1 = {GL_ZERO, GL_SRC_ALPHA};
	//shade->setBlendFunc(func1);
	
	shade->retain();
	mask->retain();
}

Shader::~Shader()
{
	shade->release();
	mask->release();
}

bool Shader::init()
{
	return true;
}

void Shader::update(Vec2 pos)
{
	shade->setPosition(pos);
	mask->setPosition(pos);
}

