#pragma once
#include <stdint.h>
namespace SparkRabbit {

	enum class ShaderDataType
	{
		None = 0, Float, Vec2, Vec3, Vec4, Mat3, Mat4, Int, Ivec2, Ivec3, Ivec4, Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type);

	struct BufferElement
	{
		ShaderDataType Type;
		std::string Name;
		uint32_t Size;
		uint32_t Offset;
		bool Normalized;

		BufferElement () = default;

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			:Type(type), Name(name), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{
		}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
				case ShaderDataType::Float: return 1;
				case ShaderDataType::Vec2: return 2;
				case ShaderDataType::Vec3: return 3;
				case ShaderDataType::Vec4: return 4;
				case ShaderDataType::Mat3: return 3 * 3;
				case ShaderDataType::Mat4: return 4 * 4;
				case ShaderDataType::Int: return 1;
				case ShaderDataType::Ivec2: return 2;
				case ShaderDataType::Ivec3: return 3;
				case ShaderDataType::Ivec4: return 4;
				case ShaderDataType::Bool: return 1;
			}
			SPARK_CORE_ASSERT(false, "Unsupported Shader Data Type!");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() {}
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }
		inline uint32_t GetStride() const { return m_Stride; }
		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		void CalculateOffsetsAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}

		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	enum class VertexBufferUsage
	{
		None = 0, Static = 1, Dynamic = 2
	};
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void SetData(void* buffer, uint32_t size, uint32_t offset = 0) = 0;
		virtual void Bind() const = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		virtual unsigned int GetSize() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		static std::shared_ptr<VertexBuffer> Create(void* data, uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Static);
		static std::shared_ptr<VertexBuffer> Create(uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void SetData(void* buffer, uint32_t size, uint32_t offset = 0) = 0;
		virtual void Bind() const = 0;

		virtual uint32_t GetCount() const = 0;

		virtual unsigned int GetSize() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		static std::shared_ptr<IndexBuffer> Create(void* data, uint32_t size = 0);
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float: return 4;
			case ShaderDataType::Vec2: return 4 * 2;
			case ShaderDataType::Vec3: return 4 * 3;
			case ShaderDataType::Vec4: return 4 * 4;
			case ShaderDataType::Mat3: return 4 * 3 * 3;
			case ShaderDataType::Mat4: return 4 * 4 * 4;
			case ShaderDataType::Int: return 4;
			case ShaderDataType::Ivec2: return 4 * 2;
			case ShaderDataType::Ivec3: return 4 * 3;
			case ShaderDataType::Ivec4: return 4 * 4;
			case ShaderDataType::Bool: return 1;
		}

		SPARK_CORE_ASSERT(false, "Unsupported Shader Data Type!");
		return 0;
	}
}