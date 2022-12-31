#include "crpch.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include "Audio.h"

namespace Cockroach
{
	ma_engine Audio::engine;

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

	void Audio::Play(std::string filepath)
	{
		ma_engine_play_sound(&engine, filepath.c_str(), NULL);
	}

	void Audio::ToggleSound(bool mute)
	{
		ma_engine_set_volume(&engine, mute ? 0.0f : 1.0f);
	}
}