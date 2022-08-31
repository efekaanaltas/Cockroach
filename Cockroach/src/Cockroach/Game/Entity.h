#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Component.h"
#include "Cockroach/Renderer/Texture.h"
#include "Cockroach/Renderer/Renderer.h"

namespace Cockroach
{
	class Component;

	class Entity
	{
	public:
		Entity();
		Entity(const glm::ivec2& position);

		static int lastID;
		int ID = 0;
		int type = -1;

		glm::ivec2 position;
		Sprite sprite;

		virtual void Update(float dt) {}
		void Render();

		bool operator==(const Entity& other) { return ID == other.ID; }
		bool operator!=(const Entity& other) { return ID != other.ID; }
	};
}