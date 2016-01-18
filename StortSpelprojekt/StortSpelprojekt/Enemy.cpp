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
	if ( tempPriority > 0 && tile != _tilePosition && (_pathLength <= 0 || tempPriority * GetApproxDistance(tile) < _goalPriority * GetApproxDistance(GetGoal()) ) )
	{
		_goalPriority = tempPriority;
		SetGoal(tile);
	} 
}

void Enemy::EvaluateTile(GameObject* obj)
{
	int tempPriority = 0;
	switch (obj->GetType())
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
	if (tempPriority > 0 && obj->GetTilePosition() != _tilePosition && (_pathLength <= 0 || tempPriority * GetApproxDistance(obj->GetTilePosition()) < _goalPriority * GetApproxDistance(GetGoal())))
	{
		_goalPriority = tempPriority;
		SetGoal(obj);
	}
}

void Enemy::act(GameObject* obj)
{
	switch (obj->GetType())
	{
	case LOOT:
		if (_heldObject == nullptr)
		{
			obj->SetPickUpState(PICKINGUP);
			_heldObject = obj;
		}
	case GUARD:
	case TRAP:
	case TRIGGER:										//Guards don't react to these (yet)
		break;
	case ENEMY:											//The guard hits the enemy
		break;
	default:
		break;
	}
}

void Enemy::Release()
{}
