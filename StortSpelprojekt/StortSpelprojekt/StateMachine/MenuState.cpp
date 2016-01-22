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
	if (_controls->IsFunctionKeyDown("DEBUG:ENABLE_FREECAM"))
	{
		//Get mouse coord
		//Calculate mouse coord to screenspace
		//Check collision with all the buttons
		//Execute code for button
	}
	
}

void MenuState::OnStateEnter()
{
	
}

void MenuState::OnStateExit()
{

}