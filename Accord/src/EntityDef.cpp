#pragma once

#include "Cockroach.h"
#include "EntityDef.h"

#include "State.h"

#include "Game.h"

#include "imgui/imgui.h"

using namespace Cockroach;

namespace Entities
{
	#define CustomReset(ENTITY_TYPE) \
	void ENTITY_TYPE::Reset()

	#define CustomUpdate(ENTITY_TYPE) \
	void ENTITY_TYPE::Update()

	#define CustomRender(ENTITY_TYPE) \
	void ENTITY_TYPE::Render()

	#define CustomUI(ENTITY_TYPE) \
	void ENTITY_TYPE::RenderInspectorUI()

	#define CustomDefinition(ENTITY_TYPE) \
	EntityDefinition ENTITY_TYPE::GenerateDefinition()

	void RenderDynamicSizedEntity(Entity* entity, int2 texCoordOffset)
	{
		int w = entity->size.x / 8;
		int h = entity->size.y / 8;

		float2 size = { entity->sprite.XSize(), entity->sprite.YSize() };
		float4 color = entity->color;
		float4 overlay = { entity->overlayColor, entity->overlayWeight };
		bool flipX = entity->HasFlag(FlipX);
		bool flipY = entity->HasFlag(FlipY);

		if (w > 1.0f && h > 1.0f)
		for (int x = 0; x < w; x++)
		for (int y = 0; y < h; y++)
		{
			int xOffset = (x == 0) ? texCoordOffset.x - 3 : (x == w - 1) ? texCoordOffset.x - 1 : texCoordOffset.x - 2;
			int yOffset = (y == 0) ? texCoordOffset.y + 1 : (y == h - 1) ? texCoordOffset.y + 3 : texCoordOffset.y + 2;
			Renderer::DrawQuad(float3(entity->position, entity->z) + float3(8 * x, 8 * y, 0), size, Sprite::CreateFromCoords(Game::baseSpriteSheet, { xOffset,yOffset }, { 8,8 }), color, overlay, flipX, flipY);
		}
		else if (w > 1.0f) 
		for (int x = 0; x < w; x++)
		{
			int xOffset = (x == 0) ? texCoordOffset.x - 3 : (x == w - 1) ? texCoordOffset.x - 1 : texCoordOffset.x - 2;
			Renderer::DrawQuad(float3(entity->position, entity->z) + float3(8 * x, 0, 0), size, Sprite::CreateFromCoords(Game::baseSpriteSheet, { xOffset,texCoordOffset.y }, { 8,8 }), color, overlay, flipX, flipX);
		}
		else if (h > 1.0f)
		for (int y = 0; y < h; y++)
		{
			int yOffset = (y == 0) ? texCoordOffset.y + 1 : (y == h - 1) ? texCoordOffset.y + 3 : texCoordOffset.y + 2;
			Renderer::DrawQuad(float3(entity->position, entity->z) + float3(0, 8 * y, 0), size, Sprite::CreateFromCoords(Game::baseSpriteSheet, { texCoordOffset.x,yOffset }, { 8,8 }), color, overlay, flipX, flipY);
		}
		else
		{
			Renderer::DrawQuad(float3(entity->position, entity->z), size, Sprite::CreateFromCoords(Game::baseSpriteSheet, texCoordOffset, { 8,8 }), color, overlay, flipX, flipY);
		}
	}

	void Dynamic::MoveX(float amount)
	{
		xRemainder += amount;
		int move = (int)xRemainder;
		int sign = move > 0 ? 1 : -1;

		if (move != 0)
		{
			xRemainder -= move;

			while (move != 0)
			{
				Dynamic* collidingDynamic = GetEntityCollision(sign, 0);
				bool tilemapCollision = GetTilemapCollision(sign, 0);
				if (!collidingDynamic && !tilemapCollision)
				{
					position.x += sign;
					move -= sign;
				}
				else
				{
					OnCollide(collidingDynamic, sign, 0);
					break;
				}
			}
		}
	}

