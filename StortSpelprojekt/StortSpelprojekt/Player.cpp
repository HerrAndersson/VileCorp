#include "Player.h"



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
			if (unit->GetType() == GUARD)
			{
				((Guard*)unit)->RemovePatrol();
			}
			unit->SetGoalTilePosition(movePoint);
 		//	unit->SetGoal(movePoint);
		//	unit->Wait(-1);
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
			if (unit->GetType() == GUARD)
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