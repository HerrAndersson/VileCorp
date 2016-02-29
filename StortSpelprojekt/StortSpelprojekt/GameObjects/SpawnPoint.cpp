#include "SpawnPoint.h"

SpawnPoint::SpawnPoint()
	: GameObject()
{
}

SpawnPoint::SpawnPoint(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject* renderObject, System::SoundModule* soundModule)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject, soundModule)
{
}

SpawnPoint::~SpawnPoint()
{
}

void SpawnPoint::Update(float deltaTime)
{
}

void SpawnPoint::Release()
{
}