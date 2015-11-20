

#include "Tilemap.h"

bool Tilemap::IsValid(int x, int z) const
{
	return (x >= 0 && x < _width && z >= 0 && z < _height);
}

Tilemap::Tilemap()
{
	_height = 10;
	_width = 10;
	_map = new TileType*[_width];

	for (int i = 0; i < _width; i++)
	{
		_map[i] = new TileType[_height];

		for (int j = 0; j < _height; j++)
		{
			_map[i][j] = TileType::Unwalkable;
		}
	}
}
Tilemap::Tilemap(int width, int height)
{
	if (width > 0 && height > 0)
	{
		_height = height;
		_width = width;
		_map = new TileType*[_width];

		for (int i = 0; i < _width; i++)
		{
			_map[i] = new TileType[_height];

			for (int j = 0; j < _height; j++)
			{
				_map[i][j] = TileType::Unwalkable;
			}
		}
	}
}
Tilemap::Tilemap(const Tilemap& copy)
{
	_height = copy._height;
	_width = copy._width;
	_map = new TileType*[copy._width];

	for (int i = 0; i < copy._width; i++)
	{
		_map[i] = new TileType[copy._height];

		for (int j = 0; j < copy._height; j++)
		{
			_map[i][j] = TileType(copy._map[i][j]);
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
	//Note that when using this method in another file the TileType has to be written as "Tilemap::TileType::Unwalkable/Walkable", Aron
	if (IsValid(x, z))
	{
		_map[x][z] = type;
	}
}
bool Tilemap::IsTileOccupied(int x, int z) const
{
	if (IsValid(x, z))
	{
		return _map[x][z] == TileType::Unwalkable;
	}
	else
	{
		return false;
	}
}
Tilemap::TileType Tilemap::GetTileType(int x, int z) const
{
	if (IsValid(x, z))
	{
		return _map[x][z];
	}
	else
	{
		return TileType::Unwalkable;
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