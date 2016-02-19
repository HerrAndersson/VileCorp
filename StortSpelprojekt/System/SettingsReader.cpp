#include "SettingsReader.h"
#include <cereal/archives/xml.hpp>

#include <fstream>

namespace System
{
	SettingsReader::SettingsReader(const std::string& settingsFileName, const std::string& profileFileName) :
		_settings(),
		_settingsHasChanged(false),
		_profile()
	{
		_settingsFileName = new std::string(settingsFileName);
		LoadXML(*_settingsFileName, _settings);

		_profileFileName = new std::string(profileFileName);
		LoadXML(*_profileFileName, _profile);
	}


	SettingsReader::~SettingsReader()
	{
		SaveXML(*_profileFileName, _profile);
		delete _settingsFileName;
		delete _profileFileName;
	}

	Settings* SettingsReader::GetSettings()
	{
		return &_settings;
	}

	void SettingsReader::ApplySettings()
	{
		_settingsHasChanged = true;
		SaveXML(*_settingsFileName, _settings);
	}

	bool SettingsReader::GetSettingsChanged()
	{
		return _settingsHasChanged;
	}
	void SettingsReader::SetSettingsChanged(bool changed)
	{
		_settingsHasChanged = changed;
	}

	Profile* SettingsReader::GetProfile()
	{
		return &_profile;
	}

	void SettingsReader::ApplyProfileSettings()
	{
		SaveXML(*_profileFileName, _profile);
	}
}