#include "Enemy.h"

/*
Decides the best direction to run away from a foe.
*/
void Enemy::Flee()
{
	//if (_tileMap->IsFloorOnTile(_tilePosition))
	//{
	//	_tileMap->GetObjectOnTile(_tilePosition, FLOOR)->SetColorOffset({0,0,0});
	//}
	_tilePosition = _nextTile;
	//if (_tileMap->IsFloorOnTile(_tilePosition))
	//{
	//	_tileMap->GetObjectOnTile(_tilePosition, FLOOR)->SetColorOffset({0,0,4});
	//}
	if (_pursuer == nullptr)
	{
		_moveState = MoveState::IDLE;
	}
	else
	{
		float distance = _aStar->GetHeuristicDistance(_tilePosition, _pursuer->GetTilePosition());
		if (!_visible || distance > (float)_pursuer->GetVisionRadius())
		{
			_moveState = MoveState::IDLE;
			_pursuer = nullptr;
		}
		else
		{
			AI::Vec2D offset = _tilePosition - _pursuer->GetTilePosition();
			AI::Vec2D bestDir = { 0,0 };
			float bestDist = 0;
			float tempDist = 0;
			for (int i = 0; i < 8; i++)
			{
				//	AI::Vec2D tempDist = offset + AI::NEIGHBOUR_OFFSETS[i];

				if (i < 4)						//weighting to normalize diagonal and straight directions
				{
					tempDist = pow(offset._x + AI::SQRT2 * AI::NEIGHBOUR_OFFSETS[i]._x, 2) + pow(offset._y + AI::SQRT2 * AI::NEIGHBOUR_OFFSETS[i]._y, 2);
				}
				else
				{
					tempDist = (float)pow(offset._x + AI::NEIGHBOUR_OFFSETS[i]._x, 2) + (float)pow(offset._y + AI::NEIGHBOUR_OFFSETS[i]._y, 2);
				}

				if (_tileMap->IsFloorOnTile(_tilePosition + AI::NEIGHBOUR_OFFSETS[i]) && !_tileMap->IsEnemyOnTile(_tilePosition + AI::NEIGHBOUR_OFFSETS[i]) && tempDist > bestDist)
				{
					bestDist = tempDist;
					bestDir = AI::NEIGHBOUR_OFFSETS[i];
				}
			}
			_direction = bestDir;
			_nextTile = _tilePosition + _direction;
			Rotate();
			_moveState = MoveState::MOVING;
		}
	}
}

bool Enemy::SafeToAttack(AI::Vec2D direction)
{
	int weight = 0;
	if (direction == _direction)
	{
		weight = 5;
	}
	else
	{
		weight = std::abs(_direction._x + direction._x) + std::abs(_direction._y + direction._y) + 1;
	}

	bool safeToAttack = false;

	if ((rand() % 20) * weight > 17)
	{
		safeToAttack = true;
	}

	return safeToAttack;
}

bool Enemy::TryToDisarm(Trap* trap)
{
	return trap->IsTrapActive() && SpotTrap(trap) && (_disarmSkill - trap->GetDisarmDifficulty() > (rand() % 50) - 25);
}

