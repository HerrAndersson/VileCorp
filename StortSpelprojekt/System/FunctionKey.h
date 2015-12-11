#pragma once

namespace System
{
	enum FunctionKey
	{
		//Camera functions
		ROTATE_CAMERA_LEFT,
		ROTATE_CAMERA_RIGHT,
		MOVE_CAMERA_UP,
		MOVE_CAMERA_DOWN,
		MOVE_CAMERA_LEFT,
		MOVE_CAMERA_RIGHT,
		ZOOM_CAMERA_IN,
		ZOOM_CAMERA_OUT,

		//Mouse functions
		SELECT,
		MOVE,

		//Unit interaction functions
		CONTROL_GROUP0,
		CONTROL_GROUP1,
		CONTROL_GROUP2,
		CONTROL_GROUP3,
		CONTROL_GROUP4,
		CONTROL_GROUP5,
		CONTROL_GROUP6,
		CONTROL_GROUP7,
		CONTROL_GROUP8,
		CONTROL_GROUP9,
		DELETE_UNIT,

		//Menu functions
		MENU,
		MENU_UP,
		MENU_DOWN,

		//Editor functions
		UNDO,
		REDO,
		SAVE,

		//Test functions
		DEBUG_KEY,
		DEBUG_ENABLE_FREECAM,

		//Cheating functions
		UNLIMITED_MONEY,
		INSTANT_WIN,
		NO_FOG,

	};
}