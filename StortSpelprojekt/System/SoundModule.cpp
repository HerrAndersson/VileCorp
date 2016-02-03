#include "SoundModule.h"

namespace System
{
	/*Helpers*/
	int SoundModule::findSoundIndex(Sound array[], std::string soundName, int imin, int imax) //Binary search
	{
		int answer = -1;
		if (imax >= imin)
		{
			int imid = imin + ((imax - imin) / 2);

			if (array[imid]._soundName > soundName)
			{
				answer = findSoundIndex(array, soundName, imin, imid - 1);
			}
			else if (array[imid]._soundName < soundName)
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
		Sound* temp = new Sound[_arraySize];
		for (int i = 0; i < _nrOfSounds; i++)
		{
			temp[i]._soundName = _allSounds[i]._soundName;
			//temp[i]._sound = _allSounds[i]._sound; //FIXA HÄR
		}
		delete[] _allSounds;

		_allSounds = temp;
	}

	/*Functions*/
	SoundModule::SoundModule()
	{
		_nrOfSounds = 0;
		_arraySize = 10;
		_allSounds = new Sound[_arraySize];
	}

	SoundModule::~SoundModule()
	{
		delete[] _allSounds;
	}

	void SoundModule::AddSound()
	{
	}

	void SoundModule::RemoveSound()
	{
	}

	void SoundModule::Update()
	{
		/*
		Updates the position of the listener to keep it synced with camera position
		*/

	}

	bool SoundModule::Play(std::string soundName)
	{
		int index = findSoundIndex(_allSounds, soundName, 0, _nrOfSounds);
		if (index != -1)
		{
			_allSounds[index]._sound.play();
			return true;
		}

		return false;
	}

	bool SoundModule::Stop(std::string soundName)
	{
		int index = findSoundIndex(_allSounds, soundName, 0, _nrOfSounds);
		if (index != -1)
		{
			_allSounds[index]._sound.stop();
			return true;
		}

		return false;
	}
}