#pragma once

#include "BaseState.h"
#include "LevelEdit.h"

class PlacementState : public BaseState
{
private:
	LevelEdit _levelEdit;
	int _budget;
	bool _trapChosen = false;

public:
	PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height);
	~PlacementState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};