	void Dynamic::MoveY(float amount)
	{
		yRemainder += amount;
		int move = (int)yRemainder;
		int sign = move > 0 ? 1 : -1;

		if (move != 0)
		{
			yRemainder -= move;

			while (move != 0)
			{
				Dynamic* collidingDynamic = GetEntityCollision(0, sign);
				bool tilemapCollision = GetTilemapCollision(0, sign);
				if (!collidingDynamic && !tilemapCollision)
				{
					position.y += sign;
					move -= sign;
				}
				else
				{
					OnCollide(collidingDynamic, 0, sign);
					break;
				}
			}
		}
	}

	void Dynamic::MoveTo(int2 pos)
	{
		int2 delta = pos - this->position;
		MoveX(delta.x);
		MoveY(delta.y);
	}

	Dynamic* Dynamic::GetEntityCollision(int xForesense, int yForesense)
	{	
		for (auto& ent : Room::current->entities)
		{
			if (Dynamic* dyn = ent->As<Dynamic>())
				if (dyn != this && OverlapsWith(dyn, xForesense, yForesense) && dyn->HasFlag(IsSolid))
					return dyn;
		}

		// Also check for collisions with player
		if (Game::player != this && OverlapsWith(Game::player, xForesense, yForesense) && Game::player->HasFlag(IsSolid))
			return Game::player;
		return nullptr;
	}

	bool Dynamic::GetTilemapCollision(int xForesense, int yForesense)
	{
		return Room::current->CollidesWith(WorldHitbox(), xForesense, yForesense);
	}

	bool Dynamic::GetCollision(int xForesense, int yForesense)
	{
		return GetTilemapCollision(xForesense, yForesense) || GetEntityCollision(xForesense, yForesense);
	}

	bool Dynamic::IsRiding(Dynamic* carrier)
	{
		return OverlapsWith(carrier, 1, 0) || OverlapsWith(carrier, -1, 0) || OverlapsWith(carrier, 0, -1) || OverlapsWith(carrier, 0, 1);
	}

	CustomUpdate(OscillatorA)
	{
		int2 desiredPos = startPos + int2(10*cos(time), 10*sin(time));
		float2 move = desiredPos - position;
		MoveX(move.x);
		MoveY(move.y);
	}

	CustomUpdate(CameraController)
	{
		for (int i = CR_KEY_1; i <= CR_KEY_3; i++)
			if (Input::IsDown(i))
				SetZoom(pow(10.0f, (float)(i - CR_KEY_1 + 1)));

		if (Game::editMode)
		{			
			if (Input::IsPressed(CR_KEY_A))
				positionHighRes.x -= speed * dt;
			if (Input::IsPressed(CR_KEY_D))
				positionHighRes.x += speed * dt;
			if (Input::IsPressed(CR_KEY_S))
				positionHighRes.y -= speed * dt;
			if (Input::IsPressed(CR_KEY_W))
				positionHighRes.y += speed * dt;

			position = int2(positionHighRes.x, positionHighRes.y);

			camera.SetPosition({positionHighRes.x, positionHighRes.y, 0.0f});

			speed = zoom; // Change speed according to zoom level

			zoom -= Input::scroll * 0.1f * zoom;
			zoom = clamp(zoom, 5.0f, 1000.0f);

			//float newAspect = (float)Application::Get().GetWindow//().width / (float)Application::Get().GetWindow().height;
			//if (aspectRatio != newAspect)
			//	aspectRatio = newAspect;

			camera.SetZoom(-aspectRatio * zoom, aspectRatio * zoom, -zoom, zoom);
		}
		else
		{
			if (isTransitioning)
			{
				position = transitionTween.Step();
				if (transitionTween.Finished())
					isTransitioning = false;
			}
			else
			{
				position = Game::player->position;
				position = RoomBoundedPosition();
			}

			positionHighRes = position;

			camera.SetPosition(float3(position.x, position.y, 0.0f));
		}

		Audio::SetListenerPosition(positionHighRes);
	}

