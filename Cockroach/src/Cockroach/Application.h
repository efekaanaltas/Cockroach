#pragma once

#include "Core.h"

#include "Window.h"
#include "Cockroach/LayerStack.h"
#include "Events/Event.h"
#include "Cockroach/Events/ApplicationEvent.h"

#include "Cockroach/ImGui/ImGuiLayer.h"

namespace Cockroach
{
	class COCKROACH_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }
		
		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}