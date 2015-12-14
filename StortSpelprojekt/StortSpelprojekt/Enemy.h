#pragma once

#include "Unit.h"

class Enemy : public Unit
{
private:

public:
	Enemy();
	Enemy(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject, const Tilemap* tileMap);
	~Enemy();
	void EvaluateTile(Type objective, AI::Vec2D tile);
	void act(Type obj);
};