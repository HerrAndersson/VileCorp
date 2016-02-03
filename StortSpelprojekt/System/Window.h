#pragma once
#include <Windows.h>
#include "Settings/settings.h"

#define SYSTEM_EXPORT __declspec(dllexport)

namespace System
{
	class SYSTEM_EXPORT Window
	{

	private:

		LPCSTR			_applicationName;
		HINSTANCE		_hinstance;
		HWND			_hwnd;
		System::Settings*	_settings;

		void InitializeWindow(WNDPROC wndProc);
		void ShutdownWindow();

		LONG _style;
		LONG _exStyle;

	public:

		Window(LPCSTR applicationName, HINSTANCE hinstance, System::Settings* settings, WNDPROC wndProc);
		virtual ~Window();

		bool Run();
		void ResizeWindow(System::Settings* settings);

		HWND GetHWND();
	};

	
	static Window* _windowHandle = nullptr;
}
