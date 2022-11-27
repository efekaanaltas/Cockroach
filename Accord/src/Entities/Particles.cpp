#include "Particles.h"
#include "Game.h"

namespace Entities
{
	float random(float min, float max)
	{
		static int seed = 0;
		++seed;
		float noise = sin(dot({seed+3.486f, seed+26.846f}, float2(12.9898f, 78.233f) * 2.0f)) * 43758.5453f;
		noise = fmod(abs(noise), 1.0f) * (max-min) + min;
		return noise;
	}
	Particle::Particle(float2 position, float2 velocity, float duration, float4 color)
		: position(position), velocity(velocity), life(duration), color(color)
	{
	}

	Particle::Particle(float2 basePosition, float2 positionDev, float2 baseVelocity, float2 velocityDev, float baseDuration, float durationDev, float4 baseColor, float4 colorDev)
	{
		position = { basePosition.x + random(-positionDev.x, positionDev.x), basePosition.y + random(-positionDev.y, positionDev.y) };
		velocity = { baseVelocity.x + random(-velocityDev.x, velocityDev.x), baseVelocity.y + random(-velocityDev.y, velocityDev.y) };

		life = Timer(baseDuration + random(-durationDev, durationDev));

		color.r = baseColor.r + random(-colorDev.r, colorDev.r);
		color.g = baseColor.g + random(-colorDev.g, colorDev.g);
		color.b = baseColor.b + random(-colorDev.b, colorDev.b);
		color.a = baseColor.a + random(-colorDev.a, colorDev.a);
	}

	Particles::Particles()
	{
		pixel = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 2,0 }, { 1,1 }); // The second pixel in the base texture is pure white so let's just use that
	}

}