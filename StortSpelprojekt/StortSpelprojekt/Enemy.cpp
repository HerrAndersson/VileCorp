#include "Enemy.h"

Enemy::Enemy()
	: Unit()
{}

Enemy::Enemy(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject, const Tilemap * tileMap)
	: Unit(ID, position, rotation, tilePosition, type, renderObject, tileMap)
{}

Enemy::~Enemy()
{ 

}

void Enemy::EvaluateTile(Type objective, AI::Vec2D tile)
{
	int tempPriority = 0;
	switch (objective)
	{
	case LOOT:
	case GUARD:
	case TRAP:
	case TRIGGER:				//Guards don't react to these
		break;
	case ENEMY:
		tempPriority = 10;
		break;
	default:
		break;
	}
	tempPriority;
	if (_goalPriority <= 0 || tempPriority * GetApproxDistance(tile) < _goalPriority * GetApproxDistance(GetGoal()))
	{
		SetGoal(tile);
	} 
}

void Enemy::act(Type obj)
{}

void Enemy::Release()
{}
