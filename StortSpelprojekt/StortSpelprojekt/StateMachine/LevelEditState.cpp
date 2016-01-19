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

	//tempAddObj

	//R Adds Floor
	if (_inputDevice->IsPressed(0x52))
	{
		_levelEdit.Add(FLOOR, FLOOR);
	}

	//T adds Wall
	if (_inputDevice->IsPressed(0x54))
	{
		_levelEdit.Add(WALL, WALL);
	}

	//Y adds Unit
	if (_inputDevice->IsPressed(0x59))
	{
		_levelEdit.Add(ENEMY, ENEMY);
	}
}

void LevelEditState::OnStateEnter()
{
	_levelEdit.Initialize(_objectHandler, _inputDevice, _controls, _pickingDevice, _camera, _uiHandler);
}

void LevelEditState::OnStateExit()
{

}

