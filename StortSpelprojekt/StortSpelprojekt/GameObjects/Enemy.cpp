#include "Enemy.h"

/*
Decides the best direction to run away from a foe.
*/
void Enemy::Flee()
{
	_tilePosition = _nextTile;
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

				if (_tileMap->IsFloorOnTile(_tilePosition + AI::NEIGHBOUR_OFFSETS[i]) && !_tileMap->IsEnemyOnTile(_tilePosition + AI::NEIGHBOUR_OFFSETS[i]) && 
					!_tileMap->IsFurnitureOnTile(_tilePosition + AI::NEIGHBOUR_OFFSETS[i]) && tempDist > bestDist)
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
	return trap->IsTrapActive() && (_disarmSkill - trap->GetDisarmDifficulty() > (rand() % 50) - 25);
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

Enemy::Enemy(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject * renderObject, System::SoundModule* soundModule, Renderer::ParticleEventQueue* particleEventQueue, const Tilemap * tileMap, int enemyType, AI::Vec2D direction)
	: Unit(ID, position, rotation, tilePosition, type, renderObject, soundModule, particleEventQueue, tileMap, direction)
{
	_subType = enemyType;
	_visibilityTimer = TIME_TO_HIDE;
	_pursuer = nullptr;
	_checkAllTilesTimer = -1;
	_moveSpeed = 0.025f;
	InitializePathFinding();
	switch (_subType)
	{
	case BASICENEMY:
		_health = 80;
		_baseDamage = 25;
		_detectionSkill = 40;
		_disarmSkill = 30;
		_trapInteractionTime = 2;
		break;
	case DISABLER:
		_health = 60;
		_baseDamage = 20;
		_detectionSkill = 80;
		_disarmSkill = 70;
		_trapInteractionTime = 1;
		break;
	case ASSASSIN:
		_health = 100;
		_baseDamage = 50;
		_detectionSkill = 20;
		_disarmSkill = 20;
		_trapInteractionTime = 3;
		break;
	default:
		break;
	}

	//Show Unit Lifebar
	XMFLOAT3 pos = _position;
	pos.y += 2.5f;
	ParticleRequestMessage* msg = new ParticleRequestMessage(ParticleType::ICON, ParticleSubType::HEALTH_SUBTYPE, _ID, pos, XMFLOAT3(0, 0, 0), 1.0f, 1, _health*0.0025f, true, false);
	_particleEventQueue->Insert(msg);

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
	if (obj != nullptr)
	{
		if (obj->GetPickUpState() == ONTILE)
		{
			switch (obj->GetType())
			{
			case System::LOOT:
				if (_heldObject == nullptr)
				{
					tempPriority = 2;
				}
				break;
			case System::SPAWN:
				if (_heldObject != nullptr || _tileMap->GetNrOfLoot() <= 0)
				{
					tempPriority = 2;
				}
				break;
			case System::TRAP:
			{
				Trap* trap = static_cast<Trap*>(obj);
				if (SpotTrap(trap))
				{
					if (TryToDisarm(trap))
					{
						tempPriority = 1;
					}
					else
					{
						bool changeRoute = false;
						AI::Vec2D* triggers = trap->GetTriggers();
						for (int i = 0; i < trap->GetNrOfTriggerTiles(); i++)
						{
							if (_aStar->GetTileCost(triggers[i]) != 15)					//Arbitrary cost. Just make sure the getter and setter use the same number
							{
								_aStar->SetTileCost(triggers[i], 15);
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
			}
			break;
			case System::GUARD:
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
			case System::ENEMY:
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
}

void Enemy::Act(GameObject* obj)
{
	if (obj != nullptr)
	{
		switch (obj->GetType())
		{
		case System::LOOT:
			if (_heldObject == nullptr && !((GameObject*)obj)->IsTargeted())
			{
				obj->SetPickUpState(PICKEDUP);
				Animate(PICKUPOBJECTANIM);
				if (System::FrameCountdown(_interactionTime, (int)_animation->GetLength(PICKUPOBJECTANIM)))
				{
					obj->SetPickUpState(PICKEDUP);
					obj->SetTargeted(true);
					_heldObject = obj;
					obj->SetVisibility(_visible);
					ClearObjective();
				}
			}
			else
			{
				ClearObjective();
			}

			break;
		case System::SPAWN:
			TakeDamage(_health);
			break;
		case System::TRAP:
		{
			if (static_cast<Trap*>(obj)->IsTrapActive())
			{
				Animate(DISABLETRAPANIM);
				if (System::FrameCountdown(_interactionTime, (int)_animation->GetLength(DISABLETRAPANIM)))
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
		case System::GUARD:
			Animate(FIGHTANIM);
			if (System::FrameCountdown(_interactionTime, (int)_animation->GetLength(FIGHTANIM)))
			{
				if (static_cast<Unit*>(obj)->GetHealth() > 0 && InRange(obj->GetTilePosition()))
				{
					Unit* guard = static_cast<Unit*>(obj);
					guard->TakeDamage(_baseDamage);
					guard->Animate(HURTANIM);
				}
				else if (static_cast<Unit*>(obj)->GetHealth() <= 0 || !InRange(obj->GetTilePosition()))
				{
					ClearObjective();
				}
				break;
		case System::ENEMY:
			break;
		default:
			break;
			}
		}
	}
	else if (_objective == nullptr)
	{
		_moveState = MoveState::MOVING;
		Animate(WALKANIM);
	}
}

void Enemy::Release()
{}

void Enemy::Update(float deltaTime)
{
	Unit::Update(deltaTime);

	if (_status != StatusEffect::STUNNED && _health > 0)
	{
		switch (_moveState)
		{
		case MoveState::IDLE:
		{
			srand((int)time(NULL));

			if (_checkAllTilesTimer < 0)
			{
				_checkAllTilesTimer = rand() % 40;
			}
			else
			{
				_checkAllTilesTimer--;
			}
			
			if (System::FrameCountdown(_interactionTime, 20 + _checkAllTilesTimer))
			{
				ClearObjective();
				CheckAllTiles();
				_checkAllTilesTimer = -1;
			}

			Animate(IDLEANIM);
		}
			break;
		case MoveState::FINDING_PATH:
			if (_objective != nullptr)
			{
				SetGoal(_objective);
			}
			break;
		case MoveState::MOVING:
			Moving();
			if (_moveState == MoveState::MOVING)
			{
				Animate(WALKANIM);
			}
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
