#include "Trap.h"

/*
	Get the tiles that will be activated for traps with a circular aoe
	The tiles are saved in arr (should be either _occupiedTiles or _areaOfEffect)
	Returns the array size
*/
int Trap::CalculateCircle(int radius, AI::Vec2D basePosition, AI::Vec2D* arr)
{
	int x = radius;
	int y = 0;
	int D = 1 - x;   // Decision criterion divided by 2 evaluated at x=r, y=0
	int size = 0;
	AI::Vec2D pos = { 0,0 };
	//_nrOfAOETiles = 0;
	while (y <= x)
	{
		if (y != x)
		{
			for (int i = -x; i <= x; i++)
			{
				pos = { y, i };
				if (IsUnblocked(pos))
				{
					arr[size++] = pos + basePosition;
				}
				if (y != 0)
				{
					pos = { -y, i };
					if (IsUnblocked(pos))
					{
						arr[size++] = pos + basePosition;
					}
				}
			}
		}
		y++;
		if (D < 0)
		{
			D += 2 * y + 1;   // Change in decision criterion for y -> y+1
		}
		else
		{
			for (int i = -y + 1; i < y; i++)
			{
				pos = { x, i };
				if (IsUnblocked(pos))
				{
					arr[size++] = pos + basePosition;
				}
				if (x != 0)			//If x = 0, -x is also 0
				{
					pos = { -x, i };
					if (IsUnblocked(pos))
					{
						arr[size++] = pos + basePosition;
					}
				}

			}
			x--;
			D += 2 * (y - x) + 1;   // Change for y -> y+1, x -> x-1
		}
	}
	return size;
}


/*
Get the tiles that will be activated for traps with a linear aoe
The tiles are saved in arr (should be either _occupiedTiles or _areaOfEffect)
Returns the array size
*/
int Trap::CalculateLine(int length, AI::Vec2D basePosition, AI::Vec2D* arr)
{
	int size = 0;
	AI::Vec2D pos = basePosition;

	for (int i = 0; i < length && !_tileMap->IsWallOnTile(pos); i++)
	{
		arr[size++] = pos;
		pos += _direction;
	}
	return size;
}

/*
Get the tiles forming a rectangle. This will not check for wall and should be forced to be placed in an open area.
Base position is the lower right corner when parallel to _direction.
The tiles are saved in arr (should be either _occupiedTiles or _areaOfEffect).
Returns the array size.
*/
int Trap::CalculateRectangle(int length, int width, AI::Vec2D basePosition, AI::Vec2D * arr)
{
	int size = 0;
	for (int i = 0; i < length; i++)
	{
		for (int j = 0; j < width; j++)
		{
			AI::Vec2D offset = { _direction._y * (-j),  _direction._x * j };
			arr[size++] = basePosition + (_direction * i) + offset;
		}
	}

	return size;
}

//Check that there's an unblocked Bresenham line to the centre of the trap
bool Trap::IsUnblocked(AI::Vec2D pos)
{
	int octant = 0;

	if (pos._y < 0 || (pos._y == 0 && pos._x < 0))
	{
		octant += 4;
	}
	if (abs(pos._y) > abs(pos._x))		//Checks if vertical distance is further than horizontal distance
	{
		octant++;
		if (pos._y * pos._x < 0)			//checks if y and x are signed differently
		{
			octant++;
		}
	}
	else if (pos._y * pos._x < 0)				//checks if y and x are signed differently
	{
		octant += 3;
	}

	pos = ConvertOctant(octant, pos);
	int size = max(pos._x, pos._y);
	AI::Vec2D* line = new AI::Vec2D[size + 1];
	//std::unique_ptr<AI::Vec2D[]> line(new AI::Vec2D[max(pos._x, pos._y)]);
	line[0] = { 0,0 };
	int lineLength = 1;
	int D = 2 * pos._y - pos._x;
	int y = 0;
	if (D > 0)
	{
		y++;
		D -= 2 * pos._x;
	}
	for (int x = 0; x < pos._x; x++)
	{
		AI::Vec2D outPos = ConvertOctant(octant, { x + 1, y }, false);
		line[lineLength++] = outPos;
		D += 2 * pos._y;
		if (D > 0)
		{
			y++;
			D -= 2 * pos._x;
		}
	}
	bool result = true;
	for (int i = 0; i < lineLength && result; i++)
	{
		if (_tileMap->IsWallOnTile(_tilePosition + line[i]))
		{
			result = false;
		}
	}
	delete[]line;
	return result;
}

