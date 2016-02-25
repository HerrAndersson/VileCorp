#include "PlayState.h"

PlayState::PlayState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule, DirectX::XMFLOAT3* ambientLight)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule)
{
	_gameLogic = nullptr;
	_ambientLight = ambientLight;
	_soundModule->AddSound("in_game_2", 0.2f, 1.0f, true, true);
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
	if (_gameLogic->GoToMenu())
	{
		ChangeState(State::MENUSTATE);
	}
	if (_controls->IsFunctionKeyDown("MOUSE:SELECT") && !_uiTree.GetNode("Tutorial")->GetHidden())
	{
		_uiTree.GetNode("Tutorial")->SetHidden(true);
		_objectHandler->EnableSpawnPoints();
	}

	_gameLogic->Update(deltaTime);
	HandleCam(deltaTime);
}

void PlayState::OnStateEnter()
{
	_ambientLight->x = AMBIENT_LIGHT_NIGHT.x;
	_ambientLight->y = AMBIENT_LIGHT_NIGHT.y;
	_ambientLight->z = AMBIENT_LIGHT_NIGHT.z;
	_uiTree.GetNode("Tutorial")->SetHidden(false);
	_gameLogic = new GameLogic(_objectHandler, _camera, _controls, _pickingDevice, &_uiTree, _assetManager, _settingsReader, _soundModule);

	//Play music
	_soundModule->Play("in_game_2");
}

void PlayState::OnStateExit()
{
	delete _gameLogic;
	_gameLogic = nullptr;
	_soundModule->Pause("in_game_2");
}