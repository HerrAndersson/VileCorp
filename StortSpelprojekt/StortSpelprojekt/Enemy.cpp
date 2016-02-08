#include "Enemy.h"

Enemy::Enemy()
	: Unit()
{
	SetVisibility(false);
}

Enemy::Enemy(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject, const Tilemap * tileMap)
	: Unit(ID, position, rotation, tilePosition, type, renderObject, tileMap)
{
	SetVisibility(false);
	_visibilityTimer = TIME_TO_HIDE;

	_detectionSkill = 50;
	_disarmSkill = 50;
}

Enemy::~Enemy()
{

}

void Enemy::EvaluateTile(Type objective, AI::Vec2D tile)
{
	if (_tileMap->IsTypeOnTile(tile, objective));
	{
		EvaluateTile(_tileMap->GetObjectOnTile(tile, objective));
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
			break;
		case SPAWN:
			if (_heldObject != nullptr)
			{
				tempPriority = 2;
			}
			break;
		case TRAP:
			if (SpotTrap(static_cast<Trap*>(obj)) && _disarmSkill - static_cast<Trap*>(obj)->GetDisarmDifficulty() > 20)
			{
				tempPriority = 5;
			}
			break;
		case GUARD:
			if (IsVisible())
			{
				_isFleeing = true;
				_pursuer = obj;
				ClearObjective();
			}
			else if (GetApproxDistance(obj->GetTilePosition()) < 3)
			{
		//		tempPriority = 2;			//TODO Make actual calculations based on attack skill etc --Victor
			}
			break;
		case ENEMY:
			break;
		default:
			break;
		}

		//Head to the objective
		if (obj->GetPickUpState() == ONTILE && 
			tempPriority > 0 &&
			obj->GetTilePosition() != _tilePosition && 
			(_pathLength <= 0 || tempPriority * GetApproxDistance(obj->GetTilePosition()) < _goalPriority * GetApproxDistance(GetGoal())))
		{
			_goalPriority = tempPriority;
			SetGoal(obj);
		}
		//Head for the exit, all objectives are taken
		else if (_heldObject == nullptr && _pathLength <= 0)
		{
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
			obj->SetVisibility(_visible);
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

void Enemy::Update(float deltaTime)
{
	Unit::Update(deltaTime);
	_visibilityTimer--;
	if (_visibilityTimer <= 0)
	{
		_visible = false;
		_visibilityTimer = TIME_TO_HIDE;
	}
}

void Enemy::ResetVisibilityTimer()
{
	_visible = true;
	_visibilityTimer = TIME_TO_HIDE;
}

bool Enemy::SpotTrap(Trap * trap)
{
	if (!trap->IsVisibleToEnemies())
	{
		srand(time(NULL));
		int detectRoll = rand() % 100;
		if (detectRoll + _detectionSkill - trap->GetDetectionDifficulty() >= 50)
		{
			trap->SetVisibleToEnemies(true);
		}
	}

	return trap->IsVisibleToEnemies();
}
