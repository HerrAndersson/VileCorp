#include "LevelEditState.h"

LevelEditState::LevelEditState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule, DirectX::XMFLOAT3* ambientLight)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule)
{
	_ambientLight = ambientLight;
	_baseEdit = nullptr;
}

LevelEditState::~LevelEditState()
{
	delete _baseEdit;
}

void LevelEditState::Update(float deltaTime)
{
	HandleCam(deltaTime);
	HandleInput();
	bool clickedOnGUI = HandleButtons();
	if (_noPlacementZoneToggleButton.GetIsToggled())
	{
		//TODO: No placement /Rikhard
	}
	else
	{
		if (!clickedOnGUI)
		{
			_baseEdit->Update();
		}
	}
}

void LevelEditState::OnStateEnter()
{
	_moveCheck = -1;
	_pageCheck = false;
	_leaveCheck = -1;
	_dialogWindowLock = false;
	_loadLevelSelectedIndex = 0;
	_currentPage = 0;

	_objectHandler->UnloadLevel();
	_objectHandler->SetTileMap(new Tilemap());

	_controls->ResetInputBuffers();

	_levelHeaderFilenames.clear();

	_uiTree.ReloadTree(System::LEVELEDIT_GUI_PATH);
	_objectTabs = _uiTree.GetNode("Buttons")->GetChildren();
	_settingsTabs = _uiTree.GetNode("Otherbuttons")->GetChildren();

	_currentList = nullptr;

	_currentlySelectedTextBox = nullptr;
	_storyTitleTextNode = _uiTree.GetNode("StoryTitleText");

	_textBoxesGeneral = std::vector<GUI::TextBox>();
	_textBoxesGeneral.push_back(GUI::TextBox(_storyTitleTextNode, 50, false, false, true));
	_storyTextNode = _uiTree.GetNode("StoryText");
	_textBoxesGeneral.push_back(GUI::TextBox(_storyTextNode, 850, true, false, true));
	_budgetTextNode = _uiTree.GetNode("BudgetBoxText");
	_textBoxesGeneral.push_back(GUI::TextBox(_budgetTextNode, 7, false, true));
	_textBoxesGeneral.push_back(GUI::TextBox(_uiTree.GetNode("tileNumberX"), 3, false, true, true));
	_textBoxesGeneral.push_back(GUI::TextBox(_uiTree.GetNode("tileNumberY"), 3, false, true, true));
	_textBoxesGeneral.push_back(GUI::TextBox(_uiTree.GetNode("MinuteText"), 3, false, true, true));
	_textBoxesGeneral.push_back(GUI::TextBox(_uiTree.GetNode("SecondText"), 3, false, true, true));

	_saveLevelNameTextNode = _uiTree.GetNode("SaveLevelText");
	_saveLevelNameTextBox = GUI::TextBox(_saveLevelNameTextNode, 25, false, false, true);
	_textBoxesGeneral.push_back(_saveLevelNameTextBox);

	_textBoxesSpawnSettings = std::vector<GUI::TextBox>();
	_textBoxesSpawnSettings.push_back(GUI::TextBox(_uiTree.GetNode("UnitAmmountText"), 2, false, true, true));
	_textBoxesSpawnSettings.push_back(GUI::TextBox(_uiTree.GetNode("UnitStartingTimeText"), 3, false, true, true));
	_textBoxesSpawnSettings.push_back(GUI::TextBox(_uiTree.GetNode("UnitSpawnFrequencyText"), 3, false, true, true));
	_selectedSpawnWaveNode = _uiTree.GetNode("WaveNumber");

	_spawnWaveTypeRadioButtons = GUI::RadioButtonCollection();
	GUI::Node* spawnTypesRadioButtonParentNode = _uiTree.GetNode("SpawnTypesRadioButton");
	_spawnTypesRadioButtonsNodes = spawnTypesRadioButtonParentNode->GetChildren();
	for each (GUI::Node* radioButton in *_spawnTypesRadioButtonsNodes)
	{
		_spawnWaveTypeRadioButtons.AddRadioButtonNode(radioButton);
	}
	_spawnWaveTypeRadioButtons.SelectRadioButtonNode(_spawnTypesRadioButtonsNodes->at(0));

	std::vector<std::vector<System::Blueprint*>>* blueprints = _objectHandler->GetBlueprintsOrderedByType();

	for (unsigned type = 0; type < System::NR_OF_TYPES; type++)
	{
		GUI::Node* currentTab = _uiTree.GetNode(_typeLists[type]);
		for (unsigned subType = 0; subType < blueprints->at(type).size(); subType++)
		{
			_uiTree.CreateBlueprintNodes(blueprints->at(type)[subType], currentTab);
		}
	}

	for (unsigned i = 0; i < _objectTabs->size(); i++)
	{
		_buttonPositions[i] = _objectTabs->at(i)->GetLocalPosition();
	}
	for (unsigned i = 0; i < _uiTree.GetNode("Otherbuttons")->GetChildren()->size(); i++)
	{
		_buttonPositions[i + _objectTabs->size()] = _uiTree.GetNode("Otherbuttons")->GetChildren()->at(i)->GetLocalPosition();
		_isPressed[i] = false;
	}

	_toggleButtons = std::vector<GUI::ToggleButton>();

	std::vector<GUI::Node*>* _unitLockButtons = _uiTree.GetNode("UnitLockButtons")->GetChildren();
	for (unsigned i = 0; i < _unitLockButtons->size(); i++)
	{
		_toggleButtons.push_back(GUI::ToggleButton(_unitLockButtons->at(i)));
	}
	_uiTree.GetRootNode()->SetPosition(_uiTree.GetRootNode()->GetFinalPosition());

	_noPlacementZoneToggleButton = GUI::ToggleButton(_uiTree.GetNode("NoPlacementButton"));

	_ambientLight->x = AMBIENT_LIGHT_DAY.x;
	_ambientLight->y = AMBIENT_LIGHT_DAY.y;
	_ambientLight->z = AMBIENT_LIGHT_DAY.z;

	_uiTree.GetNode("wholelist")->SetHidden(true);
	_uiTree.GetNode("SettingList")->SetHidden(true);
	_uiTree.GetNode("UnitLockToolTip")->SetHidden(true);
	_uiTree.GetNode("StoryList")->SetHidden(true);
	_uiTree.GetNode("ControlsList")->SetHidden(true);
	_uiTree.GetNode("MapSurviveSelected")->SetHidden(true);
	_uiTree.GetNode("ThiefControlList")->SetHidden(true);
	_uiTree.GetNode("NoPlacementButton")->SetHidden(true);
	_uiTree.GetNode("LeaveMap")->SetHidden(true);
	_uiTree.GetNode("listbuttons")->SetHidden(true);
	_uiTree.GetNode("ImportMapList")->SetHidden(true);
	_uiTree.GetNode("ExportMapList")->SetHidden(true);

	_levelHeader = Level::LevelHeader();
	_levelBinary = Level::LevelBinary();
	AddSpawnWaveAndSelectIt();

	_currentLevelFileName = "";
	_isNewLevel = true;

	_objectHandler->EnlargeTilemap(50);

	_baseEdit = new BaseEdit(_objectHandler, _controls, _pickingDevice, true);

	XMFLOAT3 campos;
	campos.x = (float)_objectHandler->GetTileMap()->GetWidth() / 2;
	campos.y = 15;
	campos.z = (float)_objectHandler->GetTileMap()->GetHeight() / 2 - 10;
	_camera->SetPosition(campos);

	_listId = -2;
	_settingsId = -2;
	for (int i = 0; i < 12; i++)
	{
		if (i < 7)
		{
			GUI::Node* currentButton = _uiTree.GetNode("Buttons")->GetChildren()->at(i);

			currentButton->SetPosition(_buttonPositions[i]);
		}
		else
		{
			GUI::Node* currentButton = _uiTree.GetNode("Otherbuttons")->GetChildren()->at(i - 7);

			currentButton->SetPosition(_buttonPositions[i]);
			_isPressed[i - 7] = false;
		}
	}

	GetFilenamesInDirectory(const_cast<char*>(System::SKIRMISH_FOLDER_PATH.c_str()), ".json", _levelHeaderFilenames, false);
	_loadLevelTextNode = _uiTree.GetNode("LoadedLevelText");
	if (_levelHeaderFilenames.size() > 0)
	{
		_loadLevelTextNode->SetText(System::StringToWstring(_levelHeaderFilenames[_loadLevelSelectedIndex]));
	}
}

