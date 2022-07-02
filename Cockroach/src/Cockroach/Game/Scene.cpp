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

	Ref<Entity> Scene::AddEntity(const glm::ivec2 position)
	{
		Ref<Entity> e = CreateRef<Entity>(position);
		entities.push_back(e);
		return e;
	}
}