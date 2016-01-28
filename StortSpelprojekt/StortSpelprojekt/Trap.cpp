#include "Trap.h"

//Get the tiles that will be activated for traps with a circular aoe
void Trap::CalculateCircleAOE(int radius)
{
	int x = radius;
	int y = 0;
	/*int x0 = _tilePosition._x;
	int y0 = _tilePosition._y;*/
	int D = 1 - x;   // Decision criterion divided by 2 evaluated at x=r, y=0
	AI::Vec2D pos = {0,0};

	while (y <= x)
	{
		if (y != x)
		{
			for (int i = -x; i <= x; i++)
			{
				pos = {y, i};
				if (isUnblocked(pos))
				{
					_areaOfEffect[_nrOfAOETiles++] = pos + _tilePosition;
				}
				if (y != 0)
				{
					pos = {-y, i};
					if (isUnblocked(pos))
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
				if (isUnblocked(pos))
				{
					_areaOfEffect[_nrOfAOETiles++] = pos + _tilePosition;
				}
				if (x != 0)			//If x = 0, -x is also 0
				{
					pos = {-x, i};
					if (isUnblocked(pos))
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
	_areaOfEffectArrayCapacity = length;
	_nrOfAOETiles = 0;
	_areaOfEffect = new AI::Vec2D[_areaOfEffectArrayCapacity];
	AI::Vec2D pos = _tilePosition;

	for (int i = 0; i < length && !_tileMap->IsWallOnTile(pos._x,pos._y); i++)
	{
		_areaOfEffect[_nrOfAOETiles++] = pos;
		pos += direction;
	}
}

//Check that there's an unblocked Bresenham line to the centre of the trap
bool Trap::isUnblocked( AI::Vec2D pos)
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

	pos = convertOctant(octant, pos);
	AI::Vec2D* line = new AI::Vec2D[max(pos._x, pos._y)];
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
		AI::Vec2D outPos = convertOctant(octant, {x + 1, y}, false);
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
		if (_tileMap->IsWallOnTile(_tilePosition._x + line[i]._x, _tilePosition._y + line[i]._y))
		{
			result = false;
		}
	}
	return result;
}

AI::Vec2D Trap::convertOctant(int octant, AI::Vec2D pos, bool in)
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

void Trap::Initialize(int damage, int tileSize, int AOESize)
{
	_damage = damage;
	_tileSize = tileSize;
	_occupiedTiles = new AI::Vec2D[_tileSize];
	_areaOfEffectArrayCapacity = AOESize;
	_areaOfEffect = new AI::Vec2D[_areaOfEffectArrayCapacity];
}

Trap::Trap()
	:GameObject()
{
	_areaOfEffect = nullptr;
	_occupiedTiles = nullptr;
}

Trap::Trap(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject, 
		  const Tilemap* tileMap, TrapType trapType, AI::Vec2D direction)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject)
{
	_trapType = SHARK;
	_tileMap = tileMap;
	_nrOfAOETiles = 0;

	int radius = 0;
	
	//TODO Make an initialize function and stop wasting space with case code --Victor
	switch (_trapType)
	{
	case SPIKE:
		Initialize(3, 1, 1);
		_occupiedTiles[0] = _tilePosition;
		_areaOfEffect[_nrOfAOETiles++] = _tilePosition;								//Temporary failsafe to not have an ermpty array. Might be easier with a check against _nrOfAOETIles
		break;
	case TESLACOIL:						//AOE with radius 3, 3x3 trigger in the middle
		Initialize(2, 9, 37);
		for (int i = 0; i < _tileSize; i++)
		{
			AI::Vec2D offset = {i / 3 - 1, i % 3 - 1};
			_occupiedTiles[i] = _tilePosition + offset;
		}
		CalculateCircleAOE(3);
		break;
	//Trigger area is currently the same as the trap's physical area. Might be awkward since the shark trap is larger than its AOE.
	case SHARK:	
	{
		Initialize(100, 18, 3);
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
	for (int i = 0; i < _nrOfAOETiles; i++)
	{
		AI::Vec2D pos = _areaOfEffect[i];
		if (_tileMap->IsFloorOnTile(_areaOfEffect[i]._x, _areaOfEffect[i]._y))
		{
			_tileMap->GetObjectOnTile(_areaOfEffect[i]._x, _areaOfEffect[i]._y, FLOOR)->AddColorOffset({0,0,2});
		}
		//_tileMap->GetObjectOnTile(_tilePosition._x, _tilePosition._y, TRAP)->SetColorOffset({0,0,4});
	}
}

Trap::~Trap()
{
	delete[] _areaOfEffect;
	_areaOfEffect = nullptr;
	delete[] _occupiedTiles;
	_occupiedTiles = nullptr;
}

AI::Vec2D * Trap::GetTiles() const
{
	return _occupiedTiles;
}

int Trap::GetTileSize() const
{
	return _tileSize;
}

void Trap::Activate( Unit* target)
{
	switch (_trapType)
	{
	case SPIKE:

		break;
	case TESLACOIL:
		break;
	case SHARK:
		break;
	default:
		break;
	}
	if (_isActive && target->GetType() == ENEMY)
	{
		target->TakeDamage(3);
		_isActive = false;
	}
	else if (!_isActive && target->GetType() == GUARD)
	{
		_isActive = true;
	}
	
}

void Trap::Update(float deltaTime)
{
	if (_tileMap->IsTrapOnTile(_tilePosition._x, _tilePosition._y))
	{
		_tileMap->GetObjectOnTile(_tilePosition._x, _tilePosition._y, TRAP)->SetColorOffset({0,0,4});
	}
	
	for (int i = 0; i < _nrOfAOETiles; i++)
	{
		if (_tileMap->IsEnemyOnTile(_areaOfEffect[i]._x, _areaOfEffect[i]._y))
		{
			Activate(static_cast<Unit*>(_tileMap->GetObjectOnTile(_areaOfEffect[i]._x, _areaOfEffect[i]._y, ENEMY)));
		}
	}
}

void Trap::Release()
{
	//_renderObject = nullptr;
}
