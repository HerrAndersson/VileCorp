#include "PlayState.h"

PlayState::PlayState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::Settings* settings, System::SoundModule* soundModule)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, "PLAY", assetManager, fontWrapper, settings, soundModule)
{
	_controls = controls;
	_objectHandler = objectHandler;
	
	_camera = camera;
	_pickingDevice = pickingDevice;
	_gamePaused = false;
}

PlayState::~PlayState()
{}

void PlayState::Update(float deltaTime)
{
	HandleInput();

	IngameMenu();
	if (!_gamePaused)
	{
		_gameLogic.Update(deltaTime);
	}
}

void PlayState::OnStateEnter()
{
	_gameLogic.Initialize(_objectHandler, _camera, _controls, _pickingDevice);
	_uiTree.GetNode("root")->SetHidden(true);
	_objectHandler->EnableSpawnPoints();
}

void PlayState::OnStateExit()
{
	_gamePaused = false;
}

void PlayState::HandleInput()
{
	
}

void PlayState::IngameMenu()
{
	if (_controls->IsFunctionKeyDown("MENU:MENU"))
	{
		_uiTree.GetNode("root")->SetHidden(false);
		_gamePaused = true;
	}
	
	if (_gamePaused)
	{
		if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			System::MouseCoord coord = _controls->GetMouseCoord();
			if (_uiTree.IsButtonColliding("resume", coord._pos.x, coord._pos.y))
			{
				_uiTree.GetNode("root")->SetHidden(true);
				_gamePaused = false;
			}
		
			if (_uiTree.IsButtonColliding("mainmenu", coord._pos.x, coord._pos.y) || _controls->IsFunctionKeyDown("MENU:MENU"))
			{
				ChangeState(State::MENUSTATE);
			}
		
			if (_uiTree.IsButtonColliding("quit", coord._pos.x, coord._pos.y))
			{
				ChangeState(State::EXITSTATE);
			}
		}
	}
}