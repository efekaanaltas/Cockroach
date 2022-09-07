#pragma once

namespace Cockroach
{
	class Timer
	{
	public:

		float duration, remainingTime;

		Timer(float duration);

		void Tick(float dt);

		bool Finished();
		float ElapsedTime();
		float Progress01();
		void Reset();
	};
}