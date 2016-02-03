#pragma once

#include <cereal/cereal.hpp>

#define SYSTEM_EXPORT __declspec(dllexport)

namespace System
{
	struct SYSTEM_EXPORT Settings
	{
	public:
		int _screenWidth;
		int _screenHeight;
		int _shadowMapWidth;
		int _shadowMapHeight;
		bool _fullscreen;
		bool _borderless;
		bool _showMouseCursor;
		float _nearClip;
		float _farClip;
		float _fov;
	public:
		Settings(int screenWidth = 1920,
			int screenHeight = 1080,
			int shadowMapWidth = 256,
			int shadowMapHeight = 256,
			bool fullscreen = true,
			bool borderless = false,
			bool showMouseCursor = true,
			float nearClip = 0.1f,
			float farClip = 1000.0f,
			float fov = 1.57079f //almost PI/2
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
				(CEREAL_NVP(_fullscreen)),
				(CEREAL_NVP(_borderless)),
				(CEREAL_NVP(_showMouseCursor)),
				(CEREAL_NVP(_nearClip)),
				(CEREAL_NVP(_farClip)),
				(CEREAL_NVP(_fov))
				);
		}
	};
}
