#pragma once
#include <miniaudio.h>

namespace Cockroach
{
	class Audio
	{
	public:
		static ma_engine engine;
		static ma_sound_group sfxGroup;
		static ma_sound_group musicGroup;

		static void Init();
		static void PlayOneShot(std::string filepath);
		static void PlayOneShot(std::string filepath, ma_sound_group group);
		static void ToggleSound(bool mute);
		static void SetListenerPosition(float2 pos);
	};
}