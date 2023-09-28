#pragma once

namespace Cockroach
{
	enum TimerType { seconds, secondsUnscaled, frames };

	class Timer
	{
	public:
		TimerType type;
		float duration, finishTime;

		Timer(float duration, TimerType type = TimerType::seconds, bool initializeFinished = false);

		float Time();

		bool Finished(bool autoReset = false);
		float RemainingTime();
		float ElapsedTime();
		float Progress01();
		void Reset();
		void ForceFinish();
	};
}