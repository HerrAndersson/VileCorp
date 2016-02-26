#include "Architecture.h"

Architecture::Architecture()
{
	_noPlacementZone = false;
}
Architecture::Architecture(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject* renderObject, System::SoundModule* soundModule, bool noPlacementZone/* = false*/)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject, soundModule)
{
	_noPlacementZone = noPlacementZone;
}
Architecture::~Architecture(){}

void Architecture::Update(float deltaTime)
{}

void Architecture::Release()
{}

bool Architecture::InRange(AI::Vec2D pos) const
{
	if (_type == System::LOOT)
	{
		return GameObject::InRange(pos);
	}
	else
	{
		return pos == _tilePosition;
	}
	
}

bool Architecture::IsNoPlacementZone() const
{
	return _noPlacementZone;
}

void Architecture::SetNoPlacementZone(const bool & noPlacementZone)
{
	_noPlacementZone = noPlacementZone;
	if (_noPlacementZone)
	{
		_colorOffset.x = 1;
	}
	else
	{
		_colorOffset.x = 0;
	}
}
