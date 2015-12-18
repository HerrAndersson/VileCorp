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
		tempPriority = 2;
	case GUARD:
	case TRAP:
	case TRIGGER:
		break;
	case ENEMY:
		break;
	default:
		break;
	}
	if ( tempPriority > 0 && ( _goalPriority <= 0 || tempPriority * GetApproxDistance(tile) < _goalPriority * GetApproxDistance(GetGoal()) ) )
	{
		SetGoal(tile);
	} 
}

void Enemy::act(GameObject* obj)
{}

void Enemy::Release()
{}
