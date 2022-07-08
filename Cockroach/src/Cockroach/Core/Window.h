#pragma once

#include "crpch.h"

#include "Cockroach/Core/Core.h"
#include "Cockroach/Events/Event.h"

#include "Cockroach/Renderer/GraphicsContext.h"
#include <GLFW/glfw3.h>

namespace Cockroach
{
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "Cockroach Engine",
			unsigned int width = 1280,
			unsigned int height = 720)
			: Title(title), Width(width), Height(height)
		{

		}
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		Window(const WindowProps& props);
		~Window();

		void OnUpdate();

		GLFWwindow* GLFWWindow;
		GraphicsContext* Context;

		uint32_t Width() { return data.Width; }
		uint32_t Height() { return data.Height; }

		// Window attributes
		void SetEventCallback(const EventCallbackFn& callback) { data.EventCallback = callback; }
		void SetVSync(bool enabled);
		bool IsVSync();
	private:
		struct WindowData
		{
			std::string Title;
			uint32_t Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData data;
	};
}