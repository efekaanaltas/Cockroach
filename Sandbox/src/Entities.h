#pragma once

#include "Cockroach.h"
#include "Components.h"

using namespace Cockroach;

namespace Entities
{
	Ref<Entity> Cockroach(glm::ivec2 position)
	{
		Ref<Entity> e = Scene::current->AddEntity(position);
		e->sprite = Scene::current->GetSubTexture("assets/textures/SpriteSheet.png", { 0, 3 }, { 16, 16 });
		Ref<Hitbox> h = e->AddComponent<Hitbox>();
		h->min = { 6, 0 };
		h->max = { 10, 10 };
		e->AddComponent<Player>();
		return e;
	}

	Ref<Entity> Tile(glm::ivec2 position)
	{
		Ref<Entity> e = Scene::current->AddEntity(position);
		e->sprite = Scene::current->GetSubTexture("assets/textures/SpriteSheet.png", { 11, 2 }, { 8, 8 });
		e->AddComponent<Hitbox>();
		return e;
	}
}