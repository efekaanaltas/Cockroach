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
	private:
		float aspectRatio;
		float zoom = 10.0f;

		float3 position = { 0.0f, 0.0f, 0.0f };
		float speed = 1.0f;
	public:
		Camera camera;
	};
}