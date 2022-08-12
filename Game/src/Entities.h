#pragma once

#include "Cockroach.h"
#include "Components.h"

#include "Game.h"

using namespace Cockroach;

namespace Entities
{
	enum EntityType
	{
		Cockroach, Camera, SpikeLeft, SpikeRight, SpikeDown, SpikeUp, PushBlock
	};

	Entity* Create(int2 position, int entityType)
	{
		Entity* e = Room::current->AddEntity(position);
		e->type = entityType;
		switch (entityType)
		{
		case EntityType::Cockroach:
		{
			CR_CORE_WARN("Do not create an instance of Cockroach");
			break;
		}
		case EntityType::Camera:
		{
			CR_CORE_WARN("Do not create an instance of Camera");
			break;
		}
		case EntityType::SpikeLeft:
		{
			e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 9, 1 }, { 8, 8 });
			Ref<DynamicObject> dyn = e->AddComponent<Hazard>();
			dyn->hitbox = Rect({ 4,0 }, { 8,8 });
			break;
		}
		case EntityType::SpikeRight:
		{
			e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 9, 0 }, { 8, 8 });
			Ref<DynamicObject> dyn = e->AddComponent<Hazard>();
			dyn->hitbox = Rect({ 0,0 }, { 4,8 });
			break;
		}
		case EntityType::SpikeDown:
		{
			e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 8, 1 }, { 8, 8 });
			Ref<DynamicObject> dyn = e->AddComponent<Hazard>();
			dyn->hitbox = Rect({ 0,4 }, { 8,8 });
			break;
		}
		case EntityType::SpikeUp:
		{
			e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 8, 0 }, { 8, 8 });
			Ref<DynamicObject> dyn = e->AddComponent<Hazard>();
			dyn->hitbox = Rect({ 4,8 }, { 8,4 });
			break;
		}
		case EntityType::PushBlock:
		{
			e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 11,2 }, { 8,8 });
			Ref<DynamicObject> dyn = e->AddComponent<Pusher>();
			dyn->hitbox = Rect({ 0,0 }, { 8,8 });
			break;
		}
		}
		return e;
	}

}