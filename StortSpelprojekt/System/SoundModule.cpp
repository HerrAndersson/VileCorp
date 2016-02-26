#include "SoundModule.h"

namespace System
{
	SoundModule::SoundModule(System::Settings* settings)
	{
		_allSounds = new std::map<std::string, YSE::sound*>;
		_initiated = false;

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
			_initiated = true;
			SetVolume(settings->_volume / 100.0f, CHMASTER);
		}
	}

	SoundModule::~SoundModule()
	{
		if (_initiated)
		{
			for (auto &i : (*_allSounds))
			{
				delete i.second;
			}

			YSE::System().close();
			delete _allSounds;
		}
	}

	/*
	AddSound: Builds its own path using fileName
	*/
	bool SoundModule::AddSound(const std::string &fileName, float volume, float speed, bool relative, bool looping, bool streaming)
	{
		bool answer = false;
		if (_initiated)
		{
			/*
			stdPath set "Assets/Sounds/<name>.ogg
			name is set to pathName
			*/
			std::string name = fileName;
			std::string stdPath = "Assets/Sounds/";
			stdPath.append(name);
			stdPath.append(soundExtension);

			answer = true;
			(*_allSounds)[name] = new YSE::sound();
			(*_allSounds)[name]->create(stdPath.c_str());

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
		}

		return answer;
	}

	bool SoundModule::RemoveSound(const std::string &fileName)
	{
		bool answer = false;

		if (_initiated)
		{
			std::string name = fileName;

			auto it = _allSounds->find(name);
			if (it != _allSounds->end())
			{
				delete it->second;
				_allSounds->erase(it);
				answer = true;
			}
		}

		return answer;
	}

	void SoundModule::Update(float listenerX, float listenerY, float listenerZ)
	{
		if (_initiated)
		{
			/*
			Updates the position of the listener to keep it synced with camera position
			*/
			YSE::Listener().setPosition(YSE::Vec(listenerX / OFFSET, listenerY / OFFSET, listenerZ / OFFSET));
			YSE::System().update();
		}
	}

	bool SoundModule::Play(std::string fileName)
	{
		bool answer = false;

		if (_initiated)
		{
			if (_allSounds->find(fileName) != _allSounds->end())
			{
				(*_allSounds)[fileName]->play();
				answer = true;
			}
			else
			{
				throw std::runtime_error("Could not play audio: " + fileName);
			}
		}

		return answer;
	}

	bool SoundModule::Pause(std::string fileName)
	{
		bool answer = false;

		if (_initiated)
		{
			if (_allSounds->find(fileName) != _allSounds->end())
			{
				(*_allSounds)[fileName]->pause();
				answer = true;
			}
			else
			{
				throw std::runtime_error("Could not pause audio: " + fileName);
			}
		}

		return answer;
	}

	bool SoundModule::Stop(std::string fileName)
	{
		bool answer = false;

		if (_initiated)
		{
			if (_allSounds->find(fileName) != _allSounds->end())
			{
				(*_allSounds)[fileName]->stop();
				answer = true;
			}
		}

		return answer;
	}

	//fadeTime is in milliseconds
	bool SoundModule::Stop(std::string fileName, int fadeTime)
	{
		bool answer = false;

		if (_initiated)
		{
			if (_allSounds->find(fileName) != _allSounds->end())
			{
				(*_allSounds)[fileName]->fadeAndStop(fadeTime);
				answer = true;
			}
		}

		return answer;
	}

	void SoundModule::SetSoundPosition(std::string fileName ,float x, float y, float z)
	{
		if (_initiated)
		{
			if (_allSounds->find(fileName) != _allSounds->end())
			{
				(*_allSounds)[fileName]->setPosition(YSE::Vec(x / OFFSET, y / OFFSET, z / OFFSET));
			}
			else
			{
				throw std::runtime_error("Could not set audio position for: " + fileName);
			}
		}
	}

	void SoundModule::SetVolume(float volume, int channel)
	{
		if (_initiated)
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

	}

	float SoundModule::GetVolume(int channel)
	{
		if (_initiated)
		{
			return _volume[channel];
		}
	}
}