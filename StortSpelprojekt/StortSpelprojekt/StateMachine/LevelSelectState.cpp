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

	_campaignTabNode = _uiTree.GetNode("CampaignTab");
	_skirmishTabNode = _uiTree.GetNode("SkirmishTab");

	_campaignSelectionMin = TUTORIAL;
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
		if (_isCampaignMode || _skirmishHeaderFilenames.size() > 0)
		{
			if (_uiTree.IsButtonColliding("playbutton", coord._pos.x, coord._pos.y))
			{
				_soundModule->Play("Assets/Sounds/page");

				std::string levelBinaryPath;
				if (_isCampaignMode)
				{
					levelBinaryPath = CAMPAIGN_FOLDER_PATH;
					//if we have selected the tutorial map
					if (_campaignSelection == TUTORIAL)
					{
						_tutorialState = TutorialState::NEWTUTORIAL;
					}
					{
						_tutorialState = TutorialState::NOTUTORIAL;
					}
				}
				else
				{
					levelBinaryPath = SKIRMISH_FOLDER_PATH;
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
				else if (!_isCampaignMode && _skirmishSelectedIndex != 0)
				{
					_skirmishSelectedIndex--;
					LoadLevelHeader(_skirmishHeaderFilenames[_skirmishSelectedIndex], &_selectedLevelHeader);
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
				else if (!_isCampaignMode && _skirmishSelectedIndex != _skirmishHeaderFilenames.size() - 1)
				{
					_skirmishSelectedIndex++;
					LoadLevelHeader(_skirmishHeaderFilenames[_skirmishSelectedIndex], &_selectedLevelHeader);
					SelectedLevelHeaderToGUI();
				}

				UpdateButtonsNextPreviousVisability();
			}
		}
		if (_isCampaignMode)
		{
			if (_uiTree.IsButtonColliding("SkirmishTab", coord._pos.x, coord._pos.y))
			{
				SwitchLevelSelectionMode();
				if (_skirmishHeaderFilenames.size() > 0)
				{
					_skirmishSelectedIndex = 0;
					LoadLevelHeader(_skirmishHeaderFilenames[_skirmishSelectedIndex], &_selectedLevelHeader);
					SelectedLevelHeaderToGUI();
				}
				else
				{
					_uiTree.GetNode("LevelSelectionTabContent")->SetHidden(true);
				}
				UpdateButtonsNextPreviousVisability();
			}
		}
		else
		{
			if (_uiTree.IsButtonColliding("CampaignTab", coord._pos.x, coord._pos.y))
			{
				_uiTree.GetNode("LevelSelectionTabContent")->SetHidden(false);
				SwitchLevelSelectionMode();
				_campaignSelection = _profile->_level;
				LoadLevelHeader(_campaignSelection, &_selectedLevelHeader);
				UpdateButtonsNextPreviousVisability();
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
	_uiTree.GetNode("LevelSelectionTabContent")->SetHidden(false);

	for (unsigned i = 0; i < _uiTree.GetNode("LevelSelectionTabs")->GetChildren()->size(); i++)
	{
		GUI::Node* modeSelect = _uiTree.GetNode("LevelSelectionTabs")->GetChildren()->at(i);

		modeSelect->SetPosition(_tabPosition[i]);
	}

	//Load level information into respektive variables.
	_skirmishHeaderFilenames.clear();
	GetFilenamesInDirectory(const_cast<char*>(SKIRMISH_FOLDER_PATH.c_str()), ".json", _skirmishHeaderFilenames, false);
	_campaignSelectionMax = _campaignSelection = _profile->_level;
	//if player havent completed the tutorial before. Show tutorial first.
	if (_profile->_firstTime && _campaignSelectionMin == TUTORIAL)
	{
		_campaignSelection = TUTORIAL;
	}
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
		levelPath = CAMPAIGN_FOLDER_PATH + levelFilename + ".json";
	}
	else
	{
		levelPath = SKIRMISH_FOLDER_PATH + levelFilename;
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
		if (_skirmishSelectedIndex > 0)
		{
			isHidden = false;
		}
		_previousLevelButtonNode->SetHidden(isHidden);

		isHidden = true;
		if (_skirmishSelectedIndex < _skirmishHeaderFilenames.size() - 1)
		{
			isHidden = false;
		}
		_nextLevelButtonNode->SetHidden(isHidden);
	}
}

void LevelSelectState::SwitchLevelSelectionMode()
{
	_isCampaignMode = !_isCampaignMode;

	DirectX::XMFLOAT2 _skirmishTabNodePos = _skirmishTabNode->GetLocalPosition();
	DirectX::XMFLOAT2 _campaignTabNodePos = _campaignTabNode->GetLocalPosition();
	float tempPosX = _skirmishTabNodePos.x;
	_skirmishTabNodePos.x = _campaignTabNodePos.x;
	_campaignTabNodePos.x = tempPosX;

	_skirmishTabNode->SetPosition(_skirmishTabNodePos);
	_campaignTabNode->SetPosition(_campaignTabNodePos);

	_levelNameNode->SetHidden(!_levelNameNode->GetHidden());
}
