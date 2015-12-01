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
	Trigger(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject);
	~Trigger();

	bool ConnectTrap(Trap* trap);
	bool DisconnectTrap(short ID);
	void ActivateTraps();
	void Update();
	void Release();
};

