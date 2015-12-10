#ifndef INITVAR_H
#define INITVAR_H

#include "UIHandler.h"
#include "ObjectHandler.h"
#include "InputDevice.h"

struct InitVar
{
	ObjectHandler*			_objectHandler;
	UIHandler*				_uiHandler;
	System::InputDevice*	_inputDevice;
};

#endif