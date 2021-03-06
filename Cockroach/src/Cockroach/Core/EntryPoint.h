#pragma once

#ifdef CR_PLATFORM_WINDOWS

extern Cockroach::Application* Cockroach::CreateApplication();

int main(int argc, char** argv)
{
	Cockroach::Log::Init();

	auto app = Cockroach::CreateApplication();
	app->Run();
	delete app;
}

#endif