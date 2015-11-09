#include "PausePanel.h"
#include "UI/FilePanel.h"
#include "UI/ConfigPanel.h"
#include "scene/StartScene.h"
#include "PopPanel.h"
#include "Detect.h"
#include "utils/PlotScript.h"
#include "scene/GameScene.h"
#include "ui/OptionPanel.h"
#include "utils/StringHandler.h"

#define NONE_IMAGE "none.png"

PausePanel* PausePanel::create(std::string name)
{
	PausePanel *pRet = new PausePanel(name);
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

bool PausePanel::init()
{
	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(PausePanel::onButtonClicked, this));
	}
	return true;
}

void PausePanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
		switch(tag)
		{
		case CONTINUE:
			{
				destroy();
			}			
			break;
		case SAVE:
			{
				destroy();
				/*if(PlotScript::sharedHD()->delayCount == 0)
				{
					auto detect = Detect::shareDetect();
					auto map = detect->saveModel();
					auto path = FileUtils::sharedFileUtils()->getWritablePath();
					FileUtils::getInstance()->writeToFile(map,path+"save1.plist");
				}
				else
				{
					PopPanel::getInstance()->note("fileNot",a2u("�������޷��浵"));
				}*/
				auto filePanel = FilePanel::create("file");
				filePanel->setType(FilePanel::SAVE);
				PopPanel::getInstance()->addPanel(filePanel,1);
			}
			break;
		case ABOUT:
			{
				destroy();
				auto filePanel = FilePanel::create("file");
				filePanel->setType(FilePanel::LOAD);
				PopPanel::getInstance()->addPanel(filePanel,1);
			}
			break;
		case BACK:
			{
				destroy();

				PopPanel::getInstance()->confirm("back","type:Text---text:"+ a2u("������Ϸ֮ǰȷ��Ҫ���ر�����棿"),PromptPanel::TYPE_DONE_CANCEL,[=](){
					auto gameScene = dynamic_cast<GameScene*>(Director::getInstance()->getRunningScene());
					gameScene->resetInstance();
					auto scene = StartScene::create();
					Director::getInstance()->replaceScene(scene);
				},nullptr,true,true);
				//Detect::shareDetect()->refreshCells();
				/*Detect::shareDetect()->refreshPlayer();
				Detect::shareDetect()->getWorld()->refreshMap();*/

				//Model::shareModel()->refresh();
			}
			break;
		case OPTION:
			{
				destroy();
				auto optionPanel = OptionPanel::create("option");
				PopPanel::getInstance()->addPanel(optionPanel,1);
			}
			break;
		}
		//CC_SAFE_RETAIN(this); 
		//this->removeFromParentAndCleanup(true);
	}
}