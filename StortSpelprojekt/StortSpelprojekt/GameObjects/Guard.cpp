#include "Guard.h"

Guard::Guard(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject * renderObject, System::SoundModule* soundModule, const Tilemap * tileMap, int guardType, AI::Vec2D direction)
	: Unit(ID, position, rotation, tilePosition, type, renderObject, soundModule, tileMap, direction)
{
	_subType = guardType;
	_isSelected = false;
	_currentPatrolGoal = 0;
	_health = 100;
	_moveSpeed = 0.03;
	_baseDamage = 30;

	switch (_subType)				//TODO: Repair time, vision radius, etc --Victor
	{
	case BASICGUARD:
		_health = 100;
		_baseDamage = 30;
		_trapInteractionTime = 2;
		break;
	case ENGINEER:
		_health = 70;
		_baseDamage = 20;
		_trapInteractionTime = 1;
		break;
	case MARKSMAN:
		_health = 100;
		_baseDamage = 50;
		_trapInteractionTime = 2;
		break;
	default:
		break;
	}
}

Guard::~Guard()
{}

void Guard::EvaluateTile(System::Type objective, AI::Vec2D tile)
{
	EvaluateTile(_tileMap->GetObjectOnTile(tile, objective));
}

void Guard::EvaluateTile(GameObject * obj)
{
	if (obj != nullptr)
	{
		int tempPriority = 0;
		switch (obj->GetType())
		{
		case System::LOOT:
			obj->SetVisibility(true);
			break;
		case System::GUARD:
		case System::TRAP:
		case System::CAMERA:				//Guards don't react to these
			break;
		case System::ENEMY:
			_soundModule->SetSoundPosition("guard_react", _position.x, 0.0f, _position.z);
			_soundModule->Play("guard_react");
			static_cast<Enemy*>(obj)->ResetVisibilityTimer();
			tempPriority = 10;
			break;
		default:
			break;
		}
		tempPriority;
		if (tempPriority > 0 && obj->GetTilePosition() != _tilePosition && 
			(_pathLength <= 0 || tempPriority * GetApproxDistance(obj->GetTilePosition()) < _goalPriority * GetApproxDistance(GetGoalTilePosition())))			//TODO Either optimize properly or check path properly --Victor
		{
			SetGoalTilePosition(obj->GetTilePosition());
			_goalPriority = tempPriority;
			_objective = obj;
		}
	}
}

void Guard::SetPatrolPoint(AI::Vec2D patrolPoint)
{
	if (patrolPoint != _tilePosition)
	{
		if (_patrolRoute.empty())
		{
			_patrolRoute.push_back(_tilePosition);
			_currentPatrolGoal = 1;

		}
		if (patrolPoint != _patrolRoute[_patrolRoute.size() - 1])
		{
			_patrolRoute.push_back(patrolPoint);
		}
		ClearObjective();
		_goalTilePosition = _patrolRoute[_currentPatrolGoal % _patrolRoute.size()];
		_moveState = MoveState::FINDING_PATH;
		//SetGoal(_goalTilePosition);

	}
}

void Guard::RemovePatrol()
{
	_patrolRoute.clear();
	_currentPatrolGoal = 0;
}

std::vector<AI::Vec2D> Guard::GetPatrolRoute()
{
	return _patrolRoute;
}

void Guard::Wait()
{
	if (_waiting > 0)
	{
		_waiting--;
	}
	else
	{
		CheckVisibleTiles();
		if (_moveState == MoveState::IDLE)
		{
			_waiting = 30;			//Arbitrary number. Just pick something you like
		}
	}
}

void Guard::Release()
{}


void Guard::Update(float deltaTime)
{
	Unit::Update(deltaTime);
	if (_health > 0 && _status != StatusEffect::STUNNED)
	{
		switch (_moveState)
		{
		case MoveState::IDLE:
			Animate(IDLEANIM);
			Wait();
			break;
		case MoveState::FINDING_PATH:
			if (_objective != nullptr)
			{
				SetGoal(_objective);
			}
			else
			{
				SetGoal(_goalTilePosition); //Switch state at the end
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
		default:
			break;
		}
	}
}

void Guard::Act(GameObject* obj)
{
	//AI::Vec2D dist = obj->GetTilePosition() - _tilePosition;
	if (obj != nullptr && obj->InRange(_tilePosition))
	{
		switch (obj->GetType())
		{
		case  System::LOOT:
		case  System::GUARD:
			break;
		case  System::TRAP:
			if (!static_cast<Trap*>(obj)->IsTrapActive())
			{
				Animate(FIXTRAPANIM);
				if(System::FrameCountdown(_interactionTime, _animation->GetLength(FIXTRAPANIM)))
				{
					static_cast<Trap*>(obj)->SetTrapActive(true);
					//	obj->SetColorOffset({0,0,0});
					ClearObjective();
				}
			}
			break;
		case  System::ENEMY:											//The guard hits the enemy
			Animate(FIGHTANIM);
			if(System::FrameCountdown(_interactionTime, _animation->GetLength(FIGHTANIM)))
			{
				Unit* enemy = static_cast<Unit*>(obj);
				enemy->TakeDamage(1);
				enemy->Animate(HURTANIM);
				if (enemy->GetHealth() <= 0)
				{
					ClearObjective();
				}
			}
			break;
		case  System::FLOOR:
			if (!_patrolRoute.empty())
			{
				if (_tilePosition == _patrolRoute[_currentPatrolGoal % _patrolRoute.size()])
				{
					_currentPatrolGoal++;
					SetGoalTilePosition(_patrolRoute[_currentPatrolGoal % _patrolRoute.size()]);
					if (_tileMap->IsFloorOnTile(_goalTilePosition))
					{
						_objective = _tileMap->GetObjectOnTile(_goalTilePosition, System::FLOOR);
					}
				}
			}
			else
			{
				ClearObjective();
			}

			break;
		default:
			break;
		}
	}
	else
	{
		ClearObjective();
	}
	if (_objective == nullptr)
	{
		_moveState = MoveState::MOVING;
		Animate(WALKANIM);
	}
}
void Guard::SwitchingNode()
{
	Unit::SwitchingNode();
	if (_tileMap->IsEnemyOnTile(_nextTile))
	{
		_objective = _tileMap->GetObjectOnTile(_nextTile, System::ENEMY);
		_moveState = MoveState::AT_OBJECTIVE;
	}
}