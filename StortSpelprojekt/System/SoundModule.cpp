#include "SoundModule.h"

namespace System
{
	SoundModule::SoundModule()
	{
		_allSounds = new std::map<std::string, YSE::sound*>;

		int nrDevices = YSE::System().getNumDevices();

		if (nrDevices > 0)
		{
			YSE::System().init();
		}

	}

	SoundModule::~SoundModule()
	{
		for (auto &i: (*_allSounds))
		{
			delete i.second;
		}

		YSE::System().close();
		delete _allSounds;
	}

	bool SoundModule::AddSound(const std::string &pathName, float volume, float speed, bool relative, bool looping)
	{
		bool answer = true;
		(*_allSounds)[pathName] = new YSE::sound();
		(*_allSounds)[pathName]->create(pathName.c_str());
		//Check if file could be loaded
		if (!(*_allSounds)[pathName]->isValid())
		{
			delete (*_allSounds)[pathName];
			_allSounds->erase(pathName);
			answer = false;

			throw std::runtime_error("Could not load audio: " + pathName);
		}
		else
		{
			(*_allSounds)[pathName]->setVolume(volume);
			(*_allSounds)[pathName]->setSpeed(speed);
			(*_allSounds)[pathName]->setRelative(relative);
			(*_allSounds)[pathName]->setLooping(looping);
		}

		return answer;
	}

	bool SoundModule::RemoveSound(const std::string &pathName)
	{
		bool answer = false;
		auto it = _allSounds->find(pathName);
		if (it != _allSounds->end())
		{
			delete it->second;
			_allSounds->erase(it);
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

	bool SoundModule::Play(std::string pathName)
	{
		bool answer = false;

		if (_allSounds->find(pathName) != _allSounds->end())
		{
			(*_allSounds)[pathName]->play();
			answer = true;
		}
		else
		{
			throw std::runtime_error("Could not play audio: " + pathName);
		}

		return answer;
	}

	bool SoundModule::Stop(std::string pathName)
	{
		bool answer = false;

		if (_allSounds->find(pathName) != _allSounds->end())
		{
			(*_allSounds)[pathName]->stop();
			answer = true;
		}

		return answer;
	}
}