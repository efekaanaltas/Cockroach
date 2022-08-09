#pragma once

#include "Cockroach.h"
#include "Components.h"

using namespace Cockroach;

namespace Entities
{
	const std::string spriteSheetPath = "assets/textures/SpriteSheet.png";
	Ref<Texture2D> spriteSheet;

	enum EntityType
	{
		Cockroach, Camera, SpikeLeft, SpikeRight, SpikeDown, SpikeUp, PushBlock
	};

	Entity* Create(int2 position, int entityType)
	{
		if (spriteSheet == nullptr)
		{
			spriteSheet = CreateRef<Texture2D>(spriteSheetPath);
		}
		Entity* e = Room::current->AddEntity(position);
		e->ID = entityType;
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
			e->sprite = SubTexture2D::CreateFromCoords(spriteSheet, { 9, 1 }, { 8, 8 });
			Ref<DynamicObject> dyn = e->AddComponent<Hazard>();
			dyn->hitbox = Rect({ 4,0 }, { 8,8 });
			break;
		}
		case EntityType::SpikeRight:
		{
			e->sprite = SubTexture2D::CreateFromCoords(spriteSheet, { 9, 0 }, { 8, 8 });
			Ref<DynamicObject> dyn = e->AddComponent<Hazard>();
			dyn->hitbox = Rect({ 0,0 }, { 4,8 });
			break;
		}
		case EntityType::SpikeDown:
		{
			e->sprite = SubTexture2D::CreateFromCoords(spriteSheet, { 8, 1 }, { 8, 8 });
			Ref<DynamicObject> dyn = e->AddComponent<Hazard>();
			dyn->hitbox = Rect({ 0,4 }, { 8,8 });
			break;
		}
		case EntityType::SpikeUp:
		{
			e->sprite = SubTexture2D::CreateFromCoords(spriteSheet, { 8, 0 }, { 8, 8 });
			Ref<DynamicObject> dyn = e->AddComponent<Hazard>();
			dyn->hitbox = Rect({ 4,8 }, { 8,4 });
			break;
		}
		case EntityType::PushBlock:
		{
			e->sprite = SubTexture2D::CreateFromCoords(spriteSheet, { 11,2 }, { 8,8 });
			Ref<DynamicObject> dyn = e->AddComponent<Pusher>();
			dyn->hitbox = Rect({ 0,0 }, { 8,8 });
			break;
		}
		}
		return e;
	}

}