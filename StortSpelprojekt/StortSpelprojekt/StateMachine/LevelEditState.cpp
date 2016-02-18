#include "LevelEditState.h"

LevelEditState::LevelEditState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule)
{
	_baseEdit = nullptr;
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
	//TODO: Move this function to LevelSelection when that state is created. /Alex
	_objectHandler->LoadLevel(3);
	_objectHandler->DisableSpawnPoints();
	//_uiTree.GetNode("TrapLeaf")->SetHidden(true);
	//_uiTree.GetNode("UnitLeaf")->SetHidden(true);
	//_uiTree.GetNode("DecLeaf")->SetHidden(true);

	_objectHandler->EnlargeTilemap(50);

	_baseEdit = new BaseEdit(_objectHandler, _controls, _pickingDevice, _camera);

	XMFLOAT3 campos;
	campos.x = _objectHandler->GetTileMap()->GetWidth() / 2;
	campos.y = 15;
	campos.z = _objectHandler->GetTileMap()->GetHeight() / 2 - 10;
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
}

void LevelEditState::HandleButtons()
{
	System::MouseCoord coord = _controls->GetMouseCoord();
	//if (_uiTree.IsButtonColliding("Traps", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	//{
	//	_uiTree.GetNode("TrapLeaf")->SetHidden(false);
	//	_uiTree.GetNode("UnitLeaf")->SetHidden(true);
	//	_uiTree.GetNode("DecLeaf")->SetHidden(true);

	//	_trapButtonClick = true;
	//	_unitButtonClick = false;
	//	_decButtonClick = false;

	//	// Temp, should be replaced with blueprint
	//	_toPlace._type = TRAP;
	//	_toPlace._name = "trap_proto";

	//	if (_baseEdit->IsSelection() && !_baseEdit->IsPlace())
	//	{
	//		_baseEdit->DragActivate(_toPlace._type, _toPlace._name);
	//	}
	//}

	//if (_uiTree.IsButtonColliding("Units", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
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

	//	if (_baseEdit->IsSelection())
	//	{
	//		_baseEdit->DragActivate(_toPlace._type, _toPlace._name);
	//	}
	//}


	//if (_uiTree.IsButtonColliding("Decorations", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	//{
	//	_uiTree.GetNode("DecLeaf")->SetHidden(false);
	//	_uiTree.GetNode("TrapLeaf")->SetHidden(true);
	//	_uiTree.GetNode("UnitLeaf")->SetHidden(true);

	//	_trapButtonClick = false;
	//	_unitButtonClick = true;
	//	_decButtonClick = false;

	//	// Not really implemented
	//}

	//if (_controls->IsFunctionKeyDown("MOUSE:SELECT") && _trapButtonClick == false && _unitButtonClick == false && _decButtonClick == false)
	//{
	//	_uiTree.GetNode("DecLeaf")->SetHidden(true);
	//	_uiTree.GetNode("TrapLeaf")->SetHidden(true);
	//	_uiTree.GetNode("UnitLeaf")->SetHidden(true);
	//}

	//_trapButtonClick = false;
	//_unitButtonClick = false;
	//_decButtonClick = false;
}

void LevelEditState::ExportLevel()
{
	LevelHeader levelHead;
	levelHead._version = 10;
	Tilemap* tileMap = _objectHandler->GetTileMap();
	levelHead._levelSizeY = tileMap->GetHeight();
	levelHead._levelSizeX = tileMap->GetWidth();
	levelHead._nrOfGameObjects = _objectHandler->GetObjectCount();

	//Iterating through all game objects and saving only the relevant data for exporting.
	std::vector<MapData> mapData;
	mapData.reserve(levelHead._nrOfGameObjects);
	std::vector<std::vector<GameObject*>>* gameObjects = _objectHandler->GetGameObjects();

	for (uint i = 0; i < gameObjects->size(); i++)
	{
		for (GameObject* g : gameObjects->at(i))
		{
			MapData mapD;

			mapD._tileType = g->GetType();
			mapD._posX = g->GetPosition().x;
			mapD._posZ = g->GetPosition().z;
			mapD._rotY = g->GetRotation().y;

			mapData.push_back(mapD);
		}
	}

	char userPath[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, userPath);

	string outputPath, copyPath;
	string levelName = "exportedLevel.lvl";

	//Setting the output folder depending on if running debug or release mode.

	outputPath = ("/Assets/Levels/");

	outputPath += levelName;
	copyPath += levelName;

	ofstream outputFile;

	outputFile.open(outputPath, ios::out | ios::binary);

	outputFile.write((const char*)&levelHead, sizeof(levelHead));

	for (auto md : mapData)
	{
		outputFile.write((const char*)&md, sizeof(mapData));
	}

	outputFile.close();

	char buf[BUFSIZ];
	size_t size;
	FILE* source;
	FILE* dest;
	fopen_s(&source, outputPath.c_str(), "rb");
	fopen_s(&dest, copyPath.c_str(), "wb");

	while (size = fread(buf, 1, BUFSIZ, source))
	{
		fwrite(buf, 1, size, dest);
	}

	//fclose(source);
	//fclose(dest);

	mapData.clear();

}