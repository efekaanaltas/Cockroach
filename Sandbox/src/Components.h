#pragma once

#include "Cockroach.h"

using namespace Cockroach;

template<typename T>
class State;
class WalkingState;
class JumpingState;
class ClingingState;
class DashingState;

class Hitbox : public Component
{
public:
	static std::vector<Hitbox*> all;

	Hitbox();

	bool enabled = true;

	glm::ivec2 min = { 0, 0 };
	glm::ivec2 max = { 8, 8 };

	glm::ivec2 Hitbox::Position() const { return entity->position; }

	int32_t Hitbox::Left() const { return Position().x + min.x; }
	int32_t Hitbox::Right() const { return Position().x + max.x; }
	int32_t Hitbox::Bottom() const { return Position().y + min.y; }
	int32_t Hitbox::Top() const { return Position().y + max.y; }

	bool OverlapsWith(Hitbox other, float xForesense = 0, float yForesense = 0); // TODO: Const reference of Hitbox can't call functions such as Left(), why?
	bool Contains(Hitbox other);
	bool Contains(int2 coord);
};

class StaticObject : public Component
{
public:
	static std::vector<StaticObject*> all;

	StaticObject();
};

class DynamicObject : public Component
{
public:
	static std::vector<DynamicObject*> all;

	DynamicObject();

	DynamicObject* parent = nullptr;
	std::vector<DynamicObject*> children;

	float xRemainder = 0.0f, yRemainder = 0.0f;

	i8 MoveX(float amount);
	i8 MoveY(float amount);
	virtual void OnCollide(Ref<DynamicObject> other, bool horizontalCollision) = 0;
	Hitbox* GetCollidingHitbox(i32 xForesense, i32 yForesense);
};

class Player : public DynamicObject
{
public:
	Player();
	virtual void Update(float dt) override;

	i8 faceDir = 1; // -1 for left, 1 for right
	float gravity = 200.0f;
	bool grounded = false;

	float2 velocity = { 0.0f, 0.0f };
	float2 velocityLastFrame = { 0.0f, 0.0f };

	State<Player>* currentState = nullptr;
	WalkingState* walkingState = nullptr;
	JumpingState* jumpingState = nullptr;
	JumpingState* superjumpingState = nullptr;
	JumpingState* walljumpingState = nullptr;
	JumpingState* climbingState = nullptr;
	ClingingState* clingingState = nullptr;
	DashingState* dashingState = nullptr;

	Timer jumpBufferTimer = Timer(10.0f);
	Timer coyoteTimer = Timer(10.0f);

	virtual void OnCollide(Ref<DynamicObject> other, bool horizontalCollision) override;

	i8 InputDirX() const;
	i8 InputDirY() const;
	bool NextToWall();

	void TrySwitchState(State<Player>* newState);
};

class Hazard : public DynamicObject
{
public:
	Hazard() {}
};