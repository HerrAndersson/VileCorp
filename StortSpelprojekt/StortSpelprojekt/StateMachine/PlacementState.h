#pragma once

#include "BaseState.h"
#include "../BaseEdit.h"
#include "../JsonStructs.h"
#include "jsonparser.h"
#include "Assetmanager.h"
#include "../ToPlace.h"

#include "../Player.h"
#include "CommonUtils.h"
#include "../System/Settings/Profile.h"

class PlacementState : public BaseState
{
protected:
	BaseEdit* _baseEdit;
	
	int _budget;
	int _costOfAnvilTrap;
	int _costOfTeslaCoil;
	int _costOfCamera;
	int _costOfGuard;
	//TODO: Remove all _costOfX --Victor

	DirectX::XMFLOAT3* _ambientLight;

	System::Profile* _profile;

	ToPlace _toPlace;

	//GUI::NodeBar _unitBar;
	std::vector<GUI::Node*>* _buttons;

	void EvaluateGoldCost();
	void HandleDescriptions();

public:
	PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule, DirectX::XMFLOAT3* ambientLight);
	virtual ~PlacementState();

	virtual void Update(float deltaTime);
	virtual void OnStateEnter();
	virtual void OnStateExit();

	void HandleInput();
	void HandleButtons();
};

