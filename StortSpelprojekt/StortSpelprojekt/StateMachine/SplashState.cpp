#include "SplashState.h"

SplashState::SplashState(System::Controls* controls, ObjectHandler* objectHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height)
	: BaseState(controls, objectHandler, inputDevice, camera, pickingDevice, filename, "SPLASH", assetManager, fontWrapper, width, height)
{
	_controls = controls;
	_objectHandler = objectHandler;
	
	_inputDevice = inputDevice;
	_camera = camera;
	_pickingDevice = pickingDevice;
}

SplashState::~SplashState()
{}

void SplashState::Update(float deltaTime)
{
	ChangeState(State::PLACEMENTSTATE);
}

void SplashState::OnStateEnter()
{

}

void SplashState::OnStateExit()
{

}