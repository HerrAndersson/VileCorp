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
	bool xChanged = true;

/*alternate method. Useful for circle outlines*/
/*
int decisionOver2 = 1 - x;
	while (y <= x)
	{
		pos = {x, y};
		if (isUnblocked(pos))
		{
			_areaOfEffect[_nrOfAOETiles++] = pos + _tilePosition;
		}
		pos = {y, x};
		if (isUnblocked(pos))
		{
			_areaOfEffect[_nrOfAOETiles++] = pos + _tilePosition;
		}
		pos = {-x, y};
		if (isUnblocked(pos))
		{
			_areaOfEffect[_nrOfAOETiles++] = pos + _tilePosition;
		}
		pos = {-y, x};
		if (isUnblocked(pos))
		{
			_areaOfEffect[_nrOfAOETiles++] = pos + _tilePosition;
		}
		pos = {-x, -y};
		if (isUnblocked(pos))
		{
			_areaOfEffect[_nrOfAOETiles++] = pos + _tilePosition;
		}
		pos = {-y, -x};
		if (isUnblocked(pos))
		{
			_areaOfEffect[_nrOfAOETiles++] = pos + _tilePosition;
		}
		pos = {x, -y};
		if (isUnblocked(pos))
		{
			_areaOfEffect[_nrOfAOETiles++] = pos + _tilePosition;
		}
		pos = {y, -x};
		if (isUnblocked(pos))
		{
			_areaOfEffect[_nrOfAOETiles++] = pos + _tilePosition;
		}
		y++;
		if (decisionOver2 <= 0)
		{
			decisionOver2 += 2 * y + 1;   // Change in decision criterion for y -> y+1
		}
		else
		{
			x--;
			decisionOver2 += 2 * (y - x) + 1;   // Change for y -> y+1, x -> x-1
		}
	}
	if (radius > 1)
	{
		CalculateCircleAOE(radius - 1);
	}
	*/

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

void Trap::CalculateLineAOE(int length)
{
	_areaOfEffectArrayCapacity = length;
	_nrOfAOETiles = 0;
	_areaOfEffect = new AI::Vec2D[_areaOfEffectArrayCapacity];
	AI::Vec2D pos = _tilePosition;

	for (int i = 0; i < length && !_tileMap->IsWallOnTile(pos._x,pos._y); i++)
	{
		_areaOfEffect[_nrOfAOETiles++] = pos;
		pos += _direction;
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

Trap::Trap()
	:GameObject()
{
	_direction = {0,0};
	_areaOfEffect = nullptr;
}

Trap::Trap(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject, 
		  const Tilemap* tileMap, TrapType trapType, AI::Vec2D direction)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject)
{
	_trapType = trapType;
	_direction = direction;
	_tileMap = tileMap;

	int radius = 0;
	switch (_trapType)
	{
	case SPIKE:
		_damage = 3;

		radius = 3;
		_areaOfEffectArrayCapacity = (radius * 2 + 1) * (radius * 2 + 1);				//Note: Since the max area should be constant this can be more carefully calculated offline
		_areaOfEffect = new AI::Vec2D[_areaOfEffectArrayCapacity];
		_nrOfAOETiles = 0;
		CalculateCircleAOE(radius);
		break;
	case TESLACOIL:
		break;
	case SHARK:
		break;
	default:
		_areaOfEffect = nullptr;
		break;
	}
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
