#ifndef __MONSTER_INFO_PANEL_H__
#define __MONSTER_INFO_PANEL_H__

#include "UI/SimplePanel.h"

class Monster;

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class MonsterInfoPanel : public SimplePanel
{	
public:
	enum buttons
	{
	};
	MonsterInfoPanel(std::string name):SimplePanel(name){}
	static MonsterInfoPanel* create(std::string name);
	bool init();
	void initWithMonster(Monster* monster);
private:
	Text* _labelHp;
	Text* _labelStr;
	Text* _labelDef;
	Text* _labelGold;
	Text* _labelXp;
	Text* _labelHoeNum;
	Text* _labelBombNum;
	Text* _labelTime;
	Text* _labelEndTag;
	virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
};


#endif /*__MONSTER_INFO_PANEL_H__*/