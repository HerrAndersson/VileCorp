#include "BaseEdit.h"
#include <stdexcept>
#include <DirectXMath.h>
#include <sstream>

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
	_droppedObject = false;
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
	_isObjectButtonReleased = false;
}

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

		// Move marker graphically
		_movingGhostImage._g->SetPosition(p);

		// Move marker logically but unconnected
		_movingGhostImage._g->SetTilePosition(pickedTile);

		// Check validity of placement
		_movingGhostImage._placeable = CheckValidity(pickedTile, _movingGhostImage._g);

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
			if (type == System::TRAP)
			{
				AI::Vec2D* tempTiles = static_cast<Trap*>(_movingGhostImage._g)->GetTiles();
				for (int i = 0; i < static_cast<Trap*>(_movingGhostImage._g)->GetNrOfOccupiedTiles(); i++)
				{
					_tileMap->RemoveObjectFromTile(tempTiles[i], _movingGhostImage._g);
				}
			}
			else
			{
				_tileMap->RemoveObjectFromTile(_movingGhostImage._g->GetTilePosition(), _movingGhostImage._g);
			}
			_isDragAndDropMode = true;
		}
	}
}

void BaseEdit::DropEvent()
{
	_modeLock = false;
	XMFLOAT3 p;
	if (_movingGhostImage._g)
	{
		_movingGhostImage._g->SetColorOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		p = XMFLOAT3(_movingGhostImage._g->GetPosition());
	}
	

	if (!_movingGhostImage._placeable)
	{
		//Redirect position to old pos
		p.x = _movingGhostImage._origPos._x;
		p.z = _movingGhostImage._origPos._y;
		if (_movingGhostImage._g)
		{
			_movingGhostImage._g->SetTilePosition(AI::Vec2D((int)p.x, (int)p.z));
			_movingGhostImage._g->SetPosition(p);
			_movingGhostImage._g->SetRotation(_movingGhostImage._origRot);
			_movingGhostImage._g->SetDirection(_movingGhostImage._origDir);
		}
	}


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
		//	 Bind position logically
		if (_movingGhostImage._g->GetType() == System::TRAP)
		{
			AI::Vec2D* tempTiles = static_cast<Trap*>(_movingGhostImage._g)->GetTiles();
			for (int i = 0; i < static_cast<Trap*>(_movingGhostImage._g)->GetNrOfOccupiedTiles(); i++)
			{
				_tileMap->AddObjectToTile(tempTiles[i], _movingGhostImage._g);
			}
			//Remove area of effect for traps when moved
			static_cast<Trap*>(_movingGhostImage._g)->HideAreaOfEffect();
		}
		else
		{
			_tileMap->AddObjectToTile((int)p.x, (int)p.z, _movingGhostImage._g);

			//Remove area of effect for guards and cameras when moved
			if (_movingGhostImage._g->GetType() == System::GUARD)
			{
				static_cast<Unit*>(_movingGhostImage._g)->HideAreaOfEffect();
			}
			else if (_movingGhostImage._g->GetType() == System::CAMERA)
			{
				static_cast<SecurityCamera*>(_movingGhostImage._g)->HideAreaOfEffect();
			}
		}
	}
	else
	{
		if (_droppedObject)
		{
			_createdObject = _objectHandler->Add(_sB->_blueprint, _sB->_textureId, _movingGhostImage._g->GetPosition(), _movingGhostImage._g->GetRotation(), true, _movingGhostImage._g->GetDirection());

			if (_createdObject!= nullptr && _createdObject->GetType() == System::TRAP)
			{
				AI::Vec2D* tempTiles = static_cast<Trap*>(_createdObject)->GetTiles();
				for (int i = 0; i < static_cast<Trap*>(_createdObject)->GetNrOfOccupiedTiles(); i++)
				{
					_tileMap->AddObjectToTile(tempTiles[i], _createdObject);
				}
			}
			else
			{
				_tileMap->AddObjectToTile((int)p.x, (int)p.z, _createdObject);
			}
		}

		// Disables multiplacement with mouse in placement state
		if (!_extendedMode)
		{
			_isPlace = false;
			_isDragAndDropMode = false;
		}
	}
	// Special camera non floating fix
	if (_movingGhostImage._g->GetType() == System::CAMERA)
	{
		if (_movingGhostImage._g && _movingGhostImage._g->GetDirection()._x != 0 && _movingGhostImage._g->GetDirection()._y != 0)
		{
			XMFLOAT3 cameraDiagonal = _movingGhostImage._g->GetPosition();
			cameraDiagonal.x -= (float)_movingGhostImage._g->GetDirection()._x*0.2f;
			cameraDiagonal.z -= (float)_movingGhostImage._g->GetDirection()._y*0.2f;
			_movingGhostImage._g->SetPosition(cameraDiagonal);
			if (_createdObject != nullptr)
			{
				_createdObject->SetPosition(cameraDiagonal);
			}

		}
	}
	//Resetting ghost image here to not mess with camera offset
	if (_isDragAndDropMode)
	{
		_movingGhostImage.Reset();
		_isPlace = false;
		_isDragAndDropMode = false;
	}
}

