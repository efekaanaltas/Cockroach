#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Renderer/Texture.h"
#include "Rect.h"

namespace Cockroach
{
	struct EntityDefinition
	{
		int type = 0;
		bool isDecoration = false;
		int2 position = ZEROi;
		int2 size = 8*ONEi;
		std::optional<int> z = {};
		std::optional<int2> altPosition = {};
		std::optional<int> variant = {};

		EntityDefinition(std::stringstream& definition);
		EntityDefinition(int type, bool isDecoration, int2 position, int2 size);

		std::string ToString();
	};

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
		float4 color = WHITE;
		float3 overlayColor = CLEAR;
		float overlayWeight = 0.0f;
		bool flipX = false, flipY = false;

		virtual void Reset() {};
		virtual void Update() {}
		virtual void Render();
		virtual void RenderInspectorUI();

		virtual EntityDefinition GenerateDefinition();

		template<typename T>
		T* As();

		Rect SpriteBounds();

		bool operator==(const Entity& other) { return ID == other.ID; }
		bool operator!=(const Entity& other) { return ID != other.ID; }
	};

	template<typename T>
	T* Entity::As()
	{
		return dynamic_cast<T*>(this);
	}

	Entity* CreateEntity(const EntityDefinition& def);
}