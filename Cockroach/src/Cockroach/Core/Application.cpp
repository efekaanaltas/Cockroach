#include "crpch.h"
#include "Application.h"

#include "Cockroach/Core/Log.h"
#include "Input.h"
#include "Cockroach/Renderer/Renderer.h"

#include <GLFW/glfw3.h>

#include "Cockroach/ImGui/ImGuiLayer.h"

namespace Cockroach
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		CR_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		window = Scope<Window>( new Window(WindowProps()) );
		window->SetEventCallback(BIND_EVENT_FN(BaseOnEvent));

		Renderer::Init();
		ImGuiLayer::OnAttach();
	}

	Application::~Application()
	{

	}

	void Application::ImGuiBegin()
	{
		ImGuiLayer::Begin();
	}

	void Application::ImGuiEnd()
	{
		ImGuiLayer::End();
	}

	void Application::BaseOnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
		
		if (e.GetEventType() == EventType::KeyPressed)
			Input::stateMap[dynamic_cast<KeyPressedEvent&>(e).GetKeyCode()].pressed = true;
		if (e.GetEventType() == EventType::KeyReleased)
			Input::stateMap[dynamic_cast<KeyReleasedEvent&>(e).GetKeyCode()].pressed = false;
		if (e.GetEventType() == EventType::MouseButtonPressed)
			Input::stateMap[dynamic_cast<MouseButtonPressedEvent&>(e).GetMouseButton()].pressed = true;
		if (e.GetEventType() == EventType::MouseButtonReleased)
			Input::stateMap[dynamic_cast<MouseButtonReleasedEvent&>(e).GetMouseButton()].pressed = false;
		if (e.GetEventType() == EventType::MouseScrolled)
			Input::scroll = dynamic_cast<MouseScrollEvent&>(e).GetYOffset();

		CR_CORE_TRACE("{0}", e);

		OnEvent(e);
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

}