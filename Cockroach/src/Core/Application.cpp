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
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		CR_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		window = new Window("Cockroach Engine", 1280, 720);

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
			const float frameTime = 1 / 60.0f;
			static float accumulatedDelta = 0.0f;
			timeUnscaled_ = (float)glfwGetTime();
			dtUnscaled_ = (timeUnscaled_ - lastFrameTime);
			accumulatedDelta += dtUnscaled_;
			lastFrameTime = timeUnscaled_;

			if (!minimized)
			{
				while (accumulatedDelta > frameTime)
				{
					accumulatedDelta -= frameTime;
					dtUnscaled_ = frameTime;
					dt_ = dtUnscaled_ * timeScale_;
					time_ += dt_; // I know this is a bit crowded and messy at the moment, but all I want is results for now.
					frameCount_++;
					Update();
					Input::Update();
				}
				Render();
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
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImVector<ImWchar> ranges;
		ImFontGlyphRangesBuilder builder;
		builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
		builder.AddRanges(io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
		builder.AddRanges(io.Fonts->GetGlyphRangesJapanese());
		builder.AddRanges(io.Fonts->GetGlyphRangesKorean());
		builder.BuildRanges(&ranges);

		io.Fonts->AddFontFromFileTTF("assets/fonts/NotoSansCJKjp-Medium.otf", 17.0f, nullptr, ranges.Data);
		io.Fonts->Build();

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