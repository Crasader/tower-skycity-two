#include "cell/Cell.h"
#include "Floor.h"
#include "Tower.h"
#include "cell/actor/Actor.h"
#include "cell/item/Item.h"
#include "utils/Shake.h"
#include "utils/Altar.h"
#include "utils/AnimateByGID.h"
#include "utils/StringHandler.h"
#include "cell/CellFactory.h"
#include "base/CCNS.h"
#include "model/Model.h"
#include "Detect.h"
#include "utils/SpotLight.h"
#include "cell/CellFactory.h"
#include "utils/EventHelper.h"
#include "utils/Path.h"


USING_NS_CC;

Floor::Floor(ValueMap model)
{
	_model = model;
	_id = _model["id"].asInt();
	setName(_model["name"].asString());
}

Floor::Floor(int id)
{
	_id = id;
	setName(StringUtils::format("floor%d", id));
}

Floor *Floor::create(int id)
{
	Floor *ret = new Floor(id);	
	if (ret && ret->init())
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}

Floor* Floor::create(ValueMap model)
{
	Floor *ret = new Floor(model);
	if (ret && ret->init())
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}

bool Floor::init()
{
	Layer::init();
	bool bRet = false;
	do{
		
		initModel();
		//initMap();
		initOthers();
		bRet = true;
	} while (0);
	return bRet;
}


void Floor::initModel()
{
	initMap();
}

void Floor::loadModel(ValueMap &map)
{
	//removeAllCells();
	// 构建墙碰撞区
	initWalls();
	// 创建cell
	initCells(map["objects"].asValueVector());
	if(!map["altar"].isNull() && altar != nullptr)
	{
		altar->loadModel(map["altar"].asValueMap());
	}
	initPaths();
}

void Floor::refreshMap()
{
	auto mapNew = TMXTiledMap::create(_model["url"].asString());
	auto models = mapNew->getProperties();
	auto objects = mapNew->getObjectGroup("stage")->getObjects();
	std::map<std::string, cocos2d::Value> tempObjects;
	for(auto object:objects)
	{
		auto objectMap = object.asValueMap();
		Vec2 site;
		site.x = (int)((objectMap["x"].asFloat() + CELL_HALF_WIDTH) * 0.025);
		site.y = (int)(objectMap["y"].asFloat() * 0.025);
		if(!objectMap["site"].isNull())
		{
			site = PointFromString(objectMap["site"].asString());
		}
		std::string name;
		if(objectMap["name"].isNull())
			name = StringUtils::format("Cell_%02d%02d%02d%02d",_tower->getId(),getId(),(int)site.x,(int)site.y);
		else
			name = objectMap["name"].asString();

		if(!objectMap["remove"].isNull())
		{
			auto cell = Detect::shareDetect()->getCellByName(name);
			if(cell != nullptr) cell->removeFromFloor();
		}
		else if(!objectMap["add"].isNull())
		{
			if(objectMap["typeId"].isNull() && !objectMap["gid"].isNull())
			{
				auto value = _map0->getPropertiesForGID(objectMap["gid"].asInt());
				if(!value.isNull())
					objectMap["typeId"] = value.asValueMap()["typeId"];
				else
					continue;
			}
			objectMap["name"] = name;
			auto cell = CellFactory::getInstance()->createCell(objectMap);
			Detect::shareDetect()->addCell(cell,site,1,getId(),_tower->getId());
		}
		else if(!objectMap["refresh"].isNull())
		{
			auto cell = Detect::shareDetect()->getCellByName(name);
			if(cell != nullptr) cell->refresh();
		}
	}
	updateFight();
}

ValueMap Floor::saveModel()
{
	ValueMap model;
	ValueVector objects;
	for(auto cell:_cells)
	{
		// 墙壁不用记录
		if(cell->getType() != "Wall")
			objects.push_back(cocos2d::Value(cell->saveModel()));
	}
	model["objects"] = objects;

	if(altar != nullptr)
	{
		model["altar"] = altar->saveModel();
	}

	return model;
}

void Floor::initOthers()
{
	//_listener = EventListenerCustom::create(getName(), CC_CALLBACK_1(Floor::onTrigger, this));
	_listener = EventListenerCustom::create(getName(), [=](EventCustom *event){onTrigger(event);});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, this);
	_mapScale = 1.0f;
	_mapBuilded = false;
}



