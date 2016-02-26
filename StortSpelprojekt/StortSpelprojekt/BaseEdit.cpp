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
	_noPlacementZoning = false;

	_sB = nullptr;
	_marker._g = nullptr;
	_baseMarker._g = nullptr;
	_tileMap = _objectHandler->GetTileMap();
}

BaseEdit::~BaseEdit()
{
	ReleaseMarkers();
}


// Marker functions

void BaseEdit::CreateMarkers()
{
	_modeLock = true;

	// Create ghost/blueprint for _baseMarker
	if (_marker._g == nullptr)
	{
		CreateMarker();
	}
	MarkerMoveEvent();
	_baseMarker = _marker;
	_baseMarker._origPos = _baseMarker._g->GetTilePosition();

	// Create ghost/blueprint for _marker
	CreateMarker();
}

void BaseEdit::CreateMarker()
{
	AI::Vec2D pickedTile = _pickingDevice->PickTile(_controls->GetMouseCoord()._pos);
	XMFLOAT3 pos = XMFLOAT3(pickedTile._x, 0, pickedTile._y);

	_objectHandler->Add(_sB->_blueprint, _sB->_textureId, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), false);
	_marker._g = _objectHandler->GetGameObjects()->at(_sB->_blueprint->_type).back();
	_marker._origPos = _marker._g->GetTilePosition();
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

void BaseEdit::ReleaseMarkers()
{
	// Delete ghosts/blueprints
	if (_marker._g != nullptr)
	{
		_objectHandler->Remove(_marker._g);
		_marker._g = nullptr;
	}
	if (_baseMarker._g != nullptr)
	{
		_objectHandler->Remove(_baseMarker._g);
		_baseMarker._g = nullptr;
	}
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

void BaseEdit::DragEvent(System::Type type)
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

	// Special camera non floating fix
	if (_marker._g->GetType() == System::CAMERA)
	{
		if (_marker._g->GetDirection()._x != 0 && _marker._g->GetDirection()._y != 0)
		{
			XMFLOAT3 cameraDiagonal = _marker._g->GetPosition();
			cameraDiagonal.x -= (float)_marker._g->GetDirection()._x*0.2;
			cameraDiagonal.z -= (float)_marker._g->GetDirection()._y*0.2;
			_marker._g->SetPosition(cameraDiagonal);
		}
	}

	// Bind position logically
	_tileMap->AddObjectToTile(p.x, p.z, _marker._g);

	if (_marker._g != nullptr && _isPlace)
	{
		_droppedObject = true;
		if (!_marker._g->IsVisible() || (_marker._created == false && _marker._placeable == false))
		{
			_droppedObject = false;
			_objectHandler->Remove(_marker._g);
		}
	}

	if (_isDragAndDropMode)
	{
		_marker.Reset();
		_isPlace = false;
		_isDragAndDropMode = false;
	}
	else
	{
		if (_droppedObject)
		{
			_createdObject = _marker._g;
		}
		_marker.Reset();
		CreateMarker();
	}
}

void BaseEdit::BoxEvent()
{
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
		if (_noPlacementZoning)
		{
			AI::Vec2D pickedTile = _pickingDevice->PickTile(_controls->GetMouseCoord()._pos);
			Architecture* floor = static_cast<Architecture*>(_tileMap->GetObjectOnTile(pickedTile, System::Type::FLOOR));
			if (floor != nullptr)
			{
				//TODO: Eventually remake control scheme for this. /Zache, Rikhard
				if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
				{
					floor->SetNoPlacementZone(true);
				}
				if (_controls->IsFunctionKeyDown("MOUSE:DESELECT"))
				{
					floor->SetNoPlacementZone(false);
				}
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
				if (_marker._g == nullptr)
				{
					// Drag in world
					bool found = false;
					for (int i = System::Type::NR_OF_TYPES - 1; i > -1 && !found; i--)
					{
						DragEvent((System::Type)i);
						if (_marker._g != nullptr)
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
							CreateMarkers();
							_isPlace = true;
						}

						// Not really diselect but activates remove mode (temp)
						if (_controls->IsFunctionKeyDown("MOUSE:BOX_DELETE"))
						{
							CreateMarkers();
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

		if (_isSelectionMode && _marker._g == nullptr)
		{
			DragEvent(System::TRAP);
			DragEvent(System::GUARD);
			DragEvent(System::CAMERA);
		}
	}



	if (_marker._g != nullptr)
	{
		MarkerMoveEvent();

		if ((!_isDragAndDropMode && _controls->IsFunctionKeyUp("MOUSE:DESELECT")) || _controls->IsFunctionKeyDown("MOUSE:DESELECT"))
		{
			if (_isDragAndDropMode)
			{
				_deletedObjectBlueprint = _objectHandler->GetBlueprintByType(_marker._g->GetType(), _marker._g->GetSubType());
			}
			ReleaseMarkers();
			_isPlace = false;
			_isDragAndDropMode = false;
		}
		
		if (_controls->IsFunctionKeyDown("MOUSE:SELECT") && !_isPlace && !_isDragAndDropMode)
		{
			_isPlace = true;
		}

		if ((_isDragAndDropMode && _controls->IsFunctionKeyUp("MOUSE:SELECT"))
			|| (!_isDragAndDropMode && _controls->IsFunctionKeyDown("MOUSE:DRAG") && _isPlace))
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
			_marker._g->SetDirection(AI::GetNextDirection(static_cast<Unit*>(_marker._g)->GetDirection(), clockwise));

			if (_marker._g->GetType() != System::CAMERA && _marker._g->GetType() != System::GUARD)			//Traps need to be right angles
			{
				XMFLOAT3 tempRot = _marker._g->GetRotation();
				_marker._g->SetDirection(AI::GetNextDirection(static_cast<Unit*>(_marker._g)->GetDirection(), clockwise));
				if (clockwise)
				{
					_marker._g->SetRotation(XMFLOAT3(tempRot.x, tempRot.y + (DirectX::XM_PI / 4), tempRot.z));
				}
				else
				{
					_marker._g->SetRotation(XMFLOAT3(tempRot.x, tempRot.y - (DirectX::XM_PI / 4), tempRot.z));
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
				else if (type == System::CAMERA)
				{
					if (!_tileMap->IsWallOnTile(tile - _marker._g->GetDirection()))
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

void BaseEdit::HandleBlueprint(SpecificBlueprint* sB)
{
	_noPlacementZoning = false;
	_sB = sB;
	if (_isSelectionMode)
	{
		ReleaseMarkers();
		CreateMarker();
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

bool BaseEdit::DroppedObject()
{
	return _droppedObject;
}

GameObject * BaseEdit::CreatedObject()
{
	return _createdObject;
}

System::Blueprint * BaseEdit::DeletedObjectBlueprint()
{
	return _deletedObjectBlueprint;
}

void BaseEdit::EnableNoPlacementZoning()
{
	ReleaseMarkers();
	_noPlacementZoning = true;
}

void BaseEdit::Update(float deltaTime, bool clickedOnGUI)
{
	_droppedObject = false;
	_createdObject = false;
	_deletedObjectBlueprint = nullptr;
	if (!clickedOnGUI)
	{
		HandleMouseInput();
	}
	HandleKeyInput(deltaTime);
}