#include "BaseEdit.h"
#include <stdexcept>
#include <DirectXMath.h>
#include <sstream>

// local function
bool compareFloat3(XMFLOAT3 a, XMFLOAT3 b)
{
	return (a.x == b.x && a.z == b.z);
}

// Instancing
BaseEdit::BaseEdit()
{
	_aStar = nullptr;
}

void BaseEdit::Initialize(ObjectHandler* objectHandler, System::Controls* controls, PickingDevice* pickingDevice, System::Camera* camera)
{
	_objectHandler = objectHandler;
	_controls = controls;
	_pickingDevice = pickingDevice;
	_camera = camera;
	_aStar = new AI::AStar();

	_tileMultiplier = 1;

	// Don´t let both be true
	_isSelectionMode = true;
	_isDragAndPlaceMode = false;
	_isPlace = false;

	LoadLevel(3);

	_marker = nullptr;
	_markedTile = nullptr;

	//_grid = new Grid(_renderModule->GetDevice(), 1, 10);
}

BaseEdit::~BaseEdit()
{
	delete _aStar;
}

// Other functions
GameObject* BaseEdit::GetSelectedObject()
{
	return _marker;
}

bool BaseEdit::Add(Type type, std::string name)
{
	return _objectHandler->Add(type, name, _marker->GetPosition(), XMFLOAT3(0.0f, 0.0f, 0.0f));
}

bool BaseEdit::Delete(Type type)
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

bool BaseEdit::TypeOn(Type type)
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

void BaseEdit::DragAndDrop(Type type)
{


	if (_marker != nullptr && _isSelectionMode && _controls->IsFunctionKeyDown("MAP_EDIT:DRAG"))
	{
		AI::Vec2D pickedTile = _pickingDevice->pickTile(_controls->GetMouseCoord()._pos);
		
		if (_objectHandler->GetTileMap()->IsValid(pickedTile._x, pickedTile._y))
		{
			GameObject* objectOnTile = _objectHandler->GetTileMap()->GetObjectOnTile(pickedTile._x, pickedTile._y, type);

			if (objectOnTile == nullptr && _marker->GetType() == type)
			{
				// Update positions
				XMFLOAT3 p = XMFLOAT3(_marker->GetPosition());
				p.x = pickedTile._x;
				p.z = pickedTile._y;
				Tilemap* tm = _objectHandler->GetTileMap();

				//Check to see if the tile the object will be placed on is free, Aron
				if (tm->IsPlaceable(p.x, p.z, type))
				{
					if (type == WALL && !tm->IsTileEmpty(p.x, p.z))
					{
						return;
					}

					if (_isPlace && !_marker->IsVisible())
					{
						_marker->SetVisibility(true);
					}

					// Remove from old tile
					_objectHandler->GetTileMap()->RemoveObjectFromTile(_marker);

					//Update the object to the new position
					_marker->SetPosition(p);
					_objectHandler->GetTileMap()->AddObjectToTile(p.x, p.z, _marker);
				}				
			}
		}
	}
	if (_controls->IsFunctionKeyUp("MAP_EDIT:SELECT"))
	{
		if (_isSelectionMode)
		{
			if (_marker != nullptr && _isPlace && !_marker->IsVisible())
			{
				_objectHandler->Remove(_marker);
			}
			_marker = nullptr;
			_isPlace = false;
		}
	}
}

void BaseEdit::DragAndDrop()
{
	if (_marker != nullptr)
	{
		DragAndDrop(_marker->GetType());
	}
}

void BaseEdit::DragAndPlace(Type type, std::string objectName)
{
	if (_isDragAndPlaceMode && _controls->IsFunctionKeyUp("MAP_EDIT:SELECT"))
	{
		AI::Vec2D pickedTile = _pickingDevice->pickTile(_controls->GetMouseCoord()._pos);

		// Identify min and max
		int minX, maxX;
		if (_markedTile->_x < pickedTile._x)
		{
			minX = _markedTile->_x;
			maxX = pickedTile._x;
		}
		else
		{
			minX = pickedTile._x;
			maxX = _markedTile->_x;
		}
		int minY, maxY;
		if (_markedTile->_y < pickedTile._y)
		{
			minY = _markedTile->_y;
			maxY = pickedTile._y;
		}
		else
		{
			minY = pickedTile._y;
			maxY = _markedTile->_y;
		}

		// Check if extreme poins is outside Tilemap
		if (minX < 0 || maxX >= _objectHandler->GetTileMap()->GetWidth() ||
			minY < 0 || maxY >= _objectHandler->GetTileMap()->GetHeight())
		{
			return;
		}

		// Check tiles
		GameObject* objectOnTile;
		if (_isPlace) // Place
		{
			for (int x = minX; x <= maxX; x++)
			{
				for (int y = minY; y <= maxY; y++)
				{
					objectOnTile = _objectHandler->GetTileMap()->GetObjectOnTile(x, y, type);

					if (objectOnTile == nullptr)
					{
						// Add to valid place
						_objectHandler->Add(type, objectName, XMFLOAT3(x, 0, y), XMFLOAT3(0.0f, 0.0f, 0.0f));
					}
				}
			}
		}
		else // Remove
		{
			for (int x = minX; x <= maxX; x++)
			{
				for (int y = minY; y <= maxY; y++)
				{
					// TRAP/LOOT/SPAWN OBS!
					objectOnTile = _objectHandler->GetTileMap()->GetObjectOnTile(x, y, type);

					if (objectOnTile != nullptr && type == objectOnTile->GetType())
					{
						// Remove
						_objectHandler->Remove(objectOnTile);
					}
				}
			}
		}

		delete _markedTile;
		_markedTile = nullptr;
	}
}

