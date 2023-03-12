#pragma once

#include "Cockroach.h"
#include "EntityDef.h"

#include "State.h"

#include "Game.h"

using namespace Cockroach;

namespace Entities
{
	void RenderDynamicSizedEntity(Entity* entity, int2 texCoordOffset)
	{
		int w = entity->size.x / 8;
		int h = entity->size.y / 8;

		if (w > 1.0f && h > 1.0f)
		{
			for (int x = 0; x < w; x++)
				for (int y = 0; y < h; y++)
				{
					int xOffset = (x == 0) ? texCoordOffset.x - 3 : (x == w - 1) ? texCoordOffset.x - 1 : texCoordOffset.x - 2;
					int yOffset = (y == 0) ? texCoordOffset.y + 1 : (y == h - 1) ? texCoordOffset.y + 3 : texCoordOffset.y + 2;
					Renderer::DrawQuad(float3(entity->position, entity->z) + float3(8 * x, 8 * y, 0), { entity->sprite.XSize(), entity->sprite.YSize() }, Sprite::CreateFromCoords(Game::baseSpriteSheet, { xOffset,yOffset }, { 8,8 }), { entity->overlayColor, entity->overlayWeight }, entity->flipX, entity->flipY);
				}

		}
		else if (w > 1.0f)
			for (int x = 0; x < w; x++)
			{
				int xOffset = (x == 0) ? texCoordOffset.x - 3 : (x == w - 1) ? texCoordOffset.x - 1 : texCoordOffset.x - 2;
				Renderer::DrawQuad(float3(entity->position, entity->z) + float3(8 * x, 0, 0), { entity->sprite.XSize(), entity->sprite.YSize() }, Sprite::CreateFromCoords(Game::baseSpriteSheet, { xOffset,2 }, { 8,8 }), { entity->overlayColor, entity->overlayWeight }, entity->flipX, entity->flipX);
			}
		else if (h > 1.0f)
			for (int y = 0; y < h; y++)
			{
				int yOffset = (y == 0) ? texCoordOffset.y + 1 : (y == h - 1) ? texCoordOffset.y + 3 : texCoordOffset.y + 2;
				Renderer::DrawQuad(float3(entity->position, entity->z) + float3(0, 8 * y, 0), { entity->sprite.XSize(), entity->sprite.YSize() }, Sprite::CreateFromCoords(Game::baseSpriteSheet, { 11,yOffset }, { 8,8 }), { entity->overlayColor, entity->overlayWeight }, entity->flipX, entity->flipY);
			}
		else
			Renderer::DrawQuad(float3(entity->position, entity->z), { entity->sprite.XSize(), entity->sprite.YSize() }, Sprite::CreateFromCoords(Game::baseSpriteSheet, texCoordOffset, { 8,8 }), { entity->overlayColor, entity->overlayWeight }, entity->flipX, entity->flipY);
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
				if (dyn != this && OverlapsWith(dyn, xForesense, yForesense) && dyn->blockOnCollision)
					return dyn;
		}

