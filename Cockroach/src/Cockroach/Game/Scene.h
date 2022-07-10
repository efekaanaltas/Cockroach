#pragma once

#include <vector>
#include "Entity.h"
#include "Room.h"
#include "Cockroach/Renderer/Camera.h"

namespace Cockroach
{
	class Scene
	{
	public:
		static const uint32_t MAX_TEXTURES = 32;

		static Scene* current;

		uint32_t nextEmptyTextureSlot = 0;
		Ref<Texture2D> sceneTextures[MAX_TEXTURES];

		Scene();

		std::vector<Ref<Entity>> entities = std::vector<Ref<Entity>>();
		std::vector<Ref<Room>> rooms = std::vector<Ref<Room>>();

		void Update(float dt);
		void Render();

		void Load();

		Ref<SubTexture2D> GetSubTexture(const std::string& filepath, const float2& coords, const float2& spriteSize);

		Ref<Entity> AddEntity(const glm::ivec2 position);
		Ref<Room> AddRoom(const Ref<Room>& room);
	};

}