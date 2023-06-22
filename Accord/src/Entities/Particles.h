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

		float deathTime = 0.0f;

		float4 color = WHITE;
	};

	class Particles : public Entity
	{
	public:
		Particles();

		virtual void Update(float dt) override;
		virtual void Render() override;

		void Add(const Particle& particle);

		Sprite pixel;
	private:
		std::vector<Particle> particles;
	};
}
