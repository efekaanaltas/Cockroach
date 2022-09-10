#pragma once

#include "../EntityDef.h"
#include "../State.h"

template<typename T>
class State;
class WalkingState;
class JumpingState;
class ClingingState;
class DashingState;

class Player : public Dynamic
{
public:
	Player(int2 position, int2 hitboxMin, int2 hitboxMax);

	virtual void Update(float dt) override;

	int faceDir = 1;
	float gravity = 200.0f;
	bool grounded = false;
	bool canDash = false;

	float2 velocity = { 0.0f, 0.0f };
	float2 velocityLastFrame = { 0.0f, 0.0f };

	State<Player>* currentState = nullptr;
	WalkingState* walkingState = nullptr;
	JumpingState* jumpingState = nullptr;
	JumpingState* superjumpingState = nullptr;
	JumpingState* walljumpingState = nullptr;
	ClingingState* clingingState = nullptr;
	DashingState* dashingState = nullptr;

	Sheet idleSheet, walkingSheet, fallingSheet, jumpingSheet, clingingSheet, dashingSheet;
	Sheet currentSheet;

	Timer jumpBufferTimer = Timer(10.0f);
	Timer coyoteTimer = Timer(10.0f);
	Timer flashTimer = Timer(5.0f);

	bool transitioning = false;

	virtual bool OnCollide(Dynamic* other, int horizontal, int vertical) override;
	void OnTrigger(Trigger* trigger);

	int InputDirX() const;
	int InputDirY() const;
	int WallDir();

	void TrySwitchState(State<Player>* state);
};