void BaseEdit::DragActivate(Type type, std::string objectName)
{
	_isPlace = false;
	AI::Vec2D pickedTile = _pickingDevice->pickTile(_controls->GetMouseCoord()._pos);
	
	XMFLOAT3 pos;

	Tilemap* tm = _objectHandler->GetTileMap();
	for (int x = 1; x < tm->GetWidth() - 1; x++)
	{
		for (int z = 1; z < tm->GetHeight() - 1; z++)
		{
			if (tm->IsPlaceable(x, z, type))
			{
				pos = XMFLOAT3(x, 0, z);
				if (_objectHandler->Add(type, objectName, pos, XMFLOAT3(0.0f, 0.0f, 0.0f)))
				{
					_marker = _objectHandler->GetGameObjects()->at(type).back();
					_marker->SetVisibility(false);
					_isPlace = true;
					return;
				}
			}
		}
	}
}

void BaseEdit::ChangePlaceState()
{
	if (_isSelectionMode)
	{
		_isSelectionMode = false;
		_isDragAndPlaceMode = true;
		_isPlace = false;
	}
	else
	{
		_isSelectionMode = true;
		_isDragAndPlaceMode = false;
		_isPlace = false;
	}
}

bool BaseEdit::IsSelection() const
{
	return _isSelectionMode;
}

bool BaseEdit::IsDragAndPlace() const
{
	return _isDragAndPlaceMode;
}

bool BaseEdit::IsPlace() const
{
	return _isPlace;
}

void BaseEdit::HandleInput()
{
	if (_controls->IsFunctionKeyDown("MAP_EDIT:SELECT"))
	{
		if (_isSelectionMode && !_isPlace)
		{
			AI::Vec2D pickedTile = _pickingDevice->pickTile(_controls->GetMouseCoord()._pos);
			std::vector<GameObject*> objectsOnTile = _objectHandler->GetTileMap()->GetAllObjectsOnTile(pickedTile);
			if (!objectsOnTile.empty())
			{
				//Fetches either the floor if there is no other object on the tile, or the object that is on the tile
				_marker = objectsOnTile.back();
			}
		}
		if (_isDragAndPlaceMode)
		{
			_isPlace = true;
			_markedTile = new AI::Vec2D(_pickingDevice->pickTile(_controls->GetMouseCoord()._pos));
		}
	}

	if (_controls->IsFunctionKeyDown("MAP_EDIT:REMOVE"))
	{
		if (_isDragAndPlaceMode)
		{
			_isPlace = false;
			_markedTile = new AI::Vec2D(_pickingDevice->pickTile(_controls->GetMouseCoord()._pos));
		}
	}

	if (_marker != nullptr)
	{
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

	if (_controls->CursorLocked())
	{
		XMFLOAT3 rotation = _camera->GetRotation();
		rotation.x += _controls->GetMouseCoord()._deltaPos.y / 10.0f;
		rotation.y += _controls->GetMouseCoord()._deltaPos.x / 10.0f;

		_camera->SetRotation(rotation);
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

	if (isMoving)
	{
		_camera->SetPosition(XMFLOAT3(position.x + (forward.x + right.x) * v, position.y + (forward.y + right.y) * v, position.z + (forward.z + right.z) * v));
	}
}

void BaseEdit::Update(float deltaTime)
{
	HandleInput();
	//_objectHandler->Update(deltaTime);
}

void BaseEdit::LoadLevel(int levelID)
{
	//load existing level.
	_objectHandler->Release();
	_objectHandler->LoadLevel(levelID);

	_tileMap = _objectHandler->GetTileMap();
	_tilemapHeight = _tileMap->GetHeight();
	_tilemapWidth = _tileMap->GetWidth();
}