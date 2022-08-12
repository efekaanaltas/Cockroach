#pragma once

#include "Cockroach/Core/Core.h"
#include <string>

#include <glm/glm.hpp>

namespace Cockroach
{
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		
		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual bool operator ==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		Texture2D(const std::string& path);
		virtual ~Texture2D() override;

		virtual uint32_t GetWidth() const override { return width; }
		virtual uint32_t GetHeight() const override { return height; }

		virtual void Bind(uint32_t slot = 0) const override;

		virtual bool operator ==(const Texture& other) const override
		{
			return rendererID == ((Texture2D&)other).rendererID;
		}

		std::string path;
	private:
		uint32_t width, height;
		uint32_t rendererID;
	};

	class Sprite
	{
	public:
		Sprite() {}
		Sprite(const Ref<Texture2D>& texture, const float2& min, const float2& max);

		static Sprite CreateFromCoords(const Ref<Texture2D>& texture, const float2& coords, const float2& spriteSize);

		inline float XSize() { return (max.x - min.x) * texture->GetWidth(); }
		inline float YSize() { return (max.y - min.y) * texture->GetHeight(); }

		Ref<Texture2D> texture;
		float2 min, max;
		bool flipX = false, flipY = false;
	};

}