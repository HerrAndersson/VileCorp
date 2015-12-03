#include "Tilemap.h"

bool Tilemap::IsValid(int x, int z) const
{
	return (x >= 0 && x < _width && z >= 0 && z < _height);
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
	if (IsValid(x, z))
	{
		switch (obj->GetType())
		{
		case FLOOR:
		case WALL:
			_map[x][z]._objectsOnTile[0] = obj;
			result = true;
			break;
		case UNIT:
			if (_map[x][z]._objectsOnTile[1] == nullptr)
			{
				_map[x][z]._objectsOnTile[1] = obj;
				result = true;
			}
			else if(_map[x][z]._objectsOnTile[2] == nullptr)
			{
				_map[x][z]._objectsOnTile[2] = obj;
				result = true;
			}
			break;
		case TRAP:
		case LOOT:
			if (_map[x][z]._objectsOnTile[3] == nullptr)
			{
				_map[x][z]._objectsOnTile[3] = obj;
				result = true;
			}
			break;
		case TRIGGER:
			if (_map[x][z]._objectsOnTile[4] == nullptr)
			{
				_map[x][z]._objectsOnTile[4] = obj;
				result = true;
			}
			break;
		default:
			break;
		}
	}
	return result;
}

bool Tilemap::RemoveObjectFromTile(int x, int z, GameObject * obj)
{
	bool result = false;
	if (IsValid(x, z))
	{
		switch (obj->GetType())
		{
		case FLOOR:
		case WALL:
			if (_map[x][z]._objectsOnTile[1]->GetID() == obj->GetID())
			{
				_map[x][z]._objectsOnTile[1] = nullptr;
				result = true;
			}
			break;
		case UNIT:
			if (_map[x][z]._objectsOnTile[1]->GetID() == obj->GetID())
			{
				_map[x][z]._objectsOnTile[1] = nullptr;
				result = true;
			}
			else if (_map[x][z]._objectsOnTile[2]->GetID() == obj->GetID())
			{
				_map[x][z]._objectsOnTile[2] = nullptr;
				result = true;
			}
			break;
		case TRAP:
		case LOOT:
			if (_map[x][z]._objectsOnTile[3]->GetID() == obj->GetID())
			{
				_map[x][z]._objectsOnTile[3] = nullptr;
				result = true;
			}
			break;
		case TRIGGER:
			if (_map[x][z]._objectsOnTile[4]->GetID() == obj->GetID())
			{
				_map[x][z]._objectsOnTile[4] = nullptr;
				result = true;
			}
			break;
		default:
			break;
		}
	}
	return result;
}

void Tilemap::ClearTile(int x, int z)
{
	for (int i = 0; i < Tile::OBJECT_CAPACITY; i++)
	{
		_map[x][z]._objectsOnTile[i] = nullptr;
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

GameObject* Tilemap::GetObjectOnTile(int x, int z, int index) const
{
	return _map[x][z]._objectsOnTile[index];
}

bool Tilemap::IsWallOnTile(int x, int z) const
{
	if (_map[x][z]._objectsOnTile[0] == nullptr)
	{
		return false;
	}
	else
	{
		return  _map[x][z]._objectsOnTile[0]->GetType() == WALL;
	}
}

int Tilemap::UnitsOnTile(int x, int z) const
{
	int result = 0;
	if (_map[x][z]._objectsOnTile[1] != nullptr && _map[x][z]._objectsOnTile[1]->GetType() == UNIT)
	{
		result++;
	}
	if (_map[x][z]._objectsOnTile[2] != nullptr && _map[x][z]._objectsOnTile[2]->GetType() == UNIT)
	{
		result++;
	}
	return result;
}

bool Tilemap::IsTrapOnTile(int x, int z) const
{
	if (_map[x][z]._objectsOnTile[3] == nullptr)
	{
		return false;
	}
	else
	{
		return  _map[x][z]._objectsOnTile[3]->GetType() == TRAP;
	}
}

bool Tilemap::IsTriggerOnTile(int x, int z) const
{
	if (_map[x][z]._objectsOnTile[4] == nullptr)
	{
		return false;
	}
	else
	{
		return  _map[x][z]._objectsOnTile[4]->GetType() == TRIGGER;
	}
}

bool Tilemap::IsObjectiveOnTile(int x, int z) const
{
	if (_map[x][z]._objectsOnTile[3] == nullptr)
	{
		return false;
	}
	else
	{
		return  _map[x][z]._objectsOnTile[3]->GetType() == LOOT;
	}
}

bool Tilemap::IsTypeOnTile(int x, int z, Type type) const
{
	for (int i = 0; i < Tile::OBJECT_CAPACITY; i++)
	{
		if (_map[x][z]._objectsOnTile[i]->GetType() == type)
		{
			return true;
		}
	}
	return false;
}