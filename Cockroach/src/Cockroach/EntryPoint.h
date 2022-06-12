#pragma once

#ifdef CR_PLATFORM_WINDOWS

extern Cockroach::Application* Cockroach::CreateApplication();

int main(int argc, char** argv)
{
	Cockroach::Log::Init();
	CR_CORE_WARN("Initialized log!");
	int a = 5;
	CR_INFO("Hello! Var={0}, a");

	auto app = Cockroach::CreateApplication();
	app->Run();
	delete app;
}

#endif