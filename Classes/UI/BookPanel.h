#ifndef __BOOK_PANEL_H__
#define __BOOK_PANEL_H__

#include "UI/SimplePanel.h"
class Actor;
class Cell;
class Monster;

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class BookLine : public Ref
{
public:
	static BookLine* create(Monster* monster, Layout* layout);
	BookLine(Monster* monster, Layout* layout);
	bool init();
	CC_SYNTHESIZE(Monster*, _monster, Monster);
};

class BookPanel : public SimplePanel
{	
public:
	enum buttons
	{
		EXIT = 1
	};
	BookPanel(std::string name):SimplePanel(name){}
	static BookPanel* create(std::string name);
	bool init();
	void initWithMonsters(Map<int, Cell*> map);
	CC_SYNTHESIZE(float, _gapV, GapV);
private:
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
private:
	Vector<BookLine*> _bookLines;
	Layout* _panelLine;
	ListView* _listViewLines;
	void selectedItemEvent(Ref* pSender, ListView::EventType type);
	void selectedItemEventScrollView(Ref* pSender, ui::ScrollView::EventType type);
};


#endif /*__BOOK_PANEL_H__*/