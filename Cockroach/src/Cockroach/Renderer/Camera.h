#pragma once

#include <glm/glm.hpp>

namespace Cockroach
{
	class Camera
	{
	public:
		Camera(float left, float right, float bottom, float top);
		void SetZoom(float left, float right, float bottom, float top);

		const glm::vec3& GetPosition() const { return position; }
		void SetPosition(const glm::vec3& position) { this->position = position; RecalculateViewMatrix(); }

		const mat4& GetViewMatrix() const { return viewMatrix; }
		const mat4& GetProjectionMatrix() const { return projectionMatrix; }
		const mat4& GetViewProjectionMatrix() const { return viewProjectionMatrix; }

		glm::vec2 ScreenToWorldPosition(const glm::ivec2& screenCoord);
	private:
		void RecalculateViewMatrix();
	private:
		mat4 viewMatrix;
		mat4 projectionMatrix;
		mat4 viewProjectionMatrix;

		glm::vec3 position;
	};
}