void Floor::initMap()
{
	// 初始化地图
	_map0 = TMXTiledMap::create("map_0_0.tmx");
	_map0->retain();

	_map = TMXTiledMap::create(_model["url"].asString());
	CC_SAFE_RETAIN(_map);
	addChild(_map);
	_horizonTileNum = _map->getMapSize().width / 2;
	_verticalTileNum = _map->getMapSize().height / 2;

	// 初始化单元格
	for(int i = -TILE_BORDER; i < _horizonTileNum + TILE_BORDER; i++)
	{
		for(int j = -TILE_BORDER; j < _verticalTileNum + TILE_BORDER; j++)
		{
			auto unit = Unit::create(Vec2(i,j));
			unit->setFloor(this);
			_units.insert(StringUtils::format("%d,%d", i, j), unit);
		}
	}
	
	_ground = _map->getLayer("ground");
	_under = _map->getLayer("under");
	_above = _map->getLayer("above");
	_stageGroup = _map->getObjectGroup("stage");
	_pathGroup = _map->getObjectGroup("path");

	_stage = Layer::create();
	_stage->addChild(Layer::create(),0,0);
	_stage->addChild(Layer::create(),1,1);//物品
	_stage->addChild(Layer::create(),2,2);
	_stage->addChild(Layer::create(),3,3);//角色
	_stage->addChild(Layer::create(),4,4);
	_stage->addChild(Layer::create(),10,10);//天空
	_map->addChild(_stage);

	_air = Layer::create();
	_map->addChild(_air);

	_dark = Layer::create();
	_pLayer= LayerColor::create(Color4B(0,0,0,255));
	_pLayer->setContentSize(Size(_map->getMapSize().width * _map->getTileSize().width,_map->getMapSize().height * _map->getTileSize().height));
	ClippingNode* pClip = ClippingNode::create();
	pClip->setInverted(true);
	pClip->setAlphaThreshold(0.5f);
	_dark->addChild(pClip, 0, 0);
	pClip->addChild(_pLayer);
	auto mask = Layer::create();
	auto shade = Layer::create();
	_dark->addChild(shade, 1, 1);
	pClip->setStencil(mask);
	addChild(_dark,1000);

	//_ground->setZOrder(0);
	//_under->setZOrder(1);
	auto zOrder = _map->getChildrenCount() - 3;
	auto zOrderValue = _stageGroup->getProperty("zOrder");
	if(!zOrderValue.isNull())
	{
		zOrder = zOrderValue.asInt() - 1;
	}
	_stage->setZOrder(zOrder);
	_air->setZOrder(_map->getChildrenCount() - 1);

	// 构建墙碰撞区
	initWalls();
	// 初始化元素层
	initCells(_stageGroup->getObjects());
	initPaths();
	altar = nullptr;

	this->scheduleUpdate();
}

void Floor::initWalls()
{
	ssize_t count = _map->getChildrenCount();
	
	TMXLayer* layer;
	for(int k = 0; k < count;k++)
	{
		layer = dynamic_cast<TMXLayer*>(_map->getChildByTag(k));
		if(layer == nullptr) continue;
		for(auto i = 0; i < _horizonTileNum; i++)
		{
			for(auto j = 0; j < _verticalTileNum; j++)
			{
				// 四点
				Vec2 center = Vec2(i,_verticalTileNum - 1 - j);
				Vec2 point[] = {
					Vec2(2 * center.x, 2 * center.y + 1),
					Vec2(2 * center.x, 2 * center.y),
					Vec2(2 * center.x + 1, 2 * center.y + 1),
					Vec2(2 * center.x + 1, 2 * center.y)
				};
				for(int l = 0; l < 4; l++)
				{
					auto gid = layer->getTileGIDAt(point[l]);
					if(gid > 0)
					{
						auto value = _map0->getPropertiesForGID(gid);
						if(!value.isNull())
						{
							auto block = value.asValueMap()["block"].asInt();
							if(block > 0)
							{
								ValueMap model;
								model["typeId"] = 9000000;
								model["type"] = "Wall";
								auto wall = CellFactory::getInstance()->createCell(model);
								addCell(wall,Vec2(i,j),wall->getLayer());
								break;
							}
						}
					}
				}
			}
		}
	}
}

void Floor::lightUpdate(float dt)
{
	if(lightCount >= lightNum)
	{
		lightCount = 0;
		lighting();
		drawShanDian();
	}
	lightCount++;
}

void Floor::lighting()
{
	lightNum = CCRANDOM_0_1()*(10-2+1)+2;
	_lightColor->runAction(_lightAction);
}

void Floor::drawShanDian()
{
	_lightDraw->clear();
	int num = CCRANDOM_0_1()*(3-0+1)+0;
	while(num--)
	{
		Point pos1;
		Point pos2;
		pos1.x = CCRANDOM_0_1()*(480-0+1)+0;
		pos2.x = CCRANDOM_0_1()*(480-0+1)+0;

		pos1.y = CCRANDOM_0_1()*(550-500+1)+500;
		pos2.y = CCRANDOM_0_1()*(220-100+1)+100;
		//调用函数
		drawLighting(pos1.x, pos1.y, pos2.x, pos2.y, 200);
	}
	
}

void Floor::drawLighting(float x1, float y1, float x2, float y2, float displace)
{
	if(displace < 5.0f){
		//ccDrawLine(ccp(x1, y1), ccp(x2, y2));
		_lightDraw->drawSegment(ccp(x1, y1),ccp(x2, y2),0.4f,Color4F(1, 1, 1, 0.5));
	}else{
		float mid_x = (x2+x1) / 2;
		float mid_y = (y2+y1) / 2;
		mid_x += (CCRANDOM_0_1() -.5)*displace;
		mid_y += (CCRANDOM_0_1() -.5)*displace;

		drawLighting(x1, y1, mid_x, mid_y, displace/2);
		drawLighting(x2, y2, mid_x, mid_y, displace/2);
	}
}

void Floor::setWeather(std::string type)
{
	
}

