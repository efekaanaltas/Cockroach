#pragma once

#include "Cockroach.h"

using namespace Cockroach;

#include "Entities/Particles.h"

#define CustomReset virtual void Reset() override
#define CustomUpdate virtual void Update(float dt) override
#define CustomRender virtual void Render() override
#define CustomUI virtual void RenderInspectorUI() override
#define CustomDefinition virtual EntityDefinition GenerateDefinition() override

enum DashType;

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
		Dynamic(int2 position, int2 hitboxMin, int2 hitboxMax, bool solid, bool carriable)
			: Entity(position), solid(solid), carriable(carriable)
		{
			hitbox = Rect(hitboxMin, hitboxMax);
		}

		CustomUpdate {}

		Rect hitbox;
		float xRemainder = 0.0f, yRemainder = 0.0f;
		bool solid = true;
		bool carriable = false;

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
		virtual bool OnCollide(Dynamic* other, int horizontal, int vertical) { return solid; }

		Dynamic* GetEntityCollision(int xForesense, int yForesense);
		bool GetTilemapCollision(int xForesense, int yForesense);
		bool GetCollision(int xForesense, int yForesense);
		bool IsRiding(Dynamic* carrier);
	};

	class Carrier : public Dynamic
	{
	public:
		Carrier(int2 position, int2 hitboxMin, int2 hitboxMax)
			: Dynamic(position, hitboxMin, hitboxMax, true, false)
		{}

		virtual void MoveX(float amount) override;
		virtual void MoveY(float amount) override;

		std::vector<Dynamic*> GetRiders();
	};

	class Spike : public Dynamic
	{
	public:
		Spike(int2 position, int2 hitboxMin, int2 hitboxMax, int2 direction)
			: Dynamic(position, hitboxMin, hitboxMax, false, true), direction(direction)
		{}

		int2 direction;

		CustomUpdate;
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

		CustomUpdate;
	};

	class Turbine : public Dynamic
	{
	public:
		Turbine(int2 position, int2 size, int horizontal, int vertical);

		float turbineAcceleration = 300.0f;
		int horizontal = 1, vertical = 0;
		int span = 56;
		Rect turbineRect;
		ParticleSystem airParticles;

		CustomReset;
		CustomUpdate;
		CustomRender;
	};

	class Essence : public Dynamic
	{
	public:
		Essence(int2 position, int2 hitboxMin, int2 hitboxMax, DashType dashType);

		bool active = true;
		DashType dashType;
		Timer refreshTimer = Timer(2.0f);

		CustomUpdate;
		CustomUI;

		CustomDefinition;

		void Absorb();
		void Refresh();
	};

	class Attractor : public Dynamic
	{
	public:
		Attractor(int2 position, int2 hitboxMin, int2 hitboxMax)
			: Dynamic(position, hitboxMin, hitboxMax, false, false)
		{
			dissolveTimer.remainingTime = 0;
		}

		bool active = true;
		bool dissolving = false;
		float attractionRadius = 15.0f;
		float attraction = 200.0f;
		Timer dissolveTimer = Timer(0.5f);
		Timer refreshTimer = Timer(2.0f);

		CustomUpdate;

		void Dissolve();
		void Refresh();
	};

	class Igniter : public Dynamic
	{
	public:
		Igniter(int2 position, int2 size)
			: Dynamic(position, ZEROi, size, true, true)
		{}

		Timer igniteTimer = Timer(1.0f);
		Timer flashTimer = Timer(0.3f);

		CustomUpdate;
		CustomRender;
	};

	class Propeller : public Dynamic
	{
	public:
		Propeller(int2 position, int2 size)
			: Dynamic(position, ZEROi, size, true, true)
		{}

		CustomUpdate {};
		CustomRender;
	};

	class MovingPlatform : public Carrier
	{
	public:
		MovingPlatform(int2 position, int2 size, int2 altPosition);
		
		float startTime;

		int2 startPosition;
		int2 endPosition;

		CustomReset;
		CustomUpdate;
		CustomRender;
		CustomUI;
		
		CustomDefinition;
	};

	class Checkpoint : public Dynamic
	{
	public:
		Checkpoint(int2 position, int2 size)
			: Dynamic(position, ZEROi, size, false, false)
		{}

		CustomUpdate;
		CustomRender {} // Don't render
	};

	class CameraController : public Entity
	{
	public:
		Cockroach::Camera camera;

		CameraController()
			: camera(-aspectRatio * zoom, aspectRatio* zoom, -zoom, zoom)
		{}

		CustomUpdate;
		void StartTransition();
		void SetZoom(float zoom);

		float2 positionHighRes = ZERO;
		float2 targetPosition = ZERO;

		float aspectRatio = 16.0f / 9.0f;
		float zoom = 90.0f;

		float speed = 1.0f;

		bool isTransitioning = false;

		Tween<int2> transitionTween;

		Rect Bounds() { return Rect((float2)camera.GetPosition() - float2(aspectRatio * zoom, zoom), { (float2)camera.GetPosition() + float2(aspectRatio * zoom, zoom) }); }
		int2 RoomBoundedPosition();
	};

	class Decoration : public Entity
	{
	public:
		Decoration(int2 position, int z, int decorationIndex, Sprite sprite)
			: Entity(position), hitbox(ZEROi, { sprite.XSize(), sprite.YSize() }), decorationIndex(decorationIndex)
		{
			size = { sprite.XSize(), sprite.YSize() };
			type = -1;
			this->z = z;
			this->sprite = sprite;
		}

		int decorationIndex;
		Rect hitbox;

		CustomDefinition
		{
			EntityDefinition definition = EntityDefinition(decorationIndex, true, position, size);
			definition.z = z;
			return definition;
		}

		CustomUpdate {}

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