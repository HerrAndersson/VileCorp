#include "LevelSelectState.h"



LevelSelectState::LevelSelectState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule) :
	BaseState(controls, objectHandler, camera, pickingDevice, filename, "LEVELSELECT", assetManager, fontWrapper, settingsReader, soundModule)
{

}


LevelSelectState::~LevelSelectState()
{
}

void LevelSelectState::Update(float deltaTime)
{
	if (_controls->IsFunctionKeyDown("DEBUG:RELOAD_GUI"))
	{
		_uiTree.ReloadTree("../../../../StortSpelprojekt/Assets/gui.json", "LEVELSELECT");
	}
	if (_controls->IsFunctionKeyDown("MENU:MENU"))
	{
		ChangeState(State::MENUSTATE);
	}
}

void LevelSelectState::OnStateEnter()
{
}

void LevelSelectState::OnStateExit()
{
}
