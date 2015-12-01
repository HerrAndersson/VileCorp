#pragma once
#include "GameObject.h"
class Unit : public GameObject
{
private:
	int _healthLethal;
	int _healthNonLethal;

public:
	Unit();
	Unit(unsigned short ID, XMFLOAT3 position, int renderObjectID);
	~Unit();

	// Input negative value to decrease lethal health
	void ChangeHealthLethal(int change);

	// Input negative value to decrease nonlethal health
	void ChangeHealthNonLethal(int change);

	void Update();
	void Release();
};

