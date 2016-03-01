#pragma once

#include "BaseEdit.h"
#include "Controls.h"
#include "UITree.h"
#include "AssetManager.h"
#include "FontWrapper.h"
#include "SettingsReader.h"
#include "CommonUtils.h"
#include "Player.h"
//#include "GhostImage.h"
#include "ObjectHandler.h"
#include "../System/Settings/Profile.h"

//Since subtypes are so complicated to access
//We define those that we are going to use
//Ugly I know. But fine due to the circumstances imo. -Alex
#define SUBTYPEGUARD 0
#define SUBTYPESPIKE 0
#define SUBTYPETESLACOIL 1
#define SUBTYPESECURITYCAMERA 0

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
	
	BaseEdit* _baseEdit;
	
	
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
	bool _objectPlaced;

private:
	void InputButtons(System::Type type, int subType);
	void InputButtons();

public:
	TutorialLogic(GUI::UITree* uiTree, System::Controls* controls, Player* player, std::vector<GUI::Node*>* buttons, BaseEdit* baseEdit, ObjectHandler* objectHandler, PickingDevice* pickingDevice, System::Profile* profile);
	~TutorialLogic();

	bool Update(float deltaTime);
	void OnStateEnter();
	void ResetUiTree();
	bool IsTutorialCompleted()const;
};