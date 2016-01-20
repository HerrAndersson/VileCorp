#include "OptionsState.h"

OptionsState::OptionsState(System::Controls* controls, ObjectHandler* objectHandler, UIHandler* uiHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice)
	: BaseState (_controls, _objectHandler, _uiHandler, _inputDevice, _camera, _pickingDevice)
{
	_controls = controls;
	_objectHandler = objectHandler;
	_uiHandler = uiHandler;
	_inputDevice = inputDevice;
	_camera = camera;
	_pickingDevice = pickingDevice;
}

OptionsState::~OptionsState()
{}

void OptionsState::Update(float deltaTime)
{

}

void OptionsState::OnStateEnter()
{

}

void OptionsState::OnStateExit()
{

}