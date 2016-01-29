#include "Tilemap.h"

bool Tilemap::IsValid(int x, int z) const
{
	return (x >= 0 && x < _width && z >= 0 && z < _height);
}

bool Tilemap::IsPlacable(int x, int z, Type type) const
{
	GameObject* result = nullptr;
	if (IsValid(x, z))
	{
		if (!IsWallOnTile(x, z))
		{
			switch (type)
			{
			case FLOOR:
			case WALL:
				result = _map[x][z]._objectsOnTile[0];
				break;
			case ENEMY:
				result = _map[x][z]._objectsOnTile[1];
				break;
			case GUARD:
				result = _map[x][z]._objectsOnTile[2];
				break;
			case LOOT:
			case SPAWN:
			case TRAP:
				result = _map[x][z]._objectsOnTile[3];
				break;
			default:
				break;
			}
		}
	}

	if (result == nullptr)
	{
		return true;
	}
	else
	{
		return false;
	}
}

Tilemap::Tilemap()
{
	_height = 10;
	_width = 10;
	_map = new Tile*[_width];

	for (int i = 0; i < _width; i++)
	{
		_map[i] = new Tile[_height];

		for (int j = 0; j < _height; j++)
		{
			_map[i][j] = Tile();
		}
	}
}
Tilemap::Tilemap(int width, int height)
{
	if (width > 0 && height > 0)
	{
		_height = height;
		_width = width;
		_map = new Tile*[_width];
		for (int i = 0; i < _width; i++)
		{
			_map[i] = new Tile[_height];

			for (int j = 0; j < _height; j++)
			{
				_map[i][j] = Tile();
			}
		}
	}
}
Tilemap::Tilemap(const Tilemap& copy)
{
	_height = copy._height;
	_width = copy._width;
	_map = new Tile*[copy._width];

	for (int i = 0; i < copy._width; i++)
	{
		_map[i] = new Tile[copy._height];

		for (int j = 0; j < copy._height; j++)
		{
			_map[i][j] = Tile(copy._map[i][j]);
		}
	}
}
Tilemap::~Tilemap()
{
	for (int i = 0; i < _width; i++)
	{
		delete[] _map[i];
	}
	delete[] _map;
}

bool Tilemap::AddObjectToTile(int x, int z, GameObject * obj)
{
	bool result = false;
	if (IsValid(x, z) && obj != nullptr)
	{
		int pos = -1;
		switch (obj->GetType())
		{
		case FLOOR:
		case WALL:
			pos = 0;
			break;
		case ENEMY:
			pos = 1;
			break;
		case GUARD:
			pos = 2;
			break;
		case TRAP:
		case LOOT:
		case SPAWN:
			pos = 3;
			break;
		default:
			break;
		}

		if (pos != -1)
		{
			if (_map[x][z]._objectsOnTile[pos] == nullptr)
			{
				_map[x][z]._objectsOnTile[pos] = obj;
				result = true;
			}
			obj->SetTilePosition(AI::Vec2D(x, z));
		}
	}
	return result;
}

bool Tilemap::RemoveObjectFromTile(int x, int z, GameObject * obj)
{
	bool result = false;
	if (IsValid(x, z) && obj != nullptr)
	{
		int pos = -1;
		switch (obj->GetType())
		{
		case FLOOR:
		case WALL:
			pos = 0;
			break;
		case ENEMY:
			pos = 1;
			break;
		case GUARD:
			pos = 2;
			break;
		case TRAP:
		case LOOT:
		case SPAWN:
			pos = 3;
			break;
		default:
			break;
		}
		if (pos != -1)
		{
			if (_map[x][z]._objectsOnTile[pos] != nullptr && _map[x][z]._objectsOnTile[pos]->GetID() == obj->GetID())
			{
				_map[x][z]._objectsOnTile[pos] = nullptr;
				result = true;
			}
		}
		
	}
	return result;
}

bool Tilemap::RemoveObjectFromTile(GameObject* obj)
{
	if (obj != nullptr)
	{
		AI::Vec2D tileCoord = obj->GetTilePosition();
		return RemoveObjectFromTile(tileCoord._x, tileCoord._y, obj);
	}
	else
	{
		return false;
	}
}

void Tilemap::ClearTile(int x, int z)
{
	if (IsValid(x, z))
	{
		for (int i = 0; i < Tile::OBJECT_CAPACITY; i++)
		{
			_map[x][z]._objectsOnTile[i] = nullptr;
		}
	}
}

int Tilemap::GetNrOfTiles() const
{
	return _height*_width;
}

int Tilemap::GetHeight() const
{
	return _height;
}

int Tilemap::GetWidth() const
{
	return _width;
}

