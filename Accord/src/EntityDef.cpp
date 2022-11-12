#pragma once

#include "Cockroach.h"
#include "EntityDef.h"

#include "State.h"

#include "Game.h"

using namespace Cockroach;

void Trigger::Update(float dt)
{
	if (OverlapsWith(Game::player, 0, 0))
		Game::player->OnTrigger(this);
}

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

Cockroach::Entity* Cockroach::CreateEntity(int2 position, int entityType)
{
	switch (entityType)
	{
	case EntityType::Payga:
	{
		CR_WARN("Do not create an instance of Payga");
		return nullptr;
	}
	case EntityType::Camera:
	{
		CR_WARN("Do not create an instance of Camera");
		return nullptr;
	}
	case EntityType::SpikeLeft:
	{
		Trigger* e = new Trigger(position, { 4,0 }, { 8,8 });
		e->type = entityType;
		e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 9, 1 }, { 8, 8 });
		return e;
	}
	case EntityType::SpikeRight:
	{
		Trigger* e = new Trigger(position, { 0,0 }, { 4,8 });
		e->type = entityType;
		e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 9, 0 }, { 8, 8 });
		return e;
	}
	case EntityType::SpikeDown:
	{
		Trigger* e = new Trigger(position, { 0,4 }, { 8,8 });
		e->type = entityType;
		e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 8, 1 }, { 8, 8 });
		return e;
	}
	case EntityType::SpikeUp:
	{
		Trigger* e = new Trigger(position, { 0,0 }, { 8,4 });
		e->type = entityType;
		e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 8, 0 }, { 8, 8 });
		return e;
	}
	case EntityType::Oscillator:
	{
		OscillatorA* e = new OscillatorA(position, { 0, 0 }, { 8, 8 });
		e->type = entityType;
		e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 11,2 }, { 8,8 });
		return e;
	}
	}
}