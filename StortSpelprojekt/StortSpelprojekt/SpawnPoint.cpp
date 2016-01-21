#include "SpawnPoint.h"

SpawnPoint::SpawnPoint()
	: GameObject()
{
	_timer = 1;
	_time = _timer;
	_unitsToSpawn = 0;
}

SpawnPoint::SpawnPoint(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject)
{
	_timer = 1;
	_time = _timer;
	_unitsToSpawn = 0;
}

SpawnPoint::SpawnPoint(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject, int timer, int unitCap)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject)
{
	_timer = timer;
	_time = timer;
	_unitsToSpawn = unitCap;
}

SpawnPoint::~SpawnPoint()
{
}

void SpawnPoint::Update()
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

void SpawnPoint::Release()
{

}

bool SpawnPoint::isSpawning()
{
	return _time <= 0;
}
