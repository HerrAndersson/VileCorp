#pragma once
#include "../Controls.h"
#include "../ObjectHandler.h"
#include "../UIHandler.h"
#include "../../System/Camera.h"
#include "../PickingDevice.h"


enum State
{
	SPLASHSTATE,
	MENUSTATE,
	PLAYSTATE,
	LEVELEDITSTATE,
	OPTIONSSTATE,
	EXITSTATE,
};

class BaseState
{
private:
	static State BaseState::_newStateRequest;

protected:
	System::Controls*		_controls;
	ObjectHandler*			_objectHandler;
	UIHandler*				_uiHandler;
	System::InputDevice*	_inputDevice;
	System::Camera*			_camera;
	PickingDevice*			_pickingDevice;
	
	void ChangeState(State newState);
public:
	BaseState(System::Controls* controls, ObjectHandler* objectHandler, UIHandler* uiHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice);
	~BaseState();

	virtual void Update(float deltaTime) = 0;
	virtual void OnStateEnter() = 0;
	virtual void OnStateExit() = 0;

	State GetNewStateRequest()const;
};
