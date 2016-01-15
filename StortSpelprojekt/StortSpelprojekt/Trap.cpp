#include "Trap.h"

Trap::Trap()
{}

Trap::Trap(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject)
{

}

Trap::~Trap()
{}

void Trap::Activate()
{}

void Trap::Update(float deltaTime)
{
}

void Trap::Release()
{
	_renderObject = nullptr;
}
