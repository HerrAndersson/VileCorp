#include "Player.h"



Player::Player()
{

}

Player::~Player()
{

}

void Player::SelectUnit(Unit* pickedUnit)
{
	//Will for now only select the unit closest amongst the picked units
	//DeselectUnits();
	_selectedUnits.push_back(pickedUnit);
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
	return _selectedUnits;
}

void Player::MoveUnits(AI::Vec2D movePoint)
{
	for (unsigned int i = 0; i < _selectedUnits.size(); i++)
	{
		_selectedUnits[i]->SetGoal(movePoint);
	}
}