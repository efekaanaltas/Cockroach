#pragma once

namespace Cockroach
{
	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:     return 4;
			case ShaderDataType::Float2:    return 4 * 2;
			case ShaderDataType::Float3:    return 4 * 3;
			case ShaderDataType::Float4:    return 4 * 4;
			case ShaderDataType::Mat3:      return 4 * 3 * 3;
			case ShaderDataType::Mat4:      return 4 * 4 * 4;
			case ShaderDataType::Int:       return 4;
			case ShaderDataType::Int2:      return 4 * 2;
			case ShaderDataType::Int3:      return 4 * 3;
			case ShaderDataType::Int4:      return 4 * 4;
			case ShaderDataType::Bool:      return 1;
		}

		CR_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Offset;
		uint32_t Size;
		bool Normalized;

		BufferElement() {}

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{
		}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
				case ShaderDataType::Float:     return 1;
				case ShaderDataType::Float2:    return 2;
				case ShaderDataType::Float3:    return 3;
				case ShaderDataType::Float4:    return 4;
				case ShaderDataType::Mat3:      return 3; // * 3;
				case ShaderDataType::Mat4:      return 4; // * 4;
				case ShaderDataType::Int:       return 1;
				case ShaderDataType::Int2:      return 2;
				case ShaderDataType::Int3:      return 3;
				case ShaderDataType::Int4:      return 4;
				case ShaderDataType::Bool:      return 1;
			}

			CR_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() {}

		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		std::vector<BufferElement>::iterator begin() { return elements.begin(); }
		std::vector<BufferElement>::iterator end() { return elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return elements.end(); }
	private:
		void CalculateOffsetsAndStride()
		{
			stride = 0;
			for (auto& element : elements)
			{
				element.Offset = stride; // Stride is equal to element offset
				stride += element.Size;
			}
		}
	public:
		std::vector<BufferElement> elements;
		uint32_t stride = 0;
	};
	
	class VertexBuffer
	{
	public:
		VertexBuffer(uint32_t size);
		VertexBuffer(float* vertices, uint32_t size);
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;

		void SetData(const void* data, uint32_t size);

		BufferLayout layout;
	private:
		uint32_t rendererID;
	};

	class IndexBuffer
	{
	public:
		IndexBuffer(uint32_t* indices, uint32_t count);
		~IndexBuffer();

		void Bind() const;
		void Unbind() const;

		uint32_t count;
	private:
		uint32_t rendererID;
	};
}