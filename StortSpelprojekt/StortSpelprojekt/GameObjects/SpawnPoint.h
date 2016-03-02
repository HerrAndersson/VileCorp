#pragma once

#include "GameObject.h"
#include "Enemy.h"

/*
Floor tiles serving as entry- and exit points for enemies
--Victor
*/
class SpawnPoint : public GameObject
{
private:
	int _timer;
	int _time;
	int _unitsToSpawn;
	bool _enabled;

public:
	SpawnPoint(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject* renderObject, System::SoundModule* soundModule);
	SpawnPoint(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject* renderObject, System::SoundModule* soundModule, int timer, int unitCap);
	~SpawnPoint();
	void Update(float deltaTime);
	void Release();
	bool isSpawning();
	void Disable();
	void Enable();
	int GetUnitsToSpawn()const;
	void AddUnitsToSpawn(int amount);
};