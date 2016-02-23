#include "LevelSelectState.h"



LevelSelectState::LevelSelectState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule) :
	BaseState(controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule)
{
	_levelSelectionMin = 6;
	_levelSelectionMax = 9; //TODO: Add to this if more levels are added
	for (int i = _levelSelectionMin; i < _levelSelectionMax + 1; i++)
	{
		_uiTree.GetNode(std::to_string(i))->SetHidden(true);
	}
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
	System::MouseCoord coord = _controls->GetMouseCoord();
	XMFLOAT4 color(0.3f, 0.3f, 0.3f, 1.0f);
	HandleHoverColorOffset("nextlevel", "nextlevel", coord, color);
	HandleHoverColorOffset("prevlevel", "prevlevel", coord, color);
	HandleHoverColorOffset("playbutton", "playbutton", coord, color);

	if (_controls->IsFunctionKeyDown("MENU:MENU"))
	{
		ChangeState(State::MENUSTATE);
	}
	if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		if (_uiTree.IsButtonColliding("playbutton", coord._pos.x, coord._pos.y))
		{
			if (_levelSelection == 6)
			{
				_tutorialState = TutorialState::NEWTUTORIAL;
			}
			else
			{
				_tutorialState = TutorialState::NOTUTORIAL;
			}
			_objectHandler->LoadLevel(_levelSelection);
			_uiTree.GetNode(std::to_string(_levelSelection))->SetHidden(true);
			ChangeState(State::PLACEMENTSTATE);
		}
		if (_uiTree.IsButtonColliding("prevlevel", coord._pos.x, coord._pos.y))
		{
			if (_levelSelection > _levelSelectionMin)
			{
				_soundModule->Play("Assets/Sounds/page");
				_uiTree.GetNode(std::to_string(_levelSelection))->SetHidden(true);
				_levelSelection--;
				_uiTree.GetNode(std::to_string(_levelSelection))->SetHidden(false);
			}
		}
		if (_uiTree.IsButtonColliding("nextlevel", coord._pos.x, coord._pos.y))
		{
			if (_levelSelection < _levelSelectionMax)
			{
				_soundModule->Play("Assets/Sounds/page");
				_uiTree.GetNode(std::to_string(_levelSelection))->SetHidden(true);
				_levelSelection++;
				_uiTree.GetNode(std::to_string(_levelSelection))->SetHidden(false);
			}
		}
	}
}

void LevelSelectState::OnStateEnter()
{
	if (_settingsReader->GetProfile()->_firstTime)
	{
		_levelSelection = _levelSelection = 6;
	}
	else
	{
		_levelSelection = 7;
	}
	_uiTree.GetNode(std::to_string(_levelSelection))->SetHidden(false);
	//TODO: This is hardcoded to nine due to the functionality in LevelSelectState constructor /Sebastian
	_levelSelectionMax = min(_levelSelectionMin + _settingsReader->GetProfile()->_level, _levelSelectionMax);
}

void LevelSelectState::OnStateExit()
{
	_uiTree.GetNode(std::to_string(_levelSelection))->SetHidden(true);
}
