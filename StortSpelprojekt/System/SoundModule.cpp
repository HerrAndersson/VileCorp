#include "SoundModule.h"

namespace System
{
	SoundModule::SoundModule(System::Settings* settings, const std::string &stdPath, const std::string &extension)
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

		_stdPath = stdPath;
		_soundExtension = extension;

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
		}
		delete _allSounds;
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
			path set "Assets/Sounds/<name>.ogg
			name is set to pathName
			*/
			std::string name = fileName;
			std::string path = _stdPath;
			path.append(name);
			path.append(_soundExtension);

			answer = true;
			(*_allSounds)[name] = new YSE::sound();
			(*_allSounds)[name]->create(path.c_str(), NULL, looping, volume, streaming);

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
			auto it = _allSounds->find(fileName);
			if (it != _allSounds->end())
			{
				delete it->second;
				_allSounds->erase(it);
				answer = true;
			}
		}

		return answer;
	}

	void SoundModule::Update(const DirectX::XMFLOAT3& position)
	{
		if (_initiated)
		{
			/*
			Updates the position of the listener to keep it synced with camera position
			*/
			YSE::Listener().setPosition(YSE::Vec(position.x / OFFSET, position.y / OFFSET, position.z / OFFSET));
			YSE::System().update();
		}
	}

	bool SoundModule::Play(const std::string &fileName)
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

	bool SoundModule::Pause(const std::string &fileName)
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

	bool SoundModule::Stop(const std::string &fileName)
	{
		bool answer = false;

		if (_initiated)
		{
			if (_allSounds->find(fileName) != _allSounds->end())
			{
				(*_allSounds)[fileName]->stop();
				answer = (*_allSounds)[fileName]->isStopped();
			}
		}

		return answer;
	}

	//fadeTime is in milliseconds
	bool SoundModule::Stop(const std::string &fileName, int fadeTime)
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

	//Hard stop is done when resetting streamed files. Removes and adds file again
	bool SoundModule::HardStop(const std::string &fileName)
	{
		bool answer = false;

		if (_initiated)
		{
			if (_allSounds->find(fileName) != _allSounds->end())
			{
				(*_allSounds)[fileName]->stop();
				float volume = (*_allSounds)[fileName]->getVolume();
				float speed = (*_allSounds)[fileName]->getSpeed();
				bool relative = (*_allSounds)[fileName]->isRelative();
				bool looping = (*_allSounds)[fileName]->isLooping();
				bool streaming = true;//(*_allSounds)[fileName]->isStreaming();

				RemoveSound(fileName);
				AddSound(fileName, volume, speed, relative, looping, streaming);

				answer = true;
			}
		}

		return answer;
	}

	void SoundModule::SetSoundPosition(const std::string &fileName, float x, float y, float z)
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
			_volume[channel] = volume;
			YSE::ChannelMaster().setVolume(_volume[channel]);
		}
	}

	float SoundModule::GetVolume(int channel)
	{
		if (_initiated)
		{
			return _volume[channel];
		}
		return 0.0f;
	}
}