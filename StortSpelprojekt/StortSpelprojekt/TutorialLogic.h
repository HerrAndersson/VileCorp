#pragma once

#include "BaseEdit.h"
#include "Controls.h"
#include "UITree.h"
#include "AssetManager.h"
#include "FontWrapper.h"
#include "SettingsReader.h"
#include "ToPlace.h"
#include "CommonUtils.h"

enum TutorialStage
{
	NOTUTORIAL,
	NEWTUTORIAL,
	OBJECTIVE,
	CONTROLS,
	GUARDEXPLAINED,
	GUARDPLACE,
	ANVILEXPLAINED,
	ANVILPLACE,
	TESLAEXPLAINED,
	TESLAPLACE,
	SECURITYCAMERAEXPLAINED,
	SECURITYCAMERAPLACE,
	BUDGETEXPLAINED,
	PLAYEXPLAINED
};

class TutorialLogic
{
private:
	

private:
	GUI::UITree* _uiTree;
	System::Controls* _controls;
	TutorialStage _currentStage;
	int _gold;
	bool _sCameraPlaced;
	bool _tutorialCompleted;
	

public:
	TutorialLogic(GUI::UITree* uiTree, System::Controls* controls);
	~TutorialLogic();

	bool Update(float deltaTime, BaseEdit* baseEdit, ToPlace& toPlace, PlayerInfo playerProfile);
	void OnStateEnter();
	void ResetUiTree();
	bool IsTutorialCompleted()const;
};