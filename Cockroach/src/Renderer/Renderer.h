#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "Framebuffer.h"

#include <glm/glm.hpp>

namespace Cockroach
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void OnWindowResize(int width, int height);
		static void BeginScene(Camera& camera);
		static void EndScene();
		static void BlitToScreen(Ref<Framebuffer> framebuffer);

		static void SetClearColor(const float4& color);
		static void Clear();

		static void DrawQuad(const float3& position, const float2& size, const Ref<Texture2D>& texture, const float2& min, const float2& max, const float4& overlayColor);
		static void DrawQuad(const float3& position, const float2& size, const Sprite& sprite, const float4& overlayColor, bool flipX, bool flipY);
		static void DrawQuadWithOutline(const float3& position, const float2& size, const Sprite& subTexture, float4 overlayColor, float4 outlineColor, bool flipX, bool flipY);
		static void DrawQuadOutline(float x0, float x1, float y0, float y1, float4 color);
		static void DrawLine(const float3& p0, const float3& p1, const float4& color);
		static void Draw(const Ref<Shader> shader, const Ref<VertexArray>& vertexArray, const mat4& transform = mat4(1.0f));
	private:
		static void FlushAndReset();
	};
}