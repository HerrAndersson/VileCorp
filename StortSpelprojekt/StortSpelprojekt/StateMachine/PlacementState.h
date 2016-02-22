#pragma once

#include "BaseState.h"
#include "../BaseEdit.h"
#include "../JsonStructs.h"
#include "jsonparser.h"
#include "Assetmanager.h"

class PlacementState : public BaseState
{
private:
	BaseEdit* _baseEdit;
	bool _trapChosen = false;
	int _budget;

	int _currentPlayer = 0;
	DirectX::XMFLOAT3* _ambientLight;

	std::vector<PlayerInfo> _playerProfile;
	std::vector<string> _playerProfilesPath;

	// Temp, should be replaced with blueprint
	struct ToPlace
	{
		ToPlace()
		{
			ResetTemps();
		}
		void ResetTemps()
		{
			_goldCost = -1;
			_markerID = -1;
		}
		SpecificBlueprint _sB;
		int _goldCost;
		int _markerID;
	} _toPlace;

	void EvaluateGoldCost();

public:
	PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule, DirectX::XMFLOAT3* ambientLight);
	~PlacementState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();

	void HandleInput();
	void HandleButtons();
};

