#pragma once

#include "BaseState.h"
#include "LevelEdit.h"

class LevelEditState : public BaseState
{
private:
	LevelEdit _levelEdit;


	bool _floorChosen = false;
	bool _wallChosen = false;

public:
	LevelEditState(System::Controls* controls, ObjectHandler* objectHandler, UIHandler* uiHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice);
	virtual ~LevelEditState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};