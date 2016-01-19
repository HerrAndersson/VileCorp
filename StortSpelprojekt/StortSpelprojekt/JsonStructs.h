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

struct GameObjectFloorInfo
{
	std::string _name = "";
	std::string _defaultRenderObject = "";
	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name)),
			a(CEREAL_NVP(_defaultRenderObject));
	}
};

struct GameObjectWallInfo
{
	std::string _name = "";
	std::string _defaultRenderObject = "";

	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name)),
		a(CEREAL_NVP(_defaultRenderObject));
	}
};

struct GameObjectLootInfo
{
	std::string _name = "";
	std::string _defaultRenderObject = "";
	int _radius = 50;
	int _value = 100;
	bool _steal = true;
	bool _destroy = false;

	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name)),
		a(CEREAL_NVP(_defaultRenderObject)),
		a(CEREAL_NVP(_radius)),
		a(CEREAL_NVP(_value)),
		a(CEREAL_NVP(_steal)),
		a(CEREAL_NVP(_destroy));
	}
};

struct GameObjectTrapInfo
{
	std::string _name = "";
	std::string _defaultRenderObject = "";
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
		a(CEREAL_NVP(_defaultRenderObject)),
		a(CEREAL_NVP(_type)),
		a(CEREAL_NVP(_cost)),
		a(CEREAL_NVP(_speed)),
		a(CEREAL_NVP(_hitpoints)),
		a(CEREAL_NVP(_damage)),
		a(CEREAL_NVP(_radius));
	}
};

struct GameObjectTriggerInfo
{
	std::string _name = "";
	std::string _defaultRenderObject = "";

	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name)),
		a(CEREAL_NVP(_defaultRenderObject));
	}
};

struct GameObjectGuardInfo
{
	std::string _name = "guard_proto";
	std::string _defaultRenderObject = "basemanBodyAnimation";
	int _cost = 10;
	int _speed = 10;
	int _hitpoints = 10;
	int _damage = 10;
	int _radius = 10;

	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name)),
		a(CEREAL_NVP(_defaultRenderObject)),
		a(CEREAL_NVP(_cost)),
		a(CEREAL_NVP(_speed)),
		a(CEREAL_NVP(_hitpoints)),
		a(CEREAL_NVP(_damage)),
		a(CEREAL_NVP(_radius));
	}
};

struct GameObjectEnemyInfo
{
	std::string _name = "enemy_proto";
	std::string _defaultRenderObject = "basemanBodyAnimation";
	int _cost = 10;
	int _speed = 10;
	int _hitpoints = 10;
	int _damage = 10;
	int _radius = 10;

	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name)),
		a(CEREAL_NVP(_defaultRenderObject)),
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
	std::vector<GameObjectFloorInfo> _floors;
	std::vector<GameObjectWallInfo> _walls;
	std::vector<GameObjectLootInfo> _loot;
	std::vector<GameObjectTrapInfo> _traps;
	std::vector<GameObjectTriggerInfo> _triggers;
	std::vector<GameObjectGuardInfo> _guards;
	std::vector<GameObjectEnemyInfo> _enemies;
};