#pragma once

#include "BaseState.h"
#include "../JsonStructs.h"
#include "jsonparser.h"
#include "Assetmanager.h"
#include "../Player.h"
#include "../TutorialLogic.h"

struct SpecificBlueprint
{
	SpecificBlueprint()
	{
		_blueprint = nullptr;
		_textureId = -1;
	}
	void Reset()
	{
		_blueprint = nullptr;
		_textureId = -1;
	}
	Blueprint* _blueprint = nullptr;
	int _textureId = -1;
};

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

	std::vector<PlayerInfo> _playerProfile;
	std::vector<string> _playerProfilesPath;

	Player* _player;
	SpecificBlueprint _selectedBlueprint;

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

