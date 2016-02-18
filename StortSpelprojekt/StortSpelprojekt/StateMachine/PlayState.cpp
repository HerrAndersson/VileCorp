#include "PlayState.h"

PlayState::PlayState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule)
{
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
	if (_gameLogic->IsGameDone() && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		ChangeState(State::MENUSTATE);
	}
	if (_controls->IsFunctionKeyDown("MOUSE:SELECT") && !_uiTree.GetNode("Tutorial")->GetHidden())
	{
		_uiTree.GetNode("Tutorial")->SetHidden(true);
		_objectHandler->EnableSpawnPoints();
	}

	_gameLogic->Update(deltaTime);
}

void PlayState::OnStateEnter()
{
	_uiTree.GetNode("Tutorial")->SetHidden(false);
	_gameLogic = new GameLogic(_objectHandler, _camera, _controls, _pickingDevice, &_uiTree, _assetManager, _settingsReader);
}

void PlayState::OnStateExit()
{
	delete _gameLogic;
	_gameLogic = nullptr;
}