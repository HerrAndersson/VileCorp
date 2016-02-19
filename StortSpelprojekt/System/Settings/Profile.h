#pragma once

#include <cereal/cereal.hpp>

#define SYSTEM_EXPORT __declspec(dllexport)

namespace System
{
	struct SYSTEM_EXPORT Profile
	{
	public:
		std::string _name;
		int _money;
		int _level;
		bool _firstTime;
	public:
		Profile();
		~Profile();

		template<class A>
		void serialize(A& a)
		{
			a(
				CEREAL_NVP(_name),
				CEREAL_NVP(_money),
				CEREAL_NVP(_level),
				CEREAL_NVP(_firstTime)
				);
		}
	};
}

