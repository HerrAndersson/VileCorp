#include "PlayState.h"

PlayState::PlayState(System::Controls* controls, ObjectHandler* objectHandler, UIHandler* uiHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice)
	: BaseState(_controls, _objectHandler, _uiHandler, _inputDevice, _camera, _pickingDevice)
{
	_controls = controls;
	_objectHandler = objectHandler;
	_uiHandler = uiHandler;
	_inputDevice = inputDevice;
	_camera = camera;
	_pickingDevice = pickingDevice;
}

PlayState::~PlayState()
{}

void PlayState::Update(float deltaTime)
{
	_gameLogic.Update(deltaTime);
}

void PlayState::OnStateEnter()
{
	_gameLogic.Initialize(_inputDevice, _objectHandler, _uiHandler, _camera, _controls, _pickingDevice);
}

void PlayState::OnStateExit()
{

}