void LevelEditState::OnStateExit()
{
	// Needs to be deallocated first because BaseEdit has GameObject pointers
	delete _baseEdit;
	_baseEdit = nullptr;

	_levelBinary = Level::LevelBinary();
	_levelHeader = Level::LevelHeader();
}

void LevelEditState::HandleInput()
{
	if (_controls->IsFunctionKeyDown("MENU:MENU"))
	{
		if (!_dialogWindowLock)
		{
			_uiTree.GetNode("LeaveMap")->SetHidden(false);
			_leaveCheck = 1;
		}
		_dialogWindowLock = true;
	}

	if (_controls->IsFunctionKeyDown("DEBUG:EXPORT_LEVEL"))
	{
		_uiTree.GetNode("ExportMapList")->SetHidden(false);
		_dialogWindowLock = true;
	}

	if (_controls->IsFunctionKeyDown("DEBUG:RELOAD_GUI"))
	{
		_uiTree.ReloadTree("../../../../StortSpelprojekt/Assets/GUI/leveledit.json");
	}
}

bool LevelEditState::HandleButtons()
{
	//Use this if there is enough objects to take up a whole page:
	//int currentFloorPage = 0;
	//_uiTree.GetNode("floorpage0")->SetHidden(true);

	System::MouseCoord coord = _controls->GetMouseCoord();
	bool clickedOnGUI = false;

	if (_currentlySelectedTextBox != nullptr)
	{
		if (_controls->GetIsTextInputMode())
		{
			_currentlySelectedTextBox->Update();
			_currentlySelectedTextBox->SetText(_controls->GetCurrentText());

			//User clicks outside of the textbox
			if (!_uiTree.IsButtonColliding(_currentlySelectedTextBox->GetBoxNode(), coord) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
			{
				_currentlySelectedTextBox->DeselectTextBox();
				_currentlySelectedTextBox = nullptr;
				_controls->ResetTextInputMode();
			}
		}
		else
		{
			_currentlySelectedTextBox->DeselectTextBox();
			_currentlySelectedTextBox = nullptr;
			_controls->ResetTextInputMode();
		}
	}
	if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		if (_dialogWindowLock)
		{
			clickedOnGUI = HandleButtonsForDialogWindows(coord, clickedOnGUI);
		}
		else
		{
			//Open Level editor placement GUI
			bool buttonClicked = false;
			for (unsigned i = 0; i < _objectTabs->size() && !buttonClicked; i++)
			{
				if (_uiTree.IsButtonColliding(_objectTabs->at(i), coord._pos.x, coord._pos.y))
				{
					clickedOnGUI = true;
					if (_listId == i)
					{
						_listId = -1;
					}
					else
					{
						_listId = i;
						_currentPage = 0;
					}
					_moveCheck = i;
					buttonClicked = true;
				}
			}
			if (_currentList != nullptr)
			{
				//Changing pages for objects
				if (_uiTree.IsButtonColliding("right", coord._pos.x, coord._pos.y))
				{
					clickedOnGUI = true;
					if ((int)_currentList->GetChildren()->size() - 1 > _currentPage)
					{
						_currentPage++;
						_pageCheck = true;
					}
				}
				else if (_uiTree.IsButtonColliding("left", coord._pos.x, coord._pos.y))
				{
					clickedOnGUI = true;
					if (_currentPage > 0)
					{
						_currentPage--;
						_pageCheck = true;
					}
				}
				//Need to go through "" json group to get to object buttons
				if (_currentList != nullptr && _currentList->GetChildren()->size())
				{
					buttonClicked = false;
					std::vector<GUI::Node*>* currentPageButtons = _currentList->GetChildren()->at(_currentPage)->GetChildren();
					for (unsigned y = 0; y < currentPageButtons->size(); y++)
					{
						GUI::Node* currentButton = currentPageButtons->at(y);
						if (_uiTree.IsButtonColliding(currentButton, coord._pos.x, coord._pos.y))
						{
							clickedOnGUI = true;
							GUI::BlueprintNode* currentBlueprintButton = static_cast<GUI::BlueprintNode*>(currentButton);
							_toPlace._blueprint = currentBlueprintButton->GetBlueprint();
							_toPlace._textureId = currentBlueprintButton->GetTextureId();

							_baseEdit->HandleBlueprint(&_toPlace);
							break;
						}
					}
				}
			}
			for (unsigned i = 0; i < _settingsTabs->size() - 3 && !buttonClicked; i++)
			{
				//Map/Unitlock data
				if (_uiTree.IsButtonColliding(_settingsTabs->at(i), coord._pos.x, coord._pos.y))
				{
					clickedOnGUI = true;
					if (_settingsId == i)
					{
						_settingsId = -1;
					}
					else
					{
						_settingsId = i;
					}
					_moveCheck = i;
					buttonClicked = true;
				}
			}
			//Lock Units for this level
			if (_settingsId == 1 && !clickedOnGUI)
			{
				for (unsigned i = 0; i < _toggleButtons.size() && !clickedOnGUI; i++)
				{
					GUI::ToggleButton* currentToggleButton = &_toggleButtons[i];
					GUI::Node* currentToggleButtonNode = currentToggleButton->GetAttachedGUINode();
					if (!_uiTree.IsNodeHidden(currentToggleButtonNode) && _uiTree.IsButtonColliding(currentToggleButtonNode, coord))
					{
						clickedOnGUI = true;
						currentToggleButton->Toggle();
					}
				}
			}
			else if (_uiTree.IsButtonColliding("ExportMap", coord._pos.x, coord._pos.y) && !_uiTree.IsNodeHidden("ExportMap"))
			{
				clickedOnGUI = true;
				_uiTree.GetNode("ExportMapList")->SetHidden(false);
				_dialogWindowLock = true;

			}
			else if (_uiTree.IsButtonColliding("ImportMap", coord._pos.x, coord._pos.y) && !_uiTree.IsNodeHidden("ImportMap"))
			{
				clickedOnGUI = true;
				//Is only supposed to bring up the Import GUI
				_uiTree.GetNode("ImportMapList")->SetHidden(false);
				_dialogWindowLock = true;
			}
			else if (_uiTree.IsButtonColliding("NewMap", coord._pos.x, coord._pos.y) && !_uiTree.IsNodeHidden("NewMap"))
			{
				clickedOnGUI = true;
				//Is only supposed to bring up the confirmation GUI
				_uiTree.GetNode("LeaveMap")->SetHidden(false);
				_leaveCheck = 0;
				_dialogWindowLock = true;
			}
			//Story data
			if (_uiTree.IsButtonColliding("StoryButton", coord._pos.x, coord._pos.y))
			{
				clickedOnGUI = true;
				if (!_isPressed[2])
				{
					//Move Button
					GUI::Node* node = _uiTree.GetNode("StoryButton");
					XMFLOAT2 move = _buttonPositions[9];
					move.x = move.x - 0.404f;

					node->SetPosition(move);

					//Show List
					_uiTree.GetNode("StoryList")->SetHidden(false);

					_isPressed[2] = true;
				}
				else
				{
					//Move Button
					GUI::Node* node = _uiTree.GetNode("StoryButton");
					node->SetPosition(_buttonPositions[9]);

					//Hide List
					_uiTree.GetNode("StoryList")->SetHidden(true);

					_isPressed[2] = false;
				}
			}
			//Control data
			else if (_uiTree.IsButtonColliding("ControlsButton", coord._pos.x, coord._pos.y))
			{
				clickedOnGUI = true;
				if (!_isPressed[3])
				{
					//Move Button
					GUI::Node* node = _uiTree.GetNode("ControlsButton");
					XMFLOAT2 move = _buttonPositions[10];
					move.x -= 0.275f;
					move.y -= 1.075f;

					//Show List
					_uiTree.GetNode("ControlsList")->SetHidden(false);

					node->SetPosition(move);
					_isPressed[3] = true;
				}
				else
				{
					//Move gui
					GUI::Node* node = _uiTree.GetNode("ControlsButton");
					node->SetPosition(_buttonPositions[10]);

					//Show List
					_uiTree.GetNode("ControlsList")->SetHidden(true);

					_isPressed[3] = false;
				}
			}
			//Thief data
			else if (_uiTree.IsButtonColliding("ThiefButton", coord._pos.x, coord._pos.y))
			{
				clickedOnGUI = true;
				if (!_isPressed[4])
				{
					//Move Button
					GUI::Node* node = _uiTree.GetNode("ThiefButton");
					XMFLOAT2 move = _buttonPositions[11];
					move.x += 0.449f;

					//Show List
					_uiTree.GetNode("ThiefControlList")->SetHidden(false);

					node->SetPosition(move);
					_isPressed[4] = true;
				}
				else
				{
					//Move gui
					GUI::Node* node = _uiTree.GetNode("ThiefButton");
					node->SetPosition(_buttonPositions[11]);

					//Show List
					_uiTree.GetNode("ThiefControlList")->SetHidden(true);

					_isPressed[4] = false;
				}
			}
			else if (_uiTree.IsButtonColliding(_noPlacementZoneToggleButton.GetAttachedGUINode(), coord) && !_uiTree.IsNodeHidden(_noPlacementZoneToggleButton.GetAttachedGUINode()))
			{
				_noPlacementZoneToggleButton.Toggle();
			}
			clickedOnGUI = HandleButtonsForSpawn(coord, clickedOnGUI);
			if (!clickedOnGUI)
			{
				if (_uiTree.IsButtonColliding("GridOff", coord._pos.x, coord._pos.y) && _isPressed[0] == true)
				{
					clickedOnGUI = true;
					//Move Button
					GUI::Node* node = _uiTree.GetNode("GridOff");
					XMFLOAT2 gridOffPosition = node->GetLocalPosition();

					GUI::Node* node2 = _uiTree.GetNode("GridOn");
					XMFLOAT2 gridOnPosition = node2->GetLocalPosition();

					node2->SetPosition(gridOffPosition);
					node->SetPosition(gridOnPosition);
				}
				else if (_uiTree.IsButtonColliding("ObjectiveOff", coord._pos.x, coord._pos.y) && _isPressed[0] == true)
				{
					clickedOnGUI = true;
					//Move Button
					GUI::Node* node = _uiTree.GetNode("ObjectiveOff");
					XMFLOAT2 objectiveOffPosition = node->GetLocalPosition();

					GUI::Node* node2 = _uiTree.GetNode("ObjectiveOn");
					XMFLOAT2 objectiveOnPosition = node2->GetLocalPosition();

					node2->SetPosition(objectiveOffPosition);
					node->SetPosition(objectiveOnPosition);

					if (objectiveOnPosition.y > objectiveOffPosition.y)
					{
						//Survival
						_uiTree.GetNode("MapSurviveSelected")->SetHidden(false);
						_levelHeader._gameMode = Level::GameModes::SURVIVAL;
					}
					else
					{
						//Kill
						_uiTree.GetNode("MapSurviveSelected")->SetHidden(true);
						_levelHeader._gameMode = Level::GameModes::KILL_THEM_ALL;
					}
				}
				else
				{
					for (int i = 0; i < _textBoxesGeneral.size(); i++)
					{
						clickedOnGUI = SelectTextBox(&_textBoxesGeneral[i], coord, clickedOnGUI);
					}
				}

			}
		}
	}

	if (_listId == -1)
	{
		//Hide Gui
		_currentList = nullptr;
		_currentPage = 0;
		_uiTree.GetNode("wholelist")->SetHidden(true);
		_uiTree.GetNode("listbuttons")->SetHidden(true);
		_uiTree.GetNode("NoPlacementButton")->SetHidden(true);
		for (unsigned i = 0; i < _uiTree.GetNode("wholelist")->GetChildren()->size(); i++)
		{
			//Move gui
			GUI::Node* node = _objectTabs->at(i);
			node->SetPosition(_buttonPositions[i]);
		}
		_listId = -2;
	}
	else if (_listId >= 0 && _moveCheck != -2)
	{
		//Show Gui
		_uiTree.GetNode("wholelist")->SetHidden(false);
		for (unsigned i = 0; i < _uiTree.GetNode("wholelist")->GetChildren()->size(); i++)
		{
			//Move gui
			GUI::Node* node = _objectTabs->at(i);
			XMFLOAT2 move = _buttonPositions[i];
			move.x = move.x + 0.414f;
			XMFLOAT2 moveShort = move;
			moveShort.x -= 0.015f;

			node->SetPosition(_listId == i ? move : moveShort);

			if (_listId != i)
			{
				_uiTree.GetNode("wholelist")->GetChildren()->at(i)->SetHidden(true);
			}
			else
			{
				_currentList = _uiTree.GetNode("wholelist")->GetChildren()->at(i);
				_currentList->SetHidden(false);
				if (_currentList->GetChildren()->size() > 1)
				{
					_uiTree.GetNode("listbuttons")->SetHidden(false);
				}
				else
				{
					_uiTree.GetNode("listbuttons")->SetHidden(true);
				}
				for (unsigned j = 0; j < _currentList->GetChildren()->size(); j++)
				{
					if (j != _currentPage)
					{
						_currentList->GetChildren()->at(j)->SetHidden(true);
					}
					_currentList->GetChildren()->at(_currentPage)->SetHidden(false);
				}
				_uiTree.GetNode("NoPlacementButton")->SetHidden(false);
			}
		}
		_moveCheck = -2;
	}
	else if (_pageCheck == true)
	{
		for (unsigned j = 0; j < _currentList->GetChildren()->size(); j++)
		{
			if (j != _currentPage)
			{
				_currentList->GetChildren()->at(j)->SetHidden(true);
			}
			_currentList->GetChildren()->at(_currentPage)->SetHidden(false);
		}
		_pageCheck = false;
	}

	if (_settingsId == -1)
	{
		//Hide Gui
		_uiTree.GetNode("SettingList")->SetHidden(true);
		for (unsigned i = 0; i < _uiTree.GetNode("Otherbuttons")->GetChildren()->size() - 3; i++)
		{
			//Move gui
			GUI::Node* node = _uiTree.GetNode("Otherbuttons")->GetChildren()->at(i);
			node->SetPosition(_buttonPositions[i + 7]);
			_isPressed[i] = false;
		}
		_settingsId = -2;
	}
	else if (_settingsId >= 0 && _moveCheck != -3)
	{
		////Show Gui
		_uiTree.GetNode("SettingList")->SetHidden(false);
		for (unsigned i = 0; i < _uiTree.GetNode("Otherbuttons")->GetChildren()->size() - 3; i++)
		{
			//Move gui
			GUI::Node* node = _uiTree.GetNode("Otherbuttons")->GetChildren()->at(i);
			XMFLOAT2 move = _buttonPositions[i + 7];
			move.x = move.x - 0.296f;
			XMFLOAT2 moveShort = move;
			moveShort.x += 0.01f;

			node->SetPosition(_settingsId == i ? move : moveShort);

			if (_settingsId != i)
			{
				_uiTree.GetNode("SettingList")->GetChildren()->at(i)->SetHidden(true);
				_isPressed[i] = false;
			}
			else
			{
				_uiTree.GetNode("SettingList")->GetChildren()->at(i)->SetHidden(false);
				_isPressed[i] = true;
			}
		}
		_moveCheck = -3;
	}

	if (_settingsId == 1)
	{
		//Need a means of hiding UnitLockToolTip when not holding over a button
		_uiTree.GetNode("UnitLockToolTip")->SetHidden(false);
		for (int i = 0; i < _uiTree.GetNode("UnitLockButtons")->GetChildren()->size(); i++)
		{
			if (_uiTree.IsButtonColliding(_uiTree.GetNode("UnitLockButtons")->GetChildren()->at(i)->GetId(), coord._pos.x, coord._pos.y))
			{
				_uiTree.GetNode("UnitLockToolTip")->GetChildren()->at(i)->SetHidden(false);
			}
			else
			{
				_uiTree.GetNode("UnitLockToolTip")->GetChildren()->at(i)->SetHidden(true);
			}
		}
	}
	return clickedOnGUI;
}

