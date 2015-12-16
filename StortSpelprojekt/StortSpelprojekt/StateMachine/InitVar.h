#pragma once

#include "../UIHandler.h"
#include "../ObjectHandler.h"
#include "InputDevice.h"
#include "Camera.h"
#include "../PickingDevice.h"

struct InitVar
{
	ObjectHandler*			_objectHandler;
	UIHandler*				_uiHandler;
	System::InputDevice*	_inputHandler;
	System::Camera*			_camera;
	PickingDevice*			_pickingDevice;

};