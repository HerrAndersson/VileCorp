#include "MenuState.h"

MenuState::MenuState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height)
	: BaseState (controls, objectHandler, camera, pickingDevice, filename, "MENU", assetManager, fontWrapper, width, height)
{
	_controls = controls;
	_objectHandler = objectHandler;
	
	_camera = camera;
	_pickingDevice = pickingDevice;
}

MenuState::~MenuState()
{
	
}

void MenuState::Update(float deltaTime)
{
	if (_controls->IsFunctionKeyDown("DEBUG:RELOAD_GUI"))
	{
		_uiTree.ReloadTree("../../../../StortSpelprojekt/Assets/gui.json", "MENU");
	}
	if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		System::MouseCoord coord = _controls->GetMouseCoord();
		if (_uiTree.IsButtonColliding("playbutton", coord._pos.x, coord._pos.y))
		{
			ChangeState(State::PLACEMENTSTATE);
		}
		if (_uiTree.IsButtonColliding("optionsbutton", coord._pos.x, coord._pos.y))
		{
			ChangeState(State::LEVELEDITSTATE);
		}
		if (_uiTree.IsButtonColliding("exitbutton", coord._pos.x, coord._pos.y))
		{
			//TODO: This is a hack, this fuction should instead return a value if the game should quit //Mattias
			PostQuitMessage(0);
		}
	}
	if (_controls->IsFunctionKeyDown("MENU:MENU"))
	{
		PostQuitMessage(0);
	}
}

void MenuState::OnStateEnter()
{
	
}

void MenuState::OnStateExit()
{

}