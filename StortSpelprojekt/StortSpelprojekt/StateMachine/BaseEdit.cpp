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
	_modeLock = false;
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

	ReleaseBlueprints();
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

bool BaseEdit::CheckValidity(AI::Vec2D tile, Type type)
{
	_tileMap = _objectHandler->GetTileMap();
	GameObject* objectOnTile = _tileMap->GetObjectOnTile(tile._x, tile._y, type);

	bool valid = true;

	if (objectOnTile == nullptr && _tileMap->IsPlaceable(tile, type))
	{
		if (type == WALL && !_tileMap->IsTileEmpty(tile))
		{
			valid = false;
		}
		else
		{
			if (type == GUARD || type == ENEMY)
			{
				if (_tileMap->IsTrapOnTile(tile) || _tileMap->UnitsOnTile(tile))
				{
					valid = false;
				}
			}
			else if (type == TRAP)
			{
				if (_tileMap->IsEnemyOnTile(tile) || _tileMap->IsGuardOnTile(tile))
				{
					valid = false;
				}
			}
		}
	}
	else
	{
		// If outside tilemap
		valid = false;
	}

	return valid;
}

void BaseEdit::SetValidity(Marker* m, Type type)
{
	AI::Vec2D pickedTile = _pickingDevice->PickTile(_controls->GetMouseCoord()._pos);
	Tilemap* tilemap = _objectHandler->GetTileMap();

	if (tilemap && m->_g->GetType() == type)
	{
		GameObject* objectOnTile = tilemap->GetObjectOnTile(pickedTile._x, pickedTile._y, type);

		// New position
		XMFLOAT3 p = XMFLOAT3(m->_g->GetPosition());
		p.x = pickedTile._x;
		p.z = pickedTile._y;

		// Check validity of placement
		m->_placeable = CheckValidity(/*m->_g,*/ pickedTile, type);

		// Move marker grafically
		m->_g->SetPosition(p);

		// Move marker logically but unconnected
		m->_g->SetTilePosition(pickedTile);

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
		_modeLock = true;

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

void BaseEdit::ReleaseBlueprints()
{
	// Delete ghosts/blueprints
	_objectHandler->Remove(_marker._g);
	_marker._g = nullptr;
	_objectHandler->Remove(_baseMarker._g);
	_baseMarker._g = nullptr;
}

void BaseEdit::DragAndDrop(Type type)
{
	if (_controls->IsFunctionKeyUp("MOUSE:SELECT") && _marker._g != nullptr)
	{
		_modeLock = false;
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
		_modeLock = true;
		_isPlace = true;

		// Create ghost/blueprint for _baseMarker
		DragActivate(type, objectName);
		_baseMarker = _marker;
		SetValidity(&_baseMarker, _baseMarker._g->GetType());
		_baseMarker._origPos = _baseMarker._g->GetTilePosition();

		// Create ghost/blueprint for _marker
		DragActivate(type, objectName);
	}

	// Not really diselect but activates remove mode (temp)
	if (_isDragAndPlaceMode && _controls->IsFunctionKeyDown("MOUSE:DESELECT"))
	{
		_isPlace = false;
	}

	if (_isDragAndPlaceMode && _controls->IsFunctionKeyUp("MOUSE:SELECT"))
	{
		_modeLock = false;

		_tileMap = _objectHandler->GetTileMap();
		_marker._origPos = _marker._g->GetTilePosition();

		// Identify min and max
		int minX, maxX;
		if (_baseMarker._origPos._x < _marker._origPos._x)
		{
			minX = _baseMarker._origPos._x;
			maxX = _marker._origPos._x;
		}
		else
		{
			minX = _marker._origPos._x;
			maxX = _baseMarker._origPos._x;
		}
		int minY, maxY;
		if (_baseMarker._origPos._y < _marker._origPos._y)
		{
			minY = _baseMarker._origPos._y;
			maxY = _marker._origPos._y;
		}
		else
		{
			minY = _marker._origPos._y;
			maxY = _baseMarker._origPos._y;
		}

		ReleaseBlueprints();

		// Check if extreme poins is outside Tilemap
		if (minX < 0) minX == 0;
		if (minY < 0) minY == 0;
		if (maxX >= _tileMap->GetWidth()) maxX >= _tileMap->GetWidth() - 1;
		if (maxY >= _tileMap->GetHeight()) maxX >= _tileMap->GetHeight() - 1;


		// Check tiles
		GameObject* objectOnTile;
		if (_isPlace) // Place
		{
			for (int x = minX; x <= maxX; x++)
			{
				for (int y = minY; y <= maxY; y++)
				{
					objectOnTile = _objectHandler->GetTileMap()->GetObjectOnTile(x, y, type);

					if (CheckValidity(AI::Vec2D(x, y), type))
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
	AI::Vec2D pickedTile = _pickingDevice->PickTile(_controls->GetMouseCoord()._pos);

	XMFLOAT3 pos = XMFLOAT3(pickedTile._x, 0, pickedTile._y);

	_objectHandler->Add(type, objectName, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), subType, true);
	_marker._g = _objectHandler->GetGameObjects()->at(type).back();
	_marker._created = false;
	_isPlace = true;
}

void BaseEdit::ChangePlaceState()
{
	if (!_modeLock)
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
	if (_isSelectionMode)
	{
		DragAndDropEvent();
	}
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