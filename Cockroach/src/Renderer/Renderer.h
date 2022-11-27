#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

#include "Camera.h"
#include "Shader.h"
#include "Texture.h"

#include <glm/glm.hpp>

namespace Cockroach
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void BeginScene(Camera& camera);
		static void EndScene();

		static void SetClearColor(const float4& color);
		static void Clear();

		static void DrawQuad(const float3& position, const float2& size, const Ref<Texture2D>& texture, const float2& min, const float2& max, const float4& overlayColor);
		static void DrawQuad(const float3& position, const float2& size, const Ref<Texture2D>& texture);
		static void DrawQuad(const float3& position, const float2& size, const Sprite& subTexture, const float4& overlayColor);
		static void DrawQuad(const float2& position, const float2& size, const Sprite& subTexture, const float4& overlayColor);
		static void DrawQuad(const float2& position, const float2& size, const Ref<Texture2D>& texture, const float2& min, const float2& max);

		static void DrawQuadWithOutline(const float2& position, const float2& size, const Sprite& subTexture, float4 overlayColor, float4 outlineColor);

		static void DrawQuadOutline(float x0, float x1, float y0, float y1, float4 color);

		static void DrawLine(const float3& p0, const float3& p1, const float4& color);

		static void Draw(const Ref<Shader> shader, const Ref<VertexArray>& vertexArray, const mat4& transform = mat4(1.0f));
	private:
		static void FlushAndReset();
	};
}