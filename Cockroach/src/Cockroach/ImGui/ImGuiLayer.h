#pragma once

#include "Cockroach/Events/ApplicationEvent.h"
#include "Cockroach/Events/KeyEvent.h"
#include "Cockroach/Events/MouseEvent.h"

namespace Cockroach
{
	class ImGuiLayer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		static void OnAttach();
		static void OnDetach();

		static void Begin();
		static void End();
	private:
		float m_Time = 0.0f;
	};
}