#ifndef __POP_PANEL_H__
#define __POP_PANEL_H__

#include "cocos2d.h"
#include "SimplePanel.h"
#include "ChatPanel.h"
#include "Prompt.h"
USING_NS_CC;

class PopPanel:public Layer
{
public:
	PopPanel();
	~PopPanel();
	static PopPanel* getInstance();
	void onEnter();
	void onExit();
	// order: 0:�Ի��� 1:�������� 2:ȷ����ȡ�� 3:������ʾ
	void addPanel(Layer* panel, int order);
	// order: 0:�Ի��� 1:�������� 2:ȷ����ȡ�� 3:������ʾ
	SimplePanel* addPanelByName(std::string name, int order);
	Node* hasPanelByName(std::string name);
	bool hasPanel(Layer* panel);
	void removePanel(Layer* panel);
	void removePanelByName(std::string name);
	void removeAllPanelsByOrder(int order);
	void check();
	void reset();

	void confirm(std::string name, std::string content, PromptPanel::buttonType type = PromptPanel::buttonType::TYPE_DONE_CANCEL,std::function<void()> onDone = nullptr,std::function<void()> onCancel = nullptr, bool single = true, bool center = false);

	void note(std::string name, std::string content, float delay = 1.0f, bool single = false, bool center = false);

	ChatPanel* chatPanel;

};
#endif /*__POP_PANEL_H__*/