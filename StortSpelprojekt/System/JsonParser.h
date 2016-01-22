#pragma once
#include <fstream>
#include <cereal/archives/json.hpp>

namespace System
{
	template<class T> void loadJSON(T data, std::string path)
	{
		std::ifstream inStream(path);
		if (inStream.is_open())
		{
			cereal::JSONInputArchive inArchive(inStream);
			inArchive(*data);
		}
	}

	//Error C2100: Illegal Indirection means you passed a nonpointer in data
	template<class T> void saveJSON(T data, std::string path, std::string title)
	{
		std::ofstream outStream(path);
		cereal::JSONOutputArchive outArchive(outStream);
		outArchive.setNextName(title.c_str());
		outArchive(*data);
	}
}