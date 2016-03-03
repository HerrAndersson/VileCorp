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
BaseEdit::BaseEdit(ObjectHandler* objectHandler, System::Controls* controls, PickingDevice* pickingDevice, bool extendedMode)
{
	_objectHandler = objectHandler;
	_controls = controls;
	_pickingDevice = pickingDevice;

	_extendedMode = extendedMode;
	_isSelectionMode = true;
	_isDragAndDropMode = false;
	_isDragAndPlaceMode = false;

	_isPlace = false;
	_modeLock = false;
	_isInvalidateFloor = false;
	_isObjectButtonReleased = true;

	_sB = nullptr;
	_movingGhostImage._g = nullptr;
	_baseGhostImage._g = nullptr;
	_tileMap = _objectHandler->GetTileMap();
}

BaseEdit::~BaseEdit()
{
	RemoveGhostImage();
}


// Marker functions

void BaseEdit::AddBoxGhostImage()
{
	_modeLock = true;

	// Create ghost/blueprint for _baseMarker
	if (_movingGhostImage._g == nullptr)
	{
		AddGhostImage();
	}
	MarkerMoveEvent();
	_baseGhostImage = _movingGhostImage;
	_baseGhostImage._origPos = _baseGhostImage._g->GetTilePosition();


	// Create ghost/blueprint for _marker
	AddGhostImage();
}

void BaseEdit::AddGhostImage()
{
	AI::Vec2D pickedTile = _pickingDevice->PickTile(_controls->GetMouseCoord()._pos);
	XMFLOAT3 pos = XMFLOAT3(pickedTile._x, 0, pickedTile._y);

	_movingGhostImage._g = _objectHandler->Add(_sB->_blueprint, _sB->_textureId, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), false);
	_movingGhostImage._origPos = _movingGhostImage._g->GetTilePosition();
	_movingGhostImage._origRot = _movingGhostImage._g->GetRotation();
	_movingGhostImage._origDir = _movingGhostImage._g->GetDirection();
	_movingGhostImage._created = false;
	_isPlace = true;
	_isObjectButtonReleased = false;
}

//GameObject* BaseEdit::GetMarkedObject()
//{
//	return _movingGhostImage._g;
//}

void BaseEdit::RemoveGhostImage()
{
	// Delete ghosts/blueprints
	if (_movingGhostImage._g != nullptr)
	{
		_objectHandler->Remove(_movingGhostImage._g);
		_movingGhostImage._g = nullptr;
	}
	if (_baseGhostImage._g != nullptr)
	{
		_objectHandler->Remove(_baseGhostImage._g);
		_baseGhostImage._g = nullptr;
	}
}


// Key events

void BaseEdit::MarkerMoveEvent()
{
	AI::Vec2D pickedTile = _pickingDevice->PickTile(_controls->GetMouseCoord()._pos);

	if (_tileMap)
	{
		GameObject* objectOnTile = _tileMap->GetObjectOnTile(pickedTile._x, pickedTile._y, _movingGhostImage._g->GetType());

		// New position
		XMFLOAT3 p = XMFLOAT3(_movingGhostImage._g->GetPosition());
		p.x = pickedTile._x;
		p.z = pickedTile._y;

		// Check validity of placement
		_movingGhostImage._placeable = CheckValidity(pickedTile, _movingGhostImage._g->GetType());

		// Move marker graphically
		_movingGhostImage._g->SetPosition(p);

		// Move marker logically but unconnected
		_movingGhostImage._g->SetTilePosition(pickedTile);

		// Change color to represent placement validity
		if (!_movingGhostImage._placeable)
		{
			_movingGhostImage._g->SetColorOffset(XMFLOAT3(1.0f, 0.0f, 0.0f));
		}
		else
		{
			_movingGhostImage._g->SetColorOffset(XMFLOAT3(0.0f, 1.0f, 0.0f));
		}
	}
}

void BaseEdit::DragEvent(System::Type type)
{
	if (_controls->IsFunctionKeyDown("MOUSE:SELECT") && !_isPlace)
	{
		AI::Vec2D pickedTile = _pickingDevice->PickTile(_controls->GetMouseCoord()._pos);

		if (_tileMap->IsTypeOnTile(pickedTile, type))
		{
			_modeLock = true;

			//Fetches either the floor if there is no other object on the tile, or the object that is on the tile
			_movingGhostImage._g = _tileMap->GetObjectOnTile(pickedTile, type);
			_movingGhostImage._origPos = pickedTile;
			_movingGhostImage._origRot = _movingGhostImage._g->GetRotation();
			_movingGhostImage._origDir = _movingGhostImage._g->GetDirection();

			// Remove logically from old tile
			_tileMap->RemoveObjectFromTile(_movingGhostImage._g);

			_isDragAndDropMode = true;
		}
	}
}

