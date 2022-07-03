#include "crpch.h"
#include "Scene.h"

namespace Cockroach
{
	Scene* Scene::current = nullptr;

	Scene::Scene()
	{
	}

	void Scene::Update(float dt)
	{
		for (auto& ent : entities)
			ent->Update(dt);
	}

	void Scene::Render()
	{
		for (auto& ent : entities)
			ent->Render();
	}

	void Scene::Load()
	{
		current = this;
	}

	Ref<SubTexture2D> Scene::GetSubTexture(const std::string& filepath, const glm::vec2& coords, const glm::vec2& spriteSize)
	{
		for (uint32_t i = 0; i < nextEmptyTextureSlot; i++)
			if (filepath == sceneTextures[i].get()->path)
				return SubTexture2D::CreateFromCoords(sceneTextures[i], coords, spriteSize);

		// Texture not found in scene storage

		sceneTextures[nextEmptyTextureSlot] = CreateRef<Texture2D>(filepath);
		return SubTexture2D::CreateFromCoords(sceneTextures[nextEmptyTextureSlot++], coords, spriteSize);
	}

	Ref<Entity> Scene::AddEntity(const glm::ivec2 position)
	{
		Ref<Entity> e = CreateRef<Entity>(position);
		entities.push_back(e);
		return e;
	}
}