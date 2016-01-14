#pragma once
#include <cereal/cereal.hpp>
//#include <fstream>
//#include <cereal/archives/json.hpp>

//#ifdef _DEBUG
//#define DBG_PATH "../../Output/Bin/x86/Debug/"
//#else
//#define DBG_PATH ""
//#endif

struct PlayerInfo
{
	int _test = 0;

	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_test));
	}

	//void read(std::string path)
	//{
	//	std::ifstream inStream(DBG_PATH + path);
	//	if (inStream.is_open())
	//	{
	//		cereal::JSONInputArchive inArchive(inStream);
	//		inArchive(*this);
	//	}
	//}

	//void write(std::string path)
	//{
	//	std::ofstream outStream(DBG_PATH + path);
	//	cereal::JSONOutputArchive outArchive(outStream);
	//	outArchive(*this);
	//}
};