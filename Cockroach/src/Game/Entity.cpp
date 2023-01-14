#include "crpch.h"
#include "Entity.h"

#include "Renderer/Renderer.h"

namespace Cockroach
{
	int Entity::lastID = 0;

	Entity::Entity()
		: Entity({ 0,0 })
	{}

	Entity::Entity(const int2& position)
	{
		this->position = position;
		ID = lastID++;
	}

	void Entity::Render()
	{
		Renderer::DrawQuad(float3(position, z), { sprite.XSize(), sprite.YSize() }, sprite, { overlayColor, overlayWeight }, flipX, flipY);
	}
}