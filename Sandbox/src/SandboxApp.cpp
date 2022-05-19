#include <Cockroach.h>

class Sandbox : public Cockroach::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}
};

Cockroach::Application* Cockroach::CreateApplication()
{
	return new Sandbox();
}