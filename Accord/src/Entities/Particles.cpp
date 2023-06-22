#include "Particles.h"
#include "Game.h"

namespace Entities
{
	Particle::Particle(float2 position, float2 velocity, float duration, float4 color)
		: position(position), velocity(velocity), deathTime(Game::Time()+duration), color(color)
	{
		
	}

	Particle::Particle(float2 basePosition, float2 positionDev, float2 baseVelocity, float2 velocityDev, float baseDuration, float durationDev, float4 baseColor, float4 colorDev)
	{
		position = { basePosition.x + random(-positionDev.x, positionDev.x), basePosition.y + random(-positionDev.y, positionDev.y) };
		velocity = { baseVelocity.x + random(-velocityDev.x, velocityDev.x), baseVelocity.y + random(-velocityDev.y, velocityDev.y) };

		deathTime = Game::Time() + baseDuration + random(-durationDev, durationDev);

		color.r = baseColor.r + random(-colorDev.r, colorDev.r);
		color.g = baseColor.g + random(-colorDev.g, colorDev.g);
		color.b = baseColor.b + random(-colorDev.b, colorDev.b);
		color.a = baseColor.a + random(-colorDev.a, colorDev.a);
	}

	Particles::Particles()
	{	
		pixel = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 2,0 }, { 1,1 }); // The second pixel in the base texture is pure white so let's just use that
	}

	void Particles::Update(float dt)
	{
		for (auto& particle : particles)
		{
			if (Game::Time() < particle.deathTime)
				particle.position += particle.velocity * dt;
			else break;
		}
	}

	void Particles::Render()
	{
		for (auto& particle : particles)
		{
			if (Game::Time() < particle.deathTime)
				Renderer::DrawQuad(glm::floor(float3(particle.position.x, particle.position.y, 10.0f)), { 1,1 }, pixel.texture, pixel.min, pixel.max, particle.color);
			else break;
		}
	}

	void Particles::Add(const Particle& particle)
	{
		if (particles.empty() || particles.back().deathTime > Game::Time())
			particles.push_back(particle);
		else
			particles.back() = particle;

		static auto compare = [](Particle p1, Particle p2) {return p1.deathTime > p2.deathTime; };
		std::sort(particles.begin(), particles.end(), compare);
	}

}