		// Also check for collisions with player
		if (Game::player != this && OverlapsWith(Game::player, xForesense, yForesense) && Game::player->blockOnCollision)
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
		return OverlapsWith(carrier, 1, 0) || OverlapsWith(carrier, -1, 0) || OverlapsWith(carrier, 0, -1);
	}

	void OscillatorA::Update(float dt)
	{
		float time = (float)glfwGetTime();
		int2 desiredPos = startPos + int2(10*std::cos(time), 10*std::sin(time));
		float2 move = desiredPos - position;
		MoveX(move.x);
		MoveY(move.y);
	}

	void CameraController::Update(float dt)
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
			position = Game::player->position;

			Rect bounds = Room::current->Bounds();
			position = glm::clamp(position, bounds.min + int2(aspectRatio * zoom, zoom), bounds.max - int2(aspectRatio * zoom, zoom));
		
			positionHighRes = position;

			camera.SetPosition(float3(position.x, position.y, 0.0f));
		}
	}

	void CameraController::SetZoom(float zoom)
	{
		this->zoom = zoom;
		camera.SetZoom(-aspectRatio * zoom, aspectRatio * zoom, -zoom, zoom);
	}

	Turbine::Turbine(int2 position, int2 size, int horizontal, int vertical)
	: Dynamic(position, int2(0, 0), size), horizontal(horizontal), vertical(vertical)
	{
		int xStart = std::min(position.x, position.x + horizontal * span);
		int xEnd = std::max(position.x, position.x + horizontal * span) + size.x;
		int yStart = std::min(position.y, position.y + vertical * span);
		int yEnd = std::max(position.y, position.y + vertical * span) + size.y;
		turbineRect = Rect({ xStart, yStart }, { xEnd, yEnd });
	}

	void Turbine::Update(float dt)
	{
		if (!Game::player->grounded)
		{
			if (horizontal && turbineRect.OverlapsWith(Game::player->WorldHitbox(), 0, 0))
				Game::player->velocity.x += horizontal * turbineAcceleration * dt;
			if (vertical && turbineRect.OverlapsWith(Game::player->WorldHitbox(), 0, 0))
				Game::player->velocity.y += vertical * turbineAcceleration * dt;
		}

		Game::particles->particles.push_back(Particle
		(
			position + size / 2 + UPi*vertical*4, { size.x / 2, 0 },
			UPi * vertical * 25, { 1.0f, 1.0f },
			1.0f, 1.3f,
			WHITE, BLACK)
		);
	}

	void Turbine::Render()
	{
		RenderDynamicSizedEntity(this, { 23,2 });
	}

	EssenceRed::EssenceRed(int2 position, int2 hitboxMin, int2 hitboxMax)
		: Dynamic(position, hitboxMin, hitboxMax)
	{
		blockOnCollision = false;
		respawnTimer.remainingTime = 0;
		overlayColor = WHITE;
	}

	void EssenceRed::Update(float dt)
	{
		if (!active && respawnTimer.Finished())
			Refresh();
		else respawnTimer.Tick(dt);

		if (active)
		{
			overlayWeight = std::clamp(overlayWeight - 4 * dt, 0.0f, 1.0f);

			if (OverlapsWith(Game::player, 0, 0) && !Game::player->canDash)
			{
				Game::player->RegainDash();
				Absorb();
			}
		}
	}

	void EssenceRed::Absorb()
	{
		Game::Freeze(3);
		respawnTimer.Reset();
		sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 0,1 }, { 8,8 });
		active = false;
	}

	void EssenceRed::Refresh()
	{
		sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 1,0 }, { 8,8 });
		overlayWeight = 1.0f;
		active = true;
	}

	void Spike::Update(float dt)
	{
		if(OverlapsWith(Game::player, 0, 0) && glm::dot((float2)direction, Game::player->velocity) <= 0.0f)
			Game::player->Die();
	}

