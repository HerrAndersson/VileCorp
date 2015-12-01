#include "Unit.h"



Unit::Unit()
{
	_healthLethal = 100;
	_healthNonLethal = 100;
}

Unit::Unit(unsigned short ID, XMFLOAT3 position, int renderObjectID) 
	: GameObject(ID, position, renderObjectID)
{
	_type = UNIT;
}


Unit::~Unit()
{
}

void Unit::ChangeHealthLethal(int change)
{
	_healthLethal += change;
}

void Unit::ChangeHealthNonLethal(int change)
{
	_healthNonLethal += change;
}

void Unit::Update()
{
}

void Unit::Release()
{
}
