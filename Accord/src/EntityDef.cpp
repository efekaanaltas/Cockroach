#pragma once

#include "Cockroach.h"
#include "EntityDef.h"

#include "State.h"

#include "Game.h"

using namespace Cockroach;

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
			Dynamic* collidingHitbox = GetEntityCollision(0, sign, Heavy);
			bool tilemapCollision = GetTilemapCollision(0, sign);
			if (!collidingHitbox && !tilemapCollision)
			{
				position.y += sign;
				move -= sign;
			}
			else if (OnCollide(collidingHitbox, 0, sign))
				return sign;
		}
	}
	return GetCollision(0, sign) ? sign : 0;
}

Dynamic* Dynamic::GetEntityCollision(int xForesense, int yForesense, CollisionLayer layer)
{	
	for (auto& ent : Room::current->entities)
	{
		if (Dynamic* dyn = ent->As<Dynamic>())
			if (dyn != this && OverlapsWith(dyn, xForesense, yForesense) && (layer & All || layer == layer))
				return dyn;
	}

	// Also check for collisions with player
	if (Game::player != this && OverlapsWith(Game::player, xForesense, yForesense) && (layer & All || layer == Game::player->layer))
		return Game::player;
	return nullptr;
}

bool Dynamic::GetTilemapCollision(int xForesense, int yForesense)
{
	return Room::current->CollidesWith(WorldHitbox(), xForesense, yForesense);
}

bool Dynamic::GetCollision(int xForesense, int yForesense, CollisionLayer layer)
{
	return GetTilemapCollision(xForesense, yForesense) || GetEntityCollision(xForesense, yForesense, layer);
}

void CameraController::Update(float dt)
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
		Dynamic* e = new Dynamic(position, { 4,0 }, { 8,8 });
		e->type = entityType;
		e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 9, 1 }, { 8, 8 });
		e->layer = Trigger;
		Room::current->AddEntity(e);
		return e;
	}
	case EntityType::SpikeRight:
	{
		Dynamic* e = new Dynamic(position, { 0,0 }, { 4,8 });
		e->type = entityType;
		e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 9, 1 }, { 8, 8 });
		e->layer = Trigger;
		Room::current->AddEntity(e);
		return e;
	}
	case EntityType::SpikeDown:
	{
		Dynamic* e = new Dynamic(position, { 0,4 }, { 8,8 });
		e->type = entityType;
		e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 9, 1 }, { 8, 8 });
		e->layer = Trigger;
		Room::current->AddEntity(e);
		return e;
	}
	case EntityType::SpikeUp:
	{
		Dynamic* e = new Dynamic(position, { 4,8 }, { 8,4 });
		e->type = entityType;
		e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 9, 1 }, { 8, 8 });
		e->layer = Trigger;
		Room::current->AddEntity(e);
		return e;
	}
	}
}