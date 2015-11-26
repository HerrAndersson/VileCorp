#pragma once
#include "Component.h"
#include "Trap.h"
#include "Unit.h"
#include <string>

class DamageComponent : public Component
{
private:
	int _damage;
	int _AOE;
	int _range;
	bool _lethal;

public:
	DamageComponent(int damage, int AOE, int range, bool lethal);
	~DamageComponent();

	int GetAOE() const;
	int GetRange() const;
	bool GetLethal() const;

	void Damage(Trap* trap);
	void Damage(Unit* unit);

	std::string GetInfo();
};

