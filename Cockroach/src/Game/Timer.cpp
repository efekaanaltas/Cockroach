#include "crpch.h"
#include "Timer.h"
#include "../Core/Application.h"
#include "Cockroach.h"

namespace Cockroach
{
	Timer::Timer(float duration, TimerType type, bool initializeFinished)
	: type(type), duration(duration)
	{
		if (initializeFinished)
			finishTime = Time();
		else
			finishTime = Time() + duration;
	}

	float Timer::Time()
	{
		switch (type)
		{
		case TimerType::seconds: return time;
		case TimerType::frames: return frameCount;
		case TimerType::secondsUnscaled: return timeUnscaled;
		}
		return 99999999999.9f;
	}

	bool Timer::Finished(bool autoReset) 
	{
		if (Time() < finishTime) return false;
		if (autoReset) Reset();

		return true;
	}

	float Timer::RemainingTime() { return std::clamp(finishTime - Time(), 0.0f, duration); }
	float Timer::ElapsedTime() { return duration - RemainingTime(); }
	float Timer::Progress01() { return (duration - RemainingTime()) / duration; }
	void Timer::Reset() { finishTime = Time() + duration; }
	void Timer::ForceFinish() { finishTime = Time(); }
}