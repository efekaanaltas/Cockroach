#pragma once

#include <glm/glm.hpp>

namespace Cockroach
{
	class Camera
	{
	public:
		Camera(float left, float right, float bottom, float top);
		void SetZoom(float left, float right, float bottom, float top);

		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

		const mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		glm::vec2 ScreenToWorldCoord(const glm::ivec2& screenCoord);
	private:
		void RecalculateViewMatrix();
	private:
		mat4 m_ViewMatrix;
		mat4 m_ProjectionMatrix;
		mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position;
	};
}