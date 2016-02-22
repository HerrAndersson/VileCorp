#include "Architecture.h"

Architecture::Architecture()
{

}
Architecture::Architecture(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject)
{

}
Architecture::~Architecture(){}

void Architecture::Update(float deltaTime)
{}

void Architecture::Release()
{}

bool Architecture::InRange(AI::Vec2D pos) const
{
	if (_type == LOOT)
	{
		return GameObject::InRange(pos);
	}
	else
	{
		return pos == _tilePosition;
	}
	
}
