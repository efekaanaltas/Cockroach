#include "crpch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include "Cockroach/Core/Application.h"

namespace Cockroach
{
	Camera::Camera(float left, float right, float bottom, float top)
		: position(0.0f), projectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), viewMatrix(1.0f)
	{
		viewProjectionMatrix = projectionMatrix * viewMatrix;
	}

	void Camera::SetZoom(float left, float right, float bottom, float top)
	{
		projectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		viewProjectionMatrix = projectionMatrix * viewMatrix;
	}

	float2 Camera::ScreenToWorldCoord(const glm::ivec2& screenCoord)
	{
		Window& window = Application::Get().GetWindow();
		float2 screenCoordYFlip = { screenCoord.x, window.Height() - screenCoord.y};
		float2 screenCoordNormalized = { screenCoordYFlip.x / (float)window.Width(), screenCoordYFlip.y / (float)window.Height() };
		float2 screenCoordCentered = { screenCoordNormalized.x * 2 - 1, screenCoordNormalized.y * 2 - 1 };
		float2 screenUnits = { 1.0f / projectionMatrix[0][0], 1.0f / projectionMatrix[1][1] };
		float2 worldCoord = { screenCoordCentered.x * screenUnits.x, screenCoordCentered.y * screenUnits.y };
		float2 worldCoordTranslated = { worldCoord.x - viewMatrix[3][0], worldCoord.y - viewMatrix[3][1] };
		return worldCoordTranslated;
	}

	void Camera::RecalculateViewMatrix()
	{
		mat4 transform = glm::translate(mat4(1.0f), position);
		viewMatrix = glm::inverse(transform);
		viewProjectionMatrix = projectionMatrix * viewMatrix;
	}
}