#include "SettingsReader.h"
#include <cereal/archives/xml.hpp>

#include <fstream>

namespace System
{
	SettingsReader::SettingsReader(const std::string& settingsFileName) :
		_settings(),
		_settingsHasChanged(false)
	{
		_settingsFileName = new std::string(settingsFileName);
		LoadXML(*_settingsFileName, _settings);
	}


	SettingsReader::~SettingsReader()
	{
		delete _settingsFileName;
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
}