#pragma once

#include "BaseState.h"
#include "../BaseEdit.h"
#include "../JsonStructs.h"
#include "jsonparser.h"
#include "Assetmanager.h"
#include "../TutorialLogic.h"

#include "../ToPlace.h"

class PlacementState : public BaseState
{
private:
	TutorialLogic* _tutorialLogic;
	BaseEdit* _baseEdit;
	UnitsInfo* unitsInfo;
	bool _trapChosen = false;

	int _costOfAnvilTrap;
	int _costOfTeslaCoil;
	int _costOfCamera;
	int _costOfGuard;

	int _currentPlayer = 0;

	std::vector<PlayerInfo> _playerProfile;
	std::vector<string> _playerProfilesPath;

	// Temp, should be replaced with blueprint
	ToPlace _toPlace;

	void EvaluateGoldCost();

public:
	PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule);
	~PlacementState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();

	void HandleInput();
	void HandleButtons();
	void HandleDescriptions();
};

