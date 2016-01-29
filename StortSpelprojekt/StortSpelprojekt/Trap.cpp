#include "Trap.h"

Trap::Trap()
{}

Trap::Trap(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject, int cost)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject)
{
	_cost = cost;
}

Trap::~Trap()
{}

void Trap::Activate(Unit* target)
{
	if (_isActive && target->GetType() == ENEMY)
	{
		target->TakeDamage(3);
		_isActive = false;
	}
	else if (!_isActive && target->GetType() == GUARD)
	{
		_isActive = true;
	}

}

void Trap::Update(float deltaTime)
{}

void Trap::Release()
{
	//_renderObject = nullptr;
}
