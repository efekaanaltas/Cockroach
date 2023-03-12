#include "crpch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include "Core/Application.h"

namespace Cockroach
{
	Camera::Camera(float left, float right, float bottom, float top)
		: position(0.0f), projectionMatrix(glm::ortho(left, right, bottom, top, -100.0f, 100.0f)), viewMatrix(1.0f)
	{
		viewProjectionMatrix = projectionMatrix * viewMatrix;
	}

	void Camera::SetZoom(float left, float right, float bottom, float top)
	{
		projectionMatrix = glm::ortho(left, right, bottom, top, -100.0f, 100.0f);
		viewProjectionMatrix = projectionMatrix * viewMatrix;
	}

	float2 Camera::ScreenToWorldPosition(const int2& screenCoord)
	{
		Window& window = Application::Get().GetWindow();
		const float targetAspect = 16.0f / 9.0f;
		float2 size = float2(window.width, window.height);
		float2 origin = ZERO;
		float aspect = (float)window.width / window.height;
		if (aspect >= targetAspect)
		{
			size.x = window.height * targetAspect;
			origin.x = (window.width - size.x) / 2.0f;
		}
		else
		{
			size.y = window.width / targetAspect;
			origin.y = (window.height - size.y) / 2.0f;
		}
		float2 screenCoordYFlip = { screenCoord.x-origin.x, window.height - (screenCoord.y-origin.y)};
		float2 screenCoordNormalized = { screenCoordYFlip.x / (float)size.x, screenCoordYFlip.y / (float)size.y};
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