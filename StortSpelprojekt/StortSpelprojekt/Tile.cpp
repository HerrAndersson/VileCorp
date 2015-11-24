#include "Tile.h"
#include <iostream>



Tile::Tile()
{
	_objectsOnTile = std::vector<GameObject*>(5);
}

Tile::Tile(TileType tiletype)
{
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

void Tile::Update()
{
	// TODO: Check std::vector - Zache/Aron
}

void Tile::Release()
{
	// TODO: The soo what question - Kryztof Wnuk
	_objectsOnTile.clear();
}