AI::Vec2D Trap::ConvertOctant(int octant, AI::Vec2D pos, bool in)
{
	AI::Vec2D convertedPos;
	switch (octant)
	{
	case 0:
		convertedPos = pos;
		break;
	case 1:
		convertedPos = { pos._y, pos._x };
		break;
	case 2:
		if (in)
		{
			convertedPos = { pos._y, -pos._x };
		}
		else
		{
			convertedPos = { -pos._y, pos._x };
		}
		break;
	case 3:
		convertedPos = { -pos._x, pos._y };
		break;
	case 4:
		convertedPos = { -pos._x, -pos._y };
		break;
	case 5:
		convertedPos = { -pos._y, -pos._x };
		break;
	case 6:
		if (in)
		{
			convertedPos = { -pos._y, pos._x };
		}
		else
		{
			convertedPos = { pos._y, -pos._x };
		}
		break;
	case 7:
		convertedPos = { pos._x, -pos._y };
		break;
	default:
		convertedPos = pos;
		break;
	}
	return convertedPos;
}


/*
	Sets any subtype specific stats besides those related to area, which are set in SetTiles().  

	TODO: Add individual reset time.
	--Victor
*/
void Trap::Initialize(int damage, int tileSize, int triggerSize, int AOESize, int detectDifficulty, int disarmDifficulty, int repairTime, int disarmTime,
					  Unit::StatusEffect statusEffect, int statusTimer, int statusInterval, int triggerTimer, int ammunition)
{
	delete[] _areaOfEffect;
	delete[] _occupiedTiles;
	_damage = damage;
	_occupiedTiles = new AI::Vec2D[tileSize];
	_triggerTiles = new AI::Vec2D[triggerSize];
	_areaOfEffectArrayCapacity = AOESize;
	_areaOfEffect = new AI::Vec2D[_areaOfEffectArrayCapacity];
	_detectDifficulty = detectDifficulty;
	_disarmDifficulty = disarmDifficulty;
	_repairTime = repairTime;
	_disarmTime = disarmTime;
	_statusEffect = statusEffect;
	_statusTimer = statusTimer;
	_statusInterval = statusInterval;
	_maxTimeToTrigger = triggerTimer;
	_maxAmmunition = ammunition;
	_currentAmmunition = ammunition;
}

