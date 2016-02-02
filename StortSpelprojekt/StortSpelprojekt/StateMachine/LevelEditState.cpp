#include "LevelEditState.h"

LevelEditState::LevelEditState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, "LEVELEDIT", assetManager, fontWrapper, width, height)
{
	_controls = controls;
	_objectHandler = objectHandler;

	_camera = camera;
	_pickingDevice = pickingDevice;
	_listId = -1;

	_objectTabs = _uiTree.GetNode("Buttons")->GetChildren();

	for (int i = 0; i < _objectTabs->size(); i++)
	{
		_buttonPositions[i] = _objectTabs->at(i)->GetPosition();
	}
}

LevelEditState::~LevelEditState()
{}

void LevelEditState::Update(float deltaTime)
{
	if (_controls->IsFunctionKeyDown("MAP_EDIT:PLACEMENTFLAG"))
	{
		_baseEdit.ChangePlaceState();
	}
	_baseEdit.Update(deltaTime);
	HandleInput();
	HandleButtons();


	_baseEdit.DragAndDrop();
	_baseEdit.DragAndPlace(_toPlace._type, _toPlace._name);
}

void LevelEditState::OnStateEnter()
{
	_baseEdit.Initialize(_objectHandler, _controls, _pickingDevice, _camera);
	_objectHandler->DisableSpawnPoints();
	_uiTree.GetNode("wholelist")->SetHidden(true);
	_uiTree.GetNode("listbuttons")->SetHidden(true);

	_objectHandler->EnlargeTilemap(50);

	XMFLOAT3 campos;
	campos.x = _objectHandler->GetTileMap()->GetWidth() / 2;
	campos.y = 15;
	campos.z = _objectHandler->GetTileMap()->GetHeight() / 2 - 10;
	_camera->SetPosition(campos);

}

void LevelEditState::OnStateExit()
{
	_objectHandler->MinimizeTileMap();
}

