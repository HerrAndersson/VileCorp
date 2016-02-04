#pragma once

#include "Settings/settings.h"

#define SYSTEM_EXPORT __declspec(dllexport)

namespace System
{
	class SYSTEM_EXPORT SettingsReader
	{
	private:
		Settings _settings;
		std::string* _settingsFileName;
		bool _settingsHasChanged;
	public:
		SettingsReader(const std::string& settingsFileName);
		~SettingsReader();

		Settings* GetSettings();
		void ApplySettings();
		bool GetSettingsChanged();
		void SetSettingsChanged(bool changed);

		template<typename Archive>
		void LoadXML(const std::string& filename, Archive& a)
		{
			std::ifstream is(filename);
			if (is.is_open())
			{
				cereal::XMLInputArchive inArchive(is);
				inArchive(a);
			}
		}

		template<typename Archive>
		void SaveXML(const std::string& filename, Archive& a)
		{
			std::ofstream os(filename);
			if (os.is_open())
			{
				cereal::XMLOutputArchive inArchive(os);
				inArchive(a);
			}
		}
	};
}
