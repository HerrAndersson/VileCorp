#pragma once
#include <cereal/cereal.hpp>

struct PlayerInfo
{
	std::string _name = "Jonas";
	int _gold = 100;
	int _level = 0;




	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name)),
			a(CEREAL_NVP(_gold)),
			a(CEREAL_NVP(_level));
	}
};

/*struct GameObjectSpawnInfo
{
	std::string _name = "";

	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name));
	}
};*/

struct GameObjectBaseInfo
{
	std::string _name = "proto";
	RenderObject* _renderObject;
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
//	std::vector<GameObjectSpawnInfo> _spawns;
	std::vector<std::vector<GameObjectBaseInfo*>*> _objects;
	GameObjectInfo()
	{
		_objects.resize(7);
		_objects[0] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectFloorInfo*>;
		_objects[1] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectWallInfo*>;
		_objects[2] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectLootInfo*>;
		_objects[3] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectTrapInfo*>;
		_objects[4] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectTriggerInfo*>;
		_objects[5] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectGuardInfo*>;
		_objects[6] = (std::vector<GameObjectBaseInfo*>*)new std::vector<GameObjectEnemyInfo*>;
	}
	GameObjectFloorInfo* Floors(int i)
	{
		return (GameObjectFloorInfo*)_objects[0]->at(i);
	}
	GameObjectWallInfo* Walls(int i)
	{
		return (GameObjectWallInfo*)_objects[1]->at(i);
	}
	GameObjectLootInfo* Loot(int i)
	{
		return (GameObjectLootInfo*)_objects[2]->at(i);
	}
	GameObjectTrapInfo* Traps(int i)
	{
		return (GameObjectTrapInfo*)_objects[3]->at(i);
	}
	GameObjectTriggerInfo* Triggers(int i)
	{
		return (GameObjectTriggerInfo*)_objects[4]->at(i);
	}
	GameObjectGuardInfo* Guards(int i)
	{
		return (GameObjectGuardInfo*)_objects[5]->at(i);
	}
	GameObjectEnemyInfo* Enemies(int i)
	{
		return (GameObjectEnemyInfo*)_objects[6]->at(i);
	}
};