#include "LevelEditState.h"

LevelEditState::LevelEditState(System::Controls* controls, ObjectHandler* objectHandler, UIHandler* uiHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice)
	: BaseState(_controls, _objectHandler, _uiHandler, _inputDevice, _camera, _pickingDevice)
{
	_controls = controls;
	_objectHandler = objectHandler;
	_uiHandler = uiHandler;
	_inputDevice = inputDevice;
	_camera = camera;
	_pickingDevice = pickingDevice;
}

LevelEditState::~LevelEditState()
{}

void LevelEditState::Update(float deltaTime)
{
	_levelEdit.Update(deltaTime);
}

void LevelEditState::OnStateEnter()
{
	_levelEdit.Initialize(_objectHandler, _inputDevice, _controls, _pickingDevice, _camera);
}

void LevelEditState::OnStateExit()
{

}

