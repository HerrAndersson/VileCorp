#pragma once
#include "GameObject.h"
#include "Unit.h"
class Trap : public GameObject
{
private:
	int _cost;
	bool _isActive;
public:
	Trap();
	Trap(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject,
		int cost);
	~Trap();

	void Activate(Unit* target);
	void Update(float deltaTime);
	void Release();
};

