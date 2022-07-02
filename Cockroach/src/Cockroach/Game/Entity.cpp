#include "crpch.h"
#include "Entity.h"

namespace Cockroach
{
	Entity::Entity()
	{
		position = { 0, 0 };
	}

	Entity::Entity(const glm::ivec2& position)
	{
		this->position = position;
	}

	void Entity::Update(float dt)
	{
		for (auto& comp : components)
		{
			comp->Update(dt);
		}
	}

	void Entity::Render()
	{
		Renderer::DrawQuad(position /*glm::ivec2(sprite->XSize() / 2.0f, sprite->YSize() / 2.0f)*/, {sprite->XSize(), sprite->YSize()}, sprite);
	}
}