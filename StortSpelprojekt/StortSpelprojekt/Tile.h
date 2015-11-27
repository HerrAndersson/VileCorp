#pragma once
#include "GameObject.h"
#include <vector>

enum TileType { WALKABLE, UNWALKABLE };

class Tile : public GameObject
{
private:
	TileType _tiletype;
	int _size;
	std::vector<GameObject*> _objectsOnTile;

public:
	Tile();
	Tile(TileType tiletype);
	~Tile();

	TileType GetTileType() const;
	void SetTileType(TileType tiletype);

	bool ConnectGameObject(GameObject* trap);
	bool DisconnectGameObject(short ID);
	   
	void Update(); // TODO: Objektinteraktion ska delvis skötas här Zache/Aron

	void Release();
};

