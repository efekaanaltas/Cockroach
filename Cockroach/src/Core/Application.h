#pragma once

#include "Core.h"

#include "Window.h"

#include "Game/Timer.h"

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

		void OnQuit();
		void OnWindowResize(int width, int height);
		void OnInputCallback(int code, bool down);

		void ImGuiInit();
		void ImGuiBegin();
		void ImGuiEnd();

		inline Window& GetWindow() { return *window; }
		
		inline static Application& Get() { return *s_Instance; }
		
		int frameCount = 0;
		Timer freezeTimer = Timer(0.0f);
	private:
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