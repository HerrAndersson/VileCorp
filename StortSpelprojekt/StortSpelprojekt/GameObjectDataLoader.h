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
	std::map< System::Type, std::string> typeStrings =
	{
		{System::FLOOR, "Floors/" },
		{System::WALL, "Walls/" },
		{System::LOOT, "Loot/" },
		{System::SPAWN, "Spawns/" },
		{System::TRAP, "Traps/" },
		{System::GUARD, "Guards/" },
		{System::ENEMY, "Enemies/" },
		{System::CAMERA, "Cameras/" },
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
	for (uint a = 0; a <  System::Type::NR_OF_TYPES; a++)
	{
		string folder = "Assets/Gameobjects/";
		folder.append(typeStrings[(System::Type)a]);
		GetFilenamesInDirectory((char*)folder.c_str(), ".json", filenames);
		for (uint i = 0; i < filenames.size(); i++)
		{
			switch (a)
			{
			case  System::FLOOR:
				System::loadJSON(data->Floors(i), filenames[i]);
				break;
			case  System::WALL:
				System::loadJSON(data->Walls(i), filenames[i]);
				break;
			case  System::LOOT:
				System::loadJSON(data->Loot(i), filenames[i]);
				break;
			case  System::SPAWN:
				System::loadJSON(data->Spawns(i), filenames[i]);
				break;
			case  System::TRAP:
				System::loadJSON(data->Traps(i), filenames[i]);
				break;
			case  System::CAMERA:
				System::loadJSON(data->Cameras(i), filenames[i]);
				break;
			case  System::GUARD:
				System::loadJSON(data->Guards(i), filenames[i]);
				break;
			case  System::ENEMY:
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
		GameObjectGuardInfo sample;
		System::saveJSON(&sample, "Assets/Gameobjects/Guards/sample.json", "sample");
	}
	{
		GameObjectEnemyInfo sample;
		System::saveJSON(&sample, "Assets/Gameobjects/Enemies/sample.json", "sample");
	}
};