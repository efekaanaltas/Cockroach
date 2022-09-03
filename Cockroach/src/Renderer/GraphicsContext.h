#pragma once

struct GLFWwindow;

namespace Cockroach
{
	class GraphicsContext
	{
	public:
		GraphicsContext(GLFWwindow* windowHandle);

		void Init();
		void SwapBuffers();
	private:
		GLFWwindow* windowHandle;
	};
}