/*
	Sets the various areas associated with the trap. Used in the constructor as well as when setting a new position.
	TODO: Move functionality to construct if placement gets reworked to no longer necessitate a position change.
	--Victor
*/
void Trap::SetTiles()
{
	//Coloring
	for (int i = 0; i < _nrOfAOETiles; i++)
	{
		if (_tileMap->IsFloorOnTile(_areaOfEffect[i]))
		{
			_tileMap->GetObjectOnTile(_areaOfEffect[i], System::FLOOR)->SetColorOffset({0.0f,0.0f,0.0f});
		}
	}
	for (int i = 0; i < _nrOfOccupiedTiles; i++)
	{
		if (_tileMap->IsFloorOnTile(_occupiedTiles[i]))
		{
			_tileMap->GetObjectOnTile(_occupiedTiles[i], System::FLOOR)->SetColorOffset({0,0,0});
		}
	}

	_nrOfOccupiedTiles = 0;
	_nrOfTriggers = 0;
	_nrOfAOETiles = 0;
	switch (_subType)
	{
	case ANVIL:
		_occupiedTiles[_nrOfOccupiedTiles++] = _tilePosition;
		_triggerTiles[_nrOfTriggers++] = _tilePosition;
		_areaOfEffect[_nrOfAOETiles++] = _tilePosition;
		break;
	case TESLACOIL:
		_nrOfOccupiedTiles = CalculateRectangle(3, 3, (_tilePosition - _direction - AI::Vec2D(-_direction._y, _direction._x)), _occupiedTiles);
		_nrOfTriggers = CalculateRectangle(3, 3, (_tilePosition - _direction - AI::Vec2D(-_direction._y, _direction._x)), _triggerTiles);
		_nrOfAOETiles = CalculateCircle(3, _tilePosition, _areaOfEffect);
		break;
	case SHARK:
		_nrOfOccupiedTiles = CalculateRectangle(5, 2, _tilePosition - AI::Vec2D(-_direction._y, _direction._x), _occupiedTiles);
		_nrOfTriggers = CalculateRectangle(1, 2, _tilePosition - AI::Vec2D(-_direction._y, _direction._x), _triggerTiles);
		_nrOfAOETiles = CalculateRectangle(1, 2, _tilePosition - AI::Vec2D(-_direction._y, _direction._x), _areaOfEffect);
		break;
	case GUN:
		_nrOfOccupiedTiles = CalculateLine(10, _tilePosition, _occupiedTiles);
		_nrOfAOETiles = CalculateLine(10, _tilePosition, _areaOfEffect);
		_nrOfTriggers = CalculateLine(10, _tilePosition, _triggerTiles);
		break;
	case SAW:
		_nrOfOccupiedTiles = CalculateRectangle(3, 1, _tilePosition, _occupiedTiles);
		_areaOfEffect[_nrOfAOETiles++] = _tilePosition;
		_triggerTiles[_nrOfTriggers++] = _tilePosition;
		break;
	case CAKEBOMB:
		_occupiedTiles[_nrOfOccupiedTiles++] = _tilePosition;
		_triggerTiles[_nrOfTriggers++] = _tilePosition;
		_areaOfEffect[_nrOfAOETiles++] = _tilePosition;
		break;
	case BEAR:
		_occupiedTiles[_nrOfOccupiedTiles++] = _tilePosition;
		_triggerTiles[_nrOfTriggers++] = _tilePosition;
		_areaOfEffect[_nrOfAOETiles++] = _tilePosition;
		break;
	case FLAMETHROWER:
		_nrOfOccupiedTiles = CalculateLine(6, _tilePosition, _occupiedTiles);
		_nrOfAOETiles = CalculateLine(6, _tilePosition, _areaOfEffect);
		_nrOfTriggers = CalculateLine(6, _tilePosition, _triggerTiles);
		break;
	case WATER_GUN:
		_nrOfOccupiedTiles = CalculateLine(6, _tilePosition, _occupiedTiles);
		_nrOfAOETiles = CalculateLine(6, _tilePosition, _areaOfEffect);
		_nrOfTriggers = CalculateLine(6, _tilePosition, _triggerTiles);
		break;
	case SPIN_TRAP:
		_occupiedTiles[_nrOfOccupiedTiles++] = _tilePosition;
		_triggerTiles[_nrOfTriggers++] = _tilePosition;
		_areaOfEffect[_nrOfAOETiles++] = _tilePosition;
		break;
	default:
		_areaOfEffect = nullptr;
		break;
	}

	for (int i = 0; i < _nrOfAOETiles; i++)
	{
		if (_tileMap->IsFloorOnTile(_areaOfEffect[i]))
		{
			_tileMap->GetObjectOnTile(_areaOfEffect[i], System::FLOOR)->SetColorOffset({ 3.0f,1.0f,0.0f });
		}
	}
	for (int i = 0; i < _nrOfOccupiedTiles; i++)
	{
		if (_tileMap->IsFloorOnTile(_occupiedTiles[i]))
		{
			_tileMap->GetObjectOnTile(_occupiedTiles[i], System::FLOOR)->SetColorOffset({ 2,2,0 });
		}
	}
}

