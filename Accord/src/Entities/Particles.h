#pragma once

#include "Cockroach.h"
using namespace Cockroach;

namespace Entities
{
	class Particle
	{
	public:
		Particle(float2 position, float2 velocity, float duration, float4 color);
		Particle(float2 basePosition, float2 positionDev, float2 baseVelocity, float2 velocityDev, float baseDuration, float durationDev, float4 baseColor, float4 colorDev);

		float2 position = ZERO;
		float2 velocity = ZERO;

		Timer life = Timer(1.0f);

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
