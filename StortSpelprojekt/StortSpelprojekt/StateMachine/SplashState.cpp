#include "SplashState.h"

SplashState::SplashState(System::Controls* controls, ObjectHandler* objectHandler, UIHandler* uiHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice)
	: BaseState(_controls, _objectHandler, _uiHandler, _inputDevice, _camera, _pickingDevice)
{
	_controls = controls;
	_objectHandler = objectHandler;
	_uiHandler = uiHandler;
	_inputDevice = inputDevice;
	_camera = camera;
	_pickingDevice = pickingDevice;
}

SplashState::~SplashState()
{}

void SplashState::Update(float deltaTime)
{
	ChangeState(State::PLAYSTATE);

	//_inputDevice->SetCursorLock(true);
}

void SplashState::OnStateEnter()
{

}

void SplashState::OnStateExit()
{

}