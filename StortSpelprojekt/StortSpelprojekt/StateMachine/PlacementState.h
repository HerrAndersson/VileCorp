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
	PlacementState(System::Controls* controls, ObjectHandler* objectHandler, UIHandler* uiHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice);
	~PlacementState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};