void Floor::addShade(Shader* shader)
{
	if(shader == nullptr) return;
	dynamic_cast<ClippingNode*>(_dark->getChildByTag(0))->getStencil()->addChild(shader->mask);
	_dark->getChildByTag(1)->addChild(shader->shade);
}

void Floor::addShadeAt(std::string fShade, std::string fMask, Vec2 pos)
{
	auto shader = Shader::create(fShade, fMask);
	shader->autorelease();
	shader->update(pos);
	dynamic_cast<ClippingNode*>(_dark->getChildByTag(0))->getStencil()->addChild(shader->mask);
	_dark->getChildByTag(1)->addChild(shader->shade);
}

void Floor::removeShade(Shader* shader)
{
	if(shader == nullptr) return;
	dynamic_cast<ClippingNode*>(_dark->getChildByTag(0))->getStencil()->removeChild(shader->mask);
	_dark->getChildByTag(1)->removeChild(shader->shade);
}

void Floor::update(float delta)
{
	auto player = Detect::shareDetect()->getPlayer();
	if(player != nullptr)
	{
		auto winSize = Director::getInstance()->getWinSize();
		auto size = this->getPosition();
		Size viewSize(440, 520);
		auto position = player->getPosition() * _mapScale;
		int x = MAX(position.x, viewSize.width / 2);
		int y = MAX(position.y, viewSize.height / 2);
		x = MIN(x, (_map->getMapSize().width * _map->getTileSize().width * _mapScale) - viewSize.width / 2);
		y = MIN(y, (_map->getMapSize().height * _map->getTileSize().height * _mapScale) - viewSize.height / 2);
		Point actualPosition(x, y);
		Point centerOfView(viewSize.width / 2, viewSize.height / 2);
		Point viewPoint = centerOfView - actualPosition + Vec2(winSize.width / 2, winSize.height / 2) * (_mapScale - 1);
		this->setPosition(viewPoint);
	}
}

void Floor::initCells(ValueVector &objects)
{
	// 添加对象
	for(auto object : objects)
	{
		// tiled右下侧编辑器数据
		auto objectMap = object.asValueMap();
		if(objectMap["typeId"].isNull() && !objectMap["gid"].isNull())
		{
			auto value = _map0->getPropertiesForGID(objectMap["gid"].asInt());
			if(!value.isNull())
				objectMap["typeId"] = value.asValueMap()["typeId"];
			else
				continue;
		}
		if(objectMap["name"].asString() == "hello")
		{
			log("DFSFE");
		}
		Vec2 site;
		site.x = (int)((objectMap["x"].asFloat() + CELL_HALF_WIDTH) * 0.025);
		site.y = (int)(objectMap["y"].asFloat() * 0.025);
		if(!objectMap["site"].isNull())
		{
			site = PointFromString(objectMap["site"].asString());
		}
		auto cell = CellFactory::getInstance()->createCell(objectMap);
		addCell(cell,site,cell->getLayer());
	}
#if DETECT_ACTOR == 1
	updateFight();
#endif
	_cellInited = true;
}

void Floor::initPaths()
{
	if(_pathGroup == nullptr)
		return;
	auto vec = _pathGroup->getObjects();
	for(auto path:vec)
	{
		auto map = path.asValueMap();
		auto path = Path::create(map);
		path->setFloor(this);
		//if(map["name"].asString() == "path_1")
		//{
		//	log("DFSFE");
		//}
		_paths.insert(map["name"].asString(), path);
	}
}

void Floor::updateMap(float dt)
{

}

Cell* Floor::getCellByType(int type)
{
	for(auto cell:_cells)
	{
		if(cell->getTypeId() == type)
		{
			return cell;
		}
	}
	return nullptr;
}

Cell* Floor::getCellByType(int type, Vec2 site)
{
	Cell* cell = nullptr;
	auto unit = _units.at(StringUtils::format("%d,%d", (int)site.x, (int)site.y));
	auto cells = unit->getCells();
	for(auto cell:cells)
	{
		if(cell->getTypeId() == type)
		{
			return cell;
		}
	}
	return cell;
}

void Floor::addCell(Cell* cell, Point site, int layerId)
{
	if(site.x >= _horizonTileNum + TILE_BORDER || site.x < -TILE_BORDER || site.y >= _verticalTileNum + TILE_BORDER || site.y < -TILE_BORDER)
		return;
	if(Detect::shareDetect()->attachCell(cell) == false) return;
	_cells.pushBack(cell);
	// 添加显示对象
	_stage->getChildByTag(layerId)->addChild(cell);
	// 设置楼层
	cell->setFloor(this);
	cell->setBlockFunc(CC_CALLBACK_1(Floor::isBlockBySite,this));
	// 添加碰撞检测单元
	cell->setSite(site);
	// 创建视野
	cell->addVisions();
	addShade(cell->getShader());
}

bool Floor::removeCell(Cell* cell)
{
	if(Detect::shareDetect()->detachCell(cell) == false) return false;
	_cells.eraseObject(cell);
	// 删除视野
	cell->removeVisions();
	// 删除碰撞检测
	cell->setSite(Vec2(-1,-1));
	//// 停止更新
	//cell->unscheduleUpdate();
	// 删除楼层
	cell->setFloor(nullptr);
	cell->setBlockFunc(nullptr);
	// 删除显示对象
	removeShade(cell->getShader());
	cell->removeFromParent();
	return true;
}

