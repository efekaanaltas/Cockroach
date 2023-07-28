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
	void ENTITY_TYPE::Update(float dt)

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
		bool flipX = entity->flipX;
		bool flipY = entity->flipY;

		if (w > 1.0f && h > 1.0f)
		{
			for (int x = 0; x < w; x++)
				for (int y = 0; y < h; y++)
				{
					int xOffset = (x == 0) ? texCoordOffset.x - 3 : (x == w - 1) ? texCoordOffset.x - 1 : texCoordOffset.x - 2;
					int yOffset = (y == 0) ? texCoordOffset.y + 1 : (y == h - 1) ? texCoordOffset.y + 3 : texCoordOffset.y + 2;
					Renderer::DrawQuad(float3(entity->position, entity->z) + float3(8 * x, 8 * y, 0), size, Sprite::CreateFromCoords(Game::baseSpriteSheet, { xOffset,yOffset }, { 8,8 }), color, overlay, flipX, flipY);
				}

		}
		else if (w > 1.0f)
			for (int x = 0; x < w; x++)
			{
				int xOffset = (x == 0) ? texCoordOffset.x - 3 : (x == w - 1) ? texCoordOffset.x - 1 : texCoordOffset.x - 2;
				Renderer::DrawQuad(float3(entity->position, entity->z) + float3(8 * x, 0, 0), size, Sprite::CreateFromCoords(Game::baseSpriteSheet, { xOffset,2 }, { 8,8 }), color, overlay, flipX, flipX);
			}
		else if (h > 1.0f)
			for (int y = 0; y < h; y++)
			{
				int yOffset = (y == 0) ? texCoordOffset.y + 1 : (y == h - 1) ? texCoordOffset.y + 3 : texCoordOffset.y + 2;
				Renderer::DrawQuad(float3(entity->position, entity->z) + float3(0, 8 * y, 0), size, Sprite::CreateFromCoords(Game::baseSpriteSheet, { 11,yOffset }, { 8,8 }), color, overlay, flipX, flipY);
			}
		else
			Renderer::DrawQuad(float3(entity->position, entity->z), size, Sprite::CreateFromCoords(Game::baseSpriteSheet, texCoordOffset, { 8,8 }), color, overlay, flipX, flipY);
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

	Dynamic* Dynamic::GetEntityCollision(int xForesense, int yForesense)
	{	
		for (auto& ent : Room::current->entities)
		{
			if (Dynamic* dyn = ent->As<Dynamic>())
				if (dyn != this && OverlapsWith(dyn, xForesense, yForesense) && dyn->solid)
					return dyn;
		}

		// Also check for collisions with player
		if (Game::player != this && OverlapsWith(Game::player, xForesense, yForesense) && Game::player->solid)
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
		int2 desiredPos = startPos + int2(10*std::cos(Game::Time()), 10*std::sin(Game::Time()));
		float2 move = desiredPos - position;
		MoveX(move.x);
		MoveY(move.y);
	}

	CustomUpdate(CameraController)
	{
		for (int i = CR_KEY_1; i <= CR_KEY_3; i++)
			if (Input::IsDown(i))
				SetZoom(std::powf(10.0f, (float)(i - CR_KEY_1 + 1)));

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
			zoom = std::clamp(zoom, 5.0f, 1000.0f);

			//float newAspect = (float)Application::Get().GetWindow//().width / (float)Application::Get().GetWindow().height;
			//if (aspectRatio != newAspect)
			//	aspectRatio = newAspect;

			camera.SetZoom(-aspectRatio * zoom, aspectRatio * zoom, -zoom, zoom);
		}
		else
		{
			if (isTransitioning)
			{
				position = transitionTween.Step(dt);
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

	Turbine::Turbine(int2 position, int2 size, int horizontal, int vertical)
	: Dynamic(position, int2(0, 0), size, true, true), horizontal(horizontal), vertical(vertical)
	{
		int xStart = std::min(position.x, position.x + horizontal * span);
		int xEnd = std::max(position.x, position.x + horizontal * span) + size.x;
		int yStart = std::min(position.y, position.y + vertical * span);
		int yEnd = std::max(position.y, position.y + vertical * span) + size.y;
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
		if (!Game::player->grounded)
		{
			if (horizontal && turbineRect.OverlapsWith(Game::player->WorldHitbox(), 0, 0))
				Game::player->velocity.x += horizontal * turbineAcceleration * dt;
			if (vertical && turbineRect.OverlapsWith(Game::player->WorldHitbox(), 0, 0))
				Game::player->velocity.y += vertical * turbineAcceleration * dt;
		}

		airParticles.Update();
	}

	Essence::Essence(int2 position, int2 hitboxMin, int2 hitboxMax, DashType dashType)
		: Dynamic(position, hitboxMin, hitboxMax, false, false), dashType(dashType)
	{
		solid = false;
		refreshTimer.remainingTime = 0;
		overlayColor = WHITE;
	}

	CustomUpdate(Essence)
	{
		if (!active && refreshTimer.Finished())
			Refresh();
		else refreshTimer.Tick(dt);

		if (active)
		{
			overlayWeight = std::clamp(overlayWeight - 4 * dt, 0.0f, 1.0f);

			if (OverlapsWith(Game::player, 0, 0) && (!Game::player->canDash || dashType != Game::player->currentDashType))
			{
				Game::player->RegainDash(dashType);
				Absorb();
			}
		}
	}

	void Essence::Absorb()
	{
		Game::Freeze(3);
		Audio::Play("assets/audio/sound1_dontforgettochange.wav");
		for (int i = 0; i < 500; i++)
		{
			float randomAngle = random(0.0f, 2*PI);
			float2 randomDir = float2(std::cos(randomAngle), std::sin(randomAngle));
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
		case Dash: sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 1,0 }, { 8,8 }); break;
		case Drift: sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 2,0 }, { 8,8 }); break;
		}
		overlayWeight = 1.0f;
		active = true;
	}

