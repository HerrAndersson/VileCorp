#pragma once
#include "GameObject.h"
#include <vector>

enum TileType { WALKABLE, UNWALKABLE };

class Tile : public GameObject
{
private:
	TileType _tiletype;
	std::vector<GameObject*> _objectsOnTile;

public:
	Tile();
	Tile(TileType tiletype);
	~Tile();

	TileType GetTileType() const;
	void SetTileType(TileType tiletype);
	   
	void Update();

	void Release();
};

