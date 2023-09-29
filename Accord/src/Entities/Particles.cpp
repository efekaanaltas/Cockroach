#include "Particles.h"
#include "Game.h"

namespace Entities
{
	Particle::Particle(float2 position, float2 velocity, float duration, float4 color)
		: position(position), velocity(velocity), deathTime(time+duration), color(color)
	{
		
	}

	template<glm::length_t L, typename T, glm::qualifier Q>
	glm::vec<L, T, Q> RandomBetweenValues(glm::vec<L, T, Q> a, glm::vec<L, T, Q> b)
	{
		return lerp(a, b, random(0.0f, 1.0f));
	}

	Particle::Particle(float2 positionA, float2 positionB, float2 velocityA, float2 velocityB, float durationA, float durationB, float4 colorA, float4 colorB)
	{
		position = RandomBetweenValues(positionA, positionB);
		velocity = RandomBetweenValues(velocityA, velocityB);
		deathTime = time + random(std::min(durationA, durationB), std::max(durationA, durationB));
		color = RandomBetweenValues(colorA, colorB);
	}

	Particles::Particles()
	{	
		pixel = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 2,0 }, { 1,1 }); // The second pixel in the base texture is pure white so let's just use that
	}

	void Particles::Update()
	{
		for (auto& particle : particles)
		{
			if (time < particle.deathTime)
				particle.position += particle.velocity * dt;
			else break;
		}
	}

	void Particles::Render()
	{
		for (auto& particle : particles)
		{
			Rect pixelRect = Rect(particle.position, (int2)particle.position + RIGHTi);
			if(!Game::cameraController->Bounds().OverlapsWith(pixelRect)) continue;
			if (time < particle.deathTime)
				Renderer::DrawQuad(glm::floor(float3(particle.position.x, particle.position.y, 10.0f)), { 1,1 }, pixel.texture, pixel.min, pixel.max, particle.color);
			else break;
		}
	}

	void Particles::Add(const Particle& particle)
	{
		if (particles.empty() || particles.back().deathTime > time)
			particles.push_back(particle);
		else
			particles.back() = particle;

		static auto compare = [](Particle p1, Particle p2) {return p1.deathTime > p2.deathTime; };
		std::sort(particles.begin(), particles.end(), compare);
	}

	ParticleSystem::ParticleSystem()
		: particlesPerFrame(1), positionA(ZERO), positionB(ZERO), velocityA(ZERO), velocityB(ZERO), durationA(1.0f), durationB(1.0f), colorA(WHITE), colorB(WHITE)
	{
	}

	ParticleSystem::ParticleSystem(int particlesPerFrame, float2 positionA, float2 positionB, float2 velocityA, float2 velocityB, float durationA, float durationB, float4 colorA, float4 colorB)
		: particlesPerFrame(particlesPerFrame), positionA(positionA), positionB(positionB), velocityA(velocityA), velocityB(velocityB), durationA(durationA), durationB(durationB), colorA(colorA), colorB(colorB)
	{
	}

	void ParticleSystem::Prewarm()
	{
		float maxDuration = std::max(durationA, durationB);

		int simulationFrameCount = (int)(maxDuration * 60.0f);

		for (float i = 0; i <= simulationFrameCount; i++)
		{
			for (int j = 0; j <= particlesPerFrame; j++)
				Add(RandomBetweenValues(velocityA, velocityB)*i/60.0f, i/60.0f);
		}
	}

	void ParticleSystem::Update()
	{
		for (int i = 0; i <= particlesPerFrame; i++)
			Add(ZERO, 0.0f);
	}

	void ParticleSystem::Add(float2 accumulatedVelocity, float elapsedLifeTime)
	{
		float duration = random(std::min(durationA, durationB), std::max(durationA, durationB)) - elapsedLifeTime;
		if (duration <= 0)
			return;

		Game::particles->Add(Particle
		(
			positionA + accumulatedVelocity, positionB + accumulatedVelocity,
			velocityA, velocityB,
			duration, duration,
			colorA, colorB
		));
	}
}