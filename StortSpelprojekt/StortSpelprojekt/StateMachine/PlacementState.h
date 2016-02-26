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
	bool _trapChosen = false;
	
	int _budget;
	int _costOfAnvilTrap;
	int _costOfTeslaCoil;
	int _costOfCamera;
	int _costOfGuard;

	int _currentPlayer = 0;
	DirectX::XMFLOAT3* _ambientLight;

	std::vector<PlayerInfo> _playerProfile;
	std::vector<string> _playerProfilesPath;

	// Temp, should be replaced with blueprint
	ToPlace _toPlace;

	GUI::NodeBar _unitBar;

	void EvaluateGoldCost();
	void HandleDescriptions();

public:
	PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule, DirectX::XMFLOAT3* ambientLight);
	~PlacementState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();

	void HandleInput();
	void HandleButtons();
};

