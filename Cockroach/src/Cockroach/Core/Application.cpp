#include "crpch.h"
#include "Application.h"

#include "Cockroach/Core/Log.h"
#include "Cockroach/Core/Input.h"

#include "Cockroach/Renderer/Renderer.h"

#include "Cockroach/Events/Event.h"
#include "Cockroach/Events/ApplicationEvent.h"
#include "Cockroach/Events/KeyEvent.h"
#include "Cockroach/Events/MouseEvent.h"

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

		window = Scope<Window>( new Window(WindowProps()) );
		window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		Renderer::Init();
		ImGuiInit();
	}

	Application::~Application()
	{

	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		ImGuiIO io = ImGui::GetIO();
			
		if (e.GetEventType() == EventType::KeyPressed && !io.WantCaptureKeyboard)
			Input::stateMap[dynamic_cast<KeyPressedEvent&>(e).GetKeyCode()].pressed = true;
		if (e.GetEventType() == EventType::KeyReleased)
			Input::stateMap[dynamic_cast<KeyReleasedEvent&>(e).GetKeyCode()].pressed = false;

		if (e.GetEventType() == EventType::MouseButtonPressed && !io.WantCaptureMouse)
			Input::stateMap[dynamic_cast<MouseButtonPressedEvent&>(e).GetMouseButton()].pressed = true;
		if (e.GetEventType() == EventType::MouseButtonReleased)
			Input::stateMap[dynamic_cast<MouseButtonReleasedEvent&>(e).GetMouseButton()].pressed = false;
		if (e.GetEventType() == EventType::MouseScrolled)
			Input::scroll = dynamic_cast<MouseScrollEvent&>(e).GetYOffset();

		CR_CORE_TRACE("{0}", e);
	}

	void Application::Run()
	{
		while (running)
		{
			float time = (float)glfwGetTime();
			float dt = (time - lastFrameTime);
			lastFrameTime = time;

			if (!minimized)
			{
				Update(dt);
				Render();
				Input::Update();
			}

			window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			minimized = true;
			return false;
		}

		minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
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
		io.DisplaySize = ImVec2((float)app.GetWindow().Width(), (float)app.GetWindow().Height());

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