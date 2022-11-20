#include "crpch.h"
#include "Entity.h"

#include "Renderer/Renderer.h"

namespace Cockroach
{

	int Entity::lastID = 0;

	Entity::Entity()
		: Entity({ 0,0 })
	{}

	Entity::Entity(const glm::ivec2& position)
	{
		this->position = position;
		ID = lastID++;
	}

	void Entity::Render()
	{
		Renderer::DrawQuad((float2)position, {sprite.XSize(), sprite.YSize()}, sprite);
	}
}