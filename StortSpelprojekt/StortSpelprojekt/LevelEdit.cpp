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

void LevelEdit::Initialize(ObjectHandler* objectHandler, System::InputDevice* inputDevice, System::Controls* controls, PickingDevice* pickingDevice, System::Camera* camera, UIHandler* uiHandler)
{
	_objectHandler = objectHandler;
	_inputDevice = inputDevice;
	_controls = controls;
	_pickingDevice = pickingDevice;
	_camera = camera;
	_aStar = new AI::AStar();
	_uiHandler = uiHandler;

	_selectedObj = nullptr;
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

	LoadLevel(0);
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

	//Move tile greater distance per button press
	/*if (_inputDevice->IsPressed(VK_PRIOR))
	{
		_tileMultiplier++;
	}

	if (_inputDevice->IsPressed(VK_NEXT))
	{
		if (_tileMultiplier != 1)
		{
			_tileMultiplier--;
		}
	}
	*/
	/*
		//Check if there is a tile to move
		if (_objectHandler->GetTotalNrOfGameObjects() > 0)
		{
			//Translation and rotation controls
			if (_inputHandler->IsPressed(VK_LEFT))
			{
				_selectedObj->Translate(XMFLOAT3(-1, 0, 0));

				//XMFLOAT3 tempPos = temp->GetPosition();
				//tempPos = XMFLOAT3(tempPos.x - 1 * _tileMultiplier, tempPos.y, tempPos.z);
				//temp->SetPosition(tempPos);
				//_lastObjPosition = tempPos;
			}

			if (_inputHandler->IsPressed(VK_RIGHT))
			{
				_selectedObj->Translate(XMFLOAT3(+1, 0, 0));

				//XMFLOAT3 tempPos = temp->GetPosition();
				//tempPos = XMFLOAT3(tempPos.x + 1 * _tileMultiplier, tempPos.y, tempPos.z);
				//temp->SetPosition(tempPos);
				//_lastObjPosition = tempPos;
			}

			if (_inputHandler->IsPressed(VK_UP))
			{
				_selectedObj->Translate(XMFLOAT3(0, 1, 0));

				//XMFLOAT3 tempPos = temp->GetPosition();
				//tempPos = XMFLOAT3(tempPos.x, tempPos.y, tempPos.z + 1 * _tileMultiplier);
				//temp->SetPosition(tempPos);
				//_lastObjPosition = tempPos;
			}


			if (_inputHandler->IsPressed(VK_DOWN))
			{
				_selectedObj->Translate(XMFLOAT3(0, -1, 0));

				//XMFLOAT3 tempPos = temp->GetPosition();
				//tempPos = XMFLOAT3(tempPos.x, tempPos.y, tempPos.z - 1 * _tileMultiplier);
				//temp->SetPosition(tempPos);
				//_lastObjPosition = tempPos;
			}
		}
		//Scale Objects
		if (_inputHandler->IsPressed(VK_NUMPAD6))
		{
			_selectedObj->Scale(XMFLOAT3(1, 0, 0));

			//XMFLOAT3 tempPos = temp->GetScale();
			//temp->SetScale(XMFLOAT3(tempPos.x+ 1, tempPos.y, tempPos.z ));
		}

		if (_inputHandler->IsPressed(VK_NUMPAD4))
		{
			_selectedObj->Scale(XMFLOAT3(-1, 0, 0));

			//XMFLOAT3 tempPos = temp->GetScale();
			//temp->SetScale(XMFLOAT3(tempPos.x - 1, tempPos.y, tempPos.z ));
		}

		if (_inputHandler->IsPressed(VK_NUMPAD8))
		{
			_selectedObj->Scale(XMFLOAT3(0, 0, 1));

			//XMFLOAT3 tempPos = temp->GetScale();
			//temp->SetScale(XMFLOAT3(tempPos.x, tempPos.y, tempPos.z + 1));
		}

		if (_inputHandler->IsPressed(VK_NUMPAD2))
		{
			_selectedObj->Scale(XMFLOAT3(0, 0, -1));

			//XMFLOAT3 tempPos = temp->GetScale();
			//temp->SetScale(XMFLOAT3(tempPos.x, tempPos.y, tempPos.z - 1));
		}


		if (_inputHandler->IsPressed(0x5A))
		{
			_selectedObj->Rotate(XMFLOAT3(0, (DirectX::XM_PI / 2), 0));

			//XMFLOAT3 tempRot = temp->GetRotation();
			//temp->SetRotation(XMFLOAT3(tempRot.x, tempRot.y + (DirectX::XM_PI/2), tempRot.z));
		}
		if (_inputHandler->IsPressed(0x58))
		{
			_selectedObj->Rotate(XMFLOAT3(0, -(DirectX::XM_PI / 2), 0));

			//XMFLOAT3 tempRot = temp->GetRotation();
			//temp->SetRotation(XMFLOAT3(tempRot.x, tempRot.y - (DirectX::XM_PI/2), tempRot.z));
		}
	*/
	//Temp LevelLoads
	/*if (_inputDevice->IsPressed(0x31))
	{
		LoadLevel(0);
	}

	if (_inputDevice->IsPressed(0x32))
	{
		LoadLevel(1);
	}

	if (_inputDevice->IsPressed(0x33))
	{
		LoadLevel(2);
	}

	if (_inputDevice->IsPressed(0x34))
	{
		LoadLevel(3);
	}


	//Press C to init new level
	if (_inputDevice->IsPressed(System::Input::C))
	{
		InitNewLevel();
	}

	*/
	//tempAddObj
	/*
	//R Adds Floor
	if (_inputHandler->IsPressed(System::Input::R))
	{
			_objectHandler->Add(FLOOR, FLOOR, _lastObjPosition, XMFLOAT3(0.0f, 0.0f, 0.0f));
			//_selectedObj = _objectHandler->Find(_objectHandler->GetTotalNrOfGameObjects() - 1);
	}

	//T adds Wall
	if (_inputHandler->IsPressed(System::Input::T))
	{
			_objectHandler->Add(WALL, WALL, _lastObjPosition, XMFLOAT3(0.0f, 0.0f, 0.0f));
			//_selectedObj = _objectHandler->Find(_objectHandler->GetTotalNrOfGameObjects() - 1);
	}

	//Y adds Unit
	if (_inputHandler->IsPressed(System::Input::Y))
	{
			_objectHandler->Add(UNIT, UNIT, _lastObjPosition, XMFLOAT3(0.0f, 0.0f, 0.0f));
			//_selectedObj = _objectHandler->Find(_objectHandler->GetTotalNrOfGameObjects() - 1);
	}

	//U adds Goal
	if (_inputHandler->IsPressed(System::Input::U))
	{
		_objectHandler->Add(LOOT, LOOT, _lastObjPosition, XMFLOAT3(0.0f, 0.0f, 0.0f));
		//_selectedObj = _objectHandler->Find(_objectHandler->GetTotalNrOfGameObjects() - 1);
	}

	//I adds ENTRY
	if (_inputHandler->IsPressed(System::Input::I))
	{
		//TODO: Add ENTRY to enum list
		//_objectHandler->Add(ENTRY, ENTRY, _lastObjPosition, XMFLOAT3(0.0f, 0.0f, 0.0f));
		//_selectedObj = _objectHandler->GetSize() - 1;
	}
	*/

	/*if (_inputDevice->IsPressed(VK_DELETE))
	{
		DeleteObject();
	}
	*/

	//Camera mouse control
	//if (_inputDevice->GetCursorLock())
	//{
	//	float sensitivity = 5.0f;
	//	if (mouseCoord._deltaPos.x != 0 || mouseCoord._deltaPos.y != 0)
	//	{
	//		XMFLOAT3 rotation = _camera->GetRotation();
	//		rotation.y += mouseCoord._deltaPos.x / sensitivity;
	//		rotation.x += mouseCoord._deltaPos.y / sensitivity;
	//		_camera->SetRotation(rotation);
	//	}
	//}
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

	if (isMoving)
	{
		_camera->SetPosition(XMFLOAT3(position.x + (forward.x + right.x) * v, position.y + (forward.y + right.y) * v, position.z + (forward.z + right.z) * v));
	}
	/*

	if (GetAsyncKeyState('Q'))
	{
		ExportLevel();
	}*/

	/*
	// Testing of map
	if (GetAsyncKeyState('F'))
	{
		_aStar->SetStartPosition({ (int)_lastObjPosition.x, (int)_lastObjPosition.y });
	}

	else if (GetAsyncKeyState('G'))
	{
		_aStar->SetGoalPosition({ (int)_lastObjPosition.x, (int)_lastObjPosition.y });
	}

	else if (GetAsyncKeyState('H'))
	{
		_aStar->CleanMap();
		_aStar->FindPath();
	}
	*/
}

void LevelEdit::Update(float deltaTime)
{
	mouseCoord = _inputDevice->GetMouseCoord();
	HandleHUD();
	HandleInput();
	_objectHandler->Update(deltaTime);
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

void LevelEdit::DeleteObject()
{
	//GameObject* temp;
	if (_selectedObj != nullptr)
	{
		//temp = _objectHandler->Find(_selectedObj);
		_objectHandler->Remove(_selectedObj);
		_selectedObj = nullptr;
	}
}

void LevelEdit::ExportLevel()
{
	LevelHeader levelHead;
	levelHead._version = 10;
	Tilemap* tileMap = _objectHandler->GetTileMap();
	levelHead._levelSizeY = tileMap->GetHeight();
	levelHead._levelSizeX = tileMap->GetWidth();
	levelHead._nrOfGameObjects = _objectHandler->GetTotalNrOfGameObjects();

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

	while (size = fread(buf, 1, BUFSIZ, source)) {
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
		_selectedObj->Translate(DirectX::XMFLOAT3(0,-yOffset,0));
	}
	_selectedObj = selectedObject;

	if (_selectedObj != nullptr)
	{
		_selectedObj->Translate(DirectX::XMFLOAT3(0, yOffset, 0));
	}
}