bool LevelEditState::HandleButtonsForDialogWindows(System::MouseCoord &coord, bool clickedOnGUI)
{
	//Export, Import or make a new Map
	if (!clickedOnGUI)
	{
		clickedOnGUI = SelectTextBox(&_saveLevelNameTextBox, coord, clickedOnGUI);
		//Export Map Functions
		if (_uiTree.IsButtonColliding("ExportYes", coord._pos.x, coord._pos.y) && !_uiTree.IsNodeHidden("ExportYes"))
		{
			clickedOnGUI = true;
			_uiTree.GetNode("ExportMapList")->SetHidden(true);
			ExportLevel();

			_dialogWindowLock = false;
		}
		else if (_uiTree.IsButtonColliding("ExportNo", coord._pos.x, coord._pos.y) && !_uiTree.IsNodeHidden("ExportNo"))
		{
			//Only close down Export GUI
			clickedOnGUI = true;
			_uiTree.GetNode("ExportMapList")->SetHidden(true);
			_dialogWindowLock = false;
		}
		//Import Map Functions
		else if (_uiTree.IsButtonColliding("ImportYes", coord._pos.x, coord._pos.y) && !_uiTree.IsNodeHidden("ImportYes"))
		{
			clickedOnGUI = true;
			if (_levelHeaderFilenames.size() > 0)
			{
				_uiTree.GetNode("ImportMapList")->SetHidden(true);
				LoadLevel(_levelHeaderFilenames[_loadLevelSelectedIndex]);
			}
			_dialogWindowLock = false;
		}
		else if (_uiTree.IsButtonColliding("ImportNo", coord._pos.x, coord._pos.y) && !_uiTree.IsNodeHidden("ImportNo"))
		{
			//Only close down Import GUI
			clickedOnGUI = true;
			_uiTree.GetNode("ImportMapList")->SetHidden(true);
			_dialogWindowLock = false;
		}
		else if (_uiTree.IsButtonColliding("LevelUp", coord) && !_uiTree.IsNodeHidden("LevelUp"))
		{
			clickedOnGUI = true;
			if (_loadLevelSelectedIndex != 0)
			{
				_loadLevelSelectedIndex--;
				_loadLevelTextNode->SetText(System::StringToWstring(_levelHeaderFilenames[_loadLevelSelectedIndex]));
			}
		}
		else if (_uiTree.IsButtonColliding("LevelDown", coord) && !_uiTree.IsNodeHidden("LevelDown"))
		{
			clickedOnGUI = true;
			if ((_loadLevelSelectedIndex < _levelHeaderFilenames.size()))
			{
				_loadLevelSelectedIndex++;
				_loadLevelTextNode->SetText(System::StringToWstring(_levelHeaderFilenames[_loadLevelSelectedIndex]));
			}
		}
		//New Map Functions
		else if (_uiTree.IsButtonColliding("LeaveMapYes", coord._pos.x, coord._pos.y) && !_uiTree.IsNodeHidden("LeaveMapYes"))
		{
			clickedOnGUI = true;
			if (_leaveCheck == 0) //Reset level editor
			{
				ResetLevel();
			}
			else //Exit level editor
			{
				_leaveCheck = -1;
				ChangeState(MENUSTATE);
			}
			_dialogWindowLock = false;
		}
		else if (_uiTree.IsButtonColliding("LeaveMapNo", coord._pos.x, coord._pos.y))
		{
			clickedOnGUI = true;
			_uiTree.GetNode("LeaveMap")->SetHidden(true);
			_leaveCheck = -1;
			_dialogWindowLock = false;
		}
	}
	return clickedOnGUI;
}

