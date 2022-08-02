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
		Cockroach = 0, Camera, SpikeLeft, SpikeRight, SpikeDown, SpikeUp, PushBlock
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
			e->sprite = SubTexture2D::CreateFromCoords(spriteSheet, { 0, 3 }, { 16, 16 });
			Ref<Hitbox> h = e->AddComponent<Hitbox>();
			h->min = { 6, 0 };
			h->max = { 10, 8 };
			e->AddComponent<Animator>();
			e->AddComponent<Player>();
			break;
		}
		case EntityType::Camera:
		{
			e->sprite = SubTexture2D::CreateFromCoords(spriteSheet, { 0,0 }, { 1,1 });
			Ref<Hitbox> h = e->AddComponent<Hitbox>();
			h->min = { -160, -90 };
			h->max = { 160, 90 };
			Ref<CameraController> cc = e->AddComponent<CameraController>();
			break;
		}
		case EntityType::SpikeLeft:
		{
			e->sprite = SubTexture2D::CreateFromCoords(spriteSheet, { 9, 1 }, { 8, 8 });
			Ref<Hitbox> h = e->AddComponent<Hitbox>();
			h->min = { 4, 0 };
			h->max = { 8, 8 };
			e->AddComponent<Hazard>();
			break;
		}
		case EntityType::SpikeRight:
		{
			e->sprite = SubTexture2D::CreateFromCoords(spriteSheet, { 9, 0 }, { 8, 8 });
			Ref<Hitbox> h = e->AddComponent<Hitbox>();
			h->min = { 0, 0 };
			h->max = { 4, 8 };
			e->AddComponent<Hazard>();
			break;
		}
		case EntityType::SpikeDown:
		{
			e->sprite = SubTexture2D::CreateFromCoords(spriteSheet, { 8, 1 }, { 8, 8 });
			Ref<Hitbox> h = e->AddComponent<Hitbox>();
			h->min = { 0, 4 };
			h->max = { 8, 8 };
			e->AddComponent<Hazard>();
			break;
		}
		case EntityType::SpikeUp:
		{
			e->sprite = SubTexture2D::CreateFromCoords(spriteSheet, { 8, 0 }, { 8, 8 });
			Ref<Hitbox> h = e->AddComponent<Hitbox>();
			h->min = { 4, 0 };
			h->max = { 8, 4 };
			e->AddComponent<Hazard>();
			break;
		}
		case EntityType::PushBlock:
		{
			e->sprite = SubTexture2D::CreateFromCoords(spriteSheet, { 11,2 }, { 8,8 });
			Ref<Hitbox> h = e->AddComponent<Hitbox>();
			h->min = { 0,0 };
			h->max = { 8,8 };
			e->AddComponent<Pusher>();
			break;
		}
		}
		return e;
	}

}