Trap::Trap(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject * renderObject, System::SoundModule* soundModule, 
	const Tilemap* tileMap, int trapType, AI::Vec2D direction)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject, soundModule)
{
	_isActive = true;
	_direction = direction;
	_tileMap = tileMap;
	_triggerTimer = -1;
	_isVisibleToEnemies = false;
	_areaOfEffect = nullptr;
	_occupiedTiles = nullptr;
	_nrOfOccupiedTiles = 0;
	_triggerTiles = nullptr;
	_nrOfTriggers = 0;
	_subType = trapType;
	_nrOfAOETiles = 0;
	int radius = 0;
	_nrOfOccupiedTiles = 0;
	_nrOfTriggers = 0;
	_nrOfAOETiles = 0;

	bool frozen = true;
	_resetAnimTime = true;
	switch (_subType)
	{
	case ANVIL:																						//Basic one tile, damage only, trap
		Initialize(60, 1, 1, 1, 70, 40, 140, 140, Unit::StatusEffect::NO_EFFECT, 0, 0, 200, 3);
		break;
	case TESLACOIL:																					//AOE that stuns for a few seconds and does a small amount of damage
		Initialize(10, 9, 9, 37, 80, 80, 140, 140, Unit::StatusEffect::STUNNED, 120, 120, 180, 3);
		frozen = false;
		break;
	case SHARK:																						//Takes up a lot of space, but is an instant kill if it hits.
		Initialize(999, 12, 2, 2, 60, 80, 140, 140, Unit::StatusEffect::NO_EFFECT, 0, 0);
		break;
	case GUN:																						//Mid tier standard damage dealer. Damage is done in a straight line. Can trigger multiple times
		Initialize(20, 10, 10, 10, 40, 90, 140, 140, Unit::StatusEffect::NO_EFFECT, 0, 0, 80, 10);
		break;
	case SAW:																						//Goes back and forth in a line. doesn't need to be reset unless an enemy disarms it.
		Initialize(40, 3, 1, 1, 30, 80, 140, 140, Unit::StatusEffect::NO_EFFECT, 0, 0, 60, -1);
		_resetAnimTime = false;
		break;
	case CAKEBOMB:																					//Instant kill on one tile. TODO: Make it seem easier to disarm than it is
		Initialize(999, 1, 1, 1, 90, 60, 280, 140, Unit::StatusEffect::NO_EFFECT, 0, 0);
		break;
	case BEAR:																						//Another one tile trap. Easy to disarm to lure enemies into wasting time on it
		Initialize(60, 1, 1, 1, 10, 20, 140, 280, Unit::StatusEffect::NO_EFFECT, 0, 0);
		break;
	case FLAMETHROWER:																				//Shorter range and less direct damage than a gun, but does damage over time.
		Initialize(10, 6, 6, 6, 60, 60, 140, 140, Unit::StatusEffect::BURNING, 300, 60, 80, 8);
		break;
	case WATER_GUN:																					//No damage, but inflicts slow for a long time. Range is the same as the flamethrower
		Initialize(0, 6, 6, 6, 60, 60, 140, 140, Unit::StatusEffect::SLOWED, 450, 450, 100, 5);
		break;
	case SPIN_TRAP:																					//No damage, but inflicts confusion, which cause the enemy to move randomly for the duration.
		Initialize(0, 1, 1, 1, 70, 90, 140, 140, Unit::StatusEffect::CONFUSED, 240, 240, 480, -1);
		break;
	default:
		_areaOfEffect = nullptr;
		break;
	}

	SetTiles();

	if (_renderObject->_mesh->_isSkinned)
	{
		_animation = new Animation(_renderObject->_mesh->_skeleton, true, frozen);
		Animate(IDLEANIM);
	}
}

