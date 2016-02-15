#include "LevelEditState.h"

LevelEditState::LevelEditState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::Settings* settings, System::SoundModule* soundModule)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, "LEVELEDIT", assetManager, fontWrapper, settings, soundModule)
{
	_controls = controls;
	_objectHandler = objectHandler;
	_tileset = assetManager->LoadTileset("Assets/Tilesets/default.json");
	_camera = camera;
	_pickingDevice = pickingDevice;
	_listId = -1;
	_baseEdit = nullptr;

	_objectTabs = _uiTree.GetNode("Buttons")->GetChildren();

	std::map<Type, std::string> typeLists =
	{
		{	   FLOOR, "floorlist"		},
		{	    WALL, "walllist"		},
		{	    LOOT, "objectivelist"	},
		{	   SPAWN, "entrylist"		},
		{	    TRAP, "traplist"		},
		{	  CAMERA, "traplist"		},
		{	   GUARD, "unitlist"		},
		{	   ENEMY, "unitlist"		},
		{  FURNITURE, "decorationlist"	}
	};
	for (unsigned i = 0; i < NR_OF_TYPES; i++)
	{
		int index = 0;
		for (unsigned o = 0; o < _tileset->_objects[i].size(); o++)
		{
			index += _uiTree.CreateTilesetObject(&_tileset->_objects[i][o], _uiTree.GetNode(typeLists[(Type)i]), o);
		}
	}

	for (unsigned i = 0; i < _objectTabs->size(); i++)
	{
		_buttonPositions[i] = _objectTabs->at(i)->GetPosition();
	}
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


	_baseEdit->DragAndDrop();
	_baseEdit->DragAndPlace(_toPlace._type, _toPlace._name);
}

void LevelEditState::OnStateEnter()
{
	_objectHandler->DisableSpawnPoints();
	_uiTree.GetNode("wholelist")->SetHidden(true);
	_uiTree.GetNode("listbuttons")->SetHidden(true);

	_objectHandler->EnlargeTilemap(50);

	_baseEdit = new BaseEdit(_objectHandler, _controls, _pickingDevice, _camera);

	XMFLOAT3 campos;
	campos.x = (float)_objectHandler->GetTileMap()->GetWidth() / 2;
	campos.y = 15;
	campos.z = (float)_objectHandler->GetTileMap()->GetHeight() / 2 - 10;
	_camera->SetPosition(campos);

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
		ChangeState(MENUSTATE);
	}

	if (_controls->IsFunctionKeyDown("DEBUG:EXPORT_LEVEL"))
	{
		ExportLevel();
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
				}
			}
			else if (_uiTree.IsButtonColliding("left", coord._pos.x, coord._pos.y))
			{
				if (_currentPage > 0)
				{
					_currentPage--;
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
						_toPlace._name = currentButton->GetId();
						_toPlace._type = (Type)_listId;

						break;
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
		for (unsigned i = 0; i < _uiTree.GetNode("wholelist")->GetChildren()->size(); i++)
		{
			//Move gui
			GUI::Node* node = _objectTabs->at(i);
			node->SetPosition(_buttonPositions[i]);
		}
	}
	else
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
			}
		}
	}
}

