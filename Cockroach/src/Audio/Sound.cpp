#include "crpch.h"
#include "Sound.h"

namespace Cockroach
{
	Sound::Sound(const std::string& filepath, bool isMusic)
	{
		MA_ASSERT(ma_sound_init_from_file(&Audio::engine, filepath.c_str(), MA_SOUND_FLAG_NO_SPATIALIZATION, isMusic ? &Audio::musicGroup : &Audio::sfxGroup, nullptr, &sound),
				  "Sound could not be loaded.");
	}

	Sound::~Sound()
	{
		ma_sound_uninit(&sound);
	}

	void Sound::Start()
	{
		ma_sound_start(&sound);
	}

	void Sound::Stop()
	{
		ma_sound_stop(&sound);
	}

	void Sound::SetPosition(float2 pos)
	{
		ma_sound_set_position(&sound, pos.x, pos.y, 0);
	}
}