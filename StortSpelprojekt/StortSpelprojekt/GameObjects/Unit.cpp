#include "Unit.h"

void Unit::CalculatePath()
{
	if (_aStar->FindPath())
	{
		_path = _aStar->GetPath();
		_pathLength = _aStar->GetPathLength();
		//for (int i = 0; i < _pathLength; i++)
		//{
		//	/*if (_tileMap->IsFloorOnTile(_path[i]))
		//	{
		//		_tileMap->GetObjectOnTile(_path[i], FLOOR)->SetColorOffset({0,4,0});
		//	}*/
		//}
	}
	else
	{
		ClearObjective();
	}
	_moveState = MoveState::MOVING;
}

void Unit::Rotate()
{
	if (_direction._x != 0 || _direction._y != 0)
	{
		if (_direction._x == 0)
		{
			_rotation.y = DirectX::XM_PIDIV2 * (_direction._y + 1);
		}
		else if (_direction._x == -1)
		{
			_rotation.y = DirectX::XM_PIDIV2 + DirectX::XM_PIDIV4 * _direction._y;
		}
		else
		{
			_rotation.y = 3 * DirectX::XM_PIDIV2 - DirectX::XM_PIDIV4 * _direction._y;
		}
		CalculateMatrix();
	}
	//_visionCone->ColorVisibleTiles({0,0,0});
	_visionCone->FindVisibleTiles(_tilePosition, _direction);
	//_visionCone->ColorVisibleTiles({0,0,3});
}

int Unit::GetApproxDistance(AI::Vec2D target) const
{
	return (int)_aStar->GetHeuristicDistance(_tilePosition, target);
}

/*
Moves the goal and finds the path to the new goal
*/
void Unit::SetGoal(AI::Vec2D goal)
{
	if (_tileMap->IsTrapOnTile(goal))
	{
		SetGoal(_tileMap->GetObjectOnTile(goal, System::TRAP));
	}
	else if (_tileMap->IsFloorOnTile(goal))
	{
		SetGoal(_tileMap->GetObjectOnTile(goal, System::FLOOR));
	}
	else
	{
		_moveState = MoveState::MOVING;
	}
}

void Unit::SetGoal(GameObject * objective)
{

	_goalTilePosition = objective->GetTilePosition();
	_objective = objective;
	_aStar->CleanMap();
	_aStar->SetStartPosition(_nextTile);
	_aStar->SetGoalPosition(_goalTilePosition);
	CalculatePath();
}

Unit::Unit()
	: GameObject()
{
	_goalPriority = -1;
	_aStar = new AI::AStar();
	_visionCone = nullptr;
	_visionRadius = 0;
	_goalTilePosition = { 0,0 };
	_heldObject = nullptr;
	_objective = nullptr;
	_waiting = -1;
	_health = 1;
	_pathLength = 0;
	_path = nullptr;
	_direction = {0, -1};
	_nextTile = _tilePosition;
	_interactionTime = -1;
	_isSwitchingTile = false;
	_status = NO_EFFECT;
	_statusTimer = 0;
	_statusInterval = 0;
	Rotate();
}

Unit::Unit(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject* renderObject, System::SoundModule* soundModule, const Tilemap* tileMap)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject, soundModule)
{
	_goalPriority = -1;
	_visionRadius = 6;
	_goalTilePosition = _tilePosition;
	_tileMap = tileMap;
	_visionCone = new VisionCone(_visionRadius, _tileMap);
	_aStar = new AI::AStar(_tileMap->GetWidth(), _tileMap->GetHeight(), _tilePosition, { 0,0 }, AI::AStar::OCTILE);
	_heldObject = nullptr;
	_objective = nullptr;
	_waiting = -1;
	_pathLength = 0;
	_path = nullptr;
	_direction = {0, 1};
	_nextTile = _tilePosition;
	_isSwitchingTile = false;
	Rotate();
	if (_renderObject->_mesh->_isSkinned)
	{
		_animation = new Animation(_renderObject->_mesh->_skeleton, true);
		_animation->Freeze(false);
		Animate(IDLEANIM);
	}
	_moveState = MoveState::IDLE;
	_interactionTime = -1;
	_status = NO_EFFECT;
	_statusTimer = 0;
	_statusInterval = 0;
}

Unit::~Unit()
{
	delete _aStar;
	_aStar = nullptr;
	delete _visionCone;
	if (_animation != nullptr)
	{
		delete _animation;
	}
}

int Unit::GetPathLength() const
{
	return _pathLength;
}

AI::Vec2D Unit::GetGoalTilePosition()
{
	return _goalTilePosition;
}

AI::Vec2D Unit::GetDirection()
{
	return _direction;
}

AI::Vec2D Unit::GetNextTile() const
{
	return _nextTile;
}

int Unit::GetHealth()
{
	return _health;
}

