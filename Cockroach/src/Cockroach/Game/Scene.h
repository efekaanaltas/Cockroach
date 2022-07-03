#pragma once

#include <vector>
#include "Entity.h"
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

		void Update(float dt);
		void Render();

		void Load();

		Ref<SubTexture2D> GetSubTexture(const std::string& filepath, const glm::vec2& coords, const glm::vec2& spriteSize);

		Ref<Entity> AddEntity(const glm::ivec2 position);
	};

}