void LevelEditState::ResetLevel()
{
	_uiTree.GetNode("LeaveMap")->SetHidden(true);
	_leaveCheck = -1;

	delete _baseEdit;
	_baseEdit = nullptr;

	_levelBinary = Level::LevelBinary();
	_levelHeader = Level::LevelHeader();

	_objectHandler->UnloadLevel();
	_objectHandler->SetTileMap(new Tilemap(AI::Vec2D(110,110)));

	for (unsigned i = 0; i < _toggleButtons.size(); i++)
	{
		_toggleButtons[i].SetIsToggled(false);
	}

	AddSpawnWaveAndSelectIt();
	ShowSelectedSpawnWave();

	_budgetTextNode->SetText(std::to_wstring(500));
	std::wstring defaultStoryText = L"Write great and awesome stories here!:D";
	_storyTextNode->SetText(defaultStoryText);
	_storyTitleTextNode->SetText(defaultStoryText);

	_currentLevelFileName = "";
	_isNewLevel = true;

	_baseEdit = new BaseEdit(_objectHandler, _controls, _pickingDevice, true);
}

bool LevelEditState::HandleButtonsForSpawn(System::MouseCoord &coord, bool clickedOnGUI)
{
	std::vector<GUI::Node*>* notSelectedRadioButtonNodes = _spawnWaveTypeRadioButtons.GetNotSelectedRadioButtonNodes();
	for (int i = 0; i < notSelectedRadioButtonNodes->size() && !clickedOnGUI; i++)
	{
		if (!_uiTree.IsNodeHidden(notSelectedRadioButtonNodes->at(i)) && _uiTree.IsButtonColliding(notSelectedRadioButtonNodes->at(i), coord))
		{
			_spawnWaveTypeRadioButtons.SelectRadioButtonNode(notSelectedRadioButtonNodes->at(i));
			SaveCurrentSpawnWave();
			clickedOnGUI = true;
		}
	}
	if (!clickedOnGUI)
	{
		if (_uiTree.IsButtonColliding("WaveAdd", coord._pos.x, coord._pos.y) && _isPressed[4] == true)
		{
			clickedOnGUI = true;
			SaveCurrentSpawnWave();
			AddSpawnWaveAndSelectIt();
			ShowSelectedSpawnWave();
		}
		else if (_uiTree.IsButtonColliding("WaveDelete", coord._pos.x, coord._pos.y) && _isPressed[4] == true)
		{
			clickedOnGUI = true;
			if (RemoveSelectedSpawnWave())
			{
				ShowSelectedSpawnWave();
			}
		}
		else if (_uiTree.IsButtonColliding("WaveLeft", coord._pos.x, coord._pos.y) && _isPressed[4] == true)
		{
			clickedOnGUI = true;
			if (_selectedSpawnWave > 0)
			{
				SaveCurrentSpawnWave();
				_selectedSpawnWave--;
				ShowSelectedSpawnWave();
			}
		}
		else if (_uiTree.IsButtonColliding("WaveRight", coord._pos.x, coord._pos.y) && _isPressed[4] == true)
		{
			clickedOnGUI = true;
			if (_selectedSpawnWave < _levelBinary._enemyWavesGUIData.size() - 1)
			{
				SaveCurrentSpawnWave();
				_selectedSpawnWave++;
				ShowSelectedSpawnWave();
			}
		}
		else
		{
			for (int i = 0; i < _textBoxesSpawnSettings.size() && !clickedOnGUI; i++)
			{
				clickedOnGUI = SelectTextBox(&_textBoxesSpawnSettings[i], coord, clickedOnGUI);
			}
		}
	}
	return clickedOnGUI;
}

