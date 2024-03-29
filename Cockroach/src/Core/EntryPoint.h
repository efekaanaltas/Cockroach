#pragma once

#ifdef CR_PLATFORM_WINDOWS

extern Cockroach::Application* Cockroach::CreateApplication();
extern Cockroach::Entity* Cockroach::CreateEntity(const EntityDefinition& def);

int main(int argc, char** argv)
{
	Cockroach::Log::Init();

	auto app = Cockroach::CreateApplication();
	app->Run();
	delete app;
}

#endif