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
BaseEdit::BaseEdit(ObjectHandler* objectHandler, System::Controls* controls, PickingDevice* pickingDevice, System::Camera* camera, bool extendedMode)
{
	_objectHandler = objectHandler;
	_controls = controls;
	_pickingDevice = pickingDevice;
	_camera = camera;

	_extendedMode = extendedMode;
	_isSelectionMode = true;
	_isDragAndDropMode = false;
	_isDragAndPlaceMode = false;

	_isPlace = false;
	_modeLock = false;

	_marker._g = nullptr;
	_baseMarker._g = nullptr;

	_tileMap = _objectHandler->GetTileMap();
}

BaseEdit::~BaseEdit()
{
	ReleaseMarkers();
}



// Marker functions

void BaseEdit::CreateMarkers(Type type, const std::string & objectName)
{
	_modeLock = true;

	// Create ghost/blueprint for _baseMarker
	CreateMarker(type, objectName);
	MarkerMoveEvent();
	_baseMarker = _marker;
	_baseMarker._origPos = _baseMarker._g->GetTilePosition();

	// Create ghost/blueprint for _marker
	CreateMarker(type, objectName);
}

void BaseEdit::ReleaseMarkers()
{
	// Delete ghosts/blueprints
	_objectHandler->Remove(_marker._g);
	_marker._g = nullptr;
	_objectHandler->Remove(_baseMarker._g);
	_baseMarker._g = nullptr;
}

void BaseEdit::CreateMarker(Type type, const std::string& objectName, int subType)
{
	AI::Vec2D pickedTile = _pickingDevice->PickTile(_controls->GetMouseCoord()._pos);
	XMFLOAT3 pos = XMFLOAT3(pickedTile._x, 0, pickedTile._y);

	_objectHandler->Add(type, objectName, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), subType, true);
	_marker._g = _objectHandler->GetGameObjects()->at(type).back();
	_marker._created = false;
	_isPlace = true;
}

GameObject* BaseEdit::GetMarkedObject()
{
	return _marker._g;
}

bool BaseEdit::DeleteMarkedObject()
{
	bool removed = _objectHandler->Remove(_marker._g);
	_marker.Reset();
	_isPlace = false;
	return removed;
}



// Key events

void BaseEdit::MarkerMoveEvent()
{
	AI::Vec2D pickedTile = _pickingDevice->PickTile(_controls->GetMouseCoord()._pos);

	if (_tileMap)
	{
		GameObject* objectOnTile = _tileMap->GetObjectOnTile(pickedTile._x, pickedTile._y, _marker._g->GetType());

		// New position
		XMFLOAT3 p = XMFLOAT3(_marker._g->GetPosition());
		p.x = pickedTile._x;
		p.z = pickedTile._y;

		// Check validity of placement
		_marker._placeable = CheckValidity(pickedTile, _marker._g->GetType());

		// Move marker grafically
		_marker._g->SetPosition(p);

		// Move marker logically but unconnected
		_marker._g->SetTilePosition(pickedTile);

		// Change color to represent placement validity
		if (!_marker._placeable)
		{
			_marker._g->SetColorOffset(XMFLOAT3(1.0f, 0.0f, 0.0f));
		}
		else
		{
			_marker._g->SetColorOffset(XMFLOAT3(0.0f, 1.0f, 0.0f));
		}
	}
}

void BaseEdit::DragEvent(Type type)
{
	if (_controls->IsFunctionKeyDown("MOUSE:SELECT") && !_isPlace)
	{
		AI::Vec2D pickedTile = _pickingDevice->PickTile(_controls->GetMouseCoord()._pos);

		if (_tileMap->IsTypeOnTile(pickedTile, type))
		{
			_modeLock = true;

			//Fetches either the floor if there is no other object on the tile, or the object that is on the tile
			_marker._g = _tileMap->GetObjectOnTile(pickedTile, type);
			_marker._origPos = pickedTile;

			// Remove logically from old tile
			_tileMap->RemoveObjectFromTile(_marker._g);

			_isDragAndDropMode = true;
		}
	}
}

