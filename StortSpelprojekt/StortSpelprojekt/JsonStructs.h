#pragma once
#include <cereal/cereal.hpp>
#include <cereal\types\vector.hpp>

#include "GameObject.h"

struct LevelLoad
{
	int level = 1;
	int editLevel = 1;

	template<class A>
	void serialize(A& a)
	{
		a((CEREAL_NVP(level)),
			(CEREAL_NVP(editLevel)));
	}
};

struct SettingInfo
{
	int _resX = 1920;
	int _resY = 1080;
	int _shadowResX = 256;
	int _shadowResY = 256;
	bool _default = true;
	bool _shadowmap = true;
	bool _fullScreen = true;
	bool _debugMode = false;
	bool _bordeless = false;
	bool _showMouseCursor = true;

	template<class A>
	void serialize(A& a)
	{
		a((CEREAL_NVP(_default)),
		(CEREAL_NVP(_resX)),
		(CEREAL_NVP(_resY)),
		(CEREAL_NVP(_shadowResX)),
		(CEREAL_NVP(_shadowResY)),
		(CEREAL_NVP(_shadowmap)),
		(CEREAL_NVP(_fullScreen)),
		(CEREAL_NVP(_debugMode)),
		(CEREAL_NVP(_bordeless)),
		(CEREAL_NVP(_showMouseCursor)));
	}
};

/*

1. Inkludera JsonStructs.h där du behöver den.
2. Skapa en struct lik PlayerInfo i den här .h filel enligt ovan med den data du vill skriva till fil.
3. Skapa ett objekt av din datatyp 
4. Använd sedan i .cpp enligt följande mall:
	
	System::saveJSON(&_dataStruct, "filePath/test.json", "dataStruct Name");
	System::loadJSON(&_dataStruct, "filePath/test.json");

	_dataStruc är din struct

	"filePath/test.json" är din filepath

	"dataStruct Name" är vad containern i filen kommer att heta
*/

struct GameObjectBaseInfo
{
	std::string _name = "proto";
	unsigned int _renderObject;
	std::string _textureName;
	virtual ~GameObjectBaseInfo(){}
};

struct GameObjectFloorInfo : GameObjectBaseInfo
{
	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name)),
		a(CEREAL_NVP(_textureName));
	}
	virtual ~GameObjectFloorInfo()
	{}
};

struct GameObjectWallInfo : GameObjectBaseInfo
{
	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name)),
		a(CEREAL_NVP(_textureName));
	}

};

struct GameObjectLootInfo : GameObjectBaseInfo
{
	int _radius = 50;
	int _value = 100;
	bool _steal = true;
	bool _destroy = false;

	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name)),
			a(CEREAL_NVP(_radius)),
			a(CEREAL_NVP(_value)),
			a(CEREAL_NVP(_steal)),
			a(CEREAL_NVP(_destroy)),
			a(CEREAL_NVP(_textureName));
	}
};

struct GameObjectSpawnInfo : GameObjectBaseInfo
{

	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name));
	}
};

struct GameObjectCameraInfo : GameObjectBaseInfo
{

	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name));
	}
};

struct GameObjectFurnitureInfo : GameObjectBaseInfo
{

	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name));
	}
};

struct GameObjectTrapInfo : GameObjectBaseInfo
{
	int _type = 0;
	int _cost = 40;
	int _hitpoints = 30;
	int _damage = 80;
	int _radius = 50;

	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name)),
		a(CEREAL_NVP(_type)),
		a(CEREAL_NVP(_cost)),
		a(CEREAL_NVP(_hitpoints)),
		a(CEREAL_NVP(_damage)),
		a(CEREAL_NVP(_radius));
	}
};

struct GameObjectGuardInfo : GameObjectBaseInfo
{
	int _cost = 10;
	int _speed = 10;
	int _hitpoints = 10;
	int _damage = 10;
	int _radius = 10;

	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name)),
		a(CEREAL_NVP(_cost)),
		a(CEREAL_NVP(_speed)),
		a(CEREAL_NVP(_hitpoints)),
		a(CEREAL_NVP(_damage)),
		a(CEREAL_NVP(_radius)),
		a(CEREAL_NVP(_textureName));
	}
};

struct GameObjectEnemyInfo : GameObjectBaseInfo
{
	int _cost = 10;
	int _speed = 10;
	int _hitpoints = 10;
	int _damage = 10;
	int _radius = 10;

	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name)),
		a(CEREAL_NVP(_cost)),
		a(CEREAL_NVP(_speed)),
		a(CEREAL_NVP(_hitpoints)),
		a(CEREAL_NVP(_damage)),
		a(CEREAL_NVP(_radius)),
		a(CEREAL_NVP(_textureName));
	}
};

