#include "Player.h"




void Player::ColorObject(GameObject * obj)
{
	obj->SetColorOffset(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
}

void Player::DecolorObject(GameObject * obj)
{
	obj->SetColorOffset(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
}


Player::Player()
{
	_objectHandler = nullptr;
}

Player::Player(ObjectHandler* objectHandler)
{
	_objectHandler = objectHandler;
}

Player::Player(ObjectHandler * objectHandler, PickingDevice * pickingDevice)
{
	_objectHandler = objectHandler;
	_pickingDevice = pickingDevice;
}

Player::~Player()
{
	_objectHandler = nullptr;
	_selectedUnits.clear();
}

void Player::UpdateDragPositions(System::MouseCoord coord)
{
	XMFLOAT3 deltaPos, currentPos;
	bool move = false;
	if (_drag)
	{
		if (IsSelectedObjects())
		{

			//We want to compare the picked tile with the object we clicked on.
			AI::Vec2D pickedTile = _pickingDevice->PickTile(coord._pos);
			DirectX::XMFLOAT3 clickedPos = XMFLOAT3(pickedTile._x, 0, pickedTile._y);
			//Deltapos for the objects
			deltaPos.x = clickedPos.x - _dragOrigin._x;
			deltaPos.y = 0;
			deltaPos.z = clickedPos.z - _dragOrigin._y;
			//give the new values to the objects
			for (auto i : _selectedObjects)
			{
				currentPos = i->GetPosition();
				i->SetPosition(XMFLOAT3(deltaPos.x + currentPos.x, 0, deltaPos.z + currentPos.z));
			}
			_dragOrigin = pickedTile;
		}
	}
}

void Player::SelectUnit(Unit* pickedUnit)
{
	bool exists = false;
	for (auto v : _selectedUnits)
	{
		if (v == pickedUnit->GetID())
		{
			exists = true;
		}
	}
	if (!exists)
	{
		_selectedUnits.push_back(pickedUnit->GetID());
	}
}

void Player::DeselectUnits()
{
	_selectedUnits.clear();
}

bool Player::AreUnitsSelected()
{
	return !_selectedUnits.empty();
}

vector<Unit*> Player::GetSelectedUnits()
{
	vector<Unit*> units;
	for (int i = 0; i < _selectedUnits.size(); i++)
	{
		Unit* unit = (Unit*)_objectHandler->Find(_selectedUnits.at(i));
		if (unit != nullptr)
		{
			units.push_back(unit);
		}
	}

	return units;
}

void Player::MoveUnits(AI::Vec2D movePoint)
{
	for (unsigned int i = 0; i < _selectedUnits.size(); i++)
	{
		Unit* unit = (Unit*)_objectHandler->Find(_selectedUnits[i]);

		if (unit != nullptr)
		{
			//If a patrolling unit is told to move it will break its patrolroute
			if (unit->GetType() == System::GUARD)
			{
				((Guard*)unit)->RemovePatrol();
			}
			unit->SetGoalTilePosition(movePoint);
		}
	}
}

void Player::PatrolUnits(AI::Vec2D patrolPoint)
{
	for (unsigned int i = 0; i < _selectedUnits.size(); i++)
	{
		Unit* unit = (Unit*)_objectHandler->Find(_selectedUnits[i]);

		if (unit != nullptr)
		{
			if (unit->GetType() == System::GUARD)
			{
				((Guard*)unit)->SetPatrolPoint(patrolPoint);
			}
		}
	}
}

int Player::GetNumberOfSelectedUnits()
{
	return _selectedUnits.size();
}

void Player::SelectObjects(GameObject* pickedObject)
{
	ColorObject(pickedObject);
	_selectedObjects.push_back(pickedObject);
}

void Player::SelectObjects(vector<vector<GameObject*>> pickedObjects)
{
	for (vector<GameObject*> i : pickedObjects)
	{
		for (GameObject* j : i)
		{
			ColorObject(j);
			_selectedObjects.push_back(j);
		}
	}
}

void Player::DeselectObjects()
{
	//Decolourize everything
	for (GameObject* i : _selectedObjects)
	{
		DecolorObject(i);
	}
	_selectedObjects.clear();
}

void Player::DeleteSelectedObjects()
{
	for (GameObject* i : _selectedObjects)
	{
		_objectHandler->Remove(i);
	}
	_selectedObjects.clear();
}

vector<GameObject*> Player::GetSelectedObjects()
{
	return _selectedObjects;
}

unsigned int Player::GetNumSelectedObjects()
{
	return _selectedObjects.size();
}

bool Player::IsDragging()const
{
	return _drag;
}

void Player::ActivateDragging(GameObject* object)
{
	_drag = true;
	_dragOrigin = object->GetTilePosition();
}

void Player::DeactivateDragging()
{
	_drag = false;
	_dragOrigin = AI::Vec2D(0,0);
	_selectedObjectsPreviousPos.clear();
}

bool Player::IsSelectedObjects()
{
	if (_selectedObjects.size() > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}