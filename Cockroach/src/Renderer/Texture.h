#pragma once

#include "Core/Core.h"
#include <string>

#include <glm/glm.hpp>

namespace Cockroach
{
	class Texture
	{
	public:
		virtual ~Texture() = default;
		
		virtual void Bind(u32 slot = 0) const = 0;
		virtual bool operator ==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		Texture2D(const std::string& path);
		virtual ~Texture2D() override;

		virtual void Bind(u32 slot = 0) const override;

		virtual bool operator ==(const Texture& other) const override
		{
			return rendererID == ((Texture2D&)other).rendererID;
		}

		std::string path;
		u32 rendererID;
		int width, height;
	};

	class Sprite
	{
	public:
		Sprite() {}
		Sprite(const Ref<Texture2D>& texture, const float2& min, const float2& max);

		static Sprite CreateFromCoords(const Ref<Texture2D>& texture, const float2& coords, const float2& spriteSize);

		inline float XSize() { return (max.x - min.x) * texture->width; }
		inline float YSize() { return (max.y - min.y) * texture->height; }

		Ref<Texture2D> texture;
		float2 min = {0.0f, 0.0f}, max = {1.0f, 1.0f};
		bool flipX = false, flipY = false; // Make flips part of entity?
	};

}