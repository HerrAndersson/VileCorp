#pragma once

#include "Unit.h"

class Enemy : public Unit
{
private:

public:
	Enemy();
	Enemy(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject, const Tilemap* tileMap);
	virtual ~Enemy();
	void EvaluateTile(Type objective, AI::Vec2D tile);
	void EvaluateTile(GameObject* obj);
	void act(GameObject* obj);
	void Release();
};