Trap::~Trap()
{
	delete[] _areaOfEffect;
	_areaOfEffect = nullptr;
	delete[] _occupiedTiles;
	_occupiedTiles = nullptr;
	delete[] _triggerTiles;
	_triggerTiles = nullptr;
	delete _animation;
}
	_particleEventQueue->Insert(new ParticleUpdateMessage(GetID(), false, GetPosition()));
	_hasParticleEffect = false;

AI::Vec2D * Trap::GetTiles() const
{
	return _occupiedTiles;
}

int Trap::GetNrOfOccupiedTiles() const
{
	return _nrOfOccupiedTiles;
}

int Trap::GetDetectionDifficulty() const
{
	return _detectDifficulty;
}


int Trap::GetDisarmDifficulty() const
{
	return _disarmDifficulty;
}

bool Trap::InRange(AI::Vec2D pos) const
{
	bool result = false;
	for (int i = 0; i < _nrOfOccupiedTiles && !result; i++)
	{
		for (int j = 0; j < 8 && !result; j++)
		{
			result = (_occupiedTiles[i] + AI::NEIGHBOUR_OFFSETS[j] == pos);
		}
	}
	return result;
}

bool Trap::IsTrapActive() const
{
	return _isActive;
}

void Trap::SetTrapActive(bool active)
{
	_isActive = active;
	if (!active)
	{
		_currentAmmunition = _maxAmmunition;

		XMFLOAT3 pos = GetPosition();
		XMFLOAT3 iconPos;
		iconPos.x = _renderObject->_mesh->_iconPos[0] + pos.x;
		iconPos.y = _renderObject->_mesh->_iconPos[1] + pos.y;
		iconPos.z = _renderObject->_mesh->_iconPos[2] + pos.z;
		_particleEventQueue->Insert(new ParticleRequestMessage(ParticleType::ICON, ParticleSubType::WRENCH_SUBTYPE, GetID(), iconPos, XMFLOAT3(0, 0, 0), 0.0f, 1, 0.25f, true, false));
		_hasParticleEffect = true;

		Animate(FIXANIM);
	}
	else
	{
		_particleEventQueue->Insert(new ParticleUpdateMessage(GetID(), false, GetPosition()));
		_hasParticleEffect = false;
		Animate(DISABLEANIM);
	}
}

