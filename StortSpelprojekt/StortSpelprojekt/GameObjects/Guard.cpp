#include "Guard.h"

Guard::Guard()
	: Unit()
{
	_health = 100;
	_baseDamage = 30;

	_isSelected = false;
	_currentPatrolGoal = -1;
}

Guard::Guard(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject, const Tilemap * tileMap, const int guardType)
	: Unit(ID, position, rotation, tilePosition, type, renderObject, tileMap)
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
		break;
	case ENGINEER:
		_health = 100;
		_baseDamage = 30;
		break;
	case MARKSMAN:
		_health = 100;
		_baseDamage = 30;
		break;
	default:
		break;
	}
}

Guard::~Guard()
{}

void Guard::EvaluateTile(Type objective, AI::Vec2D tile)
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
		case LOOT:
			obj->SetVisibility(true);
			break;
		case GUARD:
		case TRAP:
		case CAMERA:				//Guards don't react to these
			break;
		case ENEMY:
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

	switch( _moveState ) {
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
		Animate(WALKANIM);
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

void Guard::Act(GameObject* obj)
{
	//AI::Vec2D dist = obj->GetTilePosition() - _tilePosition;
	if (obj != nullptr && obj->InRange(_tilePosition))
	{
		switch (obj->GetType())
		{
		case LOOT:
		case GUARD:
			break;
		case TRAP:
			if (!static_cast<Trap*>(obj)->IsTrapActive())
			{
				if (_interactionTime != 0)
				{
					UseCountdown(_animation->GetLength(2, 1.0f * _speedMultiplier));
					Animate(FIXTRAPANIM);
				}
				else if (_interactionTime == 0)
				{
					static_cast<Trap*>(obj)->SetTrapActive(true);
					//	obj->SetColorOffset({0,0,0});
					ClearObjective();
				}
				else
				{
					UseCountdown();
				}
			}
			break;
		case ENEMY:											//The guard hits the enemy
			if (_interactionTime != 0)
			{
				UseCountdown(_animation->GetLength(4, 4.5f * _speedMultiplier));
				Animate(FIGHTANIM);
			}
			else if (_interactionTime == 0)
			{
				static_cast<Unit*>(obj)->TakeDamage(1);
				if (static_cast<Unit*>(obj)->GetHealth() <= 0)
				{
					ClearObjective();
				}
			}
			else
			{
				UseCountdown();
			}
			break;
		case FLOOR:
			if (!_patrolRoute.empty())
			{
				if (_tilePosition == _patrolRoute[_currentPatrolGoal % _patrolRoute.size()])
				{
					_currentPatrolGoal++;
					SetGoalTilePosition(_patrolRoute[_currentPatrolGoal % _patrolRoute.size()]);
					if (_tileMap->IsFloorOnTile(_goalTilePosition))
					{
						_objective = _tileMap->GetObjectOnTile(_goalTilePosition, FLOOR);
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
	}
}
void Guard::SwitchingNode()
{
	Unit::SwitchingNode();
	if (_tileMap->IsEnemyOnTile(_nextTile))
	{
		_objective = _tileMap->GetObjectOnTile(_nextTile, ENEMY);
		_moveState = MoveState::AT_OBJECTIVE;
	}
}