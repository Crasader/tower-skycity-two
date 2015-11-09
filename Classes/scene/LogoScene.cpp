#include "scene/LogoScene.h"
#include "UI/LogoPanel.h"

USING_NS_CC;

bool LogoScene::init()
{
	bool bRet = false;
	do{
		setName("logoScene");
		_logoPanel = LogoPanel::create("logo");
		addChild(_logoPanel);
		bRet = true;
	} while (0);
	return bRet;
}