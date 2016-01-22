#include "MenuState.h"

MenuState::MenuState(System::Controls* controls, ObjectHandler* objectHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height)
	: BaseState (controls, objectHandler, inputDevice, camera, pickingDevice, filename, "MENU", assetManager, fontWrapper, width, height)
{
	_controls = controls;
	_objectHandler = objectHandler;
	
	_inputDevice = inputDevice;
	_camera = camera;
	_pickingDevice = pickingDevice;
}

MenuState::~MenuState()
{
	
}

void MenuState::Update(float deltaTime)
{
	bool val = _uiTree.IsButtonColliding("playbutton", 100, 260);
}

void MenuState::OnStateEnter()
{
	
}

void MenuState::OnStateExit()
{

}