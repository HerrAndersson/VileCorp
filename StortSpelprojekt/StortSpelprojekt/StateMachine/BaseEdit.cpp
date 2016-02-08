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
}

void BaseEdit::Initialize(ObjectHandler* objectHandler, System::Controls* controls, PickingDevice* pickingDevice, System::Camera* camera)
{
	_objectHandler = objectHandler;
	_controls = controls;
	_pickingDevice = pickingDevice;
	_camera = camera;

	// Don´t let both be true
	_isSelectionMode = true;
	_isDragAndPlaceMode = false;
	_isPlace = false;

	LoadLevel(3);

	_marker._g = nullptr;
	_baseMarker._g = nullptr;
}

BaseEdit::~BaseEdit()
{
}

void BaseEdit::Release()
{
	_objectHandler->Release();
}

// Other functions
GameObject* BaseEdit::GetSelectedObject()
{
	return _marker._g;
}

bool BaseEdit::Add(Type type, const std::string& name)
{
	return _objectHandler->Add(type, name, _marker._g->GetPosition(), XMFLOAT3(0.0f, 0.0f, 0.0f));
}

bool BaseEdit::Delete(Type type)
{
	vector<GameObject*>* vec = &_objectHandler->GetGameObjects()->at(type);
	for (GameObject* g : *vec)
	{
		if (_marker._g != g && compareFloat3(_marker._g->GetPosition(), g->GetPosition()))
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
		if (_marker._g != g && compareFloat3(_marker._g->GetPosition(), g->GetPosition()))
		{
			return true;
		}
	}

	return false;
}

void BaseEdit::SetValidity(Marker* m, Type type)
{
	AI::Vec2D pickedTile = _pickingDevice->PickTile(_controls->GetMouseCoord()._pos);
	Tilemap* tilemap = _objectHandler->GetTileMap();

	if (tilemap)
	{
		if (tilemap->IsValid(pickedTile._x, pickedTile._y) && m->_g->GetType() == type)
		{
			GameObject* objectOnTile = tilemap->GetObjectOnTile(pickedTile._x, pickedTile._y, type);

			// New position
			XMFLOAT3 p = XMFLOAT3(m->_g->GetPosition());
			p.x = pickedTile._x;
			p.z = pickedTile._y;

			// Reset variable
			m->_placeable = true;

			// Check validity of placement
			if (objectOnTile == nullptr && tilemap->IsPlaceable(p.x, p.z, type))
			{
				if (type == WALL && !tilemap->IsTileEmpty(p.x, p.z))
				{
					m->_placeable = false;
				}
				else
				{
					if (_isPlace && !_marker._g->IsVisible())
					{
						m->_g->SetVisibility(true);
					}

					if (type == GUARD || type == ENEMY)
					{
						if (tilemap->IsTrapOnTile(pickedTile) || tilemap->UnitsOnTile(pickedTile))
						{
							m->_placeable = false;
						}
					}
					else if (type == TRAP)
					{
						if (tilemap->IsEnemyOnTile(pickedTile) || tilemap->IsGuardOnTile(pickedTile))
						{
							m->_placeable = false;
						}
					}
				}
			}
			else
			{
				m->_placeable = false;
			}

			// Move marker grafically
			m->_g->SetPosition(p);

			// Change color to represent placement validity
			if (!m->_placeable)
			{
				m->_g->SetColorOffset(XMFLOAT3(1.0f, 0.0f, 0.0f));
			}
			else
			{
				m->_g->SetColorOffset(XMFLOAT3(0.0f, 1.0f, 0.0f));
			}
		}
	}
}

void BaseEdit::MarkerMoveEvent(Type type)
{
	if (_marker._g != nullptr && _controls->IsFunctionKeyDown("MOUSE:DRAG"))
	{
		SetValidity(&_marker, type);
	}
}

void BaseEdit::DragAndDropEvent()
{
	if (_controls->IsFunctionKeyDown("MOUSE:SELECT") && _isSelectionMode && !_isPlace)
	{
		AI::Vec2D pickedTile = _pickingDevice->PickTile(_controls->GetMouseCoord()._pos);
		std::vector<GameObject*> objectsOnTile = _objectHandler->GetTileMap()->GetAllObjectsOnTile(pickedTile);
		if (!objectsOnTile.empty())
		{
			//Fetches either the floor if there is no other object on the tile, or the object that is on the tile
			_marker._g = objectsOnTile.back();
			_marker._origPos = pickedTile;

			// Remove logically from old tile
			_objectHandler->GetTileMap()->RemoveObjectFromTile(_marker._g);
		}
	}
}

void BaseEdit::DragAndDrop(Type type)
{
	if (_controls->IsFunctionKeyUp("MOUSE:SELECT") && _marker._g != nullptr)
	{
		if (_isSelectionMode)
		{
			Tilemap* tilemap = _objectHandler->GetTileMap();
			_marker._g->SetColorOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
			XMFLOAT3 p = XMFLOAT3(_marker._g->GetPosition());

			if (!_marker._placeable)
			{
				//Redirect position to old pos
				p.x = _marker._origPos._x;
				p.z = _marker._origPos._y;
				_marker._g->SetPosition(p);
			}
			// Bind position logically
			tilemap->AddObjectToTile(p.x, p.z, _marker._g);

			if (_marker._g != nullptr && _isPlace)
			{
				if (!_marker._g->IsVisible())
				{
					_objectHandler->Remove(_marker._g);
				}
				if (_marker._created == false && _marker._placeable == false)
				{
					_objectHandler->Remove(_marker._g);
				}
			}

			_marker.Reset();
			_isPlace = false;
		}
	}
}

