#include "LevelEditState.h"

LevelEditState::LevelEditState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule)
{
	_listId = -1;
	_moveCheck = -1;
	_baseEdit = nullptr;
	_pageCheck = false;
	_leaveCheck = -1;

	_objectTabs = _uiTree.GetNode("Buttons")->GetChildren();
	_settingsTabs = _uiTree.GetNode("Otherbuttons")->GetChildren();

	std::map<Type, std::string> typeLists =
	{
		{ FLOOR, "floorlist" },
		{ WALL, "walllist" },
		{ LOOT, "objectivelist" },
		{ SPAWN, "entrylist" },
		{ TRAP, "traplist" },
		{ CAMERA, "traplist" },
		{ GUARD, "unitlist" },
		{ ENEMY, "unitlist" },
		{ FURNITURE, "decorationlist" }
	};
	for (unsigned i = 0; i < NR_OF_TYPES; i++)
	{
		int index = 0;
		vector<Blueprint>* blueprints = _objectHandler->GetBlueprints();
		for (unsigned b = 0; b < blueprints->size();b++)
		{
			if (blueprints->at(b)._type == i)
			{
				index += _uiTree.CreateTilesetObject(&blueprints->at(b), _uiTree.GetNode(typeLists[(Type)blueprints->at(b)._type]), index);
			}
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
	for (unsigned i = 0; i < 7; i++)
	{
		_OrginLockColor[i] = _uiTree.GetNode("UnitLockButtons")->GetChildren()->at(i)->GetColorOffset();
	}
	_uiTree.GetRootNode()->SetPosition(_uiTree.GetRootNode()->GetFinalPosition());
}

LevelEditState::~LevelEditState()
{
	delete _baseEdit;
}

void LevelEditState::Update(float deltaTime)
{
	if (_controls->IsFunctionKeyDown("MAP_EDIT:PLACEMENTFLAG"))
	{
		_baseEdit->ChangePlaceState();
	}
	_baseEdit->Update(deltaTime);
	HandleInput();
	HandleButtons();
}

void LevelEditState::OnStateEnter()
{
	_objectHandler->DisableSpawnPoints();
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

	_levelHeader = Level::LevelHeader();
	_currentLevelFileName = "";
	_isNewLevel = true;

	_objectHandler->EnlargeTilemap(50);

	_baseEdit = new BaseEdit(_objectHandler, _controls, _pickingDevice, _camera, true);

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
			GUI::Node* currentButton = _uiTree.GetNode("Otherbuttons")->GetChildren()->at(i-7);

			currentButton->SetPosition(_buttonPositions[i]);
			_isPressed[i - 7] = false;
		}
	}
	for (unsigned i = 0; i < 7; i++)
	{
		_isLocked[i] = false;
	}
}

void LevelEditState::OnStateExit()
{
	_objectHandler->MinimizeTileMap();
	//TODO: Remove this function to LevelSelection when that state is created. /Alex
	_objectHandler->UnloadLevel();
	delete _baseEdit;
	_baseEdit = nullptr;
}

void LevelEditState::HandleInput()
{
	//Press C to init new level
	if (_controls->IsFunctionKeyDown("MAP_EDIT:NEWLEVEL"))
	{
		_objectHandler->UnloadLevel();
	}

	if (_controls->IsFunctionKeyDown("MENU:MENU"))
	{
		_uiTree.GetNode("LeaveMap")->SetHidden(false);
		_leaveCheck = 1;
	}

	if (_controls->IsFunctionKeyDown("DEBUG:EXPORT_LEVEL"))
	{
		ExportLevel();
	}

	if (_controls->IsFunctionKeyDown("DEBUG:RELOAD_GUI"))
	{
		_uiTree.ReloadTree("../../../../StortSpelprojekt/Assets/gui.json", "LEVELEDIT");
	}
}