void LevelEditState::SaveCurrentSpawnWave()
{
	GUI::Node* selectedSpawnTypeNode = _spawnWaveTypeRadioButtons.GetSelectedRadioButtonNode();
	std::string selectedSpawnTypeString = selectedSpawnTypeNode->GetId();
	int selectedSpawnType;
	if (selectedSpawnTypeString == "BurglarRadioButton")
	{
		selectedSpawnType = 0;
	}
	else if (selectedSpawnTypeString == "DemolisherRadioButton")
	{
		selectedSpawnType = 1;
	}
	else if (selectedSpawnTypeString == "AssassinRadioButton")
	{
		selectedSpawnType = 2;
	}
	else
	{
		selectedSpawnType = -1;
	}
	if (selectedSpawnType != -1)
	{
		_levelBinary._enemyWavesGUIData[_selectedSpawnWave][0] = selectedSpawnType;
		_levelBinary._enemyWavesGUIData[_selectedSpawnWave][1] = std::stoi(_textBoxesSpawnSettings[0].GetText());
		_levelBinary._enemyWavesGUIData[_selectedSpawnWave][2] = std::stoi(_textBoxesSpawnSettings[1].GetText());
		_levelBinary._enemyWavesGUIData[_selectedSpawnWave][3] = std::stoi(_textBoxesSpawnSettings[2].GetText());
	}
}

