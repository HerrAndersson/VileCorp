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
	case TRAP:
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
			static_cast<Enemy*>(obj)->ResetVisibilityTimer();
			tempPriority = 10;
			break;
		default:
			break;
		}
		tempPriority;
		if (tempPriority > 0 && obj->GetTilePosition() != _tilePosition && (_pathLength <= 0 || tempPriority * GetApproxDistance(obj->GetTilePosition()) < _goalPriority * GetApproxDistance(GetGoal())))			//TODO Either optimize properly or check path properly --Victor
		{
			_goalPriority = tempPriority;
		//	SetGoal(obj);
			_goalTilePosition = obj->GetTilePosition();
			_moveState = MoveState::FINDING_PATH;
		}
	}
}

void Guard::act(GameObject* obj)
{
	AI::Vec2D dist = obj->GetTilePosition() - _tilePosition;
	if (obj != nullptr && abs(dist._x) < 2 && abs(dist._y) < 2)
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
	else
	{
		_moveState = MoveState::IDLE;
		_nextTile = _tilePosition;
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
		SetGoal(_goalTilePosition); //Switch state at the end
		break;
	case MoveState::MOVING:
		if (_direction._x == 0 || _direction._y == 0)		//Right angle movement
		{
			_position.x += MOVE_SPEED * _direction._x;
			_position.z += MOVE_SPEED * _direction._y;
		}
		else												//Diagonal movement
		{
			_position.x += AI::SQRT2 * 0.5f * MOVE_SPEED * _direction._x;
			_position.z += AI::SQRT2 * 0.5f *MOVE_SPEED * _direction._y;
		}
		CalculateMatrix();
		if (IsCenteredOnTile())
		{
			_moveState = MoveState::SWITCHING_NODE;
		}
		break;
	case MoveState::SWITCHING_NODE:
		SetNextTile();
		break;
	case MoveState::AT_OBJECTIVE:
		act(_objective);
		break;
	default:
		break;
	}
}

void Guard::SetNextTile() {
	_tileMap->GetObjectOnTile(_tilePosition, FLOOR)->SetColorOffset({0,0,0});
	_tilePosition = _nextTile;
	_tileMap->GetObjectOnTile(_tilePosition, FLOOR)->SetColorOffset({0,4,0});

	if (_pathLength > 0)
	{
		_nextTile = _path[--_pathLength];
		_direction = _nextTile - _tilePosition;
		_moveState = MoveState::MOVING;
	}
	else
	{
		_moveState = MoveState::AT_OBJECTIVE;
	}
	CheckVisibleTiles();
}