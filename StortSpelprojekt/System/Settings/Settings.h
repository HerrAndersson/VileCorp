#pragma once

#include <cereal/cereal.hpp>

#define SYSTEM_EXPORT __declspec(dllexport)

namespace System
{
	enum ScreenModes
	{
		WINDOWED, BORDERLESS, FULLSCREEN
	};

	struct SYSTEM_EXPORT Settings
	{
	public:
		unsigned int _screenWidth;
		unsigned int _screenHeight;
		unsigned int _windowWidth;
		unsigned int _windowHeight;
		unsigned int _shadowMapSize;
		int _volume;
		float _nearClip;
		float _farClip;
		float _fov;
		int _screenMode;
		bool _showMouseCursor;
		bool _antialiasing;
		int _brightness;
	public:
		Settings();
		~Settings();

		template<class A>
		void serialize(A& a)
		{
			a(
				CEREAL_NVP(_screenWidth),
				CEREAL_NVP(_screenHeight),
				CEREAL_NVP(_windowWidth),
				CEREAL_NVP(_windowHeight),
				CEREAL_NVP(_shadowMapSize),
				CEREAL_NVP(_screenMode),
				CEREAL_NVP(_showMouseCursor),
				CEREAL_NVP(_nearClip),
				CEREAL_NVP(_farClip),
				CEREAL_NVP(_fov),
				CEREAL_NVP(_antialiasing),
				CEREAL_NVP(_volume),
				CEREAL_NVP(_brightness)
				);
		}
	};
}
