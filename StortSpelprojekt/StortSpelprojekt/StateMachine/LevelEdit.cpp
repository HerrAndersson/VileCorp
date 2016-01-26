#include "LevelEdit.h"
#include <stdexcept>
#include <DirectXMath.h>
#include <sstream>

// local function
bool compareFloat3(XMFLOAT3 a, XMFLOAT3 b)
{
	if (a.x == b.x && a.z == b.z)
	{
		return true;
	}
	return false;
}

// Instancing
LevelEdit::LevelEdit()
{
	_aStar = nullptr;
}

void LevelEdit::Initialize(ObjectHandler* objectHandler, System::Controls* controls, PickingDevice* pickingDevice, System::Camera* camera)
{
	_objectHandler = objectHandler;
	_controls = controls;
	_pickingDevice = pickingDevice;
	_camera = camera;
	_aStar = new AI::AStar();


	_lastSelected = nullptr;
	_tileMultiplier = 1;
	_isSelectionMode = true;

	LoadLevel(1);

	_marker = nullptr;

	//_grid = new Grid(_renderModule->GetDevice(), 1, 10);
}

LevelEdit::~LevelEdit()
{
	delete _aStar;
}

// Other functions
GameObject* LevelEdit::GetSelectedObject()
{
	return _marker;
}

void LevelEdit::ResetSelectedObj()
{
	//_marker = nullptr;
	_lastSelected = nullptr;
}

bool LevelEdit::Add(Type type, std::string name)
{
	return _objectHandler->Add(type, name, _marker->GetPosition(), XMFLOAT3(0.0f, 0.0f, 0.0f));
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

void LevelEdit::DragAndDrop(Type type)
{
	if (_marker != nullptr && _controls->IsFunctionKeyDown("MAP_EDIT:DRAG"))
	{
		AI::Vec2D pickedTile = _pickingDevice->pickTile(_controls->GetMouseCoord()._pos);
		
		if (_objectHandler->GetTileMap()->IsValid(pickedTile._x, pickedTile._y))
		{
			GameObject* objectOnTile;
			switch (type)
			{
			case FLOOR:
			case WALL:
				objectOnTile = _objectHandler->GetTileMap()->GetObjectOnTile(pickedTile._x, pickedTile._y, 0);
				break;
			case ENEMY:
			case GUARD:
				objectOnTile = _objectHandler->GetTileMap()->GetObjectOnTile(pickedTile._x, pickedTile._y, 1);
				if (objectOnTile == nullptr)
				{
					break;
				}
				objectOnTile = _objectHandler->GetTileMap()->GetObjectOnTile(pickedTile._x, pickedTile._y, 2);
				break;
			case TRAP:
			case LOOT:
				objectOnTile = _objectHandler->GetTileMap()->GetObjectOnTile(pickedTile._x, pickedTile._y, 3);
				break;
			case TRIGGER:
				objectOnTile = _objectHandler->GetTileMap()->GetObjectOnTile(pickedTile._x, pickedTile._y, 4);
				break;
			default:
				break;
			}

			if (objectOnTile == nullptr && _marker->GetType() == type)
			{
				// Remove from old tile
				_objectHandler->GetTileMap()->RemoveObjectFromTile(_marker);

				// Update positions
				//_marker->Translate(pos);
				XMFLOAT3 p = XMFLOAT3(_marker->GetPosition());
				p.x = pickedTile._x;
				p.z = pickedTile._y;

				_marker->SetPosition(p);
				_objectHandler->GetTileMap()->AddObjectToTile(p.x, p.z, _marker);
			}
		}
	}
	if (_controls->IsFunctionKeyUp("MAP_EDIT:SELECT"))
	{
		if (_isSelectionMode)
		{
			_marker = nullptr;
		}
	}
}

void LevelEdit::DragAndDrop()
{
	if (_marker != nullptr)
	{
		DragAndDrop(_marker->GetType());
	}
}

void LevelEdit::HandleInput()
{
	int maxObject = _objectHandler->GetObjectCount();
	int selectedLevel = 1;

	if (_controls->IsFunctionKeyDown("MAP_EDIT:SELECT"))
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

	//Camera mouse control_
	System::MouseCoord mouseCoord = _controls->GetMouseCoord();
	if (mouseCoord._deltaPos.x != 0 || mouseCoord._deltaPos.y != 0)
	{
		XMFLOAT3 rotation = _camera->GetRotation();
		rotation.y += mouseCoord._deltaPos.x / 10.0f;
		rotation.x += mouseCoord._deltaPos.y / 10.0f;
		_camera->SetRotation(rotation);
	}


	if (_camera->GetMode() == System::LOCKED_CAM)
	{
		if (_controls->IsFunctionKeyDown("PLAY:SCROLLDOWN") &&
			_camera->GetPosition().y > 4.0f)
		{
			_camera->Move(XMFLOAT3(0.0f, -1.0f, 0.0f));
		}
		else if (_controls->IsFunctionKeyDown("PLAY:SCROLLUP") &&
			_camera->GetPosition().y < 12.0f)
		{
			_camera->Move(XMFLOAT3(0.0f, 1.0f, 0.0f));
		}
	}


	XMFLOAT3 forward(0, 0, 0);
	XMFLOAT3 position = _camera->GetPosition();
	XMFLOAT3 right(0, 0, 0);
	bool isMoving = false;
	float v = 0.06f + (_camera->GetPosition().y * 0.01);
	if (GetAsyncKeyState('W'))
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
	else if (GetAsyncKeyState('S'))
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

	if (_controls->CursorLocked())
	{
		XMFLOAT3 rotation = _camera->GetRotation();
		rotation.x += _controls->GetMouseCoord()._deltaPos.y / 10.0f;
		rotation.y += _controls->GetMouseCoord()._deltaPos.x / 10.0f;

		_camera->SetRotation(rotation);
	}
}

void LevelEdit::Update(float deltaTime)
{
	HandleInput();
	_objectHandler->Update(deltaTime);
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