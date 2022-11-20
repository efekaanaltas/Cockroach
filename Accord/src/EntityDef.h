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

namespace Entities
{
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
		bool blockOnCollision = true;

		int Left() const { return position.x + hitbox.min.x; }
		int Right() const { return position.x + hitbox.max.x; }
		int Bottom() const { return position.y + hitbox.min.y; }
		int Top() const { return position.y + hitbox.max.y; }

		Rect WorldHitbox() const { return Rect(hitbox.min + position, hitbox.max + position); }

		bool OverlapsWith(Dynamic* other, int xForesense, int yForesense) const { return WorldHitbox().OverlapsWith(other->WorldHitbox(), xForesense, yForesense); }
		bool Contains(Dynamic* other, int xForesense, int yForesense) const { return WorldHitbox().Contains(other->WorldHitbox(), xForesense, yForesense); }
		bool Contains(int2 coord) const { return WorldHitbox().Contains(coord); }

		virtual int MoveX(float amount);
		virtual int MoveY(float amount);
		virtual bool OnCollide(Dynamic* other, int horizontal, int vertical) { return blockOnCollision; }

		Dynamic* GetEntityCollision(int xForesense, int yForesense);
		bool GetTilemapCollision(int xForesense, int yForesense);
		bool GetCollision(int xForesense, int yForesense);
		bool IsRiding(Dynamic* carrier);
	};

	class Carrier : public Dynamic
	{
	public:
		Carrier(int2 position, int2 hitboxMin, int2 hitboxMax)
			: Dynamic(position, hitboxMin, hitboxMax)
		{}

		virtual int MoveX(float amount) override;
		virtual int MoveY(float amount) override;

		std::vector<Dynamic*> GetRiders();
	};

	class Spike : public Dynamic
	{
	public:
		Spike(int2 position, int2 hitboxMin, int2 hitboxMax)
			: Dynamic(position, hitboxMin, hitboxMax)
		{
			blockOnCollision = false;
		}

		virtual void Update(float dt) override;
	};

	class OscillatorA : public Carrier
	{
	public:
		OscillatorA(int2 position, int2 hitboxMin, int2 hitboxMax)
			: Carrier(position, hitboxMin, hitboxMax), startPos(position)
		{}

		int2 startPos;
		int moveDir = 1;

		virtual void Update(float dt) override;
	};

	class Turbine : public Dynamic
	{
	public:
		Turbine(int2 position, int2 hitboxMin, int2 hitboxMax, int horizontal, int vertical);

		float turbineAcceleration = 200.0f;
		int horizontal = 1, vertical = 0;
		int span = 56;
		Rect turbineRect;

		virtual void Update(float dt) override;
	};

	class EssenceRed : public Dynamic
	{
	public:
		EssenceRed(int2 position, int2 hitboxMin, int2 hitboxMax);

		bool active = true;
		Timer respawnTimer = Timer(2.0f);

		virtual void Update(float dt) override;

		void Absorb();
		void Refresh();
	};

	class Igniter : public Dynamic
	{
	public:
		Igniter(int2 position, int2 size)
			: Dynamic(position, int2(0, 0), size)
		{}

		Timer igniteTimer = Timer(2.0f);
		Timer flashTimer = Timer(0.3f);

		virtual void Update(float dt) override;
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

		bool editMode = true;

		Rect Bounds() { return Rect((float2)camera.GetPosition() - float2(aspectRatio * zoom, zoom), { (float2)camera.GetPosition() + float2(aspectRatio * zoom, zoom) }); }
	};
}