#pragma once
#include <Windows.h>
#define SYSTEM_EXPORT __declspec(dllexport)

namespace System
{
	struct SYSTEM_EXPORT WindowSettings
	{
		enum
		{
			FULLSCREEN	= 1,
			BORDERLESS	= 2,
			SHOW_CURSOR	= 4
		};

		int _width;
		int _height;
		int _flags;

		WindowSettings(int width = 1280, int height = 720, int flags = SHOW_CURSOR)
		{
			_width = width;
			_height = height;
			_flags = flags;
		}
	};
	class SYSTEM_EXPORT Window
	{

	private:

		LPCSTR			_applicationName;
		HINSTANCE		_hinstance;
		HWND			_hwnd;
		WindowSettings	_settings;

		void InitializeWindow(WNDPROC wndProc);
		void ShutdownWindow();

		LONG _style;
		LONG _exStyle;

	public:

		Window(LPCSTR applicationName, HINSTANCE hinstance, WindowSettings settings, WNDPROC wndProc);
		virtual ~Window();

		bool Run();
		void ResizeWindow(WindowSettings settings);

		HWND GetHWND();

		WindowSettings GetWindowSettings();
	};

	
	static Window* _windowHandle = nullptr;
}