void BaseEdit::DropEvent()
{
	_modeLock = false;
	_movingGhostImage._g->SetColorOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
	XMFLOAT3 p = XMFLOAT3(_movingGhostImage._g->GetPosition());
	
	if (!_movingGhostImage._placeable)
	{
		//Redirect position to old pos
		p.x = _movingGhostImage._origPos._x;
		p.z = _movingGhostImage._origPos._y;
		_movingGhostImage._g->SetPosition(p);
		_movingGhostImage._g->SetRotation(_movingGhostImage._origRot);
		_movingGhostImage._g->SetDirection(_movingGhostImage._origDir);
	}

	// Special camera non floating fix
	if (_movingGhostImage._g->GetType() == System::CAMERA || _movingGhostImage._g->GetType())
	{
		if (_movingGhostImage._g->GetDirection()._x != 0 && _movingGhostImage._g->GetDirection()._y != 0)
		{
			XMFLOAT3 cameraDiagonal = _movingGhostImage._g->GetPosition();
			cameraDiagonal.x -= (float)_movingGhostImage._g->GetDirection()._x*0.2;
			cameraDiagonal.z -= (float)_movingGhostImage._g->GetDirection()._y*0.2;
			_movingGhostImage._g->SetPosition(cameraDiagonal);
		}
	}

	// Bind position logically
	_tileMap->AddObjectToTile(p.x, p.z, _movingGhostImage._g);

	if (_movingGhostImage._g != nullptr && _isPlace)
	{
		_droppedObject = true;
		if (!_movingGhostImage._g->IsVisible() || (_movingGhostImage._created == false && _movingGhostImage._placeable == false))
		{
			_droppedObject = false;
			_objectHandler->Remove(_movingGhostImage._g);
		}
	}

	if (_isDragAndDropMode)
	{
		_movingGhostImage.Reset();
		_isPlace = false;
		_isDragAndDropMode = false;
	}
	else
	{
		if (_droppedObject)
		{
			_tileMap->RemoveObjectFromTile(_movingGhostImage._g);
			_createdObject = _objectHandler->Add(_sB->_blueprint, _sB->_textureId, _movingGhostImage._g->GetPosition(), _movingGhostImage._g->GetRotation(), true);
			if (_createdObject != nullptr)
			{
				_createdObject->SetDirection(_movingGhostImage._g->GetDirection());
			}
		}

		// Disables multiplacement with mouse in placement state
		if (!_extendedMode)
		{
			//RemoveGhostImage();
			_isPlace = false;
			_isDragAndDropMode = false;
		}
	}
}

