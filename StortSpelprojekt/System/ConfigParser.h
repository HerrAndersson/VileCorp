#pragma once
#include <fstream>
#include <cereal/archives/json.hpp>

namespace System
{
#ifdef _DEBUG
#define DBG_PATH "../../Output/Bin/x86/Debug/"
#else
#define DBG_PATH ""
#endif

	template<class T> void loadJSON(T data, std::string path)
	{
		std::ifstream inStream(DBG_PATH + path);
		if (inStream.is_open())
		{
			cereal::JSONInputArchive inArchive(inStream);
			inArchive(*data);
		}
	}

	template<class T> void saveJSON(T data, std::string path, std::string title)
	{
		std::ofstream outStream(DBG_PATH + path);
		cereal::JSONOutputArchive outArchive(outStream);
		outArchive.setNextName(title.c_str());
		outArchive(*data);
	}
}