bool LevelEditState::SelectTextBox(GUI::TextBox* textBox, System::MouseCoord & coord, bool clickedOnGUI)
{
	GUI::Node* boxNode = textBox->GetBoxNode();

	//User clicked the textbox while it was visible
	if (_uiTree.IsButtonColliding(boxNode, coord) && !_uiTree.IsNodeHidden(boxNode) && _currentlySelectedTextBox != textBox)
	{
		clickedOnGUI = true;
		textBox->SelectTextBox();
		_currentlySelectedTextBox = textBox;
		_controls->SetIsTextInputMode(_currentlySelectedTextBox->GetText(), true, !_currentlySelectedTextBox->GetAllowMultipleLines(), true, _currentlySelectedTextBox->GetCharacterLimit(), _currentlySelectedTextBox->GetOnlyNumbersAllowed());
	}
	return clickedOnGUI;
}

bool LevelEditState::RemoveSelectedSpawnWave() //Returns true if removed
{
	if (_levelBinary._enemyWavesGUIData.size() > 1)
	{
		_levelBinary._enemyWavesGUIData.erase(_levelBinary._enemyWavesGUIData.begin() + _selectedSpawnWave);
		_selectedSpawnWave--;
		if (_selectedSpawnWave < 0)
		{
			_selectedSpawnWave = 0;
		}
		return true;
	}
	return false;
}

