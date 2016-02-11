#include "PlayState.h"

PlayState::PlayState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, "PLAY", assetManager, fontWrapper, settingsReader, soundModule)
{
	_controls = controls;
	_objectHandler = objectHandler;
	_camera = camera;
	_pickingDevice = pickingDevice;
	_assetManager = assetManager;

	_gameLogic = nullptr;
}

PlayState::~PlayState()
{
	delete _gameLogic;
	_gameLogic = nullptr;
}

void PlayState::Update(float deltaTime)
{
	if (_controls->IsFunctionKeyDown("MENU:MENU"))
	{
		ChangeState(State::PAUSESTATE);
	}

	_gameLogic->Update(deltaTime);
}

void PlayState::OnStateEnter()
{
	_gameLogic = new GameLogic(_objectHandler, _camera, _controls, _pickingDevice, &_uiTree, _assetManager);
	_objectHandler->EnableSpawnPoints();
}

void PlayState::OnStateExit()
{
	delete _gameLogic;
	_gameLogic = nullptr;
}