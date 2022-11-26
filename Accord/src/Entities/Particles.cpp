#include "Particles.h"
#include "Game.h"

namespace Entities
{
	Particle::Particle(float2 position, float2 velocity, float duration, float4 color)
		: position(position), velocity(velocity), life(duration), color(color)
	{}

	Particles::Particles()
	{
		pixel = Sprite::CreateFromCoords(Game::baseSpriteSheet, { 2,0 }, { 1,1 }); // The second pixel in the base texture is pure white so let's just use that
	}
}