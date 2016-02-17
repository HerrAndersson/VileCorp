//Handles the walkable and non-walkable tiles
//Aron and Victor worked on this

#pragma once

#include "GameObject.h"

class Tilemap
{
private:
	struct Tile
	{
		static const int OBJECT_CAPACITY = 5;
		GameObject* _objectsOnTile[OBJECT_CAPACITY];					//0 = floor or wall, 1 = enemy, 2 = guard, 3 = trap or thief objectives or spawnpoint, 4 = furniture
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
	Tilemap(AI::Vec2D size);
	Tilemap(const Tilemap& copy);
	~Tilemap();

	bool AddObjectToTile(AI::Vec2D pos, GameObject* obj);
	bool AddObjectToTile(int x, int z, GameObject* obj);

	bool RemoveObjectFromTile(AI::Vec2D pos, GameObject* obj);
	bool RemoveObjectFromTile(GameObject* obj);
	void ClearTile(AI::Vec2D pos);

	int GetNrOfTiles() const;
	int GetHeight() const;
	int GetWidth() const;

	GameObject* GetObjectOnTile(AI::Vec2D pos, Type type) const;
	GameObject* GetObjectOnTile(int x, int z, Type type) const;

	std::vector<GameObject*> GetAllObjectsOnTile(AI::Vec2D tileCoords) const;
	
	bool IsValid(AI::Vec2D pos) const;
	bool IsValid(int x, int z) const;

	bool IsPlaceable(int x, int z, Type type) const;
	bool IsPlaceable(AI::Vec2D pos, Type type) const;
	bool CanPlaceObject(AI::Vec2D pos) const;
	bool CanPlaceObject(int x, int z) const;

	bool IsArchitectureOnTile(int x, int z)const;
	bool IsArchitectureOnTile(AI::Vec2D pos)const;
	bool IsWallOnTile(int x, int z)const;
	bool IsWallOnTile(AI::Vec2D pos)const;
	bool IsFurnitureOnTile(int x, int z)const;
	bool IsFurnitureOnTile(AI::Vec2D pos)const;
	bool IsFloorOnTile(int x, int z)const;
	bool IsFloorOnTile(AI::Vec2D pos)const;
	int UnitsOnTile(int x, int z)const;
	int UnitsOnTile(AI::Vec2D pos)const;
	bool IsGuardOnTile(int x, int z)const;
	bool IsGuardOnTile(AI::Vec2D pos)const;
	bool IsEnemyOnTile(int x, int z)const;
	bool IsEnemyOnTile(AI::Vec2D pos)const;
	bool IsTrapOnTile(int x, int z)const;
	bool IsTrapOnTile(AI::Vec2D pos)const;
	bool IsObjectiveOnTile(int x, int z)const;
	bool IsObjectiveOnTile(AI::Vec2D pos)const;
	bool IsSpawnOnTile(int x, int z)const;
	bool IsSpawnOnTile(AI::Vec2D pos)const;
	bool IsTypeOnTile(int x, int z, Type type)const;
	bool IsTypeOnTile(AI::Vec2D pos, Type type)const;
	bool IsTileVisible(int x, int z) const;
	bool IsTileVisible(AI::Vec2D pos) const;
	bool IsTileEmpty(int x, int z) const;
	bool IsTileEmpty(AI::Vec2D pos) const;
};