#include "LevelSelectState.h"



LevelSelectState::LevelSelectState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule) :
	BaseState(controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule)
{
	if (_settingsReader->GetProfile()->_firstTime)
	{
		_levelSelection = _levelSelection = 6;
		_tutorial = true;
	}
	else
	{
		_levelSelection = 7;
	}

	
	_levelSelectionMin = 6;
	_levelSelectionMax = 9; //TODO: Add to this if more levels are added
	for (int i = _levelSelectionMin; i < _levelSelectionMax + 1; i++)
	{
		_uiTree.GetNode(std::to_string(i))->SetHidden(true);
	}
	_uiTree.GetNode(std::to_string(_levelSelection))->SetHidden(false);
}


LevelSelectState::~LevelSelectState()
{
}

void LevelSelectState::Update(float deltaTime)
{
	if (_controls->IsFunctionKeyDown("DEBUG:RELOAD_GUI"))
	{
		_uiTree.ReloadTree("../../../../StortSpelprojekt/Assets/GUI/Levelselect.json");
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
			//_soundModule->Play("Assets/Sounds/page.wav");
			_objectHandler->LoadLevel(_levelSelection);
			ChangeState(State::PLACEMENTSTATE);
		}
		if (_uiTree.IsButtonColliding("prevlevel", coord._pos.x, coord._pos.y))
		{
			if (_levelSelection > _levelSelectionMin)
			{
				_soundModule->Play("Assets/Sounds/page.wav");
				_uiTree.GetNode(std::to_string(_levelSelection))->SetHidden(true);
				_levelSelection--;
				_uiTree.GetNode(std::to_string(_levelSelection))->SetHidden(false);
			}
		}
		if (_uiTree.IsButtonColliding("nextlevel", coord._pos.x, coord._pos.y))
		{
			if (_levelSelection < _levelSelectionMax)
			{
				_soundModule->Play("Assets/Sounds/page.wav");
				_uiTree.GetNode(std::to_string(_levelSelection))->SetHidden(true);
				_levelSelection++;
				_uiTree.GetNode(std::to_string(_levelSelection))->SetHidden(false);
			}
		}
	}
}

void LevelSelectState::OnStateEnter()
{
	//TODO: This is hardcoded to nine due to the functionality in LevelSelectState constructor /Sebastian
	_levelSelectionMax = min(_levelSelectionMin + _settingsReader->GetProfile()->_level, _levelSelectionMax);
}

void LevelSelectState::OnStateExit()
{
}
