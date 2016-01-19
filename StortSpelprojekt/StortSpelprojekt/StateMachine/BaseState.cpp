#include "BaseState.h"

State BaseState::_newStateRequest;

BaseState::BaseState(System::Controls* controls, ObjectHandler* objectHandler, UIHandler* uiHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice)
{
	_controls			= controls;
	_objectHandler		= objectHandler;
	_uiHandler			= uiHandler;
	_inputDevice		= inputDevice;
	_camera				= camera;
	_pickingDevice		= pickingDevice;
	_newStateRequest	= SPLASHSTATE;
}

BaseState::~BaseState()
{}

void BaseState::ChangeState(State newState)
{
	_newStateRequest = newState;
}

State BaseState::GetNewStateRequest()const
{
	return _newStateRequest;
}