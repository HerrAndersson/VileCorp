#include "LevelEditState.h"

LevelEditState::LevelEditState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, "LEVELEDIT", assetManager, fontWrapper, width, height)
{
	_controls = controls;
	_objectHandler = objectHandler;

	_camera = camera;
	_pickingDevice = pickingDevice;
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
	_uiTree.GetNode("TrapLeaf")->SetHidden(true);
	_uiTree.GetNode("UnitLeaf")->SetHidden(true);
	_uiTree.GetNode("DecLeaf")->SetHidden(true);

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
}

void LevelEditState::HandleButtons()
{
	System::MouseCoord coord = _controls->GetMouseCoord();
	if (_uiTree.IsButtonColliding("Traps", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("PLACEMENT:CLICK"))
	{
		_uiTree.GetNode("TrapLeaf")->SetHidden(false);
		_uiTree.GetNode("UnitLeaf")->SetHidden(true);
		_uiTree.GetNode("DecLeaf")->SetHidden(true);

		_trapButtonClick = true;
		_unitButtonClick = false;
		_decButtonClick = false;

		// Temp, should be replaced with blueprint
		_toPlace._type = TRAP;
		_toPlace._name = "trap_proto";
	}

	if (_uiTree.IsButtonColliding("Units", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("PLACEMENT:CLICK"))
	{
		_uiTree.GetNode("UnitLeaf")->SetHidden(false);
		_uiTree.GetNode("TrapLeaf")->SetHidden(true);
		_uiTree.GetNode("DecLeaf")->SetHidden(true);

		_trapButtonClick = false;
		_unitButtonClick = true;
		_decButtonClick = false;

		// Temp, should be replaced with blueprint
		_toPlace._type = GUARD;
		_toPlace._name = "guard_proto";
	}


	if (_uiTree.IsButtonColliding("Decorations", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("PLACEMENT:CLICK"))
	{
		_uiTree.GetNode("DecLeaf")->SetHidden(false);
		_uiTree.GetNode("TrapLeaf")->SetHidden(true);
		_uiTree.GetNode("UnitLeaf")->SetHidden(true);

		_trapButtonClick = false;
		_unitButtonClick = true;
		_decButtonClick = false;

		// Not really implemented
	}

	if (_controls->IsFunctionKeyDown("PLACEMENT:CLICK") && _trapButtonClick == false && _unitButtonClick == false && _decButtonClick == false)
	{
		_uiTree.GetNode("DecLeaf")->SetHidden(true);
		_uiTree.GetNode("TrapLeaf")->SetHidden(true);
		_uiTree.GetNode("UnitLeaf")->SetHidden(true);
	}

	_trapButtonClick = false;
	_unitButtonClick = false;
	_decButtonClick = false;
}

void LevelEditState::InitNewLevel()
{
	_objectHandler->Release();

	_baseEdit.ResetSelectedObj();
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
		for (uint y = 0; y < gameObjects[i].size(); y++)
		{
			GameObject *gameObj = (*gameObjects)[i][y];
			MapData mapD;

			mapD._tileType = gameObj->GetType();
			DirectX::XMFLOAT3 position = gameObj->GetPosition();
			mapD._posX = position.x;
			mapD._posZ = position.z;
			mapD._rotY = gameObj->GetRotation().y;

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