void BaseEdit::BoxEvent()
{
	if ((_controls->IsFunctionKeyUp("MOUSE:SELECT") && _isPlace) || (_controls->IsFunctionKeyUp("MOUSE:DESELECT") && !_isPlace))
	{
		_modeLock = false;

		_movingGhostImage._origPos = _movingGhostImage._g->GetTilePosition();

		// Identify min and max
		int minX, maxX;
		if (_baseGhostImage._origPos._x < _movingGhostImage._origPos._x)
		{
			minX = _baseGhostImage._origPos._x;
			maxX = _movingGhostImage._origPos._x;
		}
		else
		{
			minX = _movingGhostImage._origPos._x;
			maxX = _baseGhostImage._origPos._x;
		}
		int minY, maxY;
		if (_baseGhostImage._origPos._y < _movingGhostImage._origPos._y)
		{
			minY = _baseGhostImage._origPos._y;
			maxY = _movingGhostImage._origPos._y;
		}
		else
		{
			minY = _movingGhostImage._origPos._y;
			maxY = _baseGhostImage._origPos._y;
		}

		RemoveGhostImage();

		// Check tiles
		GameObject* objectOnTile;
		if (_isPlace) // Place
		{
			for (int x = minX; x <= maxX; x++)
			{
				for (int y = minY; y <= maxY; y++)
				{
					if (CheckValidity(AI::Vec2D(x, y), _sB->_blueprint->_type))
					{
						// Add to valid place
						_objectHandler->Add(_sB->_blueprint, _sB->_textureId, XMFLOAT3(x, 0, y), XMFLOAT3(0.0f, 0.0f, 0.0f), true);
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
					GameObject* objectOnTile = _tileMap->GetObjectOnTile(x, y, _sB->_blueprint->_type);
					if (objectOnTile != nullptr && _sB->_blueprint->_type == objectOnTile->GetType())
					{
						// Remove
						_objectHandler->Remove(objectOnTile);
					}
				}
			}
		}

		_isSelectionMode = true;
		_isDragAndPlaceMode = false;

		_isDragAndDropMode = false;
		_isPlace = false;

	}
}

void BaseEdit::HandleMouseInput()
{
	if (_extendedMode)
	{
		if (_controls->IsFunctionKeyDown("MAP_EDIT:UNDO"))
		{
			_isInvalidateFloor = !_isInvalidateFloor;
		}
		if (_isInvalidateFloor)
		{
			AI::Vec2D pickedTile = _pickingDevice->PickTile(_controls->GetMouseCoord()._pos);
			if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
			{
				_tileMap->LockTile(pickedTile);

			}
			if (_controls->IsFunctionKeyDown("MOUSE:DESELECT"))
			{
				_tileMap->UnlockTile(pickedTile);
			}

		}
		else
		{
			if (_controls->IsFunctionKeyDown("MOUSE:BOX_PLACE") || _controls->IsFunctionKeyDown("MOUSE:BOX_DELETE"))
			{
				if (!_modeLock && _isSelectionMode)
				{
					_isSelectionMode = false;
					_isDragAndPlaceMode = true;

					_isDragAndDropMode = false;
					_isPlace = false;
				}
			}

			if (_isSelectionMode)
			{
				if (_movingGhostImage._g == nullptr)
				{
					// Drag in world
					bool found = false;
					for (int i = System::Type::NR_OF_TYPES - 1; i > -1 && !found; i--)
					{
						DragEvent((System::Type)i);
						if (_movingGhostImage._g != nullptr)
						{
							found = true;
						}
					}
				}
				//else
				//{
				//	// For buttons
				//	DragEvent(_marker._g->GetType());
				//}
			}
			else
			{
				if (_sB != nullptr)
				{
					if (!_modeLock)
					{
						if (_controls->IsFunctionKeyDown("MOUSE:BOX_PLACE"))
						{
							AddBoxGhostImage();
							_isPlace = true;
						}

						// Not really diselect but activates remove mode (temp)
						if (_controls->IsFunctionKeyDown("MOUSE:BOX_DELETE"))
						{
							AddBoxGhostImage();
							_isPlace = false;
						}
					}
					BoxEvent();
				}
			}
		}
	}
	else
	{

		if (_isSelectionMode && _movingGhostImage._g == nullptr)
		{
			DragEvent(System::TRAP);
			DragEvent(System::GUARD);
			DragEvent(System::CAMERA);
		}
	}



	if (_movingGhostImage._g != nullptr)
	{
		MarkerMoveEvent();

		if ((!_isDragAndDropMode && _controls->IsFunctionKeyUp("MOUSE:DESELECT")) || _controls->IsFunctionKeyDown("MOUSE:DESELECT"))
		{
			if (_isDragAndDropMode)
			{
				_deletedObjectBlueprint = _objectHandler->GetBlueprintByType(_movingGhostImage._g->GetType(), _movingGhostImage._g->GetSubType());
			}
			RemoveGhostImage();
			_isPlace = false;
			_isDragAndDropMode = false;
		}
		else // necessary because left and right mouse button might be pressed at once
		{
			if (_controls->IsFunctionKeyDown("MOUSE:SELECT") && !_isPlace && !_isDragAndDropMode)
			{
				_isPlace = true;
			}

			if ((_isDragAndDropMode && _controls->IsFunctionKeyUp("MOUSE:SELECT"))
				|| (!_isDragAndDropMode && _controls->IsFunctionKeyDown("MOUSE:DRAG") && _isPlace && _movingGhostImage._placeable && _isObjectButtonReleased))
			{
				DropEvent();
			}
			else if ( !_isObjectButtonReleased && _controls->IsFunctionKeyUp("MOUSE:SELECT"))
			{
				_isObjectButtonReleased = true;
			}
		}
	}
}

void BaseEdit::HandleKeyInput()
{
	if (_movingGhostImage._g != nullptr)
	{
		// Rotation
		bool rotated = false;
		bool clockwise = false;
		if (_controls->IsFunctionKeyDown("MAP_EDIT:ROTATE_MARKER_CLOCK"))
		{
			XMFLOAT3 tempRot = _movingGhostImage._g->GetRotation();
			_movingGhostImage._g->SetRotation(XMFLOAT3(tempRot.x, tempRot.y + (DirectX::XM_PI / 4), tempRot.z));
			rotated = true;
			clockwise = true;

		}
		if (_controls->IsFunctionKeyDown("MAP_EDIT:ROTATE_MARKER_COUNTERCLOCK"))
		{
			XMFLOAT3 tempRot = _movingGhostImage._g->GetRotation();
			_movingGhostImage._g->SetRotation(XMFLOAT3(tempRot.x, tempRot.y - (DirectX::XM_PI / 4), tempRot.z));
			rotated = true;
			clockwise = false;
		}
		if (rotated)
		{
			_movingGhostImage._g->SetDirection(AI::GetNextDirection(static_cast<Unit*>(_movingGhostImage._g)->GetDirection(), clockwise));

			if (_movingGhostImage._g->GetType() != System::CAMERA && _movingGhostImage._g->GetType() != System::GUARD)			//Traps need to be right angles
			{
				XMFLOAT3 tempRot = _movingGhostImage._g->GetRotation();
				_movingGhostImage._g->SetDirection(AI::GetNextDirection(static_cast<Unit*>(_movingGhostImage._g)->GetDirection(), clockwise));
				if (clockwise)
				{
					_movingGhostImage._g->SetRotation(XMFLOAT3(tempRot.x, tempRot.y + (DirectX::XM_PI / 4), tempRot.z));
				}
				else
				{
					_movingGhostImage._g->SetRotation(XMFLOAT3(tempRot.x, tempRot.y - (DirectX::XM_PI / 4), tempRot.z));
				}
			}
		}
	}
}


// Other functions

bool BaseEdit::CheckValidity(AI::Vec2D tile, System::Type type)
{
	GameObject* objectOnTile = _tileMap->GetObjectOnTile(tile._x, tile._y, type);

	bool valid = true;

	if (objectOnTile == nullptr && _tileMap->IsPlaceable(tile, type))
	{
		if (type == System::WALL && !_tileMap->IsTileEmpty(tile))
		{
			valid = false;
		}
		else
		{
			if (_tileMap->IsFloorOnTile(tile))
			{
				if (type == System::GUARD || type == System::ENEMY)
				{
					if (_tileMap->UnitsOnTile(tile) || _tileMap->IsTrapOnTile(tile) || _tileMap->IsTypeOnTile(tile, System::CAMERA))
					{
						valid = false;
					}
				}
				else if (type == System::TRAP)
				{
					if (_tileMap->UnitsOnTile(tile) || _tileMap->IsTypeOnTile(tile, System::CAMERA))
					{
						valid = false;
					}
				}
				else if (type == System::CAMERA || type == System::SPAWN)
				{
					if (!_tileMap->IsWallOnTile(tile - _movingGhostImage._g->GetDirection()))
					{
						valid = false;
					}

					if (_tileMap->UnitsOnTile(tile) || _tileMap->IsTrapOnTile(tile))
					{
						valid = false;
					}
				}
			}
			else
			{
				if (type != System::FLOOR && type != System::WALL)
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

void BaseEdit::HandleBlueprint(System::SpecificBlueprint* sB)
{
	_sB = sB;
	if (_isSelectionMode)
	{
		RemoveGhostImage();
		AddGhostImage();
		_createdObject = _movingGhostImage._g;
		_isPlace = false;
		//_modeLock = true;
	}
	else
	{
		_isSelectionMode = true;
		_isDragAndPlaceMode = false;

		_isDragAndDropMode = false;
		_isPlace = false;
	}
}

bool BaseEdit::IsObjectDropValid() const
{
	return _droppedObject;
}

GameObject * BaseEdit::GetCreatedObject()
{
	return _createdObject;
}

System::Blueprint * BaseEdit::GetDeletedObjectBlueprint()
{
	return _deletedObjectBlueprint;
}

void BaseEdit::Update()
{
	_droppedObject = false;
	_createdObject = false;
	_deletedObjectBlueprint = nullptr;
	HandleMouseInput();
	HandleKeyInput();
}