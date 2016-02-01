#pragma once

#include "Settings/Settings.h"
//#include "Settings/Profile.h"
//#include "Settings/Prefrences.h"
//#include "Settings/DebugSettings.h"

namespace Settings
{
	class SettingsReader
	{
	private:
		Settings _userSettings;
		bool _userSettingsHasChanged;

		//TODO: Implement //Mattias
		//Profile			_userProfile;
		//Prefrences		_prefrences;
		//DebugSettings	_debugSettings;
	public:
		SettingsReader();
		~SettingsReader();

		void ApplySettings();
		bool HasChanged();
	};
}
