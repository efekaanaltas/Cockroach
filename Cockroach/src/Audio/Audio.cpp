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
		MA_CHECK(ma_engine_init(NULL, &engine),						 "Failed to initialize audio engine");
		MA_CHECK(ma_sound_group_init(&engine, 0, nullptr, &sfxGroup),	 "Failed to initilize SFX sound group");
		MA_CHECK(ma_sound_group_init(&engine, 0, nullptr, &musicGroup), "Failed to initilize music sound group");
	}

	void Audio::PlayOneShot(std::string filepath)
	{
		PlayOneShot(filepath, sfxGroup);
	}

	void Audio::PlayOneShot(std::string filepath, ma_sound_group group)
	{
		ma_engine_play_sound(&engine, filepath.c_str(), NULL);
	}

	void Audio::ToggleSoundMaster(bool mute)
	{
		ma_engine_set_volume(&engine, mute ? 0.0f : 1.0f);
	}

	void Audio::ToggleSoundSFX(bool mute)
	{
		ma_sound_group_set_volume(&sfxGroup, mute ? 0.0f : 1.0f);
	}

	void Audio::ToggleSoundMusic(bool mute)
	{
		ma_sound_group_set_volume(&musicGroup, mute ? 0.0f : 1.0f);
	}

	void Audio::SetListenerPosition(float2 pos)
	{
		ma_engine_listener_set_position(&engine, 0, pos.x, pos.y, 0);
	}
}