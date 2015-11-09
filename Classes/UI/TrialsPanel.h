#ifndef __TRIALS_PANEL_H__
#define __TRIALS_PANEL_H__

#include "UI/SimplePanel.h"

class Cell;
class Trial;

USING_NS_CC;
using namespace cocostudio;
using namespace ui;

class TrialsPanel : public SimplePanel
{	
public:
	enum buttons
	{
		EXIT = 1
	};
	TrialsPanel(std::string name):SimplePanel(name){}
	static TrialsPanel* create(std::string name);
	bool init();
	void initWithTrials(Map<int, Trial*> trialss, int type);
private:
	Vector<Trial*> _trials;
	ListView* _listView;
	Text* _endNumLab;
	Layout* _trialUnit;
	std::vector<ValueMap> _model;
	virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void selectedItemEvent(Ref* pSender, ListView::EventType type);
	void selectedItemEventScrollView(Ref* pSender, ui::ScrollView::EventType type);
};


#endif /*__TRIALS_PANEL_H__*/