void LevelEditState::ExportLevel()
{
//	std::string levelName = "exportedLevel.lvl";
//
//	LevelFormat exportedLevel;
//	
//	Tilemap* tileMap = _objectHandler->GetTileMap();
//	exportedLevel._tileMapWidth = tileMap->GetWidth();
//	exportedLevel._tileMapHeight = tileMap->GetHeight();
//
//	std::vector<std::vector<GameObject*>>* gameObjects = _objectHandler->GetGameObjects();
//	exportedLevel._gameObjectData.resize(_objectHandler->GetObjectCount());
//
//	float topLeftCenterAngle = DirectX::XM_PIDIV4;
//	float topRightCenterAngle = DirectX::XM_PI - DirectX::XM_PIDIV4;
//	float bottomRightCenterAngle = DirectX::XM_PI + DirectX::XM_PIDIV4;
//	float bottomLeftCenterAngle = DirectX::XM_2PI - DirectX::XM_PIDIV4;
//
//	int gameObjectIndex = 0;
//	for (uint i = 0; i < gameObjects->size(); i++)
//	{
//		for (GameObject* g : gameObjects->at(i))
//		{
//			exportedLevel._gameObjectData[gameObjectIndex].resize(7);
//
//			//position
//			AI::Vec2D position = g->GetTilePosition();
//			exportedLevel._gameObjectData[gameObjectIndex][0] = static_cast<int>(position._x);
//			exportedLevel._gameObjectData[gameObjectIndex][1] = static_cast<int>(position._y);
//
//			//rotation
//			float rotation = g->GetRotation().y;
//			if (rotation >= topLeftCenterAngle && rotation < topRightCenterAngle)
//			{
//				exportedLevel._gameObjectData[gameObjectIndex][2] = 0;
//			}
//			else if (rotation >= topRightCenterAngle && rotation < bottomLeftCenterAngle)
//			{
//				exportedLevel._gameObjectData[gameObjectIndex][2] = 1;
//			}
//			else if (rotation >= bottomLeftCenterAngle && rotation < bottomRightCenterAngle)
//			{
//				exportedLevel._gameObjectData[gameObjectIndex][2] = 2;
//			}
//			else
//			{
//				exportedLevel._gameObjectData[gameObjectIndex][2] = 3;
//			}
//
//			//type
//			int type = g->GetType();
//			exportedLevel._gameObjectData[gameObjectIndex][3] = type;
//
//			//subtype
//			if (type == Type::TRAP)
//			{
//				exportedLevel._gameObjectData[gameObjectIndex][4] = static_cast<Trap*>(g)->GetTrapType();
//			}
//			else
//			{
//				exportedLevel._gameObjectData[gameObjectIndex][4] = 0;
//			}
//
//			//model
//			RenderObject* renderObject = g->GetRenderObject();
//			std::string modelName = renderObject->_mesh->_name;
//			exportedLevel._gameObjectData[gameObjectIndex][5] = GetVectorIndexOfString(&exportedLevel._modelReferences, modelName);
//
//			//texture
//			std::string textureName = renderObject->_diffuseTexture->_name.c_str();
//			exportedLevel._gameObjectData[gameObjectIndex][6] = GetVectorIndexOfString(&exportedLevel._textureReferences, textureName);
//
//			gameObjectIndex++;
//		}
//	}
//
//	std::string levelPath;
//
//#ifdef _DEBUG
//	char userPath[MAX_PATH];
//	SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, userPath);
//	
//	levelPath = userPath;
//	levelPath += "\\Google Drive\\Stort spelprojekt\\Assets\\Levels\\";
//#else
//	levelPath = LEVEL_FOLDER_PATH + "/";
//#endif
//	levelPath += levelName;
//	
//	std::ofstream out(levelPath);
//	cereal::BinaryOutputArchive archive(out);
//	//cereal::XMLOutputArchive archive(out);
//	archive(exportedLevel);
}

int LevelEditState::GetVectorIndexOfString(std::vector<std::string>* vec, std::string str)
{
	int referenceIndex = 0;
	bool foundReference = false;
	for (unsigned referenceIndex = 0; referenceIndex < vec->size() && !foundReference; referenceIndex++)
	{
		if (vec->at(referenceIndex) == str)
		{
			foundReference = true;
		}
	}
	if (!foundReference)
	{
		vec->push_back(str);
		referenceIndex = vec->size() - 1;
	}
	return referenceIndex;
}

//void LevelEditState::ExportLevel()
//{
//	LevelHeader levelHead;
//	levelHead._version = 26;
//	Tilemap* tileMap = _objectHandler->GetTileMap();
//	levelHead._levelSizeY = tileMap->GetHeight();
//	levelHead._levelSizeX = tileMap->GetWidth();
//	levelHead._nrOfGameObjects = _objectHandler->GetObjectCount();
//
//	//Iterating through all game objects and saving only the relevant data for exporting.
//	std::vector<MapData> mapData;
//	mapData.reserve(levelHead._nrOfGameObjects);
//	std::vector<std::vector<GameObject*>>* gameObjects = _objectHandler->GetGameObjects();
//
//	for (uint i = 0; i < gameObjects->size(); i++)
//	{
//		for (GameObject* g : gameObjects->at(i))
//		{
//			MapData mapD;
//
//			mapD._tileType = g->GetType();
//			mapD._posX = g->GetPosition().x;
//			mapD._posZ = g->GetPosition().z;
//			mapD._rotY = g->GetRotation().y;
//
//			mapData.push_back(mapD);
//		}
//	}
//

// UNDERNEATH: Important pathfinding function

//	char userPath[MAX_PATH];
//	SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, userPath);
//
//	string outputPath, copyPath;
//	string levelName = "exportedLevel.lvl";
//
//	//Setting the output folder depending on if running debug or release mode.
//
//	outputPath = ("/Assets/Levels/");
//
//	outputPath += levelName;
//	copyPath += levelName;
//
//	ofstream outputFile;
//
//	outputFile.open(outputPath, ios::out | ios::binary);
//
//	outputFile.write((const char*)&levelHead, sizeof(levelHead));
//
//	for (auto md : mapData)
//	{
//		outputFile.write((const char*)&md, sizeof(mapData));
//	}
//
//	outputFile.close();
//
//	char buf[BUFSIZ];
//	size_t size;
//	FILE* source;
//	FILE* dest;
//	fopen_s(&source, outputPath.c_str(), "rb");
//	fopen_s(&dest, copyPath.c_str(), "wb");
//
//	while (size = fread(buf, 1, BUFSIZ, source))
//	{
//		fwrite(buf, 1, size, dest);
//	}
//
//	//fclose(source);
//	//fclose(dest);
//
//	mapData.clear();
//
//}
