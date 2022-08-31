#include "crpch.h"
#include "Entity.h"

namespace Cockroach
{

	int Entity::lastID = 0;

	Entity::Entity()
	{
		position = { 0, 0 };
		ID = lastID++;
	}

	Entity::Entity(const glm::ivec2& position)
	{
		this->position = position;
		ID = lastID++;
	}

	void Entity::Render()
	{
		Renderer::DrawQuad(position, {sprite.XSize(), sprite.YSize()}, sprite);
	}
}