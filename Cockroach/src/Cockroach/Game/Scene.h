#pragma once

#include <vector>
#include "Entity.h"
#include "Cockroach/Renderer/Camera.h"

namespace Cockroach
{
	enum EntityType
	{
		Player, Tile, SpikeRight, SpikeUp, SpikeLeft, SpikeDown, MovingPlatform
	};

	class Scene
	{
	public:
		static Scene* current;

		Scene();

		std::vector<Ref<Entity>> entities = std::vector<Ref<Entity>>();

		void Update(float dt);
		void Render();

		void Load();

		Ref<Entity> AddEntity(const glm::ivec2 position);
	};

}