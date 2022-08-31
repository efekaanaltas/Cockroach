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

		Entity* entity;
		virtual void Update(float dt) {}
	};
}