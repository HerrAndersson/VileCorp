#pragma once
#include <vector>
#include "GameObject.h"
#include "Unit.h"
#include "Trap.h"
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

// TODO: Objecthandler bör hantera rendering
// TODO: Söka via ID behövs antagligen inte

struct RenderList
{
	RenderObject* _renderObject;
	vector<XMMATRIX> modelMatrices;
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
	ObjectHandler() {}; // TODO: Fix - Zache
	ObjectHandler(ID3D11Device* device);
	~ObjectHandler();

	int GetSize() const;

	GameObject* Add(Type type, int renderObjectID, XMFLOAT3 position);
	bool Remove(short ID);
	void Clear();
	 
	GameObject* Find(short ID); // TODO: Söka via ID behövs antagligen inte
	GameObject* Find(int index);
	vector<GameObject*> GetAll(Type type);
	RenderList GetAll(int renderObjectID);


	Tilemap* GetTileMap() const;
	void SetTileMap(Tilemap* tilemap);

	bool LoadLevel(string filename);


	//Update gamelogic of all objects
	void Update();
	//Relase all object resources
	void Release();
};

