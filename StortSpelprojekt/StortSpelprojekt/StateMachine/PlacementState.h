#pragma once

#include "BaseState.h"
#include "../JsonStructs.h"
#include "jsonparser.h"
#include "Assetmanager.h"
#include "../Player.h"
#include "../TutorialLogic.h"
#include "CommonUtils.h"
#include "../GhostImage.h"
#include "../System/Settings/Profile.h"

class PlacementState : public BaseState
{
private:
	TutorialLogic* _tutorialLogic;
	bool _trapChosen = false;
	
	int _budget;
	int _costOfAnvilTrap;
	int _costOfTeslaCoil;
	int _costOfCamera;
	int _costOfGuard;

	int _currentPlayer = 0;
	DirectX::XMFLOAT3* _ambientLight;

	System::Profile* _profile;

	Player* _player;
	System::SpecificBlueprint _selectedBlueprint;
	std::vector<GUI::Node*>* _buttons;
	GhostImage _ghostImage;

	void EvaluateGoldCost();
	void HandleDescriptions();

public:
	PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule, DirectX::XMFLOAT3* ambientLight);
	~PlacementState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();

	void HandleInput();
};

