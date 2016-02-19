#include "Blueprints.h"

Blueprints::Blueprints()
{
	std::map<std::string, Type> stringTypes =
	{
		{ "FLOOR", FLOOR },
		{ "WALL", WALL },
		{ "LOOT", LOOT },
		{ "SPAWN", SPAWN },
		{ "TRAP", TRAP },
		{ "CAMERA", CAMERA },
		{ "GUARD", GUARD },
		{ "ENEMY", ENEMY },
		{ "FURNITURE", FURNITURE }
	};
	std::ifstream infile;
	infile.open(BLUEPRINTS_PATH);
	cereal::JSONInputArchive blueprintsIn(infile);
	BlueprintList blueprintList;
	blueprintsIn(blueprintList);
	infile.close();

	for (BlueprintData indata : blueprintList._blueprints)
	{
		Blueprint blueprint;
		blueprint._mesh = indata._mesh;
		blueprint._name = indata._name;
		blueprint._subType = indata._subType;
		blueprint._type = stringTypes[indata._type];
		blueprint._textures = indata._textures;
		blueprint._thumbnails = indata._thumbnails;
		blueprint._tooltip = indata._tooltip;
		_blueprintsByName.push_back(blueprint);
	}

	_blueprintsByType.resize(NR_OF_TYPES);

	for (int i = 0; i < _blueprintsByName.size(); i++)
	{
		Blueprint* blueprint = &_blueprintsByName[i];
		_blueprintsByType[blueprint->_type].push_back(blueprint);
	}
}

Blueprints::~Blueprints()
{

}

std::vector<Blueprint>* Blueprints::GetBlueprints()
{
	return &_blueprintsByName;
}

Blueprint* Blueprints::GetBlueprintByName(std::string name)
{
	for (int i = 0; i < _blueprintsByName.size(); i++)
	{
		if (_blueprintsByName[i]._name == name)
		{
			return &_blueprintsByName[i];
		}
	}
	return nullptr;
}

Blueprint* Blueprints::GetBlueprintByType(int type, int subType /*= 0*/)
{
	if (type <= _blueprintsByType.size() - 1 && type >= 0)
	{
		if (subType <= _blueprintsByType[type].size() - 1 && subType >= 0)
		{
			return _blueprintsByType[type][subType];
		}
	}
	return nullptr;
}
