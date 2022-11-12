#pragma once

#include "Cockroach.h"

using namespace Cockroach;

class Player;
#include "EntityDef.h"

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
	float gravity = 300.0f, maxFallSpeed = 200.0f;
	float maxWalkSpeed = 60.0f, acceleration = 400.0f, deceleration = 1000.0f;

	virtual void Enter(Player* player) override;
	virtual State<Player>* Update(Player* player, float dt) override;
};

class JumpingState : public WalkingState
{
public:
	float minJumpSpeed = 100.0f, maxJumpSpeed = 160.0f;
	float horizontalBoost = 0.0f;

	Sheet jumpingSheet;

	JumpingState(float minJumpSpeed, float maxJumpSpeed, float horizontalBoost)
		: WalkingState(), minJumpSpeed(minJumpSpeed), maxJumpSpeed(maxJumpSpeed), horizontalBoost(horizontalBoost)
	{}

	virtual void Enter(Player* player) override;
	virtual State<Player>* Update(Player* player, float dt) override;
};

class ClingingState : public State<Player>
{
public:
	float reducedGravity = 100.0f;
	float fallSpeed = 50.0f;
	float jumpExhaustionTime = 0.1f;

	virtual void Enter(Player* player) override;
	virtual State<Player>* Update(Player* player, float dt) override;
};

class DashingState : public State<Player>
{
public:
	float dashSpeed = 150.0f;
	Timer dashTimer = Timer(0.2f);
	float2 dashDir = { 1.0f, 0.0f };

	virtual void Enter(Player* player) override;
	virtual State<Player>* Update(Player* player, float dt) override;
	virtual void Exit(Player* player) override;
};