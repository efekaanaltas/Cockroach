#include "crpch.h"
#include "Application.h"

#include "Core/Log.h"
#include "Core/Input.h"
#include "Renderer/Renderer.h"
#include "Audio/Audio.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "backends/imgui_impl_opengl3.cpp"
#include "backends/imgui_impl_glfw.cpp"

namespace Cockroach
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		CR_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		window = Scope<Window>( new Window("Cockroach Engine", 1280, 720));

		Renderer::Init();
		Audio::Init();
		ImGuiInit();
	}

	Application::~Application()
	{

	}

	void Application::OnInputCallback(int code, bool down)
	{
		ImGuiIO io = ImGui::GetIO();

		bool imguiFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow) || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);

		if (down && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
		{
			ImGui::SetWindowFocus(nullptr); // Lose focus if keys are pressed but the mouse is over the game world.
			imguiFocused = false;
		}

		if (!down || !imguiFocused)
			Input::stateMap[code].pressed = down;
	}

	void Application::Run()
	{
		while (running)
		{
			float time = (float)glfwGetTime();
			float dt = (time - lastFrameTime);
			lastFrameTime = time;
			frameCount++;

			if (!minimized)
			{
				if (freezeTimer.Finished())
				{
					Update(dt);
					Render();
					Input::Update();
				}
				else freezeTimer.Tick(1.0f);
			}

			window->OnUpdate();
		}
	}

	void Application::OnQuit()
	{
		running = false;
	}

	void Application::OnWindowResize(int width, int height)
	{
		Application::Get().GetWindow().width = width;
		Application::Get().GetWindow().height = height;
		bool minimized = width == 0 || height == 0;

		if (!minimized)
			Renderer::OnWindowResize(width, height);
	}

	void Application::ImGuiInit()
	{
		static bool imguiInitialized = false;
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GLFWWindow);

		if (!imguiInitialized)
		{
			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init("#version 410");
			imguiInitialized = true;
		}
	}

	void Application::ImGuiBegin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void Application::ImGuiEnd()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().width, (float)app.GetWindow().height);

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}
}