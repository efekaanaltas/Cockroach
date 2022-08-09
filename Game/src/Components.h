#pragma once

#include "Cockroach.h"

using namespace Cockroach;

template<typename T>
class State;
class WalkingState;
class JumpingState;
class ClingingState;
class DashingState;

using Sheet = std::vector<Ref<SubTexture2D>>;
class Animator : public Component
{
public:
	Animator(Entity* entity)
		: Component(entity)
	{}

	virtual void Update(float dt) override;

	Sheet sheet;
	int framePerSecond = 3;
};

class DynamicObject : public Component
{
public:
	DynamicObject(Entity* entity);

	DynamicObject* parent = nullptr;
	std::vector<DynamicObject*> children;

	Rect hitbox;
	float xRemainder = 0.0f, yRemainder = 0.0f;

	int Left() const { return entity->position.x + hitbox.min.x; }
	int Right() const { return entity->position.x + hitbox.max.x; }
	int Bottom() const { return entity->position.y + hitbox.min.y; }
	int Top() const { return entity->position.y + hitbox.max.y; }

	bool OverlapsWith(Ref<DynamicObject> other, int xForesense, int yForesense) const
	{ return hitbox.OverlapsWith(other->hitbox, entity->position.x + xForesense, entity->position.y + yForesense); }

	bool Contains(Ref<DynamicObject> other, int xForesense, int yForesense) const
	{ return hitbox.Contains(other->hitbox, entity->position.x + xForesense, entity->position.y + yForesense); }

	bool Contains(int2 coord) const { return hitbox.Contains(coord); }

	int MoveX(float amount);
	int MoveY(float amount);
	virtual bool OnCollide(Ref<DynamicObject> other, int horizontal, int vertical) = 0;

	Ref<DynamicObject> GetEntityCollision(int xForesense, int yForesense);
	bool GetTilemapCollision(int xForesense, int yForesense);
	bool GetCollision(int xForesense, int yForesense);
};

class Player : public DynamicObject
{
public:
	Player(Entity* entity);
	virtual void Update(float dt) override;

	int faceDir = 1; // -1 for left, 1 for right
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

	Sheet idleSheet, walkingSheet, fallingSheet, jumpingSheet, clingingSheet, dashingSheet;
	Ref<Animator> animator = entity->GetComponent<Animator>();

	Timer jumpBufferTimer = Timer(10.0f);
	Timer coyoteTimer = Timer(10.0f);

	virtual bool OnCollide(Ref<DynamicObject> other, int horizontal, int vertical) override;

	int InputDirX() const;
	int InputDirY() const;
	bool NextToWall();

	void TrySwitchState(State<Player>* newState);
};

class Hazard : public DynamicObject
{
public:
	Hazard(Entity* entity)
		: DynamicObject(entity)
	{}

	virtual bool OnCollide(Ref<DynamicObject> other, int horizontal, int vertical) override { return true; }
};

class Pusher : public DynamicObject
{
public:
	Pusher(Entity* entity)
		: DynamicObject(entity)
	{}

	int dir = 1;

	virtual void Update(float dt) override
	{
		if (entity->position.y > 10 || entity->position.y < -10)
		{
			entity->position.y = std::clamp(entity->position.y, -10, 10);
			dir = dir * -1;
		}
		MoveY(5 * dir * dt);
	}

	virtual bool OnCollide(Ref<DynamicObject> other, int horizontal, int vertical) override;
};

class CameraController : public Component
{
public:
	CameraController(Entity* entity);

	virtual void Update(float dt) override;

	float2 positionHighRes = { 0.0f, 0.0f };
	
	float aspectRatio = 16.0f/9.0f;
	float zoom = 10.0f;

	float speed = 1.0f;
public:
	Camera camera;
};