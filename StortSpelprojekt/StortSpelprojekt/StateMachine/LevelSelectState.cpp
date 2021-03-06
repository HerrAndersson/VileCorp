#include "LevelSelectState.h"


LevelSelectState::LevelSelectState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule, CombinedMeshGenerator* combinedMeshGenerator) :
	BaseState(controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule),
	_selectedLevelHeader()
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

	_campaignSelectionMin = 0;
	_skirmishSelectedIndex = 1;

	for (unsigned i = 0; i < _uiTree.GetNode("LevelSelectionTabs")->GetChildren()->size(); i++)
	{
		GUI::Node* modeSelect = _uiTree.GetNode("LevelSelectionTabs")->GetChildren()->at(i);

		_tabPosition[i] = modeSelect->GetLocalPosition();
	}

	_combinedMeshGenerator = combinedMeshGenerator;

	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("nextlevel")));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("prevlevel")));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("playbutton")));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("CampaignTab")));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("SkirmishTab")));

	_profile = _settingsReader->GetProfile();
	_isCampaignMode = true;
	_campaignSelection = 0;
	_campaignSelectionMax = 0;

	_uiTree.GetNode("GameModeText")->SetHidden(true);
	_uiTree.GetNode("GameMode")->SetHidden(true);
}

LevelSelectState::~LevelSelectState()
{}

void LevelSelectState::Update(float deltaTime)
{
	if (_controls->IsFunctionKeyDown("DEBUG:RELOAD_GUI"))
	{
		_uiTree.ReloadTree("../../../../StortSpelprojekt/Assets/GUI/levelselect.json");
	}
	System::MouseCoord coord = _controls->GetMouseCoord();
	HandleButtonHighlight(coord);

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
				_soundModule->Play("page");

				std::string levelBinaryPath;
				if (_isCampaignMode)
				{
					_selectedLevelHeader._isCampaignMode = true;
					_selectedLevelHeader._campaignLevelIndex = _campaignSelection;
					levelBinaryPath = System::CAMPAIGN_FOLDER_PATH;
				}
				else
				{
					_selectedLevelHeader._isCampaignMode = false;
					levelBinaryPath = System::SKIRMISH_FOLDER_PATH;
				}
				levelBinaryPath += _selectedLevelHeader._levelBinaryFilename;

				_objectHandler->SetCurrentLevelHeader(_selectedLevelHeader);
				_objectHandler->LoadLevel(levelBinaryPath);


				//Combining objects into bigger meshes used for rendering to reduce draw calls
				_combinedMeshGenerator->Reset();
				_combinedMeshGenerator->CombineAndOptimizeMeshes(_objectHandler->GetTileMap(), System::FLOOR);
				_combinedMeshGenerator->CombineMeshes(_objectHandler->GetTileMap(), System::WALL);


				ChangeState(State::PLACEMENTSTATE);
			}
			if (_uiTree.IsButtonColliding("prevlevel", coord._pos.x, coord._pos.y))
			{
				_soundModule->Play("page");
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
				_soundModule->Play("page");
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
	GetFilenamesInDirectory(const_cast<char*>(System::SKIRMISH_FOLDER_PATH.c_str()), ".json", _skirmishHeaderFilenames, false);
	std::vector<std::string> campaignHeaderFilenames;
	GetFilenamesInDirectory(const_cast<char*>(System::CAMPAIGN_FOLDER_PATH.c_str()), ".json", campaignHeaderFilenames, false);
	
	if (_profile->_level > (int)campaignHeaderFilenames.size() - 1)
	{
		_campaignSelection = _campaignSelectionMax = _profile->_level = campaignHeaderFilenames.size() - 1;
		_settingsReader->ApplyProfileSettings();
	}
	else
	{
		_campaignSelection = _campaignSelectionMax = _profile->_level;
	}

	LoadLevelHeader(_campaignSelection, &_selectedLevelHeader);
	SelectedLevelHeaderToGUI();
	UpdateButtonsNextPreviousVisability();
}

void LevelSelectState::OnStateExit()
{}

void LevelSelectState::LoadLevelHeader(int levelFilename, Level::LevelHeader * headerToLoad)
{
	LoadLevelHeader(std::to_string(levelFilename), headerToLoad);
}

void LevelSelectState::LoadLevelHeader(const std::string& levelFilename, Level::LevelHeader * headerToLoad)
{
	std::string levelPath;
	if (_isCampaignMode)
	{
		levelPath = System::CAMPAIGN_FOLDER_PATH + levelFilename + ".json";
	}
	else
	{
		levelPath = System::SKIRMISH_FOLDER_PATH + levelFilename;
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
		_levelNameNode->SetText(System::StringToWstring(_selectedLevelHeader._levelBinaryFilename));
	}
	_storyTitleNode->SetText(System::StringToWstring(_selectedLevelHeader._storyTitle));
	_storyBodyNode->SetText(System::StringToWstring(_selectedLevelHeader._storyBody));

	std::stringstream ss;
	ss << _selectedLevelHeader._gameMode;
	int gameModeIndex;
	ss >> gameModeIndex;
	std::string gameModeName = Level::GameModeNames[gameModeIndex];
	_gameModeNode->SetText(System::StringToWstring(gameModeName));
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
		if (_skirmishSelectedIndex < (int)_skirmishHeaderFilenames.size() - 1)
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
