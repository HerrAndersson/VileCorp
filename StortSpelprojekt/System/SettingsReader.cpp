#include "SettingsReader.h"
#include <cereal/archives/xml.hpp>

#include <fstream>

namespace System
{
	SettingsReader::SettingsReader(const std::string& settingsFileName) :
		_settings(),
		_settingsFileName(settingsFileName),
		_settingsHasChanged(false)
	{
		LoadXML(_settingsFileName, _settings);
	}


	SettingsReader::~SettingsReader()
	{
	}

	Settings* SettingsReader::GetSettings()
	{
		return &_settings;
	}

	void SettingsReader::ApplySettings(const Settings& settings)
	{
		_settings = settings;
		_settingsHasChanged = true;
		SaveXML(_settingsFileName, _settings);
	}

	bool SettingsReader::GetSettingsChanged()
	{
		return _settingsHasChanged;
	}
	void SettingsReader::SetSettingsChanged(bool changed)
	{
		_settingsHasChanged = changed;
	}
}