	void CameraController::StartTransition()
	{
		if (Game::editMode) return;
		isTransitioning = true;
		targetPosition = RoomBoundedPosition();
		transitionTween = Tween<int2>(position, targetPosition, 0.5f, TweenFunc::Linear);
	}

	int2 CameraController::RoomBoundedPosition()
	{
		Rect roomBounds = Room::current->Bounds();
		return glm::clamp(position, roomBounds.min + int2(aspectRatio * zoom, zoom), roomBounds.max - int2(aspectRatio * zoom, zoom));
	}

	void CameraController::SetZoom(float zoom)
	{
		this->zoom = zoom;
		camera.SetZoom(-aspectRatio * zoom, aspectRatio * zoom, -zoom, zoom);
	}

	Turbine::Turbine(EntityDefinition def)
	: Dynamic(def, int2(0, 0), def.size, true, true), horizontal(0), vertical(0)
	{
		switch (def.type)
		{
		case EntityType::TurbineLeft:
			horizontal = -1;
		break;
		case EntityType::TurbineRight:
			horizontal = +1;
		break;
		case EntityType::TurbineDown:
			vertical = -1;
		break;
		case EntityType::TurbineUp:
			vertical = +1;
		break;
		}

		int xStart = min(position.x, position.x + horizontal * span);
		int xEnd = max(position.x, position.x + horizontal * span) + size.x;
		int yStart = min(position.y, position.y + vertical * span);
		int yEnd = max(position.y, position.y + vertical * span) + size.y;
		turbineRect = Rect({ xStart, yStart }, { xEnd, yEnd });

		airParticles = ParticleSystem
		(
			1,
			position, position+size,
			24.0f*float2(horizontal, vertical), 26.0f * float2(horizontal, vertical),
			1.0f, 1.3f,
			WHITE, CLEAR
		);
	}

	CustomReset(Turbine)
	{
		airParticles.Prewarm();
	}

	CustomUpdate(Turbine)
	{
		if (horizontal && turbineRect.OverlapsWith(Game::player->WorldHitbox(), 0, 0))
			Game::player->velocity.x += horizontal * turbineAcceleration * dt;
		if (vertical && turbineRect.OverlapsWith(Game::player->WorldHitbox(), 0, 0))
			Game::player->velocity.y += vertical * turbineAcceleration * dt;

		airParticles.Update();
	}

	Essence::Essence(EntityDefinition def)
		: Dynamic(def, {1,1}, {7,7}, false, false), dashType((DashType)def.variant.value_or(0))
	{
		RemoveFlag(IsSolid);
		overlayColor = WHITE;
	}

	CustomUpdate(Essence)
	{
		if (!active && refreshTimer.Finished(false))
			Refresh();

		if (active)
		{
			overlayWeight = clamp(overlayWeight - 4 * dt, 0.0f, 1.0f);

			if (OverlapsWith(Game::player, 0, 0) && (!Game::player->canDash || dashType != Game::player->currentDashType))
			{
				Game::player->RegainDash(dashType);
				Absorb();
			}
		}

		absorbSound.SetPosition(position);
	}

	void Essence::Absorb()
	{
		Game::Freeze(3);
		absorbSound.Start();
		for (int i = 0; i < 500; i++)
		{
			float randomAngle = random(0.0f, 2*PI);
			float2 randomDir = float2(cos(randomAngle), sin(randomAngle));
			Game::particles->Add(Particle
			(
				(float2)position + randomDir, (float2)position + randomDir,
				random(4.0f, 26.0f)*randomDir, random(4.0f, 26.0f) * randomDir,
				0.2f, 0.4f,
				RED, WHITE)
			);
		}
		refreshTimer.Reset();
		sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 0,1 }, { 8,8 });
		active = false;
	}

	void Essence::Refresh()
	{
		switch (dashType)
		{
		case DashType::BasicDash: sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 1,0 }, { 8,8 }); break;
		case DashType::Drift: sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 2,0 }, { 8,8 }); break;
		}
		overlayWeight = 1.0f;
		active = true;
	}

