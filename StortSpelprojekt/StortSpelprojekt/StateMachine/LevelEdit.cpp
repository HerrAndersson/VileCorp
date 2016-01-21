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

GameObject* LevelEdit::GetSelectedObject()
{
	return _selectedObj;
}

void LevelEdit::Add(Type type, int renderObjectID)
{
	_objectHandler->Add(type, (int)type, _selectedObj->GetPosition(), XMFLOAT3(0.0f, 0.0f, 0.0f));
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
	_selectedObj = _objectHandler->GetGameObjects()->at(1)[0];

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

void LevelEdit::HandleHUD()
{
	/*for (int i = 0; i < buttonInfo.size(); i++)
	{
	if (buttonInfo[i].parent != -1)
	{
	if (buttonInfo[buttonInfo[i].parent].active == true)
	{
	_uiHandler->SetButtonVisibility(i, true);

	if (_uiHandler->Intersect(mouseCoord._clientPos, i) == true)
	{
	buttonInfo[i].active = true;
	}
	}
	else
	{
	_uiHandler->SetButtonVisibility(i, false);
	}
	}
	else if(_uiHandler->Intersect(mouseCoord._clientPos, i) == true)
	{
	buttonInfo[i].active = true;
	}
	}*/
}

void LevelEdit::HandleInput()
{
	//int maxObject = _objectHandler->GetObjectCount();
	//int selectedLevel = 1;

	////Move tile greater distance per button press
	//if (_inputHandler->IsPressed(VK_PRIOR))
	//{
	//	_tileMultiplier++;
	//}

	//if (_inputHandler->IsPressed(VK_NEXT))
	//{
	//	if (_tileMultiplier != 1)
	//	{
	//		_tileMultiplier--;
	//	}
	//}


	////Check if there is a tile to move
	//if (_objectHandler->GetObjectCount() > 0)
	//{
	//	//Translation and rotation controls
	//	if (_inputHandler->IsPressed(VK_LEFT))
	//	{
	//		if (_selectedObj->GetType() == WALL || FLOOR || UNIT)
	//		{
	//			XMFLOAT3 tempPos = _selectedObj->GetPosition();
	//			_selectedObj->SetPosition(XMFLOAT3(tempPos.x - 1 * _tileMultiplier, tempPos.y, tempPos.z));
	//		}

	//	}

	//	if (_inputHandler->IsPressed(VK_RIGHT))
	//	{
	//		if (_selectedObj->GetType() == WALL || FLOOR || UNIT)
	//		{
	//			XMFLOAT3 tempPos = _selectedObj->GetPosition();
	//			_selectedObj->SetPosition(XMFLOAT3(tempPos.x + 1 * _tileMultiplier, tempPos.y, tempPos.z));
	//		}
	//	}

	//	if (_inputHandler->IsPressed(VK_UP))
	//	{
	//		if (_selectedObj->GetType() == WALL || FLOOR || UNIT)
	//		{
	//			XMFLOAT3 tempPos = _selectedObj->GetPosition();
	//			_selectedObj->SetPosition(XMFLOAT3(tempPos.x, tempPos.y, tempPos.z + 1 * _tileMultiplier));
	//		}
	//	}


	//	if (_inputHandler->IsPressed(VK_DOWN))
	//	{
	//		if (_selectedObj->GetType() == WALL || FLOOR || UNIT)
	//		{
	//			XMFLOAT3 tempPos = _selectedObj->GetPosition();
	//			_selectedObj->SetPosition(XMFLOAT3(tempPos.x, tempPos.y, tempPos.z - 1 * _tileMultiplier));
	//		}
	//	}
	//}
	////Scale Objects
	//if (_inputHandler->IsPressed(VK_NUMPAD6))
	//{
	//	if (_selectedObj->GetType() == WALL || FLOOR || UNIT)
	//	{
	//		XMFLOAT3 tempPos = _selectedObj->GetScale();
	//		_selectedObj->SetScale(XMFLOAT3(tempPos.x+ 1, tempPos.y, tempPos.z ));
	//	}
	//}

	//if (_inputHandler->IsPressed(VK_NUMPAD4))
	//{
	//	if (_selectedObj->GetType() == WALL || FLOOR || UNIT)
	//	{
	//		XMFLOAT3 tempPos = _selectedObj->GetScale();
	//		_selectedObj->SetScale(XMFLOAT3(tempPos.x - 1, tempPos.y, tempPos.z ));
	//	}
	//}

	//if (_inputHandler->IsPressed(VK_NUMPAD8))
	//{
	//	if (_selectedObj->GetType() == WALL || FLOOR || UNIT)
	//	{
	//		XMFLOAT3 tempPos = _selectedObj->GetScale();
	//		_selectedObj->SetScale(XMFLOAT3(tempPos.x , tempPos.y, tempPos.z + 1));
	//	}
	//}

	//if (_inputHandler->IsPressed(VK_NUMPAD2))
	//{
	//	if (_selectedObj->GetType() == WALL || FLOOR || UNIT)
	//	{
	//		XMFLOAT3 tempPos = _selectedObj->GetScale();
	//		_selectedObj->SetScale(XMFLOAT3(tempPos.x, tempPos.y , tempPos.z - 1));
	//	}
	//}
	//

	//if (_inputHandler->IsPressed(0x5A))
	//{
	//	if (_selectedObj->GetType() == WALL || FLOOR || UNIT)
	//	{
	//		XMFLOAT3 tempRot = _selectedObj->GetRotation();
	//		_selectedObj->SetRotation(XMFLOAT3(tempRot.x, tempRot.y + (DirectX::XM_PI/2), tempRot.z));
	//	}
	//}
	//if (_inputHandler->IsPressed(0x58))
	//{
	//	if (_selectedObj->GetType() == WALL || FLOOR || UNIT)
	//	{
	//		XMFLOAT3 tempRot = _selectedObj->GetRotation();
	//		_selectedObj->SetRotation(XMFLOAT3(tempRot.x, tempRot.y - (DirectX::XM_PI / 2), tempRot.z));
	//	}
	//}

	////Temp LevelLoads
	//if (_inputHandler->IsPressed(0x31))
	//{
	//	_objectHandler->Release();
	//	ResetSelectedObj();
	//	_objectHandler->LoadLevel(1);
	//	
	//}

	//if (_inputHandler->IsPressed(0x32))
	//{
	//	_objectHandler->Release();
	//	ResetSelectedObj();
	//	_objectHandler->LoadLevel(2);
	//}

	//if (_inputHandler->IsPressed(0x33))
	//{
	//	_objectHandler->Release();
	//	ResetSelectedObj();
	//	_objectHandler->LoadLevel(3);
	//	
	//}

	//if (_inputHandler->IsPressed(0x34))
	//{
	//	_objectHandler->Release();
	//	ResetSelectedObj();
	//	_objectHandler->LoadLevel(4);
	//	
	//}

	////Press C to init new level
	//if (_inputHandler->IsPressed(0x43))
	//{
	//	InitNewLevel();
	//}

	////Camera mouse control
	//System::MouseCoord mouseCoord = _inputHandler->GetMouseCoord();
	//if (mouseCoord._deltaPos.x != 0 || mouseCoord._deltaPos.y != 0)
	//{
	//	XMFLOAT3 rotation = _camera->GetRotation();
	//	rotation.y += mouseCoord._deltaPos.x / 10.0f;
	//	rotation.x += mouseCoord._deltaPos.y / 10.0f;
	//	_camera->SetRotation(rotation);
	//}

	//XMFLOAT3 forward(0, 0, 0);
	//XMFLOAT3 position = _camera->GetPosition();
	//XMFLOAT3 right(0, 0, 0);
	//bool isMoving = false;
	//float v = 0.1f;
	//if (GetAsyncKeyState('W'))
	//{
	//	forward = _camera->GetForwardVector();
	//	isMoving = true;
	//}
	//else if (GetAsyncKeyState('S'))
	//{
	//	forward = _camera->GetForwardVector();
	//	forward.x *= -1;
	//	forward.y *= -1;
	//	forward.z *= -1;
	//	isMoving = true;
	//}

	//if (GetAsyncKeyState('D'))
	//{
	//	right = _camera->GetRightVector();
	//	isMoving = true;
	//}
	//else if (GetAsyncKeyState('A'))
	//{
	//	right = _camera->GetRightVector();
	//	right.x *= -1;
	//	right.y *= -1;
	//	right.z *= -1;
	//	isMoving = true;
	//}

	//if (isMoving)
	//{
	//	_camera->SetPosition(XMFLOAT3(position.x + (forward.x + right.x) * v, position.y + (forward.y + right.y) * v, position.z + (forward.z + right.z) * v));
	//}

	if (_controls->IsFunctionKeyDown("DEBUG:ENABLE_FREECAM"))
	{
		//_inputDevice->ToggleCursorLock();
	}

	if (_inputDevice->IsPressed(System::Input::LeftMouse))
	{
		if (_isSelectionMode)
		{
			AI::Vec2D pickedTile = _pickingDevice->pickTile(_inputDevice->GetMouseCoord()._pos);
			std::vector<GameObject*> objectsOnTile = _objectHandler->GetTileMap()->GetAllObjectsOnTile(pickedTile);
			if (!objectsOnTile.empty())
			{
				SelectObject(*(objectsOnTile.end() - 1));
			}
		}
		else
		{

		}
	}

	if (_inputDevice->IsPressed(System::Input::RightMouse))
	{
		if (_isSelectionMode)
		{
			SelectObject(nullptr);
		}
		else
		{
			_isSelectionMode = true;
		}
	}

	XMFLOAT3 forward(0, 0, 0);
	XMFLOAT3 position = _camera->GetPosition();
	XMFLOAT3 right(0, 0, 0);
	bool isMoving = false;
	float v = 0.1f;

	if (_controls->IsFunctionKeyDown("MAP_EDIT:MOVE_CAMERA_UP"))
	{
		forward = _camera->GetForwardVector();
		isMoving = true;
	}
	else if (_controls->IsFunctionKeyDown("MAP_EDIT:MOVE_CAMERA_DOWN"))
	{
		forward = _camera->GetForwardVector();
		forward.x *= -1;
		forward.y *= -1;
		forward.z *= -1;
		isMoving = true;
	}

	if (_controls->IsFunctionKeyDown("MAP_EDIT:MOVE_CAMERA_RIGHT"))
	{
		right = _camera->GetRightVector();
		isMoving = true;
	}
	else if (_controls->IsFunctionKeyDown("MAP_EDIT:MOVE_CAMERA_LEFT"))
	{
		right = _camera->GetRightVector();
		right.x *= -1;
		right.y *= -1;
		right.z *= -1;
		isMoving = true;
	}

	if (_controls->IsFunctionKeyDown("MAP_EDIT:MOVE_MARKER_LEFT"))
	{

	}

	if (isMoving)
	{
		_camera->SetPosition(XMFLOAT3(position.x + (forward.x + right.x) * v, position.y + (forward.y + right.y) * v, position.z + (forward.z + right.z) * v));
	}
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
		{
			if (_lastSelected->GetType() == WALL || FLOOR || ENEMY || GUARD)
			{
				_lastSelected = _selectedObj;
				_lastSelected->SetPosition(XMFLOAT3(_lastSelected->GetPosition().x, 0.1f, _lastSelected->GetPosition().z));
				//XMFLOAT3 tempScale = temp->GetScale();
				//temp->SetScale(XMFLOAT3(1.1, 1.1, 1.1));
			}
		}
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
