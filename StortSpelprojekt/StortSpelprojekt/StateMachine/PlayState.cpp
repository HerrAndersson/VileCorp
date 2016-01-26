#include "PlayState.h"

PlayState::PlayState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, "PLAY", assetManager, fontWrapper, width, height)
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
	IngameMenu();
	if (!_gamePaused)
	{
		_gameLogic.Update(deltaTime);
	}
}

void PlayState::OnStateEnter()
{
	_gameLogic.Initialize(_objectHandler, _camera, _controls, _pickingDevice);
	//TODO: hide ingame menu
}

void PlayState::OnStateExit()
{

}

void PlayState::IngameMenu()
{

	if (_controls->IsFunctionKeyDown("PLAY:SHOWMENU"))
	{
		//TODO::unhide menu
		_gamePaused = true;
	}
	
	if (_gamePaused)
	{
	if (_controls->IsFunctionKeyDown("MENU:CLICK"))
	{
		System::MouseCoord coord = _controls->GetMouseCoord();
		if (_uiTree.IsButtonColliding("resume", coord._pos.x, coord._pos.y))
		{
			//TODO: hide menu
			_gamePaused = false;
		}
		
		if (_uiTree.IsButtonColliding("mainmenu", coord._pos.x, coord._pos.y))
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