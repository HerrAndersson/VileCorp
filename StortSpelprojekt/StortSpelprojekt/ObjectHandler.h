#pragma once
#include <vector>
#include "GameObject.h"
#include "Unit.h"
#include "Trap.h"
#include "Architecture.h"
#include "Trigger.h"
#include "Tilemap.h"
#include "AssetManager.h"

/*
ObjectHandler
Used for updating gamelogic and rendering.

Stores pointers of all GameObjects in a std::vector

Handles GameObjects

Can add and remove GameObject pointers from the std::vector
Finds objects based on their objectID or vector index.
Finds an object and return its vector index
Returns all objects of a certain Type (i.e. Traps) as a seperate objectHandler
*/


//A struct containing 1 renderobject and all the gameobjects' matrices that uses said renderobject
struct RenderList
{
	RenderObject* _renderObject;
	vector<XMMATRIX> _modelMatrices;
};

class ObjectHandler
{
private:
	int _size;
	std::vector<GameObject*> _gameObjects;
	short _idCounter;
	Tilemap* _tilemap;

	AssetManager* _assetManager;

public:
	ObjectHandler(ID3D11Device* device, AssetManager* assetManager);
	~ObjectHandler();

	int GetSize() const;

	//Add a gameobject
	GameObject* Add(Type type, int renderObjectID, XMFLOAT3 position, XMFLOAT3 rotation);
	bool Remove(short ID);
	void Clear();
	

	GameObject* Find(short ID);
	GameObject* Find(int index);
	//Returns a vector containing all gameobjects with the same type
	vector<GameObject*> GetAll(Type type);
	//Returns a list of a renderobject and matrices for all objects using the renderobject
	RenderList GetAll(int renderObjectID);
	std::vector<GameObject*>* GetGameObjects();

	Tilemap* GetTileMap() const;
	void SetTileMap(Tilemap* tilemap);

	bool LoadLevel(int lvlIndex);

	void InitPathfinding();

	//Update gamelogic of all objects
	void Update();
	//Relase all object resources
	void Release();
};

