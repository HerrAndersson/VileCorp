#pragma once
#define SYSTEM_EXPORT __declspec(dllexport)


#include "yse\yse.hpp"
#include <map>
#include <string>
#include <vector>
#include <mmdeviceapi.h>
#include <DirectXMath.h>

#include "Settings\Settings.h"

#pragma comment(lib, "libyse32.lib")

#pragma warning( disable: 4251 )

/*
Sound device has to be conected to computer when program starts, 
else SoundModule is muted to avoid crashes.

Using a map of sounds to quickly find a desired sound to play.

Lower OFFSET to make sound "travel" less. 
Increase OFFSET to make sound "travel" further.

Currently can't play the same sound over itself. 
*/

enum Channel{CHMASTER, CHAMBIENT, CHFX, CHGUI, CHMUSIC, CHVOICE, NR_OF_CHANNELS};

namespace System
{
	class SYSTEM_EXPORT SoundModule
	{
	private:
		const float OFFSET = 2.0f;

		std::map<std::string, YSE::sound*>* _allSounds;
		std::string _soundExtension;
		std::string _stdPath;
		float _volume[NR_OF_CHANNELS];
		bool _initiated;

	public:
		/*Functions*/
		SoundModule(System::Settings* settings, const std::string &stdPath, const std::string &extension);
		~SoundModule();

		bool AddSound(const std::string &fileName, float volume = 1.0f, float speed = 1.0f, bool relative = true, bool looping = false, bool streaming = false);
		bool RemoveSound(const std::string &fileName);   //Not needed but good to have

		void Update(const DirectX::XMFLOAT3& position);
		bool Play(const std::string &fileName);
		bool Pause(const std::string &fileName);
		bool Stop(const std::string &fileName);
		bool Stop(const std::string &fileName, int fadeTime);
		bool HardStop(const std::string &fileName);

		void SetSoundPosition(const std::string &fileName,float x, float y, float z);  //Sets position of a sound
		void SetVolume(float volume, int channel);
		float GetVolume(int channel);
	};
}