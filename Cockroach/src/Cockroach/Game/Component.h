#pragma once

#include <glm/glm.hpp>

namespace Cockroach
{
	class Entity;

	class Component
	{
	public:
		Component(Entity* entity) 
			: entity(entity)
		{}
		Entity* entity = nullptr;
		virtual void Update(float dt) {}
	};
}