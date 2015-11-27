

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
			_map[i][j] = Tile(WALKABLE);
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
				_map[i][j] = Tile(WALKABLE);
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

void Tilemap::ChangeTileState(int x, int z, TileType type)
{
	if (IsValid(x, z))
	{
		_map[x][z].SetTileType(type);
	}
}
bool Tilemap::IsTileOccupied(int x, int z) const
{
	if (IsValid(x, z))
	{
		return _map[x][z].GetTileType() == UNWALKABLE;
	}
	else
	{
		return false;
	}
}
TileType Tilemap::GetTileType(int x, int z) const
{
	if (IsValid(x, z))
	{
		return _map[x][z].GetTileType();
	}
	else
	{
		return UNWALKABLE;
	}
}

int Tilemap::SizeOfTileMap() const
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