#pragma once
#include <string>
#include <vector>

struct Blueprint
{
	std::string _name, _tooltip, _mesh;
	int _type, _subType;
	std::vector<std::string> _textures, _thumbnails;
};