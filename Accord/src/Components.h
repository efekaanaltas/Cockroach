#pragma once

#include "Cockroach.h"

using namespace Cockroach;

template<typename T>
class State;
class WalkingState;
class JumpingState;
class ClingingState;
class DashingState;

struct Sheet
{
	std::vector<Sprite> sheet;
	int framePerSecond = 4;

	void Add(const Sprite& sprite) { sheet.push_back(sprite); }
	int Size() const { return (int)sheet.size(); }
	Sprite operator[](int i) { return sheet[i]; }
};
class Animator : public Component
{
public:
	Animator(Entity* entity)
		: Component(entity)
	{}

	virtual void Update(float dt) override;

	Sheet sheet;
};

enum CollisionLayer
{
	Trigger = 0b001, Light = 0b010, Heavy = 0b100, All = 0b111
};
class DynamicObject : public Component
{
public:

	DynamicObject(Entity* entity);

	CollisionLayer layer = CollisionLayer::Light;

	DynamicObject* parent = nullptr;
	std::vector<DynamicObject*> children;

	Rect hitbox;
	float xRemainder = 0.0f, yRemainder = 0.0f;

	int Left() const { return entity->position.x + hitbox.min.x; }
	int Right() const { return entity->position.x + hitbox.max.x; }
	int Bottom() const { return entity->position.y + hitbox.min.y; }
	int Top() const { return entity->position.y + hitbox.max.y; }

	Rect WorldHitbox() const { return Rect(hitbox.min + entity->position, hitbox.max + entity->position); }

	bool OverlapsWith(Ref<DynamicObject> other, int xForesense, int yForesense) const { return WorldHitbox().OverlapsWith(other->WorldHitbox(), xForesense, yForesense); }
	bool Contains(Ref<DynamicObject> other, int xForesense, int yForesense) const { return WorldHitbox().Contains(other->WorldHitbox(), xForesense, yForesense); }
	bool Contains(int2 coord) const { return WorldHitbox().Contains(coord); }

	int MoveX(float amount);
	int MoveY(float amount);
	virtual bool OnCollide(Ref<DynamicObject> other, int horizontal, int vertical) = 0;

	Ref<DynamicObject> GetEntityCollision(int xForesense, int yForesense, CollisionLayer layer = CollisionLayer::All);
	bool GetTilemapCollision(int xForesense, int yForesense);
	bool GetCollision(int xForesense, int yForesense, CollisionLayer layer = CollisionLayer::All);
};

class Player : public DynamicObject
{
public:
	Player(Entity* entity);
	virtual void Update(float dt) override;

	int faceDir = 1; // -1 for left, 1 for right
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
	Ref<Animator> animator = entity->GetComponent<Animator>();

	Timer jumpBufferTimer = Timer(10.0f);
	Timer coyoteTimer = Timer(10.0f);

	virtual bool OnCollide(Ref<DynamicObject> other, int horizontal, int vertical) override;

	int InputDirX() const;
	int InputDirY() const;
	int WallDir();

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

class CameraController : public Component
{
public:
	CameraController(Entity* entity);

	virtual void Update(float dt) override;

	void SetZoom(float zoom);

	float2 positionHighRes = { 0.0f, 0.0f };
	
	float aspectRatio = 16.0f/9.0f;
	float zoom = 100.0f;

	float speed = 1.0f;

	Rect Bounds() { return Rect((float2)camera.GetPosition() - float2(aspectRatio * zoom, zoom), { (float2)camera.GetPosition() + float2(aspectRatio * zoom, zoom)}); }
public:
	Camera camera;
};