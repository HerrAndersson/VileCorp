#include "SpawnPoint.h"

SpawnPoint::SpawnPoint()
	: GameObject()
{
	_timer = 1;
	_time = _timer;
	_unitsToSpawn = 0;
	_enabled = false;
}

SpawnPoint::SpawnPoint(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject* renderObject, System::SoundModule* soundModule)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject, soundModule)
{
	_timer = 1;
	_time = _timer;
	_unitsToSpawn = 0;
	_enabled = false;
}

SpawnPoint::SpawnPoint(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject* renderObject, System::SoundModule* soundModule, int timer, int unitCap)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject, soundModule)
{
	_timer = timer;
	_time = timer;
	_unitsToSpawn = unitCap;
	_enabled = false;
}

SpawnPoint::~SpawnPoint()
{
}

void SpawnPoint::Update(float deltaTime)
{
	if (_enabled)
	{
		if (_time <= 0)
		{
			_time = _timer;
			_unitsToSpawn--;
		}
		if (_unitsToSpawn > 0)
		{
			_time--;
		}
	}
}

void SpawnPoint::Release()
{

}

bool SpawnPoint::isSpawning()
{
	return _time <= 0;
}

void SpawnPoint::Disable()
{
	_enabled = false;
}

void SpawnPoint::Enable()
{
	_enabled = true;
}

int SpawnPoint::GetUnitsToSpawn() const
{
	return _unitsToSpawn;
}

void SpawnPoint::AddUnitsToSpawn(int amount)
{
	_unitsToSpawn += amount;
}