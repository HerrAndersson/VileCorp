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

	std::vector<PlayerInfo> _playerProfile;
	std::vector<string> _playerProfilesPath;

	// Temp, should be replaced with blueprint
	struct ToPlace
	{
		ToPlace()
		{
			_type = TRAP;
			_subType = 0;
			_name = "trap_proto";
			ResetTemps();
		}
		void ResetTemps()
		{
			_goldCost = -1;
			_blueprintID = 1;
		}
		Type _type;
		int _subType;
		std::string _name;
		int _goldCost;
		int _blueprintID;
	} _toPlace;

	void EvaluateGoldCost();

public:
	PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::Settings* settings, System::SoundModule* soundModule);
	~PlacementState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();

	void HandleInput();
	void HandleButtons();
};

