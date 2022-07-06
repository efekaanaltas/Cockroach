#include "crpch.h"
#include "CameraController.h"

#include "Cockroach/Core/Input.h"

namespace Cockroach
{
		//: m_AspectRatio(aspectRatio), camera(-aspectRatio * m_Zoom, aspectRatio* m_Zoom, -m_Zoom, m_Zoom)
	CameraController::CameraController(float aspectRatio)
		: m_Zoom(10.0f), m_AspectRatio(aspectRatio), camera(-aspectRatio * m_Zoom, aspectRatio* m_Zoom, -m_Zoom, m_Zoom)
	{
		m_AspectRatio = aspectRatio;
	}

	void CameraController::OnUpdate(float dt)
	{
		if (Input::IsPressed(CR_KEY_A))
			m_CameraPosition.x -= m_Speed * dt;
		if (Input::IsPressed(CR_KEY_D))
			m_CameraPosition.x += m_Speed * dt;
		if (Input::IsPressed(CR_KEY_S))
			m_CameraPosition.y -= m_Speed * dt;
		if (Input::IsPressed(CR_KEY_W))
			m_CameraPosition.y += m_Speed * dt;

		camera.SetPosition(m_CameraPosition);

		m_Speed = m_Zoom; // Change speed according to zoom level
	}

	void CameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrollEvent>(CR_BIND_EVENT_FN(CameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(CR_BIND_EVENT_FN(CameraController::OnWindowResized));
	}

	bool CameraController::OnMouseScrolled(MouseScrollEvent& e)
	{
		m_Zoom -= e.GetYOffset() * 0.1f * m_Zoom;
		m_Zoom = std::min(std::max(m_Zoom, 5.0f), 100.0f);
		camera.SetZoom(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, -m_Zoom, m_Zoom);

		return false;
	}
	bool CameraController::OnWindowResized(WindowResizeEvent& e)
	{
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		camera.SetZoom(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, -m_Zoom, m_Zoom);
		return false;
	}
}