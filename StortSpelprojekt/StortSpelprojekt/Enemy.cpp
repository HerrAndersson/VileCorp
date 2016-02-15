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
	if (_tileMap->IsTypeOnTile(tile, objective))
	{
		EvaluateTile(_tileMap->GetObjectOnTile(tile, objective));
	}
}

void Enemy::EvaluateTile(GameObject* obj)
{
	int tempPriority = 0;
	if (obj != nullptr)
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
		{
			Trap* trap = static_cast<Trap*>(obj);
			if (trap->IsTrapActive() && SpotTrap(trap) && (_disarmSkill - trap->GetDisarmDifficulty() > -20))
			{
				tempPriority = 1;
			}
		}
			break;	
		case GUARD:
			if (static_cast<Unit*>(obj)->GetHealth() > 0)
			{
				if (!SafeToAttack(static_cast<Unit*>(obj)->GetDirection()) && (obj != _objective || _visible))
				{
					_isFleeing = true;
					_pursuer = obj;
					ClearObjective();
					Flee();
				}
				else if (GetApproxDistance(obj->GetTilePosition()) < 3 && !_visible)
				{
					tempPriority = 2;			//TODO Make actual calculations based on attack skill etc --Victor
				}
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
		else if (_heldObject == nullptr && _pathLength <= 0 && !_isFleeing)
		{
			//Lowest possible priority, temporary solution that will be solved with the state machine, Aron and Victor
			_goalPriority = _MAX_INT_DIG;
			SetGoal(obj);
		}
	}
}

void Enemy::act(GameObject* obj)
{
	if (obj != nullptr)
	{
		switch (obj->GetType())
		{
		case LOOT:
			if (_heldObject == nullptr)
			{
				obj->SetPickUpState(PICKINGUP);
				Animate(PICKUPOBJECT);
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
		{
			if (static_cast<Trap*>(obj)->IsTrapActive())
			{
				if (_trapInteractionTime < 0)
				{
					UseTrap();
				}
				else if (_trapInteractionTime == 0)
				{
					DisarmTrap(static_cast<Trap*>(obj));
					ClearObjective();
				}
			}
			else
			{
				ClearObjective();
			}
		}
		break;
		case GUARD:
			if (static_cast<Unit*>(obj)->GetHealth() > 0)
			{
				static_cast<Unit*>(obj)->TakeDamage(10);
				Animate(FIGHT);
			}
			break;
		case ENEMY:
			break;
		default:
			break;
		}
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
		if (_heldObject != nullptr)
		{
			_heldObject->SetVisibility(false);
		}
	}
}

void Enemy::ResetVisibilityTimer()
{
	_visible = true;
	_visibilityTimer = TIME_TO_HIDE;
	if (_heldObject != nullptr)
	{
		_heldObject->SetVisibility(true);
	}
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
	//trap->SetColorOffset({ 3, 0, 3 });
	return trap->IsVisibleToEnemies();
}

void Enemy::DisarmTrap(Trap * trap)
{
	if (trap->IsTrapActive())
	{
		srand(time(NULL));
		int disarmRoll = rand()%100;
		if (disarmRoll + _disarmSkill - trap->GetDisarmDifficulty() >= 50)
		{
			trap->SetTrapActive(false);
		}
		else
		{
			trap->Activate();
		}
	}
}

bool Enemy::SafeToAttack(AI::Vec2D direction)
{
	srand(time(NULL));
	int weight = 0;
	if (direction == _direction)
	{
		weight = 5;
	}
	else
	{
		weight = std::abs(_direction._x + direction._x) + std::abs(_direction._y + direction._y) + 1;
	}
	
	bool safeToAttack = true;

	if ((rand()%20) * weight > 13)
	{
		safeToAttack = false;
	}

	return safeToAttack;
}