bool Enemy::SpotTrap(Trap * trap)
{
	if (!trap->IsVisibleToEnemies())
	{
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
		int disarmRoll = rand() % 100;
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

Enemy::Enemy()
	: Unit()
{
	_subType = BASICENEMY;
	SetVisibility(false);
	_visibilityTimer = TIME_TO_HIDE;
	_detectionSkill = 50;
	_disarmSkill = 50;
	_pursuer = nullptr;
}

Enemy::Enemy(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject * renderObject, System::SoundModule* soundModule, const Tilemap * tileMap, int enemyType)
	: Unit(ID, position, rotation, tilePosition, type, renderObject, soundModule, tileMap)
{
	_subType = enemyType;
	_visibilityTimer = TIME_TO_HIDE;
	_pursuer = nullptr;
	_moveSpeed = 0.025;
	InitializePathFinding();
	switch (_subType)
	{
	case BASICENEMY:
		_health = 80;
		_baseDamage = 25;
		_detectionSkill = 40;
		_disarmSkill = 30;
		break;
	case DISABLER:
		_health = 60;
		_baseDamage = 20;
		_detectionSkill = 80;
		_disarmSkill = 70;
		break;
	case ASSASSIN:
		_health = 100;
		_baseDamage = 50;
		_detectionSkill = 20;
		_disarmSkill = 20;
		break;
	default:
		break;
	}
}

Enemy::~Enemy()
{

}

void Enemy::EvaluateTile(System::Type objective, AI::Vec2D tile)
{
	if (_tileMap->IsTypeOnTile(tile, objective))
	{
		EvaluateTile(_tileMap->GetObjectOnTile(tile, objective));
	}
}

void Enemy::EvaluateTile(GameObject* obj)
{
	int tempPriority = 0;
	if (obj != nullptr && obj->GetPickUpState() == ONTILE)
	{
		switch (obj->GetType())
		{
		case  System::LOOT:
			if (_heldObject == nullptr)
			{
				tempPriority = 2;
			}
			break;
		case  System::SPAWN:
			if (_heldObject != nullptr || _tileMap->GetNrOfLoot() <= 0)
			{
				tempPriority = 2;
			}
			break;
		case  System::TRAP:
		{
			Trap* trap = static_cast<Trap*>(obj);
			if (TryToDisarm(trap))
			{
				tempPriority = 1;
			}
			else
			{
				bool changeRoute = false;
				for (int i = 0; i < trap->GetNrOfOccupiedTiles(); i++)
				{
					AI::Vec2D pos = trap->GetTiles()[i];
					if (_aStar->GetTileCost(pos) != 15)					//Arbitrary cost. Just make sure the getter and setter use the same number
					{
						_aStar->SetTileCost(trap->GetTiles()[i], 15);
						changeRoute = true;
					}
				}
				if (changeRoute)
				{
					GameObject* temp = _objective;
					SetGoalTilePosition(_goalTilePosition);					//resets pathfinding to goal
					_objective = temp;
				}
			}
		}
		break;
		case  System::GUARD:
			if (static_cast<Unit*>(obj)->GetHealth() > 0)
			{
				if (SafeToAttack(static_cast<Unit*>(obj)->GetDirection()))
				{
					tempPriority = 100 / _baseDamage;
				}
				else if (obj != _objective && _visible)
				{
					_pursuer = static_cast<Unit*>(obj);
					ClearObjective();
					_moveState = MoveState::MOVING;
				}
			}
			break;
		case  System::ENEMY:
			break;
		default:
			break;
		}

		//Head to the objective
		if (tempPriority > 0 &&
			obj->GetTilePosition() != _tilePosition &&
			(_objective == nullptr || tempPriority * GetApproxDistance(obj->GetTilePosition()) < _goalPriority * GetApproxDistance(GetGoalTilePosition())))
		{
			SetGoalTilePosition(obj->GetTilePosition());
			_objective = obj;
			_goalPriority = tempPriority;
		}
	}
}

void Enemy::Act(GameObject* obj)
{
	if (obj != nullptr)
	{
		switch (obj->GetType())
		{
		case  System::LOOT:
			if (_heldObject == nullptr)
			{
				if (_interactionTime < 0)
				{
					obj->SetPickUpState(PICKINGUP);
					if (_animation != nullptr)
					{
						System::FrameCountdown(_interactionTime, _animation->GetLength(3, 1.0f * _speedMultiplier));
						Animate(PICKUPOBJECTANIM);
					}
				}
				else if (_interactionTime == 0)
				{
					obj->SetPickUpState(PICKEDUP);
					_heldObject = obj;
					obj->SetVisibility(_visible);
					ClearObjective();
				}
				else
				{
					System::FrameCountdown(_interactionTime);
				}

			}
			break;
		case  System::SPAWN:
			TakeDamage(_health);						//TODO: Right now despawn is done by killing the unit. This should be changed to reflect that it's escaping --Victor
														//^ This will also make the guard_death sound not play if it's an escape -- Sebastian
			break;
		case  System::TRAP:
		{
			if (static_cast<Trap*>(obj)->IsTrapActive())
			{
				if (System::FrameCountdown(_interactionTime, 60))
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
		case  System::GUARD:
			if (!System::FrameCountdown(_interactionTime, _animation->GetLength(1, 1.0f * _speedMultiplier)))
			{
				if (_animation != nullptr)
				{
					Animate(FIGHTANIM);
				}
			}
			else
			{
				static_cast<Unit*>(obj)->TakeDamage(_baseDamage);
				if (static_cast<Unit*>(obj)->GetHealth() <= 0)
				{
					ClearObjective();
				}
			}
			break;
		case  System::ENEMY:
			break;
		default:
			break;
		}
		if (_objective == nullptr)
		{
			_moveState = MoveState::MOVING;
		}
	}
}

void Enemy::Release()
{}

void Enemy::Update(float deltaTime)
{
	Unit::Update(deltaTime);
	if (_animation != nullptr)
	{
		_animation->Update(deltaTime);
	}

	if (_status != StatusEffect::STUNNED)
	{
		switch (_moveState)
		{
		case MoveState::IDLE:
			CheckAllTiles();
			Animate(IDLEANIM);
			break;
		case MoveState::FINDING_PATH:
			if (_objective != nullptr)
			{
				SetGoal(_objective);
			}
			break;
		case MoveState::MOVING:
			Moving();
			Animate(WALKANIM);
			break;
		case MoveState::SWITCHING_NODE:
			SwitchingNode();
			break;
		case MoveState::AT_OBJECTIVE:
			Act(_objective);
			break;
		case MoveState::FLEEING:
			Flee();
			break;
		default:
			break;
		}

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

void Enemy::Moving()
{
	if (_pursuer != nullptr && IsCenteredOnTile(_nextTile))
	{
		_moveState = MoveState::FLEEING;
		_isSwitchingTile = true;
		_position.x = _nextTile._x;
		_position.z = _nextTile._y;
	}
	else
	{
		Unit::Moving();
	}
}
