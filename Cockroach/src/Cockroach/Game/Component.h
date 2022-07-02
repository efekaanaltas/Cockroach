#pragma once

#include <glm/glm.hpp>

namespace Cockroach
{
	class Entity;

	class Component
	{
	public:
		Component() {};
		Entity* entity = nullptr;
		virtual void Update(float dt) = 0;
	};
}