#include "Blueprints.h"

Blueprints::Blueprints()
{
	std::map<std::string, System::Type> stringTypes =
	{
		{ "FLOOR", System::FLOOR },
		{ "WALL", System::WALL },
		{ "LOOT", System::LOOT },
		{ "SPAWN", System::SPAWN },
		{ "TRAP", System::TRAP },
		{ "CAMERA", System::CAMERA },
		{ "GUARD", System::GUARD },
		{ "ENEMY", System::ENEMY },
		{ "FURNITURE", System::FURNITURE }
	};
	std::ifstream infile;
	infile.open(System::BLUEPRINTS_PATH);
	cereal::JSONInputArchive blueprintsIn(infile);
	BlueprintList blueprintList;
	blueprintsIn(blueprintList);
	infile.close();

	for (BlueprintData indata : blueprintList._blueprints)
	{
		System::Blueprint blueprint;
		blueprint._mesh = indata._mesh;
		blueprint._name = indata._name;
		blueprint._subType = indata._subType;
		blueprint._type = stringTypes[indata._type];
		blueprint._textures = indata._textures;
		blueprint._thumbnails = indata._thumbnails;
		blueprint._tooltip = indata._tooltip;
		_blueprintsByName.push_back(blueprint);
	}

	_blueprintsByType.resize(System::NR_OF_TYPES);

	for (int i = 0; i < _blueprintsByName.size(); i++)
	{
		System::Blueprint* blueprint = &_blueprintsByName[i];
		_blueprintsByType[blueprint->_type].push_back(blueprint);
	}
}

Blueprints::~Blueprints()
{

}

std::vector<System::Blueprint>* Blueprints::GetBlueprints()
{
	return &_blueprintsByName;
}

System::Blueprint* Blueprints::GetBlueprintByName(std::string name)
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

System::Blueprint* Blueprints::GetBlueprintByType(int type, int subType /*= 0*/)
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
