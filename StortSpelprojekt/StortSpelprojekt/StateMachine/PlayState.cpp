#include "PlayState.h"

PlayState::PlayState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, "PLAY", assetManager, fontWrapper, width, height)
{
	_controls = controls;
	_objectHandler = objectHandler;
	
	_camera = camera;
	_pickingDevice = pickingDevice;
}

PlayState::~PlayState()
{}

void PlayState::Update(float deltaTime)
{
	if(_controls->IsFunctionKeyDown("PLAY:MENU"))
	{
		ChangeState(MENUSTATE);
	}

	_gameLogic.Update(deltaTime);
}

void PlayState::OnStateEnter()
{
	_gameLogic.Initialize(_objectHandler, _camera, _controls, _pickingDevice);
	_objectHandler->EnableSpawnPoints();
}

void PlayState::OnStateExit()
{

}