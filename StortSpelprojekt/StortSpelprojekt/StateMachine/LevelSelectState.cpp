#include "LevelSelectState.h"



LevelSelectState::LevelSelectState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule) :
	BaseState(controls, objectHandler, camera, pickingDevice, filename, "LEVELSELECT", assetManager, fontWrapper, settingsReader, soundModule)
{
	_profile = _settingsReader->GetProfile();
	_levelSelection = _profile->_level;
	_levelSelectionMin = 7;
	_levelSelectionMax = 9; //TODO: Add to this if more levels are added
	for (int i = _levelSelectionMin+1; i < _levelSelectionMax + 1; i++)
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
			_soundModule->Play("Assets/Sounds/page.wav");
			//_objectHandler->LoadLevel(_levelSelection);
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
		else if (_uiTree.IsButtonColliding("SkirmishTab", coord._pos.x, coord._pos.y) || _uiTree.IsButtonColliding("CampaignTab", coord._pos.x, coord._pos.y))
		{
			//Move Button icon
			GUI::Node* node = _uiTree.GetNode("CampaignTab");
			XMFLOAT2 unpickedTab = node->GetLocalPosition();

			GUI::Node* node2 = _uiTree.GetNode("SkirmishTab");
			XMFLOAT2 pickedTab = node2->GetLocalPosition();

			node2->SetPosition(XMFLOAT2(unpickedTab.x,pickedTab.y));
			node->SetPosition(XMFLOAT2(pickedTab.x, unpickedTab.y));

			if (pickedTab.x > unpickedTab.x)
			{
				int help = 0;
				//CAMPAIGN has been picked
				//TODO: Save this setting for this wave
			}
			else
			{
				int help = 0;
				//SKIRMISH has been picked
				//TODO: Save this setting for this wave
			}
		}

	}
}

void LevelSelectState::OnStateEnter()
{
	//TODO: This is hardcoded to nine due to the functionality in LevelSelectState constructor /Sebastian
	_levelSelectionMax = min(_levelSelectionMin + _settingsReader->GetProfile()->_level, 9);
}

void LevelSelectState::OnStateExit()
{
}
