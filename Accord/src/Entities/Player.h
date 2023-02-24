#pragma once

#include "../State.h"

template<typename T>
class State;
class WalkingState;
class JumpingState;
class ClingingState;
class DashingState;
class RollingState;

struct BufferedInput
{
	BufferedInput(u16 keycode, int bufferFrames)
		: keycode(keycode), bufferFramesTimer((float)bufferFrames)
	{
	};

	u16 keycode;
	Timer bufferFramesTimer;

	void Update()
	{
		if (Input::IsDown(keycode))
			bufferFramesTimer.Reset();
		else
			bufferFramesTimer.Tick(1.0f);
	}

	void Cancel() { bufferFramesTimer.remainingTime = 0.0f; }
	bool Active() { return !bufferFramesTimer.Finished(); }
};

namespace Entities
{
	class Player : public Dynamic
	{
	public:
		Player(int2 position, int2 hitboxMin, int2 hitboxMax);

		virtual void Update(float dt) override;
		virtual void Render() override;

		int faceDir = 1;
		float gravity = 200.0f;
		bool grounded = false;
		bool canDash = false;

		bool lookingForCheckpoint = false;
		int2 checkpointPosition = ZEROi;

		float2 velocity = ZERO;
		float2 velocityLastFrame = ZERO;

		State<Player>* currentState = nullptr;
		WalkingState* walkingState = nullptr;
		JumpingState* jumpingState = nullptr;
		JumpingState* superjumpingState = nullptr;
		JumpingState* rolljumpingState = nullptr;
		JumpingState* walljumpingState = nullptr;
		JumpingState* ledgeJumpingState = nullptr;
		ClingingState* clingingState = nullptr;
		DashingState* dashingState = nullptr;
		RollingState* rollingState = nullptr;

		float2 renderSize = ONE;

		Sheet idleSheet, walkingSheet, fallingSheet, jumpingSheet, clingingSheet, dashingSheet, rollingSheet;
		Sheet currentSheet;

		BufferedInput bufferedJumpInput = BufferedInput(CR_KEY_SPACE, 6);
		BufferedInput bufferedDashInput = BufferedInput(CR_KEY_LEFT_SHIFT, 10);

		Timer coyoteTimer = Timer(10.0f);
		Timer flashTimer = Timer(5.0f);
		Timer gravityHaltTimer = Timer(10.0f);
		Timer dashRegainTimer = Timer(4.0f);

		virtual bool OnCollide(Dynamic* other, int horizontal, int vertical) override;

		int InputDirX() const;
		int InputDirY() const;
		int WallDir();

		void TrySwitchState(State<Player>* state);

		void TryChangeRoom();
		void RegainDash();
		void Die();
	};
}