void Trap::RequestParticleByType(Unit* unit)
{
	XMFLOAT3 pos = GetPosition();
	XMFLOAT3 particlePos;
	particlePos.x = _renderObject->_mesh->_particleSpawnerPos[0] + pos.x;
	particlePos.y = _renderObject->_mesh->_particleSpawnerPos[1] + pos.y;
	particlePos.z = _renderObject->_mesh->_particleSpawnerPos[2] + pos.z;

	switch (GetSubType())
	{
	case SPIKE:
	{
		_particleEventQueue->Insert(new ParticleRequestMessage(ParticleType::SPLASH, ParticleSubType::BLOOD_SUBTYPE, -1, particlePos, XMFLOAT3(0, 1, 0), 300.0f, 20, 0.1f, true));
		break;
	}
	case TESLACOIL:
	{
		_particleEventQueue->Insert(new ParticleRequestMessage(ParticleType::ELECTRICITY, ParticleSubType::SPARK_SUBTYPE, -1, particlePos, XMFLOAT3(0, 0, 0), 1000.0f, 20, 0.3f, true, true, unit->GetPosition()));
		break;
	}
	case SHARK:
	{
		_particleEventQueue->Insert(new ParticleRequestMessage(ParticleType::SPLASH, ParticleSubType::WATER_SUBTYPE, -1, pos, XMFLOAT3(0, 1, 0), 400.0f, 20, 0.1f, true));
		break;
	}
	case GUN:
	{
		AI::Vec2D objectDir = GetDirection();
		XMFLOAT3 dir(objectDir._x, 0, objectDir._y);
		_particleEventQueue->Insert(new ParticleRequestMessage(ParticleType::MUZZLE_FLASH, ParticleSubType::MUZZLE_FLASH_SUBTYPE, -1, particlePos, dir, 200.0f, 100.0f, 0.1f, true));
		break;
	}
	case SAW:
	{
		_particleEventQueue->Insert(new ParticleRequestMessage(ParticleType::SPLASH, ParticleSubType::BLOOD_SUBTYPE, -1, particlePos, XMFLOAT3(0, 1, 0), 300.0f, 20, 0.1f, true));
		break;
	}
	case CAKEBOMB:
	{
		_particleEventQueue->Insert(new ParticleRequestMessage(ParticleType::FIRE, ParticleSubType::FIRE_SUBTYPE, -1, pos, XMFLOAT3(0, 1, 0), 1000.0f, 50, 0.1f, true));
		break;
	}
	case BEAR:
	{
		_particleEventQueue->Insert(new ParticleRequestMessage(ParticleType::SPLASH, ParticleSubType::BLOOD_SUBTYPE, -1, particlePos, XMFLOAT3(0, 1, 0), 300.0f, 20, 0.1f, true));
		break;
	}
	case FLAMETHROWER:
	{
		AI::Vec2D objectDir = GetDirection();
		XMFLOAT3 dir(objectDir._x, 0, objectDir._y);
		_particleEventQueue->Insert(new ParticleRequestMessage(ParticleType::FIRE, ParticleSubType::FIRE_SUBTYPE, -1, pos, dir, 1000.0f, 50, 0.1f, true));
		break;
	}
	case WATER_GUN:
	{
		AI::Vec2D objectDir = GetDirection();
		XMFLOAT3 dir(objectDir._x, 0, objectDir._y);
		_particleEventQueue->Insert(new ParticleRequestMessage(ParticleType::SPLASH, ParticleSubType::WATER_SUBTYPE, -1, pos, dir, 400.0f, 20, 0.1f, true));
		break;
	}
	case SPIN_TRAP:
	{
		break;
	}
	default:
		break;
	}
}

void Trap::Activate()
{
	for (int i = 0; i < _nrOfAOETiles; i++)
	{
		if (_tileMap->IsEnemyOnTile(_areaOfEffect[i]))
		{
			Unit* unit = static_cast<Unit*>(_tileMap->GetObjectOnTile(_areaOfEffect[i], System::ENEMY));

			if (unit)
			{
				unit->TakeDamage(_damage);
				unit->SetStatusEffect(_statusEffect, _statusInterval, _statusTimer);
				RequestParticleByType(unit);
			}
		}
		if (_tileMap->IsGuardOnTile(_areaOfEffect[i]))
		{
			Unit* unit = static_cast<Unit*>(_tileMap->GetObjectOnTile(_areaOfEffect[i], System::GUARD));
			if (unit)
			{
				unit->SetStatusEffect(_statusEffect, _statusInterval, _statusTimer);
				unit->TakeDamage(_damage);
				RequestParticleByType(unit);
			}
		}
	}
	Animate(ACTIVATEANIM);
	PlayActivateSound();
	_currentAmmunition--;
	if (_currentAmmunition == 0)
	{
		SetTrapActive(false);
	}
	else
	{
		_triggerTimer = _maxTimeToTrigger;
	}
}

