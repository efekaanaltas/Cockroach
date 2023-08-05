#pragma once
#include "Audio.h"

namespace Cockroach
{
	class Sound
	{
	public:
		ma_sound sound;

		Sound(const std::string& filepath, bool isMusic = false);
		~Sound();

		void Start();
		void Stop();
		void SetPosition(float2 pos);
	};
}