#pragma once

#include "Unit.h"
#include "Enemy.h"

class Guard : public Unit
{
private:
	bool _isSelected;
	std::vector<AI::Vec2D> _patrolRoute;
	unsigned int _currentPatrolGoal;
public:
	Guard();
	Guard(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject, const Tilemap* tileMap);
	virtual ~Guard();
	void EvaluateTile(Type objective, AI::Vec2D tile);
	void EvaluateTile(GameObject* obj);
	void SetPatrolPoint(AI::Vec2D patrolPoint);
	void RemovePatrol();

	void Wait();
	void Release();

	void Update(float deltaTime);
	//void Moving();
	//void SetNextTile();			//replacement for move'
	void Act(GameObject* obj);
	void SwitchingNode();
};