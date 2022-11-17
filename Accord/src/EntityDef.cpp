#pragma once

#include "Cockroach.h"
#include "EntityDef.h"

#include "State.h"

#include "Game.h"

using namespace Cockroach;

namespace Entities
{
	int Dynamic::MoveX(float amount)
	{
		xRemainder += amount;
		int move = (int)xRemainder;
		int sign = move > 0 ? 1 : -1;

		if (move != 0)
		{
			xRemainder -= move;

			while (move != 0)
			{
				Dynamic* collidingHitbox = GetEntityCollision(sign, 0);
				bool tilemapCollision = GetTilemapCollision(sign, 0);
				if (!collidingHitbox && !tilemapCollision)
				{
					position.x += sign;
					move -= sign;
				}
				else if (OnCollide(collidingHitbox, sign, 0))
						return sign;
				else
				{
					position.x += sign;
					move -= sign;
				}
			}
		}
		return GetCollision(sign, 0) ? sign : 0;
	}

	int Dynamic::MoveY(float amount)
	{
		yRemainder += amount;
		int move = (int)yRemainder;
		int sign = move > 0 ? 1 : -1;
	
		if (move != 0)
		{
			yRemainder -= move;

			while (move != 0)
			{
				Dynamic* collidingHitbox = GetEntityCollision(0, sign);
				bool tilemapCollision = GetTilemapCollision(0, sign);
				if (!collidingHitbox && !tilemapCollision)
				{
					position.y += sign;
					move -= sign;
				}
				else if (OnCollide(collidingHitbox, 0, sign))
					return sign;
				else
				{
					position.y += sign;
					move -= sign;
				}
			}
		}
		return GetCollision(0, sign) ? sign : 0;
	}

	Dynamic* Dynamic::GetEntityCollision(int xForesense, int yForesense)
	{	
		for (auto& ent : Room::current->entities)
		{
			if (Dynamic* dyn = ent->As<Dynamic>())
				if (dyn != this && OverlapsWith(dyn, xForesense, yForesense))
					return dyn;
		}

		// Also check for collisions with player
		if (Game::player != this && OverlapsWith(Game::player, xForesense, yForesense))
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

	void OscillatorA::Update(float dt)
	{
		int oldPos = position.x;
		if (std::abs(startPos.x - position.x) > 10)
		{
			position.x = std::clamp(position.x, startPos.x - 10, startPos.x + 10);
			moveDir *= -1;
		}

		bool carryingPlayer = OverlapsWith(Game::player, -1, 0) || OverlapsWith(Game::player, 1, 0) || OverlapsWith(Game::player, 0, 1);

		MoveX(moveDir * dt * 10);
		if (carryingPlayer)
				Game::player->position.x += (position.x - oldPos);

		sprite.overlayWeight = 0.5f;
		sprite.overlayColor = CR_COLOR_GREEN;
	}

	void CameraController::Update(float dt)
	{
		for (int i = CR_KEY_KP_1; i <= CR_KEY_KP_3; i++)
			if (Input::IsDown(i))
				SetZoom(std::powf(10.0f, (float)(i - CR_KEY_KP_1 + 1)));
		if (Input::IsDown(CR_KEY_KP_0))
		{
			SetZoom(90.0f);
			editMode = !editMode;
		}

		if (editMode)
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

			float newAspect = (float)Application::Get().GetWindow().width / (float)Application::Get().GetWindow().height;
			if (aspectRatio != newAspect)
				aspectRatio = newAspect;

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

	Turbine::Turbine(int2 position, int2 hitboxMin, int2 hitboxMax, int horizontal, int vertical)
	: Dynamic(position, hitboxMin, hitboxMax), horizontal(horizontal), vertical(vertical)
	{
		int iter = 0;
		int xStart = std::min(position.x, position.x + horizontal * span);
		int xEnd = std::max(position.x, position.x + horizontal * span) + 8;
		int yStart = std::min(position.y, position.y + vertical * span);
		int yEnd = std::max(position.y, position.y + vertical * span) + 8;
		turbineRect = Rect({ xStart, yStart }, { xEnd, yEnd });
	}

	void Turbine::Update(float dt)
	{
		if (horizontal && turbineRect.OverlapsWith(Game::player->WorldHitbox(), 0, 0))
			Game::player->velocity.x += horizontal * turbineAcceleration * dt;
		if (vertical && turbineRect.OverlapsWith(Game::player->WorldHitbox(), 0, 0))
			Game::player->velocity.y += vertical * turbineAcceleration * dt;
	}

	EssenceRed::EssenceRed(int2 position, int2 hitboxMin, int2 hitboxMax)
		: Dynamic(position, hitboxMin, hitboxMax)
	{
		blockOnCollision = false;
		respawnTimer.remainingTime = 0;
	}

	void EssenceRed::Update(float dt)
	{
		if (respawnTimer.Finished())
			Refresh();
		else respawnTimer.Tick(dt);

		if (active && OverlapsWith(Game::player, 0, 0) && !Game::player->canDash)
		{
			Game::player->canDash = true;
			Absorb();
		}
	}

	void EssenceRed::Absorb()
	{
		respawnTimer.Reset();
		sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 0,1 }, { 8,8 });
		active = false;
	}

	void EssenceRed::Refresh()
	{
		sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 1,0 }, { 8,8 });
		active = true;
	}

	void Spike::Update(float dt)
	{
		if(OverlapsWith(Game::player, 0, 0))
			Game::player->Die();
	}
}

Cockroach::Entity* Cockroach::CreateEntity(int2 position, int entityType)
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
	case EntityType::SpikeLeft:
	{
		e = new Spike(position, { 4,0 }, { 8,8 });
		e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 9, 1 }, { 8, 8 });
		break;
	}
	case EntityType::SpikeRight:
	{
		e = new Spike(position, { 0,0 }, { 4,8 });
		e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 9, 0 }, { 8, 8 });
		break;
	}
	case EntityType::SpikeDown:
	{
		e = new Spike(position, { 0,4 }, { 8,8 });
		e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 8, 1 }, { 8, 8 });
		break;
	}
	case EntityType::SpikeUp:
	{
		e = new Spike(position, { 0,0 }, { 8,4 });
		e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 8, 0 }, { 8, 8 });
		break;
	}
	case EntityType::Oscillator:
	{
		e = new OscillatorA(position, { 0, 0 }, { 8, 8 });
		e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 11,2 }, { 8,8 });
		break;
	}
	case EntityType::TurbineLeft:
	{
		e = new Turbine(position, { 0, 0 }, { 8, 8 }, -1, 0);
		e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 11,2 }, { 8,8 });
		break;
	}
	case EntityType::TurbineRight:
	{
		e = new Turbine(position, { 0,0 }, { 8,8 }, 1, 0);
		e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 11,2 }, { 8,8 });
		break;
	}
	case EntityType::TurbineDown:
	{
		e = new Turbine(position, { 0,0 }, { 8,8 }, 0, -1);
		e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 11,2 }, { 8,8 });
		break;
	}
	case EntityType::TurbineUp:
	{
		e = new Turbine(position, { 0,0 }, { 8,8 }, 0, 1);
		e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 11,2 }, { 8,8 });
		break;
	}
	case EntityType::EssenceRed:
	{
		e = new Entities::EssenceRed(position, { 0,0 }, { 8,8 });
		e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 1,0 }, { 8,8 });
		break;
	}
	}
	if (e != nullptr)
		e->type = entityType;
	return e;
}