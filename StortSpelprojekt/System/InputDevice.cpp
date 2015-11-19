#include "InputDevice.h"

namespace System
{
	InputDevice::InputDevice()
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
	}
	InputDevice::~InputDevice()
	{}

	void InputDevice::Update()
	{
		POINT lOldPos = _mouseCoord._pos;
		GetCursorPos(&_mouseCoord._pos);
		_mouseCoord._deltaPos.x = lOldPos.x - _mouseCoord._pos.y;
		_mouseCoord._deltaPos.y = lOldPos.y - _mouseCoord._pos.y;

		for (int i = 1; i < KEYCODECAP; i++)
		{
			_last[i] = _current[i];	
			_current[i] = GetAsyncKeyState(i) ? 1:0 & 0x0800;
		}
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