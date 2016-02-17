#include "Guard.h"

Guard::Guard()
	: Unit()
{
	_isSelected = false;
	_currentPatrolGoal = -1;
}

Guard::Guard(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject, const Tilemap * tileMap)
	: Unit(ID, position, rotation, tilePosition, type, renderObject, tileMap)
{
	_isSelected = false;
	_currentPatrolGoal = 0;
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
		if (tempPriority > 0 && obj->GetTilePosition() != _tilePosition && (_pathLength <= 0 || tempPriority * GetApproxDistance(obj->GetTilePosition()) < _goalPriority * GetApproxDistance(GetGoalTilePosition())))			//TODO Either optimize properly or check path properly --Victor
		{
			_goalPriority = tempPriority;
			_objective = obj;
			SetGoalTilePosition(obj->GetTilePosition());
			//SetGoal(obj);
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
		_patrolRoute.push_back(patrolPoint);
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
	switch( _moveState ) {
	case MoveState::IDLE:
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

//void Guard::Moving()
//{
//	if (IsCenteredOnTile(_nextTile))
//	{
//		_moveState = MoveState::SWITCHING_NODE;
//		_isSwitchingTile = true;
//		_position.x = _nextTile._x;
//		_position.z = _nextTile._y;
//	}
//	else
//	{
//		if (_direction._x == 0 || _direction._y == 0)		//Right angle movement
//		{
//			_position.x += MOVE_SPEED * _direction._x;
//			_position.z += MOVE_SPEED * _direction._y;
//		}
//		else												//Diagonal movement
//		{
//			_position.x += AI::SQRT2 * 0.5f * MOVE_SPEED * _direction._x;
//			_position.z += AI::SQRT2 * 0.5f *MOVE_SPEED * _direction._y;
//		}
//		CalculateMatrix();
//	}
//}

//void Guard::SetNextTile()
//{
//	//_tileMap->GetObjectOnTile(_tilePosition, FLOOR)->SetColorOffset({0,0,0});
//	_tilePosition = _nextTile;
//	//_tileMap->GetObjectOnTile(_tilePosition, FLOOR)->SetColorOffset({0,4,0});
//	if (_objective != nullptr)
//	{
//		if (_objective->InRange(_tilePosition))
//		{
//			_moveState = MoveState::AT_OBJECTIVE;
//		}
//		else if (_pathLength > 0 /*&& !_tileMap->IsGuardOnTile(_path[_pathLength - 1])*/)
//		{
//			_nextTile = _path[--_pathLength];
//			_direction = _nextTile - _tilePosition;
//			Rotate();
//			_moveState = MoveState::MOVING;
//		}
//		else			// TODO: else find unblocked path to goal --Victor
//		{
//			ClearObjective();
//			_moveState = MoveState::IDLE;
//		}
//		_isSwitchingTile = false;
//		CheckVisibleTiles();
//	}
//	else
//	{
//		_moveState = MoveState::IDLE;
//	}
//}

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
				if (_trapInteractionTime != 0)
				{
					UseTrap();
				}
				else
				{
					static_cast<Trap*>(obj)->SetTrapActive(true);
					//	obj->SetColorOffset({0,0,0});
					ClearObjective();
				}
			}
			break;
		case ENEMY:											//The guard hits the enemy
			static_cast<Unit*>(obj)->TakeDamage(1);
			if (static_cast<Unit*>(obj)->GetHealth() < 0)
			{
				ClearObjective();
			}
			break;
		case FLOOR:
			if (!_patrolRoute.empty())
			{
				if (_tilePosition == _patrolRoute[_currentPatrolGoal % _patrolRoute.size()])
				{
					_currentPatrolGoal++;
					SetGoalTilePosition(_patrolRoute[_currentPatrolGoal % _patrolRoute.size()]);
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