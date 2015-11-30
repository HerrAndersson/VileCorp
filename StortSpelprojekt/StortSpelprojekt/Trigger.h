#pragma once
#include "GameObject.h"
#include "Trap.h"
#include <vector>
class Trigger : public GameObject
{
private:
	int _size;
	std::vector<Trap*> _traps;

public:
	Trigger();
	~Trigger();

	bool ConnectTrap(Trap* trap);
	bool DisconnectTrap(short ID);
	void ActivateTraps();

};

