#pragma once
#include <vector>
#include "GameObject.h"

/*
ObjectHandler
Used for updating gamelogic and rendering.

Stores pointers of all GameObjects in a std::vector

Handles GameObjects
Handles interaction between objects

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

	/*
	Take objects or shapes and check collision
	between them i.e. guards vision cone against thiefs hitbox
	*/
	bool Collision(/*stuff*/);
	
	ObjectHandler GetAll(Type type);

	// Probably unnecessary
	int GetIndex(GameObject* gameObject);

public:
	ObjectHandler();
	~ObjectHandler();

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

