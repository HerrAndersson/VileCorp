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
		break;
	case ENEMY:
		break;
	default:
		break;
	}
	if (tempPriority > 0 && tile != _tilePosition && (_pathLength <= 0 || tempPriority * GetApproxDistance(tile) < _goalPriority * GetApproxDistance(GetGoal())))
	{
		_goalPriority = tempPriority;
		SetGoal(tile);
	}
}

void Enemy::EvaluateTile(GameObject* obj)
{
	int tempPriority = 0;
	if (obj!= nullptr)
	{
		switch (obj->GetType())
		{
		case LOOT:
			if (_heldObject == nullptr)
			{
				tempPriority = 2;
			}
		case SPAWN:
			if (_heldObject != nullptr)
			{
				tempPriority = 2;
			}
			break;
		case TRAP:
			break;
		case GUARD:
			break;
		case ENEMY:
			break;
		default:
			break;
		}
		if (obj->GetPickUpState() == ONTILE && tempPriority > 0 && obj->GetTilePosition() != _tilePosition && (_pathLength <= 0 || tempPriority * GetApproxDistance(obj->GetTilePosition()) < _goalPriority * GetApproxDistance(GetGoal())))
		{
			_goalPriority = tempPriority;
			SetGoal(obj);
		}
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
		break;
	case SPAWN:
		if (_heldObject != nullptr)
		{
			TakeDamage(10);						//TODO: Right now despawn is done by killing the unit. This should be changed to reflect that it's escaping --Victor
		}
		break;
	case TRAP:
		break;
	case GUARD:
		break;
	case ENEMY:
		break;
	default:
		break;
	}
}

void Enemy::Release()
{}
