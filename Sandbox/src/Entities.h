#pragma once

#include "Cockroach.h"
#include "Components.h"

using namespace Cockroach;

namespace Entities
{
	const std::string spriteSheetPath = "assets/textures/SpriteSheet.png";

	enum EntityType
	{
		Cockroach = 0, Tile, SpikeLeft, SpikeRight, SpikeDown, SpikeUp
	};

	Ref<Entity> CreateEntity(glm::ivec2 position, EntityType entityType)
	{
		Ref<Entity> e = Scene::current->AddEntity(position);
		e->ID = entityType;
		switch (entityType)
		{
		case EntityType::Cockroach: 
		{
			e->sprite = Scene::current->GetSubTexture(spriteSheetPath, { 0, 3 }, { 16, 16 });
			Ref<Hitbox> h = e->AddComponent<Hitbox>();
			h->min = { 6, 0 };
			h->max = { 10, 10 };
			e->AddComponent<Player>();
			break;
		}
		case EntityType::Tile:
		{
			e->sprite = Scene::current->GetSubTexture(spriteSheetPath, { 11, 2 }, { 8, 8 });
			e->AddComponent<Hitbox>();
			break;
		}
		case EntityType::SpikeLeft:
		{
			e->sprite = Scene::current->GetSubTexture(spriteSheetPath, { 9, 1 }, { 8, 8 });
			Ref<Hitbox> h = e->AddComponent<Hitbox>();
			h->min = { 4, 0 };
			h->max = { 8, 8 };
			break;
		}
		case EntityType::SpikeRight:
		{
			e->sprite = Scene::current->GetSubTexture(spriteSheetPath, { 9, 0 }, { 8, 8 });
			Ref<Hitbox> h = e->AddComponent<Hitbox>();
			h->min = { 0, 0 };
			h->max = { 4, 8 };
			break;
		}
		case EntityType::SpikeDown:
		{
			e->sprite = Scene::current->GetSubTexture(spriteSheetPath, { 8, 1 }, { 8, 8 });
			Ref<Hitbox> h = e->AddComponent<Hitbox>();
			h->min = { 0, 4 };
			h->max = { 8, 8 };
			break;
		}
		case EntityType::SpikeUp:
		{
			e->sprite = Scene::current->GetSubTexture(spriteSheetPath, { 8, 0 }, { 8, 8 });
			Ref<Hitbox> h = e->AddComponent<Hitbox>();
			h->min = { 4, 0 };
			h->max = { 8, 4 };
			break;
		}
		}
		return e;
	}

}