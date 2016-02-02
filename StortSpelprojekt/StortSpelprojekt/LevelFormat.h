#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <cereal\archives\binary.hpp>
#include <cereal\archives\json.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\string.hpp>

struct LevelFormat
{
	int _levelVersion = 26;
	int _tileMapHeight = 0;
	int _tileMapWidth = 0;
	std::string _modelPath = "";
	std::string _texturePath = "";
	std::vector<std::string> _modelReferences = std::vector<std::string>();
	std::vector<std::string> _textureReferences = std::vector<std::string>();
	std::vector<std::vector<int>> _gameObjectData = std::vector<std::vector<int>>();

	template<class A>
	void serialize(A& a)
	{
		a(
			CEREAL_NVP(_levelVersion),
			CEREAL_NVP(_tileMapHeight),
			CEREAL_NVP(_tileMapWidth),
			CEREAL_NVP(_modelPath),
			CEREAL_NVP(_texturePath),
			CEREAL_NVP(_modelReferences),
			CEREAL_NVP(_textureReferences),
			CEREAL_NVP(_gameObjectData)
			);
	}
};