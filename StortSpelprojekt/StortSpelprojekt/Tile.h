#pragma once
#include "GameObject.h"
#include <vector>

enum TileType { WALKABLE, UNWALKABLE };

class Tile : public GameObject
{
public:
	const int OBJECT_CAPACITY = 4;
private:
	TileType _tiletype;
	GameObject* _objectsOnTile[4];					//0 = 1st unit, 1 = 2nd unit, 2 = trap/thief objectives, 3 = trigger
	//int _size;
	//std::vector<GameObject*> _objectsOnTile;


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

	GameObject* GetGameObject( int index ) const;				//since we've got proper indices, we should probably call by index

	Tile& operator=(const Tile& comp)
	{
		for (int i = 0; i < OBJECT_CAPACITY; i++)
		{
			_objectsOnTile[i] = comp._objectsOnTile[i];
		}
		_tiletype = comp._tiletype;
		return *this;
	}
};

