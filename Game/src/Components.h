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

	Hitbox(Entity* entity);

	bool enabled = true;

	glm::ivec2 min = { 0, 0 };
	glm::ivec2 max = { 8, 8 };

	glm::ivec2 Hitbox::Position() const { return entity->position; }

	int Hitbox::Left() const { return Position().x + min.x; }
	int Hitbox::Right() const { return Position().x + max.x; }
	int Hitbox::Bottom() const { return Position().y + min.y; }
	int Hitbox::Top() const { return Position().y + max.y; }

	bool OverlapsWith(Hitbox other, int xForesense = 0, int yForesense = 0); // TODO: Const reference of Hitbox can't call functions such as Left(), why?
	bool OverlapsWith(Hitbox other);
	bool OverlapsWith(int2 coord);
};

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
	static std::vector<DynamicObject*> all;

	DynamicObject(Entity* entity);

	DynamicObject* parent = nullptr;
	std::vector<DynamicObject*> children;

	float xRemainder = 0.0f, yRemainder = 0.0f;

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