#pragma warning (disable: 4244) // Lots of int->float conversions, no need for warnings.

	void Carrier::MoveX(float amount)
	{
		xRemainder += amount;

		int moveX = (int)xRemainder;
		
		if (moveX != 0)
		{
			std::vector<Dynamic*> riders = GetRiders();

			xRemainder -= moveX;
			position.x += moveX;

			Game::player->xRemainder = 0;

			if (OverlapsWith(Game::player, 0, 0))
			{
				if (moveX > 0)
					Game::player->MoveX(Right() - Game::player->Left());
				else
					Game::player->MoveX(Left() - Game::player->Right());
			}
			else if (std::find(riders.begin(), riders.end(), Game::player) != riders.end()) // If riders contains this dynamic
				Game::player->MoveX(moveX);
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

			Game::player->yRemainder = 0;

			if (OverlapsWith(Game::player, 0, 0))
			{
				if (moveY > 0)
					Game::player->MoveY(Top() - Game::player->Bottom());
				else
					Game::player->MoveY(Bottom() - Game::player->Top());
			}
			else if (std::find(riders.begin(), riders.end(), Game::player) != riders.end()) // If riders contains this dynamic
				Game::player->MoveY(moveY);
		}
	}

	std::vector<Dynamic*> Carrier::GetRiders()
	{
		std::vector<Dynamic*> riders;
		for (auto& ent : Room::current->entities)
		{
			Dynamic* dyn = ent->As<Dynamic>();
			if (dyn && dyn != this && dyn->IsRiding(this))
				riders.push_back(this);
		}

		if (Game::player->IsRiding(this))
			riders.push_back(Game::player);

		return riders;
	}

	void Igniter::Update(float dt)
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
			overlayWeight = fmod(glfwGetTime(), 0.2f) < 0.15f ? 1.0f : 0.0f;
		}

		if (!flashTimer.Finished())
		{
			flashTimer.Tick(dt);
			overlayColor = WHITE;
			overlayWeight = lerp(1.0f, 0.0f, flashTimer.Progress01());
		}
	}

	void Igniter::Render()
	{
		RenderDynamicSizedEntity(this, { 11,2 });
	}

	void Propeller::Render()
	{
		RenderDynamicSizedEntity(this, { 15,2 });
	}
}

Cockroach::Entity* Cockroach::CreateEntity(int2 position, int2 size, int entityType)
{
	Entity* e = nullptr;
	switch (entityType)
	{
	case EntityType::Payga:
	{
		CR_WARN("Do not create an instance of Payga");
		break;
	}
	case EntityType::Camera:
	{
		CR_WARN("Do not create an instance of Camera");
		break;
	}
	case EntityType::Particles:
	{
		CR_WARN("Do not create an instance of Particles");
		break;
	}
	case EntityType::SpikeLeft:
	{
		e = new Spike(position, { 4,0 }, { 8,8 }, LEFTi);
		break;
	}
	case EntityType::SpikeRight:
	{
		e = new Spike(position, { 0,0 }, { 4,8 }, RIGHTi);
		break;
	}
	case EntityType::SpikeDown:
	{
		e = new Spike(position, { 0,4 }, { 8,8 }, DOWNi);
		break;
	}
	case EntityType::SpikeUp:
	{
		e = new Spike(position, { 0,0 }, { 8,4 }, UPi);
		break;
	}
	case EntityType::Oscillator:
	{
		e = new OscillatorA(position, { 0, 0 }, { 8, 8 });
		break;
	}
	case EntityType::TurbineLeft:
	{
		e = new Turbine(position, size, -1, 0);
		break;
	}
	case EntityType::TurbineRight:
	{
		e = new Turbine(position, size, 1, 0);
		break;
	}
	case EntityType::TurbineDown:
	{
		e = new Turbine(position, size, 0, -1);
		break;
	}
	case EntityType::TurbineUp:
	{
		e = new Turbine(position, size, 0, 1);
		break;
	}
	case EntityType::EssenceRed:
	{
		e = new Entities::EssenceRed(position, { 1,1 }, { 7,7 });
		break;
	}
	case EntityType::Igniter:
	{
		e = new Entities::Igniter(position, size);
		break;
	}
	case EntityType::Propeller:
	{
		e = new Entities::Propeller(position, size);
		break;
	}
	}
	if (e != nullptr)
	{
		e->sprite = Game::entitySprites[entityType];
		e->type = entityType;
		e->size = size;
	}
	return e;
}

Cockroach::Entity* Cockroach::CreateDecoration(int2 position, int z, int decorationType)
{
	Sprite& sprite = Game::decorationSprites[decorationType];
	Decoration* e = new Decoration(position, {sprite.XSize(), sprite.YSize()});
	e->decorationIndex = decorationType;
	e->type = -1;
	e->z = z;
	e->sprite = sprite;
	e->size = { e->sprite.XSize(), e->sprite.YSize() };
	return e;
}