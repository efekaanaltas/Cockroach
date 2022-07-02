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

		inline Window& GetWindow() { return *m_Window; }
		
		inline static Application& Get() { return *s_Instance; }
	private:
		void BaseOnEvent(Event& e);
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		Scope<Window> m_Window;
		bool m_Running = true;
		bool m_Minimized = false;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}