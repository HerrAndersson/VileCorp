#include "Window.h"
namespace System
{
	Window::Window(LPCSTR applicationName, HINSTANCE hinstance, WindowSettings settings)
	{
		_applicationName = applicationName;
		_hinstance = hinstance;
		_settings = settings;

		//Will be set on resize anyway, but not setting them here gives warnings
		_exStyle = 0;
		_style = 0;

		InitializeWindow();
	}

	Window::~Window()
	{
		ShutdownWindow();
	}

	void Window::ShutdownWindow()
	{
		ShowCursor(true);

		if (_settings._fullscreen)
		{
			ChangeDisplaySettings(NULL, 0);
		}

		SetCursorPos(GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2);

		DestroyWindow(_hwnd);
		_hwnd = NULL;

		UnregisterClass(_applicationName, _hinstance);
		_hinstance = NULL;
	}

	void Window::InitializeWindow()
	{
		windowHandle = this;

		int posX, posY;

		//Setup the windows class with default settings.
		WNDCLASSEX wc;
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = WndProc;
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

		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		//Setup the screen settings depending on whether it is running in full screen or in windowed mode.
		if (_settings._fullscreen)
		{
			_settings._height = screenHeight;
			_settings._width = screenWidth;

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
			posX = (screenWidth - _settings._width) / 2;
			posY = (screenHeight - _settings._height) / 2;
		}

		RECT rc = { 0, 0, _settings._height, _settings._width };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		_hwnd = CreateWindowEx(WS_EX_APPWINDOW, _applicationName, _applicationName,
			WS_OVERLAPPEDWINDOW | CW_USEDEFAULT | CW_USEDEFAULT,
			posX, posY, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, _hinstance, NULL);

		ShowWindow(_hwnd, SW_SHOW);
		SetForegroundWindow(_hwnd);
		SetFocus(_hwnd);

		SetCursorPos(screenWidth / 2, screenHeight / 2);
		ShowCursor(_settings._showCursor);
	}

	bool Window::Run()
	{
		MSG msg;
		bool result = true;

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

	void Window::ResizeWindow(WindowSettings settings)
	{
		_settings = settings;
		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);
		int posX = 0;
		int posY = 0;

		LONG style = GetWindowLong(_hwnd, GWL_STYLE);
		_style = style;
		LONG exStyle = GetWindowLong(_hwnd, GWL_EXSTYLE);
		_exStyle = exStyle;

		RECT rect = { 0, 0, _settings._width, _settings._height };

		if (_settings._borderless || _settings._fullscreen)
		{
			style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
			SetWindowLong(_hwnd, GWL_STYLE, style);

			exStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
			SetWindowLong(_hwnd, GWL_EXSTYLE, exStyle);

			AdjustWindowRect(&rect, WS_POPUP, FALSE);
		}
		else
		{
			SetWindowLong(_hwnd, GWL_STYLE, _style);
			SetWindowLong(_hwnd, GWL_EXSTYLE, _exStyle);

			AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
		}

		if (!_settings._fullscreen)
		{
			posX = (screenWidth - _settings._width) / 2;
			posY = (screenHeight - _settings._height) / 2;

			SetWindowPos(_hwnd, NULL, posX, posY, rect.right - rect.left, rect.bottom - rect.top, SWP_FRAMECHANGED |SWP_NOOWNERZORDER | SWP_NOZORDER);
		}
		else
		{
			_settings._width = GetSystemMetrics(SM_CXSCREEN);
			_settings._height = GetSystemMetrics(SM_CYSCREEN);

			SetWindowLong(_hwnd, GWL_EXSTYLE, WS_POPUP);
			SetWindowPos(_hwnd, HWND_TOP, posX, posY, _settings._width, _settings._height, SWP_FRAMECHANGED);

			SetWindowPos(_hwnd, HWND_TOP, 0, 0, _settings._width, _settings._height, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		}

		SetForegroundWindow(_hwnd);
		SetFocus(_hwnd);

		SetCursorPos(screenWidth / 2, screenHeight / 2);
		ShowCursor(_settings._showCursor);
	}

	HWND Window::GetHWND()
	{
		return _hwnd;
	}

	LRESULT CALLBACK Window::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}

	LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
	{
		switch (umessage)
		{
		case WM_QUIT:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		default:
		{
			return windowHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
		}
	}
}