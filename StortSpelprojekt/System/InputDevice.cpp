#include "InputDevice.h"

namespace System
{
	InputDevice::InputDevice(HWND hwnd)
	{
		for (int i = 0; i < KEYCODECAP; i++)
		{
			_current[i] = false;
			_last[i] = false;
		}

		_mouseCoord._pos.x = 0;
		_mouseCoord._pos.y = 0;
		_mouseCoord._deltaPos.x = 0;
		_mouseCoord._deltaPos.y = 0;

		_hwnd = hwnd;
	}
	InputDevice::~InputDevice()
	{}

	void InputDevice::Update()
	{
		RECT rect;
		GetWindowRect(_hwnd, &rect);


		GetCursorPos(&_mouseCoord._pos);
		
		_mouseCoord._deltaPos.x = _mouseCoord._pos.x - (rect.left + (rect.right - rect.left) / 2);
		_mouseCoord._deltaPos.y = _mouseCoord._pos.y - (rect.top + (rect.bottom - rect.top) / 2);


		for (int i = 1; i < KEYCODECAP; i++)
		{
			_last[i] = _current[i];	
			_current[i] = GetAsyncKeyState(i) ? 1:0 & 0x0800;
		}
		
		if (GetFocus() == _hwnd)
		{
			//Sets mouse position to the middle of the window
			SetCursorPos(rect.left + (rect.right - rect.left) / 2, rect.top + (rect.bottom - rect.top) / 2);
			ClipCursor(&rect);
		}
		ScreenToClient(_hwnd, &_mouseCoord._pos);
	}

	bool InputDevice::IsDown(int key)
	{
		return _current[key];
	}

	bool InputDevice::IsUp(int key)
	{
		return !_current[key];
	}

	bool InputDevice::IsPressed(int key)
	{
		return _current[key] && !_last[key];
	}

	bool InputDevice::IsReleased(int key)
	{
		return !_current[key] && _last[key];
	}

	MouseCoord InputDevice::GetMouseCoord()const
	{
		
		return _mouseCoord;
	}
}