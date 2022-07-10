#pragma once

namespace Cockroach
{
	class Timer
	{
	public:

		float duration, remainingTime;

		Timer(float duration);

		void Tick(float dt);

		inline bool Finished();
		inline float ElapsedTime();
		inline float GetProgress01();
		void Reset();
	};
}