#include "PlayState.h"

PlayState::PlayState(System::Controls* controls, ObjectHandler* objectHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height)
	: BaseState(controls, objectHandler, inputDevice, camera, pickingDevice, filename, "PLAY", assetManager, fontWrapper, width, height)
{
	_controls = controls;
	_objectHandler = objectHandler;
	
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
	_gameLogic.Initialize(_inputDevice, _objectHandler, _camera, _controls, _pickingDevice);
}

void PlayState::OnStateExit()
{

}