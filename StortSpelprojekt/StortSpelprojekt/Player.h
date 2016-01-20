#pragma once
#include "Unit.h"
#include "PickingDevice.h"

class Player
{
private:
	vector<Unit*> _selectedUnits;


public:
	Player();
	~Player();

	//Unit Control
	void SelectUnit(Unit* pickedUnit);
	void DeselectUnits();
	bool AreUnitsSelected();
	vector<Unit*> GetSelectedUnits();
	void MoveUnits(AI::Vec2D movePoint);
	//void PatrolUnits(AI::Vec2D patrolPoint);

};

