#pragma once

#include "Unit.h"

class Guard : public Unit
{
private:
	bool _isSelected;
public:
	Guard();
	Guard(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject, const Tilemap* tileMap);
	~Guard();
	void EvaluateTile(Type objective, AI::Vec2D tile);
	void act(Type obj);
};