void BaseEdit::DropEvent()
{
	_modeLock = false;
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
	_tileMap->AddObjectToTile(p.x, p.z, _marker._g);

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
	_isDragAndDropMode = false;
}

void BaseEdit::HandleMouseInput()
{
	if (_extendedMode && _isSelectionMode)
	{
		if (_marker._g == nullptr)
		{
			// Drag in world
			bool found = false;
			for (int i = Type::NR_OF_TYPES - 1; i > -1 && !found; i--)
			{
				DragEvent((Type)i);
				if (_marker._g != nullptr)
				{
					found = true;
				}
			}
		}
		else
		{
			// For buttons
			DragEvent(_marker._g->GetType());
		}
	}
	else
	{
		DragEvent(TRAP);
		DragEvent(GUARD);
		DragEvent(CAMERA);
	}

	if (_marker._g != nullptr)
	{
		MarkerMoveEvent();

		if ((_isDragAndDropMode && _controls->IsFunctionKeyUp("MOUSE:SELECT"))
			|| (!_isDragAndDropMode && _controls->IsFunctionKeyDown("MOUSE:SELECT")))
		{
			DropEvent();
		}
	}
}

void BaseEdit::HandleKeyInput(double deltaTime)
{
	if (_marker._g != nullptr)
	{
		// Rotation
		bool rotated = false;
		bool clockwise = false;
		if (_controls->IsFunctionKeyDown("MAP_EDIT:ROTATE_MARKER_CLOCK"))
		{
			XMFLOAT3 tempRot = _marker._g->GetRotation();
			_marker._g->SetRotation(XMFLOAT3(tempRot.x, tempRot.y + (DirectX::XM_PI / 4), tempRot.z));
			rotated = true;
			clockwise = true;
		}
		if (_controls->IsFunctionKeyDown("MAP_EDIT:ROTATE_MARKER_COUNTERCLOCK"))
		{
			XMFLOAT3 tempRot = _marker._g->GetRotation();
			_marker._g->SetRotation(XMFLOAT3(tempRot.x, tempRot.y - (DirectX::XM_PI / 4), tempRot.z));
			rotated = true;
			clockwise = false;
		}
		if (rotated)
		{
			//TODO Make general for GameObject --Victor
			if (_marker._g->GetType() == GUARD || _marker._g->GetType() == ENEMY)
			{
				static_cast<Unit*>(_marker._g)->SetDirection(AI::GetNextDirection(static_cast<Unit*>(_marker._g)->GetDirection(), clockwise));
			}
			else if (_marker._g->GetType() == CAMERA)
			{
				static_cast<SecurityCamera*>(_marker._g)->SetDirection(AI::GetNextDirection(static_cast<SecurityCamera*>(_marker._g)->GetDirection(), clockwise));
			}
		}
	}

	/*
	Toggle free camera mode
	*/
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

	/*
	Camera scroll
	*/
	if (_camera->GetMode() == System::LOCKED_CAM)
	{
		if (_controls->IsFunctionKeyDown("CAMERA:ZOOM_CAMERA_IN") &&
			_camera->GetPosition().y > 4.0f)
		{
			_camera->Move(XMFLOAT3(0.0f, -1.0f, 0.0f), deltaTime);
		}
		else if (_controls->IsFunctionKeyDown("CAMERA:ZOOM_CAMERA_OUT") &&
			_camera->GetPosition().y < 12.0f)
		{
			_camera->Move(XMFLOAT3(0.0f, 1.0f, 0.0f), deltaTime);
		}
	}

	/*
	Camera rotation
	*/
	bool isRotating = false;
	XMFLOAT3 rotation(0.0f, 0.0f, 0.0f);
	float rotV = 1.4f;

	if (_camera->GetMode() == System::LOCKED_CAM)
	{
		if (_controls->IsFunctionKeyDown("CAMERA:ROTATE_CAMERA_LEFT"))
		{
			rotation.y = 1.0f;
			isRotating = true;
		}
		if (_controls->IsFunctionKeyDown("CAMERA:ROTATE_CAMERA_RIGHT"))
		{
			rotation.y = -1.0f;
			isRotating = true;
		}

		if (isRotating)
		{
			_camera->Rotate(XMFLOAT3((rotation.x * rotV), (rotation.y * rotV), (rotation.z * rotV)));
		}
	}

	//Camera rotation - locked mouse
	if (_controls->CursorLocked())
	{
		XMFLOAT3 rotation = _camera->GetRotation();
		rotation.x += _controls->GetMouseCoord()._deltaPos.y / 10.0f;
		rotation.y += _controls->GetMouseCoord()._deltaPos.x / 10.0f;

		_camera->SetRotation(rotation);
	}

	/*
	Camera move
	*/
	XMFLOAT3 right(0.0f, 0.0f, 0.0f);
	XMFLOAT3 forward(0.0f, 0.0f, 0.0f);
	bool isMoving = false;
	float v = 0.06f + (_camera->GetPosition().y * 0.01);

	if (_controls->IsFunctionKeyDown("CAMERA:MOVE_CAMERA_UP"))
	{
		if (_camera->GetMode() == System::FREE_CAM)
		{
			forward = _camera->GetForwardVector();
		}
		else if (_camera->GetMode() == System::LOCKED_CAM)
		{
			forward = (Vec3(_camera->GetRightVector()).Cross(Vec3(XMFLOAT3(0.0f, 1.0f, 0.0f))).convertToXMFLOAT());
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
			forward = (Vec3(_camera->GetRightVector()).Cross(Vec3(XMFLOAT3(0.0f, 1.0f, 0.0f))).convertToXMFLOAT());
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
		_camera->Move(XMFLOAT3((forward.x + right.x) * v, (forward.y + right.y) * v, (forward.z + right.z) * v), deltaTime);
	}
}



// Other functions

bool BaseEdit::CheckValidity(AI::Vec2D tile, Type type)
{
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
				if (_tileMap->UnitsOnTile(tile) || _tileMap->IsTrapOnTile(tile) || _tileMap->IsTypeOnTile(tile, CAMERA))
				{
					valid = false;
				}
			}
			else if (type == TRAP)
			{
				if (_tileMap->UnitsOnTile(tile) || _tileMap->IsTypeOnTile(tile, CAMERA))
				{
					valid = false;
				}
			}
			else if (type == CAMERA)
			{
				if (_tileMap->UnitsOnTile(tile) || _tileMap->IsTrapOnTile(tile))
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

void BaseEdit::DragAndPlace(Type type, const std::string& objectName)
{
	if (_isDragAndPlaceMode)
	{
		if (!_modeLock)
		{
			if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
			{
				CreateMarkers(type, objectName);
				_isPlace = true;
			}

			// Not really diselect but activates remove mode (temp)
			if (_controls->IsFunctionKeyDown("MOUSE:DESELECT"))
			{
				CreateMarkers(type, objectName);
				_isPlace = false;
			}
		}

		if ((_controls->IsFunctionKeyUp("MOUSE:SELECT") && _isPlace) || (_controls->IsFunctionKeyUp("MOUSE:DESELECT") && !_isPlace))
		{
			_modeLock = false;

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

			ReleaseMarkers();

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
						objectOnTile = _tileMap->GetObjectOnTile(x, y, type);

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
						objectOnTile = _tileMap->GetObjectOnTile(x, y, type);

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
}

void BaseEdit::ChangePlaceState()
{
	if (!_modeLock)
	{
		if (_isSelectionMode)
		{
			_isSelectionMode = false;
			_isDragAndDropMode = true;
			_isDragAndPlaceMode = false;
			_isPlace = false;
		}
		else
		{
			_isSelectionMode = true;
			_isDragAndDropMode = false;
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

void BaseEdit::Update(float deltaTime)
{
	HandleMouseInput();
	HandleKeyInput(deltaTime);
}