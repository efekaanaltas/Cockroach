#pragma once

#include "Core.h"

#include "Window.h"

#include "Cockroach/Events/Event.h"
#include "Cockroach/Events/ApplicationEvent.h"

namespace Cockroach
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		
		virtual void Update(float dt) = 0;
		virtual void Render() = 0;

		void ImGuiInit();
		void ImGuiBegin();
		void ImGuiEnd();

		inline Window& GetWindow() { return *window; }
		
		inline static Application& Get() { return *s_Instance; }
		
		int frameCount = 0;
	private:
		void OnEvent(Event& e);
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		Scope<Window> window;
		bool running = true;
		bool minimized = false;
		float lastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};

	// Defined in client
	Application* CreateApplication();
}