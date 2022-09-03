#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Renderer/Texture.h"

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

		template<typename T>
		T* As();

		bool operator==(const Entity& other) { return ID == other.ID; }
		bool operator!=(const Entity& other) { return ID != other.ID; }
	};

	template<typename T>
	T* Entity::As()
	{
		return dynamic_cast<T*>(this);
	}
}