void Floor::removeAllCells()
{
	/*for(auto pair:_units)
	{
		auto uCells = pair.second->getCells();
		Vector<Cell*> cells;
		for(auto cell:uCells)
		{
			cells.pushBack(cell);
		}
		uCells.clear();
		for(auto cell:cells)
		{
			cell->removeFromFloor();
		}
		cells.clear();
	}*/

	Vector<Cell*> cells;
	for(auto cell:_cells)
	{
		cells.pushBack(cell);
	}
	for(auto cell:cells)
	{
		cell->removeFromFloor();
	}
	cells.clear();
	/*for(auto pair:_units)
	{
	auto unit = pair.second;
	auto cells = unit->getCells();
	for(auto cell:cells)
	{
	if(cell->getName() == "player")
	{
	log("dfdfdf");
	}
	Detect::shareDetect()->detachCell(cell);
	cell->destroy();
	removeShade(cell->getShader());
	}
	unit->clearCells();
	}*/
}

//bool Floor::removeCellByType(int type)
//{
//	for(auto pair:_units)
//	{
//		auto unit = pair.second;
//		auto cells = unit->getCells();
//		for(auto cell:cells)
//		{
//			if(cell->getTypeId() == type)
//			{
//				unit->detach(cell);
//				cell->removeFromParent();
//				removeShade(cell->getShader());
//				return true;
//			}
//		}
//	}
//	return false;
//}
//
//bool Floor::removeCellByType(int type, Vec2 site)
//{
//	if(site.x < 0 || site.x >= _horizonTileNum || site.y < 0 || site.y >= _verticalTileNum)
//		return false;
//	auto unit = _units.at(site.x + site.y * _horizonTileNum);
//	auto cells = unit->getCells();
//	for(auto cell:cells)
//	{
//		if(cell->getTypeId() == type)
//		{
//			unit->detach(cell);
//			cell->removeFromParent();
//			removeShade(cell->getShader());
//			return true;
//		}
//	}
//	return false;
//}

// 1:墙壁、2、物件、10、超范围
int Floor::isBlockBySite(const Point &p)
{
	//log("%d",_id);
	int bRet = 0;
	ssize_t count = _map->getChildrenCount();
	if(p.x >= _horizonTileNum || p.y >= _verticalTileNum || p.x < 0 || p.y < 0)
	{
		return 10;
	}
	
	auto block2 = _units.at(StringUtils::format("%d,%d", (int)p.x, (int)p.y))->isBlock();
	if(block2 > 0)
	{
		bRet = block2;
	}
	else
	{
		TMXLayer* layer;
		for(int i = 0; i < count;i++)
		{
			layer = dynamic_cast<TMXLayer*>(_map->getChildByTag(i));
			if(layer == nullptr) continue;
			Vec2 center = Vec2(p.x,_verticalTileNum - 1 - p.y);
			Vec2 point[] = {
				Vec2(2 * center.x, 2 * center.y + 1),
				Vec2(2 * center.x, 2 * center.y),
				Vec2(2 * center.x, 2 * center.y + 1),
				Vec2(2 * center.x + 1, 2 * center.y + 1)
			};
			for(int j = 0; j < 4; j++)
			{
				auto gid = layer->getTileGIDAt(point[j]);
				if(gid > 0)
				{
					auto value = _map0->getPropertiesForGID(gid);
					if(!value.isNull())
					{
						auto block = value.asValueMap()["block"].asInt();
						if(block > 0)
						{
							//log("%f, %f",p.x, p.y);
							bRet = block;
						}
					}				
				}
			}
		}
	}
	// 破墙
	if(bRet == 100)
		bRet = 0;
	//log("GID:%i, Properties:%s", b);
	return bRet;
}

Floor::~Floor()
{
	_eventDispatcher->removeEventListener(_listener);
}

const int Floor::getId() const
{
	return _id;
}

std::string Floor::getType()
{
	return "Floor";
}

void Floor::onEnter()
{
	Layer::onEnter();	
	
}

void Floor::onExit()
{
	Layer::onExit();
}

const std::string Floor::getNameId()
{
	return _model["name"].asString();
}

Vector<Cell*> Floor::getAllActor()
{
	Vector<Cell*> vec;
	for(auto unit:_units)
	{
		for(auto cell:unit.second->getCells())
		{
			auto actor = dynamic_cast<Actor*>(cell);
			if(actor != nullptr)
			{
				vec.pushBack(cell);
			}
		}
	}
	return vec;
}

Vector<Cell*> Floor::getAllItem()
{
	Vector<Cell*> vec;
	for(auto unit:_units)
	{
		for(auto cell:unit.second->getCells())
		{
			auto item = dynamic_cast<Item*>(cell);
			if(item != nullptr)
			{
				vec.pushBack(cell);
			}
		}
	}
	return vec;
}

Map<int, Cell*> Floor::getAllActorType()
{
	Map<int, Cell*> map;
	for(auto cell:_cells)
	{
		auto actor = dynamic_cast<Actor*>(cell);
		if(actor != nullptr && map.at(actor->getTypeId()) == nullptr)
		{
			map.insert(actor->getTypeId(),cell);
		}
	}
	return map;
}

