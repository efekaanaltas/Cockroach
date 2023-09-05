#pragma once

namespace Cockroach
{
	class Framebuffer
	{
	public:
		Framebuffer(int width, int height, bool bilinear = false);
		~Framebuffer();

		void Bind();
		void Unbind();

		u32 rendererID;
		u32 colorAttachment, depthAttachment;

		int width = 320, height = 180;
		bool swapChainTarget = false;
	};
}