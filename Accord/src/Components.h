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

	void Update(Entity* e)
	{
		int index = (int)std::fmodf(Application::Get().frameCount * framePerSecond / 60.0f, (float)sheet.size());
		e->sprite = sheet[index];
	}
};

enum CollisionLayer
{
	Trigger = 0b001, Light = 0b010, Heavy = 0b100, All = 0b111
};

class Dynamic : public Entity
{
public:
	CollisionLayer layer = CollisionLayer::Light;

	virtual void Update(float dt) override {}

	Rect hitbox;
	float xRemainder = 0.0f, yRemainder = 0.0f;

	int Left() const { return position.x + hitbox.min.x; }
	int Right() const { return position.x + hitbox.max.x; }
	int Bottom() const { return position.y + hitbox.min.y; }
	int Top() const { return position.y + hitbox.max.y; }

	Rect WorldHitbox() const { return Rect(hitbox.min + position, hitbox.max + position); }

	bool OverlapsWith(Dynamic* other, int xForesense, int yForesense) const { return WorldHitbox().OverlapsWith(other->WorldHitbox(), xForesense, yForesense); }
	bool Contains(Dynamic* other, int xForesense, int yForesense) const { return WorldHitbox().Contains(other->WorldHitbox(), xForesense, yForesense); }
	bool Contains(int2 coord) const { return WorldHitbox().Contains(coord); }

	int MoveX(float amount);
	int MoveY(float amount);
	virtual bool OnCollide(Dynamic* other, int horizontal, int vertical) { return false; }

	Dynamic* GetEntityCollision(int xForesense, int yForesense, CollisionLayer layer = CollisionLayer::All);
	bool GetTilemapCollision(int xForesense, int yForesense);
	bool GetCollision(int xForesense, int yForesense, CollisionLayer layer = CollisionLayer::All);
};

class Player : public Dynamic
{
public:
	Player();

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

	virtual bool OnCollide(Dynamic* other, int horizontal, int vertical) override;

	int InputDirX() const;
	int InputDirY() const;
	int WallDir();

	void TrySwitchState(State<Player>* state);
};

class CameraController : public Entity
{
public:
	Camera camera;

	CameraController()
		: camera(-aspectRatio * zoom, aspectRatio* zoom, -zoom, zoom)
	{}

	virtual void Update(float dt) override;

	void SetZoom(float zoom);

	float2 positionHighRes = { 0.0f, 0.0f };

	float aspectRatio = 16.0f / 9.0f;
	float zoom = 100.0f;

	float speed = 1.0f;

	Rect Bounds() { return Rect((float2)camera.GetPosition() - float2(aspectRatio * zoom, zoom), { (float2)camera.GetPosition() + float2(aspectRatio * zoom, zoom) }); }
};