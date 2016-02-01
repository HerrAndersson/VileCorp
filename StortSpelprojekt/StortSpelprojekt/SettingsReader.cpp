#include "SettingsReader.h"
#include "JsonParser.h"

namespace Settings
{
	SettingsReader::SettingsReader() :
		_userSettings(),
		_userSettingsHasChanged(false)
	{
		//System::saveJSON(&_userSettings, "Assets/userSettings.json", "userSettings");
		System::loadJSON(&_userSettings, "Assets/userSettings.json");
		if (!_userSettingsHasChanged)
		{
			std::cout << "aaaah";
		}
	}


	SettingsReader::~SettingsReader()
	{
	}
	void SettingsReader::ApplySettings()
	{

	}
	bool SettingsReader::HasChanged()
	{
		return _userSettingsHasChanged;
	}
}
