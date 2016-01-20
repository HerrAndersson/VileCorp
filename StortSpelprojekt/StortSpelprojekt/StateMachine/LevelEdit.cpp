#include "LevelEdit.h"
#include <stdexcept>
#include <DirectXMath.h>
#include <sstream>

LevelEdit::LevelEdit()
{
	_aStar = nullptr;
}

LevelEdit::~LevelEdit()
{
	delete _aStar;
}

bool compareFloat3(XMFLOAT3 a, XMFLOAT3 b)
{
	if (a.x == b.x && a.z == b.z)
	{
		return true;
	}
	return false;
}

GameObject* LevelEdit::GetSelectedObject()
{
	return _marker;
}

void LevelEdit::Add(Type type, int renderObjectID)
{
	_objectHandler->Add(type, _marker->GetPosition(), XMFLOAT3(0.0f, 0.0f, 0.0f));	
}

bool LevelEdit::Delete(Type type)
{
	vector<GameObject*>* vec = &_objectHandler->GetGameObjects()->at(type);
	for (GameObject* g : *vec)
	{
		if (_marker != g && compareFloat3(_marker->GetPosition(), g->GetPosition()))
		{
			_objectHandler->Remove(g->GetType(), g->GetID());
			return true;
		}
	}

	return false;
}

bool LevelEdit::Marked(Type type)
{
	vector<GameObject*>* vec = &_objectHandler->GetGameObjects()->at(type);
	for (GameObject* g : *vec)
	{
		if (_marker != g && compareFloat3(_marker->GetPosition(), g->GetPosition()))
		{
			return true;
		}
	}

	return false;
}

void LevelEdit::Initialize(ObjectHandler* objectHandler, System::InputDevice* inputDevice, System::Controls* controls, PickingDevice* pickingDevice, System::Camera* camera, UIHandler* uiHandler)
{
	_objectHandler = objectHandler;
	_inputDevice = inputDevice;
	_controls = controls;
	_pickingDevice = pickingDevice;
	_camera = camera;
	_aStar = new AI::AStar();
	_uiHandler = uiHandler;

	_lastSelected = nullptr;
	_tileMultiplier = 1;
	_isSelectionMode = true;

	// Add all buttons and hide them
	//buttonInfo.resize(3);
	_uiHandler->AddButton("floor3.png", DirectX::XMFLOAT2(-0.9f, 0.95f), DirectX::XMFLOAT2(0.1f, 0.05f), true);
	_uiHandler->AddButton("floor.png", DirectX::XMFLOAT2(-0.9f, 0.85f), DirectX::XMFLOAT2(0.1f, 0.05f), false);
	_uiHandler->AddButton("floor2.png", DirectX::XMFLOAT2(-0.7f, 0.85f), DirectX::XMFLOAT2(0.1f, 0.05f), false);

	//buttonInfo[0].parent = -1;
	//buttonInfo[1].parent = 0;
	//buttonInfo[2].parent = 1;

	LoadLevel(9);

	// Temporary hack because no mouse interface
	_marker = _objectHandler->GetGameObjects()->at(1)[0];

	//_grid = new Grid(_renderModule->GetDevice(), 1, 10);
}

void LevelEdit::LoadLevel(int levelID)
{
	//load existing level.
	_objectHandler->Release();
	ResetSelectedObj();
	_objectHandler->LoadLevel(levelID);
	_tileMap = _objectHandler->GetTileMap();
	_tilemapHeight = _tileMap->GetHeight();
	_tilemapWidth = _tileMap->GetWidth();
}


void LevelEdit::Update(float deltaTime)
{
	_inputDevice->Update();
	_mouseCoord = _inputDevice->GetMouseCoord();
	HandleHUD();
	HandleSelected();
	HandleInput();
	_objectHandler->Update(deltaTime);
}

void LevelEdit::HandleSelected()
{
	if (_objectHandler->GetObjectCount() != 0)
	{
		if (_lastSelected != nullptr && _selectedObj->GetID() != _lastSelected->GetID())
		if (_marker->GetID() != _lastSelected)
		{
			if (_lastSelected->GetType() == WALL || FLOOR || ENEMY || GUARD)
			{
				temp = _marker;
				_lastSelected->SetPosition(XMFLOAT3(_lastSelected->GetPosition().x, 0.1, _lastSelected->GetPosition().z));
				//XMFLOAT3 tempScale = temp->GetScale();
				//temp->SetScale(XMFLOAT3(1.1, 1.1, 1.1));
			}
		}
		_lastSelected = _marker->GetID();
	}
}

void LevelEdit::ResetSelectedObj()
{
	_selectedObj = nullptr;
	_lastSelected = nullptr;
}

void LevelEdit::InitNewLevel()
{
	_objectHandler->Release();

	ResetSelectedObj();
}

void LevelEdit::ExportLevel()
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

	fclose(source);
	fclose(dest);

	mapData.clear();

}

void LevelEdit::SelectObject(GameObject* selectedObject)
{
	float yOffset = 0.1f;

	if (_selectedObj != nullptr)
	{
		_selectedObj->Translate(DirectX::XMFLOAT3(0, -yOffset, 0));
	}
	_selectedObj = selectedObject;

	if (_selectedObj != nullptr)
	{
		_selectedObj->Translate(DirectX::XMFLOAT3(0, yOffset, 0));
	}
}
