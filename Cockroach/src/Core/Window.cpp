#include "crpch.h"
#include "Window.h"

#include "Application.h" // Temporary
#include "Input.h"		 // Temporary

#include "glad/glad.h"

namespace Cockroach
{
	static bool GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		("GLFW Error ({0}): {1}", error, description);
	}

	Window::Window(std::string title, int width, int height)
		: title(title), width(width), height(height)
	{
		if (!GLFWInitialized)
		{
			// TODO: glfwTerminate on system shutdown
			int success = glfwInit();
			CR_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			GLFWInitialized = true;
		}

		GLFWWindow = glfwCreateWindow((int)width, (int)height, title.c_str(), nullptr, nullptr);
	
		glfwMakeContextCurrent(GLFWWindow);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		CR_CORE_ASSERT(status, "Failed to initilalize Glad!");

		glfwGetWindowPos(GLFWWindow, &pos.x, &pos.y);
		GLFWMonitor = glfwGetPrimaryMonitor();
		fullscreen = glfwGetWindowMonitor(GLFWWindow) != nullptr;

		glfwSwapInterval(1);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(GLFWWindow, [](GLFWwindow* window, int width, int height)
		{
			Application::Get().OnWindowResize(width, height);
		});

		glfwSetWindowCloseCallback(GLFWWindow, [](GLFWwindow* window)
		{
			Application::Get().OnQuit();
		});

		glfwSetKeyCallback(GLFWWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			Application::Get().OnInputCallback(key, action == GLFW_PRESS || action == GLFW_REPEAT);
		});

		glfwSetMouseButtonCallback(GLFWWindow, [](GLFWwindow* window, int button, int action, int mods)
		{
			Application::Get().OnInputCallback(button, action == GLFW_PRESS || action == GLFW_REPEAT);
		});

		glfwSetScrollCallback(GLFWWindow, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			Input::scroll = yOffset;
		});
	}

	Window::~Window()
	{
		glfwDestroyWindow(GLFWWindow);
	}

	void Window::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(GLFWWindow);
	}

	void Window::SetWindowMode(bool fullscreen)
	{

		CR_CORE_INFO(width);
		if (fullscreen == this->fullscreen)
			return;

		if (fullscreen)
		{
			glfwGetWindowPos(GLFWWindow, &pos.x, &pos.y);
			glfwGetWindowSize(GLFWWindow, &width, &height);

			const GLFWvidmode* mode = glfwGetVideoMode(GLFWMonitor);

			glfwSetWindowMonitor(GLFWWindow, GLFWMonitor, 0, 0, mode->width, mode->height, 0);
		}
		else // Using half the screen width and height when switching to windowed works well for now.
			glfwSetWindowMonitor(GLFWWindow, nullptr, pos.x, pos.y, width/2, height/2, 0);

		this->fullscreen = !this->fullscreen;
	}	
}