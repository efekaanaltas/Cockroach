#pragma once
#include <miniaudio.h>

namespace Cockroach
{
	class Audio
	{
	public:
		static ma_engine engine;

		static void Init();
		static void Play(std::string filepath);
		static void ToggleSound(bool mute);
	};
}