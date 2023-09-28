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
		float4 Color;
		float4 OverlayColor;
	};

	struct LineVertex
	{
		float3 Position;
		float4 Color;
	};

	struct RendererData
	{
		static const int BatchQuadCount = 10000;
		static const int BatchVertexCount = BatchQuadCount * 4;
		static const int BatchIndexCount = BatchQuadCount * 6;
		static const int MaxTextureSlots = 32;

		Ref<VertexArray> QuadVA;
		Ref<VertexBuffer> QuadVB;
		Ref<Shader> QuadShader;
		int QuadIndexCount = 0;
		QuadVertex* QuadVBBase = nullptr;
		QuadVertex* QuadVBPtr = nullptr;

		Ref<VertexArray> LineVA;
		Ref<VertexBuffer> LineVB;
		Ref<Shader> LineShader;
		int LineVertexCount = 0;
		LineVertex* LineVBBase = nullptr;
		LineVertex* LineVBPtr = nullptr;

		Ref<Shader> PostProcessingShader;
		Ref<Shader> BloomPrefilter;
		Ref<Shader> BloomDownsample;
		Ref<Shader> BloomUpsample;
		Ref<Shader> Distortion;

		mat4 ViewProjectionMatrix;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		int TextureSlotIndex = 0;
	};

	static RendererData data;



	void Renderer::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		// Quads
		data.QuadVA = CreateRef<VertexArray>();

		data.QuadVB = CreateRef<VertexBuffer>(data.BatchVertexCount * sizeof(QuadVertex));
		data.QuadVB->layout =
		{
			{ ShaderDataType::Float3, "a_Position"		},
			{ ShaderDataType::Float2, "a_TexCoord"		},
			{ ShaderDataType::Float,  "a_TexIndex"		},
			{ ShaderDataType::Float4, "a_Color"			},
			{ ShaderDataType::Float4, "a_OverlayColor"	}
		};
		data.QuadVA->AddVertexBuffer(data.QuadVB);

		data.QuadVBBase = new QuadVertex[data.BatchVertexCount];

		u32* quadIndices = new u32[data.BatchIndexCount];
		
		int offset = 0;
		for (int i = 0; i < data.BatchIndexCount; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 1;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 2;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = CreateRef<IndexBuffer>(quadIndices, data.BatchIndexCount);
		data.QuadVA->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		int samplers[data.MaxTextureSlots];
		for (int i = 0; i < data.MaxTextureSlots; i++)
			samplers[i] = i;

		data.QuadShader = CreateRef<Shader>("Texture", "Texture");
		data.QuadShader->UploadUniformIntArray("u_Textures", samplers, data.MaxTextureSlots);

		// Lines
		data.LineVA = CreateRef<VertexArray>();

		data.LineVB = CreateRef<VertexBuffer>(data.BatchVertexCount * sizeof(LineVertex));
		data.LineVB->layout =
		{
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    }
		};
		data.LineVA->AddVertexBuffer(data.LineVB);
		data.LineVBBase = new LineVertex[data.BatchVertexCount];

		data.LineShader = CreateRef<Shader>("Line", "Line");

		data.PostProcessingShader = CreateRef<Shader>("Screen", "PostProcessing");
		data.BloomPrefilter = CreateRef<Shader>("Screen", "Prefilter13Tap");
		data.BloomDownsample = CreateRef<Shader>("Screen", "Downsample13Tap");
		data.BloomUpsample = CreateRef<Shader>("Screen", "UpsampleTent");
		data.Distortion = CreateRef<Shader>("Screen", "Distortion");
	}

	void Renderer::Shutdown()
	{
	}

	void Renderer::OnWindowResize(int width, int height)
	{
		//glViewport(0, 0, width, height);
		const float targetAspect = 16.0f / 9.0f;
		float aspect = (float)width / height;
		if (aspect >= targetAspect)
		{
			float targetWidth = height * targetAspect;
			float originX = (width - targetWidth) / 2.0f;
			glViewport(originX, 0, targetWidth, height);
		}
		else
		{
			float targetHeight = width / targetAspect;
			float originY = (height - targetHeight) / 2.0f;
			glViewport(0, originY, width, targetHeight);
		}
	}

	void Renderer::SetClearColor(const float4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void Renderer::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::BloomDownsample(Ref<Framebuffer> src, Ref<Framebuffer> dst)
	{
		glDisable(GL_DEPTH_TEST);
		dst->Bind();
		glViewport(0, 0, dst->width, dst->height);
		data.BloomDownsample->Bind();
		glBindTexture(GL_TEXTURE_2D, src->colorAttachment);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		dst->Unbind();
	}

	void Renderer::BloomUpsample(Ref<Framebuffer> src1, Ref<Framebuffer> src2, Ref<Framebuffer> dst)
	{
		glDisable(GL_DEPTH_TEST);
		dst->Bind();
		glViewport(0, 0, dst->width, dst->height);
		data.BloomUpsample->Bind();
		int samplers[2] = { 0,1 };
		data.BloomUpsample->UploadUniformIntArray("u_Textures", samplers, 2);
		glBindTextureUnit(0, src1->colorAttachment);
		glBindTextureUnit(1, src2->colorAttachment);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		dst->Unbind();
	}

	void Renderer::Distortion(Ref<Framebuffer> colorSrc, Ref<Framebuffer> distortionSrc, Ref<Framebuffer> dst)
	{
		glDisable(GL_DEPTH_TEST);
		dst->Bind();
		glViewport(0, 0, dst->width, dst->height);
		data.Distortion->Bind();
		int samplers[2] = { 0,1 };
		data.Distortion->UploadUniformIntArray("u_Textures", samplers, 2);
		glBindTextureUnit(0, colorSrc->colorAttachment);
		glBindTextureUnit(1, distortionSrc->colorAttachment);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		dst->Unbind();
	}

	void Renderer::BeginScene(Camera& camera)
	{
		data.ViewProjectionMatrix = camera.GetViewProjectionMatrix();
		data.QuadShader->Bind();
		data.QuadShader->UploadUniformMat4("u_ViewProjection", data.ViewProjectionMatrix);
		data.QuadIndexCount = 0;
		data.QuadVBPtr = data.QuadVBBase;

		data.TextureSlotIndex = 0;

		data.LineShader->Bind();
		data.LineShader->UploadUniformMat4("u_ViewProjection", data.ViewProjectionMatrix);
		data.LineVertexCount = 0;
		data.LineVBPtr = data.LineVBBase;
	}

	void Renderer::EndScene()
	{
		if (data.QuadIndexCount)
		{
			u32 dataSize = (u32)((u8*)data.QuadVBPtr - (u8*)data.QuadVBBase);
			data.QuadVB->SetData(data.QuadVBBase, dataSize);

			for (u32 i = 0; i < data.TextureSlotIndex; i++)
				data.TextureSlots[i]->Bind(i);

			data.QuadVA->Bind();
			data.QuadShader->Bind();
			glDrawElements(GL_TRIANGLES, data.QuadIndexCount, GL_UNSIGNED_INT, nullptr);
		}
		if (data.LineVertexCount)
		{
			u32 dataSize = (u32)((u8*)data.LineVBPtr - (u8*)data.LineVBBase);
			data.LineVB->SetData(data.LineVBBase, dataSize);

			data.LineVA->Bind();
			data.LineShader->Bind();
			glDrawArrays(GL_LINES, 0, data.LineVertexCount);
		}
	}

	void Renderer::FlushAndReset()
	{
		EndScene();

		data.QuadIndexCount = 0;
		data.QuadVBPtr = data.QuadVBBase;

		data.LineVertexCount = 0;
		data.LineVBPtr = data.LineVBBase;

		data.TextureSlotIndex = 0;
	}

	void Renderer::BlitToScreen(Ref<Framebuffer> framebuffer)
	{
		glDisable(GL_DEPTH_TEST);
		data.PostProcessingShader->Bind();
		glBindTexture(GL_TEXTURE_2D, framebuffer->colorAttachment);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	void Renderer::BloomPrefilter(Ref<Framebuffer> src, Ref<Framebuffer> dst, float threshold)
	{
		glDisable(GL_DEPTH_TEST);
		dst->Bind();
		glViewport(0, 0, dst->width, dst->height);
		data.BloomPrefilter->Bind();
		data.BloomPrefilter->UploadUniformFloat("brightnessThreshold", threshold);
		glBindTexture(GL_TEXTURE_2D, src->colorAttachment);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		dst->Unbind();
	}

	void Renderer::DrawQuad(const float3& position, const float2& size, const Ref<Texture2D>& texture, const float2& min, const float2& max, const float4& color, const float4& overlayColor)
	{
		if (data.QuadIndexCount >= RendererData::BatchIndexCount)
			FlushAndReset();

		float textureIndex = -1.0f;
		for (int i = 0; i < data.TextureSlotIndex; i++)
		{
			if (*data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == -1.0f)
		{
			textureIndex = (float)data.TextureSlotIndex;
			data.TextureSlots[data.TextureSlotIndex] = texture;
			data.TextureSlotIndex++;
		}

		data.QuadVBPtr->Position = position;
		data.QuadVBPtr->TexCoord = { min.x, min.y };
		data.QuadVBPtr->TexIndex = textureIndex;
		data.QuadVBPtr->Color = color;
		data.QuadVBPtr->OverlayColor = overlayColor;
		data.QuadVBPtr++;

		data.QuadVBPtr->Position = { position.x + size.x, position.y, position.z };
		data.QuadVBPtr->TexCoord = { max.x, min.y };
		data.QuadVBPtr->TexIndex = textureIndex;
		data.QuadVBPtr->Color = color;
		data.QuadVBPtr->OverlayColor = overlayColor;
		data.QuadVBPtr++;

		data.QuadVBPtr->Position = { position.x, position.y + size.y, position.z };
		data.QuadVBPtr->TexCoord = { min.x, max.y };
		data.QuadVBPtr->TexIndex = textureIndex;
		data.QuadVBPtr->Color = color;
		data.QuadVBPtr->OverlayColor = overlayColor;
		data.QuadVBPtr++;

		data.QuadVBPtr->Position = { position.x + size.x, position.y + size.y, position.z };
		data.QuadVBPtr->TexCoord = { max.x, max.y };
		data.QuadVBPtr->TexIndex = textureIndex;
		data.QuadVBPtr->Color = color;
		data.QuadVBPtr->OverlayColor = overlayColor;
		data.QuadVBPtr++;

		data.QuadIndexCount += 6;
	}

	void Renderer::DrawQuad(const float3& position, const float2& size, const Sprite& sprite, const float4& color, const float4& overlayColor, bool flipX, bool flipY)
	{
		float2 min = sprite.min;
		float2 max = sprite.max;

		if (flipX) { min.x = sprite.max.x; max.x = sprite.min.x; }
		if (flipY) { min.y = sprite.max.y; max.y = sprite.min.y; }

		DrawQuad(position, size, sprite.texture, min, max, color, overlayColor);
	}

	void Renderer::DrawQuadWithOutline(const float3& position, const float2& size, const Sprite& subTexture, float4 color, float4 overlayColor, float4 outlineColor, bool flipX, bool flipY)
	{
		DrawQuad({ position.x + 1, position.y,     position.z }, size, subTexture, color, outlineColor, flipX, flipY);
		DrawQuad({ position.x - 1, position.y,     position.z }, size, subTexture, color, outlineColor, flipX, flipY);
		DrawQuad({ position.x    , position.y + 1, position.z }, size, subTexture, color, outlineColor, flipX, flipY);
		DrawQuad({ position.x    , position.y - 1, position.z }, size, subTexture, color, outlineColor, flipX, flipY);
		DrawQuad({ position.x    , position.y,     position.z }, size, subTexture, color, overlayColor, flipX, flipY);
	}

	void Renderer::DrawQuadOutline(float x0, float x1, float y0, float y1, float4 color)
	{
		DrawLine({ x0, y0, 90.0f }, { x0, y1, 0.0f }, color);
		DrawLine({ x0, y1, 90.0f }, { x1, y1, 0.0f }, color);
		DrawLine({ x1, y1, 90.0f }, { x1, y0, 0.0f }, color);
		DrawLine({ x1, y0, 90.0f }, { x0, y0, 0.0f }, color);
	}

	void Renderer::DrawLine(const float3& p0, const float3& p1, const float4& color)
	{
		data.LineVBPtr->Position = p0;
		data.LineVBPtr->Color = color;
		data.LineVBPtr++;

		data.LineVBPtr->Position = p1;
		data.LineVBPtr->Color = color;
		data.LineVBPtr++;

		data.LineVertexCount += 2;
	}

	void Renderer::Draw(const Ref<Shader> shader, const Ref<VertexArray>& vertexArray, const mat4& transform)
	{
		shader->Bind();
		shader->UploadUniformMat4("u_ViewProjection", data.ViewProjectionMatrix);

		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->count, GL_UNSIGNED_INT, nullptr);
	}
}