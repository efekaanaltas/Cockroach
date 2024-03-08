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
		
		virtual void Update() = 0;
		virtual void Render() = 0;

		virtual void OnQuit();
		void OnWindowResize(int width, int height);
		void OnInputCallback(int code, bool down);

		void ImGuiInit();
		void ImGuiBegin();
		void ImGuiEnd();

		inline Window& GetWindow() { return *window; }
		
		inline static Application& Get() { return *s_Instance; }
		
		float time_ = 0;
		float dt_ = 0;
		float timeUnscaled_ = 0;
		float dtUnscaled_ = 0;
		int frameCount_ = 0;

		float timeScale_ = 1.0f;
	private:
		Window* window;
		bool running = true;
		bool minimized = false;
		float lastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};

	// Defined in client
	Application* CreateApplication();
}