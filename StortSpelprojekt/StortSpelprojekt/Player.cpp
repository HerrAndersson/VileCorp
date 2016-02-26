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


Player::Player()
{
	_objectHandler = nullptr;
}

Player::Player(ObjectHandler* objectHandler)
{
	_objectHandler = objectHandler;
}

Player::~Player()
{
	_objectHandler = nullptr;
	_selectedUnits.clear();
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
	bool exists = false;
	for (auto v : _selectedObjects)
	{
		if (v == pickedObject->GetID())
		{
			exists = true;
		}
	}
	if (!exists)
	{
		_selectedObjects.push_back(pickedObject->GetID());
	}
}

void Player::SelectObjects(vector<vector<GameObject*>> pickedObjects)
{
	bool exists = false;
	for (auto x : pickedObjects)
	{
		for (auto object : x)
		{
			exists = false;
			for (auto v : _selectedObjects)
			{
				if (v == object->GetID())
				{
					exists = true;
				}
			}
			if (!exists)
			{
				_selectedObjects.push_back(object->GetID());
			}
		}
	}
}

void Player::DeselectObjects()
{
	//Decolourize everything
	vector<GameObject*> deselectObjects = GetSelectedObjects();
	for (GameObject* i : deselectObjects)
	{
		i->SetColorOffset(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	}
	_selectedObjects.clear();
}

vector<GameObject*> Player::GetSelectedObjects()
{
	vector<GameObject*> objects;
	for (auto i : _selectedObjects)
	{
		GameObject* ob = _objectHandler->Find(i);
		if (ob != nullptr)
		{
			objects.push_back(ob);
		}
	}
	return objects;
}

bool Player::IsDragging()const
{
	return _drag;
}

void Player::SetDragging(bool drag)
{
	_drag = drag;
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