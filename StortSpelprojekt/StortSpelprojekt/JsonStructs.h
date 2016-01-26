#pragma once
#include <cereal/cereal.hpp>
#include <cereal\types\vector.hpp>

struct LevelLoad
{
	int level = 1;
	int editLevel = 1;

	template<class A>
	void serialize(A& a)
	{
		a((CEREAL_NVP(level)));
	}
};

struct PlayerInfo
{
	std::string _name = "Jonas";
	int _gold = 100;
	int _level = 0;
	
	template<class A>
	void serialize(A& a)
	{
		a((CEREAL_NVP(_name)),
		(CEREAL_NVP(_gold)),
		(CEREAL_NVP(_level)));
	}
};

struct SettingInfo
{
	int _resX = 1920;
	int _resY = 1080;
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
	unsigned _renderObject;
};

struct GameObjectFloorInfo : GameObjectBaseInfo
{
	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name));
	}
};

struct GameObjectWallInfo : GameObjectBaseInfo
{
	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name));
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
			a(CEREAL_NVP(_destroy));
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

struct GameObjectTrapInfo : GameObjectBaseInfo
{
	int _type = 0;
	int _cost = 40;
	int _speed = 200;
	int _hitpoints = 30;
	int _damage = 80;
	int _radius = 50;

	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name)),
		a(CEREAL_NVP(_type)),
		a(CEREAL_NVP(_cost)),
		a(CEREAL_NVP(_speed)),
		a(CEREAL_NVP(_hitpoints)),
		a(CEREAL_NVP(_damage)),
		a(CEREAL_NVP(_radius));
	}
};

struct GameObjectTriggerInfo : GameObjectBaseInfo
{
	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name));
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
		a(CEREAL_NVP(_radius));
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
		a(CEREAL_NVP(_radius));
	}
};

struct GameObjectInfo
{
	std::vector<std::vector<GameObjectBaseInfo*>*> _objects;
	GameObjectInfo()
	{
		_objects.resize(NR_OF_TYPES);
		_objects[FLOOR] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectFloorInfo*>;
		_objects[WALL] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectWallInfo*>;
		_objects[LOOT] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectLootInfo*>;
		_objects[SPAWN] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectSpawnInfo*>;
		_objects[TRAP] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectTrapInfo*>;
		_objects[TRIGGER] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectTriggerInfo*>;
		_objects[GUARD] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectGuardInfo*>;
		_objects[ENEMY] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectEnemyInfo*>;
	}
	~GameObjectInfo()
	{
		for (GameObjectBaseInfo* obj : *_objects[FLOOR])
		{
			delete (GameObjectFloorInfo*)obj;
		}
		for (GameObjectBaseInfo* obj : *_objects[WALL])
		{
			delete (GameObjectWallInfo*)obj;
		}
		for (GameObjectBaseInfo* obj : *_objects[LOOT])
		{
			delete (GameObjectLootInfo*)obj;
		}
		for (GameObjectBaseInfo* obj : *_objects[SPAWN])
		{
			delete (GameObjectSpawnInfo*)obj;
		}
		for (GameObjectBaseInfo* obj : *_objects[TRAP])
		{
			delete (GameObjectTrapInfo*)obj;
		}
		for (GameObjectBaseInfo* obj : *_objects[TRIGGER])
		{
			delete (GameObjectTriggerInfo*)obj;
		}
		for (GameObjectBaseInfo* obj : *_objects[GUARD])
		{
			delete (GameObjectGuardInfo*)obj;
		}
		for (GameObjectBaseInfo* obj : *_objects[ENEMY])
		{
			delete (GameObjectEnemyInfo*)obj;
		}
		delete _objects[FLOOR];
		delete _objects[WALL];
		delete _objects[LOOT];
		delete _objects[SPAWN];
		delete _objects[TRAP];
		delete _objects[TRIGGER];
		delete _objects[GUARD];
		delete _objects[ENEMY];
	}
	GameObjectFloorInfo* Floors(unsigned i)
	{
		if (_objects[FLOOR]->size() < i + 1)
		{
			_objects[FLOOR]->push_back((GameObjectBaseInfo*)new GameObjectFloorInfo());
		}
		return (GameObjectFloorInfo*)_objects[FLOOR]->at(i);
	}
	GameObjectWallInfo* Walls(unsigned i)
	{
		if (_objects[WALL]->size() < i + 1)
		{
			_objects[WALL]->push_back((GameObjectBaseInfo*)new GameObjectWallInfo());
		}
		return (GameObjectWallInfo*)_objects[WALL]->at(i);
	}
	GameObjectLootInfo* Loot(unsigned i)
	{
		if (_objects[LOOT]->size() < i + 1)
		{
			_objects[LOOT]->push_back((GameObjectBaseInfo*)new GameObjectLootInfo());
		}
		return (GameObjectLootInfo*)_objects[LOOT]->at(i);
	}
	GameObjectSpawnInfo* Spawns(unsigned i)
	{
		if (_objects[SPAWN]->size() < i + 1)
		{
			_objects[SPAWN]->push_back((GameObjectBaseInfo*)new GameObjectSpawnInfo());
		}
		return (GameObjectSpawnInfo*)_objects[SPAWN]->at(i);
	}
	GameObjectTrapInfo* Traps(unsigned i)
	{
		if (_objects[TRAP]->size() < i + 1)
		{
			_objects[TRAP]->push_back((GameObjectBaseInfo*)new GameObjectTrapInfo());
		}
		return (GameObjectTrapInfo*)_objects[TRAP]->at(i);
	}
	GameObjectTriggerInfo* Triggers(unsigned i)
	{
		if (_objects[TRIGGER]->size() < i + 1)
		{
			_objects[TRIGGER]->push_back((GameObjectBaseInfo*)new GameObjectTriggerInfo());
		}
		return (GameObjectTriggerInfo*)_objects[TRIGGER]->at(i);
	}
	GameObjectGuardInfo* Guards(unsigned i)
	{
		if (_objects[GUARD]->size() < i + 1)
		{
			_objects[GUARD]->push_back((GameObjectBaseInfo*)new GameObjectGuardInfo());
		}
		return (GameObjectGuardInfo*)_objects[GUARD]->at(i);
	}
	GameObjectEnemyInfo* Enemies(unsigned i)
	{
		if (_objects[ENEMY]->size() < i + 1)
		{
			_objects[ENEMY]->push_back((GameObjectBaseInfo*)new GameObjectEnemyInfo());
		}
		return (GameObjectEnemyInfo*)_objects[ENEMY]->at(i);
	}
};