#include "Trap.h"

//Get the tiles that will be activated for traps with a circular aoe
void Trap::CalculateCircleAOE(int radius)
{
	int x = radius;
	int y = 0;
	int D = 1 - x;   // Decision criterion divided by 2 evaluated at x=r, y=0
	AI::Vec2D pos = {0,0};
	_nrOfAOETiles = 0;
	while (y <= x)
	{
		if (y != x)
		{
			for (int i = -x; i <= x; i++)
			{
				pos = {y, i};
				if (IsUnblocked(pos))
				{
					_areaOfEffect[_nrOfAOETiles++] = pos + _tilePosition;
				}
				if (y != 0)
				{
					pos = {-y, i};
					if (IsUnblocked(pos))
					{
						_areaOfEffect[_nrOfAOETiles++] = pos + _tilePosition;
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
				pos = {x, i};
				if (IsUnblocked(pos))
				{
					_areaOfEffect[_nrOfAOETiles++] = pos + _tilePosition;
				}
				if (x != 0)			//If x = 0, -x is also 0
				{
					pos = {-x, i};
					if (IsUnblocked(pos))
					{
						_areaOfEffect[_nrOfAOETiles++] = pos + _tilePosition;
					}
				}

			}
			x--;
			D += 2 * (y - x) + 1;   // Change for y -> y+1, x -> x-1
		}
	}

}

void Trap::CalculateLineAOE(int length, AI::Vec2D direction)
{
//	_areaOfEffectArrayCapacity = length;
	_nrOfAOETiles = 0;
//	_areaOfEffect = new AI::Vec2D[_areaOfEffectArrayCapacity];
	AI::Vec2D pos = _tilePosition;

	for (int i = 0; i < length && !_tileMap->IsWallOnTile(pos); i++)
	{
		_areaOfEffect[_nrOfAOETiles++] = pos;
		pos += direction;
	}
}

//Check that there's an unblocked Bresenham line to the centre of the trap
bool Trap::IsUnblocked( AI::Vec2D pos)
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
		octant+= 3;
	}

	pos = ConvertOctant(octant, pos);
	int size = max(pos._x, pos._y);
	AI::Vec2D* line = new AI::Vec2D[size + 1];
	//std::unique_ptr<AI::Vec2D[]> line(new AI::Vec2D[max(pos._x, pos._y)]);
	line[0] = {0,0};
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
		AI::Vec2D outPos = ConvertOctant(octant, {x + 1, y}, false);
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
		convertedPos = {pos._y, pos._x};
		break;
	case 2:
		if (in)
		{
			convertedPos = {pos._y, -pos._x};
		}
		else
		{
			convertedPos = {-pos._y, pos._x};
		}
		break;
	case 3:
		convertedPos = {-pos._x, pos._y};
		break;
	case 4:
		convertedPos = {-pos._x, -pos._y};
		break;
	case 5:
		convertedPos = {-pos._y, -pos._x};
		break;
	case 6:
		if (in)
		{
			convertedPos = {-pos._y, pos._x};
		}
		else
		{
			convertedPos = {pos._y, -pos._x};
		}
		break;
	case 7:
		convertedPos = {pos._x, -pos._y};
		break;
	default:
		convertedPos = pos;
		break;
	}
	return convertedPos;
}

void Trap::Initialize(int damage, int tileSize, int AOESize, int detectDifficulty, int disarmDifficulty)
{
	delete[] _areaOfEffect;
	delete[] _occupiedTiles;
	_damage = damage;
	_tileSize = tileSize;
	_occupiedTiles = new AI::Vec2D[_tileSize];
	_areaOfEffectArrayCapacity = AOESize;
	_areaOfEffect = new AI::Vec2D[_areaOfEffectArrayCapacity];
	_detectDifficulty = detectDifficulty;
	_disarmDifficulty = disarmDifficulty;
}

Trap::Trap()
	:GameObject()
{
	_areaOfEffect = nullptr;
	_occupiedTiles = nullptr;
}

