#include "LevelSelectState.h"


LevelSelectState::LevelSelectState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule) :
	BaseState(controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule)
{
	std::vector<GUI::Node*>* levelSelectionPageNodes = _uiTree.GetNode("LevelSelectionPageContent")->GetChildren();

	_levelNameNode = levelSelectionPageNodes->at(0);
	_storyTitleNode = levelSelectionPageNodes->at(1);
	_storyBodyNode = levelSelectionPageNodes->at(2);
	_gameModeNode = levelSelectionPageNodes->at(3);

	_nextLevelButtonNode = _uiTree.GetNode("nextlevel");
	_previousLevelButtonNode = _uiTree.GetNode("prevlevel");

	_campaignSelectionMin = 1;
	_skirmishSelectedIndex = 1;

	for (unsigned i = 0; i < _uiTree.GetNode("LevelSelectionTabs")->GetChildren()->size(); i++)
	{
		GUI::Node* modeSelect = _uiTree.GetNode("LevelSelectionTabs")->GetChildren()->at(i);

		_tabPosition[i] = modeSelect->GetLocalPosition();
	}
}

LevelSelectState::~LevelSelectState()
{
}

void LevelSelectState::Update(float deltaTime)
{
	if (_controls->IsFunctionKeyDown("DEBUG:RELOAD_GUI"))
	{
		_uiTree.ReloadTree("../../../../StortSpelprojekt/Assets/GUI/levelselect.json");
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
			_soundModule->Play("Assets/Sounds/page");

			std::string levelBinaryPath;
			if (_isCampaignMode)
			{
				levelBinaryPath = CAMPAIGN_PATH;
			}
			else
			{
				levelBinaryPath = SKIRMISH_PATH;
			}
			levelBinaryPath += _selectedLevelHeader._levelBinaryFilename;

			_objectHandler->SetCurrentLevelHeader(_selectedLevelHeader);
			_objectHandler->LoadLevel(levelBinaryPath);
			ChangeState(State::PLACEMENTSTATE);
		}
		if (_uiTree.IsButtonColliding("prevlevel", coord._pos.x, coord._pos.y))
		{
			_soundModule->Play("Assets/Sounds/page");
			if (_isCampaignMode && _campaignSelection != _campaignSelectionMin)
			{
				_campaignSelection--;
				LoadLevelHeader(_campaignSelection, &_selectedLevelHeader);
				SelectedLevelHeaderToGUI();
			}
			else if (!_isCampaignMode && _skirmishSelectedIndex != _campaignSelectionMin)
			{
				_skirmishSelectedIndex--;
				LoadLevelHeader(_skirmishSelectedIndex, &_selectedLevelHeader);
				SelectedLevelHeaderToGUI();
			}

			UpdateButtonsNextPreviousVisability();
		}
		if (_uiTree.IsButtonColliding("nextlevel", coord._pos.x, coord._pos.y))
		{
			_soundModule->Play("Assets/Sounds/page");
			if (_isCampaignMode && _campaignSelection != _campaignSelectionMax)
			{
				_campaignSelection++;
				LoadLevelHeader(_campaignSelection, &_selectedLevelHeader);
				SelectedLevelHeaderToGUI();
			}
			else if(!_isCampaignMode && _skirmishSelectedIndex != _campaignSelectionMax)
			{
				_skirmishSelectedIndex++;
				LoadLevelHeader(_skirmishSelectedIndex, &_selectedLevelHeader);
				SelectedLevelHeaderToGUI();
			}

			UpdateButtonsNextPreviousVisability();
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
				//CAMPAIGN has been picked
				int help = 0;
				_isCampaignMode = true;
				_levelNameNode->SetHidden(true);
				LoadLevelHeader(_campaignSelection, &_selectedLevelHeader);
				SelectedLevelHeaderToGUI();
			}
			else
			{
				//SKIRMISH has been picked
				int help = 0;
				_isCampaignMode = false;
				_levelNameNode->SetHidden(false);
				LoadLevelHeader(_skirmishSelectedIndex, &_selectedLevelHeader);
				SelectedLevelHeaderToGUI();
			}
		}

	}
}

void LevelSelectState::OnStateEnter()
{
	_profile = _settingsReader->GetProfile();

	_isCampaignMode = true;
	_uiTree.GetNode("LevelName")->SetHidden(true);

	for (unsigned i = 0; i < _uiTree.GetNode("LevelSelectionTabs")->GetChildren()->size(); i++)
	{
		GUI::Node* modeSelect = _uiTree.GetNode("LevelSelectionTabs")->GetChildren()->at(i);

		modeSelect->SetPosition(_tabPosition[i]);
	}

	_campaignSelectionMax = _campaignSelection = _profile->_level;
	LoadLevelHeader(_campaignSelection, &_selectedLevelHeader);
	SelectedLevelHeaderToGUI();
	UpdateButtonsNextPreviousVisability();
}

void LevelSelectState::OnStateExit()
{
}

void LevelSelectState::LoadLevelHeader(int levelFilename, Level::LevelHeader * headerToLoad)
{
	LoadLevelHeader(std::to_string(levelFilename), headerToLoad);
}

void LevelSelectState::LoadLevelHeader(std::string levelFilename, Level::LevelHeader * headerToLoad)
{
	std::string levelPath;
	if (_isCampaignMode)
	{
		levelPath = CAMPAIGN_PATH + levelFilename + ".json";
	}
	else
	{
		levelPath = SKIRMISH_PATH + levelFilename + ".json";
	}
	System::loadJSON(headerToLoad, levelPath);
}

void LevelSelectState::SelectedLevelHeaderToGUI()
{
	if (_isCampaignMode)
	{
		_levelNameNode->SetHidden(true);
	}
	else
	{
		_levelNameNode->SetHidden(false);
		_levelNameNode->SetText(StringToWstring(_selectedLevelHeader._levelBinaryFilename));
	}
	_storyTitleNode->SetText(StringToWstring(_selectedLevelHeader._storyTitle));
	_storyBodyNode->SetText(StringToWstring(_selectedLevelHeader._storyBody));

	std::stringstream ss;
	ss << _selectedLevelHeader._gameMode;
	int gameModeIndex;
	ss >> gameModeIndex;
	std::string gameModeName = Level::GameModeNames[gameModeIndex];
	_gameModeNode->SetText(StringToWstring(gameModeName));
}

void LevelSelectState::UpdateButtonsNextPreviousVisability()
{
	if (_isCampaignMode)
	{
		bool isHidden = true;
		if (_campaignSelection > _campaignSelectionMin)
		{
			isHidden = false;
		}
		_previousLevelButtonNode->SetHidden(isHidden);

		isHidden = true;
		if (_campaignSelection < _campaignSelectionMax)
		{
			isHidden = false;
		}
		_nextLevelButtonNode->SetHidden(isHidden);
	}
	else
	{
		bool isHidden = true;
		if (_skirmishSelectedIndex > _campaignSelectionMin)
		{
			isHidden = false;
		}
		_previousLevelButtonNode->SetHidden(isHidden);

		isHidden = true;
		if (_skirmishSelectedIndex < _campaignSelectionMax)
		{
			isHidden = false;
		}
		_nextLevelButtonNode->SetHidden(isHidden);
	}
}
