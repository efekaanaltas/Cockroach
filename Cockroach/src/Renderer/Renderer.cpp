#include "crpch.h"
#include "Renderer.h"

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>

namespace Cockroach
{
	struct QuadVertex
	{
		float3 Position;
		float2 TexCoord;
		float TexIndex;
		float4 OverlayColor;
	};

	struct LineVertex
	{
		float3 Position;
		float4 Color;
	};

	struct RendererData
	{
		static const uint32_t BatchQuadCount = 10000;
		static const uint32_t BatchVertexCount = BatchQuadCount * 4;
		static const uint32_t BatchIndexCount = BatchQuadCount * 6;
		static const uint32_t MaxTextureSlots = 32;

		Ref<VertexArray> QuadVA;
		Ref<VertexBuffer> QuadVB;
		Ref<Shader> QuadShader;
		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVBBase = nullptr;
		QuadVertex* QuadVBPtr = nullptr;

		mat4 ViewProjectionMatrix;

		Ref<VertexArray> LineVA;
		Ref<VertexBuffer> LineVB;
		Ref<Shader> LineShader;
		uint32_t LineVertexCount = 0;
		LineVertex* LineVBBase = nullptr;
		LineVertex* LineVBPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 0;
	};

	static RendererData s_Data;

	void Renderer::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		// Quads
		s_Data.QuadVA = CreateRef<VertexArray>();

