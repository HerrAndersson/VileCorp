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
			unit->SetGoal(movePoint);
			unit->Wait(-1);
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