Trap::Trap(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject, 
		  const Tilemap* tileMap, int trapType, AI::Vec2D direction, int cost)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject)
{
	_isActive = true;
	_cost = cost;
	_direction = direction;
	_trapType = (TrapType)trapType;
	//_trapType = SHARK;
	_tileMap = tileMap;
	_nrOfAOETiles = 0;
	_isVisibleToEnemies = false;
	_areaOfEffect = nullptr;
	_occupiedTiles = nullptr;
	_subType = trapType;

	if (_renderObject->_isSkinned)
	{
		_animation = new Animation(_renderObject->_skeleton);
		_animation->Freeze(false);
		_animation->SetActionAsCycle(0, true);
	}

	int radius = 0;
	
	switch (_trapType)
	{
	case SPIKE:
		Initialize(3, 1, 1, 50, 50);
		_occupiedTiles[0] = _tilePosition;
		_areaOfEffect[_nrOfAOETiles++] = _tilePosition;
		break;
	case TESLACOIL:	
		Initialize(2, 9, 37, 50, 50);
		for (int i = 0; i < _tileSize; i++)
		{
			AI::Vec2D offset = {i / 3 - 1, i % 3 - 1};
			_occupiedTiles[i] = _tilePosition + offset;
		}
		CalculateCircleAOE(3);
		break;
	case SHARK:			//Trigger area is currently the same as the trap's physical area. Might be awkward since the shark trap is larger than its AOE.
	{
		Initialize(100, 18, 3, 50, 50);
		AI::Vec2D offset = {direction._y, direction._x};
		_nrOfAOETiles = 3;
		_areaOfEffect[0] = tilePosition;
		_areaOfEffect[1] = tilePosition + offset;
		_areaOfEffect[2] = tilePosition - offset;

		AI::Vec2D dirOffset = {0, 0};
		for (int i = 0; i < 6; i++)
		{ 
			_occupiedTiles[3 * i] = _tilePosition + dirOffset - offset;
			_occupiedTiles[3 * i + 1] = _tilePosition + dirOffset;
			_occupiedTiles[3 * i + 2] = _tilePosition + dirOffset + offset;
			dirOffset += direction;
		}
		break;
	}
	default:
		_areaOfEffect = nullptr;
		break;
	}

	//Floor gets colored for debugging. Note that the tiles won't get decolored if the trap is removed
	/*for (int i = 0; i < _nrOfAOETiles; i++)
	{
		AI::Vec2D pos = _areaOfEffect[i];
		if (_tileMap->IsFloorOnTile(_areaOfEffect[i]))
		{
			_tileMap->GetObjectOnTile(_areaOfEffect[i], FLOOR)->AddColorOffset({0,0,1});
		}
	}*/
}

Trap::~Trap()
{
	delete[] _areaOfEffect;
	_areaOfEffect = nullptr;
	delete[] _occupiedTiles;
	_occupiedTiles = nullptr;
	if (_animation != nullptr)
	{
		delete _animation;
	}
}

AI::Vec2D * Trap::GetTiles() const
{
	return _occupiedTiles;
}

