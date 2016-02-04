#ifndef SOUNDMODULE_H
#define SOUNDMODULE_H

#define SYSTEM_EXPORT __declspec(dllexport)

#include <sstream>
#include "yse\yse.hpp"

#pragma comment(lib, "libyse32.lib")

/*
Is using a sorted array of sounds to quickly find a desired sound to play.
*/

namespace System
{
	class SYSTEM_EXPORT SoundModule
	{
	private:
		struct Sound
		{
			std::string _soundName;
			YSE::sound _sound;
			Sound() {};
		};

		int		_arraySize;
		int		_nrOfSounds;
		Sound**	_allSounds;

		/*Helpers*/
		int findSoundIndex(Sound* array[], std::string soundName, int imin, int imax);
		void expandArray();

	public:
		/*Functions*/
		SoundModule();
		~SoundModule();

		bool AddSound(char* fileName, float volume = 1.0f, float speed = 1.0f, bool relative = true, bool looping = true);
		bool RemoveSound(char* fileName);   //Not needed but good to have

		void Update(/*position*/);
		bool Play(std::string soundName);
		bool Stop(std::string soundName);
		//SetPosition();  //Sets position of a sound
	};
}

#endif