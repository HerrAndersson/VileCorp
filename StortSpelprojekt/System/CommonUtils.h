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
const std::string CAMPAIGN_PATH = LEVEL_FOLDER_PATH + "Campaign/";
const std::string ANIMATION_FOLDER_PATH = "Assets/Animations/";
const std::string BLUEPRINTS_PATH = "Assets/blueprints.json";

struct Blueprint
{
	std::string _name, _tooltip, _mesh;
	Type _type;
	int _subType;
	std::vector<std::string> _textures, _thumbnails;
};

struct Hitbox
{
	float _center[3], _height, _width, _depth;
};

inline std::string WStringToString(std::wstring ws)
{
	std::string s(ws.begin(), ws.end());
	return s;
}

inline std::wstring StringToWstring(std::string s)
{
	std::wstring ws(s.begin(), s.end());
	return ws;
}