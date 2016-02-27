#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cereal\archives\binary.hpp>
#include <cereal\archives\json.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\map.hpp>

namespace Level
{
	enum GameModes { KILL_THEM_ALL, SURVIVAL };
	static const char * GameModeNames[] = { "Kill them all", "Survival" };

	struct LevelHeader
	{
		std::string _storyTitle = "";
		std::string _storyBody = "";
		int _budget = 0;
		int _gameMode = 0;
		int _surviveForSeconds = 0; //Specifically for the survival gamemode 
		std::string _levelBinaryFilename = "";

		template<class A>
		void serialize(A& a)
		{
			a(
				CEREAL_NVP(_storyTitle),
				CEREAL_NVP(_storyBody),
				CEREAL_NVP(_budget),
				CEREAL_NVP(_gameMode),
				CEREAL_NVP(_surviveForSeconds),
				CEREAL_NVP(_levelBinaryFilename)
				);
		}
	};

	struct LevelBinary
	{
		int _tileMapSizeX = 0;
		int _tileMapSizeZ = 0;
		std::vector<std::vector<int>> _gameObjectData = std::vector<std::vector<int>>();
		std::vector<std::vector<int>> _enemyWavesData = std::vector<std::vector<int>>();
		std::map<int, int> _enemySpawnMap = std::map<int, int>();

		template<class A>
		void serialize(A& a)
		{
			a(
				CEREAL_NVP(_tileMapSizeX),
				CEREAL_NVP(_tileMapSizeZ),
				CEREAL_NVP(_gameObjectData),
				CEREAL_NVP(_enemyWavesData),
				CEREAL_NVP(_enemySpawnMap)
				);
		}
	};
}