void BaseEdit::DragAndDrop()
{
	if (_marker._g != nullptr)
	{
		DragAndDrop(_marker._g->GetType());
	}
}

void BaseEdit::DragAndPlace(Type type, const std::string& objectName)
{
	if (_isDragAndPlaceMode && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		_isPlace = true;

		DragActivate(type, objectName);

		AI::Vec2D _markedTile = AI::Vec2D(_pickingDevice->PickTile(_controls->GetMouseCoord()._pos));

		XMFLOAT3 p = XMFLOAT3(_markedTile._x, 0, _markedTile._y);

		_objectHandler->Add(type, objectName, p, XMFLOAT3(0.0f, 0.0f, 0.0f));

		_baseMarker._g = _objectHandler->GetGameObjects()->at(type).back();
		_baseMarker._origPos = _baseMarker._g->GetTilePosition();
		
		// Remove logically from tilemap to make it ghost
		_objectHandler->GetTileMap()->RemoveObjectFromTile(_baseMarker._g);
		
		SetValidity(&_baseMarker, _baseMarker._g->GetType());

		//Add(type, objectName);
	}

	// Not really diselect but activates remove mode (temp)
	if (_isDragAndPlaceMode && _controls->IsFunctionKeyDown("MOUSE:DESELECT"))
	{
		_isPlace = false;
	}

	//MarkerMoveEvent(type);

	if (_isDragAndPlaceMode && _controls->IsFunctionKeyUp("MOUSE:SELECT"))
	{
		AI::Vec2D pickedTile = _pickingDevice->PickTile(_controls->GetMouseCoord()._pos);

		// Identify min and max
		int minX, maxX;
		if (_baseMarker._origPos._x < pickedTile._x)
		{
			minX = _baseMarker._origPos._x;
			maxX = pickedTile._x;
		}
		else
		{
			minX = pickedTile._x;
			maxX = _baseMarker._origPos._x;
		}
		int minY, maxY;
		if (_baseMarker._origPos._y < pickedTile._y)
		{
			minY = _baseMarker._origPos._y;
			maxY = pickedTile._y;
		}
		else
		{
			minY = pickedTile._y;
			maxY = _baseMarker._origPos._y;
		}

		// Check if extreme poins is outside Tilemap
		if (minX < 0 || maxX >= _objectHandler->GetTileMap()->GetWidth() ||
			minY < 0 || maxY >= _objectHandler->GetTileMap()->GetHeight())
		{
			return;
		}

		// Delete ghosts/blueprints
		_objectHandler->Remove(_marker._g);
		_marker._g = nullptr;
		_objectHandler->Remove(_baseMarker._g);
		_baseMarker._g = nullptr;

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
						_objectHandler->Add(type, objectName, XMFLOAT3(x, 0, y), XMFLOAT3(0.0f, 0.0f, 0.0f), SHARK);
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
	}
}

void BaseEdit::DragActivate(Type type, const std::string& objectName, int subType)
{
	_isPlace = false;
	AI::Vec2D pickedTile = _pickingDevice->PickTile(_controls->GetMouseCoord()._pos);

	XMFLOAT3 pos;

	Tilemap* tm = _objectHandler->GetTileMap();
	for (int x = 1; x < tm->GetWidth() - 1; x++)
	{
		for (int z = 1; z < tm->GetHeight() - 1; z++)
		{
			if (tm->IsPlaceable(x, z, type))
			{
				pos = XMFLOAT3(x, 0, z);
				if (_objectHandler->Add(type, objectName, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), subType))
				{
					_marker._g = _objectHandler->GetGameObjects()->at(type).back();
					_marker._g->SetVisibility(false);
					_marker._created = false;
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
	DragAndDropEvent();

	if (_marker._g != nullptr)
	{
		MarkerMoveEvent(_marker._g->GetType());
	}

	if (_marker._g != nullptr)
	{
		// Rotation
		if (_controls->IsFunctionKeyDown("MAP_EDIT:ROTATE_marker._g_CLOCK"))
		{
			XMFLOAT3 tempRot = _marker._g->GetRotation();
			_marker._g->SetRotation(XMFLOAT3(tempRot.x, tempRot.y + (DirectX::XM_PI / 4), tempRot.z));
		}
		if (_controls->IsFunctionKeyDown("MAP_EDIT:ROTATE_marker._g_COUNTERCLOCK"))
		{
			XMFLOAT3 tempRot = _marker._g->GetRotation();
			_marker._g->SetRotation(XMFLOAT3(tempRot.x, tempRot.y - (DirectX::XM_PI / 4), tempRot.z));
		}
	}

	if (_camera->GetMode() == System::LOCKED_CAM)
	{
		if (_controls->IsFunctionKeyDown("CAMERA:ZOOM_CAMERA_IN") &&
			_camera->GetPosition().y > 4.0f)
		{
			_camera->Move(XMFLOAT3(0.0f, -1.0f, 0.0f));
		}
		else if (_controls->IsFunctionKeyDown("CAMERA:ZOOM_CAMERA_OUT") &&
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

	if (_controls->IsFunctionKeyDown("CAMERA:MOVE_CAMERA_UP"))
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

	if (_controls->IsFunctionKeyDown("CAMERA:MOVE_CAMERA_DOWN"))
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

	if (_controls->IsFunctionKeyDown("CAMERA:MOVE_CAMERA_RIGHT"))
	{
		right = _camera->GetRightVector();
		isMoving = true;
	}

	if (_controls->IsFunctionKeyDown("CAMERA:MOVE_CAMERA_LEFT"))
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
}

void BaseEdit::LoadLevel(int levelID)
{
	//load existing level.
	_objectHandler->Release();
	_objectHandler->LoadLevel(levelID);

	_tileMap = _objectHandler->GetTileMap();
}