#include "LevelEditState.h"

LevelEditState::LevelEditState(System::Controls* controls, ObjectHandler* objectHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height)
	: BaseState(controls, objectHandler, inputDevice, camera, pickingDevice, filename, "LEVELEDIT", assetManager, fontWrapper, width, height)
{
	_controls = controls;
	_objectHandler = objectHandler;
	
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
	_levelEdit.Initialize(_objectHandler, _inputDevice, _controls, _pickingDevice, _camera);
}

void LevelEditState::OnStateExit()
{

}

