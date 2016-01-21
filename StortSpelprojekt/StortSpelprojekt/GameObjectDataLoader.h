#pragma once
#include "JsonStructs.h"
#include "JsonParser.h"

class GameObjectDataLoader
{
public:
	GameObjectDataLoader();
	~GameObjectDataLoader();

	bool LoadGameObjectInfo(GameObjectInfo* data);
	void WriteSampleGameObjects();
	std::map<Type, std::string> typeStrings =
	{
		{ FLOOR, "Floors/" },
		{ WALL, "Walls/" },
		{ LOOT, "Loot/" },
		{ SPAWN, "Spawns/" },
		{ TRAP, "Traps/" },
		{ TRIGGER, "Triggers/" },
		{ GUARD, "Guards/" },
		{ ENEMY, "Enemies/" },
	};
};

inline GameObjectDataLoader::GameObjectDataLoader()
{
};

inline GameObjectDataLoader::~GameObjectDataLoader()
{
};

inline bool GameObjectDataLoader::LoadGameObjectInfo(GameObjectInfo* data)
{
	vector<string> filenames;
	for (uint a = 0; a < Type::NR_OF_TYPES; a++)
	{
		string folder = "Assets/Gameobjects/";
		folder.append(typeStrings[(Type)a]);
		GetFilenamesInDirectory((char*)folder.c_str(), ".json", filenames);
		for (uint i = 0; i < filenames.size(); i++)
		{
			switch (a)
			{
			case FLOOR:
				System::loadJSON(data->Floors(i), filenames[i]);
				break;
			case WALL:
				System::loadJSON(data->Walls(i), filenames[i]);
				break;
			case LOOT:
				System::loadJSON(data->Loot(i), filenames[i]);
				break;
			case SPAWN:
				System::loadJSON(data->Spawns(i), filenames[i]);
				break;
			case TRAP:
				System::loadJSON(data->Traps(i), filenames[i]);
				break;
			case TRIGGER:
				System::loadJSON(data->Triggers(i), filenames[i]);
				break;
			case GUARD:
				System::loadJSON(data->Guards(i), filenames[i]);
				break;
			case ENEMY:
				System::loadJSON(data->Enemies(i), filenames[i]);
				break;
			}
		}
		filenames.clear();
	}
	return false;
}

inline void GameObjectDataLoader::WriteSampleGameObjects()
{
	{
		GameObjectFloorInfo sample;
		System::saveJSON(&sample, "Assets/Gameobjects/Floors/sample.json", "sample");
	}
	{
		GameObjectWallInfo sample;
		System::saveJSON(&sample, "Assets/Gameobjects/Walls/sample.json", "sample");
	}
	{
		GameObjectLootInfo sample;
		System::saveJSON(&sample, "Assets/Gameobjects/Loot/sample.json", "sample");
	}
	{
		GameObjectSpawnInfo sample;
		System::saveJSON(&sample, "Assets/Gameobjects/Spawns/sample.json", "sample");
	}
	{
		GameObjectTrapInfo sample;
		System::saveJSON(&sample, "Assets/Gameobjects/Traps/sample.json", "sample");
	}
	{
		GameObjectTriggerInfo sample;
		System::saveJSON(&sample, "Assets/Gameobjects/Triggers/sample.json", "sample");
	}
	{
		GameObjectGuardInfo sample;
		System::saveJSON(&sample, "Assets/Gameobjects/Guards/sample.json", "sample");
	}
	{
		GameObjectEnemyInfo sample;
		System::saveJSON(&sample, "Assets/Gameobjects/Enemies/sample.json", "sample");
	}
};