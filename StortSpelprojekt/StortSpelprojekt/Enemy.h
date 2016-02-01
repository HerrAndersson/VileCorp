#pragma once

#include "Unit.h"

const int TIME_TO_HIDE = 60;		//frames needed to stop being visible
class Enemy : public Unit
{
private:
	int _visibilityTimer;			//becomes hidden if staying out of sight long enough
public:
	Enemy();
	Enemy(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject, const Tilemap* tileMap);
	virtual ~Enemy();
	void EvaluateTile(Type objective, AI::Vec2D tile);
	void EvaluateTile(GameObject* obj);
	void act(GameObject* obj);
	void Release();
	void Update(float deltaTime);
	void ResetVisibilityTimer();
};