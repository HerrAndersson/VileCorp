#pragma once

#include "BaseEdit.h"
#include "Controls.h"
#include "UITree.h"
#include "AssetManager.h"
#include "FontWrapper.h"
#include "SettingsReader.h"
#include "CommonUtils.h"

enum TutorialState
{
	NOTUTORIAL,
	OLDTUTORIAL,
	NEWTUTORIAL
};

class TutorialLogic
{
private:
	enum TutorialStage
	{
		WELCOME,
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

private:
	GUI::UITree* _uiTree;
	System::Controls* _controls;
	TutorialStage _currentStage;
	int _gold;
	bool _sCameraPlaced;
	bool _tutorialCompleted;
	float _time;
	bool _light;

public:
	TutorialLogic(GUI::UITree* uiTree, System::Controls* controls);
	~TutorialLogic();

	bool Update(float deltaTime, PlayerInfo playerProfile);
	void OnStateEnter();
	void ResetUiTree();
	bool IsTutorialCompleted()const;
};