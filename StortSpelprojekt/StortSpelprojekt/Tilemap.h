//Handles the walkable and non-walkable tiles
//Aron and Victor worked on this

#ifndef TILEMAP_H
#define TILEMAP_H

#include "GameObject.h"

class Tilemap
{
private:
	struct Tile
	{
		static const int OBJECT_CAPACITY = 5;
		GameObject* _objectsOnTile[OBJECT_CAPACITY];					//0 = floor or wall, 1 = 1st unit, 2 = 2nd unit, 3 = trap/thief objectives, 4 = trigger
		Tile()
		{
			for (int i = 0; i < OBJECT_CAPACITY; i++)
			{
				_objectsOnTile[i] = nullptr;
			}
		}
		~Tile(){}

		Tile& operator=(const Tile& comp)
		{
			for (int i = 0; i < OBJECT_CAPACITY; i++)
			{
				_objectsOnTile[i] = comp._objectsOnTile[i];
			}
			return *this;
		}
	};
	int _height;
	int _width;
	
	Tile** _map;

	bool IsValid(int x, int z) const;
	
public:
	Tilemap();
	Tilemap(int width, int height);
	Tilemap(const Tilemap& copy);
	~Tilemap();

	bool AddObjectToTile(int x, int z, GameObject* obj);
	bool RemoveObjectFromTile(int x, int z, GameObject* obj);
	void ClearTile(int x, int z);
	
	int GetNrOfTiles() const;
	int GetHeight() const;
	int GetWidth() const;

	GameObject* GetObjectOnTile(int x, int z, int index) const; // More thinking
	bool IsWallOnTile(int x, int z)const;
	int UnitsOnTile(int x, int z)const;
	bool IsTrapOnTile(int x, int z)const;
	bool IsTriggerOnTile(int x, int z) const;
	bool IsObjectiveOnTile(int x, int z)const;
	bool IsTypeOnTile(int x, int z, Type type)const;
};
#endif