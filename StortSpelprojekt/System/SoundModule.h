#ifndef SOUNDMODULE_H
#define SOUNDMODULE_H

#define SYSTEM_EXPORT __declspec(dllexport)

#include "yse\yse.hpp"
#include <map>
#include <string>

#pragma comment(lib, "libyse32.lib")

/*
Is using a sorted array of sounds to quickly find a desired sound to play.
*/

namespace System
{
	class SYSTEM_EXPORT SoundModule
	{
	private:
		std::map<std::string, YSE::sound> _allSounds;

	public:
		/*Functions*/
		SoundModule();
		~SoundModule();

		bool AddSound(const std::string &fileName, float volume = 1.0f, float speed = 1.0f, bool relative = true, bool looping = true);
		bool RemoveSound(const std::string &fileName);   //Not needed but good to have

		void Update(/*position*/);
		bool Play(std::string soundName);
		bool Stop(std::string soundName);
		//SetPosition();  //Sets position of a sound
	};
}

#endif