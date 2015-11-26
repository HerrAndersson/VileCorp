#pragma once
#include "GameObjectComp.h"
class Unit : public GameObjectComp
{
private:
	int _healthLethal;
	int _healthNonLethal;

public:
	Unit();
	~Unit();

	// Input negative value to decrease lethal health
	void ChangeHealthLethal(int change);

	// Input negative value to decrease nonlethal health
	void ChangeHealthNonLethal(int change);
};

