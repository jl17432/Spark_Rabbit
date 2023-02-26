#pragma once

namespace SparkRabbit {

	/*enum class ShaderDatatype {
		None = 0,
		Float, Float2, Float3, Float4,
		Mat3, Mat4,
		Int, Int2, Int3, Int4, Bool
	};

	static uint32_t ShaderDatasize(ShaderDatatype type)
	{
		switch (type)
		{
		case ShaderDatatype::Float: return 4;
		case ShaderDatatype::Float2: return 4 * 2;
		case ShaderDatatype::Float3: return 4 * 3;
		case ShaderDatatype::Float4: return 4 * 4;
		case ShaderDatatype::Mat3: return 4 * 3 * 3;
		case ShaderDatatype::Mat4: return 4 * 4 * 4;
		case ShaderDatatype::Int: return 4;
		case ShaderDatatype::Int2: return 4 * 2;
		case ShaderDatatype::Int3: return 4 * 3;
		case ShaderDatatype::Int4: return 4 * 4;
		case ShaderDatatype::Bool: return 1;
		}
		return 0;
	}

	struct BufferElements
	{
		ShaderDatatype Type;
		std::string Name;
		uint32_t offset;
		uint32_t size;
		bool Normalized;

		BufferElements(ShaderDatatype type, const std::string& name,bool normalized = false) : Type(type),Name(name),offset(0),size(ShaderDatasize(type)),Normalized(normalized) {}

		BufferElements(){}

		uint32_t getComponentCount() const
		{
			switch (Type)
			{
			case ShaderDatatype::Float: return 1;
			case ShaderDatatype::Float2: return 2;
			case ShaderDatatype::Float3: return 3;
			case ShaderDatatype::Float4: return 4;
			case ShaderDatatype::Mat3: return 3 * 3;
			case ShaderDatatype::Mat4: return 4 * 4;
			case ShaderDatatype::Int: return 1;
			case ShaderDatatype::Int2: return 2;
			case ShaderDatatype::Int3: return 3;
			case ShaderDatatype::Int4: return 4;
			case ShaderDatatype::Bool: return 1;
			}return 0;
		}
	};*/

	
	class vertexBuffer
	{
	public:
		virtual ~vertexBuffer(){}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		/*virtual const BufferLayout& getLayout() const = 0;
		virtual void setLayout(const BufferLayout& layout) = 0;*/

		static vertexBuffer* create(float* vertices, uint32_t size);
	};

	class indexBuffer
	{
	public:
		virtual ~indexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		/*virtual const BufferLayout& getLayout() const = 0;
		virtual void setLayout(const BufferLayout& layout) = 0;*/

		static indexBuffer* create(uint32_t* indices, uint32_t count);
	};

	//class BufferLayout
	//{
	//public:
	//	//BufferLayout(){}
	//	BufferLayout(const std::initializer_list<BufferElements>element) : m_element(element)
	//	{
	//		CalculateOffsetAndStride();
	//	}
	//	inline uint32_t GetStride()const { return m_stride; }
	//	inline const std::vector<BufferElements>& GetElements() const { return m_element; }

	//	std::vector<BufferElements>::iterator begin() { return m_element.begin(); }
	//	std::vector<BufferElements>::iterator end() { return m_element.end(); }

	//private:
	//	void CalculateOffsetAndStride()
	//	{
	//		uint32_t offset = 0;
	//		m_stride = 0;
	//		for (auto& element : m_element)
	//		{
	//			element.offset = offset;
	//			offset += element.size;
	//			m_stride += element.size;
	//		}
	//	}
	//private:
	//	std::vector<BufferElements> m_element;
	//	uint32_t m_stride = 0;
	//};
}

