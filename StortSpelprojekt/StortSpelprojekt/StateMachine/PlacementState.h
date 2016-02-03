#pragma once

#include "BaseState.h"
#include "BaseEdit.h"

class PlacementState : public BaseState
{
private:
	BaseEdit _baseEdit;
	bool _trapChosen = false;
	int _budget;

public:
	PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::Settings* settings);
	~PlacementState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};

