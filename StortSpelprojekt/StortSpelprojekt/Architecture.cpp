#include "Architecture.h"

Architecture::Architecture()
{

}
Architecture::Architecture(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject)
{

}
Architecture::~Architecture(){}
