#pragma once

#include "Cockroach/Layer.h"

#include "Cockroach/Events/ApplicationEvent.h"
#include "Cockroach/Events/KeyEvent.h"
#include "Cockroach/Events/MouseEvent.h"

namespace Cockroach
{
	class COCKROACH_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};
}