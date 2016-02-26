#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include "RenderUtils.h"
#include "CommonUtils.h"

class Blueprints
{
private:
	struct BlueprintData
	{
		std::string _name, _tooltip, _mesh;
		int _type, _subType;
		std::vector<std::string> _textures, _thumbnails;
		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(_mesh),
					CEREAL_NVP(_name),
					CEREAL_NVP(_type),
					CEREAL_NVP(_subType),
					CEREAL_NVP(_tooltip),
					CEREAL_NVP(_textures),
					CEREAL_NVP(_thumbnails));
		}
	};

	struct BlueprintList
	{
		std::string _name;
		int _levelformatversion;
		std::vector<BlueprintData> _blueprints;
		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(_name),
					CEREAL_NVP(_levelformatversion),
					CEREAL_NVP(_blueprints));
		}
	};

	std::vector<System::Blueprint> _blueprintsByName;
	std::vector<std::vector<System::Blueprint*>> _blueprintsByType;

public:
	Blueprints();
	~Blueprints();

	std::vector<System::Blueprint>* GetBlueprints();
	System::Blueprint* GetBlueprintByName(std::string name);
	System::Blueprint* GetBlueprintByType(int type, int subType = 0);

};