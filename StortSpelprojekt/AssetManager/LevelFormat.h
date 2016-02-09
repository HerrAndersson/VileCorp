#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <cereal\archives\binary.hpp>
#include <cereal\archives\xml.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\string.hpp>

struct LevelFormat
{
	int _levelVersion = 26;
	int _tileMapHeight = 0;
	int _tileMapWidth = 0;
	std::vector<std::vector<int>> _gameObjectData = std::vector<std::vector<int>>();

	template<class A>
	void serialize(A& a)
	{
		a(
			CEREAL_NVP(_levelVersion),
			CEREAL_NVP(_tileMapHeight),
			CEREAL_NVP(_tileMapWidth),
			CEREAL_NVP(_gameObjectData)
			);
	}
};