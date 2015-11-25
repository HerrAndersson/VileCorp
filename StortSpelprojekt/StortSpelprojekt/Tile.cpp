#include "Tile.h"
#include <iostream>



Tile::Tile()
{
	_size = 0;
	_objectsOnTile = std::vector<GameObject*>(5);
}

Tile::Tile(TileType tiletype)
{
	_size = 0;
	_tiletype = tiletype;
	_objectsOnTile = std::vector<GameObject*>(5);
}

Tile::~Tile() 
{}

TileType Tile::GetTileType() const
{
	return _tiletype;
}

void Tile::SetTileType(TileType tiletype)
{
	_tiletype = tiletype;
}

bool Tile::ConnectGameObject(GameObject * trap)
{
	bool exist = false;

	for (int i = 0; i < _size; i++)
	{
		if (_objectsOnTile[i] == trap)
		{
			exist = true;
		}
	}

	if (!exist)
	{
		_objectsOnTile.push_back(trap);
	}

	return exist;
}

bool Tile::DisconnectGameObject(short ID)
{
	bool removed = false;

	for (int i = 0; i < _size; i++)
	{
		if (_objectsOnTile[i]->GetID() == ID)
		{
			// Decrease size of vector
			_size--;

			// Replace pointer with the last pointer int the vector
			_objectsOnTile[i] = _objectsOnTile[_size];

			// Remove pointer value to avoid various problems
			_objectsOnTile.pop_back();

			removed = true;
		}
	}

	return removed;
}

void Tile::Update()
{
	// TODO: Check std::vector - Zache/Aron
}

void Tile::Release()
{
	// TODO: The soo what question - Kryztof Wnuk
	_objectsOnTile.clear();
}