		s_Data.QuadVB = CreateRef<VertexBuffer>(s_Data.BatchVertexCount * sizeof(QuadVertex));
		s_Data.QuadVB->layout =
		{
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float,  "a_TexIndex" },
			{ ShaderDataType::Float4, "a_OverlayColor"	   }
		};
		s_Data.QuadVA->AddVertexBuffer(s_Data.QuadVB);

		s_Data.QuadVBBase = new QuadVertex[s_Data.BatchVertexCount];

		uint32_t* quadIndices = new uint32_t[s_Data.BatchIndexCount];
		
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.BatchIndexCount; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 1;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 2;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = CreateRef<IndexBuffer>(quadIndices, s_Data.BatchIndexCount);
		s_Data.QuadVA->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		int32_t samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;

		s_Data.QuadShader = CreateRef<Shader>("assets/shaders/Texture.glsl");
		s_Data.QuadShader->UploadUniformIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);

		// Lines
		s_Data.LineVA = CreateRef<VertexArray>();

		s_Data.LineVB = CreateRef<VertexBuffer>(s_Data.BatchVertexCount * sizeof(LineVertex));
		s_Data.LineVB->layout =
		{
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    }
		};
		s_Data.LineVA->AddVertexBuffer(s_Data.LineVB);
		s_Data.LineVBBase = new LineVertex[s_Data.BatchVertexCount];

		s_Data.LineShader = CreateRef<Shader>("assets/shaders/Line.glsl");
	}

	void Renderer::Shutdown()
	{
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		glViewport(0, 0, width, height);
	}

	void Renderer::SetClearColor(const float4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void Renderer::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::BeginScene(Camera& camera)
	{
		s_Data.ViewProjectionMatrix = camera.GetViewProjectionMatrix();
		s_Data.QuadShader->Bind();
		s_Data.QuadShader->UploadUniformMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVBPtr = s_Data.QuadVBBase;

		s_Data.TextureSlotIndex = 0;

		s_Data.LineShader->Bind();
		s_Data.LineShader->UploadUniformMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);
		s_Data.LineVertexCount = 0;
		s_Data.LineVBPtr = s_Data.LineVBBase;
	}

	void Renderer::EndScene()
	{
		if (s_Data.QuadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVBPtr - (uint8_t*)s_Data.QuadVBBase);
			s_Data.QuadVB->SetData(s_Data.QuadVBBase, dataSize);

			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
				s_Data.TextureSlots[i]->Bind(i);

			s_Data.QuadVA->Bind();
			s_Data.QuadShader->Bind();
			glDrawElements(GL_TRIANGLES, s_Data.QuadIndexCount, GL_UNSIGNED_INT, nullptr);
		}
		if (s_Data.LineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.LineVBPtr - (uint8_t*)s_Data.LineVBBase);
			s_Data.LineVB->SetData(s_Data.LineVBBase, dataSize);

			s_Data.LineVA->Bind();
			s_Data.LineShader->Bind();
			glDrawArrays(GL_LINES, 0, s_Data.LineVertexCount);
		}
	}

	void Renderer::FlushAndReset()
	{
		EndScene();

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVBPtr = s_Data.QuadVBBase;

		s_Data.LineVertexCount = 0;
		s_Data.LineVBPtr = s_Data.LineVBBase;

		s_Data.TextureSlotIndex = 0;
	}

	void Renderer::DrawQuad(const float3& position, const float2& size, const Ref<Texture2D>& texture, const float2& min, const float2& max, const float4& overlayColor)
	{
		if (s_Data.QuadIndexCount >= RendererData::BatchIndexCount)
			FlushAndReset();

		float textureIndex = -1.0f;
		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == -1.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		s_Data.QuadVBPtr->Position = position;
		s_Data.QuadVBPtr->TexCoord = { min.x, min.y };
		s_Data.QuadVBPtr->TexIndex = textureIndex;
		s_Data.QuadVBPtr->OverlayColor = overlayColor;
		s_Data.QuadVBPtr++;

		s_Data.QuadVBPtr->Position = { position.x + size.x, position.y, 0.0f };
		s_Data.QuadVBPtr->TexCoord = { max.x, min.y };
		s_Data.QuadVBPtr->TexIndex = textureIndex;
		s_Data.QuadVBPtr->OverlayColor = overlayColor;
		s_Data.QuadVBPtr++;

		s_Data.QuadVBPtr->Position = { position.x, position.y + size.y, 0.0f };
		s_Data.QuadVBPtr->TexCoord = { min.x, max.y };
		s_Data.QuadVBPtr->TexIndex = textureIndex;
		s_Data.QuadVBPtr->OverlayColor = overlayColor;
		s_Data.QuadVBPtr++;

		s_Data.QuadVBPtr->Position = { position.x + size.x, position.y + size.y, 0.0f };
		s_Data.QuadVBPtr->TexCoord = { max.x, max.y };
		s_Data.QuadVBPtr->TexIndex = textureIndex;
		s_Data.QuadVBPtr->OverlayColor = overlayColor;
		s_Data.QuadVBPtr++;

		s_Data.QuadIndexCount += 6;
	}

	void Renderer::DrawQuad(const float3& position, const float2& size, const Ref<Texture2D>& texture)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, { 0.0f, 0.0f }, { 1.0f, 1.0f }, CR_COLOR_WHITE);
	}
	
	void Renderer::DrawQuad(const float3& position, const float2& size, const Sprite& subTexture)
	{
		float2 min = subTexture.min;
		float2 max = subTexture.max;

		if (subTexture.flipX)
		{
			min.x = subTexture.max.x;
			max.x = subTexture.min.x;
		}

		if (subTexture.flipY)
		{
			min.y = subTexture.max.y;
			max.y = subTexture.min.y;
		}

		DrawQuad({ position.x, position.y, 0.0f }, size, subTexture.texture, min, max, { subTexture.overlayColor, subTexture.overlayWeight });
	}

	void Renderer::DrawQuad(const float2& position, const float2& size, const Sprite& subTexture)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, subTexture);
	}
	
	void Renderer::DrawQuad(const float2& position, const float2& size, const Ref<Texture2D>& texture, const float2& min, const float2& max)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, min, max, CR_COLOR_WHITE);
	}

	void Renderer::DrawQuadOutline(float x0, float x1, float y0, float y1, float4 color)
	{
		DrawLine({ x0, y0, 0.0f }, { x0, y1, 0.0f }, color);
		DrawLine({ x0, y1, 0.0f }, { x1, y1, 0.0f }, color);
		DrawLine({ x1, y1, 0.0f }, { x1, y0, 0.0f }, color);
		DrawLine({ x1, y0, 0.0f }, { x0, y0, 0.0f }, color);
	}

	void Renderer::DrawLine(const float3& p0, const float3& p1, const float4& color)
	{
		s_Data.LineVBPtr->Position = p0;
		s_Data.LineVBPtr->Color = color;
		s_Data.LineVBPtr++;

		s_Data.LineVBPtr->Position = p1;
		s_Data.LineVBPtr->Color = color;
		s_Data.LineVBPtr++;

		s_Data.LineVertexCount += 2;
	}

	void Renderer::Draw(const Ref<Shader> shader, const Ref<VertexArray>& vertexArray, const mat4& transform)
	{
		shader->Bind();
		shader->UploadUniformMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);

		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->count, GL_UNSIGNED_INT, nullptr);
	}
}