#pragma once

#include "Cockroach/Renderer/Camera.h"

#include "Cockroach/Events/MouseEvent.h"
#include "Cockroach/Events/ApplicationEvent.h"

namespace Cockroach
{
	class CameraController
	{
	public:
		CameraController(float aspectRatio);

		void OnUpdate(float dt);
		void OnEvent(Event& e);
	private:
		bool OnMouseScrolled(MouseScrollEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_AspectRatio;
		float m_Zoom = 10.0f;

		float3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_Speed = 1.0f;
	public:
		Camera camera;
	};
}