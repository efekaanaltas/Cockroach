#pragma once

#include "Core.h"

#include "Window.h"

#include "Cockroach/Events/Event.h"
#include "Cockroach/Events/ApplicationEvent.h"

#include "Cockroach/Game/Scene.h"

namespace Cockroach
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		void ImGuiBegin();
		void ImGuiEnd();
		virtual void Update(float dt) = 0;
		virtual void Render() = 0;
		virtual void OnEvent(Event& e) = 0;

		inline Window& GetWindow() { return *window; }
		
		inline static Application& Get() { return *s_Instance; }
	private:
		void BaseOnEvent(Event& e);
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		Scope<Window> window;
		bool running = true;
		bool minimized = false;
		float lastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}