#pragma warning (disable: 4244) // Lots of int->float conversions, no need for warnings.

	CustomUpdate(Attractor)
	{
		dissolveSound.SetPosition(position);

		if (!active && refreshTimer.Finished(false))
			Refresh();

		if (active)
		{
			overlayWeight = clamp(overlayWeight - 4 * dt, 0.0f, 1.0f);

			float2 delta = WorldHitbox().Center() - Game::player->WorldHitbox().Center();
			float distanceSqr = delta.x*delta.x+delta.y*delta.y;

			if (distanceSqr <= attractionRadius*attractionRadius)
			{
				dissolve += dt;
				Game::player->velocity = ZERO;
				if (delta.x != 0) Game::player->MoveX((delta.x > 0 ? 1 : -1));
				if (delta.y != 0) Game::player->MoveY((delta.y > 0 ? 1 : -1));

				if (!dissolving)
				{
					dissolve = 0.0f;
					dissolving = true;
				}

				if (dissolve > 1.0f || Input::IsDown(Dash))
				{
					Dissolve();
				}
			}
		}
	}

	void Attractor::Dissolve()
	{
		refreshTimer.Reset();
		color.a = 0.0f;
		dissolveSound.Start();
		active = false;
		dissolving = false;

		for (int i = 0; i < 20; i++)
		{
			float randomAngle = random(0.0f, 2 * PI);
			float2 randomDir = float2(cos(randomAngle), sin(randomAngle));
			Game::particles->Add(Particle
			(
				(float2)position + randomDir, (float2)position + randomDir,
				random(50.0f, 70.0f) * randomDir, random(50.0f, 70.0f) * randomDir,
				0.2f, 0.4f,
				RED, WHITE)
			);
		}
	}

	void Attractor::Refresh()
	{
		overlayWeight = 1.0f;
		color.a = 1.0f;
		active = true;
	}

	CustomUpdate(Spike)
	{
		if(OverlapsWith(Game::player, 0, 0) && glm::dot((float2)direction, Game::player->velocity) <= 0.0f)
			Game::player->Die();
	}

	void Carrier::MoveX(float amount)
	{
		xRemainder += amount;

		int moveX = (int)xRemainder;
		
		if (moveX != 0)
		{
			vector<Dynamic*> riders = GetRiders();

			xRemainder -= moveX;
			position.x += moveX;

			for (auto& rider : riders)
			{
				if (OverlapsWith(rider, 0, 0))
				{
					if (moveX > 0)
						rider->position.x += Right() - rider->Left();  // Regarding the following 4 lines of direct position changing:  (rider->position._ += ...) 
					else											   // I whill probably add a "collidable" flag to the Dynamic struct in the future,
						rider->position.x += Left() - rider->Right();  // so I can disable this carrier's collision when I want to push other dynamics out of it.
				}													   // But this works well for now and I refuse to do it properly until edge cases show themselves.
				else rider->MoveX(moveX);
			}
		}
	}

	void Carrier::MoveY(float amount)
	{
		yRemainder += amount;

		int moveY = (int)yRemainder;

		if (moveY != 0)
		{
			vector<Dynamic*> riders = GetRiders();

			yRemainder -= moveY;
			position.y += moveY;

			for (auto& rider : riders)
			{
				if (OverlapsWith(rider, 0, 0))
				{
					if (moveY > 0)
						rider->position.y += Top() - rider->Bottom();
					else
						rider->position.y += Bottom() - rider->Top();
				}
				else rider->MoveY(moveY);
			}
		}
	}

	vector<Dynamic*> Carrier::GetRiders()
	{
		vector<Dynamic*> riders;
		for (auto& ent : Room::current->entities)
		{
			Dynamic* dyn = ent->As<Dynamic>();
			if (dyn && (dyn != this) && dyn->HasFlag(IsCarriable) && dyn->IsRiding(this))
			{
				dyn->carrier = this;
				riders.push_back(dyn);
			}
		}

		if (Game::player->IsRiding(this))
		{
			Game::player->carrier = this;
			riders.push_back(Game::player);
		}

		return riders;
	}

	CustomUpdate(Igniter)
	{
		if (Game::player->IsRiding(this))
			ignition += dt;
		else
			ignition -= dt;
		ignition = clamp(ignition, 0.0f, 1.0f);

		if (ignition == 1.0f)
		{
			ignition = 0.0f;
			flashTimer.Reset();
			Game::player->Die();
		}

		color.a = 1.0f;
		overlayColor = RED;
		overlayWeight = lerp(0.0f, 1.0f, ignition);
		
		if (ignition > 0.7f)
		{
			overlayWeight = fmod(time, 0.2f) < 0.15f ? 1.0f : 0.0f;
		}

		if (!flashTimer.Finished(false))
		{
			overlayColor = WHITE;
			overlayWeight = lerp(1.0f, 0.0f, flashTimer.Progress01());
		}
	}

	MovingPlatform::MovingPlatform(EntityDefinition def)
		: Carrier(def, ZEROi, def.size), startPosition(def.position), endPosition(def.altPosition.value_or(def.position)), startTime(time)
	{}

	CustomReset(MovingPlatform)
	{
		startTime = time;
	}

	CustomUpdate(MovingPlatform)
	{
		float elapsedTime = time - startTime;
		int2 desiredPos = lerp(startPosition, endPosition, (sin(elapsedTime)+1)/2.0f );
		MoveTo(desiredPos);
	}

	DashSwitchPlatform::DashSwitchPlatform(EntityDefinition def)
		: Carrier(def, ZEROi, def.size), startPosition(position), endPosition(def.altPosition.value_or(def.position)), targetIsAltPos(false)
	{}

	CustomReset(DashSwitchPlatform)
	{
		targetIsAltPos = false;
		MoveTo(startPosition);
	}

	CustomUpdate(DashSwitchPlatform)
	{
		static bool playerWasDashingLastFrame = false;
		static bool playerIsDashingThisFrame = false;

		playerIsDashingThisFrame = Game::player->currentState == Game::player->dashingState;

		if (playerIsDashingThisFrame && !playerWasDashingLastFrame)
		{
			targetIsAltPos = !targetIsAltPos;
			int2 target = targetIsAltPos ? endPosition : startPosition;
			int2 start = targetIsAltPos ? startPosition : endPosition;
			moveTween = Tween<int2>(start, target, 0.3f, TweenFunc::Quad);
		}

		if(!moveTween.Finished())
			MoveTo(moveTween.Step());

		playerWasDashingLastFrame = playerIsDashingThisFrame;
	}

	CustomRender(Turbine)
	{
		RenderDynamicSizedEntity(this, { 23,2 });
	}

	CustomRender(Igniter)
	{
		RenderDynamicSizedEntity(this, { 11,2 });
	}

	CustomRender(Propeller)
	{
		RenderDynamicSizedEntity(this, { 15,2 });
	}

	CustomRender(MovingPlatform)
	{
		RenderDynamicSizedEntity(this, { 15,2 });
	}

	CustomRender(DashSwitchPlatform)
	{
		RenderDynamicSizedEntity(this, { 15,2 });
	}

	CustomUI(MovingPlatform)
	{
		using namespace ImGui;
		Begin("Inspector");
		InputInt("X", &startPosition.x, 8, 1);
		InputInt("Y", &startPosition.y, 8, 1);
		InputInt("X1", &endPosition.x, 8, 1);
		InputInt("Y1", &endPosition.y, 8, 1);
		InputInt("W", &size.x, 8, 8);
		InputInt("H", &size.y, 8, 8);
		End();
	}

	CustomUI(DashSwitchPlatform)
	{
		using namespace ImGui;
		Begin("Inspector");
		InputInt("X", &startPosition.x, 8, 1);
		InputInt("Y", &startPosition.y, 8, 1);
		InputInt("X1", &endPosition.x, 8, 1);
		InputInt("Y1", &endPosition.y, 8, 1);
		InputInt("W", &size.x, 8, 8);
		InputInt("H", &size.y, 8, 8);
		End();
	}

	CustomDefinition(MovingPlatform)
	{
		EntityDefinition definition = EntityDefinition(type, false, startPosition, size);
		definition.altPosition = endPosition;
		return definition;
	}

	CustomDefinition(DashSwitchPlatform)
	{
		EntityDefinition definition = EntityDefinition(type, false, startPosition, size);
		definition.altPosition = endPosition;
		return definition;
	}

	CustomUI(Essence)
	{
		using namespace ImGui;
		Begin("Inspector");
		InputInt("X", &position.x, 8, 1);
		InputInt("Y", &position.y, 8, 1);
		InputInt("V", (int*)&dashType, 1, 1);
		InputInt("W", &size.x, 8, 8);
		InputInt("H", &size.y, 8, 8);
		End();
	}

	CustomDefinition(Essence)
	{
		EntityDefinition definition = EntityDefinition(type, false, position, size);
		definition.variant = (int)dashType;
		return definition;
	}

	CustomUpdate(Checkpoint)
	{
		if (OverlapsWith(Game::player, 0, 0))
		{
			Game::player->checkpointPosition = Game::player->position; // Lazy implementation, should calculate ground level and place player on ground.
			Game::data.playerPosition = Game::player->checkpointPosition;
			Game::data.Save();
		}
	}

}

