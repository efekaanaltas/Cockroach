#include <Cockroach.h>

class ExampleLayer : public Cockroach::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		if (Cockroach::Input::IsKeyPressed(CR_KEY_TAB))
			CR_TRACE("Tab key is pressed!");
	}

	void OnEvent(Cockroach::Event& event) override
	{
		//CR_TRACE("{0}", event);
	}
};

class Sandbox : public Cockroach::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Cockroach::ImGuiLayer());
	}

	~Sandbox()
	{

	}
};

Cockroach::Application* Cockroach::CreateApplication()
{
	return new Sandbox();
}