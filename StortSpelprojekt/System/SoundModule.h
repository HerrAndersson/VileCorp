#pragma once
#define SYSTEM_EXPORT __declspec(dllexport)


#include "yse\yse.hpp"
#include <map>
#include <string>
#include <vector>
#include <mmdeviceapi.h>

#pragma comment(lib, "libyse32.lib")

/*
Sound device has to be conected to computer when program starts, 
else SoundModule is muted to avoid crashes.

Using a map of sounds to quickly find a desired sound to play.

Lower OFFSET to make sound "travel" less. 
Increase OFFSET to make sound "travel" further.
*/

namespace System
{
	class SYSTEM_EXPORT SoundModule
	{
	private:
		const float OFFSET = 2.0f;

		std::map<std::string, YSE::sound*>* _allSounds;
		const std::string soundExtension = ".ogg";

	public:
		/*Functions*/
		SoundModule();
		~SoundModule();

		bool AddSound(const std::string &pathName, float volume = 1.0f, float speed = 1.0f, bool relative = true, bool looping = false);
		bool RemoveSound(const std::string &pathName);   //Not needed but good to have

		void Update(float listenerX, float listenerY, float listenerZ);
		bool Play(std::string pathName);
		bool Pause(std::string pathName);
		bool Stop(std::string pathName);
		void SetSoundPosition(std::string pathName,float x, float y, float z);  //Sets position of a sound
	};
}
