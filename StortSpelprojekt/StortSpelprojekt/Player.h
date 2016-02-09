#pragma once
#include "Unit.h"
#include "Guard.h"
#include "PickingDevice.h"
#include "ObjectHandler.h"

class Player
{
private:
	ObjectHandler* _objectHandler;
	vector<short> _selectedUnits;

public:
	Player();
	Player(ObjectHandler* objectHandler);
	~Player();

	//Unit Control
	void SelectUnit(Unit* pickedUnit);
	void DeselectUnits();
	bool AreUnitsSelected();
	vector<Unit*> GetSelectedUnits();
	void MoveUnits(AI::Vec2D movePoint);
	void PatrolUnits(AI::Vec2D patrolPoint);

};

