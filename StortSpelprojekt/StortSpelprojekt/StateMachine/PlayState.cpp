#include "PlayState.h"

PlayState::PlayState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule, DirectX::XMFLOAT3* ambientLight)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule)
{
	_gameLogic = nullptr;
	_ambientLight = ambientLight;

	/*
	Add all sounds used in playstate (even though they might not be played in this class)
	*/
	//Music
	_soundModule->AddSound("in_game_2", 0.2f, 1.0f, true, true);
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

	//TODO: Check this more! /Jonas
	std::vector<std::vector<GameObject*>>* gos = _objectHandler->GetGameObjects();
	for (auto& gv : *gos)
	{
		std::sort(gv.begin(), gv.end(),
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