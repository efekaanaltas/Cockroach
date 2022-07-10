#pragma once

#include "Cockroach.h"

using namespace Cockroach;

class Player;

template<typename T>
class State
{
public:
	virtual void Enter(T* obj) {};
	virtual State<T>* Update(T* obj, float dt) { return nullptr; }
	virtual void Exit(T* obj) {};
};

class WalkingState : public State<Player>
{
public:
	float gravity = 200.0f, maxFallSpeed = 400.0f;
	float maxWalkSpeed = 200.0f, acceleration = 150.0f, deceleration = 120.0f;

	// Jump buffering
	Timer jumpBufferTimer = Timer(10.0f);
	bool jumpBuffered = false;

	// Coyote jumping
	Timer groundedTimer = Timer(10.0f);
	bool canJump = false;

	virtual State<Player>* Update(Player* player, float dt) override;
};

class JumpingState : public State<Player>
{
public:
	float minJumpSpeed = 0.0f, maxJumpSpeed = 0.0f;

	JumpingState(float minJumpSpeed, float maxJumpSpeed)
		: minJumpSpeed(minJumpSpeed), maxJumpSpeed(maxJumpSpeed)
	{}

	virtual void Enter(Player* player) override;
	virtual State<Player>* Update(Player* player, float dt) override;
};

class ClingingState : public State<Player>
{
public:
	float fallSpeed = 50.0f;

	virtual State<Player>* Update(Player* player, float dt) override;
};

class DashingState : public State<Player>
{
public:
	float dashSpeed = 100.0f;
	Timer dashTimer = Timer(0.5f);

	virtual void Enter(Player* player) override;
	virtual State<Player>* Update(Player* player, float dt) override;
};