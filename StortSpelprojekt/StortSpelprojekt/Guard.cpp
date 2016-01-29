#include "Guard.h"

Guard::Guard()
	: Unit()
{}

Guard::Guard(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject, const Tilemap * tileMap)
	: Unit(ID, position, rotation, tilePosition, type, renderObject, tileMap)
{
	_currentPatrolGoal = 0;
}

Guard::~Guard()
{}

void Guard::EvaluateTile(Type objective, AI::Vec2D tile)
{
	int tempPriority = 0;
	switch (objective)
	{ 
	case LOOT:
	case GUARD:					
	case TRAP:				//Guards don't react to these
		break;
	case ENEMY:
		tempPriority = 10;
		break;
	default:
		break;
	}
	tempPriority;
	if (tempPriority > 0 && tile != _tilePosition && (_pathLength <= 0 || tempPriority * GetApproxDistance(tile) < _goalPriority * GetApproxDistance(GetGoal())))
	{
		_goalPriority = tempPriority;
		SetGoal(tile);
	}
}

void Guard::EvaluateTile(GameObject * obj)
{
	if (obj != nullptr)
	{
		int tempPriority = 0;
		switch (obj->GetType())
		{
		case LOOT:
		case GUARD:
		case TRAP:
		case CAMERA:				//Guards don't react to these
			break;
		case ENEMY:
			tempPriority = 10;
			break;
		default:
			break;
		}
		tempPriority;
		if (tempPriority > 0 && obj->GetTilePosition() != _tilePosition && (_pathLength <= 0 || tempPriority * GetApproxDistance(obj->GetTilePosition()) < _goalPriority * GetApproxDistance(GetGoal())))
		{
			_goalPriority = tempPriority;
			SetGoal(obj);
		}
	}
}

void Guard::act(GameObject* obj)
{
	if (obj != nullptr)
	{
		switch (obj->GetType())
		{
		case LOOT:
		case GUARD:
		case TRAP:
			break;
		case ENEMY:											//The guard hits the enemy
			static_cast<Unit*>(obj)->TakeDamage(1);
			break;
	case FLOOR:
		if (!_patrolRoute.empty())
		{
			if (_tilePosition == _patrolRoute[_currentPatrolGoal % _patrolRoute.size()])
			{
				_currentPatrolGoal++;
				SetGoal(_patrolRoute[_currentPatrolGoal % _patrolRoute.size()]);
			}
		}

		break;
		default:
			break;
		}
	}
}

void Guard::SetPatrolPoint(AI::Vec2D patrolPoint)
{
	if (_patrolRoute.empty())
	{
		_patrolRoute.push_back(_tilePosition);
		_currentPatrolGoal = 1;

	}
	_patrolRoute.push_back(patrolPoint);
	SetGoal(_patrolRoute[_currentPatrolGoal % _patrolRoute.size()]);
}

void Guard::RemovePatrol()
{
	_patrolRoute.clear();
	_currentPatrolGoal = 0;
}

void Guard::Release()
{}
