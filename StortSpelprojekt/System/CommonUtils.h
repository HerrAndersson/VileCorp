#pragma once
#include <string>
#include <vector>

enum Type
{
	FLOOR, WALL, LOOT, SPAWN, TRAP, CAMERA, GUARD, ENEMY, FURNITURE, NR_OF_TYPES/*Has to be last*/
};

const std::string MODEL_FOLDER_PATH = "Assets/Models/";
const std::wstring TEXTURE_FOLDER_PATH_W = L"Assets/Textures/";
const std::string LEVEL_FOLDER_PATH = "Assets/Levels/Format12/";
const std::string ANIMATION_FOLDER_PATH = "Assets/Animations/";
const std::string BLUEPRINTS_PATH = "Assets/blueprints.json";

struct Blueprint
{
	std::string _name, _tooltip, _mesh;
	Type _type;
	int _subType;
	std::vector<std::string> _textures, _thumbnails;
};

inline std::string WStringToString(std::wstring ws)
{
	std::string s(ws.begin(), ws.end());
	return s;
}