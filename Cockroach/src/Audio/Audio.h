#pragma once
#include <miniaudio.h>

namespace Cockroach
{
	#define MA_ASSERT(stmt, message) {ma_result result = stmt; \
		if(result != MA_SUCCESS) \
		{ \
			CR_CORE_WARN(message); \
			return; \
		}}

	class Audio
	{
	public:
		static ma_engine engine;
		static ma_sound_group sfxGroup;
		static ma_sound_group musicGroup;

		static void Init();
		static void PlayOneShot(std::string filepath);
		static void PlayOneShot(std::string filepath, ma_sound_group group);
		static void ToggleSoundMaster(bool mute);
		static void ToggleSoundSFX(bool mute);
		static void ToggleSoundMusic(bool mute);
		static void SetListenerPosition(float2 pos);
	};
}