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

		int32_t ID = -1;

		std::vector<Ref<Component>> components = std::vector<Ref<Component>>();
		glm::ivec2 position;
		Ref<SubTexture2D> sprite;

		void Update(float dt);
		void Render();

		template<typename T>
		Ref<T> AddComponent();

		template<typename T>
		Ref<T> GetComponent();
	};

	template<typename T>
	inline Ref<T> Entity::AddComponent()
	{
		Ref<T> c = CreateRef<T>();
		c->entity = this;
		components.push_back(c);
		return c;
	}

	template<typename T>
	inline Ref<T> Entity::GetComponent()
	{
		for (uint32_t i = 0; i < components.size(); i++)
		{
			T* comp = dynamic_cast<T*>(components[i].get());
			if (comp != nullptr)
				return std::dynamic_pointer_cast<T>(components[i]);
		}
		return nullptr;
	}
}