Map<int, Cell*> Floor::getAllItemType()
{
	Map<int, Cell*> map;
	for(auto unit:_units)
	{
		for(auto cell:unit.second->getCells())
		{
			auto item = dynamic_cast<Item*>(cell);
			if(item != nullptr && map.at(item->getTypeId()) == nullptr)
			{
				map.insert(item->getTypeId(),cell);
			}
		}
	}
	return map;
}

void Floor::attachToUnit(Cell* cell, Vec2 site)
{
	//auto newSite = PointFromString((*data)["effect"].asValueMap()["newSite"].asString());
	if(site.x >= _horizonTileNum + TILE_BORDER || site.x < -TILE_BORDER || site.y >= _verticalTileNum + TILE_BORDER || site.y < -TILE_BORDER)
		return;
	auto unit = _units.at(StringUtils::format("%d,%d", (int)site.x, (int)site.y));
 	unit->attach(cell);
}

void Floor::detachFromUnit(Cell* cell, Vec2 site)
{
	//auto lastSite = PointFromString((*data)["effect"].asValueMap()["lastSite"].asString());
	if(site.x >= _horizonTileNum + TILE_BORDER || site.x < -TILE_BORDER || site.y >= _verticalTileNum + TILE_BORDER || site.y < -TILE_BORDER)
		return;
	auto key = StringUtils::format("%d,%d", (int)site.x, (int)site.y);
	auto unit = _units.at(key);
	unit->detach(cell);
}

void Floor::centerToUnit(Cell* cell, Vec2 site)
{
	//auto site = PointFromString((*data)["effect"].asValueMap()["newSite"].asString());
	if(site.x >= _horizonTileNum + TILE_BORDER || site.y >= _verticalTileNum + TILE_BORDER || site.x < -TILE_BORDER || site.y < -TILE_BORDER)
		return;
	auto unit = _units.at(StringUtils::format("%d,%d", (int)site.x, (int)site.y));
	unit->center(cell);
}

void Floor::centerOutUnit(Cell* cell, Vec2 site)
{
	//auto site = PointFromString((*data)["effect"].asValueMap()["newSite"].asString());
	if(site.x >= _horizonTileNum + TILE_BORDER || site.y >= _verticalTileNum + TILE_BORDER || site.x < -TILE_BORDER || site.y < -TILE_BORDER)
		return;
	auto unit = _units.at(StringUtils::format("%d,%d", (int)site.x, (int)site.y));
	unit->centerOut(cell);
}

void Floor::touchBeganUnit(Cell* cell, Vec2 site)
{
	if(site.x >= _horizonTileNum + TILE_BORDER || site.y >= _verticalTileNum + TILE_BORDER || site.x < -TILE_BORDER || site.y < -TILE_BORDER)
		return;
	if(cell->getCanTouch() == false)
		return;
	auto unit = _units.at(StringUtils::format("%d,%d", (int)site.x, (int)site.y));
	unit->touchBegan(cell);
}

void Floor::touchEndedUnit(Cell* cell, Vec2 site)
{
	if(site.x >= _horizonTileNum + TILE_BORDER || site.y >= _verticalTileNum + TILE_BORDER || site.x < -TILE_BORDER || site.y < -TILE_BORDER)
		return;
	if(cell->getCanTouch() == false)
		return;
	auto unit = _units.at(StringUtils::format("%d,%d", (int)site.x, (int)site.y));
	unit->touchEnded(cell);
}

Unit* Floor::getUnit(Vec2 site)
{
	return _units.at(StringUtils::format("%d,%d", (int)site.x, (int)site.y));
}

bool Floor::onTrigger(EventCustom *event)
{
	auto data = static_cast<ValueMap*>(event->getUserData());
	auto effect = (*data)["effect"].asValueMap();
	if((*data)["type"].asString() == "addShade")
	{
		auto fShade = effect["fShade"].asString();
		auto fMask = effect["fMask"].asString();
		Vec2 site(0, 0);
		if(!effect["site"].isNull())
		{
			site = PointFromString(effect["site"].asString());
		}
		Vec2 pos(site.x * TILE_WIDTH + TILE_HALF_WIDTH, site.y * TILE_HEIGHT + TILE_HALF_HEIGHT);
		addShadeAt(fShade, fMask, pos);
	}
	return false;
}

#if DETECT_ACTOR == 1
void Floor::updateFight()
{
	if(Detect::shareDetect()->lookActorOpen == false)
		return;
	auto player = Detect::shareDetect()->getPlayer();
	if(player == nullptr) return;
	for(auto unit:_units)
	{
		for(auto cell:unit.second->getCells())
		{
			auto monster = dynamic_cast<Monster*>(cell);
			if(monster == nullptr) continue;
			// 坚固属性
			if(monster->hasAttrs("jiangu"))
			{
				monster->setDef(player->getStr() - 1);
			}
			std::string win;
			// 伤害计算
			auto player = Detect::shareDetect()->getPlayer();
			auto mhit = monster->getStr() - (monster->hasAttrs("mogong") ? 0 : player->getDef());
			auto phit = player->getStr() - monster->getDef();
			mhit = mhit > 0 ? mhit:0;
			phit = phit > 0 ? phit:0;
			int mround = ((phit == 0) ? 10000000:ceil(monster->getHp() * 1.0 / phit));
			int damage = mhit * (monster->hasAttrs("xiangong")?mround:(mround - 1)) * (monster->hasAttrs("lianji")? 2 : 1)
				- player->getMdef()
				+ (monster->hasAttrs("jinghua") ? player->getMdef() * monster->getAttrData("jinghuaRate").asFloat() : 0) 
				+ (monster->hasAttrs("xixue") ? player->getHp() * monster->getAttrData("xixueRate").asFloat() : 0);
			win = damage > player->getHp() ? "lose" : "win";
			std::string damageStr = phit == 0 ? a2u("over") : cocos2d::Value(damage).asString();
			monster->setDamageLabel(damageStr, win);
		}
	}
}
#endif
Unit* Unit::create(Vec2 vec)
{
	Unit *pRet = new Unit(vec);
	if (pRet)
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}	
}

