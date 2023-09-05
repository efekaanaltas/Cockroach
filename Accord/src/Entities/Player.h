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

struct DashTrail
{
	DashTrail()
		: sprite()
	{}

	DashTrail(Sprite sprite, int2 position, bool flipX)
		: sprite(sprite), strength(1.0f), position(position), flipX(flipX)
	{}
	
	static const int count = 20;

	Sprite sprite;
	float strength = 1.0f;
	int2 position = ZEROi;
	bool flipX = false;
};

enum DashType
{
	Dash, Drift, Propel, Virtual, Regen, Flashback
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

		DashType currentDashType = Dash;

		State<Player>* currentState = nullptr;
		WalkingState* walkingState = nullptr;
		JumpingState* jumpingState = nullptr;
		JumpingState* superjumpingState = nullptr;
		JumpingState* rolljumpingState = nullptr;
		JumpingState* walljumpingState = nullptr;
		JumpingState* ledgeJumpingState = nullptr;
		ClingingState* clingingState = nullptr;
		DashingState* dashingState = nullptr;
		DashingState* driftingState = nullptr;
		RollingState* rollingState = nullptr;

		float2 renderSize = ONE;

		Sheet idleSheet, walkingSheet, fallingSheet, jumpingSheet, clingingSheet, dashingSheet, rollingSheet;
		Sheet currentSheet;

		BufferedInput bufferedJumpInput = BufferedInput(CR_KEY_SPACE, 6);
		BufferedInput bufferedDashInput = BufferedInput(CR_KEY_LEFT_SHIFT, 20);// 15);

		Timer coyoteTimer = Timer(10.0f);
		Timer flashTimer = Timer(5.0f);
		Timer gravityHaltTimer = Timer(10.0f);
		Timer dashRegainTimer = Timer(4.0f);

		Sound dashSound = Sound("assets/audio/Dash.wav");
		Sound jumpSound = Sound("assets/audio/Jump.wav");

		DashTrail dashTrail[DashTrail::count];
		int lastDashTrailIndex = 0;

		virtual bool OnCollide(Dynamic* other, int horizontal, int vertical) override;

		int InputDirX() const;
		int InputDirY() const;
		int WallDir();

		void TrySwitchState(State<Player>* state);

		void TryChangeRoom();
		void RegainDash(DashType dashType = Dash);
		void Die();
		void CreateDashTrail();
	};
}
