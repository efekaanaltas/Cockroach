#pragma once

#include "Core.h"

namespace Cockroach
{
	class COCKROACH_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}