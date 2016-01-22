#include "OptionsState.h"

OptionsState::OptionsState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height)
	: BaseState (controls, objectHandler, camera, pickingDevice, filename, "OPTIONS", assetManager, fontWrapper, width, height)
{
	_controls = controls;
	_objectHandler = objectHandler;

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