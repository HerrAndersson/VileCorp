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
#include "../AmbientLight.h"

class PlacementState : public BaseState
{
private:
	std::vector<short> _informationOverlayIDs;
protected:
	BaseEdit* _baseEdit;
	int _budget;
	AmbientLight* _ambientLight;
	System::Profile* _profile;
	ToPlace _toPlace;

	//GUI::NodeBar _unitBar;
	std::vector<GUI::Node*>* _buttons;

	void EvaluateGoldCost();
	void HandleDescriptions();

	void AddInformationOverlay();
	void RemoveInformationOverlay();

public:
	PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule, AmbientLight* ambientLight);
	virtual ~PlacementState();

	virtual void Update(float deltaTime);
	virtual void OnStateEnter();
	virtual void OnStateExit();

	void HandleInput();
	virtual void HandleButtons();

	
};