std::vector<GameObject*> Tilemap::GetAllObjectsOnTile(AI::Vec2D tileCoords) const
{
	std::vector<GameObject*> objectsOnTile;
	objectsOnTile.reserve(Tile::OBJECT_CAPACITY);

	if (IsValid(tileCoords._x, tileCoords._y))
	{
		for (int i = 0; i < Tile::OBJECT_CAPACITY; i++)
		{
			if (_map[tileCoords._x][tileCoords._y]._objectsOnTile[i] != nullptr)
			{
				objectsOnTile.push_back(_map[tileCoords._x][tileCoords._y]._objectsOnTile[i]);
			}
		}
	}

	return objectsOnTile;
}

GameObject * Tilemap::GetObjectOnTile(int x, int z, Type type) const
{
	GameObject* result = nullptr;
	if (IsValid(x, z))
	{
		switch (type)
		{
		case FLOOR:
		case WALL:
			result = _map[x][z]._objectsOnTile[0];
			break;
		case ENEMY:
			result = _map[x][z]._objectsOnTile[1];
			break;
		case GUARD:	
			result = _map[x][z]._objectsOnTile[2];
			break;
		case LOOT:
		case SPAWN:
		case TRAP:
			result = _map[x][z]._objectsOnTile[3];
			break;
		default:
			break;
		}
	}
	if (result!= nullptr && result->GetType() == type)
	{
		return result;
	}
	else
	{
		return nullptr;
	}
}

//GameObject * Tilemap::GetUnitOnTile(int x, int z) const
//{
//	GameObject* result = nullptr;
//	if (_map[x][z]._objectsOnTile[1] != nullptr)
//	{
//		result = _map[x][z]._objectsOnTile[1];
//	}
//	else if (_map[x][z]._objectsOnTile[2] != nullptr)
//	{
//		result = _map[x][z]._objectsOnTile[2];
//	}
//	return result;
//}

bool Tilemap::IsArchitectureOnTile(int x, int z) const
{
	return IsValid(x, z) && _map[x][z]._objectsOnTile[0] != nullptr;
}

bool Tilemap::IsWallOnTile(int x, int z) const
{
	return IsArchitectureOnTile(x, z) && _map[x][z]._objectsOnTile[0]->GetType() == WALL;
}

bool Tilemap::IsFloorOnTile(int x, int z) const
{
	return IsArchitectureOnTile(x, z) && _map[x][z]._objectsOnTile[0]->GetType() == FLOOR;
}

int Tilemap::UnitsOnTile(int x, int z) const
{
	int result = 0;
	if (IsValid(x, z))
	{
		if (_map[x][z]._objectsOnTile[1] != nullptr)
		{
			result++;
		}
		if (_map[x][z]._objectsOnTile[2] != nullptr)
		{
			result++;
		}
	}
	return result;
}

bool Tilemap::IsGuardOnTile(int x, int z) const
{
	return  IsValid(x, z) && _map[x][z]._objectsOnTile[2] != nullptr;
}

bool Tilemap::IsEnemyOnTile(int x, int z) const
{
	return  IsValid(x, z) && _map[x][z]._objectsOnTile[1] != nullptr;
}

bool Tilemap::IsTrapOnTile(int x, int z) const
{
	if (!IsValid(x, z) || _map[x][z]._objectsOnTile[3] == nullptr)
	{
		return false;
	}
	else
	{
		return  _map[x][z]._objectsOnTile[3]->GetType() == TRAP;
	}
}

bool Tilemap::IsObjectiveOnTile(int x, int z) const
{
	if (!IsValid(x, z) || _map[x][z]._objectsOnTile[3] == nullptr)
	{
		return false;
	}
	else
	{
		return  _map[x][z]._objectsOnTile[3]->GetType() == LOOT;
	}
}

bool Tilemap::IsSpawnOnTile(int x, int z) const
{
	if (!IsValid(x, z) || _map[x][z]._objectsOnTile[3] == nullptr)
	{
		return false;
	}
	else
	{
		return  _map[x][z]._objectsOnTile[3]->GetType() == SPAWN;
	}
}



bool Tilemap::IsTypeOnTile(int x, int z, Type type) const
{
	if (IsValid(x, z))
	{
		for (int i = 0; i < Tile::OBJECT_CAPACITY; i++)
		{
			if (_map[x][z]._objectsOnTile[i] != nullptr && _map[x][z]._objectsOnTile[i]->GetType() == type)
			{
				return true;
			}
		}
	}
	return false;
}

bool Tilemap::IsTileVisible(int x, int z) const
{
	return _map[x][z]._isVisible;
}

bool Tilemap::IsTileEmpty(int x, int z) const
{
	bool empty = true;
	for (int i = 0; i < Tile::OBJECT_CAPACITY; i++)
	{
		if (_map[x][z]._objectsOnTile[i] != nullptr)
		{
			empty = false;
			break;
		}
	}
	return empty;
}