#pragma warning (disable: 4244) // Lots of int->float conversions, no need for warnings.

	CustomUpdate(Attractor)
	{
		if (!active && refreshTimer.Finished())
			Refresh();
		else refreshTimer.Tick(dt);

		if (active)
		{
			overlayWeight = std::clamp(overlayWeight - 4 * dt, 0.0f, 1.0f);

			float2 delta = WorldHitbox().Center() - Game::player->WorldHitbox().Center();
			float distanceSqr = delta.x*delta.x+delta.y*delta.y;

			if (distanceSqr <= attractionRadius*attractionRadius)
			{
				Game::player->velocity = ZERO;
				if (delta.x != 0) Game::player->MoveX((delta.x > 0 ? 1 : -1));
				if (delta.y != 0) Game::player->MoveY((delta.y > 0 ? 1 : -1));

				if (!dissolving)
				{
					dissolveTimer.Reset();
					dissolving = true;
				}

				if (Input::IsDown(CR_KEY_LEFT_SHIFT))
				{
					dissolveTimer.remainingTime = 0;
					Dissolve();
				}
			}
			
			if (!dissolveTimer.Finished())
			{
				dissolveTimer.Tick(dt);
				if (dissolveTimer.Finished())
					Dissolve();
			}
		}
	}

	void Attractor::Dissolve()
	{
		refreshTimer.Reset();
		sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 0,1 }, { 8,8 });
		Audio::Play("assets/audio/sound2_dontforgettochange.wav");
		active = false;
		dissolving = false;
	}

	void Attractor::Refresh()
	{
		overlayWeight = 1.0f;
		sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 0,0 }, { 8,8 });
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
			std::vector<Dynamic*> riders = GetRiders();

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
			std::vector<Dynamic*> riders = GetRiders();

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

	std::vector<Dynamic*> Carrier::GetRiders()
	{
		std::vector<Dynamic*> riders;
		for (auto& ent : Room::current->entities)
		{
			Dynamic* dyn = ent->As<Dynamic>();
			if (dyn && (dyn != this) && dyn->carriable && dyn->IsRiding(this))
				riders.push_back(dyn);
		}

		if (Game::player->IsRiding(this))
			riders.push_back(Game::player);

		return riders;
	}

	CustomUpdate(Igniter)
	{
		if (Game::player->IsRiding(this))
			igniteTimer.Tick(dt);
		else
			igniteTimer.Tick(-dt);
		igniteTimer.remainingTime = std::min(igniteTimer.remainingTime, igniteTimer.duration);

		if (igniteTimer.Finished())
		{
			igniteTimer.Reset();
			flashTimer.Reset();
			Game::player->Die();
		}

		overlayColor = RED;
		overlayWeight = lerp(0.0f, 1.0f, igniteTimer.Progress01());
		
		if (igniteTimer.Progress01() > 0.7f)
		{
			overlayWeight = fmod(Game::Time(), 0.2f) < 0.15f ? 1.0f : 0.0f;
		}

		if (!flashTimer.Finished())
		{
			flashTimer.Tick(dt);
			overlayColor = WHITE;
			overlayWeight = lerp(1.0f, 0.0f, flashTimer.Progress01());
		}
	}

	MovingPlatform::MovingPlatform(int2 position, int2 size, int2 altPosition)
		: Carrier(position, ZEROi, size), startPosition(position), endPosition(altPosition), startTime(Game::Time())
	{}

	CustomReset(MovingPlatform)
	{
		startTime = Game::Time();
	}

	CustomUpdate(MovingPlatform)
	{
		float elapsedTime = Game::Time() - startTime;
 		int2 desiredPos = lerp(startPosition, endPosition, (std::sin(elapsedTime)+1)/2.0f );
		int2 move = desiredPos - position;

		MoveX(move.x);
		MoveY(move.y);
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
		Renderer::DrawLine(float3(startPosition, 20.0f), float3(endPosition, 20.0f), CYAN);
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

	CustomDefinition(MovingPlatform)
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

Cockroach::Entity* Cockroach::CreateEntity(const EntityDefinition& def)
{
	auto GetPositionFunc = [](std::stringstream& def) { return int2(GetProperty<int>(def, "X"), GetProperty<int>(def, "Y")); };
	auto GetSizeFunc = [](std::stringstream& def) { return int2(GetProperty<int>(def, "W"), GetProperty<int>(def, "H")); };
	auto GetAltPositionFunc = [](std::stringstream& def) { return int2(GetProperty<int>(def, "X1"), GetProperty<int>(def, "Y1")); };

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
			e = new Spike(def.position, { 4,0 }, { 8,8 }, LEFTi);
		break;
		case EntityType::SpikeRight:
			e = new Spike(def.position, { 0,0 }, { 4,8 }, RIGHTi);
		break;
		case EntityType::SpikeDown:
			e = new Spike(def.position, { 0,4 }, { 8,8 }, DOWNi);
		break;
		case EntityType::SpikeUp:
			e = new Spike(def.position, { 0,0 }, { 8,4 }, UPi);
		break;
		case EntityType::Oscillator:
			e = new OscillatorA(def.position, { 0, 0 }, { 8, 8 });
		break;
		case EntityType::TurbineLeft:
			e = new Turbine(def.position, def.size, -1, 0);
		break;
		case EntityType::TurbineRight:
			e = new Turbine(def.position, def.size, 1, 0);
		break;
		case EntityType::TurbineDown:
			e = new Turbine(def.position, def.size, 0, -1);
		break;
		case EntityType::TurbineUp:
			e = new Turbine(def.position, def.size, 0, 1);
		break;
		case EntityType::Essence:
			e = new Entities::Essence(def.position, { 1,1 }, { 7,7 }, (DashType)def.variant.value_or(0));
		break;
		case EntityType::Igniter:
			e = new Entities::Igniter(def.position, def.size);
		break;
		case EntityType::Propeller:
			e = new Entities::Propeller(def.position, def.size);
		break;
		case EntityType::MovingPlatform:
			e = new Entities::MovingPlatform(def.position, def.size, def.altPosition.value_or(def.position));
		break;
		case EntityType::Attractor:
			e = new Entities::Attractor(def.position, { 1,1 }, { 7,7 });
		break;
		case EntityType::Checkpoint:
			e = new Entities::Checkpoint(def.position, def.size);
			e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 0,1 }, { 8,8 });
		break;
		}
		if (e != nullptr)
		{
			e->sprite = Game::entitySprites[def.type];
			e->type = def.type;
			e->size = def.size;
		}
	}
	else
	{
		Sprite& sprite = Game::decorationSprites[def.type];
		e = new Decoration(def.position, def.z.value_or(0), def.type, sprite);
	}
	return e;
}