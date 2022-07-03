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

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }

		virtual void Bind(uint32_t slot = 0) const override;

		virtual bool operator ==(const Texture& other) const override
		{
			return m_RendererID == ((Texture2D&)other).m_RendererID;
		}

		std::string path;
	private:
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;

	};

	class SubTexture2D
	{
	public:
		SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);

		static Ref<SubTexture2D> CreateFromCoords(const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& spriteSize);

		inline float XSize() { return (max.x - min.x) * texture->GetWidth(); }
		inline float YSize() { return (max.y - min.y) * texture->GetHeight(); }

		Ref<Texture2D> texture;
		glm::vec2 min, max;
	};

}