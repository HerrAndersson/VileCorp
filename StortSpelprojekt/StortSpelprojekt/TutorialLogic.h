#pragma once

#include "BaseEdit.h"
#include "Controls.h"
#include "UITree.h"
#include "AssetManager.h"
#include "FontWrapper.h"
#include "SettingsReader.h"
#include "ToPlace.h"
#include "CommonUtils.h"

class TutorialLogic
{
private:
	enum Stage
	{
		WELCOME,
		OBJECTIVE,
		CONTROLS,
		GUARD,
		GUARDPLACE,
		ANVIL,
		ANVILPLACE,
		TESLA,
		TESLAPLACE,
		SECURITYCAMERA,
		SECURITYCAMERAPLACE,
		BUDGETEXPLAINED,
		PLAYEXPLAINED
	};

private:
	GUI::UITree* _uiTree;
	System::Controls* _controls;
	Stage _currentStage;
	int _gold;
	void ResetUiTree();

public:
	TutorialLogic(GUI::UITree* uiTree, System::Controls* controls);
	~TutorialLogic();

	bool Update(float deltaTime, BaseEdit* baseEdit, ToPlace& toPlace);
	void OnStateEnter();
};