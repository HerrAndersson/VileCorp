#pragma once
#include "GameObject.h"
#include <vector>

class Tile
{
public:
	static const int OBJECT_CAPACITY = 5;
private:
	GameObject* _objectsOnTile[OBJECT_CAPACITY];					//0 = floor or wall, 1 = 1st unit, 2 = 2nd unit, 3 = trap/thief objectives, 4 = trigger


public:
	Tile();
	~Tile();
	   
	void Update(); // TODO: Objektinteraktion ska delvis skötas här Zache/Aron

	void Release();

	GameObject* GetGameObject( int index ) const;			//since we've got proper indices, we should probably call by index

	bool IsWall()const;
	int UnitsOnTile()const;
	bool IsTrapOnTile()const;
	bool IsTriggerOnTile() const;
	bool IsObjectiveOnTile()const;


	Tile& operator=(const Tile& comp)
	{
		for (int i = 0; i < OBJECT_CAPACITY; i++)
		{
			_objectsOnTile[i] = comp._objectsOnTile[i];
		}
		return *this;
	}
};

