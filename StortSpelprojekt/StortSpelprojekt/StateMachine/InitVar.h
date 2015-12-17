#pragma once

#include "../UIHandler.h"
#include "../ObjectHandler.h"
#include "../System/InputDevice.h"
#include "Camera.h"
#include "../PickingDevice.h"
#include "../Controls.h"

struct InitVar
{
	ObjectHandler*			_objectHandler;
	UIHandler*				_uiHandler;
	System::InputDevice*	_inputDevice;
	System::Camera*			_camera;
	PickingDevice*			_pickingDevice;
	System::Controls*		_controls;
};