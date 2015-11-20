#pragma once
#include <Windows.h>
#define SYSTEM_EXPORT __declspec(dllexport)

namespace System
{
	struct SYSTEM_EXPORT WindowSettings
	{
		int _width = 400;
		int _height = 300;
		bool _fullscreen = false;
		bool _borderless = false;
		bool _showCursor = true;

		WindowSettings() {};

		WindowSettings(int width, int height, bool fullscreen, bool borderless, bool showCursor)
		{
			_width = width;
			_height = height;
			_fullscreen = fullscreen;
			_borderless = borderless;
			_showCursor = showCursor;
		}
	};
	class SYSTEM_EXPORT Window
	{

	private:

		LPCSTR			_applicationName;
		HINSTANCE		_hinstance;
		HWND			_hwnd;
		WindowSettings	_settings;

		void InitializeWindow();
		void ShutdownWindow();

		LONG _style;
		LONG _exStyle;

	public:

		Window(LPCSTR applicationName, HINSTANCE hinstance, WindowSettings settings);
		virtual ~Window();

		bool Run();
		void ResizeWindow(WindowSettings settings);

		HWND GetHWND();

		LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
	};

	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	static Window* windowHandle = nullptr;
}
