#pragma once

#include "Cockroach.h"
using namespace Cockroach;

namespace Entities
{
	class Particle
	{
	public:
		Particle(float2 position, float2 velocity, float duration, float4 color);
		Particle(float2 positionA, float2 positionB, float2 velocityA, float2 velocityB, float durationA, float durationB, float4 colorA, float4 colorB);

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

	class ParticleSystem
	{
	public:
		ParticleSystem();
		ParticleSystem(int particlesPerFrame, float2 positionA, float2 positionB, float2 velocityA, float2 velocityB, float durationA, float durationB, float4 colorA, float4 colorB);

		void Prewarm();
		void Update();

		int particlesPerFrame = 1;
		float2 positionA = ZERO, positionB = ZERO;
		float2 velocityA = ZERO, velocityB = ZERO;
		float durationA = 1.0f, durationB = 1.0f;
		float4 colorA = WHITE, colorB = WHITE;
	private:
		void Add(float2 accumulatedVelocity, float elapsedLifeTime);
	};
}