int Trap::GetTileSize() const
{
	return _tileSize;
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
	AI::Vec2D tempPos = _tilePosition;
	switch (_trapType)
	{
	case SPIKE:
		result = GameObject::InRange(pos);
		break;
	case TESLACOIL:
		for (int i = -2; i < 3 && !result; i++)
		{
			for (int j = -2; j < 3 && !result; j++)
			{
				if (pos == tempPos + AI::Vec2D(i, j))
				{
					result = true;
				}
			}
		}
		break;
	case SHARK:
		//TODO --Victor
		break;
	default:
		break;
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
	if (active)
	{
		SetColorOffset({0,0,0});
	}
	else
	{
		SetColorOffset({4,0,0});
	}
}

void Trap::Activate()
{
	for (int i = 0; i < _nrOfAOETiles; i++)
	{
		if (_tileMap->IsEnemyOnTile(_areaOfEffect[i]))
		{
			static_cast<Unit*>(_tileMap->GetObjectOnTile(_areaOfEffect[i], ENEMY))->TakeDamage(_damage);
		}
		if (_tileMap->IsGuardOnTile(_areaOfEffect[i]))
		{
			static_cast<Unit*>(_tileMap->GetObjectOnTile(_areaOfEffect[i], GUARD))->TakeDamage(_damage);
		}
	}

	SetTrapActive(false);
	//_animation->PlayAction(0);
}

void Trap::Update(float deltaTime)
{	
	if (_animation != nullptr && !_isActive)
	{
		_animation->Update(deltaTime);
	}
	bool triggered = false;
	for (int i = 0; i < _tileSize && !triggered; i++)
	{
		if (_tileMap->IsEnemyOnTile(_occupiedTiles[i]))
		{
			triggered = true;
		}
	}
	if (triggered && _isActive)
	{
		Activate();
	}
	if (_isActive)
	{
		SetColorOffset({0,3,0});
	}
	else
	{
		SetColorOffset({3,0,0});
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
	GameObject::SetTilePosition(pos);

	//_nrOfAOETiles = 0;
	int radius = 0;

	switch (_trapType)
	{
	case SPIKE:
	{
		_nrOfAOETiles = 0;
		_occupiedTiles[0] = _tilePosition;
		_areaOfEffect[_nrOfAOETiles++] = _tilePosition;
		break;
	}
	case TESLACOIL:
	{
		//_nrOfAOETiles = 0;
		for (int i = 0; i < _tileSize; i++)
		{
			if (_tileMap->IsFloorOnTile(_occupiedTiles[i]))
			{
				_tileMap->GetObjectOnTile(_occupiedTiles[i], FLOOR)->SetColorOffset({0,0,0});
			}
		}
		for (int i = 0; i < _nrOfAOETiles; i++)
		{
			if (_tileMap->IsFloorOnTile(_areaOfEffect[i]))
			{
				_tileMap->GetObjectOnTile(_areaOfEffect[i], FLOOR)->SetColorOffset({0,0,0});
			}
		}

		CalculateCircleAOE(3);

		for (int i = 0; i < _nrOfAOETiles; i++)
		{
			if (_tileMap->IsFloorOnTile(_areaOfEffect[i]))
			{
				_tileMap->GetObjectOnTile(_areaOfEffect[i], FLOOR)->SetColorOffset({3.0f,1.0f,0.0f});
			}
		}
		for (int i = 0; i < _tileSize; i++)
		{
			AI::Vec2D offset = {i / 3 - 1, i % 3 - 1};
			_occupiedTiles[i] = _tilePosition + offset;
			if (_tileMap->IsFloorOnTile(_occupiedTiles[i]))
			{
				_tileMap->GetObjectOnTile(_occupiedTiles[i], FLOOR)->SetColorOffset({2.0f, 2.0f, 0.0f});
			}
		}
	//	CalculateCircleAOE(3);
	}
		break;
	case SHARK:			//Trigger area is currently the same as the trap's physical area. Might be awkward since the shark trap is larger than its AOE.
	{
		_nrOfAOETiles = 0;
		AI::Vec2D offset = { _direction._y, _direction._x };
		_nrOfAOETiles = 3;
		_areaOfEffect[0] = pos;
		_areaOfEffect[1] = pos + offset;
		_areaOfEffect[2] = pos - offset;

		AI::Vec2D dirOffset = { 0, 0 };
		for (int i = 0; i < 6; i++)
		{
			_occupiedTiles[3 * i] = _tilePosition + dirOffset - offset;
			_occupiedTiles[3 * i + 1] = _tilePosition + dirOffset;
			_occupiedTiles[3 * i + 2] = _tilePosition + dirOffset + offset;
			dirOffset += _direction;
		}
		break;
	}
	default:
		_areaOfEffect = nullptr;
		break;
	}
}
