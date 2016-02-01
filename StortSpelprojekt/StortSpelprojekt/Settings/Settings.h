#pragma once

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

namespace Settings
{
	struct Settings
	{
	public:
		int _screenWidth;
		int _screenHeight;
		int _shadowMapWidth;
		int _shadowMapHeight;
		bool _fullScreen;
		bool _bordeless;
		bool _showMouseCursor;
	public:
		Settings(	int _screenWidth = 1920,
					int _screenHeight = 1080,
					int _shadowMapWidth = 256,
					int _shadowMapHeight = 256,
					bool _fullScreen = true,
					bool _bordeless = false,
					bool _showMouseCursor = true
		);
		~Settings();

		template<class A>
		void serialize(A& a)
		{
			a(
				(CEREAL_NVP(_screenWidth)),
				(CEREAL_NVP(_screenHeight)),
				(CEREAL_NVP(_shadowMapWidth)),
				(CEREAL_NVP(_shadowMapHeight)),
				(CEREAL_NVP(_fullScreen)),
				(CEREAL_NVP(_bordeless)),
				(CEREAL_NVP(_showMouseCursor))
				);
		}
	};
}
