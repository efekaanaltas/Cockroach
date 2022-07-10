#include "crpch.h"
#include "CameraController.h"

#include "Cockroach/Core/Input.h"
#include "Cockroach/Core/Application.h"

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

		zoom -= Input::scroll * 0.1f * zoom;
		zoom = std::min(std::max(zoom, 5.0f), 100.0f);
		camera.SetZoom(-aspectRatio * zoom, aspectRatio * zoom, -zoom, zoom);

		float newAspect = (float)Application::Get().GetWindow().Width() / (float)Application::Get().GetWindow().Height();
		if (aspectRatio != newAspect)
		{
			aspectRatio = newAspect;
			camera.SetZoom(-aspectRatio * zoom, aspectRatio * zoom, -zoom, zoom);
		}
	}
}