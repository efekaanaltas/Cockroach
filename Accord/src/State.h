#pragma once

#include "Cockroach.h"

using namespace Cockroach;

#include "EntityDef.h"

namespace Entities
{
	class Player;

	template<typename T>
	class State
	{
	public:
		virtual void Enter(T* obj) {};
		virtual State<T>* Update(T* obj) { return nullptr; }
		virtual void Exit(T* obj) {};
	};

	class WalkingState : public State<Player>
	{
	public:
		float gravity = 400.0f, maxFallSpeed = 150.0f;
	float maxWalkSpeed = 60.0f, acceleration = 1400.0f, deceleration = 1000.0f, airDeceleration = 700.0f;

		virtual void Enter(Player* player) override;
		virtual State<Player>* Update(Player* player) override;
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
		virtual State<Player>* Update(Player* player) override;
	};

	class ClingingState : public State<Player>
	{
	public:
		float reducedGravity = 100.0f;
		float fallSpeed = 50.0f;
		float jumpExhaustionTime = 0.1f;
		float impactSpeed;

		virtual void Enter(Player* player) override;
		virtual State<Player>* Update(Player* player) override;
	};

	class DashingState : public State<Player>
	{
	public:
		float dashSpeed = 150.0f;
		Timer dashTimer = Timer(0.2f);
		float2 dashDir = RIGHT;

		DashingState(float dashSpeed, float dashDuration)
			: dashSpeed(dashSpeed), dashTimer(dashDuration)
		{}
	
		virtual void Enter(Player* player) override;
		virtual State<Player>* Update(Player* player) override;
		virtual void Exit(Player* player) override;
	};

	class RollingState : public State<Player>
	{
	public:
		float maxRollSpeed = 170.0f;
		float groundRollAcceleration = 80.0f;
		float airRollAcceleration = 50.0f;

		virtual void Enter(Player* player) override;
		virtual State<Player>* Update(Player* player) override;
		virtual void Exit(Player* player) override;
	};
}