#define CREATE(Type) e = new Entities::Type(def)

Cockroach::Entity* Cockroach::CreateEntity(const EntityDefinition& def)
{
	auto GetPositionFunc = [](stringstream& def) { return int2(GetProperty<int>(def, "X"), GetProperty<int>(def, "Y")); };
	auto GetSizeFunc = [](stringstream& def) { return int2(GetProperty<int>(def, "W"), GetProperty<int>(def, "H")); };
	auto GetAltPositionFunc = [](stringstream& def) { return int2(GetProperty<int>(def, "X1"), GetProperty<int>(def, "Y1")); };

	Entity* e = nullptr;
	if (!def.isDecoration)
	{
		switch (def.type)
		{
		case EntityType::Payga:
			CR_WARN("Do not create an instance of Payga");
		break;
		case EntityType::Camera:
			CR_WARN("Do not create an instance of Camera");
		break; 
		case EntityType::Particles:
			CR_WARN("Do not create an instance of Particles");
		break;
		case EntityType::SpikeLeft:
		case EntityType::SpikeRight:
		case EntityType::SpikeDown:
		case EntityType::SpikeUp:
			CREATE(Spike);
		break;
		case EntityType::Oscillator:
			CREATE(OscillatorA);
		break;
		case EntityType::TurbineLeft:
		case EntityType::TurbineRight:
		case EntityType::TurbineDown:
		case EntityType::TurbineUp:
			CREATE(Turbine);
		break;
		case EntityType::Essence:
			CREATE(Essence);
		break;
		case EntityType::Igniter:
			CREATE(Igniter);
		break;
		case EntityType::Propeller:
			CREATE(Propeller);
		break;
		case EntityType::MovingPlatform:
			CREATE(MovingPlatform);
		break;
		case EntityType::Attractor:
			CREATE(Attractor);
		break;
		case EntityType::Checkpoint:
			CREATE(Checkpoint);
		break;
		case EntityType::DashSwitchPlatform:
			CREATE(DashSwitchPlatform);
		break;
		}
		if (e != nullptr)
		{
			e->sprite = Game::entitySprites[def.type];
		}
	}
	else
	{
		Sprite& sprite = Game::decorationSprites[def.type];
		e = new Decoration(def.position, def.z.value_or(0), def.type, sprite);
	}
	return e;
}