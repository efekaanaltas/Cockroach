#include "crpch.h"
#include "Texture.h"

#include <stb_image.h>

#include <glad/glad.h>

namespace Cockroach
{
	Texture2D::Texture2D(const std::string& path)
		: path(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		CR_CORE_ASSERT(data, "Failed to load image!");
		this->width = width;
		this->height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		CR_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &rendererID);
		glTextureStorage2D(rendererID, 1, internalFormat, width, height);

		glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(rendererID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &rendererID);
	}

	void Texture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, rendererID);
	}

	Sprite::Sprite(const Ref<Texture2D>& texture, const float2& min, const float2& max)
		: texture(texture), min(min), max(max)
	{
	}

	Sprite Sprite::CreateFromCoords(const Ref<Texture2D>& texture, const float2& coords, const float2& spriteSize)
	{
		float2 min = { coords.x * spriteSize.x / texture->width, coords.y * spriteSize.y / texture->height };
		float2 max = { (coords.x + 1) * spriteSize.x / texture->width, (coords.y + 1) * spriteSize.y / texture->height };
		return Sprite(texture, min, max);
	}
}