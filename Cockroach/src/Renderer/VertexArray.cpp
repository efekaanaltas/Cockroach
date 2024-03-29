#include "crpch.h"
#include "VertexArray.h"

#include <glad/glad.h>

namespace Cockroach
{

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case Cockroach::ShaderDataType::Float:     return GL_FLOAT;
			case Cockroach::ShaderDataType::Float2:    return GL_FLOAT;
			case Cockroach::ShaderDataType::Float3:    return GL_FLOAT;
			case Cockroach::ShaderDataType::Float4:    return GL_FLOAT;
			case Cockroach::ShaderDataType::Mat3:      return GL_FLOAT;
			case Cockroach::ShaderDataType::Mat4:      return GL_FLOAT;
			case Cockroach::ShaderDataType::Int:       return GL_INT;
			case Cockroach::ShaderDataType::Int2:      return GL_INT;
			case Cockroach::ShaderDataType::Int4:      return GL_INT;
			case Cockroach::ShaderDataType::Int3:      return GL_INT;
			case Cockroach::ShaderDataType::Bool:      return GL_BOOL;
		}

		CR_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	VertexArray::VertexArray()
	{
		glCreateVertexArrays(1, &rendererID);
	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(rendererID);
	}

	void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		CR_CORE_ASSERT(vertexBuffer->layout.elements.size(), "Vertex Buffer has no layout!");

		glBindVertexArray(rendererID);
		vertexBuffer->Bind();

		const auto& layout = vertexBuffer->layout;
		for (const auto& element : layout)
		{
			switch (element.Type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool:
			{
				glEnableVertexAttribArray(vertexBufferIndex);
				glVertexAttribPointer(vertexBufferIndex,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					layout.stride,
					(const void*)element.Offset);
				vertexBufferIndex++;
				break;
			}
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
			{
				u8 count = element.GetComponentCount();
				for (u8 i = 0; i < count; i++)
				{
					glEnableVertexAttribArray(vertexBufferIndex);
					glVertexAttribPointer(vertexBufferIndex,
						count,
						ShaderDataTypeToOpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.stride,
						(const void*)(sizeof(float) * count * i));
					glVertexAttribDivisor(vertexBufferIndex, 1);
					vertexBufferIndex++;
				}
				break;
			}
			default:
				CR_CORE_ASSERT(false, "Unknown ShaderDataType!");
			}
		}

		vertexBuffers.push_back(vertexBuffer);
	}

	void VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(rendererID);
		indexBuffer->Bind();

		this->indexBuffer = indexBuffer;
	}
}