void LevelEditState::HandleButtons()
{
	//Use this if there is enough objects to take up a whole page:
	//int currentFloorPage = 0;
	//_uiTree.GetNode("floorpage0")->SetHidden(true);

	System::MouseCoord coord = _controls->GetMouseCoord();


	if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		//Open Level editor placement GUI
		bool buttonClicked = false;
		for (unsigned i = 0; i < _objectTabs->size() && !buttonClicked; i++)
		{
			if (_uiTree.IsButtonColliding(_objectTabs->at(i), coord._pos.x, coord._pos.y))
			{
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
				if ((int)_currentList->GetChildren()->size() - 1 > _currentPage)
				{
					_currentPage++;
					_pageCheck = true;
				}
			}
			else if (_uiTree.IsButtonColliding("left", coord._pos.x, coord._pos.y))
			{
				if (_currentPage > 0)
				{
					_currentPage--;
					_pageCheck = true;
				}
			}
			else if (_uiTree.IsButtonColliding("NoPlacementButton", coord._pos.x, coord._pos.y))
			{
				//TODO Paint No Placement Zones! Julia and Enbom
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
						GUI::BlueprintNode* currentBlueprintButton = static_cast<GUI::BlueprintNode*>(currentButton);
						_toPlace._blueprint = currentBlueprintButton->GetBlueprint();
						_toPlace._textureId = currentBlueprintButton->GetTextureId();


						_baseEdit->HandleBlueprint(&_toPlace);
						break;
					}
				}
			}
		}
		for (unsigned i = 0; i < _settingsTabs->size()-3 && !buttonClicked; i++)
		{
			//Map/Unitlock data
			if (_uiTree.IsButtonColliding(_settingsTabs->at(i), coord._pos.x, coord._pos.y))
			{
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
		if (_settingsId == 1)
		{
			for (unsigned i = 0; i < _uiTree.GetNode("UnitLockButtons")->GetChildren()->size(); i++)
			{
				if (_uiTree.IsButtonColliding(_uiTree.GetNode("UnitLockButtons")->GetChildren()->at(i), coord._pos.x, coord._pos.y))
				{
					if (_isLocked[i] == false)
					{
						_uiTree.GetNode("UnitLockButtons")->GetChildren()->at(i)->SetColorOffset(XMFLOAT4{ 0.4f, 0.4f, 0.4f, 1.0f });
						_isLocked[i] = true;
						//TODO: Lock Unit Enbom
					}
					else
					{
						_uiTree.GetNode("UnitLockButtons")->GetChildren()->at(i)->SetColorOffset(_OrginLockColor[i]);
						_isLocked[i] = false;
						//TODO: Unlock Unit Enbom
					}
				}
			}
		}
		//Story data
		if (_uiTree.IsButtonColliding("StoryButton", coord._pos.x, coord._pos.y))
		{
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
		else if (_uiTree.IsButtonColliding("ThiefInactiveBox", coord._pos.x, coord._pos.y) && _isPressed[4] == true)
		{
			//Move Button icon
			GUI::Node* node = _uiTree.GetNode("ThiefInactiveBox");
			XMFLOAT2 unpickedPosition = node->GetLocalPosition();

			GUI::Node* node2 = _uiTree.GetNode("ThiefActiveBox");
			XMFLOAT2 pickedPosition = node2->GetLocalPosition();

			node2->SetPosition(unpickedPosition);
			node->SetPosition(pickedPosition);

			if (pickedPosition.y > unpickedPosition.y)
			{
				//BURGLAR has been picked
				//TODO: Save this setting for this wave
			}
			else if (pickedPosition.x > unpickedPosition.x)
			{
				//Assassin has been picked
				//TODO: Save this setting for this wave
			}
			else
			{ 
				//DEMOLISHER has been picked
				//TODO: Save this setting for this wave
			}
		}
		else if (_uiTree.IsButtonColliding("ThiefInactiveBox2", coord._pos.x, coord._pos.y) && _isPressed[4] == true)
		{
			//Move Button icon
			GUI::Node* node = _uiTree.GetNode("ThiefInactiveBox2");
			XMFLOAT2 unpickedPosition = node->GetLocalPosition();

			GUI::Node* node2 = _uiTree.GetNode("ThiefActiveBox");
			XMFLOAT2 pickedPosition = node2->GetLocalPosition();

			node2->SetPosition(unpickedPosition);
			node->SetPosition(pickedPosition);

			if (pickedPosition.y > unpickedPosition.y)
			{
				//BURGLAR has been picked
				//TODO: Save this setting for this wave
			}
			else if (pickedPosition.x > unpickedPosition.x)
			{
				//Assassin has been picked
				//TODO: Save this setting for this wave
			}
			else
			{
				//DEMOLISHER has been picked
				//TODO: Save this setting for this wave
			}
		}

		else if (_uiTree.IsButtonColliding("WaveAdd", coord._pos.x, coord._pos.y) && _isPressed[4] == true)
		{
			//TODO: Add functionality to add new page/wave
		}
		else if (_uiTree.IsButtonColliding("WaveDelete", coord._pos.x, coord._pos.y) && _isPressed[4] == true)
		{
			//TODO: Add functionality to delete current page/wave
		}

		else if (_uiTree.IsButtonColliding("WaveLeft", coord._pos.x, coord._pos.y) && _isPressed[4] == true)
		{
			//TODO: Find which page we are on, check so that it's not the first, if it is NOT the first, then you can press.
		}
		else if (_uiTree.IsButtonColliding("WaveRight", coord._pos.x, coord._pos.y) && _isPressed[4] == true)
		{
			//TODO: Find page so that we are not on the last page, also so that we are able to switch to a new page
		}
		else if (_uiTree.IsButtonColliding("GridOff", coord._pos.x, coord._pos.y) && _isPressed[0] == true)
		{
			//Move Button
			GUI::Node* node = _uiTree.GetNode("GridOff");
			XMFLOAT2 gridOffPosition = node->GetLocalPosition();

			GUI::Node* node2 = _uiTree.GetNode("GridOn");
			XMFLOAT2 gridOnPosition = node2->GetLocalPosition();

			node2->SetPosition(gridOffPosition);
			node->SetPosition(gridOnPosition);

			if (gridOnPosition.x > gridOffPosition.x)
			{
				//TODO: Activate Grid Julia och Enbom
			}
			else
			{
				//TODO: Deactive Grid Julia och Enbom
			}
		}
		else if (_uiTree.IsButtonColliding("ObjectiveOff", coord._pos.x, coord._pos.y) && _isPressed[0] == true)
		{
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
		else if (_uiTree.IsButtonColliding("ExportMap", coord._pos.x, coord._pos.y))
		{
			//TODO: ExportMap GUI stuff and functions Julia and Enbom
		}
		else if (_uiTree.IsButtonColliding("ImportMap", coord._pos.x, coord._pos.y))
		{
			//TODO: ImportMap GUI stuff and functions Julia and Enbom
		}
		else if (_uiTree.IsButtonColliding("NewMap", coord._pos.x, coord._pos.y))
		{
			//Is only supposed to bring up the confirmation button
			_uiTree.GetNode("LeaveMap")->SetHidden(false);
			_leaveCheck = 0;
		}
		else if (_uiTree.IsButtonColliding("LeaveMapYes", coord._pos.x, coord._pos.y) && _leaveCheck > -1)
		{
			if (_leaveCheck == 0)
			{
				//TODO: NewMap GUI stuff and functions (Make new Map) Julia and Enbom
				_uiTree.GetNode("LeaveMap")->SetHidden(true);
				_leaveCheck = -1;
			}
			else
			{
				_leaveCheck = -1;
				ChangeState(MENUSTATE);
			}
		}
		else if (_uiTree.IsButtonColliding("LeaveMapNo", coord._pos.x, coord._pos.y) && _leaveCheck > -1)
		{
			_uiTree.GetNode("LeaveMap")->SetHidden(true);
			_leaveCheck = -1;
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
		for (unsigned i = 0; i < _uiTree.GetNode("Otherbuttons")->GetChildren()->size()-3; i++)
		{
			//Move gui
			GUI::Node* node = _uiTree.GetNode("Otherbuttons")->GetChildren()->at(i);
			node->SetPosition(_buttonPositions[i+7]);
			_isPressed[i] = false;
		}
		_settingsId = -2;
	}
	else if (_settingsId >= 0 && _moveCheck != -3)
	{
		////Show Gui
		_uiTree.GetNode("SettingList")->SetHidden(false);
		for (unsigned i = 0; i < _uiTree.GetNode("Otherbuttons")->GetChildren()->size()-3; i++)
		{
			//Move gui
			GUI::Node* node = _uiTree.GetNode("Otherbuttons")->GetChildren()->at(i);
			XMFLOAT2 move = _buttonPositions[i+7];
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
}

void LevelEditState::ExportLevel()
{
	_currentLevelFileName = "test";

	////Fill Level Header:

	//Getting Story Information
	_levelHeader._storyTitle = WStringToString(_uiTree.GetNode("StoryTitleText")->GetText());
	_levelHeader._storyBody = WStringToString(_uiTree.GetNode("StoryText")->GetText());

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
	Level::LevelBinary levelBinary;

	//Tilemap size
	Tilemap* tileMap = _objectHandler->GetTileMap();
	levelBinary._tileMapSizeX = tileMap->GetWidth();
	levelBinary._tileMapSizeZ = tileMap->GetHeight();

	//Game objects
	std::vector<std::vector<GameObject*>>* gameObjects = _objectHandler->GetGameObjects();
	levelBinary._gameObjectData.resize(_objectHandler->GetObjectCount());
	int gameObjectIndex = 0;
	for (uint i = 0; i < gameObjects->size(); i++)
	{
		for (GameObject* gameObject : gameObjects->at(i))
		{
			std::vector<int>* formattedGameObject = &levelBinary._gameObjectData[gameObjectIndex];
			formattedGameObject->resize(6);

			//Type
			int type = formattedGameObject->at(0) = gameObject->GetType();

			//Sub type 
			int subType = formattedGameObject->at(1) = gameObject->GetSubType();

			//Texture ID
			Blueprint* blueprint = _objectHandler->GetBlueprintByType(type, subType);
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

	//TODO: Fill _enemyWavesData /Rikhard
	//TODO: Fill _enemySpawnMap /Rikhard

	////Write the files

	//Construct a suitable path for the level
	std::string levelPath;
#ifdef _DEBUG
	char userPath[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, userPath);

	levelPath = userPath;
	levelPath += "\\Google Drive\\Stort spelprojekt\\" + LEVEL_FOLDER_PATH;
#else
	levelPath = LEVEL_FOLDER_PATH;
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
	outStream.open(binaryPath);
	cereal::BinaryOutputArchive binOut(outStream);
	binOut(levelBinary);
	outStream.close();
}