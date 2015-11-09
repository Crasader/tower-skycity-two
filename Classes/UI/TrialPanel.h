#ifndef __TRIAL_PANEL_H__
#define __TRIAL_PANEL_H__

#include "UI/SimplePanel.h"
#include "UI/EndStatPanel.h"
#include "cell/Cell.h"
#include "map/Trial.h"


USING_NS_CC;
using namespace cocostudio;
using namespace ui;

class TrialPanel : public SimplePanel
{	
public:
	enum buttons
	{
		BEGIN = 1
	};
	TrialPanel(std::string name):SimplePanel(name){}
	static TrialPanel* create(std::string name);
	bool init();
	void initWithTrial(Trial* trial);
private:
	//Vector<EndStatPanel*> _endStatPanels;
	Trial* _trial;
	ListView* _listView;
	Layout* _achieveUnit;
	std::vector<ValueMap> _model;
	virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void selectedItemEvent(Ref* pSender, ListView::EventType type);
	void selectedItemEventScrollView(Ref* pSender, ui::ScrollView::EventType type);
};


#endif /*__TRIAL_PANEL_H__*/