GameObject * Unit::GetHeldObject() const
{
	return _heldObject;
}

Unit::MoveState Unit::GetMoveState() const
{
	return _moveState;
}

int Unit::GetVisionRadius() const
{
	return _visionRadius;
}

bool Unit::GetAnimisFinished()
{
	if(_animation != nullptr)
	{
		return _animation->GetisFinished();
	}
}

void Unit::SetGoalTilePosition(AI::Vec2D goal)
{
	ClearObjective();
	_goalTilePosition = goal;
	_moveState = MoveState::FINDING_PATH;
}

void Unit::SetDirection(const AI::Vec2D direction)
{
	_direction = direction;
	Rotate();
	//_visionCone->FindVisibleTiles(_tilePosition, _direction);
}

void Unit::SetSwitchingTile(const bool switchTile)
{
	_isSwitchingTile = switchTile;
}

void Unit::SetVisibility(bool visible)
{
	GameObject::SetVisibility(visible);
	if (_heldObject != nullptr)
	{
		_heldObject->SetVisibility(visible);
	}
}

void Unit::SetTilePosition(AI::Vec2D pos)
{
	GameObject::SetTilePosition(pos);
	//_visionCone->ColorVisibleTiles({0,0,0});
	_visionCone->FindVisibleTiles(_tilePosition, _direction);
	//_visionCone->ColorVisibleTiles({0,0,3});
	if (_moveState == MoveState::IDLE)
	{
		_nextTile = pos;
	}
}

void Unit::SetStatusEffect(StatusEffect effect, int intervalTime, int totalTime)
{
	_status = effect;
	_statusTimer = totalTime;
	_statusInterval = intervalTime;
}

bool Unit::IsSwitchingTile() const
{
	return _isSwitchingTile;
}

void Unit::CheckVisibleTiles()
{
	AI::Vec2D* visibleTiles = _visionCone->GetVisibleTiles();

	for (int i = 0; i < _visionCone->GetNrOfVisibleTiles(); i++)
	{
		if (_tileMap->IsTrapOnTile(visibleTiles[i]._x, visibleTiles[i]._y))
		{
			EvaluateTile(_tileMap->GetObjectOnTile(visibleTiles[i], System::TRAP));
		}
		if (_type != System::ENEMY && _tileMap->IsEnemyOnTile(visibleTiles[i]))
		{
			EvaluateTile(_tileMap->GetObjectOnTile(visibleTiles[i], System::ENEMY));
		}
		if (_type != System::GUARD && _tileMap->IsGuardOnTile(visibleTiles[i]))
		{
			EvaluateTile(_tileMap->GetObjectOnTile(visibleTiles[i], System::GUARD));
		}
		if (_tileMap->IsObjectiveOnTile(visibleTiles[i]))
		{
			EvaluateTile(_tileMap->GetObjectOnTile(visibleTiles[i], System::LOOT));
		}
	}
}

void Unit::CheckAllTiles()
{
	for (int i = 0; i < _tileMap->GetWidth(); i++)
	{
		for (int j = 0; j < _tileMap->GetHeight(); j++)
		{
			//Handle objectives
			if (_tileMap->IsObjectiveOnTile(AI::Vec2D(i, j)))
			{
				_aStar->SetTileCost({ i, j }, 1);
				EvaluateTile(_tileMap->GetObjectOnTile(AI::Vec2D(i, j), System::LOOT));
			}
			else if (_tileMap->IsTypeOnTile(AI::Vec2D(i, j), System::SPAWN))
			{
				EvaluateTile(_tileMap->GetObjectOnTile(AI::Vec2D(i, j), System::SPAWN));
			}
		}
	}
	//_aStar->printMap();
}

void Unit::InitializePathFinding()
{
	for (int i = 0; i < _tileMap->GetWidth(); i++)
	{
		for (int j = 0; j < _tileMap->GetHeight(); j++)
		{
			//Handle walls
			if (_tileMap->IsWallOnTile(AI::Vec2D(i, j)))
			{
				_aStar->SetTileCost({i, j}, -1);
			}
			else
			{
				_aStar->SetTileCost({i, j}, 1);
			}
		}
	}
}

void Unit::Update(float deltaTime)
{
	if (_animation != nullptr)
	{
		_animation->Update(deltaTime);
	}

	if (_statusTimer == 0)
	{
		DeactivateStatus();
	}
	else if (_status != StatusEffect::NO_EFFECT && System::FrameCountdown(_statusTimer, 0, _statusInterval * (_statusTimer / _statusInterval)))
	{
		ActivateStatus();
	}

	//bool result = false;
	//if (_statusTimer > 0)
	//{
	//	result = _statusTimer % _statusInterval == 0;
	//	_statusTimer--;
	//}
	//return result;
}

