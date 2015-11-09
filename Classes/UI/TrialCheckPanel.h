#ifndef __TRIALCHECK_PANEL_H__
#define __TRIALCHECK_PANEL_H__

#include "UI/SimplePanel.h"
#include "UI/EndStatPanel.h"
#include "cell/Cell.h"
#include "map/Trial.h"


USING_NS_CC;
using namespace cocostudio;
using namespace ui;

class TrialCheckPanel : public SimplePanel
{	
public:
	enum buttons
	{
		RESET = 1,
		BACK
	};
	~TrialCheckPanel();
	TrialCheckPanel(std::string name):SimplePanel(name){}
	static TrialCheckPanel* create(std::string name);
	bool init();
	void initWithTrial(Trial* trial, bool last);
private:
	ListView* _listView;
	Layout* _trialUnit;
	Trial* _trial;
	Button* _reset;
	Button* _back;
	Text* _timeLab;
	void timeUpdate(float delay);
	virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void selectedItemEvent(Ref* pSender, ListView::EventType type);
	void selectedItemEventScrollView(Ref* pSender, ui::ScrollView::EventType type);
};


#endif /*__TRIALCHECK_PANEL_H__*/