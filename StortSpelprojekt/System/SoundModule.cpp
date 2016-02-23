#include "SoundModule.h"

namespace System
{
	SoundModule::SoundModule()
	{
		_allSounds = new std::map<std::string, YSE::sound*>;

		UINT nrOfDevices = 0;
		IMMDeviceEnumerator *pEnumerator = nullptr;
		IMMDeviceCollection *pEndpoints = nullptr;
		CoInitialize(NULL);
		CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
		pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pEndpoints);
		pEndpoints->GetCount(&nrOfDevices);

		if (nrOfDevices > 0)
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
		std::string name = pathName;
		name.append(soundExtension);
		bool answer = true;
		(*_allSounds)[name] = new YSE::sound();
		(*_allSounds)[name]->create(name.c_str());
		//Check if file could be loaded
		if (!(*_allSounds)[name]->isValid())
		{
			delete (*_allSounds)[name];
			_allSounds->erase(name);
			answer = false;

			throw std::runtime_error("Could not load audio: " + name);
		}
		else
		{
			(*_allSounds)[name]->setVolume(volume);
			(*_allSounds)[name]->setSpeed(speed);
			(*_allSounds)[name]->setRelative(relative);
			(*_allSounds)[name]->setLooping(looping);
		}

		return answer;
	}

	bool SoundModule::RemoveSound(const std::string &pathName)
	{
		std::string name = pathName;
		name.append(soundExtension);
		bool answer = false;
		auto it = _allSounds->find(name);
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
		pathName.append(soundExtension);
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
		pathName.append(soundExtension);
		bool answer = false;

		if (_allSounds->find(pathName) != _allSounds->end())
		{
			(*_allSounds)[pathName]->stop();
			answer = true;
		}

		return answer;
	}

	void SoundModule::SetVolume(float volume, int channel)
	{
		switch (channel)
		{
		case CHMASTER:
			_volume[CHMASTER] = volume;
			YSE::ChannelMaster().setVolume(_volume[CHMASTER]);
			break;
		case CHAMBIENT:
			_volume[CHAMBIENT] = volume;
			YSE::ChannelAmbient().setVolume(_volume[CHAMBIENT]);
			break;
		case CHFX:
			_volume[CHFX] = volume;
			YSE::ChannelFX().setVolume(_volume[CHFX]);
			break;
		case CHGUI:
			_volume[CHGUI] = volume;
			YSE::ChannelGui().setVolume(_volume[CHGUI]);
			break;
		case CHMUSIC:
			_volume[CHMUSIC] = volume;
			YSE::ChannelMusic().setVolume(_volume[CHMUSIC]);
			break;
		case CHVOICE:
			_volume[CHVOICE] = volume;
			YSE::ChannelVoice().setVolume(_volume[CHVOICE]);
			break;
		default:
			break;
		}
	}

	float SoundModule::GetVolume(int channel)
	{
		return _volume[channel];
	}
}