void Unit::attach(Cell *cell)
{
	// 在碰撞范围内
	if(_site.x < getFloor()->getHorizonTileNum() && _site.x >= 0 && _site.y < getFloor()->getVerticalTileNum() && _site.y >= 0)
	{
		if(cell->getMutual() != -1)
		{
			for(int i = 0; i < _cells.size();i++)
			{
				auto c = _cells.at(i);
				if(c->getMutual() == -1)
					continue;
				// 互相碰撞
				cell->crashOn(c);
				c->crashOn(cell);
				EventHelper::getInstance()->dispatchByStr("target:"+cell->getName()+"|type:crashOn|senderType:"+c->getType()+"|senderName:"+c->getName()+"|senderTypeId:"+cocos2d::Value(c->getTypeId()).asString(),c->getEffect());
				EventHelper::getInstance()->dispatchByStr("target:"+c->getName()+"|type:crashOn|senderType:"+cell->getType()+"|senderName:"+cell->getName()+"|senderTypeId:"+cocos2d::Value(cell->getTypeId()).asString(),cell->getEffect());
			}
		}
	}
	_cells.pushBack(cell);
	cell->setUnit(this);
}

void Unit::detach(Cell *cell)
{
	_cells.eraseObject(cell);
	// 在碰撞范围内
	if(_site.x < getFloor()->getHorizonTileNum() && _site.x >= 0 && _site.y < getFloor()->getVerticalTileNum() && _site.y >= 0)
	{
		if(cell->getMutual() == -1)
			return;
		for(int i = 0; i < _cells.size();i++)
		{
			auto c = _cells.at(i);
			if(c->getMutual() == -1)
				continue;
			// 互相分离
			cell->crashOut(c);
			c->crashOut(cell);
			EventHelper::getInstance()->dispatchByStr("target:"+cell->getName()+"|type:crashOut|senderType:"+c->getType()+"|senderName:"+c->getName()+"|senderTypeId:"+cocos2d::Value(c->getTypeId()).asString(),c->getEffect());
			EventHelper::getInstance()->dispatchByStr("target:"+c->getName()+"|type:crashOut|senderType:"+cell->getType()+"|senderName:"+cell->getName()+"|senderTypeId:"+cocos2d::Value(cell->getTypeId()).asString(),cell->getEffect());
		}
	}
}

void Unit::center(Cell *cell)
{
	// 在碰撞范围内
	if(_site.x < getFloor()->getHorizonTileNum() && _site.x >= 0 && _site.y < getFloor()->getVerticalTileNum() && _site.y >= 0)
	{
		if(cell->getMutual() == -1)
			return;
		for(int i = 0; i < _cells.size();i++)
		{
			auto c = _cells.at(i);
			if(c == cell) continue;
			if(c->getMutual() == -1)
				continue;
			c->stepOn(cell);
			EventHelper::getInstance()->dispatchByStr("target:"+c->getName()+"|type:stepOn|senderType:"+cell->getType()+"|senderName:"+cell->getName()+"|senderTypeId:"+cocos2d::Value(cell->getTypeId()).asString(),cell->getEffect());
		}
	}
}

void Unit::centerOut(Cell *cell)
{
	// 在碰撞范围内
	if(_site.x < getFloor()->getHorizonTileNum() && _site.x >= 0 && _site.y < getFloor()->getVerticalTileNum() && _site.y >= 0)
	{
		if(cell->getMutual() == -1)
			return;
		for(int i = 0; i < _cells.size();i++)
		{
			auto c = _cells.at(i);
			if(c == cell) continue;
			if(c->getMutual() == -1)
				continue;
			c->stepOut(cell);
			EventHelper::getInstance()->dispatchByStr("target:"+c->getName()+"|type:stepOn|senderType:"+cell->getType()+"|senderName:"+cell->getName()+"|senderTypeId:"+cocos2d::Value(cell->getTypeId()).asString(),cell->getEffect());
		}
	}
}

