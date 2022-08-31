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
			Dynamic e = Dynamic();
			e.sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 9, 1 }, { 8, 8 });
			e.hitbox = Rect({ 4,0 }, { 8,8 });
			e.layer = Trigger;
			break;
		}
		case EntityType::SpikeRight:
		{
			Dynamic e = Dynamic();
			e.sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 9, 1 }, { 8, 8 });
			e.hitbox = Rect({ 0,0 }, { 4,8 });
			e.layer = Trigger;
			break;
		}
		case EntityType::SpikeDown:
		{
			Dynamic e = Dynamic();
			e.sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 9, 1 }, { 8, 8 });
			e.hitbox = Rect({ 0,4 }, { 8,8 });
			e.layer = Trigger;
			break;
		}
		case EntityType::SpikeUp:
		{
			Dynamic e = Dynamic();
			e.sprite = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 9, 1 }, { 8, 8 });
			e.hitbox = Rect({ 4,8 }, { 8,4 });
			e.layer = Trigger;
			break;
		}
		}
		return e;
	}
}