#include "crpch.h"
#include "Timer.h"

namespace Cockroach
{
	Timer::Timer(float duration)
	: duration(duration), remainingTime(duration)
	{}

	void Timer::Tick(float dt)
	{
		if (Finished()) return;
		remainingTime -= dt;
	}

	bool Timer::Finished() { return remainingTime <= 0; }
	inline float Timer::ElapsedTime() { return duration - remainingTime; }
	inline float Timer::GetProgress01() { return (duration - remainingTime) / duration; }
	void Timer::Reset() { remainingTime = duration; }
}