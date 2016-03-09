#include "PlayState.h"

PlayState::PlayState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule, AmbientLight* ambientLight)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule)
{
	_gameLogic = nullptr;
	_ambientLight = ambientLight;

	/*
	Add all sounds used in playstate (even though they might not be played in this class)
	*/
	//Music
	_soundModule->AddSound("in_game_2", 0.2f, 1.0f, true, true, true);
	//Unit
	_soundModule->AddSound("unit_select", 0.5f, 1.0f, true, false);
	_soundModule->AddSound("unit_move", 0.5f, 1.0f, true, false);
	_soundModule->AddSound("engineer_select", 0.7f, 1.0f, true, false);
	_soundModule->AddSound("engineer_move", 0.7f, 1.0f, true, false);

	_soundModule->AddSound("enemy_death", 0.7f, 1.0f, false, false);
	_soundModule->AddSound("guard_death", 1.0f, 0.6f, false, false);
	_soundModule->AddSound("guard_react", 1.0f, 1.0f, false, false);
	//Traps
	_soundModule->AddSound("anvil_activate", 1.0f, 1.0f, false, false);
	_soundModule->AddSound("tesla_activate", 0.8f, 1.0f, false, false);
	_soundModule->AddSound("mg_activate", 1.0f, 1.0f, false, false);
	_soundModule->AddSound("saw_activate", 1.0f, 1.0f, false, false);
	_soundModule->AddSound("bomb_activate", 1.0f, 1.0f, false, false);
	_soundModule->AddSound("bear_activate", 0.8f, 1.0f, false, false);
	_soundModule->AddSound("flame_activate", 1.0f, 1.0f, false, false);
	_soundModule->AddSound("shark_activate", 1.0f, 1.0f, false, false);
	_soundModule->AddSound("water_activate", 1.0f, 1.0f, false, false);
	_soundModule->AddSound("spin_activate", 1.0f, 1.0f, false, false);
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

	_gameLogic->Update(deltaTime);
	HandleCam(deltaTime);
}

void PlayState::OnStateEnter()
{
	_ambientLight->NightTime();
	_gameLogic = new GameLogic(_objectHandler, _camera, _controls, _pickingDevice, &_uiTree, _assetManager, _settingsReader, _soundModule);

	if (GetOldState() != State::PAUSESTATE && GetOldState() != State::PLACEMENTSTATE)
	{
		XMFLOAT3 campos;
		campos.x = (float)_objectHandler->GetTileMap()->GetWidth() / 2.0f;
		campos.y = 15.0f;
		campos.z = (float)_objectHandler->GetTileMap()->GetHeight() / 2.0f - 10.0f;
		_camera->SetPosition(campos);
		XMFLOAT3 camRot = _camera->GetRotation();
		_camera->SetRotation(XMFLOAT3(camRot.x, 0.0f, camRot.z));
	}

	//Play music
	_soundModule->Play("in_game_2");

	//TODO: Check this more! /Jonas
	std::vector<std::vector<GameObject*>>* gameObjects = _objectHandler->GetGameObjects();
	for (auto& gameObjectVector : *gameObjects)
	{
		std::sort(gameObjectVector.begin(), gameObjectVector.end(),
			[](GameObject* first, GameObject* second)
		{
			return *first < *second;
		}
		);
	}
}

void PlayState::OnStateExit()
{
	delete _gameLogic;
	_gameLogic = nullptr;
	_soundModule->Pause("in_game_2");
}