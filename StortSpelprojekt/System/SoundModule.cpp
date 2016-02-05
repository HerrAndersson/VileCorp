#include "SoundModule.h"

namespace System
{
	/*Helpers*/


	/*Functions*/
	SoundModule::SoundModule()
	{
		YSE::System().init();

	}

	SoundModule::~SoundModule()
	{
		YSE::System().close();
	}

	bool SoundModule::AddSound(const std::string &fileName, float volume, float speed, bool relative, bool looping)
	{
		bool answer = true;

		_allSounds[fileName].create(fileName.c_str());
		//Check if file could be loaded, if not delete the new Sound struct again
		if (!_allSounds[fileName].isValid()) 
		{
			answer = false;
		}
		else
		{
			_allSounds[fileName].setVolume(volume);
			_allSounds[fileName].setSpeed(speed);
			_allSounds[fileName].setRelative(relative);
			_allSounds[fileName].setLooping(looping);
		}

		return answer;
	}

	bool SoundModule::RemoveSound(const std::string &fileName)
	{
		bool answer = false;
		if (_allSounds.find(fileName) != _allSounds.end())
		{
			_allSounds.erase(fileName);
			answer = true;
		}
		

		return answer;
	}

	void SoundModule::Update()
	{
		/*
		Updates the position of the listener to keep it synced with camera position
		*/
		YSE::Listener().setPosition(YSE::Vec(0.0f, 0.0f, 0.0f));
		YSE::System().update();
	}

	bool SoundModule::Play(std::string soundName)
	{
		bool answer = false;

		if (_allSounds.find(soundName) != _allSounds.end())
		{
			_allSounds[soundName].play();
			answer = true;
		}

		return answer;
	}

	bool SoundModule::Stop(std::string soundName)
	{
		bool answer = false;

		if (_allSounds.find(soundName) != _allSounds.end())
		{
			_allSounds[soundName].stop();
			answer = true;
		}

		return answer;
	}
}