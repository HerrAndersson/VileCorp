#include "Window.h"
namespace System
{
	Window::Window(LPCSTR applicationName, HINSTANCE hinstance, System::Settings* settings, WNDPROC wndProc)
	{
		_applicationName = applicationName;
		_hinstance = hinstance;
		_settings = settings;

		_style = 0;
		_exStyle = 0;

		InitializeWindow(wndProc);
	}

	Window::~Window()
	{
		ShutdownWindow();
	}

	void Window::ShutdownWindow()
	{
		ShowCursor(true);

		if (_settings->_fullscreen)
		{
			ChangeDisplaySettings(NULL, 0);
		}

		SetCursorPos(GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2);

		DestroyWindow(_hwnd);
		_hwnd = NULL;

		UnregisterClass(_applicationName, _hinstance);
		_hinstance = NULL;
	}

	void Window::InitializeWindow(WNDPROC wndProc)
	{
		_windowHandle = this;

		int posX, posY;

		//Setup the windows class with default settings
		WNDCLASSEX wc;
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = wndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = _hinstance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = _applicationName;
		wc.cbSize = sizeof(WNDCLASSEX);

		RegisterClassEx(&wc);
		/*
		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		//Setup the screen settings depending on whether it is running in full screen or in windowed mode.
		if (_settings->_fullscreen)
		{
			_settings->_windowHeight = screenHeight;
			_settings->_windowWidth = screenWidth;

			//If full screen set the screen to maximum size of the users desktop and 32bit.
			DEVMODE dmScreenSettings;
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
			dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

			posX = 0;
			posY = 0;
		}
		else //If windowed
		{
			//Place the window in the middle of the screen.
			posX = (screenWidth - _settings->_windowWidth) / 2;
			posY = (screenHeight - _settings->_windowHeight) / 2;
		}

		RECT rc = { 0, 0, (LONG)_settings->_windowWidth, (LONG)_settings->_windowHeight };
		AdjustWindowRect(&rc, (WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX), FALSE);

		_hwnd = CreateWindowEx(WS_EX_APPWINDOW, _applicationName, _applicationName,
			(WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX) | CW_USEDEFAULT | CW_USEDEFAULT,
			0, 0, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, _hinstance, NULL);
		*/

		_hwnd = CreateWindowEx(WS_EX_APPWINDOW, _applicationName, _applicationName,
			(WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX) | CW_USEDEFAULT | CW_USEDEFAULT,
			0, 0, 300, 300, NULL, NULL, _hinstance, NULL);

		ResizeWindow(_settings);
		ShowWindow(_hwnd, SW_SHOW);
		SetForegroundWindow(_hwnd);
		SetFocus(_hwnd);

		//SetCursorPos(screenWidth / 2, screenHeight / 2);
		ShowCursor(_settings->_showMouseCursor);
	}

	bool Window::Run()
	{
		MSG msg;
		bool result = true;

		if (GetAsyncKeyState(VK_ESCAPE) != 0)
		{
			return false;
		}

		ZeroMemory(&msg, sizeof(MSG));

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			result = false;
		}
		return result;
	}

	void Window::ResizeWindow(System::Settings* settings)
	{
		_settings = settings;
		int systemWidth = GetSystemMetrics(SM_CXSCREEN);
		int systemHeight = GetSystemMetrics(SM_CYSCREEN);
		int windowWidth = _settings->_windowWidth;
		int windowHeight = _settings->_windowHeight;
		bool borderless = _settings->_borderless;

		int windowPositionX = (systemWidth - windowWidth) / 2;
		int windowPositionY = (systemHeight - windowHeight) / 2;
		LONG style = GetWindowLong(_hwnd, GWL_STYLE);
		LONG exStyle = GetWindowLong(_hwnd, GWL_EXSTYLE);
		//style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);

		//If settings is fullscreen, set the window to cover the entire screen and be borderless
		if (borderless)
		{
			style = GetWindowLong(_hwnd, GWL_STYLE);
			style &= ~(WS_CAPTION | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU | WS_POPUP | WS_BORDER);

			exStyle = GetWindowLong(_hwnd, GWL_EXSTYLE);
			exStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
		}
		else //This is a window with borders - adjust the window size acordingly
		{
			RECT rect = { 0, 0, windowWidth, windowHeight };
			AdjustWindowRect(&rect, style, FALSE);
			windowWidth = rect.right - rect.left;
			windowHeight = rect.bottom - rect.top;
		}
		
		SetWindowLong(_hwnd, GWL_STYLE, style);
		SetWindowLong(_hwnd, GWL_EXSTYLE, exStyle);

		SetWindowPos(_hwnd, NULL, windowPositionX, windowPositionY, windowWidth, windowHeight, SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_NOZORDER);
		SetForegroundWindow(_hwnd);
		SetFocus(_hwnd);
		ShowCursor(_settings->_showMouseCursor);
	}

	HWND Window::GetHWND()
	{
		return _hwnd;
	}	
}