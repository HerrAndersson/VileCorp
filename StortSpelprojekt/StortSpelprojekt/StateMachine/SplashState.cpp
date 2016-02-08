#include "SplashState.h"

SplashState::SplashState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::Settings* settings)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, "SPLASH", assetManager, fontWrapper, settings)
{
	_controls = controls;
	_objectHandler = objectHandler;
	
	_camera = camera;
	_pickingDevice = pickingDevice;
}

SplashState::~SplashState()
{}

void SplashState::Update(float deltaTime)
{
	ChangeState(State::MENUSTATE);
}

void SplashState::OnStateEnter()
{

}

void SplashState::OnStateExit()
{

}