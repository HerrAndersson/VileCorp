#pragma once

#include "BaseEdit.h"
#include "Controls.h"
#include "UITree.h"
#include "AssetManager.h"
#include "FontWrapper.h"
#include "SettingsReader.h"
#include "CommonUtils.h"
#include "Player.h"
#include "GhostImage.h"
#include "ObjectHandler.h"
#include "../System/Settings/Profile.h"

#define BUTTONGUARD 0
#define BUTTON 1

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
	Player* _player;
	std::vector<GUI::Node*>* _buttons;
	GhostImage*	_ghostImage;
	ObjectHandler* _objectHandler;
	PickingDevice* _pickingDevice;
	System::Profile* _profile;
	
	System::SpecificBlueprint _selectedBlueprint;
	TutorialStage _currentStage;
	int _budget;
	bool _sCameraPlaced;
	bool _tutorialCompleted;
	float _time;
	bool _light;

private:
	void InputButtons(int type);

public:
	TutorialLogic(GUI::UITree* uiTree, System::Controls* controls, Player* player, std::vector<GUI::Node*>* buttons, GhostImage* ghostImage, ObjectHandler* objectHandler, PickingDevice* pickingDevice, System::Profile* profile);
	~TutorialLogic();

	bool Update(float deltaTime);
	void OnStateEnter();
	void ResetUiTree();
	bool IsTutorialCompleted()const;
};