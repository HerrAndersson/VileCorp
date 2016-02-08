#include "OptionsState.h"

OptionsState::OptionsState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height, System::SoundModule* soundModule)
	: BaseState (controls, objectHandler, camera, pickingDevice, filename, "OPTIONS", assetManager, fontWrapper, width, height, soundModule)
{
	_controls = controls;
	_objectHandler = objectHandler;

	_camera = camera;
	_pickingDevice = pickingDevice;
}

OptionsState::~OptionsState()
{}

void OptionsState::Update(float deltaTime)
{
	if (_controls->IsFunctionKeyDown("DEBUG:RELOAD_GUI"))
	{
		_uiTree.ReloadTree("Assets/gui.json", "OPTIONS");
	}
	_uiTree.IsButtonColliding("res_right", 0, 0);
}

void OptionsState::OnStateEnter()
{

}

void OptionsState::OnStateExit()
{

}