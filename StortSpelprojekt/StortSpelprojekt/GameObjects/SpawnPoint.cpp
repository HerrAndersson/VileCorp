#include "SpawnPoint.h"

SpawnPoint::SpawnPoint(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject* renderObject, System::SoundModule* soundModule, Renderer::ParticleEventQueue* particleEventQueue, int subType, AI::Vec2D direction)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject, soundModule, particleEventQueue, DirectX::XMFLOAT3(0,0,0), subType, direction)
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

bool SpawnPoint::InRange(AI::Vec2D pos)const
{
	return _tilePosition == pos;
}