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
	AI::Vec2D pos = {0,0};
	//_nrOfAOETiles = 0;
	while (y <= x)
	{
		if (y != x)
		{
			for (int i = -x; i <= x; i++)
			{
				pos = {y, i};
				if (IsUnblocked(pos))
				{
					arr[size++] = pos + basePosition;
				}
				if (y != 0)
				{
					pos = {-y, i};
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
				pos = {x, i};
				if (IsUnblocked(pos))
				{
					arr[size++] = pos + basePosition;
				}
				if (x != 0)			//If x = 0, -x is also 0
				{
					pos = {-x, i};
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
			AI::Vec2D offset = {_direction._y * (-j),  _direction._x * j};
			arr[size++] = basePosition + (_direction * i) + offset;
		}
	}

	return size;
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

void Trap::SetTiles()
{
	for (int i = 0; i < _nrOfAOETiles; i++)
	{
		if (_tileMap->IsFloorOnTile(_areaOfEffect[i]))
		{
			_tileMap->GetObjectOnTile(_areaOfEffect[i], FLOOR)->SetColorOffset({0.0f,0.0f,0.0f});
		}
	}
	for (int i = 0; i < _tileSize; i++)
	{
		if (_tileMap->IsFloorOnTile(_occupiedTiles[i]))
		{
			_tileMap->GetObjectOnTile(_occupiedTiles[i], FLOOR)->SetColorOffset({0,0,0});
		}
	}
	_nrOfAOETiles = 0;
	_tileSize = 0;
	switch (_subType)
	{
	case SPIKE:
		_occupiedTiles[_tileSize++] = _tilePosition;
		_areaOfEffect[_nrOfAOETiles++] = _tilePosition;
		break;
	case TESLACOIL:
		_tileSize = CalculateRectangle(3, 3, (_tilePosition - _direction - AI::Vec2D(-_direction._y, _direction._x)), _occupiedTiles);
		_nrOfAOETiles = CalculateCircle(3, _tilePosition, _areaOfEffect);
		break;
	case SHARK:
		_tileSize = CalculateRectangle(6, 3, _tilePosition - AI::Vec2D(-_direction._y, _direction._x), _occupiedTiles);
		_nrOfAOETiles = CalculateRectangle(1, 3, _tilePosition, _areaOfEffect);
	case GUN:
		_tileSize = CalculateLine(10, _tilePosition, _occupiedTiles);
		_nrOfAOETiles = CalculateLine(10, _tilePosition, _areaOfEffect);
		break;
	default:
		_areaOfEffect = nullptr;
		break;
	}

	for (int i = 0; i < _nrOfAOETiles; i++)
	{
		if (_tileMap->IsFloorOnTile(_areaOfEffect[i]))
		{
			_tileMap->GetObjectOnTile(_areaOfEffect[i], FLOOR)->SetColorOffset({3.0f,1.0f,0.0f});
		}
	}
	for (int i = 0; i < _tileSize; i++)
	{
		if (_tileMap->IsFloorOnTile(_occupiedTiles[i]))
		{
			_tileMap->GetObjectOnTile(_occupiedTiles[i], FLOOR)->SetColorOffset({2,2,0});
		}
	}
}

Trap::Trap()
	:GameObject()
{
	_areaOfEffect = nullptr;
	_occupiedTiles = nullptr;
}

Trap::Trap(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject, System::SoundModule* soundModule, 
		  const Tilemap* tileMap, int trapType, AI::Vec2D direction)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject, soundModule)
{
	_isActive = true;
	_direction = direction;
	//_trapType = SHARK;
	_tileMap = tileMap;
	_nrOfAOETiles = 0;
	_isVisibleToEnemies = false;
	_areaOfEffect = nullptr;
	_occupiedTiles = nullptr;
	_subType = trapType;

	int radius = 0;

	bool firstFrame = false;
	switch (_subType)
	{
	case SPIKE:
		Initialize(3, 1, 1, 50, 50);
		firstFrame = true;
		break;
	case TESLACOIL:	
	{
		Initialize(2, 9, 37, 50, 50);
		break;
	}
	case SHARK:			//Trigger area is currently the same as the trap's physical area. Might be awkward since the shark trap is larger than its AOE.
	{
		Initialize(100, 18, 3, 50, 50);
		break;
	}
	case GUN:
	{
		Initialize(300, 10, 10, 100, 50);
		break;
	}
	default:
		_areaOfEffect = nullptr;
		break;
	}

	SetTiles();

	if (_renderObject->_mesh->_isSkinned)
	{
		_animation = new Animation(_renderObject->_mesh->_skeleton, firstFrame);
		_animation->Freeze(false);
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
	for (int i = 0; i < _tileSize && !result; i++)
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
	Animate(ACTIVATE);
	PlayActivateSound();

	SetTrapActive(false);
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

	//_nrOfAOETiles = 0;
	int radius = 0;
	GameObject::SetTilePosition(pos);	
	SetTiles();
}

AI::Vec2D Trap::GetDirection()
{
	return _direction;
}

void Trap::SetDirection(const AI::Vec2D direction)
{
	_direction = direction;
}


void Trap::Animate(Anim anim)
{
	if (_animation != nullptr && _animation->GetisFinished())
	{
		if (_subType == SPIKE)
		{
			switch (anim)
			{
			case IDLE:
				_animation->SetActionAsCycle(0, 2.0f);
				break;
			case ACTIVATE:
				_animation->PlayAction(0, 1.0f, true, true);
				break;
			default:
				break;
			}
		}
		else if (_subType == TESLACOIL)
		{
			switch (anim)
			{
			case IDLE:
				_animation->SetActionAsCycle(0, 2.0f);
				break;
			case ACTIVATE:
				_animation->PlayAction(1, 1.0f);
				break;
			default:
				break;
			}
		}
	}
}

//Sound
void Trap::PlayActivateSound()
{
	switch (_subType)
	{
	case SPIKE:
		_soundModule->SetSoundPosition("anvil_activate", _position.x, 0.0f, _position.z);
		_soundModule->Play("anvil_activate");
		break;
		
	case TESLACOIL:
		_soundModule->SetSoundPosition("tesla_activate", _position.x, 0.0f, _position.z);
		_soundModule->Play("tesla_activate");
		break;

	case SHARK:
		break;

	case GUN:
		break;
	}
}