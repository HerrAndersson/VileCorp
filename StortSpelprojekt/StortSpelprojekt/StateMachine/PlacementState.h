#pragma once

#include "BaseState.h"
#include "BaseEdit.h"
#include "../JsonStructs.h"
#include "jsonparser.h"
#include "Assetmanager.h"

class PlacementState : public BaseState
{
private:
	BaseEdit _baseEdit;
	bool _trapChosen = false;
	int _budget;

	int _currentPlayer = 0;

	std::vector<PlayerInfo> _playerProfile;
	std::vector<string> _playerProfilesPath;

	// Temp, should be replaced with blueprint
	struct ToPlace
	{
		ToPlace()
		{
			_type = TRAP;
			_name = "trap_proto";
		}
		Type _type;
		std::string _name;
	} _toPlace;

public:
	PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::Settings* settings, System::SoundModule* soundModule);
	~PlacementState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();

	void HandleInput();
	void HandleButtons();
};

