#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Renderer/Texture.h"

namespace Cockroach
{
	class Entity
	{
	public:
		Entity();
		Entity(const int2& position);

		static int lastID;
		int ID = 0;
		int type = -1;

		int2 position;
		int z = 0;
		int2 size = { 8, 8 };
		
		Sprite sprite;
		float3 overlayColor = CLEAR;
		float overlayWeight = 0.0f;
		bool flipX = false, flipY = false;

		virtual void Update(float dt) {}
		virtual void Render();

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

	Entity* CreateEntity(int2 position, int2 size, int type);
}