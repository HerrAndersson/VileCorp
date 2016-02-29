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

public:
	SpawnPoint();
	SpawnPoint(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject* renderObject, System::SoundModule* soundModule);
	~SpawnPoint();
	void Update(float deltaTime);
	void Release();
};