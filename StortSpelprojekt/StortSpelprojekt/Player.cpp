#include "Player.h"


void Player::AddGuardIcon(Unit* unit)
{
	//Add Icon over the Selected Guard
	ParticleRequestMessage* msg;
	XMFLOAT3 pos = unit->GetPosition();
	pos.y += 2.5f;
	msg = new ParticleRequestMessage(ParticleType::ICON, ParticleSubType::QUESTIONMARK_SUBTYPE, unit->GetID(), pos, XMFLOAT3(0, 0, 0), 1000.0f, 1, 0.25f, true, false);
	_objectHandler->GetParticleEventQueue()->Insert(msg);
	_drag = false;
}
void Player::AddPatrolIcons(Guard* guard)
{
	//Show patrolroute
	for (auto p : ((Guard*)guard)->GetPatrolRoute())
	{
		GameObject* patrolFloor = _objectHandler->GetTileMap()->GetObjectOnTile(p, System::FLOOR);
		if (patrolFloor != nullptr)
		{
			ParticleRequestMessage* msg;
			XMFLOAT3 pos = XMFLOAT3(p._x, 0.3f, p._y);

			msg = new ParticleRequestMessage(ParticleType::ICON, ParticleSubType::EXCLAMATIONMARK_SUBTYPE,patrolFloor->GetID(), pos, XMFLOAT3(0, 0, 0), 100.0f, 1, 0.25f, true, false);
			_objectHandler->GetParticleEventQueue()->Insert(msg);

		}
	}
}


void Player::RemoveGuardIcon(short guardID)
{
	ParticleUpdateMessage* msg = new ParticleUpdateMessage(guardID, false);
	_objectHandler->GetParticleEventQueue()->Insert(msg);
}
void Player::RemovePatrolIcons(Guard* guard)
{
	//Remove Icon
	for (auto p : guard->GetPatrolRoute())
	{
		GameObject* patrolFloor = _objectHandler->GetTileMap()->GetObjectOnTile(p, System::FLOOR);
		if (patrolFloor != nullptr)
		{
			ParticleUpdateMessage* msg = new ParticleUpdateMessage(patrolFloor->GetID(), false);
			_objectHandler->GetParticleEventQueue()->Insert(msg);
		}
	}
}

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
			AI::Vec2D v2dragOrigin = _dragOrigin->GetTilePosition();
			//We want to compare the picked tile with the object we clicked on.
			AI::Vec2D pickedTile = _pickingDevice->PickTile(coord._pos);
			DirectX::XMFLOAT3 clickedPos = XMFLOAT3(pickedTile._x, 0, pickedTile._y);
			deltaPos.x = clickedPos.x - v2dragOrigin._x;
			deltaPos.y = 0;
			deltaPos.z = clickedPos.z - v2dragOrigin._y;
			for (auto i : _selectedObjects)
			{
				currentPos = i->GetPosition();
				i->SetPosition(XMFLOAT3(deltaPos.x + currentPos.x, 0, deltaPos.z + currentPos.z));
			}
			_dragOrigin->SetPosition(XMFLOAT3(pickedTile._x, 0, pickedTile._y));
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

	AddGuardIcon(pickedUnit);
	AddPatrolIcons((Guard*)pickedUnit);
}

void Player::DeselectUnits()
{
	//Remove Icons from deselected Guards
	for (auto u : GetSelectedUnits())
	{
		RemovePatrolIcons((Guard*)u);
		RemoveGuardIcon(u->GetID());
	}
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
		else
		{
			ParticleUpdateMessage* msg = new ParticleUpdateMessage(_selectedUnits.at(i), false);
			_objectHandler->GetParticleEventQueue()->Insert(msg);
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
				RemovePatrolIcons((Guard*)unit);
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
				RemovePatrolIcons((Guard*)unit);
				((Guard*)unit)->SetPatrolPoint(patrolPoint);
				AddPatrolIcons((Guard*)unit);
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
	_dragOrigin = object;
}

void Player::DeactivateDragging()
{
	_drag = false;
	_dragOrigin = nullptr;
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