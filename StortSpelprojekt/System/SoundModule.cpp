#include "SoundModule.h"

namespace System
{
	/*Helpers*/
	int SoundModule::findSoundIndex(Sound* array[], std::string soundName, int imin, int imax) //Binary search
	{
		int answer = -1;
		if (imax >= imin)
		{
			int imid = imin + ((imax - imin) / 2);

			if (array[imid]->_soundName > soundName)
			{
				answer = findSoundIndex(array, soundName, imin, imid - 1);
			}
			else if (array[imid]->_soundName < soundName)
			{
				answer = findSoundIndex(array, soundName, imid + 1, imax);
			}
			else
			{
				answer = imid;
			}

		}

		return answer;
	}

	void SoundModule::expandArray()
	{
		_arraySize = _arraySize * 2;
		Sound** temp = new Sound*[_arraySize];
		for (int i = 0; i < _nrOfSounds; i++)
		{
			temp[i] = _allSounds[i];
		}
		delete _allSounds;

		_allSounds = temp;
	}

	/*Functions*/
	SoundModule::SoundModule()
	{
		YSE::System().init();
		_nrOfSounds = 0;
		_arraySize = 10;
		_allSounds = new Sound*[_arraySize];
		for (int i = 0; i < _arraySize; i++)
		{
			_allSounds[i] = nullptr;
		}
	}

	SoundModule::~SoundModule()
	{
		for (int i = 0; i < _nrOfSounds; i++)
		{
			delete _allSounds[i];
			_allSounds[i] = nullptr;
		}
		delete[] _allSounds;

		YSE::System().close();
	}

	bool SoundModule::AddSound(char* fileName, float volume, float speed, bool relative, bool looping)
	{
		bool answer = true;

		if (_arraySize <= _nrOfSounds)
		{
			expandArray();
		}

		_allSounds[_nrOfSounds] = new Sound;
		_allSounds[_nrOfSounds]->_sound.create( ("Assets/Sounds/%s", fileName) );
		//Check if file could be loaded, if not delete the new Sound struct again
		if (!_allSounds[_nrOfSounds]->_sound.isValid()) 
		{
			delete _allSounds[_nrOfSounds];
			_allSounds[_nrOfSounds] = nullptr;
			answer = false;
		}
		else
		{
			_allSounds[_nrOfSounds]->_soundName = fileName;
			_allSounds[_nrOfSounds]->_sound.setVolume(volume);
			_allSounds[_nrOfSounds]->_sound.setSpeed(speed);
			_allSounds[_nrOfSounds]->_sound.setRelative(relative);
			_allSounds[_nrOfSounds]->_sound.setLooping(looping);
			_nrOfSounds++;
		}

		return answer;
	}

	bool SoundModule::RemoveSound(char* fileName)
	{
		bool answer = false;

		int deleteIndex = findSoundIndex(_allSounds, fileName, 0, _nrOfSounds);

		if (_allSounds[deleteIndex]->_sound.isValid())
		{
			delete _allSounds[deleteIndex];
			_allSounds[deleteIndex] = nullptr;
			_nrOfSounds--;
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
		int index = findSoundIndex(_allSounds, soundName, 0, _nrOfSounds);
		if (index != -1)
		{
			_allSounds[index]->_sound.play();
			return true;
		}

		return false;
	}

	bool SoundModule::Stop(std::string soundName)
	{
		int index = findSoundIndex(_allSounds, soundName, 0, _nrOfSounds);
		if (index != -1)
		{
			_allSounds[index]->_sound.stop();
			return true;
		}

		return false;
	}
}