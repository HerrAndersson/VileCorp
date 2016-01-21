#include "OptionsState.h"

OptionsState::OptionsState(System::Controls* controls, ObjectHandler* objectHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height)
	: BaseState (controls, objectHandler, inputDevice, camera, pickingDevice, filename, "OPTIONS", assetManager, fontWrapper, width, height)
{
	_controls = controls;
	_objectHandler = objectHandler;
	
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