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

bool LevelEdit::Add(Type type)
{
	return _objectHandler->Add(type, _marker->GetPosition(), XMFLOAT3(0.0f, 0.0f, 0.0f));
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

bool LevelEdit::TypeOn(Type type)
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

void LevelEdit::Initialize(ObjectHandler* objectHandler, System::Controls* controls, PickingDevice* pickingDevice, System::Camera* camera, GUI::UITree* uiTree)
{
	_objectHandler = objectHandler;
	_controls = controls;
	_pickingDevice = pickingDevice;
	_camera = camera;
	_aStar = new AI::AStar();
	_uiTree = uiTree;

	_lastSelected = nullptr;
	_tileMultiplier = 1;
	_isSelectionMode = true;

	// Add all buttons and hide them
	//buttonInfo.resize(3);
	//TODO: Re-add these buttons //Mattias
	//_uiHandler->AddButton("floor3.png", DirectX::XMFLOAT2(-0.9f, 0.95f), DirectX::XMFLOAT2(0.1f, 0.05f), true);
	//_uiHandler->AddButton("floor.png", DirectX::XMFLOAT2(-0.9f, 0.85f), DirectX::XMFLOAT2(0.1f, 0.05f), false);
	//_uiHandler->AddButton("floor2.png", DirectX::XMFLOAT2(-0.7f, 0.85f), DirectX::XMFLOAT2(0.1f, 0.05f), false);

	//buttonInfo[0].parent = -1;
	//buttonInfo[1].parent = 0;
	//buttonInfo[2].parent = 1;

	LoadLevel(7);
	_uiNode = _uiTree->GetNode("BudgetValue");
	_uiNode->SetText(std::to_wstring(_budget));

	_marker = nullptr;

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



void LevelEdit::DragAndDrop()
{
	// Get existing objects
	if (_controls->IsFunctionKeyDown("PLACEMENT:SELECT"))
	{
		_objects = _objectHandler->GetTileMap()->GetAllObjectsOnTile(_pickingDevice->pickTile(_controls->GetMouseCoord()._pos));
	}
	else if (_controls->IsFunctionKeyUp("PLACEMENT:SELECT"))
	{
		for (int i = 1; i < _objects.size(); i++)
		{
			if (_objects[i]->GetType() == GUARD || _objects[i]->GetType() == TRIGGER || _objects[i]->GetType() == TRAP)
			{
				XMFLOAT3 p = XMFLOAT3(_objects[i]->GetPosition());
				AI::Vec2D pickedTile = _pickingDevice->pickTile(_controls->GetMouseCoord()._pos);
				p.x = pickedTile._x;
				p.z = pickedTile._y;

				_objects[i]->SetPosition(p);
				_objectHandler->GetTileMap()->AddObjectToTile(p.x, p.z, _objects[i]);
				break;
			}
		}
	}

	// Place new object
	System::MouseCoord coord = _controls->GetMouseCoord();
	if (!_uiTree->IsButtonColliding("Guard", coord._pos.x, coord._pos.y) && !_uiTree->IsButtonColliding("Trap", coord._pos.x, coord._pos.y))
	{
		if (_trapPlace == true && _controls->IsFunctionKeyDown("PLACEMENT:SELECT") && _budget - _trapCost >= 0)
		{
			AI::Vec2D pickedTileCoord = _pickingDevice->pickTile(_controls->GetMouseCoord()._pos);
			bool added = _objectHandler->Add(TRAP, XMFLOAT3(pickedTileCoord._x, 0, pickedTileCoord._y), XMFLOAT3(0, 0, 0));

			if (added)
			{
				_budget -= _trapCost;
				_guardPlace = false;
				_uiNode = _uiTree->GetNode("BudgetValue");
				_uiNode->SetText(std::to_wstring(_budget));
			}
		}
		if (_guardPlace == true && _controls->IsFunctionKeyDown("PLACEMENT:SELECT") && _budget - _guardCost >= 0)
		{
			AI::Vec2D pickedTileCoord = _pickingDevice->pickTile(_controls->GetMouseCoord()._pos);
			bool added = _objectHandler->Add(GUARD, XMFLOAT3(pickedTileCoord._x, 0, pickedTileCoord._y), XMFLOAT3(0, 0, 0));

			if (added)
			{
				_budget -= _guardCost;
				_trapPlace = false;
				_uiNode = _uiTree->GetNode("BudgetValue");
				_uiNode->SetText(std::to_wstring(_budget));
			}
		}
	}
	if (_controls->IsFunctionKeyDown("PLACEMENT:DESELECT"))
	{
		_trapPlace = false;
		_guardPlace = false;
	}
}

void LevelEdit::HandleInput()
{
	int maxObject = _objectHandler->GetObjectCount();
	int selectedLevel = 1;

	//Move tile greater distance per button press
	//if (_inputDevice->IsPressed(VK_PRIOR))
	//{
	//	_tileMultiplier++;
	//}

	//if (_inputDevice->IsPressed(VK_NEXT))
	//{
	//	if (_tileMultiplier != 1)
	//	{
	//		_tileMultiplier--;
	//	}
	//}

	//Scale Objects
	/*
		"SCALE_MARKER_LEFT": [""].
	*/
	//if (_inputDevice->IsPressed(VK_NUMPAD6))
	//{
	//	XMFLOAT3 tempPos = _marker->GetScale();
	//	_marker->SetScale(XMFLOAT3(tempPos.x + 1, tempPos.y, tempPos.z));
	//}

	//if (_inputDevice->IsPressed(VK_NUMPAD4))
	//{
	//	XMFLOAT3 tempPos = _marker->GetScale();
	//	_marker->SetScale(XMFLOAT3(tempPos.x - 1, tempPos.y, tempPos.z));
	//}

	//if (_inputDevice->IsPressed(VK_NUMPAD8))
	//{
	//	XMFLOAT3 tempPos = _marker->GetScale();
	//	_marker->SetScale(XMFLOAT3(tempPos.x, tempPos.y, tempPos.z + 1));
	//}

	//if (_inputDevice->IsPressed(VK_NUMPAD2))
	//{
	//	XMFLOAT3 tempPos = _marker->GetScale();
	//	_marker->SetScale(XMFLOAT3(tempPos.x, tempPos.y, tempPos.z - 1));
	//}

	
	//if (_controls->IsFunctionKeyDown("DEBUG:ENABLE_FREECAM"))
	//{
		//_controls->ToggleCursorLock();
	//}


	if (_controls->IsFunctionKeyDown("PLACEMENT:CLICK"))
	{
		System::MouseCoord coord = _controls->GetMouseCoord();
		if (_uiTree->IsButtonColliding("Guard", coord._pos.x, coord._pos.y))
		{
			_guardPlace = true;
			_trapPlace = false;
		}
		if (_uiTree->IsButtonColliding("Trap", coord._pos.x, coord._pos.y))
		{
			_trapPlace = true;
			_guardPlace = false;
		}
	}
	else if (_controls->IsFunctionKeyDown("PLACEMENT:SELECT"))
	{
		if (_isSelectionMode)
		{
			AI::Vec2D pickedTile = _pickingDevice->pickTile(_controls->GetMouseCoord()._pos);
			std::vector<GameObject*> objectsOnTile = _objectHandler->GetTileMap()->GetAllObjectsOnTile(pickedTile);
			if (!objectsOnTile.empty())
			{
				_marker = objectsOnTile.back();
			}
		}
		else
		{

		}
	}

	if (_marker != nullptr)
	{
		DragAndDrop();

		// Rotation
		if (_controls->IsFunctionKeyDown("MAP_EDIT:ROTATE_MARKER_CLOCK"))
		{
			XMFLOAT3 tempRot = _marker->GetRotation();
			_marker->SetRotation(XMFLOAT3(tempRot.x, tempRot.y + (DirectX::XM_PI / 4), tempRot.z));
		}
		if (_controls->IsFunctionKeyDown("MAP_EDIT:ROTATE_MARKER_COUNTERCLOCK"))
		{
			XMFLOAT3 tempRot = _marker->GetRotation();
			_marker->SetRotation(XMFLOAT3(tempRot.x, tempRot.y - (DirectX::XM_PI / 4), tempRot.z));
		}
	}

	//Press C to init new level
	if (_controls->IsFunctionKeyDown("MAP_EDIT:NEWLEVEL"))
	{
		InitNewLevel();
	}

	//Camera mouse control_
	/*System::MouseCoord mouseCoord = _inputDevice->GetMouseCoord();
	if (mouseCoord._deltaPos.x != 0 || mouseCoord._deltaPos.y != 0)
	{
		XMFLOAT3 rotation = _camera->GetRotation();
		rotation.y += mouseCoord._deltaPos.x / 10.0f;
		rotation.x += mouseCoord._deltaPos.y / 10.0f;
		_camera->SetRotation(rotation);
	}*/
	
	if (_camera->GetMode() == System::LOCKED_CAM)
	{
		if (_controls->IsFunctionKeyDown("PLAY:SCROLLDOWN") &&
			_camera->GetPosition().y > 10.0f)
		{
			_camera->Move(XMFLOAT3(0.0f, -1.0f, 0.0f));
		}
		else if (_controls->IsFunctionKeyDown("PLAY:SCROLLUP") &&
			_camera->GetPosition().y < 30.0f)
		{
			_camera->Move(XMFLOAT3(0.0f, 1.0f, 0.0f));
		}
	}


	XMFLOAT3 forward(0, 0, 0);
	XMFLOAT3 position = _camera->GetPosition();
	XMFLOAT3 right(0, 0, 0);
	bool isMoving = false;
	float v = 0.06f + (_camera->GetPosition().y * 0.01);
	if (_controls->IsFunctionKeyDown("DEBUG:ENABLE_FREECAM"))
	{
		if (_camera->GetMode() == System::LOCKED_CAM)
		{
			_controls->ToggleCursorLock();
			_camera->SetMode(System::FREE_CAM);
		}
		else
		{
			_controls->ToggleCursorLock();
			_camera->SetMode(System::LOCKED_CAM);
			_camera->SetRotation(DirectX::XMFLOAT3(70, 0, 0));
		}
	}

	if (_controls->IsFunctionKeyDown("MAP_EDIT:MOVE_CAMERA_UP"))
	{
		if (_camera->GetMode() == System::FREE_CAM)
		{
			forward = _camera->GetForwardVector();
		}
		else if (_camera->GetMode() == System::LOCKED_CAM)
		{
			forward = XMFLOAT3(0.0f, 0.0f, 1.0f);
		}

		isMoving = true;
	}
	if (_controls->IsFunctionKeyDown("MAP_EDIT:MOVE_CAMERA_DOWN"))
	{
		if (_camera->GetMode() == System::FREE_CAM)
		{
			forward = _camera->GetForwardVector();
		}
		else if (_camera->GetMode() == System::LOCKED_CAM)
		{
			forward = XMFLOAT3(0.0f, 0.0f, 1.0f);
		}

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
	if (_controls->IsFunctionKeyDown("MAP_EDIT:MOVE_CAMERA_LEFT"))
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

	if (_controls->CursorLocked())
	{
		XMFLOAT3 rotation = _camera->GetRotation();
		rotation.x += _controls->GetMouseCoord()._deltaPos.y / 10.0f;
		rotation.y += _controls->GetMouseCoord()._deltaPos.x / 10.0f;

		_camera->SetRotation(rotation);
	}

	DragAndDrop();
}

void LevelEdit::Update(float deltaTime)
{
	HandleHUD();
	HandleSelected();
	HandleInput();
	//_objectHandler->Update(deltaTime);
}

void LevelEdit::HandleSelected()
{
	//if (_objectHandler->GetObjectCount() != 0)
	//{
	//	if (_lastSelected != nullptr && _marker != _lastSelected)
	//	{
	//		_lastSelected->SetPosition(XMFLOAT3(_lastSelected->GetPosition().x, 0.1, _lastSelected->GetPosition().z));
	//		//XMFLOAT3 tempScale = temp->GetScale();
	//		//temp->SetScale(XMFLOAT3(1.1, 1.1, 1.1));
	//	}
	//	_lastSelected = _marker;
	//}
}

void LevelEdit::ResetSelectedObj()
{
	//_marker = nullptr;
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
	//float yOffset = 0.1f;

	//if (_selectedObj != nullptr)
	//{
	//	_selectedObj->Translate(DirectX::XMFLOAT3(0, -yOffset, 0));
	//}
	//_selectedObj = selectedObject;

	//if (_selectedObj != nullptr)
	//{
	//	_selectedObj->Translate(DirectX::XMFLOAT3(0, yOffset, 0));
	//}
}
