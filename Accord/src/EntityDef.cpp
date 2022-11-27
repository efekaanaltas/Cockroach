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
			if (Input::IsPressed(CR_KEY_LEFT))
				positionHighRes.x -= speed * dt;
			if (Input::IsPressed(CR_KEY_RIGHT))
				positionHighRes.x += speed * dt;
			if (Input::IsPressed(CR_KEY_DOWN))
				positionHighRes.y -= speed * dt;
			if (Input::IsPressed(CR_KEY_UP))
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
				Game::player->canDash = true;
				Absorb();
			}
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
		overlayWeight = 1.0f;
		active = true;
	}

	void Spike::Update(float dt)
	{
		if(OverlapsWith(Game::player, 0, 0))
			Game::player->Die();
	}

#pragma warning (disable: 4244) // Lots of int->float conversions, no need for warnings.

	int Carrier::MoveX(float amount)
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

		return moveX > 0 ? 1 : -1;
	}

	int Carrier::MoveY(float amount)
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

		return moveY > 0 ? 1 : -1;
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
		e = new Entities::EssenceRed(position, { 1,1 }, { 7,7 });
		e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 1,0 }, { 8,8 });
		break;
	}
	case EntityType::Igniter:
	{
		e = new Entities::Igniter(position, size);
		e->size = size;
		e->sprite = Sprite(Game::baseSpriteSheet, { 8.0f*8/ Game::baseSpriteSheet->width, 2.0f*8/ Game::baseSpriteSheet->height }, { (8.0f*8 + size.x) / Game::baseSpriteSheet->width, (2.0f*8 + size.y) / Game::baseSpriteSheet->height });
		break;
	}
	}
	if (e != nullptr)
		e->type = entityType;
	return e;
}