void LevelEditState::HandleInput()
{
	//Press C to init new level
	if (_controls->IsFunctionKeyDown("MAP_EDIT:NEWLEVEL"))
	{
		InitNewLevel();
	}

	if (_controls->IsFunctionKeyDown("MAP_EDIT:MENU"))
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


	if (_controls->IsFunctionKeyDown("PLACEMENT:CLICK"))
	{
		//Open Level editor placement GUI
		bool buttonClicked = false;
		for (int i = 0; i < _objectTabs->size() && !buttonClicked; i++)
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
				if (_currentList->GetChildren()->size() - 1 > _currentPage)
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
			if (_currentList != nullptr)
			{
				buttonClicked = false;
				std::vector<GUI::Node*>* currentPageRows = _currentList->GetChildren()->at(_currentPage)->GetChildren();
				for (int y = 0; y < currentPageRows->size() && !buttonClicked; y++)
				{
					std::vector<GUI::Node*>* currentPageRowButtons = currentPageRows->at(y)->GetChildren();

					//Object buttons 
					for (int j = 0; j < currentPageRowButtons->size() && !buttonClicked; j++)
					{
						GUI::Node* currentButton = currentPageRowButtons->at(j);
						if (_uiTree.IsButtonColliding(currentButton, coord._pos.x, coord._pos.y))
						{
							//TODO: Place objects when buttons clicked! - Jesper and Rikhard
							buttonClicked = true;
						}
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
		for (int i = 0; i < _uiTree.GetNode("wholelist")->GetChildren()->size(); i++)
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
		for (int i = 0; i < _uiTree.GetNode("wholelist")->GetChildren()->size(); i++)
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
				for (int j = 0; j < _currentList->GetChildren()->size(); j++)
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

	//if (_uiTree.GetNode("TrapLeaf")->GetHidden() == false)
	//{
	//	if (_uiTree.IsButtonColliding("Trap1", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("PLACEMENT:CLICK"))
	//	{
	//		_trapButtonClick = true;
	//		_unitButtonClick = false;
	//		_decButtonClick = false;

	//		// Temp, should be replaced with blueprint
	//		_toPlace._type = TRAP;
	//		_toPlace._name = "trap_proto";
	//	}
	//	else if (_uiTree.IsButtonColliding("Trap2", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("PLACEMENT:CLICK"))
	//	{
	//		_trapButtonClick = true;
	//		_unitButtonClick = false;
	//		_decButtonClick = false;

	//		// Temp, should be replaced with blueprint
	//		_toPlace._type = TRAP;
	//		_toPlace._name = "trap2_proto";
	//	}
	//}

	//if (_uiTree.IsButtonColliding("Units", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("PLACEMENT:CLICK"))
	//{
	//	_uiTree.GetNode("UnitLeaf")->SetHidden(false);
	//	_uiTree.GetNode("TrapLeaf")->SetHidden(true);
	//	_uiTree.GetNode("DecLeaf")->SetHidden(true);

	//	_trapButtonClick = false;
	//	_unitButtonClick = true;
	//	_decButtonClick = false;

	//	// Temp, should be replaced with blueprint
	//	_toPlace._type = GUARD;
	//	_toPlace._name = "guard_proto";
	//}


	//if (_uiTree.IsButtonColliding("Decorations", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("PLACEMENT:CLICK"))
	//{
	//	_uiTree.GetNode("DecLeaf")->SetHidden(false);
	//	_uiTree.GetNode("TrapLeaf")->SetHidden(true);
	//	_uiTree.GetNode("UnitLeaf")->SetHidden(true);

	//	_trapButtonClick = false;
	//	_unitButtonClick = true;
	//	_decButtonClick = false;

	//	// Not really implemented
	//}

	//if (_controls->IsFunctionKeyDown("PLACEMENT:CLICK") && _trapButtonClick == false && _unitButtonClick == false && _decButtonClick == false)
	//{
	//	_uiTree.GetNode("DecLeaf")->SetHidden(true);
	//	_uiTree.GetNode("TrapLeaf")->SetHidden(true);
	//	_uiTree.GetNode("UnitLeaf")->SetHidden(true);
	//}

	//_trapButtonClick = false;
	//_unitButtonClick = false;
	//_decButtonClick = false;
}

void LevelEditState::InitNewLevel()
{
	_objectHandler->Release();
}


void LevelEditState::ExportLevel()
{
	std::string levelName = "exportedLevel.lvl";

	LevelFormat exportedLevel;
	
	Tilemap* tileMap = _objectHandler->GetTileMap();
	exportedLevel._tileMapWidth = tileMap->GetWidth();
	exportedLevel._tileMapHeight = tileMap->GetHeight();
	exportedLevel._modelPath = MODEL_FOLDER_PATH;
	std::string texturePath(TEXTURE_FOLDER_PATH_W.begin(), TEXTURE_FOLDER_PATH_W.end());
	exportedLevel._texturePath = texturePath;

	std::vector<std::vector<GameObject*>>* gameObjects = _objectHandler->GetGameObjects();
	exportedLevel._gameObjectData.resize(_objectHandler->GetObjectCount());

	int gameObjectIndex = 0;
	for (uint i = 0; i < gameObjects->size(); i++)
	{
		for (GameObject* g : gameObjects->at(i))
		{
			exportedLevel._gameObjectData[gameObjectIndex].resize(7);

			//position
			AI::Vec2D position = g->GetTilePosition();
			exportedLevel._gameObjectData[gameObjectIndex][0] = static_cast<int>(position._x);
			exportedLevel._gameObjectData[gameObjectIndex][1] = static_cast<int>(position._y);

			//rotation
			DirectX::XMFLOAT3 rotation = g->GetRotation();
			float eps = std::numeric_limits<float>::epsilon();
			if ((rotation.x > 0 && rotation.z > 0) || (rotation.x < eps && rotation.z < eps))
			{
				exportedLevel._gameObjectData[gameObjectIndex][2] = 0;
			}
			else if (rotation.x < 0 && rotation.z > 0)
			{
				exportedLevel._gameObjectData[gameObjectIndex][2] = 1;
			}
			else if (rotation.x < 0 && rotation.z < 0)
			{
				exportedLevel._gameObjectData[gameObjectIndex][2] = 2;
			}
			else
			{
				exportedLevel._gameObjectData[gameObjectIndex][2] = 3;
			}

			//type
			int type = g->GetType();
			exportedLevel._gameObjectData[gameObjectIndex][3] = type;

			//subtype
			if (type == Type::TRAP)
			{
				exportedLevel._gameObjectData[gameObjectIndex][4] = static_cast<Trap*>(g)->GetTrapType();
			}
			else
			{
				exportedLevel._gameObjectData[gameObjectIndex][4] = 0;
			}

			//model
			RenderObject* renderObject = g->GetRenderObject();
			std::string modelName = renderObject->_name;
			exportedLevel._gameObjectData[gameObjectIndex][5] = GetVectorIndexOfString(&exportedLevel._modelReferences, modelName);

			//texture
			std::string textureName = renderObject->_diffuseTexture->_name.c_str();
			exportedLevel._gameObjectData[gameObjectIndex][6] = GetVectorIndexOfString(&exportedLevel._textureReferences, textureName);

			gameObjectIndex++;
		}
	}

	std::string levelPath;

#ifdef _DEBUG
	char userPath[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, userPath);
	
	levelPath = userPath;
	levelPath += "\\Google Drive\\Stort spelprojekt\\Assets\\Levels\\";
#else
	levelPath = LEVEL_FOLDER_PATH + "/";
#endif
	levelPath += levelName;
	
	std::ofstream out(levelPath);
	cereal::BinaryOutputArchive archive(out);
	//cereal::JSONOutputArchive archive(out);
	archive(exportedLevel);
}

int LevelEditState::GetVectorIndexOfString(std::vector<std::string>* vec, std::string str)
{
	int referenceIndex = 0;
	bool foundReference = false;
	for (int referenceIndex = 0; referenceIndex < vec->size() && !foundReference; referenceIndex++)
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
