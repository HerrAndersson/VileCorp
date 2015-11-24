#include "ObjectHandler.h"

ObjectHandler ObjectHandler::GetAll(Type type)
{
	ObjectHandler objectsOfSameType;

	for (int i = 0; i < _size; i++)
	{
		if (_gameObjects.at(i)->GetType() == type)
		{
			objectsOfSameType.Add(_gameObjects[i]);
		}
	}

	return objectsOfSameType;
}

ObjectHandler::ObjectHandler()
{
	_size = 0;
}

ObjectHandler::~ObjectHandler()
{}

ObjectHandler& ObjectHandler::operator+=(const ObjectHandler& rhs)
{
	for (int i = 0; i < rhs.GetSize(); i++)
	{
		this->Add(rhs._gameObjects[i]);
	}
	return *this;
}

int ObjectHandler::GetSize() const
{
	return _size;
}



bool ObjectHandler::Add(GameObject * gameObject)
{
	bool exist = false;

	for (int i = 0; i < _size && !exist; i++)
	{
		if (_gameObjects[i] == gameObject)
		{
			exist = true;
		}
	}

	if (!exist)
	{
		_gameObjects.push_back(gameObject);
		_size++;
	}

	return exist;
}

bool ObjectHandler::Remove(short ID)
{
	bool removed = false;

	for (int i = 0; i < _size && !removed; i++)
	{
		if (_gameObjects[i]->GetID() == ID)
		{
			// Release object resource
			_gameObjects[i]->Release();

			// Decrease size of vector
			_size--;

			// Replace pointer with the last pointer int the vector
			_gameObjects[i] = _gameObjects[_size];

			// Remove pointer value to avoid various problems
			_gameObjects.pop_back();
			
			removed = true;
		}
	}

	return removed;
}

void ObjectHandler::Clear()
{
	_gameObjects.clear();
}

GameObject* ObjectHandler::Find(short ID)
{
	GameObject* gameObject = nullptr;
	bool found = false;

	for (int i = 0; i < _size && !found; i++)
	{
		if (_gameObjects[i]->GetID() == ID)
		{
			gameObject = _gameObjects[i];
			found = true;
		}
	}

	return gameObject;
}

GameObject* ObjectHandler::Find(int index)
{
	GameObject* gameObject = nullptr;
	if (index >= 0 && index < _size)
	{
		gameObject = _gameObjects[index];
	}
	
	return gameObject;
}

std::vector<RenderObject*> ObjectHandler::GetRenderObjects() const
{
	std::vector<RenderObject*> renderObjects;

	for (int i = 0; i < _size; i++)
	{
		//If the object has a Renderobject and is visible
		if (_gameObjects[i]->GetRenderObject() != nullptr && _gameObjects[i]->IsVisible())
		{
			renderObjects.push_back(_gameObjects[i]->GetRenderObject());
		}
	}

	return renderObjects;
}

Tilemap * ObjectHandler::GetTileMap() const
{
	return _tilemap;
}

void ObjectHandler::SetTileMap(Tilemap * tilemap)
{
	_tilemap = tilemap;
}

void ObjectHandler::Update()
{
	//Update all objects gamelogic
	for (int i = 0; i < _size; i++)
	{
		_gameObjects[i]->Update();
	}

	//TODO check for relevant collision between objects using GetAll(Type) (i.e All thiefs vs all traps (some culling may be required))


}

void ObjectHandler::Release()
{
	for (int i = 0; i < _size; i++)
	{
		_gameObjects[i]->Release();
	}
}
