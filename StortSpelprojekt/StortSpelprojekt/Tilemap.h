//Handles the walkable and non-walkable tiles
//Aron and Victor worked on this

#ifndef TILEMAP_H
#define TILEMAP_H

#include "Tile.h"

class Tilemap
{
private:
	int _height;
	int _width;
	
	Tile** _map;

	bool IsValid(int x, int z) const;
	
public:
	Tilemap();
	Tilemap(int width, int height);
	Tilemap(const Tilemap& copy);
	~Tilemap();

	void ChangeTileState(int x, int z, TileType type);
	bool IsTileOccupied(int x, int z) const;
	TileType GetTileType(int x, int z) const;
	
	int SizeOfTileMap() const;
	int GetHeight() const;
	int GetWidth() const;

	std::vector<GameObject*> GetTilePointers() const; // More thinking
};
#endif