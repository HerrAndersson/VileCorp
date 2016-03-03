#include "Tilemap.h"

Tilemap::Tilemap()
{
	_height = 10;
	_width = 10;
	_nrOfLoot = 0;
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

Tilemap::Tilemap(AI::Vec2D size)
{
	if (size._x > 0 && size._y > 0)
	{
		_height = size._x;
		_width = size._y;
		_nrOfLoot = 0;
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
	_map = nullptr;
}

bool Tilemap::AddObjectToTile(AI::Vec2D pos, GameObject * obj)
{
	bool result = false;
	if (IsValid(pos) && obj != nullptr)
	{
		int arrayPos = -1;
		switch (obj->GetType())
		{
		case  System::FLOOR:
		case  System::WALL:
			arrayPos = 0;
			break;
		case  System::ENEMY:
			arrayPos = 1;
			break;
		case  System::GUARD:
			arrayPos = 2;
			break;
		case  System::TRAP:
		case  System::SPAWN:
		case  System::CAMERA:
			arrayPos = 3;
			break;
		case  System::LOOT:
			arrayPos = 5;
			break;
		case  System::FURNITURE:
			arrayPos = 4;
			break;
		default:
			break;
		}

		if (arrayPos != -1)
		{
			if (_map[pos._x][pos._y]._objectsOnTile[arrayPos] == nullptr)
			{
				_map[pos._x][pos._y]._objectsOnTile[arrayPos] = obj;
			//	obj->SetTilePosition(pos);
				result = true;
				if (obj->GetType() == System::LOOT)
				{
					_nrOfLoot++;
				}
			}
		}
	}
	return result;
}

bool Tilemap::AddObjectToTile(int x, int z, GameObject * obj)
{
	return AddObjectToTile(AI::Vec2D(x, z), obj);
}

bool Tilemap::RemoveObjectFromTile(AI::Vec2D pos, GameObject * obj)
{
	bool result = false;
	if (IsValid(pos) && obj != nullptr)
	{
		int arrayPos = -1;
		switch (obj->GetType())
		{
		case  System::FLOOR:
		case  System::WALL:
			arrayPos = 0;
			break;
		case  System::ENEMY:
			arrayPos = 1;
			break;
		case  System::GUARD:
			arrayPos = 2;
			break;
		case  System::TRAP:
		case  System::SPAWN:
		case  System::CAMERA:
			arrayPos = 3;
			break;
		case  System::FURNITURE:
			arrayPos = 4;
			break;
		case  System::LOOT:
			arrayPos = 5;
			break;
		default:
			break;
		}
		if (arrayPos != -1)
		{
			if (_map[pos._x][pos._y]._objectsOnTile[arrayPos] != nullptr && _map[pos._x][pos._y]._objectsOnTile[arrayPos]->GetID() == obj->GetID())
			{
				_map[pos._x][pos._y]._objectsOnTile[arrayPos] = nullptr;
				result = true;
				if (obj->GetType() == System::LOOT)
				{
					_nrOfLoot--;
				}
			}
		}
		
	}
	return result;
}

bool Tilemap::RemoveObjectFromTile(GameObject* obj)
{
	if (obj != nullptr)
	{
		return RemoveObjectFromTile(obj->GetTilePosition(), obj);
	}
	else
	{
		return false;
	}
}

void Tilemap::ClearTile(AI::Vec2D pos)
{
	if (IsValid(pos))
	{
		for (int i = 0; i < Tile::OBJECT_CAPACITY; i++)
		{
			_map[pos._x][pos._y]._objectsOnTile[i] = nullptr;
		}
	}
}

void Tilemap::LockTile(AI::Vec2D pos)
{
	if (_map[pos._x][pos._y]._objectsOnTile[0] != nullptr)
	{
		_map[pos._x][pos._y]._lock = true;
		_map[pos._x][pos._y]._objectsOnTile[0]->SetColorOffset(XMFLOAT3(0.8f, 0, 0));
	}
}

void Tilemap::UnlockTile(AI::Vec2D pos)
{
	if (_map[pos._x][pos._y]._objectsOnTile[0] != nullptr)
	{
		_map[pos._x][pos._y]._lock = false;
		_map[pos._x][pos._y]._objectsOnTile[0]->SetColorOffset(XMFLOAT3(0.8f, 0.8f, 0));
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

int Tilemap::GetNrOfLoot() const
{
	return _nrOfLoot;
}

std::vector<GameObject*> Tilemap::GetAllObjectsOnTile(AI::Vec2D tileCoords) const
{
	std::vector<GameObject*> objectsOnTile;
	objectsOnTile.reserve(Tile::OBJECT_CAPACITY);

	if (IsValid(tileCoords))
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

GameObject * Tilemap::GetObjectOnTile(AI::Vec2D pos, System::Type type) const
{
	GameObject* result = nullptr;
	if (IsValid(pos))
	{
		switch (type)
		{
		case  System::FLOOR:
		case  System::WALL:
			result = _map[pos._x][pos._y]._objectsOnTile[0];
			break;
		case  System::ENEMY:
			result = _map[pos._x][pos._y]._objectsOnTile[1];
			break;
		case  System::GUARD:
			result = _map[pos._x][pos._y]._objectsOnTile[2];
			break;
		case  System::SPAWN:
		case  System::TRAP:
		case  System::CAMERA:
			result = _map[pos._x][pos._y]._objectsOnTile[3];
			break;
		case  System::FURNITURE:
			result = _map[pos._x][pos._y]._objectsOnTile[4];
			break;
		case  System::LOOT:
			result = _map[pos._x][pos._y]._objectsOnTile[5];
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

GameObject * Tilemap::GetObjectOnTile(int x, int z, System::Type type) const
{
	return GetObjectOnTile(AI::Vec2D(x, z), type);
}

bool Tilemap::IsValid(AI::Vec2D pos) const
{
	return (pos._x >= 0 && pos._x < _width && pos._y >= 0 && pos._y < _height);
}

bool Tilemap::IsValid(int x, int z) const
{
	return IsValid(AI::Vec2D(x, z));
}

bool Tilemap::IsPlaceable(int x, int z, System::Type type) const
{
	bool placable = false;
	if (IsValid(x, z))
	{
		if (!IsWallOnTile(x, z))
		{
			GameObject* result = nullptr;
			switch (type)
			{
			case  System::FLOOR:
			case  System::WALL:
				result = _map[x][z]._objectsOnTile[0];
				break;
			case  System::ENEMY:
				result = _map[x][z]._objectsOnTile[1];
				break;
			case  System::GUARD:
				result = _map[x][z]._objectsOnTile[2];
				break;
			case  System::SPAWN:
			case  System::TRAP:
			case  System::CAMERA:
				result = _map[x][z]._objectsOnTile[3];
				break;
			case  System::FURNITURE:
				result = _map[x][z]._objectsOnTile[4];
				break;
			case  System::LOOT:
				result = _map[x][z]._objectsOnTile[5];
				break;
			default:
				break;
			}
			if (result == nullptr)
			{
				placable = true;
			}
			if (_map[x][z]._lock)
			{
				placable = false;
			}
		}
	}

	return placable;
}

bool Tilemap::IsPlaceable(AI::Vec2D pos, System::Type type) const
{
	return IsPlaceable(pos._x, pos._y, type);
}

bool Tilemap::IsArchitectureOnTile(int x, int z) const
{
	return IsValid(x, z) && _map[x][z]._objectsOnTile[0] != nullptr;
}

bool Tilemap::IsArchitectureOnTile(AI::Vec2D pos) const
{
	return IsArchitectureOnTile(pos._x, pos._y);
}

bool Tilemap::IsWallOnTile(int x, int z) const
{
	return IsArchitectureOnTile(x, z) && _map[x][z]._objectsOnTile[0]->GetType() == System::WALL;
}

bool Tilemap::IsWallOnTile(AI::Vec2D pos) const
{
	return IsWallOnTile(pos._x, pos._y);
}

bool Tilemap::IsFurnitureOnTile(int x, int z) const
{
	return IsValid(x, z) && _map[x][z]._objectsOnTile[4] != nullptr;
}

bool Tilemap::IsFurnitureOnTile(AI::Vec2D pos) const
{
	return IsFurnitureOnTile(pos._x, pos._y);
}

bool Tilemap::IsFloorOnTile(int x, int z) const
{
	return IsArchitectureOnTile(x, z) && _map[x][z]._objectsOnTile[0]->GetType() == System::FLOOR;
}

bool Tilemap::IsFloorOnTile(AI::Vec2D pos) const
{
	return IsFloorOnTile(pos._x, pos._y);
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

int Tilemap::UnitsOnTile(AI::Vec2D pos) const
{
	return UnitsOnTile(pos._x, pos._y);
}

bool Tilemap::IsGuardOnTile(int x, int z) const
{
	return  IsValid(x, z) && _map[x][z]._objectsOnTile[2] != nullptr;
}

bool Tilemap::IsGuardOnTile(AI::Vec2D pos) const
{
	return IsGuardOnTile(pos._x, pos._y);
}

bool Tilemap::IsEnemyOnTile(int x, int z) const
{
	return  IsValid(x, z) && _map[x][z]._objectsOnTile[1] != nullptr;
}

bool Tilemap::IsEnemyOnTile(AI::Vec2D pos) const
{
	return IsEnemyOnTile(pos._x, pos._y);
}

bool Tilemap::IsTrapOnTile(int x, int z) const
{
	if (!IsValid(x, z) || _map[x][z]._objectsOnTile[3] == nullptr)
	{
		return false;
	}
	else
	{
		return  _map[x][z]._objectsOnTile[3]->GetType() == System::TRAP;
	}
}

bool Tilemap::IsTrapOnTile(AI::Vec2D pos) const
{
	return IsTrapOnTile(pos._x, pos._y);
}

bool Tilemap::IsObjectiveOnTile(int x, int z) const
{
	return IsValid(x, z) && _map[x][z]._objectsOnTile[5] != nullptr;
}

bool Tilemap::IsObjectiveOnTile(AI::Vec2D pos) const
{
	return IsObjectiveOnTile(pos._x, pos._y);
}

bool Tilemap::IsSpawnOnTile(int x, int z) const
{
	if (!IsValid(x, z) || _map[x][z]._objectsOnTile[3] == nullptr)
	{
		return false;
	}
	else
	{
		return  _map[x][z]._objectsOnTile[3]->GetType() == System::SPAWN;
	}
}

bool Tilemap::IsSpawnOnTile(AI::Vec2D pos) const
{
	return IsSpawnOnTile(pos._x, pos._y);
}

bool Tilemap::IsTypeOnTile(int x, int z, System::Type type) const
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

bool Tilemap::IsTypeOnTile(AI::Vec2D pos, System::Type type) const
{
	return IsTypeOnTile(pos._x, pos._y, type);
}

bool Tilemap::IsTileVisible(int x, int z) const
{
	return _map[x][z]._isVisible;
}

bool Tilemap::IsTileVisible(AI::Vec2D pos) const
{
	return IsTileVisible(pos._x, pos._y);
}

/*
	To place an object, the tile should be empty besides a floor
*/

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

bool Tilemap::IsTileEmpty(AI::Vec2D pos) const
{
	return IsTileEmpty(pos._x, pos._y);
}