#include "crpch.h"
#include "CameraController.h"

#include "Cockroach/Core/Input.h"

namespace Cockroach
{
		//: m_AspectRatio(aspectRatio), camera(-aspectRatio * m_Zoom, aspectRatio* m_Zoom, -m_Zoom, m_Zoom)
	CameraController::CameraController(float aspectRatio)
		: zoom(10.0f), aspectRatio(aspectRatio), camera(-aspectRatio * zoom, aspectRatio* zoom, -zoom, zoom)
	{
		aspectRatio = aspectRatio;
	}

	void CameraController::OnUpdate(float dt)
	{
		if (Input::IsPressed(CR_KEY_A))
			position.x -= speed * dt;
		if (Input::IsPressed(CR_KEY_D))
			position.x += speed * dt;
		if (Input::IsPressed(CR_KEY_S))
			position.y -= speed * dt;
		if (Input::IsPressed(CR_KEY_W))
			position.y += speed * dt;

		camera.SetPosition(position);

		speed = zoom; // Change speed according to zoom level
	}

	void CameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrollEvent>(CR_BIND_EVENT_FN(CameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(CR_BIND_EVENT_FN(CameraController::OnWindowResized));
	}

	bool CameraController::OnMouseScrolled(MouseScrollEvent& e)
	{
		zoom -= e.GetYOffset() * 0.1f * zoom;
		zoom = std::min(std::max(zoom, 5.0f), 100.0f);
		camera.SetZoom(-aspectRatio * zoom, aspectRatio * zoom, -zoom, zoom);

		return false;
	}
	bool CameraController::OnWindowResized(WindowResizeEvent& e)
	{
		aspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		camera.SetZoom(-aspectRatio * zoom, aspectRatio * zoom, -zoom, zoom);
		return false;
	}
}