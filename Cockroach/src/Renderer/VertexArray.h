#pragma once

#include <memory>
#include "Renderer/Buffer.h"

namespace Cockroach
{
	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray() {}

		void Bind() const;
		void Unbind() const;
			
		void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);
		void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer);

		const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return vertexBuffers; }
		const Ref<IndexBuffer>& GetIndexBuffer() const { return indexBuffer; }
	private:
		uint32_t rendererID;
		uint32_t vertexBufferIndex = 0;
		std::vector<Ref<VertexBuffer>> vertexBuffers;
		Ref<IndexBuffer> indexBuffer;
	};
}