void Unit::touchBegan(Cell *cell)
{
	// 在碰撞范围内
	if(_site.x < getFloor()->getHorizonTileNum() && _site.x >= 0 && _site.y < getFloor()->getVerticalTileNum() && _site.y >= 0)
	{
		if(cell->getMutual() == -1)
			return;
		for(int i = 0; i < _cells.size();i++)
		{
			auto c = _cells.at(i);
			if(c->getMutual() == -1)
				continue;
			if(c->getCanTouch() == false)
				continue;
			c->touchOn(cell);
			cell->touchOn(c);
			EventHelper::getInstance()->dispatchByStr("target:"+c->getName()+"|type:touchOn|senderType:"+cell->getType()+"|senderName:"+cell->getName()+"|senderTypeId:"+cocos2d::Value(cell->getTypeId()).asString(),cell->getEffect());
			EventHelper::getInstance()->dispatchByStr("target:"+cell->getName()+"|type:touchOn|senderType:"+c->getType()+"|senderName:"+c->getName()+"|senderTypeId:"+cocos2d::Value(c->getTypeId()).asString(),c->getEffect());
		}
	}
}

void Unit::touchEnded(Cell *cell)
{
	// 在碰撞范围内
	if(_site.x < getFloor()->getHorizonTileNum() && _site.x >= 0 && _site.y < getFloor()->getVerticalTileNum() && _site.y >= 0)
	{
		if(cell->getMutual() == -1)
			return;
		for(int i = 0; i < _cells.size();i++)
		{
			auto c = _cells.at(i);
			if(c->getMutual() == -1)
				continue;
			if(c->getCanTouch() == false)
				continue;
			c->touchOut(cell);
			cell->touchOut(c);
			EventHelper::getInstance()->dispatchByStr("target:"+c->getName()+"|type:touchOut|senderType:"+cell->getType()+"|senderName:"+cell->getName()+"|senderTypeId:"+cocos2d::Value(cell->getTypeId()).asString(),cell->getEffect());
			EventHelper::getInstance()->dispatchByStr("target:"+cell->getName()+"|type:touchOut|senderType:"+c->getType()+"|senderName:"+c->getName()+"|senderTypeId:"+cocos2d::Value(c->getTypeId()).asString(),c->getEffect());
		}
	}
}

// 
int Unit::isBlock()
{
	int bRet = 0;
	if(_cells.size() == 0)
	{
		return bRet;
	}
	for(auto cell:_cells)
	{
		auto block = cell->isBlock();
		if(block > 0)
		{
			if(bRet < block)
			{
				bRet = block;
			}
		}
	}
	return bRet;
}

Vector<Cell*> Unit::getCells()
{
	return _cells;
}

void Unit::clearCells()
{
	_cells.clear();
}