void LevelEditState::AddSpawnWaveAndSelectIt()
{
	_levelBinary._enemyWavesGUIData.push_back(std::vector<int>{0, 0, 0, 0});
	_selectedSpawnWave = _levelBinary._enemyWavesGUIData.size() - 1;
}

void LevelEditState::ShowSelectedSpawnWave()
{
	_selectedSpawnWaveNode->SetText(std::to_wstring(_selectedSpawnWave + 1));
	_spawnWaveTypeRadioButtons.SelectRadioButtonNode(_spawnTypesRadioButtonsNodes->at(_levelBinary._enemyWavesGUIData[_selectedSpawnWave][0]));
	_textBoxesSpawnSettings[0].SetText(std::to_wstring(_levelBinary._enemyWavesGUIData[_selectedSpawnWave][1]));
	_textBoxesSpawnSettings[1].SetText(std::to_wstring(_levelBinary._enemyWavesGUIData[_selectedSpawnWave][2]));
	_textBoxesSpawnSettings[2].SetText(std::to_wstring(_levelBinary._enemyWavesGUIData[_selectedSpawnWave][3]));
}

void LevelEditState::ExportLevel()
{
	try
	{
		SaveCurrentSpawnWave();
		_baseEdit->RemoveGhostImage();

		_currentLevelFileName = System::WStringToString(_saveLevelNameTextNode->GetText());

		////Fill Level Header:

		//Getting Story Information
		_levelHeader._storyTitle = System::WStringToString(_uiTree.GetNode("StoryTitleText")->GetText());
		_levelHeader._storyBody = System::WStringToString(_uiTree.GetNode("StoryText")->GetText());

		//Get Budget Information
		std::wstringstream wss = std::wstringstream();
		wss << _uiTree.GetNode("BudgetBoxText")->GetText();
		wss >> _levelHeader._budget;

		//Survival time 
		if (_levelHeader._gameMode == Level::GameModes::SURVIVAL)
		{
			int survivalMinutes = 0;
			int survivalSeconds = 0;
			wss << _uiTree.GetNode("MinuteText")->GetText();
			wss >> survivalMinutes;
			wss << _uiTree.GetNode("SecondBox")->GetText();
			wss >> survivalSeconds;

			_levelHeader._surviveForSeconds = (survivalMinutes * 60) + survivalSeconds;
		}

		//Get Level Binary file name
		_levelHeader._levelBinaryFilename = _currentLevelFileName + ".bin";

		////Fill Level Binary:
		//Tilemap size
		Tilemap* tileMap = _objectHandler->GetTileMap();
		_levelBinary._tileMapSizeX = tileMap->GetWidth();
		_levelBinary._tileMapSizeZ = tileMap->GetHeight();

		//Game objects
		std::vector<std::vector<GameObject*>>* gameObjects = _objectHandler->GetGameObjects();
		_levelBinary._gameObjectData.resize(_objectHandler->GetObjectCount());
		int gameObjectIndex = 0;
		for (uint i = 0; i < gameObjects->size(); i++)
		{
			for (GameObject* gameObject : gameObjects->at(i))
			{
				std::vector<int>* formattedGameObject = &_levelBinary._gameObjectData[gameObjectIndex];
				formattedGameObject->resize(6);

				//Type
				int type = formattedGameObject->at(0) = gameObject->GetType();

				//Sub type 
				int subType = formattedGameObject->at(1) = gameObject->GetSubType();

				//Texture ID
				System::Blueprint* blueprint = _objectHandler->GetBlueprintByType(type, subType);
				std::string textureName = gameObject->GetRenderObject()->_diffuseTexture->_name;
				formattedGameObject->at(2) = 0;
				bool foundTexture = false;
				for (int i = 0; !foundTexture && i < blueprint->_textures.size(); i++)
				{
					if (blueprint->_textures[i] == textureName)
					{
						formattedGameObject->at(2) = i;
						foundTexture = true;
					}
				}

				//Position
				AI::Vec2D position = gameObject->GetTilePosition();
				formattedGameObject->at(3) = static_cast<int>(position._x);
				formattedGameObject->at(4) = static_cast<int>(position._y);

				//Rotation
				float rotYRadians = gameObject->GetRotation().y;
				int rotYDegrees = static_cast<int>((rotYRadians / DirectX::XM_PI) * 180.0);
				formattedGameObject->at(5) = rotYDegrees;

				gameObjectIndex++;
			}
		}

		//Convert spawn wave data to spawn wave map

		for (int i = 0; i < _levelBinary._enemyWavesGUIData.size(); i++)
		{
			std::vector<int>* spawnWave = &_levelBinary._enemyWavesGUIData[i];
			int enemyType = spawnWave->at(0);
			int nrOfEnemies = spawnWave->at(1);
			int startingTime = spawnWave->at(2);
			int spawnFrequency = spawnWave->at(3);

			for (int enemyIterator = 0; enemyIterator < nrOfEnemies; enemyIterator++)
			{
				std::array<int, 2> singleSpawn = { startingTime + (spawnFrequency * enemyIterator), enemyType };
				_levelBinary._enemyOrderedSpawnVector.push_back(singleSpawn);
			}
		}

		std::sort(_levelBinary._enemyOrderedSpawnVector.begin(), _levelBinary._enemyOrderedSpawnVector.end(), [](std::array<int, 2> a, std::array<int, 2> b)
		{
			return b[0] > a[0];
		});

		//Save available units
		GUI::ToggleButton* currentToggleButton;
		GUI::Node* currentToggleNode;
		for (unsigned i = 0; i < _toggleButtons.size(); i++)
		{
			currentToggleButton = &_toggleButtons.at(i);
			if (!currentToggleButton->GetIsToggled())
			{
				currentToggleNode = currentToggleButton->GetAttachedGUINode();

				_levelBinary._availableUnits.push_back(currentToggleNode->GetId());
			}
		}

		////Write the files

		//Construct a suitable path for the level
		std::string levelPath;
#ifdef _DEBUG
		char userPath[MAX_PATH];
		SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, userPath);

		levelPath = userPath;
		levelPath += "\\Google Drive\\Stort spelprojekt\\" + System::SKIRMISH_FOLDER_PATH;
#else
		levelPath = System::SKIRMISH_FOLDER_PATH;
#endif
		CreateDirectory(levelPath.c_str(), NULL);

		//Make the individual file paths
		std::string headerPath, binaryPath;
		headerPath = binaryPath = levelPath + _currentLevelFileName;
		headerPath += ".json";
		binaryPath += ".bin";

		std::ofstream outStream;

		//Write the header
		outStream.open(headerPath);
		{
			cereal::JSONOutputArchive jsonOut(outStream);
			jsonOut(cereal::make_nvp(_currentLevelFileName.c_str(), _levelHeader));
		}
		outStream.close();

		//Write the binary
		outStream.open(binaryPath, std::ios::binary);
		{
			cereal::BinaryOutputArchive binOut(outStream);
			binOut(_levelBinary);
		}
		outStream.close();
	}
	catch (...)
	{
		int i = 0;
	}


}

