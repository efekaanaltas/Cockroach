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
	void RenderDynamicSizedEntity(Entity* entity, int2 texCoordOffset);

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

		virtual void MoveX(float amount);
		virtual void MoveY(float amount);
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

		virtual void MoveX(float amount) override;
		virtual void MoveY(float amount) override;

		std::vector<Dynamic*> GetRiders();
	};

	class Spike : public Dynamic
	{
	public:
		Spike(int2 position, int2 hitboxMin, int2 hitboxMax, int2 direction)
			: Dynamic(position, hitboxMin, hitboxMax), direction(direction)
		{
			blockOnCollision = false;
		}

		int2 direction;

		virtual void Update(float dt) override;
	};

	class OscillatorA : public Carrier
	{
	public:
		OscillatorA(int2 position, int2 hitboxMin, int2 hitboxMax)
			: Carrier(position, hitboxMin, hitboxMax), startPos(position)
		{
			overlayWeight = 0.5f;
			overlayColor = GREEN;
		}

		int2 startPos;
		int moveDir = 1;

		virtual void Update(float dt) override;
	};

	class Turbine : public Dynamic
	{
	public:
		Turbine(int2 position, int2 size, int horizontal, int vertical);

		float turbineAcceleration = 300.0f;
		int horizontal = 1, vertical = 0;
		int span = 56;
		Rect turbineRect;

		virtual void Update(float dt) override;
		virtual void Render() override;
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
			: Dynamic(position, ZEROi, size)
		{}

		Timer igniteTimer = Timer(1.0f);
		Timer flashTimer = Timer(0.3f);

		virtual void Update(float dt) override;
		virtual void Render() override;
	};

	class Propeller : public Dynamic
	{
	public:
		Propeller(int2 position, int2 size)
			: Dynamic(position, ZEROi, size)
		{
			blockOnCollision = true;
		}

		virtual void Update(float dt) override {};
		virtual void Render() override;
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

		float2 positionHighRes = ZERO;

		float aspectRatio = 16.0f / 9.0f;
		float zoom = 90.0f;

		float speed = 1.0f;

		Rect Bounds() { return Rect((float2)camera.GetPosition() - float2(aspectRatio * zoom, zoom), { (float2)camera.GetPosition() + float2(aspectRatio * zoom, zoom) }); }
	};

	class Decoration : public Entity
	{
	public:
		Decoration(int2 position, int2 size)
			: Entity(position), hitbox(ZEROi, size)
		{}

		int decorationIndex;
		Rect hitbox;

		virtual std::string GenerateDefinitionString() override
		{
			return GenerateProperty("D", decorationIndex)
				+ GenerateProperty("X", position.x) + GenerateProperty("Y", position.y) + GenerateProperty("Z", z) + "\n";
		}

		virtual void Update(float dt) override {}

		int Left() const { return position.x + hitbox.min.x; }
		int Right() const { return position.x + hitbox.max.x; }
		int Bottom() const { return position.y + hitbox.min.y; }
		int Top() const { return position.y + hitbox.max.y; }

		Rect WorldHitbox() const { return Rect(hitbox.min + position, hitbox.max + position); }

		bool OverlapsWith(Dynamic* other, int xForesense, int yForesense) const { return WorldHitbox().OverlapsWith(other->WorldHitbox(), xForesense, yForesense); }
		bool Contains(Dynamic* other, int xForesense, int yForesense) const { return WorldHitbox().Contains(other->WorldHitbox(), xForesense, yForesense); }
		bool Contains(int2 coord) const { return WorldHitbox().Contains(coord); }
	};

}