void BaseEdit::BoxEvent()
{

	if ((_controls->IsFunctionKeyUp("MOUSE:SELECT") && _isPlace) || (_controls->IsFunctionKeyUp("MOUSE:DESELECT") && !_isPlace))
	{
		//if (_sB->_blueprint->_type != System::Type::TRAP)
		//{
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

			// Check tiles
			if (_isPlace) // Place
			{
				for (int x = minX; x <= maxX; x++)
				{
					for (int y = minY; y <= maxY; y++)
					{
						if (CheckValidity(AI::Vec2D(x, y), _movingGhostImage._g))
						{
							// Add to valid place
							_objectHandler->Add(_sB->_blueprint, _sB->_textureId, XMFLOAT3((float)x, 0, (float)y), XMFLOAT3(0.0f, 0.0f, 0.0f), true);
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
		//}
		RemoveGhostImage();
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
			// "No placement zone" defining

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
			// Drag mechanics for everything
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
			}

			// Box placing mechanic
			if (!_modeLock)
			{
				bool pressed = false;
				if (_controls->IsFunctionKeyDown("MOUSE:BOX_PLACE"))
				{
					pressed = true;
					_isPlace = true;
				}
				if (_controls->IsFunctionKeyDown("MOUSE:BOX_DELETE"))
				{
					pressed = true;
					_isPlace = false;
				}

				if (pressed && _isSelectionMode && _sB != nullptr)
				{
					_isSelectionMode = false;
					_isDragAndPlaceMode = true;
					_isDragAndDropMode = false;

					AddBoxGhostImage();
				}
			}
			if (_isDragAndPlaceMode && _sB != nullptr)
			{
				BoxEvent();
			}

		}
	}
	else
	{
		// Drag mechanics for game thing
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

			// Empty the memory of game objects and blueprints
			_sB = nullptr;
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
			else if (!_isObjectButtonReleased && _controls->IsFunctionKeyUp("MOUSE:SELECT"))
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

bool BaseEdit::CheckValidity(AI::Vec2D tile, GameObject* gameObject)
{
	System::Type type = gameObject->GetType();
	bool valid = _tileMap->IsPlaceable(tile, type);
	if (type == System::TRAP)
	{
		AI::Vec2D* tempTiles = static_cast<Trap*>(gameObject)->GetTiles();
		for (int i = 0; i < static_cast<Trap*>(gameObject)->GetNrOfOccupiedTiles() && valid; i++)
		{
			if (!_tileMap->IsPlaceable(tempTiles[i], type))
			{
				valid = false;
			}
		}
	}
	else if (type == System::CAMERA || type == System::SPAWN)
	{
		if (!_tileMap->IsWallOnTile(tile - _movingGhostImage._g->GetDirection()))
		{
			valid = false;
		}
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

void BaseEdit::RefreshTileMap()
{
	_tileMap = _objectHandler->GetTileMap();
}

void BaseEdit::Update()
{
	_droppedObject = false;
	_createdObject = nullptr;
	_deletedObjectBlueprint = nullptr;
	HandleMouseInput();
	HandleKeyInput();

	// To make sure a ghost object is always shown when it�s supposed to
	if (_movingGhostImage._g == nullptr && _sB != nullptr)
	{
		AddGhostImage();
	}
}