void LevelEditState::LoadLevel(std::string headerFileName)
{
	_levelBinary = Level::LevelBinary();
	_levelHeader = Level::LevelHeader();

	_objectHandler->UnloadLevel();

	std::string levelPath;
#ifdef _DEBUG
	char userPath[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, userPath);

	levelPath = userPath;
	levelPath += "\\Google Drive\\Stort spelprojekt\\" + System::SKIRMISH_FOLDER_PATH;
#else
	levelPath = System::SKIRMISH_FOLDER_PATH;
#endif

	System::loadJSON(&_levelHeader, levelPath + headerFileName);

	bool result = false;
	HRESULT success = _assetManager->ParseLevelBinary(&_levelBinary, levelPath + _levelHeader._levelBinaryFilename);

	result = _objectHandler->LoadLevel(_levelBinary);
	_baseEdit->RefreshTileMap();
	
	_budgetTextNode->SetText(std::to_wstring(_levelHeader._budget));
	_storyTitleTextNode->SetText(System::StringToWstring(_levelHeader._storyTitle));
	_storyTextNode->SetText(System::StringToWstring(_levelHeader._storyBody));

	for (unsigned i = 0; i < _toggleButtons.size(); i++)
	{
		_toggleButtons[i].SetIsToggled(true);
	}
	for (unsigned i = 0; i < _toggleButtons.size(); i++)
	{
		std::string currentToggleNodeID = _toggleButtons[i].GetAttachedGUINode()->GetId();
		std::vector<std::string>::iterator result = std::find(_levelBinary._availableUnits.begin(), _levelBinary._availableUnits.end(), currentToggleNodeID);

		if (result != _levelBinary._availableUnits.end())
		{
			_toggleButtons[i].SetIsToggled(false);
		}
	}

	_selectedSpawnWave = 0;
	ShowSelectedSpawnWave();
}
