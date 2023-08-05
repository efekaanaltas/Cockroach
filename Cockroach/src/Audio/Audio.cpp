#include "crpch.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include "Audio.h"

namespace Cockroach
{
	ma_engine Audio::engine;
	ma_sound_group Audio::sfxGroup;
	ma_sound_group Audio::musicGroup;

	void Audio::Init()
	{
        ma_result result;

		result = ma_engine_init(NULL, &engine);
		
		if (result != MA_SUCCESS)
		{
			CR_CORE_WARN("Failed to initialize audio engine.");
			return;
		}
	}

	void Audio::PlayOneShot(std::string filepath)
	{
		PlayOneShot(filepath, sfxGroup);
	}

	void Audio::PlayOneShot(std::string filepath, ma_sound_group group)
	{
		ma_engine_play_sound(&engine, filepath.c_str(), NULL);
	}

	void Audio::ToggleSound(bool mute)
	{
		ma_engine_set_volume(&engine, mute ? 0.0f : 1.0f);
	}

	void Audio::SetListenerPosition(float2 pos)
	{
		ma_engine_listener_set_position(&engine, 0, pos.x, pos.y, 0);
	}
}