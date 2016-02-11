#include "LevelSelectState.h"



LevelSelectState::LevelSelectState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::Settings* settings, System::SoundModule* soundModule) :
	BaseState(controls, objectHandler, camera, pickingDevice, filename, "LEVELSELECT", assetManager, fontWrapper, settings, soundModule)
{
	_levelSelection = 0;
	_levelSelectionMax = 0; //TODO: Add to this if more levels are added
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
	if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		System::MouseCoord coord = _controls->GetMouseCoord();
		if (_uiTree.IsButtonColliding("playbutton", coord._pos.x, coord._pos.y))
		{
			_objectHandler->LoadLevel(_levelSelection);
			ChangeState(State::PLACEMENTSTATE);
		}
		
		if (_uiTree.IsButtonColliding("prevlevel", coord._pos.x, coord._pos.y))
		{
			_levelSelection--;
		}
		else if (_uiTree.IsButtonColliding("nextlevel", coord._pos.x, coord._pos.y))
		{
			_levelSelection++;
		}
		if (_levelSelection == -1)
		{
			_levelSelection = _levelSelection - 1;
		}
		else if (_levelSelection == _levelSelectionMax)
		{
			_levelSelection = 0;
		}
	}
}

void LevelSelectState::OnStateEnter()
{

}

void LevelSelectState::OnStateExit()
{
}
