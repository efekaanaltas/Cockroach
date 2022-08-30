#include "Entities.h"

namespace Entities
{
	Entity* Create(int2 position, int entityType)
	{
		Entity* e = Room::current->AddEntity(position);
		e->type = entityType;
		switch (entityType)
		{
		case EntityType::Payga:
		{
			CR_CORE_WARN("Do not create an instance of Payga");
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
			dyn->layer = Trigger;
			break;
		}
		case EntityType::SpikeRight:
		{
			e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 9, 0 }, { 8, 8 });
			Ref<DynamicObject> dyn = e->AddComponent<Hazard>();
			dyn->hitbox = Rect({ 0,0 }, { 4,8 });
			dyn->layer = Trigger;
			break;
		}
		case EntityType::SpikeDown:
		{
			e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 8, 1 }, { 8, 8 });
			Ref<DynamicObject> dyn = e->AddComponent<Hazard>();
			dyn->hitbox = Rect({ 0,4 }, { 8,8 });
			dyn->layer = Trigger;
			break;
		}
		case EntityType::SpikeUp:
		{
			e->sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 8, 0 }, { 8, 8 });
			Ref<DynamicObject> dyn = e->AddComponent<Hazard>();
			dyn->hitbox = Rect({ 4,8 }, { 8,4 });
			dyn->layer = Trigger;
			break;
		}
		}
		return e;
	}
}