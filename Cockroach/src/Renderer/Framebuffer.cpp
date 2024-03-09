#include "crpch.h"
#include "Framebuffer.h"

#include "glad/glad.h"

namespace Cockroach
{
	Framebuffer::Framebuffer(int width, int height, bool bilinear)
		: width(width), height(height)
	{
		glCreateFramebuffers(1, &rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, rendererID);

		glCreateTextures(GL_TEXTURE_2D, 1, &colorAttachment);
		glBindTexture(GL_TEXTURE_2D, colorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		if (bilinear)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);

		glCreateTextures(GL_TEXTURE_2D, 1, &depthAttachment);
		glBindTexture(GL_TEXTURE_2D, depthAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);

		CR_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer incomplete.");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	Framebuffer::~Framebuffer()
	{
		glDeleteFramebuffers(1, &rendererID);
	}

	void Framebuffer::Bind()
	{
		CR_CORE_INFO("Bound framebuffer {0}", rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, rendererID);
	}

	void Framebuffer::Unbind()
	{
		CR_CORE_INFO("Bound framebuffer 0");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}