#include "Guard.h"

#include <limits>

Guard::Guard(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject * renderObject, System::SoundModule* soundModule, Renderer::ParticleEventQueue* particleEventQueue, const Tilemap * tileMap, int guardType, AI::Vec2D direction)
	: Unit(ID, position, rotation, tilePosition, type, renderObject, soundModule, particleEventQueue, tileMap, direction)
{
	_subType = guardType;
	_isSelected = false;
	_currentPatrolGoal = 0;
	_health = 100;
	_moveSpeed = 0.03f;
	_baseDamage = 30;

	switch (_subType)				//TODO: Repair time, vision radius, etc --Victor
	{
	case BASICGUARD:
		_health = 100;
		_baseDamage = 30;
		_trapInteractionTime = 2;
		break;
	case ENGINEER:
		_health = 50;
		_baseDamage = 0;
		_trapInteractionTime = 1;
		_visionRadius = 7;
		_moveSpeed = 0.035f;
		break;
	case MARKSMAN:
		_health = 100;
		_baseDamage = 50;
		_trapInteractionTime = 2;
		_visionRadius = 5;
		_moveSpeed = 0.025f;
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

Guard::~Guard()
{

}

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
			static_cast<Enemy*>(obj)->ResetVisibilityTimer();
			if (_subType != ENGINEER)
			{
				tempPriority = 10;
			}
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
	unsigned int newID = _ID + INT_MAX;
	XMFLOAT3 pos = _position;
	pos.x *= 0.5;
	pos.y = 1.5f;
	pos.z *= 0.5;
	_particleEventQueue->Insert(new ParticleUpdateMessage(newID, true, pos));

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
				if(System::FrameCountdown(_interactionTime, (int)_animation->GetLength(FIXTRAPANIM)))
				{
					static_cast<Trap*>(obj)->SetTrapActive(true);
					ClearObjective();
				}
			}
			break;
		case  System::ENEMY:											//The guard hits the enemy
			Animate(FIGHTANIM);
			if(System::FrameCountdown(_interactionTime, (int)_animation->GetLength(FIGHTANIM)))
			{
				Unit* enemy = static_cast<Unit*>(obj);
				enemy->TakeDamage(_baseDamage);
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

void Guard::ShowSelectIcon()
{
	unsigned int newID = _ID + INT_MAX;
	XMFLOAT3 pos = _position;
	pos.y += 3.0f;
	ParticleRequestMessage* msg = new ParticleRequestMessage(ParticleType::ICON, ParticleSubType::SELECTED_SUBTYPE, newID, pos, XMFLOAT3(0, 0, 0), 0.01f, 1, 0.25f, true, false);
	_particleEventQueue->Insert(msg);
}

void Guard::ShowPatrolIcons()
{
	for (auto p : _patrolRoute)
	{
		XMFLOAT3 pos = XMFLOAT3(p._x, 0.5, p._y);

		unsigned int newID = _tileMap->GetObjectOnTile(p, System::FLOOR)->GetID() + INT_MAX;


		ParticleRequestMessage* msg = new ParticleRequestMessage(ParticleType::ICON, ParticleSubType::PATROL_SUBTYPE, newID, pos, XMFLOAT3(0, 0, 0), 0.01f, 1, 0.25f, true, true);
		_particleEventQueue->Insert(msg);
	}
}

void Guard::HideSelectIcon()
{
	unsigned int newID = _ID + INT_MAX;
	_particleEventQueue->Insert(new ParticleUpdateMessage(newID, false));
}

void Guard::HidePatrolIcons()
{
	for (auto p : _patrolRoute)
	{
		unsigned int newID = _tileMap->GetObjectOnTile(p, System::FLOOR)->GetID() + INT_MAX;
		_particleEventQueue->Insert(new ParticleUpdateMessage(newID, false));
	}
}


