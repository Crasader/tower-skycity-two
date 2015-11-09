#include "BookPanel.h"
#include "PopPanel.h"
#include "Detect.h"
#include "utils/RichTextCreator.h"
#include "UI/MonsterInfoPanel.h"
#include "model/Model.h"
#include "cell/Cell.h"
#include "cell/actor/Actor.h"
#include "map/Floor.h"

#define NONE_IMAGE "none.png"

BookLine* BookLine::create(Monster* monster, Layout* layout)
{
	BookLine *pRet = new BookLine(monster,layout);
	if (pRet && pRet->init())
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

BookLine::BookLine(Monster* monster, Layout* layout)
{
	_monster = monster;

	auto image = static_cast<ImageView*>(Helper::seekWidgetByName(layout, "Image_image"));
	auto labelName = static_cast<Text*>(Helper::seekWidgetByName(layout, "Label_name"));
	auto labelHp = static_cast<Text*>(Helper::seekWidgetByName(layout, "Label_hp"));
	auto panelAttrs = static_cast<Layout*>(Helper::seekWidgetByName(layout, "Panel_attrs"));
	auto labelStr = static_cast<Text*>(Helper::seekWidgetByName(layout, "Label_str"));
	auto labelDef = static_cast<Text*>(Helper::seekWidgetByName(layout, "Label_def"));
	auto labelXp = static_cast<Text*>(Helper::seekWidgetByName(layout, "Label_xp"));
	auto labelGold = static_cast<Text*>(Helper::seekWidgetByName(layout, "Label_gold"));
	auto labelDamage = static_cast<Text*>(Helper::seekWidgetByName(layout, "Label_damage"));

	image->loadTexture(monster->getImageUrl(),Widget::TextureResType::PLIST);
	labelName->setString(monster->getNickName());

	
	std::string attrStr;
	auto attrs = monster->getAttrs();
	auto attrCsv = Model::shareModel()->attrCsv;
	for(auto attr:attrs)
	{
		auto attrModel = attrCsv->getModelByAssignTypeAndValue("type", cocos2d::Value(attr));
		auto nickName = attrModel["nickName"].asString();
		auto color = attrModel["color"].asString();
		attrStr += "type:Text---text:" + nickName + "|color:" + color + "|fontSize:16###";
	}
	// É¾µôÄ©Î²µÄ###
	if(attrStr.size() != 0)
	{
		attrStr.pop_back();
		attrStr.pop_back();
		attrStr.pop_back();
	}
	auto labelAttr = createRichTextWithStr(attrStr);
	panelAttrs->addChild(labelAttr);

	labelHp->setString(cocos2d::Value(monster->getHp()).asString());
	labelStr->setString(cocos2d::Value(monster->getStr()).asString());
	labelDef->setString(cocos2d::Value(monster->getDef()).asString());
	labelGold->setString(cocos2d::Value(monster->getGold()).asString());
	labelXp->setString(cocos2d::Value(monster->getXp()).asString());

	// ÉËº¦¼ÆËã
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
	std::string damageStr = phit == 0 ? a2u("XXX") : cocos2d::Value(damage).asString();
	labelDamage->setString(cocos2d::Value(damageStr).asString());
}

bool BookLine::init()
{
	return true;
}


BookPanel* BookPanel::create(std::string name)
{
	BookPanel *pRet = new BookPanel(name);
	if (pRet && pRet->init())
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

bool BookPanel::init()
{
	_gapV = 0;

	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(BookPanel::onButtonClicked, this));
	}
	_listViewLines = static_cast<ListView*>(Helper::seekWidgetByName(_root, "ListView_lines"));

	_panelLine = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_line"));
	_panelLine->retain();
	_panelLine->removeFromParent();

	_listViewLines->addEventListener((ui::ListView::ccListViewCallback)CC_CALLBACK_2(BookPanel::selectedItemEvent, this));
	_listViewLines->addEventListener((ui::ListView::ccScrollViewCallback)CC_CALLBACK_2(BookPanel::selectedItemEventScrollView,this));

	auto floor = static_cast<TextAtlas*>(Helper::seekWidgetByName(_root, "AtlasLabel_floor"));
	floor->setString(cocos2d::Value(Detect::shareDetect()->getCurFloor()->getId()).asString());

	auto actors = Detect::shareDetect()->getCurFloor()->getAllActorType();
	auto ite = actors.begin();
	for(;ite != actors.end();)
	{
		if((*ite).second->getModelByName("inBook").asInt() == 1)
		{
			ite = actors.erase(ite);
		}
		else
			ite++;
	}
	actors.erase(Detect::shareDetect()->getPlayer()->getTypeId());
	initWithMonsters(actors);

	return true;
}

void BookPanel::selectedItemEvent(Ref *pSender, ListView::EventType type)
{
	switch (type)
	{
	case cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_START:
		{
			ListView* listView = static_cast<ListView*>(pSender);
			CC_UNUSED_PARAM(listView);
			CCLOG("select child start index = %ld", listView->getCurSelectedIndex());
			//auto tag = listView->getItem(listView->getCurSelectedIndex())->getTag();
			//auto endStatUnit = _endStatsUnit.at(listView->getCurSelectedIndex());
			break;
		}
	case cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_END:
		{
			ListView* listView = static_cast<ListView*>(pSender);
			CC_UNUSED_PARAM(listView);
			CCLOG("select child end index = %ld", listView->getCurSelectedIndex());

			auto tag = listView->getCurSelectedIndex();
			auto bookline = _bookLines.at(tag);
			auto monsterInfoPan = MonsterInfoPanel::create("monsterInfo");
			monsterInfoPan->initWithMonster(bookline->getMonster());

			PopPanel::getInstance()->addPanel(monsterInfoPan, 2);
			break;
		}
	default:
		break;
	}
}

void BookPanel::selectedItemEventScrollView(Ref* pSender, ui::ScrollView::EventType type)
{
	switch (type) {
	case ui::ScrollView::EventType::SCROLL_TO_BOTTOM:
		CCLOG("SCROLL_TO_BOTTOM");
		break;
	case ui::ScrollView::EventType::SCROLL_TO_TOP:
		CCLOG("SCROLL_TO_TOP");
		break;
	default:
		break;
	}
}

bool sortActor(Cell* t1, Cell* t2)
{
	return t1->getTypeId() < t2->getTypeId();
}

void BookPanel::initWithMonsters(Map<int, Cell*> map)
{
	Vector<Cell*> actors;
	for(auto pair:map)
	{
		actors.pushBack(pair.second);
	}
	
	std::sort(actors.begin(),actors.end(),sortActor);

	int i = 0;
	for(auto cell:actors)
	{
		auto actor = dynamic_cast<Monster*>(cell);
		if(actor == nullptr) continue;
		BookLine *bookLine;
		auto panelLine = i == 0?_panelLine:static_cast<Layout*>(_panelLine->clone());

		bookLine = BookLine::create(actor, panelLine);
		panelLine->setTag(i);
		_bookLines.pushBack(bookLine);
		_listViewLines->pushBackCustomItem(panelLine);
		i++;
	}
}

void BookPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED)
	{
		auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
		switch(tag)
		{
		case EXIT:
			{
				destroy();
			}
			break;
		}

		//CC_SAFE_RETAIN(this); 
		//this->removeFromParentAndCleanup(true);
	}
}