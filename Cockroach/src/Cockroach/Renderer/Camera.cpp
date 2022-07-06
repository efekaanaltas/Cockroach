#include "crpch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include "Cockroach/Core/Application.h"

namespace Cockroach
{
	Camera::Camera(float left, float right, float bottom, float top)
		: m_Position(0.0f), m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0f)
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void Camera::SetZoom(float left, float right, float bottom, float top)
	{
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	float2 Camera::ScreenToWorldCoord(const glm::ivec2& screenCoord)
	{
		Window& window = Application::Get().GetWindow();
		float2 screenCoordYFlip = { screenCoord.x, window.Height() - screenCoord.y};
		float2 screenCoordNormalized = { screenCoordYFlip.x / (float)window.Width(), screenCoordYFlip.y / (float)window.Height() };
		float2 screenCoordCentered = { screenCoordNormalized.x * 2 - 1, screenCoordNormalized.y * 2 - 1 };
		float2 screenUnits = { 1.0f / m_ProjectionMatrix[0][0], 1.0f / m_ProjectionMatrix[1][1] };
		float2 worldCoord = { screenCoordCentered.x * screenUnits.x, screenCoordCentered.y * screenUnits.y };
		float2 worldCoordTranslated = { worldCoord.x - m_ViewMatrix[3][0], worldCoord.y - m_ViewMatrix[3][1] };
		return worldCoordTranslated;
	}

	void Camera::RecalculateViewMatrix()
	{
		mat4 transform = glm::translate(mat4(1.0f), m_Position);
		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}