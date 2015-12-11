#include "Controls.h"


namespace System
{

	Controls::Controls(HWND hwnd) : _inputDevice(hwnd)
	{
		
	}

	Controls::~Controls()
	{
	}

	void Controls::Update()
	{
		_inputDevice.Update();
	}
	void Controls::ToggleCursorLock()
	{
		_inputDevice.ToggleCursorLock();
	}

	bool Controls::IsFunctionKeyDown(FunctionKey key)
	{
		bool ret = false;
		switch (key)
		{
		case ROTATE_CAMERA_LEFT:
			ret = _inputDevice.IsDown('Q');
			break;
		case ROTATE_CAMERA_RIGHT:
			ret = _inputDevice.IsDown('E');
			break;
		case MOVE_CAMERA_UP:
			ret = _inputDevice.IsDown('W');
			break;
		case MOVE_CAMERA_DOWN:
			ret = _inputDevice.IsDown('S');
			break;
		case MOVE_CAMERA_LEFT:
			ret = _inputDevice.IsDown('A');
			break;
		case MOVE_CAMERA_RIGHT:
			ret = _inputDevice.IsDown('D');
			break;
		case ZOOM_CAMERA_IN:
			ret = _inputDevice.IsDown('Z');
			break;
		case ZOOM_CAMERA_OUT:
			ret = _inputDevice.IsDown('X');
			break;
		case CONTROL_GROUP0:
			ret = _inputDevice.IsDown(Input::Control) && _inputDevice.IsPressed('0');
			break;
		case CONTROL_GROUP1:
			ret = _inputDevice.IsDown(Input::Control) && _inputDevice.IsPressed('1');
			break;
		case CONTROL_GROUP2:
			ret = _inputDevice.IsDown(Input::Control) && _inputDevice.IsPressed('2');
			break;
		case CONTROL_GROUP3:
			ret = _inputDevice.IsDown(Input::Control) && _inputDevice.IsPressed('3');
			break;
		case CONTROL_GROUP4:
			ret = _inputDevice.IsDown(Input::Control) && _inputDevice.IsPressed('4');
			break;
		case CONTROL_GROUP5:
			ret = _inputDevice.IsDown(Input::Control) && _inputDevice.IsPressed('5');
			break;
		case CONTROL_GROUP6:
			ret = _inputDevice.IsDown(Input::Control) && _inputDevice.IsPressed('6');
			break;
		case CONTROL_GROUP7:
			ret = _inputDevice.IsDown(Input::Control) && _inputDevice.IsPressed('7');
			break;
		case CONTROL_GROUP8:
			ret = _inputDevice.IsDown(Input::Control) && _inputDevice.IsPressed('8');
			break;
		case CONTROL_GROUP9:
			ret = _inputDevice.IsDown(Input::Control) && _inputDevice.IsPressed('9');
			break;
		case DELETE_UNIT:
			ret = _inputDevice.IsPressed(Input::Delete);
			break;
		case MENU:
			ret = _inputDevice.IsDown(Input::Escape);
			break;
		case MENU_UP:
			ret = _inputDevice.IsPressed(Input::UpArrow) || _inputDevice.IsPressed('W');
			break;
		case MENU_DOWN:
			ret = _inputDevice.IsPressed(Input::DownArrow) || _inputDevice.IsPressed('S');
			break;
		case UNDO:
			ret = _inputDevice.IsDown(Input::Control) && _inputDevice.IsPressed('Z');
			break;
		case REDO:
			ret = _inputDevice.IsDown(Input::Control) && _inputDevice.IsPressed('Y');
			break;
		case SAVE:
			ret = _inputDevice.IsDown(Input::Control) && _inputDevice.IsPressed('S');
			break;
		case DEBUG_KEY:
			ret = _inputDevice.IsPressed(Input::PageUp);
			break;
		case DEBUG_ENABLE_FREECAM:
			ret = _inputDevice.IsPressed(Input::Home);
			break;
		case UNLIMITED_MONEY:
			ret = _inputDevice.IsPressed(Input::F11);
			break;
		case INSTANT_WIN:
			ret = _inputDevice.IsPressed(Input::F10);
			break;
		case NO_FOG:
			ret = _inputDevice.IsPressed(Input::F9);
			break;

		default:
			break;
		}
		return ret;
	}


	bool Controls::IsMouseFunctionKeyDown(FunctionKey key, MouseCoord& position)
	{
		bool ret = false;
		switch (key)
		{
		case SELECT:
		{
			ret = _inputDevice.IsPressed(Input::LeftMouse);
			position = _inputDevice.GetMouseCoord();
			break;
		}
		case MOVE:
			ret = _inputDevice.IsPressed(Input::RightMouse);
			position = _inputDevice.GetMouseCoord();
			break;
		}
		return ret;
	}
}