#pragma once
#include <vector>
#include "GameObject.h"

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

class ObjectHandler
{
private:
	int _size;
	std::vector<GameObject*> _gameObjects;
	
	ObjectHandler GetAll(Type type);

public:
	ObjectHandler();
	~ObjectHandler();

	ObjectHandler& operator+=(const ObjectHandler& rhs);

	int GetSize() const;

	void Add(GameObject* gameObject);
	bool Remove(short ID);
	void Clear();

	GameObject* Find(short ID);
	GameObject* Find(int index);

	std::vector<RenderObject*> GetRenderObjects() const;

	//Update gamelogic of all objects
	void Update();
	//Relase all object resources
	void Release();
};

