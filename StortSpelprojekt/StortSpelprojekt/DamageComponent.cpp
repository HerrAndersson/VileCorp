#include "DamageComponent.h"
#include <sstream>


DamageComponent::DamageComponent(int damage, int AOE, int range, bool lethal)
{
	_damage = damage;
	_AOE = AOE;
	_range = range;
	_lethal = lethal;
}

DamageComponent::~DamageComponent()
{}

int DamageComponent::GetAOE() const
{
	return _AOE;
}

int DamageComponent::GetRange() const
{
	return _range;
}

bool DamageComponent::GetLethal() const
{
	return _lethal;
}

void DamageComponent::Damage(Trap * trap)
{
	trap->Disable();
}

void DamageComponent::Damage(Unit * unit)
{
	if (_lethal)
	{
		unit->ChangeHealthLethal(_damage);
	}
	else 
	{
		unit->ChangeHealthNonLethal(_damage);
	}
}

std::string DamageComponent::GetInfo()
{
	std::stringstream ss;

	ss << "Damage: " << _damage << std::endl
		<< "Area of Effect: " << _AOE << std::endl
		<< "Range: " << _range << std::endl
		<< "Lethal: " << _lethal << std::endl;

	return ss.str();
}
