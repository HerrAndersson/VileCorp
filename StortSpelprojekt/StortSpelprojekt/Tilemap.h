//Handles the walkable and non-walkable tiles
//Aron and Victor worked on this

#pragma once

#include "GameObject.h"

class Tilemap
{
private:
	struct Tile
	{
		static const int OBJECT_CAPACITY = 4;
		GameObject* _objectsOnTile[OBJECT_CAPACITY];					//0 = floor or wall, 1 = enemy, 2 = guard, 3 = trap or thief objectives or spawnpoint
		bool _isVisible;
		Tile()
		{
			for (int i = 0; i < OBJECT_CAPACITY; i++)
			{
				_objectsOnTile[i] = nullptr;
			}
			_isVisible = false;
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


public:
	Tilemap();
	Tilemap(int width, int height);
	Tilemap(const Tilemap& copy);
	~Tilemap();

	bool AddObjectToTile(int x, int z, GameObject* obj);
	bool RemoveObjectFromTile(int x, int z, GameObject* obj);
	bool RemoveObjectFromTile(GameObject* obj);
	void ClearTile(int x, int z);

	int GetNrOfTiles() const;
	int GetHeight() const;
	int GetWidth() const;

	GameObject* GetObjectOnTile(int x, int z, Type type) const;

	std::vector<GameObject*> GetAllObjectsOnTile(AI::Vec2D tileCoords) const;
	bool IsValid(int x, int z) const;
	bool IsArchitectureOnTile(int x, int z)const;
	bool IsWallOnTile(int x, int z)const;
	bool IsFloorOnTile(int x, int z)const;
	int UnitsOnTile(int x, int z)const;
	bool IsGuardOnTile(int x, int z)const;
	bool IsEnemyOnTile(int x, int z)const;
	bool IsTrapOnTile(int x, int z)const;
	bool IsObjectiveOnTile(int x, int z)const;
	bool IsSpawnOnTile(int x, int z)const;
	bool IsTypeOnTile(int x, int z, Type type)const;
	bool IsTileVisible(int x, int z) const;
	bool IsTileEmpty(int x, int z) const;
};