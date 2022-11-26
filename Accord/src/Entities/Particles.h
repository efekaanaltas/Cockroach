#pragma once

#include "Cockroach.h"
using namespace Cockroach;

namespace Entities
{
	class Particle
	{
	public:
		Particle(float2 position, float2 velocity, float duration, float4 color);

		float2 position;
		float2 velocity;

		Timer life;

		float4 color = WHITE;
	};

	class Particles : public Entity
	{
	public:
		Particles();

		virtual void Update(float dt) override
		{
			for (auto& particle : particles)
				if (!particle.life.Finished())
				{
					particle.life.Tick(dt);
					particle.position += particle.velocity * dt;
				}
		}

		virtual void Render() override
		{
			for (auto& particle : particles)
				if (!particle.life.Finished())
					Renderer::DrawQuad(glm::floor(float3(particle.position.x, particle.position.y, 0.0f)), {1,1}, pixel.texture, pixel.min, pixel.max, particle.color);
		}

		Sprite pixel;
		std::vector<Particle> particles; // Priority Queue?
	};
}
