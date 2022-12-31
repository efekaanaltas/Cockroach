#pragma once

#include "crpch.h"

#include "Core/Core.h"

#include <GLFW/glfw3.h>

namespace Cockroach
{
	class Window
	{
	public:
		Window(std::string title, int width, int height);
		~Window();

		void OnUpdate();
		void SetWindowMode(bool fullscreen);

		GLFWwindow* GLFWWindow;
		GLFWmonitor* GLFWMonitor;

		std::string title;
		int width, height;
		int2 pos;

		bool fullscreen;
	};
}