void Trap::Update(float deltaTime)
{
	if (_animation != nullptr)
	{
		_animation->Update(deltaTime);
	}

	bool triggered = false;

	if (_subType == TrapType::SAW )
	{
		if (System::FrameCountdown(_triggerTimer, _maxTimeToTrigger, 0))
		{
			int i = 0;
			while (_occupiedTiles[i++] != _triggerTiles[0] && i < _nrOfOccupiedTiles)			//find which index matches to the current trigger
			{}
			_triggerTiles[0] = _occupiedTiles[i%_nrOfOccupiedTiles];
			_areaOfEffect[0] = _triggerTiles[0];
		//	_tileMap->GetObjectOnTile(_areaOfEffect[0], System::FLOOR)->SetColorOffset({0.0f,0.0f,3.0f});

		}
		if (_tileMap->IsEnemyOnTile(_triggerTiles[0]))
		{
			triggered = true;
		}
	}
	else
	{
		if (_triggerTimer <= 0)
		{
			for (int i = 0; i < _nrOfTriggers && !triggered; i++)
			{
				if (_tileMap->IsEnemyOnTile(_triggerTiles[i]))
				{
					triggered = true;
				}
			}
		}
		else
		{
			_triggerTimer--;
		}
	}

	if (triggered && _isActive)
	{
		Activate();
	}

}

void Trap::Release()
{
	//_renderObject = nullptr;
}

bool Trap::IsVisibleToEnemies() const
{
	return _isVisibleToEnemies;
}

void Trap::SetVisibleToEnemies(bool visible)
{
	_isVisibleToEnemies = visible;
}


// Overloaded function
void Trap::SetTilePosition(AI::Vec2D pos)
{
	//_nrOfAOETiles = 0;
	int radius = 0;
	GameObject::SetTilePosition(pos);
	SetTiles();
}

AI::Vec2D Trap::GetDirection()
{
	return _direction;
}

void Trap::SetDirection(const AI::Vec2D& direction)
{
	_direction = direction;
}

void Trap::Animate(Anim anim)
{
	if (_animation != nullptr && _animation->GetisFinished())
	{
		switch (anim)
		{
		case IDLEANIM:
			_animation->SetActionAsCycle(IDLEANIM);
			break;
		case ACTIVATEANIM:
			_animation->PlayAction(ACTIVATEANIM, true, true, _resetAnimTime);
			break;
		case DISABLEANIM:
			_animation->PlayAction(DISABLEANIM, true, true);
			break;
		case FIXANIM:
			_animation->PlayAction(FIXANIM);
			break;
		default:
			break;
		}
	}

}

//Sound
void Trap::PlayActivateSound()
{
	switch (_subType)
	{
	case ANVIL:
		_soundModule->SetSoundPosition("anvil_activate", _position.x, 0.0f, _position.z);
		_soundModule->Play("anvil_activate");
		break;

	case TESLACOIL:
		_soundModule->SetSoundPosition("tesla_activate", _position.x, 0.0f, _position.z);
		_soundModule->Play("tesla_activate");
		break;

	case SHARK:
		_soundModule->SetSoundPosition("shark_activate", _position.x, 0.0f, _position.z);
		_soundModule->Play("shark_activate");
		break;

	case GUN:
		_soundModule->SetSoundPosition("mg_activate", _position.x, 0.0f, _position.z);
		_soundModule->Play("mg_activate");
		break;

	case SAW:
		_soundModule->SetSoundPosition("saw_activate", _position.x, 0.0f, _position.z);
		_soundModule->Play("saw_activate");
		break;

	case CAKEBOMB:
		_soundModule->SetSoundPosition("bomb_activate", _position.x, 0.0f, _position.z);
		_soundModule->Play("bomb_activate");
		break;

	case BEAR:
		_soundModule->SetSoundPosition("bear_activate", _position.x, 0.0f, _position.z);
		_soundModule->Play("bear_activate");
		break;

	case FLAMETHROWER:
		_soundModule->SetSoundPosition("flame_activate", _position.x, 0.0f, _position.z);
		_soundModule->Play("flame_activate");
		break;

	case WATER_GUN:
		_soundModule->SetSoundPosition("water_activate", _position.x, 0.0f, _position.z);
		_soundModule->Play("water_activate");
		break;

	case SPIN_TRAP:
		_soundModule->SetSoundPosition("spin_activate", _position.x, 0.0f, _position.z);
		_soundModule->Play("spin_activate");
		break;
	}
}