void Unit::Moving()
{
	if (IsCenteredOnTile(_nextTile))
	{
		_moveState = MoveState::SWITCHING_NODE;
		_isSwitchingTile = true;
		_position.x = _nextTile._x;
		_position.z = _nextTile._y;
	}
	else
	{
		if (_direction._x == 0 || _direction._y == 0)		//Right angle movement
		{
			_position.x += _moveSpeed * _direction._x;
			_position.z += _moveSpeed * _direction._y;
		}
		else												//Diagonal movement
		{
			_position.x += AI::SQRT2 * 0.5f * _moveSpeed * _direction._x;
			_position.z += AI::SQRT2 * 0.5f * _moveSpeed * _direction._y;
		}
		CalculateMatrix();
	}
}

void Unit::SwitchingNode()
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
	if (_status == StatusEffect::CONFUSED)
	{
		srand((int)time(NULL));
		int randDir = rand() % 8;
		_direction = AI::NEIGHBOUR_OFFSETS[randDir];
		_nextTile = _tilePosition + _direction;
		while (_tileMap->IsWallOnTile(_nextTile))			//Will loop endlessly if surrounded by walls. That really shouldn't happen though
		{
			randDir = (randDir + 1) % 8;
			_direction = AI::NEIGHBOUR_OFFSETS[randDir];
			_nextTile = _tilePosition + _direction;
		}
		Rotate();
		_moveState = MoveState::MOVING;
	}
	else
	{
		if (_objective != nullptr && _objective->GetPickUpState() == ONTILE)
		{
			if (_objective->InRange(_tilePosition))
			{
				_moveState = MoveState::AT_OBJECTIVE;
			}
			else if (_pathLength > 0 /*&& !_tileMap->IsGuardOnTile(_path[_pathLength - 1])*/)
			{
				_nextTile = _path[--_pathLength];
				_direction = _nextTile - _tilePosition;
				Rotate();
				_moveState = MoveState::MOVING;
			}
			else			// TODO: else find unblocked path to goal --Victor
			{
				ClearObjective();
				_moveState = MoveState::IDLE;
			}
			_isSwitchingTile = false;
			CheckVisibleTiles();
		}
		else
		{
			ClearObjective();
			_moveState = MoveState::IDLE;
		}
	}
}

void Unit::ClearObjective()
{
	_objective = nullptr;
	_path = nullptr;
	_pathLength = 0;
}

void Unit::Release()
{}


void Unit::ActivateStatus()
{
	switch (_status)
	{
	case StatusEffect::NO_EFFECT:
		break;
	case StatusEffect::BURNING:
		TakeDamage(10);
		break;
	case StatusEffect::SLOWED:
		_moveSpeed /= 2.0f;
		break;
	case StatusEffect::STUNNED:					//No active effect. Instead units are prevented from updating while this is in effect.
		Animate(IDLEANIM);
		break;
	case StatusEffect::SCARED:					//TODO: Either allow pursuer to be a non-unit, or make a specific movement state
		break;
	case StatusEffect::CONFUSED:				//TODO: Pick random nearby tile when switching node --Victor
		break;
	default:
		break;
	}
}

void Unit::DeactivateStatus()
{
	switch (_status)
	{
	case StatusEffect::NO_EFFECT:
	case StatusEffect::BURNING:
	case StatusEffect::STUNNED:								//Nothing in particular to reset
		break;
	case StatusEffect::SLOWED:
		_moveSpeed *= 2.0f;
		break;
	case StatusEffect::SCARED:								//Reset movement state
	case StatusEffect::CONFUSED:
		ClearObjective();
		_moveState = MoveState::MOVING;
		break;
	default:
		break;
	}
	_statusTimer = -1;
	_status = StatusEffect::NO_EFFECT;
}

void Unit::TakeDamage(int damage)
{
	if (_health - damage > 0)
	{
		_health -= damage;
	}
	else if (_health > 0)
	{
		Animate(DEATHANIM);
		_health -= damage;
	}
}

void Unit::Animate(Anim anim)
{
	if (_animation != nullptr && _animation->GetisFinished() && _lastAnimState != anim)
	{
		switch (anim)
		{
		case IDLEANIM:
			_animation->SetActionAsCycle(IDLEANIM);
			break;
		case WALKANIM:
			_animation->SetActionAsCycle(WALKANIM);
			break;
		case FIGHTANIM:
			_animation->PlayAction(FIGHTANIM);
			break;
		case PICKUPOBJECTANIM:
			_animation->PlayAction(PICKUPOBJECTANIM);
			break;
		case FIXTRAPANIM:
			_animation->PlayAction(FIXTRAPANIM);
			break;
		case DISABLETRAPANIM:
			_animation->PlayAction(DISABLETRAPANIM);
			break;
		case HURTANIM:
			_animation->PlayAction(HURTANIM);
			break;
		case DEATHANIM:
			_animation->PlayAction(DEATHANIM);
			break;
		default:
			break;
		}
		_lastAnimState = anim;
	}
}
