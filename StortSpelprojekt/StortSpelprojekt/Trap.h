#pragma once
#include "GameObjectComp.h"
#include <string>


class Trap : public GameObjectComp
{
private:
	bool _functioning;

public:
	Trap();
	Trap(const Trap& copy); // TODO - Zache/Aron

	~Trap();

	void Fire(); // TODO - Zache/Aron

	void Enable();
	void Disable();

	//Update object gamelogic
	virtual void Update() {};

	//Release object resources
	virtual void Release() {};

	std::string GetInfo();
};

