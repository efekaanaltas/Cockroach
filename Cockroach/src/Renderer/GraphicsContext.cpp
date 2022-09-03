#include "crpch.h"
#include "GraphicsContext.h"

#include "GLFW/glfw3.h"
#include <glad/glad.h>

namespace Cockroach
{
	GraphicsContext::GraphicsContext(GLFWwindow* windowHandle)
		: windowHandle(windowHandle)
	{
		CR_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void GraphicsContext::Init()
	{
		glfwMakeContextCurrent(windowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		CR_CORE_ASSERT(status, "Failed to initilalize Glad!");

		
		("OpenGL Info:");
		CR_CORE_INFO("	Vendor: {0}", glGetString(GL_VENDOR));
		CR_CORE_INFO("	Renderer: {0}", glGetString(GL_RENDERER));
		CR_CORE_INFO("	Version: {0}", glGetString(GL_VERSION));
	}

	void GraphicsContext::SwapBuffers()
	{
		glfwSwapBuffers(windowHandle);
	}
}