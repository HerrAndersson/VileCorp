#include "Unit.h"



Unit::Unit()
{
	_healthLethal = 100;
	_healthNonLethal = 100;
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
