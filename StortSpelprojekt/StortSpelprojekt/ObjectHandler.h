#pragma once
#include <vector>
#include "RenderModule.h"

#include "GameObject.h"
#include "Tilemap.h"

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



class ObjectHandler
{
private:
	int _size;
	std::vector<GameObject*> _gameObjects;
	Tilemap* _tilemap;
	
	ObjectHandler GetAll(Type type);

	Renderer::RenderModule* _renderModule;

public:
	ObjectHandler() {}; // TODO: Fix - Zache
	ObjectHandler(Renderer::RenderModule* renderModule);
	~ObjectHandler();

	ObjectHandler& operator+=(const ObjectHandler& rhs);

	int GetSize() const;

	bool Add(GameObject* gameObject);
	bool Remove(short ID);
	void Clear();
	 
	GameObject* Find(short ID); // TODO: Söka via ID behövs antagligen inte
	GameObject* Find(int index);

	Tilemap* GetTileMap() const;
	void SetTileMap(Tilemap* tilemap);


	//Update gamelogic of all objects
	void Update();
	//Relase all object resources
	void Release();
};

