//Handles the walkable and non-walkable tiles
//Aron and Victor worked on this

#pragma once

#include "GameObject.h"
#include "Architecture.h"

class Tilemap
{
private:
	struct Tile
	{
		static const int OBJECT_CAPACITY = 6;
		std::vector<GameObject*> _objectsOnTile;					//0 = floor or wall, 1 = enemy, 2 = guard, 3 = trap or spawnpoint, 4 = furniture, 5 = thief objectives 
		bool _isVisible;
		bool _lock;
		Tile()
		{
			_objectsOnTile.resize(OBJECT_CAPACITY);
			for (int i = 0; i < OBJECT_CAPACITY; i++)
			{
				_objectsOnTile[i] = nullptr;
			}
			_isVisible = false;
			_lock = false;
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

	int _nrOfLoot;			//Note: This is the amount of loot on the tilemap. Does not count held objects.
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

	void LockTile(AI::Vec2D pos);
	void UnlockTile(AI::Vec2D pos);

	int GetNrOfTiles() const;
	int GetHeight() const;
	int GetWidth() const;
	int GetNrOfLoot()const;

	GameObject* GetObjectOnTile(AI::Vec2D pos, System::Type type) const;
	GameObject* GetObjectOnTile(int x, int z, System::Type type) const;

	std::vector<GameObject*>* GetAllObjectsOnTile(AI::Vec2D tileCoords) const;
	std::vector<GameObject*>* GetAllObjectsOnTile(int xCoord, int yCoord) const;

	bool IsValid(AI::Vec2D pos) const;
	bool IsValid(int x, int z) const;

	bool IsPlaceable(int x, int z, System::Type type) const;
	bool IsPlaceable(AI::Vec2D pos, System::Type type) const;

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
	bool IsTypeOnTile(int x, int z, System::Type type)const;
	bool IsTypeOnTile(AI::Vec2D pos, System::Type type)const;
	bool IsTileVisible(int x, int z) const;
	bool IsTileVisible(AI::Vec2D pos) const;
	bool IsTileEmpty(int x, int z) const;
	bool IsTileEmpty(AI::Vec2D pos) const;
	bool IsTileNoPlacementZone(int x, int z) const;
	bool IsTileNoPlacementZone(AI::Vec2D pos) const;
};