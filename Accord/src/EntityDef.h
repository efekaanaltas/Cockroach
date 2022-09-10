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

	Sprite CurrentSprite()
	{
		int index = (int)std::fmodf(Application::Get().frameCount * framePerSecond / 60.0f, (float)sheet.size());
		return sheet[index];
	}
};

class Dynamic : public Entity
{
public:
	Dynamic(int2 position, int2 hitboxMin, int2 hitboxMax)
		: Entity(position)
	{
		hitbox = Rect(hitboxMin, hitboxMax);
	}

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

	Dynamic* GetEntityCollision(int xForesense, int yForesense);
	bool GetTilemapCollision(int xForesense, int yForesense);
	bool GetCollision(int xForesense, int yForesense);
};

class Trigger : public Dynamic
{
public:
	Trigger(int2 position, int2 hitboxMin, int2 hitboxMax)
		: Dynamic(position, hitboxMin, hitboxMax)
	{
	}

	virtual void Update(float dt) override;

	virtual bool OnCollide(Dynamic* other, int horizontal, int vertical) { return false; }
};

class OscillatorA : public Dynamic
{
public:
	OscillatorA(int2 position, int2 hitboxMin, int2 hitboxMax)
		: Dynamic(position, hitboxMin, hitboxMax), startPos(position)
	{}

	int2 startPos;
	int moveDir = 1;

	virtual void Update(float dt) override
	{
		if (std::abs(startPos.x - position.x) > 10)
		{
			position.x = std::clamp(position.x, startPos.x-10, startPos.x+10);
			moveDir *= -1;
		}

		MoveX(moveDir * dt * 50);
	}

	virtual bool OnCollide(Dynamic* other, int hotizontal, int vertical) override { return false; }
};

class CameraController : public Entity
{
public:
	Cockroach::Camera camera;

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