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
	HandleInput();
	_levelEdit.Update(deltaTime);
}

void LevelEditState::OnStateEnter()
{
	_levelEdit.Initialize(_objectHandler, _controls, _pickingDevice, _camera);
}

void LevelEditState::OnStateExit()
{

}

void LevelEditState::HandleInput()
{
	int maxObject = _objectHandler->GetObjectCount();
	int selectedLevel = 1;

	////Scale Objects
	///*
	//"SCALE_MARKER_LEFT": [""].
	//*/
	if (_levelEdit.GetSelectedObject() != nullptr)
	{
		if (_controls->IsFunctionKeyDown("MAP_EDIT:SCALE_MARKER_XPOSITIVE"))
		{
			XMFLOAT3 tempPos = _levelEdit.GetSelectedObject()->GetScale();
			_levelEdit.GetSelectedObject()->SetScale(XMFLOAT3(tempPos.x + 1, tempPos.y, tempPos.z));
		}

		if (_controls->IsFunctionKeyDown("MAP_EDIT:SCALE_MARKER_XNEGATIVE"))
		{
			XMFLOAT3 tempPos = _levelEdit.GetSelectedObject()->GetScale();
			_levelEdit.GetSelectedObject()->SetScale(XMFLOAT3(tempPos.x - 1, tempPos.y, tempPos.z));
		}

		if (_controls->IsFunctionKeyDown("MAP_EDIT:SCALE_MARKER_YPOSITIVE"))
		{
			XMFLOAT3 tempPos = _levelEdit.GetSelectedObject()->GetScale();
			_levelEdit.GetSelectedObject()->SetScale(XMFLOAT3(tempPos.x, tempPos.y, tempPos.z + 1));
		}

		if (_controls->IsFunctionKeyDown("MAP_EDIT:SCALE_MARKER_YNEGATIVE"))
		{
			XMFLOAT3 tempPos = _levelEdit.GetSelectedObject()->GetScale();
			_levelEdit.GetSelectedObject()->SetScale(XMFLOAT3(tempPos.x, tempPos.y, tempPos.z - 1));
		}
	}

	//Press C to init new level
	if (_controls->IsFunctionKeyDown("MAP_EDIT:NEWLEVEL"))
	{
		InitNewLevel();
	}
}

void LevelEditState::InitNewLevel()
{
	_objectHandler->Release();

	_levelEdit.ResetSelectedObj();
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