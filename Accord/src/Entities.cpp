#include "Entities.h"

namespace Entities
{
	Entity* Create(int2 position, int entityType)
	{
		switch (entityType)
		{
		case EntityType::Payga:
		{
			CR_CORE_WARN("Do not create an instance of Payga");
			return nullptr;
		}
		case EntityType::Camera:
		{
			CR_CORE_WARN("Do not create an instance of Camera");
			return nullptr;
		}
		case EntityType::SpikeLeft:
		{
			Dynamic* e = new Dynamic(position, {4,0}, {8,8});
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
}