struct GameObjectInfo
{
	std::vector<std::vector<GameObjectBaseInfo*>*> _objects;
	GameObjectInfo()
	{
		_objects.resize(System::NR_OF_TYPES);
		_objects[System::FLOOR] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectFloorInfo*>;
		_objects[System::WALL] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectWallInfo*>;
		_objects[System::LOOT] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectLootInfo*>;
		_objects[System::SPAWN] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectSpawnInfo*>;
		_objects[System::TRAP] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectTrapInfo*>;
		_objects[System::CAMERA] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectCameraInfo*>;
		_objects[System::GUARD] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectGuardInfo*>;
		_objects[System::ENEMY] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectEnemyInfo*>;
		_objects[System::FURNITURE] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectFurnitureInfo*>;
	}
	~GameObjectInfo()
	{
		for (int i = 0; i <  System::NR_OF_TYPES; i++)
		{
			for (GameObjectBaseInfo* obj : *_objects[i])
			{
				delete obj;
			}
			delete _objects[i];
		}
	}

	GameObjectFloorInfo* Floors(unsigned i)
	{
		if (_objects[System::FLOOR]->size() < i + 1)
		{
			_objects[System::FLOOR]->push_back((GameObjectBaseInfo*)new GameObjectFloorInfo());
		}
		return (GameObjectFloorInfo*)_objects[System::FLOOR]->at(i);
	}
	GameObjectWallInfo* Walls(unsigned i)
	{
		if (_objects[System::WALL]->size() < i + 1)
		{
			_objects[System::WALL]->push_back((GameObjectBaseInfo*)new GameObjectWallInfo());
		}
		return (GameObjectWallInfo*)_objects[System::WALL]->at(i);
	}
	GameObjectFurnitureInfo* Furnitures(unsigned i)
	{
		if (_objects[System::FURNITURE]->size() < i + 1)
		{
			_objects[System::FURNITURE]->push_back((GameObjectBaseInfo*)new GameObjectFurnitureInfo());
		}
		return (GameObjectFurnitureInfo*)_objects[System::FURNITURE]->at(i);
	}
	GameObjectLootInfo* Loot(unsigned i)
	{
		if (_objects[System::LOOT]->size() < i + 1)
		{
			_objects[System::LOOT]->push_back((GameObjectBaseInfo*)new GameObjectLootInfo());
		}
		return (GameObjectLootInfo*)_objects[System::LOOT]->at(i);
	}
	GameObjectSpawnInfo* Spawns(unsigned i)
	{
		if (_objects[System::SPAWN]->size() < i + 1)
		{
			_objects[System::SPAWN]->push_back((GameObjectBaseInfo*)new GameObjectSpawnInfo());
		}
		return (GameObjectSpawnInfo*)_objects[System::SPAWN]->at(i);
	}
	GameObjectTrapInfo* Traps(unsigned i)
	{
		if (_objects[System::TRAP]->size() < i + 1)
		{
			_objects[System::TRAP]->push_back((GameObjectBaseInfo*)new GameObjectTrapInfo());
		}
		return (GameObjectTrapInfo*)_objects[System::TRAP]->at(i);
	}
	GameObjectCameraInfo* Cameras(unsigned i)
	{
		if (_objects[System::CAMERA]->size() < i + 1)
		{
			_objects[System::CAMERA]->push_back((GameObjectBaseInfo*)new GameObjectCameraInfo());
		}
		return (GameObjectCameraInfo*)_objects[System::CAMERA]->at(i);
	}
	GameObjectGuardInfo* Guards(unsigned i)
	{
		if (_objects[System::GUARD]->size() < i + 1)
		{
			_objects[System::GUARD]->push_back((GameObjectBaseInfo*)new GameObjectGuardInfo());
		}
		return (GameObjectGuardInfo*)_objects[System::GUARD]->at(i);
	}
	GameObjectEnemyInfo* Enemies(unsigned i)
	{
		if (_objects[System::ENEMY]->size() < i + 1)
		{
			_objects[System::ENEMY]->push_back((GameObjectBaseInfo*)new GameObjectEnemyInfo());
		}
		return (GameObjectEnemyInfo*)_objects[System::ENEMY]->at(i);
	}
};