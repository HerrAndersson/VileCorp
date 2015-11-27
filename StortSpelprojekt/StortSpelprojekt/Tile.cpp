#include "Tile.h"
#include <iostream>



Tile::Tile()
{
}

Tile::Tile(TileType tiletype)
{
	//_size = 0;
	_tiletype = tiletype;
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

	if (_objectsOnTile[2] == trap)
	{
			exist = true;
	}
	else
	{
		_objectsOnTile[2] = trap;
	}

	return exist;
}

bool Tile::DisconnectGameObject(short ID)
{
	bool removed = false;

	for (int i = 0; i < OBJECT_CAPACITY; i++)
	{
		if (_objectsOnTile[i]->GetID() == ID)
		{
			delete _objectsOnTile[i];
			_objectsOnTile[i] = nullptr;
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
	//_objectsOnTile.clear();
}

GameObject* Tile::GetGameObject(int index) const
{
	if (index < 0 || index > 3)
	{
		return nullptr;
	}
	else
	{
		return _objectsOnTile[index];
	}
}
