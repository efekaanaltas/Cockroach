#pragma once

#include "Cockroach.h"

using namespace Cockroach;

#include "Entities/Particles.h"

// As far as I'm aware, C++ doesn't have a simple way to get the name of an enum value, and having to manually update a string array when adding a new entity is cumbersome.
// We use a macro that generates the corresponding string list given the enum definition.
#define DECLARE_ENTITY_TYPE_ENUM(ENUM_NAME, ...)  \
	enum ENUM_NAME{ __VA_ARGS__ }; \
	static vector<string> entityTypeNames = Cockroach::Split( #__VA_ARGS__ );

DECLARE_ENTITY_TYPE_ENUM(EntityType, Payga, Camera, Particles, SpikeLeft, SpikeRight, SpikeDown, SpikeUp, Oscillator,
	TurbineLeft, TurbineRight, TurbineDown, TurbineUp, Essence, Igniter, Propeller, MovingPlatform,
	Attractor, Checkpoint, DashSwitchPlatform, END);

#define CustomReset virtual void Reset() override
#define CustomUpdate virtual void Update() override
#define CustomRender virtual void Render() override
#define CustomUI virtual void RenderInspectorUI() override
#define CustomDefinition virtual EntityDefinition GenerateDefinition() override

enum DashType;

struct Sheet
{
	vector<Sprite> sheet;
	int framePerSecond = 4;

	void Add(const Sprite& sprite) { sheet.push_back(sprite); }

	Sprite CurrentSprite()
	{
		int index = (int)fmodf(Application::Get().frameCount_ * framePerSecond / 60.0f, (float)sheet.size());
		return sheet[index];
	}
};

namespace Entities
{
	void RenderDynamicSizedEntity(Entity* entity, int2 texCoordOffset);

	class Dynamic : public Entity
	{
	public:
		Dynamic(const EntityDefinition& def, int2 hitboxMin, int2 hitboxMax, bool solid, bool carriable)
			: Entity(def)
		{
			hitbox = Rect(hitboxMin, hitboxMax);
			SetFlag(IsSolid, solid);
			SetFlag(IsCarriable, carriable);
		}

		CustomUpdate {}

		Rect hitbox;
		float xRemainder = 0.0f, yRemainder = 0.0f;
		Dynamic* carrier = nullptr;

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
		void MoveTo(int2 pos);
		virtual bool OnCollide(Dynamic* other, int horizontal, int vertical) { return HasFlag(IsSolid); }

		Dynamic* GetEntityCollision(int xForesense, int yForesense);
		bool GetTilemapCollision(int xForesense, int yForesense);
		bool GetCollision(int xForesense, int yForesense);
		bool IsRiding(Dynamic* carrier);
	};

	class Carrier : public Dynamic
	{
	public:
		Carrier(EntityDefinition def, int2 hitboxMin, int2 hitboxMax)
			: Dynamic(def, hitboxMin, hitboxMax, true, false)
		{
		}

		virtual void MoveX(float amount) override;
		virtual void MoveY(float amount) override;

		vector<Dynamic*> GetRiders();
	};

	class Spike : public Dynamic
	{
	public:
		Spike(EntityDefinition def)
			: Dynamic(def, ZERO, ZERO, false, true), direction(ZERO)
		{
			int2 hitboxMin = ZERO;
			int2 hitboxMax = ZERO;

			switch (def.type)
			{
			case EntityType::SpikeLeft:
				hitboxMin = { 4,0 }, hitboxMax = { 8,8 }, direction = LEFTi;
			break;
			case EntityType::SpikeRight:
				hitboxMin = { 0,0 }, hitboxMax = { 4,8 }, direction = RIGHTi;
			break;
			case EntityType::SpikeDown:
				hitboxMin = { 0,4 }, hitboxMax = { 8,8 }, direction = DOWNi;
			break;
			case EntityType::SpikeUp:
				hitboxMin = { 0,0 }, hitboxMax = { 8,4 }, direction = UPi;
			break;
			}

			hitbox = Rect(hitboxMin, hitboxMax);
		}

		int2 direction;

		CustomUpdate;
	};

	class OscillatorA : public Carrier
	{
	public:
		OscillatorA(EntityDefinition def)
			: Carrier(def, {0,0}, {8,8}), startPos(position)
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
		Turbine(EntityDefinition def);

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
		Essence(EntityDefinition def);

		bool active = true;
		DashType dashType;
		Timer refreshTimer = Timer(2.0f, seconds, true);

		Sound absorbSound = Sound("assets/audio/sound1_dontforgettochange.mp3");
		
		CustomUpdate;
		CustomUI;

		CustomDefinition;

		void Absorb();
		void Refresh();
	};

	class Attractor : public Dynamic
	{
	public:
		Attractor(EntityDefinition def)
			: Dynamic(def, {1,1}, {7,7}, false, false)
		{
		}

		bool active = true;
		bool dissolving = false;
		float attractionRadius = 15.0f;
		float attraction = 200.0f;
		float dissolve = 0.0f;
		Timer refreshTimer = Timer(2.0f, seconds, true);
		ParticleSystem dissolveParticles;

		Sound dissolveSound = Sound("assets/audio/sound2_dontforgettochange.mp3");

		CustomUpdate;

		void Dissolve();
		void Refresh();
	};

	class Igniter : public Dynamic
	{
	public:
		Igniter(EntityDefinition def)
			: Dynamic(def, ZEROi, def.size, true, true)
		{}

		Timer flashTimer = Timer(0.3f, seconds, false);

		float ignition = 0.0f;

		CustomUpdate;
		CustomRender;
	};

	class Propeller : public Dynamic
	{
	public:
		Propeller(EntityDefinition def)
			: Dynamic(def, ZEROi, def.size, true, true)
		{}

		CustomUpdate {};
		CustomRender;
	};

	class MovingPlatform : public Carrier
	{
	public:
		MovingPlatform(EntityDefinition def);
		
		float startTime;

		int2 startPosition;
		int2 endPosition;

		CustomReset;
		CustomUpdate;
		CustomRender;
		CustomUI;
		
		CustomDefinition;
	};

	class DashSwitchPlatform : public Carrier
	{
	public:
		DashSwitchPlatform(EntityDefinition def);

		bool targetIsAltPos;

		int2 startPosition;
		int2 endPosition;

		Tween<int2> moveTween;

		CustomReset;
		CustomUpdate;
		CustomRender;
		CustomUI;

		CustomDefinition;
	};

	class Checkpoint : public Dynamic
	{
	public:
		Checkpoint(EntityDefinition def)
			: Dynamic(def, ZEROi, def.size, false, false)
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

		Rect Bounds(int enlarge = 0) { return Rect((float2)camera.GetPosition() - float2(aspectRatio * zoom, zoom), { (float2)camera.GetPosition() + float2(aspectRatio * zoom, zoom) }); }
		int2 RoomBoundedPosition();
	};

	class Decoration : public Entity
	{
	public:
		Decoration(int2 position, int z, int decorationIndex, Sprite sprite)
			: Entity(position), hitbox(ZEROi, { sprite.XSize(), sprite.YSize() }), decorationIndex(decorationIndex)
		{
			//size = { sprite.XSize(), sprite.YSize() };
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