void Floor::buildMap()
{
	if(_mapBuilded == true)
		return;
	auto weather = _map->getProperty("weather");
	
	if(!weather.isNull())
	{

	}
	
	auto dark = _map->getProperty("dark");
	if(!dark.isNull())
	{
		ValueMap darkMap;
		stringToValueMap(dark.asString(),darkMap);
		_dark->setVisible(true);
		if(!darkMap["color"].isNull())
		{
			auto color = stringToColor4B(darkMap["color"].asString());
			_pLayer->setColor(Color3B(color));
			_pLayer->setOpacity(color.a);
		}
	}
	else
	{
		_dark->setVisible(false);
	}

	auto rain = _map->getProperty("rain");
	if(!rain.isNull())
	{
		ValueMap rainMap;
		stringToValueMap(rain.asString(),rainMap);
		ParticleSystemQuad* rainEmitter = ParticleSystemQuad::create("particles/rain.plist");
		rainEmitter->retain();
		rainEmitter->setName("rain");
		addChild(rainEmitter ,999);
	}

	_light = Layer::create();
	auto lighting = _map->getProperty("lighting");
	if(!lighting.isNull())
	{
		_lightColor = LayerColor::create(Color4B::WHITE);
		_light->addChild(_lightColor);
		_lightColor->setOpacity(0);
		//定时操作，来设置闪电 开关  一调用就说明要产生 一根闪电
		_lightDraw = DrawNode::create();
		_light->addChild(_lightDraw);

		_lightAction = Sequence::create(FadeTo::create(0.0f,230),Blink::create(0.4, 2),CallFunc::create([=](){_lightDraw->clear();}),FadeTo::create(0.5f,0),nullptr);
		_lightAction->retain();

		this->schedule(schedule_selector(Floor::lightUpdate),1.0f);
	}
	addChild(_light,1001);

	/*auto smoke = _map->getProperty("smoke");
	if(!smoke.isNull())
	{
	ValueMap smokeMap;
	stringToValueMap(smoke.asString(),smokeMap);
	ParticleSystemQuad* smokeEmitter = ParticleSystemQuad::create("particles/smoke.plist");
	smokeEmitter->retain();
	smokeEmitter->setName("smoke");
	addChild(smokeEmitter ,10);
	}*/

	/*auto rainDrop = _map->getProperty("rainDrop");
	if(!rainDrop.isNull())
	{
	ValueMap rainDropMap;
	stringToValueMap(rainDrop.asString(),rainDropMap);
	ParticleSystemQuad* rainDropEmitter = ParticleSystemQuad::create("particles/rainDrop.plist");
	rainDropEmitter->retain();
	rainDropEmitter->setName("rainDrop");
	addChild(rainDropEmitter ,10);
	}*/

	/*auto fog = _map->getProperty("fog");
	if(!fog.isNull())
	{
	ValueMap fogMap;
	stringToValueMap(fog.asString(),fogMap);
	ParticleSystemQuad* fogEmitter = ParticleSystemQuad::create("particles/fog.plist");
	fogEmitter->retain();
	fogEmitter->setName("fog");
	addChild(fogEmitter ,998);
	}*/
	//3D 波浪效果
	//震动快慢, ,波浪数,波浪振幅
	//static CCWaves3D* create(float duration, const CCSize& gridSize, unsigned int waves, float amplitude);
	//CCWaves3D* action = CCWaves3D::create(5.0f, CCSizeMake(5,10), 5, 5);

	//this->runAction(CCRepeatForever::create(action));
	//_emitter->setTexture( Director::getInstance()->getTextureCache()->addImage("image/fire.png") );

	/*auto s = Director::getInstance()->getWinSize();
	if (_emitter != nullptr)
	{
		_emitter->setPosition( Vec2(s.width / 2, s.height / 2) );
	}
*/

	// 动画
	//schedule(schedule_selector(Floor::updateMap), 0.2f);
	//scheduleUpdate();

	

	ssize_t count = _map->getChildrenCount();
	auto size = _map->getMapSize();

	for(int i = 0; i < count;i++)
	{
		TMXLayer* layer = dynamic_cast<TMXLayer*>(_map->getChildByTag(i));
		if(layer == nullptr) continue;
		for(int i = 0; i < size.width; i++)
		{
			for(int j = 0; j < size.height; j++)
			{					
				auto gid = layer->getTileGIDAt(Vec2(i,j));
				if(gid > 0)
				{
					auto value = _map0->getPropertiesForGID(gid);
					if(!value.isNull())
					{
						auto type = value.asValueMap()["type"].asString();
						auto id = value.asValueMap()["id"].asInt();

						auto name = value.asValueMap()["name"].asString();
						auto anis = value.asValueMap()["anis"].asString();
						auto duration = value.asValueMap()["duration"].asFloat();
						if(duration == 0) duration = 0.35;
						if(anis != "")
						{
							auto sprite = layer->getTileAt(Vec2(i,j));
							ValueVector temp;
							ValueVector anisVec;
							stringToValueVector(anis,",",temp);
							for(auto ani:temp)
							{
								anisVec.push_back(cocos2d::Value(int(gid + ani.asInt())));
							}
							sprite->runAction(RepeatForever::create(AnimateByGID::create(anisVec,layer,Vec2(i,j),duration)));
						}
					}
				}
			}
		}
	}

	auto scale = _map->getProperty("scale");
	if(!scale.isNull())
	{
		auto scaleNum = scale.asFloat();
		_mapScale = scaleNum;
		this->setScale(_mapScale);
	}

	// 抗锯齿
	SpriteBatchNode* child = nullptr;
	auto& children = _map->getChildren();
	for(const auto &node : children) 
	{
		child = dynamic_cast<SpriteBatchNode*>(node);
		if(child == nullptr)
			continue;
		child->getTexture()->setAliasTexParameters();
	}

	_mapBuilded = true;
}

void Floor::stopWeather()
{
	this->unschedule(schedule_selector(Floor::lightUpdate));

	auto node1 = this->getChildByName("fog");
	if(node1 != nullptr) removeChild(node1);

	auto node2 = this->getChildByName("rainDrop");
	if(node2 != nullptr) removeChild(node2);

	auto node3 = this->getChildByName("smoke");
	if(node3 != nullptr) removeChild(node3);

	auto node4 = this->getChildByName("rain");
	if(node4 != nullptr) removeChild(node4);

	_pLayer->setOpacity(0);
}

Cell* Floor::getNearByTypeId(Vec2 center,const Vec2* vecs, int len1,const int* typeIds, int len2)
{
	for(int i = 0; i < len1; i++)
	{
		auto site = center + vecs[i];
		if(site.x >= _horizonTileNum || site.y >= _verticalTileNum || site.x < 0 || site.y < 0)
			continue;
		auto cells = getUnit(site)->getCells();
		for(auto cell:cells)
		{
			for(int j = 0; j < len2; j++)
			{
				if(cell->getTypeId() == typeIds[j])
					return cell;
			}
		}
	}
	return nullptr;
}

Cell* Floor::getNearByType(Vec2 center, const Vec2* vecs, int len1,const std::string* types, int len2)
{
	for(int i = 0; i < len1; i++)
	{
		auto site = center + vecs[i];
		if(site.x >= _horizonTileNum || site.y >= _verticalTileNum || site.x < 0 || site.y < 0)
			continue;
		auto cells = getUnit(site)->getCells();
		for(auto cell:cells)
		{
			for(int j = 0; j < len2; j++)
			{
				if(cell->getType() == types[j])
					return cell;
			}
		}
	}
	return nullptr;
}

Path* Floor::getPathByName(std::string name)
{
	return _paths.at(name);
}

bool Floor::cellIsNearByCell(Cell* one, Cell* two, const Vec2* vecs, int len)
{
	for(int i = 0; i < len; i++)
	{
		auto site = two->getSite() + vecs[i];
		if(site.x >= _horizonTileNum || site.y >= _verticalTileNum || site.x < 0 || site.y < 0)
			continue;
		auto cells = getUnit(site)->getCells();
		if(cells.find(one) != cells.end())
			return true;
	}
	return false;
}